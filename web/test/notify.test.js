// Tests of the notice to the developer when a player sends a report (web/notify.mjs, and its use in web/stats.mjs):
// one short message per report to the webhook of CM_REPORT_WEBHOOK, with the game's message and whether a save came,
// never the player's note or log. Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { NOTICES_A_DAY, createNotifier } from '../notify.mjs';
import { createStats } from '../stats.mjs';

const memoryStore = () => { const m = new Map(); return { m, get: async (k) => m.get(k) ?? null, put: async (k, v) => { m.set(k, v); },
  list: async (p) => [...m.keys()].filter((k) => k.startsWith(p)).map((key) => ({ key })), delete: async (k) => { m.delete(k); },
  head: async (k) => (m.has(k) ? { size: m.get(k).length } : null) }; };
const wait = (ms) => new Promise((ok) => setTimeout(ok, ms));
// A webhook that records what it got.
const hook = (status = 204) => {
  const got = [];
  const fetch = async (url, init) => { got.push({ url, init, body: JSON.parse(init.body) }); return { ok: status < 300, status }; };
  return { got, fetch };
};

test('the notifier posts JSON that Discord (content), Slack and Telegram (text) take; none without a webhook', async () => {
  const h = hook();
  const n = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  assert.equal(n.on, true);
  await n.notice('hello');
  assert.equal(h.got.length, 1);
  assert.equal(h.got[0].url, 'https://example.test/hook');
  assert.equal(h.got[0].init.method, 'POST');
  assert.equal(h.got[0].init.headers['content-type'], 'application/json');
  assert.deepEqual(h.got[0].body, { content: 'hello', text: 'hello', allowed_mentions: { parse: [] } });
  const off = createNotifier({ url: '', fetch: h.fetch, log: () => {} });
  assert.equal(off.on, false);
  await off.notice('nothing');
  assert.equal(h.got.length, 1);
});

test('a webhook that fails or does not answer is logged, never thrown', async () => {
  const lines = [];
  const bad = createNotifier({ url: 'https://example.test/hook', fetch: async () => { throw new Error('connection refused'); }, log: (l) => lines.push(l) });
  await bad.notice('x');
  const refused = createNotifier({ url: 'https://example.test/hook', fetch: async () => ({ ok: false, status: 404 }), log: (l) => lines.push(l) });
  await refused.notice('x');
  assert.match(lines.join('\n'), /notice not sent: connection refused/);
  assert.match(lines.join('\n'), /notice not sent: the webhook answered 404/);
});

test(`at most ${NOTICES_A_DAY} notices a day, then one that says more came, then none until the next day`, async () => {
  let t = Date.parse('2026-11-16T10:00:00Z');
  const h = hook();
  const n = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {}, now: () => t });
  for (let i = 0; i < NOTICES_A_DAY + 5; i++) await n.notice(`report ${i}`);
  assert.equal(h.got.length, NOTICES_A_DAY + 1);
  assert.match(h.got.at(-1).body.text, /More reports came today: see the admin page/);
  t += 86400000;
  await n.notice('next day');
  assert.equal(h.got.at(-1).body.text, 'next day');
});

