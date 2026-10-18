/* Rule kind "names" of the season change (season_change.h): a competition takes its name of the new season
 * from the names table of slice 15 (season_names_get, Q226, Q238). It runs in the after hook, for a
 * competition that starts its season and for one of a changing entry that waits (an international
 * competition before its next edition): a name belongs to the season, not to an edition.
 *
 * The name is the long name of the competition record (club_comp.dat or nation_comp.dat, 0x6b bytes): +4,
 * 51 bytes with the 0 at the end (docs/research/2026-10-14-game-creation-inventory.md, the record layout;
 * the rules report reads it at the same place, season_report.c), and the short name at +0x38. A name that is
 * the same as the record's name changes nothing and gives no news. The renames go through
 * season_names_rename_object (season_names.c), which calls season_change_rename for the long name. */
#include "win.h"
#include "season_change.h"
#include "season_names.h"

#include <stdio.h>
#include <string.h>

/* The object number matters: a file with two objects (ger_regional: North and South) has a name for each. */
static void names_after(const SeasonChange *c)
{
    char old[64], now[64];
    if (!season_names_rename_object(c->comp, c->season, old, sizeof old)) return;
    uint32_t rec = rd32(g_mem, c->comp + 4);
    snprintf(now, sizeof now, "%.50s", win_str(rec + 4));
    if (!strcmp(old, now)) return;                  /* only the short name changed: no news */
    if (*old) season_change_news("%s is now called %s.", old, now);
    else season_change_news("A competition is now called %s.", now);
}

const SeasonRuleKind season_kind_names = {"names", NULL, NULL, NULL, NULL, NULL, names_after};
