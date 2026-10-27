/* Online games in the game list (0.7.0 slice 4, #45, docs/specs/2026-10-27-online-play-0.7.0-spec.md).
 *
 * The runtime does all of this under the game's own network code, which runs unchanged
 * (docs/research/2026-10-27-network-code-reading.md, section 9). It watches the datagrams that the game sends
 * (net_core.c hooks):
 * - "CM3 LIST SERVER ACK" leaves only a host: Network::host_or_join_game 0x77a9f0 broadcasts it when the host
 *   starts (0x77aa7c..0x77aafd), and Network::receive_packet 0x77af20 sends it as the answer to a REQ
 *   (0x77aff2..0x77b088). The first one makes this game a host. Its bytes 19..68 hold the computer name
 *   (gethostname at 0x77aa77) and bytes 69..118 the game name (+0xc3a6c).
 * - "CM3 LIST SERVER BYE" leaves a host when its Network object goes (Network::~Network 0x77a890, host flag
 *   only): the game ended, and its lobby entry closes.
 * - "CM3 LIST SERVER REQ" leaves the game list (0x81f906, 0x81fc2d, 0x81fddc): the list asks the lobby too.
 *
 * Where the runtime reads the fields of a host's entry (each read at most once a second, in the hook of a
 * network call of the game, so on the game's own thread):
 * - The game data is loaded when the competition table [0xadadfc] is set (setup_create_or_join_network_game
 *   0x8388e0 tests it at 0x8389c7 before free_game_data) and the staff table [0xae23bc] is set. A host
 *   registers only then: a new network game is made at Select League(s) before its data loads
 *   (tests/smoke/network_run.py), and until then the game date is not the game's.
 * - The day: the game date 0xae2c90, s16 day of the year and s16 year (savename.c, win_game_date). The season
 *   is its start year: the year of the date from July, else the year before.
 * - The human clubs: the 16 human managers are the last 16 rows of the staff table [0xae23bc] (0x6e bytes a
 *   row, count [0xae2364]). A row is in use when its byte +0x5f is not 0 (HUMAN_MANAGER::
 *   clear_invalid_watched_fixtures 0x5fe8d0 at 0x5fe8e3..0x5fe903; setup_assign_restored_network_managers
 *   0x8387e0 reads the same rows at 0x8387f0..0x83880f). Its club is at +0x39 (the side bar 0x76300e), with
 *   the short name at club +0x38 (0x763017) and the long name at club +4 (savename.c).
 * - Away (0.7.0 slice 6): a human manager on holiday. HUMAN_MANAGER::is_on_holiday 0x5fd210 reads the dword at
 *   +0x9a01 of row i (0x5fd2a5..0x5fd2d2) of the rows at +0 of the object [0xb63c98], with i the staff index less
 *   (count - 16) and rows of 0xa6ef bytes, after a check of i against the count at +4 (0x5fd2b3..0x5fd2b9). A
 *   human manager whose connection drops goes on holiday (setup_on_network_player_left 0x838c00, net_session.c).
 * - Managers online: the host itself and each connection that it accepted and that is open (nc_peers).
 *   Free seats: 16, the original's limit, less the larger of the rows in use and the managers online.
 * - The database: the name of the data set's folder (win_data_set_dir), else 3.9.68, the game's own Data.
 * - The version: CM0102_VERSION. The test variable RECOMP_TEST_VERSION sets another one.
 * The game id is new for each hosting: the save has no id that the runtime knows yet (one open copy of a
 * save, Q23, is open for slice 6).
 *
 * The lobby address: RECOMP_LOBBY_URL, else LOBBY_ADDRESS (empty until Max deploys the server, slice 5). The
 * option online_play (on by default) gates every lobby request at the moment of the request: with it off,
 * the game list shows LAN games only and a host stays off the lobby.
 *
 * The reason of a refused join. The original shows "The network game "<name>" could not be joined." with
 * the title "Unable To Join" (setup_create_or_join_network_game 0x8388e0: the text 0xa82348 is formatted
 * into 0xde1f64 at 0x838a46 and goes to confirm_push 0x591840 at 0x838a67 as its third argument). When the
 * runtime knows why the join failed, the wrap of 0x591840 gives that box the reason in place of the text. */
