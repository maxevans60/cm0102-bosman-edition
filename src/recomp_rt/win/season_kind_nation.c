/* Rule kind "nation" of the season change (season_change.h; 0.6.0 slice 17): the transfer windows, the foreign
 * player limits and the work permit rule of a nation, from the structured table that tools/seasons/encode_rules.py
 * makes (season_nation.h). Evidence and the texts the encoder cannot read: docs/research/2026-10-17-season-rules-17.md.
 *
 * The game's objects (docs/research/2026-10-16-sweep-rulebooks.md section 3, sweep-season-end.md section 4):
 * - TRANSFER_MANAGER [0xde1f54] holds 28 rules objects at +0x8ac (setup_transfer_rules 0x8d2e27..0x8d2f78): the
 *   European one first, then one for each nation. A rules object (TRANSFER_RULES::TRANSFER_RULES 0x8e84a0):
 *   +4 the window records (12 bytes each), +8 their count (byte), +9 the rules id, +0xa the nation record, +0x12
 *   the scope (1 nation), +0x13 the cap of foreign players a club may hold, +0x14 the cap of non-EU players
 *   (can_sign_player 0x8e8560 refuses a signing at the cap; 0xff turns the check off, 0x8e8614).
 * - A window record is a start bound and an end bound of 6 bytes: rules id, window index, weekday (0xff none),
 *   day, month (0 to 11), 1 for the start bound and 0 for the end bound (England's constructor 0x579c03..0x579c2f).
 * - The manager keeps a sorted table of pointers to every bound (+0x8a4, count +0x8a8; qsort 0x945333 with the
 *   comparator 0x8e5590) for the deadline news (update_transfer_window_news 0x8c6bc0). The kind makes it again
 *   as setup_transfer_rules does, after it changes a window.
 * - The match squad limits are in a 42-byte record that the lookup 0x8cf6b0 (thiscall: fixture, record, flag)
 *   fills through the nation's rules object: bytes +0..+2 foreign, +3..+5 non-EU, 0xff for none
 *   (2026-10-16-foreign-and-tactics-positions.md). The kind runs after the lookup and gives the slot that is set
 *   the number of the table, only for a fixture of a first division. It adds no limit that the game does not
 *   have: a limit in a slot that is not set stays as the game has it.
 * - Work permits: TRANSFER_MANAGER::player_needs_work_permit 0x8c7560 (thiscall: player, club, flag; ret 0xc)
 *   asks 0x53d740 (player, 0) whether the player is of the EU and stops when he is. With the flag set it returns 1
 *   for every other player at once (0x8c760d); with the flag clear it looks for a permit in the list (0x8c7650).
 *   Inference: the flag marks a new signing, the other calls ask about a player who is already at the club.
 *   After Brexit (Q249 keeps the real rules, R10) the kind answers 1 for a player of the EU who is not a citizen
 *   of the UK or Ireland, when the flag is set and the club is of a nation with the rule, from 1 January 2021.
 *   The players already in a squad are not asked again with the flag, so they stay, as the real transition did.
 *
 * It acts only in a season game (start year not 2001, or changing rules on); the base game and
 * RECOMP_SEASON_CODE=off keep the original's rules (Q342). The rules are those of the start year with changing
 * rules off, else of the last season point. RECOMP_SEASON_NATION_LOG=1 prints each change. */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_nation.h"
#include "season_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MANAGER 0xde1f54u               /* TRANSFER_MANAGER * */
#define RULES_AT 0x8acu                 /* the 28 rules objects */
#define RULES_COUNT 28
#define EDGES_AT 0x8a4u                 /* sorted pointers to the window bounds */
#define EDGES_COUNT 0x8a8u
#define R_WINDOWS 4u
#define R_COUNT 8u                      /* byte */
#define R_ID 9u
#define R_NATION 0xau
#define R_SCOPE 0x12u
#define R_CAP_FOREIGN 0x13u
#define R_CAP_NON_EU 0x14u
#define WINDOW_SIZE 12u
#define GAME_DATE 0xae2c90u
#define COMP_TABLE 0xadadfcu
#define COMP_SLOTS 0xadadf8u
#define MALLOC 0x00944e46u              /* cdecl (size): the allocation of setup_transfer_rules, 0x8d2e62 */
#define QSORT 0x00945333u               /* cdecl (base, count, size, compare), 0x8d2f73 */
#define WINDOW_COMPARE 0x008e5590u
#define IS_EU 0x0053d740u               /* cdecl (player, club nation or 0) -> 1 when of the EU */
#define NEEDS_PERMIT 0x008c7560u
#define BREXIT_YEAR 2021

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int log_on(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_SEASON_NATION_LOG") && *getenv("RECOMP_SEASON_NATION_LOG");
    return on;
}

