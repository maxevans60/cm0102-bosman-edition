"""Letters for BOSMAN EDITION, built from the logo's own SEASON 01/02 lettering (alpha masks, 'L' mode).

S, E, A, O and N are cut from the original row. I, T, D and B are made from the E's and O's strokes. M is
drawn with straight lines on the N's measured geometry. The source logo is set by make_bosman_logo.py
through the environment variable BOSMAN_LOGO_SRC.
"""
import os
from PIL import Image, ImageDraw, ImageChops
O = Image.open(os.environ['BOSMAN_LOGO_SRC']).convert('RGBA')
ROW = O.crop((150, 475, 1070, 547)).split()[3]          # alpha of the SEASON 01/02 row, 72 px high
H = ROW.height
CAP = 66                                                   # ink rows 0..65; below are only specks
ROW.paste(0, (0, CAP, ROW.width, H))
SEG = {'S': (15, 85), 'E': (91, 155), 'A': (157, 246), 'O': (326, 404), 'N': (412, 489)}
def cut(c): x0, x1 = SEG[c]; return ROW.crop((x0, 0, x1, H))
def blank(w): return Image.new('L', (w, H), 0)
def paste(dst, src, x): dst.paste(ImageChops.lighter(dst.crop((x, 0, x + src.width, H)), src), (x, 0)); return dst
E = cut('E'); Oo = cut('O'); N = cut('N')
STEM = E.crop((0, 0, 25, H))                              # the E's straight left stem, square corners
def glyph_I(): return STEM.copy()
def glyph_T():
    w = E.width; g = blank(w)
    g.paste(E.crop((0, 0, w, 11)), (0, 0))                # the E's top bar
    paste(g, STEM.crop((0, 0, 25, H)), (w - 25) // 2)
    g.paste(E.crop((0, 0, w, 11)), (0, 0))
    return g
def glyph_D():
    g = Oo.copy(); d = ImageDraw.Draw(g); d.rectangle((0, 0, 24, H), fill=0)
    return paste(g, STEM, 0)
def glyph_B():
    g = glyph_D(); w = g.width
    d = ImageDraw.Draw(g); d.rectangle((20, 29, w - 20, 37), fill=255)   # middle bar, as thick as the E's
    d.polygon([(w + 1, 27), (w - 9, 33), (w + 1, 39)], fill=0)            # notch between the two bowls
    return g
def glyph_M():
    # Straight-line M from the N's measured geometry (rows 0..65): stems x 1..13 and 64..76, a diagonal
    # 26 px wide with slope 0.733 px per row. Its left half (stem and diagonal) joins a mirror copy 34 px
    # to the right, so both diagonals meet at the baseline; the result is squeezed to 84 px, about the
    # width of the B and the O (option 1).
    s4 = 4; W4, H4 = N.width * s4, H * s4
    half = Image.new('L', (W4, H4), 0); d = ImageDraw.Draw(half)
    k, top0, top1, base = 0.733, 1.3, 27.3, CAP - 1
    d.rectangle((1 * s4, 0, 13 * s4, CAP * s4), fill=255)
    d.polygon([(top0 * s4, 0), (top1 * s4, 0), ((top1 + k * base) * s4, CAP * s4), ((top0 + k * base) * s4, CAP * s4)], fill=255)
    d.rectangle((64 * s4, 0, W4, H4), fill=0)
    half = half.resize((N.width, H), Image.LANCZOS)
    dx = 34; m = blank(N.width + dx)
    paste(m, half, 0); paste(m, half.transpose(Image.FLIP_LEFT_RIGHT), dx)
    return m.resize((84, H), Image.LANCZOS)
GLYPH = {'S': lambda: cut('S'), 'E': lambda: E, 'A': lambda: cut('A'), 'O': lambda: Oo, 'N': lambda: N,
         'I': glyph_I, 'T': glyph_T, 'D': glyph_D, 'B': glyph_B, 'M': glyph_M}
GAP = 7
def word(text):
    gs = [GLYPH[c]() for c in text]; w = sum(g.width for g in gs) + GAP * (len(gs) - 1)
    out = blank(w); x = 0
    for g in gs: paste(out, g, x); x += g.width + GAP
    return out
