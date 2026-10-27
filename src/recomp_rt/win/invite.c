/* Invite links (invite.h). */
#include "invite.h"
#include "lobby_http.h"
#include "options.h"
#include "../host.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCHEME "cm0102://join/"
#define CODE_CHARS "23456789ABCDEFGHJKMNPQRSTUVWXYZ"   /* the lobby's alphabet (server/src/validate.ts) */

void net_lobby_install(void);       /* net_lobby.c: sets the lobby address */

static volatile int looking;        /* a lookup waits for its answer */
static LobbyRow last;
static int have_last;

int invite_parse(const char *text, char code[9])
{
    size_t n = strlen(SCHEME);
    if (!text) return 0;
    for (size_t i = 0; i < n; i++)
        if (tolower((unsigned char)text[i]) != SCHEME[i]) return 0;
    const char *p = text + n;
    char c[9];
    for (int i = 0; i < 8; i++) {
        int ch = toupper((unsigned char)p[i]);
        if (!ch || !strchr(CODE_CHARS, ch)) return 0;
        c[i] = (char)ch;
    }
    c[8] = 0;
    p += 8;
    if (*p == '/') p++;
    if (*p) return 0;
    memcpy(code, c, sizeof c);
    return 1;
}

void invite_open(const char *text)
{
    char code[9];
    if (!invite_parse(text, code)) {
        fprintf(stderr, "recomp: invite: not an invite link, ignored\n");
        return;
    }
    if (!options_bool(OPT_ONLINE_PLAY)) {
        fprintf(stderr, "recomp: invite: %s: Online play is off, so the game does not ask the lobby\n", code);
        return;
    }
    net_lobby_install();
    if (!lobby_usable()) {
        fprintf(stderr, "recomp: invite: %s: the game has no lobby address\n", code);
        return;
    }
    fprintf(stderr, "recomp: invite: looking up %s\n", code);
    lobby_game_fetch(code);
    looking = 1;
}

void invite_poll(void)
{
    if (!looking) return;
    LobbyRow row;
    int result;
    if (!lobby_game_take(&row, &result)) return;
    looking = 0;
    if (result != LH_OK) {
        fprintf(stderr, "recomp: invite: the lobby has no open game for this link (%s)\n",
                result == LH_API_ERROR ? "not open" : result == LH_FULL_TODAY ? "full today" : "unavailable");
        return;
    }
    fprintf(stderr, "recomp: invite: %s is \"%s\", hosted by %s, version %s%s\n", row.code, row.g.name,
            row.g.host_name, row.g.version, row.g.password ? ", with a password" : "");
    last = row, have_last = 1;
    invite_join(&row);
}

const LobbyRow *invite_last(void) { return have_last ? &last : NULL; }

/* Slice 5 starts its join through the relay here. Until then the game only logs the game it found. */
void invite_join(const LobbyRow *row)
{
    (void)row;
}

void invite_register(void)
{
    const char *test = getenv("RECOMP_USER_DIR");
    if (test && *test) return;
    host_link_handler(options_bool(OPT_ONLINE_PLAY));
}
