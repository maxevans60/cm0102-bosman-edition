/* League sizes, promotion and relegation in existing divisions (season_sizes.h). */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_names.h"
#include "season_rules.h"
#include "season_sizes.h"

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
#define STATUS_DOWN 3               /* league_promote_and_relegate 0x689c80: moves down a row of 3 (0x689d26) */
#define MIN_CLUBS 2
#define MAX_CLUBS 60

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

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

/* The smallest count of a file's class (0: any count from MIN_CLUBS). */
static int floor_of(const char *file)
{
    int i = size_index(file);
    return i >= 0 ? season_size_floors[i] : 0;
}

/* A count within the class's floor and cap. */
static int within(const char *file, int n)
{
    int cap = cap_of(file), low = floor_of(file);
    if (cap > 0 && n > cap) n = cap;
    if (low > 0 && n < low) n = low;
    return n;
}

/* The row of a season, or the last row before it (Q248: after the last row the rules stay). */
static const SeasonRule *row_of_season(const char *file, int season)
{
    for (int s = season; s >= SEASON_FIRST_YEAR && s > season - 10; s--) {
        const SeasonRule *r = season_rules_find(s, file, 0);
        if (r) return r;
    }
    return NULL;
}

/* 1 when the real season of a row begins with the year ("2011", "2010/11", "2011-12 (first stage ...)"). */
static int real_starts(const SeasonRule *r, int year)
{
    char y[8];
    snprintf(y, sizeof y, "%d", year);
    return r && r->real_season && !strncmp(r->real_season, y, 4) &&
           (r->real_season[4] < '0' || r->real_season[4] > '9');
}

/* The row of the rules of a league object now (season_sizes.h). */
static const SeasonRule *row_now(uint32_t obj, const char *file)
{
    if (!season_changing_rules() || season_change_is_creating()) return row_of_season(file, season_start_year());
    int year = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    for (int s = year - 1; s <= year; s++) {
        const SeasonRule *r = season_rules_find(s, file, 0);
        if (real_starts(r, year)) return r;
    }
    return row_of_season(file, year);
}

