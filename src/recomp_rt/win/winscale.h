/* How the picture is scaled to the window with the option sharp_picture (user32.c update_scaling).
 *
 * Each pixel of the game is drawn as a block of whole pixels when that loses at most a fifth of the
 * size that fits the window, for example 2 times in a window that allows 2.4 times. Otherwise the
 * picture fills the window and each pixel takes the nearest whole pixels.
 *
 * The smooth band (Windows, from 0.6.0): with Windows display scaling at 125 % or 150 %, a window
 * often holds the picture between 1.2 and 2 times. Whole pixels would then give scale 1, a small
 * picture with wide bars, and the nearest pixels would give uneven letters. In that band the picture
 * fills the window with smooth (linear) scaling instead. */
#ifndef WINSCALE_H
#define WINSCALE_H

#define WINSCALE_WHOLE_SHARE 0.8
#define WINSCALE_SMOOTH_FROM 1.2
#define WINSCALE_SMOOTH_TO 2.0

typedef struct {
    int whole;          /* 1: whole pixels (SDL_RenderSetIntegerScale) */
    int smooth;         /* 1: linear filter, 0: nearest pixels */
} WinScale;

/* out_w x out_h: the window in real pixels. pic_w x pic_h: the game's picture. */
static inline WinScale winscale_pick(int out_w, int out_h, int pic_w, int pic_h, int smooth_band)
{
    WinScale s = {0, 0};
    if (out_w <= 0 || out_h <= 0 || pic_w <= 0 || pic_h <= 0) return s;
    double fit = (double)out_w / pic_w;
    if ((double)out_h / pic_h < fit) fit = (double)out_h / pic_h;
    if (smooth_band && fit > WINSCALE_SMOOTH_FROM && fit < WINSCALE_SMOOTH_TO) {
        s.smooth = 1;
        return s;
    }
    int whole = (int)fit;
    s.whole = whole >= 1 && whole >= fit * WINSCALE_WHOLE_SHARE;
    return s;
}

#endif
