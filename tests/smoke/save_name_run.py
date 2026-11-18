#!/usr/bin/env python3
"""Save names test (#34, #139, #164): Save Game suggests "Club YYYY-MM-DD", or "Nation YYYY-MM-DD" for a
national team manager with no club, and an edited name saves as typed.

Usage:
    python3 tests/smoke/save_name_run.py <cm0102_recomp> <game folder> [--shots <folder>] [--maker <cm0102_recomp>]

The runs start from the saved test game prematch.sav (tests/fixtures/fixtures.py): Arsenal, Monday
16.7.01. Checks:
    - run 1 chooses Game Options, Save Game and Ok: the user folder holds "Arsenal 2001-07-16.sav"
    - run 2 deletes the date with Backspace (the entry adds typed text at the end) and types "Final": the user folder holds "Arsenal Final.sav"
    - run 3 has only the save of run 1 in a new user folder, chooses Load Game, the first save of the list
      and Ok, and reaches the news screen of the loaded game
    - run 4 loads a game whose manager has a national team and no club: Save Game saves "Republic of Ireland
      2001-07-14.sav"
    - run 5 loads a game whose manager has a club and a national team: Save Game saves "Arsenal 2001-07-14.sav"
The saves of runs 4 and 5 come from start.sav (Arsenal, 14.7.01) with the test switch RECOMP_TEST_NATION_JOB
(src/recomp_rt/win/nation_job.c). England has a manager, so the switch takes the first free team, Republic of
Ireland. --maker makes these saves with another build, so that a build without the switch can be tested.
Two runs play at a time. With --shots the dumps of the save screens and of the load list stay in that folder
(.bmp, with a .png copy).
"""
import os
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "fixtures"))
import fixtures  # noqa: E402

SUGGESTED = "Arsenal 2001-07-16.sav"
EDITED = "Arsenal Final.sav"
NATION_ONLY = "Republic of Ireland 2001-07-14.sav"
NATION_AND_CLUB = "Arsenal 2001-07-14.sav"
# Times count from the end of the last wait step.
SAVE_GAME = ["1000 click 45 342",       # Game Options
             "3000 click 140 334"]      # Save Game


def play(program, game, user, steps, env=None):
    os.makedirs(user, exist_ok=True)
    fixtures.write_settings(user, "classic", [])
    steps = fixtures.smoke_run.scaled_steps("classic", "", steps)
    r, _ = fixtures.run(program, game, "classic", user, steps, env or {}, 300)
    return r


def national_save(program, game, work, name, only):
    """A save of start.sav whose human manager has a national team, and no club when only is set."""
    user = os.path.join(work, name)
    os.makedirs(user)
    shutil.copyfile(os.path.join(fixtures.DEFAULT_DIR, "start.sav"), os.path.join(user, "start.sav"))
    env = {"RECOMP_TEST_NATION_JOB": "England", "RECOMP_SAVE_AFTER_DAYS": "0"}
    if only:
        env["RECOMP_TEST_NATION_JOB_ONLY"] = "1"
    r = play(program, game, user, fixtures.LOAD_STEPS + ["120000 quit"], env)
    save = os.path.join(user, fixtures.smoke_run.SAVE_NAME)
    return save if r.returncode == 0 and os.path.exists(save) else None


def sav_files(folder):
    return sorted(n for n in os.listdir(folder) if n.lower().endswith(".sav"))


