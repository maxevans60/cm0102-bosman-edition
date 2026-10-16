#!/usr/bin/env python3
"""Database button order test (0.4.1, Q174): the button on Setup Game steps through the data sets in season order.

Usage:
    python3 tests/smoke/database_order_run.py <cm0102_recomp> <game folder> [--shots <folder>]

Each run uses a new user folder (RECOMP_USER_DIR). Its Databases folder holds four folders with empty data
files, made in an order that is not the season order: "May 2026", "Zoë Notes" (no date), "April 2010 SIM" and
"December 2025". The button only lists the folders, so the files can be empty. A run clicks the button n
times and quits. The database line in settings.txt must then name the data set n steps after 3.9.68:
3.9.60, 3.9.65, April 2010 SIM, December 2025, May 2026, Zoë Notes, and 3.9.68 again.
The runs play in classic and widescreen at the same time. With --shots, the classic and widescreen runs of
all seven clicks save a picture of the screen after each click in that folder.
"""
import concurrent.futures
import os
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import smoke_run  # noqa: E402

FILES = ["city.dat", "club.dat", "club_comp.dat", "club_comp_history.dat", "colour.dat", "common_names.dat",
         "continent.dat", "first_names.dat", "index.dat", "nat_club.dat", "nation.dat", "nation_comp.dat",
         "nation_comp_history.dat", "officials.dat", "second_names.dat", "stadium.dat", "staff.dat",
         "staff_comp.dat", "staff_comp_history.dat", "staff_history.dat"]
FOLDERS = ["May 2026", "Zoë Notes", "April 2010 SIM", "December 2025"]
EXPECT = ["3.9.68", "3.9.60", "3.9.65", "folder:April 2010 SIM", "folder:December 2025", "folder:May 2026",
          "folder:Zoë Notes", "3.9.68"]
BUTTON = (613, 437)   # the Database button in classic, the free cell after Web Sites


def run(program, game, mode, clicks, shots):
    """Clicks the button clicks times. Returns a list of failure texts."""
    with tempfile.TemporaryDirectory(prefix="cm0102-dborder-") as work:
        home, user = os.path.join(work, "home"), os.path.join(work, "user")
        os.mkdir(home)
        for folder in FOLDERS:
            d = os.path.join(user, "Databases", folder)
            os.makedirs(d)
            for f in FILES:
                open(os.path.join(d, f), "w").close()
        with open(os.path.join(user, "settings.txt"), "w") as f:
            f.write(f"display = {mode}\n")
        steps = ["1000 click 400 300", "8200 click 522 272", "8500 click 662 524", "wait setup"]
        for i in range(clicks):
            steps.append(f"{2000 + 1500 * i} click {BUTTON[0]} {BUTTON[1]}")
            if shots:
                steps.append(f"{2900 + 1500 * i} dump {shots}/{mode}-{i + 1}.bmp")
        steps.append(f"{2000 + 1500 * clicks + 500} quit")
        env = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
                   RECOMP_USER_DIR=user, RECOMP_SCRIPT="; ".join(smoke_run.scaled_steps(mode, "", steps)))
        r = subprocess.run([program, "--game", game], env=env, capture_output=True, text=True, timeout=300)
        if r.returncode != 0:
            return [f"{mode} {clicks}: the game ended with code {r.returncode}: {r.stderr[-300:]}"]
        got = None
        with open(os.path.join(user, "settings.txt"), encoding="utf-8") as f:
            for line in f:
                key, _, value = line.partition("=")
                if key.strip() == "database":
                    got = value.strip()
        want = EXPECT[clicks]
        print(f"{'ok   ' if got == want else 'FAIL '} {mode} {clicks} clicks: database = {got} (want {want})")
        return [] if got == want else [f"{mode} {clicks}: {got}"]


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = (os.path.abspath(a) for a in argv[1:3])
    shots = os.path.abspath(argv[argv.index("--shots") + 1]) if "--shots" in argv else None
    jobs = [(m, n, shots if n == 7 else None) for m in ("classic", "widescreen") for n in range(1, 8)]
    failures = []
    with concurrent.futures.ThreadPoolExecutor(len(jobs)) as pool:
        for res in pool.map(lambda j: run(program, game, *j), jobs):
            failures += res
    print("PASS" if not failures else "FAIL: " + "; ".join(failures))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
