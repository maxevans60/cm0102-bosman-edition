/* League sizes, promotion and relegation in existing divisions (0.6.0 slice 18, spec
 * docs/specs/2026-10-16-seasons-0.6.0-spec.md, Q247, Q272, Q281). Research and evidence:
 * docs/research/2026-10-18-league-sizes.md.
 *
 * Only a season game (season_game_changes, slice 15) gets any of it. The base game and RECOMP_SEASON_CODE=off
 * keep the original's counts.
 *
 * The rules of a league in a season: the row of the rules table (season_rules.h) of its source file. A new
 * game, and every season of a game with changing rules off, takes the row of the start season (Q232). With
 * changing rules on a later season takes the row of the edition that the object holds (+0x40): the row whose
 * real season begins with that year (a calendar-year league holds edition Y + 1 in the row of season Y), else
 * the row of that season. After the last row the last row stays (Q248).
 *
 * - Club counts: a class holds no more clubs than its largest constant (its table and date template are made
 *   for that count, tools/recomp/league_sizes.py CAPS), and no fewer than its own count where a cup of the
 *   nation needs the whole division (FLOORS, the cups are slice 21). The club moves of a new game
 *   (season_clubs.c) fill each division of tools/recomp/league_sizes.py to the count of its start season row. create_table then takes the clubs that the division holds
 *   (rt_league_clubs, the patched stores of league_sizes.py). In later seasons the game's own results decide who
 *   moves (Q247), so a division holds the count that promotion and relegation give it. With a new count the
 *   matches per club (+0xa7) go back to -1, so League::check_complete 0x689ad0 counts them again (most classes
 *   keep the first season's value, 0x689b1d..0x689b36). The French classes get a date template of the rounds
 *   of their count (rt_league_dates).
 * - Promotion and relegation places: after each set_up_rules (season_size_wraps.c) the places of the row. A
 *   row's count can hold the winner of a play-off that the class plays (the rules report reads it the same way),
 *   so the direct places are the count less one when the class has play-off places.
 * - league_promote_and_relegate 0x689c80: when the places of two size-table divisions differ, the clubs move on
 *   their own only with changing rules on and when both divisions stay within their floor and cap, else in pairs
 *   (the sizes stay). An expected count of the caller that differs from the moves is dropped (Serie A and B pass
 *   4 and 4 at 0x642476).
 * - News: one sentence for each count or place that the game changes in a season (season_change.h). */
#ifndef RECOMP_SEASON_SIZES_H
#define RECOMP_SEASON_SIZES_H

#include <stdint.h>

/* The club count that create_table of a league stores (+0x3e): original in the base game, else the clubs that
 * the division holds (rt.h declares it for the translated code). */
uint32_t rt_league_clubs(uint32_t obj, uint32_t original);
uint32_t rt_league_dates(uint32_t obj, uint32_t original);

/* After set_up_rules of a league class: the promotion and relegation places of the season. */
void season_sizes_rules(uint32_t obj);

/* After the places of season_sizes_rules, with the size engine on: Série A and B with the row's two meetings play
 * no play-off (0.7.2 league sizes, slice 17, D11; season_sizes_bra.c). */
/* The most earlier starts of 7 days of a file's date template: 13 for the Brazilian Série A and B, else
 * SEASON_TEMPLATE_EARLIER (season_sizes_bra.c). */
int season_sizes_bra_earlier(const char *file);
void season_sizes_bra_rules(uint32_t obj, const char *file);

/* The club count of a division of a league class in the row of a season (the club moves of a new game), or -1
 * when the class is not in the size table or the row holds no usable count. */
int season_sizes_clubs(const char *file, int season);

/* 1 when the club moves leave a division of this file the clubs of the database, up to the class count, where its
 * row has fewer (a cup takes the division whole). */
int season_sizes_keeps_database(const char *file);

/* The source files of the league classes of tools/recomp/league_sizes.py (season_size_wraps.c). */
extern const char *const season_size_files[];
extern const int season_size_file_count;
extern const int season_size_caps[];         /* the largest count of each file, 0: the class counts its data */
extern const int season_size_floors[];       /* the smallest count, 0: none (a cup of the nation needs it) */

/* The grown classes of tools/recomp/league_sizes.py GROWN (0.7.2 league sizes, slice 2, D21; season_size_wraps.c):
 * create_table allocates the rows of the season's count up to the cap (rt_league_table_bytes), and the wrap of
 * create_schedule gives the date template exactly the rounds of the count (season_template.h, D4 and D5). */
typedef struct {
    const char *file;
    int held;               /* the rows that the class allocates (its largest original count) */
    int cap;                /* the largest count of its rows */
    int mode;               /* SEASON_TEMPLATE_SHUFFLED, _SEQUENTIAL or _MIRRORED (league_next_schedule_round 0x68a030) */
} SeasonGrown;
extern const SeasonGrown season_grown[];
extern const int season_grown_count;

/* After create_schedule of a grown class (thiscall: stage, &count, &code, &flag): for stage -1 in a season game,
 * the meetings of the row (+0x3c), the matches per club (+0xa7) and a template of the rounds of the count, which
 * replaces the class's (the pointer in eax, the count at count_at). */
void season_sizes_schedule(uint32_t obj, int stage, uint32_t count_at);

#endif
