/* Closed leagues: the clubs that fold and join in their real seasons (0.7.3 slice 6, issue 157, decision L7).
 *
 * A closed league has no division below, so the size step cannot bring it to the count of its row. This table names
 * the clubs of each closed league and the seasons that each one plays. The plan below works on plain values, so a
 * ctest case proves it (tests/seasons/test_season_closed.c). The league's own runtime applies the plan at its season
 * change (season_mls.c). The seasons are the real seasons of the league's editions. */
#ifndef RECOMP_SEASON_CLOSED_H
#define RECOMP_SEASON_CLOSED_H

/* One club of the league's nation, as the runtime reads it. */
typedef struct {
    const char *name;
    int in_league;      /* 1 when the club plays in the league in the season that ended */
    int human;          /* 1 when a human manages it: it keeps its league */
    int reserve;        /* 1 for a reserve or B side: it never joins */
    int reputation;
} ClosedClub;

enum { CLOSED_STAY = 0, CLOSED_JOIN = 1, CLOSED_FOLD = 2, CLOSED_CUT = 3 };

typedef struct {
    int joined, folded, cut;
    int human_stays;    /* clubs that the table folds and that stay because a human manages them */
    int missing;        /* clubs of the season that no club of the data can stand for */
    int count;          /* the clubs of the league after the plan */
} ClosedResult;

/* The closed league of a database division ("usa_mls" for "American Major League"), or NULL. */
const char *season_closed_league(const char *division);

/* The table row that has this name of a club, or -1. Two names of one club give the same row. */
int season_closed_row(const char *league, const char *name);

/* 1 when the club plays in the league in the season, 0 when the table names it for other seasons only, -1 when the
 * table does not name it. */
int season_closed_member(const char *league, const char *name, int season);

/* The clubs that the table names for the season. */
int season_closed_count(const char *league, int season);

/* The real name of table row i of the league, or NULL after the last row. */
const char *season_closed_real(const char *league, int i);

/* The season that row i of the league joins (its first season), 0 after the last row. */
int season_closed_first(const char *league, int i);

/* The plan of a season change. actions[i] takes the CLOSED_ value of clubs[i]. want is the count of the league's row
 * (0 or less: no count). Clubs that the table folds leave first. Then each club of the season that the league lacks
 * joins. A club that the table does not name leaves only while the league holds more clubs than want, the lowest
 * reputation first. A club with a human manager never leaves. */
void season_closed_plan(const char *league, int season, int want, const ClosedClub *clubs, int n, int *actions,
                        ClosedResult *result);

#endif
