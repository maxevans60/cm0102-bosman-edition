/* A small zip reader and a SHA-256 (seasons_zip.h). */
#include "seasons_zip.h"
#include "../host.h"

#include <stdlib.h>
#include <string.h>

/* ---- SHA-256 (FIPS 180-4) ---- */

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

#define ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static void sha_block(SeasonsSha *s, const uint8_t *p)
{
    uint32_t w[64];
    for (int i = 0; i < 16; i++)
        w[i] = (uint32_t)p[4 * i] << 24 | (uint32_t)p[4 * i + 1] << 16 | (uint32_t)p[4 * i + 2] << 8 | p[4 * i + 3];
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = ROR(w[i - 15], 7) ^ ROR(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = ROR(w[i - 2], 17) ^ ROR(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    uint32_t a = s->h[0], b = s->h[1], c = s->h[2], d = s->h[3], e = s->h[4], f = s->h[5], g = s->h[6], h = s->h[7];
    for (int i = 0; i < 64; i++) {
        uint32_t t1 = h + (ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25)) + ((e & f) ^ (~e & g)) + K[i] + w[i];
        uint32_t t2 = (ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    s->h[0] += a; s->h[1] += b; s->h[2] += c; s->h[3] += d; s->h[4] += e; s->h[5] += f; s->h[6] += g; s->h[7] += h;
}

void seasons_sha_init(SeasonsSha *s)
{
    static const uint32_t init[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                     0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    memcpy(s->h, init, sizeof init);
    s->bytes = 0;
    s->fill = 0;
}

void seasons_sha_update(SeasonsSha *s, const void *data, size_t n)
{
    const uint8_t *p = data;
    s->bytes += n;
    if (s->fill) {
        size_t take = 64 - s->fill < n ? 64 - s->fill : n;
        memcpy(s->block + s->fill, p, take);
        s->fill += take; p += take; n -= take;
        if (s->fill < 64) return;
        sha_block(s, s->block);
        s->fill = 0;
    }
    for (; n >= 64; p += 64, n -= 64) sha_block(s, p);
    if (n) { memcpy(s->block, p, n); s->fill = n; }
}

void seasons_sha_hex(SeasonsSha *s, char hex[65])
{
    uint64_t bits = s->bytes * 8;
    uint8_t pad[72] = {0x80};
    size_t padlen = (s->fill < 56 ? 56 : 120) - s->fill;
    for (int i = 0; i < 8; i++) pad[padlen + i] = (uint8_t)(bits >> (56 - 8 * i));
    seasons_sha_update(s, pad, padlen + 8);
    static const char digits[] = "0123456789abcdef";
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++) {
            uint8_t b = (uint8_t)(s->h[i] >> (24 - 8 * j));
            hex[i * 8 + j * 2] = digits[b >> 4];
            hex[i * 8 + j * 2 + 1] = digits[b & 15];
        }
    hex[64] = 0;
}

int seasons_sha_file(const char *path, uint64_t *size, char hex[65])
{
    FILE *f = host_fopen(path, "rb");
    if (!f) return 0;
    static uint8_t buf[1 << 16];     /* the game runs this on one thread */
    SeasonsSha s;
    seasons_sha_init(&s);
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) seasons_sha_update(&s, buf, n);
    int bad = ferror(f);
    fclose(f);
    if (bad) return 0;
    if (size) *size = s.bytes;
    seasons_sha_hex(&s, hex);
    return 1;
}

/* ---- CRC-32 ---- */

uint32_t seasons_crc32(uint32_t crc, const void *data, size_t n)
{
    static uint32_t table[256];
    if (!table[1]) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int k = 0; k < 8; k++) c = c & 1 ? 0xedb88320u ^ (c >> 1) : c >> 1;
            table[i] = c;
        }
    }
    const uint8_t *p = data;
    crc = ~crc;
    while (n--) crc = table[(crc ^ *p++) & 255] ^ (crc >> 8);
    return ~crc;
}

/* ---- Inflate (RFC 1951) ---- */

#define WINDOW 32768
#define MAXBITS 15

typedef int (*Sink)(void *ctx, const uint8_t *p, size_t n);

