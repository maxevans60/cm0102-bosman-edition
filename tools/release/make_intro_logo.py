#!/usr/bin/env python3
"""Makes the intro logo (Q205 to Q208): a PNG picture to Data/bosman_logo.rgn of the game folder.

Usage:
    python3 tools/release/make_intro_logo.py <logo.png> <out.rgn>

The game shows Data/logo.rgn (the Sports Interactive logo, 520x520) first in its intro. When the game
folder has Data/bosman_logo.rgn, the runtime reads that file in its place (src/recomp_rt/win/files.c).
The picture has the same size, 520x520, so the original allocates the same memory and later game days
do not change. The logo is scaled to LOGO_W pixels wide, put on black and centred.

The intro fades each picture with 0x5e30e0, which allocates one block for each colour of the picture
(0x5e31e3, a table of 65536 entries). So the logo also has the colour count of the SI logo, COLOURS:
black and COLOURS - 1 colours of a median cut. With another count the game's later days differ (Q152).

The logo itself is not in git: make_release.sh makes the file from the site pictures (--site-assets).
Only the standard library: an 8-bit RGB or RGBA PNG without interlace.
"""
import struct
import sys
import zlib

SIZE = 520
LOGO_W = 500
COLOURS = 46                    # the colours of Data/logo.rgn of the clean files


def read_png(path):
    """Pixels of an 8-bit RGB or RGBA PNG as (width, height, rows of (r, g, b, a) tuples)."""
    data = open(path, "rb").read()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise SystemExit(f"{path}: not a PNG")
    pos, idat, head = 8, b"", None
    while pos < len(data):
        n, kind = struct.unpack_from(">I4s", data, pos)
        body = data[pos + 8:pos + 8 + n]
        if kind == b"IHDR":
            head = struct.unpack(">IIBBBBB", body)
        elif kind == b"IDAT":
            idat += body
        pos += 12 + n
    w, h, depth, colour, _, _, interlace = head
    if depth != 8 or colour not in (2, 6) or interlace:
        raise SystemExit(f"{path}: use an 8-bit RGB or RGBA PNG without interlace")
    bpp = 4 if colour == 6 else 3
    raw = zlib.decompress(idat)
    stride = w * bpp
    rows, prev = [], bytearray(stride)
    for y in range(h):
        f = raw[y * (stride + 1)]
        line = bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for i in range(stride):
            a = line[i - bpp] if i >= bpp else 0
            b = prev[i]
            c = prev[i - bpp] if i >= bpp else 0
            if f == 1:
                line[i] = (line[i] + a) & 255
            elif f == 2:
                line[i] = (line[i] + b) & 255
            elif f == 3:
                line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                line[i] = (line[i] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 255
        rows.append([tuple(line[x * bpp:x * bpp + 3]) + ((line[x * bpp + 3],) if bpp == 4 else (255,))
                     for x in range(w)])
        prev = line
    return w, h, rows


def rgb565(c):
    return (c[0] >> 3) << 11 | (c[1] >> 2) << 5 | c[2] >> 3


def reduce_colours(pixels):
    """RGB pixels to RGB565 values with exactly COLOURS different values: black, and a median cut of the
    other pixels to COLOURS - 1 boxes, each shown as the mean of its pixels."""
    counts = {}
    for c in pixels:
        if c != (0, 0, 0):
            counts[c] = counts.get(c, 0) + 1
    boxes = [list(counts)]
    while len(boxes) < COLOURS - 1:
        # Split the box with the most pixels times its widest range, at its median pixel.
        def weight(box):
            if len(box) < 2:
                return -1
            span = max(max(c[i] for c in box) - min(c[i] for c in box) for i in range(3))
            return span * sum(counts[c] for c in box)
        box = max(boxes, key=weight)
        if weight(box) <= 0:
            raise SystemExit(f"the logo has fewer than {COLOURS} colours")
        axis = max(range(3), key=lambda i: max(c[i] for c in box) - min(c[i] for c in box))
        box.sort(key=lambda c: c[axis])
        half, run, cut = sum(counts[c] for c in box) / 2, 0, 1
        for i, c in enumerate(box[:-1]):
            run += counts[c]
            cut = i + 1
            if run >= half:
                break
        boxes.remove(box)
        boxes += [box[:cut], box[cut:]]
    value = {(0, 0, 0): 0}
    used = {0}
    for box in boxes:
        n = sum(counts[c] for c in box)
        v = rgb565(tuple(round(sum(c[i] * counts[c] for c in box) / n) for i in range(3)))
        while v in used:            # two boxes with the same RGB565 value: take the next free value
            v = (v + 1) & 0xffff
        used.add(v)
        for c in box:
            value[c] = v
    return [value[c] for c in pixels]


def make_rgn(png_path):
    """The bytes of the RGN file: a 48-byte header and SIZE x SIZE RGB565 pixels."""
    w, h, rows = read_png(png_path)
    lw = LOGO_W
    lh = max(1, round(h * lw / w))
    x0, y0 = (SIZE - lw) // 2, (SIZE - lh) // 2
    pixels = [(0, 0, 0)] * (SIZE * SIZE)
    for ty in range(lh):
        sy0, sy1 = ty * h / lh, (ty + 1) * h / lh
        for tx in range(lw):
            sx0, sx1 = tx * w / lw, (tx + 1) * w / lw
            # The average of the source area, each pixel by its covered share and its alpha, on black.
            acc = [0.0, 0.0, 0.0]
            area = 0.0
            for sy in range(int(sy0), min(h, int(sy1) + 1)):
                fy = min(sy + 1, sy1) - max(sy, sy0)
                if fy <= 0:
                    continue
                row = rows[sy]
                for sx in range(int(sx0), min(w, int(sx1) + 1)):
                    fx = min(sx + 1, sx1) - max(sx, sx0)
                    if fx <= 0:
                        continue
                    r, g, b, a = row[sx]
                    k = fx * fy * a / 255
                    acc[0] += r * k
                    acc[1] += g * k
                    acc[2] += b * k
                    area += fx * fy
            r, g, b = (min(255, round(v / area)) for v in acc)
            pixels[(y0 + ty) * SIZE + x0 + tx] = (r, g, b)
    pixels = reduce_colours(pixels)
    header = struct.pack("<12I", SIZE, SIZE, SIZE * SIZE * 2, 0, 0x20, 0x40, 0, 0x10, 0xf800, 0x07e0, 0x001f, 0)
    return header + struct.pack(f"<{SIZE * SIZE}H", *pixels)


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    data = make_rgn(argv[1])
    with open(argv[2], "wb") as f:
        f.write(data)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
