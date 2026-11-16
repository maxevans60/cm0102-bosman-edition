// Tactics in the cloud (web/README.md, "Cloud saves"): the tactics of the folder tactics (tactics.js, the game's
// Tactics, Save As) on every device of an account with Cloud Saves. The cloud keeps one record per account (/api/tactics,
// cloud.mjs): each tactic with its file time, and the names removed with the time they went. A sync merges the cloud's
// record with this browser's folder, per name the newest wins (a file's time, or its removal); the result is written here
// and, when it differs from the cloud's, goes up over the version read. A 409 means another device wrote meanwhile: the
// sync reads again and merges once more. On a device's first sync nothing of its own is written over or deleted: a
// tactic here that differs from the cloud's of the same name stays under the name "<name> (this device)", and the
// cloud's older removals do not delete one. settings.txt does not go: it holds what suits each device (the picture size,
// the window), and a phone would get the desktop's.
export const TACTICS_DIR = '/user/tactics';
export const MAX_TACTIC = 4096;                 // bytes of one tactic file (the game's: 1,476)
const REMOVED_MAX = 1000, REMOVED_DAYS = 400;   // the removals kept: the newest, of the last 400 days
const TIMEOUT_MS = 30000;
const STATE = '/user/.tactics-sync.json';     // this browser's removals not yet in the cloud, and the names it synced

