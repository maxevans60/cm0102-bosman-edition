/* The encoded league rows of the rules table (season_league.h): the lookup of a row and the meetings of a
 * league in a season (0.7.2 league sizes, slice 1). Reads only the tables: no game state. */
#include "season_league.h"
#include "season_rules.h"

#include <stdio.h>
#include <string.h>

const SeasonLeagueRule *season_league_rule(const char *file, int season)
{
    for (int i = 0; i < season_league_rule_count; i++)
        if (season_league_rules[i].season == season && !strcmp(season_league_rules[i].file, file))
            return &season_league_rules[i];
    return NULL;
}

const SeasonLeagueRule *season_league_row(const char *file, int season)
{
    for (int s = season_rules_last_season(season, file); s >= SEASON_RULE_FIRST; s--) {
        const SeasonLeagueRule *e = season_league_rule(file, s);
        if (!e) return NULL;
        if (e->halves >= 0) return e;              /* -1: a future row with unknown clubs takes the row before */
    }
    return NULL;
}

int season_league_meetings(const char *file, int season)
{
    const SeasonLeagueRule *e = season_league_row(file, season);
    return e ? e->meetings : 0;
}

/* 1 when the real season of a row begins with the year ("2011", "2010/11", "2002–03 (short season)"). */
static int real_starts(const SeasonRule *r, int year)
{
    char y[8];
    snprintf(y, sizeof y, "%d", year);
    return r && r->real_season && !strncmp(r->real_season, y, 4) &&
           (r->real_season[4] < '0' || r->real_season[4] > '9');
}

static const SeasonRule *starting_row(const char *file, int year)
{
    const SeasonRule *r = season_rules_find(year, file, 0);
    if (real_starts(r, year)) return r;
    for (int n = 0; (r = season_rules_find(year - 1, file, n)) != NULL; n++)
        if (real_starts(r, year)) return r;
    return NULL;
}

const SeasonRule *season_league_edition_row(const char *file, int year)
{
    const SeasonRule *r = starting_row(file, year);
    if (r) return r;
    int s = season_rules_last_season(year, file);
    return s ? season_rules_find(s, file, 0) : NULL;
}

int season_league_edition(const char *file, int year)
{
    const SeasonRule *r = starting_row(file, year);
    return r ? r->season : year;
}

const SeasonLeagueRule *season_league_rule_of(const char *file, const SeasonRule *r)
{
    if (!r || !file) return NULL;
    int k = 0;
    while (season_rules_find(r->season, file, k) != r) {
        if (!season_rules_find(r->season, file, k)) return NULL;
        k++;
    }
    for (int i = 0; i < season_league_rule_count; i++)
        if (season_league_rules[i].season == r->season && !strcmp(season_league_rules[i].file, file) && !k--)
            return &season_league_rules[i];
    return NULL;
}

int season_league_meetings_of(const char *file, const SeasonRule *r)
{
    if (!r) return 0;
    const SeasonLeagueRule *e = season_rules_find(r->season, file, 0) != r ? season_league_rule_of(file, r) : NULL;
    if (e && e->halves >= 0 && e->meetings > 0) return e->meetings;
    return season_league_meetings(file, r->season);
}
