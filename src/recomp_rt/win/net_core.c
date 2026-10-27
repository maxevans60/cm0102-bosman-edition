/* The game's sockets (net_core.h). Items 2 to 9, 11 and 13 of section 9 of
 * docs/research/2026-10-27-network-code-reading.md.
 *
 * Handles. The game keeps a socket as a dword and compares it with -1. Each new socket gets the next
 * number, so a handle that the game closes twice (0x8aec17, then 0x8aec24) is unknown the second time
 * and that close fails quietly with WSAENOTSOCK.
 *
 * Blocking. Every host socket is non-blocking. The game sets each socket that it reads non-blocking
 * (FIONBIO), and blocks only in connect (0x8ae74e, 0x8af4e6). connect waits at most NC_CONNECT_MS, so a
 * wrong address does not freeze the game.
 *
 * Whole sends. Network::send_packet 0x77ac30 sends a 4-byte length in four 1-byte sends, then the body,
 * and checks only for -2 from TcpIp::send_to_connection 0x8ae8d0, which can return a short count
 * (0x8ae966). So nc_send takes the whole buffer every time. What the system cannot take now waits in a
 * queue that each later call moves on. The queue also holds the length bytes until the body follows,
 * so a packet leaves in one send with TCP_NODELAY: no wait on the peer's delayed acknowledgement between
 * the length and the body. Any call other than a send to that socket sends what is held.
 *
 * UDP. The game binds each UDP socket to an adapter address on port 6667 (0x8aec05) and broadcasts to
 * the adapter's subnet broadcast. Windows delivers a subnet broadcast to such a socket. macOS and Linux
 * do not (tested on this Mac, 2026-10-27). And when two games on one computer share port 6667, the
 * system gives a datagram sent to one address and port to only one of them. So a UDP socket of the game
 * is two host sockets: one on any address, port 6667, shared (SO_REUSEADDR and SO_REUSEPORT), that takes
 * the broadcasts, and one on the adapter address with a port of the system's choice, that sends. The
 * host's ACK goes back to the sender's address and port (0x8aed5a), so it reaches the right game. The
 * same on Windows, so every system behaves alike.
 *
 * Traffic. Each TCP connection counts bytes, sends and packets in both directions. A packet is a
 * 4-byte length and its body (0x77ac30, 0x77b45e). */
#include "net_core.h"
#include "../host_net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SOCKS 256
#define FIRST_HANDLE 0x100u

typedef struct {                   /* the 4-byte length framing of one direction */
    int have;                      /* length bytes seen of the current packet, 0..4 */
    uint8_t len[4];
    uint32_t body;                 /* body bytes still to come */
} Frame;

typedef struct {
    uint32_t handle;               /* 0: the entry is free */
    int stream;
    int blocking;                  /* as the game sees it */
    int connected;
    int dead;                      /* a WinSock code once the connection broke in a background send */
    int broadcast;                 /* SO_BROADCAST, for the UDP send socket */
    hn_sock s;
    hn_sock out;                   /* UDP: the send socket, else HN_INVALID */
    uint8_t *q;                    /* send queue */
    size_t qlen, qoff, qcap;
    uint32_t peer;
    uint16_t peer_port;
    uint64_t opened;
    NcCounts n, last;              /* last: the counts at the last log line */
    Frame fo, fi;
} Sock;

static Sock socks[MAX_SOCKS];
static uint32_t next_handle = FIRST_HANDLE;
static int error_code;
static FILE *log_file;
static uint64_t last_log;
static int log_ready;

int nc_error(void) { return error_code; }
void nc_set_error(int code) { error_code = code; }

static int fail(int code)
{
    error_code = code;
    return -1;
}

static int fail_host(void) { return fail(hn_error()); }

static Sock *find(uint32_t h)
{
    if (h < FIRST_HANDLE) return NULL;
    for (int i = 0; i < MAX_SOCKS; i++)
        if (socks[i].handle == h) return &socks[i];
    return NULL;
}

/* ---- traffic log ---- */

