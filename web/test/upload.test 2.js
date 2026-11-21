// Tests of the sorting of an upload to Saved games by what the file is, not its name (web/upload.js). Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { gzipSync } from 'node:zlib';
import { sortUpload } from '../upload.js';
import { writeZip } from '../zip.js';
import { fakeSaveBytes } from './fakes.mjs';

const file = (name, bytes) => new File([bytes], name);
const tactic = () => { const t = Buffer.alloc(1476); t.writeUInt32LE(0x98ec5e, 0); t.writeUInt32LE(0xf2fffff2, 1472); return t; };
const names = (r) => ({ saves: r.saves.map((s) => s.name), tactics: r.tactics.map((t) => t.name), why: r.why });

test('a save, whatever its name: .sav, no extension, an extension a phone added', async () => {
  const save = fakeSaveBytes();
  assert.deepEqual(names(await sortUpload(file('Arsenal.sav', save))), { saves: ['Arsenal.sav'], tactics: [], why: null });
  assert.deepEqual(names(await sortUpload(file('Arsenal', save))).saves, ['Arsenal.sav']);
  const r = await sortUpload(file('Arsenal.sav.txt', save));
  assert.deepEqual(r.saves.map((s) => s.name), ['Arsenal.sav']);
  assert.equal(new Uint8Array(await (await r.saves[0].blob()).arrayBuffer()).length, save.length);
});

test('a zip, also one named .sav: its saves and tactics, wherever in its folders; not its older copies', async () => {
  const zip = await writeZip([{ name: 'Matt/Matt.sav', data: fakeSaveBytes(3) }, { name: 'backups/Matt.1.sav', data: fakeSaveBytes(4) },
    { name: 'tactics/My 4-4-2.tct', data: tactic() }, { name: 'readme.txt', data: Buffer.from('hi') }]);
  assert.deepEqual(names(await sortUpload(file('Matt.zip', zip))), { saves: ['Matt.sav'], tactics: ['My 4-4-2.tct'], why: null });
  const r = await sortUpload(file('Matt.sav', zip));
  assert.deepEqual(r.saves.map((s) => s.name), ['Matt.sav']);
  assert.match(r.notes.join(' '), /Matt\.sav is a zip/);
});

test('a gzip (as a report sends a save): unpacked and sorted again', async () => {
  const r = await sortUpload(file('Matt.sav.gz', gzipSync(fakeSaveBytes(5))));
  assert.deepEqual(r.saves.map((s) => s.name), ['Matt.sav']);
  assert.equal(new Uint8Array(await (await r.saves[0].blob()).arrayBuffer())[0], fakeSaveBytes(5)[0]);
});

test('a tactic uploaded as a save goes to the tactics', async () => {
  assert.deepEqual(names(await sortUpload(file('My 4-4-2.tct', tactic()))), { saves: [], tactics: ['My 4-4-2.tct'], why: null });
});

test('what is no save is named, with what to do', async () => {
  const why = async (name, bytes) => (await sortUpload(file(name, bytes))).why;
  assert.match(await why('Matt.rar', Buffer.from('Rar!\x1a\x07\x01\x00rest')), /a RAR archive.*unpack it first/i);
  assert.match(await why('Matt.7z', Buffer.from([0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c, 0, 4])), /a 7-Zip archive.*unpack it first/i);
  assert.match(await why('Matt.sav', Buffer.from('<!DOCTYPE html><html><body>Download</body></html>')), /a web page, not a save/);
  assert.match(await why('staff.dat', Buffer.alloc(5000, 1)), /a file of a data set.*Data sets/);
  const set = await writeZip(['staff.dat', 'club.dat', 'index.dat'].map((n) => ({ name: `Update/${n}`, data: Buffer.alloc(100, 1) })));
  assert.match(await why('Update 2026.zip', set), /a data set.*Data sets/);
  assert.match(await why('photo.png', Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0, 0])), /a picture \(PNG\)/);
  assert.match(await why('Pics.zip', await writeZip([{ name: 'a.jpg', data: Buffer.from([0xff, 0xd8, 0xff, 0]) }])), /holds no saved game: a\.jpg/);
  assert.match(await why('Matt.sav', Buffer.alloc(0)), /empty \(0 bytes\)/);
  assert.match(await why('Matt.sav', Buffer.alloc(300000, 9)), /not a saved game of CM 01\/02/);
});
