#!/usr/bin/env python3
"""The Ekstraklasa in its real format of each season (GitHub issue 92, 0.6.2 open item 221).

Usage:
    python3 tests/seasons/pol_first_run.py <cm0102_recomp> <game folder> [--only 2002,2013,2025] [--keep <folder>]
        [--reuse] [--timeout <seconds>]

Each run starts a new game in a season with all nations (the rows of the Select League(s) screen differ by
database, so a click on Poland alone is not safe), changing rules on, and plays that one season
(RECOMP_SEASON_JUMP = the start year: the rules report of tests/seasons/rules_report_check.py, the end report of that
season, then the program ends). The format of the rules table (docs/research/rules-0.6.0/gre-pol-cro.tsv, pol_first):
    2002  Summer 2002 (own database): one table of 16 clubs, 30 rounds.
    2013  Autumn 2013: 16 clubs, 30 rounds, then the top 8 and the bottom 8 play 7 more, the points halved.
    2025  December 2025: one table of 18 clubs, 34 rounds.
The checks:
    report  the Polish First Division row of the start report (rules report, file pol_first) has the clubs of the row,
            each pair meeting twice, the places of the original's final table (none up, 2 to
            the play-off with the I liga's 3rd and 4th, 2 down); the end report has the same clubs, each with 2 (n - 1)
            matches of the first phase (+0xa7);
    log     RECOMP_POL_FIRST_LOG (src/recomp_rt/win/season_pol_first.c) names one table of n clubs, and in 2013 the
            split (8 and 8 clubs, 7 dates, points halved);
    box     no message box.
A run takes about 20 to 40 minutes; they run one after the other. --reuse checks the logs in --keep again.
Exit code 1 on a failure.
"""
import os
import re
import sys
import tempfile
import zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
import rules_report_check as rrc  # noqa: E402

DATABASES = "/Users/max/Projects/cm0102/test-fixtures/databases"
OWN_DBS = "/Users/max/Projects/cm0102/own-dbs"
# start year: (database, own zip or None, clubs, split: (top, bottom, dates, points) or None)
RUNS = {2002: ("Summer 2002", True, 16, None),
        2013: ("Autumn 2013", False, 16, (8, 8, 7, "halved")),
        2025: ("December 2025", False, 18, None)}


def database(year, work):
    name, own, _, _ = RUNS[year]
    if own:
        folder = os.path.join(work, "owndb")
        path = os.path.join(folder, name)
        if not os.path.isdir(path):
            with zipfile.ZipFile(os.path.join(OWN_DBS, name + ".zip")) as z:
                z.extractall(folder)
        return path
    names = sorted(n for n in os.listdir(DATABASES) if n.startswith(name))
    return rrc.data_folder(os.path.join(DATABASES, names[0]))


def report_rows(path, year):
    """{phase: row} of the pol_first rows of the season in the rules report."""
    out, cols = {}, None
    with open(path, encoding="latin-1") as f:
        for line in f:
            p = line.rstrip("\n").split("\t")
            if p[0] == "columns":
                cols = p[1:]
            elif cols and len(p) == len(cols) and p[cols.index("file")] == "pol_first" and \
                    p[cols.index("season")] == str(year):
                out[p[cols.index("phase")]] = dict(zip(cols, p))
    return out


def one(program, game, year, keep, reuse, timeout):
    _, _, clubs, split = RUNS[year]
    work = os.path.join(keep, str(year)) if keep else tempfile.mkdtemp(prefix=f"cm0102-pol-first-{year}-")
    report = os.path.join(work, "report.tsv")
    if not reuse:
        db = database(year, keep or work)
        code, report = rrc.run(program, game, year, year, db, ["all"], [], work, timeout,
                               {"changing_rules": "on"}, {"RECOMP_POL_FIRST_LOG": "1"})
        if code != 0:
            return [f"{year}: the run ended with code {code} ({work})"]
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    fails = [f"{year}: {x.strip()}" for x in log.split("\n") if "recomp: message box from" in x]
    rows = report_rows(report, year)
    want = 2 * (clubs - 1)
    start = rows.get("start")
    if not start:
        fails.append(f"{year}: no Polish First Division row in the start report")
    else:
        got = (int(start["clubs"]), int(start["rounds"]))
        if got != (clubs, 2):
            fails.append(f"{year}: start report clubs and meetings {got}, want {(clubs, 2)}")
        places = tuple(int(start[k]) for k in ("up", "playoff_up", "playoff_down", "down"))
        if places != (0, 0, 2, 2):
            fails.append(f"{year}: start report places {places}, want (0, 0, 2, 2)")
    end = rows.get("end")
    if not end:
        fails.append(f"{year}: no Polish First Division row in the end report")
    elif (int(end["clubs"]), int(end["matches"])) != (clubs, want):
        fails.append(f"{year}: end report clubs and matches {(int(end['clubs']), int(end['matches']))}, want "
                     f"{(clubs, want)}")
    tag = f"{year}/{(year + 1) % 100:02d}"
    if f"recomp: pol first: {tag}: one table of {clubs} clubs" not in log:
        fails.append(f"{year}: the log names no one table of {clubs} clubs")
    if split:
        top, bottom, dates, points = split
        line = f"recomp: pol first: {tag}: split after 30 rounds, {top} and {bottom} clubs, {dates} dates, points {points}"
        if line not in log:
            got = re.findall(r"recomp: pol first: .*split after.*", log)
            fails.append(f"{year}: no split line '{line}' ({got})")
    elif "split after 30 rounds," in log:
        fails.append(f"{year}: a split in a season of one table")
    print(("FAIL  " + "; ".join(fails)) if fails else f"ok    {year}: the Ekstraklasa of {tag} ({work})")
    return fails


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    program, game = argv[0], argv[1]
    only = [int(y) for y in argv[argv.index("--only") + 1].split(",")] if "--only" in argv else sorted(RUNS)
    keep = argv[argv.index("--keep") + 1] if "--keep" in argv else None
    timeout = int(argv[argv.index("--timeout") + 1]) if "--timeout" in argv else 7200
    fails = []
    for y in only:
        fails += one(program, game, y, keep, "--reuse" in argv, timeout)
    print(("FAIL  " + f"{len(fails)} failures") if fails else f"ok    pol_first_run: {len(only)} seasons")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
