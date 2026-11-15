/* The Croatian second tier at 18 clubs and the Greek divisions at a new game (0.7.2 league sizes, slice 11, D22; spec
 * docs/specs/2026-11-13-league-sizes-0.7.2-spec.md).
 *
 * CroA2a::create_tiebreak_playoff_stage 0x5160f0 runs when the table is complete. It walks the rows of the table
 * (+0x3e, 0x516114..0x516210) and puts both clubs of each pair of neighbours on the same points (+0x19 at 0x51614a,
 * 0x51614f) into a stack array of 16 pointers at esp+0x1c (stores 0x51616a and 0x516190; the next local, a text
 * buffer of the error path, is at esp+0x5c, 0x5162c4). It passes the array to Cup_Stage::Cup_Stage 0x522e00
 * (lea at 0x51629a, the count in ebp at 0x5162a0), which makes its own list of that size (0x52310a..0x523131). A
 * table of 18 can give 9 tied pairs, 18 pointers: two past the array.
 *
 * In a season game with a table of more than 16 rows the pointers go to a list in the game's memory of
 * TIEBREAK_MAX slots, made once. Each pointer of the walk takes the slot of its index, as the stack array, and
 * Cup_Stage reads the list. The list lives while the stage copies it (the call at 0x5162a4); the walk of the next
 * tie-break writes it again from slot 0. The base game, RECOMP_LEAGUE_SIZES=off and a table of 16 or fewer use the
 * stack array of the original (Q152).
 *
 * The regroup of the second tier. CroA1::start_new_season 0x5149a0 runs the chain 0x5150f0 (vtable +0xb0 at
 * 0x5149b3: the First Division's exchange with the North at 0x515158 and with the South at 0x515180), then 0x514ed0
 * (4 clubs of the lower divisions up to the North, the relegated rows of both groups to A Lower Division), then
 * 0x515190 (at 0x5149c2, before the edition +0x40 goes up at 0x514a08). 0x515190 puts every club of the North
 * [0x9cf8a8] and South [0x9cf8ac] divisions in a stack array of 32 at esp+0x14 (store 0x5151ff, at most 32:
 * 0x5151fc), sorts them by the latitude of their city (0x945333 at 0x51521f, comparator 0x53fd40), and gives the
 * first 16 the North and the others the South (0x515231..0x515238), for 32 slots (0x5152bb) whatever it found. In a
 * season game the regroup walks the clubs it found, in a list of the runtime of REGROUP_MAX slots, and takes the
 * rows of the season it prepares (the edition after the First Division's):
 * - Two groups (2001/02 to 2005/06; gap row 38): the North takes the cro_a2a row and the South the cro_a2b row (12
 *   each from 2002/03), by latitude as the original; the clubs that a smaller tier leaves out are the last by
 *   results of both groups.
 * - One group (the South row "not held", 2006/07 on; gap row 40): the real second tier is one division of 12 to 18
 *   clubs, the Druga HNL, which keeps its clubs by results. The North takes the cro_a2a row in order of results
 *   (sort_by_results, no sort by latitude): the clubs down from the First Division, the North's by their places,
 *   then the South's by theirs. The South stands for the third level (3. HNL, regional groups of 16 to 18): it
 *   keeps the 16 rows of its table (CroA2b::create_table 0x516dd0 stores 16 at 0x516de2; cro_a2b is not a grown
 *   class; when fewer are left the clubs of the Croatian lower divisions fill it, fill_south), its best clubs
 *   fill the North's free places, and it does not go up to the First Division
 *   (rt_cro_south_swap: no exchange at 0x515180 after a season of one group).
 * The clubs after North + South in the order go to A Lower Division [0x9cf764], the spare division that the club
 * moves and the size step take clubs from (season_clubs.c), through the same store and news of the regroup
 * (0x5152be, 0x515251..0x5152a1: the news reads the target's id, so the target is a record).
 * The base game and RECOMP_LEAGUE_SIZES=off keep the original. */
