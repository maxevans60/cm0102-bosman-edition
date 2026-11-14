/* The meetings that the table of J1 and of the K League plays in a season (0.7.2 league sizes, slice 14; the module
 * comment of season_sizes_asia.c). Reads only the encoded rows: no game state, so tests/seasons/test_season_sizes_asia.c
 * checks it. */
#include "season_league.h"
#include "season_sizes.h"

#include <string.h>

/* The files whose table holds both halves of a row of halves. */
static int halves_in_table(const char *file)
{
    return file && (!strcmp(file, "jap_j1") || !strcmp(file, "kor_league"));
}

int season_sizes_asia_meetings(const char *file, int season, int meetings)
{
    if (!halves_in_table(file) || meetings < 1 || meetings > 2) return meetings;
    const SeasonLeagueRule *r = season_league_row(file, season);
    return r && r->halves == 1 ? meetings * 2 : meetings;
}
