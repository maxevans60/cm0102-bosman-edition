/* Real round names by competition and season (0.6.0, Q370; first rows slice 22, more rows slices 23 to 29).
 *
 * The game names a round from its round code (+7 of a round record) with the round name function 0x4b4850,
 * which knows no competition and no season. The runtime wraps it (season_round_names_game.c): in a season
 * game, when the round's competition is known, a row of this table gives the real name of that season, and the
 * code stays as it is. The base game and RECOMP_SEASON_CODE=off get the original's text.
 *
 * The table: docs/research/round-names.tsv (one row per name, with its source), made into
 * season_round_names_table.c by tools/seasons/make_round_names.py (ctest season_round_names_table). */
#ifndef RECOMP_SEASON_ROUND_NAMES_H
#define RECOMP_SEASON_ROUND_NAMES_H

#define SEASON_ROUND_NAME_LONG 1        /* the name set of 0x4b4850 with flag 0: "Quarter Final" */
#define SEASON_ROUND_NAME_SHORT 2       /* the set with flag 1: "Quarter Finals" (0x4b6947 on) */

typedef struct {
    const char *comp_file;          /* the source file of the original's class: european_cup */
    int round_code;                 /* a multiple of 10 */
    int first_season;               /* the start year of the first season with the name */
    int sets;                       /* SEASON_ROUND_NAME_LONG, _SHORT or both */
    const char *name;               /* in the game's one-byte code */
} SeasonRoundName;

extern const SeasonRoundName season_round_name_rows[];
extern const int season_round_name_count;

/* The real name of round code round_code (a multiple of 10) of competition file comp_file in season season,
 * in name set which_set: the row with the latest first season up to season, or NULL (the original's name). */
const char *season_round_name(const char *comp_file, int round_code, int season, int which_set);

/* The game only (season_round_names_game.c): the name of round code code of competition object obj as the
 * game's formatter 0x4bffc0 writes it for a fixture, for the rules report. */
#include <stdint.h>
void season_round_name_label(uint32_t obj, int code, char *out, int size);

#endif
