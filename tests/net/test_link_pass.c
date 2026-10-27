/* One copy takes the invite links (0.7.0 slice 8, Q41, src/recomp_rt/host/link_pass.c). ctest runs it on macOS and
 * Linux. tests/net/link_container.sh also runs the Windows build under wine. One process plays both copies: the
 * channel works the same way between two processes, and the two-start run of the game checks that. */
#include "host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#define pause_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define pause_ms(ms) usleep((ms) * 1000)
#endif

static int failures;
#define CHECK(c, ...) do { if (!(c)) { failures++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* Waits at most 1 s for a link. */
static int take(char *out, size_t room)
{
    for (int i = 0; i < 100; i++) {
        if (host_link_take(out, room)) return 1;
        pause_ms(10);
    }
    return 0;
}

/* Two processes (link_container.sh): "serve <dir>" listens and prints the first link within 10 s, "pass <dir>
 * <link>" gives a link and exits 0 when the copy that listens kept it. */
static int two_processes(int argc, char **argv)
{
    char got[128];
    if (argc == 3 && !strcmp(argv[1], "serve")) {
        if (host_link_listen(argv[2])) return 1;
        printf("listening\n");
        fflush(stdout);
        for (int i = 0; i < 10; i++)
            if (take(got, sizeof got)) {
                printf("took %s\n", got);
                return 0;
            }
        return 1;
    }
    if (argc == 4 && !strcmp(argv[1], "pass")) return host_link_pass(argv[2], argv[3]) ? 0 : 1;
    return 2;
}

int main(int argc, char **argv)
{
    if (argc > 1) return two_processes(argc, argv);
    char dir[200], other[220], got[128], longer[200];
#ifdef _WIN32
    snprintf(dir, sizeof dir, "C:/users/test/cm0102-%lu", (unsigned long)GetCurrentProcessId());
#else
    char tmpl[] = "/tmp/link-pass-XXXXXX";
    if (!mkdtemp(tmpl)) return 2;
    snprintf(dir, sizeof dir, "%s", tmpl);
    unsetenv("XDG_RUNTIME_DIR");
#endif
    snprintf(other, sizeof other, "%s-other", dir);

    CHECK(host_link_pass(dir, "cm0102://join/K7M2Q9RT") == 0, "no copy listens: the link stays here");
    CHECK(host_link_take(got, sizeof got) == 0, "nothing to take");

    CHECK(host_link_listen(dir) == 0, "the first copy listens");
    CHECK(host_link_listen(dir) == -1, "a second copy does not listen");

    CHECK(host_link_pass(dir, "cm0102://join/K7M2Q9RT") == 1, "a second copy gives the link");
    CHECK(take(got, sizeof got) && !strcmp(got, "cm0102://join/K7M2Q9RT"), "the first copy takes it: %s", got);
    CHECK(host_link_take(got, sizeof got) == 0, "once");

    CHECK(host_link_pass(dir, "cm0102://join/ABCDEFGH/") == 1, "a second link");
    CHECK(take(got, sizeof got) && !strcmp(got, "cm0102://join/ABCDEFGH/"), "the second link: %s", got);

    CHECK(host_link_pass(dir, "cm0102://join/a b") == 0, "a link with a space is refused");
    CHECK(host_link_pass(dir, "") == 0, "an empty link is refused");
    memset(longer, 'A', sizeof longer - 1);
    longer[sizeof longer - 1] = 0;
    CHECK(host_link_pass(dir, longer) == 0, "a long text is not sent");
    pause_ms(200);
    CHECK(host_link_take(got, sizeof got) == 0, "nothing came from the refused links");

    CHECK(host_link_pass(other, "cm0102://join/K7M2Q9RT") == 0, "another user folder has its own channel");

#ifndef _WIN32
    rmdir(dir);                     /* link.sock stays until the end (atexit) */
#endif
    if (failures) {
        printf("%d failures\n", failures);
        return 1;
    }
    printf("link_pass: all checks pass\n");
    return 0;
}
