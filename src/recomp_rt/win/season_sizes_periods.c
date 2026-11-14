/* The periods of the Eerste Divisie and the Belgian Second Division at their real sizes (0.7.2 league sizes, slice 10,
 * D6; spec docs/specs/2026-11-13-league-sizes-0.7.2-spec.md). The math is season_periods.c.
 *
 * The classes (read in the original, 3.9.68):
 * - HolFirst::add_result 0x5f65b0 (thiscall: fixture, result) adds the result to the table (0x685d30 at 0x5f65bf),
 *   then gives the fixture a period by its round (+0x34): below 8 (`cmp ax, 8` at 0x5f65ce) period 0, below 16
 *   (0x5f65da) 1, below 26 (0x5f65e6) 2, else 3. It makes the period stages up to it (create_period_stage 0x5f6ad0 at
 *   0x5f6604, while +0x30 is below the period) and adds the result to the stage of the period ([+0xc][period],
 *   vtable +4 at 0x5f662b). create_period_stage gives the stage (League_Stage 0x68b100) its matches per club, +0xa7:
 *   10 for period 2 (`mov byte ptr [esp + 0x13], 0xa` at 0x5f6bd6) and 8 for the others (0x5f6be2), the period in
 *   bl (0x5f6bae), the object in esi (0x5f6aed).
 * - BelSecond::add_result 0x420110 is the same with three periods: below 10 (`cmp ax, 0xa` at 0x42012e) period 0,
 *   below 22 (0x42013a) 1, else 2. create_period_stage 0x4204d0 gives 10 for period 0 (`mov bl, 0xa` at 0x4205b9)
 *   and 12 for periods 1 and 2 (`mov bl, 0xc` at 0x4205cb), the period in al (0x4205a9), the object in esi (0x4204ec).
 * - So each class has fixed bounds of its 34 rounds. A grown league (Eerste Divisie 20: 38 rounds) would put rounds
 *   26 to 37 in period 3, whose stage wants 8 matches, and a club's results past +0xa7 write past the row's position
 *   array (init_table_row 0x687570 allocates +0xa7 bytes). Here (PATCHES of tools/recomp/league_sizes.py) the bounds
 *   and the lengths scale to the rounds of the season, R = meetings (+0x3c) x (count + odd - 1), as D6 says. At the
 *   class's own rounds (the base game, a league at its class count, a 0.7.1 save before its next season change, a
 *   template with no room, D4) they are the original's.
 * - The stages are complete when every row reached +0xa7 (League::check_complete 0x689ad0, 0x689b47..0x689b58), and
 *   League::update_for_date 0x6847c0 ends the season only when the table and every stage up to +0x30 are complete
 *   (0x6849aa..0x6849fa). With an odd count a club rests one round in each cycle, so some clubs play one match fewer
 *   in a period and the stage would never complete. For a period stage of an odd count the stage is complete when
 *   its matches are played (season_period_stage_done: the played of its rows add up to twice the matches), through
 *   rt_league_played_target (season_split.c, INSNS 0x689b47).
 * - Period winners: HolFirst::update_positions_and_status 0x5f6d20 and BelSecond's 0x420680 (vtable +0x94) call
 *   0x684cd0, then take the first period whose flag is 0 (HolFirst +0xee..+0xf1, 0x5f6d2e..0x5f6d66; BelSecond
 *   +0xf6..+0xf8, 0x42068b..0x4206b5), and once its stage exists give its winner status 1 in the table (0x5f6e09,
 *   0x420714) and set the flag (0x5f6e8e.., 0x42075e..). set_up_rules clears the flags each season (0x5f639b..
 *   0x5f63ad, 0x41ff0b..0x41ff17) and the save keeps them (0x5f7550, 0x420e30). The play-off takes the rows of
 *   status 1 and tops up with the rows of 0xff in table order (HolFirst 0x5f6720 to six at 0x5f677e, BelSecond
 *   0x420230 to four at 0x42028e). D6: periods on or off by the row. A row with no periods (0: Eerste Divisie
 *   2001/02 and 2002/03, Second Division 2007/08, 2008/09, 2015/16 and 2020/21 on) gets every flag set before the
 *   call, so no club wins a period and the play-off takes the places after the champion, as in real life (hol_first
 *   2001/02 places 2 to 7, bel_second 2007/08 places 2 to 5). A row with periods (or unknown periods) plays the
 *   class's periods, scaled. The stages are still made, so the season's stage count and the save stay the class's.
 *
 * - A split (slice 7, D7: the Second Division 2022/23, 12 clubs, 22 rounds then 6 and 6). The section fixtures stay
 *   with the parent (season_split.c) and their round (+0x34) is the section's own date index from 0 (League::
 *   create_fixtures 0x6838fe), so add_result would put them in periods 0 and 1, whose stages hold the matches of
 *   those periods only: a club's results past +0xa7 write past its row's position array. After the split (the
 *   marker in +0x3c, season_split_meetings) the wraps of add_result below give a result to the table only, through
 *   League::add_result 0x685d30, as the class does first (0x5f65bf, 0x42011f). The periods are those of the rounds
 *   before the split, so the rounds use the row's meetings, not the marker.
 *
 * The base game and RECOMP_LEAGUE_SIZES=off: every function returns the original (season_sizes_on). */
