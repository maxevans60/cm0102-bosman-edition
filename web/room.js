// Room for a save while the game writes it (web/README.md, "Where the saves live"). The game writes a save as
// temp_save_file.sav and renames it when it is done: it opens the file about once per member, reads its header, adds the
// member at the end and writes the header again (a native run under strace, 16 November 2026: 138 opens for a save of
// 46 MB, every read within the first 270,336 bytes). Emscripten's memory file system grows a file by 1.125 times each
// time a write goes past its room, copying the whole file each time: a save of 690 MB was copied about fifty times, and
// the tab went from 0.9 GB to 3.8 GB while the game wrote it. So the file gets room at once for the largest save of the
// user folder (5% more and a MB); past that, or with no save in the folder, it grows as before. Room that is never
// written takes no memory of the computer, only of the address space. Measured (16 November 2026, a save of 690 MB
// over one of that size): the tab's peak while writing 2.2 GB instead of 3.8, the write 0.4 s instead of 6.3.
const TEMP = /\/temp_save_file\.sav$/i;

export function reserveSaveRoom(FS, dir = '/user') {
  const open = FS.open;
  if (typeof open !== 'function') return;
  // The largest save of the folder, with some more: what this save is likely to need.
  const want = () => {
    let most = 0;
    let names = [];
    try { names = FS.readdir(dir); } catch { return 0; }
    for (const n of names) {
      if (!/\.sav$/i.test(n) || TEMP.test(`/${n}`)) continue;
      try { most = Math.max(most, FS.stat(`${dir}/${n}`).size); } catch { /* gone */ }
    }
    return most ? Math.ceil(most * 1.05) + (1 << 20) : 0;
  };
  const reserve = (node, need) => {
    const have = node.contents?.length ?? 0, want = node.saveRoom.want;
    if (need <= have || need > want || !(node.contents === null || node.contents === undefined || node.contents instanceof Uint8Array)) return;
    const grown = new Uint8Array(want);
    if (node.usedBytes && node.contents) grown.set(node.contents.subarray(0, node.usedBytes));
    node.contents = grown;
  };
  FS.open = (path, flags, mode) => {
    const stream = open(path, flags, mode);
    const node = stream?.node;
    if (typeof path === 'string' && TEMP.test(path) && node && typeof node.stream_ops?.write === 'function') {
      if (!node.saveRoom) {
        const ops = node.stream_ops;
        node.saveRoom = { want: want() };
        node.stream_ops = { ...ops, write(st, buffer, offset, length, position, canOwn) { reserve(node, position + length); return ops.write(st, buffer, offset, length, position, canOwn); } };
      }
      stream.stream_ops = node.stream_ops;            // this open too: a stream takes its node's operations when it opens
    }
    return stream;
  };
}
