/* The fixture plans of the Argentine Primera's zones and derby round (season_sizes_arg.h): pure, no game state
 * (tests/seasons/test_arg_prm_plan.c). */
#include "season_sizes_arg.h"

#define MAX_CLUBS SEASON_ARG_MAX_CLUBS

static void pair(int round[][SEASON_ARG_MAX_CLUBS / 2][2], int r, int k, int home, int away)
{
    round[r][k][0] = home;
    round[r][k][1] = away;
}

int season_arg_zone_plan(int z, const int *rival, const int *draw, int draw_at,
                         int round[][SEASON_ARG_MAX_CLUBS / 2][2])
{
    if (z < 3 || z % 2 == 0 || z > MAX_CLUBS / 2 || draw_at < 0 || draw_at > z || z + 1 > SEASON_ARG_MAX_ROUNDS)
        return 0;
    char seen_r[MAX_CLUBS / 2] = {0}, seen_d[MAX_CLUBS / 2] = {0};
    for (int i = 0; i < z; i++) {
        if (rival[i] < 0 || rival[i] >= z || draw[i] < 0 || draw[i] >= z || seen_r[rival[i]]++ || seen_d[draw[i]]++)
            return 0;
    }
    /* Zone B in the order of its rest rounds: club rival[r] rests in round r, as club r of zone A. */
    for (int c = 0; c < z; c++) {
        int r = c < draw_at ? c : c + 1, k = 0;     /* the circle's round c is the plan's round r */
        for (int d = 1; d <= (z - 1) / 2; d++) {
            int x = (c + d) % z, y = (c - d + z) % z;
            /* Venues by the step alone: each club is the first of a step once and the second once, so it has
             * (z - 1) / 2 home matches in its zone. */
            int first = d % 2 == 1;
            if (first) pair(round, r, k++, x, y); else pair(round, r, k++, y, x);
            if (first) pair(round, r, k++, z + rival[x], z + rival[y]); else pair(round, r, k++, z + rival[y], z + rival[x]);
        }
        if (c % 2 == 0) pair(round, r, k++, c, z + rival[c]); else pair(round, r, k++, z + rival[c], c);
    }
    for (int i = 0; i < z; i++) {
        if (i % 2 == 0) pair(round, draw_at, i, z + draw[i], i); else pair(round, draw_at, i, i, z + draw[i]);
    }
    return z + 1;
}

int season_arg_derby_plan(int n, const int *derby, int derby_at, int round[][SEASON_ARG_MAX_CLUBS / 2][2])
{
    if (n < 2 || n % 2 || n > MAX_CLUBS || derby_at < 0 || derby_at > n - 1 || n > SEASON_ARG_MAX_ROUNDS) return 0;
    for (int i = 0; i < n; i++)
        if (derby[i] < 0 || derby[i] >= n || derby[i] == i || derby[derby[i]] != i) return 0;
    int m = n - 1;
    for (int c = 0; c < m; c++) {
        int r = c < derby_at ? c : c + 1, k = 0;
        if (c % 2 == 0) pair(round, r, k++, m, c); else pair(round, r, k++, c, m);
        for (int d = 1; d < n / 2; d++) {
            int x = (c + d) % m, y = (c - d + m) % m;
            if (d % 2 == 1) pair(round, r, k++, x, y); else pair(round, r, k++, y, x);    /* as in zones */
        }
    }
    for (int i = 0, k = 0; i < n; i++)
        if (i < derby[i]) {
            if (k % 2 == 0) pair(round, derby_at, k++, i, derby[i]); else pair(round, derby_at, k++, derby[i], i);
        }
    return n;
}

