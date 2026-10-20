/* Whole pixels or not with sharp_picture (winscale.h, 0.6.0 Windows display scaling). */
#include "winscale.h"

#include <stdio.h>

static int failed;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); failed = 1; } } while (0)

int main(void)
{
    /* The old rule (Mac, Linux). */
    CHECK(winscale_whole(1280, 800, 1280, 800, 0));                 /* 1 time */
    CHECK(winscale_whole(1600, 1000, 1280, 800, 0));                /* 1.25 times: 1 is 4/5 */
    CHECK(!winscale_whole(1920, 1200, 1280, 800, 0));               /* 1.5 times: nearest pixels fill */
    CHECK(winscale_whole(2560, 1600, 1280, 800, 0));                /* 2 times */
    CHECK(winscale_whole(3072, 1920, 1280, 800, 0));                /* 2.4 times: 2 is at least 4/5 */
    CHECK(!winscale_whole(3712, 2320, 1280, 800, 0));               /* 2.9 times */
    CHECK(!winscale_whole(1000, 625, 1280, 800, 0));                /* smaller than the picture */

    /* Windows: scale 1 from 1 up to 2 times. */
    CHECK(winscale_whole(1280, 800, 1280, 800, 1));
    CHECK(winscale_whole(1281, 801, 1280, 800, 1));                 /* a 1-pixel round-up at 150 % */
    CHECK(winscale_whole(1600, 1000, 1280, 800, 1));                /* 125 % */
    CHECK(winscale_whole(1920, 1009, 1280, 800, 1));                /* 1080p at 125 %, maximised */
    CHECK(winscale_whole(1920, 1200, 1280, 800, 1));                /* 150 % */
    CHECK(winscale_whole(1599, 1199, 800, 600, 1));                 /* classic, just under 2 times */
    CHECK(winscale_whole(1600, 1200, 800, 600, 1));                 /* classic, 2 times */
    CHECK(winscale_whole(3072, 1920, 1280, 800, 1));                /* 2.4 times: the old rule */
    CHECK(!winscale_whole(3712, 2320, 1280, 800, 1));               /* 2.9 times: the old rule */
    CHECK(!winscale_whole(1279, 800, 1280, 800, 1));                /* smaller: as before */
    CHECK(!winscale_whole(0, 0, 1280, 800, 1));                     /* no output size */
    puts(failed ? "FAIL" : "PASS");
    return failed;
}
