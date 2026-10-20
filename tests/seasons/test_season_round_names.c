/* The real round names (Q370): the lookup of the table. ctest season_round_names. */
#include "season_round_names.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond)                                                                 \
    do {                                                                            \
        if (!(cond)) {                                                              \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                  \
            failures++;                                                             \
        }                                                                           \
    } while (0)

static int is(const char *got, const char *want) { return got && want && !strcmp(got, want); }

int main(void)
{
    /* The Champions League round of 16 from 2003/04 in both name sets; the original's name before. */
    CHECK(is(season_round_name("european_cup", 20, 2003, SEASON_ROUND_NAME_LONG), "Round of 16"));
    CHECK(is(season_round_name("european_cup", 20, 2008, SEASON_ROUND_NAME_PLURAL), "Round of 16"));
    CHECK(season_round_name("european_cup", 20, 2002, SEASON_ROUND_NAME_LONG) == NULL);
    /* The UEFA Cup rounds of its group format from 2004/05; the original's names in 2003/04. */
    CHECK(is(season_round_name("uefa_cup", 30, 2004, SEASON_ROUND_NAME_LONG), "Round of 32"));
    CHECK(is(season_round_name("uefa_cup", 40, 2006, SEASON_ROUND_NAME_LONG), "Round of 16"));
    CHECK(is(season_round_name("uefa_cup", 250, 2004, SEASON_ROUND_NAME_PLURAL), "First Qualifying Round"));
    /* The abbreviated set has its own short names; a set with no row keeps the original's ("Qual.1"). */
    CHECK(is(season_round_name("european_cup", 20, 2004, SEASON_ROUND_NAME_ABBREVIATED), "Rnd of 16"));
    CHECK(is(season_round_name("uefa_cup", 30, 2004, SEASON_ROUND_NAME_ABBREVIATED), "Rnd of 32"));
    CHECK(season_round_name("uefa_cup", 250, 2004, SEASON_ROUND_NAME_ABBREVIATED) == NULL);
    CHECK(is(season_round_name("uefa_cup", 260, 2008, SEASON_ROUND_NAME_LONG), "Second Qualifying Round"));
    CHECK(season_round_name("uefa_cup", 30, 2003, SEASON_ROUND_NAME_LONG) == NULL);
    /* Slice 24: round 30 of the Europa League is the knockout round play-offs from 2021/22. */
    CHECK(is(season_round_name("uefa_cup", 30, 2020, SEASON_ROUND_NAME_LONG), "Round of 32"));
    CHECK(is(season_round_name("uefa_cup", 30, 2021, SEASON_ROUND_NAME_LONG), "Knockout Round Play-offs"));
    CHECK(is(season_round_name("uefa_cup", 30, 2023, SEASON_ROUND_NAME_ABBREVIATED), "KO Play-offs"));
    CHECK(is(season_round_name("uefa_cup", 40, 2021, SEASON_ROUND_NAME_LONG), "Round of 16"));
    /* Part 24b: the Conference League's rounds, from 2021/22 only. */
    CHECK(is(season_round_name("new_conference_league", 30, 2021, SEASON_ROUND_NAME_LONG), "Knockout Round Play-offs"));
    CHECK(is(season_round_name("new_conference_league", 220, 2022, SEASON_ROUND_NAME_ABBREVIATED), "Play-off Rnd"));
    CHECK(is(season_round_name("new_conference_league", 270, 2021, SEASON_ROUND_NAME_PLURAL), "Third Qualifying Round"));
    CHECK(season_round_name("new_conference_league", 30, 2020, SEASON_ROUND_NAME_LONG) == NULL);
    /* Another competition with the same code keeps the original's name. */
    CHECK(season_round_name("eng_fa_cup", 20, 2005, SEASON_ROUND_NAME_LONG) == NULL);
    CHECK(season_round_name("european_cup", 30, 2005, SEASON_ROUND_NAME_LONG) == NULL);
    CHECK(season_round_name(NULL, 20, 2005, SEASON_ROUND_NAME_LONG) == NULL);
    /* After the last row the name stays (Q248). */
    CHECK(is(season_round_name("european_cup", 20, 2099, SEASON_ROUND_NAME_LONG), "Round of 16"));
    for (int i = 0; i < season_round_name_count; i++) CHECK(season_round_name_rows[i].round_code % 10 == 0);
    if (failures) return 1;
    printf("season_round_names: all checks pass\n");
    return 0;
}
