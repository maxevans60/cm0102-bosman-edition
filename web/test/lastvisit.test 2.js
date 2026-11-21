// Tests of the sudden-stop mark (lastvisit.js). Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { mountVisitMark, suddenStopText } from '../lastvisit.js';

const store = () => { const m = new Map(); return { getItem: (k) => m.get(k) ?? null, setItem: (k, v) => m.set(k, String(v)), removeItem: (k) => m.delete(k), m }; };

test('a visit that the system closed is found by the next one, with its last step', () => {
  const s = store();
  let t = 1_000_000;
  const first = mountVisitMark({ storage: s, now: () => t });
  assert.equal(first.takeLastStop(), null);
  first.step('ignored before the start');
  assert.equal(s.m.size, 0, 'nothing marked before the game starts loading');
  first.begin({ step: 'downloading the game files', device: 'iPhone', engine: 'jspi' });
  t += 5000;
  first.step('Loading game data');
  // The tab is closed by the system here: no pagehide, no end().
  t += 120_000;
  const next = mountVisitMark({ storage: s, now: () => t });
  assert.deepEqual(next.takeLastStop(), { step: 'Loading game data', at: 1_005_000, device: 'iPhone', engine: 'jspi' });
  assert.equal(s.m.size, 0, 'the mark is read once');
  assert.equal(mountVisitMark({ storage: s, now: () => t }).takeLastStop(), null);
});

test('a visit that closed (pagehide) or stopped on its own leaves no mark; one older than a day is not told', () => {
  const s = store();
  let t = 0;
  const a = mountVisitMark({ storage: s, now: () => t });
  a.begin({ step: 'in the game' });
  a.end();
  assert.equal(mountVisitMark({ storage: s, now: () => t }).takeLastStop(), null);
  const b = mountVisitMark({ storage: s, now: () => t });
  b.begin({ step: 'in the game' });
  t += 2 * 86400e3;
  assert.equal(mountVisitMark({ storage: s, now: () => t }).takeLastStop(), null, 'two days old');
});

test('a page hidden into the back-forward cache comes back marked; a storage that throws marks nothing', () => {
  const s = store();
  const a = mountVisitMark({ storage: s });
  a.begin({ step: 'in the game' });
  a.end({ hide: true });
  assert.equal(s.m.size, 0);
  assert.equal(a.active, false);
  a.resume();
  assert.equal(a.active, true);
  assert.equal(JSON.parse(s.m.get('cm0102-visit')).step, 'in the game');
  const broken = { getItem() { throw new Error('denied'); }, setItem() { throw new Error('denied'); }, removeItem() { throw new Error('denied'); } };
  const c = mountVisitMark({ storage: broken });
  assert.equal(c.takeLastStop(), null);
  c.begin({ step: 'x' });
  c.step('y');
  c.end();
  assert.equal(mountVisitMark({ storage: null }).takeLastStop(), null);
});

test('tabs share the storage: a tab takes off only its own mark', () => {
  const s = store();
  const old = mountVisitMark({ storage: s, id: 'old' }), now = mountVisitMark({ storage: s, id: 'new' });
  old.begin({ step: 'in the game' });
  old.end();                                          // the old tab handed the game over
  now.begin({ step: 'in the game' });
  old.end();                                          // and closes later: the playing tab keeps its mark
  assert.equal(JSON.parse(s.m.get('cm0102-visit')).id, 'new');
  assert.equal(now.takeLastStop(), null, 'a tab does not tell its own mark');
});

test('a step\'s own details go with it: the cloud save that came when the tab closed', () => {
  const s = store();
  const a = mountVisitMark({ storage: s, id: 'a' });
  a.begin({ step: 'downloading the game files', device: 'iPhone' });
  a.step('getting Hertha BSC 2011-12-11.sav from the cloud', { cloudFile: 'Hertha BSC 2011-12-11.sav' });
  assert.equal(JSON.parse(s.m.get('cm0102-visit')).cloudFile, 'Hertha BSC 2011-12-11.sav');
  a.step('in the game');
  assert.equal(JSON.parse(s.m.get('cm0102-visit')).cloudFile, undefined, 'the next step has none');
  a.step('getting Big.sav from the cloud', { cloudFile: 'Big.sav' });
  const last = mountVisitMark({ storage: s, id: 'b' }).takeLastStop();
  assert.deepEqual([last.step, last.cloudFile, last.device], ['getting Big.sav from the cloud', 'Big.sav', 'iPhone']);
});

test('the words: when, the step, the likely cause, and an iPhone\'s own advice', () => {
  const now = 10 * 60000;
  const phone = suddenStopText({ step: 'Loading game data', at: now - 3 * 60000 }, { device: 'iPhone', now });
  assert.match(phone, /stopped suddenly on your last visit \(3 min ago, while Loading game data\)/);
  assert.match(phone, /too much memory/);
  assert.match(phone, /An iPhone gives a tab less memory/);
  assert.doesNotMatch(suddenStopText({ step: 'in the game', at: now }, { device: null, now }), /iPhone/);
});
