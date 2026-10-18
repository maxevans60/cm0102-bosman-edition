/* The formats of the UEFA club competitions by season (season_uefa.h). Made by
 * tools/seasons/make_uefa_table.py from docs/research/rules-0.6.0/uefa.tsv. Do not edit by hand: change the
 * table and run the tool. */
#include "season_uefa.h"

const SeasonUefa season_uefa_rows[] = {
    /* 2001/02: rows uefa-2 uefa-5 uefa-7 uefa-8 */
    {2001, 72, 6, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"Groups 2", 16}, {"KO", 8}},
     6, {{"QR", 82}, {"R1", 96}, {"R2", 48}, {"R3", 32}, {"R4", 16}, {"KO", 8}}, 0, 0, 0, 0,
     60, 5, 3, "R1",
     "Stade Louis II", "Monaco", 24, 7},
    /* 2002/03: rows uefa-10 uefa-13 uefa-15 uefa-16 */
    {2002, 72, 6, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"Groups 2", 16}, {"KO", 8}},
     4, {{"QR", 82}, {"R1", 96}, {"R2", 48}, {"R3", 32}}, 0, 0, 0, 0,
     60, 5, 3, "R1",
     "Stade Louis II", "Monaco", 30, 7},
    /* 2003/04: rows uefa-18 uefa-21 uefa-23 uefa-24 */
    {2003, 72, 5, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     4, {{"QR", 82}, {"R1", 96}, {"R2", 48}, {"R3", 32}}, 0, 0, 0, 0,
     61, 5, 3, "R1",
     "Stade Louis II", "Monaco", 29, 7},
    /* 2004/05: rows uefa-26 uefa-29 uefa-31 uefa-32 */
    {2004, 72, 5, {{"Q1", 20}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 50}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1, 3,
     61, 5, 3, "R1",
     "Stade Louis II", "Monaco", 27, 7},
    /* 2005/06: rows uefa-34 uefa-37 uefa-39 uefa-40 */
    {2005, 74, 5, {{"Q1", 24}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 50}, {"Q2", 62}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1, 3,
     61, 5, 3, "R1",
     "Stade Louis II", "Monaco", 26, 7},
    /* 2006/07: rows uefa-42 uefa-45 uefa-47 uefa-48 */
    {2006, 73, 5, {{"Q1", 22}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 70}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1, 3,
     49, 3, 11, "Q2",
     "Stade Louis II", "Monaco", 25, 7},
    /* 2007/08: rows uefa-50 uefa-53 uefa-55 uefa-56 */
    {2007, 76, 5, {{"Q1", 28}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 74}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1, 3,
     50, 3, 11, "Q2",
     "Stade Louis II", "Monaco", 31, 7},
    /* 2008/09: rows uefa-58 uefa-61 uefa-63 uefa-64 */
    {2008, 76, 5, {{"Q1", 28}, {"Q2", 28}, {"Q3", 32}, {"Groups", 32}, {"KO", 16}},
     5, {{"Q1", 74}, {"Q2", 64}, {"R1", 80}, {"Groups", 40}, {"KO", 32}}, 8, 5, 1, 3,
     50, 3, 11, "Q2",
     "Stade Louis II", "Monaco", 29, 7},
    /* 2009/10: rows uefa-66 uefa-69 uefa-71 uefa-72 */
    {2009, 76, 8, {{"Q1", 4}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 46}, {"Q2", 80}, {"Q3", 70}, {"PO", 76}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Stade Louis II", "Monaco", 28, 7},
    /* 2010/11: rows uefa-74 uefa-77 uefa-79 uefa-80 */
    {2010, 76, 8, {{"Q1", 4}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 52}, {"Q2", 80}, {"Q3", 70}, {"PO", 74}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Stade Louis II", "Monaco", 27, 7},
    /* 2011/12: rows uefa-82 uefa-85 uefa-87 uefa-88 */
    {2011, 76, 8, {{"Q1", 4}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 50}, {"Q2", 80}, {"Q3", 70}, {"PO", 76}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Stade Louis II", "Monaco", 26, 7},
    /* 2012/13: rows uefa-90 uefa-93 uefa-95 uefa-96 */
    {2012, 76, 8, {{"Q1", 6}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 8}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 74}, {"Q2", 80}, {"Q3", 58}, {"PO", 62}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Stade Louis II", "Monaco", 31, 7},
    /* 2013/14: rows uefa-98 uefa-101 uefa-103 uefa-104 */
    {2013, 76, 8, {{"Q1", 4}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 76}, {"Q2", 80}, {"Q3", 58}, {"PO", 62}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Fortuna Arena (Eden Arena)", "Prague", 30, 7},
    /* 2014/15: rows uefa-106 uefa-109 uefa-111 uefa-112 */
    {2014, 77, 8, {{"Q1", 6}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 78}, {"Q2", 80}, {"Q3", 58}, {"PO", 62}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Cardiff City Stadium", "Cardiff", 12, 7},
    /* 2015/16: rows uefa-114 uefa-117 uefa-119 uefa-120 */
    {2015, 78, 8, {{"Q1", 8}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 102}, {"Q2", 66}, {"Q3", 58}, {"PO", 44}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Boris Paichadze Dinamo Arena", "Tbilisi", 11, 7},
    /* 2016/17: rows uefa-122 uefa-125 uefa-127 uefa-128 */
    {2016, 78, 8, {{"Q1", 8}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 96}, {"Q2", 66}, {"Q3", 58}, {"PO", 44}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Lerkendal Stadion", "Trondheim", 9, 7},
    /* 2017/18: rows uefa-130 uefa-133 uefa-135 uefa-136 */
    {2017, 79, 8, {{"Q1", 10}, {"Q2", 34}, {"Q3 CP", 20}, {"Q3 LP", 10}, {"PO CP", 10}, {"PO LP", 10}, {"Groups", 32}, {"KO", 16}},
     6, {{"Q1", 100}, {"Q2", 66}, {"Q3", 58}, {"PO", 44}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Philip II Arena", "Skopje", 8, 7},
    /* 2018/19: rows uefa-138 uefa-141 uefa-143 uefa-144 */
    {2018, 79, 10, {{"PR", 4}, {"Q1", 32}, {"Q2 CP", 20}, {"Q2 LP", 4}, {"Q3 CP", 12}, {"Q3 LP", 8}, {"PO CP", 8}, {"PO LP", 4}, {"Groups", 32}, {"KO", 16}},
     10, {{"PR", 14}, {"Q1", 94}, {"Q2 CP", 18}, {"Q2 MP", 74}, {"Q3 CP", 20}, {"Q3 MP", 52}, {"PO CP", 16}, {"PO MP", 26}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Lillek\374la Stadium", "Tallinn", 15, 7},
    /* 2019/20: rows uefa-146 uefa-149 uefa-151 uefa-152 */
    {2019, 79, 10, {{"PR", 4}, {"Q1", 32}, {"Q2 CP", 20}, {"Q2 LP", 4}, {"Q3 CP", 12}, {"Q3 LP", 8}, {"PO CP", 8}, {"PO LP", 4}, {"Groups", 32}, {"KO", 16}},
     10, {{"PR", 14}, {"Q1", 94}, {"Q2 CP", 18}, {"Q2 MP", 74}, {"Q3 CP", 20}, {"Q3 MP", 52}, {"PO CP", 16}, {"PO MP", 26}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Vodafone Park", "Istanbul", 14, 7},
    /* 2020/21: rows uefa-154 uefa-157 uefa-159 uefa-160 */
    {2020, 79, 10, {{"PR", 4}, {"Q1", 34}, {"Q2 CP", 20}, {"Q2 LP", 6}, {"Q3 CP", 10}, {"Q3 LP", 6}, {"PO CP", 8}, {"PO LP", 4}, {"Groups", 32}, {"KO", 16}},
     10, {{"PR", 16}, {"Q1", 94}, {"Q2 CP", 20}, {"Q2 MP", 72}, {"Q3 CP", 18}, {"Q3 MP", 52}, {"PO CP", 16}, {"PO MP", 26}, {"Groups", 48}, {"KO", 32}}, 12, 4, 2, 2,
     -1, 0, 0, "",
     "Pusk\341s Ar\351na", "Budapest", 24, 8},
};
const int season_uefa_row_count = (int)(sizeof season_uefa_rows / sizeof season_uefa_rows[0]);
