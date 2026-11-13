/* The structured league rules of the rules table: play-off places, splits and calendars (0.6.0 slice 19), and
 * the structure of each league row: meetings, halves, zones, periods, split sections (0.7.2 league sizes, slice 1).
 *
 * tools/seasons/encode_leagues.py reads the playoffs, split and real_season texts of the league rows of the
 * rules table and writes season_league_table.c. Its docstring gives the words; the research of the original's
 * play-off and split code is in docs/research/2026-10-18-playoffs-splits-calendar.md. */
#ifndef RECOMP_SEASON_LEAGUE_H
#define RECOMP_SEASON_LEAGUE_H

#define SEASON_LEAGUE_PARTS 4   /* zones of a division, sections of a split */

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
    /* The structure of the row (0.7.2 league sizes, slice 1). 0 for a row of a file that is not a league. */
    int meetings;           /* times each pair meets in the table (in each half, zone, group, before a split);
                             * 0: the class's own meetings stay */
    int halves;             /* 1: two halves (Apertura and Clausura, two stages), each a table of the meetings */
    int zones;              /* zones of one division, 0 for none */
    int zone_size[SEASON_LEAGUE_PARTS]; /* the clubs of each zone */
    int extra_rounds;       /* rounds beyond meetings x (n' - 1) (derby, inter-zone rounds); -1 unknown */
    int periods;            /* period titles of the season; 0 none, -1 unknown */
    int split_size[SEASON_LEAGUE_PARTS];     /* the clubs of each section of the split (split_size[0] = split_top) */
    int split_meetings[SEASON_LEAGUE_PARTS]; /* the meetings in each section; 0 unknown */
    int rounds;             /* rounds of the table (of a half, of a zone): meetings x (n' - 1) + extra_rounds, n'
                             * the count (the largest zone) rounded up to even; 0 unknown */
} SeasonLeagueRule;

extern const SeasonLeagueRule season_league_rules[];
extern const int season_league_rule_count;

/* The encoded row of a file and season, or NULL (season_playoffs.c). */
const SeasonLeagueRule *season_league_rule(const char *file, int season);

/* The meetings of a league in a season: the row of that season, or by the after-table rule the last row before
 * it (Q248). A row with unknown clubs (a future season with no announcement) takes the meetings of the last row
 * before it that has them. 0 when no row gives meetings: the class's own meetings stay. Reads only the table:
 * no game state. */
int season_league_meetings(const char *file, int season);

#endif
