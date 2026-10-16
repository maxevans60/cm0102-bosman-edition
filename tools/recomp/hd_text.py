"""Text size constants of the original that become run-time values, for HD Large text (0.5.1).

The game draws its text with its own scalable font engine (font mode -2, the "Futuristic" fonts). The size
of each of the eight font numbers, its baseline, its cursor height and its line height are constants in
the code. In HD with hd_text = large, rt_hd_text(v) gives v times 1.35, rounded. Outside HD and with
hd_text = small it gives v (src/recomp_rt/win/hd_text.c). widescreen.py adds these entries to its PATCHES,
so the translation checks each constant in the same way.

Font number: size, baseline, cursor and line height. 0: 9, +3, 15 (symbol.ttf). 1: 9, +3, 15 (small.t2k).
2: 10, +4, 18. 3: 14, +4, 21 (medium.t2k). 4: 16, +4, 24. 5: 18, +4, 27. 6: 20, +9, 39 (large.t2k).
7: 22, +11, 45. Evidence: docs/research/2026-10-16-hd-text.md and the list of the 59 instructions in
docs/research/2026-10-16-hd-text-patches.txt.
"""

T = "rt_hd_text({v:#x}u)"

# The line draw 0x5e5160: the size (push) and the baseline (lea r, [esi + n]) given to the T2K draw 0x5b0d80,
# and the cursor height (mov eax, n) of the two cursor draws (0x5e5454.., 0x5e59cb..).
LINE_SIZES = {0x5E52CE: 9, 0x5E52FA: 9, 0x5E5329: 10, 0x5E5352: 14, 0x5E537E: 16, 0x5E53AA: 18,
              0x5E53D0: 20, 0x5E53F4: 22}
LINE_BASELINES = {0x5E52D0: 3, 0x5E52FC: 3, 0x5E5324: 4, 0x5E5354: 4, 0x5E5380: 4, 0x5E53A5: 4,
                  0x5E53D2: 9, 0x5E53F6: 11}
HEIGHTS = (15, 18, 21, 24, 27, 39, 45)
CURSORS = dict(zip((0x5E5454, 0x5E5469, 0x5E547E, 0x5E5493, 0x5E54A8, 0x5E54BD, 0x5E54D2), HEIGHTS))
CURSORS_END = dict(zip((0x5E59CB, 0x5E59D2, 0x5E59D9, 0x5E59E0, 0x5E59E7, 0x5E59EE, 0x5E59F5), HEIGHTS))
# The text width 0x5e5ad0: the size given to the T2K width 0x5b18a0.
WIDTH_SIZES = {0x5E5B1F: 9, 0x5E5B3C: 9, 0x5E5B54: 10, 0x5E5B76: 14, 0x5E5B93: 16, 0x5E5BAB: 18,
               0x5E5BCD: 20, 0x5E5BEA: 22}
# The text height 0x5e5cd0, and the line heights of the text box 0x5e6d80 (two switches).
TEXT_HEIGHTS = dict(zip((0x5E5CEA, 0x5E5CF0, 0x5E5CF6, 0x5E5CFC, 0x5E5D02, 0x5E5D08, 0x5E5D0E), HEIGHTS))
BOX_HEIGHTS = dict(zip((0x5E711F, 0x5E7126, 0x5E712D, 0x5E7134, 0x5E713B, 0x5E7142, 0x5E7149), HEIGHTS))
BOX_HEIGHTS_2 = dict(zip((0x5E7618, 0x5E761F, 0x5E7626, 0x5E762D, 0x5E7634, 0x5E763B, 0x5E7642), HEIGHTS))

# address: (constant in the original, C expression)
PATCHES = {}
for table in (LINE_SIZES, LINE_BASELINES, CURSORS, CURSORS_END, WIDTH_SIZES, TEXT_HEIGHTS, BOX_HEIGHTS,
              BOX_HEIGHTS_2):
    for addr, v in table.items():
        if addr in PATCHES:
            raise SystemExit(f"hd_text.py: {addr:#x} is in the list twice")
        PATCHES[addr] = (v, T.format(v=v))
if len(PATCHES) != 59:
    raise SystemExit(f"hd_text.py: {len(PATCHES)} constants, not 59")
