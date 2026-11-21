// Tests of tactics in the cloud (web/tacticsync.js): the merge (planTactics) and the sync against a stand-in of
// /api/tactics (createTacticSync). Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createTacticSync, planTactics, pruneRemoved, validTactic } from '../tacticsync.js';

const L = (o) => new Map(Object.entries(o).map(([n, mtime]) => [n, { mtime }]));
const C = (o) => new Map(Object.entries(o).map(([n, mtime]) => [n, { name: n, mtime, data: 'x' }]));

test('per name the newest wins: a file here, a file in the cloud, or its removal', () => {
  const p = planTactics(
    L({ 'Same.tct': 5, 'Newer here.tct': 9, 'Older here.tct': 2, 'Only here.tct': 4, 'Removed there.tct': 3, 'Changed after.tct': 8 }),
    C({ 'Same.tct': 5, 'Newer here.tct': 6, 'Older here.tct': 7, 'Only there.tct': 1, 'Changed after.tct': 6 }),
    { 'Removed there.tct': 4, 'Changed after.tct': 7, 'Long gone.tct': 1 });
  assert.deepEqual(p.write.sort(), ['Older here.tct', 'Only there.tct']);
  assert.deepEqual(p.remove, ['Removed there.tct']);
  assert.deepEqual(p.keep.sort(), ['Changed after.tct', 'Newer here.tct', 'Only here.tct', 'Same.tct'].sort(), 'a file changed after its removal stays');
  assert.deepEqual(p.removed, { 'Removed there.tct': 4, 'Long gone.tct': 1 });
  assert.equal(p.changed, true);
});

test('nothing to send when both sides agree', () => {
  const p = planTactics(L({ 'A.tct': 1 }), C({ 'A.tct': 1 }), { 'B.tct': 3 });
  assert.deepEqual([p.write, p.remove, p.changed], [[], [], false]);
});

test('a removal here goes up and the file goes', () => {
  const p = planTactics(L({ 'A.tct': 1 }), C({ 'A.tct': 1 }), { 'A.tct': 2 });
  assert.deepEqual([p.remove, p.keep, p.removed, p.changed], [['A.tct'], [], { 'A.tct': 2 }, true]);
});

test('a removal here of a tactic the cloud has as removed already sends nothing; a new removal does', () => {
  assert.equal(planTactics(L({}), C({}), { 'A.tct': 2 }, { 'A.tct': 2 }).changed, false);
  assert.equal(planTactics(L({}), C({}), { 'A.tct': 2 }, {}).changed, true);
});

test('a tactic this device never synced is never deleted by an older removal elsewhere', () => {
  const p = planTactics(L({ 'Attack.tct': 1 }), C({}), { 'Attack.tct': 5 }, { 'Attack.tct': 5 }, { known: new Set() });
  assert.deepEqual([p.remove, p.keep, p.removed], [[], ['Attack.tct'], {}]);
});

test('the rules the cloud keeps: names, sizes, and at most 1000 removals of the last 400 days', () => {
  assert.ok(validTactic('My 4-4-2.tct', 1476));
  for (const [n, size] of [['../x.tct', 10], ['notes.txt', 10], ['.hidden.tct', 10], ['Big.tct', 5000], ['Empty.tct', 0]]) assert.ok(!validTactic(n, size), n);
  const now = Date.parse('2026-11-16T00:00:00Z');
  const many = Object.fromEntries(Array.from({ length: 1200 }, (_, i) => [`T${i}.tct`, now - i * 1000]));
  const kept = pruneRemoved({ ...many, 'Ancient.tct': now - 500 * 86400000 }, now);
  assert.equal(Object.keys(kept).length, 1000);
  assert.ok('T0.tct' in kept && !('T1100.tct' in kept) && !('Ancient.tct' in kept), 'the newest stay');
});

