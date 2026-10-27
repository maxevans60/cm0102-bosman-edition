/* Drop in, drop out and host quit (0.7.0 slice 6, #45, docs/specs/2026-10-27-online-play-0.7.0-spec.md).
 *
 * What the original already does (docs/research/2026-10-27-network-code-reading.md, section 7):
 * - A human manager whose connection closes goes on holiday. Network::receive_packet 0x77af20 frees the slot and
 *   calls scrman_remove_player_records 0x7ff7b0, which reaches setup_on_network_player_left 0x838c00. It calls
 *   HUMAN_MANAGER::set_on_holiday(1, 0) (0x838c3e..0x838c41) for a manager who is made and not on holiday, and
 *   frees a manager row that is still being made (state 0 or 5 at +0x5f, 0x838c48). Away is this holiday.
 * - Rejoin. A joiner who connects again gets setup_on_network_player_joined 0x838a80. When a manager is
 *   computer controlled (+8 of the human manager object 0xb63c98) or on holiday (+0xc), it opens Manager Status
 *   (0x81ff10, 0x838a85..0x838a95, 0x838b86..0x838b9b). A click on a manager's row (command 0x18,
 *   setup_handle_manager_status_input 0x820950 at 0x820b9d) opens the password box (0x822ec0, input 0x8230b0,
 *   0x820bf2..0x820c01) when the manager has a password, else the confirm of resume_control (0x823420,
 *   0x820c36..0x820c45). The right password calls set_computer_controlled(0) and set_on_holiday(0)
 *   (0x823363..0x82337a) and gives the club back. A restored game does the same: setup_assign_restored_network_
 *   managers 0x8387e0 makes each manager with a password computer controlled until its owner returns
 *   (0x83888d..0x8388a5). So rejoin needs no new code: the runtime only has to see the drop (net_core.c,
 *   keepalive timers).
 * - Host quit. ScreenManager::disconnect_network 0x7fc690 sends the quit packet 0x04 to every joiner
 *   (0x7fc738..0x7fc766). On a joiner, opcode 4 ends the client loop run_network_client 0x7fdc00 (0x7fdeba), and
 *   the game list input returns -2 (0x81fe11..0x81fe31). The original shows nothing, and it has no save at a quit.
 *   A host that crashes or drops sends no 0x04: the joiner learns of it only at its next click (0x569456).
 *
 * What this file adds:
 * - The host's save at its quit. Every way out of a game passes free_game_data 0x82ff40 (the wrap in season.c
 *   calls net_session_game_ends first): Exit Game (0x762c42 calls game_fatal_cleanup 0x5cc7d0, which calls
 *   0x82ff40 at 0x5cc7d4), a new or loaded game, and a new network game (0x8389d2). 0x82ff40 ends the network
 *   session first (0x82ff46), so the save comes before. The save is the Auto Save's: save_game_data 0x82e430 with
 *   the flags 0x16 and the path that _makepath gives at 0x7617c9 (the save folder of the game options
 *   [0xae3138] + 0xf, the game's file name + 0x113, ".sav").
 * - A window close or the script's quit on a host waits for the poll of the screen loop (0x7fcc14 in
 *   ScreenManager::monitor 0x7fc7e0), saves there, then ends the program (net_session_defer_quit).
 * - The joiner's box. After the client loop ends because the host left (net_core.c, nc_host_left), the game's
 *   message box 0x5e8290 says so. The original's network error box is the same box (0x56946b). Then Network Play
 *   goes back to the start screen.
 * - The test switch of Kick (RECOMP_TEST_KICK=<s>): s seconds after the first joiner, the host kicks it. */
#include "win.h"
#include "net_core.h"
#include "season.h"
#include "../host_net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NETWORK_HOST 0xb77c80u      /* 1 on the host, 0 on a joiner (setup_create_or_join_network_game 0x8388e0 at
                                     * 0x838993..0x83899f); scrman_remove_player_records 0x7ff7b0 tests it */
#define NETWORK_SESSION 0xb77c90u   /* the screen manager's Network object (+0x3840 of 0xb74450), 0 with none */
#define GAME_OPTIONS 0xae3138u      /* +0xf the save folder, +0x113 the game's file name (0x7617ab..0x7617bb) */
#define SAVE_GAME 0x0082e430u       /* cdecl (path, flags) */
#define SAVE_FLAGS 0x16u            /* the Auto Save's (0x7617d5) */
#define MESSAGE_BOX 0x005e8290u     /* cdecl (title, text, 0) */

static int turns;                   /* human turns of the game that runs: it has started */

void net_session_turn(void) { turns++; }

int net_session_hosting(void)
{
    return rd32(g_mem, NETWORK_HOST) == 1 && rd32(g_mem, NETWORK_SESSION) != 0;
}

