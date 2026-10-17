#!/usr/bin/env python3
"""Lists the gaps in the competition history of the season databases before 2010/11 (0.6.0 slice 11c, Q347).

Usage:
    python3 -I tools/seasons/history_gaps.py <3.9.68 Data folder> <folder of database folders> [--write]

For each database whose season starts before 2010 (and the 3.9.68 data on the 2002/03 tile), the tool reads the
club and nation competition history (club_comp_history.dat, nation_comp_history.dat), applies the year shift of
slice 11b (docs/research/rules-0.6.0/databases-date-shifts.tsv and databases-date-shifts-last-rows.tsv), and
lists for each competition with history rows the label years that it holds and the label years from 2002 to the
start year that it does not hold.

A label year is the year that the history row holds (+0x08). Most competitions use the year where the season
ends (3.9.68: English Premier Division 2002 Arsenal = 2001/02), calendar-year competitions the calendar year
(3.9.68: Swedish Premier Division 2001 Hammarby = the 2001 season). The research table
docs/research/history-0.6.0/ gives the convention of each competition.

Record (26 bytes, read by 0x550230, linked by the id pass at 0x532c11): +0x00 row id, +0x04 competition id (the
nation competitions follow the club competitions, base = count of club_comp.dat), +0x08 u16 year, +0x0a first,
+0x0e second, +0x12 third, +0x16 host (club or national side ids, -1 for none). The game writes a new row in
0x4afce0 with the current year [0xae2c92] and the first three places and the host.

--write writes docs/research/history-0.6.0/gaps.tsv (one row per database and competition) and targets.tsv
(one row per competition: the label years to research, over all databases). The files hold competition names
and the last rows only. A database is named by its content (databases-signatures.tsv), never by its folder.
"""
import collections
import csv
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
RULES = os.path.join(ROOT, "docs", "research", "rules-0.6.0")
SIGS = os.path.join(RULES, "databases-signatures.tsv")
SHIFTS = os.path.join(RULES, "databases-date-shifts.tsv")
LAST = os.path.join(RULES, "databases-date-shifts-last-rows.tsv")
OUT = os.path.join(ROOT, "docs", "research", "history-0.6.0")

SIZES = {0: 0x245, 1: 0x245, 4: 0x122, 12: 0x6b, 16: 0x6b, 19: 0x1a, 20: 0x1a}
FIRST_LABEL = 2002          # the 2001/02 season
LAST_ROW = 2001             # the row where the databases before 2010/11 keep the last champions (slice 11b)
NATION_AT, NAME_AT = 0x5d, 0x04   # club_comp.dat: +0x5d nation id (linked at 0x532d2d), +0x04 name


def s32(b, o):
    return struct.unpack_from("<i", b, o)[0]


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


class Data:
    def __init__(self, folder):
        idx = open(os.path.join(folder, "index.dat"), "rb").read()
        files, self.t = {}, {}
        for e in range(22):
            o = 8 + e * 0x43
            name = idx[o:o + 51].split(b"\0")[0].decode("latin-1")
            tid, cnt, off, fmt = struct.unpack_from("<iiii", idx, o + 0x33)
            if tid not in SIZES:
                continue
            if name not in files:
                files[name] = open(os.path.join(folder, name), "rb").read()
            self.t[tid] = (files[name][off:off + cnt * SIZES[tid]], cnt, SIZES[tid])

    def count(self, tid):
        return self.t.get(tid, (b"", 0, 1))[1]

    def row(self, tid, i):
        buf, cnt, rs = self.t.get(tid, (b"", 0, 1))
        return buf[i * rs:(i + 1) * rs] if 0 <= i < cnt else None

    def rows(self, tid):
        buf, cnt, rs = self.t.get(tid, (b"", 0, 1))
        for i in range(cnt):
            yield buf[i * rs:(i + 1) * rs]

    def text(self, tid, i, at=4):
        r = self.row(tid, i)
        return r[at:at + 51].split(b"\0")[0].decode("latin-1") if r else ""

    def club(self, i):
        if i < 0:
            return ""
        if i < self.count(0):
            return self.text(0, i)
        return self.text(1, i - self.count(0))

    def comp(self, cid):
        """(kind, name, nation) of a competition id, or None."""
        nc = self.count(12)
        r, kind = (self.row(12, cid), "club") if cid < nc else (self.row(16, cid - nc), "nation")
        if r is None:
            return None
        nat = s32(r, NATION_AT)
        return kind, r[NAME_AT:NAME_AT + 51].split(b"\0")[0].decode("latin-1"), self.text(4, nat) if nat >= 0 else ""

    def history(self):
        """(competition id, year, first, second, third, host) for each row with a valid competition."""
        for tid in (19, 20):
            for r in self.rows(tid):
                cid = s32(r, 4)
                if cid >= 0 and self.comp(cid):
                    yield cid, u16(r, 8), s32(r, 0xa), s32(r, 0xe), s32(r, 0x12), s32(r, 0x16)


