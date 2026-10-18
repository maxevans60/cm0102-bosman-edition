/* The part of the cup rule kind that does not touch the game's memory (season_cups.h). */
#include "season_cups.h"

#include <string.h>

const SeasonCupRow *season_cups_row(const char *file, int season)
{
    const SeasonCupRow *best = NULL;
    for (int i = 0; i < season_cup_row_count; i++) {
        const SeasonCupRow *r = &season_cup_rows[i];
        if (!strcmp(r->file, file) && r->season <= season && (!best || r->season > best->season)) best = r;
    }
    return best;
}

/* The days to a second leg when a single-match round becomes a two-leg round (the original's two-leg rounds use
 * 7 and 14: SpaCup 0x84d220, BelFaCup 0x41d310). */
#define SECOND_LEG_DAYS 7
/* The days to a replay when a round that had none gets one (the FA Cup rounds use 10, EngFaCup 0x56f320). */
#define REPLAY_DAYS 10

int season_cups_apply(char type, SeasonCupRound *r)
{
    SeasonCupRound was = *r;
    uint16_t keep = (uint16_t)(r->first & ~CUP_RULE_BITS);
    switch (type) {
    case 'E':                                   /* single match, extra time and penalties on the day */
        r->first = (uint16_t)(keep | CUP_EXTRA_TIME | CUP_PENALTIES);
        r->replays = 0;
        r->legs = 1;
        break;
    case 'P':                                   /* single match, penalties after 90 minutes */
        r->first = (uint16_t)(keep | CUP_PENALTIES);
        r->replays = 0;
        r->legs = 1;
        break;
    case 'R':                                   /* single match, one replay with extra time and penalties */
    case 'S':                                   /* the same, a level replay goes straight to penalties */
        r->first = keep;
        r->deciding = type == 'S' ? CUP_PENALTIES : CUP_EXTRA_TIME | CUP_PENALTIES;
        if (r->replays == 0) r->days = REPLAY_DAYS;
        r->replays = 1;
        r->legs = 1;
        break;
    case 'L':                                   /* two legs, away goals count */
    case 'N':                                   /* two legs, no away goals rule */
    case 'Q':                                   /* the same, penalties after the second leg, no extra time */
    case 'H':                                   /* two legs, away goals count only after extra time */
        r->first = (uint16_t)(keep | (type == 'H' ? CUP_AWAY_AFTER_ET : 0));
        r->deciding = (uint16_t)(type == 'Q' ? CUP_PENALTIES | CUP_NO_AWAY_GOALS :
                                 CUP_EXTRA_TIME | CUP_PENALTIES |
                                 (type == 'N' ? CUP_NO_AWAY_GOALS : type == 'H' ? CUP_AWAY_AFTER_ET : 0));
        r->replays = 0;
        if (r->legs != 2) r->days = SECOND_LEG_DAYS;
        r->legs = 2;
        break;
    default:
        return 0;
    }
    return was.first != r->first || was.deciding != r->deciding || was.replays != r->replays || was.legs != r->legs ||
           was.days != r->days;
}