static int active(void)
{
    return season_code_on() && (season_start_year() != SEASON_FIRST_YEAR || season_changing_rules());
}

/* The season whose rules hold now. */
static int current_season(void)
{
    if (!season_changing_rules()) return season_start_year();
    int p = season_change_point();
    return p > season_start_year() ? p : season_start_year();
}

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

/* ---- The rules objects ---- */

static uint32_t manager(void)
{
    uint32_t m = rd32(g_mem, MANAGER);
    return readable(m) ? m : 0;
}

/* The rules object of a nation (scope 1), or 0. The nation's record id is in the global of the table. */
static uint32_t rules_of(const SeasonNation *n)
{
    uint32_t m = manager();
    int32_t id = (int32_t)rd32(g_mem, n->key_global);
    if (!m || id < 0) return 0;
    for (int i = 0; i < RULES_COUNT; i++) {
        uint32_t r = rd32(g_mem, m + RULES_AT + 4u * (uint32_t)i);
        if (!readable(r) || rd8(g_mem, r + R_SCOPE) != 1) continue;
        uint32_t nat = rd32(g_mem, r + R_NATION);
        if (readable(nat) && (int32_t)rd32(g_mem, nat) == id) return r;
    }
    return 0;
}

/* ---- The windows ---- */

static void put_bound(uint8_t *p, int id, int index, int day, int month, int start)
{
    p[0] = (uint8_t)id;
    p[1] = (uint8_t)index;
    p[2] = 0xff;                                  /* no weekday */
    p[3] = (uint8_t)day;
    p[4] = (uint8_t)(month - 1);
    p[5] = (uint8_t)start;
}

/* The records the nation should have: regular windows from the table, and for a nation with a partial flag the
 * window of index 2 that the game keeps (Italy: free transfers only, 0x661520). Returns the count. */
static int build_records(uint32_t rules, const SeasonNationState *st, int partial, uint8_t *out, int room)
{
    int count = 0;
    uint32_t old = rd32(g_mem, rules + R_WINDOWS);
    int old_count = (int8_t)rd8(g_mem, rules + R_COUNT);
    /* Byte 0 of a bound is the index of the rules object in the manager (update_transfer_window_news 0x8c6d17,
     * 0x8c6d1c: [manager + byte * 4 + 0x8ac]), also the rules id +9 of the object. */
    int id = old_count > 0 && readable(old) ? (int)rd8(g_mem, old) : (int)rd8(g_mem, rules + R_ID);
    const uint8_t *kept = NULL;
    for (int i = 0; partial && readable(old) && i < old_count; i++)
        if (rd8(g_mem, old + 12u * (uint32_t)i + 1) == 2) kept = g_mem + old + 12u * (uint32_t)i;
    for (int w = 0; w < st->windows && count < room; w++) {
        if (kept && count == 2) {                 /* index 2 belongs to the kept window */
            memcpy(out + 12 * count, kept, 12);
            count++;
            if (count >= room) break;
        }
        int index = kept && w >= 2 ? w + 1 : w;
        put_bound(out + 12 * count, id, index, st->win[w].sd, st->win[w].sm, 1);
        put_bound(out + 12 * count + 6, id, index, st->win[w].ed, st->win[w].em, 0);
        count++;
    }
    if (kept && count <= 2 && count < room) {     /* fewer than two regular windows: the kept one follows */
        memcpy(out + 12 * count, kept, 12);
        count++;
    }
    return count;
}

