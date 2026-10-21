#!/usr/bin/env python3
"""Screen sweep (0.6.0 slice 30a, check 4 of docs/research/brief-0.6.0/slice-30a.md): a season jump with
RECOMP_SCREEN_SWEEP=1, which opens and draws, one a human turn, views 1 to 4 (stages or table, results, fixtures,
schedule) of the competition screen and the history screen of every competition, one pass each season
(src/recomp_rt/win/screen_sweep.c).

Usage:
    python3 tests/seasons/screen_sweep_run.py <cm0102_recomp> <game folder> --keep <folder> [--year 2001]
        [--last <year>] [--database <folder>] [--nations England|all|England,Italy,...] [--changing-rules on|off]
        [--timeout 7200]

The run is tests/seasons/rules_report_check.py's season jump (England by default) from --year to the end of
--last (default: the season of --year), with RECOMP_DATA_SWEEP=1 too (data_sweep_run.py's checks of the same
competitions).

The checks:
    - the jump ends with code 0, and the report holds the end of --last
    - no message box of the game: each box is printed with the screen that was up (the last "screen sweep" line
      before it)
    - each season of the run has a pass, each pass ends ("done") and has at least one screen
    - each screen that the sweep opened was built ("built" before the next screen)
    - no "sweep" line of the data sweep
A season of England takes about 6 to 10 minutes; Select All much longer.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import data_sweep_run as dsr  # noqa: E402
import rules_report_check as rrc  # noqa: E402

PREFIX = "recomp: screen sweep: "
BOX = "recomp: message box from"
SCREEN = re.compile(r"recomp: screen sweep: (\d+)/\d+ on (\S+): (.*) view (\d)$")
PASS = re.compile(r"recomp: screen sweep: (\d+)/\d+ pass of (\d+) competitions")
DONE = re.compile(r"recomp: screen sweep: (\d+)/\d+ done: (\d+) screens")


def check_log(log, first, last):
    fails, current, built, passes, done, screens = [], None, True, set(), {}, 0
    try:
        lines = open(log, encoding="latin-1").read().split("\n")
    except OSError:
        return [f"no log ({log})"]
    for line in lines:
        m = SCREEN.match(line)
        if m:
            if current and not built:
                fails.append(f"not built: {current}")
            current, built = f"{m.group(3)} view {m.group(4)} on {m.group(2)}", False
            screens += 1
            continue
        if line == PREFIX + "built":
            built = True
        elif line.startswith(BOX):
            fails.append(f"box while {current or 'no sweep screen'} was up: {line}")
        m = PASS.match(line)
        if m:
            passes.add(int(m.group(1)))
        m = DONE.match(line)
        if m:
            done[int(m.group(1))] = int(m.group(2))
    for s in range(first, last + 1):
        if s not in passes:
            fails.append(f"no screen sweep pass in {s}/{(s + 1) % 100:02d}")
        elif s not in done:
            fails.append(f"the pass of {s}/{(s + 1) % 100:02d} did not end")
        elif done[s] == 0:
            fails.append(f"the pass of {s}/{(s + 1) % 100:02d} opened no screen")
    print(f"  {screens} screens; passes {sorted(passes)}, done {done}")
    return fails


def main(argv):
    if len(argv) < 3 or "--keep" not in argv:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    work = os.path.abspath(dsr.arg(argv, "--keep"))
    year = int(dsr.arg(argv, "--year", 2001))
    last = int(dsr.arg(argv, "--last", year))
    database = os.path.abspath(dsr.arg(argv, "--database")) if "--database" in argv else None
    nations = dsr.arg(argv, "--nations", "England").split(",")
    rules = dsr.arg(argv, "--changing-rules", "on")
    if os.path.exists(work):
        print(f"screen_sweep_run.py: {work} exists; give a new --keep folder")
        return 2
    code, _ = rrc.run(program, game, last, year if year != 2001 else None, database, nations, [], work,
                      int(dsr.arg(argv, "--timeout", 7200)), {"changing_rules": rules},
                      {"RECOMP_SCREEN_SWEEP": "1", "RECOMP_DATA_SWEEP": "1"}, stop_on_box=True)
    fails = [] if code == 0 else [f"the run ended with code {code} ({work})"]
    fails += check_log(os.path.join(work, "stderr.txt"), year, last)
    fails += [f for f in dsr.check(work, last) if not f.startswith("box: ")]     # boxes are named above
    for f in fails:
        print(f"FAIL  {f}")
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
