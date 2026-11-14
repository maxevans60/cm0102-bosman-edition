#!/usr/bin/env python3
"""The size step at a season change (0.7.2 league sizes, slice 3, D13; spec
docs/specs/2026-11-13-league-sizes-0.7.2-spec.md).

Usage:
    python3 tests/seasons/size_step_run.py <cm0102_recomp> <game folder> <case> [--keep <folder>] [--reuse]
        [--timeout <seconds>]
    python3 tests/seasons/size_step_run.py save <cm0102_recomp> <game folder> --old <0.7.1 cm0102_recomp>
        --keep <folder>
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

save (D18, slice 3 check 2): the 0.7.1 build (--old, driven by this tree's scripts: the 0.7.1 scripts click the row
below Portugal on a 2015 database, open item 231) makes a 2015 game of
England and Portugal with changing rules on and saves it 200 game days after its start (RECOMP_SAVE_AFTER_DAYS,
tests/seasons/soak_run.py new_game_leg), in the 2015/16 season. 0.7.1 holds the Segunda Liga at 18 (its class
count). This build loads the save and plays to the end of 2016/17 (soak_run.load_leg). The checks: the Segunda Liga
holds 22 clubs (its 2016/17 row) at the start of 2016/17 with its step line, the division check of every season start,
and no message box in either leg.
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
    "grow": (2005, 2008, ["England", "Sweden"], {"swe_prm": (2008, 16)},
             "D23: the Allsvenskan grows from 14 to 16 in 2008 through the real places of 2007 (1 down, 3 up)"),
    "shrink": (2021, 2023, ["England", "France"], {"fra_first": (2023, 18)},
               "D23: Ligue 1 shrinks from 20 to 18 in 2023/24 through the real places of 2022/23 (4 down, 2 up)"),
    "italy": (2003, 2004, ["England", "Italy"], {"ita_ser_a": (2004, 20)},
              "Serie A 18 to 20 in 2004/05 (slice 9 grows the class to 20)"),
    # Slice 9: a 2001 game with changing rules on to 2005/06: Serie B 20, 24 in 2003/04, 22 from 2004/05; Serie A
    # 20 from 2004/05; C1/A 19 in 2004/05 and 18 again in 2005/06.
    "italy2001": (2001, 2005, ["England", "Italy"],
                  {"ita_ser_a": (2004, 20), "ita_ser_b": (2004, 22), "ita_ser_c1a": (2005, 18)},
                  "Italy from 2001 to 2005/06: Serie B 24 then 22, Serie A 20, Serie C1/A 19 then 18"),
}

# D23: the places of the season before a change (report season, up, down; None: not checked) and the files whose
# count the real places reach, so the size step moves no club into or out of them.
PLACES = {
    "grow": {"swe_prm": (2007, None, 1), "swe_first": (2007, 3, None)},
    "shrink": {"fra_first": (2022, None, 4), "fra_second": (2022, 2, None)},
}
NO_STEP = {"grow": ["swe_prm"], "shrink": ["fra_first"]}

# Division differences of a start that slice 3 does not make (printed as KNOWN, not failed): open item 236, the slice
# 2 date planner keeps the Segunda Liga's table at 18 at a 2013/14 start while the fill puts 22 clubs in it.
KNOWN = {"portugal": {(2013, "por_second")}}

MOVE_LINE = re.compile(r"recomp: league sizes: step: (.*) from (.*) to (.*)$")

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
    # A file can give two lines: the French Lower Division holds fra_first with no clubs and year 0.
    return {(c["file"], c["season"]): c["clubs"] for c in comps
            if c["phase"] == "start" and c["kind"] == "league" and c["year"]}


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
    for file, (season, up, down) in PLACES.get(case, {}).items():
        line = [c for c in comps if c["file"] == file and c["season"] == season and c["phase"] == "start"
                and c["kind"] == "league" and c["year"]]
        if not line:
            fails.append(f"{file} {season}: no start line")
            continue
        for key, want in (("up", up), ("down", down)):
            # The reading of rules_report_check.py: the row's count is the direct places, or the direct places and
            # the winners of the class's own play-off (the Allsvenskan 2007: 0 down and its play-off, the row 1).
            c, extra = line[0][key], line[0]["playoff_" + key]
            if want is not None and want not in (c, c + (1 if extra else 0), c + extra):
                fails.append(f"{file} {season}: {key} {c} (+{extra} play-off places), its row {want}")
    for file in NO_STEP.get(case, []):
        names = {c["name"] for c in comps if c["file"] == file}
        moves = [m.group(0) for m in map(MOVE_LINE.match, log.split("\n")) if m and
                 (m.group(2) in names or m.group(3) in names)]
        fails += [f"{file}: the step moved a club: {m}" for m in moves]
    for d in sorted(rrc.division_differences(comps, clubs)):
        if (d[0], d[2].split(" ")[0]) in KNOWN.get(case, set()):
            print(f"KNOWN {d[0]} {d[2]} (open item 236)")
        else:
            fails.append(f"{d[0]} {d[2]}")
    return fails


SAVE_CASE = {"start": 2015, "last": 2016, "nations": ["England", "Portugal"], "days": 200,
             "wants": {"por_second": (2016, 22), "por_prm": (2016, 18)}}


def save_case(argv):
    import rules_report_check as rrc
    import soak_run
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    old, keep = (os.path.abspath(rrc.arg(argv, n)) for n in ("--old", "--keep"))
    os.makedirs(keep, exist_ok=True)
    db = rrc.run_database([], SAVE_CASE["start"], os.path.join(keep, "owndb"))
    leg0, leg1 = os.path.join(keep, "leg00"), os.path.join(keep, "leg01")
    o = {"program": old, "game": game, "rules": "on", "database": db, "nations": SAVE_CASE["nations"], "extra": [],
         "start": SAVE_CASE["start"], "last": SAVE_CASE["last"], "seed": 1, "timeout": 14400}
    if "--reuse" not in argv:
        print("leg 0:", soak_run.new_game_leg(o, leg0, SAVE_CASE["days"]))
        save = os.path.join(leg0, "user", "samesave.sav")
        if not os.path.exists(save):
            print(f"FAIL save: the 0.7.1 leg made no save ({leg0})")
            return 1
        soak_run.LEG_ENV["RECOMP_SIZE_LOG"] = "1"
        o["program"] = program
        result = soak_run.load_leg(o, leg1, save, 0)
        print("leg 1:", result)
    fails = []
    for leg in (leg0, leg1):
        log = os.path.join(leg, "stderr.txt")
        fails += [f"{os.path.basename(leg)}: {x.strip()}" for x in open(log, encoding="latin-1")
                  if "recomp: message box from" in x] if os.path.exists(log) else [f"no {log}"]
    CASES["save"] = (SAVE_CASE["start"], SAVE_CASE["last"], SAVE_CASE["nations"], SAVE_CASE["wants"], "")
    fails += faults(leg1, "save")
    for f in fails:
        print("FAIL", f)
    print(("FAIL" if fails else "PASS") + f" size step save of 0.7.1 ({keep})")
    return 1 if fails else 0


def self_test():
    bad = []
    log = ("recomp: league sizes: step: por_prm (Portuguese Premier League) 2014: 18 clubs, the row 18\n"
           "recomp: league sizes: step: por_second (Portuguese Second League) 2014: 22 clubs, the row 24: no club of "
           "its nation with no division\n")
    s = step_lines(log)
    if s != [("por_prm", 2014, 18, 18, ""),
             ("por_second", 2014, 22, 24, ": no club of its nation with no division")]:
        bad.append(("step_lines", s))
    c = start_counts([{"file": "por_prm", "season": 2014, "phase": "start", "kind": "league", "clubs": 18, "year": 2014},
                      {"file": "por_prm", "season": 2014, "phase": "start", "kind": "league", "clubs": 0, "year": 0},
                      {"file": "por_prm", "season": 2014, "phase": "end", "kind": "league", "clubs": 16, "year": 2015}])
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
    if argv and argv[0] == "save":
        return save_case(argv)
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