/* The manager's sorted table of window bounds, made again as setup_transfer_rules does (0x8d2e27..0x8d2f78). */
static int rebuild_edges(uint32_t m)
{
    uint32_t total = 0;
    for (int i = 0; i < RULES_COUNT; i++) {
        uint32_t r = rd32(g_mem, m + RULES_AT + 4u * (uint32_t)i);
        if (readable(r) && (int8_t)rd8(g_mem, r + R_COUNT) > 0) total += 2u * (uint32_t)(int8_t)rd8(g_mem, r + R_COUNT);
    }
    uint32_t size[1] = {total * 4u ? total * 4u : 4u};
    uint32_t table = call_cdecl(MALLOC, 1, size);
    if (rt_unwinding || !readable(table)) return 0;
    uint32_t j = 0;
    for (int i = 0; i < RULES_COUNT; i++) {
        uint32_t r = rd32(g_mem, m + RULES_AT + 4u * (uint32_t)i);
        int n = readable(r) ? (int8_t)rd8(g_mem, r + R_COUNT) : 0;
        uint32_t arr = readable(r) ? rd32(g_mem, r + R_WINDOWS) : 0;
        for (int k = 0; k < n && readable(arr); k++) {
            wr32(g_mem, table + 4u * j++, arr + WINDOW_SIZE * (uint32_t)k);
            wr32(g_mem, table + 4u * j++, arr + WINDOW_SIZE * (uint32_t)k + 6u);
        }
    }
    wr32(g_mem, m + EDGES_AT, table);
    wr32(g_mem, m + EDGES_COUNT, j);
    uint32_t args[4] = {table, j, 4, WINDOW_COMPARE};
    call_cdecl(QSORT, 4, args);
    return !rt_unwinding;
}

/* Gives the nation's rules object the windows of the state. Returns 1 when it changed them. */
static int apply_windows(const SeasonNation *n, uint32_t rules, const SeasonNationState *st)
{
    if (st->windows == SEASON_NATION_KEEP) return 0;
    uint8_t want[WINDOW_SIZE * (SEASON_NATION_MAX_WINDOWS + 1)];
    int count = 0, partial = (n->flags & SEASON_NATION_PARTIAL_WINDOWS) != 0;
    if (st->windows != SEASON_NATION_WINDOWS_NONE) count = build_records(rules, st, partial, want, SEASON_NATION_MAX_WINDOWS + 1);
    else if (partial) return 0;                   /* the nation keeps a window: no table row gives none for it */
    uint32_t old = rd32(g_mem, rules + R_WINDOWS);
    int old_count = (int8_t)rd8(g_mem, rules + R_COUNT);
    if (old_count == count && (count == 0 || (readable(old) && !memcmp(g_mem + old, want, WINDOW_SIZE * (size_t)count))))
        return 0;
    if (count > 0) {
        uint32_t size[1] = {WINDOW_SIZE * (uint32_t)count};
        uint32_t arr = call_cdecl(MALLOC, 1, size);
        if (rt_unwinding || !readable(arr)) return 0;
        memcpy(g_mem + arr, want, WINDOW_SIZE * (size_t)count);
        wr32(g_mem, rules + R_WINDOWS, arr);
    }
    wr8(g_mem, rules + R_COUNT, (uint32_t)count);
    if (log_on()) {
        fprintf(stderr, "recomp: season nation: %s windows %d -> %d:", n->name, old_count, count);
        uint32_t arr = rd32(g_mem, rules + R_WINDOWS);
        for (int w = 0; w < count && readable(arr); w++)
            fprintf(stderr, " %d.%d-%d.%d(#%d)", rd8(g_mem, arr + 12u * (uint32_t)w + 3), rd8(g_mem, arr + 12u * (uint32_t)w + 4) + 1,
                    rd8(g_mem, arr + 12u * (uint32_t)w + 9), rd8(g_mem, arr + 12u * (uint32_t)w + 10) + 1,
                    rd8(g_mem, arr + 12u * (uint32_t)w + 1));
        fputc('\n', stderr);
    }
    return 1;
}

