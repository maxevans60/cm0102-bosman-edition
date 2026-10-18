/* The Nations Leagues (0.6.0 slice 28; spec docs/specs/2026-10-16-seasons-0.6.0-spec.md, slice 28, Q169, Q248,
 * Q277, Q278, Q288). Research and evidence: docs/research/2026-10-18-nations-league.md.
 *
 * This header has two parts. The rules (season_nl_rules.c) read no game memory, so the unit test
 * tests/seasons/test_season_nl.c builds them alone: the editions, the real leagues of each edition, the overall
 * ranking of an edition, the leagues of the next edition (promotion and relegation) and the play-off paths that
 * the results pick. The game part (season_nl.c) makes the competition objects and keeps the leagues of each
 * nation in the season part of the save. */
#ifndef RECOMP_SEASON_NL_H
#define RECOMP_SEASON_NL_H

#include <stdint.h>

#define NL_MAX_LEAGUES 4        /* UEFA A to D; CONCACAF A to C */
#define NL_MAX_NATIONS 64       /* UEFA has 55 members */

/* One edition of a Nations League (season_nl_table.c, made by tools/seasons/make_nl.py from the rules table
 * int-continental.tsv and docs/research/rules-0.6.0/nations-league-tiers.tsv). */
typedef struct {
    const char *confed;         /* "UEFA" or "CONCACAF" */
    const char *edition;        /* "2018-19" */
    int season;                 /* the season of the first match of the league phase (2018 for 2018/19) */
    int leagues;                /* 4 (A to D) or 3 (A to C) */
    int size[NL_MAX_LEAGUES];   /* nations in each league */
    int groups[NL_MAX_LEAGUES]; /* groups in each league */
    /* Promotion and relegation into the next edition (the up and down columns of the row). */
    int up_winners;             /* 1: the group winners of each league below A go up one league */
    int down_last[NL_MAX_LEAGUES];   /* 1: the last of each group of the league goes down one league */
    int down_lowest[NL_MAX_LEAGUES]; /* more nations of the league that go down: the lowest of the ranking */
    const char *row;            /* id of the rules table row */
} SeasonNlEdition;

/* The real league and group of a nation in an edition (the draw). */
typedef struct {
    const char *confed;
    const char *edition;
    int league;                 /* 0 = A */
    int group;                  /* 1 to 4, 0 when the draw is not known */
    const char *nation;         /* names as the game's nation.dat writes them, "/" between other names */
} SeasonNlTier;

extern const SeasonNlEdition season_nl_editions[];
extern const int season_nl_edition_count;
extern const SeasonNlTier season_nl_tiers[];
extern const int season_nl_tier_count;

/* The first edition of a confederation whose league phase starts in season s or later, or NULL. */
const SeasonNlEdition *season_nl_first(const char *confed, int s);
/* The edition after e, or NULL. */
const SeasonNlEdition *season_nl_after(const SeasonNlEdition *e);

/* The result of one nation in one edition. The rules rank and move these rows; the nation is the game's id. */
typedef struct {
    int nation;                 /* the game's nation id (or a test id) */
    int league;                 /* 0 = A */
    int group;                  /* 1 to 4 */
    int pos;                    /* final place in the group, 1 = winner */
    int played, points, gd, gf;
    int qualified;              /* 1: qualified for the finals of the cycle by the qualifying groups */
} SeasonNlRow;

/* Sorts the rows into the overall ranking of the edition: by league (A first), then group place, then points,
 * goal difference and goals scored, then nation id (UEFA Nations League regulations, overall ranking; the
 * results against the fourth of a group of four are not left out: inference, see the doc). */
void season_nl_rank(SeasonNlRow *rows, int n);

/* The leagues of the next edition. rows: the edition's results in any order (sorted on return). Writes the
 * league of each row of the sorted rows into next[]. The group winners go up and the last go down as the
 * edition says, then the leagues are filled to the sizes of `to` from the overall ranking: a league with room
 * takes the best of the leagues below it, a full league passes its lowest down, and the last league takes all
 * that are left (Russia in a game where it still plays). Returns 0 for bad arguments, else the row count. */
int season_nl_next_leagues(const SeasonNlEdition *from, const SeasonNlEdition *to, SeasonNlRow *rows, int n,
                           int *next);

/* The play-off paths of a qualifying cycle (UEFA Euro 2020 and Euro 2024 qualifying, World Cup 2022): each path
 * takes `per` nations that did not qualify. rows: the ranked rows (season_nl_rank). Path p (0 = league A)
 * first takes the group winners of its league, then the other nations of its league by the ranking; the
 * places that a league cannot fill go to the best nations of the ranking that no path holds yet, path A
 * first. Writes paths * per nation ids into out (path p at out[p * per]), -1 for a place with no nation.
 * Returns the count of nations placed. */
int season_nl_paths(const SeasonNlRow *rows, int n, int paths, int per, int *out);

/* The semi-finals of a path of four (out[0..3] in the path's order, best first): 1 v 4 and 2 v 3, the
 * better nation at home. Writes home and away of each semi-final into pairs[0..3]. */
void season_nl_path_pairs(const int *path, int *pairs);

#endif
