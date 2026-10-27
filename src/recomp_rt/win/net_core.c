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
 * the adapter's subnet broadcast. Windows delivers a subnet broadcast to such a socket. macOS does not
 * (tested on this Mac, 2026-10-27), and Linux delivers a broadcast only to sockets on any address. And when two games on one computer share port 6667, the
 * system gives a datagram sent to one address and port to only one of them. So a UDP socket of the game
 * is two host sockets: one on any address, port 6667, shared (SO_REUSEADDR and SO_REUSEPORT), that takes
 * the broadcasts, and one on the adapter address with a port of the system's choice, that sends. The
 * host's ACK goes back to the sender's address and port (0x8aed5a), so it reaches the right game. The
 * same on Windows, so every system behaves alike. The game opens one UDP socket for each adapter
 * (0x8aea80), and each shared socket gets every broadcast. So a shared socket keeps only the datagrams
 * whose sender is in its adapter's subnet, as a socket bound to the adapter address on Windows does.
 * Else a host would answer one request once for each adapter, and a joiner that gets one BYE for each
 * adapter would add a row for each copy after the first (0x77b448).
 *
 * Traffic. Each TCP connection counts bytes, sends and packets in both directions. A packet is a
 * 4-byte length and its body (0x77ac30, 0x77b45e).
 *
 * Join check (0.7.0 slice 4, net_core.h). The joiner's side runs inside connect, which the game calls on a
 * blocking socket and waits for (connect_to_host 0x8ae640, connect at 0x8ae74e). The host's side runs inside
 * accept: TcpIp::accept_connection 0x8ae840 polls accept on each listen socket while a player slot is free
 * (0x77af5a). A new connection waits in a list until its block is read, and accept says "would block" until
 * then. So the game sees only joiners that passed, as if the others never came. */
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
    uint32_t size;                 /* the body length of the current packet, until its first byte is seen */
    uint32_t op_packets[32];       /* by the packet's first byte, the opcode of 0x564080 (0..26) */
    uint64_t op_bytes[32];
} Frame;