static void apply_caps(const SeasonNation *n, uint32_t rules, const SeasonNationState *st)
{
    if (st->sign_foreign != SEASON_NATION_KEEP && rd8(g_mem, rules + R_CAP_FOREIGN) != (uint32_t)st->sign_foreign) {
        if (log_on())
            fprintf(stderr, "recomp: season nation: %s foreign cap %u -> %d\n", n->name, rd8(g_mem, rules + R_CAP_FOREIGN),
                    st->sign_foreign);
        wr8(g_mem, rules + R_CAP_FOREIGN, (uint32_t)st->sign_foreign);
    }
    if (st->sign_non_eu != SEASON_NATION_KEEP && rd8(g_mem, rules + R_CAP_NON_EU) != (uint32_t)st->sign_non_eu) {
        if (log_on())
            fprintf(stderr, "recomp: season nation: %s non-EU cap %u -> %d\n", n->name, rd8(g_mem, rules + R_CAP_NON_EU),
                    st->sign_non_eu);
        wr8(g_mem, rules + R_CAP_NON_EU, (uint32_t)st->sign_non_eu);
    }
}

/* The rules of every nation for a season. */
static void apply_all(int season)
{
    uint32_t m = manager();
    if (!m) {
        if (log_on()) fprintf(stderr, "recomp: season nation: no transfer manager yet\n");
        return;
    }
    int changed = 0;
    for (int i = 0; i < season_nation_count && !rt_unwinding; i++) {
        const SeasonNation *n = &season_nations[i];
        uint32_t rules = rules_of(n);
        SeasonNationState st;
        if (!rules || !season_nation_state(n->name, season, &st)) continue;
        apply_caps(n, rules, &st);
        changed |= apply_windows(n, rules, &st);
    }
    if (changed && !rt_unwinding) rebuild_edges(m);
}

/* ---- The news ---- */

static const char *const month_names[] = {"January", "February", "March", "April", "May", "June", "July", "August",
                                          "September", "October", "November", "December"};

static void windows_text(const SeasonNationState *st, char *out, size_t room)
{
    out[0] = 0;
    for (int w = 0; w < st->windows && w < SEASON_NATION_MAX_WINDOWS; w++) {
        char one[96];
        snprintf(one, sizeof one, "%s%d %s to %d %s", w == 0 ? "" : (w == st->windows - 1 ? " and " : ", "), st->win[w].sd,
                 month_names[st->win[w].sm - 1], st->win[w].ed, month_names[st->win[w].em - 1]);
        strncat(out, one, room - strlen(out) - 1);
    }
}

static int windows_same(const SeasonNationState *a, const SeasonNationState *b)
{
    if (a->windows != b->windows) return 0;
    for (int w = 0; w < a->windows && w < SEASON_NATION_MAX_WINDOWS; w++)
        if (memcmp(&a->win[w], &b->win[w], sizeof a->win[w])) return 0;
    return 1;
}

static void limit_text(int v, char *out, size_t room, const char *kind)
{
    if (v == SEASON_NATION_NONE) snprintf(out, room, "no limit on %s players", kind);
    else snprintf(out, room, "at most %d %s players", v, kind);
}

/* The nations that have a competition in the game: only their changes are news. */
static int nation_in_game(const char *nation, int season)
{
    uint32_t comps[512];
    int n = season_change_comps(comps, 512);
    for (int i = 0; i < n; i++) {
        const char *file = season_change_file(comps[i]);
        const SeasonRule *r = strcmp(file, "?") ? season_rules_find(season, file, 0) : NULL;
        if (r && !strcmp(r->nation, nation)) return 1;
    }
    return 0;
}

