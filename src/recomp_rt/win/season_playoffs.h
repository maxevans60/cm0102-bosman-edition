/* Promotion play-offs in leagues whose class has none (0.6.0 slice 19). See season_playoffs.c. */
#ifndef RECOMP_SEASON_PLAYOFFS_H
#define RECOMP_SEASON_PLAYOFFS_H

#include <stdint.h>

/* The wrap of the empty function 0x5a8f60 (advance_stage, season_uefa_cup.c) calls it first: 1 when obj is a
 * league of this kind whose table is complete and the play-off stage is built now (the wrap then returns). */
int season_playoffs_advance(uint32_t obj);

/* 1 when the rules row of file and season gives a play-off that this kind plays (4 places in a row just below
 * the direct places); *direct (may be NULL) gets the direct promotion places. */
int season_playoffs_shape(const char *file, int season, int *direct);

/* After set_up_rules of a league (season_sizes_rules calls it): the places and the stage count of the play-off
 * of the season, or none. */
void season_playoffs_rules(uint32_t obj);

#endif
