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
    g2001   the test seam RECOMP_CUP_GROUPS_TEST=1, 2001/02, changing rules on, England: the League Cup plays 16
            groups and a knockout of 32 clubs in 5 rounds (17 stages) and has a winner
    e2016   October 2016, changing rules off, England: the EFL Trophy plays 16 groups (56 clubs) and a knockout
            of 32 clubs in 5 rounds, and has a winner
    n2008   September 2008, changing rules on, 2008 to 2010, Norway: the Norwegian Super Cup is made, plays its
            editions of March 2009 and March 2010 with two clubs, and the row of 2010/11 holds no edition of March 2011
    c2007   November 2007, changing rules off, England with the Conference: the Conference League Cup is made and
            has a winner
    t2010   start run, September 2010 ODB, changing rules off, Select All without France (Turkey, Russia and the
            Republic of Ireland are not in the 16 rows that the steps click): the Turkish Cup has 4 groups after 3
            rounds, the Turkish Super Cup is made with two clubs, the Russian one of March 2011 (the edition of
            the row of 2010/11) with two clubs, the All-Ireland cup with 12 clubs
    r2002   start run, 2002/03 (the original data), changing rules off, Select All without France: the start hook
            makes the editions that the row of 2002/03 holds, the Russian Premier League Cup of 2003 (first round in
            March 2003) with 16 clubs and the Russian Super Cup of March 2003 with two clubs (log lines)

A run starts a game only when fewer than 15 games run on this Mac and the load of the last minute is below 24 (Max's
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


def need_groups(lines, file, season, want, fails, name):
    """want: stages, groups, group clubs (None: any), knockout clubs, knockout rounds at the end of the season."""
    got = lines_of(lines, file, season, "end")
    if not got:
        fails.append(f"{name}: no {file} line at the end of {season}")
    for x in got:
        have = (x["stages"], x["groups"], x["group_clubs"] if want[2] is not None else None, x["ko_clubs"],
                x["ko_rounds"])
        if have != want:
            fails.append(f"{name}: {file} at the end of {season}: stages, groups, group clubs, knockout clubs and "
                         f"rounds {have}, expected {want}")


def need_log(log, text, fails, name):
    if text not in log:
        fails.append(f"{name}: no log line \"{text}\"")


def need_made(lines, file, fails, name, clubs=None):
    got = [x for x in lines if x["file"] == file]
    if not got:
        fails.append(f"{name}: no {file} line in the report")
    for x in got:
        if clubs is not None and x["clubs"] not in (0, clubs):
            fails.append(f"{name}: {file} at the {x['phase']} of {x['season']}: {x['clubs']} clubs, expected {clubs}")


def check_g2001(lines, log, fails, name):
    need_groups(lines, "eng_cc_cup", 2001, (17, 16, None, 32, 5), fails, name)
    need_winner(lines, "eng_cc_cup", 2001, fails, name)
    need_log(log, "cup groups: test_eng_cc_cup 2001 with 16 groups after 0 rounds", fails, name)


def check_e2016(lines, log, fails, name):
    need_groups(lines, "eng_auto_cup", 2016, (17, 16, 56, 32, 5), fails, name)
    need_winner(lines, "eng_auto_cup", 2016, fails, name)


def check_n2008(lines, log, fails, name):
    need_made(lines, "new_nor_super", fails, name, 2)
    for y in (2009, 2010):
        if not any(x["file"] == "new_nor_super" and (x["winner"] != "-" or x["winner_next"] != "-") and
                   y in (x["season"], x["season"] + 1) for x in lines):
            fails.append(f"{name}: new_nor_super has no winner of {y} in the history")
    need_log(log, "new cups: new_nor_super is not held in 2011", fails, name)


def check_c2007(lines, log, fails, name):
    need_made(lines, "new_conf_cup", fails, name)
    need_winner(lines, "new_conf_cup", 2007, fails, name)


def check_t2010(lines, log, fails, name):
    need_log(log, "cup groups: tur_cup 2010 with 4 groups after 3 rounds", fails, name)
    need_made(lines, "new_tur_super", fails, name, 2)
    # Changing rules off: the edition that the row of 2010/11 holds, the Russian Super Cup of March 2011.
    need_log(log, "new cups: new_rus_super 2011 with 2 clubs", fails, name)
    need_made(lines, "new_all_ireland", fails, name, 12)


def check_r2002(lines, log, fails, name):
    need_log(log, "new cups: new_rus_lge_cup 2003 with 16 clubs", fails, name)
    need_log(log, "new cups: new_rus_super 2003 with 2 clubs", fails, name)


# name: (start year, database folder, changing rules, last season, nations, start run, time limit in seconds, check,
# more: "env" more environment variables, "extra" the lower divisions of rules_report_check.py)
RUNS = {
    "i2023": (2023, os.path.join(DB, "October 2023"), "on", 2024, ["Italy"], False, 3600, check_i2023, {}),
    "s2019": (2019, os.path.join(DB, "October 2019"), "off", 2019, ALL_BUT_FRANCE, True, 1800, check_s2019, {}),
    "g2001": (2001, None, "on", 2001, ["England"], False, 3600, check_g2001, {"env": {"RECOMP_CUP_GROUPS_TEST": "1"}}),
    "e2016": (2016, os.path.join(DB, "October 2016"), "off", 2016, ["England"], False, 3600, check_e2016, {}),
    "n2008": (2008, os.path.join(DB, "September 2008"), "on", 2010, ["Norway"], False, 5400, check_n2008, {}),
    "c2007": (2007, os.path.join(DB, "November 2007"), "off", 2007, ["England"], False, 3600, check_c2007,
              {"extra": ["conference"]}),
    "t2010": (2010, os.path.join(DB, "September 2010 ODB"), "off", 2010, ALL_BUT_FRANCE, True, 1800, check_t2010,
              {}),
    "r2002": (2002, None, "off", 2002, ALL_BUT_FRANCE, True, 1800, check_r2002, {}),
}


# The lead's hold file: while it exists no test game starts (docs/HANDOVER.md, Rules).
HOLD = "/Users/max/Projects/cm0102/run-logs/HOLD"


def wait_for_room():
    """Starts a game only when fewer than 15 games run on this Mac and the load of the last minute is below 24."""
    while True:
        games = subprocess.run(["pgrep", "-x", "cm0102_recomp"], capture_output=True, text=True).stdout.split()
        if len(games) < 15 and os.getloadavg()[0] < 24 and not os.path.exists(HOLD):
            return
        time.sleep(20)


def check_run(name, program, game, keep, reuse):
    year, database, rules, last, nations, start_run, limit, check, more = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-cups-more-{name}-")
    report = os.path.join(work, "report.tsv")
    if reuse and os.path.exists(report):
        code = 0
    elif os.path.exists(work) and keep:
        return [f"{name}: the folder {work} exists; use --reuse to check it again, or another --keep folder"]
    else:
        wait_for_room()
        env = dict({"RECOMP_SEASON_CHANGE_LOG": "1"}, **more.get("env", {}))
        code, report = rrc.run(program, game, last, year, database, nations, more.get("extra", []), work, limit,
                               {"changing_rules": rules}, env)
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
