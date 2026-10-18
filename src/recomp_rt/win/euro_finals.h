/* The fill of a short European Championship finals list (euro_finals.c, research
 * docs/research/2026-10-18-staff-reserve-boxes.md). */
#ifndef RECOMP_EURO_FINALS_H
#define RECOMP_EURO_FINALS_H

#include <stdint.h>

/* Fills the team list of the EuroChamp at ec before EuroChamp::seed_teams 0x57c710, only when the list holds
 * fewer teams than places. A wrap of 0x57c710 calls it before the original. */
void euro_finals_fill_short_list(uint32_t ec);

#endif