test('a sent report: one notice after a while, with the game\'s message and no save; the note and the log stay out', async () => {
  const h = hook();
  const notify = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  const s = createStats({ store: memoryStore(), notify, noticeMs: 30, site: 'https://cm.example.test/' });
  const sent = await s.send({ message: 'Error: v3.9.68 discipline..cpp 274', note: 'my secret note', log: ['page: a log line'], engine: 'jspi x' }, null,
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/142.0.0.0 Safari/537.36');
  assert.ok(sent.key);
  assert.equal(h.got.length, 0, 'not at once: a save may follow');
  await wait(80);
  assert.equal(h.got.length, 1);
  const text = h.got[0].body.text;
  assert.match(text, /A player sent a report: "Error: v3\.9\.68 discipline\.\.cpp 274"/);
  assert.match(text, /Chrome 142 · Windows/);
  assert.match(text, /with a note/);
  assert.match(text, /No save/);
  assert.match(text, /https:\/\/cm\.example\.test\/admin/);
  assert.doesNotMatch(text, /secret|a log line/);
});

test('a sent report whose save arrives: the notice goes then, with the save, and only once', async () => {
  const h = hook();
  const notify = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  const store = memoryStore();
  const s = createStats({ store, notify, noticeMs: 60 });
  const sent = await s.send({ log: [] }, null, '');
  await s.saveLink({ key: sent.key, token: sent.token, name: 'BIF.sav', size: 3 << 20, rawSize: 177 << 20 });
  store.m.set(sent.key.replace(/\.json$/, '.sav.gz'), Buffer.alloc(3 << 20));
  await s.saveDone({ key: sent.key, token: sent.token });
  await wait(10);
  assert.equal(h.got.length, 1);
  assert.match(h.got[0].body.text, /A player sent a report from Help/);
  assert.match(h.got[0].body.text, /Save: BIF\.sav, 3\.0 MB packed \(177 MB\)/);
  await wait(100);
  assert.equal(h.got.length, 1, 'no second notice when the wait ends');
});

test('at the end of the wait the report file says whether a save came (it may reach another machine)', async () => {
  const h = hook();
  const notify = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  const store = memoryStore();
  const s = createStats({ store, notify, noticeMs: 40 });
  const other = createStats({ store });                    // the other machine, where the save arrives
  const sent = await s.send({ message: 'x', log: [] }, null, '');
  await other.saveLink({ key: sent.key, token: sent.token, name: 'Far.sav', size: 1 << 20, rawSize: 0 });
  store.m.set(sent.key.replace(/\.json$/, '.sav.gz'), Buffer.alloc(1 << 20));
  await other.saveDone({ key: sent.key, token: sent.token });
  const s2 = await s.send({ message: 'y', log: [] }, null, '');
  await other.saveLink({ key: s2.key, token: s2.token, name: 'Slow.sav', size: 1 << 20 });
  await wait(100);
  const texts = h.got.map((g) => g.body.text).join('\n');
  assert.match(texts, /"x".*Save: Far\.sav, 1\.0 MB packed\./);
  assert.match(texts, /"y".*A save is on its way: Slow\.sav\./);
});

test('the notices still waiting go out when the server stops', async () => {
  const h = hook();
  const notify = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  const s = createStats({ store: memoryStore(), notify, noticeMs: 60000 });
  await s.send({ message: 'x', log: [] }, null, '');
  await s.stop();
  assert.equal(h.got.length, 1);
});

test('what a sender wrote cannot ping a channel or make a link: Discord mentions off, Slack markup escaped, @ broken', async () => {
  const h = hook();
  const n = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  await n.notice('Error: @everyone <!channel> <http://evil.example|click> & more');
  const { content, text, allowed_mentions: am } = h.got[0].body;
  assert.deepEqual(am, { parse: [] });
  assert.doesNotMatch(content, /@everyone|<!channel>|<http/);
  assert.match(text, /&lt;!channel&gt;/);
  assert.match(text, /&amp; more/);
  assert.match(content, /@\u200beveryone/);
});

test('a notice waiting for its save keeps only what it says, not the report', async () => {
  const h = hook();
  const notify = createNotifier({ url: 'https://example.test/hook', fetch: h.fetch, log: () => {} });
  const s = createStats({ store: memoryStore(), notify, noticeMs: 60000 });
  await s.send({ message: 'x', note: 'n', log: Array.from({ length: 2000 }, () => 'y'.repeat(400)) }, null, '');
  const kept = JSON.stringify(s.pendingNotices());
  assert.ok(kept.length < 400, `${kept.length} characters kept: ${kept.slice(0, 120)}`);
  await s.stop();
});
