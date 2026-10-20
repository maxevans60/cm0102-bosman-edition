#!/usr/bin/env python3
"""Italy's single Serie C of three groups (0.6.0 slice 20b, 2014/15 on): groups A, B and C on the objects of C1/A,
C1/B and C2/A, groups of 20 with 38 match days, the Serie C chain and regroup, and the change from C1 and C2.

Usage:
    python3 tests/seasons/ita_serie_c_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py with Italy. The runs:
    2014      October 2014, changing rules off, 2014/15 and 2015/16 (the Serie C start code runs at the change)
    2014-c2   the same with the extra division (Serie C2 selected: the original makes C2/A, C2/B and C2/C)
    change    Autumn 2013, changing rules on, 2013/14 to the end of 2014/15 (C1 and C2 end)
    change-c2 the same with the extra division
The checks: the jump ends with code 0 and shows no message box of the game; the report has the groups (files
ita_ser_c1a, ita_ser_c1b and ita_ser_c2a, Q520) at the end of each season of the era, each with the clubs of its row (20 in 2014/15;
with changing rules off every later season keeps the start season's counts, Q232), and C2/B and C2/C with
year 2999 at the end of a season of the era (Q623); the log has the 38 match days of each group of 20, the regroup line of each
season change into the era, and the news sentence of the change in the change runs.
"""
import concurrent.futures
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rules_report_check as rrc  # noqa: E402
import uefa_formats_run as ufr  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
GROUPS = ["ita_ser_c1a", "ita_ser_c1b", "ita_ser_c2a"]      # Q520: the rows take the files of their objects
NEWS = "Serie C1 and Serie C2 end: the Serie C is now one division of three groups of 20, 20 and 20 clubs."


def comps(report):
    """{(season, phase): {file: (clubs, year)}} of the comp lines of a report."""
    out = {}
    with open(report, encoding="latin-1") as f:
        for line in f:
            c = line.rstrip("\n").split("\t")
            if c[0] == "comp" and len(c) > 10:
                out.setdefault((int(c[1]), c[2]), {})[c[5]] = (int(c[10]), int(c[9]))
    return out


def check_report(name, report, seasons, want):
    fails = []
    if not os.path.exists(report):
        return [f"{name}: no report"]
    got = comps(report)
    for s in seasons:
        end = got.get((s, "end"), {})
        for g, file in enumerate(GROUPS):
            got_clubs = end.get(file, (None, None))[0]
            if got_clubs != want[s][g]:
                fails.append(f"{name}: {file} at the end of {s} holds {got_clubs} clubs, want {want[s][g]}")
        for old in ("ita_ser_c2b", "ita_ser_c2c"):
            if old in end and end[old][1] != 2999:  # Q623: an ended league waits for year 2999
                fails.append(f"{name}: {old} has year {end[old][1]} at the end of {s} (it ends in the era)")
    return fails


def run_season(name, extra):
    def run(program, game, work):
        code, report = rrc.run(program, game, 2015, 2014, os.path.join(DB, "October 2014"), ["Italy"], extra, work,
                               7200, None, {"RECOMP_SEASON_CHANGE_LOG": "1"})
        fails = [] if code == 0 else [f"{name}: the jump ended with code {code}"]
        found, log = ufr.boxes(work)
        fails += [f"{name}: message box {b}" for b in found]
        fails += check_report(name, report, [2014, 2015], {2014: [20, 20, 20], 2015: [20, 20, 20]})
        if len(re.findall(r"serie c: .*: 20 clubs, 38 match days", log)) < 3:
            fails.append(f"{name}: fewer than 3 groups of 20 with 38 match days")
        if not re.search(r"serie c 2015/16: 60 clubs in groups of 20, 20 and 20", log):
            fails.append(f"{name}: no regroup into 2015/16")
        return fails
    return run


def run_change(name, extra):
    def run(program, game, work):
        code, report = rrc.run(program, game, 2014, 2013, os.path.join(DB, "Autumn 2013"), ["Italy"], extra, work,
                               10800, {"changing_rules": "on"}, {"RECOMP_SEASON_CHANGE_LOG": "1"})
        fails = [] if code == 0 else [f"{name}: the jump ended with code {code}"]
        found, log = ufr.boxes(work)
        fails += [f"{name}: message box {b}" for b in found]
        fails += check_report(name, report, [2014], {2014: [20, 20, 20]})
        if not re.search(r"serie c 2014/15: 60 clubs in groups of 20, 20 and 20", log):
            fails.append(f"{name}: no regroup into 2014/15")
        if NEWS not in log:
            fails.append(f"{name}: no news sentence of the change")
        return fails
    return run


RUNS = {
    "2014": run_season("2014", []),
    "2014-c2": run_season("2014-c2", ["c2"]),
    "change": run_change("change", []),
    "change-c2": run_change("change-c2", ["c2"]),
}


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = argv[1], argv[2]
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    jobs = int(rrc.arg(argv, "--jobs", "1"))
    keep = rrc.arg(argv, "--keep", None) or tempfile.mkdtemp(prefix="ita-serie-c-")
    fails = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
        futures = {n: ex.submit(RUNS[n], program, game, os.path.join(keep, n)) for n in only}
        for n, fut in futures.items():
            got = fut.result()
            print(f"{n}: {'ok' if not got else 'FAIL'} ({os.path.join(keep, n)})")
            fails += got
    for f in fails:
        print("FAIL ", f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 0 if not fails else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
