#!/usr/bin/env python3
"""Right ages and dates in every season (0.6.0 slice 11b, Q344).

Usage:
    python3 tests/seasons/season_dates_run.py <cm0102_recomp> <game folder> <folder of databases>
        [--only <season>,<season>] [--jobs n] [--extras] [--no-base]

<folder of databases> holds the Update Team databases, one folder each (test-fixtures/databases). The test
finds each database by its content (databases-signatures.tsv) and copies it into Databases of a new user
folder under its neutral name.

For each season database (and the 3.9.68 data on the 2002/03 tile) a new game starts in its season
(RECOMP_START_SEASON) and plays to the first news screen. The test log (RECOMP_DATES_LOG, season_dates.c)
gives the data in memory at the first human turn. The checks:
    people     at least five well-known people of that database have their real year of birth, and the age
               that the game keeps (+0x18) is the real age on the day of the first turn.
    champions  the last row of the English Premier Division history is the season before the start, with the
               real champions (for example 2019 Manchester City in a 2019/20 game).
Also:
    base       October 2019 in a game that starts in 2001/02: nothing moves (the database plays as in 0.5.0).
    off        October 2019 in 2019/20 with RECOMP_SEASON_CODE=off: nothing moves.
    again      the 2010/11 game twice: the saves are byte for byte the same.
With --extras the extra databases of each season run too (people and champions of their data season).
At most --jobs (3) games run at the same time.
"""
import concurrent.futures
import csv
import datetime
import os
import re
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import measure_date_shifts as mds  # noqa: E402
import season_core_run as scr  # noqa: E402

# Well-known people with their real dates of birth (day, month, year). Names in plain letters only.
PEOPLE = {
    "Alex Ferguson": (31, 12, 1941), "Ryan Giggs": (29, 11, 1973), "Thierry Henry": (17, 8, 1977),
    "Frank Lampard": (20, 6, 1978), "Gianluigi Buffon": (28, 1, 1978), "Steven Gerrard": (30, 5, 1980),
    "John Terry": (7, 12, 1980), "Iker Casillas": (20, 5, 1981), "Wayne Rooney": (24, 10, 1985),
    "Sergio Ramos": (30, 3, 1986), "Lionel Messi": (24, 6, 1987), "Gareth Bale": (16, 7, 1989),
    "Mohamed Salah": (15, 6, 1992), "Harry Kane": (28, 7, 1993), "Erling Haaland": (21, 7, 2000),
    "Bukayo Saka": (5, 9, 2001), "Jude Bellingham": (29, 6, 2003),
}
# English champions by the year the season ends.
CHAMPIONS = {
    2002: "Arsenal", 2003: "Manchester United", 2004: "Arsenal", 2005: "Chelsea", 2006: "Chelsea",
    2007: "Manchester United", 2008: "Manchester United", 2009: "Manchester United", 2010: "Chelsea",
    2011: "Manchester United", 2012: "Manchester City", 2013: "Manchester United", 2014: "Manchester City",
    2015: "Chelsea", 2016: "Leicester City", 2017: "Chelsea", 2018: "Manchester City", 2019: "Manchester City",
    2020: "Liverpool", 2021: "Manchester City", 2022: "Manchester City", 2023: "Manchester City",
    2024: "Manchester City", 2025: "Liverpool",
}
COMP = "English Premier Division"
MIN_PEOPLE = 5


def databases(folder):
    """{neutral name: (signature row, folder)} of the known databases in the folder."""
    sigs = mds.signatures()
    out = {}
    for sub in sorted(os.listdir(folder)):
        p = os.path.join(folder, sub)
        if os.path.isdir(p):
            r = mds.identify(p, sigs)
            if r and r["data_season"] != "none":
                out[r["database"]] = (r, p)
    return out


