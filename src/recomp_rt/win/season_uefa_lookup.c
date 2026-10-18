/* The lookups of the UEFA formats table (season_uefa.h). It reads only the table, so the ctest season_uefa
 * links it without the game. */
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

static int round_clubs(const SeasonUefaRound *rs, int n, const char *name)
{
    for (int i = 0; i < n && i < SEASON_UEFA_ROUNDS; i++)
        if (rs[i].name && !strcmp(rs[i].name, name)) return rs[i].clubs;
    return 0;
}

static int level_clubs(const SeasonUefaRound *rs, int n, const char *name)
{
    size_t k = strlen(name);
    int sum = 0;
    for (int i = 0; i < n && i < SEASON_UEFA_ROUNDS; i++)
        if (rs[i].name && !strncmp(rs[i].name, name, k) && (rs[i].name[k] == 0 || rs[i].name[k] == ' '))
            sum += rs[i].clubs;
    return sum;
}

int season_uefa_cl_level(const SeasonUefa *r, const char *name)
{
    return r ? level_clubs(r->cl_rounds, r->cl_round_count, name) : 0;
}

int season_uefa_uc_level(const SeasonUefa *r, const char *name)
{
    return r ? level_clubs(r->uc_rounds, r->uc_round_count, name) : 0;
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
