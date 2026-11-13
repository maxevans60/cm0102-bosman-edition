/* The size step at a season change (0.7.2 league sizes, slice 3, D13): season_size_step.c.
 *
 * Each case builds divisions and clubs after the promotion chain: the rows of a division are its last table, the
 * best place first; a club's division is where the chain left it. */
#include "season_size_step.h"

#include <stdio.h>
#include <string.h>

static int fails;

#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); \
                                       printf("\n"); } } while (0)

static StepClub club[200];
static int nclub;
static StepDivision div[8];
static StepMove mv[200];

static void reset(void)
{
    memset(club, 0, sizeof club);
    memset(div, 0, sizeof div);
    nclub = 0;
}

/* A division of n clubs at the places 1..n, all still in it, of nation 1. Returns the index of its first club. */
static int add_division(int d, int want, int n)
{
    div[d].want = want;
    div[d].nation = 1;
    int first = nclub;
    for (int i = 0; i < n; i++) {
        club[nclub] = (StepClub){d, 1, 100, 1, 1};
        div[d].rows[div[d].nrows++] = nclub++;
    }
    return first;
}

static int add_free(int nation, int rep, int up_ok)
{
    club[nclub] = (StepClub){STEP_NONE, nation, rep, up_ok, 1};
    return nclub++;
}

static void below(int upper, int lower)
{
    div[upper].lower[div[upper].nlower++] = lower;
}

static int count_in(int d)
{
    int n = 0;
    for (int i = 0; i < nclub; i++) n += club[i].div == d;
    return n;
}

static int moved(int n, int c, int from, int to)
{
    for (int i = 0; i < n; i++)
        if (mv[i].club == c && mv[i].from == from && mv[i].to == to) return 1;
    return 0;
}

/* Serie A from 18 to 20 (2004/05): 2 down and 2 up by the places, then A takes the best two of B that stayed. B (the
 * lowest division here) then takes two clubs with no division of its nation, the best reputation first, and no
 * reserve side and no club of another nation. */
static void grow(void)
{
    reset();
    int a = add_division(0, 20, 18), b = add_division(1, 22, 22);
    below(0, 1);
    club[a + 16].div = club[a + 17].div = 1;     /* relegated */
    club[b + 0].div = club[b + 1].div = 0;       /* promoted */
    int r = add_free(1, 900, 0), f1 = add_free(1, 500, 1), f2 = add_free(1, 400, 1), f3 = add_free(1, 300, 1);
    int other = add_free(2, 999, 1);
    int n = season_size_step_plan(div, 2, club, nclub, mv, 200);
    CHECK(count_in(0) == 20, "grow: A holds %d", count_in(0));
    CHECK(count_in(1) == 22, "grow: B holds %d", count_in(1));
    CHECK(moved(n, b + 2, 1, 0) && moved(n, b + 3, 1, 0), "grow: A takes B's 3rd and 4th");
    CHECK(moved(n, f1, STEP_NONE, 1) && moved(n, f2, STEP_NONE, 1), "grow: B takes the two best free clubs");
    CHECK(club[r].div == STEP_NONE && club[f3].div == STEP_NONE && club[other].div == STEP_NONE,
          "grow: no reserve side, third free club or club of another nation");
    CHECK(n == 4, "grow: %d moves", n);
    CHECK(div[0].short_by == 0 && div[1].short_by == 0, "grow: short %d %d", div[0].short_by, div[1].short_by);
}

/* Ligue 1 from 20 to 18: the places moved 2 and 2 in pairs, so A sends its worst two that stayed (17th and 18th:
 * 19th and 20th went down already) to B. B, the lowest, has 22 for a row of 20 and sends its worst two that stayed
 * to no division, but not a club with a human manager. */
