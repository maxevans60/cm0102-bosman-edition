#!/usr/bin/env python3
"""UEFA formats check (0.6.0 slice 22): the Champions League, UEFA Cup and Intertoto Cup of each season play the
format of the rules table, after a season change and after a load.

Usage:
    python3 tests/seasons/uefa_formats_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py. The rules report holds a "uefa" line for the
Champions League and the UEFA Cup at the start and end of each season (src/recomp_rt/win/season_uefa.c): the
stage count, the current stage, the clubs of the group stages in stages 0 to 7 and after them, and the clubs and
rounds of the knockout stage, and the winner of its history row of the season. The runs:
    2007    November 2007, changing rules off, the season 2007/08: Champions League with one group stage and a
            round of 16 (9 stages, 32 clubs in groups, 16 in the knockout), UEFA Cup with 40 clubs in groups and
            a knockout of 32 (9 stages), Intertoto Cup of 3 rounds (the log line of season_uefa_cup.c)
    change  3.9.68 data from 2002/03, changing rules on, to the end of 2004/05: 2002/03 is the original's
            format (13 stages, a second group stage of 16, quarter-finals, a UEFA Cup with no stages);
            2003/04 has the round of 16; 2004/05 the UEFA Cup groups. The news log
            (RECOMP_SEASON_CHANGE_LOG=1) has the sentence of each change in its season, and the report has no
            difference with the rules table that tests/seasons/rules_report_expected/changing-2002-england.tsv
            does not list (it lists no format line: each season plays the format of its row). The label of a
            round (the game's formatter 0x4bffc0, as a fixture line writes it) has its real name from the
            season of the rename (Q370): the Champions League "Round of 16" from 2003/04 (not in 2002/03),
            the UEFA Cup "Round of 32" and "First Qualifying Round" from 2004/05 ("Third Round" in 2003/04)
    load    November 2007: the game saves 240 days after the start (March 2008, the knockout stages of both
            cups), a new program loads the save, the holiday goes on to 1 June 2008, and the end report of
            2007/08 shows both knockout stages played to the end of the season (16 and 32 clubs)
The checks: each jump ends with code 0 and shows no message box of the game, the uefa lines are as above, and
each end line names a winner.
A run takes about 3 to 10 minutes. --jobs (default 2) runs that many at the same time.
"""
import concurrent.futures
import os
import re
import shutil
import subprocess
import sys
import tempfile
import threading

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
NOV_2007 = os.path.join(DB, "November 2007")
CL_ORIGINAL = {"stages": 13, "groups": 32, "groups2": 16, "ko_clubs": 8, "ko_rounds": 3}
CL_R16 = {"stages": 9, "groups": 32, "groups2": 0, "ko_clubs": 16, "ko_rounds": 4}
UC_ORIGINAL = {"stages": 0, "groups": 0, "groups2": 0, "ko_clubs": 0, "ko_rounds": 0}
UC_GROUPS = {"stages": 9, "groups": 40, "groups2": 0, "ko_clubs": 32, "ko_rounds": 5}
NEWS_R16 = "The Champions League now has one group stage of 8 groups and a knockout round of 16."
NEWS_UC = "The UEFA Cup now has a group stage of 8 groups of 5 clubs, then a round of 32."
CHECK_LOCK = threading.Lock()   # rules_report_check.check keeps the uefa lines of a report in a module global
BOX = re.compile(r"message box from 0x[0-9a-f]+: (.*)")


def uefa_lines(report):
    """{(season, phase, file): line} of the uefa lines of one report. Each run reads its own report here, so the
    runs of the thread pool share no state."""
    out = {}
    with open(report, encoding="latin-1") as f:
        for line in f:
            c = line.rstrip("\n").split("\t")
            if c[0] != "uefa" or len(c) < 10:
                continue
            u = {"season": int(c[1]), "phase": c[2], "file": c[3], "stages": int(c[4]), "current": int(c[5]),
                 "groups": int(c[6]), "groups2": int(c[7]), "ko_clubs": int(c[8]), "ko_rounds": int(c[9]),
                 "winner": c[10] if len(c) > 10 else "-"}
            out[(u["season"], u["phase"], u["file"])] = u
    return out


def round_labels(report):
    """{(season, file, code): label} of the roundname lines (Q370): the label the game's formatter
    0x4bffc0 gives a round code of the competition at the end of the season."""
    out = {}
    with open(report, encoding="latin-1") as f:
        for line in f:
            c = line.rstrip("\n").split("\t")
            if c[0] == "roundname" and len(c) >= 6:
                out[(int(c[1]), c[3], int(c[4]))] = c[5]
    return out


def expect_label(labels, season, file, code, want, label, absent=False):
    got = labels.get((season, file, code))
    if got is None:
        return [f"{label}: no round label of {file} {code} in {season}"]
    if (want in got) == absent:
        return [f"{label}: {file} round {code} in {season}/{(season + 1) % 100:02d} shows {got!r}"
                f"{', which has' if absent else ', not'} {want!r}"]
    return []


def expect(lines, season, file, want, label):
    u = lines.get((season, "end", file))
    if not u:
        return [f"{label}: no end line of {file} in {season}"]
    got = {k: u[k] for k in want}
    fails = [] if got == want else [f"{label}: {file} {season}/{(season + 1) % 100:02d} {got}, want {want}"]
    if u["winner"] in ("", "-"):
        fails.append(f"{label}: {file} {season}/{(season + 1) % 100:02d} has no winner in its history")
    return fails