def season_rows():
    with open(os.path.join(mds.RULES, "databases-signatures.tsv"), newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def neutral_copy(work, name, src):
    """A folder named by the neutral name, holding the files of src (the name of src may hold a person's)."""
    dest = os.path.join(work, "src", name)
    if not os.path.isdir(dest):
        scr.cdr.copy_data(src, dest)
    return dest


def play(program, game, work, label, year, db_folder, env=None):
    """As season_core_run.play, with the log read as Latin-1: the game's own log lines may hold the letters of
    code page 1252 (club names)."""
    env = dict(env or {}, RECOMP_START_SEASON=str(year),
               RECOMP_DATES_LOG=";".join(list(PEOPLE) + ["comp:" + COMP]))
    steps = [s + " 600000" if s in ("wait typing", "wait first-news") else s for s in scr.new_game_steps()]
    run = os.path.join(work, label)
    user, home, shots = (os.path.join(run, d) for d in ("user", "home", "shots"))
    for d in (user, home, shots):
        os.makedirs(d)
    settings = "display = classic\n"
    if db_folder:
        scr.cdr.copy_data(db_folder, os.path.join(user, "Databases", os.path.basename(db_folder)))
        settings += f"database = folder:{os.path.basename(db_folder)}\n"
    with open(os.path.join(user, "settings.txt"), "w") as f:
        f.write(settings)
    full = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
                RECOMP_USER_DIR=user, RECOMP_FIXED_CLOCK="1", RECOMP_SAVE_AFTER_DAYS="0", RECOMP_SCREEN_LOG="1",
                RECOMP_SCRIPT="; ".join(scr.smoke_run.scaled_steps("classic", shots, steps)), **env)
    start = time.time()
    try:
        r = subprocess.run([program, "--game", game, "-seed", "1"], env=full, capture_output=True, timeout=1500)
    except subprocess.TimeoutExpired as e:
        return None, (e.stderr or b"").decode("latin-1"), time.time() - start
    log = r.stderr.decode("latin-1")
    path = os.path.join(user, scr.smoke_run.SAVE_NAME)
    return (path if r.returncode == 0 and os.path.exists(path) else None), log, time.time() - start


def parse(log):
    m = re.search(r"dates log: date (-?\d+)/(\d+), start year (\d+), shifts (.*)", log)
    if not m:
        return None
    day, year = int(m.group(1)), int(m.group(2))
    out = {"date": (day, year), "shifts": m.group(4).strip(), "people": {}, "comp": None}
    careers = {}
    for p in re.finditer(r"dates log: career (.+?) born (\d+)\.(\d+): (\d+) to (\d+)", log):
        careers.setdefault((p.group(1), int(p.group(2)), int(p.group(3))), (int(p.group(4)), int(p.group(5))))
    for p in re.finditer(r"dates log: person (.+?): born (\d+)\.(\d+)\.(\d+), age (-?\d+), joined (\d+), "
                         r"contract (\d+), club (.*)", log):
        born = (int(p.group(2)), int(p.group(3)), int(p.group(4)))
        out["people"].setdefault(p.group(1), []).append(
            dict(born=born, age=int(p.group(5)), joined=int(p.group(6)), contract=int(p.group(7)),
                 club=p.group(8).strip(), career=careers.get((p.group(1), born[0], born[1]), (0, 0))))
    c = re.search(r"dates log: competition " + re.escape(COMP) + r": (\d+) (.*?); (\d+) ", log)
    if c:
        out["comp"] = (int(c.group(1)), c.group(2))
    return out


def game_date(day, year):
    """The game date: the day of the year is 0 on 1 January (include/date.h, Date.cpp)."""
    return datetime.date(year, 1, 1) + datetime.timedelta(days=day)


def age_on(born, when):
    d, m, y = born
    return when.year - y - ((when.month, when.day) < (m, d))


def check_people(res):
    """(texts of the people that are right, texts of those that are wrong)."""
    when = game_date(*res["date"])
    good, bad = [], []
    for name, (d, m, y) in PEOPLE.items():
        rows = [r for r in res["people"].get(name, []) if r["born"][:2] == (d, m)]
        if not rows:
            continue
        r = rows[0]
        want_age = age_on((d, m, y), when)
        text = (f"{name} born {r['born'][2]} age {r['age']} ({r['club']}, joined {r['joined']}, contract "
                f"{r['contract']}, career {r['career'][0]}-{r['career'][1]})")
        if r["born"][2] == y and r["age"] == want_age:
            good.append(text)
        else:
            bad.append(f"{text}: real {y}, age {want_age}")
    return good, bad


