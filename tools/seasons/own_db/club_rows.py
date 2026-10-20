"""Rows of the rules club tables (docs/research/rules-0.6.0/<part>-clubs.tsv) for an own season database: every
club of each real list of the season settings, with its record id, its name and its division in the new database.
Slice 18 uses them to give each division its real size (make_clubs.py reads the part files).

Usage: python3 -I club_rows.py <season start year> <template folder> <work folder> [--write]

The work folder holds leagues.pkl (leagues.py: realrows, the real name and record id of each listed club, and cur,
the club's division in the new database). Without --write the tool prints the rows. With --write it puts them
into the part files, after the rows of the seasons before. Rows of the season with a label that the run writes
are replaced (a rebuild), other rows stay; a part file that does not exist is made. A list goes to a part file only when the season settings' PARTS name its part and label (the labels
that make_clubs.py TARGETS maps to a database division); the rest are printed as left out.
"""
import collections
import csv
import os
import pickle
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from cmdb import Folder, cstr  # noqa: E402
from seasons import RULES, repair_clubs, settings  # noqa: E402

COLUMNS = ["season", "division", "club", "db_club_id", "db_club", "db_division", "source"]


def sources(year):
    """Template division name -> the URL in the comment of its REAL line."""
    out = {}
    with open(os.path.join(HERE, f"season_{year}.py"), encoding="utf-8") as f:
        text = f.read()
    real = text[text.index("REAL = {"):text.index("PROMOTED = {")] if "PROMOTED = {" in text else text
    for line in real.splitlines():
        m = re.match(r"\s*['\"](.+?)['\"]: \[.*#.*?(https://\S+)", line)
        if m:
            out[m.group(1)] = m.group(2)
    return out


def main(argv):
    if len(argv) not in (3, 4):
        sys.exit(__doc__)
    year = int(argv[0])
    S = settings(year)
    T = Folder(argv[1])
    repair_clubs(T, S)
    L = pickle.load(open(os.path.join(argv[2], "leagues.pkl"), "rb"))
    write = "--write" in argv
    comps = {struct.unpack_from("<i", r, 0)[0]: cstr(r, 4, 51) for r in T.rows(12)}
    clubs = {struct.unpack_from("<i", r, 0)[0]: cstr(r, 4, 51) for r in T.rows(0)}
    src = sources(year)
    season = f"{year}/{(year + 1) % 100:02d}"
    by_part, left = collections.defaultdict(list), collections.Counter()
    for dname, name, c in L["realrows"]:
        part = S.PARTS.get(dname)
        if part is None:
            left[dname] += 1
            continue
        div = comps.get(L["cur"].get(c, -1), "") if c is not None else ""
        by_part[part[0]].append([season, part[1], name, "" if c is None else str(c),
                                 clubs.get(c, "") if c is not None else "", div, src.get(dname, "")])
    for p, rows in sorted(by_part.items()):
        print(f"{p}: {len(rows)} rows")
    print("left out (no part file target):", dict(left))
    if not write:
        for p, rows in sorted(by_part.items()):
            for r in rows:
                print(p, *r, sep="\t")
        return
    for p, rows in sorted(by_part.items()):
        path = os.path.join(RULES, f"{p}-clubs.tsv")
        if os.path.exists(path):
            with open(path, newline="", encoding="utf-8") as f:
                lines = list(csv.reader(f, delimiter="\t", quoting=csv.QUOTE_NONE))
        else:
            lines = [COLUMNS]
        if lines[0] != COLUMNS:
            sys.exit(f"club_rows: {path}: not a clubs file")
        labels = {r[1] for r in rows}
        old = [r for r in lines[1:] if r and r[0] == season and r[1] in labels]
        lines = [lines[0]] + [r for r in lines[1:] if not (r and r[0] == season and r[1] in labels)]
        if old:
            print(f"{path}: {len(old)} rows of {season} replaced")
        at = next((i for i, r in enumerate(lines[1:], 1) if r and r[0] > season), len(lines))
        lines[at:at] = rows
        text = "".join("\t".join(r) + "\n" for r in lines)   # as read: plain tabs, no quoting
        with open(path + ".tmp", "w", newline="", encoding="utf-8") as f:
            f.write(text)
        os.replace(path + ".tmp", path)
        print("wrote", path)


if __name__ == "__main__":
    main(sys.argv[1:])
