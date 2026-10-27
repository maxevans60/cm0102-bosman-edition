/* host_link_pass, host_link_listen and host_link_take (host.h): one copy of the game takes the invite links of
 * a user (0.7.0 slice 8, Q41).
 *
 * On Windows and Linux a click on an invite link starts the program again. The first copy listens on a local
 * channel of its own user: a named pipe on Windows, a Unix socket elsewhere. A copy that starts with a link
 * gives the link to the copy that listens and ends, before it opens a window or reads the settings. The channel
 * never uses a network address, so no other computer can reach it, and the pipe or socket file lets only its
 * own user write. The name comes from the user folder, so test runs with their own user folder (RECOMP_USER_DIR)
 * do not meet the player's game.
 *
 * The message is the link and a line end, at most LINK_MAX bytes. The copy that listens answers "ok\n" when it
 * keeps the link. A copy that gets no answer within REPLY_MS starts as before and opens the link itself. */
#include "../host.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINK_MAX 128
#define REPLY_MS 2000

/* FNV-1a of the user folder: the name of the channel. */
static uint32_t name_hash(const char *s)
{
    uint32_t h = 2166136261u;
    for (; *s; s++) h = (h ^ (unsigned char)*s) * 16777619u;
    return h;
}

/* A message as a link: up to the line end, printable ASCII only. 0 when it is not one. */
static int clean_link(const char *in, size_t n, char *out, size_t room)
{
    size_t i = 0;
    for (; i < n && in[i] != '\n'; i++) {
        unsigned char c = (unsigned char)in[i];
        if (c < 0x21 || c > 0x7e || i + 1 >= room) return 0;
        out[i] = (char)c;
    }
    if (i == n || i == 0) return 0;          /* no line end, or empty */
    out[i] = 0;
    return 1;
}

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static char slot[LINK_MAX];
static volatile LONG have;                   /* 1: slot holds a link that the game did not take yet */
static CRITICAL_SECTION lock;
static wchar_t pipe_name[64];

static void make_name(const char *user_dir)
{
    _snwprintf(pipe_name, sizeof pipe_name / sizeof pipe_name[0], L"\\\\.\\pipe\\cm0102-link-%08x",
               (unsigned)name_hash(user_dir));
}

