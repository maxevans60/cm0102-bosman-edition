/* The UEFA formats table (0.6.0 slice 22): the rows that the game reads for each season. ctest season_uefa. */
#include "season_uefa.h"

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

    /* Outside the table: the first row before it, the last row after it (Q248). */
    CHECK(season_uefa_table_row(1999) == &season_uefa_rows[0]);
    CHECK(season_uefa_table_row(2099) == &season_uefa_rows[season_uefa_row_count - 1]);
    CHECK(season_uefa_table_row(2099)->season >= r2008->season);

    if (failures) return 1;
    printf("season_uefa: all checks pass\n");
    return 0;
}
