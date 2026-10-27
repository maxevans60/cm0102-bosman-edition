/* Invite links (0.7.0 slice 8): the link text (invite.c), the answer for one game (lobby.c), and a link that
 * looks up its game in a fake lobby. The page itself is tested in server/test/join.test.ts. */
#include "invite.h"
#include "lobby.h"
#include "lobby_http.h"
#include "options.h"
#include "host_net.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static int failures;
#define CHECK(c, ...) do { if (!(c)) { failures++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* Stubs of the game side. */
static int online_play = 1;
static char address[64];
int options_bool(OptionId id) { return id == OPT_ONLINE_PLAY ? online_play : 0; }
void net_lobby_install(void) { lobby_set_address(address); }
int host_link_handler(int on) { (void)on; return 0; }

#define GAME "{\"game\":{\"code\":\"K7M2Q9RT\",\"name\":\"Sunday \\u00e9 League\",\"hostName\":\"Max\",\"version\":\"0.7.0\"," \
             "\"database\":\"Original 2001/02\",\"season\":2001,\"day\":\"2001-08-11\",\"humanClubs\":[\"Arsenal\"]," \
             "\"freeSeats\":14,\"managersOnline\":2,\"password\":true}}"

static pid_t fake_lobby(uint16_t port)
{
    int l = socket(AF_INET, SOCK_STREAM, 0), one = 1;
    setsockopt(l, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET, a.sin_port = htons(port), a.sin_addr.s_addr = htonl(0x7f000001u);
    if (bind(l, (struct sockaddr *)&a, sizeof a) || listen(l, 8)) {
        perror("fake lobby");
        exit(2);
    }
    pid_t pid = fork();
    if (pid) {
        close(l);
        return pid;
    }
    for (;;) {
        int c = accept(l, NULL, NULL);
        if (c < 0) continue;
        char req[4096], path[256] = "", r[1024];
        long n = recv(c, req, sizeof req - 1, 0);
        req[n > 0 ? n : 0] = 0;
        sscanf(req, "%*s %255s", path);
        if (!strcmp(path, "/v1/games/K7M2Q9RT"))
            snprintf(r, sizeof r, "HTTP/1.1 200 OK\r\nX-Bosman-Api: 1\r\nContent-Length: %zu\r\n\r\n%s", strlen(GAME), GAME);
        else if (!strcmp(path, "/v1/games/22222222"))       /* an answer for another code */
            snprintf(r, sizeof r, "HTTP/1.1 200 OK\r\nX-Bosman-Api: 1\r\nContent-Length: %zu\r\n\r\n%s", strlen(GAME), GAME);
        else
            snprintf(r, sizeof r, "HTTP/1.1 404 Not Found\r\nX-Bosman-Api: 1\r\nContent-Length: 24\r\n\r\n"
                     "{\"error\":\"unknown_game\"}");
        send(c, r, strlen(r), 0);
        close(c);
    }
}

static void test_parse(void)
{
    char code[9];
    static const struct { const char *text; const char *code; } cases[] = {
        {"cm0102://join/K7M2Q9RT", "K7M2Q9RT"},
        {"cm0102://join/K7M2Q9RT/", "K7M2Q9RT"},            /* Windows and some browsers add a "/" */
        {"CM0102://JOIN/k7m2q9rt", "K7M2Q9RT"},
        {"cm0102://join/K7M2Q9R", NULL},                    /* 7 characters */
        {"cm0102://join/K7M2Q9RTX", NULL},
        {"cm0102://join/K7M2Q9R1", NULL},                   /* 1 is not in the alphabet */
        {"cm0102://join/K7M2Q9RO", NULL},                   /* nor O */
        {"cm0102://join/K7M2Q9RT//", NULL},
        {"cm0102://join/K7M2Q9RT?x=1", NULL},
        {"cm0102://join/../K7M2Q9", NULL},
        {"cm0102://host/K7M2Q9RT", NULL},
        {"cm0102:join/K7M2Q9RT", NULL},
        {"http://join/K7M2Q9RT", NULL},
        {"cm0102://join/", NULL},
        {"", NULL},
        {NULL, NULL},
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        memset(code, 'x', sizeof code);
        int ok = invite_parse(cases[i].text, code);
        CHECK(ok == (cases[i].code != NULL), "parse %s gives %d", cases[i].text ? cases[i].text : "NULL", ok);
        if (ok && cases[i].code) CHECK(!strcmp(code, cases[i].code), "parse %s gives code %s", cases[i].text, code);
    }
}

static void test_json(void)
{
    LobbyRow r;
    CHECK(lobby_parse_game(GAME, &r) == 1, "the game answer parses");
    CHECK(!strcmp(r.code, "K7M2Q9RT") && !strcmp(r.g.name, "Sunday \xe9 League") && !strcmp(r.g.version, "0.7.0"),
          "the fields: %s %s %s", r.code, r.g.name, r.g.version);
    CHECK(r.g.password == 1 && r.g.clubs_n == 1 && r.g.season == 2001, "password, clubs, season");
    CHECK(lobby_parse_game("{\"game\":{\"name\":\"x\"}}", &r) == 0, "no code: not valid");
    CHECK(lobby_parse_game("{\"error\":\"unknown_game\"}", &r) == 0, "an error: not valid");
    CHECK(lobby_parse_game("{\"game\":", &r) == 0, "cut: not valid");
}

/* Polls as the game's event loop does, for at most ms. */
static void poll_for(int ms)
{
    for (int t = 0; t < ms; t += 10) {
        invite_poll();
        usleep(10000);
    }
}

static void test_lookup(uint16_t port)
{
    snprintf(address, sizeof address, "http://127.0.0.1:%u", port);

    invite_open("cm0102://join/K7M2Q9RT");
    poll_for(1500);
    const LobbyRow *g = invite_last();
    CHECK(g && !strcmp(g->code, "K7M2Q9RT") && !strcmp(g->g.host_name, "Max"), "the link finds its game");

    invite_open("cm0102://join/ABCDEFGH");                 /* not open */
    poll_for(1500);
    CHECK(invite_last() == g && !strcmp(g->code, "K7M2Q9RT"), "a game that is not open is not joined");

    invite_open("cm0102://join/22222222");                 /* the lobby answers with another game */
    poll_for(1500);
    CHECK(invite_last() == g && !strcmp(g->code, "K7M2Q9RT"), "an answer for another code is not joined");

    online_play = 0;
    address[0] = 0;                                        /* any request would fail the next check */
    invite_open("cm0102://join/K7M2Q9RT");
    int got_row, result;
    LobbyRow row;
    poll_for(300);
    got_row = lobby_game_take(&row, &result);
    CHECK(!got_row, "Online play off: no lookup");
    online_play = 1;
}

int main(void)
{
    signal(SIGPIPE, SIG_IGN);
    hn_start();
    test_parse();
    test_json();
    uint16_t port = (uint16_t)(42000 + getpid() % 2000);
    pid_t lobby = fake_lobby(port);
    usleep(100000);
    test_lookup(port);
    kill(lobby, SIGKILL);
    waitpid(lobby, NULL, 0);
    if (failures) {
        printf("%d failures\n", failures);
        return 1;
    }
    printf("invite: all checks pass\n");
    return 0;
}
