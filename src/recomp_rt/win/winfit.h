/* The size of a new window on a small screen (0.4.0 slice 10).
 *
 * A new window opens at the size of the game's screen, 1280x800 in widescreen mode. On a small screen the
 * menu bar or the task bar and the title bar take part of the height, so the window could go past the
 * edge of the screen. winfit_size makes the window smaller, with the same shape, until it fits in
 * max_w x max_h. The picture is scaled to the window as before (user32.c). */
#ifndef WINFIT_H
#define WINFIT_H

static inline void winfit_size(int w, int h, int max_w, int max_h, int *out_w, int *out_h)
{
    *out_w = w;
    *out_h = h;
    if (max_w <= 0 || max_h <= 0 || (w <= max_w && h <= max_h)) return;
    if ((long long)w * max_h > (long long)h * max_w) {      /* the width limits */
        *out_w = max_w;
        *out_h = (int)((long long)h * max_w / w);
    } else {                                                /* the height limits */
        *out_h = max_h;
        *out_w = (int)((long long)w * max_h / h);
    }
}

/* Windows display scaling (0.6.0): the window size in scaled points that holds at least px real
 * pixels, when the window has pixels x points. 1280 pixels at 125 % (1600 x 1280) give 1024 points;
 * at 150 % 854 points give 1281 pixels, as 853 would give 1279, less than the picture. */
static inline int winfit_points(int px, int pixels, int points)
{
    if (pixels <= 0 || points <= 0) return px;
    return (int)(((long long)px * points + pixels - 1) / pixels);
}

#endif