// A user folder and /api/tactics, in memory.
function standIn() {
  const files = new Map();
  const FS = {
    readdir: (d) => ['.', '..', ...[...files.keys()].filter((p) => p.startsWith(`${d}/`)).map((p) => p.slice(d.length + 1))],
    stat: (p) => { const f = files.get(p); if (!f) throw new Error(`ENOENT ${p}`); return { mtime: new Date(f.mtime), size: f.data.length }; },
    readFile: (p, o) => { const f = files.get(p); if (!f) throw new Error(`ENOENT ${p}`); return o?.encoding ? new TextDecoder().decode(f.data) : f.data; },
    writeFile: (p, d) => files.set(p, { data: typeof d === 'string' ? new TextEncoder().encode(d) : d, mtime: Date.now() }),
    utime: (p, a, m) => { files.get(p).mtime = m; },
    unlink: (p) => { if (!files.delete(p)) throw new Error(`ENOENT ${p}`); },
    rename: (a, b) => { files.set(b, files.get(a)); files.delete(a); },
    mkdir: () => {},
  };
  const server = { record: null, etag: 0, puts: 0, conflictOnce: false };
  const fetch = async (url, init = {}) => {
    const json = (status, body) => ({ ok: status < 300, status, json: async () => body });
    if (!init.method || init.method === 'GET') {
      const r = server.record ?? { tactics: [], removed: {} };
      return json(200, { ...r, etag: server.record ? `"${server.etag}"` : null });
    }
    server.puts++;
    if (server.conflictOnce) { server.conflictOnce = false; server.etag++; return json(409, { error: 'changed' }); }
    const base = init.headers['x-base-etag'];
    if (base !== (server.record ? `"${server.etag}"` : 'none')) return json(409, { error: 'changed' });
    server.record = JSON.parse(init.body);
    server.etag++;
    return json(200, { etag: `"${server.etag}"` });
  };
  return { FS, files, server, fetch };
}
const bytes = (n) => new Uint8Array(1476).fill(n);
const b64 = (u) => Buffer.from(u).toString('base64');

test('the first sync of a device: a different tactic of the same name here is kept under another name', async () => {
  const s = standIn();
  s.server.record = { tactics: [{ name: 'Attack.tct', data: b64(bytes(1)), mtime: 1000 }, { name: 'Same.tct', data: b64(bytes(3)), mtime: 50 }], removed: {} };
  s.server.etag = 1;
  s.FS.writeFile('/user/tactics/Attack.tct', bytes(2)); s.FS.utime('/user/tactics/Attack.tct', 0, 5000);   // newer, and another tactic
  s.FS.writeFile('/user/tactics/Same.tct', bytes(3)); s.FS.utime('/user/tactics/Same.tct', 0, 60);
  const t = createTacticSync({ FS: s.FS, sync: () => {}, log: () => {}, account: 'a@b', fetch: s.fetch });
  await t.run();
  assert.deepEqual(s.files.get('/user/tactics/Attack.tct').data, bytes(1), 'the cloud\'s tactic under its name');
  assert.deepEqual(s.files.get('/user/tactics/Attack (this device).tct').data, bytes(2), 'this device\'s under another name');
  assert.deepEqual(s.server.record.tactics.map((x) => x.name).sort(), ['Attack (this device).tct', 'Attack.tct', 'Same.tct']);
});

test('a write that meets another device\'s write reads and merges again; a failed sync is tried again', async () => {
  const s = standIn();
  s.FS.writeFile('/user/tactics/Mine.tct', bytes(4));
  s.server.conflictOnce = true;
  let up = true;
  const fetch = (u, i) => (up ? s.fetch(u, i) : Promise.reject(new TypeError('offline')));
  const t = createTacticSync({ FS: s.FS, sync: () => {}, log: () => {}, account: 'a@b', fetch });
  await t.run();
  assert.equal(s.server.puts, 2);
  assert.deepEqual(s.server.record.tactics.map((x) => x.name), ['Mine.tct']);
  up = false;
  s.FS.writeFile('/user/tactics/Next.tct', bytes(5));
  await t.runIfChanged();
  up = true;
  await t.runIfChanged();                                   // the folder is as before, but the last sync failed
  assert.deepEqual(s.server.record.tactics.map((x) => x.name).sort(), ['Mine.tct', 'Next.tct']);
});

test('a tactic the cloud would refuse stays here and does not stop the others', async () => {
  const s = standIn();
  s.FS.writeFile('/user/tactics/Good.tct', bytes(6));
  s.FS.writeFile('/user/tactics/Huge.tct', new Uint8Array(9000));
  const t = createTacticSync({ FS: s.FS, sync: () => {}, log: () => {}, account: 'a@b', fetch: s.fetch });
  await t.run();
  assert.deepEqual(s.server.record.tactics.map((x) => x.name), ['Good.tct']);
  assert.ok(s.files.has('/user/tactics/Huge.tct'));
});
