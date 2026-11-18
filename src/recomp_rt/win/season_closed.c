/* Closed leagues: the clubs that fold and join in their real seasons (season_closed.h).
 *
 * MLS: docs/research/rules-0.6.0/arg-usa.tsv (rows arg-usa-217 and on, the notes of each season name the clubs that
 * join and fold) and docs/research/club-records-0.6.0/closed-leagues.tsv (one source for each new club). San Jose
 * Earthquakes stopped after 2005, when the club's players and staff moved to Houston, and came back in 2008. */
#include <stdlib.h>
#include <string.h>
#include "season_closed.h"

#define FOREVER 9999
#define MAX_WORDS 4

typedef struct {
    const char *league, *real;
    int from, to, from2, to2;           /* the seasons that it plays; a second spell when from2 is set */
    const char *words[MAX_WORDS];       /* lower case parts of its names in the databases */
} ClosedRow;

static const ClosedRow ROWS[] = {
    {"usa_mls", "Chicago Fire", 1998, FOREVER, 0, 0, {"chicago fire"}},
    {"usa_mls", "Colorado Rapids", 1996, FOREVER, 0, 0, {"colorado rapids"}},
    {"usa_mls", "Columbus Crew", 1996, FOREVER, 0, 0, {"columbus crew"}},
    {"usa_mls", "D.C. United", 1996, FOREVER, 0, 0, {"dc united", "d.c. united"}},
    {"usa_mls", "FC Dallas", 1996, FOREVER, 0, 0, {"dallas burn", "fc dallas"}},
    {"usa_mls", "Los Angeles Galaxy", 1996, FOREVER, 0, 0, {"galaxy"}},
    {"usa_mls", "New England Revolution", 1996, FOREVER, 0, 0, {"new england rev"}},
    {"usa_mls", "New York Red Bulls", 1996, FOREVER, 0, 0, {"metrostars", "red bull"}},
    {"usa_mls", "San Jose Earthquakes", 1996, 2005, 2008, FOREVER, {"san jose earthquakes", "san jos\xe9 earthquakes"}},
    {"usa_mls", "Sporting Kansas City", 1996, FOREVER, 0, 0, {"kansas city wizards", "sporting kansas", "sporting kc"}},
    {"usa_mls", "Miami Fusion", 1998, 2001, 0, 0, {"miami fusion"}},
    {"usa_mls", "Tampa Bay Mutiny", 1996, 2001, 0, 0, {"tampa bay mutiny"}},
    {"usa_mls", "Real Salt Lake", 2005, FOREVER, 0, 0, {"real salt lake"}},
    {"usa_mls", "Chivas USA", 2005, 2014, 0, 0, {"chivas usa"}},
    {"usa_mls", "Houston Dynamo", 2006, FOREVER, 0, 0, {"houston dynamo"}},
    {"usa_mls", "Toronto FC", 2007, FOREVER, 0, 0, {"toronto fc"}},
    {"usa_mls", "Seattle Sounders", 2009, FOREVER, 0, 0, {"seattle sounders"}},
    {"usa_mls", "Philadelphia Union", 2010, FOREVER, 0, 0, {"philadelphia union"}},
    {"usa_mls", "Portland Timbers", 2011, FOREVER, 0, 0, {"portland timbers"}},
    {"usa_mls", "Vancouver Whitecaps", 2011, FOREVER, 0, 0, {"vancouver whitecaps"}},
    {"usa_mls", "Montreal Impact", 2012, FOREVER, 0, 0, {"montreal impact", "montr\xe9" "al impact", "cf montr"}},
    {"usa_mls", "New York City FC", 2015, FOREVER, 0, 0, {"new york city"}},
    {"usa_mls", "Orlando City SC", 2015, FOREVER, 0, 0, {"orlando city"}},
    {"usa_mls", "Atlanta United FC", 2017, FOREVER, 0, 0, {"atlanta united"}},
    {"usa_mls", "Minnesota United FC", 2017, FOREVER, 0, 0, {"minnesota united"}},
    {"usa_mls", "Los Angeles FC", 2018, FOREVER, 0, 0, {"los angeles fc", "los angeles football", "lafc"}},
    {"usa_mls", "FC Cincinnati", 2019, FOREVER, 0, 0, {"fc cincinnati"}},
    {"usa_mls", "Inter Miami CF", 2020, FOREVER, 0, 0, {"inter miami"}},
    {"usa_mls", "Nashville SC", 2020, FOREVER, 0, 0, {"nashville sc"}},
    {"usa_mls", "Austin FC", 2021, FOREVER, 0, 0, {"austin fc"}},
    {"usa_mls", "Charlotte FC", 2022, FOREVER, 0, 0, {"charlotte fc"}},
    {"usa_mls", "St. Louis City SC", 2023, FOREVER, 0, 0, {"st. louis city", "st louis city", "saint louis city"}},
    {"usa_mls", "San Diego FC", 2025, FOREVER, 0, 0, {"san diego fc"}},
};
#define NROWS ((int)(sizeof ROWS / sizeof ROWS[0]))

