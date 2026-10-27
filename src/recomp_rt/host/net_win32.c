/* Sockets of 64-bit Windows (host_net.h): WinSock 2, whose error codes are the ones the game expects.
 * The adapters come from GetAdaptersAddresses. */
#include "../host_net.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <string.h>

static int last_error;

static int fail(int code)
{
    last_error = code;
    return -1;
}

static int fail_wsa(void) { return fail(WSAGetLastError()); }

int hn_start(void)
{
    static int started;
    WSADATA d;
    if (!started && WSAStartup(MAKEWORD(2, 2), &d) == 0) started = 1;
    return started ? 0 : fail(HN_SYSNOTREADY);
}

int hn_error(void) { return last_error; }
uint64_t hn_ms(void) { return GetTickCount64(); }

static int make_nonblocking(SOCKET s)
{
    u_long on = 1;
    return ioctlsocket(s, FIONBIO, &on);
}

hn_sock hn_socket(int stream)
{
    SOCKET s = socket(AF_INET, stream ? SOCK_STREAM : SOCK_DGRAM, stream ? IPPROTO_TCP : IPPROTO_UDP);
    if (s == INVALID_SOCKET) return fail_wsa(), HN_INVALID;
    if (make_nonblocking(s)) {
        fail_wsa();
        closesocket(s);
        return HN_INVALID;
    }
    SetHandleInformation((HANDLE)s, HANDLE_FLAG_INHERIT, 0);
    return (hn_sock)s;
}

int hn_close(hn_sock s) { return closesocket((SOCKET)s) ? fail_wsa() : 0; }

int hn_set_flag(hn_sock s, int flag, int on)
{
    int level = SOL_SOCKET, name;
    BOOL v = on != 0;
    switch (flag) {
    case HN_REUSEADDR: name = SO_REUSEADDR; break;
    case HN_REUSEPORT: return 0;        /* SO_REUSEADDR already shares the port on Windows */
    case HN_BROADCAST: name = SO_BROADCAST; break;
    case HN_KEEPALIVE: name = SO_KEEPALIVE; break;
    case HN_NODELAY: level = IPPROTO_TCP; name = TCP_NODELAY; break;
    default: return fail(HN_NOPROTOOPT);
    }
    return setsockopt((SOCKET)s, level, name, (const char *)&v, sizeof v) ? fail_wsa() : 0;
}

int hn_set_linger(hn_sock s, int on, int seconds)
{
    struct linger l;
    l.l_onoff = (u_short)on;
    l.l_linger = (u_short)seconds;
    return setsockopt((SOCKET)s, SOL_SOCKET, SO_LINGER, (const char *)&l, sizeof l) ? fail_wsa() : 0;
}

static struct sockaddr_in sin_of(uint32_t addr, uint16_t port)
{
    struct sockaddr_in a;
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    a.sin_addr.s_addr = addr;
    return a;
}

int hn_bind(hn_sock s, uint32_t addr, uint16_t port)
{
    struct sockaddr_in a = sin_of(addr, port);
    return bind((SOCKET)s, (struct sockaddr *)&a, sizeof a) ? fail_wsa() : 0;
}

int hn_listen(hn_sock s, int backlog) { return listen((SOCKET)s, backlog) ? fail_wsa() : 0; }

hn_sock hn_accept(hn_sock s, uint32_t *addr, uint16_t *port)
{
    struct sockaddr_in a;
    int n = sizeof a;
    SOCKET c = accept((SOCKET)s, (struct sockaddr *)&a, &n);
    if (c == INVALID_SOCKET) return fail_wsa(), HN_INVALID;
    if (make_nonblocking(c)) {
        fail_wsa();
        closesocket(c);
        return HN_INVALID;
    }
    SetHandleInformation((HANDLE)c, HANDLE_FLAG_INHERIT, 0);
    if (addr) *addr = a.sin_addr.s_addr;
    if (port) *port = ntohs(a.sin_port);
    return (hn_sock)c;
}

int hn_wait(hn_sock s, int read, int ms)
{
    fd_set set, bad;
    struct timeval t = {ms / 1000, (ms % 1000) * 1000};
    FD_ZERO(&set);
    FD_ZERO(&bad);
    FD_SET((SOCKET)s, &set);
    FD_SET((SOCKET)s, &bad);
    /* A failed non-blocking connect shows in the except set. */
    int r = select(0, read ? &set : NULL, read ? NULL : &set, &bad, &t);
    if (r == SOCKET_ERROR) return fail_wsa();
    return r > 0;
}