static void ip_text(char *out, size_t room, uint32_t a)
{
    const uint8_t *b = (const uint8_t *)&a;
    snprintf(out, room, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
}

void nc_log_to(const char *path)
{
    if (log_file && log_file != stderr) fclose(log_file);
    log_file = NULL;
    if (!path || !*path) return;
    log_file = strcmp(path, "-") ? fopen(path, "a") : stderr;
    if (log_file) last_log = hn_ms();
}

static void log_line(const Sock *k, const char *what, const NcCounts *c)
{
    char peer[20];
    ip_text(peer, sizeof peer, k->peer);
    fprintf(log_file, "net %s t=%llu h=%u peer=%s:%u secs=%llu out=%llu bytes %llu packets %llu sends in=%llu bytes %llu packets\n",
            what, (unsigned long long)hn_ms(), k->handle, peer, k->peer_port,
            (unsigned long long)((hn_ms() - k->opened) / 1000), (unsigned long long)c->bytes_out,
            (unsigned long long)c->packets_out, (unsigned long long)c->sends,
            (unsigned long long)c->bytes_in, (unsigned long long)c->packets_in);
    fflush(log_file);
}

/* Every NC_LOG_MS: one line for each connection that moved bytes, with the counts since the last line. */
static void log_tick(void)
{
    if (!log_file) return;
    uint64_t now = hn_ms();
    if (now - last_log < NC_LOG_MS) return;
    last_log = now;
    for (int i = 0; i < MAX_SOCKS; i++) {
        Sock *k = &socks[i];
        if (!k->handle || !k->connected) continue;
        NcCounts d = {k->n.bytes_out - k->last.bytes_out, k->n.bytes_in - k->last.bytes_in,
                      k->n.packets_out - k->last.packets_out, k->n.packets_in - k->last.packets_in,
                      k->n.sends - k->last.sends, 0};
        k->last = k->n;
        if (d.bytes_out || d.bytes_in) log_line(k, "tick", &d);
    }
}

void nc_log_all(const char *why)
{
    if (!log_file) return;
    for (int i = 0; i < MAX_SOCKS; i++)
        if (socks[i].handle && socks[i].connected) log_line(&socks[i], why, &socks[i].n);
}

static void log_at_exit(void) { nc_log_all("end"); }

/* Feeds n bytes of one direction to its framing. The count of packets that end in them. */
static unsigned frame_feed(Frame *f, const uint8_t *p, uint32_t n)
{
    unsigned done = 0;
    while (n) {
        if (f->body) {
            uint32_t take = n < f->body ? n : f->body;
            f->body -= take, p += take, n -= take;
            if (!f->body) done++;
            continue;
        }
        f->len[f->have++] = *p++, n--;
        if (f->have == 4) {
            f->have = 0;
            f->body = (uint32_t)f->len[0] | (uint32_t)f->len[1] << 8 | (uint32_t)f->len[2] << 16 |
                      (uint32_t)f->len[3] << 24;
            /* A length of 0 or above 50,000 is not a packet of the game (0x77ac43, 0x77acb4). */
            if (f->body > 50000u) f->body = 0;
        }
    }
    return done;
}

static int frame_between(const Frame *f) { return !f->have && !f->body; }

/* ---- send queue ---- */

/* Sends what the queue holds, as far as the system takes it. -1 when the connection broke. */
static int flush(Sock *k)
{
    while (k->qoff < k->qlen && !k->dead) {
        long r = hn_send(k->s, k->q + k->qoff, k->qlen - k->qoff);
        if (r > 0) {
            k->qoff += (size_t)r;
            continue;
        }
        if (r < 0 && hn_error() == HN_WOULDBLOCK) break;
        k->dead = r < 0 ? hn_error() : HN_CONNRESET;
    }
    if (k->qoff == k->qlen) k->qoff = k->qlen = 0;
    k->n.queued = k->qlen - k->qoff;
    return k->dead ? -1 : 0;
}

/* Every call moves the queues on, but a send leaves the bytes that it holds back for its own packet. */
static void flush_all(const Sock *except)
{
    for (int i = 0; i < MAX_SOCKS; i++)
        if (socks[i].handle && &socks[i] != except && socks[i].qlen) flush(&socks[i]);
    log_tick();
}

static int enqueue(Sock *k, const uint8_t *p, uint32_t n)
{
    if (k->qoff && k->qoff == k->qlen) k->qoff = k->qlen = 0;
    if (k->qlen - k->qoff + n > NC_QUEUE_MAX) {
        k->dead = HN_CONNRESET;
        return -1;
    }
    if (k->qoff > k->qcap / 2) {   /* move the rest to the front before the queue grows */
        memmove(k->q, k->q + k->qoff, k->qlen - k->qoff);
        k->qlen -= k->qoff, k->qoff = 0;
    }
    if (k->qlen + n > k->qcap) {
        size_t cap = k->qcap ? k->qcap : 4096;
        while (cap < k->qlen + n) cap *= 2;
        uint8_t *q = realloc(k->q, cap);
        if (!q) {
            k->dead = HN_CONNRESET;
            return -1;
        }
        k->q = q, k->qcap = cap;
    }
    memcpy(k->q + k->qlen, p, n);
    k->qlen += n;
    return 0;
}

/* ---- calls ---- */

int nc_start(void)
{
    if (!log_ready) {
        log_ready = 1;
        const char *path = getenv("RECOMP_NET_LOG");
        if (path && *path && !log_file) nc_log_to(path);
        atexit(log_at_exit);
    }
    return hn_start() < 0 ? fail_host() : 0;
}

static Sock *new_sock(hn_sock s, int stream)
{
    for (int i = 0; i < MAX_SOCKS; i++)
        if (!socks[i].handle) {
            Sock *k = &socks[i];
            memset(k, 0, sizeof *k);
            k->handle = next_handle++;
            if (next_handle == NC_INVALID) next_handle = FIRST_HANDLE;
            k->stream = stream;
            k->blocking = 1;
            k->s = s;
            k->out = HN_INVALID;
            k->opened = hn_ms();
            return k;
        }
    return NULL;
}

uint32_t nc_socket(int af, int type)
{
    flush_all(NULL);
    if (af != 2) return fail(HN_AFNOSUPPORT), NC_INVALID;
    if (type != 1 && type != 2) return fail(10044), NC_INVALID;   /* WSAESOCKTNOSUPPORT */
    if (hn_start() < 0) return fail_host(), NC_INVALID;
    hn_sock s = hn_socket(type == 1);
    if (s == HN_INVALID) return fail_host(), NC_INVALID;
    Sock *k = new_sock(s, type == 1);
    if (!k) {
        hn_close(s);
        return fail(10024), NC_INVALID;                           /* WSAEMFILE */
    }
    return k->handle;
}

int nc_close(uint32_t h)
{
    Sock *k = find(h);
    if (!k) return fail(HN_NOTSOCK);
    flush_all(k);
    /* The bytes still queued, for example the quit packet 0x04 of disconnect_network 0x7fc690. */
    uint64_t until = hn_ms() + NC_CLOSE_MS;
    while (k->qlen && !k->dead && flush(k) == 0 && k->qlen && hn_ms() < until)
        hn_wait(k->s, 0, 50);
    if (log_file && k->connected) log_line(k, "close", &k->n);
    hn_close(k->s);
    if (k->out != HN_INVALID) hn_close(k->out);
    free(k->q);
    memset(k, 0, sizeof *k);
    return 0;
}

int nc_nonblocking(uint32_t h, int on)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    k->blocking = !on;
    return 0;
}