static void shrink(void)
{
    reset();
    int a = add_division(0, 18, 20), b = add_division(1, 20, 20);
    below(0, 1);
    club[a + 18].div = club[a + 19].div = 1;
    club[b + 0].div = club[b + 1].div = 0;
    club[b + 19].down_ok = 0;
    int n = season_size_step_plan(div, 2, club, nclub, mv, 200);
    CHECK(count_in(0) == 18 && count_in(1) == 20, "shrink: %d and %d", count_in(0), count_in(1));
    CHECK(moved(n, a + 17, 0, 1) && moved(n, a + 16, 0, 1), "shrink: A's 17th and 18th go down");
    CHECK(moved(n, b + 18, 1, STEP_NONE) && moved(n, b + 17, 1, STEP_NONE), "shrink: B's 19th and 18th leave");
    CHECK(club[b + 19].div == 1, "shrink: the human club stays");
    CHECK(n == 4, "shrink: %d moves", n);
}

/* A division below that made its new table, or that takes no part, gives no club: A stays short and says so. */
static void started(void)
{
    reset();
    add_division(0, 20, 18);
    add_division(1, 22, 22);
    below(0, 1);
    div[1].started = 1;
    int n = season_size_step_plan(div, 2, club, nclub, mv, 200);
    CHECK(n == 0 && div[0].short_by == 2, "started: %d moves, short %d", n, div[0].short_by);
    div[1].started = 0;
    div[1].want = -1;
    div[0].short_by = 0;
    n = season_size_step_plan(div, 2, club, nclub, mv, 200);
    CHECK(n == 0 && div[0].short_by == 2, "no part: %d moves, short %d", n, div[0].short_by);
    CHECK(count_in(1) == 22, "no part: B holds %d", count_in(1));
}

/* Two divisions below (Serie B over the two C1 groups): a short division takes by place, first of the first group
 * then of the second; a reserve side of the first is passed over. A division with too many sends a club to the
 * group below with the most need. */
static void two_below(void)
{
    reset();
    add_division(0, 22, 20);
    int c1 = add_division(1, 18, 18), c2 = add_division(2, 18, 18);
    below(0, 1);
    below(0, 2);
    club[c1].up_ok = 0;
    int n = season_size_step_plan(div, 3, club, nclub, mv, 200);
    CHECK(moved(n, c1 + 1, 1, 0) && moved(n, c2, 2, 0), "two below: takes the 2nd of C1a and the 1st of C1b");
    CHECK(count_in(0) == 22, "two below: B holds %d", count_in(0));
    CHECK(div[1].short_by == 1 && div[2].short_by == 1, "two below: the lowest have no free club: %d %d",
          div[1].short_by, div[2].short_by);

    reset();
    int b = add_division(0, 20, 22);
    add_division(1, 18, 18);
    add_division(2, 18, 17);
    below(0, 1);
    below(0, 2);
    div[1].want = div[2].want = -2;     /* only the destinations matter here: they take no part */
    n = season_size_step_plan(div, 3, club, nclub, mv, 200);
    CHECK(n == 0 && div[0].short_by == -2, "two below, no part: %d moves, short %d", n, div[0].short_by);
    div[1].want = 18;
    div[2].want = 18;
    div[0].short_by = 0;
    n = season_size_step_plan(div, 3, club, nclub, mv, 200);
    CHECK(moved(n, b + 21, 0, 2), "two below: the worst goes to the group with the most need");
    CHECK(count_in(0) == 20, "two below: B holds %d", count_in(0));
}

/* A division at its row moves nothing. */
static void steady(void)
{
    reset();
    add_division(0, 18, 18);
    add_division(1, 20, 20);
    below(0, 1);
    add_free(1, 500, 1);
    CHECK(season_size_step_plan(div, 2, club, nclub, mv, 200) == 0, "steady: moves");
}

int main(void)
{
    grow();
    shrink();
    started();
    two_below();
    steady();
    printf("%s season_size_step (%d failures)\n", fails ? "FAIL" : "PASS", fails);
    return fails != 0;
}
