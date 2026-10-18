#!/usr/bin/env python3
"""UEFA 2021/22 to 2023/24 check (0.6.0 slice 24): the Europa League of 32 clubs with knockout round play-offs,
no away goals rule, and no UEFA places for a suspended nation (Russia from 2022/23).

Usage:
    python3 tests/seasons/uefa_2021_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py with England. The runs:
    2021    October 2021, changing rules off, the season 2021/22
    2022    October 2022, changing rules off, the season 2022/23: Russia is suspended
    2023    October 2023, the season 2023/24: Russia is suspended, the Super Cup goes to penalties after 90
            minutes
    change  November 2020, changing rules on, 2020/21 to the end of 2021/22: the news sentences of the change
The checks of each run: the jump ends with code 0 and shows no message box of the game; the log of
src/recomp_rt/win/season_uefa*.c shows the access list places, 8 Europa League groups of 4 drawn, the knockout
round play-offs drawn, and the no away goals rule in the two-legged rounds of the Champions League and the
Europa League (qualifying rounds and knockout stages, 4 round tables); the uefa line of the report at the end of
each season of the rules: Champions League 9 stages, 32 clubs in groups, a knockout of 16 in 4 rounds; Europa
League 9 stages, 32 clubs in 8 groups, a knockout stage of 24 clubs (16 in the play-offs and the 8 group winners)
in 5 rounds, each with a winner; the label of round code 30 of the Europa League is the real name of 2021/22.
2022 and 2023: the log line of the seeding counts 0 clubs of Russia with a UEFA place. No Champions League or
Europa League tie of a season from 2021/22 is decided on away goals (the log line of the wrap of 0x51f2f0), and
in the change run at least one of 2020/21 is.
The Conference League is not in this check (not in the game yet: docs/research/2026-10-18-uefa-2021-2023.md).
A run takes about 10 to 40 minutes on a loaded machine. --jobs (default 1).
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
CL = {"stages": 9, "groups": 32, "groups2": 0, "ko_clubs": 16, "ko_rounds": 4}
EL = {"stages": 9, "groups": 32, "groups2": 0, "ko_clubs": 24, "ko_rounds": 5}
EL_2020 = {"stages": 13, "groups": 48, "groups2": 0, "ko_clubs": 32, "ko_rounds": 5}
KO_PO_NAME = "Knockout Round Play-offs"
NEWS = ["The Europa League group winners now go to the round of 16, and the runners-up meet the Champions League "
        "group thirds in knockout round play-offs.",
        "Away goals no longer decide UEFA ties: a tie level on aggregate goes to extra time and penalties."]


def season_checks(name, log, report, season):
    key = f"{season}/{(season + 1) % 100:02d}"
    fails = []
    for pattern in (rf"access list {key}: \d+ Champions League and \d+ Europa League places",
                    rf"Champions League {key}: \d+ entrants",
                    rf"Europa League {key}: \d+ qualifying rounds, 8 groups of 4",
                    rf"UEFA Cup {key}: 8 groups of 4 drawn", r"Europa League knockout round play-offs drawn",
                    r"Champions League round of 16 drawn"):
        if not re.search(pattern, log):
            fails.append(f"{name}: no log line {pattern!r}")
    # Each round table by its first code (a knockout can log twice: cl_rank_teams asks for its table too): the
    # Champions League qualifying rounds (250), its knockout (20), the Europa League qualifying rounds (270) and
    # its knockout (30).
    tables = {int(c): int(n) for n, c in
              re.findall(rf"no away goals rule in (\d+) of \d+ rounds, first code (\d+) \({key}\)", log)}
    for code in (250, 20, 270, 30):
        if tables.get(code, 0) <= 0:
            fails.append(f"{name}: no round table of first code {code} with the no away goals rule in {key}")
    # Real ties: no Champions League or Europa League tie of the season is decided on away goals (the log line of
    # the wrap of 0x51f2f0, season_uefa.c). The log of the season starts at its access list line.
    at = log.find(f"access list {key}:")
    if at >= 0 and "decided on away goals" in log[at:]:
        fails.append(f"{name}: a UEFA tie decided on away goals in {key}")
    for bad in ("not even", "gives no layout", "not 32", "no Champions League third", "runners-up\n",
                "clubs, not 32", "no format for"):
        if bad in log:
            fails.append(f"{name}: log line with {bad!r}")
    if os.path.exists(report):
        lines = ufr.uefa_lines(report)
        fails += ufr.expect(lines, season, "european_cup", CL, name)
        fails += ufr.expect(lines, season, "uefa_cup", EL, name)
        fails += ufr.expect_label(ufr.round_labels(report), season, "uefa_cup", 30, KO_PO_NAME, name)
    else:
        fails.append(f"{name}: no report")
    return fails


def run_season(name, season, database, suspended_check, super_cup_penalties=False):
    def run(program, game, work):
        code, report = rrc.run(program, game, season, season, os.path.join(DB, database), ["England"], [], work, 7200)
        fails = [] if code == 0 else [f"{name}: the jump ended with code {code}"]
        found, log = ufr.boxes(work)
        fails += [f"{name}: message box {b}" for b in found]
        fails += season_checks(name, log, report, season)
        m = re.search(r"suspended Russia: (\d+) clubs with a place", log)
        if suspended_check and (not m or m.group(1) != "0"):
            fails.append(f"{name}: Russia: {m.group(0) if m else 'no log line of the suspension'}")
        if not suspended_check and m:
            fails.append(f"{name}: Russia is suspended in {season}")
        sc = f"Super Cup {season}: penalties after 90 minutes" in log
        if sc != super_cup_penalties:
            fails.append(f"{name}: the Super Cup of {season} {'has' if super_cup_penalties else 'has no'} extra time")
        return fails
    return run


def run_change(program, game, work):
    code, report = rrc.run(program, game, 2021, 2020, os.path.join(DB, "November 2020"), ["England"], [], work, 10800,
                           {"changing_rules": "on"}, {"RECOMP_SEASON_CHANGE_LOG": "1"})
    fails = [] if code == 0 else [f"change: the jump ended with code {code}"]
    found, log = ufr.boxes(work)
    fails += [f"change: message box {b}" for b in found]
    if os.path.exists(report):
        lines = ufr.uefa_lines(report)
        fails += ufr.expect(lines, 2020, "uefa_cup", EL_2020, "change")
        fails += ufr.expect_label(ufr.round_labels(report), 2020, "uefa_cup", 30, KO_PO_NAME, "change", absent=True)
    if re.search(r"no away goals rule in \d+ of \d+ rounds, first code \d+ \(2020/21\)", log):
        fails.append("change: the no away goals rule in 2020/21")
    before = log[:log.find("access list 2021/22:")] if "access list 2021/22:" in log else log
    if "decided on away goals" not in before:
        fails.append("change: no UEFA tie of 2020/21 decided on away goals (the rule still holds in 2020/21)")
    fails += season_checks("change", log, report, 2021)
    for sentence in NEWS:
        if sentence not in log:
            fails.append(f"change: no news sentence {sentence!r}")
    return fails


RUNS = {
    "2021": run_season("2021", 2021, "October 2021", False),
    "2022": run_season("2022", 2022, "October 2022", True),
    "2023": run_season("2023", 2023, "October 2023", True, True),
    "change": run_change,
}


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = argv[1], argv[2]
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    jobs = int(rrc.arg(argv, "--jobs", "1"))
    keep = rrc.arg(argv, "--keep", None) or tempfile.mkdtemp(prefix="uefa-2021-")
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
