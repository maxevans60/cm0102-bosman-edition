#!/usr/bin/env python3
"""HD text check (0.5.1): the screens of HD with hd_text = large, for a check by eye.

Usage:
    python3 tests/smoke/hd_text_run.py <cm0102_recomp> <game folder> <out folder>

Three runs in parallel, each from a saved test game (tests/fixtures):
    screens      start.sav: the manager menu (a side bar popup), the squad screen, the profile of Vieira,
                 the Bosman Edition Options box (both tabs) and Game Settings.
    traditional  start.sav: Game Settings > Fonts > Traditional > Ok, then the news, the manager menu and the
                 squad screen in the bitmap fonts.
    match        prematch.sav: the match screen before the kick off, with the Commentary Speed popup open.
The pictures go to the out folder as <run>-<name>.png. Look for text that is cut or that overlaps, and count
the rows of the squad list: at least 9 a column (spec 2026-10-16-hd-text-0.5.1, screen seam). The run fails
when a run ends without its pictures, or when its log has no "HD text large" line.
"""
import concurrent.futures
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
FIXTURES = os.path.join(HERE, "..", "fixtures", "fixtures.py")
sys.path.insert(0, HERE)
import smoke_run  # noqa: E402

# Positions in HD pixels (1920x1080) with HD Large text, measured on the dumps: popup items and list rows
# are taller than in HD Small, so they are not scaled from 800x600.
MENU = "click 108 297"                  # the manager's menu
SQUAD = "click 288 274"                 # Arsenal Squad, the first item
GAME_OPTIONS = "click 108 645"
GAME_SETTINGS = "click 288 672"         # Game Settings in Game Options
OPTIONS_ITEM = "click 288 697"          # Bosman Edition Options, after Game Settings

SCREENS = [
    "1000 " + MENU,
    "2500 dump {d}/menu.bmp",
    "3000 " + SQUAD,
    "wait screen:0x454900",
    "2000 dump {d}/squad.bmp",
    "2500 click 461 755",                                       # Vieira, row 7
    "wait screen:0x870700",
    "2000 dump {d}/profile.bmp",
    "2500 " + GAME_OPTIONS,
    "4000 " + OPTIONS_ITEM,
    "wait options",
    "1500 dump {d}/options-screen.bmp",
    "2000 click 1401 268",                                      # the Game tab
    "3500 dump {d}/options-game.bmp",
    "4000 click 1065 943",                                      # Close
    "wait screen:0x870700",
    "1500 " + GAME_OPTIONS,
    "3000 " + GAME_SETTINGS,
    "5000 dump {d}/game-settings.bmp",
    "5500 quit",
]
TRADITIONAL = [
    "1000 " + GAME_OPTIONS,
    "2500 " + GAME_SETTINGS,
    "4500 click 1421 371",                                      # Fonts: the arrow button
    "6000 click 1338 363",                                      # Traditional, the first item
    "7500 dump {d}/game-settings.bmp",
    "8000 click 1590 943",                                      # Ok
    "wait first-news",
    "2000 dump {d}/news.bmp",
    "2500 " + MENU,
    "4000 dump {d}/menu.bmp",
    "4500 " + SQUAD,
    "wait screen:0x454900",
    "2000 dump {d}/squad.bmp",
    "2500 quit",
]
RUNS = {"screens": ("start.sav", SCREENS), "traditional": ("start.sav", TRADITIONAL)}


def match_steps():
    """The steps of the match check (smoke_run.MATCH_STEPS, scaled) to the match screen, then the speed popup."""
    steps = smoke_run.MATCH_STEPS
    return smoke_run.scaled_steps("hd", "{d}", steps[:steps.index("wait match") + 1]) + [
        "1500 dump {d}/match.bmp", "2000 click 108 457", "3500 dump {d}/speed.bmp", "4000 quit"]


def run(program, game, name, save, steps, work):
    out = os.path.join(work, name)
    os.makedirs(out)
    path = os.path.join(work, name + ".steps")
    with open(path, "w") as f:
        f.write("\n".join(steps) + "\n")
    r = subprocess.run([sys.executable, FIXTURES, "play", program, game, save, path, out, "--mode", "hd"],
                       capture_output=True, text=True, timeout=900)
    return out, r.stdout + r.stderr


def main(argv):
    if len(argv) < 4:
        print(__doc__)
        return 2
    program, game, dest = (os.path.abspath(a) for a in argv[1:4])
    os.makedirs(dest, exist_ok=True)
    runs = dict(RUNS, match=("prematch.sav", match_steps()))
    failed = 0
    with tempfile.TemporaryDirectory(prefix="cm0102-hdtext-") as work, \
            concurrent.futures.ThreadPoolExecutor(len(runs)) as pool:
        jobs = {n: pool.submit(run, program, game, n, s, st, work) for n, (s, st) in runs.items()}
        for name, job in jobs.items():
            out, text = job.result()
            pngs = sorted(f for f in os.listdir(out) if f.endswith(".png"))
            wanted = sum(1 for s in runs[name][1] if " dump " in s)
            for f in pngs:
                shutil.copyfile(os.path.join(out, f), os.path.join(dest, f"{name}-{f}"))
            log = open(os.path.join(out, "stderr.txt")).read() if os.path.exists(os.path.join(out, "stderr.txt")) else ""
            large = "recomp: HD text large" in log
            ok = len(pngs) == wanted and large
            failed += not ok
            print(f"{'ok  ' if ok else 'FAIL'}  {name}: {len(pngs)} of {wanted} pictures, HD text large: {large}")
            if not ok:
                print(text[-1500:])
    print(f"pictures in {dest}")
    print("FAIL" if failed else "PASS (now check the pictures by eye)")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
