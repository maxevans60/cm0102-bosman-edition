/* The Choose Season screen and the season lines of Setup Game (0.6.0 slice 13, season_screen.c). */
#ifndef RECOMP_SEASON_SCREEN_H
#define RECOMP_SEASON_SCREEN_H

#include <stddef.h>
#include <stdint.h>

/* Pushes the Choose Season screen over Setup Game. */
void season_screen_open(void);

/* The build function of the screen, for the wait state "seasons" (waits.c). */
uint32_t season_screen_build_fn(void);

/* The widget draw wrap (names.c, through views_widget_drawn) calls this after the game draws widget w. */
void season_screen_widget_drawn(uint32_t w);

/* The start year of the next new game from the options database and season: 3.9.60 and 3.9.65 are 2001/02,
 * 3.9.68 is 2001/02 or 2002/03 (season), a known database of Databases is its own season and a Custom
 * database is the season that the player named (season). A missing folder is 2001/02, as files.c then loads
 * 3.9.68. It hashes a folder only the first time (seasons_db_identify). */
int season_screen_year(void);

/* The name of the database of the next new game, for Setup Game: "3.9.68 data", "October 2003" or the name
 * of a Custom database folder. */
void season_screen_database_text(char *out, size_t room);

/* The credit line of a database by its name (Q147, Q269). */
const char *season_screen_credit(const char *name);

/* The changing rules switch of Setup Game for the next new game. choice is -1 until the player clicks it:
 * then the option changing_rules applies. on gives the value that Setup Game shows. */
int season_screen_rules_choice(void);
int season_screen_rules_on(void);
void season_screen_toggle_rules(void);

#endif
