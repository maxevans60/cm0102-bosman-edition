/* Test of the substitute rule lookup (0.6.0 slice 17, src/recomp_rt/win/season_subs_rule.c).
 *
 * Usage: season_subs_test <rules.tsv made by tools/seasons/make_rules.py>
 * ctest makes the table first (test season_rules_table). The checks: known rows of the table (the English
 * Premier Division in the seasons of 3 and 5, 3 and 7, 3 and 9, 5 and 9 substitutes), a count above what the
 * match engine holds (12 on the bench for the World Cup, clamped to 9), a file with no row, a file whose
 * count is unknown, the first row of a file that has qualifying rows after it (the European Champions Cup),
 * and a season outside the table. */
#include "season_rules.h"
#include "season_subs_rule.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } } while (0)

static void expect(int season, const char *file, int want_mask, int want_used, int want_bench)
{
    int used = -1, bench = -1;
    int mask = season_subs_rule(season, file, &used, &bench);
    CHECK(mask == want_mask, "%d %s: mask %d, want %d", season, file, mask, want_mask);
    if (want_mask & SEASON_SUBS_USED) CHECK(used == want_used, "%d %s: used %d, want %d", season, file, used, want_used);
    else CHECK(used == -1, "%d %s: used was written (%d)", season, file, used);
    if (want_mask & SEASON_SUBS_BENCH) CHECK(bench == want_bench, "%d %s: bench %d, want %d", season, file, bench, want_bench);
    else CHECK(bench == -1, "%d %s: bench was written (%d)", season, file, bench);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: season_subs_test <rules.tsv>\n");
        return 2;
    }
    CHECK(season_rules_load(argv[1]) > 0, "the table loads");
    const int both = SEASON_SUBS_USED | SEASON_SUBS_BENCH;
    expect(2001, "eng_prm", both, 3, 5);
    expect(2007, "eng_prm", both, 3, 5);
    expect(2008, "eng_prm", both, 3, 7);
    expect(2021, "eng_prm", both, 3, 9);
    expect(2022, "eng_prm", both, 5, 9);
    expect(2027, "eng_prm", both, 5, 9);
    /* 12 named for the World Cup in 2001: the match engine clamps to 9 (0x6b7feb), so the lookup does too. */
    expect(2001, "world_cup", both, 3, SEASON_SUBS_MAX);
    /* A file that the table does not hold, and a season outside it. */
    expect(2001, "no_such_file", 0, 0, 0);
    expect(2100, "eng_prm", 0, 0, 0);
    expect(2000, "eng_prm", 0, 0, 0);
    /* The first numeric row of a file with qualifying rows after it: the Champions League in 2012/13. */
    expect(2012, "european_cup", both, 3, 7);
    /* NULL arguments do nothing. */
    CHECK(season_subs_rule(2001, NULL, NULL, NULL) == 0, "NULL file");
    int u = -1;
    CHECK(season_subs_rule(2001, "eng_prm", &u, NULL) == both && u == 3, "NULL bench pointer");
    if (failures) {
        fprintf(stderr, "%d checks failed\n", failures);
        return 1;
    }
    puts("ok");
    return 0;
}
