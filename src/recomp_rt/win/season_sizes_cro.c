/* The Croatian second tier at 18 clubs (0.7.2 league sizes, slice 11, D22; spec
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
 * The regroup of the second tier. CroA1::start_new_season 0x5149a0 calls 0x515190 (at 0x5149c2, before the edition
 * +0x40 goes up at 0x514a08). It puts every club of the North [0x9cf8a8] and South [0x9cf8ac] divisions in a stack
 * array of 32 at esp+0x14 (store 0x5151ff, at most 32: 0x5151fc), sorts them by the latitude of their city (0x945333
 * at 0x51521f), and gives the first 16 the North and the others the South (0x515231..0x515238), for 32 slots
 * (0x5152bb) whatever it found. The real second tier is one division from 2006/07 (the South row of the rules table
 * is "not held"), of 12 to 18 clubs. In those seasons of a season game the regroup gives the North the count of its
 * row (season_sizes_clubs, the edition after the one of the First Division object) and the South the rest, and it
 * walks the clubs it found, in a list of the runtime of REGROUP_MAX slots. The South keeps at most the 16 rows of
 * its table (CroA2b::create_table 0x516dd0 stores 16 at 0x516de2 and stops filling at the count, 0x516e9b; cro_a2b
 * is not a grown class): with a North of 12 to 15 the clubs after North + 16 in the sort (the southernmost) go to A
 * Lower Division [0x9cf764], the spare division that the club moves and the size step take clubs from
 * (season_clubs.c), through the same store and news of the regroup (0x5152be, 0x515251..0x5152a1: the news reads
 * the target's id, so the target is a record). In 2001/02 to 2005/06 the two groups play as now, 16 each (D16).
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

static uint32_t list;              /* the list in the game's memory, made at the first table of more than 16 */
static uint32_t regroup;           /* the list of the regroup, made at the first regroup of a season game */
static int regroup_found;          /* the clubs that the regroup found */
static int regroup_north;          /* the North's clubs of this regroup, 0 for the original's 16 */

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


/* The edition that the regroup prepares: the one after the First Division's, or the start season's with changing
 * rules off (the sizes of the start season stay, Q232). */
static int regroup_year(void)
{
    if (!season_changing_rules()) return season_start_year();
    uint32_t leagues = rd32(g_mem, LEAGUES), a1 = readable(leagues) ? rd32(g_mem, leagues + rd32(g_mem, CRO_A1) * 4u) : 0;
    return readable(a1) ? (int16_t)rd16(g_mem, a1 + COMP_YEAR) + 1 : season_start_year();
}

/* The North's count of a regroup of found clubs (0: the original's). */
static int north_of(int found)
{
    int year = regroup_year();
    if (season_sizes_clubs("cro_a2b", year) >= 2) return 0;   /* two groups (D16) */
    int north = season_sizes_clubs("cro_a2a", year);
    if (north < 2) return 0;
    if (north > found) north = found;
    int south = found - north > SOUTH_ROWS ? SOUTH_ROWS : found - north;
    if (getenv("RECOMP_SIZE_LOG"))
        fprintf(stderr, "recomp: league sizes: cro_a2a regroup %d: %d clubs, %d North, %d South, %d to A Lower "
                        "Division\n", year, found, north, south, found - north - south);
    return north;
}

/* The array for the sort, with the clubs found (0x515219), and for the walk, count -1 (0x51522d). */
uint32_t rt_cro_regroup_list(uint32_t array, uint32_t count)
{
    if (!regroup_on()) return array;
    if ((int32_t)count >= 0) {
        regroup_found = (int32_t)count;
        regroup_north = north_of(regroup_found);
    }
    return regroup;
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
 * chose (ebx), or A Lower Division for a place after North + 16. */
uint32_t rt_cro_regroup_target(uint32_t chosen, uint32_t place)
{
    if (!regroup_on() || regroup_north <= 0 || (int32_t)place < regroup_north + SOUTH_ROWS) return chosen;
    int32_t key = (int32_t)rd32(g_mem, KEY_LOWER);
    uint32_t comps = rd32(g_mem, COMPS);
    return key >= 0 && readable(comps) ? comps + (uint32_t)key * COMP_SIZE : chosen;
}
