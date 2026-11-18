// Large files through the browser's disk instead of the page's memory (web/README.md, "Cloud saves"). A cloud save
// came down as one Blob of new Response(stream).blob(): Chrome keeps a Blob that large on disk, but an iPhone kept it
// in the tab's memory, about 446 MB for a long career, on top of the game files, and iOS closed the tab at each start
// ("A problem repeatedly occurred", an iPhone 17 Pro Max, 18 November 2026; iOS gives a tab about 2 GB whatever the
// phone has).
//
// toDisk writes the stream into a file of the origin's private file system (OPFS, cm0102-tmp/) as it comes, a piece at
// a time, and gives that file: a Blob the browser reads from disk. done() removes the file once its bytes are kept
// elsewhere (the vault copies them into IndexedDB, an upload has sent them). A browser without a writable OPFS gets the
// Blob in memory, as before.

const DIR = 'cm0102-tmp';
let dirOf = null;

async function tempDir() {
  dirOf ??= (async () => {
    try {
      if (typeof navigator.storage?.getDirectory !== 'function') return null;
      const dir = await (await navigator.storage.getDirectory()).getDirectoryHandle(DIR, { create: true });
      return typeof FileSystemFileHandle === 'function' && 'createWritable' in FileSystemFileHandle.prototype ? dir : null;
    } catch { return null; }
  })();
  return dirOf;
}

// { blob, done, onDisk }: the stream's bytes, on disk when the browser can.
export async function toDisk(stream) {
  const dir = await tempDir();
  if (dir) {
    const name = `${Date.now()}-${Math.random().toString(36).slice(2, 10)}`;
    let writable = null;
    try {
      writable = await (await dir.getFileHandle(name, { create: true })).createWritable();
    } catch {
      writable = null;                               // no writable here after all: in memory, before the stream is read
      await dir.removeEntry(name).catch(() => {});
    }
    if (writable) {
      const done = () => dir.removeEntry(name).catch(() => {});
      try {
        await stream.pipeTo(writable);
      } catch (e) {
        await done();
        throw e;
      }
      const blob = await (await dir.getFileHandle(name)).getFile();
      return { blob, done, onDisk: true };
    }
  }
  return { blob: await new Response(stream).blob(), done: async () => {}, onDisk: false };
}

// Whether this browser's IndexedDB keeps its own copy of a Blob that comes from a file, or only a reference to the file.
// Chromium keeps a reference, for a file of the OPFS and for one the player picked (Upload) alike: the vault's record
// then broke when that file went (NotFoundError), a save uploaded from Downloads and deleted there could no longer be
// read (18 November 2026, web/test/vault-check.cjs). A Blob that the browser made itself (new Response(…).blob()) is
// copied. Found out once per browser version with a file of 16 bytes, and kept in localStorage.
const PROBE_KEY = 'cm0102-storage-copies-files';
let copies = null;
export function storageCopiesFiles() {
  copies ??= (async () => {
    const ua = typeof navigator === 'object' ? navigator.userAgent : '';
    try { const kept = JSON.parse(localStorage.getItem(PROBE_KEY) ?? 'null'); if (kept?.ua === ua) return kept.copies; } catch { /* probe */ }
    const dir = await tempDir();
    if (!dir || typeof indexedDB === 'undefined') return false;      // no file to try with: copy, to be sure
    const name = `${Date.now()}-probe`, db = 'cm0102-probe';
    let result = false;
    try {
      const h = await dir.getFileHandle(name, { create: true });
      const w = await h.createWritable();
      await w.write(new Uint8Array(16).fill(7));
      await w.close();
      const file = await h.getFile();
      const idb = await new Promise((ok, bad) => { const r = indexedDB.open(db, 1); r.onupgradeneeded = () => r.result.createObjectStore('s'); r.onsuccess = () => ok(r.result); r.onerror = () => bad(r.error); });
      await new Promise((ok, bad) => { const t = idb.transaction('s', 'readwrite'); t.objectStore('s').put(file, 'f'); t.oncomplete = ok; t.onerror = t.onabort = () => bad(t.error); });
      await dir.removeEntry(name);
      const back = await new Promise((ok, bad) => { const r = idb.transaction('s').objectStore('s').get('f'); r.onsuccess = () => ok(r.result); r.onerror = () => bad(r.error); });
      try { const a = new Uint8Array(await back.arrayBuffer()); result = a.length === 16 && a[15] === 7; } catch { result = false; }
      idb.close();
    } catch { result = false; } finally {
      await dir.removeEntry(name).catch(() => {});
      try { indexedDB.deleteDatabase(db); } catch { /* left */ }
    }
    try { localStorage.setItem(PROBE_KEY, JSON.stringify({ ua, copies: result })); } catch { /* found again next time */ }
    return result;
  })();
  return copies;
}

// A Blob whose bytes the browser's storage keeps (the vault stores it): as it is where the storage copies files, else a
// copy that the browser makes (on its own disk in Chromium, not in the page's memory).
export async function ownable(blob) {
  return await storageCopiesFiles() ? blob : new Response(blob.stream()).blob();
}

// A stream to store in the vault (a cloud download): through a file on the disk where the storage copies files (an
// iPhone kept new Response(…).blob() in memory), else as a Blob the browser makes and the storage copies. { blob, done,
// onDisk, owned: true }.
export async function toStoredBlob(stream) {
  if (await storageCopiesFiles()) return { ...(await toDisk(stream)), owned: true };
  return { blob: await new Response(stream).blob(), done: async () => {}, onDisk: false, owned: true };
}

// Files left by a visit that ended in the middle (a closed tab): those older than an hour go (a younger one may be
// another tab's work in progress).
export async function sweepDisk({ olderThan = 3600e3 } = {}) {
  const dir = await tempDir();
  if (!dir) return 0;
  let n = 0;
  try {
    for await (const [name, handle] of dir.entries()) {
      const at = Number(name.split('-')[0]);
      if (handle.kind === 'file' && at > 0 && Date.now() - at > olderThan) { await dir.removeEntry(name).catch(() => {}); n++; }
    }
  } catch { /* nothing to sweep */ }
  return n;
}
