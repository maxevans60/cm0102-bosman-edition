/* The lookup of the real round names (season_round_names.h). It reads only the table, so the ctest
 * season_round_names links it without the game. */
#include "season_round_names.h"

#include <string.h>

const char *season_round_name(const char *comp_file, int round_code, int season, int which_set)
{
    const SeasonRoundName *best = NULL;
    if (!comp_file) return NULL;
    for (int i = 0; i < season_round_name_count; i++) {
        const SeasonRoundName *r = &season_round_name_rows[i];
        if (r->round_code != round_code || r->first_season > season || !(r->sets & which_set) ||
            strcmp(r->comp_file, comp_file))
            continue;
        if (!best || r->first_season > best->first_season) best = r;
    }
    return best ? best->name : NULL;
}
