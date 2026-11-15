/* The fixture plans of the Argentine Primera (0.7.2 league sizes, slice 15, D9): season_arg_plan.c.
 *
 * Two zones of 15 (the 2025 and 2026 form): 16 rounds of 15 matches; each club meets each club of its zone once, its
 * rival of the other zone once (in the round where both rest in their zone) and its drawn opponent once (in the drawn
 * round); no club plays twice in a round. One table of 30 with a derby round (2015, 2016/17): 30 rounds; each pair
 * meets once, the derby pairs twice (once in the derby round). Odd zones of other sizes and other counts too. */
#include "season_sizes_arg.h"

#include <stdio.h>
#include <string.h>

static int fails;

#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); \
                                       printf("\n"); } } while (0)

static int round[SEASON_ARG_MAX_ROUNDS][SEASON_ARG_MAX_CLUBS / 2][2];

/* Each round holds every club once; returns the meetings of each pair in meet[a][b] (a < b) and the home matches. */
static void count(int n, int rounds, int meet[][SEASON_ARG_MAX_CLUBS], int *home, const char *what)
{
    memset(meet, 0, sizeof(int) * SEASON_ARG_MAX_CLUBS * SEASON_ARG_MAX_CLUBS);
    memset(home, 0, sizeof(int) * SEASON_ARG_MAX_CLUBS);
    for (int r = 0; r < rounds; r++) {
        int seen[SEASON_ARG_MAX_CLUBS] = {0};
        for (int k = 0; k < n / 2; k++) {
            int h = round[r][k][0], a = round[r][k][1];
            CHECK(h >= 0 && h < n && a >= 0 && a < n && h != a, "%s: round %d match %d: %d v %d", what, r, k, h, a);
            if (h < 0 || h >= n || a < 0 || a >= n || h == a) continue;
            seen[h]++;
            seen[a]++;
            home[h]++;
            meet[h < a ? h : a][h < a ? a : h]++;
        }
        for (int c = 0; c < n; c++) CHECK(seen[c] == 1, "%s: round %d: club %d plays %d times", what, r, c, seen[c]);
    }
}

static void zones(int z, int draw_at, int shift)
{
    int rival[SEASON_ARG_MAX_CLUBS / 2], draw[SEASON_ARG_MAX_CLUBS / 2];
    for (int i = 0; i < z; i++) {
        rival[i] = (i * 7 + 3) % z;               /* any perfect matching */
        draw[i] = (rival[i] + shift) % z;
    }
    char what[64];
    snprintf(what, sizeof what, "zones of %d, drawn round %d", z, draw_at + 1);
    int rounds = season_arg_zone_plan(z, rival, draw, draw_at, round);
    CHECK(rounds == z + 1, "%s: %d rounds", what, rounds);
    if (rounds != z + 1) return;
    static int meet[SEASON_ARG_MAX_CLUBS][SEASON_ARG_MAX_CLUBS];
    int home[SEASON_ARG_MAX_CLUBS];
    count(2 * z, rounds, meet, home, what);
    for (int a = 0; a < 2 * z; a++)
        for (int b = a + 1; b < 2 * z; b++) {
            int same = (a < z) == (b < z), want = same;
            if (!same) {
                int x = a < z ? a : b, y = (a < z ? b : a) - z;
                want = (rival[x] == y) + (draw[x] == y);
            }
            CHECK(meet[a][b] == want, "%s: clubs %d and %d meet %d times, want %d", what, a, b, meet[a][b], want);
        }
    /* The drawn round holds only inter-zone matches; club i of zone A rests in its zone in round i (before the drawn
     * round) and plays its rival there. */
    for (int k = 0; k < z; k++) {
        int h = round[draw_at][k][0], a = round[draw_at][k][1];
        CHECK((h < z) != (a < z), "%s: drawn round match %d is in one zone", what, k);
    }
    for (int i = 0; i < z; i++) {
        int r = i < draw_at ? i : i + 1, found = 0;
        for (int k = 0; k < z; k++) {
            int h = round[r][k][0], a = round[r][k][1];
            found += (h == i && a == z + rival[i]) || (a == i && h == z + rival[i]);
        }
        CHECK(found == 1, "%s: club %d does not meet its rival in round %d", what, i, r + 1);
    }
    for (int c = 0; c < 2 * z; c++)
        CHECK(home[c] >= rounds / 2 - 2 && home[c] <= rounds / 2 + 2, "%s: club %d has %d home matches of %d", what, c,
              home[c], rounds);
}

static void derby(int n, int derby_at)
{
    int partner[SEASON_ARG_MAX_CLUBS];
    for (int i = 0; i < n; i += 2) partner[i] = i + 1, partner[i + 1] = i;
    char what[64];
    snprintf(what, sizeof what, "one table of %d, derby round %d", n, derby_at + 1);
    int rounds = season_arg_derby_plan(n, partner, derby_at, round);
    CHECK(rounds == n, "%s: %d rounds", what, rounds);
    if (rounds != n) return;
    static int meet[SEASON_ARG_MAX_CLUBS][SEASON_ARG_MAX_CLUBS];
    int home[SEASON_ARG_MAX_CLUBS];
    count(n, rounds, meet, home, what);
    for (int a = 0; a < n; a++)
        for (int b = a + 1; b < n; b++)
            CHECK(meet[a][b] == 1 + (partner[a] == b), "%s: clubs %d and %d meet %d times", what, a, b, meet[a][b]);
    for (int k = 0; k < n / 2; k++) {
        int h = round[derby_at][k][0], a = round[derby_at][k][1];
        CHECK(partner[h] == a, "%s: derby round match %d is %d v %d", what, k, h, a);
    }
    for (int c = 0; c < n; c++)
        CHECK(home[c] >= rounds / 2 - 2 && home[c] <= rounds / 2 + 2, "%s: club %d has %d home matches of %d", what, c,
              home[c], rounds);
}

int main(void)
{
    zones(15, 7, 1);        /* 2025: the drawn round is fecha 8 */
    zones(15, 5, 4);        /* 2026: fecha 6 */
    zones(15, 0, 2);
    zones(15, 15, 3);
    zones(13, 6, 5);
    zones(5, 2, 1);
    derby(30, 23);          /* 2015 and 2016/17: fecha 24 of 30 */
    derby(30, 0);
    derby(30, 29);
    derby(24, 11);
    derby(2, 0);
    /* Refusals: an even zone, a rival twice, a derby that is not a pairing. */
    int r2[4] = {0, 0, 1, 2}, d2[4] = {1, 2, 3, 0}, p3[4] = {1, 0, 3, 3};
    CHECK(season_arg_zone_plan(4, r2, d2, 0, round) == 0, "an even zone gives a plan");
    CHECK(season_arg_zone_plan(3, r2, d2, 0, round) == 0, "a rival twice gives a plan");
    CHECK(season_arg_derby_plan(4, p3, 0, round) == 0, "a derby that is not a pairing gives a plan");
    int r5[5] = {0, 1, 2, 3, 4}, d5[5] = {1, 2, 3, 4, 4};
    d5[4] = 0;
    d5[0] = 0;
    d5[1] = 2;
    CHECK(season_arg_zone_plan(5, r5, d5, 0, round) == 0, "a drawn opponent that is the rival gives a plan");
    if (fails) {
        printf("%d checks failed\n", fails);
        return 1;
    }
    printf("arg_prm plans: all checks passed\n");
    return 0;
}
