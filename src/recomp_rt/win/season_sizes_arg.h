/* The Argentine Primera at its real size: halves on and off by the row, one table of up to 30, two zones with the
 * inter-zone rounds, the derby round and the relegation averages at the count (0.7.2 league sizes, slice 15; spec
 * docs/specs/2026-11-13-league-sizes-0.7.2-spec.md, D9 and D22).
 *
 * The class ArgPrm plays the Apertura in the league object (19 dates of create_schedule(-1) 0x404570, one meeting at
 * 0x405680), the Clausura in stage 0 (a League_Stage, 0x405a94, of the 19 dates of create_schedule(0)) and the
 * aggregate table of both in stage 1 (no dates, 2 meetings at 0x405b7c; ArgPrm::add_result 0x405c50 gives it every
 * result, match +0x42 = 1 at 0x405c7a). In a season game with the sizes on (league_sizes.py GROWN mode halves):
 * - a row of halves (Apertura and Clausura) plays each half in the rounds of the row: 19 for 20 clubs, 16 for two
 *   zones of 15 (14 zone matches, the rival in the round where both rest, and one full round of drawn opponents);
 * - a row of one table plays it in the league object over the dates of both halves, the Clausura stage has no dates
 *   and is complete from the start, and its honours are not given;
 * - the zones and the derby round are fixed fixture lists (League::create_fixtures 0x6835c0 plays [+0xea] when its
 *   edition is the league's, 0x683828..0x683a8f);
 * - the averages hold the clubs of the division (ArgPrm::update_relegation_averages 0x406570 finds a free record for
 *   a club of stage 1 with none, 0x4065d2..0x406622), and the places of honours and progress follow the count
 *   (tools/recomp/arg_prm.py).
 * The base game and RECOMP_LEAGUE_SIZES=off keep the class. */
#ifndef RECOMP_SEASON_SIZES_ARG_H
#define RECOMP_SEASON_SIZES_ARG_H

#include <stdint.h>

enum { SEASON_ARG_RIVAL, SEASON_ARG_DRAW, SEASON_ARG_DERBY };

/* A club of a zone in the Apertura of a rules row (season 2024: the 2025 season). */
typedef struct {
    int season;
    char zone;              /* 'A' or 'B' */
    const char *club;       /* the long name of the club in the season's database (club.dat +4) */
} SeasonArgZone;

/* A match of an inter-zone or derby round of a rules row: the rival match of the round where both clubs rest, a match
 * of the round of drawn opponents, or a derby of the derby round. */
typedef struct {
    int season;
    int kind;               /* SEASON_ARG_RIVAL, _DRAW, _DERBY */
    int round;              /* the real round (fecha), from 1 */
    const char *home, *away;
} SeasonArgPair;

extern const SeasonArgZone season_arg_prm_zones[];
extern const int season_arg_prm_zone_count;
extern const SeasonArgPair season_arg_pairs[];
extern const int season_arg_pair_count;

/* After the original create_schedule 0x404570 of the Primera (season_sizes_schedule): the dates of the stage (-1 the
 * Apertura or the one table, 0 the Clausura) in the rounds of the row; the template in eax, its count at count_at. */
void season_arg_schedule(uint32_t obj, int stage, uint32_t count_at);

/* The matches of a club of the Primera with n clubs this edition (+0xa7 of the league object). */
int season_arg_matches(uint32_t obj, int n);

/* Before League::create_fixtures 0x6835c0 of any league: the fixed list of the Primera's zones or derby round for
 * this league (the Primera, or its Clausura stage being made), or 0. */
uint32_t season_arg_fixtures(uint32_t league);

/* 1 when a division record is the Argentine third division (key [0x9cf688], whose clubs
 * arg_prm_swap_second_and_third_division_clubs 0x4068d0 exchanges with the second division, 0x4069a6..0x4069ae): the
 * size step takes its clubs as clubs with no division, so the second division can refill to its 25. */
int season_arg_third(uint32_t record);
/* That record (0 outside a season game): the clubs that the step sends down from the second division go there, where
 * the swap 0x4068d0 finds them again. */
uint32_t season_arg_third_record(void);

/* The order of the Primera's clubs for the size step (season_sizes.c step_index, D13): best first by the relegation
 * averages (the records of ArgPrm::update_relegation_averages 0x406570, sorted at 0x40671b; ArgPrm::
 * record_season_honours 0x405ca0 sends down the last two), so the step sends down the worst averages. A club with no
 * record keeps its table order after those with one. */
