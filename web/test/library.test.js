// Tests of the database library (library.mjs and its routes in server.mjs) against the fake S3 store: the admin's
// upload (through the server, and straight to the store), the check of the zip, the list, a player's download, an edit
// and a removal. Run: cd web && npm test
import { test, before, after } from 'node:test';
import assert from 'node:assert/strict';
import { mkdtempSync, mkdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { createLibrary, entryFields } from '../library.mjs';
import { createS3 } from '../s3.mjs';
import { createStats } from '../stats.mjs';
import { createApp } from '../server.mjs';
import { writeZip } from '../zip.js';
import { fakeDataSet, fakeS3 } from './fakes.mjs';

const TOKEN = 't'.repeat(24);
let tmp, s3fake, s3, server, base, direct = false;
const logs = [];

before(async () => {
  tmp = mkdtempSync(join(tmpdir(), 'cm0102-library-test-'));
  mkdirSync(join(tmp, 'game'));
  writeFileSync(join(tmp, 'manifest.tsv'), '');
  s3fake = await fakeS3();
  s3 = createS3({ endpoint: s3fake.endpoint, bucket: s3fake.bucket, accessKeyId: 'tid_test', secretAccessKey: 'tsec_test', pathStyle: true });
  const library = createLibrary({ s3, presign: (m, k, o) => s3.presign(m, k, o), isDirect: () => direct, log: (t) => logs.push(t) });
  server = createApp({ gameDir: join(tmp, 'game'), buildDir: join(tmp, 'build'), manifestPath: join(tmp, 'manifest.tsv'),
    stats: createStats({ store: null }), adminToken: TOKEN, library });
  await new Promise((r) => server.listen(0, '127.0.0.1', r));
  base = `http://127.0.0.1:${server.address().port}`;
});
after(() => { server.close(); s3fake.close(); rmSync(tmp, { recursive: true, force: true }); });

const admin = (path, { method = 'GET', body, headers = {} } = {}) => fetch(`${base}${path}`, { method, body: body === undefined ? undefined : typeof body === 'string' || body instanceof Uint8Array ? body : JSON.stringify(body),
  headers: { authorization: `Bearer ${TOKEN}`, ...(body && !(body instanceof Uint8Array) ? { 'content-type': 'application/json' } : {}), ...headers } });
const dataSetZip = async (folder = '2011 update') => writeZip(Object.entries(fakeDataSet()).map(([name, data]) => ({ name: `${folder}/${name}`, data })));
const list = async () => (await (await fetch(`${base}/api/library`)).json()).entries;

test('the fields of an entry: a folder name, a season of the game, a short note', () => {
  assert.deepEqual(entryFields({ name: ' 2011 update ', season: '2011', note: 'From the forum\u0007, by A.' }).fields, { name: '2011 update', season: 2011, note: 'From the forum, by A.' });
  assert.match(entryFields({ name: 'a/b', season: 2011 }).why, /a name of 1 to 60/);
  assert.match(entryFields({ name: 'ok', season: 1999 }).why, /a season from 2001 to 2026/);
  assert.equal(entryFields({ name: 'ok', season: 2026 }).fields.season, 2026, '2026/27: a database after the game\'s last season');
  assert.match(entryFields({ name: 'ok', season: 2027 }).why, /a season/);
  assert.match(entryFields({ name: 'ok', season: 2011.5 }).why, /a season/);
  assert.equal(entryFields({ name: 'ok', season: 2025, note: 'x'.repeat(900) }).fields.note.length, 500);
});

test('an empty library; only an admin adds to it, and the page knows there is one', async () => {
  assert.deepEqual(await list(), []);
  assert.equal((await fetch(`${base}/api/admin/library`, { method: 'POST', body: '{"size":10}' })).status, 401);
  assert.equal((await (await fetch(`${base}/files.json`)).json()).library, true);
  assert.equal((await fetch(`${base}/api/library`, { method: 'POST' })).status, 405);
});

test('the admin uploads through the server, names it; everyone lists it and gets the same bytes', async () => {
  direct = false;
  const zip = await dataSetZip();
  const start = await (await admin('/api/admin/library', { method: 'POST', body: { size: zip.length } })).json();
  assert.equal(start.direct, false);
  assert.match(start.id, /^[0-9a-f]{16}$/);
  assert.equal((await admin(`/api/admin/library/${start.id}/body`, { method: 'PUT', body: zip, headers: { 'content-type': 'application/zip' } })).status, 204);
  const done = await admin(`/api/admin/library/${start.id}/done`, { method: 'POST', body: { name: '2011 update', season: 2011, note: 'Squads of December 2011' } });
  assert.equal(done.status, 201);
  const entries = await list();
  assert.deepEqual(entries.map((e) => [e.id, e.name, e.season, e.note, e.size]), [[start.id, '2011 update', 2011, 'Squads of December 2011', zip.length]]);
  assert.ok(entries[0].rawSize > 0 && entries[0].added);
  assert.ok(!s3fake.objects.has(`library/incoming/${start.id}.zip`), 'the upload left its incoming place');
  assert.deepEqual(await (await fetch(`${base}/api/library/${start.id}?direct=get`)).json(), { direct: false });
  const got = new Uint8Array(await (await fetch(`${base}/api/library/${start.id}`)).arrayBuffer());
  assert.ok(Buffer.from(got).equals(Buffer.from(zip)), 'the same zip');
  assert.match(logs.join(' '), /library: 2011 update \(2011\) added/);
});

test('straight to the store: a signed upload link for the admin, a signed download link for everyone', async () => {
  direct = true;
  const zip = await dataSetZip('2025 update');
  const start = await (await admin('/api/admin/library', { method: 'POST', body: { size: zip.length } })).json();
  assert.equal(start.direct, true);
  assert.equal((await fetch(start.url, { method: 'PUT', body: zip, headers: start.headers })).status, 200);
  assert.equal((await admin(`/api/admin/library/${start.id}/done`, { method: 'POST', body: { name: '2025 update', season: 2025 } })).status, 201);
  const entries = await list();
  assert.deepEqual(entries.map((e) => e.season), [2025, 2011], 'the newest season first');
  const link = await (await fetch(`${base}/api/library/${start.id}?direct=get`)).json();
  assert.equal(link.direct, true);
  assert.ok(Buffer.from(await (await fetch(link.url)).arrayBuffer()).equals(Buffer.from(zip)));
  direct = false;
});

test('a zip that is not a data set is refused and not listed; sizes and fields are checked', async () => {
  const junk = await writeZip([{ name: 'readme.txt', data: new TextEncoder().encode('hello') }]);
  const start = await (await admin('/api/admin/library', { method: 'POST', body: { size: junk.length } })).json();
  await admin(`/api/admin/library/${start.id}/body`, { method: 'PUT', body: junk });
  const r = await admin(`/api/admin/library/${start.id}/done`, { method: 'POST', body: { name: 'Junk', season: 2011 } });
  assert.equal(r.status, 422);
  assert.match((await r.json()).error, /not a data set: it does not hold the 20 database files/);
  assert.ok(!(await list()).some((e) => e.name === 'Junk'));
  assert.ok(!s3fake.objects.has(`library/incoming/${start.id}.zip`));
  assert.equal((await admin('/api/admin/library', { method: 'POST', body: { size: 200 << 20 } })).status, 413);
  assert.equal((await admin('/api/admin/library/0123456789abcdef/done', { method: 'POST', body: { name: 'x', season: 2011 } })).status, 404, 'nothing uploaded');
  assert.equal((await admin('/api/admin/library/0123456789abcdef/done', { method: 'POST', body: { name: 'x', season: 1990 } })).status, 400);
  assert.equal((await fetch(`${base}/api/library/nothere`)).status, 404);
});

test('an upload never named goes after a day, when the admin page lists the library', async () => {
  const old = 'library/incoming/00000000000000aa.zip';
  await s3.put(old, Buffer.from('x'), { type: 'application/zip' });
  s3fake.objects.get(old).modified = new Date(Date.now() - 2 * 86400e3).toISOString();
  await s3.put('library/incoming/00000000000000bb.zip', Buffer.from('y'), { type: 'application/zip' });
  assert.equal((await admin('/api/admin/library')).status, 200);
  assert.ok(!s3fake.objects.has(old) && s3fake.objects.has('library/incoming/00000000000000bb.zip'));
  await s3.delete('library/incoming/00000000000000bb.zip');
});

test('the admin changes an entry and takes one away; the page cannot', async () => {
  const [newest, older] = await list();
  const changed = await admin(`/api/admin/library/${older.id}`, { method: 'PATCH', body: { note: 'Squads of 1 December 2011', season: 2010 } });
  assert.equal(changed.status, 200);
  assert.deepEqual((await list()).find((e) => e.id === older.id).note, 'Squads of 1 December 2011');
  // From a page of another site, with a session but no token: refused (the token is the script's way in).
  assert.equal((await fetch(`${base}/api/admin/library/${newest.id}`, { method: 'DELETE' })).status, 401);
  assert.equal((await admin(`/api/admin/library/${newest.id}`, { method: 'DELETE' })).status, 204);
  assert.deepEqual((await list()).map((e) => e.id), [older.id]);
  assert.ok(!s3fake.objects.has(`library/${newest.id}.zip`), 'its zip is gone');
  assert.match(logs.join(' '), /library: 2025 update \(2025\) removed/);
});