int host_link_pass(const char *user_dir, const char *link)
{
    char msg[LINK_MAX + 1], reply[4] = "";
    size_t n = strlen(link);
    if (n + 1 > LINK_MAX) return 0;
    make_name(user_dir);
    HANDLE p = CreateFileW(pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (p == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PIPE_BUSY && WaitNamedPipeW(pipe_name, REPLY_MS))
        p = CreateFileW(pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (p == INVALID_HANDLE_VALUE) return 0;     /* no copy listens */
    DWORD mode = PIPE_READMODE_BYTE;
    SetNamedPipeHandleState(p, &mode, NULL, NULL);
    snprintf(msg, sizeof msg, "%s\n", link);
    DWORD done = 0;
    int ok = WriteFile(p, msg, (DWORD)(n + 1), &done, NULL) && done == n + 1;
    /* The copy that runs may bring its window to the front. */
    AllowSetForegroundWindow(ASFW_ANY);
    if (ok) {
        DWORD avail = 0, waited = 0;
        while (waited < REPLY_MS && PeekNamedPipe(p, NULL, 0, NULL, &avail, NULL) && avail < 3) Sleep(20), waited += 20;
        ok = avail >= 3 && ReadFile(p, reply, 3, &done, NULL) && done == 3 && !memcmp(reply, "ok\n", 3);
    }
    CloseHandle(p);
    return ok;
}

static HANDLE new_pipe(DWORD first)
{
    return CreateNamedPipeW(pipe_name, PIPE_ACCESS_DUPLEX | first, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT |
                            PIPE_REJECT_REMOTE_CLIENTS, 1, 64, LINK_MAX, 0, NULL);
}

/* p: the first instance of the pipe. After each client the thread makes the next one. */
static DWORD WINAPI listen_thread(LPVOID first)
{
    HANDLE p = first;
    for (;; p = new_pipe(0)) {
        if (p == INVALID_HANDLE_VALUE) return 0;
        if (ConnectNamedPipe(p, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            char buf[LINK_MAX], link[LINK_MAX];
            DWORD got = 0, n;
            while (got < sizeof buf && ReadFile(p, buf + got, (DWORD)(sizeof buf - got), &n, NULL) && n) {
                got += n;
                if (memchr(buf, '\n', got)) break;
            }
            if (clean_link(buf, got, link, sizeof link)) {
                EnterCriticalSection(&lock);
                memcpy(slot, link, sizeof slot);
                have = 1;
                LeaveCriticalSection(&lock);
                WriteFile(p, "ok\n", 3, &n, NULL);
                FlushFileBuffers(p);
            }
            DisconnectNamedPipe(p);
        }
        CloseHandle(p);
    }
}

int host_link_listen(const char *user_dir)
{
    make_name(user_dir);
    /* FILE_FLAG_FIRST_PIPE_INSTANCE fails when another copy has the pipe: then this copy does not listen. */
    HANDLE first = new_pipe(FILE_FLAG_FIRST_PIPE_INSTANCE);
    if (first == INVALID_HANDLE_VALUE) return -1;
    InitializeCriticalSection(&lock);
    HANDLE t = CreateThread(NULL, 0, listen_thread, first, 0, NULL);
    if (!t) {
        CloseHandle(first);
        return -1;
    }
    CloseHandle(t);
    return 0;
}

int host_link_take(char *out, size_t room)
{
    if (!have) return 0;
    EnterCriticalSection(&lock);
    snprintf(out, room, "%s", slot);
    have = 0;
    LeaveCriticalSection(&lock);
    return 1;
}

#else

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0                            /* macOS: SO_NOSIGPIPE on the socket (no_sigpipe) */
#endif

static char slot[LINK_MAX];
static volatile int have;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int server = -1;
static struct sockaddr_un where;

/* $XDG_RUNTIME_DIR/cm0102-link-<hash>.sock (Linux, a folder of the user only), else <user folder>/link.sock.
 * 0 when the path does not fit in a socket address. */
static int make_address(const char *user_dir)
{
    const char *run = getenv("XDG_RUNTIME_DIR");
    int n;
    memset(&where, 0, sizeof where);
    where.sun_family = AF_UNIX;
    if (run && run[0] == '/')
        n = snprintf(where.sun_path, sizeof where.sun_path, "%s/cm0102-link-%08x.sock", run, (unsigned)name_hash(user_dir));
    else n = snprintf(where.sun_path, sizeof where.sun_path, "%s/link.sock", user_dir);
    return n > 0 && (size_t)n < sizeof where.sun_path;
}

/* A peer that closes early must not end the game with SIGPIPE. */
static void no_sigpipe(int s)
{
#ifdef SO_NOSIGPIPE
    int one = 1;
    setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof one);
#else
    (void)s;
#endif
}

static int connect_to(void)
{
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) return -1;
    no_sigpipe(s);
    if (connect(s, (struct sockaddr *)&where, sizeof where)) {
        close(s);
        return -1;
    }
    return s;
}

int host_link_pass(const char *user_dir, const char *link)
{
    char msg[LINK_MAX + 1], reply[4] = "";
    size_t n = strlen(link), got = 0;
    if (n + 1 > LINK_MAX || !make_address(user_dir)) return 0;
    int s = connect_to();
    if (s < 0) return 0;                          /* no copy listens */
    snprintf(msg, sizeof msg, "%s\n", link);
    int ok = send(s, msg, n + 1, MSG_NOSIGNAL) == (ssize_t)(n + 1);
    while (ok && got < 3) {
        struct pollfd pf = {s, POLLIN, 0};
        if (poll(&pf, 1, REPLY_MS) <= 0) break;
        ssize_t r = recv(s, reply + got, 3 - got, 0);
        if (r <= 0) break;
        got += (size_t)r;
    }
    close(s);
    return ok && got == 3 && !memcmp(reply, "ok\n", 3);
}

static void *listen_thread(void *unused)
{
    (void)unused;
    for (;;) {
        int c = accept(server, NULL, NULL);
        if (c < 0) {
            if (errno == EINTR) continue;
            return NULL;
        }
        char buf[LINK_MAX], link[LINK_MAX];
        size_t got = 0;
        no_sigpipe(c);
        while (got < sizeof buf) {
            struct pollfd pf = {c, POLLIN, 0};
            if (poll(&pf, 1, REPLY_MS) <= 0) break;
            ssize_t r = recv(c, buf + got, sizeof buf - got, 0);
            if (r <= 0) break;
            got += (size_t)r;
            if (memchr(buf, '\n', got)) break;
        }
        if (clean_link(buf, got, link, sizeof link)) {
            pthread_mutex_lock(&lock);
            memcpy(slot, link, sizeof slot);
            have = 1;
            pthread_mutex_unlock(&lock);
            send(c, "ok\n", 3, MSG_NOSIGNAL);
        }
        close(c);
    }
}

static void remove_socket(void)
{
    if (server >= 0) unlink(where.sun_path);
}

int host_link_listen(const char *user_dir)
{
    if (!make_address(user_dir)) return -1;
    int probe = connect_to();
    if (probe >= 0) {                             /* another copy listens */
        close(probe);
        return -1;
    }
    unlink(where.sun_path);                       /* a file that a copy left when it crashed */
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) return -1;
    mode_t old = umask(077);                      /* only this user may connect */
    int bad = bind(s, (struct sockaddr *)&where, sizeof where) || listen(s, 4);
    umask(old);
    if (bad) {
        close(s);
        return -1;
    }
    server = s;
    atexit(remove_socket);
    pthread_t t;
    if (pthread_create(&t, NULL, listen_thread, NULL)) return -1;
    pthread_detach(t);
    return 0;
}

int host_link_take(char *out, size_t room)
{
    if (!have) return 0;
    pthread_mutex_lock(&lock);
    snprintf(out, room, "%s", slot);
    have = 0;
    pthread_mutex_unlock(&lock);
    return 1;
}

#endif
