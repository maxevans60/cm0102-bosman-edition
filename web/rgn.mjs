// A database's title card as a small picture for the page (library.mjs): the Update Team's intro picture of its release
// (logo.rgn, else splash.rgn), shown beside the database in the library's list. Only on the page: the game never reads a
// data set's pictures (Q205, src/recomp_rt/win/files.c): its intro allocates by the colours of each picture, and an
// 800x600 card in place of the 520x520 logo of 46 colours would change the game's later days (Q152,
// tools/release/make_intro_logo.py).
//
// RGN, the game's picture format (src/si/rgn_loader.c, after FUN_005e40d0): a header of 12 little-endian 32-bit
// numbers (width, height, data size = width * height * 2, ..., the green mask at [9]: 0x07e0 RGB565, else RGB555),
// then the pixels, 16 bits each, row by row.
import { deflateSync } from 'node:zlib';
import { zipEntries, zipRead } from './zip.js';

export const CARD_W = 200, CARD_H = 150;          // shown at 100x75: sharp on a screen of two pixels a point
const CARD_FILES = ['logo.rgn', 'splash.rgn'];   // the card is logo.rgn in every release seen (2008 to 2026)

// { w, h, rgb } of an RGN file, or null when it is not one.
export function decodeRgn(bytes) {
  if (!bytes || bytes.length < 48) return null;
  const dv = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const w = dv.getInt32(0, true), h = dv.getInt32(4, true), size = dv.getInt32(8, true), green = dv.getInt32(36, true);
  if (!(w > 0 && h > 0 && w <= 4096 && h <= 4096) || size !== w * h * 2 || bytes.length < 48 + size) return null;
  const rgb = new Uint8Array(w * h * 3), b565 = green === 0x07e0;
  for (let i = 0; i < w * h; i++) {
    const p = dv.getUint16(48 + i * 2, true);
    const r = b565 ? p >> 11 : (p >> 10) & 31, g = b565 ? (p >> 5) & 63 : (p >> 5) & 31, b = p & 31;
    rgb[i * 3] = (r << 3) | (r >> 2);
    rgb[i * 3 + 1] = b565 ? (g << 2) | (g >> 4) : (g << 3) | (g >> 2);
    rgb[i * 3 + 2] = (b << 3) | (b >> 2);
  }
  return { w, h, rgb };
}

// The picture made smaller: each new pixel the mean of the pixels it covers.
export function shrink({ w, h, rgb }, tw, th) {
  const out = new Uint8Array(tw * th * 3);
  for (let y = 0; y < th; y++) {
    const y0 = Math.floor((y * h) / th), y1 = Math.max(y0 + 1, Math.floor(((y + 1) * h) / th));
    for (let x = 0; x < tw; x++) {
      const x0 = Math.floor((x * w) / tw), x1 = Math.max(x0 + 1, Math.floor(((x + 1) * w) / tw));
      let r = 0, g = 0, b = 0;
      for (let yy = y0; yy < y1; yy++) for (let xx = x0; xx < x1; xx++) { const i = (yy * w + xx) * 3; r += rgb[i]; g += rgb[i + 1]; b += rgb[i + 2]; }
      const n = (y1 - y0) * (x1 - x0), o = (y * tw + x) * 3;
      out[o] = Math.round(r / n); out[o + 1] = Math.round(g / n); out[o + 2] = Math.round(b / n);
    }
  }
  return { w: tw, h: th, rgb: out };
}

const CRC = new Int32Array(256).map((_, n) => { let c = n; for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1; return c; });
function crc(bytes) { let c = -1; for (const b of bytes) c = CRC[(c ^ b) & 255] ^ (c >>> 8); return (c ^ -1) >>> 0; }

// A PNG of 8-bit RGB, each row with the filter that leaves the smallest sum (as most encoders do).
export function encodePng({ w, h, rgb }) {
  const stride = w * 3, raw = new Uint8Array((stride + 1) * h), row = new Uint8Array(stride);
  for (let y = 0; y < h; y++) {
    const cur = rgb.subarray(y * stride, (y + 1) * stride), up = y ? rgb.subarray((y - 1) * stride, y * stride) : null;
    let best = null, bestSum = Infinity, bestType = 0;
    for (let t = 0; t < 5; t++) {
      let sum = 0;
      for (let i = 0; i < stride; i++) {
        const a = i >= 3 ? cur[i - 3] : 0, b = up ? up[i] : 0, c = up && i >= 3 ? up[i - 3] : 0;
        let p;
        if (t === 0) p = cur[i];
        else if (t === 1) p = cur[i] - a;
        else if (t === 2) p = cur[i] - b;
        else if (t === 3) p = cur[i] - ((a + b) >> 1);
        else { const q = a + b - c, pa = Math.abs(q - a), pb = Math.abs(q - b), pc = Math.abs(q - c); p = cur[i] - (pa <= pb && pa <= pc ? a : pb <= pc ? b : c); }
        row[i] = p & 255;
        sum += row[i] < 128 ? row[i] : 256 - row[i];
      }
      if (sum < bestSum) { bestSum = sum; bestType = t; best = row.slice(); }
    }
    raw[y * (stride + 1)] = bestType;
    raw.set(best, y * (stride + 1) + 1);
  }
  const chunk = (type, data) => {
    const out = new Uint8Array(12 + data.length), dv = new DataView(out.buffer);
    dv.setUint32(0, data.length);
    out.set(new TextEncoder().encode(type), 4);
    out.set(data, 8);
    dv.setUint32(8 + data.length, crc(out.subarray(4, 8 + data.length)));
    return out;
  };
  const ihdr = new Uint8Array(13), dv = new DataView(ihdr.buffer);
  dv.setUint32(0, w); dv.setUint32(4, h); ihdr[8] = 8; ihdr[9] = 2;
  const parts = [new Uint8Array([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ihdr), chunk('IDAT', deflateSync(raw, { level: 9 })), chunk('IEND', new Uint8Array(0))];
  const png = new Uint8Array(parts.reduce((n, p) => n + p.length, 0));
  let o = 0;
  for (const p of parts) { png.set(p, o); o += p.length; }
  return png;
}

// The card of a data set's zip as a PNG, or null when the zip has no picture that reads.
export async function cardOfZip(zip) {
  let entries;
  try { entries = zipEntries(zip); } catch { return null; }
  for (const want of CARD_FILES) {
    const e = entries.find((x) => x.name.slice(x.name.lastIndexOf('/') + 1).toLowerCase() === want && !/(^|\/)__MACOSX\//.test(x.name));
    if (!e) continue;
    let picture;
    try { picture = decodeRgn(await zipRead(zip, e)); } catch { picture = null; }
    if (picture) return encodePng(shrink(picture, CARD_W, CARD_H));
  }
  return null;
}