int nc_readable(uint32_t h, uint32_t *n)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    return hn_readable_bytes(k->s, n) < 0 ? fail_host() : 0;
}

static int set_flag(Sock *k, int flag, int on)
{
    if (hn_set_flag(k->s, flag, on) < 0) return fail_host();
    if (k->out != HN_INVALID) hn_set_flag(k->out, flag, on);
    return 0;
}

int nc_setopt(uint32_t h, int level, int name, int value, int seconds)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (level == NC_IPPROTO_TCP && name == NC_TCP_NODELAY) return set_flag(k, HN_NODELAY, value);
    if (level != NC_SOL_SOCKET) return fail(HN_NOPROTOOPT);
    switch (name) {
    case NC_SO_REUSEADDR: return set_flag(k, HN_REUSEADDR, value);
    case NC_SO_KEEPALIVE: return set_flag(k, HN_KEEPALIVE, value);
    case NC_SO_BROADCAST:
        k->broadcast = value != 0;
        return set_flag(k, HN_BROADCAST, value);
    case NC_SO_LINGER:
        /* TCP only: the game's {1, 0} on UDP (0x8aeb9e) means nothing there. */
        if (k->stream && hn_set_linger(k->s, value, seconds) < 0) return fail_host();
        return 0;
    case NC_SO_DEBUG:              /* Linux refuses it to normal users; the game does not check it */
    case NC_SO_SNDBUF:             /* 15,000 bytes (0x8ae54c): the system's own size is larger */
    case NC_SO_RCVBUF:
        return 0;
    default:
        return fail(HN_NOPROTOOPT);
    }
}

int nc_bind(uint32_t h, uint32_t addr, uint16_t port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (k->stream) return hn_bind(k->s, addr, port) < 0 ? fail_host() : 0;
    /* UDP: see the head of this file. */
    if (k->out != HN_INVALID) return fail(HN_INVAL);
    hn_set_flag(k->s, HN_REUSEADDR, 1);
    hn_set_flag(k->s, HN_REUSEPORT, 1);
    if (hn_bind(k->s, 0, port) < 0) return fail_host();
    hn_sock out = hn_socket(0);
    if (out == HN_INVALID) return fail_host();
    if (k->broadcast) hn_set_flag(out, HN_BROADCAST, 1);
    if (hn_bind(out, addr, 0) < 0) {
        int e = hn_error();
        hn_close(out);
        return fail(e);
    }
    k->out = out;
    return 0;
}

