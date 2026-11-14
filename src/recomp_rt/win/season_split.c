/* The split engine (season_split.h; 0.7.2 league sizes, slice 7, D7).
 *
 * How the original splits the Scottish Premier, and what this file takes from it:
 * - ScoPrm::update_for_date 0x7f2d20 (vtable +0x14) checks, before League::update_for_date 0x6847c0, that the
 *   meetings (+0x3c) are 3 (0x7f2d2d) and that every row has played 33 matches (s8 row +0x11, 0x7f2d56), then calls
 *   create_split_stages 0x7f2d90. Here: the wrap of 0x6847c0, for any league with a split row, after N matches, and
 *   after the original's update of the day (see the wrap).
 * - create_split_stages takes the clubs (row +0) of rows 0..5 and 11..6 (0x7f2e36..0x7f2e4e, 0x7f2f04..0x7f2f2d),
 *   gets a date template from create_schedule (vtable +0x3c, 0x7f2e63, 0x7f2f41) and makes a League_Stage of 6
 *   clubs and 1 meeting with it (0x68b100 at 0x7f2ed7, 22 arguments: parent, count, clubs, meetings, record +4,
 *   template, its dates, the bytes +0xc2..+0xc4, &+0xc5, &+0xbe, the edition +0x40, -1, the name code, 0x14, 1, 0,
 *   3, -1, 0, 2). It deletes each stage at once (vtable +0, 0x7f2ef5, 0x7f2fd3) and frees the template: the
 *   fixtures stay with the parent, whose one table takes the results, so the points carry over. Then +0x3c = 4
 *   (0x7f2feb) and the table order +0xa3 = 0x7f3220 (0x7f2ff1). Here: the same stage for each section of the row,
 *   with the section's meetings and the dates after the table's rounds (season_sizes_split_dates: the wrap of the
 *   class's create_schedule plans the whole season, D4 and D5, and gives the rounds after the table's).
 * - The order 0x7f3220 (cdecl, two rows) puts a row whose position after match 33 (the byte at index 0x20 of the row's
 *   position array, row +9) is 6 or more below one whose position is less (0x7f3227..0x7f324a). The sort of the
 *   table (0x6827d0) calls +0xa3 first through its qsort callback 0x683210 (0x682a9a; 0x683248..0x683255) or
 *   0x683410 (0x68289d; 0x683438..0x683454), each twice, with the league in [0xb679fc] (0x68281e), and compares the
 *   points only when it gives 0. The display copy of a league (League::copy_for_display 0x682380) copies the whole
 *   object first (0xec bytes from +0, 0x68239c), so it holds the same vtable, edition, counts and +0xa3. Here: a host function that gives the section of each row by its
 *   position after match N, the position bounds of the row's sizes.
 * - +0xa7 (the matches of each club, the size of each row's position array, init_table_row 0x687570) is the
 *   matches before the split and those of the largest section (ScoPrm: 38 at 0x7f29d5). League::check_complete
 *   0x689ad0 wants +0xa7 of every row (0x689b47..0x689b58); with sections that play different matches
 *   rt_league_played_target wants each row's own.
 * - The save keeps +0x3c, +0xa7, the rows and their position arrays (League::save_to_temp_file 0x6891c0), and the
 *   fixtures. +0xa3 is not saved (ScoPrm's load sets it at 0x7f0bf1..0x7f0c08): season_split_loaded sets it.
 *
 * Known limits (D2, the nearest form). update_positions_and_status 0x684cd0 gives each row its best and worst place
 * from +0xa7 less the matches played (0x684f44, 0x684f75, 0x68501e) and does not know the sections, as for the
 * Scottish split of the original; with sections of different matches (6+4+6) a club of the smaller section counts
 * matches it does not play there. A table match put back past the first section date delays the split, and the
 * section fixtures of a date already past are as in the original's Scottish split.
 *
 * The state of a league is in the save: before the split +0x3c holds the row's meetings, after it the marker
 * (meetings + the most section meetings), with +0xa7 the matches of the season. A league whose object does not hold
 * the engine's count, template (+0xa9) and +0xa7 (a 0.7.1 save before its next season change, D18) plays no
 * split. */
