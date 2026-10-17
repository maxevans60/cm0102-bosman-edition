"""Player career history of Football Manager 2005 and 2007 (pl_hist_dt/id/index.dat), keyed by people_all index.

Usage as a module:
    careers(db_folder, people_all, clubs=None) -> {people_all index: [dict(season, club, apps, goals, loan, ...)]}
Usage as a script:
    python3 -I careers.py <db folder> <people_all.pkl> <clubs.pkl> <out.pkl>

Pure reader. Reads the files as data only. read_fm2005.py and read_fm2007.py call it and write careers.pkl.
Evidence: Henry, Rooney, Lampard, Gerrard, Terry, van Nistelrooy and Ronaldinho have their real league apps and
goals for 2002/03 and 2003/04 (and 2004/05, 2005/06 in FM2007); the loan flag is set on known loans (Lampard at
Swansea, Defoe at Bournemouth).

Files (little-endian, each starts with the 6 bytes 02 01 't' 'a' 'd' '.' and a u16 file version):
- pl_hist_index.dat: u8 0, u32 n, then n pairs (u32 person unique id, u32 slot). FM2005 repeats a unique id for
  the first 12,065 slots (one season per slot), so all slots of a key belong to the person.
- pl_hist_id.dat: u8 0, then 2 x i32 per slot: offset of the first and of the last row in pl_hist_dt.dat.
- pl_hist_dt.dat: u8 0, then rows. Offsets count from byte 9. Each row ends with the i32 offset of the next row of
  the same person (-1 = last row).
  FM2005 (17 bytes): u16 season start year, u8 loan, u8 apps, u8 goals, i32 fee (-1 none), u32 club unique id, i32 next.
  FM2007 (21 bytes): the same, plus u32 competition unique id (-1 none) before next. Apps and goals 255 = unknown.
- The person's unique id sits in the person record in people_db.dat directly after the person id (the contract
  person id `pid` of people_all): "... u32 pid, u32 unique id ...". For people without a contract the first such
  id block of the record is used (see _uid_2005 and _uid_2007).
"""
import os
import pickle
import re
import struct
import sys
from collections import defaultdict

MAGIC = b"\x02\x01tad."


def _read(path):
    with open(path, "rb") as f:
        d = f.read()
    if d[:6] != MAGIC:
        raise ValueError("not a tad file: %s" % path)
    return d


def history_by_uid(db):
    """{person unique id: [row tuples]} with rows (season, loan, apps, goals, fee, club_uid, comp_uid, dt offset)."""
    ix = _read(os.path.join(db, "pl_hist_index.dat"))
    idf = _read(os.path.join(db, "pl_hist_id.dat"))
    dt = _read(os.path.join(db, "pl_hist_dt.dat"))
    version = struct.unpack_from("<H", ix, 6)[0]
    size = {0xF0: 17, 0x42C: 21}.get(version)
    if size is None:
        raise ValueError("unknown history file version 0x%x" % version)
    if (len(dt) - 9) % size:
        raise ValueError("pl_hist_dt.dat length does not fit %d-byte rows" % size)
    n = struct.unpack_from("<I", ix, 9)[0]
    if 13 + 8 * n != len(ix) or 9 + 8 * n != len(idf):
        raise ValueError("index and id files do not agree")
    ids = struct.unpack_from("<%di" % (2 * n), idf, 9)
    fmt = "<HBBBiIi" if size == 17 else "<HBBBiIii"
    out = defaultdict(list)
    for i in range(n):
        uid, slot = struct.unpack_from("<II", ix, 13 + 8 * i)
        o, seen = ids[2 * slot], 0
        while o != -1:
            r = struct.unpack_from(fmt, dt, 9 + o)
            comp = r[6] if size == 21 else -1
            out[uid].append((r[0], r[1], r[2], r[3], r[4], r[5], comp, o))
            o = r[-1]
            seen += 1
            if seen > 200:
                raise ValueError("history chain loop at slot %d" % slot)
    for rows in out.values():
        rows.sort(key=lambda r: (r[0], r[7]))
    return dict(out), size


