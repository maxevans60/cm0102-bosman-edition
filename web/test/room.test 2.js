// Tests of the room for a save while the game writes it (web/room.js), on a stand-in of Emscripten's memory file
// system: its first write to an empty file keeps a copy of the written bytes, and a write past the room grows the file
// by 1.125 times, copying it. Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { reserveSaveRoom } from '../room.js';

function memfs() {
  const nodes = new Map(), grown = { n: 0 };
  const ops = {
    write(stream, buffer, offset, length, position) {
      const node = stream.node;
      if (node.usedBytes === 0 && position === 0) { node.contents = buffer.slice(offset, offset + length); node.usedBytes = length; return length; }
      const need = position + length, have = node.contents?.length ?? 0;
      if (need > have) {
        const next = new Uint8Array(Math.max(need, Math.floor(have * 1.125)));
        next.set(node.contents.subarray(0, node.usedBytes));
        node.contents = next;
        grown.n++;
      }
      node.contents.set(buffer.subarray(offset, offset + length), position);
      node.usedBytes = Math.max(node.usedBytes, need);
      return length;
    },
  };
  const FS = {
    open(path) { let node = nodes.get(path); if (!node) { node = { contents: null, usedBytes: 0, stream_ops: ops }; nodes.set(path, node); } return { node, stream_ops: node.stream_ops, position: 0 }; },
    write(stream, buffer, offset, length, position) { return stream.stream_ops.write(stream, buffer, offset, length, position); },
    readdir: () => ['.', '..', ...[...nodes.keys()].map((p) => p.slice('/user/'.length))],
    stat: (p) => ({ size: nodes.get(p).usedBytes }),
  };
  return { FS, nodes, grown };
}

function writeSave(FS, size, piece = 65536) {
  const s = FS.open('/user/temp_save_file.sav', 'w+');
  const buf = new Uint8Array(piece).fill(7);
  for (let at = 0; at < size; at += piece) FS.write(s, buf, 0, Math.min(piece, size - at), at);
  return s.node;
}

test('a save written over one of its size: room at once, no copy of the file while it grows', () => {
  const { FS, nodes, grown } = memfs();
  const old = FS.open('/user/Big.sav');
  old.node.usedBytes = 40 << 20;                       // a save of 40 MB in the folder
  reserveSaveRoom(FS);
  let rooms = 0;
  const Real = globalThis.Uint8Array;
  globalThis.Uint8Array = class extends Real { constructor(...a) { super(...a); if (typeof a[0] === 'number' && a[0] > (30 << 20)) rooms++; } };
  let node;
  try { node = writeSave(FS, 40 << 20); } finally { globalThis.Uint8Array = Real; }
  assert.equal(rooms, 1, 'the room is made once (not before the first write, whose bytes the file system copies anyway)');
  assert.equal(grown.n, 0, 'the file system never grew it');
  assert.equal(node.contents.length, Math.ceil((40 << 20) * 1.05) + (1 << 20));
  assert.equal(node.usedBytes, 40 << 20);
  assert.ok(node.contents.subarray(0, node.usedBytes).every((b) => b === 7), 'every byte where it was written');
  assert.ok(nodes.get('/user/temp_save_file.sav') === node);
});

test('larger than the room, or with no save in the folder: it grows as before', () => {
  const a = memfs();
  a.FS.open('/user/Small.sav').node.usedBytes = 1 << 20;
  reserveSaveRoom(a.FS);
  const node = writeSave(a.FS, 6 << 20);
  assert.ok(a.grown.n > 0);
  assert.equal(node.usedBytes, 6 << 20);
  assert.ok(node.contents.subarray(0, node.usedBytes).every((b) => b === 7));
  const b = memfs();
  reserveSaveRoom(b.FS);
  const plain = writeSave(b.FS, 2 << 20);
  const c = memfs();
  const before = writeSave(c.FS, 2 << 20);
  assert.equal(b.grown.n, c.grown.n, 'the same growth as without the room');
  assert.equal(plain.contents.length, before.contents.length);
});

test('another file is left alone', () => {
  const { FS, grown } = memfs();
  FS.open('/user/Big.sav').node.usedBytes = 40 << 20;
  reserveSaveRoom(FS);
  const s = FS.open('/user/settings.txt');
  assert.equal(s.stream_ops.write, s.node.stream_ops.write);
  FS.write(s, new Uint8Array(10), 0, 10, 0);
  FS.write(s, new Uint8Array(10), 0, 10, 10);
  assert.equal(grown.n, 1);
});

test('off while the vault is off: IDBFS would store the whole room with the save', () => {
  const { FS, grown } = memfs();
  FS.open('/user/Big.sav').node.usedBytes = 40 << 20;
  let on = false;
  reserveSaveRoom(FS, { when: () => on });
  const node = writeSave(FS, 2 << 20);
  assert.ok(grown.n > 0 && node.contents.length < (4 << 20), `${node.contents.length} bytes of room`);
});
