#!/usr/bin/env python3
"""Foreign player colour check (spec 2026-10-16-features-0.5.0, slice 4, issue #47).

Usage:
    python3 tests/smoke/foreign_run.py <cm0102_recomp> <game folder> <out folder> [--dir <fixtures folder>]
        [--mode classic|widescreen|hd]... [--make]

Saved test games (tests/fixtures/fixtures.py): start.sav (Arsenal 14.7.01, England only) and foreign.sav, a
new game with England and Croatia (Arsenal, 14.7.01). --make plays that new game and saves foreign.sav in
the fixtures folder (about a minute). Without foreign.sav the Croatia run fails with a message.

Runs, at the same time:
    - start.sav in each mode (classic, widescreen and hd when no --mode is given): own squad, Assistant >
      Pick best team, Tactics, Cancel, then Go on Holiday; the game is saved 6 game days after the load.
    - start.sav in classic with foreign_colours = off, the same steps.
    - foreign.sav in classic: Competitions > Croatia > First Division > Dinamo > Squad.
Screen dumps of each step in <out>/<run>.

The runtime writes a line for each rule it finds and for each name that gets the colour (RECOMP_FOREIGN_LOG=1):
    recomp: foreign rule club <club> fixture <day>.<year> competition <id> kind <k> non-eu <n> foreign <n>
    recomp: foreign mark <squad|tactics> club <club> staff <staff> "<name>"

Checks:
    - Arsenal (July, five friendlies first): the rule comes from the first competitive fixture, the Charity
      Shield on Sunday 12.8.01 (day 223 from 0), 3 non-EU players. On the squad screen Gilberto Silva,
      Luzhny, Toure, Kanu and Stepanovs are violet; Lauren, Edu, Pires and Wiltord are not (EU or second
      nationality in the data). On the tactics screen the picked team's non-EU substitutes are violet.
    - Dinamo Zagreb (Croatia, a foreign rule): 4 foreign players; Cesar and Kwedi are violet. The squad
      screen's own "Fgn" boxes show for the same players.
    - Each squad and tactics dump of the "on" runs holds violet pixels (RGB565 0xCC7F); the "off" run's do not,
      and it writes no mark.
    - The saves of the classic on and off runs are equal byte for byte (Q152).
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "fixtures"))
import fixtures  # noqa: E402
import smoke_run  # noqa: E402

TOUR = [
    "2500 click 45 165",                    # the manager's menu
    "4000 click 150 156 | 199 203 | 270 270 | 288 274",  # Arsenal Squad
    "wait screen:0x454900",
    "1500 click 592 135",                   # Assistant
    "3000 click 540 159 | 928 205 | 1460 274 | 1460 277",  # Pick best team
    "5000 dump {d}/1-squad.bmp",
    "5500 click 169 527",                   # Tactics
    "8000 dump {d}/2-tactics.bmp",
    "8500 click 358 573",                   # Cancel
    "wait screen:0x454900",
    "1500 click 45 165",                    # the manager's menu
    "3500 click 150 427 | 199 474 | 270 541 | 288 610",  # Go on Holiday
    "6000 click 705 572",                   # Please Confirm: Yes
]
SAVE_DAYS = 6

CROATIA_TOUR = [
    "2500 click 45 210",                    # Competitions
    "4000 move 150 282",                    # Croatia
    "5000 move 270 285",
    "5500 click 270 285",                   # First Division
    "8500 click 225 293",                   # Dinamo
    "11500 click 169 97",                   # Squad
    "14000 dump {d}/1-squad.bmp",
    "14500 quit",
]

# The new game of foreign.sav: the smoke steps, with Croatia on Select League(s), England on Select Start
# Season and Arsenal one row lower on Select Team (the list gets a Nation drop-down).
def make_steps():
    steps = [s for s in smoke_run.STEPS if "dump" not in s]
    steps = steps[:steps.index("wait first-news") + 1]
    i = steps.index("wait leagues")
    steps[i + 1:i + 1] = ["1500 click 390 276"]                    # Croatia: SELECTED
    k = steps.index("wait typing")
    steps[k:k + 1] = ["wait screen:0x81e8d0", "2000 click 705 572", "wait typing"]   # Select Start Season: Next
    j = steps.index("wait screen:0x822260")
    steps[j + 1] = "1000 click 155 188"                            # Select Team: Arsenal
    return steps + ["600000 quit"]


RULE = re.compile(r"recomp: foreign rule club (\d+) fixture (\d+)\.(\d+) competition (\d+) kind (\d+) "
                  r"non-eu (-?\d+) foreign (-?\d+)")
MARK = re.compile(r'recomp: foreign mark (\w+) club (\d+) staff (\d+) "([^"]*)"')
VIOLET = (206, 142, 255)


def make(program, game, folder):
    with tempfile.TemporaryDirectory(prefix="cm0102-foreign-") as work:
        user = os.path.join(work, "user")
        os.makedirs(user)
        fixtures.write_settings(user, "classic", [])
        r, took = fixtures.run(program, game, "classic", user, smoke_run.scaled_steps("classic", work, make_steps()),
                               {"RECOMP_SAVE_AFTER_DAYS": "0"}, 900)
        save = os.path.join(user, smoke_run.SAVE_NAME)
        if r.returncode != 0 or not os.path.exists(save):
            print(r.stderr[-2000:])
            print(f"FAIL: no saved game (code {r.returncode})")
            return 1
        shutil.copyfile(save, os.path.join(folder, "foreign.sav"))
    print(f"saved {os.path.join(folder, 'foreign.sav')} in {took:.0f} s")
    return 0


def run(program, game, folder, save, mode, out, tour, options, days):
    os.makedirs(out, exist_ok=True)
    work = tempfile.mkdtemp(prefix="cm0102-foreign-")
    user = os.path.join(work, "user")
    os.makedirs(user)
    shutil.copyfile(os.path.join(folder, save), os.path.join(user, "start.sav"))
    fixtures.write_settings(user, mode, options)
    steps = smoke_run.scaled_steps(mode, out, fixtures.LOAD_STEPS) + smoke_run.scaled_steps(mode, out, tour)
    env = {"RECOMP_SCREEN_LOG": "1", "RECOMP_FOREIGN_LOG": "1"}
    if days is not None:
        env["RECOMP_SAVE_AFTER_DAYS"] = str(days)
        steps.append("600000 quit")
    r, took = fixtures.run(program, game, mode, user, steps, env, 1200)
    with open(os.path.join(out, "stderr.txt"), "w") as f:
        f.write(r.stderr)
    result = os.path.join(user, smoke_run.SAVE_NAME)
    if os.path.exists(result):
        shutil.copyfile(result, os.path.join(out, smoke_run.SAVE_NAME))
    for f in sorted(os.listdir(out)):
        if f.endswith(".bmp") and shutil.which("sips"):
            subprocess.run(["sips", "-s", "format", "png", os.path.join(out, f), "--out",
                            os.path.join(out, f[:-4] + ".png")], capture_output=True)
    return r.returncode, r.stderr


def violet_pixels(path):
    if not os.path.exists(path):
        return -1
    w, h, bpp, rows = smoke_run.read_bmp(path)
    n = 0
    for row in rows:
        for i in range(0, w * bpp, bpp):
            b, g, r = row[i], row[i + 1], row[i + 2]
            n += abs(r - VIOLET[0]) <= 8 and abs(g - VIOLET[1]) <= 8 and abs(b - VIOLET[2]) <= 8
    return n


def marks(log, screen, field=4):
    return {m.group(field) for m in MARK.finditer(log) if m.group(1) == screen}


def check_arsenal(name, code, log, out):
    problems = [] if code == 0 else [f"{name}: exit code {code}"]
    rules = [m.groups() for m in RULE.finditer(log)]
    if not rules or rules[0][1:3] != ("223", "2001") or rules[0][5:7] != ("3", "-1"):
        problems.append(f"{name}: Arsenal's rule {rules[:1]}, expected the Charity Shield 223.2001, 3 non-EU")
    squad = marks(log, "squad")
    want = {"Gilberto Silva", "Luzhny, O", "Tour?, K", "Kanu, N", "Stepanovs, I"}
    if squad != want:
        problems.append(f"{name}: violet on the squad screen {sorted(squad)}, expected {sorted(want)}")
    tactics = marks(log, "tactics")
    if not tactics or not marks(log, "tactics", 3) <= marks(log, "squad", 3):
        problems.append(f"{name}: violet on the tactics screen {sorted(tactics)}, expected some of the squad's")
    for shot in ("1-squad", "2-tactics"):
        if violet_pixels(os.path.join(out, shot + ".bmp")) < 50:
            problems.append(f"{name}: {shot} has no violet names")
    if not os.path.exists(os.path.join(out, smoke_run.SAVE_NAME)):
        problems.append(f"{name}: no saved game")
    return problems


def check_off(name, code, log, out):
    problems = [] if code == 0 else [f"{name}: exit code {code}"]
    if MARK.search(log) or RULE.search(log):
        problems.append(f"{name}: the runtime looked up a rule or marked a name with the option off")
    for shot in ("1-squad", "2-tactics"):
        n = violet_pixels(os.path.join(out, shot + ".bmp"))
        if n != 0:
            problems.append(f"{name}: {shot} has {n} violet pixels with the option off")
    return problems


def check_croatia(name, code, log, out):
    problems = [] if code == 0 else [f"{name}: exit code {code}"]
    rules = [m.groups() for m in RULE.finditer(log)]
    if not rules or rules[0][5:7] != ("-1", "4"):
        problems.append(f"{name}: Dinamo's rule {rules[:1]}, expected 4 foreign players")
    squad = marks(log, "squad")
    if not {"Cesar, B", "Kwedi, P"} <= squad:
        problems.append(f"{name}: violet on Dinamo's squad screen {sorted(squad)}, expected Cesar and Kwedi")
    if violet_pixels(os.path.join(out, "1-squad.bmp")) < 50:
        problems.append(f"{name}: the squad dump has no violet names")
    return problems


def main(argv):
    args, modes, folder, do_make = [], [], fixtures.DEFAULT_DIR, False
    it = iter(argv[1:])
    for a in it:
        if a == "--mode":
            modes.append(next(it))
        elif a == "--dir":
            folder = next(it)
        elif a == "--make":
            do_make = True
        else:
            args.append(a)
    if len(args) != 3:
        print(__doc__)
        return 2
    program, game, out = (os.path.abspath(a) for a in args)
    if do_make and make(program, game, folder):
        return 1
    if not os.path.exists(os.path.join(folder, "foreign.sav")):
        print(f"FAIL: no {os.path.join(folder, 'foreign.sav')}: run with --make once")
        return 1
    modes = modes or ["classic", "widescreen", "hd"]
    jobs = [(m, "start.sav", m, TOUR, [], SAVE_DAYS, check_arsenal) for m in modes]
    jobs.append(("classic-off", "start.sav", "classic", TOUR, [("foreign_colours", "off")], SAVE_DAYS, check_off))
    jobs.append(("croatia", "foreign.sav", "classic", CROATIA_TOUR, [], None, check_croatia))
    with ThreadPoolExecutor(len(jobs)) as pool:
        results = list(pool.map(lambda j: run(program, game, folder, j[1], j[2], os.path.join(out, j[0]), j[3], j[4],
                                              j[5]), jobs))
    problems = []
    for job, (code, log) in zip(jobs, results):
        problems += job[6](job[0], code, log, os.path.join(out, job[0]))
    if "classic" in modes:
        saves = [os.path.join(out, n, smoke_run.SAVE_NAME) for n in ("classic", "classic-off")]
        if all(os.path.exists(s) for s in saves):
            if open(saves[0], "rb").read() != open(saves[1], "rb").read():
                problems.append("the saves with foreign_colours on and off differ")
        else:
            problems.append("a classic run made no save to compare")
    for p in problems:
        print("FAIL:", p)
    print("PASS" if not problems else f"{len(problems)} problems", f"(dumps in {out})")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
