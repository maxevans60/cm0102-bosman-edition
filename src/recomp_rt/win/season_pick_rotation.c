/* The pick's rotation keeps enough players for the match it picks for (season games only).
 *
 * MATCH_MAN::pick_team 0x6dbc50 rotates an AI club of reputation 6000 or more (club +0x80, 0x6dbdc7) with more than
 * 30 players (0x53c590, 0x6dbe0c): it looks for the club's next fixture in the 7 days after the match
 * (FIXTURES search 0x5acbe0, called at 0x6dbe7e) and weighs the two matches (0x6dbe8c..0x6dc518). When the next match
 * counts more, it sets the pick's mode +0x1a to 0, 1 or 2 (0x6dc544, 0x6dc572, 0x6dc593). Its first loop
 * (0x6dc6b9..0x6dc88a) takes the players who can play the next match (bans 0x55d270 and eligibility 0x8cf5e0 with the
 * next fixture, 0x6dc74e and 0x6dc75e), and 0x6e2000 keeps the best 14 of them back. The second loop (0x6dc9f1) picks
 * the match from the rest: a player kept back is skipped (0x6dcae7), and the current match's eligibility comes only
 * there (0x8cf5e0 with the current fixture, 0x6dcbe6). Nothing checks that the rest can field 11.
 *
 * In gate (a) run 8 (2005 start, September 2005 database, on ec579543), AJ Auxerre (45 players, 25 registered for the
 * UEFA Cup) played its UEFA Cup group match at Siroki Brijeg on day 348 of 2005, with a Coupe de France tie on day 350.
 * The pick took mode 0 and kept 14 registered players back for the cup tie. Of the rest, 7 were registered (code 0 of
 * EUROPE_RULES::get_ineligibility_code 0x5832e0) and 24 were not (code 8). With 7 players, init_match 0x6b7f20 stopped
 * at match_eng.cpp 652 (0x6b8aca).
 *
 * The original keeps back at most 14 players who can play the current match too. So in a season game, when fewer than
 * 25 squad players can play the current match (not injured, 0x62c400 not 2, and eligible, 0x8cf5e0 code 0), the pick
 * finds no next fixture and does not rotate: the search at 0x6dbe7e returns 0 fixtures. The base game keeps the
 * original (Q152).
 *
 * The reserve side (open item 237). For a club's reserve side (pick +0x19 not 0) pick_team skips the rotation test
 * (0x6dbdbe) and keeps back players for the first team by the pick's mode +0x1a, which its caller sets
 * (0x6dc59f..0x6dc6a4): 14 for mode 0, 7 for 1, 2 for 2, none for 3. Its first loop (0x6dc6b9..0x6dc87c) takes the
 * players who can play (not injured, 0x62c400 at 0x6dc6f4; no ban, 0x55d270 at 0x6dc71f; no national duty, 0x7757d0
 * at 0x6dc7b1 and 0x6dc7d0; not away on loan, 0x4cf6d0 and 0x4cf5a0 at 0x6dc7e4..0x6dc81f), and 0x6e1940 at 0x6dc8a6
 * keeps the best of them back. The second loop picks the side from the rest; for a reserve side it also drops a
 * player who is recovering (0x62c400 at 0x6dcb88). As for the first team above, nothing checks that the rest can
 * field 11. init_match 0x6b7f20 then stops at match_eng.cpp 612 or 652 (0x6b8973, 0x6b8aca).
 *
 * The case: since item 229 a season game gets the database's loans, and a player on loan keeps his squad slot at his
 * parent club (the loan is a second contract). In a 2014 start (October 2014, all nations, changing rules on, seed 1,
 * session sz02) GNK Dinamo Zagreb had all 50 slots filled on day 269 of 2014, 18 of those players away on loan and 4
 * injured or recovering. The pick kept 14 back for the first team, and its Reserve side (record 87) had 10. The
 * original's answer to a short side, the virtual players of VIRTUAL_STAFF::top_up_club_squad 0x90dae0, needs a free
 * slot, and there was none.
 *
 * The fix, in a season game, for a club's reserve side: when the pick gives fewer than 11 players, it runs again
 * with the next mode (fewer players kept back), up to mode 3, until the side has 11. pick_team writes every slot of
 * its selection (0x6e0d00 at 0x6dd6de), so a later pick replaces an earlier one. The mode is set back after the pick.
 * Logged. The base game keeps the original (Q152). Test switch RECOMP_TEST_RESERVE_PICK=original: the original's
 * short side in a season game too (the proof of tests/seasons/reserve_pick_run.py). */
#include "win.h"
#include "season.h"
#include "season_names.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQUAD_SLOTS 50
#define KEPT_BACK 14
#define INJURY 0x62c400u                /* thiscall [0xb63d28] (staff), 2 = cannot play */
#define INJURIES 0xb63d28u
#define ELIGIBILITY 0x8cf5e0u           /* thiscall [0xde1f54] (staff, fixture), 0 = can play */
#define TRANSFERS 0xde1f54u
#define NEXT_SEARCH_RET 0x6dbe83u       /* the return address of the next-fixture search in 0x6dbc50 */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static uint32_t call_this(uint32_t fn, uint32_t self, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, n, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t no_rotation_pick;       /* the pick (0x6dbc50 this) whose next-fixture search finds nothing */
static int no_rotation_free;

