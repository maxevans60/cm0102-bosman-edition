// The data sets in the Saved games dialog (web/README.md, "Data sets"), as the saves are there: each with Use (for
// the next new game, in a season), Edit, Download (a zip) and Delete, and Upload for a zip or the 20 files. With
// Cloud Saves, a Cloud column and Remove, as for the saves. The work is in datasets.js.
import { whenCell } from './saves.js';
import { DATA_SET_FILES } from './dataset.js';
import { FIRST_SEASON, LAST_SEASON, chooseForNextGame, cleanName, dataSetZip, dataSetsInZip, deleteDataSet, fromLibrary, listDataSets,
  markFromLibrary, seasonText, uniqueName, writeDataSet } from './datasets.js';

// An uploaded data set starts on the latest season the game has (an update of the current season, most often); the
// game measures its own year from its dates of birth and moves it to the season chosen (season_dates.c).

const $ = (id) => document.getElementById(id);
const esc = (s) => String(s ?? '').replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[c]);
const MB = (n) => `${(n / 1048576).toFixed(0)} MB`;
const CLOUD = { cloud: 'In the cloud', up: 'Saving…', here: 'Only here', waiting: 'Not yet' };

// root: the user folder; module(): the engine's Module; cloud(): the data sets' sync (datasets.js) or null; edit(name),
// create(): the editor; changed(): after a data set came or went (the cloud sync runs). library: the server has the
// database library (library.mjs): Get a database lists it.
export function mountSetsPanel({ FS, root, sync, log, toast, module, cloud = () => null, edit, create, changed = () => {}, library = false }) {
  const box = $('sets-list'), note = $('sets-note'), input = $('sets-file'), lib = $('sets-lib');
  const say = (text, bad = false) => { note.textContent = text; note.className = bad ? 'small bad' : 'small'; };
  let shown = [], using = null, watched = null;
  let entries = [], askedAt = 0;                      // the library's list (showLibrary), and when it was asked

  const render = () => { drawOwn(); showLibrary(); };
  // The player's own data sets (the library's list is drawn by showLibrary).
  function drawOwn() {
    shown = listDataSets(FS, root);
    const c = cloud();
    if (c && c !== watched) { watched = c; c.onChange(() => { if ($('saves').open) render(); }); }
    if (!shown.length) {
      box.innerHTML = `<p class="small">No data sets yet. ${entries.length ? 'Get one from the list below, upload one,' : 'Upload one,'} or make one from the game's data (New data set).</p>`;
      return;
    }
    box.innerHTML = `<table><tr><th>Data set</th><th class="num">Size</th><th>Changed</th>${c ? '<th>Cloud</th>' : ''}<th></th></tr>
      ${shown.map((s, i) => {
        const st = c?.status(s.name);
        return `<tr><td>${esc(s.name)}</td><td class="num">${MB(s.size)}</td>${whenCell(s.mtime)}
        ${c ? (fromLibrary(s) ? '<td class="cloud">From the library</td>'
          : `<td class="cloud ${st}">${CLOUD[st]}${st === 'cloud' ? ` <button class="link" data-set-remove="${i}">Remove</button>` : ''}</td>`) : ''}
        <td class="acts"><div class="btns"><button class="primary" data-set-use="${i}">Use</button><button data-set-edit="${i}">Edit</button>
          <button data-set-download="${i}">Download</button><button class="link" data-set-delete="${i}">Delete</button></div></td></tr>
        ${using === s.name ? `<tr class="use"><td colspan="${c ? 5 : 4}"><div class="use-row"><label>Season of the next new game
          <select id="sets-season">${Array.from({ length: LAST_SEASON - FIRST_SEASON + 1 }, (_, k) => FIRST_SEASON + k)
            .map((y) => `<option value="${y}"${y === (s.season ?? FIRST_SEASON) ? ' selected' : ''}>${seasonText(y)}</option>`).join('')}</select></label>
          <button class="primary" data-set-go="${i}">Use for my next new game</button></div>
          <span class="small">A season database that the game knows keeps its own season. A copy of the game's data is 2001/02;
          a newer database moves to the season you choose, its players keeping their ages.</span></td></tr>` : ''}`;
      }).join('')}</table>`;
  }

  async function upload(files) {
    const done = [], refused = [];
    const taken = () => listDataSets(FS, root).map((s) => s.name);
    const zips = files.filter((f) => /\.zip$/i.test(f.name)), dats = files.filter((f) => /\.dat$/i.test(f.name));
    // Anything else was once left out without a word, so a database as a .rar (as many are shared) seemed to do nothing.
    for (const f of files.filter((x) => !/\.(zip|dat)$/i.test(x.name))) {
      refused.push(/\.(rar|7z)$/i.test(f.name)
        ? `${f.name}: a .rar or .7z cannot be read here. Unpack it (on a phone, a file manager app can), then upload the folder as a .zip, or its .dat files.`
        : `${f.name}: a data set uploads as a .zip, or as its .dat files.`);
    }
    for (const f of zips) {
      say(`Reading ${f.name}…`);
      try {
        for (const set of await dataSetsInZip(new Uint8Array(await f.arrayBuffer()), f.name.replace(/\.zip$/i, ''))) {
          const name = uniqueName(set.name, taken());
          writeDataSet(FS, root, name, set.files, { season: LAST_SEASON });
          done.push(name);
        }
      } catch (e) { refused.push(`${f.name}: ${e.message}`); }
    }
    if (dats.length) {
      const have = new Map(dats.map((f) => [f.name.toLowerCase(), f]));
      const lacking = DATA_SET_FILES.filter((n) => !have.has(n));
      if (lacking.length) refused.push(`The ${DATA_SET_FILES.length} database files go together: ${lacking.join(', ')} ${lacking.length === 1 ? 'is' : 'are'} missing.`);
      else {
        const asked = prompt('A name for this data set:', 'Uploaded data set');
        if (asked !== null) {
          const name = uniqueName(cleanName(asked), taken());
          try {
            const files = {};
            for (const n of DATA_SET_FILES) files[n] = new Uint8Array(await have.get(n).arrayBuffer());
            writeDataSet(FS, root, name, files, { season: LAST_SEASON });
            done.push(name);
          } catch (e) { refused.push(`${name}: ${e.message}`); }
        }
      }
    }
    if (done.length) { sync(); log(`page: data set(s) uploaded: ${done.join(', ')}`); changed(); }
    render();
    say([done.length ? `Uploaded ${done.join(', ')}. Use one to start a new game with it.` : '', ...refused].filter(Boolean).join('\n'), refused.length > 0 && !done.length);
  }

  box.addEventListener('click', async (e) => {
    const b = e.target.closest('button');
    if (!b) return;
    const s = (k) => shown[Number(b.dataset[k])];
    if (b.dataset.setUse !== undefined) { using = using === s('setUse').name ? null : s('setUse').name; render(); return; }
    if (b.dataset.setGo !== undefined) {
      const set = s('setGo'), season = Number($('sets-season').value);
      try {
        const how = await chooseForNextGame(module(), FS, root, set.name, season);
        sync();                                   // settings.txt to the browser's storage (the engine's call does not wait for it)
        using = null;
        render();
        say(how === 'now' ? `The next new game starts ${set.name} in ${seasonText(season)}: Start New Game on the game's first screen starts it. The button at the top left of that screen shows the new choice the next time the screen opens.`
          : `${set.name} in ${seasonText(season)} is chosen for the next new game from the next start of this page. Reload the page (after saving your game) to use it now.`);
        log(`page: ${set.name} chosen for the next new game, ${seasonText(season)} (${how})`);
      } catch (err) { say(`${set.name} was not chosen: ${err.message}`, true); }
      return;
    }
    if (b.dataset.setEdit !== undefined) { $('saves').close(); edit(s('setEdit').name); return; }
    if (b.dataset.setDownload !== undefined) {
      const set = s('setDownload');
      say(`Packing ${set.name}…`);
      try {
        const url = URL.createObjectURL(new Blob([await dataSetZip(FS, root, set.name)], { type: 'application/zip' }));
        const a = document.createElement('a');
        a.href = url;
        a.download = `${set.name}.zip`;
        document.body.appendChild(a);
        a.click();
        a.remove();
        setTimeout(() => URL.revokeObjectURL(url), 60000);
        say(`Downloaded ${set.name}.zip. The desktop Bosman Edition takes it too: unpack it into Databases in its save folder.`);
      } catch (err) { say(`${set.name} was not packed: ${err.message}`, true); }
      return;
    }
    if (b.dataset.setDelete !== undefined) {
      const set = s('setDelete'), c = cloud();
      if (!confirm(`Delete the data set ${set.name}${c ? ' here and in the cloud' : ''}? Games started with it keep running; their saves hold their own data.`)) return;
      try {
        deleteDataSet(FS, root, set.name);
        await c?.deleted(set.name);
        sync();
        say(`${set.name} is deleted.`);
      } catch (err) { say(`${set.name} was not deleted: ${err.message}`, true); }
      render();
      return;
    }
    if (b.dataset.setRemove !== undefined) {
      const set = s('setRemove');
      try { await cloud().remove(set.name); say(`${set.name} is no longer in the cloud. It stays in this browser.`); }
      catch (err) { say(`${set.name} was not removed: ${err.message}`, true); }
      render();
    }
  });
  // ---- Get a database: the library of the server (library.mjs), each database for a season. ----
  // The library's list, always on the Databases tab under the player's own (asked again a minute after the last time).
  const head = $('sets-lib-head');
  function drawLibrary() {
    const have = new Set(listDataSets(FS, root).map((s) => s.library?.id).filter(Boolean));
    head.hidden = lib.hidden = !entries.length;
    head.innerHTML = `Available to download <span>· ${entries.length} database${entries.length === 1 ? '' : 's'}, free</span>`;
    $('sets-step1').innerHTML = entries.length ? '<b>Get</b> a database below, or upload your own' : '<b>Upload</b> a database, or make one from the game\'s data';
    lib.innerHTML = entries.length ? `<table><tr><th>Database</th><th>Season</th><th class="num">Size</th><th></th></tr>
      ${entries.map((e, i) => `<tr><td>${esc(e.name)}${e.note ? `<div class="small">${esc(e.note)}</div>` : ''}</td><td>${seasonText(e.season)}</td>
        <td class="num">${MB(e.size)}</td><td class="acts"><div class="btns"><button class="primary" data-lib-get="${i}">${have.has(e.id) ? 'Get again' : 'Get'}</button></div></td></tr>`).join('')}</table>` : '';
  }
  async function showLibrary() {
    if (!library) return;
    drawLibrary();
    if (Date.now() - askedAt < 60000) return;
    askedAt = Date.now();
    try {
      const r = await fetch('/api/library', { cache: 'no-store' });
      if (!r.ok) throw new Error(`HTTP ${r.status}`);
      entries = (await r.json()).entries;
    } catch (err) { askedAt = 0; log(`page: library: the list did not come (${err.message})`); }
    if (!shown.length) drawOwn();                     // its "No data sets yet" names the list below
    drawLibrary();
  }
  // One database: its zip (straight from the store where it can, else through the server), into Databases with its
  // season, then Use opens with that season.
  async function getFromLibrary(e) {
    const show = (got) => say(`Getting ${e.name}: ${Math.min(100, Math.floor((100 * got) / Math.max(1, e.size)))}%`);
    show(0);
    const read = async (r) => {
      if (!r.ok) throw new Error(`HTTP ${r.status}`);
      const out = new Uint8Array(e.size), reader = r.body.getReader();
      let at = 0;
      for (;;) {
        const { value, done } = await reader.read();
        if (done) break;
        if (at + value.length > out.length) throw new Error('more bytes than the library said');
        out.set(value, at);
        at += value.length;
        show(at);
      }
      if (at !== out.length) throw new Error('the download stopped before its end');
      return out;
    };
    let zip = null;
    try {
      const link = await (await fetch(`/api/library/${e.id}?direct=get`, { cache: 'no-store' })).json();
      if (link.direct) zip = await read(await fetch(link.url, { cache: 'no-store' }));
    } catch (err) { log(`page: library: ${e.name} not straight from the store (${err.message}): through the server`); }
    zip ??= await read(await fetch(`/api/library/${e.id}`, { cache: 'no-store' }));
    const [set] = await dataSetsInZip(zip, e.name);
    const name = uniqueName(set.name === e.name ? set.name : e.name, listDataSets(FS, root).map((s) => s.name));
    writeDataSet(FS, root, name, set.files, { season: Math.min(e.season, LAST_SEASON) });   // 2026/27: the game's last season
    markFromLibrary(FS, root, name, e.id);
    sync();
    changed();
    log(`page: ${name} came from the database library (${seasonText(e.season)}, ${MB(e.size)})`);
    return name;
  }
  lib.addEventListener('click', async (ev) => {
    const b = ev.target.closest('button[data-lib-get]');
    if (!b) return;
    const e = entries[Number(b.dataset.libGet)];
    for (const x of lib.querySelectorAll('button')) x.disabled = true;
    try {
      const name = await getFromLibrary(e);
      using = name;                                    // Use opens, with its season chosen
      render();
      say(e.season > LAST_SEASON
        ? `${name} is here: a ${seasonText(e.season)} database. The game's latest season is ${seasonText(LAST_SEASON)}, so a new game with it starts there, its players keeping their ages. Press Use for my next new game, then start a new game.`
        : `${name} is here, for ${seasonText(e.season)}. Press Use for my next new game, then start a new game.`);
      box.closest('.dlg-body').scrollTop = 0;         // the new data set and its Use panel, in view
    } catch (err) {
      say(`${e.name} did not come: ${err.message}`, true);
      drawLibrary();
    }
  });
  $('sets-upload').onclick = () => input.click();
  $('sets-new').onclick = () => { $('saves').close(); create(); };
  input.onchange = async () => { const files = [...input.files]; input.value = ''; await upload(files); };
  return { render, say };
}
