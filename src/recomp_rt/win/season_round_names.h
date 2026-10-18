/* Real round names by competition and season (0.6.0, Q370; first rows slice 22, more rows slices 23 to 29).
 *
 * The game names a round from its round code (+7 of a round record) with the round name function 0x4b4850,
 * which knows no competition and no season. The runtime wraps it (season_round_names_game.c): in a season
 * game, when the round's competition is known, a row of this table gives the real name of that season, and the
 * code stays as it is. The base game and RECOMP_SEASON_CODE=off get the original's text, and so does a set or a
 * season with no row.
 *
 * The table: docs/research/round-names.tsv (one row per name, with its source), made into
 * season_round_names_table.c by tools/seasons/make_round_names.py (ctest season_round_names_table). */
#ifndef RECOMP_SEASON_ROUND_NAMES_H
#define RECOMP_SEASON_ROUND_NAMES_H

#include <stdint.h>

/* The three name sets of 0x4b4850, by its flag argument: 0 the long set (0x4b48d2, "Quarter Final"), 3 the
 * plural set (0x4b6937, "Quarter Finals"), any other value the abbreviated set (0x4b7a84, "Qtr Final"). */
#define SEASON_ROUND_NAME_LONG 1
#define SEASON_ROUND_NAME_PLURAL 2
#define SEASON_ROUND_NAME_ABBREVIATED 4

typedef struct {
    const char *comp_file;          /* the source file of the original's class: european_cup */
    int round_code;                 /* a multiple of 10 */
    int first_season;               /* the start year of the first season with the name */
    int sets;                       /* SEASON_ROUND_NAME_* joined */
    const char *name;               /* in the game's one-byte code; NULL: the original's name again */
} SeasonRoundName;

extern const SeasonRoundName season_round_name_rows[];
extern const int season_round_name_count;

/* The real name of round code round_code (a multiple of 10) of competition file comp_file in season season,
 * in name set which_set: the row with the latest first season up to season, or NULL (the original's name, also
 * after a row that ends a name). */
const char *season_round_name(const char *comp_file, int round_code, int season, int which_set);

/* The game only (season_round_names_game.c): the label of round code code of competition object obj through
 * the label function 0x4bfa30 with style (1 long, 3 abbreviated), for the rules report. */
void season_round_name_label(uint32_t obj, int code, int style, char *out, int size);

#endif
