/* The structured league rules of the rules table: play-off places, splits and calendars (0.6.0 slice 19).
 *
 * tools/seasons/encode_leagues.py reads the playoffs, split and real_season texts of the league rows of the
 * rules table and writes season_league_table.c. Its docstring gives the words; the research of the original's
 * play-off and split code is in docs/research/2026-10-18-playoffs-splits-calendar.md. */
#ifndef RECOMP_SEASON_LEAGUE_H
#define RECOMP_SEASON_LEAGUE_H

typedef struct {
    const char *file;       /* comp_file of the row: eng_first */
    int season;             /* start year of the row: 2001 for 2001/02 */
    int po_up;              /* promotion play-off places of the division (0 for none) */
    int po_up_first;        /* the first of them (3 for 3rd to 6th), 0 for none */
    int po_down;            /* relegation play-off places */
    int exact;              /* 1: the tool read the whole text; 0: only a part (the gaps file says why) */
    int split_after;        /* a split after this many matches, 0 for none */
    int split_top;          /* the clubs of the top group of the split */
    int calendar;           /* 0 a season over two years, 1 a calendar year, 2 a transition row */
    int split_points;       /* 0 points kept, 1 halved, 2 unknown */
} SeasonLeagueRule;

extern const SeasonLeagueRule season_league_rules[];
extern const int season_league_rule_count;

#endif
