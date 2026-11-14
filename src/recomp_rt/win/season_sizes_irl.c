/* The League of Ireland First Division at its real count (0.7.2 league sizes, slice 13; spec
 * docs/specs/2026-11-13-league-sizes-0.7.2-spec.md). IreFirst counts the clubs of its data (tools/recomp/league_sizes.py
 * DATA_COUNT), so the engine gives it the template of that count and the row's meetings (season_sizes.c, the wrap of
 * create_schedule 0x634f40), and these two values let IreFirst::create_table 0x635910 take them:
 * - the least count: the original takes only 9 to 12 (cmp ax, 9 at 0x63596f; cmp ax, 0xc at 0x635979) and returns
 *   0 for any other count, and the constructor stops at its box (ire_first.cpp, 0x6359e9). The 2012 to 2017 seasons
 *   have 8 clubs;
 * - the meetings: the original sets 4 for 9 or 10 clubs and 3 for 11 or 12 (0x635983..0x635997), after
 *   set_up_rules 0x635aa0 called create_schedule (0x635b21), whose template the engine made for the row's meetings.
 *   An 8-club season would get 3 meetings on a template of 4.
 * Outside a season game with the sizes on, and in a season whose template had no room for the count, both return the
 * original's value. */
#include "win.h"
#include "season_sizes.h"

#define COMP_MEETINGS 0x3cu     /* s16: meetings of each pair */
#define LEAST_CLUBS 2u          /* season_sizes.c MIN_CLUBS: the least count that the engine plans */

uint32_t rt_ire_first_least(uint32_t obj, uint32_t original)
{
    return season_sizes_planned(obj) ? LEAST_CLUBS : original;
}

uint32_t rt_ire_first_meetings(uint32_t obj, uint32_t original)
{
    if (!season_sizes_planned(obj)) return original;
    int m = (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
    return m >= 1 && m <= 4 ? (uint32_t)m : original;
}
