/* New divisions (0.6.0 slice 20, spec docs/specs/2026-10-16-seasons-0.6.0-spec.md, Q290, Q291, Q331, Q363).
 * Research and evidence: docs/research/2026-10-20-new-divisions.md.
 *
 * A new division that replaces a division of the original lives in the object of that division (Q363: it takes
 * the extra division flag of the division it replaces). Only a season game (season_game_changes) gets any of
 * it. The base game and RECOMP_SEASON_CODE=off keep the original. RECOMP_NEW_DIVISIONS=off turns it off in a
 * season game (a test switch).
 *
 * Germany (20a): from 2008/09 the German Regional object (GerRegional, made only with the extra division flag,
 * create_german_comps 0x8343f0 at 0x8344d5) holds the 3. Liga: one table of 20 clubs, no South group. */
#ifndef RECOMP_SEASON_DIVISIONS_H
#define RECOMP_SEASON_DIVISIONS_H

#include <stdint.h>

/* The club count that GerRegional::create_table stores (0x5dd801, 0x5dd809; rt.h declares it for the
 * translated code). */
uint32_t rt_ger_regional_clubs(uint32_t obj, uint32_t original);

/* After the set_up_rules of a league (season_sizes_rules): the places towards a new division. */
void season_divisions_rules(uint32_t obj);

/* Club moves at a new game (season_clubs.c). The count of a database division in a season, or -1 for the
 * class count of season_clubs.c (0 empties the division: its clubs go to no division, Q290). */
int season_divisions_class_size(const char *division, int season);
/* 1 when the club list of a division in a season applies (the 3. Liga list only with the extra division flag). */
int season_divisions_list_on(const char *division, int season);

#endif
