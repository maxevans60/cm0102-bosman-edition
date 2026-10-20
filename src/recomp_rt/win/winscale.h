/* Whole pixels or not, with the option sharp_picture (user32.c update_scaling).
 *
 * Each pixel of the game is drawn as a block of whole pixels when that loses at most a fifth of the
 * size that fits the window, for example 2 times in a window that allows 2.4 times. Otherwise the
 * picture fills the window and each pixel takes the nearest whole pixels.
 *
 * Windows (from 0.6.0): with display scaling at 125 % or 150 %, a window often holds the picture
 * between 1 and 2 times. The nearest pixels would then give uneven letters, so the picture stays at
 * scale 1, as at 100 %, with bars round it. */
#ifndef WINSCALE_H
#define WINSCALE_H

#define WINSCALE_WHOLE_SHARE 0.8

/* out_w x out_h: the window in real pixels. pic_w x pic_h: the game's picture. 1: whole pixels. */
static inline int winscale_whole(int out_w, int out_h, int pic_w, int pic_h, int windows)
{
    if (out_w <= 0 || out_h <= 0 || pic_w <= 0 || pic_h <= 0) return 0;
    double fit = (double)out_w / pic_w;
    if ((double)out_h / pic_h < fit) fit = (double)out_h / pic_h;
    int whole = (int)fit;
    if (windows && whole == 1) return 1;
    return whole >= 1 && whole >= fit * WINSCALE_WHOLE_SHARE;
}

#endif
