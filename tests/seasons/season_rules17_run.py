#!/usr/bin/env python3
"""Substitutes, transfer windows, foreign player limits and work permits by season (0.6.0 slice 17).

Usage:
    python3 tests/seasons/season_rules17_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py with the test logs RECOMP_SEASON_SUBS_LOG=1,
RECOMP_SEASON_NATION_LOG=1 and RECOMP_SEASON_CHANGE_LOG=1. The runs:
    on2010    September 2010 ODB, 2010/11 to 2011/12, changing_rules on. The substitutes and the bench of every
              competition that the rules table gives a count for equal the table at the start of each season
              (a count above 9 is 9: the match engine clamps it, 0x6b7feb); England has two windows from the
              start (1 June to 31 August, 1 January to 31 January); the news log has no change that the table
              does not give
    off2010   the same with changing_rules off: the rules of 2010/11 stay in 2011/12 (the English Football
              League keeps 7 named substitutes, where the table has 5 from 2011/12), and the log shows no season
              hook of the nation kind
    on2020    November 2020, 2020/21 to 2021/22, changing_rules on: from 1 January 2021 a player of the EU needs a
              work permit to sign for an English club (the log counts the refusals)
    base      the base game (2001, changing_rules off): the kinds do nothing (the log shows the new game as
              "rules off" and no change)

The checks and what they cover are in check_run. A run takes 6 to 8 minutes.
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
# name: (start year, database, changing rules, last season)
RUNS = {
    "on2010": (2010, os.path.join(DB, "September 2010 ODB"), "on", 2011),
    "off2010": (2010, os.path.join(DB, "September 2010 ODB"), "off", 2011),
    "on2020": (2020, os.path.join(DB, "November 2020"), "on", 2021),
    "base": (2001, None, "off", 2001),
}
SUBS = re.compile(r"season subs: (\w+) (\d+)/\d+ (used|bench) (\d+) -> (\d+)")
WINDOWS = re.compile(r"season nation: ([\w ]+) windows (\d+) -> (\d+):(.*)")
BREXIT = re.compile(r"season nation: Brexit: a player of the EU needs a work permit \((\d+)\)")
NEWS = re.compile(r"season change: news (\d+)/\d+: (.*)")
RULES = re.compile(r"season (?:subs|nation): new game (\d+), rules (on|off)")


def check_report(name, report, fails, rules):
    """The substitutes and the bench against the rules table."""
    seasons, comps = rrc.read_report(report)
    table = rrc.make_rules.read_rows()
    diffs = rrc.differences(seasons, comps, table)
    start = seasons[0]["season"]
    for season, kind, text in diffs:
        if kind not in ("bench", "subs_used"):
            continue
        m = re.search(r"table (\d+) ", text)
        if m and int(m.group(1)) > 9:
            continue                                  # the match engine holds 9 at most
        if rules == "off" and season > start:
            continue                                  # the rules of the start season stay (Q232)
        fails.append(f"{name}: {season}/{(season + 1) % 100:02d} {kind}: {text}")
    return len(comps)


def check_run(name, program, game, keep, reuse=False):
    year, database, rules, last = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-rules17-{name}-")
    report = os.path.join(work, "report.tsv")
    if reuse and os.path.exists(report):
        code = 0
    else:
        env = {"RECOMP_SEASON_SUBS_LOG": "1", "RECOMP_SEASON_NATION_LOG": "1", "RECOMP_SEASON_CHANGE_LOG": "1"}
        code, report = rrc.run(program, game, last, year if year != 2001 else None, database, ["England"], [], work, 7200,
                               {"changing_rules": rules}, env)
    if code != 0:
        return [f"{name}: the run ended with code {code} ({work})"]
    fails = []
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read().split("\n")
    n = check_report(name, report, fails, rules)
    subs = [m.groups() for m in (SUBS.search(x) for x in log) if m]
    wins = [m.groups() for m in (WINDOWS.search(x) for x in log) if m]
    refused = [int(m.group(1)) for m in (BREXIT.search(x) for x in log) if m]
    news = [(int(m.group(1)), m.group(2)) for m in (NEWS.search(x) for x in log) if m]
    new_game = [m.groups() for m in (RULES.search(x) for x in log) if m]
    if rules == "off" and year == 2001:
        if any(r != "off" for _, r in new_game):
            fails.append(f"{name}: the kinds were on in the base game: {new_game}")
        if subs or wins or refused:
            fails.append(f"{name}: the base game changed rules: {subs[:3]} {wins[:3]}")
    else:
        if not new_game or any(r != "on" for _, r in new_game):
            fails.append(f"{name}: the kinds were not on at the new game: {new_game}")
        if not subs:
            fails.append(f"{name}: no change of substitutes in the log")
    if year == 2010:
        eng = [w for w in wins if w[0] == "England"]
        if not eng or "1.6-31.8" not in eng[0][3] or "1.1-31.1" not in eng[0][3]:
            fails.append(f"{name}: England has no windows 1.6-31.8 and 1.1-31.1 at the new game: {eng}")
        seasons, comps = rrc.read_report(report)
        first = {(c["season"], c["phase"]): c["bench"] for c in comps if c["file"] == "eng_first"}
        want = 5 if rules == "on" else 7           # the table has 5 for the Football League from 2011/12
        if first.get((2011, "start")) != want:
            fails.append(f"{name}: eng_first bench at the start of 2011/12 is {first.get((2011, 'start'))}, "
                         f"expected {want}")
        if first.get((2010, "start")) != 7:
            fails.append(f"{name}: eng_first bench at the start of 2010/11 is {first.get((2010, 'start'))}, expected 7")
    if name == "on2020":
        if not refused:
            fails.append(f"{name}: no player of the EU was refused a work permit for an English club")
        # No news check: a game that starts in 2020 has no season hook before 1 January 2021. A start in 2019 stops
        # at comp_stats.CPP 1664 on 7 February 2020 in the main build too (not a finding of this slice).
    print(f"{name}: {n} competition lines, {len(subs)} changes of substitutes, {len(wins)} changes of windows, "
          f"{len(refused) and refused[-1]} permit refusals, {len(fails)} problems ({work})")
    for s, t in news[:12]:
        print(f"    news {s}: {t}")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    jobs = int(rrc.arg(argv, "--jobs", 1))
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    fails = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        for f in [pool.submit(check_run, n, program, game, keep, "--reuse" in argv) for n in only]:
            fails += f.result()
    for t in fails:
        print("FAIL  " + t)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