#include "win.h"
#include "../rt_host.h"
#include "season_change.h"
#include "season_league.h"
#include "season_periods.h"
#include "season_sizes.h"
#include "season_split.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEAGUES 0xadadfcu           /* the competition objects by id */
#define COMP_RECORD 4u
#define COMP_MEETINGS 0x3cu         /* s16 */
#define COMP_CLUBS 0x3eu            /* s16 */
#define COMP_YEAR 0x40u             /* s16: the edition */
#define COMP_NAME_CODE 0x3au        /* s16: a stage's name (League_Stage 0x68b211) */
#define COMP_MATCHES 0xa7u          /* s16: matches per club */
#define COMP_TABLE 0xb1u
#define ROW_SIZE 0x3bu
#define ROW_PLAYED 0x11u            /* s8 */
#define STAGE_VTABLE 0x0096d0f0u    /* League_Stage (0x68b196) */
#define PERIOD_NAME_FIRST 0x439     /* "Period 1".."Period 4" (0x5f6bb9..0x5f6bdd, 0x4205b4..0x4205c6) */
#define PERIOD_NAME_LAST 0x43c
#define CLASS_ROUNDS 34             /* both classes: 18 clubs, 2 meetings (D 0x22 at 0x5f57ca, 0x41f37e) */

typedef struct {
    const char *file;
    uint32_t key;                   /* the id of the division (HolCup 0x5f4820, BelFaCup 0x41d8d0) */
    int starts[3];                  /* the rounds where periods 1.. begin */
    int parts;
    uint32_t flags;                 /* the "winner given" flag of each period */
} PeriodClass;

static const PeriodClass CLASSES[] = {
    {"hol_first", 0x9cf56cu, {8, 16, 26}, 4, 0xeeu},
    {"bel_second", 0x9cf5a0u, {10, 22, 0}, 3, 0xf6u},
};
#define NCLASSES ((int)(sizeof CLASSES / sizeof CLASSES[0]))

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int logging(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_SIZE_LOG") && *getenv("RECOMP_SIZE_LOG");
    return on;
}

static uint32_t league_of(uint32_t key)
{
    uint32_t leagues = rd32(g_mem, LEAGUES);
    int32_t id = (int32_t)rd32(g_mem, key);
    if (!readable(leagues) || id < 0 || id > 0xffff) return 0;
    uint32_t league = rd32(g_mem, leagues + 4u * (uint32_t)id);
    return readable(league) ? league : 0;
}

/* The class of a HolFirst or BelSecond object, or NULL. */
static const PeriodClass *class_of(uint32_t obj)
{
    if (!readable(obj)) return NULL;
    for (int i = 0; i < NCLASSES; i++)
        if (league_of(CLASSES[i].key) == obj) return &CLASSES[i];
    return NULL;
}

/* The rounds of the season of the object: meetings x (count rounded up to even - 1). */
static int rounds_of(uint32_t obj)
{
    int m = season_split_meetings(obj), n = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    if (m < 1 || m > 4 || n < 2 || n > 60) return CLASS_ROUNDS;
    return m * (n + (n & 1) - 1);
}

/* One log line a season of each class: the periods of the season (tests/seasons/league_size_run.py reads it). */
static void log_periods(uint32_t obj, const PeriodClass *c, int rounds)
{
    static int logged[NCLASSES];
    int year = (int16_t)rd16(g_mem, obj + COMP_YEAR), i = (int)(c - CLASSES);
    if (!logging() || logged[i] == year) return;
    logged[i] = year;
    int season = season_sizes_row_season(obj, c->file);
    const SeasonLeagueRule *r = season < 0 ? NULL : season_league_row(c->file, season);
    fprintf(stderr, "recomp: league sizes: %s periods %d: rounds %d, lengths", c->file, year, rounds);
    for (int p = 0; p < c->parts; p++)
        fprintf(stderr, "%s%d", p ? "+" : " ", season_period_length(c->starts, c->parts, CLASS_ROUNDS, rounds, p));
    fprintf(stderr, ", winners %s\n", r && r->periods == 0 ? "off" : "on");
}