typedef struct {
    uint32_t handle;               /* 0: the entry is free */
    int stream;
    int blocking;                  /* as the game sees it */
    int connected;
    int dead;                      /* a WinSock code once the connection broke in a background send */
    int broadcast;                 /* SO_BROADCAST, for the UDP send socket */
    int accepted;                  /* a joiner's connection on this host */
    hn_sock s;
    hn_sock out;                   /* UDP: the send socket, else HN_INVALID */
    uint32_t net, mask;            /* UDP: the adapter's subnet, for the shared socket's datagrams */
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

static int connect_ms = NC_CONNECT_MS;

static const NcHooks *hooks;
static int in_tick;
void nc_set_hooks(const NcHooks *h) { hooks = h; }

/* The join check: this game's version and passwords, and the reason of the last refused connect. */
static char my_version[32], host_password[72], join_password[72], refused_version[32];
static int identity, refused;

void nc_set_identity(const char *version, const char *host_pw, const char *join_pw)
{
    snprintf(my_version, sizeof my_version, "%s", version ? version : "");
    snprintf(host_password, sizeof host_password, "%s", host_pw ? host_pw : "");
    snprintf(join_password, sizeof join_password, "%s", join_pw ? join_pw : "");
    identity = version != NULL;
}

int nc_refusal(char *host_version, unsigned room)
{
    if (host_version && room) snprintf(host_version, room, "%s", refused_version);
    return refused;
}

int nc_error(void) { return error_code; }
void nc_set_connect_ms(int ms) { connect_ms = ms; }
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

/* The packets of one direction by opcode: "op=packets/bytes" for each opcode seen. */
static void log_ops(const Sock *k, const char *dir, const Frame *f)
{
    fprintf(log_file, "net ops h=%u %s", k->handle, dir);
    for (int i = 0; i < 32; i++)
        if (f->op_packets[i]) fprintf(log_file, " %d=%u/%llu", i, f->op_packets[i], (unsigned long long)f->op_bytes[i]);
    fprintf(log_file, "\n");
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
    if (!strcmp(what, "close") || !strcmp(what, "end")) {
        log_ops(k, "out", &k->fo);
        log_ops(k, "in", &k->fi);
    }
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
            if (f->size) {         /* the opcode */
                f->op_packets[*p & 31]++, f->op_bytes[*p & 31] += f->size + 4;
                f->size = 0;
            }
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
            f->size = f->body;
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
    if (hooks && hooks->tick && !in_tick) {
        in_tick = 1;
        hooks->tick();
        in_tick = 0;
    }
}

int nc_peers(void)
{
    int n = 0;
    for (int i = 0; i < MAX_SOCKS; i++)
        if (socks[i].handle && socks[i].accepted && socks[i].connected && !socks[i].dead) n++;
    return n;
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
    uint32_t ad[32], mask[32];
    int n = hn_adapters(ad, mask, 32);
    for (int i = 0; i < n; i++)
        if (ad[i] == addr) k->net = addr & mask[i], k->mask = mask[i];
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

static int is_loopback(uint32_t a) { return ((const uint8_t *)&a)[0] == 127; }

/* Connections that wait for their join block (see the head of this file). */
#define PENDING 16
static struct {
    uint32_t listen;               /* the game's listen handle, 0: free */
    hn_sock s;
    uint32_t addr;
    uint16_t port;
    uint64_t since;
    int len;
    char buf[256];
} pending[PENDING];

static void send_line(hn_sock s, const char *line)
{
    size_t n = strlen(line);
    uint64_t until = hn_ms() + 500;
    while (n) {
        long r = hn_send(s, line, n);
        if (r > 0) {
            line += r, n -= (size_t)r;
            continue;
        }
        if ((r < 0 && hn_error() != HN_WOULDBLOCK) || hn_ms() >= until || hn_wait(s, 0, 50) < 0) return;
    }
}

/* The value of "key=" in a join block, cut to room - 1 bytes. */
static void block_value(const char *b, const char *key, char *out, size_t room)
{
    out[0] = 0;
    size_t k = strlen(key);
    for (const char *line = b; *line; line += strcspn(line, "\n") + (line[strcspn(line, "\n")] ? 1 : 0))
        if (!strncmp(line, key, k) && line[k] == '=') {
            size_t n = strcspn(line + k + 1, "\n");
            if (n >= room) n = room - 1;
            memcpy(out, line + k + 1, n);
            out[n] = 0;
            return;
        }
}

/* Reads the block of pending connection i. 1: it passed (the connection goes to the game), 0: not yet,
 * -1: it failed and is closed. */
static int check_pending(int i)
{
    for (;;) {
        if (pending[i].len >= (int)sizeof pending[i].buf - 1) break;
        long r = hn_recv(pending[i].s, pending[i].buf + pending[i].len, 1);
        if (r < 0 && hn_error() == HN_WOULDBLOCK) {
            if (hn_ms() - pending[i].since < NC_HELLO_MS) return 0;
            break;
        }
        if (r <= 0) break;
        pending[i].buf[++pending[i].len] = 0;
        if (pending[i].len < 2 || memcmp(pending[i].buf + pending[i].len - 2, "\n\n", 2)) continue;
        char version[32], password[72];
        block_value(pending[i].buf, "version", version, sizeof version);
        block_value(pending[i].buf, "password", password, sizeof password);
        char line[64];
        const char *why = NULL;
        if (strncmp(pending[i].buf, "BOSMAN HELLO 1\n", 15)) why = "not a join block";
        else if (strcmp(version, my_version)) {
            snprintf(line, sizeof line, "BOSMAN VERSION %s\n", my_version);
            why = "another version";
        } else if (host_password[0] && strcmp(password, host_password)) {
            snprintf(line, sizeof line, "BOSMAN PASSWORD\n");
            why = "a wrong password";
        }
        if (why) {
            if (strcmp(why, "not a join block")) send_line(pending[i].s, line);
            fprintf(stderr, "recomp: net: a joiner was refused: %s (version %s)\n", why, version);
            break;
        }
        send_line(pending[i].s, "BOSMAN OK\n");
        return 1;
    }
    hn_close(pending[i].s);
    pending[i].listen = 0;
    return -1;
}

uint32_t nc_accept(uint32_t h, uint32_t *addr, uint16_t *port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK), NC_INVALID;
    uint32_t a = 0;
    uint16_t p = 0;
    hn_sock s = HN_INVALID;
    for (int i = 0; identity && s == HN_INVALID && i < PENDING; i++)
        if (pending[i].listen == h && check_pending(i) > 0) {
            s = pending[i].s, a = pending[i].addr, p = pending[i].port;
            pending[i].listen = 0;
        }
    while (s == HN_INVALID) {
        s = hn_accept(k->s, &a, &p);
        if (s == HN_INVALID) return fail_host(), NC_INVALID;
        if (!identity || is_loopback(a)) break;
        int i = 0;
        while (i < PENDING && pending[i].listen) i++;
        if (i == PENDING) {
            hn_close(s);
            return fail(HN_WOULDBLOCK), NC_INVALID;
        }
        pending[i].listen = h, pending[i].s = s, pending[i].addr = a, pending[i].port = p;
        pending[i].since = hn_ms(), pending[i].len = 0;
        s = HN_INVALID;
        if (check_pending(i) > 0) {
            s = pending[i].s;
            pending[i].listen = 0;
        }
    }
    Sock *c = new_sock(s, 1);
    if (!c) {
        hn_close(s);
        return fail(10024), NC_INVALID;
    }
    /* Windows gives an accepted socket the mode of the listen socket. The game's listen sockets are
     * non-blocking (0x8ae48d), and it never sets the accepted one (0x8ae840). */
    c->blocking = k->blocking;
    c->accepted = 1;
    connected(c, a, p);
    if (addr) *addr = a;
    if (port) *port = p;
    return c->handle;
}

/* The joiner's side of the join check on a connected socket. 0 when the host said OK. */
static int hello(Sock *k)
{
    char block[160], line[64];
    snprintf(block, sizeof block, "BOSMAN HELLO 1\nversion=%s\npassword=%s\n\n", my_version, join_password);
    send_line(k->s, block);
    uint64_t until = hn_ms() + (uint64_t)connect_ms;
    int n = 0;
    while (n < (int)sizeof line - 1) {
        long r = hn_recv(k->s, line + n, 1);                      /* one byte at a time: the game's bytes follow */
        if (r == 1) {
            if (line[n++] == '\n') break;
            continue;
        }
        if (r == 0 || hn_error() != HN_WOULDBLOCK || hn_ms() >= until || hn_wait(k->s, 1, 50) < 0) break;
    }
    line[n] = 0;
    if (!strcmp(line, "BOSMAN OK\n")) return 0;
    if (!strncmp(line, "BOSMAN VERSION ", 15)) {
        refused = NC_REFUSED_VERSION;
        snprintf(refused_version, sizeof refused_version, "%.*s", (int)strcspn(line + 15, "\n"), line + 15);
    } else refused = !strcmp(line, "BOSMAN PASSWORD\n") ? NC_REFUSED_PASSWORD : NC_REFUSED_NOT_BOSMAN;
    fprintf(stderr, "recomp: net: the host refused the join: %s\n", n ? line : "no answer\n");
    return -1;
}

int nc_connect(uint32_t h, uint32_t addr, uint16_t port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (!k->stream) return fail(10045);                          /* WSAEOPNOTSUPP: the game never does */
    refused = NC_REFUSED_NONE;
    refused_version[0] = 0;
    if (hooks && hooks->connect && hooks->connect(addr, port) < 0) return -1;
    if (hn_connect(k->s, addr, port, connect_ms) < 0) return fail_host();
    if (identity && !is_loopback(addr) && hello(k) < 0) {
        /* The game keeps its handle and closes it after the failed join: give it a fresh socket. */
        hn_close(k->s);
        k->s = hn_socket(1);
        return fail(10061);                                       /* WSAECONNREFUSED */
    }
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

static uint64_t resets_in;   /* opcode 0 packets that the game read from all TCP connections (nc_resets_in) */

long nc_recv(uint32_t h, void *buf, uint32_t n)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    /* Bytes that arrived before a send broke the connection still count, for example the host's quit
     * packet 0x04 (0x7fc738). */
    long r = hn_recv(k->s, buf, n);
    if (r < 0 && k->blocking && hn_error() == HN_WOULDBLOCK && !k->dead && hn_wait(k->s, 1, NC_CONNECT_MS) > 0)
        r = hn_recv(k->s, buf, n);
    if (r < 0 && k->dead && hn_error() == HN_WOULDBLOCK) return fail(k->dead);
    if (r < 0) return fail_host();
    k->n.bytes_in += (uint64_t)r;
    uint64_t resets = k->fi.op_packets[0];
    k->n.packets_in += frame_feed(&k->fi, buf, (uint32_t)r);
    resets_in += k->fi.op_packets[0] - resets;
    return r;
}

uint64_t nc_resets_in(void) { return resets_in; }

long nc_sendto(uint32_t h, const void *buf, uint32_t n, uint32_t addr, uint16_t port)
{
    Sock *k = find(h);
    flush_all(NULL);
    if (!k) return fail(HN_NOTSOCK);
    if (k->stream) return nc_send(h, buf, n);
    long r = hn_sendto(k->out != HN_INVALID ? k->out : k->s, buf, n, addr, port);
    if (r < 0) return fail_host();
    k->n.sends++, k->n.bytes_out += (uint64_t)r, k->n.packets_out++;
    if (hooks && hooks->sent) hooks->sent(buf, n, addr, port);
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
    uint32_t from = 0, a = 0;
    uint16_t p = 0;
    for (int tries = 0; r < 0 && tries < 64; tries++) {
        r = k->out != HN_INVALID ? hn_recvfrom(k->out, buf, n, &a, &p) : -1;
        /* The shared socket: drop the datagrams from other adapters' subnets (see the head of this file). */
        for (int i = 0; r < 0 && i < 64; i++) {
            r = hn_recvfrom(k->s, buf, n, &from, &p);
            if (r >= 0 && k->mask && (from & k->mask) != k->net) r = -1;
            else if (r < 0) break;
            else a = from;
        }
        if (r < 0) break;
        /* The game list (lobby_rows.c) may drop a datagram: then read the next one. */
        if (hooks && hooks->arrived && !hooks->arrived(buf, r, a)) r = -1;
    }
    if (r < 0 && hooks && hooks->extra) {
        r = hooks->extra(buf, n, &a, &p);
        if (r >= 0) {
            if (addr) *addr = a;
            if (port) *port = p;
            k->n.packets_in++;
            return r;
        }
        nc_set_error(HN_WOULDBLOCK);
        return -1;
    }
    if (r < 0) return fail_host();
    if (addr) *addr = a;
    if (port) *port = p;
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
