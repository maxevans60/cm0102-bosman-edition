# Windows display scaling (0.6.0)

Status: built on `claude/windows-dpi`, 2026-10-20.

## Problem

Two players on the champman0102 forum saw blurred text on Windows laptops. One of them set Windows display scaling from 125 % to 100 % and the text became sharp (2026-10-20). The game exe declared no DPI awareness, so Windows drew the game at 100 % and stretched the whole window to 125 %.

## Decisions (Max, 2026-10-20)

1. The game sets `SDL_WINDOWS_DPI_SCALING=1` before `SDL_Init` (`src/recomp_rt/win/main.c`). This turns on per-monitor awareness. Window sizes and places stay in scaled points, as on a Mac. `window.txt` from older versions, the first window size, the window fit (`winfit.h`), full screen and the mouse keep their units. Only the renderer output is in real pixels.
2. A new window opens at the same logical size as before (1280x800 points in widescreen, 1600x1000 pixels at 125 %).
3. With `sharp_picture` on, on Windows only, a window that holds the picture more than 1.2 and less than 2 times fills with smooth (linear) scaling (`winscale.h`). Without this, whole pixels give scale 1 with wide bars on a 1080p laptop at 125 % or 150 %. Mac and Linux keep the old rule.
4. A window smaller than the picture stays out of this change (nearest scaling, as before).
5. No separate test build. The 0.6.0 public beta is the first Windows test. The display report shows "Game handles the Windows scale: per screen" and the output size in pixels.

## Tests

- `winscale` (ctest): the scale rule with and without the smooth band.
- Cross-build of `cm0102_recomp.exe` with no warnings. Nobody can run it on Windows before the beta.
