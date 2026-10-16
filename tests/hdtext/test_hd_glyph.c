/* Test of the HD text sizes and the glyph scale of the Traditional fonts (src/recomp_rt/win/hd_glyph.c).
 * Usage: hd_glyph_test */
#include "hd_glyph.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

/* The nibble at (x, y) of a packed glyph w pixels wide: the high nibble first, rows of (w + 1) / 2 bytes. */
static int px(const uint8_t *g, int w, int x, int y)
{
    uint8_t b = g[y * ((w + 1) / 2) + x / 2];
    return x % 2 ? b & 15 : b >> 4;
}

int main(void)
{
    /* 1. The sizes of the spec (Q224): 9, 10, 14, 16, 18, 20, 22 become 12, 14, 19, 22, 24, 27, 30. */
    static const uint32_t from[] = {9, 10, 14, 16, 18, 20, 22}, to[] = {12, 14, 19, 22, 24, 27, 30};
    for (int i = 0; i < 7; i++) CHECK(hd_glyph_size(from[i]) == to[i]);
    /* Baselines and line heights. */
    CHECK(hd_glyph_size(3) == 4 && hd_glyph_size(4) == 5 && hd_glyph_size(9) == 12 && hd_glyph_size(11) == 15);
    CHECK(hd_glyph_size(15) == 20 && hd_glyph_size(18) == 24 && hd_glyph_size(21) == 28 && hd_glyph_size(24) == 32);
    CHECK(hd_glyph_size(27) == 36 && hd_glyph_size(39) == 53 && hd_glyph_size(45) == 61);
    CHECK(hd_glyph_size(0) == 0 && hd_glyph_size(1) == 1);

    /* 2. A full glyph stays full, an empty one empty, and the packing has (w + 1) / 2 bytes a row. */
    uint8_t full[3 * 4], out[64];
    memset(full, 0xff, sizeof full);
    memset(out, 0xaa, sizeof out);
    hd_glyph_scale(full, 5, 4, out, 7, 5);          /* 5x4 to 7x5: 4 bytes a row, 20 bytes */
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 7; x++) CHECK(px(out, 7, x, y) == 15);
    CHECK((out[3] & 15) == 0);                      /* the unused low nibble of an odd row is 0 */
    CHECK(out[20] == 0xaa);                         /* nothing after the glyph */
    uint8_t empty[3 * 4] = {0};
    hd_glyph_scale(empty, 5, 4, out, 7, 5);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 7; x++) CHECK(px(out, 7, x, y) == 0);

    /* 3. A vertical stroke one pixel wide in the middle of a 3-wide glyph: the middle column of the bigger
     * glyph keeps most of its strength, the edges get less, and every value is 0 to 15. No stroke is lost. */
    uint8_t stroke[2 * 3] = {0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00};   /* rows: 0 15 0 */
    hd_glyph_scale(stroke, 3, 3, out, 4, 4);
    for (int y = 0; y < 4; y++) {
        int max = 0;
        for (int x = 0; x < 4; x++) if (px(out, 4, x, y) > max) max = px(out, 4, x, y);
        CHECK(max >= 8);
        CHECK(px(out, 4, 0, y) < max && px(out, 4, 3, y) < max);
    }

    /* 4. Same size: the glyph is copied as it is. */
    uint8_t g[2 * 2] = {0x5a, 0x30, 0xc1, 0xe0};
    hd_glyph_scale(g, 3, 2, out, 3, 2);
    CHECK(!memcmp(out, g, sizeof g));

    if (failures) { printf("%d failures\n", failures); return 1; }
    printf("hd_glyph: all checks passed\n");
    return 0;
}