typedef struct {
    /* input: a file part or a memory block */
    FILE *in;
    uint64_t in_left;
    const uint8_t *mem;
    size_t mem_left;
    uint8_t ibuf[1 << 16];
    size_t ipos, ilen;
    uint32_t bitbuf;
    int bitcnt;
    /* output: a circular window that goes to the sink */
    uint8_t win[WINDOW];
    uint64_t total;
    Sink sink;
    void *ctx;
    int fail;
} Inf;

typedef struct {
    uint16_t count[MAXBITS + 1];
    uint16_t symbol[288];
} Huff;

static int in_byte(Inf *s)
{
    if (s->ipos == s->ilen) {
        if (s->mem) {
            if (!s->mem_left) return -1;
            s->ibuf[0] = *s->mem++;       /* one byte path for the test buffer */
            s->mem_left--;
            s->ipos = 0; s->ilen = 1;
        } else {
            size_t want = s->in_left < sizeof s->ibuf ? (size_t)s->in_left : sizeof s->ibuf;
            if (!want) return -1;
            size_t got = fread(s->ibuf, 1, want, s->in);
            if (!got) return -1;
            s->in_left -= got;
            s->ipos = 0; s->ilen = got;
        }
    }
    return s->ibuf[s->ipos++];
}

/* need bits from the stream, least significant first. -1 at the end of the input. */
static int bits(Inf *s, int need)
{
    if (s->fail) return -1;                 /* after the end of the input the reader stays in error */
    uint32_t v = s->bitbuf;
    while (s->bitcnt < need) {
        int b = in_byte(s);
        if (b < 0) { s->fail = 1; return -1; }
        v |= (uint32_t)b << s->bitcnt;
        s->bitcnt += 8;
    }
    s->bitbuf = v >> need;
    s->bitcnt -= need;
    return (int)(v & ((1u << need) - 1));
}

static int flush_window(Inf *s, size_t n)
{
    if (!n) return 0;
    return s->sink(s->ctx, s->win, n);
}

static int put(Inf *s, uint8_t c)
{
    s->win[s->total & (WINDOW - 1)] = c;
    s->total++;
    if (!(s->total & (WINDOW - 1))) return flush_window(s, WINDOW);
    return 0;
}

/* Builds the decoding tables. Returns 0 for a complete code, > 0 for an incomplete one, < 0 for an
 * over-subscribed one. */
static int construct(Huff *h, const uint16_t *length, int n)
{
    uint16_t offs[MAXBITS + 1];
    for (int len = 0; len <= MAXBITS; len++) h->count[len] = 0;
    for (int sym = 0; sym < n; sym++) h->count[length[sym]]++;
    if (h->count[0] == n) return 0;
    int left = 1;
    for (int len = 1; len <= MAXBITS; len++) {
        left <<= 1;
        left -= h->count[len];
        if (left < 0) return left;
    }
    offs[1] = 0;
    for (int len = 1; len < MAXBITS; len++) offs[len + 1] = offs[len] + h->count[len];
    for (int sym = 0; sym < n; sym++)
        if (length[sym]) h->symbol[offs[length[sym]]++] = (uint16_t)sym;
    return left;
}

