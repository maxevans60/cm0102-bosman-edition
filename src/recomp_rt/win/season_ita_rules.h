/* The Italian rules of the league sizes that depend only on numbers (0.7.2 gap fixes, rows 30 and 32 of the size
 * gaps; season_sizes_ita.c applies them to the game). */
#ifndef SEASON_ITA_RULES_H
#define SEASON_ITA_RULES_H

/* The last season of the tie-break match (spareggio) of Serie B. From 2005/06 the head-to-head table (classifica
 * avulsa) settles a tie on points (it.wikipedia "Incontri di spareggio del campionato italiano di calcio",
 * "Serie B 2005-2006"). */
#define ITA_SPAREGGIO_LAST 2004

/* The tie-break zone of row `row` of a Serie B table of `clubs` rows in the season of `year` with `up` direct
 * promotion places and `down` direct relegation places (+0xbe, +0xc1): 1 the champion, 2 the other promotion places,
 * 3 the relegation places, 0 none. ItaSerB::update_for_date 0x647050 and create_tiebreak_playoff_stage 0x646bb0
 * start a tie-break when two rows with equal points have different zones. From 2005/06 every row has 0. */
int ita_serie_b_zone(int year, int clubs, int up, int down, int row);

/* A club as compare_clubs_by_seeding_pot 0x640590 reads it: its last division (+0x5b), that division against Serie B
 * ([0x9cf574]) and Serie A ([0x9cf570]), its division now (+0x57) against Serie B, whether the two divisions are one
 * record, and its last place (+0x5f, s8). */
typedef struct {
    int has_last, last_is_b, last_is_a, cur_is_b, last_is_cur, place;
} ItaCupClub;

/* The seeding pot of a club in the Coppa Italia (0x640590): 4 down from Serie A into Serie B or Serie B 3rd to 6th,
 * 3 Serie B 7th to 14th, 2 Serie B 15th to the 3rd-last or now in Serie B from below, 1 any other. The original
 * ends the third range at 18th, the 3rd-last of its 20 (0x6405fe, 0x64068e); `b_count` is the count of the last
 * Serie B (0 or 20: the original's 18th). */
int ita_cup_pot(const ItaCupClub *club, int b_count);

/* The count of the last Serie B that ita_cup_pot takes: `scanned` is the last place of the clubs whose last division
 * is Serie B. In the start season of a game (`season` <= `start`) the last places come from the database, and a
 * season database can hold part of the table (a 2004 start found 17, a 2005 start 18, against the real 24 and 22):
 * a count under 20 there gives 0, the original's 18th. A count over 20 there, or any count in a later season,
 * which the game played, stays. */
int ita_cup_b_count(int scanned, int season, int start);

/* The row (0 = 1st) of a C1 group of `clubs` that takes the play-out place of row `row18` of the class's 18
 * (ItaSerC1a::create_playoff_stages 0x64ba70 puts rows 16, 13, 15 and 14 in the play-out: 17th v 14th, 16th v 15th,
 * 0x64bc2c..0x64bc70). The play-out is the four rows above the last row at any count, the last row going down
 * directly: 2004/05 C1/A of 19 played 15th v 18th and 16th v 17th, 2012/13 groups of 17 and 16 played 13th v 16th,
 * 14th v 15th and 12th v 15th, 13th v 14th (it.wikipedia "Serie C1 2004-2005", "Lega Pro Prima Divisione
 * 2012-2013"; en.wikipedia "2004-05 Serie C1", "2012-13 Lega Pro Prima Divisione"). A row outside the table, or a
 * group under 6, keeps row18. */
int ita_c1_playout_row(int clubs, int row18);

#endif
