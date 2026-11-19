#!/usr/bin/env python3
"""A club in two key ids keeps both after the trim of the club table (open item 295, original-faults group club_keys).

Usage:
    python3 tests/seasons/club_key_twins_run.py <cm0102_recomp> <game folder> <out folder> [--keep-faults]

A new game on the October 2026 database, one day of holiday. remap_club_key_id 0x60ba80 rewrites only the first key id
of a row, so the second key ids of C.D. Logrones, C.D. Logrones B, C.D. Badajoz and C.D. Badajoz B kept their old rows
after database_trim_club_table 0x5267c0.

The checks, on the log of RECOMP_SEASON_LOG:
    - each of the four second key ids follows its row
    - --keep-faults (RECOMP_ORIGINAL_FAULTS=keep:club_keys): none of them moves, so the test can see the fault
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402
import season_core_run as scr  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases/October 2026/October 2026 Data Update"
NATIONS = ["England", "Italy", "Spain", "France", "Germany", "Holland", "Portugal", "Belgium"]
SECOND_KEYS = {0x9d04c0, 0x9d04c4, 0x9d04c8, 0x9d04cc}


def main(argv):
    args = [a for a in argv[1:] if a != "--keep-faults"]
    if len(args) != 3:
        print(__doc__)
        return 2
    keep = "--keep-faults" in argv
    program, game, out = (os.path.abspath(a) for a in args)
    os.makedirs(out, exist_ok=True)
    env = {"RECOMP_START_SEASON": "2026"}
    if keep:
        env["RECOMP_ORIGINAL_FAULTS"] = "keep:club_keys"
    save, log, _ = scr.play(program, game, out, "new", {"changing_rules": "on"}, rrc.league_steps(NATIONS, []) + ["900000 quit"],
                            1, env, database=DB, timeout=1200)
    moved = {int(k, 16) for k in re.findall(r"recomp: club key (0x[0-9a-f]+) follows row", log)} & SECOND_KEYS
    wanted = set() if keep else SECOND_KEYS
    faults = []
    if not save:
        faults.append("no save")
    if moved != wanted:
        faults.append("second key ids that moved: " + (", ".join(hex(k) for k in sorted(moved)) or "none"))
    for x in faults:
        print("FAIL ", x)
    print("ok" if not faults else f"FAIL  {len(faults)} checks")
    return 1 if faults else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
