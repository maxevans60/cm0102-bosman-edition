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

int season_uefa_access_league_cup(int season, const char *name)
{
    int s = access_season(season);
    for (int i = 0; s >= 0 && i < season_uefa_access_count; i++)
        if (season_uefa_access[i].season == s && season_uefa_access[i].comp == 'L' &&
            !strcmp(season_uefa_access[i].places, name))
            return 1;
    return 0;
}

/* ---- Entry codes and round sizes (slice 23) ---- */

/* The code of a token of the access list. Champions League: the plain Q1, Q2 and PR rounds are for champions
 * only, so they are the Champions Path; a plain PO or Q3 too. */
int season_uefa_code(char comp, const char *tok, int n)
{
    static const struct { const char *t; int cl, el; } map[] = {
        {"G", SEASON_UEFA_CL_G, SEASON_UEFA_EL_G},       {"PO-LP", SEASON_UEFA_CL_PO_LP, SEASON_UEFA_EL_PO},
        {"PO-CP", SEASON_UEFA_CL_PO_CP, SEASON_UEFA_EL_PO}, {"PO", SEASON_UEFA_CL_PO_CP, SEASON_UEFA_EL_PO},
        {"Q3-LP", SEASON_UEFA_CL_Q3_LP, SEASON_UEFA_EL_Q3}, {"Q3-CP", SEASON_UEFA_CL_Q3_CP, SEASON_UEFA_EL_Q3},
        {"Q3", SEASON_UEFA_CL_Q3_CP, SEASON_UEFA_EL_Q3},  {"Q2-LP", SEASON_UEFA_CL_Q2_LP, SEASON_UEFA_EL_Q2},
        {"Q2-CP", SEASON_UEFA_CL_Q2_CP, SEASON_UEFA_EL_Q2}, {"Q2", SEASON_UEFA_CL_Q2_CP, SEASON_UEFA_EL_Q2},
        {"Q1", SEASON_UEFA_CL_Q1, SEASON_UEFA_EL_Q1},     {"PR", SEASON_UEFA_CL_PR, SEASON_UEFA_EL_PR},
    };
    for (unsigned i = 0; i < sizeof map / sizeof map[0]; i++)
        if ((int)strlen(map[i].t) == n && !strncmp(map[i].t, tok, (size_t)n)) return comp == 'c' ? map[i].cl : map[i].el;
    return 0;
}


static const char *const CL_ROUND[SEASON_UEFA_CL_CODES][2] = {
    {NULL, NULL}, {"Groups", NULL}, {"PO LP", NULL}, {"PO CP", "PO"}, {"Q3 LP", NULL}, {"Q3 CP", "Q3"},
    {"Q2 LP", NULL}, {"Q2 CP", "Q2"}, {"Q1", NULL}, {"PR", NULL}};
static const char *const EL_LEVEL[SEASON_UEFA_EL_CODES - SEASON_UEFA_EL_G + 1] = {"Groups", "PO", "Q3", "Q2", "Q1", "PR",
                                                                               NULL};

int season_uefa_code_clubs(const SeasonUefa *r, char comp, int code)
{
    if (!r) return 0;
    if (comp == 'c') {
        if (code < 1 || code >= SEASON_UEFA_CL_CODES) return 0;
        int n = season_uefa_cl_round(r, CL_ROUND[code][0]);
        return n || !CL_ROUND[code][1] ? n : season_uefa_cl_round(r, CL_ROUND[code][1]);
    }
    if (code < SEASON_UEFA_EL_G || code >= SEASON_UEFA_EL_CODES) return 0;
    return season_uefa_uc_level(r, EL_LEVEL[code - SEASON_UEFA_EL_G]);
}

/* The winners of a Champions League round of clubs clubs. The preliminary round is a tournament of semi-finals
 * and a final (rows uefa-138, uefa-139: "PR 4", one winner to Q1), so a quarter of its clubs go through. */
static int cl_winners(int code, int clubs) { return code == SEASON_UEFA_CL_PR ? clubs / 4 : clubs / 2; }

/* The Champions League losers of round tok ("Q3", "Q3-LP", "PR") of the row. */
static int cl_losers(const SeasonUefa *r, const char *tok, int n)
{
    char name[8];
    if (n <= 0 || n >= (int)sizeof name) return 0;
    memcpy(name, tok, (size_t)n);
    name[n] = 0;
    if (!strcmp(name, "PR")) {
        int clubs = season_uefa_cl_round(r, "PR");
        return clubs - cl_winners(SEASON_UEFA_CL_PR, clubs);
    }
    if (name[2] == '-') {                       /* a path: "Q3-LP" is the round "Q3 LP" */
        name[2] = ' ';
        return season_uefa_cl_round(r, name) / 2;
    }
    return season_uefa_cl_level(r, name) / 2;
}

int season_uefa_dropped(const SeasonUefa *r, int el_code)
{
    if (!r || !r->drops || el_code < SEASON_UEFA_EL_G || el_code >= SEASON_UEFA_EL_CODES) return 0;
    int sum = 0;
    for (const char *p = r->drops; *p;) {
        while (*p == ' ') p++;
        const char *gt = strchr(p, '>');
        if (!gt) break;
        const char *e = gt + 1;
        while (*e && *e != ' ') e++;
        int to = season_uefa_code('e', gt + 1, (int)(e - gt - 1));
        if (to == el_code) sum += cl_losers(r, p, (int)(gt - p));
        p = e;
    }
    return sum;
}

void season_uefa_new_clubs(const SeasonUefa *r, char comp, int *out)
{
    for (int i = 0; i < SEASON_UEFA_CODE_SLOTS; i++) out[i] = 0;
    if (!r) return;
    if (comp == 'c') {
        static const int champions[] = {SEASON_UEFA_CL_G, SEASON_UEFA_CL_PO_CP, SEASON_UEFA_CL_Q3_CP,
                                        SEASON_UEFA_CL_Q2_CP, SEASON_UEFA_CL_Q1, SEASON_UEFA_CL_PR};
        static const int league[] = {SEASON_UEFA_CL_G, SEASON_UEFA_CL_PO_LP, SEASON_UEFA_CL_Q3_LP, SEASON_UEFA_CL_Q2_LP};
        const int *paths[2] = {champions, league};
        int lens[2] = {6, 4};
        out[SEASON_UEFA_CL_G] = season_uefa_code_clubs(r, 'c', SEASON_UEFA_CL_G);
        for (int p = 0; p < 2; p++)
            for (int i = 0; i < lens[p]; i++) {
                int code = paths[p][i], below = 0, won = 0;
                for (int j = i + 1; j < lens[p] && !below; j++) {
                    below = season_uefa_code_clubs(r, 'c', paths[p][j]);
                    won = cl_winners(paths[p][j], below);
                }
                if (code == SEASON_UEFA_CL_G) out[code] -= won;
                else out[code] = season_uefa_code_clubs(r, 'c', code) - won;
            }
        return;
    }
    for (int code = SEASON_UEFA_EL_G; code < SEASON_UEFA_EL_CODES; code++) {
        int size = season_uefa_code_clubs(r, 'e', code), below = 0;
        for (int j = code + 1; j < SEASON_UEFA_EL_CODES && !below; j++) below = season_uefa_code_clubs(r, 'e', j);
        out[code] = size ? size - below / 2 - season_uefa_dropped(r, code) : 0;
    }
}
