// The vault alone (web/vault.js), in Chromium with a real IndexedDB and a small file system like MEMFS: a quick check
// (seconds), where page-check.cjs takes minutes. Needs Playwright: NODE_PATH=$(npm root -g) node web/test/vault-check.cjs
//
// Reading in pieces. A player's saves are 690 MB each. The game opens a save about 100 times during a load, and the
// vault once read the whole file from the browser's storage at every open (a run of the real engine, 15 November 2026:
// four saves of 690 MB, the Load Game list 17 s, a load 99 s with 5.6 GB of memory in the tab). A save opened to be
// read now loads nothing; each read takes the pieces of the stored Blob it needs.
const { chromium } = require('playwright');
const http = require('node:http');
const fs = require('node:fs');
const path = require('node:path');

const WEB = path.resolve(__dirname, '..');

(async () => {
  const server = http.createServer((q, r) => {
    if (q.url === '/') { r.writeHead(200, { 'content-type': 'text/html' }); r.end('<!doctype html><title>vault check</title>'); return; }
    try { r.writeHead(200, { 'content-type': 'text/javascript' }); r.end(fs.readFileSync(path.join(WEB, path.basename(q.url)))); }
    catch { r.writeHead(404); r.end(); }
  });
  await new Promise((ok) => server.listen(0, '127.0.0.1', ok));
  let ok = true;
  const check = (what, cond, detail = '') => { console.log(`${cond ? 'ok  ' : 'FAIL'} ${what}${detail ? `: ${detail}` : ''}`); ok &&= Boolean(cond); };
  const browser = await chromium.launch();
  try {
    const page = await browser.newPage();
    await page.goto(`http://127.0.0.1:${server.address().port}/`);
    const r = await page.evaluate(async () => {
      const { createVault } = await import('/vault.js');
      const nodes = new Map(), dirs = new Set(['/', '/user']);
      const enoent = (p) => Object.assign(new Error(`ENOENT ${p}`), { errno: 44 });
      // As MEMFS: a file node has contents, usedBytes, mtime and stream_ops.read.
      const memRead = (stream, buffer, offset, length, position) => {
        const c = stream.node.contents, n = Math.max(0, Math.min(length, stream.node.usedBytes - position));
        buffer.set(c.subarray(position, position + n), offset);
        return n;
      };
      const FS = {
        streams: [], cwd: () => '/',
        mkdirTree: (p) => { let cur = ''; for (const part of p.split('/').filter(Boolean)) { cur += `/${part}`; dirs.add(cur); } },
        modeStringToFlags: (m) => (m.includes('w') ? 577 : 0),
        lookupPath(p) { if (nodes.has(p)) return { node: nodes.get(p) }; if (dirs.has(p)) return { node: { path: p } }; throw enoent(p); },
        getPath: (n) => n.path,
        writeFile(p, data) { const n = nodes.get(p) ?? { path: p, stream_ops: { read: memRead } }; nodes.set(p, n); n.contents = data; n.usedBytes = data.length; n.mtime = Date.now(); },
        readFile(p) { const n = nodes.get(p); if (!n) throw enoent(p); return n.contents.slice(0, n.usedBytes); },
        unlink(p) { if (!nodes.delete(p)) throw enoent(p); },
        rename(a, b) { const n = nodes.get(a); if (!n) throw enoent(a); nodes.delete(a); nodes.set(b, n); n.path = b; },
        readdir(d) {
          if (!dirs.has(d)) throw enoent(d);
          const names = new Set(['.', '..']);
          for (const p of [...nodes.keys(), ...dirs]) if (p.startsWith(`${d}/`)) names.add(p.slice(d.length + 1).split('/')[0]);
          return [...names];
        },
        stat(p) {
          if (nodes.has(p)) return { mode: 0o100644, size: nodes.get(p).usedBytes, mtime: new Date(nodes.get(p).mtime) };
          if (dirs.has(p)) return { mode: 0o040755, size: 0, mtime: new Date(0) };
          throw enoent(p);
        },
        isFile: (m) => (m & 0o170000) === 0o100000, isDir: (m) => (m & 0o170000) === 0o040000,
      };
      const MB = 1 << 20, SIZE = 18 * MB + 123;
      const pattern = (i, salt = 0) => (i * 7 + (i >>> 16) + salt) & 255;
      const make = (salt) => { const b = new Uint8Array(SIZE); for (let i = 0; i < SIZE; i++) b[i] = pattern(i, salt); return b; };
      const vault = createVault({ FS, stable: 0 });
      await vault.open(); vault.active = true; await vault.mount();
      const name = '/user/Big 2011-12-11.sav';
      FS.writeFile(name, make(0));
      await vault.persist();
      const node = FS.lookupPath(name).node;
      // What comes from the browser's storage: the size of each Blob read.
      const reads = [];
      const realBuffer = Blob.prototype.arrayBuffer;
      Blob.prototype.arrayBuffer = function () { reads.push(this.size); return realBuffer.call(this); };
      const out = { stored: node.vault };
      const read = async (position, length, salt = 0) => {
        const wait = vault.readForGame(node, position, length);
        if (wait) await wait;
        const buf = new Uint8Array(length), n = node.stream_ops.read({ node }, buf, 0, length, position);
        let same = true;
        for (let i = 0; i < n; i++) if (buf[i] !== pattern(position + i, salt)) { same = false; break; }
        return { n, same };
      };
      try {
        const w = vault.openForGame(node, 0);
        if (w) await w;
        out.opened = { state: node.vault, inMemory: node.contents.length, storageReads: [...reads] };
        out.header = await read(0, 270336);
        out.afterHeader = { largest: Math.max(0, ...reads), total: reads.reduce((a, b) => a + b, 0) };
        out.acrossPieces = await read(4 * MB - 100, 200);
        out.atEnd = await read(SIZE - 10, 100);
        let unasked;
        try { node.stream_ops.read({ node }, new Uint8Array(10), 0, 10, 13 * MB); unasked = 'read'; } catch (e) { unasked = `error ${e.errno ?? e.message}`; }
        out.unasked = unasked;
        vault.settle(node);                                    // the game closed it
        out.closed = node.vault;
        const before = reads.length;
        const again = vault.openForGame(node, 0);
        out.reopenWaits = Boolean(again);
        if (again) await again;
        out.headerAgain = await read(0, 270336);
        out.readsForReopen = reads.length - before;
        vault.settle(node);
        // The whole file through the pieces, as a load reads it.
        vault.openForGame(node, 0);
        let all = true;
        for (let at = 0; at < SIZE; at += 65536) { const x = await read(at, 65536); if (!x.same) all = false; }
        out.whole = { all, largest: Math.max(0, ...reads) };
        vault.settle(node);
        // A new version of the save (the game opens it to write it anew, O_TRUNC): the old pieces are not served.
        vault.openForGame(node, 577);
        FS.writeFile(name, make(3));
        await vault.persist();
        const w2 = vault.openForGame(FS.lookupPath(name).node, 0);
        if (w2) await w2;
        out.newVersion = await read(0, 1000, 3);
        // Opened to be written (not truncated): the whole file comes into memory, as before.
        vault.settle(node);
        const w3 = vault.openForGame(node, 2);
        if (w3) await w3;
        out.forWriting = { state: node.vault, inMemory: node.contents.length };
        // An upload or a cloud download goes straight into the storage (adopt, and storeSave of saves.js): the page
        // once read the whole file into memory and copied it into the user folder (690 MB: 3.4 GB in the tab).
        const { storeSave, useVault, SAVE_HEADER } = await import('/saves.js');
        const save = (n, fill) => { const b = new Uint8Array(SAVE_HEADER + n), dv = new DataView(b.buffer); dv.setInt32(0, 2, true); dv.setInt32(8, 1, true); dv.setUint32(12, SAVE_HEADER, true); b.fill(fill, SAVE_HEADER); return b; };
        const big = save(9 * MB, 5);
        useVault(vault);
        reads.length = 0;
        const why = await storeSave(FS, 'Uploaded 2002-01-05.sav', new Blob([big]), { mtime: 1762770000000 });
        const up = FS.lookupPath('/user/Uploaded 2002-01-05.sav').node;
        out.upload = { why, largestRead: Math.max(0, ...reads), state: up.vault, inMemory: up.contents.length, size: FS.stat('/user/Uploaded 2002-01-05.sav').size,
          mtime: +FS.stat('/user/Uploaded 2002-01-05.sav').mtime };
        out.uploadKept = (await vault.persist()).removed;
        const back = new Uint8Array(await (await vault.blob('/user/Uploaded 2002-01-05.sav')).arrayBuffer());
        out.uploadSame = back.length === big.length && back[SAVE_HEADER + 5] === 5 && back[back.length - 1] === 5;
        // A cloud download (disk.js, toStoredBlob) and a save the player picked (Upload): the vault keeps their bytes,
        // not a reference to a file. Chromium's IndexedDB keeps a reference to a file's Blob, and the save then broke
        // when that file went; storeSave gives the vault a copy the browser makes there (ownable).
        const { toStoredBlob, storageCopiesFiles } = await import('/disk.js');
        out.copiesFiles = await storageCopiesFiles();
        const fromCloud = save(6 * MB, 7);
        const unpacked = await toStoredBlob(new Blob([fromCloud]).stream());
        out.diskWhy = await storeSave(FS, 'From the cloud.sav', unpacked.blob, { mtime: 1762770000000, owned: unpacked.owned });
        await unpacked.done();
        const opfs = await (await navigator.storage.getDirectory()).getDirectoryHandle('cm0102-tmp', { create: true });
        const fromVault = new Uint8Array(await (await vault.blob('/user/From the cloud.sav')).arrayBuffer());
        out.diskSame = fromVault.length === fromCloud.length && fromVault[SAVE_HEADER + 1] === 7 && fromVault[fromVault.length - 1] === 7;
        const picked = save(5 * MB, 4);
        const ph = await opfs.getFileHandle('picked.sav', { create: true });
        const pw = await ph.createWritable(); await pw.write(picked); await pw.close();
        out.pickedWhy = await storeSave(FS, 'Picked.sav', await ph.getFile(), { mtime: 1762770000000 });
        await opfs.removeEntry('picked.sav');                        // the player deletes the file from Downloads
        try {
          const b2 = new Uint8Array(await (await vault.blob('/user/Picked.sav')).arrayBuffer());
          out.pickedSame = b2.length === picked.length && b2[SAVE_HEADER + 1] === 4 && b2[b2.length - 1] === 4;
        } catch (e) { out.pickedSame = `${e.name}`; }
        const left = [];
        for await (const [n] of opfs.entries()) left.push(n);
        out.diskLeft = left.length;
        // The save editor keeps the save as it was as an older copy (a rename), then stores the edited one under the
        // name (adopt) before the vault's round moved the renamed file's record: each keeps its own bytes.
        dirs.add('/user/backups');
        FS.rename('/user/Uploaded 2002-01-05.sav', '/user/backups/Uploaded 2002-01-05 (before edit).sav');
        await storeSave(FS, 'Uploaded 2002-01-05.sav', new Blob([save(2 * MB, 9)]));
        await vault.persist();
        const firstOf = async (p) => { const b = new Uint8Array(await (await vault.blob(p)).arrayBuffer()); return [b.length, b[SAVE_HEADER + 1]]; };
        out.renamedThenAdopted = { old: await firstOf('/user/backups/Uploaded 2002-01-05 (before edit).sav'), now: await firstOf('/user/Uploaded 2002-01-05.sav') };
        // The save editor's older copy is a copy of the record (copy): the save keeps its name all the time, so a cloud
        // sync in between never finds it missing; then the edited save is stored under the name (adopt).
        await storeSave(FS, 'Edited.sav', new Blob([save(3 * MB, 4)]));
        await vault.persist();
        const copied = await vault.copy('/user/Edited.sav', '/user/backups/Edited (before edit).sav');
        const listedBetween = (() => { try { return FS.stat('/user/Edited.sav').size; } catch { return null; } })();
        await storeSave(FS, 'Edited.sav', new Blob([save(1 * MB, 6)]));
        await vault.persist();
        out.copied = { copied, listedBetween, old: await firstOf('/user/backups/Edited (before edit).sav'), now: await firstOf('/user/Edited.sav'),
          backupState: FS.lookupPath('/user/backups/Edited (before edit).sav').node.vault };
        FS.writeFile('/user/Fresh.sav', save(10, 1));          // written by the game, not yet in the vault
        out.copyOfFresh = await vault.copy('/user/Fresh.sav', '/user/backups/Fresh (before edit).sav');
        out.junk = await storeSave(FS, 'Junk.sav', new Blob([new Uint8Array(3 * MB)]));
        out.junkStored = Boolean(nodes.get('/user/Junk.sav'));
        out.empty = await storeSave(FS, 'Empty.sav', new Blob([]));
        // A cloud download as a stream (storeSaveStream, vault.adoptStream): stored in parts of 4 MB as it comes, never one
        // Blob of the whole save (an iPhone could not hold a 446 MB one, 18 November 2026). The parts are no files.
        const { storeSaveStream } = await import('/saves.js');
        const { PART } = await import('/vault.js');
        const partKeys = async () => {
          const db = await new Promise((ok, bad) => { const q = indexedDB.open('cm0102-vault'); q.onsuccess = () => ok(q.result); q.onerror = () => bad(q.error); });
          const store = db.objectStoreNames[0];
          const all = await new Promise((ok, bad) => { const q = db.transaction(store).objectStore(store).getAllKeys(); q.onsuccess = () => ok(q.result); q.onerror = () => bad(q.error); });
          db.close();
          return all.filter((k) => typeof k === 'string' && k.startsWith(PART));
        };
        const inChunks = (bytes, size) => new ReadableStream({ start(c) { for (let i = 0; i < bytes.length; i += size) c.enqueue(bytes.slice(i, i + size)); c.close(); } });
        const streamed = save(9 * MB + 12345, 3);
        const partsBefore = (await partKeys()).length;
        reads.length = 0;
        out.streamWhy = await storeSaveStream(FS, 'Streamed.sav', inChunks(streamed, 1000003), { mtime: 1762770000000 });
        out.streamReads = Math.max(0, ...reads);                // what the store read back of it: its header at most
        const sn = nodes.get('/user/Streamed.sav');
        out.streamNode = { state: sn?.vault, inMemory: sn?.contents.length, size: FS.stat('/user/Streamed.sav').size, mtime: +FS.stat('/user/Streamed.sav').mtime };
        out.streamParts = (await partKeys()).length - partsBefore;
        out.streamListed = (await vault.records()).map((r) => r.key).filter((k) => k.startsWith(PART) || k === '/user/Streamed.sav');
        const sback = new Uint8Array(await (await vault.blob('/user/Streamed.sav')).arrayBuffer());
        out.streamSame = sback.length === streamed.length && sback.every((b, i) => b === streamed[i]);
        // The game reads it in pieces through the parts.
        await vault.openForGame(sn, 0);
        await vault.readForGame(sn, 5 * MB - 10, 30);
        const piece = new Uint8Array(30);
        out.streamPiece = { state: sn.vault, n: sn.stream_ops.read({ node: sn }, piece, 0, 30, 5 * MB - 10), same: piece.every((b, i) => b === streamed[5 * MB - 10 + i]) };
        vault.settle(sn);
        // A rename keeps its parts; an older copy shares them.
        FS.rename('/user/Streamed.sav', '/user/Streamed 2.sav');
        await vault.persist();
        out.streamCopied = await vault.copy('/user/Streamed 2.sav', '/user/backups/Streamed (before edit).sav');
        const renamed = new Uint8Array(await (await vault.blob('/user/Streamed 2.sav')).arrayBuffer());
        out.streamRenamed = renamed.length === streamed.length && renamed[renamed.length - 1] === 3;
        // A stream that is no save: refused, and none of its parts stay.
        const before2 = (await partKeys()).length;
        out.streamBad = await storeSaveStream(FS, 'Bad.sav', inChunks(new Uint8Array(5 * MB), 700000));
        out.streamBadParts = (await partKeys()).length - before2;
        out.streamBadFile = nodes.has('/user/Bad.sav');
        const other = createVault({ FS: (() => { const n2 = new Map(); return { ...FS, lookupPath(p) { if (n2.has(p)) return { node: n2.get(p) }; if (dirs.has(p)) return { node: { path: p } }; throw enoent(p); },
          writeFile(p, data) { const n = n2.get(p) ?? { path: p }; n2.set(p, n); n.contents = data; n.usedBytes = data.length; n.mtime = Date.now(); }, stat(p) { return { mode: 0o100644, size: n2.get(p).usedBytes, mtime: new Date(n2.get(p).mtime) }; } }; })(), stable: 0 });
        await other.open(); other.active = true;
        out.nextVisit = (await other.mount()).files;
        // The parts stay while a record names them (the save and its older copy), and go at the next start once none does.
        out.partsKept = (await partKeys()).length;
        FS.unlink('/user/Streamed 2.sav');
        FS.unlink('/user/backups/Streamed (before edit).sav');
        await vault.persist();
        const third = createVault({ FS: (() => { const n3 = new Map(); return { ...FS, lookupPath(p) { if (n3.has(p)) return { node: n3.get(p) }; if (dirs.has(p)) return { node: { path: p } }; throw enoent(p); },
          writeFile(p, data) { const n = n3.get(p) ?? { path: p }; n3.set(p, n); n.contents = data; n.usedBytes = data.length; n.mtime = Date.now(); }, stat(p) { return { mode: 0o100644, size: n3.get(p).usedBytes, mtime: new Date(n3.get(p).mtime) }; } }; })(), stable: 0 });
        await third.open(); third.active = true;
        await third.mount();
        out.partsAfter = (await partKeys()).length;
        // A game file (gameFile, loader.js): a placeholder of its size that reads from its source (the browser's cache) in
        // pieces when the game opens it, never whole; the page reads it whole through the vault (the editor); the vault's
        // rounds and records leave it alone (it is no save of the player's).
        const gsize = 9 * MB + 77, gbytes = make(9).subarray(0, gsize);
        let sourced = 0;
        vault.gameFile('/game/Data/language.ldb', gsize, async () => { sourced++; return new Blob([gbytes]); });
        const gnode = FS.lookupPath('/game/Data/language.ldb').node;
        out.gamePlaceholder = { state: gnode.vault, inMemory: gnode.contents.length, size: FS.stat('/game/Data/language.ldb').size };
        reads.length = 0;
        const gw = vault.openForGame(gnode, 0);
        if (gw) await gw;
        const gwait = vault.readForGame(gnode, 4 * MB - 50, 100);
        if (gwait) await gwait;
        const gbuf = new Uint8Array(100), gn = gnode.stream_ops.read({ node: gnode }, gbuf, 0, 100, 4 * MB - 50);
        out.gameRead = { n: gn, same: gbuf.every((x, i) => x === pattern(4 * MB - 50 + i, 9)), largest: Math.max(0, ...reads), state: gnode.vault, sourced };
        vault.settle(gnode);
        const gb = await vault.bytes('/game/Data/language.ldb');
        out.gameBytes = { length: gb.length, same: gb.every((x, i) => x === pattern(i, 9)) };
        await vault.persist();
        out.gameNotStored = !(await vault.records()).some((rec) => rec.key.startsWith('/game/'));
        out.gameCount = vault.gameFiles;
        // Without the vault (an Asyncify engine): the file is in the user folder, in memory, as before.
        useVault(null);
        out.plainWhy = await storeSave(FS, 'Plain.sav', new Blob([save(1000, 1)]));
        out.plain = { inMemory: nodes.get('/user/Plain.sav')?.contents.length, state: nodes.get('/user/Plain.sav')?.vault ?? null };
      } catch (e) { out.error = e.stack ?? String(e); }
      Blob.prototype.arrayBuffer = realBuffer;
      return out;
    });
    const MB = 1 << 20, SIZE = 18 * MB + 123;
    check('no error', !r.error, r.error ?? '');
    check('a save opened to be read: nothing is loaded from the storage', r.opened?.state === 'lazy' && r.opened.inMemory === 0 && r.opened.storageReads.length === 0, JSON.stringify(r.opened));
    check('its header (270 KB) comes with one piece of at most 4 MB, not the whole 18 MB',
      r.header?.n === 270336 && r.header.same && r.afterHeader.largest <= 4 * MB && r.afterHeader.total <= 4 * MB, JSON.stringify([r.header, r.afterHeader]));
    check('a read across two pieces, and one at the end of the file', r.acrossPieces?.n === 200 && r.acrossPieces.same && r.atEnd?.n === 10 && r.atEnd.same, JSON.stringify([r.acrossPieces, r.atEnd]));
    check('a read whose pieces were not asked for fails, it gives no wrong bytes', /^error/.test(r.unasked ?? ''), r.unasked);
    check('closed, it is a placeholder again; opened again, nothing new is read from the storage',
      r.closed === 'out' && r.reopenWaits === false && r.headerAgain?.same && r.readsForReopen === 0, JSON.stringify([r.closed, r.reopenWaits, r.readsForReopen]));
    check('the whole file reads right through the pieces, never more than 4 MB at once', r.whole?.all && r.whole.largest <= 4 * MB, JSON.stringify(r.whole));
    check('a new version of the save: its bytes, not the old pieces', r.newVersion?.n === 1000 && r.newVersion.same, JSON.stringify(r.newVersion));
    check('opened to be written: the whole file is in memory', r.forWriting?.state === 'in' && r.forWriting.inMemory === SIZE, JSON.stringify(r.forWriting));
    check('an upload goes into the storage as it is: only its header is read, the user folder gets a placeholder',
      r.upload?.why === null && r.upload.largestRead <= 0x416ec && r.upload.state === 'out' && r.upload.inMemory === 0
        && r.upload.size === 0x416ec + 9 * MB && r.upload.mtime === 1762770000000, JSON.stringify(r.upload));
    check('a cloud save goes into the vault as bytes the vault keeps, and leaves no file on the browser\'s disk',
      r.diskWhy === null && r.diskSame === true && r.diskLeft === 0, JSON.stringify([r.copiesFiles, r.diskWhy, r.diskSame, r.diskLeft]));
    check('a cloud download as a stream goes into the vault in parts of 4 MB, never read whole; the user folder gets a placeholder',
      r.streamWhy === null && r.streamNode?.state === 'out' && r.streamNode.inMemory === 0 && r.streamNode.size === 9 * MB + 12345 + 0x416ec
        && r.streamNode.mtime === 1762770000000 && r.streamParts === 3 && r.streamReads <= 0x416ec, JSON.stringify([r.streamWhy, r.streamNode, r.streamParts, r.streamReads]));
    check('its parts are no files: the vault lists the save once', JSON.stringify(r.streamListed) === '["/user/Streamed.sav"]', JSON.stringify(r.streamListed));
    check('it reads back whole, and the game reads a piece across two parts', r.streamSame && r.streamPiece?.n === 30 && r.streamPiece.same && r.streamPiece.state === 'lazy', JSON.stringify(r.streamPiece));
    check('renamed and copied (the editor\'s older copy), it keeps its bytes', r.streamRenamed && r.streamCopied === true, JSON.stringify([r.streamRenamed, r.streamCopied]));
    check('a stream that is no save is refused, and leaves no parts and no file', typeof r.streamBad === 'string' && r.streamBadParts === 0 && !r.streamBadFile, JSON.stringify([r.streamBad, r.streamBadParts]));
    check('the parts stay while a record names them, and go at the next start once none does', r.partsKept >= 3 && r.partsAfter === 0, JSON.stringify([r.partsKept, r.partsAfter]));
    check('a save the player picked (Upload) stays readable after its file is deleted (Chromium kept only a reference)',
      r.copiesFiles === false && r.pickedWhy === null && r.pickedSame === true, JSON.stringify([r.copiesFiles, r.pickedWhy, r.pickedSame]));
    check('it stays through the vault\'s round, reads back the same, and the next visit lists it', r.uploadKept === 0 && r.uploadSame && r.nextVisit >= 2, JSON.stringify([r.uploadKept, r.uploadSame, r.nextVisit]));
    check('a renamed save keeps its bytes when a new file is stored under its old name before the vault\'s round',
      JSON.stringify(r.renamedThenAdopted) === JSON.stringify({ old: [0x416ec + 9 * MB, 5], now: [0x416ec + 2 * MB, 9] }), JSON.stringify(r.renamedThenAdopted));
    check('the save editor\'s older copy is a copy of the record: the save stays listed, each keeps its bytes',
      JSON.stringify(r.copied) === JSON.stringify({ copied: true, listedBetween: 0x416ec + 3 * MB, old: [0x416ec + 3 * MB, 4], now: [0x416ec + MB, 6], backupState: 'out' }),
      JSON.stringify(r.copied));
    check('a file not yet in the vault is not copied by it (the editor copies it itself)', r.copyOfFresh === false, String(r.copyOfFresh));
    check('a file that is not a save is refused and not stored; an empty one says so',
      /not a saved game/.test(r.junk ?? '') && r.junkStored === false && /empty \(0 bytes\)/.test(r.empty ?? ''), JSON.stringify([r.junk, r.junkStored, r.empty]));
    check('a game file is a placeholder of its size: nothing of it in memory', r.gamePlaceholder?.state === 'out' && r.gamePlaceholder.inMemory === 0 && r.gamePlaceholder.size === 9 * MB + 77, JSON.stringify(r.gamePlaceholder));
    check('the game reads it in pieces from its source (the cache), across two pieces, never whole',
      r.gameRead?.n === 100 && r.gameRead.same && r.gameRead.largest <= 4 * MB && r.gameRead.state === 'lazy' && r.gameRead.sourced === 1, JSON.stringify(r.gameRead));
    check('the page reads it whole through the vault (the editor), and the vault\'s rounds and records leave it alone',
      r.gameBytes?.length === 9 * MB + 77 && r.gameBytes.same && r.gameNotStored === true && r.gameCount === 1, JSON.stringify([r.gameBytes, r.gameNotStored, r.gameCount]));
    check('without the vault a save goes into the user folder in memory, as before', r.plainWhy === null && r.plain?.inMemory === 0x416ec + 1000 && r.plain.state === null, JSON.stringify([r.plainWhy, r.plain]));
  } finally {
    await browser.close();
    server.close();
  }
  console.log(ok ? 'vault check: all ok' : 'vault check: FAILED');
  process.exit(ok ? 0 : 1);
})();
