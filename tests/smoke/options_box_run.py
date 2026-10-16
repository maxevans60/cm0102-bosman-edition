#!/usr/bin/env python3
"""Options box check (spec 2026-10-16-features-0.5.0, slice 6; #56): the Bosman Edition Options box.

Usage:
    python3 tests/smoke/options_box_run.py <cm0102_recomp> <game folder> <out folder> [--modes classic,widescreen,hd]
        [--dir <fixtures folder>]

Runs at the same time:
    side-<mode>   for each mode: loads start.sav (tests/fixtures), opens the box from the side bar drop-down
                  "Game Options", switches attribute_colours off on the Screen tab and save_backups off on the
                  Game tab, closes the box and opens the profile of Vieira from the Arsenal squad.
    start         widescreen mode, a new user folder with database = folder:My Update: opens the box from the
                  start screen, sets the display mode to Classic, switches Sharp picture off and on (applied at
                  once, user32.c) and closes the box. Then a second start with the same user folder.

Checks:
    - each side run: settings.txt has attribute_colours = off and save_backups = off, and every other line is
      as before (the comments too); the profile shows at least 20 attribute values, and none of them has an
      attribute colour (RECOMP_COLOUR_LOG=1, columns.c).
    - start: settings.txt has display = classic and sharp_picture = on after the first start, the first start
      draws at 1280x800, and the second start draws at 800x600 (the restart note of the display mode).
Pictures (with .png copies) in <out>/<run>/: screen.bmp and game.bmp (the two tabs), screen-off.bmp and
game-off.bmp (after the clicks), profile.bmp; start/box.bmp, start/game.bmp and start/second.bmp.
"""
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
import fixtures  # noqa: E402
import smoke_run  # noqa: E402

# Positions for 800x600. The box scales as every area does. The side bar drop-down items keep their size,
# so the item has its own widescreen and HD positions after "|". Box rows: y = 192 + 35 * row; On and the
# first value at x 371, Off and the second value at 452; tabs Screen 304, Game 584 at y 155; Close 444, 524.
SIDE_STEPS = [
    "1000 click 45 344",                        # Game Options
    "3000 click 175 417 | 225 525 | 295 675",   # Bosman Edition Options
    "5000 dump {d}/screen.bmp",
    "6000 click 452 332",                       # Attribute colours: Off
    "7500 dump {d}/screen-off.bmp",
    "8500 click 584 155",                       # the Game tab
    "10000 dump {d}/game.bmp",
    "11000 click 452 402",                      # Save backups: Off
    "12500 dump {d}/game-off.bmp",
    "13500 click 444 524",                      # Close
    "15000 click 45 165",                       # the manager's menu
    "16500 click 150 156 | 199 203 | 271 271",  # Arsenal Squad
    "20000 click 210 369 | 336 492 | 460 664",  # Vieira
    "23000 dump {d}/profile.bmp",
    "24000 quit",
]
START_STEPS = fixtures.LOAD_STEPS[:fixtures.LOAD_STEPS.index("wait setup") + 1] + [
    "1500 click 445 503",                       # Bosman Edition Options, the last item of the menu
    "3500 dump {d}/box.bmp",
    "4000 click 452 192",                       # Display mode: Classic
    "4500 click 452 227",                       # Sharp picture: Off
    "5000 dump {d}/box-sharp-off.bmp",
    "5500 click 371 227",                       # Sharp picture: On
    "6000 click 584 155",                       # the Game tab: the custom data set
    "7000 dump {d}/game.bmp",
    "7500 click 444 524",                       # Close
    "wait setup",
    "1000 quit",
]
SECOND_STEPS = ["1000 click 400 300", "wait setup", "1500 dump {d}/second.bmp", "2000 quit"]

VALUE = re.compile(r'recomp: profile value "([^"]*)" colour (0x[0-9a-f]+)')
BAND_COLOURS = {0xFA8A, 0xFD84, 0xA70C, 0x37E6}      # columns.c COLOUR_*


def size(bmp):
    return struct.unpack_from("<ii", open(bmp, "rb").read(), 18)


def png(folder):
    for f in sorted(os.listdir(folder)):
        if f.endswith(".bmp") and shutil.which("sips"):
            subprocess.run(["sips", "-s", "format", "png", os.path.join(folder, f), "--out",
                            os.path.join(folder, f[:-4] + ".png")], capture_output=True)