#include "win.h"
#include "../rt_host.h"
#include "season_change.h"
#include "season_league.h"
#include "season_sizes.h"
#include "season_split.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_RECORD 4u              /* object +4: the competition record */
#define COMP_MEETINGS 0x3cu         /* s16 */
#define COMP_CLUBS 0x3eu            /* s16 */
#define COMP_YEAR 0x40u             /* s16: the edition */
#define COMP_ORDER 0xa3u            /* the table order function, 0 for the points (0x683410) */
#define COMP_MATCHES 0xa7u          /* s16: matches per club */
#define COMP_DATES 0xa9u            /* s16: the dates of the template */
#define COMP_TABLE 0xb1u            /* the table: rows of 0x3b bytes */
#define ROW_SIZE 0x3bu
#define ROW_CLUB 0u
#define ROW_POSITIONS 9u            /* the row's position after each match, a byte each (0x7f3227) */
#define ROW_PLAYED 0x11u            /* s8: matches played (0x7f2d56, 0x689b51) */
#define ROW_POINTS 0x19u            /* s16: points of the overall table (comparator 0x683466) */
#define DATE_SIZE 0x41u
#define GAME_DATE 0xae2c90u         /* s16 day of the year, s16 year (testrun.c) */
#define SORTED_LEAGUE 0xb679fcu     /* the league that the table sort sorts (0x68281e) */
#define F_NEW 0x00944cf1u           /* operator new, cdecl (size) (0x7f2e6d) */
#define F_MALLOC 0x00944e46u
#define F_FREE 0x009452cau
#define F_LEAGUE_STAGE 0x0068b100u  /* League_Stage::League_Stage, thiscall, 22 arguments (ret 0x58) */
#define STAGE_SIZE 0xeeu
#define MAX_CLUBS 60

void testrun_save_in(int days);    /* testrun.c */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int logging(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_SIZE_LOG") && *getenv("RECOMP_SIZE_LOG");
    return on;
}

static int grown(const char *file)
{
    for (int k = 0; file && k < season_grown_count; k++)
        if (!strcmp(season_grown[k].file, file)) return 1;
    return 0;
}

static const char *record_name(uint32_t obj)
{
    uint32_t rec = rd32(g_mem, obj + COMP_RECORD);
    return readable(rec) ? win_str(rec + 4) : "?";
}

int season_split_of(uint32_t obj, const char *file, int n, int meetings, SeasonSplit *sp)
{
    memset(sp, 0, sizeof *sp);
    if (!season_sizes_on() || !file || !strcmp(file, "sco_prm") || meetings < 1 || n < 2) return 0;
    int season = season_sizes_row_season(obj, file);
    const SeasonLeagueRule *r = season < 0 ? NULL : season_league_row(file, season);
    if (!r || r->split_after <= 0 || r->split_after != meetings * (n - 1)) return 0;
    int sum = 0, most = 0;
    for (int k = 0; k < SEASON_LEAGUE_PARTS && r->split_size[k] > 0; k++) {
        int size = r->split_size[k], m = r->split_meetings[k];
        if (size < 2 || m < 1 || m > 4) return 0;
        sp->size[k] = size;
        sp->meet[k] = m;
        sp->rounds[k] = m * (size + (size & 1) - 1);
        sp->matches[k] = m * (size - 1);
        if (sp->rounds[k] > sp->rounds_max) sp->rounds_max = sp->rounds[k];
        if (sp->matches[k] > sp->matches_max) sp->matches_max = sp->matches[k];
        if (m > most) most = m;
        sum += size;
        sp->parts = k + 1;
    }
    if (sp->parts < 2 || sum != n) return 0;
    sp->meetings = meetings;
    sp->after = r->split_after;
    sp->marker = meetings + most;
    sp->halved = r->split_points == 1;
    return 1;
}

int season_split_matches(uint32_t obj, const char *file, int n, int meetings)
{
    SeasonSplit sp;
    return season_split_of(obj, file, n, meetings, &sp) ? sp.matches_max : 0;
}

/* The split state of a league object: 0 none (no split row, or an object the engine did not make this edition),
 * 1 before the split, 2 after it. */
