/* Test of the league structure of the rules table (0.7.2 league sizes, slice 1): season_league_meetings and the
 * structure fields of season_league_table.c (src/recomp_rt/win/season_league.c).
 *
 * Usage: season_league_test <rules.tsv made by tools/seasons/make_rules.py>
 * The meetings of a row (words of the format text, a count of matches, a hand row), the after-table rule (a season
 * after the last row takes the last row), a future row with unknown clubs (it takes the row before it), a file
 * that is not a league or has no row (0: the class's meetings stay), and for every row whose values are known: the
 * rounds are meetings x (n' - 1) + the extra rounds that the rest rounds of an odd count do not hold, and a split's
 * first section is its top group. */
#include "season_league.h"
#include "season_rules.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } } while (0)

static void meet(const char *file, int season, int want)
{
    int got = season_league_meetings(file, season);
    CHECK(got == want, "%s %d: meetings %d, want %d", file, season, got, want);
}

int main(int argc, char **argv)
{
    if (argc != 2 || season_rules_load(argv[1]) <= 0) {
        fprintf(stderr, "usage: season_league_test <rules.tsv>\n");
        return 2;
    }
    meet("eng_prm", 2001, 2);           /* double round robin */
    meet("sco_first", 2001, 4);         /* 36 matches (each pair four times) */
    meet("sco_prm", 2001, 3);           /* split after 33 matches of 12 clubs */
    meet("cro_a1", 2007, 3);            /* triple round robin */
    meet("cro_a1", 2006, 3);            /* a third game set by the standings (MEETINGS_FIX) */
    meet("arg_prm", 2001, 1);           /* two halves of one meeting */
    meet("arg_prm", 2024, 1);           /* zones of one meeting */
    meet("jap_j2", 2001, 4);            /* a hand row: J2 of 12 clubs, 44 rounds */
    meet("jap_j2", 2010, 2);            /* a hand row: J2 of 19 clubs, 2 meetings */
    meet("ita_ser_a", 2027, 2);         /* unknown clubs: the row before */
    meet("ita_ser_a", 2300, 2);         /* after the table: the last row, then the row before */
    meet("gre_cup", 2010, 0);           /* a cup */
    meet("no_such_file", 2010, 0);

    const SeasonLeagueRule *a = season_league_rule("arg_prm", 2024);
    CHECK(a && a->halves == 1 && a->zones == 2 && a->zone_size[0] == 15 && a->zone_size[1] == 15 &&
          a->extra_rounds == 2 && a->rounds == 16, "arg_prm 2024: halves, two zones of 15, 2 inter-zone rounds, 16");
    const SeasonLeagueRule *n = season_league_rule("nir_prm", 2010);
    CHECK(n && n->meetings == 3 && n->split_after == 33 && n->split_size[0] == 6 && n->split_size[1] == 6 &&
          n->split_meetings[0] == 1 && n->split_meetings[1] == 1 && n->rounds == 33,
          "nir_prm 2010: 3 meetings, split after 33 into 6 and 6, one more meeting");
    const SeasonLeagueRule *h = season_league_rule("hol_first", 2005);
    CHECK(h && h->periods == 6, "hol_first 2005: six periods");

    int checked = 0;
    for (int i = 0; i < season_league_rule_count; i++) {
        const SeasonLeagueRule *e = &season_league_rules[i];
        int k = 0;                                      /* the k-th row of the file and season: the groups */
        for (int j = 0; j < i; j++)
            k += season_league_rules[j].season == e->season && !strcmp(season_league_rules[j].file, e->file);
        const SeasonRule *r = season_rules_find(e->season, e->file, k);
        if (!e->meetings || !e->rounds || e->extra_rounds < 0 || !r) continue;
        int big = 0;
        for (int z = 0; z < e->zones; z++)
            if (e->zone_size[z] > big) big = e->zone_size[z];
        int count = e->zones ? big : r->clubs;
        if (count <= 0) continue;
        checked++;
        int rests = count % 2 ? e->meetings : 0, extra = e->extra_rounds > rests ? e->extra_rounds - rests : 0;
        CHECK(e->rounds == e->meetings * (count + count % 2 - 1) + extra, "%s %d: rounds %d", e->file, e->season,
              e->rounds);
        CHECK(e->split_after == 0 || e->split_size[0] == e->split_top, "%s %d: split sections", e->file, e->season);
    }
    CHECK(checked > 1000, "rows with rounds: %d", checked);
    printf(failures ? "FAILED: %d\n" : "PASS\n", failures);
    return failures ? 1 : 0;
}
