/* The real round names (season_round_names.h). Made by tools/seasons/make_round_names.py from
 * docs/research/round-names.tsv, where each row has its source. Do not edit by hand. */
#include "season_round_names.h"

const SeasonRoundName season_round_name_rows[] = {
    {"european_cup", 20, 2003, 3, "Round of 16"},
    {"european_cup", 20, 2003, 4, "Rnd of 16"},
    {"uefa_cup", 30, 2004, 3, "Round of 32"},
    {"uefa_cup", 30, 2004, 4, "Rnd of 32"},
    {"uefa_cup", 40, 2004, 3, "Round of 16"},
    {"uefa_cup", 40, 2004, 4, "Rnd of 16"},
    {"uefa_cup", 250, 2004, 3, "First Qualifying Round"},
    {"uefa_cup", 260, 2004, 3, "Second Qualifying Round"},
    {"european_cup", 220, 2009, 3, "Play-off Round"},
    {"european_cup", 220, 2009, 4, "Play-off Rnd"},
    {"european_cup", 240, 2018, 3, "Preliminary Round"},
    {"european_cup", 240, 2018, 4, "Prelim. Rnd"},
    {"european_cup", 230, 2018, 3, "Preliminary Round Final"},
    {"european_cup", 230, 2018, 4, "Prelim. Final"},
    {"uefa_cup", 270, 2009, 3, "Third Qualifying Round"},
    {"uefa_cup", 220, 2009, 3, "Play-off Round"},
    {"uefa_cup", 220, 2009, 4, "Play-off Rnd"},
    {"uefa_cup", 240, 2018, 3, "Preliminary Round"},
    {"uefa_cup", 240, 2018, 4, "Prelim. Rnd"},
};
const int season_round_name_count = (int)(sizeof season_round_name_rows / sizeof season_round_name_rows[0]);
