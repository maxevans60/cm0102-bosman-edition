/* The game's sockets (0.7.0 slice 2, #45). net.c reads the WSOCK32 arguments from the game's memory and
 * calls these. They keep the game's socket handles and the traffic counts, and give the game the
 * WinSock behaviour that the original's code expects
 * (docs/research/2026-10-27-network-code-reading.md, sections 4 and 9).
 *
 * An address is IPv4 in network byte order, a port a plain number. A call that fails returns -1
 * (NC_INVALID for a handle) and sets the error that nc_error gives (the game's WSAGetLastError). */
#ifndef NET_CORE_H
#define NET_CORE_H

#include <stdint.h>

#define NC_INVALID 0xffffffffu

/* The WinSock option numbers that the game passes (level 0xffff, SOL_SOCKET; level 6, IPPROTO_TCP). */
#define NC_SOL_SOCKET 0xffff
#define NC_IPPROTO_TCP 6
#define NC_SO_DEBUG 0x1
#define NC_SO_REUSEADDR 0x4
#define NC_SO_KEEPALIVE 0x8
#define NC_SO_BROADCAST 0x20
#define NC_SO_LINGER 0x80
#define NC_SO_SNDBUF 0x1001
#define NC_SO_RCVBUF 0x1002
#define NC_TCP_NODELAY 0x1

/* The connect timeout. The original's connect 0x8ae74e waits with no limit. */
#define NC_CONNECT_MS 4000
/* What a close waits to send the bytes still queued. The host closes up to 16 connections at its quit. */
#define NC_CLOSE_MS 250
/* A connection whose queue grows past this is treated as broken. */
#define NC_QUEUE_MAX (16u << 20)

int nc_error(void);
void nc_set_connect_ms(int ms);    /* tests: the connect timeout, NC_CONNECT_MS by default */
void nc_set_error(int code);

int nc_start(void);
uint32_t nc_socket(int af, int type);
int nc_close(uint32_t h);
int nc_nonblocking(uint32_t h, int on);
int nc_readable(uint32_t h, uint32_t *n);
/* value: the int option value. For NC_SO_LINGER: on and seconds. */
int nc_setopt(uint32_t h, int level, int name, int value, int seconds);
int nc_bind(uint32_t h, uint32_t addr, uint16_t port);
int nc_listen(uint32_t h, int backlog);
uint32_t nc_accept(uint32_t h, uint32_t *addr, uint16_t *port);
int nc_connect(uint32_t h, uint32_t addr, uint16_t port);
/* Takes the whole buffer and returns n, or -1 when the connection is broken. Never a short count. */
long nc_send(uint32_t h, const void *buf, uint32_t n);
long nc_recv(uint32_t h, void *buf, uint32_t n);
long nc_sendto(uint32_t h, const void *buf, uint32_t n, uint32_t addr, uint16_t port);
long nc_recvfrom(uint32_t h, void *buf, uint32_t n, uint32_t *addr, uint16_t *port);

/* The adapter table of 0x8aee20: at most max addresses and their subnet broadcasts. The count. */
int nc_adapters(uint32_t *addr, uint32_t *broadcast, int max);

/* Traffic log (RECOMP_NET_LOG=<file>, or "-" for stderr): set by nc_start from the environment, or by a
 * test. Each TCP connection prints a line when it opens and closes, and one line every
 * NC_LOG_MS while it moves bytes. */
#define NC_LOG_MS 10000
void nc_log_to(const char *path);
void nc_log_all(const char *why);

/* The counts of one connection, for tests. 0 when the handle is unknown. */
typedef struct {
    uint64_t bytes_out, bytes_in, packets_out, packets_in, sends, queued;
} NcCounts;
int nc_counts(uint32_t h, NcCounts *c);

#endif
