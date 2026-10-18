/* A stand-in for the names table of slice 15 (0.6.0), until slice 15 is on main. Delete this file when
 * slice 15 merges: its season_names.c gives the same function (build brief 0.6.0, slices 15 and 16).
 *
 * season_names_get(comp_file, season): the name of a competition in a season, NULL when the name does not
 * change. Here it gives NULL, so a game changes no name.
 *
 * Test only: with RECOMP_SEASON_NAMES=table it gives the name of the rules table row (season_rules.h) of the
 * file and season, when that row's name differs from the name of the row of the season before. So the July
 * step of slice 16 can be tested with real changes of name before slice 15 is on main
 * (tests/seasons/season_change_run.py). */
#include "season_rules.h"

#include <stdlib.h>
#include <string.h>

const char *season_names_get(const char *comp_file, int season);

const char *season_names_get(const char *comp_file, int season)
{
    static int table = -1;
    if (table < 0) {
        const char *v = getenv("RECOMP_SEASON_NAMES");
        table = v && !strcmp(v, "table");
    }
    if (!table || !comp_file) return NULL;
    const SeasonRule *now = season_rules_find(season, comp_file, 0);
    const SeasonRule *before = season_rules_find(season - 1, comp_file, 0);
    if (!now || !before || !now->name || !before->name || !*now->name) return NULL;
    return strcmp(now->name, before->name) ? now->name : NULL;
}
