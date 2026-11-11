/* The Ekstraklasa in its real format of each season (GitHub issue 92, 0.6.2 open item 221).
 *
 * The original's PolFirst plays the 2001/02 format in every season: two groups of 8, then a championship group and
 * a relegation group of 8. The rules table (pol_first, docs/research/rules-0.6.0/gre-pol-cro.tsv) gives one table
 * from 2002/03 (16 clubs, 14 in 2003/04 and 2004/05, 18 from 2021/22) and from 2013/14 to 2019/20 a split after 30
 * rounds: the top 8 and the bottom 8 play 7 more matches, with the points halved (rounded up) to 2016/17 and kept
 * in full from 2017/18 (en.wikipedia.org, Ekstraklasa: "The 2016-17 season was the last when teams started an
 * extra round with half the points"). With changing rules on, this module plays that format. The 2001/02 row, the
 * base game and changing rules off keep the original.
 *
 * The class (vtable 0x96e360):
 * - the constructor 0x7c8a80 allocates 2 stage slots at +0xc (0x7c8b06..0x7c8b1f, +0x2c = 2, +0x30 = -1), calls
 *   set_up_rules 0x7c9690 (which asks vtable +0x3c, create_schedule 0x7c8df0, for stage -1 at 0x7c9727),
 *   create_table 0x7c9450, create_fixtures 0x6835c0, the sort 0x6827d0 and create_second_group_stage 0x7c9900,
 *   then sorts the 16 club slots at +0xee (qsort count 16 at 0x7c8ce5) and sets 16 predicted positions (loop to
 *   16, cmp di, 0x10 at 0x7c8d09). start_new_season 0x7c9740 runs the same steps after the promotion chain;
 * - create_table puts 8 clubs in the object's own table (group A: +0x3e = 8 at 0x7c9469, 0x1d8 bytes at
 *   0x7c945f) and all 16 in the slot list +0xee by their last place (0x7c95a0..0x7c963d);
 * - create_second_group_stage makes group B, a League_Stage of slots 8 to 15, in stage slot 0 and sets +0x30 = 0;
 * - advance_stage 0x7c9ad0 moves +0x30 on and at 1 calls create_final_stage 0x7c9b60: one League_Stage of 16 in
 *   slot 1 (the top 4 of each group, then the bottom 4; club +0x60 the record of the championship group [0x9cf814]
 *   or the relegation group [0x9cf818]; points halved at 0x7c9df5..0x7c9e18; 14 matches, +0xa7 0xe at 0x7c9d54;
 *   places 0, 0, 2, 2 from the stack at 0x7c9b85..0x7c9b92), the order 0x7ca450 at +0xa3 (championship group
 *   first), and the fixtures of two League_Stages of 8 that it deletes again (0x7c9f4e, 0x7c9ffb);
 * - process_promotion_relegation_chain 0x7ca490 (vtable +0xb0) and PolSecond::create_play_off_stage 0x7cc290
 *   (0x7cc33a..0x7cc34f) read stage slot 1 as the final table.
 *
 * One table: create_table takes every club of the division (+0x3e and +0xee as long as the division), the date
 * template is the original's two templates (stage -1, 14 dates from July, and stage 1, 14 dates to June) with
 * extra midweek rounds (or fewer) for 2 (n - 1) rounds, and there is no stage: +0x2c = 0 and +0x30 = -1, as the
 * 3. Liga (season_divisions.c). Stage slot 1 holds the object itself, so the promotion chain and PolSecond read
 * its table. The status functions 0x7ca2a0, 0x7ca320 and 0x7ca3f0 treat stage -1 as the first group stage ("qualify
 * for championship group" news for the leader, 0x9b549c), so in one table they get stage 1, as the final table.
 *
 * The split: the same first 30 rounds with +0x2c = 1. When they end, League::update_for_date 0x6847c0 calls
 * advance_stage (+0x30 from -1 to 0), and this module makes the final stage as create_final_stage does, from the
 * table: the top 8 and the others, 7 matches (each pair once), points halved or kept by the season, the places of
 * the season (+0xc0, +0xc1). It goes in slot 0 and slot 1. */
