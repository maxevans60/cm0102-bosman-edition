// What a file given to Saved games is, by what it holds and not by its name (web/README.md, "Saved games"), and what
// to do with it. A player uploaded Matt.zip (a save zipped by another site, 16 November 2026) and was told that only
// .sav files are saved games: the page judged the name alone. sortUpload looks at the first bytes, and into a zip:
// - a saved game of CM 01/02 (its container header, saves.js checkSave) is a save, whatever its name: "Matt", or
//   "Matt.sav.txt" from a phone, becomes Matt.sav;
// - a zip (also one named .sav) gives the saves in it, wherever in its folders, except its older copies (backups/),
//   and its tactics; each is read from the zip only when it is stored (zip.js, zipBlobEntry);
// - a gzip (a report sends a save as .sav.gz) is unpacked and sorted again;
// - a tactic (tactics.js readyTactic) goes to the tactics;
// - anything else is named, with what to do: a RAR or 7-Zip archive, a web page (a download link that gave its page),
//   a data set or one of its files, a picture, a text, an empty file, or a file that is not a saved game of CM 01/02.
import { DATA_SET_FILES } from './dataset.js';
import { SAVE_HEADER, checkSave } from './saves.js';
import { readyTactic } from './tactics.js';
import { zipBlobEntries, zipBlobEntry } from './zip.js';

const SETS = new Set(DATA_SET_FILES);
const base = (path) => String(path).split(/[\\/]/).pop();
const starts = (b, ...sig) => sig.every((v, i) => b[i] === v);
const UNPACK = 'unpack it first (on a computer, or with an app that opens it on a phone), then upload the .sav file inside, or zip it as a .zip';

// The name a save gets: "Matt.sav" stays, "Matt.sav.txt" and "Matt" become "Matt.sav", another extension goes.
export function saveFileName(name) {
  const b = base(name);
  const m = /^(.*?\.sav)(\.[^.]*)*$/i.exec(b);
  if (m) return m[1];
  const stem = b.replace(/\.[^.]{1,5}$/, '') || 'uploaded save';
  return `${stem}.sav`;
}

// What the first bytes say a file is, for one that is no save: a name for the message, or null.
function kindOf(head, name) {
  if (starts(head, 0x52, 0x61, 0x72, 0x21, 0x1a, 0x07)) return { why: `it is a RAR archive, which this page does not open: ${UNPACK}` };
  if (starts(head, 0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c)) return { why: `it is a 7-Zip archive, which this page does not open: ${UNPACK}` };
  if (starts(head, 0x89, 0x50, 0x4e, 0x47)) return { why: 'it is a picture (PNG), not a saved game' };
  if (starts(head, 0xff, 0xd8, 0xff)) return { why: 'it is a picture (JPEG), not a saved game' };
  if (starts(head, 0x47, 0x49, 0x46, 0x38)) return { why: 'it is a picture (GIF), not a saved game' };
  if (starts(head, 0x25, 0x50, 0x44, 0x46)) return { why: 'it is a PDF document, not a saved game' };
  const text = new TextDecoder('utf-8', { fatal: false }).decode(head.subarray(0, 512));
  if (/^\s*(<!doctype html|<html|<head|<body)/i.test(text)) {
    return { why: 'it is a web page, not a save: the site\'s download link gave its page instead of the file. Open the link, let the save itself download, and upload that' };
  }
  if (SETS.has(base(name).toLowerCase()) || /\.dat$/i.test(name)) {
    return { why: 'it is a file of a data set (the game\'s database), not a saved game: upload the data set\'s .zip, or all its .dat files together, under Data sets below' };
  }
  const plain = head.length > 0 && head.subarray(0, 512).every((c) => c === 9 || c === 10 || c === 13 || (c >= 0x20 && c !== 0x7f) || c >= 0x80);
  if (plain && /[a-z]{3}/i.test(text)) return { why: 'it is a text file, not a saved game' };
  return null;
}

// The upload sorted: { saves: [{ name, size, blob() }], tactics: [{ name, bytes }], notes: [text], why: text or null }.
// why: why nothing in it was taken, with what to do.
export async function sortUpload(file, depth = 0) {
  const out = { saves: [], tactics: [], notes: [], why: null };
  const head = new Uint8Array(await file.slice(0, Math.max(SAVE_HEADER, 4096)).arrayBuffer());
  if (file.size === 0) { out.why = checkSave(head, 0); return out; }

  if (starts(head, 0x50, 0x4b, 0x03, 0x04) || starts(head, 0x50, 0x4b, 0x05, 0x06)) {           // a zip
    if (!/\.zip$/i.test(file.name)) out.notes.push(`${file.name} is a zip: the saves in it were taken`);
    let entries;
    try { entries = await zipBlobEntries(file); } catch (e) { out.why = e.message; return out; }
    const files = entries.filter((e) => !e.name.endsWith('/'));
    for (const e of files) {
      if (/\.sav$/i.test(e.name) && !/(^|\/)backups\//i.test(e.name)) out.saves.push({ name: base(e.name), size: e.size, blob: () => zipBlobEntry(file, e) });
      else if (/\.(tct|pct)$/i.test(e.name) && e.size < 65536) {
        const t = readyTactic(new Uint8Array(await (await zipBlobEntry(file, e)).arrayBuffer()));
        if (!t.why) out.tactics.push({ name: base(e.name).replace(/\.pct$/i, '.tct'), bytes: t.bytes });
      }
    }
    const setFiles = files.filter((e) => SETS.has(base(e.name).toLowerCase())).length;
    if (setFiles >= 3 && out.saves.length) out.notes.push(`${file.name} also holds a data set: upload it under Data sets below to play with it`);
    if (!out.saves.length && !out.tactics.length) {
      out.why = setFiles >= 3
        ? 'it holds a data set (the game\'s database), not a saved game: upload it under Data sets below'
        : `it holds no saved game: ${files.slice(0, 5).map((e) => base(e.name)).join(', ')}${files.length > 5 ? ` and ${files.length - 5} more` : ''}`;
    }
    return out;
  }

  if (starts(head, 0x1f, 0x8b) && depth === 0) {                                   // a gzip: unpacked, then sorted again
    let inner;
    try { inner = await new Response(file.stream().pipeThrough(new DecompressionStream('gzip'))).blob(); }
    catch { out.why = 'it is packed (gzip) and does not unpack: it is damaged'; return out; }
    const name = file.name.replace(/\.gz$/i, '') || 'uploaded save.sav';
    const sorted = await sortUpload(new File([inner], name), 1);
    sorted.notes.unshift(`${file.name} was packed (gzip): unpacked`);
    return sorted;
  }

  if (!checkSave(head, file.size)) {                                                // a save, whatever its name
    const name = saveFileName(file.name);
    if (name !== base(file.name)) out.notes.push(`${file.name} is a saved game: it is stored as ${name}`);
    out.saves.push({ name, size: file.size, blob: async () => file });
    return out;
  }

  if (file.size <= 65536) {                                                         // a tactic
    const t = readyTactic(new Uint8Array(await file.arrayBuffer()));
    if (!t.why) { out.tactics.push({ name: base(file.name).replace(/\.(pct|tct)?$/i, '.tct'), bytes: t.bytes }); return out; }
  }

  out.why = kindOf(head, file.name)?.why ?? checkSave(head, file.size);
  return out;
}
