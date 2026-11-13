#!/usr/bin/env python3
"""The size step at a season change (0.7.2 league sizes, slice 3, D13; spec
docs/specs/2026-11-13-league-sizes-0.7.2-spec.md).

Usage:
    python3 tests/seasons/size_step_run.py <cm0102_recomp> <game folder> <case> [--keep <folder>] [--reuse]
        [--timeout <seconds>]
    python3 tests/seasons/size_step_run.py --list
    python3 tests/seasons/size_step_run.py --self-test

A case starts a season game with changing rules on in a season before a size change of the rules table and plays
through the change (tests/seasons/rules_report_check.py, the season jump), with RECOMP_SIZE_LOG=1. The checks:
    count    at the start of the season of the change each league of the case holds the count of its new row (the
             comp line of the start report);
    division at every season start each league of the size table holds as many clubs in its division as its table
             has rows (rules_report_check.division_differences);
    step     the log has the step line of each league of the case in its new edition, with no shortfall;
    box      no message box.
"""
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))

# case: (start year, last season, nations, {file: (season of the change, count)}, what it shows)
CASES = {
    "portugal": (2013, 2014, ["England", "Portugal"], {"por_prm": (2014, 18), "por_second": (2014, 24)},
                 "Primeira Liga 16 to 18 (it takes the best two of the Segunda Liga that stay), Segunda Liga 22 to 24 "
                 "(the lowest division of the chain takes clubs of Portugal with no division)"),
    "norway": (2007, 2009, ["England", "Norway"], {"nor_prm": (2009, 16)},
               "Eliteserien 14 to 16 in 2009 (the First Division gives two and takes two clubs with no division)"),
    "italy": (2003, 2004, ["England", "Italy"], {"ita_ser_a": (2004, 20)},
              "Serie A 18 to 20 in 2004/05: the class holds 18 until the Italy slice grows it (slice 9)"),
}

STEP_LINE = re.compile(r"recomp: league sizes: step: (\S+) \((.*?)\) (\d+): (\d+) clubs, the row (\d+)(.*)")


def step_lines(log):
    """[(file, edition, clubs, row, rest)] of the step lines of a log."""
    out = []
    for line in log.split("\n"):
        m = STEP_LINE.match(line.strip())
        if m:
            out.append((m.group(1), int(m.group(3)), int(m.group(4)), int(m.group(5)), m.group(6)))
    return out


def start_counts(comps):
    """{(file, season): clubs} of the start lines of the leagues."""
    return {(c["file"], c["season"]): c["clubs"] for c in comps if c["phase"] == "start" and c["kind"] == "league"}


def faults(work, case):
    import rules_report_check as rrc
    _year, _last, _nations, wants, _what = CASES[case]
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    fails = [x.strip() for x in log.split("\n") if "recomp: message box from" in x]
    report = os.path.join(work, "report.tsv")
    if not os.path.exists(report):
        return fails + ["no report"]
    clubs = []
    _seasons, comps = rrc.read_report(report, clubs)
    counts = start_counts(comps)
    steps = step_lines(log)
    for file, (season, want) in wants.items():
        have = counts.get((file, season))
        if have != want:
            fails.append(f"{file} {season}: {have} clubs at the start, want {want}")
        mine = [s for s in steps if s[0] == file]
        if not any(s[2] == s[3] == want for s in mine):
            fails.append(f"{file}: no step line with {want} clubs ({mine})")
        fails += [f"{file} {e}: step: {n} clubs, the row {r}{rest}" for f, e, n, r, rest in mine if n != r]
    fails += [f"{d[0]} {d[2]}" for d in sorted(rrc.division_differences(comps, clubs))]
    return fails


def self_test():
    bad = []
    log = ("recomp: league sizes: step: por_prm (Portuguese Premier League) 2014: 18 clubs, the row 18\n"
           "recomp: league sizes: step: por_second (Portuguese Second League) 2014: 22 clubs, the row 24: no club of "
           "its nation with no division\n")
    s = step_lines(log)
    if s != [("por_prm", 2014, 18, 18, ""),
             ("por_second", 2014, 22, 24, ": no club of its nation with no division")]:
        bad.append(("step_lines", s))
    c = start_counts([{"file": "por_prm", "season": 2014, "phase": "start", "kind": "league", "clubs": 18},
                      {"file": "por_prm", "season": 2014, "phase": "end", "kind": "league", "clubs": 16}])
    if c != {("por_prm", 2014): 18}:
        bad.append(("start_counts", c))
    for b in bad:
        print("FAIL self-test", b)
    print(("FAIL" if bad else "PASS") + " size_step_run self-test")
    return 1 if bad else 0


def main(argv):
    if "--list" in argv:
        for k, (y, last, nations, wants, what) in CASES.items():
            print(f"{k}\t{y} to {last}\t{','.join(nations)}\t{what}")
        return 0
    if "--self-test" in argv:
        return self_test()
    if len(argv) < 3 or argv[2] not in CASES:
        print(__doc__)
        return 2
    import rules_report_check as rrc
    program, game, case = os.path.abspath(argv[0]), os.path.abspath(argv[1]), argv[2]
    year, last, nations, _w, _what = CASES[case]
    keep = rrc.arg(argv, "--keep", None)
    timeout = int(rrc.arg(argv, "--timeout", "7200"))
    work = os.path.abspath(keep) if keep else tempfile.mkdtemp(prefix=f"cm0102-size-step-{case}-")
    if "--reuse" not in argv:
        db = rrc.run_database(argv, year, os.path.join(work, "owndb"))
        code, _report = rrc.run(program, game, last, year, db, nations, [], work, timeout,
                                {"changing_rules": "on"}, {"RECOMP_SIZE_LOG": "1"})
        if code != 0:
            print(f"FAIL {case}: the run ended with code {code} ({work})")
            return 1
    fails = faults(work, case)
    for f in fails:
        print("FAIL", f)
    print(("FAIL" if fails else "PASS") + f" size step {case} ({work})")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
