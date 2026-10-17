/* Right ages and dates in every season (0.6.0 slice 11b, Q344).
 *
 * The Update Team databases expect the original start in 2001, so they store their dates early by some years.
 * At the database load of a new game in another season, the game adds the shift of each field to the years in
 * memory. The database files stay unchanged. A loaded save needs nothing: it holds the shifted data.
 *
 * The fields, their evidence and the measured shift of each database are in
 * docs/research/2026-10-17-season-date-shifts.md. The table is made by tools/seasons/make_dates_table.py from
 * docs/research/rules-0.6.0/databases-date-shifts*.tsv (tools/seasons/measure_date_shifts.py measures them).
 *
 * When it runs: right after the load of the database files (database_load_files 0x524d50, which calls the
 * loader 0x528540 with the file source) has returned. The loader's own passes (the id pass 0x5315a0 and the
 * staff pass 0x52d20a with its 1998 compare at 0x52d2e1) then work on the dates as the database stores them,
 * as with a 2001 start. No other game code has read the data at that point.
 *
 * It does nothing when the start year is 2001 (every database plays as it does today), for the official data
 * sets (3.9.60, 3.9.65, 3.9.68: shift 0), and with the test switch RECOMP_SEASON_CODE=off. */
#ifndef RECOMP_SEASON_DATES_H
#define RECOMP_SEASON_DATES_H

#include <stdint.h>

/* The shifts of one known database, in years. */
typedef struct {
    const char *name;               /* the neutral name of seasons_known_dbs (seasons_db.h) */
    int8_t birth;                   /* staff date of birth, +0x10 */
    int8_t joined;                  /* staff date joined, +0x3e */
    int8_t contract;                /* staff contract end +0x46, national job dates +0x29 and +0x31 */
    int8_t officials;               /* officials date of birth, +0x0c */
    int8_t staff_history;           /* staff history year, +0x08, except the kept careers */
    int8_t comp_history;            /* club and nation competition history year, +0x08, every row */
    int8_t last_rows;               /* with comp_history 0: the shift of the 2001 rows of last_ids */
    uint16_t last_count;
    const uint16_t *last_ids;       /* competition ids (club comps, then nation comps) */
    uint32_t kept_rows;             /* the staff rows that kept_hex covers, 0 for none */
    const char *kept_hex;           /* bit i (byte i / 8, bit i % 8): the career of staff row i keeps its years */
} SeasonDateShifts;

extern const SeasonDateShifts season_date_shifts[];     /* season_dates_table.c */
extern const int season_date_shifts_count;

/* The row of a known database by its neutral name, or NULL. */
const SeasonDateShifts *season_dates_for(const char *name);

/* The shifts that the next database load applies (a test reads it): the row of a known database, a row
 * measured for a Custom database, or NULL for none. Valid after a load. */
const SeasonDateShifts *season_dates_applied(void);

/* At each human turn (testrun.c): the test log of RECOMP_DATES_LOG=<name>;<name>;... (see season_dates.c). */
void season_dates_log_turn(void);

#endif
