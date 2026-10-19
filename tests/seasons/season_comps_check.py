#!/usr/bin/env python3
"""New competitions module (0.6.0, season_comps.h): a new competition record and a new object across a save and a
load. It uses the test def of season_comps.c (RECOMP_SEASON_COMPS_TEST=1): a record "Test New Cup" that the data
lacks, with an object of the Inter-American Cup class in entry 2, made in the seasons of the Intercontinental Cup.

Usage:
    python3 tests/seasons/season_comps_check.py <cm0102_recomp> <game folder> [--only <name>,...] [--keep <folder>]

The runs (the 3.9.68 data from 2001/02, England, changing_rules on):
    jump   to 2002/03. The data load of the new game adds one record after the 19 nation comps (index 452 in the
           3.9.68 data), the season change of 2002 makes the object, and the rules report of 2002 holds it with that
           record and the table's name of the season (the module renames it, season_comps.h)
    load   the game saves 400 days after the start (August 2002, after the object is made); a new program loads that
           save and plays 10 days: the module makes the object again from its saved file, and the record is in the
           loaded data, so no new record is added at the load
    off    the jump to 2002/03 without the test variable: no new record and no object
A run takes about 5 to 8 minutes.
"""
import os
import re
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402
import club_world_check as cwc  # noqa: E402

TEST = {"RECOMP_SEASON_COMPS_TEST": "1"}
NAME = "Test New Cup"
COMP = "intercontinental_cup"


def jump(program, game, work, env):
    code, report = rrc.run(program, game, 2002, 2001, None, ["England"], [], work, 7200,
                           options_more={"changing_rules": "on"}, env_more=env)
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    return code, report, log


def check_jump(program, game, work):
    code, report, log = jump(program, game, work, TEST)
    fails = [] if code == 0 else [f"jump: code {code} ({work})"]
    m = re.search(r"new competitions: (\d+) new records from index (\d+)", log)
    if not m or m.group(1) != "1":
        fails.append(f"jump: no new record at the data load ({work}/stderr.txt)")
    index = int(m.group(2)) if m else -1
    if "new competition test_new_cup made for 2002" not in log:
        fails.append("jump: the object was not made in the season change of 2002")
    found = []
    if os.path.exists(report):
        _, comps = rrc.read_report(report)
        found = sorted({(c["season"], c["record"], c["name"]) for c in comps if c["record"] == index})
    print(f"  jump: record index {index}, report lines {found}")
    # The module names the competition with the table's name of the season (the rows of the Intercontinental Cup).
    want = next(r["name"] for r in rrc.make_rules.read_rows() if r["season"] == 2002 and r["comp"] == COMP)
    if (2002, index, want) not in found:
        fails.append(f"jump: the rules report of 2002 does not hold record {index} named {want!r}")
    if "message box" in log:
        fails.append(f"jump: a message box ({work}/stderr.txt)")
    return fails


def check_load(program, game, work):
    code, _ = rrc.run(program, game, 2002, 2001, None, ["England"], [], work, 7200,
                      options_more={"changing_rules": "on"}, env_more=dict(TEST, RECOMP_SAVE_AFTER_DAYS="400"))
    first = os.path.join(work, "user", "samesave.sav")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    if code != 0 or not os.path.exists(first):
        return [f"load: the first run made no save (code {code}, {work})"]
    if "new competition test_new_cup made for 2002" not in log:
        return [f"load: the object was not made before the save ({work}/stderr.txt)"]
    again = os.path.join(work, "again")
    os.makedirs(again, exist_ok=True)
    shutil.copyfile(first, os.path.join(again, "comps.sav"))
    os.environ.update(TEST)                                  # fixtures.run takes the environment of this process
    try:
        r, saved = cwc.load_and_play(program, game, os.path.join(again, "comps.sav"), again, 10)
    finally:
        for k in TEST:                                       # the off run after this one must not have it
            os.environ.pop(k, None)
    fails = []
    made = re.findall(r"new competition test_new_cup made for (\d+) \(entry 2, record (\d+)\) from the save", r.stderr)
    print(f"  load: made again {made}")
    if r.returncode != 0 or not saved:
        fails.append(f"load: the save did not load and play on (code {r.returncode}, {again})")
    if not made:
        fails.append(f"load: the object was not made again from the save ({again}/stderr.txt)")
    if "new records from index" in r.stderr:
        fails.append("load: the load added a record (the save holds it)")
    if "message box" in r.stderr:
        fails.append(f"load: a message box ({again}/stderr.txt)")
    return fails


def check_off(program, game, work):
    code, report, log = jump(program, game, work, {})
    fails = [] if code == 0 else [f"off: code {code} ({work})"]
    if "new records from index" in log or "new competition test_new_cup" in log:
        fails.append(f"off: a record or an object without the test variable ({work}/stderr.txt)")
    return fails


RUNS = {"jump": check_jump, "load": check_load, "off": check_off}


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only")
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else tempfile.mkdtemp(prefix="cm0102-comps-")
    failed = []
    for name in (only.split(",") if only else list(RUNS)):
        failed += RUNS[name](program, game, os.path.join(keep, name))
    for text in failed:
        print("FAIL  " + text)
    print("PASS" if not failed else f"FAILED: {len(failed)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
