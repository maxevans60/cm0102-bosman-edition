/* The lobby client (0.7.0 slice 4, #45): a host's game in the lobby, and the lobby's list of games.
 *
 * The calls follow the API contract in server/README.md. Each call goes through lobby_http (lobby_http.h)
 * on a thread of its own, so the game never waits on the network. The functions below only set a wish or
 * read a result under one lock (hn_lock), and return at once. Nothing is sent before a wish: a game that
 * never asks makes no lobby request.
 *
 * Text fields hold printable Latin-1 (the lobby's rule). lobby_clean makes a game text fit. */
#ifndef LOBBY_H
#define LOBBY_H

#define LOBBY_TEXT 41              /* 40 characters and the end */
#define LOBBY_CLUBS 16
#define LOBBY_ROWS 200             /* the most games that the lobby lists */

typedef struct {
    char game_id[65];
    char name[LOBBY_TEXT], host_name[LOBBY_TEXT], version[25], database[LOBBY_TEXT];
    int season;                    /* start year of the season */
    char day[11];                  /* YYYY-MM-DD */
    char clubs[LOBBY_CLUBS][LOBBY_TEXT];
    int clubs_n, free_seats, managers_online;
    int password;                  /* in the list: the game has a password */
} LobbyGame;

typedef struct {
    char code[9];
    LobbyGame g;
} LobbyRow;

/* What the host's entry is now, for logs and tests. */
typedef enum { LOBBY_NONE, LOBBY_REGISTERING, LOBBY_OPEN, LOBBY_REFUSED, LOBBY_FULL_TODAY, LOBBY_UNAVAILABLE,
               LOBBY_CLOSED } LobbyHostState;

/* The lobby address, for example "http://127.0.0.1:8787". An empty or unusable address sends nothing. */
void lobby_set_address(const char *base);
int lobby_usable(void);

/* Host. The first call registers the game. Each later call gives the fields of the next heartbeat (season,
 * day, human clubs, free seats, managers online). password is sent at the registration only. */
void lobby_host_set(const LobbyGame *g, const char *password);
/* The game ended: close its entry. */
void lobby_host_close(void);
LobbyHostState lobby_host_state(char *code_out);    /* code_out: 9 bytes or NULL */
/* The code and the host token of an open entry, for the host's relay connection (relay.h). 1 when open. */
int lobby_host_relay(char code[9], char token[96]);
/* The lobby address that lobby_set_address set. */
const char *lobby_address(void);

/* List. lobby_list_fetch asks for the list. lobby_list_take gives the rows of the last answer once: the count,
 * or -1 while no new answer waits. *result is an LhResult (lobby_http.h). */
void lobby_list_fetch(void);
int lobby_list_take(LobbyRow *rows, int max, int *result);

/* At quit: waits at most ms for a close that is not sent yet. */
void lobby_finish(int ms);

/* A game text as a lobby text: cp1252 bytes that are not printable Latin-1 become '?', spaces at the ends go,
 * and the text is cut to room - 1 bytes. */
void lobby_clean(char *out, unsigned room, const char *in);

/* Tests: the JSON of the list answer as rows, and a game as the JSON of a registration. */
int lobby_parse_games(const char *json, LobbyRow *rows, int max);
int lobby_game_json(char *out, unsigned room, const LobbyGame *g, const char *password, const char *code,
                    const char *token);

#endif
