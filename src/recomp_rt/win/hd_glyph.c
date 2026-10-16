/* HD Large text: the size rule and the glyph scale (hd_glyph.h). */
#include "hd_glyph.h"

#include <string.h>

uint32_t hd_glyph_size(uint32_t v)
{
    return (v * 135u + 50u) / 100u;
}

static uint32_t nibble(const uint8_t *g, uint32_t row_bytes, uint32_t x, uint32_t y)
{
    uint8_t b = g[y * row_bytes + x / 2];
    return x & 1 ? b & 15u : b >> 4;
}

/* The source place of pixel i of n, for a source of m pixels, in 256ths: the centre of the new pixel on the
 * old pixels, (i + 0.5) * m / n - 0.5, kept inside 0 .. m - 1. */
static uint32_t place(uint32_t i, uint32_t n, uint32_t m)
{
    int64_t p = ((int64_t)(2 * i + 1) * m * 256) / (2 * (int64_t)n) - 128;
    if (p < 0) p = 0;
    if (p > (int64_t)(m - 1) * 256) p = (int64_t)(m - 1) * 256;
    return (uint32_t)p;
}

void hd_glyph_scale(const uint8_t *src, uint32_t w, uint32_t h, uint8_t *dst, uint32_t w2, uint32_t h2)
{
    uint32_t rb = (w + 1) / 2, rb2 = (w2 + 1) / 2;
    memset(dst, 0, (size_t)rb2 * h2);
    if (!w || !h) return;
    for (uint32_t y = 0; y < h2; y++) {
        uint32_t sy = place(y, h2, h), y0 = sy >> 8, y1 = y0 + 1 < h ? y0 + 1 : y0, fy = sy & 255;
        for (uint32_t x = 0; x < w2; x++) {
            uint32_t sx = place(x, w2, w), x0 = sx >> 8, x1 = x0 + 1 < w ? x0 + 1 : x0, fx = sx & 255;
            uint32_t v = nibble(src, rb, x0, y0) * (256 - fx) * (256 - fy) + nibble(src, rb, x1, y0) * fx * (256 - fy) +
                         nibble(src, rb, x0, y1) * (256 - fx) * fy + nibble(src, rb, x1, y1) * fx * fy;
            v = (v + 32768) >> 16;
            dst[y * rb2 + x / 2] |= (uint8_t)(x & 1 ? v : v << 4);
        }
    }
}
