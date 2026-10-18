/* International competitions in the original (0.6.0 slice 27): the editions and the rules of the code sites
 * (season_intl.h). This file reads no game memory, so the unit test tests/seasons/test_season_intl.c builds
 * it with its own season state. Research and evidence: docs/research/2026-10-18-international.md. */
#include "season.h"
#include "season_intl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The source file of each class of the code sites (enum of season_intl.h). */
static const char *const class_files[INTL_CLASSES] = {
    "world_cup", "euro_champ", "fifa_confed", "goldcup", "african_nations", "asia_nations", "oceania_nations",
    "olympics",
};

/* The year that a class holds when the table has no later edition: the object waits and plays no more
 * finals (the Confederations Cup after 2017). No game reaches it.
 * TODO (slice 16 interface): idle the object with season_change_skip() in a before hook instead, once a game
 * run has tested that interface with an international class. */
#define IDLE_YEAR 2200u

/* The last season with a golden goal (int-world: the 2003 Confederations Cup used it, "golden goal ceased in
 * 2004"; Q367: the silver goal of Euro 2004, season 2003/04, becomes full extra time and penalties). */
#define LAST_GOLDEN_SEASON 2002
#define GOLDEN_GOAL 0x80u

static int logging(void)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_SEASON_INTL_LOG");
        on = v && *v == '1';
    }
    return on;
}

int season_intl_on(void)
{
    return season_code_on() && (season_start_year() != SEASON_FIRST_YEAR || season_changing_rules());
}

const SeasonIntlEdition *season_intl_edition(const char *file, int year)
{
    for (int i = 0; i < season_intl_edition_count; i++)
        if (season_intl_editions[i].year == year && !strcmp(season_intl_editions[i].file, file))
            return &season_intl_editions[i];
    return NULL;
}

/* The table is sorted by file, then year (make_intl.py). */
const SeasonIntlEdition *season_intl_first(const char *file, int start_season)
{
    for (int i = 0; i < season_intl_edition_count; i++)
        if (!strcmp(season_intl_editions[i].file, file) && season_intl_editions[i].season >= start_season)
            return &season_intl_editions[i];
    return NULL;
}

const SeasonIntlEdition *season_intl_next(const char *file, int year)
{
    for (int i = 0; i < season_intl_edition_count; i++)
        if (!strcmp(season_intl_editions[i].file, file) && season_intl_editions[i].year > year)
            return &season_intl_editions[i];
    return NULL;
}

static const char *class_file(uint32_t cls)
{
    return cls < INTL_CLASSES ? class_files[cls] : "?";
}

/* The constructor of a class: the original rounds the game year up to its cycle. A game that starts in
 * another season takes the first edition of the table from its start season. The base game and a 2001 start
 * keep the original's year (the table holds the same editions for 2001/02). */
uint32_t rt_intl_first_year(uint32_t cls, uint32_t year)
{
    if (!season_intl_on() || season_start_year() == SEASON_FIRST_YEAR) return year;
    const SeasonIntlEdition *e = season_intl_first(class_file(cls), season_start_year());
    uint32_t now = e ? (uint32_t)e->year : IDLE_YEAR;
    if (logging())
        fprintf(stderr, "recomp: season intl: %s first edition %u (the original %u)\n", class_file(cls), now,
                year & 0xffffu);
    return now;
}

/* start_new_season of a class: the original adds its step. With changing rules on, the next edition of the
 * table. With changing rules off, the rules of the start season stay: the original's step from the first
 * edition. After the last edition of the table, the object waits (Q248: the rules stay; a competition that
 * ended does not come back). */
uint32_t rt_intl_next_year(uint32_t cls, uint32_t year, uint32_t step)
{
    uint32_t old = year & 0xffffu;
    if (!season_intl_on() || !season_changing_rules()) return old + step;
    const SeasonIntlEdition *e = season_intl_next(class_file(cls), (int)(int16_t)old);
    uint32_t now = e ? (uint32_t)e->year : (old >= IDLE_YEAR ? old : IDLE_YEAR);
    if (logging())
        fprintf(stderr, "recomp: season intl: %s next edition %u after %u (the original %u)\n", class_file(cls),
                now, old, old + step);
    return now;
}

/* The rule flags of a knockout round (sweep-international section 7): bit 0x80 is the golden goal. With
 * changing rules on, the edition of the table decides, or the season of the year for a year that the table
 * has no edition for. With changing rules off, the rule of the start season stays. */
uint32_t rt_intl_round_flags(uint32_t cls, uint32_t year, uint32_t flags)
{
    if (!season_intl_on() || !(flags & GOLDEN_GOAL)) return flags;
    int golden;
    if (season_changing_rules()) {
        int y = (int)(int16_t)(year & 0xffffu);
        const SeasonIntlEdition *e = season_intl_edition(class_file(cls), y);
        golden = e ? e->golden : y <= LAST_GOLDEN_SEASON + 1;
    } else {
        golden = season_start_year() <= LAST_GOLDEN_SEASON;
    }
    return golden ? flags : flags & ~GOLDEN_GOAL;
}

/* The first World Cup at which the holder had to qualify (int-world: "2002 is the last World Cup with an
 * automatic place for the holders"). */
#define FIRST_WC_WITHOUT_HOLDER 2006

/* The World Cup holder that a qualifying section leaves out of its pool. In a season game every section plays
 * for the 2006 World Cup or later, or for 2002 without the holder in its pool (season_intl.h, seasons.py). */
uint32_t rt_intl_wc_holder(uint32_t holder)
{
    return season_intl_on() ? 0u : holder;
}

/* Slot 0 of the World Cup teams: the holder, to 2002. */
uint32_t rt_intl_wc_holder_slot(uint32_t year)
{
    return !season_intl_on() || (int)(int16_t)(year & 0xffffu) < FIRST_WC_WITHOUT_HOLDER;
}
