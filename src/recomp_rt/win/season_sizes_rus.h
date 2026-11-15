/* The Russian First Division at its real count (0.7.2 league sizes, slice 12; spec
 * docs/specs/2026-11-13-league-sizes-0.7.2-spec.md, D22). season_sizes_rus.c, tools/recomp/sizes_rus_tur.py. */
#ifndef RECOMP_SEASON_SIZES_RUS_H
#define RECOMP_SEASON_SIZES_RUS_H

#include <string.h>

/* RusFirst::create_table 0x7ea6c0 takes the clubs of the division into a stack array of 18 pointers at esp + 0x10
 * (`cmp bl, 0x12` at 0x7ea77d, store at 0x7ea787). The array runs into the text buffer at esp + 0x58, which only the
 * error path of a failed allocation uses (0x7ea6eb..0x7ea741, before the loop). The next buffer starts at
 * esp + 0x158 (frame 0x248, sub esp at 0x7ea6c0), so 22 pointers (to esp + 0x68) are safe. */
#define RUS_FIRST_TAKE 18
#define RUS_FIRST_TAKE_MAX 22

/* The bound of the take loop for a table of `count` clubs: the original's 18, or the count when it is larger, up to
 * RUS_FIRST_TAKE_MAX. A smaller count keeps 18: the loop takes the clubs of the division up to 18, orders them
 * (0x4afbd0: a qsort, then random swaps) and adds the first `count` (0x7ea7bf), so a division of `count` clubs still
 * enters in full. */
static inline int season_rus_first_take(int count)
{
    if (count <= RUS_FIRST_TAKE) return RUS_FIRST_TAKE;
    return count > RUS_FIRST_TAKE_MAX ? RUS_FIRST_TAKE_MAX : count;
}

/* 1 when two consecutive rows hold the stages of one long season (the real seasons "2011–12 (first stage, ...)" then
 * "2011–12 (second stage, ...)" of rus_first and rus_prm rows 2010 and 2011). The edition takes row 2011 (the second
 * stage: its moves and play-offs end the season), but the split of the whole season is in the first stage row: 38
 * rounds, then 8 + 12 (season_sizes_split_season). */
static inline int season_stage_rows(const char *first, const char *second)
{
    static const char a[] = " (first stage", b[] = " (second stage";
    const char *at = first ? strstr(first, a) : NULL;
    if (!at || !second) return 0;
    size_t n = (size_t)(at - first);
    return n > 0 && !strncmp(first, second, n) && !strncmp(second + n, b, sizeof b - 1);
}

#endif