#include "win.h"
#include "options.h"
#include "net_core.h"
#include "lobby.h"
#include "lobby_rows.h"
#include "lobby_http.h"
#include "../host_net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CM0102_VERSION
#define CM0102_VERSION "dev"
#endif
#define LOBBY_ADDRESS ""            /* slice 5: the deployed https address */

#define COMP_TABLE 0xadadfcu        /* g_comp_object_table */
#define STAFF_TABLE 0xae23bcu
#define STAFF_COUNT 0xae2364u
#define STAFF_SIZE 0x6eu
#define STAFF_HUMAN 0x5fu
#define STAFF_CLUB 0x39u
#define CLUB_SHORT 0x38u
#define CLUB_LONG 0x4u
#define HUMAN_MANAGERS 0xb63c98u    /* HUMAN_MANAGER object: +0 the rows, +4 their count */
#define HM_ROW 0xa6efu              /* HUMAN_MANAGER::is_on_holiday 0x5fd210: row = staff index - (count - 16) */
#define HM_HOLIDAY 0x9a01u          /* the dword that 0x5fd2d2 reads */
#define JOIN_TEXT "could not be joined"
#define LIST_CACHE_MS 5000
#define PORT 6667

static int hosting;
static char host_text[51], game_name[51], game_id[40];
static uint64_t last_look;
static LobbyHostState shown_state;

static LobbyRow cached[LOBBY_ROWS];
static int cached_n = -1, cached_result;
static uint64_t cached_at, asked;

static char reason[400];            /* the reason of the last refused join, for the box */

static const char *version(void)
{
    const char *v = getenv("RECOMP_TEST_VERSION");
    return v && *v ? v : CM0102_VERSION;
}

static int online(void) { return options_bool(OPT_ONLINE_PLAY) && lobby_usable(); }

static int data_loaded(void)
{
    return rd32(g_mem, COMP_TABLE) && rd32(g_mem, STAFF_TABLE) && rd32(g_mem, STAFF_COUNT) >= 16;
}

/* The fields of the host's entry from the game's memory (see the head of this file). */
static void snapshot(LobbyGame *g)
{
    memset(g, 0, sizeof *g);
    snprintf(g->game_id, sizeof g->game_id, "%s", game_id);
    lobby_clean(g->name, sizeof g->name, game_name);
    lr_host_key(g->host_name, host_text);
    lobby_clean(g->version, sizeof g->version, version());
    const char *dir = win_data_set_dir(), *slash = strrchr(dir, '/');
    lobby_clean(g->database, sizeof g->database, *dir ? (slash ? slash + 1 : dir) : "3.9.68");
    if (!g->database[0]) snprintf(g->database, sizeof g->database, "3.9.68");
    int y, m, d;
    win_game_date(&y, &m, &d);
    if (y < 1900 || y > 2199) y = 2001, m = 7, d = 1;
    g->season = m >= 7 ? y : y - 1;
    snprintf(g->day, sizeof g->day, "%04d-%02d-%02d", y, m, d);
    uint32_t table = rd32(g_mem, STAFF_TABLE), count = rd32(g_mem, STAFF_COUNT);
    uint32_t hm = rd32(g_mem, HUMAN_MANAGERS), hm_rows = hm ? rd32(g_mem, hm) : 0, hm_n = hm ? rd32(g_mem, hm + 4) : 0;
    int used = 0;
    for (uint32_t i = 0; i < 16; i++) {
        uint32_t row = table + (count - 16 + i) * STAFF_SIZE;
        if (!g_mem[row + STAFF_HUMAN]) continue;
        used++;
        uint32_t club = rd32(g_mem, row + STAFF_CLUB);
        if (!club || g->clubs_n == LOBBY_CLUBS) continue;
        char name[LOBBY_TEXT];
        lobby_clean(name, sizeof name, (const char *)g_mem + club + CLUB_SHORT);
        if (!name[0]) lobby_clean(name, sizeof name, (const char *)g_mem + club + CLUB_LONG);
        if (!name[0]) continue;
        memcpy(g->clubs[g->clubs_n++], name, sizeof name);
        if (hm_rows && i < hm_n && rd32(g_mem, hm_rows + i * HM_ROW + HM_HOLIDAY))
            memcpy(g->away[g->away_n++], name, sizeof name);
    }
    g->managers_online = 1 + nc_peers();
    if (g->managers_online > 16) g->managers_online = 16;
    int taken = used > g->managers_online ? used : g->managers_online;
    g->free_seats = taken < 16 ? 16 - taken : 0;
}

