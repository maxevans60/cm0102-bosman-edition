/* The substitute rule lookup (season_subs_rule.h). Host code only. */
#include "season_subs_rule.h"
#include "season_rules.h"

#include <stddef.h>

static int clamp(int n) { return n > SEASON_SUBS_MAX ? SEASON_SUBS_MAX : n; }

int season_subs_rule(int season, const char *file, int *used, int *bench)
{
    return season_subs_rule_from(season, file, used, bench, NULL, NULL);
}

int season_subs_rule_from(int season, const char *file, int *used, int *bench, int *used_from, int *bench_from)
{
    if (!file) return 0;
    int found = 0;
    for (int n = 0; found != (SEASON_SUBS_USED | SEASON_SUBS_BENCH); n++) {
        const SeasonRule *r = season_rules_find(season, file, n);
        if (!r) break;
        if (!(found & SEASON_SUBS_USED) && r->subs_used >= 0) {
            if (used) *used = clamp(r->subs_used);
            if (used_from) *used_from = season;
            found |= SEASON_SUBS_USED;
        }
        if (!(found & SEASON_SUBS_BENCH) && r->bench >= 0) {
            if (bench) *bench = clamp(r->bench);
            if (bench_from) *bench_from = season;
            found |= SEASON_SUBS_BENCH;
        }
    }
    /* Item 274: a count that the season's rows give as "unknown" keeps the latest earlier count of the file, so a
     * new season row with no source does not fall back to the competition's own 3 and 5. Only "unknown" carries:
     * "n/a", "as the Champions League" and a description stay as they are. */
    int want = 0;
    for (int n = 0; ; n++) {
        const SeasonRule *r = season_rules_find(season, file, n);
        if (!r) break;
        if (!(found & SEASON_SUBS_USED) && r->subs_used == SEASON_RULE_UNKNOWN) want |= SEASON_SUBS_USED;
        if (!(found & SEASON_SUBS_BENCH) && r->bench == SEASON_RULE_UNKNOWN) want |= SEASON_SUBS_BENCH;
    }
    for (int y = season - 1; want && y >= SEASON_RULE_FIRST; y--) {
        for (int n = 0; want; n++) {
            const SeasonRule *r = season_rules_find(y, file, n);
            if (!r) break;
            if ((want & SEASON_SUBS_USED) && r->subs_used >= 0) {
                if (used) *used = clamp(r->subs_used);
                if (used_from) *used_from = y;
                found |= SEASON_SUBS_USED;
                want &= ~SEASON_SUBS_USED;
            }
            if ((want & SEASON_SUBS_BENCH) && r->bench >= 0) {
                if (bench) *bench = clamp(r->bench);
                if (bench_from) *bench_from = y;
                found |= SEASON_SUBS_BENCH;
                want &= ~SEASON_SUBS_BENCH;
            }
        }
    }
    return found;
}