uint32_t rt_league_period_bound(uint32_t obj, uint32_t original)
{
    if (!season_sizes_on()) return original;
    const PeriodClass *c = class_of(obj);
    if (!c) return original;
    int rounds = rounds_of(obj);
    for (int k = 0; k < c->parts - 1; k++)
        if ((uint32_t)c->starts[k] == original) {
            log_periods(obj, c, rounds);
            return (uint32_t)season_period_bound(c->starts, c->parts, CLASS_ROUNDS, rounds, k);
        }
    return original;
}

uint32_t rt_league_period_length(uint32_t obj, uint32_t period, uint32_t original)
{
    if (!season_sizes_on()) return original;
    const PeriodClass *c = class_of(obj);
    if (!c || (int)period < 0 || (int)period >= c->parts) return original;
    int len = season_period_length(c->starts, c->parts, CLASS_ROUNDS, rounds_of(obj), (int)period);
    return len >= 1 && len <= 127 ? (uint32_t)len : original;
}

uint32_t season_periods_played_target(uint32_t obj, uint32_t original)
{
    if (!season_sizes_on() || !readable(obj) || rd32(g_mem, obj) != STAGE_VTABLE) return original;
    int name = (int16_t)rd16(g_mem, obj + COMP_NAME_CODE), n = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    if (name < PERIOD_NAME_FIRST || name > PERIOD_NAME_LAST || n < 3 || n > 60 || !(n & 1)) return original;
    const PeriodClass *c = NULL;
    for (int i = 0; i < NCLASSES && !c; i++) {
        uint32_t parent = league_of(CLASSES[i].key);
        if (parent && rd32(g_mem, parent + COMP_RECORD) == rd32(g_mem, obj + COMP_RECORD) &&
            rd16(g_mem, parent + COMP_YEAR) == rd16(g_mem, obj + COMP_YEAR))
            c = &CLASSES[i];
    }
    uint32_t t = rd32(g_mem, obj + COMP_TABLE);
    if (!c || !readable(t)) return original;
    int sum = 0;
    for (int i = 0; i < n; i++) sum += (int8_t)g_mem[t + (uint32_t)i * ROW_SIZE + ROW_PLAYED];
    return season_period_stage_done(n, (int16_t)rd16(g_mem, obj + COMP_MATCHES), sum) ? 0 : original;
}

void season_periods_winners(uint32_t obj)
{
    if (!season_sizes_on()) return;
    const PeriodClass *c = class_of(obj);
    if (!c) return;
    int season = season_sizes_row_season(obj, c->file);
    const SeasonLeagueRule *r = season < 0 ? NULL : season_league_row(c->file, season);
    if (!r || r->periods != 0) return;
    for (int p = 0; p < c->parts; p++) g_mem[obj + c->flags + (uint32_t)p] = 1;
}

/* HolFirst::update_positions_and_status 0x5f6d20, thiscall, no arguments (BelSecond's 0x420680 is wrapped in
 * season_bel_second.c, which calls season_periods_winners). */
void o_005f6d20(void);
RT_WRAP(005f6d20, hol_first_status)
{
    season_periods_winners(cpu.ecx);
    o_005f6d20();
}

/* 1 for a HolFirst or BelSecond object after its split (the marker in +0x3c). */
static int after_split(uint32_t obj)
{
    return season_sizes_on() && class_of(obj) &&
           season_split_meetings(obj) != (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
}

#define F_LEAGUE_ADD_RESULT 0x00685d30u   /* League::add_result, thiscall (fixture, result), ret 8 */

/* HolFirst::add_result 0x5f65b0 and BelSecond::add_result 0x420110, thiscall (fixture, result), ret 8, return 1
 * (0x5f6638, 0x42018a). */
static void add_result(void (*original)(void))
{
    uint32_t self = cpu.ecx;
    if (!after_split(self)) {
        original();
        return;
    }
    uint32_t a[2] = {rt_arg(0), rt_arg(1)};
    Cpu saved = cpu;
    cpu.ecx = self;
    rt_callback(F_LEAGUE_ADD_RESULT, 2, a);
    if (rt_unwinding) return;
    cpu = saved;
    rt_ret_stdcall(1, 2);
}

void o_005f65b0(void);
RT_WRAP(005f65b0, hol_first_add_result) { add_result(o_005f65b0); }

void o_00420110(void);
RT_WRAP(00420110, bel_second_add_result) { add_result(o_00420110); }