#include "win.h"
#include "../rt_host.h"
#include "season.h"
#include "season_sizes.h"

#include <stdio.h>
#include <stdlib.h>

#define COMP_CLUBS 0x3eu           /* s16: the rows of the table */
#define ARRAY_SLOTS 16             /* the stack array of the original (esp+0x1c..esp+0x5c) */
#define TIEBREAK_MAX 32            /* more than the largest cap of the class (league_sizes.py GROWN cro_a2a 18) */
#define REGROUP_MAX 64             /* North at its cap 18 and South at its class count 16 is 34 */
#define LEAGUES 0x00adadfcu        /* the league objects by division index (0x5150f0) */
#define CRO_A1 0x009cf8a4u         /* the division index of the First Division */
#define KEY_LOWER 0x009cf764u      /* the division index of A Lower Division (season_clubs.c) */
#define COMPS 0x00ae23d0u          /* the division records, 0x6b bytes each (0x5151b2..0x5151d2) */
#define COMP_SIZE 0x6bu
#define SOUTH_ROWS 16              /* CroA2b's table (0x516de2) */
#define COMP_YEAR 0x40u
#define COMP_TABLE 0xb1u           /* the table: 0x3b bytes a row, the club at +0 */
#define ROW_SIZE 0x3bu
#define ROW_POS 0x39u              /* s8: the place in the table, from 1 */
#define CRO_A2A 0x009cf8a8u        /* the division index of the North */
#define CRO_A2B 0x009cf8acu        /* the division index of the South */

static uint32_t list;              /* the list in the game's memory, made at the first table of more than 16 */
static uint32_t regroup;           /* the list of the regroup, made at the first regroup of a season game */
static int regroup_found;          /* the clubs that the regroup found */
static int regroup_north;          /* the North's clubs of this regroup, 0 for the original's 16 */
static int regroup_south;          /* the South's clubs of this regroup */
static int regroup_by_results;     /* 1: one group, the order by results */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int uses_list(uint32_t obj)
{
    if (!season_sizes_on() || !readable(obj)) return 0;
    int rows = (int16_t)rd16(g_mem, obj + COMP_CLUBS);
    if (rows <= ARRAY_SLOTS || rows > TIEBREAK_MAX) return 0;
    if (!list) list = rt_alloc_zero(TIEBREAK_MAX * 4u);
    return list != 0;
}

uint32_t rt_cro_tiebreak_slot(uint32_t obj, uint32_t array, uint32_t index)
{
    int i = (int16_t)(index & 0xffffu);
    if (uses_list(obj) && i >= 0 && i < TIEBREAK_MAX) return list + (uint32_t)i * 4u;
    return array + index * 4u;
}

uint32_t rt_cro_tiebreak_list(uint32_t obj, uint32_t array)
{
    return uses_list(obj) ? list : array;
}

/* 1 when the regroup of a season game uses the runtime list (made once). */
static int regroup_on(void)
{
    if (!season_sizes_on()) return 0;
    if (!regroup) regroup = rt_alloc_zero(REGROUP_MAX * 4u);
    return regroup != 0;
}

uint32_t rt_cro_regroup_max(uint32_t original)
{
    return regroup_on() ? REGROUP_MAX : original;
}

uint32_t rt_cro_regroup_slot(uint32_t array, uint32_t index)
{
    int i = (int16_t)(index & 0xffffu);
    if (regroup_on() && i >= 0 && i < REGROUP_MAX) return regroup + (uint32_t)i * 4u;
    return array + index * 4u;
}


/* The season of the rules: the edition of the First Division plus offset (0: the season that ends, 1: the one that
 * the regroup prepares), or the start season's with changing rules off (the sizes of the start season stay, Q232). */
static int rules_year(int offset)
{
    if (!season_changing_rules()) return season_start_year();
    uint32_t leagues = rd32(g_mem, LEAGUES), a1 = readable(leagues) ? rd32(g_mem, leagues + rd32(g_mem, CRO_A1) * 4u) : 0;
    return readable(a1) ? (int16_t)rd16(g_mem, a1 + COMP_YEAR) + offset : season_start_year();
}

