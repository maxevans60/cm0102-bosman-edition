#!/usr/bin/env python3
"""Error boxes in a season of a season database (0.6.0, docs/research/2026-10-18-staff-reserve-boxes.md).

Usage:
    python3 tests/seasons/season_boxes_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py over one whole season (start in X, last season
X), England selected, with the staff reserve of the build for that database (no RECOMP_STAFF_RESERVE):
    2008    September 2008. Before the fix it stopped on 23.8.2008 at the box "news..cpp 7797": a call-up news
            item with the same player three times (NewsItem::get_slot 0x785830 from 0x76e440)
    2023    October 2023. Before the fix it stopped on 9.2.2024 at the box "Database..cpp 13126"
            (0x53b3d0 with no nation, from 0x600c02)

The checks (each run):
    - the jump ends with code 0 and the report holds the start and the end of season X
    - the log has no message box of the game (RECOMP_SCREEN_LOG prints each one, src/recomp_rt/win/waits.c)
A run takes about 3 to 5 minutes. --jobs (default 2) runs that many at the same time.
"""
import concurrent.futures
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
RUNS = {
    "2008": (2008, os.path.join(DB, "September 2008")),
    "2023": (2023, os.path.join(DB, "October 2023")),
}
BOX = re.compile(r"recomp: message box from (0x[0-9a-f]+): (.*)")


def check_run(program, game, name, keep):
    year, database = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-season-boxes-{name}-")
    code, report = rrc.run(program, game, year, year, database, ["England"], [], work, 2400)
    fails = []
    if code != 0:
        fails.append(f"{name}: the run ended with code {code} ({work})")
    with open(os.path.join(work, "stderr.txt"), encoding="latin-1") as f:
        boxes = [m.groups() for m in (BOX.search(x) for x in f.read().split("\n")) if m]
    for at, text in boxes:
        fails.append(f"{name}: message box from {at}: {text}")
    phases = set()
    if os.path.exists(report):
        seasons, _ = rrc.read_report(report)
        phases = {(s["season"], s["phase"]) for s in seasons}
    for phase in ("start", "end"):
        if (year, phase) not in phases:
            fails.append(f"{name}: the report has no {phase} of season {year} ({work})")
    print(f"{name}: code {code}, {len(boxes)} boxes, {len(fails)} problems ({work})")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    names = argv[argv.index("--only") + 1].split(",") if "--only" in argv else list(RUNS)
    jobs = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 2
    keep = os.path.abspath(argv[argv.index("--keep") + 1]) if "--keep" in argv else None
    for n in names:
        if n not in RUNS:
            print(f"--only: no run named {n}")
            return 2
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        fails = sum(pool.map(lambda n: check_run(program, game, n, keep), names), [])
    for f in fails:
        print("FAIL ", f)
    print("FAILED:", len(fails) if fails else "none")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
