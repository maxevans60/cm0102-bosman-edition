// Tests of the zip made of Blobs (web/zip.js, zipOfBlobs: Saved games, Download all). Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { crc32, crc32Blob, zipEntries, zipOfBlobs, zipRead } from '../zip.js';

test('zipOfBlobs: the files stored as they are, read back by zipRead, with their CRC', async () => {
  const big = new Uint8Array(200000).map((_, i) => (i * 7) & 255);
  const files = [
    { name: 'Juventus 2001-07-29.sav', blob: new Blob([big]) },
    { name: 'backups/Juventus 2001-07-29.1.sav', blob: new Blob([new Uint8Array([1, 2, 3])]) },
    { name: 'Databases/My edit/index.dat', blob: new Blob([]) },
  ];
  let seen = 0;
  const zip = await zipOfBlobs(files, { onBytes: (n) => { seen += n; } });
  assert.equal(seen, big.length + 3, 'every byte read once for the CRC');
  const bytes = new Uint8Array(await zip.arrayBuffer());
  const entries = zipEntries(bytes);
  assert.deepEqual(entries.map((e) => e.name), files.map((f) => f.name));
  assert.deepEqual([...await zipRead(bytes, entries[0])].slice(0, 10), [...big].slice(0, 10));
  assert.equal((await zipRead(bytes, entries[0])).length, big.length);
  assert.deepEqual([...await zipRead(bytes, entries[1])], [1, 2, 3]);
  assert.equal((await zipRead(bytes, entries[2])).length, 0);
  assert.equal(await crc32Blob(new Blob([big])), crc32(big));
});

test('a zip read as a Blob: its entries from the end of the file, each entry as a Blob, packed or stored, checked', async () => {
  const { writeZip: w, zipOfBlobs: zb, zipBlobEntries, zipBlobEntry } = await import('../zip.js');
  const save = new Uint8Array(300000).map((_, i) => (i * 7) & 0xff);
  const packed = new Blob([await w([{ name: 'Matt/Matt.sav', data: save }, { name: 'notes.txt', data: new TextEncoder().encode('hi') }])]);
  let entries = await zipBlobEntries(packed);
  assert.deepEqual(entries.map((e) => [e.name, e.size, e.method]), [['Matt/Matt.sav', 300000, 8], ['notes.txt', 2, 0]]);
  const back = await zipBlobEntry(packed, entries[0]);
  assert.ok(back instanceof Blob);
  assert.deepEqual(new Uint8Array(await back.arrayBuffer()), save);
  const stored = await zb([{ name: 'Arsenal.sav', blob: new Blob([save]) }]);
  entries = await zipBlobEntries(stored);
  assert.deepEqual(new Uint8Array(await (await zipBlobEntry(stored, entries[0])).arrayBuffer()), save);
  // A damaged entry is refused, not stored.
  const bytes = new Uint8Array(await stored.arrayBuffer());
  bytes[100] ^= 0xff;
  await assert.rejects(zipBlobEntry(new Blob([bytes]), entries[0]), /checksum/);
  await assert.rejects(zipBlobEntries(new Blob([new Uint8Array(100)])), /not a zip/);
});
