/* How the picture is scaled to the window with sharp_picture (winscale.h, 0.6.0 Windows scaling). */
#include "winscale.h"

#include <stdio.h>

static int failed;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); failed = 1; } } while (0)

int main(void)
{
    WinScale s;
    /* The old rule, without the smooth band (Mac, Linux). */
    s = winscale_pick(1280, 800, 1280, 800, 0);                     /* 1 time: whole pixels */
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(1600, 1000, 1280, 800, 0);                    /* 1.25 times: scale 1 with bars */
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(1920, 1200, 1280, 800, 0);                    /* 1.5 times: nearest pixels fill */
    CHECK(!s.whole && !s.smooth);
    s = winscale_pick(2560, 1600, 1280, 800, 0);                    /* 2 times: whole pixels */
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(3072, 1920, 1280, 800, 0);                    /* 2.4 times: 2 is at least 4/5 */
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(3712, 2320, 1280, 800, 0);                    /* 2.9 times: nearest pixels fill */
    CHECK(!s.whole && !s.smooth);
    s = winscale_pick(1000, 625, 1280, 800, 0);                     /* smaller than the picture */
    CHECK(!s.whole && !s.smooth);

    /* Windows: the smooth band between 1.2 and 2 times. */
    s = winscale_pick(1280, 800, 1280, 800, 1);
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(1500, 940, 1280, 800, 1);                     /* 1.17 times: still whole pixels */
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(1600, 1000, 1280, 800, 1);                    /* 125 %: smooth fill */
    CHECK(!s.whole && s.smooth);
    s = winscale_pick(1920, 1009, 1280, 800, 1);                    /* 1080p at 125 %, maximised: 1.26 */
    CHECK(!s.whole && s.smooth);
    s = winscale_pick(1920, 1200, 1280, 800, 1);                    /* 150 % */
    CHECK(!s.whole && s.smooth);
    s = winscale_pick(1200, 900, 800, 600, 1);                      /* classic at 150 % */
    CHECK(!s.whole && s.smooth);
    s = winscale_pick(2560, 1600, 1280, 800, 1);                    /* 200 %: whole pixels */
    CHECK(s.whole && !s.smooth);
    s = winscale_pick(1000, 625, 1280, 800, 1);                     /* smaller than the picture: as before */
    CHECK(!s.whole && !s.smooth);
    s = winscale_pick(0, 0, 1280, 800, 1);                          /* no output size */
    CHECK(!s.whole && !s.smooth);
    puts(failed ? "FAIL" : "PASS");
    return failed;
}
