#!/usr/bin/env python3
"""Choose Season screen check (0.6.0 slice 13): each tile starts its season database.

Usage:
    python3 -I tests/seasons/season_screen_run.py <cm0102_recomp> <game folder> <folder of source zips>
        [--ref-build <program>] [--only <run>] [--shots <folder>]

<folder of source zips> is test-fixtures/databases/zips. The run makes the season zips of four seasons with
tools/release/make_season_zips.py and a new game folder: links to the files of <game folder> and Seasons/ with
those zips, as in a download. Each run has a new user folder, -seed 1, the test clock and classic mode.

Runs, played at the same time (one per era, Q240):
    2002    Original era: the 2002/03 tile (3.9.68 data, no unpack)
    2003    2000s: the 2003/04 tile (October 2003)
    2010    2010s: the 2010/11 tile (September 2010 ODB)
    2021    early 2020s: the 2021/22 tile (October 2021)
    2025    league phase: the 2025/26 tile (December 2025)
Each run opens the screen from Setup Game, clicks the tile and Choose, waits for Setup Game, starts a new game
with the steps of tests/smoke/smoke_run.py and saves 1 day after the first news screen. Checks: the first news
screen is in July of the season, settings.txt has database and season of the tile, a zipped database is
unpacked into Databases (the log of seasons_db_prepare), and the save has the season part with the year.

    base        the 2001/02 tile through the screen with changing_rules = off, saved 14 days after the first
                news screen: the save has no season part. With --ref-build the run "ref" plays the plain new game
                of smoke_run.py --same-save with that build (no screen), and the two saves must be the same
                byte for byte (the base game rule, Q152).
--shots keeps a picture of the screen after the tile click of each run in that folder.
"""
import concurrent.futures
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tests", "smoke"))
sys.path.insert(0, HERE)
import season_core_run as core  # noqa: E402
import smoke_run  # noqa: E402

MAKE = os.path.join(ROOT, "tools", "release", "make_season_zips.py")

# Classic positions (800 x 600). Setup Game: the Choose Season item, the cell after Web Sites. The screen:
# the tiles (season_screen.c layout) and the Choose button of the strip.
SETUP_SEASON = "click 613 437"
TILE = {2001: (140, 113), 2002: (227, 113), 2003: (140, 175), 2010: (227, 238), 2021: (227, 341),
        2025: (227, 403)}
CHOOSE = "click 706 445"
# year: (database value, zip to make or None)
RUNS = {2002: ("3.9.68", None), 2003: ("folder:October 2003", "October 2003"),
        2010: ("folder:September 2010 ODB", "September 2010 ODB"), 2021: ("folder:October 2021", "October 2021"),
        2025: ("folder:December 2025", "December 2025")}


def screen_steps(year, shots):
    x, y = TILE[year]
    steps = [f"2000 {SETUP_SEASON}", "wait seasons", f"2000 click {x} {y}"]
    if shots:
        steps.append(f"3500 dump {shots}/tile-{year}.bmp")
    return steps + [f"4000 {CHOOSE}", "wait setup 300000"]


def game_steps(year, shots):
    """The new game steps of smoke_run.py with the screen steps after the first wait for Setup Game."""
    base = core.new_game_steps()
    i = base.index("wait setup") + 1
    return base[:i] + screen_steps(year, shots) + base[i:]


def make_game(game, zips, work):
    """A game folder as in a download: links to the files of game, and Seasons/ with the season zips."""
    out = os.path.join(work, "zips")
    only = [a for _, (_, z) in RUNS.items() if z for a in ("--only", z)]
    r = subprocess.run([sys.executable, "-I", MAKE, "--season-dbs", zips, "--out", out, *only], capture_output=True,
                       text=True)
    if r.returncode:
        raise SystemExit("make_season_zips.py failed: " + r.stderr[-400:])
    folder = os.path.join(work, "game")
    os.makedirs(folder)
    for n in os.listdir(game):
        if n != "Seasons":
            os.symlink(os.path.join(game, n), os.path.join(folder, n))
    os.symlink(out, os.path.join(folder, "Seasons"))
    return folder


