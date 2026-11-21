// Tests of the title cards (rgn.mjs): the game's RGN pictures read, made small and written as PNG. Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { inflateSync } from 'node:zlib';
import { decodeRgn, shrink, encodePng, cardOfZip, CARD_W, CARD_H } from '../rgn.mjs';
import { writeZip } from '../zip.js';
import { fakeRgn as rgn } from './fakes.mjs';

test('an RGN reads as RGB, in both of the game\'s pixel formats; anything else is not one', () => {
  const p = decodeRgn(rgn(2, 1, (x) => (x ? 0x001f : 0xf800)));                         // red, blue in RGB565
  assert.deepEqual([p.w, p.h, ...p.rgb], [2, 1, 255, 0, 0, 0, 0, 255]);
  const q = decodeRgn(rgn(1, 1, () => 0x03e0, { green: 0x03e0 }));                        // green in RGB555
  assert.deepEqual([...q.rgb], [0, 255, 0]);
  assert.equal(decodeRgn(new Uint8Array(20)), null);
  const bad = rgn(2, 2, () => 0);
  new DataView(bad.buffer).setInt32(8, 99, true);                                         // a size that does not match
  assert.equal(decodeRgn(bad), null);
});

test('made small, each pixel is the mean of those it covers; the PNG holds every row', () => {
  const big = { w: 4, h: 2, rgb: new Uint8Array([0, 0, 0, 200, 200, 200, 10, 10, 10, 10, 10, 10, 0, 0, 0, 200, 200, 200, 30, 30, 30, 30, 30, 30]) };
  const s = shrink(big, 2, 1);
  assert.deepEqual([...s.rgb], [100, 100, 100, 20, 20, 20]);
  const png = encodePng(s);
  assert.deepEqual([...png.subarray(0, 8)], [137, 80, 78, 71, 13, 10, 26, 10]);
  const dv = new DataView(png.buffer, png.byteOffset);
  assert.deepEqual([dv.getUint32(16), dv.getUint32(20)], [2, 1], 'IHDR: 2 x 1');
  const idat = png.indexOf(0x49, 33);                                                     // "IDAT" after IHDR
  const len = dv.getUint32(idat - 4);
  assert.equal(inflateSync(png.subarray(idat + 4, idat + 4 + len)).length, (2 * 3 + 1) * 1);
});

test('a zip\'s card: logo.rgn, else splash.rgn, at the card\'s size; none without a picture', async () => {
  const pic = rgn(80, 60, (x, y) => ((x + y) % 2 ? 0xffff : 0));
  const withLogo = await writeZip([{ name: 'Update/logo.RGN', data: pic }, { name: 'Update/club.dat', data: new Uint8Array(4) }]);
  const card = await cardOfZip(withLogo);
  const dv = new DataView(card.buffer, card.byteOffset);
  assert.deepEqual([dv.getUint32(16), dv.getUint32(20)], [CARD_W, CARD_H]);
  assert.ok(await cardOfZip(await writeZip([{ name: 'splash.rgn', data: pic }])), 'splash.rgn when there is no logo');
  assert.equal(await cardOfZip(await writeZip([{ name: 'Update/club.dat', data: new Uint8Array(4) }])), null);
  assert.equal(await cardOfZip(await writeZip([{ name: 'logo.rgn', data: new Uint8Array(30) }])), null, 'a broken picture: none');
  assert.equal(await cardOfZip(new Uint8Array(10)), null, 'not a zip: none');
});
