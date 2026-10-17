/* The substitute rule lookup (season_subs_rule.h). Host code only. */
#include "season_subs_rule.h"
#include "season_rules.h"

#include <stddef.h>

static int clamp(int n) { return n > SEASON_SUBS_MAX ? SEASON_SUBS_MAX : n; }

int season_subs_rule(int season, const char *file, int *used, int *bench)
{
    if (!file) return 0;
    int found = 0;
    for (int n = 0; found != (SEASON_SUBS_USED | SEASON_SUBS_BENCH); n++) {
        const SeasonRule *r = season_rules_find(season, file, n);
        if (!r) break;
        if (!(found & SEASON_SUBS_USED) && r->subs_used >= 0) {
            if (used) *used = clamp(r->subs_used);
            found |= SEASON_SUBS_USED;
        }
        if (!(found & SEASON_SUBS_BENCH) && r->bench >= 0) {
            if (bench) *bench = clamp(r->bench);
            found |= SEASON_SUBS_BENCH;
        }
    }
    return found;
}
