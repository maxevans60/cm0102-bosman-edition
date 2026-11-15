/* League sizes, promotion and relegation in existing divisions (season_sizes.h). */
#include "win.h"
#include "../rt_host.h"
#include "season.h"
#include "season_change.h"
#include "season_names.h"
#include "season_rules.h"
#include "season_sizes.h"
#include "season_playoffs.h"
#include "season_divisions.h"
#include "season_divisions_ita.h"
#include "season_league.h"
#include "season_template.h"
#include "season_cup_sizes_west.h"
#include "season_split.h"
#include "season_size_step.h"
#include "season_clubs.h"
#include "season_second_teams.h"
#include "season_fin_second.h"
#include "season_bel_second.h"
#include "season_esp_second.h"
#include "season_sizes_arg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUBS 0xae23b4u             /* the club table (0x245 bytes a club) */
#define CLUB_COUNT 0xae235cu
#define CLUB_SIZE 0x245u
#define CLUB_DIVISION 0x57u         /* club_comp pointer */
#define COMP_RECORD 4u              /* object +4: the competition record */
#define COMP_YEAR 0x40u             /* s16: the edition (start_new_season adds 1, 0x574c10) */
#define COMP_CLUBS 0x3eu            /* s16: the club count of a league (League::check_complete 0x689b3d) */
#define COMP_MEETINGS 0x3cu         /* s16: meetings of each pair */
#define COMP_MATCHES 0xa7u          /* s16: matches per club, -1 until check_complete 0x689ad0 sets it */
#define COMP_TABLE 0xb1u            /* the table: 0x3b bytes a row, the club at +0, the status at +0x37 */
#define ROW_SIZE 0x3bu
#define ROW_STATUS 0x37u
#define UP 0xbeu                    /* s8: promotion places (League::has_automatic_promotion 0x684790) */
#define UP_PLAYOFF 0xbfu            /* s8: promotion play-off places (inference, rules report doc) */
#define DOWN_PLAYOFF 0xc0u          /* s8: relegation play-off places (inference) */
#define DOWN 0xc1u                  /* s8: relegation places (League::has_relegation 0x6847b0) */
#define COMP_STAGES 0xcu            /* the stage array (League load 0x688e3a allocates +0x2c * 4 bytes) */
#define COMP_STAGE_COUNT 0x2cu      /* s32: stages of an edition, set by the constructor */
#define COMP_STAGE_NOW 0x30u        /* s32: the stage now, -1 before the first */
#define ABOVE 0x1cu                 /* s32: record id of the division above, -1 for none */
#define BELOW 0x20u                 /* s32: record id of the division below */
#define COMP_OBJECTS 0xadadfcu      /* g_comp_object_table: the object of each competition record */
#define COMP_SLOTS 0xadadf8u        /* its slot count, one per club competition record (0x8386f0) */
#define STATUS_DOWN 3               /* league_promote_and_relegate 0x689c80: moves down a row of 3 (0x689d26) */
#define CLUB_NAME 4u                /* long name, 52 bytes */
#define CLUB_NATION 0x53u           /* nation pointer */
#define CLUB_LAST_DIVISION 0x5bu    /* club_comp pointer: the division of the last season (0x689d99) */
#define CLUB_REPUTATION 0x80u       /* s16 */
#define NATION_COUNT 0xae2350u      /* the last 2 x nations rows of the club table are national teams (0x53b900) */
#define REC_NATION 0x5du            /* club_comp record: nation pointer */
#define ROW_POS 0x39u               /* s8: the place in the table, from 1 (0x685225) */
#define F_MOVE_CLUB 0x006830b0u     /* League::move_club_to_division (club, record, vacancy), ret 0xc */
#define F_RELEGATION_CLAUSES 0x004cd030u    /* CONTRACT_MANAGER::trigger_relegation_clauses, thiscall (club, old) */
#define CONTRACTS 0xae1050u         /* the contract manager (0x689da0) */
#define F_IS_HUMAN 0x007875e0u      /* thiscall on [HUMANS] (club): 1 for a club with a human manager (0x6426ec) */
#define HUMANS 0xb7431cu
#define DATE_SIZE 0x41u             /* a date of the template: +0 s16 day of the year, +2 s16 year (0x68a1a5..0x68a1b7) */
#define F_MALLOC 0x00944e46u        /* cdecl (size) */
#define F_FREE 0x009452cau          /* cdecl (block) */
#define MIN_CLUBS 2
#define MAX_CLUBS 60

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* A season game with the sizes on. Test switch RECOMP_LEAGUE_SIZES=off: the original's counts and places (the
 * club moves of slice 15 still run). */
static int sizes_on(void)
{
    static int off = -1;
    if (off < 0) off = getenv("RECOMP_LEAGUE_SIZES") && !strcmp(getenv("RECOMP_LEAGUE_SIZES"), "off");
    return !off && season_game_changes();
}

int season_sizes_on(void) { return sizes_on(); }

static int logging(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_SIZE_LOG") && *getenv("RECOMP_SIZE_LOG");
    return on;
}

/* The index of a file in season_size_files, or -1. */
static int size_index(const char *file)
{
    for (int i = 0; file && i < season_size_file_count; i++)
        if (!strcmp(season_size_files[i], file)) return i;
    return -1;
}

static int size_file(const char *file) { return size_index(file) >= 0; }

/* The largest count of a file's class, 0 when the class counts the clubs of its data (or is not here). */
static int cap_of(const char *file)
{
    int i = size_index(file);
    return i >= 0 ? season_size_caps[i] : 0;
}

/* The smallest count of a file's class (0: any count from MIN_CLUBS). League One plays the count of its row in every
 * season (slice 8): the floor of 23 in a 2019 start (Q282) went with the rows of the check, which give 24 (Bury was
 * expelled after the season began, Q278). */
static int floor_of(const char *file)
{
    int i = size_index(file);
    return i < 0 ? 0 : season_size_floors[i];
}

/* The smallest count of a file's exchange: its floor, or the count of FIXED_EXCHANGE (league_sizes.py) whose places
 * and exchange stay the class's until the league's own slice (0.7.2 slice 4: the floors of the cups of Britain and
 * Ireland went, their exchanges stay). sizes_rules and the wrap of 0x689c80 read it, the club moves do not. */
static int exchange_floor(const char *file)
{
    int i = size_index(file), low = floor_of(file);
    return low > 0 || i < 0 ? low : season_size_fixed[i];
}

/* A count within the class's floor and cap. */
static int within(const char *file, int n)
{
    int cap = cap_of(file), low = floor_of(file);
    if (cap > 0 && n > cap) n = cap;
    if (low > 0 && n < low) n = low;
    return n;
}

/* The row of a season, or the last row before it (Q248: after the last row the rules stay, with no end year:
 * the after-table rule). */
static const SeasonRule *row_of_season(const char *file, int season)
{
    int s = season_rules_last_season(season, file);
    return s ? season_rules_find(s, file, 0) : NULL;
}

/* 1 when the real season of a row begins with the year ("2011", "2010/11", "2011-12 (first stage ...)"). */
static int real_starts(const SeasonRule *r, int year)
{
    char y[8];
    snprintf(y, sizeof y, "%d", year);
    return r && r->real_season && !strncmp(r->real_season, y, 4) &&
           (r->real_season[4] < '0' || r->real_season[4] > '9');
}

/* The season of the row of the edition of a year (season_sizes.h): row Y when its real season begins with Y
 * ("2008/09", "2026/27"), else row Y - 1 when its real season begins with Y, else Y. A calendar-year league holds
 * the real season Y + 1 in row Y ("Calendar-year league: real season 2009" in row 2008 of nor_prm), so its edition
 * Y takes row Y - 1: a 2008 start plays the Tippeligaen 2008 with the 14 clubs of row 2007 and the 2009 edition
 * with the 16 of row 2008. Row Y comes first, so a league that changes from calendar years to two-year seasons
 * (jap_j1 row 2025 "2026", row 2026 "2026/27") plays its two-year season in the year of the change. */
int season_sizes_edition(const char *file, int year)
{
    if (real_starts(season_rules_find(year, file, 0), year)) return year;
    if (real_starts(season_rules_find(year - 1, file, 0), year)) return year - 1;
    return year;
}

static const SeasonRule *row_of_edition(const char *file, int year)
{
    return row_of_season(file, season_sizes_edition(file, year));
}

/* The row of the rules of a league object now (season_sizes.h). */
static const SeasonRule *row_now(uint32_t obj, const char *file)
{
    if (!season_changing_rules() || season_change_is_creating()) return row_of_edition(file, season_start_year());
    return row_of_edition(file, (int16_t)rd16(g_mem, obj + COMP_YEAR));
}

int season_sizes_row_season(uint32_t obj, const char *file)
{
    const SeasonRule *r = row_now(obj, file);
    return r ? r->season : -1;
}

static const SeasonGrown *grown_of(const char *file);

int season_sizes_clubs(const char *file, int season)
{
    if (!sizes_on() || !size_file(file)) return -1;
    const SeasonRule *r = row_of_edition(file, season);
    if (!r || r->clubs < MIN_CLUBS || r->clubs > MAX_CLUBS || cap_of(file) <= 0) return -1;
    return within(file, r->clubs);
}

/* The count of a season for a grown class that counts the clubs of its data (league_sizes.py DATA_COUNT, held 0:
 * IreFirst::create_table 0x635910 counts division [0x9cf810] and allocates count x 0x3b): its row's count, so that
 * the club moves of a new game give the division that many clubs. -1 for any other class or with the sizes off. */
int season_sizes_data_clubs(const char *file, int season)
{
    const SeasonGrown *g = grown_of(file);
    if (!sizes_on() || !g || g->held) return -1;
    const SeasonRule *r = row_of_edition(file, season);
    if (!r || r->clubs < MIN_CLUBS || r->clubs > g->cap) return -1;
    return r->clubs;
}

/* The clubs whose division is the record of the object. */
static int clubs_in_division(uint32_t obj)
{
    uint32_t rec = rd32(g_mem, obj + COMP_RECORD), table = rd32(g_mem, CLUBS);
    int count = (int)rd32(g_mem, CLUB_COUNT), n = 0;
    if (!readable(rec) || !readable(table)) return -1;
    for (int i = 0; i < count; i++)
        n += rd32(g_mem, table + (uint32_t)i * CLUB_SIZE + CLUB_DIVISION) == rec;
    return n;
}

static const char *record_name(uint32_t obj)
{
    uint32_t rec = rd32(g_mem, obj + COMP_RECORD);
    return readable(rec) ? win_str(rec + 4) : "?";
}

/* The size step (slice 3): runs the moves noted at the promotion chain before the first new table (below). */
static void size_step_run(uint32_t trigger);
static void step_note_pair(uint32_t upper, uint32_t lower);
static void step_note_outside(uint32_t upper, uint32_t lower);
static int row_reachable(uint32_t obj, const char *file);

/* The club count of a league object in a season game: the clubs of its division, up to the class's cap; 0 when the
 * original's count stays. */