void season_arg_step_order(uint32_t obj, uint32_t *club, int n);

/* tools/recomp/arg_prm.py: a constant of the class for 20 clubs plus (count - 20) in a season game. */
uint32_t rt_arg_count(uint32_t original);

/* tools/recomp/arg_prm.py: the Apertura's date count that create_schedule(0) checks (0x4054a8), 19 in the base game. */
uint32_t rt_arg_apertura_dates(uint32_t obj, uint32_t original);

/* Pure, for the test (tests/seasons/arg_prm_plan_test.c): the rounds of two zones of z clubs (z odd), one meeting,
 * the rival of each club of zone A at rival[i] (an index of zone B) played in the round where both rest, and a full
 * round of drawn opponents draw[i] at round draw_at. round[r][k] = {home, away}: 0..z-1 zone A, z..2z-1 zone B.
 * Returns the rounds (z + 1), or 0. */
#define SEASON_ARG_MAX_ROUNDS 64
#define SEASON_ARG_MAX_CLUBS 60
int season_arg_zone_plan(int z, const int *rival, const int *draw, int draw_at,
                         int round[][SEASON_ARG_MAX_CLUBS / 2][2]);

/* Pure: the rounds of one table of n clubs (n even), one meeting, and a derby round of derby[i] (the partner of club
 * i) at round derby_at. Returns the rounds (n), or 0. */
int season_arg_derby_plan(int n, const int *derby, int derby_at, int round[][SEASON_ARG_MAX_CLUBS / 2][2]);

/* Pure: the derby partners of n clubs (n even). listed[i] is the index of club i's partner in the research list when
 * that partner is in the division, -2 when the list pairs club i with a club not in the division, -1 when club i is
 * not in the list. A club of no list takes the partner of a listed club whose partner is not in the division (the
 * place of the club it replaced), the best reputation (rep[i]) first. Clubs left over meet the club next to them by
 * reputation. derby[i] gets the partner of club i. Returns the pairs made by replacement. */
int season_arg_derby_pairs(int n, const int *listed, const int *rep, int *derby);

/* The title stages of a season (season_arg_stages.c): none, two zone tables a half and a knockout of 16 after each
 * half (2025, 2026), two zone tables and a final of the zone winners with a match of the runners-up (2016), or a
 * knockout of the first 16 of each half's table (2027 on). */
enum { SEASON_ARG_KO_NONE, SEASON_ARG_KO_ZONES, SEASON_ARG_KO_FINAL, SEASON_ARG_KO_TABLE };
int season_arg_stage_kind(uint32_t obj, int *season);
/* The clubs of zone A and zone B of the Primera's plan this edition (made with its fixtures); returns the zone size,
 * 0 when there is none. */
int season_arg_zone_lists(uint32_t obj, uint32_t *a, uint32_t *b);
/* The club records that go down at the end of the season (arg_down), for the Clausura's knockout. Returns the count. */
int season_arg_relegated(uint32_t prm, uint32_t *club, int max);

/* season_arg_stages.c: after ArgPrm::create_stages 0x405950 (the wrap in season_sizes_arg.c). */
void season_arg_stages_made(uint32_t self);
/* After League::update_for_date 0x6847c0 of any league (season_split.c): the Apertura's knockout when its table is
 * complete. */
void season_arg_update(uint32_t obj);
/* The empty advance_stage 0x5a8f60 (season_uefa_cup.c): 1 when it built the Primera's last stages. */
int season_arg_advance(uint32_t obj);
/* League's +0x44 base 0x686940 (season_playoffs.c): 1 when the result is of a knockout stage of the Primera. */
int season_arg_ko_result(uint32_t obj, int stage);
/* League::record_season_honours 0x684640 of the Primera or its Clausura: the winner and the finalist of the half's
 * knockout take rows 0 and 1 for the call (their pointers swap with those rows'); restore puts every row back. saved
 * holds SEASON_ARG_HONOURS_ROWS. */
#define SEASON_ARG_HONOURS_ROWS 64
int season_arg_honours_swap(uint32_t league, uint32_t *saved);
void season_arg_honours_restore(uint32_t league, const uint32_t *saved);
/* The round name wrap (season_round_names_game.c): the names of the zone tables of the Primera, or NULL. */
const char *season_arg_stage_name(uint32_t context, int id);

#endif