/* 1 when the second tier of a season is one group: the South row is not held (2006/07 on). */
static int one_group(int year)
{
    return season_sizes_clubs("cro_a2b", year) < 2 && season_sizes_clubs("cro_a2a", year) >= 2;
}

static uint32_t league_at(uint32_t key)
{
    uint32_t leagues = rd32(g_mem, LEAGUES);
    if (!readable(leagues)) return 0;
    uint32_t obj = rd32(g_mem, leagues + rd32(g_mem, key) * 4u);
    return readable(obj) ? obj : 0;
}

/* The rank of a club in the last table of a league (from 1), or 0 when the table does not hold it. The rank is the
 * row's index: League::update_positions_and_status 0x684cd0 sorts the table (0x6827d0 at 0x684d18) at each match
 * day; row +0x39 is the best place a club can still reach, which level clubs share (season_level_ties_game.c). */
static int place_in(uint32_t obj, uint32_t club)
{
    uint32_t t = obj ? rd32(g_mem, obj + COMP_TABLE) : 0;
    int n = obj ? (int16_t)rd16(g_mem, obj + COMP_CLUBS) : 0;
    if (!readable(t)) return 0;
    for (int r = 0; r < n && r < REGROUP_MAX; r++)
        if (rd32(g_mem, t + (uint32_t)r * ROW_SIZE) == club) return r + 1;
    return 0;
}

#define KEY_NEW 300                /* a club that 0x514ed0 brought up from the lower divisions */

/* The order of the found clubs by results. The chain 0x5150f0 and 0x514ed0 run before the regroup, so the tables of
 * the season that ends are still there (League::free_table 0x687970 of the First Division is at 0x5149ca; neither
 * 0x6830b0 nor 0x6831a0 rewrites a table row). First the clubs down from the First Division, then:
 * - one group: the North's clubs by rank, then the South's (the South holds the third level, so its best fill the
 *   North's free places);
 * - two groups: the clubs of both groups by rank together (the 5th of the North and the 5th of the South side by
 *   side), so the clubs that a smaller tier sends to A Lower Division are the last of both groups.
 * Then the clubs that 0x514ed0 brought up from the lower divisions (KEY_NEW; with one group they move up to the
 * South's part in plan, so the South's last clubs go down rather than these). */
static int merit(uint32_t club, uint32_t a1, uint32_t north, uint32_t south, int one)
{
    int p;
    if ((p = place_in(a1, club)) > 0) return p;
    if ((p = place_in(north, club)) > 0) return 100 + p;
    if ((p = place_in(south, club)) > 0) return (one ? 200 : 100) + p;
    return KEY_NEW;
}

static void sort_by_results(int found, int one, int north_count)
{
    uint32_t a1 = league_at(CRO_A1), north = league_at(CRO_A2A), south = league_at(CRO_A2B);
    int key[REGROUP_MAX];
    for (int i = 0; i < found; i++) {
        uint32_t club = rd32(g_mem, regroup + (uint32_t)i * 4u);
        int k = merit(club, a1, north, south, one), j = i;
        while (j > 0 && key[j - 1] > k) {          /* stable: the club table's order within a key */
            key[j] = key[j - 1];
            wr32(g_mem, regroup + (uint32_t)j * 4u, rd32(g_mem, regroup + (uint32_t)(j - 1) * 4u));
            j--;
        }
        key[j] = k;
        wr32(g_mem, regroup + (uint32_t)j * 4u, club);
    }
    if (!one) return;
    /* One group: the new clubs go just after the North, at the head of the South's part. */
    uint32_t moved[REGROUP_MAX];
    int nm = 0, at = north_count;
    for (int i = north_count; i < found; i++)
        if (key[i] == KEY_NEW) moved[nm++] = rd32(g_mem, regroup + (uint32_t)i * 4u);
    for (int i = found - 1, w = found - 1; i >= north_count; i--)
        if (key[i] != KEY_NEW) wr32(g_mem, regroup + (uint32_t)w-- * 4u, rd32(g_mem, regroup + (uint32_t)i * 4u));
    for (int i = 0; i < nm; i++) wr32(g_mem, regroup + (uint32_t)(at + i) * 4u, moved[i]);
}