def play(program, game, mode, user, out, steps, env=None):
    os.makedirs(out, exist_ok=True)
    r, _ = fixtures.run(program, game, mode, user, smoke_run.scaled_steps(mode, out, steps), env or {}, 600)
    with open(os.path.join(out, "stderr.txt"), "a") as f:
        f.write(r.stderr)
    png(out)
    return r.returncode


def changed_lines(before, after):
    """The lines that differ, as (old, new) pairs, when the files have the same number of lines; else None."""
    a, b = before.splitlines(), after.splitlines()
    if len(a) != len(b):
        return None
    return [(x, y) for x, y in zip(a, b) if x != y]


def side_run(program, game, folder, mode, out, work):
    user = os.path.join(work, "side-" + mode, "user")
    os.makedirs(user)
    shutil.copyfile(os.path.join(folder, "start.sav"), os.path.join(user, "start.sav"))
    fixtures.write_settings(user, mode, [])
    steps = [s for s in fixtures.LOAD_STEPS] + SIDE_STEPS
    # The first start writes the missing option lines; keep that file to compare with.
    code = play(program, game, mode, user, out, steps, {"RECOMP_COLOUR_LOG": "1"})
    return code, user


def main(argv):
    args = argv[1:]
    opts = {"--modes": "classic,widescreen,hd", "--dir": fixtures.DEFAULT_DIR}
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
    modes = opts["--modes"].split(",")
    work = tempfile.mkdtemp(prefix="cm0102-options-box-")
    ok = True

    def check(passed, text):
        nonlocal ok
        print(("PASS" if passed else "FAIL") + ": " + text)
        ok &= bool(passed)

    # The settings file as the first start writes it: the same steps up to the load, with no box.
    def reference(mode):
        user = os.path.join(work, "ref-" + mode, "user")
        os.makedirs(user)
        fixtures.write_settings(user, mode, [])
        play(program, game, mode, user, os.path.join(work, "ref-" + mode, "out"), ["1000 quit"])
        return open(os.path.join(user, "settings.txt")).read()

    def start_run():
        user = os.path.join(work, "start", "user")
        os.makedirs(user)
        fixtures.write_settings(user, "widescreen", [("database", "folder:My Update")])
        s = os.path.join(out, "start")
        c1 = play(program, game, "widescreen", user, s, START_STEPS)
        after_first = open(os.path.join(user, "settings.txt")).read()
        c2 = play(program, game, "classic", user, s, SECOND_STEPS)
        return c1, c2, after_first

    with ThreadPoolExecutor(2 * len(modes) + 1) as pool:
        refs = {m: pool.submit(reference, m) for m in modes}
        sides = {m: pool.submit(side_run, program, game, opts["--dir"], m, os.path.join(out, "side-" + m), work)
                 for m in modes}
        start = pool.submit(start_run)
        for m in modes:
            code, user = sides[m].result()
            check(code == 0, f"side-{m}: the run ends with code 0")
            before, after = refs[m].result(), open(os.path.join(user, "settings.txt")).read()
            diff = changed_lines(before, after)
            check(diff is not None and sorted(diff) == [("attribute_colours = on", "attribute_colours = off"),
                                                        ("save_backups = on", "save_backups = off")],
                  f"side-{m}: settings.txt changes only attribute_colours and save_backups to off ({diff})")
            log = open(os.path.join(out, "side-" + m, "stderr.txt"), errors="replace").read()
            check("recomp: options box opened" in log, f"side-{m}: the box opens from the side bar")
            values = [(int(t), int(c, 16)) for t, c in VALUE.findall(log) if t.strip().isdigit()
                      and 1 <= int(t) <= 20]
            coloured = [v for v in values if v[1] in BAND_COLOURS]
            check(len(values) >= 20 and not coloured,
                  f"side-{m}: the profile shows {len(values)} attribute values, {len(coloured)} with an attribute colour")
        c1, c2, after_first = start.result()
        check(c1 == 0 and c2 == 0, "start: both starts end with code 0")
        lines = after_first.splitlines()
        check("display = classic" in lines and "sharp_picture = on" in lines,
              "start: settings.txt has display = classic and sharp_picture = on")
        s = os.path.join(out, "start")
        check(size(os.path.join(s, "box.bmp")) == (1280, 800) and size(os.path.join(s, "second.bmp")) == (800, 600),
              "start: the first start draws at 1280x800, the second at 800x600")
        log = open(os.path.join(s, "stderr.txt"), errors="replace").read()
        check("recomp: options box opened" in log, "start: the box opens from the start screen")
    print(f"pictures in {out}/<run>/*.png; work folder {work}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
