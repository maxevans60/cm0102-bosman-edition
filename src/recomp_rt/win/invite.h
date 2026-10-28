/* Invite links (0.7.0 slice 8, #45, docs/specs/2026-10-27-online-play-0.7.0-spec.md).
 *
 * The invite link page of the lobby (server/src/join.ts) has a button that opens cm0102://join/<code>. The
 * computer starts the game with that link:
 * - Windows and Linux give it as an argument of the program (main.c takes it off the game's command line). When
 *   a copy of the game runs already, the new copy gives the link to it and ends (host_link_pass, Q41), and the
 *   copy that runs gets it in its event poll (user32.c).
 * - macOS sends it as an Apple event, also to a game that runs. SDL gives it as a drop event (user32.c).
 * The runtime looks up the game code in the lobby and gives the game it found to invite_join, the join path
 * of slice 5. */
#ifndef INVITE_H
#define INVITE_H

#include "lobby.h"

/* 1 when text is an invite link, "cm0102://join/<code>" with an optional "/" at the end, and code then holds
 * the game code in capitals (9 bytes). The scheme and "join" may be in any case. */
int invite_parse(const char *text, char code[9]);

/* A link from the command line or a drop event. An invite link starts a lookup of its game in the lobby, when
 * Online play is on. Any other text does nothing. */
void invite_open(const char *text);

/* Each pass of the game's event poll: when the lookup has an answer, it is logged, and a game that is open goes
 * to invite_join. Cheap while no lookup runs. */
void invite_poll(void);

/* The last game that an invite link found, or NULL. For tests and logs. */
const LobbyRow *invite_last(void);

/* Slice 5: join this online game through the relay. Called once for each invite link, on the game's thread. */
void invite_join(const LobbyRow *row);

/* At start: write the cm0102:// link handler of this computer when Online play is on, and remove it when it is
 * off (host_link_handler, host.h). A test run (RECOMP_USER_DIR set) touches nothing. */
void invite_register(void);

#endif
