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
    const char *drops;              /* where the losers of Champions League qualifying rounds enter the Europa
                                     * League: "Q3>PO PO>G" (2009/10 on), "Q3-LP>G" a path; "" before 2009/10 */
    /* Slice 24 (2021/22 on). */
    int uc_ko_po;                   /* clubs of the Europa League knockout round play-offs (the group runners-up
                                     * and the Champions League thirds; the winners go to the round of 16), 0
                                     * when the groups send their clubs to the first knockout round */
    int away_goals;                 /* 1: away goals decide a level two-legged tie (to 2020/21); 0: no away goals
                                     * rule, extra time and penalties (U9, 2021/22 on), in all three cups */
    const char *suspended;          /* nations with no UEFA places, by name, separated by commas ("Russia"
                                     * from 2022/23, Q293); "" for none */
    int ecl_round_count;            /* the Conference League (U8, 2021/22 on), 0 when not held */
    SeasonUefaRound ecl_rounds[SEASON_UEFA_ROUNDS];
    int ecl_groups, ecl_group_clubs, ecl_ko_po;
    int sc_extra_time;              /* 1: a level Super Cup has extra time, then penalties; 0: penalties after 90
                                     * minutes (2023 on) */
    /* Slice 25 (2024/25 on): the matches of a club in the league phase of the Champions League, the Europa League
     * and the Conference League (8, 8 and 6), 0 for a group stage. A league phase is the "Groups" round of 36
     * (uc_groups 1, uc_group_clubs 36, uc_through 24): places 1 to 8 go to the round of 16 and 9 to 24 to the
     * knockout phase play-offs ("KO PO" 16, then "KO" 16). */
    int cl_league, uc_league, ecl_league;
} SeasonUefa;

/* The entry codes of the access list (club +0x1df) in a season with an access list (season_uefa_seeding.c).
 * They replace the original's codes (1 to 4 for the Champions League, build_entrants 0x584fa0; 1, 2, 3 and
 * 10 * b + 3 for the UEFA Cup, 0x902b90) only in those seasons, where the runtime builds the entrants of both
 * cups. The group stage is 1, as the original's, so the group pots (0x586f60: code, then reputation) put the
 * clubs that enter there first. */
enum {
    SEASON_UEFA_CL_G = 1,
    SEASON_UEFA_CL_PO_LP, SEASON_UEFA_CL_PO_CP, SEASON_UEFA_CL_Q3_LP, SEASON_UEFA_CL_Q3_CP,
    SEASON_UEFA_CL_Q2_LP, SEASON_UEFA_CL_Q2_CP, SEASON_UEFA_CL_Q1, SEASON_UEFA_CL_PR,
    SEASON_UEFA_CL_CODES                                        /* 10: codes 1 to 9 */
};
/* The Europa League codes are 11 to 16, apart from the Champions League's: a Champions League loser that goes
 * to the Europa League keeps its code, so the two are never counted as one. The group pots then put the
 * Champions League losers first (0x586f60 sorts by code). */
enum {
    SEASON_UEFA_EL_G = 11, SEASON_UEFA_EL_PO, SEASON_UEFA_EL_Q3, SEASON_UEFA_EL_Q2, SEASON_UEFA_EL_Q1, SEASON_UEFA_EL_PR,
    SEASON_UEFA_EL_CODES                                        /* 17: codes 11 to 16 */
};
#define SEASON_UEFA_CODE_SLOTS 20                               /* arrays indexed by an entry code */

/* The code of an access list token of n characters ("Q3-CP") for comp 'c' or 'e', 0 when unknown. */
int season_uefa_code(char comp, const char *tok, int n);

/* The clubs of a round of the row by code: the Champions League round of a code ("Q3 CP", or "Q3" when it has
 * no paths), the Europa League round of all paths ("Q2 CP" + "Q2 MP"). 0 when the row has no such round. */
int season_uefa_code_clubs(const SeasonUefa *r, char comp, int code);

/* The Champions League losers of the row that enter Europa League round code (the drops of the row). */
int season_uefa_dropped(const SeasonUefa *r, int el_code);

/* The new clubs of each entry code of comp ('c' or 'e') that the round sizes of row r give: the clubs of the
 * round, less the winners of the round below in its path, less (Europa League) the Champions League losers that
 * enter it. out has SEASON_UEFA_CODE_SLOTS entries; out[code], 0 for codes the row does not play. */
void season_uefa_new_clubs(const SeasonUefa *r, char comp, int *out);

/* The access list by association rank (slice 23, Q369: from 2009/10; the original's brackets before). */
typedef struct {
    int season;
    char comp;                      /* c Champions League, e Europa League, h CL title holder, H EL title holder
                                     * (EL place), X EL title holder with a CL place, f fair play places, L a
                                     * nation whose last EL place is its league cup winner's (places: the name);
                                     * slice 24: k Conference League, K its title holder, Y the same with an EL
                                     * place, M as L for the Conference League */
    int rank_from, rank_to;         /* 1 is the top association; 0 for h, H, X, f, K and Y. Places "" (2021/22
                                     * on): the ranks of the bracket have no place in that competition */
    const char *places;             /* the entry round of each place in domestic order (CL: the champion first;
                                     * EL: the cup winner first): G, PO, PO-CP, PO-LP, Q3, Q3-CP, Q3-LP, Q2, Q2-CP,
                                     * Q2-LP, Q1, PR */
} SeasonUefaAccess;

extern const SeasonUefaAccess season_uefa_access[];
extern const int season_uefa_access_count;

/* The places of comp ('c' or 'e') for an association of rank (1 is the top) in season S, or NULL before the
 * first season of the access list. A rank between brackets (Liechtenstein's, which has no league) takes the
 * next bracket; a rank after the last takes the last. After the last season the last season applies (Q248). */
const char *season_uefa_access_places(int season, char comp, int rank);

/* The places of a rule row ('h', 'H', 'X', 'f') in season S, or NULL. */
/* 1 when the last Europa League place of the nation named name ("England") goes to its league cup winner in S. */
int season_uefa_access_league_cup(int season, const char *name);

const char *season_uefa_access_rule(int season, char comp);

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

/* The clubs of a named round of a row's Conference League, and of a round of all its paths. */
int season_uefa_ecl_round(const SeasonUefa *r, const char *name);
int season_uefa_ecl_level(const SeasonUefa *r, const char *name);

/* 1 when the clubs of the nation named name have no UEFA places in row r (a suspension). */
int season_uefa_suspended(const SeasonUefa *r, const char *name);

/* 1 when the Champions League of row r has one group stage and a knockout round of 16 (2003/04 on). */
int season_uefa_cl_round_of_16(const SeasonUefa *r);

/* The rules report (season_report.c): writes the uefa lines of one season and phase. */
void season_uefa_report(void *file, int season, const char *phase);

/* Slice 23, season_uefa_qual.c: the game only. */
uint32_t season_uefa_cl_qual_table(uint32_t cl, int *rounds, uint32_t *name, uint32_t *league);
int season_uefa_uc_entrants(uint32_t uc);
uint32_t season_uefa_uc_qual_table(uint32_t uc, uint32_t rec, int *rounds);
int season_uefa_uc_direct_first(uint32_t uc);
void season_uefa_uc_fill_drops(uint32_t uc);
int season_uefa_report_clubs(uint32_t obj);

/* Slice 24, season_uefa.c: flag 0x4 (no away goals rule) in +0x0f of each two-legged round of a round table of
 * a season whose row has no away goals rule. The game only. */
void season_uefa_no_away_goals(uint32_t table, int rounds, int season);

#endif
