#!/usr/bin/env python3
"""UEFA 2009/10 to 2020/21 check (0.6.0 slice 23): the access list, the Champions League paths, the Europa
League and the end of the Intertoto Cup, in a game that starts in a season of the access list.

Usage:
    python3 tests/seasons/uefa_access_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py with England (changing rules off: the start
season's rules). The runs:
    2010    September 2010 ODB, the season 2010/11 (the format of 2009/10 to 2017/18)
    2018    October 2018, the season 2018/19 (paths from Q2, four group places for the top four associations;
            the preliminary round clubs enter the first qualifying round)
The checks of each run: the jump ends with code 0 and shows no message box of the game; the log of
src/recomp_rt/win/season_uefa_seeding.c and season_uefa_qual.c shows the places of the access list and the
entrants of both cups (Champions League 76 clubs); the uefa lines of the report at the end of
the season: Champions League 9 stages, 32 clubs in groups, a knockout of 16 and 4 rounds; Europa League 13
stages, 48 clubs in 12 groups, a knockout of 32 and 5 rounds, each with a winner; the round labels of Q370
(Play-off Round, Preliminary Round from 2018/19, Third Qualifying Round of the Europa League); and the
Intertoto Cup plays no round (no log line of its 3 or 5 rounds).
A 2009/10 start is not in the list: the World Cup box of a moved season (world_cup.cpp 1438, slice 27) stops it
in December 2009. A run takes about 10 to 40 minutes on a loaded machine. --jobs (default 1).
"""
import concurrent.futures
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rules_report_check as rrc  # noqa: E402
import uefa_formats_run as ufr  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
CL = {"stages": 9, "groups": 32, "groups2": 0, "ko_clubs": 16, "ko_rounds": 4}
EL = {"stages": 13, "groups": 48, "groups2": 0, "ko_clubs": 32, "ko_rounds": 5}


def run_season(name, season, database, cl_clubs, labels_want):
    def run(program, game, work):
        code, report = rrc.run(program, game, season, season, os.path.join(DB, database), ["England"], [], work, 7200)
        fails = [] if code == 0 else [f"{name}: the jump ended with code {code}"]
        found, log = ufr.boxes(work)
        fails += [f"{name}: message box {b}" for b in found]
        key = f"{season}/{(season + 1) % 100:02d}"
        for pattern in (rf"access list {key}: {cl_clubs} Champions League and \d+ Europa League places",
                        rf"Champions League {key}: {cl_clubs} entrants",
                        rf"Europa League {key}: \d+ qualifying rounds, 12 groups of 4",
                        rf"UEFA Cup {key}: 12 groups of 4 drawn", r"UEFA Cup round of 32 drawn",
                        r"Champions League round of 16 drawn"):
            if not re.search(pattern, log):
                fails.append(f"{name}: no log line {pattern!r}")
        if re.search(r"Intertoto Cup \d+ with \d rounds", log):
            fails.append(f"{name}: the Intertoto Cup played rounds")
        for bad in ("not even", "gives no layout", "not 32", "no Champions League third", "clubs, not 48"):
            if bad in log:
                fails.append(f"{name}: log line with {bad!r}")
        if os.path.exists(report):
            lines = ufr.uefa_lines(report)
            fails += ufr.expect(lines, season, "european_cup", CL, name)
            fails += ufr.expect(lines, season, "uefa_cup", EL, name)
            labels = ufr.round_labels(report)
            for file, code, want in labels_want:
                fails += ufr.expect_label(labels, season, file, code, want, name)
        return fails
    return run


LABELS_2009 = [("european_cup", 220, "Play-off Round"), ("uefa_cup", 220, "Play-off Round"),
               ("uefa_cup", 270, "Third Qualifying Round"), ("european_cup", 250, "First Qualifying Round")]
RUNS = {
    "2010": run_season("2010", 2010, "September 2010 ODB", 76, LABELS_2009),
    # 2018/19: 79 clubs in the row, 76 in the game: the 4 clubs of the preliminary round enter the first
    # qualifying round and the lowest-ranked 3 of them get no place (season_uefa_code_clubs).
    "2018": run_season("2018", 2018, "October 2018", 76, LABELS_2009 + [("european_cup", 240, "Preliminary Round")]),
}


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = argv[1], argv[2]
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    jobs = int(rrc.arg(argv, "--jobs", "1"))
    keep = rrc.arg(argv, "--keep", None) or __import__("tempfile").mkdtemp(prefix="uefa-access-")
    fails = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
        futures = {name: ex.submit(RUNS[name], program, game, os.path.join(keep, name)) for name in only}
        for name, fut in futures.items():
            got = fut.result()
            print(f"{name}: {'ok' if not got else 'FAIL'} ({os.path.join(keep, name)})")
            fails += got
    for f in fails:
        print("FAIL ", f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 0 if not fails else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
