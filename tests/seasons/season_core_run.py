#!/usr/bin/env python3
"""Season core check (0.6.0 slice 11): a game that starts in another season, and the season part of a save.

Usage:
    python3 tests/seasons/season_core_run.py <cm0102_recomp> <game folder> [--database <folder>] [--year y]
        [--days n] [--only base|season]

The checks, played at the same time:
    base-off    a new base game (2001/02, 3.9.68, changing_rules = off) saved after n days (default 14): its save
                has no season part, as a save of 0.5.0.
    base-on     the same with changing_rules = on: the save has the part with 2001 and changing rules on.
    season      a new game in the year (default 2010) with the database folder (default the September 2010 ODB
                of test-fixtures/databases): the first news screen is in July of the year, the fixture store
                holds no calendar year before it (RECOMP_SEASON_LOG), the game goes on holiday for n days without
                a crash and the save has the part with the year and changing rules on.
    season-again  the same steps again: the save is byte for byte the same (the same clicks, the same game).
    season-italy  the season game with Italy selected too (the Italian leagues hold fixed 2001/02 fixture lists):
                the first news screen is in July of the year and the fixture store holds no year before it.
    reload      loads the save of "season" in a new user folder with no RECOMP_START_SEASON, plays 3 days and
                saves: the runtime reads the year from the part, and the new save has the part with the year.
Each run uses -seed 1, the test clock and a new user folder. The steps are those of tests/smoke/smoke_run.py
(Arsenal, or the first club of the list) in classic mode.
"""
import concurrent.futures
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
sys.path.insert(0, os.path.join(HERE, "..", "..", "tools", "save"))
import custom_database_run as cdr  # noqa: E402
import fixtures  # noqa: E402
import savfile  # noqa: E402
import smoke_run  # noqa: E402

PART = "bosman_season.dat"
NEWS = "0x7881e0"
DEFAULT_DB = os.path.join(fixtures.DEFAULT_DIR, "databases", "September 2010 ODB")
MONTH_DAYS = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]


def leap(y):
    return (y % 4 == 0 and y % 100 != 0) or y % 400 == 0


def month_of(day, year):
    """The month (1 to 12) of a day of the year of the game date (from 1 on 1 January, Date::set 0x54a130)."""
    days = list(MONTH_DAYS)
    if leap(year):
        days[1] = 29
    n = day
    for m, d in enumerate(days):
        if n <= d:
            return m + 1
        n -= d
    return 13


def read_part(path):
    """(version, length, start year, changing rules) of the season part of a save, or None."""
    sav = savfile.Sav(path)
    try:
        data = sav.read_member(PART)
    except KeyError:
        return None
    return struct.unpack_from("<4I", data, 0)


def new_game_steps():
    steps = [s for s in smoke_run.STEPS if "5-holiday" not in s and "quit" not in s]
    return smoke_run.without_intro_click(steps) + ["400000 quit"]


def italy_steps():
    """The new game steps with Italy selected too: the Italian leagues hold the fixed 2001/02 fixture lists.
    With two leagues the game asks for the start season (screen 0x81e8d0, England is chosen), and the Select
    Team screen has a Nation box above the list, so Arsenal is one row lower."""
    out = []
    for s in new_game_steps():
        if s == "2500 click 705 572" and not any("390 451" in x for x in out):
            out += ["2000 click 390 451",               # Select League(s): Italy
                    "4000 click 705 572",               # Next
                    "wait screen:0x81e8d0",
                    "2500 click 705 572"]               # Select Start Season: Next (England)
            continue
        if s in ("wait typing", "wait first-news"):
            s += " 600000"                              # the game is made after the start season
        if s == "1000 click 155 163":
            s = "2000 click 155 188"                    # Select Team: Arsenal
        out.append(s)
    return out


def play(program, game, work, name, options, steps, days, env_extra=None, database=None, save=None, timeout=900):
    run = os.path.join(work, name)
    user, home, shots = (os.path.join(run, d) for d in ("user", "home", "shots"))
    for d in (user, home, shots):
        os.makedirs(d)
    if database:
        cdr.copy_data(database, os.path.join(user, "Databases", os.path.basename(database)))
        options = dict(options, database="folder:" + os.path.basename(database))
    if save:
        shutil.copyfile(save, os.path.join(user, os.path.basename(save)))
    with open(os.path.join(user, "settings.txt"), "w") as f:
        f.write("display = classic\n")
        for k, v in options.items():
            f.write(f"{k} = {v}\n")
    env = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
               RECOMP_USER_DIR=user, RECOMP_FIXED_CLOCK="1", RECOMP_SAVE_AFTER_DAYS=str(days),
               RECOMP_SCREEN_LOG="1", RECOMP_SEASON_LOG="1",
               RECOMP_SCRIPT="; ".join(smoke_run.scaled_steps("classic", shots, steps)), **(env_extra or {}))
    start = time.time()
    try:
        r = subprocess.run([program, "--game", game, "-seed", "1"], env=env, capture_output=True, text=True,
                           timeout=timeout)
    except subprocess.TimeoutExpired as e:
        return None, (e.stderr or b"").decode("latin-1") if isinstance(e.stderr, bytes) else (e.stderr or ""), \
            time.time() - start
    path = os.path.join(user, smoke_run.SAVE_NAME)
    with open(os.path.join(run, "stderr.txt"), "w") as f:
        f.write(r.stderr)
    return (path if r.returncode == 0 and os.path.exists(path) else None), r.stderr, time.time() - start


