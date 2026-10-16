import os, sys; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from glyphs import *
from PIL import ImageFilter, ImageChops
S = 4  # work at 4x for smooth edges
def up(m):
    # 4x, then smooth the jagged edges of the scanned artwork and drop specks
    m = m.resize((m.width * S, m.height * S), Image.LANCZOS).filter(ImageFilter.GaussianBlur(S * 0.9))
    m = m.point(lambda v: 255 if v > 120 else 0)
    m = m.filter(ImageFilter.MinFilter(5)).filter(ImageFilter.MaxFilter(5))   # removes isolated specks
    return m.filter(ImageFilter.GaussianBlur(1))
def grow(m, px): return m.filter(ImageFilter.MaxFilter(2 * px * S + 1)) if px else m
def shift(m, dx, dy):
    o = Image.new('L', m.size, 0); o.paste(m, (dx * S, dy * S)); return o
def vgrad(size, stops, y0=0, y1=None):
    # the gradient runs from row y0 to row y1 (the letters' top and baseline), flat outside
    w, h = size; g = Image.new('RGBA', size); y1 = h - 1 if y1 is None else y1
    px = []
    for y in range(h):
        t = min(1, max(0, (y - y0) / max(1, y1 - y0)))
        for i in range(len(stops) - 1):
            (t0, c0), (t1, c1) = stops[i], stops[i + 1]
            if t0 <= t <= t1:
                f = (t - t0) / (t1 - t0) if t1 > t0 else 0
                px.append(tuple(int(c0[k] + (c1[k] - c0[k]) * f) for k in range(3)) + (255,)); break
    for y, c in enumerate(px): g.paste(c, (0, y, w, y + 1))
    return g
def layer(canvas, mask, colour_or_img):
    src = colour_or_img if isinstance(colour_or_img, Image.Image) else Image.new('RGBA', mask.size, colour_or_img)
    canvas.paste(src, (0, 0), mask)
PAD = 8
def padded(m):
    o = Image.new('L', (m.width + 2 * PAD, m.height + 2 * PAD), 0); o.paste(m, (PAD, PAD)); return o
def italic(m, k=0.2):
    w, h = m.size; extra = int(k * h)
    o = Image.new('L', (w + extra, h), 0); o.paste(m, (extra, 0))
    return o.transform(o.size, Image.AFFINE, (1, k, 0, 0, 1, 0), Image.BICUBIC)
# Championship: white top, silver, then blue below the middle, a dark blue outline and a dark shadow
CHROME = [(0, (237, 239, 241)), (0.2, (231, 236, 239)), (0.38, (233, 238, 240)), (0.5, (221, 231, 236)),
          (0.53, (214, 228, 235)), (0.57, (183, 219, 234)), (0.61, (145, 202, 227)), (0.67, (101, 185, 218)),
          (0.74, (79, 156, 194)), (0.82, (70, 125, 166)), (0.9, (56, 90, 132)), (1, (59, 99, 138))]
# MANAGER: yellow, a little lighter at the top
YELLOW = [(0, (250, 214, 52)), (0.5, (238, 192, 30)), (1, (226, 178, 24))]
def chrome(m):
    m = up(padded(m)); c = Image.new('RGBA', m.size, (0, 0, 0, 0))
    layer(c, shift(grow(m, 2), 2, 3), (15, 13, 11, 255))      # shadow, as dark as Championship's
    layer(c, grow(m, 2), (15, 13, 11, 255))                    # near-black outline sampled from Championship
    fill = vgrad(m.size, CHROME, PAD * S, (PAD + CAP) * S)
    layer(c, m, fill)
    # the rounded look of Championship's strokes: the edge of each stroke is darker than its middle
    inner = m.filter(ImageFilter.MinFilter(2 * 2 * S + 1)).filter(ImageFilter.GaussianBlur(S * 1.5))
    edge = ImageChops.subtract(m, inner)
    r, g, b, _ = fill.split()
    dark = Image.merge('RGBA', (r.point(lambda v: int(v * 0.62)), g.point(lambda v: int(v * 0.72)), b.point(lambda v: int(v * 0.82)), Image.new('L', fill.size, 255)))
    layer(c, edge, dark)
    return c.resize((c.width // S, c.height // S), Image.LANCZOS)
def yellow3d(m):
    m = up(padded(m)); c = Image.new('RGBA', m.size, (0, 0, 0, 0))
    layer(c, shift(grow(m, 3), 2, 3), (25, 32, 55, 255))       # shadow, bottom right
    layer(c, grow(m, 3), (70, 92, 128, 255))                   # slate blue bevel
    layer(c, grow(m, 1), (232, 236, 240, 255))                 # white rim
    hi = shift(m, -1, -1); layer(c, grow(m, 1), (232, 236, 240, 255))
    layer(c, m, vgrad(m.size, YELLOW, PAD * S, (PAD + CAP) * S))
    return c.resize((c.width // S, c.height // S), Image.LANCZOS)
def compose(out_path):
    """The original logo with a line BOSMAN EDITION under SEASON 01/02 (Q211 to Q218)."""
    b = chrome(word('BOSMAN')); e = yellow3d(italic(word('EDITION')))
    gap = 40
    line = Image.new('RGBA', (b.width + gap + e.width, max(b.height, e.height)), (0, 0, 0, 0))
    line.alpha_composite(b, (0, 0)); line.alpha_composite(e, (b.width + gap, 0))
    top = O.getbbox()[3]
    out = Image.new('RGBA', (O.width, top + 22 + line.height), (0, 0, 0, 0))
    out.alpha_composite(O.crop((0, 0, O.width, top)), (0, 0))
    out.alpha_composite(line, ((150 + 15 + 150 + 900) // 2 - line.width // 2, top + 22 - PAD))
    out.save(out_path)
    return out.size
