/* HD Large text (0.5.1): the size rule and the glyph scale of the Traditional fonts, without game memory, so
 * that tests/hdtext/test_hd_glyph.c can test them. hd_text.c applies them. */
#ifndef RECOMP_HD_GLYPH_H
#define RECOMP_HD_GLYPH_H

#include <stdint.h>

/* v times 1.35, rounded half up: 9, 10, 14, 16, 18, 20, 22 give 12, 14, 19, 22, 24, 27, 30 (Q224). */
uint32_t hd_glyph_size(uint32_t v);

/* Scales a glyph of the bitmap fonts from w x h to w2 x h2 pixels, by linear mixing of the four nearest
 * pixels. A glyph holds one value of 0 (clear) to 15 (full colour) per pixel, two pixels to a byte, the
 * first in the high nibble, and each row starts at a new byte: (w + 1) / 2 bytes a row, as the line
 * draw 0x5e5160 reads it. dst gets h2 rows of (w2 + 1) / 2 bytes. */
void hd_glyph_scale(const uint8_t *src, uint32_t w, uint32_t h, uint8_t *dst, uint32_t w2, uint32_t h2);

#endif
