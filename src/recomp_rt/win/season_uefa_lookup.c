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

/* The season of the access list for S: S itself, the last season after the table, -1 before it. */
static int access_season(int season)
{
    int first = 1 << 30, last = -1;
    for (int i = 0; i < season_uefa_access_count; i++) {
        if (season_uefa_access[i].season < first) first = season_uefa_access[i].season;
        if (season_uefa_access[i].season > last) last = season_uefa_access[i].season;
    }
    if (last < 0 || season < first) return -1;
    return season > last ? last : season;
}

const char *season_uefa_access_places(int season, char comp, int rank)
{
    int s = access_season(season);
    const char *best = NULL;
    int best_from = 1 << 30, last_to = -1;
    const char *last = NULL;
    for (int i = 0; s >= 0 && i < season_uefa_access_count; i++) {
        const SeasonUefaAccess *a = &season_uefa_access[i];
        if (a->season != s || a->comp != comp) continue;
        if (rank >= a->rank_from && rank <= a->rank_to) return a->places;
        if (a->rank_from > rank && a->rank_from < best_from) best = a->places, best_from = a->rank_from;
        if (a->rank_to > last_to) last = a->places, last_to = a->rank_to;
    }
    return best ? best : last;
}

const char *season_uefa_access_rule(int season, char comp)
{
    int s = access_season(season);
    for (int i = 0; s >= 0 && i < season_uefa_access_count; i++)
        if (season_uefa_access[i].season == s && season_uefa_access[i].comp == comp) return season_uefa_access[i].places;
    return NULL;
}