def main(argv):
    options = dict(zip(argv[3::2], argv[4::2]))
    if len(argv) < 3 or len(argv) % 2 == 0 or set(options) - {"--shots", "--maker"}:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    maker = os.path.abspath(options.get("--maker", program))
    fixture = os.path.join(fixtures.DEFAULT_DIR, "prematch.sav")
    for save in (fixture, os.path.join(fixtures.DEFAULT_DIR, "start.sav")):
        if not os.path.exists(save):
            print(f"no saved game {save}: see tests/fixtures/fixtures.py")
            return 2
    failed = []

    def check(ok, text):
        print(("ok   " if ok else "FAIL ") + text)
        if not ok:
            failed.append(text)

    with tempfile.TemporaryDirectory(prefix="cm0102-savename-") as work:
        shots = os.path.abspath(options.get("--shots", work))
        os.makedirs(shots, exist_ok=True)
        users = [os.path.join(work, n) for n in ("user1", "user2", "user3")]
        for user in users[:2]:
            os.makedirs(user)
            shutil.copyfile(fixture, os.path.join(user, "prematch.sav"))
        suggested = fixtures.LOAD_STEPS + SAVE_GAME + [f"4500 dump {shots}/save_suggested.bmp",
                                                       "5000 click 705 572", "9000 quit"]     # Ok
        edited = (fixtures.LOAD_STEPS + SAVE_GAME + [f"{4500 + 200 * i} key 8" for i in range(10)] +
                  ["7000 type Final", f"8500 dump {shots}/save_edited.bmp", "9000 click 705 572", "13000 quit"])
        with ThreadPoolExecutor(2) as pool:
            r1, r2 = pool.map(lambda a: play(program, game, *a), [(users[0], suggested), (users[1], edited)])
        check(r1.returncode == 0, f"run 1 (suggested name) ended with code {r1.returncode}")
        check(SUGGESTED in sav_files(users[0]), f"run 1 saved {sav_files(users[0])}")
        check(r2.returncode == 0, f"run 2 (edited name) ended with code {r2.returncode}")
        check(EDITED in sav_files(users[1]), f"run 2 saved {sav_files(users[1])}")

        if os.path.exists(os.path.join(users[0], SUGGESTED)):
            os.makedirs(users[2])
            shutil.copyfile(os.path.join(users[0], SUGGESTED), os.path.join(users[2], SUGGESTED))
            load = list(fixtures.LOAD_STEPS)
            load.insert(load.index("4000 click 445 257"), f"2000 dump {shots}/load_list.bmp")
            loaded = os.path.join(work, "loaded.bmp")
            r3 = play(program, game, users[2], load + [f"2000 dump {loaded}", "3000 quit"])
            check(r3.returncode == 0 and os.path.exists(loaded),
                  f"run 3 loaded {SUGGESTED} from the load list (code {r3.returncode})")

        # A national team manager, with no club and with a club.
        cases = [("user4", True, NATION_ONLY, "save_nation.bmp"), ("user5", False, NATION_AND_CLUB, "save_nation_club.bmp")]
        with ThreadPoolExecutor(2) as pool:
            saves = list(pool.map(lambda c: national_save(maker, game, work, c[0] + "-make", c[1]), cases))

        def national(case, save):
            user = os.path.join(work, case[0])
            os.makedirs(user)
            shutil.copyfile(save, os.path.join(user, "national.sav"))
            return user, play(program, game, user, fixtures.LOAD_STEPS + SAVE_GAME + [
                f"4500 dump {shots}/{case[3]}", "5000 click 705 572", "9000 quit"])

        ready = [(c, s) for c, s in zip(cases, saves) if s]
        check(len(ready) == len(cases), f"made {len(ready)} of {len(cases)} national team saves")
        with ThreadPoolExecutor(2) as pool:
            for (case, _), (user, r) in zip(ready, pool.map(lambda a: national(*a), ready)):
                check(r.returncode == 0, f"{case[0]} ended with code {r.returncode}")
                check(case[2] in sav_files(user), f"{case[0]} saved {sav_files(user)}, want {case[2]}")

        for f in sorted(os.listdir(shots)):
            if f.endswith(".bmp") and shutil.which("sips"):
                subprocess.run(["sips", "-s", "format", "png", os.path.join(shots, f), "--out",
                                os.path.join(shots, f[:-4] + ".png")], capture_output=True)
    print("PASS" if not failed else "FAIL")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