def read_tsv(path):
    with open(path, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def sha(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def identify(folder, sigs):
    c, s = os.path.join(folder, "club.dat"), os.path.join(folder, "staff.dat")
    if not (os.path.isfile(c) and os.path.isfile(s)):
        return None
    return sigs.get((os.path.getsize(c), sha(c), os.path.getsize(s), sha(s)))


def databases(dbroot):
    """[(name, start year, folder)] of the databases before 2010/11, in season order."""
    sigs = {(int(r["club_size"]), r["club_sha256"], int(r["staff_size"]), r["staff_sha256"]): r
            for r in read_tsv(SIGS)}
    out = []
    for sub in sorted(os.listdir(dbroot)):
        p = os.path.join(dbroot, sub)
        r = identify(p, sigs) if os.path.isdir(p) else None
        if not r or r["data_season"] in ("none", "n/a") or r["role"] in ("official version", "official base"):
            continue
        start = int(r["data_season"][:4])
        if start < 2010:
            out.append((r["database"], start, p))
    return sorted(out, key=lambda x: (x[1], x[0]))


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    shifts_path, last_path = SHIFTS, LAST
    if "--rules" in argv:     # another folder with the two shift tables of slice 11b
        f = argv[argv.index("--rules") + 1]
        shifts_path, last_path = (os.path.join(f, os.path.basename(p)) for p in (SHIFTS, LAST))
    shifts = {r["database"]: r for r in read_tsv(shifts_path)}
    last = {r["database"]: set(map(int, r["competition_ids"].split())) for r in read_tsv(last_path)}
    dbs = [("3.9.68", 2002, argv[0])] + databases(argv[1])
    gaps = ["\t".join(["database", "start_year", "kind", "comp_id", "competition", "nation", "first_label",
                       "last_label", "labels_held", "labels_missing", "last_row"])]
    targets = collections.OrderedDict()
    for name, start, folder in dbs:
        d = Data(folder)
        sh = shifts.get(name, {})
        comp_shift, last_shift = int(sh.get("comp_history", 0) or 0), int(sh.get("last_rows", 0) or 0)
        moved = last.get(name, set())
        rows = collections.defaultdict(list)
        for cid, y, a, b, c, h in d.history():
            if comp_shift:
                y += comp_shift
            elif y == LAST_ROW and cid in moved:
                y += last_shift
            rows[cid].append((y, a, b, c, h))
        n_missing = 0
        for cid in sorted(rows):
            kind, cname, nation = d.comp(cid)
            ys = sorted(rows[cid])
            held = sorted({y for y, *_ in ys if FIRST_LABEL <= y <= start})
            missing = [y for y in range(FIRST_LABEL, start + 1) if y not in held]
            n_missing += len(missing)
            y, a, b, c, h = ys[-1]
            lastrow = f"{y} {d.club(a)} / {d.club(b)}" + (f" / {d.club(c)}" if c >= 0 else "")
            gaps.append("\t".join(map(str, [name, start, kind, cid, cname, nation, ys[0][0], ys[-1][0],
                                             " ".join(map(str, held)), " ".join(map(str, missing)), lastrow])))
            t = targets.setdefault((kind, cname, nation), {"dbs": [], "labels": set(), "last": 0})
            t["dbs"].append(name)
            t["labels"] |= set(range(FIRST_LABEL, start + 1))
            t["last"] = max(t["last"], max(y for y, *_ in ys if y <= LAST_ROW) if any(
                y <= LAST_ROW for y, *_ in ys) else 0)
        print(f"{name}\tstart {start}\t{len(rows)} competitions with history\t{n_missing} label years missing",
              flush=True)
    tl = ["\t".join(["kind", "competition", "nation", "last_label_before_2002", "labels_to_research", "databases"])]
    for (kind, cname, nation), t in sorted(targets.items(), key=lambda x: (x[0][0], x[0][2], x[0][1])):
        lab = sorted(t["labels"])
        tl.append("\t".join([kind, cname, nation, str(t["last"]), f"{lab[0]}-{lab[-1]}", ", ".join(t["dbs"])]))
    print(f"{len(targets)} competitions in all")
    if "--write" in argv:
        os.makedirs(OUT, exist_ok=True)
        for fn, lines in (("gaps.tsv", gaps), ("targets.tsv", tl)):
            with open(os.path.join(OUT, fn), "w", encoding="utf-8") as f:
                f.write("\n".join(lines) + "\n")
        print("wrote gaps.tsv and targets.tsv in", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
