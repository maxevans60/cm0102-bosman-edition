/* The UEFA formats table (0.6.0 slice 22): the rows that the game reads for each season. ctest season_uefa. */
#include "season.h"
#include "season_uefa.h"

#include <stdio.h>
#include <string.h>

static int failures;

/* Stand-ins of the season of the game (season.c). */
static int code_on = 1, start_year = 2001, changing = 0;
int season_code_on(void) { return code_on; }
int season_start_year(void) { return start_year; }
int season_changing_rules(void) { return changing; }

#define CHECK(cond)                                                                 \
    do {                                                                            \
        if (!(cond)) {                                                              \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                  \
            failures++;                                                             \
        }                                                                           \
    } while (0)

int main(void)
{
    CHECK(season_uefa_row_count >= 8);
    for (int i = 1; i < season_uefa_row_count; i++) CHECK(season_uefa_rows[i].season == season_uefa_rows[i - 1].season + 1);

    /* Two group stages and a quarter-final to 2002/03 (the original's format), one group stage and a round
     * of 16 from 2003/04 (change point U1). */
    const SeasonUefa *r2001 = season_uefa_table_row(2001), *r2002 = season_uefa_table_row(2002);
    const SeasonUefa *r2003 = season_uefa_table_row(2003), *r2008 = season_uefa_table_row(2008);
    CHECK(r2001 && r2001->season == 2001 && !season_uefa_cl_round_of_16(r2001));
    CHECK(r2002 && !season_uefa_cl_round_of_16(r2002));
    CHECK(season_uefa_cl_round(r2002, "Groups 2") == 16 && season_uefa_cl_round(r2002, "KO") == 8);
    CHECK(r2003 && r2003->season == 2003 && season_uefa_cl_round_of_16(r2003));
    CHECK(season_uefa_cl_round(r2003, "Q3") == 32 && season_uefa_cl_round(r2003, "Groups") == 32);

    /* UEFA Cup groups from 2004/05 (U2): 8 groups of 5, single round robin, a round of 32 after them. */
    CHECK(r2003->uc_groups == 0);
    const SeasonUefa *r2004 = season_uefa_table_row(2004);
    CHECK(r2004->uc_groups == 8 && r2004->uc_group_clubs == 5 && r2004->uc_meetings == 1);
    CHECK(season_uefa_uc_round(r2004, "R1") == 80 && season_uefa_uc_round(r2004, "KO") == 32);

    /* The Intertoto Cup: 5 rounds and 3 winners into the UEFA Cup first round to 2005, then 3 rounds and 11
     * winners into the second qualifying round to 2008 (U3). */
    CHECK(season_uefa_table_row(2005)->uic_rounds == 5 && season_uefa_table_row(2005)->uic_winners == 3);
    CHECK(!strcmp(season_uefa_table_row(2005)->uic_entry, "R1"));
    CHECK(season_uefa_table_row(2006)->uic_rounds == 3 && season_uefa_table_row(2006)->uic_winners == 11);
    CHECK(!strcmp(season_uefa_table_row(2006)->uic_entry, "Q2"));

    /* Slice 23. The Europa League from 2009/10 (U4): 12 groups of 4, double round robin, top two and the 8
     * Champions League thirds to a round of 32. The UEFA Cup groups of 2004/05 to 2008/09 send three. */
    const SeasonUefa *r2009 = season_uefa_table_row(2009), *r2018 = season_uefa_table_row(2018);
    CHECK(r2008->uc_groups == 8 && r2008->uc_through == 3);
    CHECK(r2009 && r2009->season == 2009 && r2009->uc_groups == 12 && r2009->uc_group_clubs == 4);
    CHECK(r2009->uc_meetings == 2 && r2009->uc_through == 2 && season_uefa_uc_round(r2009, "KO") == 32);
    CHECK(season_uefa_uc_round(r2009, "PO") == 76 && season_uefa_uc_round(r2009, "Groups") == 48);
    /* The Intertoto Cup is not held from 2009 (U3). */
    CHECK(r2008->uic_clubs == 50 && r2009->uic_clubs == -1 && r2009->uic_rounds == 0);
    /* The Champions League paths from 2009/10 (U5, Q296): Q3 and PO of two paths. */
    CHECK(season_uefa_cl_round(r2009, "Q3 CP") == 20 && season_uefa_cl_round(r2009, "Q3 LP") == 10);
    CHECK(season_uefa_cl_level(r2009, "Q3") == 30 && season_uefa_cl_level(r2009, "PO") == 20);
    CHECK(season_uefa_cl_level(r2009, "Q2") == 34 && season_uefa_cl_round_of_16(r2009));
    CHECK(season_uefa_cl_level(r2008, "Q3") == 32);
    /* 2018/19: a preliminary round, paths from Q2; Europa League paths CP and MP. */
    CHECK(season_uefa_cl_round(r2018, "PR") == 4 && season_uefa_cl_level(r2018, "Q2") == 24);
    CHECK(season_uefa_uc_level(r2018, "Q2") == 92 && season_uefa_uc_round(r2018, "Q2 MP") == 74);
    /* The Super Cup: Monaco to 2012, then a venue each year (U6). */
    CHECK(!strcmp(r2008->sc_city, "Monaco") && r2008->sc_day == 29 && r2008->sc_month == 7);
    const SeasonUefa *r2013 = season_uefa_table_row(2013);
    CHECK(!strcmp(r2013->sc_city, "Prague") && r2013->sc_day == 30 && r2013->sc_month == 7);
    CHECK(!strcmp(season_uefa_table_row(2020)->sc_city, "Budapest") && season_uefa_table_row(2020)->sc_month == 8);

    /* The access list by rank (Q369): none to 2008/09, the real list from 2009/10. */
    CHECK(season_uefa_access_places(2008, 'c', 1) == NULL);
    CHECK(!strcmp(season_uefa_access_places(2009, 'c', 1), "G G G PO-LP"));
    CHECK(!strcmp(season_uefa_access_places(2009, 'c', 39), "Q2"));       /* Liechtenstein's rank: the next bracket */
    CHECK(!strcmp(season_uefa_access_places(2009, 'c', 70), "Q1"));       /* after the last bracket: the last */
    CHECK(!strcmp(season_uefa_access_places(2009, 'e', 1), "PO PO Q3"));
    CHECK(!strcmp(season_uefa_access_places(2018, 'c', 4), "G G G G"));  /* four places for the top four (U7) */
    CHECK(!strcmp(season_uefa_access_places(2017, 'c', 4), "G G PO-LP"));
    CHECK(!strcmp(season_uefa_access_places(2030, 'c', 4), "G G G G"));  /* after the table: the last season */
    CHECK(!strcmp(season_uefa_access_rule(2009, 'h'), "G"));
    CHECK(!strcmp(season_uefa_access_rule(2015, 'X'), "PO-LP") && !strcmp(season_uefa_access_rule(2018, 'X'), "G"));
    CHECK(season_uefa_access_rule(2015, 'f') && season_uefa_access_rule(2016, 'f') == NULL);

    /* Outside the table: the first row before it, the last row after it (Q248). */
    CHECK(season_uefa_table_row(1999) == &season_uefa_rows[0]);
    CHECK(season_uefa_table_row(2099) == &season_uefa_rows[season_uefa_row_count - 1]);
    CHECK(season_uefa_table_row(2099)->season >= r2008->season);

    /* The row of the game that runs: none for the base game and with the season code off (the original runs),
     * the row of each season with changing rules on, the row of the start year with changing rules off. */
    start_year = 2001, changing = 0;
    CHECK(season_uefa_row(2001) == NULL && season_uefa_row(2005) == NULL);
    changing = 1;
    CHECK(season_uefa_row(2003) == r2003 && season_uefa_row(2001) == r2001);
    code_on = 0;
    CHECK(season_uefa_row(2003) == NULL);
    code_on = 1, start_year = 2005, changing = 0;
    CHECK(season_uefa_row(2007) == season_uefa_table_row(2005) && season_uefa_row(2003) == season_uefa_table_row(2005));
    changing = 1;
    CHECK(season_uefa_row(2007) == season_uefa_table_row(2007));
    start_year = 2010, changing = 0;
    CHECK(season_uefa_row(2010) == &season_uefa_rows[season_uefa_row_count - 1]);
    code_on = 0;
    CHECK(season_uefa_row(2010) == NULL);

    if (failures) return 1;
    printf("season_uefa: all checks pass\n");
    return 0;
}
