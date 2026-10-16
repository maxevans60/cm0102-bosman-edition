/* HD Large text (spec 2026-10-16-hd-text-0.5.1, Q223 to Q230).
 *
 * In HD the screens are 1.5 times wider and 1.35 times taller than in widescreen, but the text keeps its
 * pixel size. With hd_text = large (the default) HD draws its text 1.35 times bigger. The option is read
 * once at start (hd_text_init from win_read_settings), as the display mode, because bigger text changes
 * later days like another display mode does (Q152). Outside HD and with hd_text = small nothing changes.
 *
 * - The font engine fonts ("Futuristic", font mode -2, the default): tools/recomp/hd_text.py turns the 59
 *   size, baseline, cursor and line height constants of the line draw 0x5e5160, the text width 0x5e5ad0,
 *   the text height 0x5e5cd0 and the text box 0x5e6d80 into rt_hd_text(v) (docs/research/2026-10-16-hd-text.md).
 * - The bitmap fonts ("Traditional", font mode -1): the font set load 0x5e4ba0 (font, path) clears the table of
 *   the font at 0xae1dac + font * 0x1404 (0x501 dwords, rep stosd at 0x5e4c03..0x5e4c22) and reads the file
 *   into it: the height +0 (0x5e4c24), then for each character c of 0x20 to 0xff at +4 + c * 20 the width,
 *   the bytes of a glyph row, the space before and after it and the glyph, which it allocates (0x5e4c47 to
 *   0x5e4d15). For fonts 6 and 7 it copies some characters into new glyphs (0x5e4e13 to 0x5e4ead). The line
 *   draw reads a glyph as one value of 0 to 15 per pixel, two to a byte, (width + 1) / 2 bytes a row
 *   (0x5e56d7 to 0x5e58d8). After each load the wrap scales every glyph by 1.35 with linear mixing
 *   (hd_glyph.c), and the height, widths and spaces with it. The draw, the width 0x5e5ad0, the spacing
 *   0x5e5d60, the height 0x5e5cd0 and the line height of the text box read these values, so the text and its lines grow together.
 *   The display init 0x5e2620 loads the fonts at start in every font mode, and again when the player changes
 *   the font in Game Settings (it clears the flag 0xaebe0c at 0x5e2c3f). Each load reads the file again, so
 *   a font is never scaled twice. */
#include "win.h"
#include "options.h"
#include "hd_glyph.h"

static int large;       /* HD with hd_text = large */

void hd_text_init(void)
{
    large = rt_screen_w == 1920u && options_enum(OPT_HD_TEXT) == HD_TEXT_LARGE;
    rt_hd_text_percent = large ? HD_TEXT_PERCENT : 100u;
    if (large) fprintf(stderr, "recomp: HD text large\n");
}

#define FONTS 0xae1dacu
#define FONT_SIZE 0x1404u      /* 0x501 dwords: the height and 256 characters of 20 bytes (0x5e4c03) */
#define FIRST_CHAR 0x20u
#define LAST_CHAR 0xffu
#define CALLOC 0x945501u        /* the original's calloc (count, size), as display.c resize_picture */
#define FREE 0x9452cau          /* the original's free, which the loader also uses for glyphs (0x5e4e2a) */

/* A space before or after a glyph, which may be below 0, at the HD Large size. */
static uint32_t scale_space(uint32_t v)
{
    int32_t s = (int32_t)v;
    return s < 0 ? (uint32_t)-(int32_t)hd_glyph_size((uint32_t)-s) : hd_glyph_size(v);
}

/* Scales the bitmap font in slot to the HD Large size. */
static void scale_font(uint32_t slot)
{
    uint32_t font = FONTS + slot * FONT_SIZE;
    uint32_t h = rd32(g_mem, font), h2 = hd_glyph_size(h);
    for (uint32_t c = FIRST_CHAR; c <= LAST_CHAR; c++) {
        uint32_t e = font + 4 + c * 20, w = rd32(g_mem, e), glyph = rd32(g_mem, e + 16);
        uint32_t w2 = hd_glyph_size(w);
        if (glyph && w && h && rd32(g_mem, e + 4)) {
            uint32_t rb2 = (w2 + 1) / 2;
            uint32_t args[2] = {1, rb2 * h2};
            uint32_t pixels = rt_callback(CALLOC, 2, args);
            if (!pixels) rt_fatal("HD text: no memory for a glyph of font %u", slot);   /* every glyph must have the new height */
            hd_glyph_scale(g_mem + glyph, w, h, g_mem + pixels, w2, h2);
            uint32_t free_args[1] = {glyph};
            rt_callback(FREE, 1, free_args);
            wr32(g_mem, e + 4, rb2);
            wr32(g_mem, e + 16, pixels);
        }
        wr32(g_mem, e, w2);
        wr32(g_mem, e + 8, scale_space(rd32(g_mem, e + 8)));
        wr32(g_mem, e + 12, scale_space(rd32(g_mem, e + 12)));
    }
    wr32(g_mem, font, h2);
}

void o_005e4ba0(void);

/* The font set load 0x5e4ba0, cdecl: (font, path); returns 1 when the font is loaded. */
RT_WRAP(005e4ba0, font_load)
{
    uint32_t slot = (uint32_t)(int32_t)(int16_t)rt_arg(0);
    o_005e4ba0();
    if (rt_unwinding || !large || cpu.eax != 1 || slot > 7) return;
    Cpu saved = cpu;
    scale_font(slot);
    cpu = saved;
}
