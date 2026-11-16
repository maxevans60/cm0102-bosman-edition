// Tactics in the cloud (web/README.md, "Cloud saves"): the tactics of the folder tactics (tactics.js, the game's
// Tactics, Save As) on every device of an account with Cloud Saves. The cloud keeps one record per account (/api/tactics,
// cloud.mjs): each tactic with its file time, and the names removed with the time they went. A sync merges the cloud's
// record with this browser's folder, per name the newest wins (a file's time, or its removal); the result is written here
// and, when it differs from the cloud's, goes up over the version read. A 409 means another device wrote meanwhile: the
// sync reads again and merges once more. settings.txt does not go: it holds what suits each device (the picture size,
// the window), and a phone would get the desktop's.
export const TACTICS_DIR = '/user/tactics';
const STATE = '/user/.tactics-sync.json';     // this browser's removals not yet in the cloud, by account

const b64 = (bytes) => { let s = ''; for (let i = 0; i < bytes.length; i += 0x8000) s += String.fromCharCode(...bytes.subarray(i, i + 0x8000)); return btoa(s); };
const unb64 = (text) => Uint8Array.from(atob(text), (c) => c.charCodeAt(0));

// The plan of one sync. local: Map name -> { mtime }; cloud: Map name -> { mtime, data }; removed: name -> time, the
// cloud's and this browser's together; cloudRemoved: the cloud's own. Returns { write: [names from the cloud], remove:
// [names to delete here], keep: [names whose local file goes up], removed: the removals that stay, changed: whether the
// cloud's record differs }.
export function planTactics(local, cloud, removed, cloudRemoved = removed) {
  const write = [], remove = [], keep = [], gone = {};
  for (const name of new Set([...local.keys(), ...cloud.keys(), ...Object.keys(removed)])) {
    const l = local.get(name)?.mtime ?? -1, c = cloud.get(name)?.mtime ?? -1, r = removed[name] ?? -1;
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

export function createTacticSync({ FS, sync, log, account, onChange = () => {} }) {
  let chain = Promise.resolve(), last = '', stopped = false;
  const state = () => { try { const s = JSON.parse(FS.readFile(STATE, { encoding: 'utf8' })); return s.account === account ? s : { account, removed: {} }; } catch { return { account, removed: {} }; } };
  const keepState = (s) => { FS.writeFile(STATE, JSON.stringify(s)); sync(); };
  const listLocal = () => {
    const out = new Map();
    let names = [];
    try { names = FS.readdir(TACTICS_DIR).filter((n) => /\.tct$/i.test(n)); } catch { return out; }
    for (const n of names) { try { out.set(n, { mtime: +new Date(FS.stat(`${TACTICS_DIR}/${n}`).mtime) }); } catch { /* gone */ } }
    return out;
  };
  const signature = () => [...listLocal()].map(([n, f]) => `${n}:${f.mtime}`).sort().join('|');

  async function once() {
    for (let round = 0; round < 3 && !stopped; round++) {
      const r = await fetch('/api/tactics', { cache: 'no-store' });
      if (r.status === 401 || r.status === 403) { stopped = true; return; }
      if (!r.ok) throw new Error(`the cloud's tactics: HTTP ${r.status}`);
      const there = await r.json();
      const cloud = new Map(there.tactics.map((t) => [t.name, t]));
      const mine = state();
      const removed = { ...there.removed };
      for (const [n, at] of Object.entries(mine.removed)) removed[n] = Math.max(removed[n] ?? 0, at);
      const local = listLocal();
      const plan = planTactics(local, cloud, removed, there.removed);
      if (plan.write.length || plan.remove.length) {
        try { FS.mkdir(TACTICS_DIR); } catch { /* there */ }
        for (const n of plan.write) {
          const t = cloud.get(n);
          FS.writeFile(`${TACTICS_DIR}/${n}`, unb64(t.data));
          try { FS.utime(`${TACTICS_DIR}/${n}`, t.mtime, t.mtime); } catch { /* the time of the copy */ }
        }
        for (const n of plan.remove) { try { FS.unlink(`${TACTICS_DIR}/${n}`); } catch { /* gone */ } }
        sync();
        log(`page: tactics from the cloud: ${plan.write.length} in, ${plan.remove.length} removed`);
        onChange();
      }
      if (plan.changed) {
        const tactics = [...plan.write.map((n) => cloud.get(n)), ...plan.keep.map((n) => ({ name: n, mtime: local.get(n).mtime,
          data: b64(FS.readFile(`${TACTICS_DIR}/${n}`)) }))];
        const put = await fetch('/api/tactics', { method: 'PUT', body: JSON.stringify({ tactics, removed: plan.removed }),
          headers: { 'content-type': 'application/json', 'x-cm': '1', 'x-base-etag': there.etag ?? 'none' } });
        if (put.status === 409) continue;                    // another device wrote meanwhile: read and merge again
        if (put.status === 401 || put.status === 403) { stopped = true; return; }
        if (!put.ok) throw new Error(`tactics up: ${(await put.json().catch(() => ({}))).error ?? `HTTP ${put.status}`}`);
        log(`page: tactics in the cloud: ${tactics.length}`);
      }
      keepState({ account, removed: {} });                    // this browser's removals are in the cloud's record now
      last = signature();
      return;
    }
  }

  // One sync after the ones before it; never throws (a failure waits for the next).
  function run() {
    chain = chain.then(once).catch((e) => log(`page: tactics sync: ${e.message ?? e}`));
    return chain;
  }

  return {
    run,
    // A sync when the folder changed since the last one (the game saved a tactic): each minute (loader.js).
    runIfChanged() { return signature() !== last ? run() : chain; },
    // The page deleted a tactic (tactics.js): it goes from the other devices at the next sync.
    removed(name) { const s = state(); s.removed[name] = Date.now(); keepState(s); return run(); },
    stop() { stopped = true; },
  };
}
