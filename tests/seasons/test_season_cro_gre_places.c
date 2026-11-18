/* The Croatian and Greek places of a row (0.7.3 slice 3, open items 281 and 282): season_cro_gre_places.c. */
#include "season_cro_gre_places.h"

#include <stdio.h>

static int failed;

static void same(const char *what, int got, int want)
{
    if (got == want) return;
    fprintf(stderr, "FAIL %s: got %d, want %d\n", what, got, want);
    failed = 1;
}

int main(void)
{
    /* The Druga HNL of one group takes the row's promotion place (rows gre-pol-cro-361 to -376: 1 up). */
    same("cro_a2a 2010 up", season_cro_gre_up("cro_a2a", 1, 1, 0), 1);
    /* Two groups: the rows give 0 up and the group winners go through the chain. */
    same("cro_a2a 2003 up", season_cro_gre_up("cro_a2a", 0, 0, 0), 0);
    /* An unknown row (2008, 2009) keeps the class's places. */
    same("cro_a2a 2009 up unknown", season_cro_gre_up("cro_a2a", 1, -2, 0), 0);
    same("gre_second up is not written here", season_cro_gre_up("gre_second", 1, 3, 2), 2);

    /* The lowest divisions have no division below in the size table, so set_places leaves them. */
    same("cro_a2b 2001 down", season_cro_gre_down("cro_a2b", 6, 2), 6);
    same("gre_second 2005 down", season_cro_gre_down("gre_second", 3, 2), 3);
    same("gre_second 2017 down", season_cro_gre_down("gre_second", 6, 2), 6);
    same("gre_second 2004 down unknown", season_cro_gre_down("gre_second", -2, 2), 2);
    same("gre_second n/a", season_cro_gre_down("gre_second", -1, 2), 2);
    same("a count past the places of a byte", season_cro_gre_down("gre_second", 31, 2), 2);
    same("cro_a1 is set_places' own", season_cro_gre_down("cro_a1", 4, 2), 2);

    /* The Super League with two groups below: both winners, and the row when it sends more (2014/15: 4 down). */
    same("gre_prm 2014 down", season_gre_prm_down(2, 4), 4);
    same("gre_prm 2013 down", season_gre_prm_down(2, 2), 2);
    same("gre_prm 2021 down keeps both winners", season_gre_prm_down(2, 1), 2);
    same("gre_prm unknown keeps both winners", season_gre_prm_down(2, -2), 2);

    if (!failed) puts("season_cro_gre_places: ok");
    return failed;
}