# FM2005 id block: a date (day 151/152 of a year, or the empty date 00 00 6c 07), a 0 byte, then u32 person id,
# u32 unique id. The contract block (u32 pid, 0, u32 club, u32 wage, date, 02) and the joined date at +43 also
# follow a date, so they are skipped. 99.8 % of people with a pid have it in their first id block.
_ID05 = re.compile(rb"(?:\x00\x00\x6c\x07|[\x97\x98]\x00[\xa0-\xd6]\x07)\x00", re.S)
# FM2007 id block: ff, the empty date, two flag bytes (00/08, 00/40), 00 00, then u32 person id, u32 unique id.
_ID07 = re.compile(rb"\xff\x00\x00\x6c\x07[\x00\x08][\x00\x40]\x00\x00", re.S)


def _blocks_2005(p, o, e):
    for m in _ID05.finditer(p, o + 40, min(e + 8, len(p) - 18)):
        k = m.end()
        pid, uid = struct.unpack_from("<II", p, k)
        if (p[k + 4] == 0 and p[k + 17] == 2) or pid >= 400000 or k - o == 48 or p[k:k + 2] == b"\x02\x19":
            continue
        yield pid, uid


def _blocks_2007(p, o, e):
    for m in _ID07.finditer(p, o, min(e + 8, len(p) - 8)):
        yield struct.unpack_from("<II", p, m.end())


def person_uids(db, people_all, size):
    """{people_all index: unique id} from people_db.dat."""
    with open(os.path.join(db, "people_db.dat"), "rb") as f:
        p = f.read()
    blocks = _blocks_2005 if size == 17 else _blocks_2007
    out = {}
    for n, x in enumerate(people_all):
        o = x["off"]
        e = people_all[n + 1]["off"] if n + 1 < len(people_all) else len(p)
        found = list(blocks(p, o, e))
        if not found:
            continue
        if x.get("pid") is not None:
            hit = [b for b in found if b[0] == x["pid"]]
            if hit:
                out[n] = hit[0][1]
        else:
            out[n] = found[0][1]
    return out


def careers(db_folder, people_all, clubs=None):
    """{people_all index: [dict(season, club, apps, goals, loan, fee, club_uid, comp_uid)]}.

    season = start year of the season (2003 = 2003/04). club = FM club id (key of clubs.pkl) when `clubs` (the
    facts clubs dict or a path to clubs.pkl) is given, else None. apps/goals None when the file has 255. loan = bool.
    people_all may be the list or a path to people_all.pkl. A unique id claimed by two people without a pid
    match is dropped for both.
    """
    if isinstance(people_all, str):
        with open(people_all, "rb") as f:
            people_all = pickle.load(f)
    if isinstance(clubs, str):
        with open(clubs, "rb") as f:
            clubs = pickle.load(f)
    u2c = {v["id2"]: k for k, v in clubs.items()} if clubs else {}
    hist, size = history_by_uid(db_folder)
    link = person_uids(db_folder, people_all, size)
    owners = defaultdict(list)
    for n, u in link.items():
        if u in hist:
            owners[u].append(n)
    out = {}
    for u, ns in owners.items():
        if len(ns) > 1:
            ns = [n for n in ns if people_all[n].get("pid") is not None]
            if len(ns) != 1:
                continue
        out[ns[0]] = [dict(season=r[0], club=u2c.get(r[5]), apps=None if r[2] == 255 else r[2],
                           goals=None if r[3] == 255 else r[3], loan=bool(r[1]), fee=None if r[4] == -1 else r[4],
                           club_uid=r[5], comp_uid=None if r[6] == -1 else r[6])
                      for r in hist[u]]
    return out


def main(argv):
    if len(argv) != 5:
        sys.exit(__doc__)
    res = careers(argv[1], argv[2], argv[3])
    with open(argv[4], "wb") as f:
        pickle.dump(res, f)
    print("people with careers: %d, rows: %d" % (len(res), sum(len(v) for v in res.values())))


if __name__ == "__main__":
    main(sys.argv)
