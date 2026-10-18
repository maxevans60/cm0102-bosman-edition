#!/usr/bin/env python3
"""Seasons of the club competitions outside UEFA (0.6.0 slice 26, part a): each competition of the original plays an
edition only in the seasons that the rules table holds it (docs/research/rules-0.6.0/club-world.tsv,
src/recomp_rt/win/season_kind_club_world.c).

Usage:
    python3 tests/seasons/club_world_check.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py with England, Argentina, Australia, Brazil and Japan
selected (a creator makes its objects only when a nation of its continent is selected, 0x6699d0). The runs:
    2001-on   the 3.9.68 data from 2001/02 with changing_rules on, to 2002/03: the Inter-American Cup and the
              Oceania Champions Cup wait from the new game, the Club World Championship waits for 2005, the Copa
              Mercosur, the Merconorte Cup and the Asian Cup Winners' Cup stop after 2001/02
    2003-on   October 2003 with changing_rules on, to 2005/06: the Oceania Champions Cup comes back in 2004/05, the
              Intercontinental Cup stops after 2004/05, the Club World Championship comes back in 2005/06
    2007      November 2007 with changing_rules off, to 2008/09: the start season rules at a new game, and the
              Club World Championship in two seasons in a row. (A 2005/06 start stops at the World Cup draw of
              December 2005 with the box "world_cup..cpp 1438", 0x92f2b1, before this slice: fewer than 32
              qualified nations. That is not a rule of this slice.)
    2023-on   October 2023 with changing_rules on, to 2024/25

The checks (each run):
    - the jump ends with code 0 and the report holds the start and end of each season (a stopped run fails)
    - for each object of the classes of the kind and each season S of the report: the object holds year S at the
      start or at the end of S exactly when the table holds the competition in S (an edition of year Y plays in
      game season Y, docs/research/2026-10-18-club-world-idle.md section 6)
    - changing_rules on: the news log of the season change (RECOMP_SEASON_CHANGE_LOG=1) has one sentence for
      each competition that the table starts or stops holding in a season of the run
A run takes about 6 to 12 minutes. --jobs (default 2) runs that many at the same time. --reuse checks the reports
already in the --keep folder again without a new run.
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
NATIONS = ["England", "Argentina", "Australia", "Brazil", "Japan"]
# name: (start year, database folder or None, last season, changing rules)
RUNS = {
    "2001-on": (2001, None, 2002, "on"),
    "2003-on": (2003, "October 2003", 2005, "on"),
    "2007": (2007, "November 2007", 2008, "off"),
    "2023-on": (2023, "October 2023", 2024, "on"),
}
FILES = ["world_club_champ", "world_club_cup", "inter_amer_cup", "con_merc_cup", "conmebol_merc", "asia_club_champ",
         "asia_cup_winner", "asia_super_cup", "oceania_club_champ"]


def folder(name):
    """The database folder whose name starts with the neutral name (some folder names add more words)."""
    if not name:
        return None
    for d in sorted(os.listdir(DB)):
        if d == name or d.startswith(name + " "):
            return os.path.join(DB, d)
    raise SystemExit(f"no database folder for {name} in {DB}")


def held(rows, file, season):
    """The rule of the file in season (after the last row the rule stays, Q248), or None with no row."""
    found = None
    for r in rows:
        if file in r["comp_file"].split(";") and r["season"] <= season and (found is None or r["season"] > found["season"]):
            found = r
    if found is None:
        return None
    return "not_held" not in (found["clubs"], found["format"])


def check(name, report, log, rules_on):
    failed = []
    seasons, comps = rrc.read_report(report)
    rows = rrc.make_rules.read_rows()
    start, last = RUNS[name][0], RUNS[name][2]
    got = [(s["season"], s["phase"]) for s in seasons]
    want = [(y, p) for y in range(start, last + 1) for p in ("start", "end")]
    if got != want:
        failed.append(f"{name}: the report holds {got[:2]}...{got[-2:]}, not the start and end of {start} to {last}")
    years = {}
    names = {}
    for c in comps:
        if c["file"] in FILES:
            years.setdefault((c["file"], c["slot"]), {}).setdefault(c["season"], set()).add(c["year"])
            names[c["file"]] = c["name"]
    found = {f for f, _ in years}
    for (file, slot), by_season in sorted(years.items()):
        for s in range(start, last + 1):
            table = held(rows, file, s)
            if table is None:
                continue
            game = s in by_season.get(s, set())
            line = f"{name}: {file} in {s}/{(s + 1) % 100:02d}: years {sorted(by_season.get(s, set()))}, table " \
                   f"{'holds it' if table else 'not held'}"
            print(("  ok    " if game == table else "  FAIL  ") + line)
            if game != table:
                failed.append(line)
    missing = [f for f in FILES if f not in found]
    print(f"  {name}: classes not in the report (no nation of their continent): {', '.join(missing) or 'none'}")
    if rules_on:
        text = open(log, encoding="latin-1").read() if os.path.exists(log) else ""
        sentences = re.findall(r"The (.+?) (is played again from the \d{4}/\d\d season|is no longer played|is not "
                               r"played in the \d{4}/\d\d season)\.", text)
        for file in sorted(found):
            for s in range(start + 1, last + 1):
                a, b = held(rows, file, s - 1), held(rows, file, s)
                if a is None or b is None or a == b:
                    continue
                hits = [x for x in sentences if x[0] == names.get(file)]
                ok = len(hits) >= 1
                print(f"  {'ok  ' if ok else 'FAIL'}  {name}: news for {file} ({names.get(file)}) in {s}: {hits}")
                if not ok:
                    failed.append(f"{name}: no news sentence for {file} in {s}")
    return failed


def one(program, game, name, keep, reuse):
    year, db, last, rules = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-club-world-{name}-")
    report = os.path.join(work, "report.tsv")
    log = os.path.join(work, "stderr.txt")
    if not reuse:
        code, report = rrc.run(program, game, last, year, folder(db), NATIONS, [], work, 7200,
                               options_more={"changing_rules": rules}, env_more={"RECOMP_SEASON_CHANGE_LOG": "1"})
        if code != 0:
            return [f"{name}: the run ended with code {code} ({log})"]
    if not os.path.exists(report):
        return [f"{name}: no report ({log})"]
    return check(name, report, log, rules == "on")


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only")
    names = only.split(",") if only else list(RUNS)
    jobs = int(rrc.arg(argv, "--jobs", 2))
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    reuse = "--reuse" in argv
    failed = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        for f in [pool.submit(one, program, game, n, keep, reuse) for n in names]:
            failed += f.result()
    for text in failed:
        print("FAIL  " + text)
    print("PASS" if not failed else f"FAILED: {len(failed)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