static void news_for(const SeasonNation *n, int season)
{
    SeasonNationState was, now;
    season_nation_state(n->name, season - 1, &was);
    if (!season_nation_state(n->name, season, &now) || !nation_in_game(n->name, season)) return;
    char a[160], b[160];
    if (!windows_same(&was, &now) && now.windows != SEASON_NATION_KEEP) {
        if (now.windows == SEASON_NATION_WINDOWS_NONE) {
            season_change_news("%s: clubs can sign players at any time.", n->name);
        } else {
            windows_text(&now, a, sizeof a);
            season_change_news("%s: the transfer windows are now %s.", n->name, a);
        }
    }
    if (was.sign_non_eu != now.sign_non_eu && now.sign_non_eu != SEASON_NATION_KEEP) {
        limit_text(now.sign_non_eu, a, sizeof a, "non-EU");
        season_change_news("%s: a club may now hold %s.", n->name, a);
    }
    if (was.sign_foreign != now.sign_foreign && now.sign_foreign != SEASON_NATION_KEEP) {
        limit_text(now.sign_foreign, a, sizeof a, "foreign");
        season_change_news("%s: a club may now hold %s.", n->name, a);
    }
    if (was.squad_non_eu != now.squad_non_eu && now.squad_non_eu != SEASON_NATION_KEEP) {
        limit_text(now.squad_non_eu, a, sizeof a, "non-EU");
        season_change_news("%s: a match squad in the top division may now hold %s.", n->name, a);
    }
    if (was.squad_foreign != now.squad_foreign && now.squad_foreign != SEASON_NATION_KEEP) {
        limit_text(now.squad_foreign, b, sizeof b, "foreign");
        season_change_news("%s: a match squad in the top division may now hold %s.", n->name, b);
    }
    if (!was.brexit && now.brexit)
        season_change_news("%s: from 1 January 2021 players from the EU need a work permit to sign for a club.", n->name);
}

/* ---- The hooks ---- */

static void nation_new_game(int start_year)
{
    if (log_on()) fprintf(stderr, "recomp: season nation: new game %d, rules %s\n", start_year, active() ? "on" : "off");
    if (active()) apply_all(start_year);
}

static void nation_loaded(int start_year)
{
    (void)start_year;
    if (active()) apply_all(current_season());
}

static void nation_season(int season)
{
    if (!active()) return;
    apply_all(season);
    for (int i = 0; i < season_nation_count && !rt_unwinding; i++) news_for(&season_nations[i], season);
}

/* ---- The match squad limits ---- */

typedef struct {
    const char *file;
    int season;
    const SeasonNation *nation;     /* NULL: no nation rule for the file in that season */
} FileNation;
static FileNation file_cache[256];
static int file_cached;

/* The nation whose first division the competition class file is in the season, or NULL. */
static const SeasonNation *top_division_nation(const char *file, int season)
{
    for (int i = 0; i < file_cached; i++)
        if (file_cache[i].file == file && file_cache[i].season == season) return file_cache[i].nation;
    const SeasonRule *r = season_rules_find(season, file, 0);
    const SeasonNation *n = r && r->tier == 1 ? season_nation_find(r->nation) : NULL;
    if (file_cached < (int)(sizeof file_cache / sizeof file_cache[0])) file_cache[file_cached++] = (FileNation){file, season, n};
    return n;
}

static void set_slots(uint32_t rec, int first, int v)
{
    if (v == SEASON_NATION_KEEP) return;
    for (int s = first; s < first + 3; s++)
        if (rd8(g_mem, rec + (uint32_t)s) != 0xff) wr8(g_mem, rec + (uint32_t)s, (uint32_t)v);
}

