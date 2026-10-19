/* The league phase of the UEFA club competitions, 2024/25 on (0.6.0 slice 25; research
 * docs/research/2026-10-19-uefa-league-phase.md).
 *
 * The draw (season_uefa_league_draw.c, no game memory, ctest season_uefa_league): 36 clubs in pots by seed. With
 * 8 matches (Champions League, Europa League) the pots are 4 of 9, and each club meets two clubs of each pot, one
 * at home and one away. With 6 matches (Conference League) the pots are 6 of 6, and each club meets one club of
 * each pot; of the pots 1 and 2, 3 and 4, 5 and 6 it plays one at home and one away. Two clubs of one nation do
 * not meet, and a club meets at most two clubs of one nation (regulations of 2024/25, Art. 16.02). When no draw
 * is found with these rules in many tries, the second rule is dropped, then the first (as Art. 16.03 lets UEFA
 * change them). Then the matches go into match days, each club once a match day. */
#ifndef RECOMP_SEASON_UEFA_LEAGUE_H
#define RECOMP_SEASON_UEFA_LEAGUE_H

#define SEASON_LEAGUE_CLUBS 36
#define SEASON_LEAGUE_MAX_DAYS 8
#define SEASON_LEAGUE_DAY_MATCHES (SEASON_LEAGUE_CLUBS / 2)

/* A number from 0 to n - 1 (the game's random number function in the game). */
typedef int (*SeasonLeagueRandom)(int n, void *ctx);

typedef struct {
    signed char home, away;         /* club indexes 0 to 35, in seed order: pot 1 first */
} SeasonLeagueMatch;

typedef struct {
    int days;                       /* the matches of a club: 8 or 6 */
    int relax;                      /* 0: both nation rules held; 1: the "at most two" rule dropped; 2: both */
    SeasonLeagueMatch match[SEASON_LEAGUE_MAX_DAYS][SEASON_LEAGUE_DAY_MATCHES];
} SeasonLeagueDraw;

/* Draws the league phase of 36 clubs in seed order. nation[i] is the nation of club i (any number, equal for one
 * nation). days 8 or 6. Returns 1 and fills out, or 0 when no draw is found (or rnd stops: it returns < 0). */
int season_league_draw(int days, const int *nation, SeasonLeagueRandom rnd, void *ctx, SeasonLeagueDraw *out);

/* The knockout phase play-offs and the round of 16 from the table places (regulations of 2024/25, Art. 19). The
 * play-off ties: places 9 or 10 against 23 or 24, 11 or 12 against 21 or 22, 13 or 14 against 19 or 20, 15 or 16
 * against 17 or 18, which of each pair by draw; the seeded club (9 to 16) plays the second leg at home. Then the
 * round of 16: 1 or 2 against the winner of the 15/16 tie, 3 or 4 the 13/14 tie, 5 or 6 the 11/12 tie, 7 or 8
 * the 9/10 tie. po[2k] (unseeded) and po[2k + 1] (seeded) are the places (1 to 36) of play-off tie k, r16[k]
 * the place (1 to 8) that meets the winner of tie k. Ties 0 to 7 are in bracket order: the winners of ties
 * 2j and 2j + 1 meet in the quarter-finals... (the order of the bracket: see the doc). Returns 0 when rnd stops. */
int season_league_knockout(SeasonLeagueRandom rnd, void *ctx, int po[16], int r16[8]);

#endif
