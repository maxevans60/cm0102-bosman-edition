/* The club name of the save name suggestion (savename.c), as a pure function that tests/savename can
 * run on its own.
 *
 * The game holds names in code page 1252: "Köln" is the bytes 4b f6 6c 6e (club.dat). The path code of
 * the runtime (files.c) passes the bytes of a path to the host as they are, and a host path is UTF-8.
 * A byte of 0x80 or more that is not UTF-8 makes the open fail (tested on macOS: the save stays
 * temp_save_file.sav). So the suggested name is converted to UTF-8, the form that a name typed by the
 * test script has. */
#ifndef RECOMP_CP1252_H
#define RECOMP_CP1252_H

#include <stddef.h>
#include <string.h>

/* The code points of the bytes 0x80 to 0x9f in code page 1252; 0 for a byte that has none. */
static const unsigned short cp1252_high[32] = {
    0x20ac, 0, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021, 0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0, 0x017d, 0,
    0, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0, 0x017e, 0x0178};

/* Copies a club name (code page 1252) to out as UTF-8: at most max characters and size - 1 bytes, without
 * the characters that a file name cannot hold on Windows and without trailing spaces. */
static inline void cp1252_file_name(char *out, size_t size, const char *in, int max)
{
    size_t n = 0;
    int chars = 0;
    for (; *in && chars < max; in++) {
        unsigned c = (unsigned char)*in;
        if (c < 0x20 || c == 0x7f || strchr("\\/:*?\"<>|", (int)c)) continue;
        unsigned cp = c >= 0x80 && c < 0xa0 ? cp1252_high[c - 0x80] : c;
        if (!cp) continue;
        size_t len = cp < 0x80 ? 1 : cp < 0x800 ? 2 : 3;
        if (n + len + 1 > size) break;
        if (len == 1) out[n] = (char)cp;
        else if (len == 2) { out[n] = (char)(0xc0 | cp >> 6); out[n + 1] = (char)(0x80 | (cp & 0x3f)); }
        else { out[n] = (char)(0xe0 | cp >> 12); out[n + 1] = (char)(0x80 | ((cp >> 6) & 0x3f)); out[n + 2] = (char)(0x80 | (cp & 0x3f)); }
        n += len;
        chars++;
    }
    while (n > 0 && out[n - 1] == ' ') n--;
    out[n] = 0;
}

/* A UTF-8 text as code page 1252 (for the game's message box): a character outside the code page, a
 * 4-byte sequence, a stray continuation byte and a cut sequence each become one '?'. A cut sequence ends
 * at the first byte that does not continue it, so the terminating zero is never passed. At most
 * room - 1 bytes. */
static inline void cp1252_from_utf8(char *out, size_t room, const char *in)
{
    size_t n = 0;
    if (!room) return;
    while (*in && n + 1 < room) {
        unsigned char c = (unsigned char)*in;
        if (c < 0x80) { out[n++] = (char)c; in++; continue; }
        int len = c >= 0xf0 ? 4 : c >= 0xe0 ? 3 : c >= 0xc0 ? 2 : 1;
        unsigned cp = len == 4 ? c & 0x07 : len == 3 ? c & 0x0f : len == 2 ? c & 0x1f : 0;
        int used = 1, whole = len > 1;
        for (; used < len; used++) {
            unsigned char b = (unsigned char)in[used];
            if ((b & 0xc0) != 0x80) { whole = 0; break; }      /* includes the terminating zero */
            cp = cp << 6 | (b & 0x3f);
        }
        char b = '?';
        if (whole && len < 4) {
            if (cp >= 0xa0 && cp < 0x100) b = (char)cp;
            for (int i = 0; i < 32; i++) if (cp1252_high[i] && cp1252_high[i] == cp) b = (char)(0x80 + i);
        }
        out[n++] = b;
        in += used;
    }
    out[n] = 0;
}

/* The length of the valid UTF-8 sequence at p (2 to 4 bytes), or 0 when p does not start one: a stray or
 * missing continuation byte, an overlong form (c0 aa), a surrogate or a code point over U+10FFFF. */
static inline int utf8_sequence(const unsigned char *p)
{
    unsigned c = p[0], cp, min;
    int len = c >= 0xc2 && c <= 0xdf ? 2 : c >= 0xe0 && c <= 0xef ? 3 : c >= 0xf0 && c <= 0xf4 ? 4 : 0;
    if (!len) return 0;
    cp = c & (len == 2 ? 0x1f : len == 3 ? 0x0f : 0x07);
    for (int i = 1; i < len; i++) {
        if ((p[i] & 0xc0) != 0x80) return 0;
        cp = cp << 6 | (p[i] & 0x3f);
    }
    min = len == 2 ? 0x80 : len == 3 ? 0x800 : 0x10000;
    if (cp < min || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) return 0;
    return len;
}

/* 1 when s is valid UTF-8 (utf8_sequence for every byte of 0x80 or more). */
static inline int utf8_ok(const char *s)
{
    for (const unsigned char *p = (const unsigned char *)s; *p;) {
        if (*p < 0x80) { p++; continue; }
        int len = utf8_sequence(p);
        if (!len) return 0;
        p += len;
    }
    return 1;
}

/* A file name part as UTF-8, byte by byte: a valid UTF-8 sequence stays as it is, and every other byte of
 * 0x80 or more is a code page 1252 character. The five bytes that 1252 does not define (0x81, 0x8d, 0x8f,
 * 0x90, 0x9d) take their Latin-1 code points. At most room - 1 bytes; a character that does not fit ends it. */
static inline void cp1252_to_utf8(const char *in, char *out, size_t room)
{
    size_t n = 0;
    if (!room) return;
    for (const unsigned char *p = (const unsigned char *)in; *p;) {
        int len = *p < 0x80 ? 1 : utf8_sequence(p);
        if (len) {
            if (n + (size_t)len >= room) break;
            memcpy(out + n, p, (size_t)len);
            n += (size_t)len;
            p += len;
            continue;
        }
        unsigned cp = *p < 0xa0 && cp1252_high[*p - 0x80] ? cp1252_high[*p - 0x80] : *p;
        size_t w = cp < 0x800 ? 2 : 3;
        if (n + w >= room) break;
        if (w == 2) out[n] = (char)(0xc0 | cp >> 6), out[n + 1] = (char)(0x80 | (cp & 0x3f));
        else out[n] = (char)(0xe0 | cp >> 12), out[n + 1] = (char)(0x80 | ((cp >> 6) & 0x3f)),
             out[n + 2] = (char)(0x80 | (cp & 0x3f));
        n += w;
        p++;
    }
    out[n] = 0;
}

#endif
