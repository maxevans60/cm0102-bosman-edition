#!/usr/bin/env python3
"""International competitions check (0.6.0 slice 27): the editions, hosts and golden goal of the finals in a
season jump.

Usage:
    python3 tests/seasons/season_intl_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py (England, the quick result engine) with the
test log RECOMP_SEASON_INTL_LOG=1. The runs:
    2003    October 2003, 2003/04 to 2004/05, changing rules on (Euro 2004, no golden goal from 2004/05)
    2005    September 2005, 2005/06 to 2006/07, changing rules on (World Cup 2006, Australia to Asia)
    2017    October 2017, 2017/18 to 2018/19, changing rules on (the last Confederations Cup lies before)
    2019    October 2019, 2019/20 to 2020/21, changing rules on (Euro 2020 and Copa America 2020 at their
            planned time, the Tokyo Olympics in 2020, Q277, Q366)
    off     October 2017 with changing rules off: the first editions of the table, then the original's steps

The checks (each run):
    - the jump ends with code 0 and the report holds the end of the last season
    - every year that the game holds for a finals class (world_cup, euro_champ, fifa_confed, olympics,
      goldcup, african_nations, asia_nations, oceania_nations) at the first report is the first edition of
      the table from the start season (or the waiting year 2200 when the table has none)
    - on: when the year of a class changes between two reports, the new year is the next edition of the
      table (tools/seasons/make_intl.py); off: the old year plus the original's step
    - on: each host row that the runtime writes names the first two hosts of the table (log "host")
A run takes about 6 to 8 minutes. --jobs (default 2) runs that many at the same time. --reuse checks the
reports already in the --keep folder again without a new run.
"""
import concurrent.futures
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import make_intl  # noqa: E402
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
RUNS = {
    "2003": (2003, "October 2003", "on"),
    "2005": (2005, "September 2005", "on"),
    "2017": (2017, "October 2017", "on"),
    "2019": (2019, "October 2019", "on"),
    "off": (2017, "October 2017", "off"),
}
STEPS = {"world_cup": 4, "euro_champ": 4, "fifa_confed": 2, "olympics": 4, "goldcup": 2, "african_nations": 2,
         "asia_nations": 4, "oceania_nations": 2}
IDLE = 2200
BASE_ENV = {"RECOMP_STAFF_RESERVE": "23000", "RECOMP_SEASON_INTL_LOG": "1"}
HOST_LOG = re.compile(r"recomp: season intl: host (\w+) (\d+): (.*)$")


def database_folder(name):
    """The unpacked folder of a season database by its neutral name: some folder names add more words after it,
    which no committed file names."""
    hits = sorted(d for d in os.listdir(DB) if d == name or d.startswith(name + " "))
    if not hits:
        raise SystemExit(f"season_intl_run.py: no database folder {name!r} in {DB}")
    return os.path.join(DB, hits[0])


def table():
    out = {}
    for e in make_intl.editions():
        out.setdefault(e["file"], []).append(e)
    return out


def first(eds, start):
    return next((e["year"] for e in eds if e["season"] >= start), IDLE)


def nxt(eds, year):
    return next((e["year"] for e in eds if e["year"] > year), IDLE if year < IDLE else year)


def check(name, start, rules, work):
    report = os.path.join(work, "report.tsv")
    if not os.path.exists(report):
        return [f"{name}: no report ({work})"]
    seasons, comps = rrc.read_report(report)
    fails = []
    if not seasons or seasons[-1]["season"] != start + 1 or seasons[-1]["phase"] != "end":
        fails.append(f"{name}: the report does not end with the end of {start + 1}/{(start + 2) % 100:02d}")
    eds = table()
    years = {}                                  # file -> the years of the reports, in order
    for c in comps:
        if c["file"] in STEPS:
            years.setdefault(c["file"], [])
            if not years[c["file"]] or years[c["file"]][-1] != c["year"]:
                years[c["file"]].append(c["year"])
    for f in STEPS:
        seen = years.get(f)
        if not seen:
            fails.append(f"{name}: {f} is not in the report")
            continue
        want = first(eds[f], start)
        if seen[0] != want:
            fails.append(f"{name}: {f} holds {seen[0]} at the start, the table's first edition is {want}")
        for a, b in zip(seen, seen[1:]):
            want = nxt(eds[f], a) if rules == "on" else a + STEPS[f]
            if b != want:
                fails.append(f"{name}: {f} goes from {a} to {b}, expected {want}")
    if rules == "on":
        log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read().split("\n")
        hosts = [m.groups() for m in (HOST_LOG.search(x) for x in log) if m]
        if not hosts:
            fails.append(f"{name}: no host row in the log")
        for f, y, names in hosts:
            e = next((e for e in eds.get(f, []) if e["year"] == int(y)), None)
            # A host row holds two hosts (HostCountry pair 1): the first two of the table.
            want = ";".join(h.split("/")[0] for h in e["hosts"][:2]) if e else None
            got = ";".join(h.split("/")[0] for h in names.split(";")) if names != "-" else ""
            if want is not None and got != want:
                fails.append(f"{name}: host row {f} {y}: {names}, the table gives {want}")
    return fails


def one(program, game, name, keep, reuse):
    start, database, rules = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-season-intl-{name}-")
    if not reuse:
        code, _ = rrc.run(program, game, start + 1, start, database_folder(database), ["England"], [], work, 7200,
                          {"changing_rules": rules}, BASE_ENV)
        if code != 0:
            return [f"{name}: the run ended with code {code} ({work})"]
    return check(name, start, rules, work)


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = argv[argv.index("--only") + 1].split(",") if "--only" in argv else list(RUNS)
    jobs = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 2
    keep = os.path.abspath(argv[argv.index("--keep") + 1]) if "--keep" in argv else None
    reuse = "--reuse" in argv
    fails = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        for r in pool.map(lambda n: one(program, game, n, keep, reuse), only):
            fails += r
    for f in fails:
        print("FAIL  " + f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