#include "win.h"
#include "season.h"
#include "season_league.h"
#include "season_names.h"
#include "season_rules.h"
#include "season_pol_first.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUBS 0xae23b4u             /* the club table (0x245 bytes a club) */
#define CLUB_COUNT 0xae235cu
#define CLUB_SIZE 0x245u
#define CLUB_DIVISION 0x57u
#define CLUB_GROUP 0x60u            /* club_comp pointer: the final group (0x7c9c5b) */
#define COMPS 0xae23d0u             /* the club_comp table, 0x6b bytes a record */
#define COMP_SIZE 0x6bu
#define KEY_POL_FIRST 0x9cf858u     /* the record id of the Polish First Division (0x7c95ad) */
#define KEY_CHAMPIONS 0x9cf814u     /* the championship group (0x7c9c38) */
#define KEY_RELEGATION 0x9cf818u    /* the relegation group (0x7c9cc7) */
#define POL_FIRST_VTABLE 0x96e360u  /* 0x7c8ac7 */

#define OBJ_RECORD 4u
#define OBJ_STAGES 0xcu
#define OBJ_STAGE_COUNT 0x2cu
#define OBJ_STAGE_NOW 0x30u
#define OBJ_CLUBS 0x3eu
#define OBJ_YEAR 0x40u
#define OBJ_MATCHES 0xa7u
#define OBJ_ORDER_FN 0xa3u          /* the row compare of a stage (0x7c9ead) */
#define OBJ_TABLE 0xb1u
#define OBJ_SLOTS 0xeeu
#define ROW_SIZE 0x3bu
#define DATE_SIZE 0x41u             /* a date of the template: +0 s16 day of the year, +2 s16 year */
#define STAGE_SIZE 0xeeu            /* a League_Stage (0x7c9d23) */

#define F_SCHEDULE 0x007c8df0u      /* PolFirst::create_schedule, thiscall (stage, &count, &code, &flag), ret 0x10 */
#define F_NEW 0x00944cf1u           /* operator new, cdecl (size) */
#define F_MALLOC 0x00944e46u        /* cdecl (size) */
#define F_FREE 0x009452cau          /* cdecl (block) */
#define F_LEAGUE_STAGE 0x0068b100u  /* League_Stage::League_Stage, thiscall, 22 arguments */
#define F_ROW 0x00687430u           /* League::init_table_row, thiscall (index, club, 0, 0), ret 0x10 */
#define F_TV 0x00683010u            /* League::pay_tv_rights, thiscall (amount, 0), ret 8 */
#define F_SORT 0x006827d0u          /* League::sort_table, thiscall (0), ret 4 */
#define F_FIND_ROW 0x0068ab70u      /* League::find_club_row, thiscall (club, 0), ret 8 */
#define F_PRIZE 0x00682f70u         /* League::calc_prize_fund, thiscall (share), ret 4 */
#define POL_ORDER 0x007ca450u       /* the row compare of the final stage: championship group first */

#define MIN_CLUBS 10                /* more than the original's group A of 8, so the two forms never mix */
#define MAX_CLUBS 24
#define SPLIT_TOP 8                 /* the championship round: the top 8 (rules table, 2013/14 to 2019/20) */
#define MAX_DATES 64

enum { ORIGINAL = 0, ONE_TABLE = 1, SPLIT = 2 };

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int logging(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_POL_FIRST_LOG") && *getenv("RECOMP_POL_FIRST_LOG");
    return on;
}

