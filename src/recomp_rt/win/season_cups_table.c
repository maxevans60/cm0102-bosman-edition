/* The round formats of the domestic cups by season (season_cups.h). Made by tools/seasons/make_cups_table.py
 * from tools/seasons/cup_formats.tsv. Do not edit by hand: change the table and run the tool. */
#include "season_cups.h"

const SeasonCupRow season_cup_rows[] = {
    {"bel_fa_cup", 2002, 8, ".....LLE", "The Belgian Cup quarter-finals and semi-finals are over two legs. The final is a single match."},
    {"bel_fa_cup", 2015, 8, ".....ELE", "The Belgian Cup quarter-finals are single matches again. Only the semi-finals have two legs."},
    {"bel_fa_cup", 2020, 8, ".....EEE", "The Belgian Cup semi-finals are single matches this season."},
    {"bel_fa_cup", 2021, 8, ".....ELE", "The Belgian Cup semi-finals are over two legs again."},
    {"eng_cc_cup", 2018, 7, "PPPPPQE", "League Cup ties are now settled by penalties after 90 minutes before the final. The semi-finals are over two legs with no away goals rule and penalties after the second leg."},
    {"eng_cc_cup", 2020, 7, "PPPPPPE", "The League Cup semi-finals are single matches this season. Only the final has extra time."},
    {"eng_cc_cup", 2021, 7, "PPPPPQE", "The League Cup semi-finals are over two legs again, with no away goals rule and penalties after the second leg."},
    {"eng_fa_cup", 2016, 9, "RRRRRREEE", "FA Cup replays now stop after the fifth round. The quarter-finals are settled on the day."},
    {"eng_fa_cup", 2018, 9, "RRRRREEEE", "FA Cup replays now stop after the fourth round."},
    {"eng_fa_cup", 2020, 9, "EEEEEEEEE", "The FA Cup has no replays this season. Level ties go to extra time and penalties."},
    {"eng_fa_cup", 2021, 9, "RRREEEEEE", "The FA Cup brings back replays in the first and second rounds only."},
    {"eng_fa_cup", 2022, 9, "RRRRREEEE", "FA Cup replays now run up to the fourth round."},
    {"eng_fa_cup", 2024, 9, "REEEEEEEE", "FA Cup ties are now settled on the day from the first round proper. Replays stay in the qualifying rounds only."},
    {"eng_fa_trophy", 2020, 5, "PPPPE", "FA Trophy ties are now single matches settled by penalties after 90 minutes. The final has extra time first."},
    {"fra_cup", 2020, 9, "PPPPPPPPE", "Coupe de France ties are now settled by penalties after 90 minutes. Only the final has extra time."},
    {"fra_lge_cup", 2016, 7, "PPPPPPE", "Coupe de la Ligue ties are now settled by penalties after 90 minutes. Only the final has extra time."},
    {"hol_cup", 2009, 6, ".....L", "The KNVB Cup final is played over two legs this season."},
    {"hol_cup", 2010, 6, ".....E", "The KNVB Cup final is a single match again."},
    {"ita_cup", 2005, 5, "ELLLL", "The Coppa Italia third round is now a single match. The later rounds keep two legs."},
    {"ita_cup", 2007, 5, "ELLLE", "The Coppa Italia final is now a single match."},
    {"ita_cup", 2008, 5, "EEELE", "Coppa Italia ties are now single matches up to the quarter-finals. Only the semi-finals have two legs."},
    {"ita_cup", 2024, 5, "PPPLE", "Coppa Italia ties before the semi-finals now go straight to penalties after 90 minutes."},
    {"nor_cup", 2002, 7, "EEEEEEE", "Norwegian Cup ties are now settled on the day. Level ties go to extra time and penalties."},
    {"sco_fa_cup", 2007, 7, "RRRRREE", "Scottish Cup replays now stop after the quarter-finals. The semi-finals are settled on the day."},
    {"sco_fa_cup", 2020, 7, "EEEEEEE", "The Scottish Cup has no replays. Level ties go to extra time and penalties."},
    {"sco_fa_cup", 2021, 7, "SSEEEEE", "The Scottish Cup brings back replays in the early rounds. A level replay goes straight to penalties."},
    {"sco_fa_cup", 2022, 7, "EEEEEEE", "The Scottish Cup has no replays again. Level ties go to extra time and penalties."},
    {"spa_cup", 2019, 7, "EEEEELE", "Copa del Rey ties are now single matches up to the quarter-finals. Only the semi-finals have two legs."},
    {"spa_cup", 2021, 7, "EEEEENE", "The Copa del Rey semi-finals no longer use the away goals rule."},
};
const int season_cup_row_count = 29;
