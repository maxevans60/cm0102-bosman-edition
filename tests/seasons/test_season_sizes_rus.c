/* The bound of the RusFirst take loop and the stage rows of one long season (0.7.2 league sizes, slice 12;
 * season_sizes_rus.h). */
#include <stdio.h>
#include "season_sizes_rus.h"

int main(void)
{
    static const int cases[][2] = {
        {0, 18}, {17, 18}, {18, 18}, {19, 19}, {20, 20}, {22, 22}, {23, 22}, {40, 22},
    };
    int bad = 0;
    for (unsigned k = 0; k < sizeof cases / sizeof cases[0]; k++) {
        int got = season_rus_first_take(cases[k][0]);
        if (got != cases[k][1]) {
            printf("FAIL count %d: take %d, want %d\n", cases[k][0], got, cases[k][1]);
            bad++;
        }
    }
    /* Rows of one real season in two stages: the first stage row holds the format of the edition. */
    static const struct { const char *first, *second; int want; } stages[] = {
        {"2011\u201312 (first stage, April to November 2011)", "2011\u201312 (second stage, spring 2012)", 1},
        {"2011\u201312 (first stage, March to December 2011)", "2011\u201312 (second stage, March to May 2012)", 1},
        {"2011\u201312 (second stage, spring 2012)", "2011\u201312 (first stage, April to November 2011)", 0},
        {"2010", "2011\u201312 (first stage, April to November 2011)", 0},
        {"2010\u201311 (first stage)", "2011\u201312 (second stage)", 0},
        {"2019-2020", "2019-2020", 0},
        {"2026", "2026/27", 0},
        {NULL, "2011\u201312 (second stage)", 0},
        {"2011\u201312 (first stage)", NULL, 0},
    };
    for (unsigned k = 0; k < sizeof stages / sizeof stages[0]; k++) {
        int got = season_stage_rows(stages[k].first, stages[k].second);
        if (got != stages[k].want) {
            printf("FAIL stages %u: %d, want %d\n", k, got, stages[k].want);
            bad++;
        }
    }
    printf("%s season_sizes_rus\n", bad ? "FAIL" : "PASS");
    return bad != 0;
}