static uint32_t call_this(uint32_t fn, uint32_t self, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t game_malloc(uint32_t size)
{
    uint32_t a[1] = {size};
    uint32_t p = call_cdecl(F_MALLOC, 1, a);
    if (rt_unwinding || !readable(p)) return 0;
    memset(g_mem + p, 0, size);
    return p;
}

static void game_free(uint32_t p)
{
    if (!readable(p) || rt_unwinding) return;
    uint32_t a[1] = {p};
    call_cdecl(F_FREE, 1, a);
}

/* The format of a season (its start year): the rules table row of pol_first (after its last row the row stays,
 * Q248). The 2001/02 row ("two groups of 8") and a season with no row are the original's. */
static int format_of(int year)
{
    int s = season_rules_last_season(year, "pol_first");
    const SeasonRule *r = s ? season_rules_find(s, "pol_first", 0) : NULL;
    if (!r || (r->format_text && strstr(r->format_text, "two groups"))) return ORIGINAL;
    for (int i = 0; i < season_league_rule_count; i++) {
        const SeasonLeagueRule *e = &season_league_rules[i];
        if (e->season == s && !strcmp(e->file, "pol_first")) return e->split_after > 0 ? SPLIT : ONE_TABLE;
    }
    return ONE_TABLE;
}

/* 1 when the points are halved at the split of a season (split_points 1 of the league rules). */
static int halved(int year)
{
    int s = season_rules_last_season(year, "pol_first");
    for (int i = 0; i < season_league_rule_count; i++) {
        const SeasonLeagueRule *e = &season_league_rules[i];
        if (e->season == s && !strcmp(e->file, "pol_first")) return e->split_points == 1;
    }
    return 0;
}

static int switched_on(void)
{
    static int off = -1;
    if (off < 0) off = getenv("RECOMP_POL_FIRST") && !strcmp(getenv("RECOMP_POL_FIRST"), "off");
    return !off && season_game_changes() && season_changing_rules();
}

static int is_pol_first(uint32_t obj) { return readable(obj) && rd32(g_mem, obj) == POL_FIRST_VTABLE; }

/* The format of a PolFirst object in the season of its year +0x40. */
static int mode_of(uint32_t obj)
{
    if (!switched_on() || !is_pol_first(obj)) return ORIGINAL;
    return format_of((int16_t)rd16(g_mem, obj + OBJ_YEAR));
}

/* A PolFirst object in this module's form: the original always has 2 stages. */
static int ours(uint32_t obj)
{
    if (!is_pol_first(obj)) return 0;
    int32_t n = (int32_t)rd32(g_mem, obj + OBJ_STAGE_COUNT);
    return n == 0 || n == 1;
}

/* The clubs of the division of the object, in the order of the club table: count, and the list when not NULL. */
static int division_clubs(uint32_t obj, uint32_t *list, int most)
{
    uint32_t rec = rd32(g_mem, obj + OBJ_RECORD), table = rd32(g_mem, CLUBS);
    int count = (int)rd32(g_mem, CLUB_COUNT), n = 0;
    if (!readable(rec) || !readable(table)) return 0;
    int32_t id = (int32_t)rd32(g_mem, rec);
    for (int i = 0; i < count; i++) {
        uint32_t club = table + (uint32_t)i * CLUB_SIZE, div = rd32(g_mem, club + CLUB_DIVISION);
        if (!readable(div) || (int32_t)rd32(g_mem, div) != id) continue;
        if (list && n < most) list[n] = club;
        n++;
    }
    return n;
}

static int club_count(uint32_t obj)
{
    int n = division_clubs(obj, NULL, 0);
    return n < MIN_CLUBS ? 0 : n > MAX_CLUBS ? MAX_CLUBS : n;
}

/* The rounds of the first phase: 2 (n - 1) for an even n, a round more for an odd n (a club rests). The split adds
 * the rounds of its top group, each pair once. */
static int first_rounds(int n) { return 2 * (n + (n & 1) - 1); }
static int split_rounds(void) { return SPLIT_TOP - 1; }

/* ---- the date template ---- */

static int inner;                   /* 1 while this module asks the original for a template */

/* One original template (stage -1 or 1): its dates are copied to out from index at; returns the new count. */
static int original_dates(uint32_t self, int stage, uint8_t *out, int at)
{
    uint32_t saved_esp = cpu.esp;
    cpu.esp -= 16;
    uint32_t count = cpu.esp, code = cpu.esp + 4, flag = cpu.esp + 8;
    wr32(g_mem, count, 0), wr32(g_mem, code, 0), wr32(g_mem, flag, 0);
    uint32_t a[4] = {(uint32_t)(stage & 0xff), count, code, flag};
    inner = 1;
    uint32_t t = call_this(F_SCHEDULE, self, 4, a);
    inner = 0;
    if (rt_unwinding) return -1;
    int n = (int16_t)rd16(g_mem, count);
    cpu.esp = saved_esp;
    if (!readable(t) || n <= 0 || at + n > MAX_DATES) {
        game_free(t);
        return -1;
    }
    memcpy(out + (size_t)at * DATE_SIZE, g_mem + t, (size_t)n * DATE_SIZE);
    game_free(t);
    return at + n;
}

static int day_of(const uint8_t *d) { return (int16_t)(d[0] | d[1] << 8); }
static int year_of(const uint8_t *d) { return (int16_t)(d[2] | d[3] << 8); }

/* The season's dates in order: the original's 28, with extra rounds on the Wednesday after a date whose next date
 * is at least a week later in the same year (as the 3. Liga, season_divisions.c), spread over the season, or with
 * dates taken out at even points. Returns the count (want), or 0. */
static int season_dates(uint32_t self, uint8_t *out, int want)
{
    static uint8_t base[MAX_DATES * DATE_SIZE];
    int have = original_dates(self, -1, base, 0);
    if (have > 0) have = original_dates(self, 1, base, have);
    if (have <= 0 || want <= 0 || want > MAX_DATES) return 0;
    if (want <= have) {                 /* fewer rounds: drop have - want dates at even points */
        int drop = have - want, k = 0, done = 0;
        for (int i = 0; i < have; i++) {
            if (done < drop && i == (done + 1) * have / (drop + 1)) {
                done++;
                continue;
            }
            memcpy(out + (size_t)k++ * DATE_SIZE, base + (size_t)i * DATE_SIZE, DATE_SIZE);
        }
        return k == want ? want : 0;
    }
    int extra = want - have;
    for (int gap = 7; gap >= 3; gap -= 4) {     /* a week's gap first, then any gap of 3 days or more */
        int k = 0, done = 0;
        for (int i = 0; i < have; i++) {
            const uint8_t *a = base + (size_t)i * DATE_SIZE;
            memcpy(out + (size_t)k++ * DATE_SIZE, a, DATE_SIZE);
            if (done >= extra || i + 1 >= have || i < done * have / (extra + 1)) continue;
            const uint8_t *b = a + DATE_SIZE;
            int day = day_of(a), step = gap == 7 ? 4 : 1;
            if (year_of(a) == year_of(b) && day_of(b) - day >= gap && day + step < 365) {
                uint8_t *c = out + (size_t)k++ * DATE_SIZE;
                memcpy(c, a, DATE_SIZE);
                c[0] = (uint8_t)((day + step) & 0xff), c[1] = (uint8_t)((day + step) >> 8);
                done++;
            }
        }
        if (done == extra) return want;
    }
    return 0;
}

/* PolFirst::create_schedule 0x7c8df0 (thiscall: stage, &count, &code, &flag; ret 0x10). One table and the split:
 * stage -1 gets the first phase's dates, stage 1 (the split) the last 7. */
void o_007c8df0(void);
RT_WRAP(007c8df0, pol_first_schedule)
{
    uint32_t self = cpu.ecx, count_at = rt_arg(1);
    int stage = (int8_t)(rt_arg(0) & 0xff), mode = inner ? ORIGINAL : mode_of(self);
    o_007c8df0();
    if (rt_unwinding || mode == ORIGINAL || (stage != -1 && !(stage == 1 && mode == SPLIT))) return;
    int n = club_count(self);
    if (!n || !readable(count_at) || !readable(cpu.eax)) return;
    int first = first_rounds(n), total = first + (mode == SPLIT ? split_rounds() : 0);
    static uint8_t dates[MAX_DATES * DATE_SIZE];
    uint32_t old = cpu.eax;
    if (season_dates(self, dates, total) != total) {
        fprintf(stderr, "recomp: pol first: no template of %d dates for %d clubs, the original's stays\n", total, n);
        return;
    }
    int from = stage == -1 ? 0 : first, k = stage == -1 ? first : total - first;
    uint32_t t = game_malloc((uint32_t)k * DATE_SIZE);
    if (!t) return;
    memcpy(g_mem + t, dates + (size_t)from * DATE_SIZE, (size_t)k * DATE_SIZE);
    game_free(old);
    wr16(g_mem, count_at, (uint16_t)k);
    cpu.eax = t;
    if (logging())
        fprintf(stderr, "recomp: pol first: %d/%02d stage %d: %d dates for %d clubs (%s)\n",
                (int16_t)rd16(g_mem, self + OBJ_YEAR), ((int16_t)rd16(g_mem, self + OBJ_YEAR) + 1) % 100, stage, k,
                n, mode == SPLIT ? "split" : "one table");
}

/* ---- the table ---- */

/* PolFirst::create_table 0x7c9450 (thiscall, returns 1, ret 0). One table and the split: every club of the
 * division in the table and in the slot list +0xee, in the order of the club table; the TV rights as 0x7c966c. */
void o_007c9450(void);
RT_WRAP(007c9450, pol_first_table)
{
    uint32_t self = cpu.ecx;
    int mode = mode_of(self), n = mode == ORIGINAL ? 0 : club_count(self);
    if (!n) {
        o_007c9450();
        return;
    }
    uint32_t list[MAX_CLUBS];
    division_clubs(self, list, n);
    uint32_t table = game_malloc((uint32_t)n * ROW_SIZE), slots = table ? game_malloc(4u * (uint32_t)n) : 0;
    if (!slots) {
        game_free(table);
        if (!rt_unwinding) rt_ret_stdcall(0, 0);
        return;
    }
    game_free(rd32(g_mem, self + OBJ_SLOTS));
    wr32(g_mem, self + OBJ_TABLE, table);
    wr32(g_mem, self + OBJ_SLOTS, slots);
    wr16(g_mem, self + OBJ_CLUBS, (uint16_t)n);
    wr16(g_mem, self + OBJ_MATCHES, 0xffffu);     /* check_complete counts (n - 1) x meetings again */
    /* The places of the original's final table (0, 0, 2, 2 at 0x7c9b85..0x7c9b92): set_up_rules gives the object 4
     * up for its group (+0xbe at 0x7c96d9) and 0 for the others (0x7c96e1..0x7c96ed), and the season code sets none
     * for pol_first (the I liga has a floor, season_sizes.c). PolSecond::create_play_off_stage 0x7cc290 takes the 2
     * rows of status 2 (0x7cc34f) into its play-off of 4, so the exchange stays the original's: 2 down and 2 to the
     * play-off. */
    g_mem[self + 0xbe] = 0, g_mem[self + 0xbf] = 0, g_mem[self + 0xc0] = 2, g_mem[self + 0xc1] = 2;
    for (int i = 0; i < n; i++) {
        wr32(g_mem, slots + 4u * (uint32_t)i, list[i]);
        uint32_t a[4] = {(uint32_t)i, list[i], 0, 0};
        call_this(F_ROW, self, 4, a);
        if (rt_unwinding) return;
    }
    uint32_t tv[2] = {0x7a120u, 0};
    call_this(F_TV, self, 2, tv);
    if (rt_unwinding) return;
    if (logging())
        fprintf(stderr, "recomp: pol first: %d/%02d: one table of %d clubs%s\n", (int16_t)rd16(g_mem, self + OBJ_YEAR),
                ((int16_t)rd16(g_mem, self + OBJ_YEAR) + 1) % 100, n, mode == SPLIT ? ", split after 30 rounds" : "");
    rt_ret_stdcall(1, 0);
}

/* The stage slots of this module's form: a new array of 2, slot 0 and slot 1 (the final table that the promotion
 * chain and PolSecond read: the stage, or the object itself before it). */
static void set_slots(uint32_t self, uint32_t stage)
{
    uint32_t slots = game_malloc(8);
    if (!slots) return;
    uint32_t old = rd32(g_mem, self + OBJ_STAGES);
    wr32(g_mem, slots, stage);
    wr32(g_mem, slots + 4, stage ? stage : self);
    wr32(g_mem, self + OBJ_STAGES, slots);
    game_free(old);
}

/* PolFirst::create_second_group_stage 0x7c9900 (thiscall, ret 0). No group B: no stage in one table, one stage to
 * come in the split. */
void o_007c9900(void);
RT_WRAP(007c9900, pol_first_group_b)
{
    uint32_t self = cpu.ecx;
    int mode = mode_of(self);
    if (mode == ORIGINAL || (int16_t)rd16(g_mem, self + OBJ_CLUBS) <= SPLIT_TOP) {
        o_007c9900();
        return;
    }
    set_slots(self, 0);
    if (rt_unwinding) return;
    wr32(g_mem, self + OBJ_STAGE_COUNT, mode == SPLIT ? 1u : 0u);
    wr32(g_mem, self + OBJ_STAGE_NOW, 0xffffffffu);
    rt_ret_stdcall(0, 0);
}

/* ---- the split ---- */

/* One League_Stage of the clubs of list, with the arguments of create_final_stage (0x7c9d41..0x7c9d90 and
 * 0x7c9f00..0x7c9f4e): parent, clubs, list, meetings, record, round table, match days, +0xc2, +0xc3, +0xc4, order
 * +0xc5, places, year, stage 1, 0, 0x14, 1, 0, 3, the matches of a club, then the prize fund and 1 (the 16) or
 * 0 and 2 (the groups). */
static uint32_t make_stage(uint32_t self, int n, uint32_t list, int meetings, uint32_t dates, int days,
                           uint32_t places, uint32_t matches, uint32_t a20, uint32_t a21)
{
    uint32_t sz[1] = {STAGE_SIZE};
    uint32_t obj = call_cdecl(F_NEW, 1, sz);
    if (rt_unwinding || !readable(obj)) return 0;
    uint32_t a[22] = {self, (uint32_t)n, list, (uint32_t)meetings, rd32(g_mem, self + OBJ_RECORD), dates,
                      (uint32_t)days, g_mem[self + 0xc2], g_mem[self + 0xc3], g_mem[self + 0xc4], self + 0xc5, places,
                      (uint32_t)(int16_t)rd16(g_mem, self + OBJ_YEAR), 1, 0, 0x14, 1, 0, 3, matches, a20, a21};
    call_this(F_LEAGUE_STAGE, obj, 22, a);
    return rt_unwinding ? 0 : obj;
}

/* The final stage of the split (advance_stage to stage 0): the top 8 of the table in the championship round, the
 * others in the relegation round, each pair once; the statistics of the 30 rounds with the points halved (rounded
 * up, as 0x7c9df5..0x7c9e18) or kept. */
static void make_split(uint32_t self)
{
    int n = (int16_t)rd16(g_mem, self + OBJ_CLUBS), year = (int16_t)rd16(g_mem, self + OBJ_YEAR);
    uint32_t table = rd32(g_mem, self + OBJ_TABLE), comps = rd32(g_mem, COMPS);
    if (n <= SPLIT_TOP || n > MAX_CLUBS || !readable(table) || !readable(comps)) return;
    uint32_t zero[1] = {0};
    call_this(F_SORT, self, 1, zero);
    uint32_t list = game_malloc(4u * (uint32_t)n);
    if (!list) return;
    uint32_t champions = comps + rd32(g_mem, KEY_CHAMPIONS) * COMP_SIZE;
    uint32_t relegation = comps + rd32(g_mem, KEY_RELEGATION) * COMP_SIZE;
    for (int i = 0; i < n; i++) {
        uint32_t club = rd32(g_mem, table + (uint32_t)i * ROW_SIZE);
        wr32(g_mem, list + 4u * (uint32_t)i, club);
        if (readable(club)) wr32(g_mem, club + CLUB_GROUP, i < SPLIT_TOP ? champions : relegation);
    }
    /* The places of the final table: the object's (0, 0, 2, 2, set in create_table as the original passes them from
     * the stack). The bytes sit on the game stack, as the original's. */
    uint32_t saved_esp = cpu.esp;
    cpu.esp -= 8;
    uint32_t places = cpu.esp;
    g_mem[places] = 0, g_mem[places + 1] = 0, g_mem[places + 2] = g_mem[self + 0xc0], g_mem[places + 3] = g_mem[self + 0xc1];
    uint32_t share[1] = {0x11170u};
    uint32_t prize = call_this(F_PRIZE, self, 1, share);
    uint32_t stage = rt_unwinding ? 0 : make_stage(self, n, list, 2, 0, 0, places, (uint32_t)split_rounds(), prize, 1);
    cpu.esp = saved_esp;
    if (!stage) {
        game_free(list);
        return;
    }
    set_slots(self, stage);
    int half = halved(year);
    uint32_t rows = rd32(g_mem, stage + OBJ_TABLE);
    for (int i = 0; readable(rows) && i < n; i++) {
        uint32_t row = rows + (uint32_t)i * ROW_SIZE;
        uint32_t a[2] = {rd32(g_mem, row), 0};
        uint32_t from = call_this(F_FIND_ROW, self, 2, a);
        if (rt_unwinding) return;
        if (!readable(from)) continue;
        /* The three sections of 0xe bytes (+0x12, +0x20, +0x2e): wins, draws, losses (bytes), goals for and against
         * (words) as 0x7c9e1c..0x7c9e7e, and the points (+7) as 0x7c9df5..0x7c9e18, halved or kept. */
        for (uint32_t s = 0x12; s <= 0x2e; s += 0xe) {
            for (uint32_t b = 0; b < 3; b++) g_mem[row + s + b] = g_mem[from + s + b];
            wr16(g_mem, row + s + 3, rd16(g_mem, from + s + 3));
            wr16(g_mem, row + s + 5, rd16(g_mem, from + s + 5));
            wr16(g_mem, row + s + 7, rd16(g_mem, from + s + 7));
        }
        if (half) {
            int all = ((int16_t)rd16(g_mem, from + 0x19) + 1) / 2, second = ((int16_t)rd16(g_mem, from + 0x27) + 1) / 2;
            wr16(g_mem, row + 0x19, (uint16_t)all);
            wr16(g_mem, row + 0x27, (uint16_t)second);
            wr16(g_mem, row + 0x35, (uint16_t)(all - second));
        }
    }
    wr32(g_mem, stage + OBJ_ORDER_FN, POL_ORDER);
    call_this(F_SORT, stage, 1, zero);
    if (rt_unwinding) return;
    /* The fixtures: a League_Stage of each round with the season's last 7 dates, each pair once; the original
     * deletes its two at once (vtable +0, flag 1, 0x7c9f62..0x7c9f68), and so does this. */
    uint32_t count_at = 0;
    cpu.esp -= 16;
    count_at = cpu.esp;
    wr32(g_mem, count_at, 0), wr32(g_mem, count_at + 4, 0), wr32(g_mem, count_at + 8, 0);
    uint32_t sa[4] = {1, count_at, count_at + 4, count_at + 8};
    uint32_t dates = call_this(F_SCHEDULE, self, 4, sa);
    int days = rt_unwinding ? 0 : (int16_t)rd16(g_mem, count_at);
    cpu.esp = saved_esp;
    if (!readable(dates) || days <= 0) return;
    for (int g = 0; g < 2; g++) {
        int k = g ? n - SPLIT_TOP : SPLIT_TOP;
        uint32_t part = make_stage(self, k, list + (g ? 4u * SPLIT_TOP : 0), 1, dates, days, self + 0xbe,
                                   0xffffffffu, 0, 2);
        if (!part) break;
        uint32_t del[1] = {1};
        call_this(rd32(g_mem, rd32(g_mem, part)), part, 1, del);
        if (rt_unwinding) return;
    }
    game_free(dates);
    game_free(list);
    if (logging())
        fprintf(stderr, "recomp: pol first: %d/%02d: split after 30 rounds, %d and %d clubs, %d dates, points %s\n",
                year, (year + 1) % 100, SPLIT_TOP, n - SPLIT_TOP, days, half ? "halved" : "kept");
}

/* PolFirst::advance_stage 0x7c9ad0 (thiscall, ret 0). In the split +0x30 goes from -1 to 0 and the final stage is
 * made. */
void o_007c9ad0(void);
RT_WRAP(007c9ad0, pol_first_advance)
{
    uint32_t self = cpu.ecx;
    int split = ours(self) && rd32(g_mem, self + OBJ_STAGE_COUNT) == 1;
    o_007c9ad0();
    if (rt_unwinding || !split || (int32_t)rd32(g_mem, self + OBJ_STAGE_NOW) != 0) return;
    make_split(self);
}

/* ---- loads and the status functions ---- */

/* PolFirst::PolFirst 0x7c8a80 (thiscall: year, record; ret 8). After a load League::load 0x687b10 makes a stage
 * array of +0x2c slots (0x688e44..0x688e5b) and loads the stages to +0x30: here the array of 2 again, with the
 * final stage's order (as 0x7c8aee..0x7c8af4 does for slot 1 of the original). */
void o_007c8a80(void);
RT_WRAP(007c8a80, pol_first_ctor)
{
    o_007c8a80();
    uint32_t self = cpu.eax;
    if (rt_unwinding || !ours(self)) return;
    uint32_t old = rd32(g_mem, self + OBJ_STAGES), stage = 0;
    if ((int32_t)rd32(g_mem, self + OBJ_STAGE_COUNT) == 1 && (int32_t)rd32(g_mem, self + OBJ_STAGE_NOW) == 0 &&
        readable(old) && readable(rd32(g_mem, old))) {
        stage = rd32(g_mem, old);
        wr32(g_mem, stage + OBJ_ORDER_FN, POL_ORDER);
    }
    uint32_t keep = cpu.eax;
    set_slots(self, stage);
    if (!rt_unwinding) cpu.eax = keep;
}

/* The status functions with a stage argument: stage -1 of one table is the final table (stage 1). */
static void stage_one(int index)
{
    if (ours(cpu.ecx) && rd32(g_mem, cpu.ecx + OBJ_STAGE_COUNT) == 0 && (rt_arg(index) & 0xff) == 0xff)
        g_mem[cpu.esp + 4 + 4 * (uint32_t)index] = 1;
}

void o_007ca2a0(void);
RT_WRAP(007ca2a0, pol_first_staff_status)     /* (club, status, stage, ...), ret 0x18 */
{
    stage_one(2);
    o_007ca2a0();
}

void o_007ca320(void);
RT_WRAP(007ca320, pol_first_fate_news)        /* (club index, fate, stage, ...), ret 0x28 */
{
    stage_one(2);
    o_007ca320();
}

void o_007ca3f0(void);
RT_WRAP(007ca3f0, pol_first_progress)         /* (club, stage, code 1, code 2, code 3), ret 0x14 */
{
    stage_one(1);
    o_007ca3f0();
}

/* The constructor's predicted positions (qsort count at 0x7c8ce5, loop at 0x7c8d09) and set_predicted_positions
 * 0x7ca1e0 (0x7ca264, 0x7ca288) take 16 slots: in this module's form the clubs of the table (tools/recomp/pol_first.py). */
uint32_t rt_pol_first_slots(uint32_t obj, uint32_t original)
{
    if (!ours(obj)) return original;
    int n = (int16_t)rd16(g_mem, obj + OBJ_CLUBS);
    return n >= MIN_CLUBS && n <= MAX_CLUBS ? (uint32_t)n : original;
}

/* The club count of the Polish First Division at the club moves of a new game in a season (season_clubs.c): the
 * row's count in one table or the split, else -1 (the class's 16). */
int season_pol_first_clubs(int season)
{
    if (!switched_on() || format_of(season) == ORIGINAL) return -1;
    int s = season_rules_last_season(season, "pol_first");
    const SeasonRule *r = s ? season_rules_find(s, "pol_first", 0) : NULL;
    return r && r->clubs >= MIN_CLUBS && r->clubs <= MAX_CLUBS ? r->clubs : -1;
}