int nc_listen(uint32_t h, int backlog)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    return hn_listen(k->s, backlog) < 0 ? fail_host() : 0;
}

static void connected(Sock *k, uint32_t addr, uint16_t port)
{
    k->connected = 1;
    k->peer = addr, k->peer_port = port;
    k->opened = hn_ms();
    hn_set_flag(k->s, HN_NODELAY, 1);
    if (log_file) log_line(k, "open", &k->n);
}

uint32_t nc_accept(uint32_t h, uint32_t *addr, uint16_t *port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK), NC_INVALID;
    uint32_t a = 0;
    uint16_t p = 0;
    hn_sock s = hn_accept(k->s, &a, &p);
    if (s == HN_INVALID) return fail_host(), NC_INVALID;
    Sock *c = new_sock(s, 1);
    if (!c) {
        hn_close(s);
        return fail(10024), NC_INVALID;
    }
    /* Windows gives an accepted socket the mode of the listen socket. The game's listen sockets are
     * non-blocking (0x8ae48d), and it never sets the accepted one (0x8ae840). */
    c->blocking = k->blocking;
    connected(c, a, p);
    if (addr) *addr = a;
    if (port) *port = p;
    return c->handle;
}

int nc_connect(uint32_t h, uint32_t addr, uint16_t port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (!k->stream) return fail(10045);                          /* WSAEOPNOTSUPP: the game never does */
    if (hn_connect(k->s, addr, port, NC_CONNECT_MS) < 0) return fail_host();
    connected(k, addr, port);
    return 0;
}

long nc_send(uint32_t h, const void *buf, uint32_t n)
{
    Sock *k = find(h);
    flush_all(k);
    if (!k) return fail(HN_NOTSOCK);
    if (!k->stream) return nc_sendto(h, buf, n, k->peer, k->peer_port);
    if (!k->connected) return fail(HN_NOTCONN);
    if (k->dead) return fail(k->dead);
    if (enqueue(k, buf, n) < 0) return fail(k->dead);
    k->n.sends++;
    k->n.bytes_out += n;
    k->n.packets_out += frame_feed(&k->fo, buf, n);
    /* Hold the length bytes until the body follows. */
    if (frame_between(&k->fo) && flush(k) < 0) return fail(k->dead);
    k->n.queued = k->qlen - k->qoff;
    return (long)n;
}

long nc_recv(uint32_t h, void *buf, uint32_t n)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (k->dead) return fail(k->dead);
    long r = hn_recv(k->s, buf, n);
    if (r < 0 && k->blocking && hn_error() == HN_WOULDBLOCK && hn_wait(k->s, 1, NC_CONNECT_MS) > 0)
        r = hn_recv(k->s, buf, n);
    if (r < 0) return fail_host();
    k->n.bytes_in += (uint64_t)r;
    k->n.packets_in += frame_feed(&k->fi, buf, (uint32_t)r);
    return r;
}

long nc_sendto(uint32_t h, const void *buf, uint32_t n, uint32_t addr, uint16_t port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (k->stream) return nc_send(h, buf, n);
    long r = hn_sendto(k->out != HN_INVALID ? k->out : k->s, buf, n, addr, port);
    if (r < 0) return fail_host();
    k->n.sends++, k->n.bytes_out += (uint64_t)r, k->n.packets_out++;
    return r;
}

long nc_recvfrom(uint32_t h, void *buf, uint32_t n, uint32_t *addr, uint16_t *port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (k->stream) {
        long r = nc_recv(h, buf, n);
        if (r >= 0 && addr) *addr = k->peer;
        if (r >= 0 && port) *port = k->peer_port;
        return r;
    }
    long r = -1;
    if (k->out != HN_INVALID) r = hn_recvfrom(k->out, buf, n, addr, port);
    if (r < 0) r = hn_recvfrom(k->s, buf, n, addr, port);
    if (r < 0) return fail_host();
    k->n.bytes_in += (uint64_t)r, k->n.packets_in++;
    return r;
}

int nc_adapters(uint32_t *addr, uint32_t *broadcast, int max)
{
    uint32_t mask[32];
    if (max > 32) max = 32;
    int n = hn_adapters(addr, mask, max);
    for (int i = 0; i < n; i++) broadcast[i] = addr[i] | ~mask[i];   /* as 0x8af24c..0x8af267 */
    return n;
}

int nc_counts(uint32_t h, NcCounts *c)
{
    Sock *k = find(h);
    if (!k) return 0;
    *c = k->n;
    c->queued = k->qlen - k->qoff;
    return 1;
}