/* The path of the Auto Save, else NULL when the game options are not there. */
static const char *save_path(char *out, size_t room)
{
    uint32_t options = rd32(g_mem, GAME_OPTIONS);
    if (!options) return NULL;
    const char *folder = (const char *)g_mem + options + 0xf, *name = (const char *)g_mem + options + 0x113;
    /* A game that was never saved has no file name yet: it takes the name of the network game. */
    snprintf(out, room, "%s%s.sav", folder, *name ? name : "Network Game");
    return out;
}

static int must_save(void) { return net_session_hosting() && turns > 0 && season_game_on() && !rt_unwinding; }

static void save_now(void)
{
    char path[600];
    if (!save_path(path, sizeof path)) return;
    fprintf(stderr, "recomp: net: the host quits: saving %s\n", path);
    uint32_t args[2] = {rt_alloc_str(path), SAVE_FLAGS};
    uint32_t r = rt_callback(SAVE_GAME, 2, args);
    fprintf(stderr, "recomp: net: the host's save ended (result %u)\n", r);
}

void net_session_game_ends(void)
{
    int save = must_save();
    turns = 0;
    if (save) save_now();
}

/* A window close, or the script's quit, while this game hosts a game that has started. The program does not end
 * in the event loop: that runs inside any call of the game that pumps messages, also in the middle of a day. It
 * ends at the next poll of the screen loop (net_session_screen_idle), after the save, or after QUIT_WAIT_MS with no
 * save when the game does not come back to its screen loop. */
#define QUIT_WAIT_MS 20000u
static uint64_t quit_asked;

int net_session_defer_quit(void)
{
    if (quit_asked || !must_save()) return 0;
    quit_asked = hn_ms();
    fprintf(stderr, "recomp: net: the host quits: the save waits for the screen loop\n");
    return 1;
}

int net_session_screen_idle(void)
{
    if (!quit_asked) return 0;
    if (must_save()) save_now();
    return 1;
}

int net_session_quit_overdue(void)
{
    if (!quit_asked || hn_ms() - quit_asked < QUIT_WAIT_MS) return 0;
    fprintf(stderr, "recomp: net: the host quits with no save: the screen loop did not come back\n");
    return 1;
}

static void box(const char *title, const char *text)
{
    static uint32_t title_at, text_at;
    if (!title_at) {
        title_at = win_static_alloc(64);
        text_at = win_static_alloc(200);
    }
    snprintf((char *)g_mem + title_at, 64, "%s", title);
    snprintf((char *)g_mem + text_at, 200, "%s", text);
    fprintf(stderr, "recomp: net box: %s: %s\n", title, text);
    uint32_t args[3] = {title_at, text_at, 0};
    rt_callback(MESSAGE_BOX, 3, args);
}

static int back_to_start;           /* the host left: Network Play goes back once more, to the start screen */

void o_007fdc00(void);

/* ScreenManager::run_network_client, thiscall (ScreenManager *), no arguments: the joiner's whole session. */
RT_WRAP(007fdc00, run_network_client)
{
    nc_host_left_clear();
    o_007fdc00();
    if (rt_unwinding || !nc_host_left()) return;
    nc_host_left_clear();
    uint32_t eax = cpu.eax;
    box("Network Game", "The host left the game.");
    cpu.eax = eax;
    back_to_start = 1;
}

void o_0081ef70(void);

/* setup_handle_network_play_input, cdecl (short item, -1 for none): the screen result. After the client loop the
 * game list input returns -2 (0x81fe31), and the screen loop goes back one screen (0x7fd423), to Network Play
 * (build 0x81edd0). Its first input after the host left gives -2 as its Back does, so the joiner is on the start
 * screen (0x81bab0, which pushes Network Play at 0x81cb99). */
RT_WRAP(0081ef70, network_play_input)
{
    if (back_to_start) {
        back_to_start = 0;
        fprintf(stderr, "recomp: net: the host left: back to the start screen\n");
        rt_ret_cdecl(0xfffffffeu);
        return;
    }
    o_0081ef70();
}

/* At each network call of the game (net_lobby.c, tick). */
void net_session_tick(void)
{
    static int kick_s = -2;
    static uint64_t first_seen;
    if (kick_s == -2) {
        const char *v = getenv("RECOMP_TEST_KICK");
        kick_s = v && *v ? atoi(v) : -1;
    }
    if (kick_s < 0) return;
    uint32_t handle;
    if (nc_peer_list(&handle, NULL, NULL, 1) < 1) return;
    if (!first_seen) first_seen = hn_ms();
    if (hn_ms() - first_seen < (uint64_t)kick_s * 1000u) return;
    nc_kick(handle);
    kick_s = -1;
}