static void sent(const uint8_t *p, uint32_t n, uint32_t addr, uint16_t port)
{
    (void)addr, (void)port;
    if (lr_is(p, (long)n, "CM3 LIST SERVER REQ")) {
        lr_request();
        if (!online()) return;
        if (cached_n >= 0 && hn_ms() - cached_at < LIST_CACHE_MS) lr_lobby(cached, cached_n, cached_result);
        else if (!asked || hn_ms() - asked >= 1000) {     /* one request for the REQs of all adapters */
            asked = hn_ms();
            lobby_list_fetch();
        }
    } else if (n >= LR_DATAGRAM && lr_is(p, (long)n, "CM3 LIST SERVER ACK") && !hosting) {
        hosting = 1;
        memcpy(host_text, p + LR_HOST_AT, 50), host_text[50] = 0;
        memcpy(game_name, p + LR_NAME_AT, 50), game_name[50] = 0;
        char key[12];
        lr_host_key(key, host_text);
        snprintf(game_id, sizeof game_id, "be-%s-%08llx", key + 3, (unsigned long long)(hn_ms() & 0xffffffffu));
        fprintf(stderr, "recomp: lobby: this game hosts \"%s\"\n", game_name);
    } else if (lr_is(p, (long)n, "CM3 LIST SERVER BYE") && hosting) {
        hosting = 0;
        lobby_host_close();
        fprintf(stderr, "recomp: lobby: the hosted game ended\n");
    }
}

static int arrived(const uint8_t *p, long n, uint32_t addr)
{
    /* Test runs: the list shows the lobby's rows only, as on a computer that is not on the host's LAN. */
    static int lobby_only = -1;
    if (lobby_only < 0) lobby_only = getenv("RECOMP_TEST_NO_LAN_ROWS") != NULL;
    if (lobby_only && lr_is(p, n, "CM3 LIST SERVER ACK")) return 0;
    int keep = lr_arrived(p, n, addr);
    if (keep && n >= LR_DATAGRAM && lr_is(p, n, "CM3 LIST SERVER ACK")) {
        const uint8_t *b = (const uint8_t *)&addr;
        fprintf(stderr, "recomp: lobby: list row \"%.50s\" from %u.%u.%u.%u\n", (const char *)p + LR_NAME_AT, b[0],
                b[1], b[2], b[3]);
    }
    return keep;
}

static long extra(uint8_t *buf, uint32_t room, uint32_t *addr, uint16_t *port)
{
    long r = lr_extra(buf, room, addr);
    if (r >= 0) {
        *port = PORT;
        const uint8_t *b = (const uint8_t *)addr;
        int bye = lr_is(buf, r, "CM3 LIST SERVER BYE");
        fprintf(stderr, "recomp: lobby: list %s \"%.50s\" from %u.%u.%u.%u\n", bye ? "leaves" : "row",
                (const char *)buf + (bye ? LR_HOST_AT : LR_NAME_AT), b[0], b[1], b[2], b[3]);
    }
    return r;
}

static void refuse(const char *text)
{
    snprintf(reason, sizeof reason, "%s", text);
    fprintf(stderr, "recomp: join refused: %s\n", text);
}

static int before_connect(uint32_t addr, uint16_t port)
{
    (void)port;
    reason[0] = 0;
    int full;
    const LobbyRow *r = lr_row_of(addr, &full);
    char t[400];
    const uint8_t *b = (const uint8_t *)&addr;
    if (full) refuse("Online games are full today, try again later. Games on your own network still work.");
    else if (r && strcmp(r->g.version, version())) {
        snprintf(t, sizeof t, "This network game runs Bosman Edition %s, and you have %s. Install Bosman Edition %s "
                 "to join it.", r->g.version, version(), r->g.version);
        refuse(t);
    } else if (r || (b[0] == 0 && b[1] == 66)) {
        /* Every address of 0.66.0.0/16 stops here, also one that a player typed (lobby_rows.h). */
        refuse(r ? "This build cannot join online games yet. Join a game on your own network, or type the address "
                   "of the host."
                 : "This online game is no longer in the list. Click Refresh and try again.");
    } else return 0;
    nc_set_error(10061);
    return -1;
}

