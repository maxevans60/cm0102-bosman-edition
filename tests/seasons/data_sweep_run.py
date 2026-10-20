#!/usr/bin/env python3
"""Data sweep of the competitions (0.6.0 slice 30a, check 4 in data form; docs/research/brief-0.6.0/slice-30a.md):
a season jump with RECOMP_DATA_SWEEP=1, which checks at each start and end report every competition object and its
stages for round dates outside the edition and for a drawn league with an empty table or row
(src/recomp_rt/win/season_report.c, "RECOMP_DATA_SWEEP").

Usage:
    python3 tests/seasons/data_sweep_run.py <cm0102_recomp> <game folder> --keep <folder> [--year 2001]
        [--last <year>] [--database <folder>] [--nations England|all|England,Italy,...] [--changing-rules on|off]
        [--timeout 7200]

The run is tests/seasons/rules_report_check.py's season jump (England by default) from --year to the end of
--last (default: the season of --year). --database names a database folder (default: the game's own data, a
2001/02 start).

The checks:
    - the jump ends with code 0, and its log has no message box of the game
    - the report has a "sweepok" line at each start and end report, and the report holds the end of --last
    - no "sweep" line: each one is a fault, printed with its season, phase, file, name and what is wrong
A season of England takes about 5 to 8 minutes; Select All much longer.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

BOX = "recomp: message box from"


def arg(argv, name, default=None):
    return argv[argv.index(name) + 1] if name in argv else default


def check(work, last):
    fails, report = [], os.path.join(work, "report.tsv")
    faults, oks = [], []
    try:
        with open(report, encoding="latin-1") as f:
            for line in f:
                p = line.rstrip("\n").split("\t")
                if p[0] == "sweep":
                    faults.append(p)
                elif p[0] == "sweepok":
                    oks.append(p)
    except OSError:
        return [f"no report ({report})"]
    try:
        with open(os.path.join(work, "stderr.txt"), encoding="latin-1") as f:
            boxes = [line.strip() for line in f if line.startswith(BOX)]
    except OSError:
        boxes = []
    for b in boxes[:5]:
        fails.append(f"box: {b}")
    seasons, _ = rrc.read_report(report)
    if not seasons or seasons[-1]["season"] != last or seasons[-1]["phase"] != "end":
        fails.append(f"the report does not end with the end of {last}/{(last + 1) % 100:02d}")
    reports = [(s["season"], s["phase"]) for s in seasons]
    swept = {(int(p[1]), p[2]) for p in oks}
    for s in reports:
        if s not in swept:
            fails.append(f"no sweepok line for {s[0]}/{(s[0] + 1) % 100:02d} {s[1]} (RECOMP_DATA_SWEEP not in the build?)")
    for p in oks:
        print(f"  {p[1]}/{(int(p[1]) + 1) % 100:02d} {p[2]}: {', '.join(p[3:])}")
    for p in faults:
        fails.append(f"{p[1]}/{(int(p[1]) + 1) % 100:02d} {p[2]}: {p[3]} ({p[4]}): {p[5]}")
    return fails


def main(argv):
    if len(argv) < 3 or "--keep" not in argv:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    work = os.path.abspath(arg(argv, "--keep"))
    year = int(arg(argv, "--year", 2001))
    last = int(arg(argv, "--last", year))
    database = os.path.abspath(arg(argv, "--database")) if "--database" in argv else None
    nations = arg(argv, "--nations", "England").split(",")
    rules = arg(argv, "--changing-rules", "on")
    if os.path.exists(work):
        print(f"data_sweep_run.py: {work} exists; give a new --keep folder")
        return 2
    code, _ = rrc.run(program, game, last, year if year != 2001 else None, database, nations, [], work,
                      int(arg(argv, "--timeout", 7200)), {"changing_rules": rules}, {"RECOMP_DATA_SWEEP": "1"})
    fails = [] if code == 0 else [f"the run ended with code {code} ({work})"]
    fails += check(work, last)
    for f in fails:
        print(f"FAIL  {f}")
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