/* The South of a season of one group, when the North took so many that fewer than its 16 are left (2006/07 after
 * groups of 12): the clubs of the Croatian lower divisions take its free places, as 0x514ed0 takes its 4 clubs
 * for the North: the Croatian clubs (club +0x53, the nation's id against [0x9cf2ac], 0x514f98..0x514fa9) of A Lower
 * Division and the divisions [0x9cf888], [0x9cf890], [0x9cf88c] and [0x9cf894] (0x514fb2..0x514fdc), picked by
 * 0x4afbd0 (cdecl: list, count, wanted; 0x515003), at the end of the regroup's list. Returns the clubs added. */
#define CLUBS 0x00ae23b4u
#define CLUB_COUNT 0x00ae235cu
#define CLUB_SIZE 0x245u
#define CLUB_NATION 0x53u
#define CLUB_DIVISION 0x57u
#define CRO_NATION 0x009cf2acu
#define F_PICK 0x004afbd0u
#define POOL_MAX 1024
static const uint32_t LOWER_KEYS[] = {KEY_LOWER, 0x009cf888u, 0x009cf890u, 0x009cf88cu, 0x009cf894u};
static uint32_t pool;

static int fill_south(int found, int need)
{
    uint32_t clubs = rd32(g_mem, CLUBS);
    int count = (int)rd32(g_mem, CLUB_COUNT), n = 0;
    if (need <= 0 || found + need > REGROUP_MAX || !readable(clubs)) return 0;
    if (!pool) pool = rt_alloc_zero(POOL_MAX * 4u);
    if (!pool) return 0;
    for (int i = 0; i < count && n < POOL_MAX; i++) {
        uint32_t club = clubs + (uint32_t)i * CLUB_SIZE, nation = rd32(g_mem, club + CLUB_NATION),
                 rec = rd32(g_mem, club + CLUB_DIVISION);
        if (!readable(nation) || !readable(rec) || rd32(g_mem, nation) != rd32(g_mem, CRO_NATION)) continue;
        int lower = 0;
        for (unsigned k = 0; k < sizeof LOWER_KEYS / sizeof LOWER_KEYS[0]; k++)
            lower |= rd32(g_mem, rec) == rd32(g_mem, LOWER_KEYS[k]);
        if (lower) wr32(g_mem, pool + 4u * (uint32_t)n++, club);
    }
    if (need > n) need = n;
    if (need <= 0) return 0;
    uint32_t a[3] = {pool, (uint32_t)n, (uint32_t)need};
    Cpu saved = cpu;
    rt_callback(F_PICK, 3, a);
    if (rt_unwinding) return 0;
    cpu = saved;
    for (int i = 0; i < need; i++)
        wr32(g_mem, regroup + (uint32_t)(found + i) * 4u, rd32(g_mem, pool + 4u * (uint32_t)i));
    return need;
}

/* The counts of a regroup of found clubs: the North takes the cro_a2a row, the South the cro_a2b row (two groups,
 * 2001/02 to 2005/06) or its table's 16 (one group, 2006/07 on: the South holds the third level), the others go to
 * A Lower Division. The order is by results (sort_by_results); with two groups the original's sort by latitude then
 * orders the clubs kept.
 * No cro_a2a row: the original's 16 and 16. */
