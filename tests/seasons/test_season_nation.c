/* Test of the nation rules lookup (0.6.0 slice 17, src/recomp_rt/win/season_nation.c and the table
 * season_nation_table.c that tools/seasons/encode_rules.py makes).
 *
 * The checks: the fold of the rows of a nation up to a season (a row of "keep" leaves the earlier rule, the
 * rules stay after the last row), the English windows of 2002/03 (two windows from the first season with FIFA
 * windows) and of 2023/24 (14 June to 1 September, 1 January to 1 February), the Brexit flag from 2020/21, the
 * German non-European limits of 2004 to 2006, the Turkish limits of 2001 and 2013, a nation with no windows
 * (Sweden to 2003), the nation lookup, the global of each nation, and that the table is in order. */
#include "season_nation.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } } while (0)

static void window(const SeasonNationState *s, int i, int sd, int sm, int ed, int em, int line)
{
    CHECK(s->win[i].sd == sd && s->win[i].sm == sm && s->win[i].ed == ed && s->win[i].em == em,
          "line %d: window %d is %d.%d-%d.%d, want %d.%d-%d.%d", line, i, s->win[i].sd, s->win[i].sm, s->win[i].ed,
          s->win[i].em, sd, sm, ed, em);
}

int main(void)
{
    SeasonNationState s;

    /* England: the 2001/02 row has no windows to give (the game keeps its one window). */
    CHECK(season_nation_state("England", 2001, &s) == 1, "England 2001 has a row");
    CHECK(s.windows == SEASON_NATION_KEEP, "England 2001 windows %d", s.windows);
    CHECK(s.squad_non_eu == SEASON_NATION_NONE && s.squad_foreign == SEASON_NATION_NONE, "England 2001: no quota");
    CHECK(!s.brexit, "England 2001: no Brexit");
    season_nation_state("England", 2002, &s);
    CHECK(s.windows == 2, "England 2002 windows %d", s.windows);
    window(&s, 0, 1, 6, 31, 8, __LINE__);
    window(&s, 1, 1, 1, 31, 1, __LINE__);
    season_nation_state("England", 2005, &s);
    CHECK(s.windows == 2, "England 2005 keeps the windows of 2002 (%d)", s.windows);
    season_nation_state("England", 2023, &s);
    CHECK(s.windows == 2, "England 2023 windows %d", s.windows);
    window(&s, 0, 14, 6, 1, 9, __LINE__);
    window(&s, 1, 1, 1, 1, 2, __LINE__);
    season_nation_state("England", 2025, &s);
    CHECK(s.windows == 3, "England 2025 has the Club World Cup window (%d)", s.windows);
    window(&s, 0, 1, 6, 10, 6, __LINE__);
    window(&s, 1, 16, 6, 1, 9, __LINE__);
    season_nation_state("England", 2019, &s);
    CHECK(!s.brexit, "England 2019: no Brexit");
    season_nation_state("England", 2020, &s);
    CHECK(s.brexit == 1, "England 2020: Brexit");
    season_nation_state("England", 2099, &s);
    CHECK(s.brexit == 1 && s.windows == 2, "England after the last row keeps the rules");

    /* Germany: 5 non-European players to 2003, 5 under contract from 2004, 4 in 2005, no limit from 2006. */
    season_nation_state("Germany", 2003, &s);
    CHECK(s.squad_non_eu == 5 && s.sign_non_eu == SEASON_NATION_KEEP, "Germany 2003: %d %d", s.squad_non_eu, s.sign_non_eu);
    season_nation_state("Germany", 2004, &s);
    CHECK(s.squad_non_eu == 5 && s.sign_non_eu == 5, "Germany 2004: %d %d", s.squad_non_eu, s.sign_non_eu);
    season_nation_state("Germany", 2005, &s);
    CHECK(s.squad_non_eu == 4 && s.sign_non_eu == 4, "Germany 2005: %d %d", s.squad_non_eu, s.sign_non_eu);
    season_nation_state("Germany", 2006, &s);
    CHECK(s.squad_non_eu == SEASON_NATION_NONE && s.sign_non_eu == SEASON_NATION_NONE && s.sign_foreign == SEASON_NATION_NONE,
          "Germany 2006: no limit");

    /* Turkey: 5+1+2 in 2001 (6 in the match squad, 8 registered), 6+0+4 in 2013 (10 signed). */
    season_nation_state("Turkey", 2001, &s);
    CHECK(s.squad_foreign == 6 && s.sign_foreign == 8, "Turkey 2001: %d %d", s.squad_foreign, s.sign_foreign);
    season_nation_state("Turkey", 2013, &s);
    CHECK(s.squad_foreign == 6 && s.sign_foreign == 10, "Turkey 2013: %d %d", s.squad_foreign, s.sign_foreign);
    season_nation_state("Brazil", 2023, &s);
    CHECK(s.squad_foreign == 9, "Brazil 2023: %d", s.squad_foreign);

    /* Sweden has no windows until the table knows its dates. */
    season_nation_state("Sweden", 2002, &s);
    CHECK(s.windows == SEASON_NATION_WINDOWS_NONE, "Sweden 2002 windows %d", s.windows);

    /* A nation with a game-side meaning of its window indexes is left alone. */
    season_nation_state("France", 2024, &s);
    CHECK(s.windows == SEASON_NATION_KEEP, "France 2024 windows are not rewritten (%d)", s.windows);
    season_nation_state("Argentina", 2020, &s);
    CHECK(s.windows == SEASON_NATION_KEEP, "Argentina windows are not rewritten (%d)", s.windows);

    /* Unknown nation, and a season before every row. */
    CHECK(season_nation_state("Nowhere", 2010, &s) == 0 && s.windows == SEASON_NATION_KEEP, "unknown nation");
    CHECK(season_nation_state("England", 2000, &s) == 0 && s.windows == SEASON_NATION_KEEP, "before the first row");

    /* The nations, their globals, and the order of the rows (the fold needs seasons in order). */
    const SeasonNation *n = season_nation_find("England");
    CHECK(n && n->key_global == 0x9cf2e4u, "England global");
    n = season_nation_find("Italy");
    CHECK(n && (n->flags & SEASON_NATION_PARTIAL_WINDOWS), "Italy keeps its free-transfer window");
    CHECK(season_nation_find("Spain") && !(season_nation_find("Spain")->flags & SEASON_NATION_PARTIAL_WINDOWS), "Spain");
    CHECK(!season_nation_find("Nowhere") && !season_nation_find(NULL), "no such nation");
    CHECK(season_nation_count == 27, "27 nations (%d)", season_nation_count);
    for (int i = 0; i < season_nation_rule_count; i++) {
        const SeasonNationRule *r = &season_nation_rules[i];
        CHECK(season_nation_find(r->nation) != NULL, "row %d: nation %s is not in the nation list", i, r->nation);
        if (i > 0 && !strcmp(season_nation_rules[i - 1].nation, r->nation))
            CHECK(season_nation_rules[i - 1].season < r->season, "row %d: seasons not in order", i);
        CHECK(r->windows <= SEASON_NATION_MAX_WINDOWS, "row %d: %d windows", i, r->windows);
        for (int w = 0; w < (r->windows > 0 ? r->windows : 0); w++)
            CHECK(r->win[w].sd >= 1 && r->win[w].sd <= 31 && r->win[w].sm >= 1 && r->win[w].sm <= 12 &&
                  r->win[w].ed >= 1 && r->win[w].ed <= 31 && r->win[w].em >= 1 && r->win[w].em <= 12,
                  "row %d: window %d out of range", i, w);
    }
    if (failures) {
        fprintf(stderr, "%d checks failed\n", failures);
        return 1;
    }
    puts("ok");
    return 0;
}
