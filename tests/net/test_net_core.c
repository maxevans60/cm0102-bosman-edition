/* The game's sockets (src/recomp_rt/win/net_core.c, 0.7.0 slice 2): the WinSock behaviour that the original's
 * network code expects, on the host's own sockets over 127.0.0.1 (docs/research/2026-10-27-network-code-reading.md,
 * section 9). Each check names its item of section 9. */
#include "net_core.h"
#include "host_net.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int failures;
#define CHECK(c, ...) do { if (!(c)) { failures++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint32_t loopback(void) { return htonl(0x7f000001u); }

/* Polls f until it gives something other than "would block", at most about two seconds. */
#define POLL(expr, r) do { for (int t_ = 0; t_ < 2000; t_++) { r = (expr); if (r >= 0 || nc_error() != HN_WOULDBLOCK) break; usleep(1000); } } while (0)

static uint32_t tcp(void)
{
    uint32_t s = nc_socket(2, 1);
    CHECK(s != NC_INVALID, "socket: %d", nc_error());
    return s;
}

int main(void)
{
    uint16_t port = (uint16_t)(40000 + getpid() % 20000);
    CHECK(nc_start() == 0, "start");

    /* Listen as open_listen_sockets 0x8ae370 does: SO_REUSEADDR, FIONBIO, bind, listen 16. */
    uint32_t l = tcp();
    CHECK(nc_setopt(l, NC_SOL_SOCKET, NC_SO_REUSEADDR, 1, 0) == 0, "reuseaddr");
    CHECK(nc_nonblocking(l, 1) == 0, "fionbio");
    CHECK(nc_setopt(l, NC_SOL_SOCKET, NC_SO_LINGER, 0, 0) == 0, "linger");
    CHECK(nc_setopt(l, NC_SOL_SOCKET, NC_SO_DEBUG, 1, 0) == 0, "item 2: SO_DEBUG is taken and ignored");
    CHECK(nc_setopt(l, NC_SOL_SOCKET, NC_SO_SNDBUF, 15000, 0) == 0, "sndbuf");
    CHECK(nc_bind(l, loopback(), port) == 0, "bind: %d", nc_error());
    CHECK(nc_listen(l, 16) == 0, "listen");

    uint32_t a = 0, peer = 0;
    uint16_t pp = 0;
    CHECK(nc_accept(l, &peer, &pp) == NC_INVALID && nc_error() == HN_WOULDBLOCK, "item 3: accept with nothing waiting is 10035");

    /* Join as connect_to_host 0x8ae640 does: a blocking connect, then FIONBIO. */
    uint32_t c = tcp();
    CHECK(nc_setopt(c, NC_SOL_SOCKET, NC_SO_REUSEADDR, 1, 0) == 0, "reuseaddr");
    CHECK(nc_connect(c, loopback(), port) == 0, "connect: %d", nc_error());
    CHECK(nc_nonblocking(c, 1) == 0, "fionbio");
    long r;
    POLL((long)(a = nc_accept(l, &peer, &pp)) == (long)NC_INVALID ? -1 : 0, r);
    CHECK(a != NC_INVALID && peer == loopback(), "accept: %d", nc_error());

    /* Item 5: the accepted socket reads without blocking. */
    char buf[64];
    CHECK(nc_recv(a, buf, 1) == -1 && nc_error() == HN_WOULDBLOCK, "item 5: recv on an accepted socket is 10035");

    /* Item 4: send_packet 0x77ac30 sends 4 one-byte length sends, then the body. A body larger than the system
     * takes at once still gives the whole count, with no short send. */
    uint32_t big = 3u << 20;       /* not a packet of the game (over 50,000), so 0 packets */
    uint8_t *body = malloc(big), *got = malloc(big);
    for (uint32_t i = 0; i < big; i++) body[i] = (uint8_t)(i * 7 + 3);
    CHECK(nc_send(c, body, big) == (long)big, "item 4: whole send");
    NcCounts k;
    CHECK(nc_counts(c, &k) && k.queued > 0, "item 4: the rest waits in the queue (%llu)", (unsigned long long)k.queued);
    uint32_t have = 0;
    for (int t = 0; t < 20000 && have < big; t++) {
        long n = nc_recv(a, got + have, big - have);
        if (n > 0) have += (uint32_t)n;
        else if (n < 0 && nc_error() == HN_WOULDBLOCK) usleep(200);
        else break;
    }
    CHECK(have == big && !memcmp(body, got, big), "item 4: all %u bytes arrive in order (%u)", big, have);

    /* A packet of the game: the length bytes wait for the body, and the counts see one packet. */
    uint8_t len[4] = {5, 0, 0, 0};
    for (int i = 0; i < 4; i++) CHECK(nc_send(c, len + i, 1) == 1, "length byte");
    CHECK(nc_counts(c, &k) && k.queued == 4, "the length waits for the body (%llu)", (unsigned long long)k.queued);
    CHECK(nc_send(c, "hello", 5) == 5, "body");
    uint8_t in[9];
    have = 0;
    for (int t = 0; t < 2000 && have < 9; t++) {
        long n = nc_recv(a, in + have, have < 4 ? 1 : 9 - have);   /* as receive_packet 0x77af20 reads */
        if (n > 0) have += (uint32_t)n;
        else usleep(1000);
    }
    CHECK(have == 9 && !memcmp(in, len, 4) && !memcmp(in + 4, "hello", 5), "packet arrives (%u)", have);
    CHECK(nc_counts(c, &k) && k.packets_out == 1 && k.sends == 6 && k.bytes_out == big + 9, "item 13: out counts %llu %llu %llu",
          (unsigned long long)k.packets_out, (unsigned long long)k.sends, (unsigned long long)k.bytes_out);
    CHECK(nc_counts(a, &k) && k.packets_in == 1 && k.bytes_in == big + 9, "item 13: in counts %llu %llu",
          (unsigned long long)k.packets_in, (unsigned long long)k.bytes_in);

    /* The peer closes: recv gives 0 (receive_from_connection 0x8ae9c0 then closes and returns -2). */
    CHECK(nc_close(c) == 0, "close");
    POLL(nc_recv(a, buf, 1), r);
    CHECK(r == 0, "recv after the peer closed: %ld %d", r, nc_error());

    /* Item 9: a second close of a handle fails quietly, and the number is not given out again at once. */
    CHECK(nc_close(c) == -1 && nc_error() == HN_NOTSOCK, "item 9: second close");
    uint32_t c2 = tcp();
    CHECK(c2 != c, "item 9: handle %u is not reused", c);
    nc_close(c2);

    /* Item 8: a connect to a closed port fails at once, and one that no host answers ends in NC_CONNECT_MS. */
    uint32_t d = tcp();
    CHECK(nc_connect(d, loopback(), (uint16_t)(port + 1)) == -1 && nc_error() != HN_WOULDBLOCK, "refused");
    nc_close(d);
    d = tcp();
    uint64_t t0 = hn_ms();
    int cr = nc_connect(d, htonl(0xc0000201u), 6667);   /* 192.0.2.1, TEST-NET-1 */
    uint64_t took = hn_ms() - t0;
    CHECK(cr == -1 && took < NC_CONNECT_MS + 1500, "item 8: connect ends in %llu ms", (unsigned long long)took);
    nc_close(d);
    nc_close(a);
    nc_close(l);

    /* Item 11: two games on one computer bind the same UDP port (the game sets no SO_REUSEADDR, 0x8aeb41..0x8aec05),
     * and a reply to the sender's address and port reaches the game that sent. */
    uint32_t u1 = nc_socket(2, 2), u2 = nc_socket(2, 2);
    CHECK(nc_setopt(u1, NC_SOL_SOCKET, NC_SO_BROADCAST, 1, 0) == 0 && nc_setopt(u2, NC_SOL_SOCKET, NC_SO_BROADCAST, 1, 0) == 0, "broadcast");
    nc_nonblocking(u1, 1), nc_nonblocking(u2, 1);
    CHECK(nc_bind(u1, loopback(), port) == 0, "udp bind 1: %d", nc_error());
    CHECK(nc_bind(u2, loopback(), port) == 0, "item 11: udp bind of a second game: %d", nc_error());
    CHECK(nc_sendto(u2, "CM3 LIST SERVER REQ", 20, loopback(), port) == 20, "sendto");
    uint32_t from = 0;
    uint16_t fport = 0;
    uint32_t got_by = 0;
    for (int t = 0; t < 2000 && !got_by; t++) {
        if (nc_recvfrom(u1, buf, sizeof buf, &from, &fport) == 20) got_by = u1;
        else if (nc_recvfrom(u2, buf, sizeof buf, &from, &fport) == 20) got_by = u2;
        else usleep(1000);
    }
    CHECK(got_by && from == loopback() && fport != port, "the request arrives from the sender's own port (%u)", fport);
    if (got_by) {
        CHECK(nc_sendto(got_by, "CM3 LIST SERVER ACK", 20, from, fport) == 20, "reply");
        long n = -1;
        for (int t = 0; t < 2000 && n < 0; t++) {
            n = nc_recvfrom(u2, buf, sizeof buf, &from, &fport);
            if (n < 0) usleep(1000);
        }
        CHECK(n == 20 && !strcmp(buf, "CM3 LIST SERVER ACK"), "item 11: the reply reaches the game that asked");
        CHECK(nc_recvfrom(u1, buf, sizeof buf, &from, &fport) == -1 && nc_error() == HN_WOULDBLOCK, "and not the other one");
    }
    nc_close(u1);
    nc_close(u2);

    /* Item 10: the adapter table holds no loopback or 0 address, and each broadcast is in its subnet. */
    uint32_t ad[20], bc[20];
    int n = nc_adapters(ad, bc, 20);
    CHECK(n >= 0 && n <= 20, "adapters %d", n);
    for (int i = 0; i < n; i++) {
        CHECK(ad[i] && ad[i] != loopback(), "adapter %d", i);
        CHECK((bc[i] | ad[i]) == bc[i], "broadcast %d covers the address", i);
    }

    free(body);
    free(got);
    printf("%s\n", failures ? "net_core: FAILED" : "net_core: ok");
    return failures != 0;
}
