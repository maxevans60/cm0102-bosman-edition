"""The new club cups of slice 26c (src/recomp_rt/win/season_club_cups.c): the Copa Sudamericana, the Recopa
Sudamericana, the Suruga Bank Championship, the FIFA Intercontinental Cup and the AFC Champions League Two, through
the new competitions module (season_comps.h).

Usage:
    python3 tests/seasons/club_cups_check.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py with England, Argentina, Australia, Brazil and Japan
selected (entry 5 needs a South American nation, entry 6 an Asian one, 0x6699d0), with changing_rules on, and
RECOMP_SEASON_CHANGE_LOG=1. The runs and the editions each must show:
    2002-on   the 3.9.68 data from 2002/03 to 2003/04: the Copa Sudamericana of 2002 and 2003 (21 and 35 clubs,
              finals in December), the Recopa of 2003 (2 clubs, 12 July 2003)
    2010-on   September 2010 to 2011/12: the Suruga Bank Championship of 2011 (2 clubs, 3 August 2011), the
              Recopa of 2011, the Copa Sudamericana of 2010 (39 clubs)
    2016-on   October 2016, 2016/17 only (a later World Cup draw stops the run until slice 27): the Recopa and the
              Copa Sudamericana of 2016 (47 clubs), the Sudamericana of 2017 made with 44 clubs
    2024-on   October 2024 to 2025/26: the FIFA Intercontinental Cup of 2024 (6 clubs, final 18 December 2024),
              the AFC Champions League Two of 2024/25 (32 clubs, final May 2025), the Recopa and the Copa
              Sudamericana of 2025
    2001      the base game (2001/02, changing rules off) to 2002/03: no new cup at all (Q152, Q342)
    2008-all  September 2008, 2008/09 only, Select All but France (a known stop at "fra_first 108"): the CONCACAF Champions Cup of 24 clubs (the original
              plays 8; the United States, entry 4, needs Select All). It takes 2 hours or more

Load runs (LOAD_RUNS below): 2010-load (inside the Sudamericana), 2024-load-acl (inside ACL Two), 2024-load-cwc (inside
the groups of the 32-club Club World Cup) and 2008-load-all (inside the CONCACAF Champions Cup of 24 clubs, Select
All): each saves inside a running edition, loads and plays to its winner with no error box.

The checks (each run): the jump ends with code 0; each listed edition logs its club list with the table's club
count ("club cup <file> <year>: <n> clubs, <r> rounds"), and its winner ("winner") when the run passes its final;
each draw of the Asian Club Championship adds Japanese places (step A1, the table's "places: JPN n");
each CONMEBOL seeding logs 32 Libertadores places (step L1: the places by nation of the table's row, the
nations short of clubs filled from the others); the base run logs no "club cup", no "new competition new_" and no
"libertadores places" line. A run takes about 6 to 15 minutes.
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
# name: (start year, database folder or None, last season, changing rules, editions)
# An edition: (file, year, clubs, winner expected)
RUNS = {
    "2002-on": (2002, None, 2003, "on", [("new_sudamericana", 2002, 21, True), ("new_sudamericana", 2003, 35, True),
                                         ("new_recopa", 2003, 2, True)]),
    "2010-on": (2010, "September 2010 ODB", 2011, "on", [("new_suruga_bank", 2011, 2, False),
                                                        ("new_recopa", 2011, 2, True),
                                                        ("new_sudamericana", 2010, 39, True)]),
    "2016-on": (2016, "October 2016", 2016, "on", [("new_recopa", 2016, 2, True), ("new_sudamericana", 2016, 47, True),
                                                   ("new_sudamericana", 2017, 44, False)]),
    "2024-on": (2024, "October 2024", 2025, "on", [("new_fifa_intercontinental_cup", 2024, 6, True),
                                                   ("new_acl_two", 2024, 32, True), ("new_recopa", 2025, 2, True),
                                                   ("new_sudamericana", 2025, 44, True)]),
    "2001": (2001, None, 2002, "off", []),
    # The CONCACAF Champions Cup's club count (Con_Champ, 8 in the original): 24 in 2008/09. Select All: the
    # United States (entry 4) is below row 16 of Select League(s). Long (Select All).
    "2008-all": (2008, "September 2008", 2008, "on", [("concacaf", 2008, 24, False)]),
}
RUN_NATIONS = {"2008-all": ["all", "France"]}   # France with September 2008 stops at fra_first 108 (known)

# Load runs (reviews of 85c04f1): the first run saves after `save` game days, inside a running edition; a new
# program loads that save and plays `play` more days. The loaded game must make the new cup again from its save
# (when `made` is set), log each `want` line, and show no error box. Brazil is not selected: until slice 18 is on
# main a season game with Brazil cannot save (the accented names of its state championships, "league..cpp 5095").
# name: (start year, database, nations, save days, play days, made, [regex of each wanted line after the load])
LOAD_RUNS = {
    # Start 10.07.2010; saved 18.10.2010 inside the Sudamericana (3 August to 8 December 2010); played to
    # 27.01.2011, after its season start of 10 January 2011, which logs the winner.
    "2010-load": (2010, "September 2010 ODB", ["England", "Argentina", "Japan"], 100, 100, "new_sudamericana",
                  [r"club cup new_sudamericana 2010: winner "]),
    # Start 13.07.2024; saved 21.10.2024 inside ACL Two (17 September 2024 to 18 May 2025); played to 16.09.2025,
    # after its season start of 28 August 2025.
    "2024-load-acl": (2024, "October 2024", ["England", "Argentina", "Australia", "Japan"], 100, 330, "new_acl_two",
                      [r"club cup new_acl_two 2024: winner "]),
    # Saved 20.06.2025 inside the groups of the 32-club Club World Cup (14 to 24 June 2025); played to 30.07.2025:
    # places 1 and 2 of the knockout (final 13 July).
    "2024-load-cwc": (2024, "October 2024", ["England", "Argentina", "Australia", "Japan"], 342, 40, None,
                      [r"club world knockout 2024: place 1 ", r"club world knockout 2024: place 2 "]),
    # Select All but France and Brazil (the save fault above). Start 12.07.2008; saved 02.10.2008 inside the CONCACAF Champions Cup of 24 clubs (28
    # September to 10 October); played to 01.03.2009, after its season start of 21 February 2009.
    "2008-load-all": (2008, "September 2008", ["all", "France", "Brazil"], 82, 150, None,
                      [r"concacaf champions 2008: winner "]),
}


def folder(name):
    if not name:
        return None
    for d in sorted(os.listdir(DB)):
        if d == name or d.startswith(name + " "):
            return os.path.join(DB, d)
    raise SystemExit(f"no database folder for {name} in {DB}")


def check(name, log):
    text = open(log, encoding="latin-1").read() if os.path.exists(log) else ""
    failed = []
    editions = RUNS[name][4]
    if not editions:
        hits = re.findall(r"recomp: (club cup .*|new competition new_.*|libertadores places.*)", text)
        print(f"  {'ok  ' if not hits else 'FAIL'}  {name}: base game, new cup lines: {hits[:3]}")
        if hits:
            failed.append(f"{name}: the base game has new cup lines: {hits[:3]}")
        return failed
    for file, year, clubs, winner in editions:
        if file == "concacaf":
            made = re.findall(rf"concacaf champions {year}: (\d+) clubs, (\d+) rounds", text)
        else:
            made = re.findall(rf"club cup {file} {year}: (\d+) clubs, (\d+) rounds", text)
        ok = bool(made) and int(made[-1][0]) == clubs
        print(f"  {'ok  ' if ok else 'FAIL'}  {name}: {file} {year}: rounds of {made[-1] if made else None} clubs, "
              f"table {clubs}")
        if not ok:
            failed.append(f"{name}: {file} {year}: {made[-1] if made else 'no rounds'}, want {clubs} clubs")
        if winner:
            w = re.findall(rf"club cup {file} {year}: winner (.+)", text)
            print(f"  {'ok  ' if w else 'FAIL'}  {name}: {file} {year}: winner {w}")
            if not w:
                failed.append(f"{name}: {file} {year}: no winner")
    # Step L1: each CONMEBOL seeding gives exactly 32 Libertadores places (collect_entrants 0x4c3f40 needs 32).
    places = [int(n) for n in re.findall(r"libertadores places: (\d+) clubs", text)]
    ok = bool(places) and all(n == 32 for n in places)
    print(f"  {'ok  ' if ok else 'FAIL'}  {name}: Libertadores places at each seeding: {places}")
    if not ok:
        failed.append(f"{name}: Libertadores places {places}, want 32 at each seeding")
    # Step A1: the AFC places of Japan (selected in these runs) from the table: more than the original's one club.
    afc = [(int(y), int(j)) for y, j in re.findall(r"asian champions (\d+): places added JPN (\d+)", text)]
    if "Japan" in RUN_NATIONS.get(name, NATIONS) and name != "2001":
        ok = bool(afc) and all(j >= 1 for _, j in afc)
        print(f"  {'ok  ' if ok else 'FAIL'}  {name}: AFC places added for Japan (year, clubs): {afc}")
        if not ok:
            failed.append(f"{name}: AFC places for Japan {afc}, want at least one added at each draw")
    boxes = re.findall(r"recomp: message box.*", text)
    if boxes:
        failed.append(f"{name}: message boxes: {boxes[:2]}")
    return failed


def load_run(program, game, name, keep):
    """A run of LOAD_RUNS (see above)."""
    import shutil
    sys.path.insert(0, HERE)
    import club_world_check as cwc
    year, db, nations, save_days, play_days, made, wants = LOAD_RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-club-cups-{name}-")
    code, _ = rrc.run(program, game, year + 1, year, folder(db), nations, [], work, 14400,
                      options_more={"changing_rules": "on"}, env_more={"RECOMP_SAVE_AFTER_DAYS": str(save_days)})
    first = os.path.join(work, "user", "samesave.sav")
    if code != 0 or not os.path.exists(first):
        return [f"{name}: the first run made no save (code {code}, {work})"]
    again = os.path.join(work, "again")
    os.makedirs(again, exist_ok=True)
    shutil.copyfile(first, os.path.join(again, "cups.sav"))
    r, saved = cwc.load_and_play(program, game, os.path.join(again, "cups.sav"), again, play_days, timeout=7200)
    fails = []
    if r.returncode != 0 or not saved:
        fails.append(f"{name}: the save did not load and play on (code {r.returncode}, {again})")
    if made and not re.search(rf"new competition {made} made for \d+ .*from the save", r.stderr):
        fails.append(f"{name}: {made} was not made again from the save ({again}/stderr.txt)")
    for w in wants:
        hit = re.search(w + r".*", r.stderr)
        print(f"  {'ok  ' if hit else 'FAIL'}  {name}: after the load: {hit.group(0) if hit else w}")
        if not hit:
            fails.append(f"{name}: no line {w!r} after the load ({again}/stderr.txt)")
    if "message box" in r.stderr:
        fails.append(f"{name}: a message box after the load ({again}/stderr.txt)")
    return fails


def one(program, game, name, keep, reuse):
    if name in LOAD_RUNS:
        return load_run(program, game, name, keep)
    year, db, last, rules, _ = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-club-cups-{name}-")
    log = os.path.join(work, "stderr.txt")
    if not reuse:
        code, _ = rrc.run(program, game, last, year, folder(db), RUN_NATIONS.get(name, NATIONS), [], work, 14400,
                          options_more={"changing_rules": rules}, env_more={"RECOMP_SEASON_CHANGE_LOG": "1"})
        if code != 0:
            return [f"{name}: the run ended with code {code} ({log})"] + check(name, log)
    return check(name, log)


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only")
    names = only.split(",") if only else list(RUNS) + list(LOAD_RUNS)
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
