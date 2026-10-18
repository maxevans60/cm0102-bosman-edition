/* The formats of the UEFA club competitions by season (season_uefa.h). Made by
 * tools/seasons/make_uefa_table.py from docs/research/rules-0.6.0/uefa.tsv. Do not edit by hand: change the
 * table and run the tool. */
#include "season_uefa.h"

const SeasonUefa season_uefa_rows[] = {
    /* 2001/02: rows uefa-2 uefa-5 uefa-7 */
    {2001, 72, 6, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"Groups 2", 16}, {"KO", 8}},
     6, {{"QR", 82}, {"R1", 96}, {"R2", 48}, {"R3", 32}, {"R4", 16}, {"KO", 8}}, 0, 0, 0,
     60, 5, 3, "R1"},
    /* 2002/03: rows uefa-10 uefa-13 uefa-15 */
    {2002, 72, 6, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"Groups 2", 16}, {"KO", 8}},
     4, {{"QR", 82}, {"R1", 96}, {"R2", 48}, {"R3", 32}}, 0, 0, 0,
     60, 5, 3, "R1"},
    /* 2003/04: rows uefa-18 uefa-21 uefa-23 */
    {2003, 72, 5, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     4, {{"QR", 82}, {"R1", 96}, {"R2", 48}, {"R3", 32}}, 0, 0, 0,
     61, 5, 3, "R1"},
    /* 2004/05: rows uefa-26 uefa-29 uefa-31 */
    {2004, 72, 5, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 50}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1,
     61, 5, 3, "R1"},
    /* 2005/06: rows uefa-34 uefa-37 uefa-39 */
    {2005, 74, 5, {{"Q1", 24}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 50}, {"Q2", 62}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1,
     61, 5, 3, "R1"},
    /* 2006/07: rows uefa-42 uefa-45 uefa-47 */
    {2006, 73, 5, {{"Q1", 22}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 70}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1,
     49, 3, 11, "Q2"},
    /* 2007/08: rows uefa-50 uefa-53 uefa-55 */
    {2007, 76, 5, {{"Q1", 28}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 74}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1,
     50, 3, 11, "Q2"},
    /* 2008/09: rows uefa-58 uefa-61 uefa-63 */
    {2008, 76, 5, {{"Q1", 28}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 74}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1,
     50, 3, 11, "Q2"},
};
const int season_uefa_row_count = (int)(sizeof season_uefa_rows / sizeof season_uefa_rows[0]);