static int league_count(uint32_t obj, const char *file, int *have_out, int *cap_out)
{
    int have = clubs_in_division(obj);
    if (have < MIN_CLUBS || have > MAX_CLUBS) return 0;
    int cap = season_ita_cap(obj, cap_of(file));   /* slice 20b: Serie C */
    if (have_out) *have_out = have;
    if (cap_out) *cap_out = cap;
    return cap > 0 && have > cap ? cap : have;
}

/* A grown class (league_sizes.py GROWN, season_size_wraps.c): its create_table allocates the rows of its largest
 * original count (SwePrm: push 0x33a at 0x89043b, 14 rows) and its date template has the rounds of that count. */
static const SeasonGrown *grown_of(const char *file)
{
    for (int k = 0; file && k < season_grown_count; k++)
        if (!strcmp(season_grown[k].file, file)) return &season_grown[k];
    return NULL;
}

/* The objects whose template had no room for their count this edition (D4): they keep the class's count. */
#define MAX_KEPT 16
static struct { uint32_t obj; int year; } kept[MAX_KEPT];

static int kept_at(uint32_t obj)
{
    int year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    for (int k = 0; k < MAX_KEPT; k++)
        if (kept[k].obj == obj) return kept[k].year == year;
    return 0;
}

static void keep(uint32_t obj, int on)
{
    int free_at = -1;
    for (int k = 0; k < MAX_KEPT; k++) {
        if (kept[k].obj == obj) { kept[k].obj = 0; if (free_at < 0) free_at = k; }
        else if (!kept[k].obj && free_at < 0) free_at = k;
    }
    if (on && free_at >= 0) {
        kept[free_at].obj = obj;
        kept[free_at].year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    }
}

/* The count of a grown league this edition: the clubs of its division up to its cap, or 0 for the class's count. */
static int grown_count(uint32_t obj, const char *file)
{
    return grown_of(file) && !kept_at(obj) ? league_count(obj, file, NULL, NULL) : 0;
}

int season_sizes_planned(uint32_t obj)
{
    return sizes_on() && readable(obj) && grown_of(season_change_file(obj)) && !kept_at(obj);
}

uint32_t rt_league_table_bytes(uint32_t obj, uint32_t original)
{
    if (!sizes_on() || !readable(obj)) return original;
    size_step_run(obj);
    const char *file = season_change_file(obj);
    int n = grown_count(obj, file);
    if (n * (int)ROW_SIZE <= (int)original) return original;
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s table of %d rows (%.50s %d, the class makes %u)\n", file, n,
                record_name(obj), (int16_t)rd16(g_mem, obj + COMP_YEAR), original / ROW_SIZE);
    return (uint32_t)n * ROW_SIZE;
}

uint32_t rt_league_clubs(uint32_t obj, uint32_t original)
{
    if (!sizes_on() || !readable(obj)) return original;
    size_step_run(obj);
    const char *file = season_change_file(obj);
    int have = 0, cap = 0, n = league_count(obj, file, &have, &cap);
    if (!n) return original;
    /* A grown league whose template had no room keeps the class's count, or fewer: create_table fills only the
     * clubs of the division and fails with more rows than clubs (NorPrm 0x7926a0..0x7926ce, box at 0x792845). */
    if (grown_of(file) && kept_at(obj) && (uint32_t)n > original) return original;
    const SeasonRule *r = row_now(obj, file);
    int want = r && r->clubs >= MIN_CLUBS && r->clubs <= MAX_CLUBS ? r->clubs : (int)original;
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s (%.50s) %d: %d clubs in the division, %d in the table%s, class %u "
                        "(holds %d)\n", file, record_name(obj), (int16_t)rd16(g_mem, obj + COMP_YEAR), have, want,
                r ? "" : " (no row)", original, cap);
    /* Matches per club go back to -1 only for a new count, so check_complete counts them again. A class that sets
     * its own value keeps it: ScoPrm::set_up_rules writes 38 (33 and the split) at 0x7f29d5 before create_table
     * runs (0x7f2b68, then 0x7f2ba6), and with -1 check_complete would count (12 - 1) x 3 = 33. */
    /* A league that goes back to the class count after a season at another count also counts again, when its value
     * is the one check_complete made for the old count ((old - 1) x meetings +0x3c): else it kept the old season's
     * matches and was never complete (C2 of 20 back to 18 in 2012/13, slice 20b). ScoPrm's 38 is not that value.
     * In a season start the old count is gone: start_new_season frees the table first (League::free_table 0x687970,
     * for example at 0x64b830), which sets +0x3e to 0 (0x687a0c) before create_table reaches this store. So the old
     * count is the one that the value itself gives: a value of (k - 1) x meetings for another count k is the last
     * season's. The Serie C groups of 20 back to 18 in 2015/16 kept 38, check_complete never found them complete,
     * and Italy never started 2016/17 (O278). */
    int old = (int16_t)rd16(g_mem, obj + 0x3e), m = (int16_t)rd16(g_mem, obj + COMP_MATCHES);
    int meet = (int)g_mem[obj + 0x3c];
    if (old == 0 && meet > 0 && m > 0 && m % meet == 0) old = m / meet + 1;
    if ((uint32_t)n != original || (old != n && old >= MIN_CLUBS && m == (old - 1) * meet))
        wr16(g_mem, obj + COMP_MATCHES, 0xffffu);
    /* A grown class: the matches of the count and the row's meetings (slice 2), as check_complete counts them
     * ((+0x3e - 1) x +0x3c, 0x689b27..0x689b36; an odd count plays the same, with a rest round). */
    if (grown_of(file) && grown_of(file)->mode == SEASON_TEMPLATE_HALVES)
        wr16(g_mem, obj + COMP_MATCHES, (uint16_t)season_arg_matches(obj, n));   /* slice 15: zones, derby round */
    else if (grown_of(file) && meet > 0)
        wr16(g_mem, obj + COMP_MATCHES, (uint16_t)((n - 1) * meet +
                                                   (kept_at(obj) ? 0 : season_split_matches(obj, file, n, meet))));
    return (uint32_t)n;
}

