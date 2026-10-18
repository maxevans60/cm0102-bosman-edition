/* The nation rules lookup (season_nation.h). Host code only. */
#include "season_nation.h"

#include <string.h>

const SeasonNation *season_nation_find(const char *name)
{
    for (int i = 0; name && i < season_nation_count; i++)
        if (!strcmp(season_nations[i].name, name)) return &season_nations[i];
    return NULL;
}

int season_nation_state(const char *nation, int season, SeasonNationState *out)
{
    int found = 0;
    memset(out, 0, sizeof *out);
    out->windows = out->squad_non_eu = out->squad_foreign = out->sign_non_eu = out->sign_foreign = SEASON_NATION_KEEP;
    for (int i = 0; nation && i < season_nation_rule_count; i++) {
        const SeasonNationRule *r = &season_nation_rules[i];
        if (r->season > season || strcmp(r->nation, nation)) continue;
        found = 1;
        if (r->windows != SEASON_NATION_KEEP) {
            out->windows = r->windows;
            memcpy(out->win, r->win, sizeof out->win);
        }
        if (r->squad_non_eu != SEASON_NATION_KEEP) out->squad_non_eu = r->squad_non_eu;
        if (r->squad_foreign != SEASON_NATION_KEEP) out->squad_foreign = r->squad_foreign;
        if (r->sign_non_eu != SEASON_NATION_KEEP) out->sign_non_eu = r->sign_non_eu;
        if (r->sign_foreign != SEASON_NATION_KEEP) out->sign_foreign = r->sign_foreign;
        if (r->brexit) out->brexit = 1;
    }
    return found;
}

static int uk_nation(const char *n)
{
    return n && (!strcmp(n, "England") || !strcmp(n, "Scotland") || !strcmp(n, "Wales") || !strcmp(n, "Northern Ireland"));
}

int season_brexit_blocks(const char *club_nation, int season, int year, int rule_year, int eu, const char *nat1,
                         const char *nat2)
{
    SeasonNationState st;
    if (!uk_nation(club_nation) || year < rule_year || !eu) return 0;
    if (uk_nation(nat1) || uk_nation(nat2) || (nat1 && !strcmp(nat1, "Republic of Ireland")) ||
        (nat2 && !strcmp(nat2, "Republic of Ireland")))
        return 0;
    season_nation_state(club_nation, season, &st);
    return st.brexit;
}