def settings_of(path):
    out = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            if line.startswith("#") or "=" not in line:
                continue
            k, _, v = line.partition("=")
            out[k.strip()] = v.strip()
    return out


def main(argv):
    if len(argv) < 4:
        print(__doc__)
        return 2
    program, game, zips = (os.path.abspath(a) for a in argv[1:4])
    ref = os.path.abspath(argv[argv.index("--ref-build") + 1]) if "--ref-build" in argv else None
    only = argv[argv.index("--only") + 1] if "--only" in argv else None
    shots = os.path.abspath(argv[argv.index("--shots") + 1]) if "--shots" in argv else None
    if shots:
        os.makedirs(shots, exist_ok=True)
    failed = []

    def check(cond, text):
        print(("ok    " if cond else "FAIL  ") + text, flush=True)
        if not cond:
            failed.append(text)

    with tempfile.TemporaryDirectory(prefix="cm0102-season-screen-") as work:
        folder = make_game(game, zips, work)
        jobs = {}
        with concurrent.futures.ThreadPoolExecutor(7) as pool:
            for year in RUNS:
                if only in (None, str(year)):
                    jobs[str(year)] = pool.submit(core.play, program, folder, work, str(year), {},
                                                  game_steps(year, shots), 1)
            if only in (None, "base"):
                jobs["base"] = pool.submit(core.play, program, folder, work, "base", {"changing_rules": "off"},
                                           game_steps(2001, shots), 14)
                if ref:
                    jobs["ref"] = pool.submit(core.play, ref, folder, work, "ref", {"changing_rules": "off"},
                                              core.new_game_steps(), 14, None, None, None, 900)
            results = {k: v.result() for k, v in jobs.items()}
        for name, (path, log, took) in results.items():
            print(f"run   {name}: {'saved' if path else 'NO SAVE'} in {took:.0f} s", flush=True)
            if not path:
                print(log[-1500:])
        for year, (value, zipname) in RUNS.items():
            if str(year) not in results:
                continue
            path, log, _ = results[str(year)]
            if not path:
                check(False, f"{year}: no save")
                continue
            news = core.first_news(log)
            check(news is not None and news[1] == year and core.month_of(news[0], year) == 7,
                  f"{year}: the first news screen is in July {year} (date {news})")
            s = settings_of(os.path.join(work, str(year), "user", "settings.txt"))
            label = f"{year}/{(year + 1) % 100:02d}"
            check(s.get("database") == value and s.get("season") == label,
                  f"{year}: settings.txt has database = {s.get('database')}, season = {s.get('season')}")
            if zipname:
                check(re.search(rf"choose season {year}: prepare status 0, folder {re.escape(zipname)}, unpacked 1",
                                log) is not None, f"{year}: the tile unpacked {zipname} into Databases")
                check(os.path.isdir(os.path.join(work, str(year), "user", "Databases", zipname)),
                      f"{year}: Databases/{zipname} is there")
            part = core.read_part(path)
            check(part is not None and part[2] == year, f"{year}: the save has the season part with {year} ({part})")
        if "base" in results:
            path, log, _ = results["base"]
            check(path is not None and core.read_part(path) is None, "base: the save has no season part")
            news = core.first_news(log) if path else None
            check(news is not None and news[1] == 2001, f"base: the first news screen is in 2001 ({news})")
            if ref and "ref" in results and results["ref"][0] and path:
                same = open(path, "rb").read() == open(results["ref"][0], "rb").read()
                if not same:
                    a, b = smoke_run.save_parts(path), smoke_run.save_parts(results["ref"][0])
                    differ = sorted(k for k in set(a) | set(b) if a.get(k) != b.get(k))
                    print("      parts that differ: " + ", ".join(differ))
                check(same, "base: the save through the screen equals the plain new game of the reference build")
        if shots:
            for n in os.listdir(shots):
                if n.endswith(".bmp"):
                    subprocess.run(["sips", "-s", "format", "png", os.path.join(shots, n), "--out",
                                    os.path.join(shots, n[:-4] + ".png")], capture_output=True)
    print("PASS" if not failed else f"FAIL ({len(failed)})")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
