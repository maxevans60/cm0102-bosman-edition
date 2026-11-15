/* The part of the cup break kind that does not touch the game's memory (season_cups.h, slice 21b). */
#include "season_cups.h"
#include "season_rules.h"

#include <ctype.h>
#include <stdio.h>
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

/* 1 when the real season of a row begins with the year ("2020", "2002–03 and 2003"). */
static int real_starts(const SeasonRule *r, int year)
{
    char y[8];
    snprintf(y, sizeof y, "%d", year);
    return r && r->real_season && !strncmp(r->real_season, y, 4) &&
           (r->real_season[4] < '0' || r->real_season[4] > '9');
}

/* The row of an edition. A calendar-year cup (its first row holds the year after its season): row Y - 1. A cup of
 * two-year seasons: row Y, unless row Y does not begin with Y and a row of Y - 1 does: a cup that moves to calendar
 * years (gap row 2 of 0.7.2: the FAI Cup and the League of Ireland Cup from 2003, whose rows from 2003/04 hold the
 * year after the season) plays edition Y in the row that holds the real season Y. The Irish cups are the only cups
 * of two-year seasons with calendar rows (2003/04 on). */
static const SeasonRule *edition_row(const char *file, int edition)
{
    int cal = season_cups_calendar(file);
    if (!cal && !real_starts(season_rules_find(edition, file, 0), edition)) {
        const SeasonRule *r;
        for (int n = 0; (r = season_rules_find(edition - 1, file, n)) != NULL; n++)
            if (real_starts(r, edition)) return r;
    }
    return row_of(file, edition - cal);
}

int season_cups_held(const char *file, int edition)
{
    const SeasonRule *r = edition_row(file, edition);
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
