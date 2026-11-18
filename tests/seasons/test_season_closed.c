/* The closed league table and plan (0.7.3 slice 6, issue 157). */
#include <stdio.h>
#include <string.h>
#include "season_closed.h"
#include "season_mls_plan.h"

static int fails;

#define CHECK(cond, ...) do { if (!(cond)) { fails++; printf("FAIL line %d: ", __LINE__); printf(__VA_ARGS__); \
                                             printf("\n"); } } while (0)

/* The clubs of the rows of docs/research/rules-0.6.0/arg-usa.tsv (arg-usa-217 and on), real seasons 2002 to 2027. */
static const int ROW[][2] = {
    {2002, 10}, {2003, 10}, {2004, 10}, {2005, 12}, {2006, 12}, {2007, 13}, {2008, 14}, {2009, 15}, {2010, 16},
    {2011, 18}, {2012, 19}, {2013, 19}, {2014, 19}, {2015, 20}, {2016, 20}, {2017, 22}, {2018, 23}, {2019, 24},
    {2020, 26}, {2021, 27}, {2022, 28}, {2023, 29}, {2024, 29}, {2025, 30}, {2026, 30}, {2027, 30},
};

static void test_table(void)
{
    for (size_t k = 0; k < sizeof ROW / sizeof ROW[0]; k++)
        CHECK(season_closed_count("usa_mls", ROW[k][0]) == ROW[k][1], "%d: %d clubs, the row %d", ROW[k][0],
              season_closed_count("usa_mls", ROW[k][0]), ROW[k][1]);
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", NULL, 2001) == 1, "Miami Fusion 2001");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", NULL, 2002) == 0, "Miami Fusion 2002");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", NULL, 2002) == 0, "Tampa Bay 2002");
    CHECK(season_closed_member("usa_mls", "Inter Miami CF", NULL, 2019) == 0, "Inter Miami 2019");
    CHECK(season_closed_member("usa_mls", "Inter Miami CF", NULL, 2020) == 1, "Inter Miami 2020");
    CHECK(season_closed_member("usa_mls", "San Jose Earthquakes", NULL, 2005) == 1, "San Jose 2005");
    CHECK(season_closed_member("usa_mls", "San Jose Earthquakes", NULL, 2006) == 0, "San Jose 2006");
    CHECK(season_closed_member("usa_mls", "San Jose Earthquakes", NULL, 2008) == 1, "San Jose 2008");
    CHECK(season_closed_member("usa_mls", "Chivas USA", NULL, 2014) == 1, "Chivas 2014");
    CHECK(season_closed_member("usa_mls", "Chivas USA", NULL, 2015) == 0, "Chivas 2015");
    /* The names of the databases. */
    CHECK(season_closed_member("usa_mls", "NY/NJ Metrostars", NULL, 2010) == 1, "Metrostars");
    CHECK(season_closed_member("usa_mls", "NYNJ Metrostars", NULL, 2010) == 1, "NYNJ Metrostars");
    CHECK(season_closed_member("usa_mls", "Washington DC United", NULL, 2010) == 1, "DC United");
    CHECK(season_closed_member("usa_mls", "Dallas Burn", NULL, 2010) == 1, "Dallas Burn");
    CHECK(season_closed_member("usa_mls", "CF Montr\xe9" "al", NULL, 2012) == 1, "CF Montreal");
    CHECK(season_closed_member("usa_mls", "Montreal Impact", NULL, 2011) == 0, "Montreal 2011");
    /* Clubs of the same towns that the table does not name. */
    CHECK(season_closed_member("usa_mls", "San Diego Loyal SC", NULL, 2025) == -1, "San Diego Loyal");
    CHECK(season_closed_member("usa_mls", "Minnesota Thunder", NULL, 2017) == -1, "Minnesota Thunder");
    CHECK(season_closed_member("usa_mls", "Nashville Metros", NULL, 2020) == -1, "Nashville Metros");
    CHECK(season_closed_member("usa_mls", "Cincinnati Riverhawks", NULL, 2019) == -1, "Cincinnati Riverhawks");
    CHECK(season_closed_member("usa_mls", "Toronto Lynx", NULL, 2007) == -1, "Toronto Lynx");
    CHECK(season_closed_member("no_league", "Chicago Fire", NULL, 2010) == -1, "another league");
    /* The update databases give a retired record to a newer club: the short name decides. */
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "LAFC", 2024) == 1, "LAFC record 2024");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "LAFC", 2017) == 0, "LAFC record 2017");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "Inter Miami", 2024) == 1, "Inter Miami record");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "Miami", 2002) == 0, "the real Miami Fusion");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "Tampa Bay", 2002) == 0, "the real Tampa Bay");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "Seattle", 2010) == 1, "Seattle record");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "Houston Dynamo", 2010) == 1, "Houston record");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "Portland", 2022) == 1, "Portland record");
    CHECK(season_closed_member("usa_mls", "Tampa Bay Mutiny", "Real Salt Lake", 2012) == 1, "Salt Lake record");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "Philadephia", 2011) == 1, "Philadelphia spelling");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "Atlanta Utd", 2019) == 1, "Atlanta record");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "NYCFC", 2022) == 1, "NYCFC record");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "Toronto", 2018) == 1, "Toronto record");
    CHECK(season_closed_member("usa_mls", "Miami Fusion FC", "Houston", 2013) == 1, "Houston short");
    CHECK(season_closed_member("usa_mls", "San Jose Earthquakes", "Houston Dynamo", 2007) == 1, "San Jose record");
    CHECK(season_closed_member("usa_mls", "Toronto Lynx", "Toronto", 2007) == -1, "Toronto Lynx keeps out");
    CHECK(season_mls_conference(season_closed_real_name("usa_mls", "Tampa Bay Mutiny", "LAFC"), 2024) == MLS_WEST,
          "the LAFC record plays in the West");
    CHECK(!strcmp(season_closed_real_name("usa_mls", "Toronto Lynx", "Toronto"), "Toronto Lynx"), "a name of no row");
    /* Every club of the table has a conference (season_mls_plan.c). */
    for (int i = 0; season_closed_real("usa_mls", i); i++) {
        const char *real = season_closed_real("usa_mls", i);
        CHECK(season_mls_conference(real, season_closed_first("usa_mls", i)) >= 0, "%s has no conference", real);
    }
}

