/* The Italian rules of the league sizes that depend only on numbers (season_ita_rules.h; 0.7.2 gap rows 30 and 32). */
#include "season_ita_rules.h"

#include <stdio.h>

static int fails;

static void check(int got, int want, const char *what)
{
    if (got != want) {
        printf("FAIL %s: %d, want %d\n", what, got, want);
        fails++;
    }
}

int main(void)
{
    /* Row 30. The class's layout at 20 clubs with 4 up and 4 down: 1, then 2 for rows 1 to 3, 3 for rows 16 to 19
     * (0x647057..0x647077). */
    const int class20[20] = {1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3};
    for (int i = 0; i < 20; i++) check(ita_serie_b_zone(2002, 20, 4, 4, i), class20[i], "2002/03 at 20");
    /* 2003/04, 24 clubs: 5 promoted, 3 relegated directly (22nd to 24th). */
    check(ita_serie_b_zone(2003, 24, 5, 3, 0), 1, "2003/04 champion");
    check(ita_serie_b_zone(2003, 24, 5, 3, 4), 2, "2003/04 5th");
    check(ita_serie_b_zone(2003, 24, 5, 3, 5), 0, "2003/04 6th");
    check(ita_serie_b_zone(2003, 24, 5, 3, 20), 0, "2003/04 21st");
    check(ita_serie_b_zone(2003, 24, 5, 3, 21), 3, "2003/04 22nd");
    check(ita_serie_b_zone(2003, 24, 5, 3, 23), 3, "2003/04 24th");
    /* 2004/05, 22 clubs: 2 promoted directly, 3 relegated directly (20th to 22nd). */
    check(ita_serie_b_zone(2004, 22, 2, 3, 1), 2, "2004/05 2nd");
    check(ita_serie_b_zone(2004, 22, 2, 3, 2), 0, "2004/05 3rd");
    check(ita_serie_b_zone(2004, 22, 2, 3, 18), 0, "2004/05 19th");
    check(ita_serie_b_zone(2004, 22, 2, 3, 19), 3, "2004/05 20th");
    /* From 2005/06 the head-to-head table settles a tie: no zone, so no tie-break match, at any count. */
    for (int y = 2005; y <= 2030; y++)
        for (int n = y == 2018 ? 19 : 22, i = 0; i < n; i++) check(ita_serie_b_zone(y, n, 2, 3, i), 0, "no zone from 2005");
    check(ita_serie_b_zone(2019, 20, 2, 3, 0), 0, "2019/20 at 20");
    check(ita_serie_b_zone(2003, 24, 5, 3, 24), 0, "a row past the table");

    /* Row 32. compare_clubs_by_seeding_pot 0x640590 at 20 clubs: B 3rd to 6th pot 4, 7th to 14th 3, 15th to 18th 2. */
    const ItaCupClub base = {1, 1, 0, 1, 1, 0};
    ItaCupClub c = base;
    for (int p = 1; p <= 24; p++) {
        c.place = p;
        int want = p >= 3 && p <= 6 ? 4 : p >= 7 && p <= 14 ? 3 : p >= 15 && p <= 18 ? 2 : 1;
        check(ita_cup_pot(&c, 20), want, "B place at 20");
        check(ita_cup_pot(&c, 0), want, "B place, count unknown");
    }
    /* The last Serie B of 22 (2004/05 to 2017/18): 19th and 20th as 15th to 18th; 21st and 22nd pot 1. */
    for (int p = 15; p <= 22; p++) {
        c.place = p;
        check(ita_cup_pot(&c, 22), p <= 20 ? 2 : 1, "B place at 22");
    }
    c.place = 22;
    check(ita_cup_pot(&c, 24), 2, "22nd of 24");
    c.place = 18;
    check(ita_cup_pot(&c, 19), 1, "18th of 19");
    c.place = 17;
    check(ita_cup_pot(&c, 19), 2, "17th of 19");
    /* Relegated from Serie A into Serie B: pot 4. Promoted from below into Serie B: pot 2. No last division: 1. */
    ItaCupClub down = {1, 0, 1, 1, 0, 18};
    check(ita_cup_pot(&down, 22), 4, "down from A");
    ItaCupClub up = {1, 0, 0, 1, 0, 1};
    check(ita_cup_pot(&up, 22), 2, "up from C1");
    ItaCupClub up_to_a = {1, 1, 0, 0, 0, 3};
    check(ita_cup_pot(&up_to_a, 22), 4, "B 3rd now in Serie A");
    ItaCupClub stay_a = {1, 0, 1, 0, 0, 5};
    check(ita_cup_pot(&stay_a, 22), 1, "Serie A club");
    ItaCupClub none = {0, 0, 0, 1, 0, 0};
    check(ita_cup_pot(&none, 22), 1, "no last division");
    /* The count: part of a database table in the start season keeps the original; a played season stays. */
    check(ita_cup_b_count(17, 2004, 2004), 0, "17 from a 2004 database");
    check(ita_cup_b_count(24, 2004, 2004), 24, "24 from a 2004 database");
    check(ita_cup_b_count(19, 2019, 2018), 19, "19 played in 2018/19");
    check(ita_cup_b_count(22, 2006, 2005), 22, "22 played in 2005/06");
    check(ita_cup_b_count(0, 2006, 2005), 0, "no Serie B clubs");

    /* Row 31. The play-out ties of the original (rows 16 v 13, 15 v 14) as places (1st = 1) at the real counts. */
    const int real[5][5] = {{18, 17, 14, 16, 15}, {19, 18, 15, 17, 16}, {17, 16, 13, 15, 14}, {16, 15, 12, 14, 13},
                            {20, 19, 16, 18, 17}};
    const int orig[4] = {16, 13, 15, 14};
    for (int k = 0; k < 5; k++)
        for (int j = 0; j < 4; j++) check(ita_c1_playout_row(real[k][0], orig[j]) + 1, real[k][j + 1], "play-out place");
    check(ita_c1_playout_row(5, 16), 16, "a group under 6");

    if (!fails) printf("PASS season_ita_rules\n");
    return fails ? 1 : 0;
}
