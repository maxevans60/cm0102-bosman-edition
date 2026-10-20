#!/usr/bin/env python3
"""The new competitions module (src/recomp_rt/win/season_comps.c) makes at most MAX_NEW new records in a game. Every
SeasonCompDef with `like` (its sixth field, a record that the data lacks) may need one. This check fails when the defs
with `like` are more than MAX_NEW, so that a later slice raises the limit before a def is dropped.

Usage: python3 tests/seasons/comps_new_records_check.py <source folder src/recomp_rt/win>
"""
import glob
import os
import re
import sys


def defs_with_like(folder):
    found = []
    for path in sorted(glob.glob(os.path.join(folder, "*.c"))):
        text = open(path, encoding="utf-8", errors="replace").read()
        for m in re.finditer(r"const SeasonCompDef (\w+) = \{(.*?)\};", text, re.S):
            fields = [f.strip() for f in re.split(r",(?![^(]*\))", m.group(2))]
            if len(fields) > 5 and fields[5] not in ("NULL", "0"):
                found.append(f"{os.path.basename(path)}:{m.group(1)}")
    return found


def main(argv):
    if len(argv) != 2:
        print(__doc__)
        return 2
    folder = argv[1]
    text = open(os.path.join(folder, "season_comps.c"), encoding="utf-8").read()
    m = re.search(r"#define MAX_NEW (\d+)", text)
    if not m:
        print("FAIL: no MAX_NEW in season_comps.c")
        return 1
    limit, found = int(m.group(1)), defs_with_like(folder)
    if len(found) < 10:
        print(f"FAIL: only {len(found)} defs with like found; the pattern no longer matches the sources")
        return 1
    print(f"{len(found)} defs with like, MAX_NEW {limit}: {', '.join(found)}")
    if len(found) > limit:
        print(f"FAIL: {len(found)} defs may need a new record, but MAX_NEW is {limit}: raise it")
        return 1
    print("PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
