/* The encoded league rows of the rules table (season_league.h): the lookup of a row and the meetings of a
 * league in a season (0.7.2 league sizes, slice 1). Reads only the tables: no game state. */
#include "season_league.h"
#include "season_rules.h"

#include <string.h>

const SeasonLeagueRule *season_league_rule(const char *file, int season)
{
    for (int i = 0; i < season_league_rule_count; i++)
        if (season_league_rules[i].season == season && !strcmp(season_league_rules[i].file, file))
            return &season_league_rules[i];
    return NULL;
}

int season_league_meetings(const char *file, int season)
{
    for (int s = season_rules_last_season(season, file); s >= SEASON_RULE_FIRST; s--) {
        const SeasonLeagueRule *e = season_league_rule(file, s);
        if (!e) return 0;
        if (e->meetings > 0) return e->meetings;
        const SeasonRule *r = season_rules_find(s, file, 0);
        if (!r || r->clubs != SEASON_RULE_UNKNOWN) return 0;   /* only a row with unknown clubs takes the one before */
    }
    return 0;
}