static void tick(void)
{
    net_session_tick();
    uint64_t now = hn_ms();
    if (now - last_look < 1000) return;
    last_look = now;
    LobbyRow *rows = cached;
    int result, n = lobby_list_take(rows, LOBBY_ROWS, &result);
    if (n >= 0) {
        cached_n = n, cached_result = result, cached_at = now;
        lr_lobby(rows, n, result);
    }
    if (hosting && online() && data_loaded()) {
        LobbyGame g;
        snapshot(&g);
        static int shown_away = -1;
        if (g.away_n != shown_away) {                    /* slice 6: the log names each change of Away */
            shown_away = g.away_n;
            fprintf(stderr, "recomp: lobby: away %d%s%s\n", g.away_n, g.away_n ? " " : "", g.away_n ? g.away[0] : "");
        }
        if (g.name[0]) lobby_host_set(&g, "");
    } else if (hosting && !options_bool(OPT_ONLINE_PLAY)) {
        lobby_host_close();                              /* Online play went off: the entry closes */
    }
    char code[9];
    LobbyHostState s = lobby_host_state(code);
    if (s != shown_state) {
        shown_state = s;
        static const char *const names[] = {"none", "registering", "open", "refused", "full today", "unavailable",
                                            "closed"};
        fprintf(stderr, "recomp: lobby: host entry %s%s%s\n", names[s], s == LOBBY_OPEN ? " " : "",
                s == LOBBY_OPEN ? code : "");
    }
}

/* A quit with no BYE (the script's quit, or a window close) still closes the entry. */
static void at_quit(void)
{
    if (hosting) lobby_host_close();
    lobby_finish(2000);
}

void net_lobby_install(void)
{
    static const NcHooks hooks = {sent, arrived, extra, before_connect, tick};
    static int done;
    if (done) return;
    done = 1;
    const char *url = getenv("RECOMP_LOBBY_URL");
    lobby_set_address(url ? url : LOBBY_ADDRESS);
    nc_set_hooks(&hooks);
    nc_set_identity(version(), "", "");
    atexit(at_quit);
}

/* confirm_push 0x591840, cdecl (7 arguments): the third is the text (see the head of this file). */
void o_00591840(void);
RT_WRAP(00591840, confirm_push)
{
    uint32_t text = rt_arg(2);
    if (text && strstr((const char *)g_mem + text, JOIN_TEXT)) {
        char host_version[32];
        int why = nc_refusal(host_version, sizeof host_version);
        nc_refusal_clear();
        if (!reason[0] && why == NC_REFUSED_VERSION) {
            char t[400];
            snprintf(t, sizeof t, "This network game runs Bosman Edition %s, and you have %s. Install Bosman "
                     "Edition %s to join it.", host_version, version(), host_version);
            refuse(t);
        } else if (!reason[0] && why == NC_REFUSED_PASSWORD) refuse("The password of this network game is wrong.");
        else if (!reason[0] && why == NC_REFUSED_NOT_BOSMAN)
            refuse("The host does not run this version of Bosman Edition, so you cannot join its game.");
        else if (!reason[0] && why == NC_REFUSED_NO_ANSWER)
            refuse("The host did not answer. It may be busy, or it may run another version of the game. Try again "
                   "in a moment.");
        if (reason[0]) {
            static uint32_t at;
            if (!at) at = win_static_alloc(sizeof reason);
            snprintf((char *)g_mem + at, sizeof reason, "%s", reason);
            wr32(g_mem, cpu.esp + 4 + 2 * 4, at);
            fprintf(stderr, "recomp: join box: %s\n", reason);
            reason[0] = 0;
        }
    }
    o_00591840();
}