int season_sizes_clubs(const char *file, int season)
{
    if (!size_file(file)) return -1;
    const SeasonRule *r = row_of_season(file, season);
    if (!r || r->clubs < MIN_CLUBS || r->clubs > MAX_CLUBS || cap_of(file) <= 0) return -1;
    return within(file, r->clubs);
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

uint32_t rt_league_clubs(uint32_t obj, uint32_t original)
{
    if (!season_game_changes() || !readable(obj)) return original;
    const char *file = season_change_file(obj);
    int have = clubs_in_division(obj);
    if (have < MIN_CLUBS || have > MAX_CLUBS) return original;
    int cap = cap_of(file), n = cap > 0 && have > cap ? cap : have;     /* fewer than the floor: the clubs it has */
    const SeasonRule *r = row_now(obj, file);
    int want = r && r->clubs >= MIN_CLUBS && r->clubs <= MAX_CLUBS ? r->clubs : (int)original;
    if (logging() || n != want)
        fprintf(stderr, "recomp: league sizes: %s (%.50s) %d: %d clubs in the division, %d in the table%s, class %u "
                        "(holds %d)\n", file, record_name(obj), (int16_t)rd16(g_mem, obj + COMP_YEAR), have, want,
                r ? "" : " (no row)", original, cap);
    wr16(g_mem, obj + COMP_MATCHES, 0xffffu);       /* check_complete counts the matches of the new count */
    return (uint32_t)n;
}

uint32_t rt_league_dates(uint32_t obj, uint32_t original)
{
    if (!season_game_changes() || !readable(obj)) return original;
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
    if (!season_game_changes()) return original;
    int top = top_clubs(key_a, key_b, 0);
    if (top < 0 || top > (int)entrants) return original;
    return (uint32_t)((int)entrants - top) - base;
}

uint32_t rt_cup_room(uint32_t original, uint32_t entrants, uint32_t key_a, uint32_t key_b, uint32_t key_c)
{
    if (!season_game_changes()) return original;
    int top = top_clubs(key_a, key_b, key_c);
    if (top < 0) return original;
    int room = (int)entrants - top;
    if (room < 0) room = 0;
    if ((uint32_t)room * 6u != original && logging())
        fprintf(stderr, "recomp: league sizes: a cup takes %d lower clubs (the original %u)\n", room, original / 6u);
    return (uint32_t)room * 6u;
}

/* Sets a count of places so that the row's count is the direct places, or the direct places and the winners
 * of the class's own play-off (the reading of tests/seasons/rules_report_check.py). */
static void set_places(uint32_t obj, uint32_t at, uint32_t playoff_at, int want, const char *what)
{
    if (want < 0 || want > 30) return;
    int now = (int8_t)g_mem[obj + at], playoff = (int8_t)g_mem[obj + playoff_at];
    /* A class with no places in this direction has its own way or none (Argentina relegates by the average of
     * three seasons, ArgPrm::set_up_rules 0x4056d1 writes 0): it stays. */
    if (now == 0 && playoff == 0) return;
    if (want == now || want == now + (playoff ? 1 : 0) || want == now + playoff) return;
    int direct = want - (playoff ? 1 : 0);
    if (direct < 0) direct = 0;
    g_mem[obj + at] = (uint8_t)direct;
    if (logging())
        fprintf(stderr, "recomp: league sizes: %s (%.50s) %d: %s places %d, the class gave %d (+%d play-off)\n",
                season_change_file(obj), record_name(obj), (int16_t)rd16(g_mem, obj + COMP_YEAR), what, direct, now,
                playoff);
}

void season_sizes_rules(uint32_t obj)
{
    if (!season_game_changes() || !readable(obj)) return;
    const char *file = season_change_file(obj);
    if (!size_file(file)) return;
    const SeasonRule *r = row_now(obj, file);
    if (!r) return;
    set_places(obj, UP, UP_PLAYOFF, r->up, "promotion");
    set_places(obj, DOWN, DOWN_PLAYOFF, r->down, "relegation");
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

void o_00689c80(void);

/* league_promote_and_relegate 0x689c80, stdcall (upper, lower, expire_contracts, pairwise, expected_up,
 * expected_down), ret 0x18. It moves the rows of status 3 of the upper league down (0x689d26, 0x689e52) and the
 * rows of status 0 or 5 of the lower league up (0x689d47..0x689d4e, 0x689efd..0x689f03). In pairs (pairwise,
 * 0x689cf9) a row moves only with a partner, so the sizes stay. The expected counts (0xff: none) are compared
 * with the moves at 0x689f47 and 0x689fbb (asserts league.cpp 5596 and 5602). */
RT_WRAP(00689c80, promote_relegate)
{
    if (!season_game_changes()) {
        o_00689c80();
        return;
    }
    uint32_t upper = rt_arg(0), lower = rt_arg(1), pairwise = rt_arg(3);
    int down = rows_with(upper, STATUS_DOWN, STATUS_DOWN), up = rows_with(lower, 0, 5);
    int exp_up = (int8_t)(rt_arg(4) & 0xff), exp_down = (int8_t)(rt_arg(5) & 0xff);
    int change = down >= 0 && up >= 0 && down != up;
    /* Only between two classes of the size table, and only to counts that both hold. */
    if (change) {
        int cu = cap_of(season_change_file(upper)), cl = cap_of(season_change_file(lower));
        int nu = (int16_t)rd16(g_mem, upper + COMP_CLUBS) - down + up, nl = (int16_t)rd16(g_mem, lower + COMP_CLUBS) + down - up;
        int fu = floor_of(season_change_file(upper)), fl = floor_of(season_change_file(lower));
        if (cu <= 0 || cl <= 0 || nu > cu || nl > cl || nu < MIN_CLUBS * 4 || nl < MIN_CLUBS * 4 || nu < fu || nl < fl) {
            fprintf(stderr, "recomp: league sizes: %s %d clubs and %s %d clubs after the moves do not fit (hold %d "
                            "and %d): the clubs move in pairs\n", season_change_file(upper), nu,
                    season_change_file(lower), nl, cu, cl);
            change = 0;
        }
    }
    if (logging() || change)
        fprintf(stderr, "recomp: league sizes: %s (%.50s) down %d, %s (%.50s) up %d, pairs %u, expected %d up %d down%s\n",
                season_change_file(upper), record_name(upper), down, season_change_file(lower), record_name(lower), up,
                pairwise, exp_up, exp_down, change ? ": each club moves on its own" : "");
    if (change) {
        wr32(g_mem, cpu.esp + 4 + 4 * 3, 0);           /* pairwise off */
        if (exp_up != -1 && exp_up != up) wr32(g_mem, cpu.esp + 4 + 4 * 4, 0xffu);
        if (exp_down != -1 && exp_down != down) wr32(g_mem, cpu.esp + 4 + 4 * 5, 0xffu);
    }
    o_00689c80();
}

/* ---- News (rule kind "sizes") ---- */

static void sizes_after(const SeasonChange *c)
{
    if (!c->started || c->start || !size_file(c->file)) return;
    const SeasonRule *now = row_of_season(c->file, c->season), *before = row_of_season(c->file, c->season - 1);
    if (!now || !before || now == before) return;
    const char *name = record_name(c->comp);
    int n1 = within(c->file, now->clubs), n0 = within(c->file, before->clubs);
    if (now->clubs >= MIN_CLUBS && before->clubs >= MIN_CLUBS && n1 != n0)     /* the counts that the class holds */
        season_change_news("The %.50s now has %d clubs (%d last season).", name, n1, n0);
    if (now->up >= 0 && before->up >= 0 && now->up != before->up)
        season_change_news("%d club%s now go%s up from the %.50s (%d last season).", now->up, now->up == 1 ? "" : "s",
                           now->up == 1 ? "es" : "", name, before->up);
    if (now->down >= 0 && before->down >= 0 && now->down != before->down)
        season_change_news("%d club%s now go%s down from the %.50s (%d last season).", now->down,
                           now->down == 1 ? "" : "s", now->down == 1 ? "es" : "", name, before->down);
}

const SeasonRuleKind season_kind_sizes = {"sizes", NULL, NULL, NULL, NULL, NULL, sizes_after};
