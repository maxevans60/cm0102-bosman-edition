/* The fixture plans of the Argentine Primera's zones and derby round (season_sizes_arg.h): pure, no game state
 * (tests/seasons/test_arg_prm_plan.c). */
#include "season_sizes_arg.h"

#define MAX_CLUBS SEASON_ARG_MAX_CLUBS

void season_arg_down_places(int row, int *avg, int *tab)
{
    *tab = 0;
    if (row <= 2011) *avg = 2;
    else if (row <= 2013) *avg = 3;
    else if (row == 2014) *avg = 2;
    else if (row == 2015) *avg = 1;
    else if (row <= 2018) *avg = 4;
    else if (row <= 2020) *avg = 0;     /* 2019/20 and 2021: relegation suspended */
    else if (row == 2021) *avg = 2;
    else if (row == 2023) *avg = 0;     /* 2024: none */
    else *avg = 1, *tab = 1;            /* 2023, 2025, 2026 and the 2027 format */
}

int season_arg_second_down_places(int row_down, int changing_rules)
{
    if (row_down < 0 || row_down > 30) return 0;
    return !changing_rules && row_down > SEASON_ARG_SWAP_DOWN ? SEASON_ARG_SWAP_DOWN : row_down;
}

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
    /* The drawn round meets other clubs (2025, 2026), or every club's rival again with the venues reversed (2016: the
     * rival twice); a mix of both is no plan. */
    char seen_r[MAX_CLUBS / 2] = {0}, seen_d[MAX_CLUBS / 2] = {0};
    int same = 0;
    for (int i = 0; i < z; i++) {
        if (rival[i] < 0 || rival[i] >= z || draw[i] < 0 || draw[i] >= z || seen_r[rival[i]]++ || seen_d[draw[i]]++)
            return 0;
        same += draw[i] == rival[i];
    }
    if (same && same != z) return 0;
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


/* Sorts idx[0..k) by rep, best first; equal reputations keep their order. */
static void by_reputation(int *idx, int k, const int *rep)
{
    for (int i = 1; i < k; i++) {
        int v = idx[i], j = i;
        for (; j > 0 && rep[idx[j - 1]] < rep[v]; j--) idx[j] = idx[j - 1];
        idx[j] = v;
    }
}

int season_arg_derby_pairs(int n, const int *listed, const int *rep, int *derby)
{
    int orphan[SEASON_ARG_MAX_CLUBS], extra[SEASON_ARG_MAX_CLUBS], no = 0, ne = 0, made = 0;
    for (int i = 0; i < n; i++) {
        derby[i] = listed[i] >= 0 ? listed[i] : -1;
        if (listed[i] == -2) orphan[no++] = i;
        if (listed[i] == -1) extra[ne++] = i;
    }
    by_reputation(orphan, no, rep);
    by_reputation(extra, ne, rep);
    for (int k = 0; k < no && k < ne; k++, made++) {
        derby[orphan[k]] = extra[k];
        derby[extra[k]] = orphan[k];
    }
    /* Left over: the clubs of no pair yet meet the club next to them by reputation. */
    int rest[SEASON_ARG_MAX_CLUBS], nr = 0;
    for (int i = 0; i < n; i++)
        if (derby[i] < 0) rest[nr++] = i;
    by_reputation(rest, nr, rep);
    for (int j = 0; j + 1 < nr; j += 2) derby[rest[j]] = rest[j + 1], derby[rest[j + 1]] = rest[j];
    return made;
}