static void plan(int found)
{
    regroup_north = regroup_south = regroup_by_results = 0;
    int year = rules_year(1), north = season_sizes_clubs("cro_a2a", year), south = season_sizes_clubs("cro_a2b", year);
    if (north < 2) return;
    if (south < 2) {
        south = SOUTH_ROWS;
        regroup_by_results = 1;
    }
    if (north > found) north = found;
    if (south > found - north) south = found - north;
    int added = regroup_by_results ? fill_south(found, SOUTH_ROWS - south) : 0;
    regroup_found = found += added;
    south += added;
    regroup_north = north;
    regroup_south = south;
    if (getenv("RECOMP_SIZE_LOG"))
        fprintf(stderr, "recomp: league sizes: cro_a2a regroup %d: %d clubs, %d North, %d South, %d to A Lower "
                        "Division, %s, %d from the lower divisions\n", year, found, north, south,
                found - north - south, regroup_by_results ? "by results" : "by latitude", added);
}

/* The array for the sort, with the clubs found (0x515219), and for the walk, count -1 (0x51522d). */
uint32_t rt_cro_regroup_list(uint32_t array, uint32_t count)
{
    if (!regroup_on()) return array;
    if ((int32_t)count >= 0) {
        regroup_found = (int32_t)count;
        plan(regroup_found);
        /* By results always; with two groups the original's sort by latitude then orders the clubs kept (the
         * first North + South, rt_cro_regroup_sort_count), and those after them go to A Lower Division. */
        if (regroup_north > 0) sort_by_results(regroup_found, regroup_by_results, regroup_north);
    }
    return regroup;
}

/* The count of the original's sort by latitude (0x51521d): none with one group (the order is by results), the
 * clubs kept with two groups (North + South, the first of the order by results), all without a row. */
uint32_t rt_cro_regroup_sort_count(uint32_t count)
{
    if (!regroup_on() || regroup_north <= 0) return count;
    return regroup_by_results ? 0u : (uint32_t)(regroup_north + regroup_south);
}

uint32_t rt_cro_regroup_north(uint32_t original)
{
    return regroup_on() && regroup_north > 0 ? (uint32_t)regroup_north : original;
}

uint32_t rt_cro_regroup_end(uint32_t original)
{
    return regroup_on() ? (uint32_t)regroup_found : original;
}

/* The division of the club at a place of the walk (0x51523a, place [esp+0x10]): the North or South that the original
 * chose (ebx), or A Lower Division for a place after North + South. */
uint32_t rt_cro_regroup_target(uint32_t chosen, uint32_t place)
{
    if (!regroup_on() || regroup_north <= 0 || (int32_t)place < regroup_north + regroup_south) return chosen;
    int32_t key = (int32_t)rd32(g_mem, KEY_LOWER);
    uint32_t comps = rd32(g_mem, COMPS);
    return key >= 0 && readable(comps) ? comps + (uint32_t)key * COMP_SIZE : chosen;
}

/* The exchange of the South with the First Division (0x689c80 at 0x515180): none in a season game when the season
 * that ends had one group (the South was not held, so its winner does not go up). 1: the original's call. */
uint32_t rt_cro_south_swap(void)
{
    if (!season_sizes_on()) return 1;
    int year = rules_year(0);
    if (!one_group(year)) return 1;
    if (getenv("RECOMP_SIZE_LOG"))
        fprintf(stderr, "recomp: league sizes: cro_a2b %d: not held, no exchange with the First Division\n", year);
    return 0;
}

/* The count that fix_division_club_count 0x839920 gives a Greek division at a new game (0x8283c0 National A,
 * 0x8283ce National B; the original pushes 14): in a season game with the sizes on, the row of the start season
 * (season_sizes_clubs: the first row of the file, the National B Division for gre_second). The base game,
 * RECOMP_LEAGUE_SIZES=off and a season with no row keep 14. */
uint32_t rt_gre_fix_count(uint32_t original, uint32_t second)
{
    const char *file = second ? "gre_second" : "gre_prm";
    int n = season_sizes_clubs(file, season_start_year());
    if (n < 2) return original;
    if (getenv("RECOMP_SIZE_LOG"))
        fprintf(stderr, "recomp: league sizes: %s %d: the new game fixes the division at %d clubs (the original %u)\n",
                file, season_start_year(), n, original);
    return (uint32_t)n;
}
