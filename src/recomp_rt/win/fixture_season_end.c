/* A moved fixture stays before its competition's season change (O288: the Danish Superliga stalled one season).
 *
 * The season change pass game_season_update_pass 0x5d5ae0 starts the competitions of a league table entry only on
 * the entry's update day (entry +0x15, the day of the year, compared with the game day at 0x5d5bc8; Denmark: day
 * 170), and only a competition that is ready then (vtable +0xc at 0x5d5ccb; a league is ready when every row has
 * played its matches, League::check_complete 0x689ad0, 0x689b3d..0x689b58). A competition that is not ready waits for
 * the update day of the next year, and so runs one season late from then on.
 *
 * A fixture gets a new date in 0x5ab510 (no name in the function map; thiscall (fixture) ret 4; year at
 * fixture +0x28, day at +0x2a, slot at +0x3f): from FIXTURE_MANAGER::reschedule_club_fixtures_near_date 0x5abb10
 * (0x5abbd2), which NationalTeams::reschedule_clashing_club_fixtures 0x7777e0 calls for a club whose players are
 * called up (0x77795c), and from the clash check of 0x5aa680. It looks for a free day up to 20 days after the old date
 * (0x5ab6be..), else takes today + 2, with no look at the season's end. In the base game the leagues end in May, well
 * before their update days. A season game has international windows in June each year (the Nations League and the
 * qualifiers of slices 23, 28 and 29): in a 2021 Select All game FC Copenhagen's last Superliga match of 2022/23 was
 * moved on days 148, 152, 155 and 156 and came after day 170, so the Superliga was not complete on its update day and
 * started 2023/24 a year late (gate (a) run 7; Denmark alone has too few call-ups to show it).
 *
 * So in a season game a new date that takes a fixture from before its competition's update day to that day or later
 * in the same year is not taken: the fixture keeps its date, and the club plays without its called-up players, as a
 * league plays at the end of its season. The clash check of 0x5aa680 tries at most 10 times (0xae2a40 < 10) and then
 * places the fixture. The base game and RECOMP_SEASON_CODE=off keep the original (Q152). */
#include "win.h"
#include "../rt.h"
#include "season_change.h"
#include "season_names.h"

#include <stdio.h>

#define ENTRIES 0x00b63d60u     /* league table entries, 0x48 bytes each (0x5d5bc8) */
#define ENTRY_SIZE 0x48u
#define ENTRY_COUNT 35
#define ENTRY_UPDATE_DAY 0x15u
#define COMP_OBJECTS 0x00adadfcu
#define FIX_COMP 0x14u          /* the competition's record */
#define FIX_YEAR 0x28u
#define FIX_DAY 0x2au
#define FIX_SLOT 0x3fu

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* The update day of the entry that holds the competition of record rec, or -1. */
static int update_day(uint32_t rec)
{
    uint32_t table = rd32(g_mem, COMP_OBJECTS);
    if (!readable(rec) || !readable(table)) return -1;
    int32_t id = (int32_t)rd32(g_mem, rec);
    if (id < 0 || id > 0xffff) return -1;
    uint32_t obj = rd32(g_mem, table + 4u * (uint32_t)id);
    if (!readable(obj)) return -1;
    uint32_t comps[128];
    for (int e = 0; e < ENTRY_COUNT; e++) {
        int n = season_change_entry_comps(e, comps, 128);
        for (int i = 0; i < n; i++)
            if (comps[i] == obj) return (int16_t)rd16(g_mem, ENTRIES + ENTRY_SIZE * (uint32_t)e + ENTRY_UPDATE_DAY);
    }
    return -1;
}

void o_005ab510(void);

RT_WRAP(005ab510, fixture_new_date)
{
    uint32_t fix = rt_arg(0);
    if (!season_game_changes() || !readable(fix)) {
        o_005ab510();
        return;
    }
    int year = (int16_t)rd16(g_mem, fix + FIX_YEAR), day = (int16_t)rd16(g_mem, fix + FIX_DAY);
    uint8_t slot = g_mem[fix + FIX_SLOT];
    o_005ab510();
    if (rt_unwinding) return;
    int new_year = (int16_t)rd16(g_mem, fix + FIX_YEAR), new_day = (int16_t)rd16(g_mem, fix + FIX_DAY);
    if (new_year != year || new_day <= day) return;
    uint32_t rec = rd32(g_mem, fix + FIX_COMP);
    int last = update_day(rec);
    if (last < 0 || day >= last || new_day < last) return;
    wr16(g_mem, fix + FIX_YEAR, (uint16_t)year);
    wr16(g_mem, fix + FIX_DAY, (uint16_t)day);
    g_mem[fix + FIX_SLOT] = slot;
    fprintf(stderr, "recomp: fixtures: a match of %.40s stays on day %d of %d (the new day %d is on or after its "
                    "season change, day %d)\n", win_str(rec + 4), day, year, new_day, last);
}
