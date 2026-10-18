/* The lookups of the UEFA formats table (season_uefa.h). It reads only the table and the season of the game
 * (season.h), so the ctest season_uefa links it with stand-ins of those three functions. */
#include "season.h"
#include "season_uefa.h"

#include <string.h>

const SeasonUefa *season_uefa_table_row(int season)
{
    if (season_uefa_row_count <= 0) return NULL;
    const SeasonUefa *best = &season_uefa_rows[0];
    for (int i = 0; i < season_uefa_row_count; i++)
        if (season_uefa_rows[i].season <= season) best = &season_uefa_rows[i];
    return best;
}

const SeasonUefa *season_uefa_row(int season)
{
    if (!season_code_on()) return NULL;
    int start = season_start_year();
    int changing = season_changing_rules();
    if (start == SEASON_FIRST_YEAR && !changing) return NULL;    /* the base game */
    return season_uefa_table_row(changing ? season : start);
}

static int round_clubs(const SeasonUefaRound *rs, int n, const char *name)
{
    for (int i = 0; i < n && i < SEASON_UEFA_ROUNDS; i++)
        if (rs[i].name && !strcmp(rs[i].name, name)) return rs[i].clubs;
    return 0;
}

int season_uefa_cl_round(const SeasonUefa *r, const char *name)
{
    return r ? round_clubs(r->cl_rounds, r->cl_round_count, name) : 0;
}

int season_uefa_uc_round(const SeasonUefa *r, const char *name)
{
    return r ? round_clubs(r->uc_rounds, r->uc_round_count, name) : 0;
}

int season_uefa_cl_round_of_16(const SeasonUefa *r)
{
    return r && season_uefa_cl_round(r, "Groups") == 32 && season_uefa_cl_round(r, "Groups 2") == 0 &&
           season_uefa_cl_round(r, "KO") == 16;
}
