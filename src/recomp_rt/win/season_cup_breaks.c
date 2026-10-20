/* The part of the cup break kind that does not touch the game's memory (season_cups.h, slice 21b). */
#include "season_cups.h"
#include "season_rules.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* The latest row of the file that is not later than season s (Q248: after the last row the rule stays), or
 * NULL. */
static const SeasonRule *row_of(const char *file, int s)
{
    if (s > SEASON_RULE_LAST) s = SEASON_RULE_LAST;
    for (int y = s; y >= SEASON_RULE_FIRST; y--) {
        const SeasonRule *r = season_rules_find(y, file, 0);
        if (r) return r;
    }
    return NULL;
}

static const SeasonRule *first_row(const char *file)
{
    for (int y = SEASON_RULE_FIRST; y <= SEASON_RULE_LAST; y++) {
        const SeasonRule *r = season_rules_find(y, file, 0);
        if (r) return r;
    }
    return NULL;
}

static int last_row_season(const char *file)
{
    for (int y = SEASON_RULE_LAST; y >= SEASON_RULE_FIRST; y--)
        if (season_rules_find(y, file, 0)) return y;
    return 0;
}

int season_cups_calendar(const char *file)
{
    const SeasonRule *r = first_row(file);
    if (!r || !r->real_season || strlen(r->real_season) != 4) return 0;
    for (int i = 0; i < 4; i++)
        if (!isdigit((unsigned char)r->real_season[i])) return 0;
    return atoi(r->real_season) == r->season + 1;
}

int season_cups_held(const char *file, int edition)
{
    const SeasonRule *r = row_of(file, edition - season_cups_calendar(file));
    if (!r) return -1;
    return r->clubs != SEASON_RULE_NOT_HELD && strcmp(r->format, "not_held") != 0;
}

int season_cups_next(const char *file, int edition)
{
    int h = season_cups_held(file, edition);
    if (h < 0) return 0;
    if (h) return edition;
    int last = last_row_season(file) + season_cups_calendar(file);
    for (int e = edition + 1; e <= last; e++)
        if (season_cups_held(file, e) == 1) return e;
    return SEASON_CUPS_NEVER;
}
