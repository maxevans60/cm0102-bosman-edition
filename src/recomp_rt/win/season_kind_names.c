/* Rule kind "names" of the season change (season_change.h): a competition takes its name of the new season
 * from the names table of slice 15 (season_names_get, Q226, Q238). It runs in the after hook, for a
 * competition that starts its season and for one of a changing entry that waits (an international
 * competition before its next edition): a name belongs to the season, not to an edition.
 *
 * The name is the long name of the competition record (club_comp.dat or nation_comp.dat, 0x6b bytes): +4,
 * 51 bytes with the 0 at the end (docs/research/2026-10-14-game-creation-inventory.md, the record layout;
 * the rules report reads it at the same place, season_report.c). The short name (+0x38) stays: the names
 * table gives one name. A name that is the same as the record's name changes nothing and gives no news. */
#include "win.h"
#include "season_change.h"

#include <stdio.h>
#include <string.h>

#define NAME_AT 4u
#define NAME_ROOM 51u

const char *season_names_get(const char *comp_file, int season);   /* slice 15: season_names.c */

static void names_after(const SeasonChange *c)
{
    const char *name = season_names_get(c->file, c->season);
    if (!name || !*name) return;
    uint32_t rec = rd32(g_mem, c->comp + 4);
    if (rec < 0x10000u || rec >= 0xfff00000u) return;
    char old[NAME_ROOM + 1], now[NAME_ROOM];      /* 50 characters and the 0 */
    snprintf(old, sizeof old, "%.*s", (int)NAME_ROOM - 1, win_str(rec + NAME_AT));
    char text[256];
    snprintf(text, sizeof text, "%s", name);
    season_change_latin1(text);
    snprintf(now, sizeof now, "%s", text);
    if (!strcmp(old, now)) return;
    season_change_rename(rec, now);
    if (strcmp(win_str(rec + NAME_AT), now)) return;             /* not changed (not a competition record) */
    if (*old) season_change_news("%s is now called %s.", old, now);
    else season_change_news("A competition is now called %s.", now);
}

const SeasonRuleKind season_kind_names = {"names", NULL, NULL, NULL, NULL, NULL, names_after};
