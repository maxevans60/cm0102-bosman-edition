/* The formats of the UEFA club competitions by season (0.6.0 slice 22; spec
 * docs/specs/2026-10-16-seasons-0.6.0-spec.md, slice 22; research docs/research/2026-10-17-uefa-formats.md).
 *
 * The table (season_uefa_table.c, made by tools/seasons/make_uefa_table.py from
 * docs/research/rules-0.6.0/uefa.tsv) holds one row per season: the rounds of the Champions League, of the UEFA
 * Cup and of the Intertoto Cup. Slices 23 to 25 add their seasons as rows of the same table. The code reads a
 * format from the row; it does not test a year.
 *
 * The row of a season S for the game that runs (season_uefa_row): NULL for the base game (start year 2001,
 * changing rules off) and with RECOMP_SEASON_CODE=off, so the original runs as it is. With changing rules on,
 * the row of S; with changing rules off, the row of the start year (Q232). Before the first row the first row
 * applies, after the last row the last row (Q248). */
#ifndef RECOMP_SEASON_UEFA_H
#define RECOMP_SEASON_UEFA_H

#include <stdint.h>

#define SEASON_UEFA_ROUNDS 12

typedef struct {
    const char *name;               /* QR, PR, Q1, Q2, Q3, PO, R1 to R4, Groups, Groups 2, KO; a qualifying round
                                     * of two paths (2009/10 on) is two rounds: "Q3 CP" and "Q3 LP" (Champions
                                     * League: Champions Path, League Path), "Q2 CP" and "Q2 MP" (Europa League:
                                     * Champions Path, main path) */
    int clubs;
} SeasonUefaRound;

typedef struct {
    int season;                     /* the start year: 2003 for 2003/04 */
    int cl_clubs;                   /* Champions League clubs in all */
    int cl_round_count;
    SeasonUefaRound cl_rounds[SEASON_UEFA_ROUNDS];
    int uc_round_count;
    SeasonUefaRound uc_rounds[SEASON_UEFA_ROUNDS];
    int uc_groups, uc_group_clubs, uc_meetings;     /* UEFA Cup group stage, 0 when it has none */
    int uc_through;                 /* the clubs of a UEFA Cup group that go to the knockout (2 or 3) */
    int uic_clubs;                  /* Intertoto Cup clubs, -1 when not held */
    int uic_rounds, uic_winners;
    const char *uic_entry;          /* the UEFA Cup round of its winners: R1 or Q2 */
    const char *sc_stadium;         /* the Super Cup match: stadium and city (Windows-1252), day, month 0 to 11 */
    const char *sc_city;
    int sc_day, sc_month;
} SeasonUefa;

extern const SeasonUefa season_uefa_rows[];
extern const int season_uefa_row_count;

/* The row of season S in the table alone, with the first and last row for a season outside it. */
const SeasonUefa *season_uefa_table_row(int season);

/* The row of season S for the game that runs, or NULL: the original's formats (see above). */
const SeasonUefa *season_uefa_row(int season);

/* The clubs of a named round of a row's Champions League or UEFA Cup, 0 when the row has no such round. */
int season_uefa_cl_round(const SeasonUefa *r, const char *name);
int season_uefa_uc_round(const SeasonUefa *r, const char *name);

/* The clubs of a round of all paths: "Q3" gives "Q3" or "Q3 CP" + "Q3 LP" (+ "Q3 MP"). */
int season_uefa_cl_level(const SeasonUefa *r, const char *name);
int season_uefa_uc_level(const SeasonUefa *r, const char *name);

/* 1 when the Champions League of row r has one group stage and a knockout round of 16 (2003/04 on). */
int season_uefa_cl_round_of_16(const SeasonUefa *r);

/* The rules report (season_report.c): writes the uefa lines of one season and phase. */
void season_uefa_report(void *file, int season, const char *phase);

#endif
