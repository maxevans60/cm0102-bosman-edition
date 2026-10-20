/* Unit check of the file name parts that are not UTF-8 (src/recomp_rt/win/cp1252.h, used by win_host_path in
 * files.c): utf8_ok and cp1252_to_utf8. */
#include "cp1252.h"

#include <stdio.h>

static int failed;

static void ok(const char *in, int want)
{
    if (utf8_ok(in) != want) {
        printf("FAIL utf8_ok of a part: got %d, want %d\n", !want, want);
        failed = 1;
    }
}

static void conv(const char *in, const char *want)
{
    char out[64];
    memset(out, 'x', sizeof out);
    cp1252_to_utf8(in, out, sizeof out);
    if (strcmp(out, want)) {
        printf("FAIL cp1252_to_utf8: got \"%s\", want \"%s\"\n", out, want);
        failed = 1;
    }
}

int main(void)
{
    ok("Copa Am\xc3\xa9rica_450.tmp", 1);                 /* UTF-8 */
    ok("Copa Am\xe9rica_450.tmp", 0);                     /* code page 1252 */
    ok("\xc0\xaa", 0);                                    /* overlong */
    ok("\xe0\x80\xaf", 0);                                /* overlong, 3 bytes */
    ok("\xed\xa0\x80", 0);                                /* a surrogate */
    ok("\xf4\x90\x80\x80", 0);                            /* over U+10FFFF */
    ok("\xc3", 0);                                        /* cut */
    ok("plain.tmp", 1);
    conv("Copa Am\xe9rica_450.tmp", "Copa Am\xc3\xa9rica_450.tmp");
    conv("Copa Am\xc3\xa9rica_450.tmp", "Copa Am\xc3\xa9rica_450.tmp");    /* UTF-8 stays */
    conv("K\xc3\xb6ln \xe9t\xe9", "K\xc3\xb6ln \xc3\xa9t\xc3\xa9");          /* mixed */
    conv("\x80\x9f", "\xe2\x82\xac\xc5\xb8");                               /* euro sign, Y with diaeresis */
    conv("\x81\x8d\x8f\x90\x9d", "\xc2\x81\xc2\x8d\xc2\x8f\xc2\x90\xc2\x9d"); /* not in 1252: Latin-1 */
    conv("\xc0\xaa", "\xc3\x80\xc2\xaa");                                   /* overlong: two 1252 bytes */
    {
        char small[6];
        cp1252_to_utf8("ab\xe9\xe9", small, sizeof small);                  /* a character that does not fit */
        if (strcmp(small, "ab\xc3\xa9")) { printf("FAIL cut: got \"%s\"\n", small); failed = 1; }
    }
    printf(failed ? "FAILED\n" : "PASS\n");
    return failed;
}