static int decode(Inf *s, const Huff *h)
{
    int code = 0, first = 0, index = 0;
    for (int len = 1; len <= MAXBITS; len++) {
        int b = bits(s, 1);
        if (b < 0) return -1;
        code |= b;
        int count = h->count[len];
        if (code - count < first) return h->symbol[index + (code - first)];
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return -2;
}

static const uint16_t lens[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
                                  35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
static const uint16_t lext[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                                  3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
static const uint16_t dists[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                                   257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const uint16_t dext[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
                                  7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

static int codes(Inf *s, const Huff *lencode, const Huff *distcode)
{
    for (;;) {
        int sym = decode(s, lencode);
        if (sym < 0) return -1;
        if (sym < 256) {
            if (put(s, (uint8_t)sym)) return -3;
        } else if (sym == 256) {
            return 0;
        } else {
            sym -= 257;
            if (sym >= 29) return -1;
            int e = bits(s, lext[sym]);
            if (e < 0) return -1;
            int len = lens[sym] + e;
            sym = decode(s, distcode);
            if (sym < 0 || sym >= 30) return -1;
            e = bits(s, dext[sym]);
            if (e < 0) return -1;
            uint64_t dist = (uint64_t)dists[sym] + (uint64_t)e;
            if (dist > s->total) return -1;
            while (len--) {
                uint8_t c = s->win[(s->total - dist) & (WINDOW - 1)];
                if (put(s, c)) return -3;
            }
        }
    }
}

static int stored(Inf *s)
{
    s->bitbuf = 0;
    s->bitcnt = 0;
    int b0 = in_byte(s), b1 = in_byte(s), b2 = in_byte(s), b3 = in_byte(s);
    if (b3 < 0) return -1;
    unsigned len = (unsigned)b0 | (unsigned)b1 << 8, nlen = (unsigned)b2 | (unsigned)b3 << 8;
    if (len != (~nlen & 0xffff)) return -1;
    while (len--) {
        int c = in_byte(s);
        if (c < 0) return -1;
        if (put(s, (uint8_t)c)) return -3;
    }
    return 0;
}

static int fixed(Inf *s)
{
    static Huff lencode, distcode;
    static int ready;
    if (!ready) {
        uint16_t l[288];
        int i;
        for (i = 0; i < 144; i++) l[i] = 8;
        for (; i < 256; i++) l[i] = 9;
        for (; i < 280; i++) l[i] = 7;
        for (; i < 288; i++) l[i] = 8;
        construct(&lencode, l, 288);
        for (i = 0; i < 30; i++) l[i] = 5;
        construct(&distcode, l, 30);
        ready = 1;
    }
    return codes(s, &lencode, &distcode);
}

static int dynamic(Inf *s)
{
    static const uint8_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    uint16_t length[320];
    Huff lencode, distcode;
    int nlen = bits(s, 5), ndist = bits(s, 5), ncode = bits(s, 4);
    if (nlen < 0 || ndist < 0 || ncode < 0) return -1;
    nlen += 257; ndist += 1; ncode += 4;
    if (nlen > 286 || ndist > 30) return -1;
    int index;
    for (index = 0; index < ncode; index++) {
        int v = bits(s, 3);
        if (v < 0) return -1;
        length[order[index]] = (uint16_t)v;
    }
    for (; index < 19; index++) length[order[index]] = 0;
    if (construct(&lencode, length, 19) != 0) return -1;
    index = 0;
    while (index < nlen + ndist) {
        int sym = decode(s, &lencode);
        if (sym < 0) return -1;
        if (sym < 16) {
            length[index++] = (uint16_t)sym;
        } else {
            int len = 0, rep;
            if (sym == 16) {
                if (index == 0) return -1;
                len = length[index - 1];
                rep = bits(s, 2);
                if (rep < 0) return -1;
                rep += 3;
            } else if (sym == 17) {
                rep = bits(s, 3);
                if (rep < 0) return -1;
                rep += 3;
            } else {
                rep = bits(s, 7);
                if (rep < 0) return -1;
                rep += 11;
            }
            if (index + rep > nlen + ndist) return -1;
            while (rep--) length[index++] = (uint16_t)len;
        }
    }
    if (length[256] == 0) return -1;
    int err = construct(&lencode, length, nlen);
    if (err && (err < 0 || nlen - lencode.count[0] != 1)) return -1;
    err = construct(&distcode, length + nlen, ndist);
    if (err && (err < 0 || ndist - distcode.count[0] != 1)) return -1;
    return codes(s, &lencode, &distcode);
}

/* Runs the whole stream. 0 on success. The window goes to the sink at the end. */
static int inflate_run(Inf *s)
{
    int last;
    do {
        last = bits(s, 1);
        int type = bits(s, 2);
        if (last < 0 || type < 0) return -1;
        int err = type == 0 ? stored(s) : type == 1 ? fixed(s) : type == 2 ? dynamic(s) : -1;
        if (err) return err;
    } while (!last);
    return flush_window(s, (size_t)(s->total & (WINDOW - 1)));
}

typedef struct { uint8_t *out; size_t size, used; } MemSink;

static int mem_sink(void *ctx, const uint8_t *p, size_t n)
{
    MemSink *m = ctx;
    if (m->used + n > m->size) return -1;
    memcpy(m->out + m->used, p, n);
    m->used += n;
    return 0;
}

long seasons_inflate_buffer(const uint8_t *in, size_t n, uint8_t *out, size_t out_size)
{
    Inf *s = calloc(1, sizeof *s);
    if (!s) return -1;
    MemSink m = {out, out_size, 0};
    s->mem = in;
    s->mem_left = n;
    s->sink = mem_sink;
    s->ctx = &m;
    int err = inflate_run(s);
    free(s);
    return err ? -1 : (long)m.used;
}

/* ---- Zip ---- */

static void set_err(char *err, size_t room, const char *msg)
{
    if (err && room) snprintf(err, room, "%s", msg);
}

static uint32_t rd16(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8; }
static uint32_t rd32(const uint8_t *p) { return rd16(p) | rd16(p + 2) << 16; }

int seasons_zip_open(SeasonsZip *z, const char *path, char *err, size_t err_room)
{
    memset(z, 0, sizeof *z);
    z->f = host_fopen(path, "rb");
    if (!z->f) { set_err(err, err_room, "cannot open the zip"); return -1; }
    if (fseek(z->f, 0, SEEK_END)) goto bad_read;
    long size = ftell(z->f);
    if (size < 22) { set_err(err, err_room, "the file is too short for a zip"); goto fail; }
    long tail = size < 65557 ? size : 65557;
    uint8_t *buf = malloc((size_t)tail);
    if (!buf) { set_err(err, err_room, "out of memory"); goto fail; }
    if (fseek(z->f, size - tail, SEEK_SET) || fread(buf, 1, (size_t)tail, z->f) != (size_t)tail) {
        free(buf);
        goto bad_read;
    }
    long eocd = -1;
    for (long i = tail - 22; i >= 0; i--)
        if (buf[i] == 0x50 && buf[i + 1] == 0x4b && buf[i + 2] == 5 && buf[i + 3] == 6) { eocd = i; break; }
    if (eocd < 0) { free(buf); set_err(err, err_room, "no end of central directory: not a zip"); goto fail; }
    uint32_t total = rd16(buf + eocd + 10), cd_size = rd32(buf + eocd + 12), cd_off = rd32(buf + eocd + 16);
    uint32_t disk = rd16(buf + eocd + 4), cd_disk = rd16(buf + eocd + 6), here = rd16(buf + eocd + 8);
    free(buf);
    if (total == 0xffff || cd_size == 0xffffffffu || cd_off == 0xffffffffu) {
        set_err(err, err_room, "zip64 is not supported");
        goto fail;
    }
    if (disk || cd_disk || here != total) { set_err(err, err_room, "a zip in many parts is not supported"); goto fail; }
    if ((long)cd_off + (long)cd_size > size || cd_size > (64u << 20)) { set_err(err, err_room, "bad central directory"); goto fail; }
    uint8_t *cd = malloc(cd_size ? cd_size : 1);
    z->e = calloc(total ? total : 1, sizeof *z->e);
    if (!cd || !z->e) { free(cd); set_err(err, err_room, "out of memory"); goto fail; }
    if (fseek(z->f, (long)cd_off, SEEK_SET) || fread(cd, 1, cd_size, z->f) != cd_size) { free(cd); goto bad_read; }
    size_t pos = 0;
    for (uint32_t i = 0; i < total; i++) {
        if (pos + 46 > cd_size || rd32(cd + pos) != 0x02014b50u) { free(cd); set_err(err, err_room, "bad central directory entry"); goto fail; }
        const uint8_t *p = cd + pos;
        SeasonsZipEntry *e = &z->e[i];
        uint32_t flags = rd16(p + 8), nl = rd16(p + 28), xl = rd16(p + 30), cl = rd16(p + 32);
        e->method = rd16(p + 10);
        e->crc = rd32(p + 16);
        e->csize = rd32(p + 20);
        e->size = rd32(p + 24);
        e->header = rd32(p + 42);
        if (flags & 1) { free(cd); set_err(err, err_room, "an encrypted zip is not supported"); goto fail; }
        if (e->csize == 0xffffffffu || e->size == 0xffffffffu || e->header == 0xffffffffu) {
            free(cd); set_err(err, err_room, "zip64 is not supported"); goto fail;
        }
        if (nl >= sizeof e->name || pos + 46 + nl + xl + cl > cd_size) { free(cd); set_err(err, err_room, "bad entry name"); goto fail; }
        memcpy(e->name, p + 46, nl);
        e->name[nl] = 0;
        e->is_dir = nl && e->name[nl - 1] == '/';
        pos += 46 + nl + xl + cl;
        z->count = (int)i + 1;
    }
    free(cd);
    return 0;
bad_read:
    set_err(err, err_room, "cannot read the zip");
fail:
    seasons_zip_close(z);
    return -1;
}

void seasons_zip_close(SeasonsZip *z)
{
    if (z->f) fclose(z->f);
    free(z->e);
    memset(z, 0, sizeof *z);
}

typedef struct {
    FILE *out;
    uint32_t crc;
    SeasonsSha sha;
    uint64_t bytes, limit;       /* limit: the size that the central directory gives; more data stops the unpack */
    int write_failed, too_big;
} FileSink;

static int file_sink(void *ctx, const uint8_t *p, size_t n)
{
    FileSink *fs = ctx;
    if (fs->bytes + n > fs->limit) { fs->too_big = 1; return -1; }
    fs->crc = seasons_crc32(fs->crc, p, n);
    seasons_sha_update(&fs->sha, p, n);
    fs->bytes += n;
    if (fwrite(p, 1, n, fs->out) != n) { fs->write_failed = 1; return -1; }
    return 0;
}

int seasons_zip_extract(SeasonsZip *z, int i, const char *path, char sha_hex[65], char *err, size_t err_room)
{
    if (i < 0 || i >= z->count) { set_err(err, err_room, "no such entry"); return -1; }
    SeasonsZipEntry *e = &z->e[i];
    if (e->method != 0 && e->method != 8) { set_err(err, err_room, "an entry uses a compression method that is not supported"); return -1; }
    uint8_t lh[30];
    if (e->header > 0x7fffffffu || fseek(z->f, (long)e->header, SEEK_SET) || fread(lh, 1, 30, z->f) != 30 ||
        rd32(lh) != 0x04034b50u) {
        set_err(err, err_room, "bad local header");
        return -1;
    }
    long data = (long)e->header + 30 + (long)rd16(lh + 26) + (long)rd16(lh + 28);
    if (fseek(z->f, data, SEEK_SET)) { set_err(err, err_room, "cannot read the zip"); return -1; }
    Inf *s = calloc(1, sizeof *s);
    FileSink *fs = calloc(1, sizeof *fs);
    if (!s || !fs) { free(s); free(fs); set_err(err, err_room, "out of memory"); return -1; }
    fs->out = host_fopen(path, "wb");
    if (!fs->out) { free(s); free(fs); set_err(err, err_room, "cannot create a file"); return -1; }
    seasons_sha_init(&fs->sha);
    fs->limit = e->size;
    int rc = 0;
    if (e->method == 0) {
        if (e->csize != e->size) { set_err(err, err_room, "bad stored entry"); rc = -1; }
        s->in = z->f;
        s->in_left = e->csize;
        uint8_t *buf = rc ? NULL : malloc(1 << 16);
        if (!rc && !buf) { set_err(err, err_room, "out of memory"); rc = -1; }
        uint64_t left = e->csize;
        while (!rc && left) {
            size_t want = left < (1 << 16) ? (size_t)left : (1 << 16);
            if (fread(buf, 1, want, z->f) != want) { set_err(err, err_room, "the zip is cut short"); rc = -1; break; }
            if (file_sink(fs, buf, want)) { set_err(err, err_room, "cannot write a file (disk full?)"); rc = -1; break; }
            left -= want;
        }
        free(buf);
    } else {
        s->in = z->f;
        s->in_left = e->csize;
        s->sink = file_sink;
        s->ctx = fs;
        int err2 = inflate_run(s);
        if (err2) {
            set_err(err, err_room, fs->write_failed ? "cannot write a file (disk full?)" :
                                fs->too_big ? "an entry holds more data than its size says" : "the compressed data is damaged");
            rc = -1;
        }
    }
    /* Flush the file to the disk before it can be renamed into place (seasons_db.c). */
    int closed = 0;
    if (!rc && (fflush(fs->out) || host_sync(fs->out))) closed = 1;
    closed |= fclose(fs->out);
    if (!rc && closed) { set_err(err, err_room, "cannot write a file (disk full?)"); rc = -1; }
    if (!rc && (fs->bytes != e->size || fs->crc != e->crc)) { set_err(err, err_room, "an entry does not match its size or CRC-32"); rc = -1; }
    if (!rc && sha_hex) seasons_sha_hex(&fs->sha, sha_hex);
    free(s);
    free(fs);
    if (rc) host_unlink(path);
    return rc;
}