static int state(uint32_t obj, SeasonSplit *sp)
{
    if (!readable(obj)) return 0;
    const char *file = season_change_file(obj);
    if (!grown(file)) return 0;
    int season = season_sizes_row_season(obj, file);
    int meetings = season < 0 ? 0 : season_league_meetings(file, season);
    int n = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    if (n < 2 || n > MAX_CLUBS || !season_split_of(obj, file, n, meetings, sp)) return 0;
    if ((int16_t)rd16(g_mem, obj + COMP_DATES) != meetings * (n + (n & 1) - 1) ||
        (int16_t)rd16(g_mem, obj + COMP_MATCHES) != sp->after + sp->matches_max || !readable(rd32(g_mem, obj + COMP_TABLE)))
        return 0;
    int m = (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
    return m == meetings ? 1 : m == sp->marker ? 2 : 0;
}

/* The section of a row by its position after match N, or -1. */
static int section_of(uint32_t row, const SeasonSplit *sp)
{
    uint32_t at = rd32(g_mem, row + ROW_POSITIONS);
    if (!readable(at)) return -1;
    int pos = (int8_t)g_mem[at + (uint32_t)(sp->after - 1)], top = 0;
    for (int k = 0; k < sp->parts; k++) {
        top += sp->size[k];
        if (pos >= 0 && pos < top) return k;
    }
    return -1;
}

/* The table order after the split (cdecl: row a, row b): the section of a less that of b, 0 for one section (then
 * the points, 0x68345b). The league is the one the sort sorts. */
static struct { uint32_t obj; int year; int state; SeasonSplit sp; } cache[8];

static const SeasonSplit *cached(uint32_t obj)
{
    int year = readable(obj) ? (int16_t)rd16(g_mem, obj + COMP_YEAR) : 0, free_at = -1;
    for (int k = 0; k < 8; k++) {
        if (cache[k].obj == obj && cache[k].year == year)
            return cache[k].state == 2 ? &cache[k].sp : NULL;
        if (!cache[k].obj && free_at < 0) free_at = k;
    }
    if (free_at < 0) {
        static int next;
        free_at = next++ % 8;
    }
    cache[free_at].obj = obj;
    cache[free_at].year = year;
    cache[free_at].state = state(obj, &cache[free_at].sp);
    return cache[free_at].state == 2 ? &cache[free_at].sp : NULL;
}

static void forget(uint32_t obj)
{
    for (int k = 0; k < 8; k++)
        if (cache[k].obj == obj) cache[k].obj = 0;
}

static void split_order(void)
{
    uint32_t a = rt_arg(0), b = rt_arg(1);
    const SeasonSplit *sp = cached(rd32(g_mem, SORTED_LEAGUE));
    int r = 0;
    if (sp) {
        int x = section_of(a, sp), y = section_of(b, sp);
        if (x >= 0 && y >= 0) r = x - y;
    }
    rt_ret_cdecl((uint32_t)r);
}

/* The address of split_order, registered at the first split or load after one (0 before: no league holds it). */
static uint32_t order_addr;

static uint32_t order_fn(void)
{
    if (!order_addr) order_addr = rt_host_fn("split_order", split_order);
    return order_addr;
}

uint32_t rt_league_played_target(uint32_t obj, uint32_t original)
{
    /* Only a league after its split holds the order (+0xa3): every other check_complete keeps the original. */
    if (!order_addr || !readable(obj) || rd32(g_mem, obj + COMP_ORDER) != order_addr || !season_sizes_on())
        return original;
    const SeasonSplit *sp = cached(obj);
    if (!sp) return original;
    uint32_t t = rd32(g_mem, obj + COMP_TABLE);
    int n = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    for (int i = 0; i < n; i++) {
        uint32_t row = t + (uint32_t)i * ROW_SIZE;
        int k = section_of(row, sp);
        if (k < 0 || (int8_t)g_mem[row + ROW_PLAYED] < sp->after + sp->matches[k]) return original;
    }
    return 0;   /* every row reached its own section's matches */
}

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t call_this(uint32_t fn, uint32_t self, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
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
    uint32_t a[1] = {p};
    if (p) call_cdecl(F_FREE, 1, a);
}

/* One section: a League_Stage of the clubs with the section's meetings on the first `days` dates of the block,
 * deleted at once as ScoPrm's (its fixtures stay with the parent). 1 when made. */
static int make_section(uint32_t obj, const uint32_t *clubs, int size, int meetings, uint32_t dates, int days,
                        uint32_t code)
{
    uint32_t list = game_malloc(4u * (uint32_t)size), tmpl = list ? game_malloc((uint32_t)days * DATE_SIZE) : 0;
    uint32_t sz[1] = {STAGE_SIZE};
    uint32_t stage = tmpl ? call_cdecl(F_NEW, 1, sz) : 0;
    if (rt_unwinding) return 0;
    int made = 0;
    if (readable(stage)) {
        for (int k = 0; k < size; k++) wr32(g_mem, list + 4u * (uint32_t)k, clubs[k]);
        memcpy(g_mem + tmpl, g_mem + dates, (size_t)days * DATE_SIZE);
        uint32_t a[22] = {obj, (uint32_t)size, list, (uint32_t)meetings, rd32(g_mem, obj + COMP_RECORD), tmpl,
                          (uint32_t)days, g_mem[obj + 0xc2u], g_mem[obj + 0xc3u], g_mem[obj + 0xc4u], obj + 0xc5u,
                          obj + 0xbeu, (uint32_t)(int16_t)rd16(g_mem, obj + COMP_YEAR), 0xffffffffu, code, 0x14, 1, 0,
                          3, 0xffffffffu, 0, 2};
        uint32_t made_at = call_this(F_LEAGUE_STAGE, stage, 22, a);
        if (rt_unwinding) return 0;
        if (readable(made_at)) {
            uint32_t one[1] = {1};
            call_this(rd32(g_mem, rd32(g_mem, made_at)), made_at, 1, one);   /* delete, as 0x7f2eef..0x7f2ef5 */
            if (rt_unwinding) return 0;
            made = 1;
        }
    }
    game_free(tmpl);
    game_free(list);
    return made && !rt_unwinding;
}

static void split(uint32_t obj, const SeasonSplit *sp)
{
    const char *file = season_change_file(obj);
    uint32_t t = rd32(g_mem, obj + COMP_TABLE);
    int n = (int16_t)rd16(g_mem, obj + COMP_CLUBS), year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    /* The clubs by their position after match N; the table's order where the positions are not each place once. */
    uint32_t by_pos[MAX_CLUBS] = {0};
    int ok = 1;
    for (int i = 0; i < n && ok; i++) {
        uint32_t row = t + (uint32_t)i * ROW_SIZE, at = rd32(g_mem, row + ROW_POSITIONS);
        int pos = readable(at) ? (int8_t)g_mem[at + (uint32_t)(sp->after - 1)] : -1;
        if (pos < 0 || pos >= n || by_pos[pos]) ok = 0;
        else by_pos[pos] = rd32(g_mem, row + ROW_CLUB);
    }
    if (!ok && logging())
        for (int i = 0; i < n; i++) {
            uint32_t row = t + (uint32_t)i * ROW_SIZE, at = rd32(g_mem, row + ROW_POSITIONS);
            fprintf(stderr, "recomp: league sizes: %s split %d: row %d played %d positions", file, year, i,
                    (int8_t)g_mem[row + ROW_PLAYED]);
            for (int k = sp->after - 3; readable(at) && k <= sp->after + 1; k++)
                fprintf(stderr, " %d:%d", k, (int8_t)g_mem[at + (uint32_t)k]);
            fprintf(stderr, "\n");
        }
    int days = 0;
    uint32_t code = 0, dates = 0;
    if (ok) {
        dates = season_sizes_split_dates(obj, sp->meetings, &days, &code);
        if (rt_unwinding) return;
    }
    if (!ok || !dates || days < sp->rounds_max) {
        /* No sections: the table ends after N matches, as one table (the season still completes). The order and
         * the targets of the sections read the positions after match N, so without them there is no split. */
        game_free(dates);
        wr16(g_mem, obj + COMP_MATCHES, (uint16_t)sp->after);
        forget(obj);
        fprintf(stderr, "recomp: league sizes: %s split %d: %s; one table of %d matches (%.50s)\n", file, year,
                ok ? "no dates for the sections" : "the positions after the split match are not each place once",
                sp->after, record_name(obj));
        return;
    }
    int first = 0;
    for (int k = 0; k < sp->parts; k++) {
        if (!make_section(obj, by_pos + first, sp->size[k], sp->meet[k], dates, sp->rounds[k], code)) {
            if (rt_unwinding) return;
            fprintf(stderr, "recomp: league sizes: %s split %d: section %d not made (%.50s)\n", file, year, k + 1,
                    record_name(obj));
        }
        first += sp->size[k];
    }
    if (sp->halved)
        for (int i = 0; i < n; i++) {
            uint32_t row = t + (uint32_t)i * ROW_SIZE;
            int p = (int16_t)rd16(g_mem, row + ROW_POINTS);
            wr16(g_mem, row + ROW_POINTS, (uint16_t)(p >= 0 ? (p + 1) / 2 : p / 2));   /* halved, rounded up */
        }
    wr16(g_mem, obj + COMP_MEETINGS, (uint16_t)sp->marker);
    wr32(g_mem, obj + COMP_ORDER, order_fn());
    forget(obj);
    if (logging()) {
        fprintf(stderr, "recomp: league sizes: %s split %d: after %d, sections", file, year, sp->after);
        for (int k = 0; k < sp->parts; k++)
            fprintf(stderr, "%s%d", k ? "+" : " ", sp->size[k]);
        fprintf(stderr, ", meetings");
        for (int k = 0; k < sp->parts; k++)
            fprintf(stderr, "%s%d", k ? "+" : " ", sp->meet[k]);
        fprintf(stderr, ", rounds");
        for (int k = 0; k < sp->parts; k++)
            fprintf(stderr, "%s%d", k ? "+" : " ", sp->rounds[k]);
        fprintf(stderr, ", %d dates, points %s (%.50s)\n", days, sp->halved ? "halved" : "kept", record_name(obj));
    }
    /* Test runs (tests/seasons/league_size_run.py --save-in-split): RECOMP_SAVE_AFTER_SPLIT=k saves at the first
     * human turn on or after the k-th section date (testrun.c counts a day as year x 366 + day of the year). */
    const char *v = getenv("RECOMP_SAVE_AFTER_SPLIT");
    static int armed;
    if (v && *v && !armed) {
        int k = atoi(v);
        k = k < 1 ? 1 : k > sp->rounds_max ? sp->rounds_max : k;
        uint32_t rec = dates + (uint32_t)(k - 1) * DATE_SIZE;
        int target = (year + (int16_t)rd16(g_mem, rec + 2)) * 366 + (int16_t)rd16(g_mem, rec);
        int today = (int16_t)rd16(g_mem, GAME_DATE + 2) * 366 + (int16_t)rd16(g_mem, GAME_DATE);
        armed = 1;
        testrun_save_in(target > today ? target - today : 0);
        fprintf(stderr, "recomp: league sizes: %s split %d: a test save after section date %d (%d days)\n", file, year,
                k, target - today);
    }
    game_free(dates);
}

void season_split_update(uint32_t obj)
{
    if (!season_sizes_on()) return;
    SeasonSplit sp;
    if (state(obj, &sp) != 1) return;
    uint32_t t = rd32(g_mem, obj + COMP_TABLE);
    int n = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    for (int i = 0; i < n; i++)
        if ((int8_t)g_mem[t + (uint32_t)i * ROW_SIZE + ROW_PLAYED] < sp.after) return;
    split(obj, &sp);
}

void season_split_loaded(uint32_t obj)
{
    if (!season_sizes_on()) return;
    SeasonSplit sp;
    forget(obj);
    if (state(obj, &sp) != 2) return;
    wr32(g_mem, obj + COMP_ORDER, order_fn());
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s split %d: loaded after the split (%.50s)\n", season_change_file(obj),
                (int16_t)rd16(g_mem, obj + COMP_YEAR), record_name(obj));
}

