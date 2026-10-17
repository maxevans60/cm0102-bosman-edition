/* A small zip reader and a SHA-256, for the season databases (seasons_db.h).
 *
 * The reader handles the zips that tools/release/make_season_zips.py writes, and the usual zips of other
 * tools: methods stored (0) and deflate (8), the sizes from the central directory, no encryption, no zip64,
 * no multi-disk. It reads no data of its own into memory: each entry is inflated in pieces of 32 KB.
 * The code is our own (inflate follows RFC 1951, the zip layout follows the PKWARE APPNOTE), so it needs no
 * licence text and no library on Mac, Windows or Linux. */
#ifndef RECOMP_SEASONS_ZIP_H
#define RECOMP_SEASONS_ZIP_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* SHA-256 in pieces. */
typedef struct {
    uint32_t h[8];
    uint64_t bytes;
    uint8_t block[64];
    size_t fill;
} SeasonsSha;

void seasons_sha_init(SeasonsSha *s);
void seasons_sha_update(SeasonsSha *s, const void *data, size_t n);
/* The digest as 64 lower case hex characters and a 0. */
void seasons_sha_hex(SeasonsSha *s, char hex[65]);

/* The size and SHA-256 (hex) of a file. 0 when the file cannot be read. */
int seasons_sha_file(const char *path, uint64_t *size, char hex[65]);

/* CRC-32 (the zip one). Start with crc = 0. */
uint32_t seasons_crc32(uint32_t crc, const void *data, size_t n);

/* One entry of a zip. */
typedef struct {
    char name[260];            /* as in the zip, with '/' */
    uint32_t method;           /* 0 stored, 8 deflate */
    uint32_t crc;
    uint64_t csize, size;      /* compressed and real size */
    uint64_t header;           /* offset of the local header */
    int is_dir;
} SeasonsZipEntry;

typedef struct {
    FILE *f;
    SeasonsZipEntry *e;
    int count;
} SeasonsZip;

/* Opens a zip and reads its central directory. 0 on success, else a negative code and a text in err. */
int seasons_zip_open(SeasonsZip *z, const char *path, char *err, size_t err_room);
void seasons_zip_close(SeasonsZip *z);

/* Inflates entry i into the file at path (created, replaced when it exists). The CRC-32 and the size must
 * equal those of the central directory. When sha_hex is not NULL it gets the SHA-256 of the data. On any
 * failure the output file is removed. 0 on success. */
int seasons_zip_extract(SeasonsZip *z, int i, const char *path, char sha_hex[65], char *err, size_t err_room);

/* Inflates a raw deflate stream of n bytes at in to out (out_size bytes). Returns the bytes written or -1.
 * For tests. */
long seasons_inflate_buffer(const uint8_t *in, size_t n, uint8_t *out, size_t out_size);

#endif
