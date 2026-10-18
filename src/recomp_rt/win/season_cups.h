/* The round formats of the domestic cups by season (0.6.0 slice 21; spec docs/specs/2026-10-16-seasons-0.6.0-spec.md,
 * slice 21). Research and evidence: docs/research/2026-10-17-cup-round-record.md.
 *
 * A cup class of the original makes its round table at every season start (set_up_rounds, vtable +0x8c, which
 * calls create_round_table) and at a new game. A round is 0x68 bytes. Five of its fields say how the round is
 * played: the match flags of the first match (+0x0d), the flags of the deciding match (+0x0f, the replay or the
 * second leg), the replays allowed (+0x20), the legs (+0x21) and the days to the replay or second leg (+0x22).
 * The table cup_formats.tsv gives one word for each round (the types below). This file holds the part that does
 * not touch the game's memory, so a test on the host can run it. season_kind_cups.c is the rule kind. */
#ifndef RECOMP_SEASON_CUPS_H
#define RECOMP_SEASON_CUPS_H

#include <stdint.h>

typedef struct {
    const char *file;        /* the source file of the cup class: eng_fa_cup */
    int season;              /* the first season of the format (start year), 2002 or later */
    int rounds;              /* the round count that the class makes (cup+0x3c) */
    const char *types;       /* one letter for each round: . E P R S L N Q H (make_cups_table.py) */
    const char *sentence;    /* the news sentence of the change */
} SeasonCupRow;

extern const SeasonCupRow season_cup_rows[];     /* season_cups_table.c, sorted by file and season */
extern const int season_cup_row_count;

/* The latest row of the cup that is not later than the season, or NULL (the cup keeps the original's rounds). */
const SeasonCupRow *season_cups_row(const char *file, int season);

/* The five fields of a round. */
typedef struct {
    uint16_t first;          /* +0x0d match flags of the first match (both legs of a two-leg tie) */
    uint16_t deciding;       /* +0x0f flags of the deciding match */
    uint8_t replays;         /* +0x20 */
    uint8_t legs;            /* +0x21 */
    uint8_t days;            /* +0x22 */
} SeasonCupRound;

/* Match flag bits (docs/research/2026-10-17-cup-round-record.md, section 2). */
#define CUP_PENALTIES 0x0001u
#define CUP_EXTRA_TIME 0x0002u
#define CUP_NO_AWAY_GOALS 0x0004u
#define CUP_AWAY_AFTER_ET 0x4000u
#define CUP_RULE_BITS 0x4007u       /* the bits that a type sets; the draw bits 0x100, 0x200, 0x400 stay */

/* Sets a round to the type of a letter (. keeps it). Returns 1 when the round changed, 0 when it did not or the
 * letter is not a type. */
int season_cups_apply(char type, SeasonCupRound *round);

#endif
