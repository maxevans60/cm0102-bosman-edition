/* The lobby's HTTP calls (0.7.0 slice 4, #45). Every call of the lobby client (lobby.c) goes through
 * lobby_http, so the secure connection of slice 5 is added here and nowhere else.
 *
 * The rules come from the API contract in server/README.md ("Rules for the game side"): a 10 s timeout,
 * at most 64 KiB of an answer, and an answer without the header X-Bosman-Api: 1 is not from the lobby. Its
 * status 429, or "error code: 1027" in its body (Cloudflare's page for a used-up free plan; a bare "1027"
 * can be part of a Ray ID), means full today, and any
 * other such answer means unavailable. A timeout or a refused connection is unavailable too.
 *
 * Today only http:// works. An https:// address gives LH_UNAVAILABLE with no connection (slice 5). */
#ifndef LOBBY_HTTP_H
#define LOBBY_HTTP_H

#include <stddef.h>

#define LH_TIMEOUT_MS 10000
#define LH_BODY_MAX 65536

typedef enum {
    LH_OK,             /* a 2xx answer of the lobby */
    LH_API_ERROR,      /* an error answer of the lobby: its code is in LhAnswer.code */
    LH_FULL_TODAY,     /* the free plan is used up until 00:00 UTC */
    LH_UNAVAILABLE     /* no answer, or an answer that is not from the lobby */
} LhResult;

typedef struct {
    int status;                    /* the HTTP status, 0 when none came */
    int api;                       /* the answer has X-Bosman-Api: 1 */
    char code[32];                 /* LH_API_ERROR: the "error" field, for example unknown_game */
    size_t len;
    char body[LH_BODY_MAX + 1];    /* the first LH_BODY_MAX bytes, with an end */
} LhAnswer;

/* One call. base is the lobby address, for example "http://127.0.0.1:8787". method is GET, POST or DELETE,
 * or UPGRADE: a GET with the headers of a WebSocket request, which the connection test sends to the relay and
 * which gets no upgrade (0.7.0 slice 7, net_test.c). path starts with "/". token, when not NULL, goes in "Authorization: Bearer". json, when not NULL, is the
 * body. Waits at most timeout_ms in all. Blocks: call it on a thread of its own (the lobby's, or the connection
 * test's), never on the game's. */
LhResult lobby_http(const char *base, const char *method, const char *path, const char *token, const char *json,
                    int timeout_ms, LhAnswer *a);

/* 1 when base is an address that lobby_http can call today. */
int lobby_http_usable(const char *base);

#endif
