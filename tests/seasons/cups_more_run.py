#!/usr/bin/env python3
"""Domestic cups, part 21c (0.6.0): four-club super cups, group stages and new cups.

Usage:
    python3 tests/seasons/cups_more_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--keep <folder>]
        [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py with the nations of its cups selected. The checks
read the "cupmore" lines of the rules report (src/recomp_rt/win/season_super_cups.c: file, year, clubs, rounds,
stages, groups, knockout, winners) and the log (docs/research/2026-10-18-cups-more.md).

The runs:
    i2023   October 2023, changing rules on, 2023/24 to 2024/25, Italy: the Supercoppa Italiana has four clubs and
            two rounds (semi-finals and final) at the new game and after the season change, and its final has a
            winner in the history
    s2019   start run, October 2019, changing rules off, Select All without France: the Supercopa de Espana has four
            clubs and two rounds at the new game

A run starts a game only when fewer than 12 games run on this Mac and the load of the last minute is below 18 (Max's
limit), and its game stops when the script stops (SIGTERM, SIGINT, SIGHUP). Run at most three of these scripts at
a time (--only).
"""
import os
import re
import signal
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
ALL_BUT_FRANCE = ["all", "France"]


def cupmore(report):
    """The cupmore lines of a report."""
    out = []
    with open(report, encoding="latin-1") as f:
        for line in f:
            c = line.rstrip("\n").split("\t")
            if c[0] != "cupmore" or len(c) < 15:
                continue
            out.append({"season": int(c[1]), "phase": c[2], "file": c[3], "year": int(c[4]), "clubs": int(c[5]),
                        "rounds": int(c[6]), "stages": int(c[7]), "now": int(c[8]), "groups": int(c[9]),
                        "group_clubs": int(c[10]), "ko_clubs": int(c[11]), "ko_rounds": int(c[12]),
                        "winner": c[13], "winner_next": c[14]})
    return out


def lines_of(lines, file, season, phase):
    return [x for x in lines if x["file"] == file and x["season"] == season and x["phase"] == phase]


def need_four(lines, file, season, phase, fails, name):
    got = lines_of(lines, file, season, phase)
    if not got:
        fails.append(f"{name}: no {file} line at the {phase} of {season}")
    for x in got:
        if x["clubs"] != 4 or x["rounds"] != 2:
            fails.append(f"{name}: {file} at the {phase} of {season}: {x['clubs']} clubs, {x['rounds']} rounds")


def need_winner(lines, file, season, fails, name):
    got = lines_of(lines, file, season, "end")
    if not got or all(x["winner"] == "-" and x["winner_next"] == "-" for x in got):
        fails.append(f"{name}: {file} has no winner in the history at the end of {season}")


def check_i2023(lines, log, fails, name):
    need_four(lines, "ita_super", 2023, "start", fails, name)
    need_winner(lines, "ita_super", 2023, fails, name)
    need_four(lines, "ita_super", 2024, "start", fails, name)
    for s in (2023, 2024):
        if f"super cups: ita_super {s} with four clubs" not in log:
            fails.append(f"{name}: no log line for the four clubs of {s}")


def check_s2019(lines, log, fails, name):
    need_four(lines, "spa_super", 2019, "start", fails, name)


# name: (start year, database folder, changing rules, last season, nations, start run, time limit in seconds, check)
RUNS = {
    "i2023": (2023, os.path.join(DB, "October 2023"), "on", 2024, ["Italy"], False, 3600, check_i2023),
    "s2019": (2019, os.path.join(DB, "October 2019"), "off", 2019, ALL_BUT_FRANCE, True, 1800, check_s2019),
}


def wait_for_room():
    """Starts a game only when fewer than 12 games run on this Mac and the load of the last minute is below 18."""
    while True:
        games = subprocess.run(["pgrep", "-x", "cm0102_recomp"], capture_output=True, text=True).stdout.split()
        if len(games) < 12 and os.getloadavg()[0] < 18:
            return
        time.sleep(20)


def check_run(name, program, game, keep, reuse):
    year, database, rules, last, nations, start_run, limit, check = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-cups-more-{name}-")
    report = os.path.join(work, "report.tsv")
    if reuse and os.path.exists(report):
        code = 0
    elif os.path.exists(work) and keep:
        return [f"{name}: the folder {work} exists; use --reuse to check it again, or another --keep folder"]
    else:
        wait_for_room()
        code, report = rrc.run(program, game, last, year, database, nations, [], work, limit,
                               {"changing_rules": rules}, {"RECOMP_SEASON_CHANGE_LOG": "1"})
    if code != 0 and not (start_run and code == "timeout"):
        return [f"{name}: the run ended with code {code} ({work})"]
    if not os.path.exists(report):
        return [f"{name}: no report ({work})"]
    fails = []
    seasons, _ = rrc.read_report(report)
    if start_run and not any(x["season"] == year and x["phase"] == "start" for x in seasons):
        fails.append(f"{name}: the report has no start of {year}/{(year + 1) % 100:02d}")
    if not start_run and not any(x["season"] == last and x["phase"] == "end" for x in seasons):
        fails.append(f"{name}: the report has no end of {last}/{(last + 1) % 100:02d} (the run stopped early)")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    lines = cupmore(report)
    check(lines, log, fails, name)
    print(f"{name}: {len(lines)} cupmore lines, {len(fails)} problems ({work})")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    # A stop of this script stops its game too: subprocess.run kills the game when an exception leaves it.
    def stop(*_):
        raise KeyboardInterrupt()
    for sig in (signal.SIGTERM, signal.SIGHUP, signal.SIGINT):
        signal.signal(sig, stop)
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    fails = []
    for name in [n for n in RUNS if n in only]:
        fails += check_run(name, program, game, keep, "--reuse" in argv)
    for t in fails:
        print("FAIL  " + t)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