def main(argv):
    if len(argv) < 4:
        print(__doc__)
        return 2
    program, game, dbs_dir = (os.path.abspath(a) for a in argv[1:4])
    jobs_n = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 3
    only = argv[argv.index("--only") + 1].split(",") if "--only" in argv else None
    dbs = databases(dbs_dir)
    runs = []          # (label, year, database name or None, env)
    for r in season_rows():
        if r["role"] not in ("season database", "extra version") or (r["role"] == "extra version" and "--extras" not in argv):
            continue
        year = int(r["data_season"][:4])
        if only and str(year) not in only and r["database"] not in only:
            continue
        if r["database"] not in dbs:
            print(f"skip  {r['database']}: not in {dbs_dir}")
            continue
        runs.append((r["database"], year, r["database"], None))
    if not only or "2002" in only:
        runs.append(("3.9.68 in 2002/03", 2002, None, None))
    if "--no-base" not in argv and "October 2019" in dbs:
        runs.append(("base", 2001, "October 2019", None))
        runs.append(("off", 2019, "October 2019", {"RECOMP_SEASON_CODE": "off"}))
    if "--no-base" not in argv and "September 2010 ODB" in dbs:
        runs.append(("again", 2010, "September 2010 ODB", None))
        if not any(x[0] == "September 2010 ODB" for x in runs):
            runs.append(("September 2010 ODB", 2010, "September 2010 ODB", None))
    failed = []

    def fail(text):
        failed.append(text)
        print("FAIL  " + text, flush=True)

    with tempfile.TemporaryDirectory(prefix="cm0102-dates-") as work:
        folders = {n: neutral_copy(work, n, dbs[n][1]) for n in {x[2] for x in runs if x[2]}}
        with concurrent.futures.ThreadPoolExecutor(jobs_n) as pool:
            fut = {label: pool.submit(play, program, game, work, re.sub(r"[^A-Za-z0-9]", "_", label), year,
                                      folders.get(name), env)
                   for label, year, name, env in runs}
            results = {k: v.result() for k, v in fut.items()}
        for label, year, name, env in runs:
            path, log, took = results[label]
            res = parse(log)
            moved = re.search(r"recomp: season dates: .*", log)
            if moved:
                print(f"info  {label}: {moved.group(0)[len('recomp: season dates: '):]}")
            if not res:
                fail(f"{label}: no dates log (save {'made' if path else 'missing'}, {took:.0f} s); "
                     f"last lines: {log[-400:]!r}")
                continue
            if label in ("base", "off"):
                if res["shifts"] != "none":
                    fail(f"{label}: the shifts {res['shifts']} applied")
                else:
                    print(f"ok    {label}: nothing moves ({res['shifts']}, first turn {res['date']})")
                continue
            if label == "again":
                other = results.get("September 2010 ODB", (None,))[0]
                if path and other and open(path, "rb").read() == open(other, "rb").read():
                    print("ok    again: the 2010/11 game gives the same save twice")
                else:
                    fail("again: the two 2010/11 saves differ or are missing")
                continue
            good, bad = check_people(res)
            if len(good) >= MIN_PEOPLE and not bad:
                print(f"ok    {label}: {len(good)} people right on {game_date(*res['date'])}: " + "; ".join(good))
            else:
                fail(f"{label}: {len(good)} people right, wrong: {'; '.join(bad) or 'none'} (right: {'; '.join(good)})")
            want = CHAMPIONS.get(year)
            if res["comp"] and res["comp"][0] == year and (res["comp"][1] == want or label.startswith("3.9.68")):
                print(f"ok    {label}: last {COMP} row {res['comp'][0]} {res['comp'][1]}")
            elif res["comp"] and res["comp"][0] == year:
                fail(f"{label}: last {COMP} row {res['comp']} (real champions of {year}: {want})")
            else:
                fail(f"{label}: last {COMP} row {res['comp']}, not the year {year}")
    print(f"{len(failed)} failed" if failed else "all passed")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
