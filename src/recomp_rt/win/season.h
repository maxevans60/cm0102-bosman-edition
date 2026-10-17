/* The season of a game (0.6.0 slice 11): its start year and its changing rules switch.
 *
 * The original starts every game in the 2001/02 season. Here the start year is a run-time value
 * (rt_season_year in rt.h). The year sites of tools/recomp/seasons.py read it in place of the constant
 * 2001, so a game can start in July of another year. With start year 2001 every site gives the
 * original's result.
 *
 * The values of the game that runs:
 * - a new game takes the values of the last season_set_new_game call (start year 2001 and the option
 *   changing_rules before any call, or the year of the test variable RECOMP_START_SEASON);
 * - a loaded game takes the values of the season part of its save (bosman_season.dat). A save without the
 *   part, as every save of 0.5.0 and earlier and every base game save, is 2001 with changing rules off.
 *   A damaged part stops the load with a message box. A part of a later version loads with the fields
 *   that this version knows;
 * - at the end of a game (free_game_data 0x82ff40) the values go back to those of a new game.
 * The save writes the part only when the start year is not 2001 or changing rules is on. When it cannot,
 * the save reports a failure with a message box. The format and the
 * proof that the original skips the part are in docs/research/2026-10-17-season-save-part.md. */
#ifndef RECOMP_SEASON_H
#define RECOMP_SEASON_H

#define SEASON_FIRST_YEAR 2001       /* the start year of the original, the base game */
#define SEASON_LAST_YEAR 2099        /* the latest start year that the part and the test variable accept */

/* 0 with the test switch RECOMP_SEASON_CODE=off (Q342): the season code does nothing (season.c). */
int season_code_on(void);

/* 1 from a made or loaded game to its end (free_game_data 0x82ff40): a game runs. */
int season_game_on(void);

/* Reads the test variable RECOMP_START_SEASON. Call it once at start, before the game runs. */
void season_init(void);

/* The start year of the game that runs, or of the next new game on the start screen. */
int season_start_year(void);

/* 1 when the season rules of the game that runs change each July (option changing_rules of that game). */
int season_changing_rules(void);

/* Sets the season of the next new game. Call it before Start New Game (the Choose Season screen, slice 13).
 * start_year: SEASON_FIRST_YEAR to SEASON_LAST_YEAR, else the call does nothing and returns 0.
 * changing_rules: 1 or 0, or -1 for the option changing_rules at the moment the game is made.
 * On the start screen it also sets the game date to 1 January of the start year, as the original does
 * for 2001 (0x81b938). Returns 1. */
int season_set_new_game(int start_year, int changing_rules);

/* The start year of the save file at the host path, for the star of the Choose Season screen (slice 13).
 * year gets 2001 for a save without a season part. Returns 1 with a part, 0 without one (or for a file that is
 * no save), -1 for a damaged part (year stays 2001). It only reads the file. */
int season_of_save(const char *host, int *year);

/* At each human turn (testrun.c): the test log of RECOMP_SEASON_LOG=1. */
void season_log_turn(void);

#endif
