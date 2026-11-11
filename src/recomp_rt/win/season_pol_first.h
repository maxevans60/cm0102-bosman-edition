/* The Ekstraklasa in its real format of each season (season_pol_first.c, GitHub issue 92). */
#ifndef RECOMP_SEASON_POL_FIRST_H
#define RECOMP_SEASON_POL_FIRST_H

#include <stdint.h>

/* The club count of the Polish First Division at the club moves of a new game in a season: the rules table row's
 * count when that season plays one table (with changing rules on), else -1 (the class's 16). */
int season_pol_first_clubs(int season);

#endif