/* TRANSFER_MANAGER restriction lookup 0x8cf6b0, thiscall (fixture, record, collapse flag), ret 0xc. */
void o_008cf6b0(void);
RT_WRAP(008cf6b0, restriction_lookup)
{
    uint32_t fixture = rd32(g_mem, cpu.esp + 4), rec = rd32(g_mem, cpu.esp + 8);
    o_008cf6b0();
    if (rt_unwinding || !active() || !readable(fixture) || !readable(rec)) return;
    uint32_t comp_rec = rd32(g_mem, fixture + 0x14);
    if (!readable(comp_rec)) return;
    int32_t id = (int32_t)rd32(g_mem, comp_rec);
    uint32_t table = rd32(g_mem, COMP_TABLE);
    if (id < 0 || id >= (int32_t)rd32(g_mem, COMP_SLOTS) || !readable(table)) return;
    uint32_t obj = rd32(g_mem, table + 4u * (uint32_t)id);
    if (!readable(obj)) return;
    const char *file = season_change_file(obj);
    int season = current_season();
    const SeasonNation *n = strcmp(file, "?") ? top_division_nation(file, season) : NULL;
    SeasonNationState st;
    if (!n || !season_nation_state(n->name, season, &st)) return;
    uint8_t before[6];
    memcpy(before, g_mem + rec, 6);
    set_slots(rec, 3, st.squad_non_eu);
    set_slots(rec, 0, st.squad_foreign);
    if (log_on() && memcmp(before, g_mem + rec, 6)) {
        static unsigned shown;
        if (shown++ < 12)
            fprintf(stderr, "recomp: season nation: %s %s %d squad record %02x %02x %02x %02x %02x %02x -> %02x %02x %02x %02x %02x %02x\n",
                    n->name, file, season, before[0], before[1], before[2], before[3], before[4], before[5], g_mem[rec],
                    g_mem[rec + 1], g_mem[rec + 2], g_mem[rec + 3], g_mem[rec + 4], g_mem[rec + 5]);
    }
}

/* ---- Work permits after Brexit ---- */

static int32_t nation_id_of(const char *name)
{
    const SeasonNation *n = season_nation_find(name);
    return n ? (int32_t)rd32(g_mem, n->key_global) : -2;
}

/* A club of the nation with the Brexit rule from the season of the game. */
static int brexit_club(uint32_t club)
{
    if (!readable(club)) return 0;
    uint32_t nat = rd32(g_mem, club + 0x53);
    if (!readable(nat) || (int16_t)rd16(g_mem, GAME_DATE + 2) < BREXIT_YEAR) return 0;
    int32_t id = (int32_t)rd32(g_mem, nat);
    static const char *const uk[] = {"England", "Scotland", "Wales", "Northern Ireland"};
    for (int i = 0; i < 4; i++) {
        if (id != nation_id_of(uk[i])) continue;
        SeasonNationState st;
        season_nation_state(uk[i], current_season(), &st);
        return st.brexit;
    }
    return 0;
}

/* The player is a citizen of the UK or Ireland (the Common Travel Area, no permit): either nationality. */
static int uk_or_irish(uint32_t player)
{
    static const char *const names[] = {"England", "Scotland", "Wales", "Northern Ireland", "Republic of Ireland"};
    for (int k = 0; k < 2; k++) {
        uint32_t nat = rd32(g_mem, player + (k ? 0x1eu : 0x1au));
        if (!readable(nat)) continue;
        int32_t id = (int32_t)rd32(g_mem, nat);
        for (int i = 0; i < 5; i++)
            if (id == nation_id_of(names[i])) return 1;
    }
    return 0;
}

static unsigned permit_asked, permit_refused;

/* TRANSFER_MANAGER::player_needs_work_permit 0x8c7560, thiscall (player, club, flag), ret 0xc. */
void o_008c7560(void);
RT_WRAP(008c7560, player_needs_work_permit)
{
    uint32_t player = rd32(g_mem, cpu.esp + 4), club = rd32(g_mem, cpu.esp + 8), flag = rd32(g_mem, cpu.esp + 12);
    if (flag && active() && readable(player) && brexit_club(club)) {
        uint32_t args[2] = {player, 0};
        Cpu saved = cpu;
        uint32_t eu = rt_callback(IS_EU, 2, args);
        if (!rt_unwinding) cpu = saved;
        if (rt_unwinding) return;
        if (eu && !uk_or_irish(player)) {
            permit_refused++;
            if (log_on() && (permit_refused <= 5 || permit_refused % 100 == 0))
                fprintf(stderr, "recomp: season nation: Brexit: a player of the EU needs a work permit (%u)\n",
                        permit_refused);
            rt_ret_stdcall(1, 3);
            return;
        }
    }
    permit_asked++;
    o_008c7560();
}

const SeasonRuleKind season_kind_nation = {"nation", nation_new_game, nation_loaded, nation_season, NULL, NULL, NULL};
