/* The FIFA days after the table (0.6.2 slice 14, fault 16). season_fifa_day and season_fifa_meets
 * (season_fifa_days_table.c, season_uefa_days.c) used to step one year at a time from the last year of the table,
 * and season_fifa_meets asked season_fifa_day for each month and day: 4 ms a call in 2300. This test keeps those
 * steps as the reference and checks that every year from 2001 to 2300 gives the same days and the same answers,
 * and that a call of season_fifa_meets in 2300 takes less than 0.5 ms. */
#include "season_uefa_days.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int is_leap(int y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }
static int days_in(int y) { return is_leap(y) ? 366 : 365; }

/* The code of main 45128ff1. */
static int old_fifa_day(int y, int m, int want)
{
    int last = 0;
    for (int i = 0; i < season_fifa_day_count; i++)
        if (season_fifa_days[i].year > last) last = season_fifa_days[i].year;
    int best = 0;
    if (y <= last) {
        for (int i = 0; i < season_fifa_day_count; i++) {
            const FifaDay *f = &season_fifa_days[i];
            if (f->year == y && f->month == m && (!best || abs(f->day - want) < abs(best - want))) best = f->day;
        }
        return best;
    }
    int shift = 364 * (y - last), drift = 0;
    for (int k = last; k < y; k++) drift += days_in(k) - 364;
    while (drift > 3) shift += 7, drift -= 7;
    for (int i = 0; i < season_fifa_day_count; i++) {
        const FifaDay *f = &season_fifa_days[i];
        if (f->year != last) continue;
        int doy = season_day_of_year(last, f->month, f->day) + shift, yy = last;
        while (doy >= days_in(yy)) doy -= days_in(yy), yy++;
        int mm, dd;
        season_from_day_of_year(yy, doy, &mm, &dd);
        if (yy == y && mm == m && (!best || abs(dd - want) < abs(best - want))) best = dd;
    }
    return best;
}

/* old_fifa_day of years 2000 to 2301, so that the reference season_fifa_meets runs in seconds. */
static signed char old_day[302][12][33];

static int old_fifa_meets(int y, int m, int d)
{
    int doy = season_day_of_year(y, m, d), prev = -100, next = 1000;
    for (int dy = -1; dy <= 1; dy++) {
        int yy = y + dy, base = dy < 0 ? -days_in(yy) : dy > 0 ? days_in(y) : 0;
        for (int mm = 1; mm <= 12; mm++)
            for (int want = 1; want <= 31; want++) {
                int f = old_day[yy - 2000][mm - 1][want];
                if (!f) break;
                int fd = base + season_day_of_year(yy, mm, f);
                if (fd - doy >= -1 && fd - doy <= 1) return 1;
                if (fd < doy && fd > prev) prev = fd;
                if (fd > doy && fd < next) next = fd;
            }
    }
    return next - prev <= 6;
}

int main(void)
{
    static const int len[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int bad = 0, most = 0;
    long days = 0, met = 0;
    for (int y = 2000; y <= 2301; y++) {
        int doys[200], n = season_fifa_year_days(y, doys, 200);
        if (n > most) most = n;
        for (int m = 1; m <= 12; m++)
            for (int want = 0; want <= 32; want++)
                if (season_fifa_day(y, m, want) != (old_day[y - 2000][m - 1][want] = (signed char)old_fifa_day(y, m, want))) {
                    if (bad++ < 10) printf("season_fifa_day(%d, %d, %d): %d, before %d\n", y, m, want,
                                           season_fifa_day(y, m, want), old_fifa_day(y, m, want));
                }
    }
    for (int y = 2001; y <= 2300; y++)
        for (int m = 1; m <= 12; m++)
            for (int d = 1; d <= len[m - 1] + (m == 2 && is_leap(y)); d++) {
                int a = season_fifa_meets(y, m, d), b = old_fifa_meets(y, m, d);
                days++, met += a;
                if (a != b && bad++ < 20) printf("season_fifa_meets(%d, %d, %d): %d, before %d\n", y, m, d, a, b);
            }
    /* The cap of the days of a year: the rows of each year of the table (a year after it has those of the last). */
    for (int i = 0; i < season_fifa_day_count; i++) {
        int n = 0;
        for (int j = 0; j < season_fifa_day_count; j++) n += season_fifa_days[j].year == season_fifa_days[i].year;
        if (n >= SEASON_FIFA_YEAR_MAX && bad++ < 30) printf("%d has %d FIFA days, the cap is %d\n", season_fifa_days[i].year, n, SEASON_FIFA_YEAR_MAX);
    }
    /* The speed: every day of 2300, twice. */
    clock_t t0 = clock();
    int calls = 0;
    for (int k = 0; k < 2; k++)
        for (int m = 1; m <= 12; m++)
            for (int d = 1; d <= len[m - 1]; d++) calls++, met += season_fifa_meets(2300 - k, m, d);
    double ms = (double)(clock() - t0) * 1000.0 / CLOCKS_PER_SEC / calls;
    printf("%ld days 2001 to 2300, %ld meet a FIFA day, at most %d FIFA days a year; season_fifa_meets in 2300: "
           "%.4f ms a call\n", days, met, most, ms);
    if (ms > 0.5) printf("season_fifa_meets takes %.3f ms a call in 2300, more than 0.5 ms\n", ms), bad++;
    printf("%s (%d differences)\n", bad ? "FAIL" : "PASS", bad);
    return bad != 0;
}
