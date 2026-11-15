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
 *   A row with no meetings value (2002/03, the unknown future rows) takes the class's meetings for the count
 *   (season_sizes_irl_class_meetings), so the template and the table agree with the original.
 * Outside a season game with the sizes on both return the original's value; the meetings also in a season whose
 * template had no room for the count. */
#include "win.h"
#include "season_sizes.h"

#include <string.h>

#define COMP_MEETINGS 0x3cu     /* s16: meetings of each pair */
#define MOST_PLACES 30          /* season_sizes.c set_places: the most places of a row */
#define LEAST_CLUBS 2u          /* season_sizes.c MIN_CLUBS: the least count that the engine plans */

uint32_t rt_ire_first_least(uint32_t obj, uint32_t original)
{
    /* Not only a planned template: the club moves (season_clubs.c data_class_count) leave fewer than 9 whenever the
     * sizes are on, and a season whose template had no room must still make its table. */
    (void)obj;
    return season_sizes_on() ? LEAST_CLUBS : original;
}

uint32_t rt_ire_first_meetings(uint32_t obj, uint32_t original)
{
    if (!season_sizes_planned(obj)) return original;
    int m = (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
    return m >= 1 && m <= 4 ? (uint32_t)m : original;
}

int season_sizes_irl_class_meetings(const char *file, int n, int row_meetings)
{
    if (!file || strcmp(file, "ire_first") || row_meetings) return 0;
    return n == 9 || n == 10 ? 4 : 3;
}

/* Gap 43: IrePrm::create_table 0x63a740 sets 3 relegation places for any table of 12 clubs (mov byte [esi + 0xc1], 3
 * at 0x63a8b5), and IrePrm::set_up_rules 0x63a900 sets 1 and a play-off place (0x63a957, 0x63a95d). The rows of the
 * Premier Division give the direct places in "down" and the play-off place apart ("1 (pos 12)" and "pos 11:
 * relegation play-off" in 2011 to 2015). With the sizes on, the 12-club branch takes the row's direct places, so in
 * 2004 to 2006 and 2011 to 2015 the 12th goes down and the 11th plays off, as in real life. The play-off place
 * (0x63a8ae) stays the class's 1, also in the 12-club rows with no play-off (2007, 2016: open item). set_places runs in
 * the set_up_rules wrap, before create_table (0x639c3c then 0x639c77; vtable +0x8c at 0x63a9ff then 0x63aa3d), so for a
 * 12-club table this store decides +0xc1. */
uint32_t rt_ire_prm_down(uint32_t obj, uint32_t original)
{
    if (!season_sizes_on()) return original;
    int down = season_sizes_row_down(obj, "ire_prm");
    return down >= 0 && down <= MOST_PLACES ? (uint32_t)down : original;
}