int hn_connect(hn_sock s, uint32_t addr, uint16_t port, int timeout_ms)
{
    struct sockaddr_in a = sin_of(addr, port);
    if (connect((SOCKET)s, (struct sockaddr *)&a, sizeof a) == 0) return 0;
    int e = WSAGetLastError();
    if (e != WSAEWOULDBLOCK && e != WSAEINPROGRESS) return fail(e);
    int r = hn_wait(s, 0, timeout_ms);
    if (r < 0) return -1;
    if (r == 0) return fail(HN_TIMEDOUT);
    int err = 0, n = sizeof err;
    if (getsockopt((SOCKET)s, SOL_SOCKET, SO_ERROR, (char *)&err, &n)) return fail_wsa();
    return err ? fail(err) : 0;
}

long hn_send(hn_sock s, const void *buf, size_t n)
{
    int k = send((SOCKET)s, (const char *)buf, (int)n, 0);
    return k == SOCKET_ERROR ? fail_wsa() : k;
}

long hn_recv(hn_sock s, void *buf, size_t n)
{
    int k = recv((SOCKET)s, (char *)buf, (int)n, 0);
    return k == SOCKET_ERROR ? fail_wsa() : k;
}

long hn_sendto(hn_sock s, const void *buf, size_t n, uint32_t addr, uint16_t port)
{
    struct sockaddr_in a = sin_of(addr, port);
    int k = sendto((SOCKET)s, (const char *)buf, (int)n, 0, (struct sockaddr *)&a, sizeof a);
    return k == SOCKET_ERROR ? fail_wsa() : k;
}

long hn_recvfrom(hn_sock s, void *buf, size_t n, uint32_t *addr, uint16_t *port)
{
    struct sockaddr_in a;
    int len = sizeof a;
    memset(&a, 0, sizeof a);
    int k = recvfrom((SOCKET)s, (char *)buf, (int)n, 0, (struct sockaddr *)&a, &len);
    /* A datagram longer than the buffer: Windows gives WSAEMSGSIZE with the first bytes, as the game's
     * own WinSock did. */
    if (k == SOCKET_ERROR) return fail_wsa();
    if (addr) *addr = a.sin_addr.s_addr;
    if (port) *port = ntohs(a.sin_port);
    return k;
}

int hn_readable_bytes(hn_sock s, uint32_t *n)
{
    u_long v = 0;
    if (ioctlsocket((SOCKET)s, FIONREAD, &v)) return fail_wsa();
    *n = (uint32_t)v;
    return 0;
}

int hn_hostname(char *out, size_t room)
{
    char name[256];
    if (!room) return fail(HN_FAULT);
    if (hn_start() < 0) return -1;
    if (gethostname(name, sizeof name)) return fail_wsa();
    name[sizeof name - 1] = 0;
    strncpy(out, name, room - 1);
    out[room - 1] = 0;
    return 0;
}

int hn_adapters(uint32_t *addr, uint32_t *mask, int max)
{
    ULONG size = 16384;
    IP_ADAPTER_ADDRESSES *list = NULL;
    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    ULONG r = ERROR_BUFFER_OVERFLOW;
    for (int tries = 0; tries < 3 && r == ERROR_BUFFER_OVERFLOW; tries++) {
        free(list);
        list = malloc(size);
        if (!list) return 0;
        r = GetAdaptersAddresses(AF_INET, flags, NULL, list, &size);
    }
    int n = 0;
    if (r == NO_ERROR)
        for (IP_ADAPTER_ADDRESSES *a = list; a && n < max; a = a->Next) {
            if (a->OperStatus != IfOperStatusUp || a->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;
            for (IP_ADAPTER_UNICAST_ADDRESS *u = a->FirstUnicastAddress; u && n < max; u = u->Next) {
                if (!u->Address.lpSockaddr || u->Address.lpSockaddr->sa_family != AF_INET) continue;
                uint32_t v = ((struct sockaddr_in *)u->Address.lpSockaddr)->sin_addr.s_addr;
                if (!v || v == htonl(INADDR_LOOPBACK)) continue;
                unsigned bits = u->OnLinkPrefixLength > 32 ? 32 : u->OnLinkPrefixLength;
                addr[n] = v;
                mask[n] = bits ? htonl(0xffffffffu << (32 - bits)) : 0;
                n++;
            }
        }
    free(list);
    return n;
}
