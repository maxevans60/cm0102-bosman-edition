#!/usr/bin/env python3
"""Full player search check (spec 2026-10-16-features-0.5.0, slice 5): the option full_player_search.

Usage:
    python3 tests/smoke/player_search_run.py <cm0102_recomp> <game folder> <out folder> [--ref <cm0102_recomp>]
        [--dir <fixtures folder>] [--save start.sav]

Three runs from the save (default start.sav, Arsenal 14.7.01), at the same time: the Player & Staff Search
screen, Player Search tab, which lists "Famous players in your region", with
    off      full_player_search = off (the default)
    on       full_player_search = on
    ref      the program of --ref (a build without the option), when --ref is given
Each run saves the screen as <out>/<run>/search.bmp.

Checks:
    - off and ref give the same picture, byte for byte (off keeps the original's path)
    - on gives another picture: the list holds players that the manager does not know. The picture shows
      the count ("N players found") and the page count, and the rows are in name order, so a player that
      only the full search lists moves the rows.
From start.sav, off lists 1678 players (153 pages) and on lists 7166 (652 pages). Ben Abbey of Woking
(Conference, S C) is in the first rows of on and not in off: the manager does not know him.
"""
import os
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "fixtures"))
import fixtures  # noqa: E402

STEPS = [
    "1000 dump {d}/start.bmp",
    "2500 click 45 165",                    # the manager's menu
    "4000 click 190 302",                   # Player & Staff Search
    "wait screen:0x808690",
    "2000 dump {d}/search.bmp",
    "3000 quit",
]


def play(program, game, folder, save, out, mode, sets):
    steps = os.path.join(out, "steps.txt")
    os.makedirs(out, exist_ok=True)
    with open(steps, "w") as f:
        f.write("\n".join(STEPS) + "\n")
    return fixtures.play(program, game, folder, save, steps, out, mode, sets, None)


def main(argv):
    args = argv[1:]
    opts = {"--ref": None, "--dir": fixtures.DEFAULT_DIR, "--save": "start.sav"}
    rest = []
    i = 0
    while i < len(args):
        if args[i] in opts:
            opts[args[i]] = args[i + 1]
            i += 2
        else:
            rest.append(args[i])
            i += 1
    if len(rest) != 3:
        print(__doc__)
        return 2
    program, game, out = (os.path.abspath(p) for p in rest)
    runs = [("off", program, [("full_player_search", "off")]), ("on", program, [("full_player_search", "on")])]
    if opts["--ref"]:
        runs.append(("ref", os.path.abspath(opts["--ref"]), []))
    with ThreadPoolExecutor(len(runs)) as pool:
        codes = list(pool.map(lambda r: play(r[1], game, opts["--dir"], opts["--save"], os.path.join(out, r[0]),
                                             "classic", r[2]), runs))
    if any(codes):
        print("FAIL: a run failed", codes)
        return 1

    def pic(name):
        with open(os.path.join(out, name, "search.bmp"), "rb") as f:
            return f.read()

    ok = True
    if opts["--ref"]:
        same = pic("off") == pic("ref")
        print(("PASS" if same else "FAIL") + ": off gives the picture of the build without the option")
        ok &= same
    differs = pic("on") != pic("off")
    print(("PASS" if differs else "FAIL") + ": on lists other players than off")
    ok &= differs
    print(f"pictures in {out}/<run>/search.png")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
