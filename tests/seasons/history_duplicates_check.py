#!/usr/bin/env python3
"""Lists the history rows that each database holds twice (0.6.0, the duplicates of src/recomp_rt/win/season_history.h).

Usage:
    python3 tests/seasons/history_duplicates_check.py <folder of databases> [--rows]

For each database folder with club_comp_history.dat: the rows (26 bytes: id, competition id, label year, four
places) whose competition, label year and four places another row of the same file holds too. --rows prints
each such row of the English Premier Division. The game clears the second and later copy at the load of a season
database (season_history.c); tests/seasons/season_history_run.py --duplicates checks that no copy is left in the
game. Exit code 0: this only lists.
"""
import collections
import os
import struct
import sys


def scan(folder):
    out = {}
    for part in ("club_comp_history.dat", "nation_comp_history.dat"):
        path = os.path.join(folder, part)
        if not os.path.exists(path):
            continue
        data = open(path, "rb").read()
        seen = collections.Counter(struct.unpack_from("<iHiiii", data, i * 26 + 4) for i in range(len(data) // 26))
        out[part] = (len(data) // 26, {k: n for k, n in seen.items() if n > 1 and k[0] >= 0})
    return out


def premier_id(folder):
    data = open(os.path.join(folder, "club_comp.dat"), "rb").read()
    for i in range(len(data) // 0x6b):
        r = data[i * 0x6b:(i + 1) * 0x6b]
        if r[4:55].split(b"\0")[0] == b"English Premier Division":
            return struct.unpack_from("<i", r, 0)[0]
    return None


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    root = argv[1]
    for name in sorted(os.listdir(root)):
        folder = os.path.join(root, name)
        if not os.path.exists(os.path.join(folder, "club_comp_history.dat")):
            continue
        parts = scan(folder)
        line = ", ".join(f"{p.split('_')[0]} {rows} rows, {sum(n - 1 for n in d.values())} copies"
                         for p, (rows, d) in parts.items())
        print(f"{name}: {line}")
        if "--rows" in argv:
            pid = premier_id(folder)
            for k, n in sorted(parts.get("club_comp_history.dat", (0, {}))[1].items()):
                if k[0] == pid:
                    print(f"    English Premier Division {k[1]}: {n} times")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
