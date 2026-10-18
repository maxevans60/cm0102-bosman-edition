#!/usr/bin/env python3
"""Seasons of the club competitions outside UEFA (0.6.0 slice 26, part a): each competition of the original plays an
edition only in the seasons that the rules table holds it (docs/research/rules-0.6.0/club-world.tsv,
src/recomp_rt/win/season_kind_club_world.c).

Usage:
    python3 tests/seasons/club_world_check.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse] [--own-dbs <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py with England, Argentina, Australia, Brazil and Japan
selected (a creator makes its objects only when a nation of its continent is selected, 0x6699d0). The runs:
    2001-on   the 3.9.68 data from 2001/02 with changing_rules on, to 2002/03: the Inter-American Cup and the
              Oceania Champions Cup wait from the new game, the Club World Championship waits for 2005, the Copa
              Mercosur, the Merconorte Cup and the Asian Cup Winners' Cup stop after 2001/02
    2004-on   Summer 2004 (our own database: --own-dbs names a folder that holds it unpacked, made from
              Summer 2004.zip of the release input) with changing_rules on, to 2005/06, with England only
              (entry 2): the Intercontinental Cup stops after 2004/05, and the Club World Championship comes
              back in 2005/06. (A 2003/04 start with October 2003 stops on 7 February 2004 at the box
              "comp_stats..CPP 1664", 0x4a25a8, with England alone too: Comp_Stats::set_club_final_position
              0x4a2540 gets no club. The build without this slice stops there too. Which competition calls it is
              unknown.)
    2007      November 2007 with changing_rules off, to 2008/09: the start season rules at a new game, and the
              Club World Championship in two seasons in a row. (A 2005/06 start stops at the World Cup draw of
              December 2005 with the box "world_cup..cpp 1438", 0x92f2b1, before this slice: fewer than 32
              qualified nations. That is not a rule of this slice.)
    2007-load November 2007 with changing_rules off: the game saves 24 days after the start (7 August 2007, after
              the quarter-finals of the 7-club knockout), a new program loads that save, returns from holiday
              and plays 10 days (the semi-finals on 9 August, the final and the third place match on 12 August).
              The loaded game must end the knockout with places 1 to 3 (the test log of the history thunk) and
              save again. It needs no Select League(s) nations of other continents: entry 2 is made for every game
    2024-on   October 2024 with changing_rules on, to 2025/26: the Club World Championship of 2024/25 is the
              last in the table. (A 2023/24 start with October 2023 stops in February 2024 at the same
              comp_stats box as a 2003/04 start, also without this slice.)

The checks (each run):
    - the jump ends with code 0 and the report holds the start and end of each season (a stopped run fails)
    - for each object of the classes of the kind and each season S of the report: the object holds year S at the
      start or at the end of S exactly when the table holds the competition in S (an edition of year Y plays in
      game season Y, docs/research/2026-10-18-club-world-idle.md section 6)
    - changing_rules on: the news log of the season change (RECOMP_SEASON_CHANGE_LOG=1) has one sentence for
      each competition that the table starts or stops holding in a season of the run
    - part b: each season that the table holds the Club World Championship with 6 or 7 clubs plays it as a
      knockout of that many clubs (the log line of season_kind_club_world.c); with changing rules off, the club
      count of the first edition
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
# name: (start year, database folder or None, last season, changing rules, nations)
RUNS = {
    "2001-on": (2001, None, 2002, "on", NATIONS),
    "2004-on": (2004, "Summer 2004", 2005, "on", ["England"]),
    "2007": (2007, "November 2007", 2008, "off", NATIONS),
    "2024-on": (2024, "October 2024", 2025, "on", NATIONS),
    "2007-load": (2007, "November 2007", 2007, "off", ["England"]),
}
FILES = ["world_club_champ", "world_club_cup", "inter_amer_cup", "con_merc_cup", "conmebol_merc", "asia_club_champ",
         "asia_cup_winner", "asia_super_cup", "oceania_club_champ"]


def folder(name, own=None):
    """The database folder whose name starts with the neutral name (some folder names add more words), in the
    test databases or in the folder of our own databases."""
    if not name:
        return None
    for top in [DB] + ([own] if own else []):
        for d in sorted(os.listdir(top)):
            if d == name or d.startswith(name + " "):
                return os.path.join(top, d)
    raise SystemExit(f"no database folder for {name} in {DB} or {own}")


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
    # Part b: an edition of 6 or 7 clubs in the table is a knockout (season_kind_club_world.c, Form K). The form
    # of the first edition stays with changing rules off (Q232).
    text = open(log, encoding="latin-1").read() if os.path.exists(log) else ""
    made = {int(y): int(n) for n, y in re.findall(r"club world knockout of (\d+) clubs for (\d+)", text)}
    if "world_club_champ" in found:
        first = next((y for y in range(start, 2100) if held(rows, "world_club_champ", y)), None)
        for s in range(start, last + 1):
            if not held(rows, "world_club_champ", s):
                continue
            row = next((r for r in rows if r["season"] == (s if rules_on else first) and
                        "world_club_champ" in r["comp_file"].split(";")), None)
            want = row["clubs"] if row and row["clubs"] in (6, 7) else None
            ok = made.get(s) == want
            print(f"  {'ok  ' if ok else 'FAIL'}  {name}: Club World Championship {s}/{(s + 1) % 100:02d}: "
                  f"knockout of {made.get(s)} clubs, table {want}")
            if not ok:
                failed.append(f"{name}: Club World Championship {s}: knockout of {made.get(s)}, want {want}")
    if rules_on:
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


def load_and_play(program, game, save, out, days):
    """Loads save, lets the game go on with its holiday (a loaded game keeps it) and saves `days` game days after
    the load (RECOMP_SAVE_AFTER_DAYS ends the program). Returns the result and the path of the new save, or None."""
    sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
    import fixtures
    import smoke_run
    user = os.path.join(out, "user")
    os.makedirs(user, exist_ok=True)
    os.replace(save, os.path.join(user, os.path.basename(save)))
    fixtures.write_settings(user, "classic", [])
    steps = fixtures.LOAD_STEPS[:-1] + ["900000 quit"]
    table = os.path.join(os.path.dirname(os.path.abspath(program)), "seasons", "rules.tsv")
    r, _ = fixtures.run(program, game, "classic", user, smoke_run.scaled_steps("classic", out, steps),
                        {"RECOMP_SAVE_AFTER_DAYS": str(days), "RECOMP_RULES_TABLE": table}, 1200)
    with open(os.path.join(out, "stderr.txt"), "w") as f:
        f.write(r.stderr)
    saved = os.path.join(user, smoke_run.SAVE_NAME)
    return r, saved if os.path.exists(saved) else None


def knockout_load(program, game, keep):
    """The 2007-load run (module docstring)."""
    import shutil
    work = os.path.join(keep, "2007-load") if keep else tempfile.mkdtemp(prefix="cm0102-club-world-load-")
    code, _ = rrc.run(program, game, 2007, 2007, folder("November 2007"), ["England"], [], work, 7200,
                      options_more={"changing_rules": "off"}, env_more={"RECOMP_SAVE_AFTER_DAYS": "24"})
    first = os.path.join(work, "user", "samesave.sav")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    if code != 0 or not os.path.exists(first):
        return [f"2007-load: the first run made no save (code {code}, {work})"]
    if "club world knockout of 7 clubs for 2007" not in log or "club world knockout 2007: place" in log:
        return [f"2007-load: the save is not inside the knockout ({work}/stderr.txt)"]
    again = os.path.join(work, "again")
    os.makedirs(again, exist_ok=True)
    shutil.copyfile(first, os.path.join(again, "knockout.sav"))
    r, saved = load_and_play(program, game, os.path.join(again, "knockout.sav"), again, 10)
    places = sorted(set(re.findall(r"club world knockout 2007: place (\d) (.+)", r.stderr)))
    print(f"  2007-load: places after the load: {places}")
    fails = []
    if r.returncode != 0 or not saved:
        fails.append(f"2007-load: the save did not load and play on (code {r.returncode}, {again})")
    if [p for p, _ in places] != ["1", "2", "3"]:
        fails.append(f"2007-load: the knockout did not end with places 1 to 3 after the load ({again}/stderr.txt)")
    if "message box" in r.stderr:
        fails.append(f"2007-load: a message box after the load ({again}/stderr.txt)")
    return fails


def one(program, game, name, keep, reuse, own):
    if name == "2007-load":
        return knockout_load(program, game, keep)
    year, db, last, rules, nations = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-club-world-{name}-")
    report = os.path.join(work, "report.tsv")
    log = os.path.join(work, "stderr.txt")
    if not reuse:
        code, report = rrc.run(program, game, last, year, folder(db, own), nations, [], work, 7200,
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
    own = os.path.abspath(rrc.arg(argv, "--own-dbs")) if "--own-dbs" in argv else None
    failed = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        for f in [pool.submit(one, program, game, n, keep, reuse, own) for n in names]:
            failed += f.result()
    for text in failed:
        print("FAIL  " + text)
    print("PASS" if not failed else f"FAILED: {len(failed)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
