/* The season change of a closed league in the running game (0.7.3 slices 6 and 7, issue 157, decision L7). */
#ifndef RECOMP_SEASON_CLOSED_STEP_H
#define RECOMP_SEASON_CLOSED_STEP_H
#include <stdint.h>

/* Applies the plan of season_closed.h to the division of record rec. A club that folds goes to no division and keeps
 * its players. A club that joins comes from the division's nation, or from abroad when the table says so
 * (season_closed_abroad): a club of the database, or the record that the data load added (season_records.c). tag names
 * the league in the log. Returns the clubs of the division after the plan. */
int season_closed_step(uint32_t rec, const char *league, const char *tag, int season, int edition, int want);

/* AusNsl::set_up_rules 0x411ce0 (the wrap calls season_sizes_rules): the clubs of the A-League's new edition. */
void season_closed_rules(uint32_t obj);

#endif