def first_news(log):
    m = re.search(r"date (\d+)/(\d+): screen " + NEWS, log)
    return (int(m.group(1)), int(m.group(2))) if m else None


def fixture_years(log):
    years = set()
    for m in re.finditer(r"season log: date [-\d]+/[-\d]+, start year \d+, changing rules \w+, fixture years([ \d-]*)",
                         log):
        years |= {int(y) for y in m.group(1).split()}
    return years


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    database = os.path.abspath(argv[argv.index("--database") + 1]) if "--database" in argv else DEFAULT_DB
    year = int(argv[argv.index("--year") + 1]) if "--year" in argv else 2010
    days = int(argv[argv.index("--days") + 1]) if "--days" in argv else 14
    only = argv[argv.index("--only") + 1] if "--only" in argv else None
    failed = []

    def fail(text):
        failed.append(text)
        print("FAIL  " + text, flush=True)

    with tempfile.TemporaryDirectory(prefix="cm0102-season-") as work:
        steps = new_game_steps()
        season_env = {"RECOMP_START_SEASON": str(year)}
        jobs = {}
        with concurrent.futures.ThreadPoolExecutor(5) as pool:
            if only in (None, "base"):
                jobs["base-off"] = pool.submit(play, program, game, work, "base-off", {"changing_rules": "off"},
                                               steps, days)
                jobs["base-on"] = pool.submit(play, program, game, work, "base-on", {"changing_rules": "on"},
                                              steps, days)
            if only in (None, "season"):
                for name in ("season", "season-again"):
                    jobs[name] = pool.submit(play, program, game, work, name, {}, steps, days, season_env, database)
                jobs["season-italy"] = pool.submit(play, program, game, work, "season-italy", {}, italy_steps(), days,
                                                   season_env, database)
            results = {k: v.result() for k, v in jobs.items()}
        for name, (path, log, took) in results.items():
            print(f"run   {name}: {'saved' if path else 'NO SAVE'} in {took:.0f} s", flush=True)
            if not path:
                fail(f"{name}: no saved game; last lines: {log[-600:]!r}")
        if "base-off" in results and results["base-off"][0]:
            part = read_part(results["base-off"][0])
            if part is not None:
                fail(f"base-off: the save has the season part {part}")
            else:
                print("ok    base-off: the save has no season part")
        if "base-on" in results and results["base-on"][0]:
            part = read_part(results["base-on"][0])
            if part != (1, 16, 2001, 1):
                fail(f"base-on: the season part is {part}, not (1, 16, 2001, 1)")
            else:
                print("ok    base-on: the save has the part with 2001 and changing rules on")
        if "season" in results and results["season"][0]:
            path, log, _ = results["season"]
            news = first_news(log)
            if not news or news[1] != year or month_of(*news) != 7:
                fail(f"season: the first news screen is on day {news} (day of the year/year), not in July {year}")
            else:
                print(f"ok    season: the first news screen is on day {news[0]} of {year} (July)")
            years = fixture_years(log)
            if not years or min(years) < year:
                fail(f"season: the fixture store holds the years {sorted(years)}")
            else:
                print(f"ok    season: the fixture store holds the years {sorted(years)}")
            part = read_part(path)
            if part != (1, 16, year, 1):
                fail(f"season: the season part is {part}, not (1, 16, {year}, 1)")
            else:
                print(f"ok    season: the save has the part with {year} and changing rules on")
            again = results["season-again"][0]
            if again and open(again, "rb").read() != open(path, "rb").read():
                a, b = smoke_run.save_parts(path), smoke_run.save_parts(again)
                differ = sorted(k for k in set(a) | set(b) if a.get(k) != b.get(k))
                fail(f"season-again: the save differs from season in: {', '.join(differ) or 'the file header'}")
            elif again:
                print("ok    season-again: the save is the same as season")
            ipath, ilog, _ = results["season-italy"]
            if ipath:
                years = fixture_years(ilog)
                news = first_news(ilog)
                if not years or min(years) < year or not news or news[1] != year or month_of(*news) != 7:
                    fail(f"season-italy: first news {news}, fixture years {sorted(years)}")
                else:
                    print(f"ok    season-italy: with Italy, first news in July {year}, fixture years {sorted(years)}")
            # The reload: the saved 2010 game in a new folder, with no test variable.
            saved = os.path.join(work, "reload.sav")
            shutil.copyfile(path, saved)
            load = smoke_run.without_intro_click(fixtures.LOAD_STEPS) + ["400000 quit"]
            holiday = smoke_run.STEPS[smoke_run.STEPS.index("wait first-news") + 1:]
            holiday = [s for s in holiday if "dump" not in s and "quit" not in s]
            rpath, rlog, took = play(program, game, work, "reload", {}, load[:-1] + holiday + load[-1:], 3,
                                     database=database, save=saved)
            print(f"run   reload: {'saved' if rpath else 'NO SAVE'} in {took:.0f} s", flush=True)
            if not rpath:
                fail(f"reload: no saved game; last lines: {rlog[-600:]!r}")
            else:
                if f"the save is a game of {year}/{(year + 1) % 100:02d}" not in rlog:
                    fail("reload: the runtime did not read the year from the part")
                part = read_part(rpath)
                if part != (1, 16, year, 1):
                    fail(f"reload: the season part of the new save is {part}")
                else:
                    print(f"ok    reload: the load read {year} from the part, and the new save has it")
    print("PASS" if not failed else "FAIL")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