def boxes(work):
    path = os.path.join(work, "stderr.txt")
    text = open(path, encoding="latin-1").read() if os.path.exists(path) else ""
    return [m.group(1) for m in BOX.finditer(text)], text


def run_2007(program, game, work):
    code, report = rrc.run(program, game, 2007, 2007, NOV_2007, ["England"], [], work, 3600)
    fails = [] if code == 0 else [f"2007: the jump ended with code {code}"]
    found, log = boxes(work)
    fails += [f"2007: message box {b}" for b in found]
    if os.path.exists(report):
        lines = uefa_lines(report)
        fails += expect(lines, 2007, "european_cup", CL_R16, "2007")
        fails += expect(lines, 2007, "uefa_cup", UC_GROUPS, "2007")
    if "Intertoto Cup 2007 with 3 rounds" not in log:
        fails.append("2007: the Intertoto Cup did not play 3 rounds")
    return fails


def run_change(program, game, work):
    code, report = rrc.run(program, game, 2004, 2002, None, ["England"], [], work, 7200, {"changing_rules": "on"},
                           {"RECOMP_SEASON_CHANGE_LOG": "1"})
    fails = [] if code == 0 else [f"change: the jump ended with code {code}"]
    found, log = boxes(work)
    fails += [f"change: message box {b}" for b in found]
    if os.path.exists(report):
        lines = uefa_lines(report)
        fails += expect(lines, 2002, "european_cup", CL_ORIGINAL, "change")
        fails += expect(lines, 2002, "uefa_cup", UC_ORIGINAL, "change")
        fails += expect(lines, 2003, "european_cup", CL_R16, "change")
        fails += expect(lines, 2003, "uefa_cup", UC_ORIGINAL, "change")
        fails += expect(lines, 2004, "european_cup", CL_R16, "change")
        fails += expect(lines, 2004, "uefa_cup", UC_GROUPS, "change")
    if os.path.exists(report):                  # Q370: the real round names from their season
        labels = round_labels(report)
        fails += expect_label(labels, 2002, "european_cup", 20, "Round of 16", "change", absent=True)
        fails += expect_label(labels, 2003, "european_cup", 20, "Round of 16", "change")
        fails += expect_label(labels, 2004, "european_cup", 20, "Round of 16", "change")
        fails += expect_label(labels, 2003, "uefa_cup", 30, "Third Round", "change")
        fails += expect_label(labels, 2004, "uefa_cup", 30, "Round of 32", "change")
        fails += expect_label(labels, 2004, "uefa_cup", 250, "First Qualifying Round", "change")
    if os.path.exists(report):                  # the whole report against the expected file of changing rules on
        with CHECK_LOCK:
            fails += [f"change: {t}" for t in rrc.check(report, None, 2004, "changing-2002-england")]
    for sentence, season in ((NEWS_R16, 2003), (NEWS_UC, 2004)):
        if not re.search(rf"news.*{season}.*{re.escape(sentence)}|{re.escape(sentence)}", log):
            fails.append(f"change: no news sentence {sentence!r}")
    return fails


def run_load(program, game, work):
    import fixtures
    import smoke_run
    code, _ = rrc.run(program, game, 2007, 2007, NOV_2007, ["England"], [], work, 3600, None,
                      {"RECOMP_SAVE_AFTER_DAYS": "240"})
    first = os.path.join(work, "user", smoke_run.SAVE_NAME)
    if code != 0 or not os.path.exists(first):
        return [f"load: the first run made no save (code {code}, {work})"]
    again = os.path.join(work, "again")
    user = os.path.join(again, "user")
    os.makedirs(user, exist_ok=True)
    shutil.copyfile(first, os.path.join(user, "uefa.sav"))
    dbname = os.path.basename(NOV_2007)
    shutil.copytree(os.path.join(work, "user", "Databases"), os.path.join(user, "Databases"))
    fixtures.write_settings(user, "classic", [])
    with open(os.path.join(user, "settings.txt"), "a") as f:
        f.write(f"database = folder:{dbname}\nfaster_days = on\n")
    report = os.path.join(again, "report.tsv")
    steps = fixtures.LOAD_STEPS + ["3600000 quit"]       # the loaded game goes on with its holiday
    r, _ = fixtures.run(program, game, "classic", user, smoke_run.scaled_steps("classic", again, steps),
                        {"RECOMP_SEASON_JUMP": "2007", "RECOMP_RULES_REPORT": report}, 3600)
    with open(os.path.join(again, "stderr.txt"), "w") as f:
        f.write(r.stderr)
    fails = [] if r.returncode == 0 else [f"load: the loaded game ended with code {r.returncode} ({again})"]
    fails += [f"load: message box {b}" for b in BOX.findall(r.stderr)]
    if os.path.exists(report):
        lines = uefa_lines(report)
        fails += expect(lines, 2007, "european_cup", CL_R16, "load")
        fails += expect(lines, 2007, "uefa_cup", UC_GROUPS, "load")
    else:
        fails.append(f"load: no report ({again})")
    return fails


RUNS = {"2007": run_2007, "change": run_change, "load": run_load}


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    jobs = int(rrc.arg(argv, "--jobs", 2))
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else tempfile.mkdtemp(prefix="cm0102-uefa-")
    fails = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        futures = {name: ex.submit(RUNS[name], program, game, os.path.join(keep, name)) for name in only}
        for name, fut in futures.items():
            got = fut.result()
            print(f"{name}: {'ok' if not got else f'{len(got)} problems'} ({os.path.join(keep, name)})")
            fails += got
    for text in fails:
        print("FAIL  " + text)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
