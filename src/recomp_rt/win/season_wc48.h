/* The World Cup of 48 teams (0.6.0 slice 29). Research and evidence: docs/research/2026-10-19-world-cup-48.md. */
#ifndef RECOMP_SEASON_WC48_H
#define RECOMP_SEASON_WC48_H

#include <stdint.h>

/* One row of Annex C of the 2026 regulations: the groups of the eight thirds that go through (bit 0 = A), and
 * the group (0 = A) of the third that meets the winner of A, B, D, E, G, I, K and L. */
typedef struct {
    uint16_t mask;
    signed char vs[8];
} SeasonWcThirds;

extern const SeasonWcThirds season_wc48_thirds[495];

/* 1 when the World Cup object plays an edition of 48 teams (a season game only). */
int season_wc48(uint32_t self);

/* The template function of the World Cup (vtable +0x3c, 0x92c290) for editions of 48 teams. The wrap of
 * 0x92c290 (season_intl.c) calls it at the entry state with the original. It completes the call. */
uint32_t season_wc48_template(void (*original)(void));

/* After the stage builder 0x92d200 (season_intl.c, after its date shift): the seeding and draw days of an
 * edition of 48 teams. */
void season_wc48_after_stages(uint32_t self);

#endif
