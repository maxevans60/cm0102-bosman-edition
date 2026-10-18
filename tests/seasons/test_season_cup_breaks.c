/* Test of the cups that the rules table does not hold in some seasons (0.6.0 slice 21b,
 * src/recomp_rt/win/season_cup_breaks.c), with the rules table of the build.
 *
 * Usage: season_cup_breaks_test <rules.tsv made by tools/seasons/make_rules.py>
 * ctest makes the table first (test season_rules_table). The values come from the rules table parts
 * (docs/research/rules-0.6.0/*.tsv): for example the Coupe de la Ligue is not held from 2020/21, the Norwegian
 * Cup of 2020 (row 2019/20, a calendar-year cup) is not held. */
#include "season_cups.h"
#include "season_rules.h"

#include <stdio.h>

static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; printf("FAIL line %d: ", __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: season_cup_breaks_test <rules.tsv>\n");
        return 2;
    }
    if (season_rules_load(argv[1]) <= 0) {
        fprintf(stderr, "cannot read %s\n", argv[1]);
        return 2;
    }
    /* Split-season cups: the edition of year Y is the row of season Y. */
    CHECK(season_cups_calendar("fra_lge_cup") == 0, "Coupe de la Ligue is a calendar-year cup");
    CHECK(season_cups_held("fra_lge_cup", 2019) == 1, "Coupe de la Ligue 2019/20 not held");
    CHECK(season_cups_held("fra_lge_cup", 2020) == 0, "Coupe de la Ligue 2020/21 held");
    CHECK(season_cups_next("fra_lge_cup", 2019) == 2019, "Coupe de la Ligue next from 2019");
    CHECK(season_cups_next("fra_lge_cup", 2020) == SEASON_CUPS_NEVER, "Coupe de la Ligue returns");
    CHECK(season_cups_next("fra_lge_cup", 2040) == SEASON_CUPS_NEVER, "Coupe de la Ligue after the table");
    CHECK(season_cups_next("wel_prm_cup", 2008) == SEASON_CUPS_NEVER, "Welsh Premier Cup returns");
    CHECK(season_cups_next("wel_prm_cup", 2007) == 2007, "Welsh Premier Cup 2007/08");
    /* A gap: the Leinster Cup is not held 2002/03 to 2008/09 and 2019/20 to 2021/22. */
    CHECK(season_cups_next("ire_leinster_cup", 2002) == 2009, "Leinster Cup next from 2002: %d",
          season_cups_next("ire_leinster_cup", 2002));
    CHECK(season_cups_next("ire_leinster_cup", 2019) == 2022, "Leinster Cup next from 2019");
    CHECK(season_cups_next("ire_lge_cup", 2002) == 2003, "Irish League Cup next from 2002");
    CHECK(season_cups_next("wel_lge_cup", 2020) == 2021, "Welsh League Cup next from 2020");
    /* Calendar-year cups: the edition of year Y is the row of season Y - 1 (the table's rule, club-world.md). */
    CHECK(season_cups_calendar("nor_cup") == 1, "Norwegian Cup is not a calendar-year cup");
    CHECK(season_cups_held("nor_cup", 2019) == 1 && season_cups_held("nor_cup", 2020) == 0 &&
              season_cups_held("nor_cup", 2021) == 1, "Norwegian Cup 2019, 2020, 2021");
    CHECK(season_cups_next("nor_cup", 2020) == 2021, "Norwegian Cup next from 2020");
    CHECK(season_cups_held("nor_cup", 2001) == -1, "Norwegian Cup 2001 has a row (row 2000/01)");
    CHECK(season_cups_calendar("kor_league_cup") == 1, "Korean League Cup calendar");
    CHECK(season_cups_next("kor_league_cup", 2003) == 2004, "Korean League Cup 2003 (row 2002/03 not held)");
    CHECK(season_cups_next("kor_league_cup", 2012) == SEASON_CUPS_NEVER, "Korean League Cup returns");
    CHECK(season_cups_next("kor_super_cup", 2007) == 2026, "K League Super Cup next from 2007: %d",
          season_cups_next("kor_super_cup", 2007));
    CHECK(season_cups_next("jap_emp_cup", 2026) == 2027, "Emperor's Cup next from 2026");
    CHECK(season_cups_held("jap_emp_cup", 2025) == 1, "Emperor's Cup 2025");
    /* A cup that the table always holds, and a file with no row. */
    CHECK(season_cups_next("eng_fa_cup", 2010) == 2010, "FA Cup 2010");
    CHECK(season_cups_held("eng_no_such_cup", 2010) == -1 && season_cups_next("eng_no_such_cup", 2010) == 0,
          "a file with no row");
    /* After the last row the rule stays (Q248). */
    CHECK(season_cups_held("nor_cup", 2099) == 1, "Norwegian Cup 2099");
    if (failures) {
        printf("%d failures\n", failures);
        return 1;
    }
    printf("season cup breaks: all checks pass\n");
    return 0;
}
