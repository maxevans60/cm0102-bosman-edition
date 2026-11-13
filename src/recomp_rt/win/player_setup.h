/* The dates of a season database's player_setup.cfg in a season game (open item 229, player_setup.c). */
#ifndef RECOMP_PLAYER_SETUP_H
#define RECOMP_PLAYER_SETUP_H

#include <stdint.h>

#include "season_dates.h"

/* At Date::Date 0x549f70: ret is the call's return address, args the address of its first argument (day, then
 * month from 0, then year, each a dword). Moves the year when the call is one of the four moved Dates of
 * player_setup_apply_line 0x7a3a10 in a season game whose data set has its own player_setup.cfg. */
void player_setup_date_args(uint32_t ret, uint32_t args);

/* The year that a date of the file takes under plan p (*out_day: the day, 28 for a 29 February that lands in a
 * year that is not a leap year). For the tests. */
int player_setup_moved_date(int day, int month0, int year, const SeasonShiftPlan *p, int *out_day);

#endif
