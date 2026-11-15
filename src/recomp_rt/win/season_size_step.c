/* The size step at a season change: the plan (season_size_step.h). */
#include "season_size_step.h"

static int count_in(const StepClub *club, int nclub, int d)
{
    int n = 0;
    for (int i = 0; i < nclub; i++) n += club[i].div == d;
    return n;
}

/* 1 when a division below takes part: it has a row and has not made its new table. */
static int open_division(const StepDivision *div, int ndiv, int d)
{
    return d >= 0 && d < ndiv && div[d].want >= 0 && !div[d].started;
}

static int add_move(StepMove *out, int max, int *n, StepClub *club, int c, int to)
{
    if (*n >= max) return 0;
    out[*n] = (StepMove){c, club[c].div, to};
    (*n)++;
    club[c].div = to;
    return 1;
}

int season_size_step_plan(StepDivision *div, int ndiv, StepClub *club, int nclub, StepMove *out, int max)
{
    int n = 0;
    for (int d = 0; d < ndiv; d++) {
        StepDivision *v = &div[d];
        v->short_by = 0;
        if (v->want < 0 || v->started) continue;
        int open = 0;
        for (int k = 0; k < v->nlower; k++) open += open_division(div, ndiv, v->lower[k]);
        int have = count_in(club, nclub, d);
        if (have < v->want) {
            int need = v->want - have;
            if (open) {
                /* The best place first, the divisions below in their order at each place. */
                for (int place = 0; place < STEP_MAX_ROWS && need > 0; place++)
                    for (int k = 0; k < v->nlower && need > 0; k++) {
                        int l = v->lower[k];
                        if (!open_division(div, ndiv, l) || place >= div[l].nrows) continue;
                        int c = div[l].rows[place];
                        if (c < 0 || c >= nclub || club[c].div != l || !club[c].up_ok) continue;
                        if (add_move(out, max, &n, club, c, d)) need--;
                    }
            } else if (!v->nlower) {
                /* The lowest division: the clubs of its nation with no division, the best reputation first. */
                while (need > 0) {
                    int best = -1;
                    for (int c = 0; c < nclub; c++) {
                        if (club[c].div != STEP_NONE || club[c].nation != v->nation || !club[c].up_ok) continue;
                        if (best < 0 || club[c].rep > club[best].rep) best = c;
                    }
                    if (best < 0 || !add_move(out, max, &n, club, best, d)) break;
                    need--;
                }
            }
            v->short_by = need;
        } else if (have > v->want) {
            int surplus = have - v->want;
            /* The worst place first, only the clubs of its last table that are still in it. */
            for (int r = v->nrows - 1; r >= 0 && surplus > 0; r--) {
                int c = v->rows[r];
                if (c < 0 || c >= nclub || club[c].div != d || !club[c].down_ok) continue;
                int to = STEP_NONE;
                if (open) {
                    /* The division below with the most need. */
                    int most = 0;
                    to = -2;
                    for (int k = 0; k < v->nlower; k++) {
                        int l = v->lower[k];
                        if (!open_division(div, ndiv, l)) continue;
                        int need = div[l].want - count_in(club, nclub, l);
                        if (to == -2 || need > most) to = l, most = need;
                    }
                } else if (v->nlower) {
                    continue;
                }
                if (add_move(out, max, &n, club, c, to)) surplus--;
            }
            v->short_by = -surplus;
        }
    }
    return n;
}

int season_size_lone_down(int down, int pool, int have, int want)
{
    if (down <= 0) return 0;
    if (want < 0) return 0;     /* no row: the step takes no club back, so none goes */
    int most = pool + have - want;
    if (most < 0) most = 0;
    return down < most ? down : most;
}

int season_size_lone_pick(const int *human, int nrows, int down, int *out, int *kept, int *nkept)
{
    int n = 0;
    *nkept = 0;
    for (int k = nrows - 1; k >= 0 && n < down; k--) {
        if (!human[k]) out[n++] = k;
        else if (k >= nrows - down) kept[(*nkept)++] = k;   /* only a club in the places stays; one above is safe */
    }
    return n;
}
