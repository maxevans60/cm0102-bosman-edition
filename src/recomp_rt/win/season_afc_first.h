/* The first round of AFC World Cup qualifying (item 196 of 0.6.2): a two-leg knockout round of the weakest Asian
 * nations before the 40 group places (season_afc_first.c). Pure code (season_afc_first_plan.c), so ctest
 * season_afc_first links it with the FIFA days. */
#ifndef RECOMP_SEASON_AFC_FIRST_H
#define RECOMP_SEASON_AFC_FIRST_H

#include <stdint.h>

#define AFC_FIRST_MAX_POOL 96           /* Asian nations a plan holds (a database holds about 46) */

/* The plan of one cycle. pool: the Asian nations that can enter (hosts out), sorted best first by the value of
 * the pot seeding (national team reputation, club +0x80, compare 0x4b07d0). places: the group places (40 less
 * the exempt count +0xee that the draw 0x912bd0 reads through 0x912db0). */
typedef struct {
    int ties;                           /* W = pool - places, 0 when the pool fits (at most places) */
    int byes;                           /* places - W: the best, straight into the groups */
    int list[AFC_FIRST_MAX_POOL];       /* the list of the draw: for each place, the index in the sorted pool */
    int pair[AFC_FIRST_MAX_POOL][2];    /* tie k: the k-th best and the k-th weakest of the 2W (indices) */
} SeasonAfcFirstPlan;

/* Fills plan for a sorted pool of `pool` nations and `places` group places. A pool of more than 2 x places takes
 * its best 2 x places (the others are out, as the original's cap at 40). Ties: the bottom 2W of the pool, seed k
 * (the k-th best of them) against the k-th weakest. The list: the byes in pool order, then for each tie its seed
 * as a stand-in, which the tie's winner replaces on the draw day (pot 4, the last places). Returns W. */
int season_afc_first_plan(int pool, int places, SeasonAfcFirstPlan *plan);

/* The days of the round of the campaign of year c (wc_asia_league +0x40, the World Cup two years later), as days
 * from 1 January 1970: both legs on FIFA days (season_fifa_year_days) of one FIFA window, 3 to 6 days apart, the
 * first leg on or after 1 October of c and at least 8 days after today, the second leg by 10 March of c + 1 (the
 * groups are drawn on 25 March, set_up_rules 0x912eb0). The round is drawn 7 days before the first leg, not
 * before tomorrow. Returns 1, or 0 when no window fits (a game that starts after early March of c + 1). */
int season_afc_first_days(int c, long today, long *draw, long *leg1, long *leg2);

/* The day from which a campaign of year c makes its round: 15 September of c. */
long season_afc_first_build_day(int c);

#endif