uint32_t rt_league_dates(uint32_t obj, uint32_t original)
{
    if (!sizes_on() || !readable(obj)) return original;
    const char *file = season_change_file(obj);
    int have = clubs_in_division(obj), cap = cap_of(file);
    if (have < MIN_CLUBS || cap <= 0) return original;
    int n = have > cap ? cap : have, even = n + (n & 1), meetings = (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
    if (meetings < 1 || meetings > 4) meetings = 2;
    int rounds = meetings * (even - 1);
    if (rounds <= 0 || rounds >= (int)original) return original;
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s (%.50s): %d dates for %d clubs\n", file, record_name(obj), rounds, n);
    return (uint32_t)rounds;
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
    uint32_t a[1] = {p};
    if (p) call_cdecl(F_FREE, 1, a);
}

/* The meetings of a grown league in the row of its season (season_league_meetings, slice 1), written to +0x3c;
 * 0 leaves the class's meetings (set_up_rules: NorPrm 2 at 0x792713, PorSecond 0x7d16b1, SwePrm 0x890563). */
static int grown_meetings(uint32_t obj, const char *file)
{
    const SeasonRule *r = row_now(obj, file);
    int m = r ? season_league_meetings(file, r->season) : 0;
    if (m >= 1 && m <= 4 && m != (int16_t)rd16(g_mem, obj + COMP_MEETINGS)) {
        if (logging())
            fprintf(stderr, "recomp: league sizes: %s meetings %d (%.50s %d, the class sets %d)\n", file, m,
                    record_name(obj), (int16_t)rd16(g_mem, obj + COMP_YEAR), (int16_t)rd16(g_mem, obj + COMP_MEETINGS));
        wr16(g_mem, obj + COMP_MEETINGS, (uint16_t)m);
    }
    m = (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
    return m >= 1 && m <= 4 ? m : 2;
}

static const char *const MODE_NAMES[] = {"shuffled", "sequential", "mirrored", "halves"};

/* The dates of a template as YYYY-MM-DD, a new date with "+", for the test log (tests/seasons/league_size_run.py). */
static void log_days(int base, const int *days, const int *added, int n)
{
    static const int cum[2][12] = {{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
                                   {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}};
    for (int k = 0; k < n; k++) {
        int yoff, doy, mon = 11;
        season_template_split(base, days[k], &yoff, &doy);
        int leap = season_template_year_days(base + yoff) == 366;
        while (mon > 0 && cum[leap][mon] > doy) mon--;
        fprintf(stderr, " %04d-%02d-%02d%s", base + yoff, mon + 1, doy - cum[leap][mon] + 1, added && added[k] ? "+" : "");
    }
}

/* The time byte (+4) of a new midweek date whose record copies a date of another weekday: 2, as the original's
 * Wednesdays (NorPrm 0x79190b: 8 April, weekday 2, time 2). */
#define TIME_MIDWEEK 2

/* The split dates of season_sizes_split_dates: while it calls create_schedule, the wrap gives the dates after the
 * split in place of the template, for the row's meetings of the edition (0: not called). */
static int probe_meetings;
static int probe_made;      /* 1 when the probe's wrap gave the dates after the table's rounds */

/* The template builder of the Argentine Primera (season_sizes_arg.c): the plan of season_template_plan_from, built as
 * season_sizes_schedule builds a grown class's template. */
uint32_t season_sizes_build_template(uint32_t self, const char *file, uint32_t old, int have, int rounds, int mode)
{
    int base = (int16_t)rd16(g_mem, self + COMP_YEAR), n = league_count(self, file, NULL, NULL);
    if (have < 1 || have > SEASON_TEMPLATE_MAX || rounds < 1 || rounds > SEASON_TEMPLATE_MAX || !readable(old) ||
        !readable(old + (uint32_t)have * DATE_SIZE))
        return 0;
    int days[SEASON_TEMPLATE_MAX];
    for (int i = 0; i < have; i++) {
        uint32_t r = old + (uint32_t)i * DATE_SIZE;
        days[i] = season_template_day(base, (int16_t)rd16(g_mem, r + 2), (int16_t)rd16(g_mem, r));
    }
    if (logging()) {
        fprintf(stderr, "recomp: league sizes: %s template %d: class %d", file, base, have);
        log_days(base, days, NULL, have);
        fprintf(stderr, "\n");
    }
    SeasonTemplateDate plan[SEASON_TEMPLATE_MAX];
    char why[160];
    if (season_template_plan_from(base, days, have, rounds, mode, season_sizes_bra_earlier(file), plan, why,
                                  sizeof why) != rounds) {
        fprintf(stderr, "recomp: league sizes: %s dates: no room for %d rounds of %d clubs (%.50s %d): %s; the class's "
                        "%d dates and count stay\n", file, rounds, n, record_name(self), base, why, have);
        return 0;
    }
    uint32_t tmpl = game_malloc((uint32_t)rounds * DATE_SIZE);
    if (!tmpl) return 0;
    int new_days[SEASON_TEMPLATE_MAX], added[SEASON_TEMPLATE_MAX];
    for (int k = 0; k < rounds; k++) {
        uint32_t to = tmpl + (uint32_t)k * DATE_SIZE, from = old + (uint32_t)plan[k].from * DATE_SIZE;
        memcpy(g_mem + to, g_mem + from, DATE_SIZE);
        int yoff, doy;
        season_template_split(base, plan[k].day, &yoff, &doy);
        wr16(g_mem, to, (uint16_t)doy);
        wr16(g_mem, to + 2, (uint16_t)yoff);
        if (plan[k].added &&
            season_template_weekday(base, plan[k].day) != season_template_weekday(base, days[plan[k].from]))
        {
            g_mem[to + 4] = TIME_MIDWEEK;
            /* Slot 0 as the original's midweek rounds: no weekday, kind or option (NorPrm 0x79191d writes
             * (round, 0, -1, -1, -1, 0) through 0x68a1c0, which stores +5 the value, +9 the weekday, +0xa the kind,
             * +0xb the option at 0x68a3b9..0x68a3c9). A Sunday record holds weekday 6 there (PorSecond 0x7d0d21). */
            wr32(g_mem, to + 5, 0);
            g_mem[to + 9] = g_mem[to + 0xa] = g_mem[to + 0xb] = 0xff;
        }
        new_days[k] = plan[k].day;
        added[k] = plan[k].added;
    }
    if (logging()) {
        fprintf(stderr, "recomp: league sizes: %s dates: %d dates for %d clubs (%.50s %d, the class makes %d)\n", file,
                rounds, n, record_name(self), base, have);
        fprintf(stderr, "recomp: league sizes: %s template %d: new %d", file, base, rounds);
        log_days(base, new_days, added, rounds);
        fprintf(stderr, "\n");
    }
    return tmpl;
}

void season_sizes_schedule(uint32_t self, int stage, uint32_t count_at)
{
    if (!sizes_on() || !readable(self)) return;
    /* A class of halves makes the dates of each half itself (the Argentine Primera, slice 15). */
    const SeasonGrown *gh = grown_of(season_change_file(self));
    if (gh && gh->mode == SEASON_TEMPLATE_HALVES) {
        if (stage == -1) size_step_run(self);
        if (!rt_unwinding) season_arg_schedule(self, stage, count_at);
        return;
    }
    if (stage != -1) return;
    size_step_run(self);
    const char *file = season_change_file(self);
    const SeasonGrown *g = grown_of(file);
    if (!g) return;
    int probe = probe_meetings;
    if (!probe) {
        keep(self, 0);
        season_split_new_edition(self);
    }
    int class_meetings = (int16_t)rd16(g_mem, self + COMP_MEETINGS);
    int meetings = probe ? probe : grown_meetings(self, file);
    uint32_t old = cpu.eax;
    int have = readable(count_at) ? (int16_t)rd16(g_mem, count_at) : 0;
    int n = league_count(self, file, NULL, NULL);
    if (n < MIN_CLUBS || !readable(old) || have < 1 || have > SEASON_TEMPLATE_MAX ||
        !readable(old + (uint32_t)have * DATE_SIZE))
        return;
    /* A class that sets its meetings by its count in create_table (IreFirst, slice 13): a row with no meetings value
     * takes the class's meetings for this count, not the value set_up_rules wrote for another count. */
    if (!probe) {
        const SeasonRule *r = row_now(self, file);
        int row_m = r ? season_league_meetings(file, r->season) : 0;
        int m = season_sizes_irl_class_meetings(file, n, row_m >= 1 && row_m <= 4 ? row_m : 0);
        if (m && m != meetings) {
            meetings = m;
            wr16(g_mem, self + COMP_MEETINGS, (uint16_t)m);
        }
    }
    int rounds = meetings * (n + (n & 1) - 1), base = (int16_t)rd16(g_mem, self + COMP_YEAR);
    /* A split (slice 7, D7): the template plans the rounds before the split and the most rounds of a section after
     * it. The table plays the first; the sections take the rest when the split comes (season_split.c), through
     * this wrap again (probe). */
    SeasonSplit sp;
    int split = season_split_of(self, file, n, meetings, &sp), total = rounds + (split ? sp.rounds_max : 0);
    if (probe && !split) return;
    /* The mode of the row against the object's flags (+0xd9, league_next_schedule_round 0x68a030: 0x80 mirrored at
     * 0x68a090, 0x40 sequential). */
    int flags = rd16(g_mem, self + 0xd9u);
    int mode = flags & 0x80 ? SEASON_TEMPLATE_MIRRORED : flags & 0x40 ? SEASON_TEMPLATE_SEQUENTIAL : g->mode;
    if (mode != g->mode && !probe)
        fprintf(stderr, "recomp: league sizes: %s template mode %s in league_sizes.py, the object's flags 0x%x\n", file,
                MODE_NAMES[g->mode], flags);
    int days[SEASON_TEMPLATE_MAX];
    for (int i = 0; i < have; i++) {
        uint32_t r = old + (uint32_t)i * DATE_SIZE;
        days[i] = season_template_day(base, (int16_t)rd16(g_mem, r + 2), (int16_t)rd16(g_mem, r));
    }
    if (logging() && !probe) {
        fprintf(stderr, "recomp: league sizes: %s template %d: class %d", file, base, have);
        log_days(base, days, NULL, have);
        fprintf(stderr, "\n");
    }
    /* The matches of the count (rt_league_clubs writes the same when create_table runs after this), and those of
     * the largest section of a split. */
    if (!probe) wr16(g_mem, self + COMP_MATCHES, (uint16_t)((n - 1) * meetings + (split ? sp.matches_max : 0)));
    if (total == have && !split) return;
    /* A class date on the day of an earlier one (NirPrm 2010/11: 15 January 2011 twice, a date that snaps to its
     * Saturday) is left out of the plan, which never puts two rounds on one day (season_template.c). Not for a
     * mirrored class: its halves are the halves of the class's dates. */
    int uniq[SEASON_TEMPLATE_MAX], from_of[SEASON_TEMPLATE_MAX], count_u = 0;
    for (int i = 0; i < have; i++) {
        int seen = 0;
        for (int j = 0; j < count_u && mode != SEASON_TEMPLATE_MIRRORED; j++) seen |= uniq[j] == days[i];
        if (seen) continue;
        from_of[count_u] = i;
        uniq[count_u++] = days[i];
    }
    if (count_u < have && logging() && !probe)
        fprintf(stderr, "recomp: league sizes: %s template %d: %d class dates on the day of another left out\n", file,
                base, have - count_u);
    SeasonTemplateDate plan[SEASON_TEMPLATE_MAX];
    char why[160];
    if (total > SEASON_TEMPLATE_MAX || mode == SEASON_TEMPLATE_MIRRORED && split ||
        season_template_plan_from(base, uniq, count_u, total, mode, season_sizes_bra_earlier(file), plan, why,
                                  sizeof why) != total) {
        if (total > SEASON_TEMPLATE_MAX || mode == SEASON_TEMPLATE_MIRRORED && split)
            snprintf(why, sizeof why, "%s", total > SEASON_TEMPLATE_MAX ? "too many rounds" : "a split of a mirrored class");
        if (probe) {
            fprintf(stderr, "recomp: league sizes: %s split: no dates for the sections (%.50s %d): %s\n", file,
                    record_name(self), base, why);
            return;
        }
        keep(self, 1);
        wr16(g_mem, self + COMP_MEETINGS, (uint16_t)class_meetings);   /* the class's template: its meetings */
        wr16(g_mem, self + COMP_MATCHES, 0xffffu);    /* check_complete counts the class's count again */
        fprintf(stderr, "recomp: league sizes: %s dates: no room for %d rounds of %d clubs (%.50s %d): %s; the class's "
                        "%d dates and count stay\n", file, total, n, record_name(self), base, why, have);
        return;
    }
    for (int k = 0; k < total; k++) plan[k].from = from_of[plan[k].from];
    /* The table's template holds the first `rounds` dates; a probe gives the dates after them. */
    int first = probe ? rounds : 0, count = probe ? total - rounds : rounds;
    uint32_t tmpl = game_malloc((uint32_t)count * DATE_SIZE);
    if (!tmpl) return;
    int new_days[SEASON_TEMPLATE_MAX], added[SEASON_TEMPLATE_MAX];
    for (int k = 0; k < total; k++) {
        new_days[k] = plan[k].day;
        added[k] = plan[k].added;
        if (k < first || k >= first + count) continue;
        uint32_t to = tmpl + (uint32_t)(k - first) * DATE_SIZE, from = old + (uint32_t)plan[k].from * DATE_SIZE;
        memcpy(g_mem + to, g_mem + from, DATE_SIZE);
        int yoff, doy;
        season_template_split(base, plan[k].day, &yoff, &doy);
        wr16(g_mem, to, (uint16_t)doy);
        wr16(g_mem, to + 2, (uint16_t)yoff);
        if (plan[k].added &&
            season_template_weekday(base, plan[k].day) != season_template_weekday(base, days[plan[k].from]))
        {
            g_mem[to + 4] = TIME_MIDWEEK;
            /* Slot 0 as the original's midweek rounds: no weekday, kind or option (NorPrm 0x79191d writes
             * (round, 0, -1, -1, -1, 0) through 0x68a1c0, which stores +5 the value, +9 the weekday, +0xa the kind,
             * +0xb the option at 0x68a3b9..0x68a3c9). A Sunday record holds weekday 6 there (PorSecond 0x7d0d21). */
            wr32(g_mem, to + 5, 0);
            g_mem[to + 9] = g_mem[to + 0xa] = g_mem[to + 0xb] = 0xff;
        }
    }
    game_free(old);
    wr16(g_mem, count_at, (uint16_t)count);
    cpu.eax = tmpl;
    if (probe) probe_made = 1;
    if (logging() && probe) {
        fprintf(stderr, "recomp: league sizes: %s split dates %d: %d", file, base, count);
        log_days(base, new_days + first, added + first, count);
        fprintf(stderr, "\n");
    }
    if (logging() && !probe) {
        fprintf(stderr, "recomp: league sizes: %s dates: %d dates for %d clubs (%.50s %d, the class makes %d)%s\n",
                file, total, n, record_name(self), base, have, split ? ", the split's included" : "");
        fprintf(stderr, "recomp: league sizes: %s template %d: new %d", file, base, total);
        log_days(base, new_days, added, total);
        fprintf(stderr, "\n");
    }
}

uint32_t season_sizes_split_dates(uint32_t obj, int meetings, int *count, uint32_t *code)
{
    *count = 0;
    *code = 0;
    uint32_t vt = rd32(g_mem, obj), args = game_malloc(8);
    if (!readable(vt) || !args) return 0;
    uint32_t fn = rd32(g_mem, vt + 0x3cu);
    /* create_schedule (vtable +0x3c; thiscall: stage, &count, &code, &flag): stage -1 as set_up_rules calls it
     * (NirPrm 0x78ee14..0x78ee18), the count to args, the code to args + 4, no flag. */
    uint32_t a[4] = {0xffffffffu, args, args + 4, 0};
    Cpu saved = cpu;
    probe_meetings = meetings;
    probe_made = 0;
    cpu.ecx = obj;
    uint32_t t = rt_callback(fn, 4, a);
    probe_meetings = 0;
    if (rt_unwinding) return 0;
    cpu = saved;
    int n = (int16_t)rd16(g_mem, args);
    *code = rd16(g_mem, args + 4);
    game_free(args);
    /* A probe that gave no split dates leaves the class's template in t: it is no block of split dates. */
    if (!probe_made) {
        if (readable(t)) game_free(t);
        return 0;
    }
    if (!readable(t)) return 0;
    *count = n;
    return t;
}

void season_sizes_keep(uint32_t obj, int on) { keep(obj, on); }
int season_sizes_kept(uint32_t obj) { return kept_at(obj); }

int season_sizes_count(uint32_t obj)
{
    if (!sizes_on() || !readable(obj)) return 0;
    return league_count(obj, season_change_file(obj), NULL, NULL);
}

uint32_t season_sizes_game_malloc(uint32_t size) { return game_malloc(size); }
void season_sizes_game_free(uint32_t p) { game_free(p); }

/* League::free_table 0x687970 (thiscall: table, 0 for its own; ret 4): at the start of a new edition it frees the
 * table of the last one (start_new_season, for example 0x64b830). For the test log: the matches that each club of a
 * grown league played (row +0x11, the count that check_complete compares with +0xa7 at 0x689b47..0x689b58). */
void o_00687970(void);
RT_WRAP(00687970, league_free_table)
{
    uint32_t self = cpu.ecx, table = rt_arg(0);
    if (!table && readable(self)) size_step_run(self);
    if (!table && logging() && sizes_on() && readable(self) && grown_of(season_change_file(self))) {
        uint32_t t = rd32(g_mem, self + COMP_TABLE);
        int n = (int16_t)rd16(g_mem, self + COMP_CLUBS), lo = 999, hi = -1;
        if (readable(t) && n >= MIN_CLUBS && n <= MAX_CLUBS) {
            for (int i = 0; i < n; i++) {
                int p = g_mem[t + (uint32_t)i * ROW_SIZE + 0x11u];
                if (p < lo) lo = p;
                if (p > hi) hi = p;
            }
            season_split_log_end(self);
            fprintf(stderr, "recomp: league sizes: %s played %d: %d clubs, %d matches, played %d to %d (%.50s)\n",
                    season_change_file(self), (int16_t)rd16(g_mem, self + COMP_YEAR), n,
                    (int16_t)rd16(g_mem, self + COMP_MATCHES), lo, hi, record_name(self));
        }
    }
    o_00687970();
}

/* The clubs whose division has the record id of a key id global. */
static int clubs_of_key(uint32_t key)
{
    int32_t id = (int32_t)rd32(g_mem, key);
    uint32_t table = rd32(g_mem, CLUBS);
    int count = (int)rd32(g_mem, CLUB_COUNT), n = 0;
    if (id < 0 || !readable(table)) return -1;
    for (int i = 0; i < count; i++) {
        uint32_t div = rd32(g_mem, table + (uint32_t)i * CLUB_SIZE + CLUB_DIVISION);
        n += readable(div) && (int32_t)rd32(g_mem, div) == id;
    }
    return n;
}

static int top_clubs(uint32_t key_a, uint32_t key_b, uint32_t key_c)
{
    int a = key_a ? clubs_of_key(key_a) : 0, b = key_b ? clubs_of_key(key_b) : 0, c = key_c ? clubs_of_key(key_c) : 0;
    return a < 0 || b < 0 || c < 0 ? -1 : a + b + c;
}

uint32_t rt_cup_add(uint32_t original, uint32_t base, uint32_t entrants, uint32_t key_a, uint32_t key_b)
{
    if (!sizes_on()) return original;
    int top = top_clubs(key_a, key_b, 0);
    if (top < 0 || top > (int)entrants) return original;
    return (uint32_t)((int)entrants - top) - base;
}

uint32_t rt_cup_room(uint32_t original, uint32_t entrants, uint32_t key_a, uint32_t key_b, uint32_t key_c)
{
    if (!sizes_on()) return original;
    int top = top_clubs(key_a, key_b, key_c);
    if (top < 0) return original;
    int room = (int)entrants - top;
    if (room < 0) room = 0;
    if ((uint32_t)room * 6u != original && logging())
        fprintf(stderr, "recomp: league sizes: a cup takes %d lower clubs (the original %u)\n", room, original / 6u);
    return (uint32_t)room * 6u;
}

/* FraLgeCup::collect_entrants 0x5bd160: the first loop takes National clubs ([0x9cf5d4]) whose last division (+0x5b)
 * is set and is not the CFA ([0x9cf5d8]) up to 16 - 2 after 2001 (edx is 0 or -2 before `add edx, 0x10` at
 * 0x5bd244), the second loop National clubs from the CFA up to the room (0x5bd2ac). When the National holds fewer
 * clubs from the CFA than the room leaves them, the first loop takes those places too, so the cup gets its 54
 * (a 2025/26 start with the December 2025 database stopped at fra_lge_cup.cpp 73, 0x5bcb21). With enough clubs
 * from the CFA the limit is the original's. Returns the value to add to edx. */
uint32_t rt_fra_lge_first(uint32_t original, uint32_t edx)
{
    if (!sizes_on()) return original;
    int base = (int)edx + (int)original;
    int32_t national = (int32_t)rd32(g_mem, 0x9cf5d4u), cfa = (int32_t)rd32(g_mem, 0x9cf5d8u);
    uint32_t table = rd32(g_mem, CLUBS);
    int count = (int)rd32(g_mem, CLUB_COUNT), from_cfa = 0;
    if (!readable(table)) return original;
    for (int i = 0; i < count; i++) {
        uint32_t at = table + (uint32_t)i * CLUB_SIZE, div = rd32(g_mem, at + CLUB_DIVISION), last = rd32(g_mem, at + 0x5bu);
        from_cfa += readable(div) && (int32_t)rd32(g_mem, div) == national && readable(last) && (int32_t)rd32(g_mem, last) == cfa;
    }
    int room = (int)(rt_cup_room(0x60u, 54u, 0x9cf5d0u, 0x9cf5ccu, 0u) / 6u), limit = room - from_cfa;
    if (limit < base) limit = base;
    return (uint32_t)(limit - (int)edx);
}

void o_005bd160(void);

/* FraLgeCup::collect_entrants 0x5bd160, thiscall (cup). Its two National loops (0x5bd20f and 0x5bd2b1, see
 * rt_fra_lge_first) take only clubs whose last division (+0x5b) is set (0x5bd222 and 0x5bd2c4 skip a 0). A start after the last season of the club moves
 * table (2026 on, a date jump, 0.6.2 slice 10) keeps the database's National: 20 clubs with the December 2025
 * database, 3 of them with no last division, so the cup found 53 of its 54 and stopped at fra_lge_cup.cpp 73
 * (0x5bca3a, box at 0x5bcb21). When the National clubs that the loops can take are fewer than the room, a club with
 * no last division counts as a club from outside the CFA for this call: +0x5b holds its own division (the National)
 * while the original collects, and 0 again after. With enough clubs nothing changes. Only the constructor's call
 * (FraLgeCup::FraLgeCup 0x5bc980, return 0x5bca0f) stops at line 73 when short; the call of a season change (return
 * 0x5bd4d5) plays a short cup, so it stays the original's. */
RT_WRAP(005bd160, fra_lge_entrants)
{
    uint32_t cup = cpu.ecx;
    enum { MOST = 64 };
    uint32_t lent[MOST];
    int nlent = 0;
    uint32_t table = rd32(g_mem, CLUBS);
    if (sizes_on() && readable(table) && rd32(g_mem, cpu.esp) == 0x5bca0fu) {
        int32_t national = (int32_t)rd32(g_mem, 0x9cf5d4u);
        int count = (int)rd32(g_mem, CLUB_COUNT), can = 0, none = 0;
        for (int i = 0; i < count; i++) {
            uint32_t at = table + (uint32_t)i * CLUB_SIZE, div = rd32(g_mem, at + CLUB_DIVISION);
            if (!readable(div) || (int32_t)rd32(g_mem, div) != national) continue;
            if (rd32(g_mem, at + 0x5bu)) can++;
            else none++;
        }
        int top = top_clubs(0x9cf5d0u, 0x9cf5ccu, 0u), room = top < 0 ? 16 : 54 - top;     /* as rt_cup_room */
        for (int i = 0; none && can + nlent < room && nlent < MOST && i < count; i++) {
            uint32_t at = table + (uint32_t)i * CLUB_SIZE, div = rd32(g_mem, at + CLUB_DIVISION);
            if (!readable(div) || (int32_t)rd32(g_mem, div) != national || rd32(g_mem, at + 0x5bu)) continue;
            wr32(g_mem, at + 0x5bu, div);
            lent[nlent++] = at;
        }
        if (nlent)
            fprintf(stderr, "recomp: league sizes: French League Cup: %d National clubs with no last division count "
                            "as clubs from outside the CFA (%d others, room %d)\n", nlent, can, room);
    }
    o_005bd160();
    for (int i = 0; i < nlent; i++) wr32(g_mem, lent[i] + 0x5bu, 0);
    if (!cpu.eax && !rt_unwinding) cpu.eax = (uint32_t)season_cup_sizes_fra_lge(cup);   /* 0.7.2 slice 5 */
}

/* EngFaCup::collect_entrants 0x56fab0 (O294) puts the National League clubs ([0x9cf69c]) in a stack array at
 * [esp + 0x3c] (store at 0x56fd98, count [esp + 0x18]) and the English clubs below it in a stack array at
 * [esp + 0x94] (store at 0x56fd7b, count [esp + 0x14], at most 300). It then copies byte [esp + 0x12] places from
 * the first (22 at 0x56fade, one less for each FA Trophy finalist in it, copy at 0x570046..0x57008b) and byte
 * [esp + 0x13] places from the second (38 at 0x56fae3, copy at 0x56fffc..0x57003e). Neither copy tests the
 * array's count, and the frame is never cleared. With fewer National League clubs than places (a 2025/26 start
 * with the December 2025 database, changing rules on) the cup took stack values: a NULL left an empty place, any
 * other value a club at a garbage address with no players (record 351, round 10, match_eng..cpp 652 and 612).
 * Here, at `movsx di, byte ptr [esp + 0x1b]` (0x56ffe4, two pushes after 0x56ffa6, so the counts are 8 bytes
 * higher), the places the National League cannot fill go to the clubs below it, and no copy takes more than its
 * array holds. Returns the count of the second copy. With enough clubs the counts are the original's. */
uint32_t rt_fa_cup_takes(uint32_t esp)
{
    int league_take = (int8_t)rd8(g_mem, esp + 0x1au), lower_take = (int8_t)rd8(g_mem, esp + 0x1bu);
    int league_have = (int16_t)rd16(g_mem, esp + 0x20u), lower_have = (int16_t)rd16(g_mem, esp + 0x1cu);
    if (!season_game_changes() || (league_take <= league_have && lower_take <= lower_have)) return (uint32_t)lower_take;
    int league = league_take < league_have ? league_take : (league_have > 0 ? league_have : 0);
    int lower = lower_take + league_take - league;
    if (lower > lower_have) lower = lower_have > 0 ? lower_have : 0;
    fprintf(stderr, "recomp: league sizes: FA Cup takes %d National League clubs (has %d, places %d) and %d clubs "
            "below it (has %d, places %d)\n", league, league_have, league_take, lower, lower_have, lower_take);
    wr8(g_mem, esp + 0x1au, (uint8_t)league);
    wr8(g_mem, esp + 0x1bu, (uint8_t)lower);
    return (uint32_t)lower;
}

/* ItaCup::select_entrants 0x63faf0 fills a list of 32 Italian clubs (0x20 at 0x63fc21). Its first loop takes the
 * Serie A clubs, the clubs relegated from Serie A, and the clubs whose last division (+0x5b) is a Serie C1 group
 * ([0x9cf578], [0x9cf57c]) with last position (+0x5f) 3, 4 or 5 (0x63fc69..0x63fc9b). While the list is short, its
 * second loop takes C1 clubs (not in Serie A) whose last position is p, for p = 5, 6, ... (0x63fb16, 0x63fcf9..
 * 0x63fd30). A C1 club of last position 5 is then taken twice. The cup draws it twice, plays a tie with the other
 * club, and stops at Cup.cpp 1187 (Cup::process_fixture 0x51a150: Cup::find_tie 0x519c90 returns -1 at 0x51a336,
 * box at 0x51a396). The base game never showed it (inference: its data fills the list in the first loop). In a
 * season game the club moves change the Italian divisions (a 2010/11 start with the September 2010 database took
 * Foggia twice and stopped on day 330), so there the second loop starts at 6. */
uint32_t rt_ita_cup_first_position(uint32_t original)
{
    return season_game_changes() ? 6u : original;
}

/* Sets a count of places so that the row's count is the direct places, or the direct places and the winners
 * of the class's own play-off (the reading of tests/seasons/rules_report_check.py). */
static int set_places(uint32_t obj, uint32_t at, uint32_t playoff_at, int want, const char *what)
{
    if (want < 0 || want > 30) return 0;
    int now = (int8_t)g_mem[obj + at], playoff = (int8_t)g_mem[obj + playoff_at];
    /* A class with no places in this direction has its own way or none (Argentina relegates by the average of
     * three seasons, ArgPrm::set_up_rules 0x4056d1 writes 0): it stays. */
    if (now == 0 && playoff == 0) return 0;
    if (want == now || want == now + (playoff ? 1 : 0) || want == now + playoff) return 0;
    int direct = want - (playoff ? 1 : 0);
    if (direct < 0) direct = 0;
    g_mem[obj + at] = (uint8_t)direct;
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s (%.50s) %d: %s places %d, the class gave %d (+%d play-off)\n",
                season_change_file(obj), record_name(obj), (int16_t)rd16(g_mem, obj + COMP_YEAR), what, direct, now,
                playoff);    return 1;
}

/* The object of the division above or below (league +0x1c, +0x20: the record ids, -1 for none, EngFirst::
 * set_up_rules stores them at 0x572dea and 0x572df6), or 0. The object of a record is g_comp_object_table
 * [0xadadfc] at the record id, below its slot count [0xadadf8] (League::process_promotion_relegation_chain
 * 0x689c42). open_neighbour: 1 when that division is a league of the size table with no floor. */
static uint32_t neighbour(uint32_t obj, uint32_t at)
{
    int32_t id = (int32_t)rd32(g_mem, obj + at), slots = (int32_t)rd32(g_mem, COMP_SLOTS);
    uint32_t table = rd32(g_mem, COMP_OBJECTS);
    if (id < 0 || id >= slots || !readable(table)) return 0;
    uint32_t other = rd32(g_mem, table + 4u * (uint32_t)id);
    return readable(other) ? other : 0;
}

/* A class that counts the clubs of its data (IreFirst, IrePrm, PolFirst: cap 0) counts too: its moves stay in
 * pairs whenever the places of two divisions differ (the pair wrap below), so its places can change and its count
 * does not. */
static int open_neighbour(uint32_t obj, uint32_t at)
{
    uint32_t other = neighbour(obj, at);
    if (!other) return 0;
    const char *f = season_change_file(other);
    return size_file(f) && exchange_floor(f) == 0;
}

/* The divisions whose relegation places towards a division outside the size table this slice changed (D23): their
 * chain call to it moves in pairs (the wrap of 0x689c80), so that division keeps its count whatever its class. */
#define OUTSIDE_MAX 32
static uint32_t outside_set[OUTSIDE_MAX];

static void note_outside(uint32_t obj, int on)
{
    int free_at = -1;
    for (int i = 0; i < OUTSIDE_MAX; i++) {
        if (outside_set[i] == obj) {
            if (!on) outside_set[i] = 0;
            return;
        }
        if (!outside_set[i] && free_at < 0) free_at = i;
    }
    if (on && free_at >= 0) outside_set[free_at] = obj;
}

static int is_outside(uint32_t obj)
{
    for (int i = 0; obj && i < OUTSIDE_MAX; i++)
        if (outside_set[i] == obj) return 1;
    return 0;
}

/* 1 when the division below is a league outside the size table and the row's count changes after this edition. */
static int outside_change(uint32_t obj, const char *file, const SeasonRule *r)
{
    /* With changing rules off the start season's places stay (Q232; review 1 of the D23 fix). */
    if (!season_changing_rules() || season_change_is_creating()) return 0;
    uint32_t below = neighbour(obj, BELOW);
    if (!below || size_file(season_change_file(below))) return 0;
    const SeasonRule *next = row_of_season(file, r->season + 1);
    return next && next->clubs >= MIN_CLUBS && next->clubs <= MAX_CLUBS && r->clubs >= MIN_CLUBS &&
           next->clubs != r->clubs;
}

static int outside_now(uint32_t obj)
{
    if (is_outside(obj)) return 1;
    const char *file = season_change_file(obj);
    const SeasonRule *r = size_file(file) && exchange_floor(file) == 0 ? row_now(obj, file) : NULL;
    return r && outside_change(obj, file, r);
}

static void sizes_rules(uint32_t obj)
{
    if (!sizes_on() || !readable(obj)) return;
    const char *file = season_change_file(obj);
    /* A floor division keeps its places too: its count must stay, and some of its exchanges are fixed code (League
     * Two with the Conference: 2 down and 1 back gave 23 clubs in 2023/24 of a 2022/23 game). */
    if (!size_file(file) || exchange_floor(file) > 0) return;
    const SeasonRule *r = row_now(obj, file);
    if (!r) {
        note_outside(obj, 0);
        return;
    }
    /* Only towards a division of the size table with no floor: elsewhere the exchange is the class's own code
     * with fixed counts (the German Regionalliga split needs exactly 36 clubs, cmp bl, 0x24 at 0x5dbc3b, box
     * ger_first.cpp 1049 at 0x5dbcd9: the 2. Bundesliga with 2 relegation places against the Regionalliga's 4
     * promotions left it short in June 2017). */
    if (open_neighbour(obj, ABOVE))
        set_places(obj, UP, UP_PLAYOFF, r->up + season_playoffs_winners_up(file, r->season), "promotion");
    /* League Two: towards the Conference, played or not (season_sizes_eng.c, slice 8, D15). D23 (slice 3, open items
     * 248 and 249): towards a division outside the size table only in the season before the row's count changes. The
     * Segunda Liga and the Serie B relegate 3 and 4 by their class, their rows 0 to 6. The chain to such a division
     * moves in pairs (PorPrm's chain 0x7cfcbf..0x7cfd1a and BraNatFirst's 0x42afe3..0x42b124 push pairwise 1; the
     * wrap of 0x689c80 below keeps the original's call for a pair with a class outside the table), so the division
     * below keeps its count, and the size step (D13) moves the clubs of the relegation places that did not move. */
    int outside = !open_neighbour(obj, BELOW) && !season_sizes_eng_below(file) && outside_change(obj, file, r);
    int changed = 0;
    if (open_neighbour(obj, BELOW) || season_sizes_eng_below(file) || outside)
        changed = set_places(obj, DOWN, DOWN_PLAYOFF, r->down, "relegation");
    /* Each edition: the set holds a division only in the edition whose places this slice set (review 1). */
    note_outside(obj, outside && changed);
}

/* Gap row 9 (lead-notes/sizes-gaps.md): a row whose play-off cell says "none" plays no play-off. Here the pairs whose
 * lower class owns a promotion play-off against the upper division's row of status 2 (research in run-logs/0.7.2-
 * sz03/row9-research.md):
 *   IreFirst 0x635aa0 / IrePrm 0x63a900 (+0xc0 = 1 at 0x63a957), NirFirst 0x78bd70 / NirPrm 0x78ed80 (0x78edda),
 *   NorFirst 0x790b00 / NorPrm 0x792700 (0x792758), SweFirst 0x88f090 / SwePrm 0x890550 (0x8905a8).
 * The play-off plays only because of the stage count +0x2c = 1 that the lower constructor sets (IreFirst 0x634cad,
 * NirFirst 0x78b07d, NorFirst 0x78fdad, SweFirst 0x88e64d); there is no year test. With +0x2c 0 advance_stage is
 * never called (League::update_for_date 0x6849d1..0x6849e3) and check_complete is true with no stage
 * (0x689bdf..0x689be8). The builder needs both rows (no status 1 row: IreFirst asserts at 0x635e52..0x635e7d; no
 * status 2 row: a stale slot 0 at 0x635f00..0x635f08), so both divisions take one decision: the upper division's row
 * of the edition. In that edition the lower class gets +0x2c 0 and its direct up places, the upper one no relegation
 * play-off place and its direct down places. A fixed exchange (Northern Ireland, FIXED_EXCHANGE of league_sizes.py)
 * keeps the class's direct places, so only the play-off goes. start_new_season keeps +0x2c, so the next edition
 * writes 1 back. A game saved with +0x2c 0 loads with no stage array (League load 0x688e3a..0x688e5b allocates
 * +0x2c * 4 bytes), so the array gets one slot then. Only while no stage was made (+0x30 -1). */
static const char *const PLAYOFF_PAIRS[][2] = {
    {"ire_first", "ire_prm"}, {"nir_first", "nir_prm"}, {"nor_first", "nor_prm"}, {"swe_first", "swe_prm"},
};

static int pair_playoff_off(const char *upper_file, int year)
{
    /* As row_now: the start season's row with changing rules off and while the game is made (Q232). */
    if (!season_changing_rules() || season_change_is_creating()) year = season_start_year();
    const SeasonRule *r = row_of_edition(upper_file, year);
    return r && r->playoffs && !strcmp(r->playoffs, "none");
}

static void class_playoff_rules(uint32_t obj, const char *file)
{
    if (!sizes_on() || !readable(obj) || (int32_t)rd32(g_mem, obj + COMP_STAGE_NOW) != -1) return;
    int year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    for (unsigned i = 0; i < sizeof PLAYOFF_PAIRS / sizeof PLAYOFF_PAIRS[0]; i++) {
        const char *lower = PLAYOFF_PAIRS[i][0], *upper = PLAYOFF_PAIRS[i][1];
        int is_lower = !strcmp(file, lower);
        if (!is_lower && strcmp(file, upper)) continue;
        int off = pair_playoff_off(upper, year), fixed = exchange_floor(file) > 0;
        const SeasonRule *r = row_now(obj, file);
        if (is_lower) {
            int count = (int32_t)rd32(g_mem, obj + COMP_STAGE_COUNT);
            /* Every edition with no play-off: set_up_rules writes the class's places again each edition (SweFirst
             * 0x88f0dc..0x88f0ee), also when the stage count is 0 already (review 1). */
            if (off && (count == 1 || count == 0)) {
                wr32(g_mem, obj + COMP_STAGE_COUNT, 0);
                g_mem[obj + UP_PLAYOFF] = 0;
                if (!fixed && r && r->up >= 0 && r->up <= 30) g_mem[obj + UP] = (uint8_t)r->up;
            } else if (!off && count == 0) {
                uint32_t arr = rd32(g_mem, obj + COMP_STAGES);
                if (!readable(arr)) {
                    arr = game_malloc(4);
                    if (!arr) return;
                    wr32(g_mem, obj + COMP_STAGES, arr);
                }
                wr32(g_mem, obj + COMP_STAGE_COUNT, 1);
            } else {
                return;
            }
        } else {
            if (!off || (int8_t)g_mem[obj + DOWN_PLAYOFF] <= 0) return;
            g_mem[obj + DOWN_PLAYOFF] = 0;
            if (!fixed && r && r->down >= 0 && r->down <= 30) g_mem[obj + DOWN] = (uint8_t)r->down;
        }
        if (logging())
            fprintf(stderr, "recomp: league sizes: %s %d: %s (the row of %s says none): stages %d, places %d up "
                            "%d down\n", file, year, off ? "no class play-off" : "the class play-off again", upper,
                    (int32_t)rd32(g_mem, obj + COMP_STAGE_COUNT), (int8_t)g_mem[obj + UP], (int8_t)g_mem[obj + DOWN]);
        return;
    }
}

void season_sizes_rules(uint32_t obj)
{
    if (readable(obj)) size_step_run(obj);
    sizes_rules(obj);
    if (!rt_unwinding && readable(obj)) class_playoff_rules(obj, season_change_file(obj));
    if (!rt_unwinding) season_playoffs_rules(obj);   /* slice 19: a play-off in a league with none */
    if (!rt_unwinding) season_divisions_rules(obj);  /* slice 20: the places towards a new division */
    if (!rt_unwinding && sizes_on()) season_sizes_bra_rules(obj, season_change_file(obj));  /* 0.7.2 slice 17 */
}

/* ---- Promotion and relegation of a size change ---- */

/* The table rows of a league with a status. */
static int rows_with(uint32_t league, int a, int b)
{
    if (!readable(league)) return -1;
    uint32_t table = rd32(g_mem, league + COMP_TABLE);
    int n = (int16_t)rd16(g_mem, league + COMP_CLUBS), k = 0;
    if (!readable(table) || n <= 0 || n > 200) return -1;
    for (int i = 0; i < n; i++) {
        int s = g_mem[table + (uint32_t)i * ROW_SIZE + ROW_STATUS];
        k += s == a || s == b;
    }
    return k;
}

/* The rows that the places of a league send up (up 1) or down at the season change. League::update_positions_and_
 * status 0x684cd0 gives the +0xbe rows from the top status 5 or 0 and the +0xc1 rows from the bottom status 3
 * (0x684e11..0x684ef5, 0x685149..0x68525e). A play-off of +0xbf (+0xc0) places sends its one winner up (its loser
 * down), the reading of set_places. -1 for no league. */
static int place_moves(uint32_t league, int up)
{
    if (!readable(league)) return -1;
    int direct = (int8_t)g_mem[league + (up ? UP : DOWN)];
    int playoff = (int8_t)g_mem[league + (up ? UP_PLAYOFF : DOWN_PLAYOFF)];
    return direct < 0 ? -1 : direct + (playoff > 0 ? 1 : 0);
}

/* The expected count of a call of league_promote_and_relegate 0x689c80: up 1 the clubs that the lower league sends
 * up, up 0 the clubs that the upper league sends down. ItaSerA's chain 0x642410 pushes 4 and 4 for Serie A and B
 * (0x642476, 0x642478), the places of the base game. In a season game the places change (2003/04 Serie A 3 down
 * and Serie B 4 up, 2016/17 3 and 3), so there the count comes from the places of the season. */
uint32_t rt_promote_expected(uint32_t league, uint32_t up, uint32_t original)
{
    if (!sizes_on()) return original;
    int n = place_moves(league, up != 0);
    return n >= 0 ? (uint32_t)n : original;
}

void o_00689c80(void);

/* league_promote_and_relegate 0x689c80, stdcall (upper, lower, expire_contracts, pairwise, expected_up,
 * expected_down), ret 0x18. It moves the rows of status 3 of the upper league down (0x689d26, 0x689e52) and the
 * rows of status 0 or 5 of the lower league up (0x689d47..0x689d4e, 0x689efd..0x689f03). In pairs (pairwise,
 * 0x689cf9) a row moves only with a partner, so the sizes stay. The expected counts (0xff: none) are compared
 * with the moves at 0x689f47 and 0x689fbb (asserts league.cpp 5596 and 5602). */
RT_WRAP(00689c80, promote_relegate)
{
    if (!sizes_on()) {
        o_00689c80();
        return;
    }
    uint32_t upper = rt_arg(0), lower = rt_arg(1), pairwise = rt_arg(3);
    int down = rows_with(upper, STATUS_DOWN, STATUS_DOWN), up = rows_with(lower, 0, 5);
    int exp_up = (int8_t)(rt_arg(4) & 0xff), exp_down = (int8_t)(rt_arg(5) & 0xff);
    /* Only a pair of two size-table classes: only there can the places of this slice differ. Any other pair keeps
     * the original's call. */
    int ours = size_file(season_change_file(upper)) && size_file(season_change_file(lower));
    /* Slice 3 (D13): with changing rules on, each pair of the chain is noted for the size step. */
    if (ours && season_changing_rules() && !season_change_is_creating()) step_note_pair(upper, lower);
    else if (season_changing_rules() && !season_change_is_creating() && size_file(season_change_file(upper)) &&
             !size_file(season_change_file(lower)) && readable(rd32(g_mem, lower + COMP_RECORD)))
        step_note_outside(upper, lower);
    /* D23 (slice 3): a division whose relegation places towards a division outside the size table this slice set
     * sends its clubs in pairs, so the division below keeps its count. Most classes pass pairs already (0x7cfcbf,
     * 0x42afe3); the Turkish chain 0x8fe2d4..0x8fe38b pushes 0 and the base chain 0x689c60 passes the class's flag.
     * After a load the set is empty, so the row is read again. */
    if (!ours && !pairwise && season_changing_rules() && !season_change_is_creating() &&
        size_file(season_change_file(upper)) && !size_file(season_change_file(lower)) && outside_now(upper)) {
        wr32(g_mem, cpu.esp + 4 + 12, 1);
        pairwise = 1;
        if (logging())
            fprintf(stderr, "recomp: league sizes: %s to %s moves in pairs (D23 places towards a division outside the "
                            "size table)\n", season_change_file(upper), season_change_file(lower));
    }
    int change = ours && down >= 0 && up >= 0 && down != up, single = 0;
    /* When the places of the two divisions differ, the clubs move on their own only with changing rules on and
     * between two classes of the size table. Else the clubs move in pairs, so no size changes (with changing rules
     * off the start season's sizes stay, Q232). The French chain passes no pairs (FraFirst::start_new_season pushes
     * 0 at 0x5bc4f1, League::process_promotion_relegation_chain passes it at 0x689c59), so pairs are set here.
     * D23 (slice 3): the real places of the season before a change move the clubs, so the count changes in its own
     * season. A division may hold a count outside its floor and cap for a moment (Ligue 1 from 20 to 18 in 2023/24
     * puts 22 in Ligue 2 until its exchange with the National): the size step (D13, noted above) runs after the whole
     * chain and before any new table, and brings each division within its cap and floor to its row. Without the step
     * (a chain while the game is made) the moves stay in pairs unless both counts fit. A class that counts the clubs
     * of its data (cap 0) takes only some counts, so its moves stay in pairs. */
    if (change && season_changing_rules()) {
        const char *fu_name = season_change_file(upper), *fl_name = season_change_file(lower);
        int cu = cap_of(fu_name), cl = cap_of(fl_name);
        int lu = exchange_floor(fu_name) > MIN_CLUBS * 4 ? exchange_floor(fu_name) : MIN_CLUBS * 4;
        int ll = exchange_floor(fl_name) > MIN_CLUBS * 4 ? exchange_floor(fl_name) : MIN_CLUBS * 4;
        int nu = clubs_in_division(upper) - down + up, nl = clubs_in_division(lower) + down - up;
        int fits = nu >= lu && nu <= cu && nl >= ll && nl <= cl;
        /* Only when both new rows are counts that the classes hold: a row above the cap (Serie A 20 in 2004/05 with a
         * class of 18) would leave the step to send down clubs that finished safe (review 2). */
        int reach = row_reachable(upper, fu_name) && row_reachable(lower, fl_name);
        single = cu > 0 && cl > 0 && (fits || (reach && !season_change_is_creating()));
        if (logging() && !single)
            fprintf(stderr, "recomp: league sizes: %s %d clubs and %s %d clubs after the season change do not fit "
                            "(%d to %d and %d to %d): the clubs move in pairs\n", fu_name, nu, fl_name, nl, lu, cu, ll, cl);
    }
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s (%.50s) down %d, %s (%.50s) up %d, pairs %u, expected %d up %d down%s\n",
                season_change_file(upper), record_name(upper), down, season_change_file(lower), record_name(lower), up,
                pairwise, exp_up, exp_down, !change ? "" : single ? ": each club moves on its own" : ": in pairs");
    if (change) wr32(g_mem, cpu.esp + 4 + 4 * 3, single ? 0u : 1u);
    /* The moves that the call makes: each row on its own, or pairs. The callers with counts pass the moves of the
     * places (rt_promote_expected). In pairs only as many clubs move as the smaller count, so when this wrap sets
     * pairs for places that differ, both counts become the smaller one. */
    int pairs = rd32(g_mem, cpu.esp + 4 + 4 * 3) != 0, n = down < up ? down : up;
    int moved_up = pairs ? n : up, moved_down = pairs ? n : down;
    if (change && pairs && exp_up != -1 && exp_down != -1) {
        exp_up = exp_down = exp_up < exp_down ? exp_up : exp_down;
        wr32(g_mem, cpu.esp + 4 + 4 * 4, (uint32_t)exp_up);
        wr32(g_mem, cpu.esp + 4 + 4 * 5, (uint32_t)exp_down);
    }
    /* A count that still differs from the rows is a fault: the statuses of the table do not match its places (a
     * play-off with no result, a status set twice). The log names it and the check is dropped, so the season goes
     * on (asserts league.cpp 5596 and 5602). */
    if (down >= 0 && up >= 0 && ((exp_up != -1 && exp_up != moved_up) || (exp_down != -1 && exp_down != moved_down))) {
        fprintf(stderr, "recomp: league sizes: fault: %s (%.50s) and %s (%.50s) expect %d up and %d down by their "
                        "places, the rows move %d and %d; the check is dropped\n", season_change_file(upper),
                record_name(upper), season_change_file(lower), record_name(lower), exp_up, exp_down, moved_up,
                moved_down);
        if (exp_up != -1 && exp_up != moved_up) wr32(g_mem, cpu.esp + 4 + 4 * 4, 0xffu);
        if (exp_down != -1 && exp_down != moved_down) wr32(g_mem, cpu.esp + 4 + 4 * 5, 0xffu);
    }
    o_00689c80();
}

/* ---- The size step (0.7.2 league sizes, slice 3, D13 and D18; season_size_step.h) ----
 *
 * The step needs the last tables of the divisions and the clubs that the chain moved, and must run before the first
 * division of the chain makes its new table and its date template. The chain runs in the start_new_season of its
 * top league before anything else of the new edition: EngPrm::start_new_season 0x574c10 calls the chain (vtable
 * +0xb0, 0x574c23), the Conference exchanges (0x574c3e, 0x574c45), League::free_table 0x687970 (0x574c4d), adds 1 to
 * the edition (0x574c7b), then set_up_rules (vtable +0x8c, 0x574c81: create_schedule) and create_table (0x574cbf).
 * The chain takes its pairs from the top down (League::process_promotion_relegation_chain 0x689c20 at 0x689c42..
 * 0x689c6c; ItaSerA 0x642410 and EngPrm 0x5752f0 call 0x689c80 for each pair in the order of the tiers). So the
 * wrap of 0x689c80 notes each pair with the tables of both divisions (the clubs in the order of their places, row
 * +0x39), and the first of free_table, set_up_rules, create_schedule and create_table that follows runs the step on
 * every noted division. A division whose edition moved on since the chain made its new table, so the step leaves it
 * (the division that runs the step is the one that is making its table now). A loaded game of 0.7.1 holds its old
 * counts, and reaches the rows at its next season change in the same way (D18). */
#define STEP_DIVS 48
static struct {
    uint32_t obj, rec;
    int year, nlower, nrows;
    int keep;               /* gap row 8: a division outside the size table below a lowest one; it keeps its count */
    int lower[STEP_MAX_LOWER];
    uint32_t club[STEP_MAX_ROWS];
} step_seen[STEP_DIVS];
static int nstep_seen, step_running;

static int step_index(uint32_t obj)
{
    for (int i = 0; i < nstep_seen; i++)
        if (step_seen[i].obj == obj) return i;
    if (nstep_seen >= STEP_DIVS || !readable(obj)) return -1;
    uint32_t table = rd32(g_mem, obj + COMP_TABLE), rec = rd32(g_mem, obj + COMP_RECORD);
    int n = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    if (!readable(rec)) return -1;
    int i = nstep_seen++;
    memset(&step_seen[i], 0, sizeof step_seen[i]);
    step_seen[i].obj = obj;
    step_seen[i].rec = rec;
    step_seen[i].year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    if (!readable(table) || n <= 0) return i;
    if (n > STEP_MAX_ROWS) n = STEP_MAX_ROWS;
    int pos[STEP_MAX_ROWS];
    for (int r = 0; r < n; r++) {
        uint32_t row = table + (uint32_t)r * ROW_SIZE, c = rd32(g_mem, row);
        if (!readable(c)) continue;
        /* Insertion by place, a row with no place last, in the table's order. */
        int p = (int8_t)g_mem[row + ROW_POS], k = step_seen[i].nrows;
        if (p <= 0) p = 1000 + r;
        while (k > 0 && pos[k - 1] > p) {
            pos[k] = pos[k - 1];
            step_seen[i].club[k] = step_seen[i].club[k - 1];
            k--;
        }
        pos[k] = p;
        step_seen[i].club[k] = c;
        step_seen[i].nrows++;
    }
    return i;
}

void season_sizes_step_note(uint32_t upper, uint32_t lower) { step_note_pair(upper, lower); }

/* Gap row 8 (lead-notes/sizes-gaps.md): a lowest division of the size table trades its surplus and its lack with the
 * real tier below it, a division outside the table, as the chain does. That division keeps its count: the plan sends
 * its worst clubs that stayed to no division for each club that comes down, and fills it from its nation's clubs with
 * no division for each club that goes up, so a class of groups with fixed arrays (PorSecondB 0x7d2870, the
 * Regionalliga of exactly 36, cmp 0x24 at 0x5dbc3b) holds the clubs that it held after the chain. */
static void step_note_outside(uint32_t upper, uint32_t lower)
{
    if (step_running) return;
    step_note_pair(upper, lower);
    for (int i = 0; i < nstep_seen; i++)
        if (step_seen[i].obj == lower) step_seen[i].keep = 1;
}

static void step_note_pair(uint32_t upper, uint32_t lower)
{
    if (step_running) return;
    int u = step_index(upper), l = step_index(lower);
    if (u < 0 || l < 0 || u == l) return;
    for (int k = 0; k < step_seen[u].nlower; k++)
        if (step_seen[u].lower[k] == l) return;
    if (step_seen[u].nlower < STEP_MAX_LOWER) step_seen[u].lower[step_seen[u].nlower++] = l;
}

/* The count of a division's row in an edition (within the class's cap and floor), or -1: no part in the step (a
 * class that counts the clubs of its data, cap 0, takes only some counts: IrePrm 10 or 12, 0x63a79f..0x63a7c6). */
static int step_want(uint32_t obj, const char *file, int year)
{
    /* The Argentine second division takes exactly 25 (ArgSecond::fill_league_table 0x407ce2) and gives the Primera
     * its clubs (slice 15; its own row is slice 16's). */
    if (!strcmp(file, "arg_second")) return 25;
    int cap = season_ita_cap(obj, cap_of(file));     /* slice 20b: Serie C */
    if (cap <= 0) return -1;
    const SeasonRule *r = row_of_edition(file, year);
    if (!r || r->clubs < MIN_CLUBS || r->clubs > MAX_CLUBS) return -1;
    int n = r->clubs, low = floor_of(file);
    if (n > cap) n = cap;
    if (low > 0 && n < low) n = low;
    return n;
}

/* 1 when the row of the next edition of a division is a count that its class holds (within its cap and floor). */
static int row_reachable(uint32_t obj, const char *file)
{
    int cap = season_ita_cap(obj, cap_of(file));
    const SeasonRule *r = row_of_edition(file, (int16_t)rd16(g_mem, obj + COMP_YEAR) + 1);
    return cap > 0 && r && r->clubs >= MIN_CLUBS && r->clubs <= cap && r->clubs >= floor_of(file);
}

int season_sizes_plays_short(const char *file)
{
    return sizes_on() && size_file(file) && cap_of(file) > 0 && floor_of(file) == 0;
}

static uint32_t current_division(uint32_t club) { return rd32(g_mem, club + CLUB_DIVISION); }

static int is_human(uint32_t club)
{
    uint32_t a[1] = {club};
    uint32_t humans = rd32(g_mem, HUMANS);
    return readable(humans) && call_this(F_IS_HUMAN, humans, 1, a) != 0;
}

/* 1 when a club may go up into the division of record rec by the step: no reserve or B side (D12), and no club
 * that a second-team rule bars from it (the rules of the club moves, season_clubs.c fill_takes). */
static int up_ok(uint32_t club, uint32_t rec)
{
    if (season_clubs_reserve_name(win_str(club + CLUB_NAME))) return 0;
    if (season_second_barred(club, rec) || season_fin_barred(club, rec) || season_bel_barred(club, rec)) return 0;
    return !season_esp_barred(club, rec, current_division);
}

/* up_ok depends on the division that the club would join: the plan takes a club only into a division of its own
 * chain, the one above its own or, for a club with no division, the lowest of its nation that takes part. 0 when the
 * game unwinds. */
static int step_up_ok(const StepDivision *div, int ndiv, StepClub *club, int nclub, const uint32_t *at)
{
    for (int k = 0; k < nclub; k++) {
        int d = club[k].div, to = -1;
        if (d == STEP_NONE) {
            for (int e = 0; e < ndiv && to < 0; e++)
                if (!div[e].nlower && div[e].want >= 0 && div[e].nation == club[k].nation) to = e;
        } else if (d >= 0) {
            for (int e = 0; e < ndiv && to < 0; e++)
                for (int j = 0; j < div[e].nlower; j++)
                    if (div[e].lower[j] == d) to = e;
        }
        club[k].up_ok = to >= 0 && up_ok(at[k], step_seen[to].rec);
        if (rt_unwinding) return 0;
    }
    return 1;
}

static void size_step_run(uint32_t trigger)
{
    if (!nstep_seen || step_running || rt_unwinding) return;
    if (!sizes_on() || !season_changing_rules()) {
        nstep_seen = 0;
        return;
    }
    step_running = 1;
    int ndiv = nstep_seen;
    StepDivision div[STEP_DIVS];
    memset(div, 0, sizeof div);
    uint32_t table = rd32(g_mem, CLUBS);
    int count = (int)rd32(g_mem, CLUB_COUNT), national_from = count - 2 * (int)rd32(g_mem, NATION_COUNT);
    int nmax = ndiv * STEP_MAX_ROWS + (readable(table) && count > 0 ? count : 0);
    StepClub *club = calloc((size_t)nmax, sizeof *club);
    uint32_t *at = calloc((size_t)nmax, sizeof *at);
    StepMove *moves = calloc((size_t)nmax, sizeof *moves);
    int nclub = 0;
    if (!club || !at || !moves) goto done;
    for (int d = 0; d < ndiv; d++) {
        uint32_t obj = step_seen[d].obj;
        const char *file = season_change_file(obj);
        int year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
        div[d].want = step_seen[d].keep ? clubs_in_division(obj) : step_want(obj, file, step_seen[d].year + 1);
        div[d].started = obj != trigger && year != step_seen[d].year;
        div[d].nation = (int)(rd32(g_mem, step_seen[d].rec + REC_NATION) & 0x7fffffff);
        div[d].nlower = step_seen[d].nlower;
        memcpy(div[d].lower, step_seen[d].lower, sizeof div[d].lower);
    }
    /* The clubs of the last tables. */
    for (int d = 0; d < ndiv; d++)
        for (int r = 0; r < step_seen[d].nrows; r++) {
            uint32_t c = step_seen[d].club[r];
            int k = 0;
            while (k < nclub && at[k] != c) k++;
            if (k == nclub) at[nclub++] = c;
            div[d].rows[div[d].nrows++] = k;
        }
    int from_tables = nclub;
    /* Every other club of a noted division (a club that joined it outside the noted pairs, such as the Conference
     * club that EngPrm::exchange_conference_feeder_clubs 0x5750a0 moves up through 0x6830b0 at 0x575268, so the plan
     * counts what the division holds), and the clubs with no division of the nation of a lowest division (no club
     * of a national side, no club with no nation: season_clubs.c, the shells). A club of a spare division (A Lower
     * Division, A Premier Division: the small clubs of every nation) counts as a club with no division, as the fill
     * of a new game takes it (season_clubs.c group 2): the Portuguese clubs below the Segunda Liga sit there. */
    for (int i = 0; readable(table) && i < count && i < national_from; i++) {
        uint32_t c = table + (uint32_t)i * CLUB_SIZE, nation = rd32(g_mem, c + CLUB_NATION), now = current_division(c);
        if (now && (season_clubs_in_spare(c) || season_arg_third(now))) now = 0;
        int wanted = 0;
        if (now) {
            for (int d = 0; d < ndiv && !wanted; d++) wanted = now == step_seen[d].rec;
        } else if (nation) {
            for (int d = 0; d < ndiv && !wanted; d++)
                wanted = !div[d].nlower && div[d].want >= 0 && rd32(g_mem, step_seen[d].rec + REC_NATION) == nation;
        }
        if (!wanted) continue;
        int k = 0;
        while (k < from_tables && at[k] != c) k++;
        if (k == from_tables) at[nclub++] = c;
    }
    for (int k = 0; k < nclub; k++) {
        uint32_t c = at[k], now = current_division(c);
        if (now && (season_clubs_in_spare(c) || season_arg_third(now))) now = 0;
        club[k].div = now ? -3 : STEP_NONE;     /* -3: a division outside the step */
        for (int d = 0; d < ndiv; d++)
            if (now == step_seen[d].rec) club[k].div = d;
        club[k].nation = (int)(rd32(g_mem, c + CLUB_NATION) & 0x7fffffff);
        club[k].rep = (int16_t)rd16(g_mem, c + CLUB_REPUTATION);
        club[k].up_ok = 1;
        club[k].down_ok = 1;
    }
    if (!step_up_ok(div, ndiv, club, nclub, at)) goto done;
    /* The step never sends down a club with a human manager (review 2: a club that finished safe). */
    for (int d = 0; d < ndiv; d++)
        for (int r = 0; r < div[d].nrows; r++) {
            club[div[d].rows[r]].down_ok = !is_human(at[div[d].rows[r]]);
            if (rt_unwinding) goto done;
        }
    /* A division outside the table that cannot keep its count (no row of its last table to send out, no club of its
     * nation to take in) takes no part, and the plan runs again: the lowest division of the table then trades with
     * no division as before. */
    /* An outside division that made its new table already gives and takes nothing (review 1): it leaves the lists of
     * the divisions above, which then trade with no division as before. */
    for (int d = 0; d < ndiv; d++)
        if (step_seen[d].keep && div[d].started) {
            div[d].want = -1;
            for (int e = 0; e < ndiv; e++) {
                int k = 0;
                for (int j = 0; j < div[e].nlower; j++)
                    if (div[e].lower[j] != d) div[e].lower[k++] = div[e].lower[j];
                div[e].nlower = k;
            }
        }
    if (!step_up_ok(div, ndiv, club, nclub, at)) goto done;
    StepClub *start = malloc((size_t)nclub * sizeof *start + 1);
    if (!start) goto done;
    memcpy(start, club, (size_t)nclub * sizeof *club);
    int n = 0;
    for (int pass = 0; pass <= ndiv; pass++) {
        n = season_size_step_plan(div, ndiv, club, nclub, moves, nmax);
        int again = 0;
        for (int d = 0; d < ndiv; d++)
            if (step_seen[d].keep && div[d].want >= 0 && div[d].short_by != 0) {
                if (logging())
                    fprintf(stderr, "recomp: league sizes: step: %.50s (outside the size table) cannot keep its %d "
                                    "clubs: it takes no part\n", record_name(step_seen[d].obj), div[d].want);
                div[d].want = -1;
                for (int e = 0; e < ndiv; e++) {
                    int k = 0;
                    for (int j = 0; j < div[e].nlower; j++)
                        if (div[e].lower[j] != d) div[e].lower[k++] = div[e].lower[j];
                    div[e].nlower = k;
                }
                again = 1;
            }
        if (!again) break;
        memcpy(club, start, (size_t)nclub * sizeof *club);
        if (!step_up_ok(div, ndiv, club, nclub, at)) {
            free(start);
            goto done;
        }
    }
    free(start);
    for (int i = 0; i < n && !rt_unwinding; i++) {
        uint32_t c = at[moves[i].club], old = current_division(c);
        uint32_t to = moves[i].to >= 0 ? step_seen[moves[i].to].rec : 0;
        int up = moves[i].from == STEP_NONE;
        for (int j = 0; moves[i].to >= 0 && j < div[moves[i].to].nlower; j++)
            up |= div[moves[i].to].lower[j] == moves[i].from;
        if (up) {
            /* Up: as a promotion of 0x689c80 (0x689d68..0x689d83). */
            uint32_t a[3] = {c, to, 0};
            call_this(F_MOVE_CLUB, step_seen[moves[i].to].obj, 3, a);
        } else {
            /* Down: as a relegation of 0x689c80 (0x689e58..0x689e70). The Argentine second division sends its
             * surplus to the third division, not out of the league system (slice 15). */
            if (!to && moves[i].from >= 0 && !strcmp(season_change_file(step_seen[moves[i].from].obj), "arg_second"))
                to = season_arg_third_record();
            wr32(g_mem, c + CLUB_LAST_DIVISION, old);
            wr32(g_mem, c + CLUB_DIVISION, to);
            uint32_t a[2] = {c, old};
            call_this(F_RELEGATION_CLAUSES, rd32(g_mem, CONTRACTS), 2, a);
        }
        if (logging())
            fprintf(stderr, "recomp: league sizes: step: %.50s from %.50s to %.50s\n", win_str(c + CLUB_NAME),
                    old ? win_str(old + 4) : "no division", to ? win_str(to + 4) : "no division");
    }
    for (int d = 0; d < ndiv && !rt_unwinding; d++) {
        if (div[d].want < 0) continue;
        uint32_t obj = step_seen[d].obj;
        int have = clubs_in_division(obj);
        if (logging() || have != div[d].want)
            fprintf(stderr, "recomp: league sizes: step: %s (%.50s) %d: %d clubs, the row %d%s\n",
                    season_change_file(obj), record_name(obj), step_seen[d].year + 1, have, div[d].want,
                    div[d].started ? " (its new table is made: no step)"
                    : have < div[d].want ? (div[d].nlower ? ": the division below has no club to give"
                                                          : ": no club of its nation with no division or in a spare division")
                    : have > div[d].want ? ": no club can go down" : "");
    }
done:
    free(club);
    free(at);
    free(moves);
    nstep_seen = 0;
    step_running = 0;
}

/* ---- News (rule kind "sizes") ---- */

/* The count and places that each league held last season, so the news names only what the game applies. A new
 * game and a loaded game note the counts of their own objects (sizes_new_game, sizes_loaded), so a game loaded
 * after another game never compares with the objects of the other game. */
#define MAX_SEEN 512
static struct { uint32_t obj; int clubs, up, down; } seen[MAX_SEEN];
static int nseen;

static int seen_at(uint32_t obj, int add)
{
    for (int i = 0; i < nseen; i++)
        if (seen[i].obj == obj) return i;
    if (!add || nseen >= MAX_SEEN) return -1;
    seen[nseen].obj = obj;
    seen[nseen].clubs = -1;
    return nseen++;
}

static void note(uint32_t obj)
{
    int i = seen_at(obj, 1);
    if (i < 0) return;
    seen[i].clubs = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    seen[i].up = (int8_t)g_mem[obj + UP];
    seen[i].down = (int8_t)g_mem[obj + DOWN];
}

static void sizes_after(const SeasonChange *c)
{
    if (!sizes_on() || !c->started || !size_file(c->file) || !readable(c->comp)) return;
    int i = seen_at(c->comp, 0);
    if (i >= 0 && seen[i].clubs >= 0 && !c->start) {
        const char *name = record_name(c->comp);
        int clubs = (int16_t)rd16(g_mem, c->comp + COMP_CLUBS), up = (int8_t)g_mem[c->comp + UP],
            down = (int8_t)g_mem[c->comp + DOWN];
        if (clubs != seen[i].clubs)
            season_change_news("The %.50s now has %d clubs (%d last season).", name, clubs, seen[i].clubs);
        if (up != seen[i].up)
            season_change_news("%d club%s now go%s up from the %.50s (%d last season).", up, up == 1 ? "" : "s",
                               up == 1 ? "es" : "", name, seen[i].up);
        if (down != seen[i].down)
            season_change_news("%d club%s now go%s down from the %.50s (%d last season).", down, down == 1 ? "" : "s",
                               down == 1 ? "es" : "", name, seen[i].down);
    }
    note(c->comp);
}

/* At a new game the constructors' set_up_rules run before g_comp_object_table is filled (build_comp_object_table
 * 0x8386f0, called at 0x829640 in create_game), so no neighbour is known there: the places of the start season
 * are set again once the game is made. */
static void sizes_new_game(int start_year)
{
    (void)start_year;
    nstep_seen = 0;
    if (!sizes_on()) return;
    uint32_t comps[512];
    int n = season_change_comps(comps, 512);
    nseen = 0;
    for (int i = 0; i < n && !rt_unwinding; i++) {
        /* Only the classes whose set_up_rules season_size_wraps.c wraps: those are the leagues that these rules
         * meet in every later season. Another class keeps its own stages: GerRegional's constructor sets the
         * stage count +0x2c to 1 (0x5dcc3d) for its South group, and the play-off rules dropped that stage
         * (+0x30 -1) while its fixtures stayed, so a German start with the regional divisions stopped at
         * league.cpp 3214 (League::add_result 0x685d30, stage test 0x685ddb..0x685de7) on its first match day
         * (slice 20, found in 2007 and 2008 starts). */
        if (!size_file(season_change_file(comps[i]))) continue;
        season_sizes_rules(comps[i]);
        note(comps[i]);
    }
}

/* After a load: the counts of the loaded game. It only reads the objects. */
static void sizes_loaded(int start_year)
{
    (void)start_year;
    nseen = 0;
    nstep_seen = 0;
    if (!sizes_on()) return;
    uint32_t comps[512];
    int n = season_change_comps(comps, 512);
    for (int i = 0; i < n && !rt_unwinding; i++)
        if (size_file(season_change_file(comps[i]))) note(comps[i]);
}

const SeasonRuleKind season_kind_sizes = {"sizes", sizes_new_game, sizes_loaded, NULL, NULL, NULL, sizes_after};