/* The squad players who can play the pick's fixture, as the second loop of 0x6dbc50 tests them. */
static int can_play(uint32_t pick)
{
    uint32_t squad = rd32(g_mem, pick + 0x27), fixture = rd32(g_mem, pick + 0x37);
    int n = 0;
    for (uint32_t i = 0; i < SQUAD_SLOTS && !rt_unwinding; i++) {
        uint32_t p = rd32(g_mem, squad + 4 * i);
        if (!readable(p) || !rd32(g_mem, p + 0x61)) continue;
        uint32_t a[2] = {p, fixture};
        if ((call_this(INJURY, rd32(g_mem, INJURIES), 1, a) & 0xff) == 2 || rt_unwinding) continue;
        if ((call_this(ELIGIBILITY, rd32(g_mem, TRANSFERS), 2, a) & 0xff) == 0 && !rt_unwinding) n++;
    }
    return n;
}

void o_006dbc50(void);

static void pick_rotation(void)
{
    uint32_t pick = cpu.ecx, fixture = rd32(g_mem, pick + 0x37), club = rd32(g_mem, pick + 0x2f);
    if (!season_game_changes() || g_mem[pick + 0x19] || g_mem[pick + 0x1a] != 3 || !readable(fixture) ||
        rd32(g_mem, fixture) == 0xfffffffeu || !readable(club) || (int16_t)rd16(g_mem, club + 0x80) < 6000 ||
        !readable(rd32(g_mem, pick + 0x27))) {
        o_006dbc50();
        return;
    }
    Cpu saved = cpu;
    int n = can_play(pick);
    if (rt_unwinding) return;
    cpu = saved;
    if (n >= KEPT_BACK + 11) {
        o_006dbc50();
        return;
    }
    no_rotation_pick = pick;
    no_rotation_free = n;
    o_006dbc50();
    no_rotation_pick = 0;
}

static const int kept_back[4] = {14, 7, 2, 0};   /* by mode +0x1a, 0x6dc5e4..0x6dc6a4 */

static int reserve_original(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("RECOMP_TEST_RESERVE_PICK"); v = e && !strcmp(e, "original"); }
    return v;
}

/* A club of the club table ([0xae23b4], [0xae235c] rows of 0x245), not a national side. */
static int club_side(uint32_t club)
{
    uint32_t base = rd32(g_mem, 0xae23b4u), end = base + rd32(g_mem, 0xae235cu) * 0x245u;
    return readable(base) && club >= base && club < end && (club - base) % 0x245u == 0;
}

static int reserve_again;               /* a later pick of the reserve fix is running */

RT_WRAP(006dbc50, pick_team_season)
{
    uint32_t pick = cpu.ecx, opponent = rt_arg(0), mode = rt_arg(1), club = rd32(g_mem, pick + 0x2f);
    uint8_t keep = g_mem[pick + 0x1a];
    if (reserve_again || !season_game_changes() || reserve_original() || !g_mem[pick + 0x19] || keep >= 3 ||
        !club_side(club)) {
        pick_rotation();
        return;
    }
    pick_rotation();
    int first = (int8_t)cpu.eax, n = first;
    if (rt_unwinding || n >= 11) return;
    Cpu after = cpu;
    uint8_t m = keep;
    while (n < 11 && m < 3 && !rt_unwinding) {
        uint32_t a[2] = {opponent, mode};
        g_mem[pick + 0x1a] = ++m;
        reserve_again = 1;
        cpu.ecx = pick;
        n = (int8_t)rt_callback(0x6dbc50u, 2, a);
        reserve_again = 0;
    }
    if (rt_unwinding) return;
    g_mem[pick + 0x1a] = keep;
    cpu = after;
    cpu.eax = (after.eax & 0xffffff00u) | (uint8_t)n;
    uint32_t fixture = rd32(g_mem, pick + 0x37), rec = readable(fixture) ? rd32(g_mem, fixture + 0x14) : 0;
    fprintf(stderr, "recomp: reserve pick: %.30s keeps %d players back for its first team, not %d, and fields %d, not %d, "
                    "in the %.30s match on day %d of %d\n", win_str(club + 4), kept_back[m], kept_back[keep], n, first,
            readable(rec) ? win_str(rec + 4) : "?", readable(fixture) ? (int16_t)rd16(g_mem, fixture + 0x2a) : -1,
            readable(fixture) ? (int16_t)rd16(g_mem, fixture + 0x28) : -1);
}

void o_005acbe0(void);

RT_WRAP(005acbe0, pick_rotation_next)
{
    uint32_t ret = rd32(g_mem, cpu.esp), self = no_rotation_pick;
    o_005acbe0();
    if (rt_unwinding || !self || ret != NEXT_SEARCH_RET || !(cpu.eax & 0xffff)) return;
    uint32_t fixture = rd32(g_mem, self + 0x37), club = rd32(g_mem, self + 0x2f), rec = rd32(g_mem, fixture + 0x14);
    fprintf(stderr, "recomp: pick rotation: %.30s keeps its players for the %.30s match on day %d of %d: %d players can "
                    "play it, fewer than %d\n", win_str(club + 4), readable(rec) ? win_str(rec + 4) : "?",
            (int16_t)rd16(g_mem, fixture + 0x2a), (int16_t)rd16(g_mem, fixture + 0x28), no_rotation_free, KEPT_BACK + 11);
    cpu.eax &= 0xffff0000u;             /* no fixture in the next 7 days: no rotation */
}