// What the cloud keeps (cloud.mjs checks the same): a .tct name without path characters, and a file of 1 to 4096 bytes.
export function validTactic(name, size) {
  return typeof name === 'string' && /^[^\\/:*?"<>|\u0000-\u001f]{1,100}\.tct$/i.test(name) && !name.startsWith('.')
    && Number.isInteger(size) && size > 0 && size <= MAX_TACTIC;
}

// The removals kept: the newest REMOVED_MAX of the last REMOVED_DAYS days.
export function pruneRemoved(removed, now = Date.now()) {
  return Object.fromEntries(Object.entries(removed).filter(([, at]) => Number(at) > 0 && now - at < REMOVED_DAYS * 86400000)
    .sort((a, b) => b[1] - a[1]).slice(0, REMOVED_MAX));
}

const b64 = (bytes) => { let s = ''; for (let i = 0; i < bytes.length; i += 0x8000) s += String.fromCharCode(...bytes.subarray(i, i + 0x8000)); return btoa(s); };
const unb64 = (text) => Uint8Array.from(atob(text), (c) => c.charCodeAt(0));
const same = (a, b) => a.length === b.length && a.every((x, i) => x === b[i]);

// The plan of one sync. local: Map name -> { mtime }; cloud: Map name -> { mtime, data }; removed: name -> time, the
// cloud's and this browser's together; cloudRemoved: the cloud's own; known: the names this device synced before (null:
// all). Returns { write: [names from the cloud], remove: [names to delete here], keep: [names whose local file goes up],
// removed: the removals that stay, changed: whether the cloud's record differs }.
export function planTactics(local, cloud, removed, cloudRemoved = removed, { known = null } = {}) {
  const write = [], remove = [], keep = [], gone = {};
  for (const name of new Set([...local.keys(), ...cloud.keys(), ...Object.keys(removed)])) {
    const l = local.get(name)?.mtime ?? -1, c = cloud.get(name)?.mtime ?? -1, r = removed[name] ?? -1;
    if (l >= 0 && known && !known.has(name) && c < 0) { keep.push(name); continue; }   // never synced: no removal deletes it
    if (r >= l && r >= c) {                                // removed after its last change
      gone[name] = r;
      if (l >= 0) remove.push(name);
      continue;
    }
    if (c > l) write.push(name);
    else keep.push(name);
  }
  const after = new Map([...write.map((n) => [n, cloud.get(n).mtime]), ...keep.map((n) => [n, local.get(n).mtime])]);
  const changed = after.size !== cloud.size || [...after].some(([n, t]) => cloud.get(n)?.mtime !== t)
    || Object.keys(gone).length !== Object.keys(cloudRemoved).length || Object.entries(gone).some(([n, t]) => cloudRemoved[n] !== t);
  return { write, remove, keep, removed: gone, changed };
}

export function createTacticSync({ FS, sync, log, account, onChange = () => {}, fetch = globalThis.fetch }) {
  let chain = Promise.resolve(), last = null, stopped = false;
  const state = () => {
    try { const s = JSON.parse(FS.readFile(STATE, { encoding: 'utf8' })); if (s.account === account) return { removed: {}, known: null, ...s }; } catch { /* the first sync */ }
    return { account, removed: {}, known: null };
  };
  const keepState = (s) => { FS.writeFile(STATE, JSON.stringify(s)); sync(); };
  const path = (n) => `${TACTICS_DIR}/${n}`;
  const listLocal = () => {
    const out = new Map();
    let names = [];
    try { names = FS.readdir(TACTICS_DIR).filter((n) => /\.tct$/i.test(n)); } catch { return out; }
    for (const n of names) { try { const st = FS.stat(path(n)); out.set(n, { mtime: +new Date(st.mtime), size: st.size }); } catch { /* gone */ } }
    return out;
  };
  const signature = () => [...listLocal()].map(([n, f]) => `${n}:${f.mtime}`).sort().join('|');
  const freeName = (name, taken) => {
    const stem = name.slice(0, -4);
    for (let i = 1; ; i++) { const n = `${stem} (this device${i > 1 ? ` ${i}` : ''}).tct`; if (!taken.has(n)) return n; }
  };

  async function once() {
    for (let round = 0; round < 3 && !stopped; round++) {
      const r = await fetch('/api/tactics', { cache: 'no-store', signal: AbortSignal.timeout(TIMEOUT_MS) });
      if (r.status === 401 || r.status === 403) { stopped = true; return; }
      if (!r.ok) throw new Error(`the cloud's tactics: HTTP ${r.status}`);
      const there = await r.json();
      const cloud = new Map(there.tactics.map((t) => [t.name, t]));
      const mine = state();
      const known = mine.known ? new Set(mine.known) : new Set();
      let local = listLocal();
      // A tactic here that this device never synced and that differs from the cloud's of its name: kept under another name.
      let moved = 0;
      for (const [n] of local) {
        const c = cloud.get(n);
        if (known.has(n) || !c || same(FS.readFile(path(n)), unb64(c.data))) continue;
        const to = freeName(n, new Set([...local.keys(), ...cloud.keys()]));
        FS.rename(path(n), path(to));
        log(`page: tactics: ${n} here differs from the cloud's: it stays as ${to}`);
        moved++;
      }
      if (moved) local = listLocal();
      const removed = { ...there.removed };
      for (const [n, at] of Object.entries(mine.removed)) removed[n] = Math.max(removed[n] ?? 0, at);
      // Only what the cloud takes goes there; a tactic it would refuse stays here alone.
      const sendable = new Map([...local].filter(([n, f]) => validTactic(n, f.size)));
      const plan = planTactics(sendable, cloud, pruneRemoved(removed), there.removed, { known });
      if (plan.write.length || plan.remove.length || moved) {
        try { FS.mkdir(TACTICS_DIR); } catch { /* there */ }
        for (const n of plan.write) {
          const t = cloud.get(n);
          FS.writeFile(path(n), unb64(t.data));
          try { FS.utime(path(n), t.mtime, t.mtime); } catch { /* the time of the copy */ }
        }
        for (const n of plan.remove) { try { FS.unlink(path(n)); } catch { /* gone */ } }
        sync();
        log(`page: tactics from the cloud: ${plan.write.length} in, ${plan.remove.length} removed`);
        onChange();
      }
      if (plan.changed || moved) {
        const tactics = [...plan.write.map((n) => cloud.get(n)), ...plan.keep.map((n) => ({ name: n, mtime: sendable.get(n).mtime,
          data: b64(FS.readFile(path(n))) }))];
        const put = await fetch('/api/tactics', { method: 'PUT', body: JSON.stringify({ tactics, removed: plan.removed }), signal: AbortSignal.timeout(TIMEOUT_MS),
          headers: { 'content-type': 'application/json', 'x-cm': '1', 'x-base-etag': there.etag ?? 'none' } });
        if (put.status === 409) continue;                    // another device wrote meanwhile: read and merge again
        if (put.status === 401 || put.status === 403) { stopped = true; return; }
        if (!put.ok) throw new Error(`tactics up: ${(await put.json().catch(() => ({}))).error ?? `HTTP ${put.status}`}`);
        log(`page: tactics in the cloud: ${tactics.length}`);
      }
      // This browser's removals are in the cloud's record now; the names it holds are synced.
      keepState({ account, removed: {}, known: [...plan.write, ...plan.keep] });
      last = signature();
      return;
    }
    throw new Error('tactics: other devices kept writing; the next sync tries again');
  }

  // One sync after the ones before it; never throws (a failure waits for the next, which runIfChanged then makes).
  function run() {
    chain = chain.then(once).catch((e) => { last = null; log(`page: tactics sync: ${e.message ?? e}`); });
    return chain;
  }

  return {
    run,
    // A sync when the folder changed since the last one (the game saved a tactic), or the last one failed: each minute
    // (loader.js).
    runIfChanged() { return last === null || signature() !== last ? run() : chain; },
    // The page deleted a tactic (tactics.js): it goes from the other devices at the next sync.
    removed(name) { const s = state(); s.removed[name] = Date.now(); keepState(s); return run(); },
    stop() { stopped = true; },
  };
}