static void test_plan(void)
{
    int a[16];
    ClosedResult r;
    /* 2002: the two clubs that fold leave, and no other club moves. */
    ClosedClub y2002[] = {{"Miami Fusion FC", 1, 0, 0, 10},    {"Tampa Bay Mutiny", 1, 0, 0, 10},
                          {"Chicago Fire", 1, 0, 0, 90},       {"Colorado Rapids", 1, 0, 0, 50},
                          {"Columbus Crew", 1, 0, 0, 50},      {"Washington DC United", 1, 0, 0, 50},
                          {"Dallas Burn", 1, 0, 0, 50},        {"Los Angeles Galaxy", 1, 0, 0, 50},
                          {"New England Revolution", 1, 0, 0, 50}, {"NYNJ Metrostars", 1, 0, 0, 50},
                          {"San Jose Earthquakes", 1, 0, 0, 50},   {"Kansas City Wizards", 1, 0, 0, 50},
                          {"Seattle Sounders", 0, 0, 0, 30}};
    season_closed_plan("usa_mls", 2002, 10, y2002, 13, a, &r);
    CHECK(a[0] == CLOSED_FOLD && a[1] == CLOSED_FOLD, "2002 folds %d %d", a[0], a[1]);
    CHECK(a[2] == CLOSED_STAY && a[12] == CLOSED_STAY, "2002 stays %d %d", a[2], a[12]);
    CHECK(r.folded == 2 && r.joined == 0 && r.count == 10 && r.missing == 0, "2002 result %d %d %d %d", r.folded,
          r.joined, r.count, r.missing);
    /* A human manager keeps the club in the league. */
    y2002[0].human = 1;
    season_closed_plan("usa_mls", 2002, 10, y2002, 13, a, &r);
    CHECK(a[0] == CLOSED_STAY && a[1] == CLOSED_FOLD && r.human_stays == 1 && r.count == 11, "human %d %d %d %d", a[0],
          a[1], r.human_stays, r.count);
    /* A club joins in its season, and never its reserve side or a club of the same town. */
    ClosedClub y2009[] = {{"Seattle Sounders FC 2", 0, 0, 1, 90}, {"Seattle Sounders", 0, 0, 0, 30},
                          {"Portland Timbers", 0, 0, 0, 30},      {"Chicago Fire", 1, 0, 0, 50}};
    season_closed_plan("usa_mls", 2009, 0, y2009, 4, a, &r);
    CHECK(a[0] == CLOSED_STAY && a[1] == CLOSED_JOIN && a[2] == CLOSED_STAY && a[3] == CLOSED_STAY, "2009 %d %d %d %d",
          a[0], a[1], a[2], a[3]);
    CHECK(r.joined == 1 && r.missing == 13, "2009 joined %d missing %d", r.joined, r.missing);
    /* San Diego FC joins in 2025, and San Diego Loyal never does. */
    ClosedClub y2025[] = {{"San Diego Loyal SC", 0, 0, 0, 90}, {"San Diego FC", 0, 0, 0, 10}};
    season_closed_plan("usa_mls", 2024, 0, y2025, 2, a, &r);
    CHECK(a[0] == CLOSED_STAY && a[1] == CLOSED_STAY, "2024 %d %d", a[0], a[1]);
    season_closed_plan("usa_mls", 2025, 0, y2025, 2, a, &r);
    CHECK(a[0] == CLOSED_STAY && a[1] == CLOSED_JOIN, "2025 %d %d", a[0], a[1]);
    /* October 2023: the two retired records play as LAFC and Inter Miami, so they stay and San Diego FC makes 30. */
    ClosedClub slots[] = {{"Tampa Bay Mutiny", 1, 0, 0, 80, "LAFC"}, {"Miami Fusion FC", 1, 0, 0, 80, "Inter Miami"},
                          {"San Diego FC", 0, 0, 0, 10, "San Diego"}};
    season_closed_plan("usa_mls", 2025, 0, slots, 3, a, &r);
    CHECK(a[0] == CLOSED_STAY && a[1] == CLOSED_STAY && a[2] == CLOSED_JOIN, "slots %d %d %d", a[0], a[1], a[2]);
    CHECK(r.folded == 0 && r.joined == 1, "slots folded %d joined %d", r.folded, r.joined);
    /* A club that the table does not name leaves only above the count, the lowest reputation first. */
    ClosedClub extra[] = {{"Stand In A", 1, 0, 0, 20}, {"Stand In B", 1, 0, 0, 10}, {"Stand In C", 1, 1, 0, 5},
                          {"Chicago Fire", 1, 0, 0, 50}};
    season_closed_plan("usa_mls", 2010, 3, extra, 4, a, &r);
    CHECK(a[0] == CLOSED_STAY && a[1] == CLOSED_CUT && a[2] == CLOSED_STAY && a[3] == CLOSED_STAY && r.cut == 1 &&
          r.count == 3, "cut %d %d %d %d, %d", a[0], a[1], a[2], a[3], r.count);
    season_closed_plan("usa_mls", 2010, 4, extra, 4, a, &r);
    CHECK(r.cut == 0 && r.count == 4, "no cut at the count: %d", r.cut);
}

int main(void)
{
    test_table();
    test_plan();
    printf(fails ? "season_closed: %d failures\n" : "season_closed: ok\n", fails);
    return fails != 0;
}
