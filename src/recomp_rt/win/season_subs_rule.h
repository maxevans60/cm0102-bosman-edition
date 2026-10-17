/* The substitute rule of a competition file and season, from the rules table (0.6.0 slice 17; spec
 * docs/specs/2026-10-16-seasons-0.6.0-spec.md, slice 17). No game memory: the lookup is host code, so a test can
 * run it (tests/seasons/test_season_subs.c). The wraps and hooks that write the game's memory are in
 * season_kind_subs.c. */
#ifndef RECOMP_SEASON_SUBS_RULE_H
#define RECOMP_SEASON_SUBS_RULE_H

#define SEASON_SUBS_USED 1       /* the table gives the substitutes used */
#define SEASON_SUBS_BENCH 2      /* the table gives the named substitutes (the bench) */

/* The match engine holds at most 9 in each of the two nibbles of a fixture's word +0x4d: init_match 0x6b7fe6
 * (named, low nibble) and 0x6b8006 (used, high nibble) clamp a larger count to 9. So the rule never gives more. */
#define SEASON_SUBS_MAX 9

/* The substitutes used and the named substitutes of the competition class file (eng_prm) in the season (the
 * start year: 2001 for 2001/02). It reads the first row of the file in that season that holds a count, one
 * count at a time: a row with "unknown", "n/a", "as the Champions League" or a description gives no count. A
 * count above SEASON_SUBS_MAX gives SEASON_SUBS_MAX. *used and *bench (either may be NULL) are written only for
 * the counts found. Returns the bits SEASON_SUBS_USED and SEASON_SUBS_BENCH of the counts found, 0 when the file
 * has no such row (also for a season outside the table). */
int season_subs_rule(int season, const char *file, int *used, int *bench);

#endif
