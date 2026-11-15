/* Italy at its real league sizes (0.7.2 league sizes, slice 9): the code of the Italian classes that reads a fixed
 * count (season_sizes_ita.c). */
#ifndef RECOMP_SEASON_SIZES_ITA_H
#define RECOMP_SEASON_SIZES_ITA_H

#include <stdint.h>

/* Around the original's ita_ser_a_regroup_serie_c1_clubs 0x642630 (the wrap in season_divisions_ita.c, outside the
 * Serie C era): the regroup takes every club of C1/A and C1/B and gives C1/B the count of its next row. */
/* Returns 0 when the regroup has nothing to do (no club in C1/A or C1/B): the wrap then skips the original. */
int season_sizes_ita_regroup_begin(uint32_t serie_a);
void season_sizes_ita_regroup_end(void);

/* Before ItaCup::select_entrants 0x63faf0 (its wrap in season_divisions_ita.c): notes the count of the last Serie B,
 * the largest last place (+0x5f) of a club whose last division is Serie B, for its sort 0x640590 (gap row 32). */
void season_sizes_ita_cup_pick(int season);

#endif
