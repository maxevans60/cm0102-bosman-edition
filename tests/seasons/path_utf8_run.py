#!/usr/bin/env python3
"""A new game with every nation saves (0.6.0 slice 15: file names that are not UTF-8).

Usage:
    python3 tests/seasons/path_utf8_run.py <cm0102_recomp> <game folder> [--keep <folder>]

A league writes its temp file as "<long name>_<id>.tmp" (League::save_to_temp_file 0x6891c0, fopen at 0x689282).
Some names of the 3.9.68 data hold a code page 1252 byte (the Brazilian state championships: São Paulo,
Gaúcho, Goiás, Paraná). macOS refuses such a file name, and before the fix a Select All 3.9.68 new game stopped
at its first save with the box "league..cpp 5095". win_host_path (src/recomp_rt/win/files.c) now turns such a
part into UTF-8. This is a fix of the base game: it is not under the season switch.

The run: a base game (2001/02, 3.9.68 data, changing_rules off) with Select All, the season jump of
tests/seasons/rules_report_check.py to the end of 2001/02, and a save 3 days after the start
(RECOMP_SAVE_AFTER_DAYS). The checks: the jump ends with code 0, the save exists, and it holds the temp file
parts of the four state championships (ids 67, 254, 260 and 266), with UTF-8 names.
A run takes about 10 to 20 minutes.
"""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "..", "tools", "save"))
import rules_report_check as rrc  # noqa: E402
import savfile  # noqa: E402

PARTS = (67, 254, 260, 266)


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    keep = os.path.abspath(argv[argv.index("--keep") + 1]) if "--keep" in argv else None
    work = keep or tempfile.mkdtemp(prefix="cm0102-path-utf8-")
    code, _ = rrc.run(program, game, 2001, None, None, ["all"], [], work, 3600, env_more={"RECOMP_SAVE_AFTER_DAYS": "3"})
    save = os.path.join(work, "user", "samesave.sav")
    fails = []
    if code != 0:
        fails.append(f"the run ended with code {code} ({work})")
    if not os.path.exists(save):
        fails.append(f"the game made no save ({work}/stderr.txt)")
    else:
        names = [m.name for m in savfile.Sav(save).members]
        for part in PARTS:
            hits = [n for n in names if n.endswith(f"_{part}.tmp")]
            if not hits:
                fails.append(f"the save has no part _{part}.tmp")
            else:
                try:
                    name = hits[0].encode("latin-1").decode("utf-8")
                    print(f"ok    part {name!r}")
                except UnicodeDecodeError:
                    fails.append(f"the part {hits[0]!r} has a name that is not UTF-8")
    for f in fails:
        print("FAIL  " + f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