int season_split_meetings(uint32_t obj)
{
    int m = readable(obj) ? (int16_t)rd16(g_mem, obj + COMP_MEETINGS) : 0;
    if (!season_sizes_on()) return m;
    SeasonSplit sp;
    return state(obj, &sp) == 2 ? sp.meetings : m;
}

void season_split_new_edition(uint32_t obj)
{
    forget(obj);
    if (order_addr && readable(obj) && rd32(g_mem, obj + COMP_ORDER) == order_addr) wr32(g_mem, obj + COMP_ORDER, 0);
}

/* League::update_for_date 0x6847c0 (thiscall: 2 arguments, ret 8): the split after the day's update. ScoPrm's
 * 0x7f2d20 splits before it, but the sort of that update (0x6827d0, from update_positions_and_status 0x684cd0) is what
 * writes each row's position after its last match (0x682ad1..0x682b1d: the row of place i gets i at index played - 1).
 * Before it, index N - 1 holds no places yet (a test log: 0 for every row of the Welsh Premier 2012/13). After it the
 * table is sorted and index N - 1 holds each row's place. */
void o_006847c0(void);
RT_WRAP(006847c0, league_update_for_date)
{
    uint32_t self = cpu.ecx;
    o_006847c0();
    if (rt_unwinding) return;
    Cpu saved = cpu;
    season_split_update(self);
    if (!rt_unwinding) cpu = saved;
}
