#!/usr/bin/env python3
"""Full play (0.6.0 slice 30a, gate (b) of docs/research/brief-0.6.0/slice-30a.md): one new game from a start season,
played with the match engine on and the manager on holiday, for a number of seasons.

Usage:
    python3 tests/seasons/full_play_run.py <cm0102_recomp> <game folder> --keep <folder> [--start 2001]
        [--seasons 2] [--nations all|England,...] [--database <folder>] [--changing-rules on|off] [--seed 1]
        [--stall-minutes 20] [--timeout 86400]

The run takes the new-game and holiday steps of tests/seasons/rules_report_check.py, but without RECOMP_SEASON_JUMP,
so the match engine stays on (the jump turns it off, src/recomp_rt/win/season_report.c). RECOMP_SAVE_AFTER_DAYS is
365 x --seasons: at the first human turn that many game days after its first, the runtime saves samesave.sav and
ends the program (src/recomp_rt/win/testrun.c). The rules report (RECOMP_RULES_REPORT) is written as in the jump.

The checks:
    - no message box of the game in its log (RECOMP_SCREEN_LOG prints "recomp: message box from", waits.c): the
      script stops the game by its PID at the first one, so a box does not wait for the timeout
    - no stall: once the game is made, the game date (the "date d/y" of the screen log) moves at least once in
      --stall-minutes
    - the program ends with code 0 and leaves samesave.sav in its user folder
The script prints the wall time at each 1 July that the game reaches (the time per season) and waits before the
start while /Users/max/Projects/cm0102/run-logs/HOLD exists, while 15 or more games run on this Mac, or while the load
of the last minute is 24 or more.
"""
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402
import smoke_run  # noqa: E402
import custom_database_run as cdr  # noqa: E402

HOLD = "/Users/max/Projects/cm0102/run-logs/HOLD"
BOX = re.compile(r"recomp: message box from (\S+): (.*)")
DATE = re.compile(r"date (\d+)/(\d+)")


def arg(argv, name, default=None):
    return argv[argv.index(name) + 1] if name in argv else default


def wait_for_room():
    """Waits until the HOLD file is gone, fewer than 15 games run and the load is below 24."""
    while True:
        games = subprocess.run(["pgrep", "-x", "cm0102_recomp"], capture_output=True, text=True).stdout.split()
        if len(games) < 15 and os.getloadavg()[0] < 24 and not os.path.exists(HOLD):
            return
        time.sleep(30)


def nation_list(text):
    words = [x.strip().lower().replace("_", " ") for x in text.split(",")]
    names = [next(n for n in rrc.NATION_ROWS if n.lower() == w.lstrip("-")) for w in words if w != "all"]
    nations = (["all"] + names) if words[0] == "all" else names
    if nations[0] != "all" and "England" not in nations:
        nations = ["England"] + nations                      # Arsenal: England stays selected
    return nations


def main(argv):
    if len(argv) < 3 or "--keep" not in argv:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    work = os.path.abspath(arg(argv, "--keep"))
    start, seasons = int(arg(argv, "--start", 2001)), int(arg(argv, "--seasons", 2))
    database = os.path.abspath(arg(argv, "--database")) if "--database" in argv else None
    stall = 60 * int(arg(argv, "--stall-minutes", 20))
    timeout = int(arg(argv, "--timeout", 86400))
    if os.path.exists(work):
        print(f"full_play_run.py: {work} exists; give a new --keep folder")
        return 2
    user, home, shots = (os.path.join(work, d) for d in ("user", "home", "shots"))
    for d in (user, home, shots):
        os.makedirs(d)
    options = {"display": "classic", "changing_rules": arg(argv, "--changing-rules", "on"), "faster_days": "on"}
    if database:
        cdr.copy_data(database, os.path.join(user, "Databases", os.path.basename(database)))
        options["database"] = "folder:" + os.path.basename(database)
    with open(os.path.join(user, "settings.txt"), "w") as f:
        for k, v in options.items():
            f.write(f"{k} = {v}\n")
    steps = rrc.league_steps(nation_list(arg(argv, "--nations", "all")), [])
    table = os.path.join(os.path.dirname(program), "seasons", "rules.tsv")
    env = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
               RECOMP_USER_DIR=user, RECOMP_FIXED_CLOCK="1", RECOMP_SCREEN_LOG="1",
               RECOMP_SAVE_AFTER_DAYS=str(365 * seasons), RECOMP_START_SEASON=str(start),
               RECOMP_RULES_REPORT=os.path.join(work, "report.tsv"), RECOMP_RULES_TABLE=table,
               RECOMP_SCRIPT="; ".join(smoke_run.scaled_steps("classic", shots, steps)))
    wait_for_room()
    t0 = time.time()
    err = os.path.join(work, "stderr.txt")
    fails, box = [], None
    with open(err, "w") as log:
        p = subprocess.Popen([program, "--game", game, "-seed", arg(argv, "--seed", "1")], env=env,
                             stdout=subprocess.DEVNULL, stderr=log)
        last_date, moved, reached = None, time.time(), set()
        while p.poll() is None:
            time.sleep(30)
            text = open(err, errors="replace").read()
            m = BOX.search(text)
            if m:
                time.sleep(3)
                dates = DATE.findall(open(err, errors="replace").read())
                box = f"{m.group(2).strip()} (from {m.group(1)}, day {'/'.join(dates[-1]) if dates else '?'})"
                p.terminate()
                break
            dates = DATE.findall(text)
            if dates:
                day, year = int(dates[-1][0]), int(dates[-1][1])
                if (day, year) != last_date:
                    last_date, moved = (day, year), time.time()
                for y in range(start + 1, start + seasons + 1):
                    if y not in reached and (year > y or (year == y and day >= 182)):
                        reached.add(y)
                        print(f"1 July {y}: {(time.time() - t0) / 60:.0f} min", flush=True)
            # The making of a new game (date day 0) is not a stall: with Select All on a loaded Mac it takes about
            # 30 minutes. The stall clock runs from the first date past day 0.
            if last_date and last_date[0] > 0 and time.time() - moved > stall:
                fails.append(f"stall: the date stays at {last_date} for {stall // 60} minutes")
                p.terminate()
                break
            if time.time() - t0 > timeout:
                fails.append(f"timeout after {timeout} s")
                p.terminate()
                break
        p.wait()
    if box:
        fails.append(f"message box: {box}")
    elif not fails:
        if p.returncode != 0:
            fails.append(f"the program ended with code {p.returncode}")
        if not os.path.exists(os.path.join(user, smoke_run.SAVE_NAME)):
            fails.append(f"no save after {365 * seasons} game days")
    print(f"run: {(time.time() - t0) / 60:.0f} min, log {err}")
    for f in fails:
        print(f"FAIL  {f}")
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