static int lower(int ch) { return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch; }

static int has_part(const char *name, const char *part)
{
    size_t n = strlen(name), w = strlen(part);
    for (size_t i = 0; i + w <= n; i++) {
        size_t k = 0;
        while (k < w && lower((unsigned char)name[i + k]) == (unsigned char)part[k]) k++;
        if (k == w) return 1;
    }
    return 0;
}

static int row_names(const ClosedRow *r, const char *name)
{
    for (int k = 0; k < MAX_WORDS && r->words[k]; k++)
        if (has_part(name, r->words[k])) return 1;
    return 0;
}

static int row_plays(const ClosedRow *r, int season)
{
    return (season >= r->from && season <= r->to) || (r->from2 && season >= r->from2 && season <= r->to2);
}

int season_closed_row(const char *league, const char *name)
{
    for (int i = 0; league && name && i < NROWS; i++)
        if (!strcmp(ROWS[i].league, league) && row_names(&ROWS[i], name)) return i;
    return -1;
}

const char *season_closed_league(const char *division)
{
    return division && !strncmp(division, "American Major League", 52) ? "usa_mls" : NULL;
}

int season_closed_member(const char *league, const char *name, int season)
{
    int i = season_closed_row(league, name);
    return i < 0 ? -1 : row_plays(&ROWS[i], season);
}

int season_closed_count(const char *league, int season)
{
    int n = 0;
    for (int i = 0; i < NROWS; i++) n += !strcmp(ROWS[i].league, league) && row_plays(&ROWS[i], season);
    return n;
}

static const ClosedRow *league_row(const char *league, int i)
{
    for (int k = 0; k < NROWS; k++)
        if (!strcmp(ROWS[k].league, league) && i-- == 0) return &ROWS[k];
    return NULL;
}

const char *season_closed_real(const char *league, int i)
{
    const ClosedRow *r = league_row(league, i);
    return r ? r->real : NULL;
}

int season_closed_first(const char *league, int i)
{
    const ClosedRow *r = league_row(league, i);
    return r ? r->from : 0;
}

void season_closed_plan(const char *league, int season, int want, const ClosedClub *clubs, int n, int *actions,
                        ClosedResult *result)
{
    ClosedResult r = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < n; i++) {
        actions[i] = CLOSED_STAY;
        if (!clubs[i].in_league) continue;
        r.count++;
        if (season_closed_member(league, clubs[i].name, season) != 0) continue;
        if (clubs[i].human) {
            r.human_stays++;
            continue;
        }
        actions[i] = CLOSED_FOLD;
        r.folded++;
        r.count--;
    }
    /* Each club of the season that the league lacks: the club of the data with the highest reputation that has one
     * of its names. A reserve side never joins. */
    for (int k = 0; k < NROWS; k++) {
        const ClosedRow *row = &ROWS[k];
        if (strcmp(row->league, league) || !row_plays(row, season)) continue;
        int have = 0, best = -1;
        for (int i = 0; i < n && !have; i++) {
            if (!row_names(row, clubs[i].name) || clubs[i].reserve) continue;
            if (clubs[i].in_league) have = 1;
            else if (best < 0 || clubs[i].reputation > clubs[best].reputation) best = i;
        }
        if (have) continue;
        if (best < 0) {
            r.missing++;
            continue;
        }
        actions[best] = CLOSED_JOIN;
        r.joined++;
        r.count++;
    }
    /* A stand-in of the data leaves when the real clubs fill the league. */
    while (want > 0 && r.count > want) {
        int worst = -1;
        for (int i = 0; i < n; i++) {
            if (!clubs[i].in_league || actions[i] != CLOSED_STAY || clubs[i].human) continue;
            if (season_closed_member(league, clubs[i].name, season) >= 0) continue;
            if (worst < 0 || clubs[i].reputation < clubs[worst].reputation) worst = i;
        }
        if (worst < 0) break;
        actions[worst] = CLOSED_CUT;
        r.cut++;
        r.count--;
    }
    *result = r;
}
