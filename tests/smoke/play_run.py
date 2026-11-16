#!/usr/bin/env python3
"""Play test (web/README.md, "Play"): the runtime loads a save by its name, as the page's Play button asks.

The page calls rt_web_play with the file name; a run without a page sets RECOMP_PLAY. At the start screen the
runtime clicks Restore Saved Game (action id 3, 0x81ca4f), types the name in the dialog's File entry and clicks its
Ok (action id 4, 0x590724) until the dialog runs it (src/recomp_rt/win/play.c).

Usage:
    python3 tests/smoke/play_run.py <cm0102_recomp> <game folder>

The runs use the saved test game prematch.sav (tests/fixtures/fixtures.py) under other names. Checks:
    - run 1 holds Alpha.sav, "Köln 2001.sav" and Zeta.sav and asks for "Köln 2001.sav" (not first in the list, a
      name with a letter of code page 1252); run 2 holds Alpha.sav and "Ελλάδα 2001.sav" (Greek, outside code page
      1252: the game's list shows its UTF-8 bytes, and the runtime types the same bytes) and asks for the Greek one
    - each reaches the news screen of the loaded game, chooses Game Options, Save Game, Ok and Yes (Replace this
      file?): the save it writes is the one asked for (Save Game offers the name of the save in play, savename.c),
      and the others are as they were
    - run 3 starts again in the folder of run 1, where the game may keep the name of the save loaded last (game
      options + 0x113, the File entry's first text): it asks for Zeta.sav, and Save Game writes Zeta.sav
    - run 4 asks for "Cup [final].sav": the File entry takes no [ ], so the runtime says so at once and loads nothing
    - run 5 is in widescreen (1280x800) with Broken.sav first in the list (a save cut short: loading it would fail)
      and asks for Wide.sav: it reaches the news screen of the loaded game
Runs 1, 2, 4 and 5 play at the same time; run 3 after run 1.
"""
import os
import shutil
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "fixtures"))
import fixtures  # noqa: E402

WANTED = "Köln 2001.sav"
OTHERS = ["Alpha.sav", "Zeta.sav"]
GREEK = "Ελλάδα 2001.sav"
REFUSED = "Cup [final].sav"
START = fixtures.LOAD_STEPS[:fixtures.LOAD_STEPS.index("wait setup") + 1]
SAVE_GAME = ["1000 click 45 342",       # Game Options
             "3000 click 140 334",      # Save Game
             "5000 click 705 572",      # Ok
             "7000 click 705 573",      # Yes: replace the file
             "12000 quit"]


def play(program, game, user, steps, wanted, mode="classic"):
    fixtures.write_settings(user, mode, [])
    steps = fixtures.smoke_run.scaled_steps(mode, "", steps)
    r, _ = fixtures.run(program, game, mode, user, steps, {"RECOMP_PLAY": wanted}, 300)
    return r


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    fixture = os.path.join(fixtures.DEFAULT_DIR, "prematch.sav")
    if not os.path.exists(fixture):
        print(f"no saved game {fixture}: see tests/fixtures/fixtures.py")
        return 2
    failed = []

    def check(ok, text):
        print(("ok   " if ok else "FAIL ") + text)
        if not ok:
            failed.append(text)

    with tempfile.TemporaryDirectory(prefix="cm0102-play-") as work:
        users = [os.path.join(work, n) for n in ("user1", "user2", "user4", "user5")]
        held = {users[0]: OTHERS + [WANTED], users[1]: [GREEK, "Alpha.sav"], users[2]: [REFUSED], users[3]: ["Wide.sav"]}
        for user, names in held.items():
            os.makedirs(user)
            for n in names:
                shutil.copyfile(fixture, os.path.join(user, n))
                os.utime(os.path.join(user, n), (1e9, 1e9))
        with open(fixture, "rb") as f:
            cut = f.read(400 << 10)
        with open(os.path.join(users[3], "Broken.sav"), "wb") as f:
            f.write(cut)
        runs = [(users[0], START + ["wait first-news"] + SAVE_GAME, WANTED),
                (users[1], START + ["wait first-news"] + SAVE_GAME, GREEK),
                (users[2], START + ["8000 quit"], REFUSED),
                (users[3], START + ["wait first-news", "2000 quit"], "Wide.sav", "widescreen")]
        with ThreadPoolExecutor(4) as pool:
            results = list(pool.map(lambda a: play(program, game, *a), runs))
        logs = dict(zip(("1", "2", "4", "5"), results))

        for i, (r, (user, _, wanted, *_)) in enumerate(zip(results[:2], runs[:2]), 1):
            others = [n for n in held[user] if n != wanted]
            check(r.returncode == 0, f"run {i} (play {wanted}) ended with code {r.returncode}")
            check(f"recomp: play: {wanted}: Ok" in r.stderr, f"run {i}: the runtime pressed Ok on Restore Saved Game")
            saved = sorted(n for n in os.listdir(user) if n.lower().endswith(".sav"))
            check(saved == sorted(held[user]), f"run {i}: no new file: {saved}")
            check(os.path.getmtime(os.path.join(user, wanted)) > 1e9, f"run {i}: Save Game wrote {wanted}, the save in play")
            check(all(os.path.getmtime(os.path.join(user, n)) == 1e9 for n in others), f"run {i}: the other saves are as they were")

        r4 = results[2]
        check(r4.returncode == 0 and f"recomp: play: {REFUSED}: the name has characters the game's File entry does not take" in r4.stderr
              and f"recomp: play: {REFUSED}: Restore Saved Game" not in r4.stderr and "script reached first-news" not in r4.stderr,
              "run 4: a name with [ ] is refused at once, and nothing is loaded")
        r5 = results[3]
        check(r5.returncode == 0 and "recomp: play: Wide.sav: Ok" in r5.stderr and "script reached first-news" in r5.stderr,
              f"run 5 (widescreen, a broken save first in the list) loaded Wide.sav (code {r5.returncode})")

        before = os.path.getmtime(os.path.join(users[0], WANTED))
        # A second start in the folder: no settings screen, and the play leaves the start screen by itself.
        r3 = play(program, game, users[0], [START[0], "wait first-news"] + SAVE_GAME, "Zeta.sav")
        logs["3"] = r3
        check(r3.returncode == 0 and "recomp: play: Zeta.sav: Ok" in r3.stderr, f"run 3 (play Zeta.sav after a game of Köln) ended with code {r3.returncode}")
        check(os.path.getmtime(os.path.join(users[0], "Zeta.sav")) > 1e9 and os.path.getmtime(os.path.join(users[0], WANTED)) == before,
              "run 3: Save Game wrote Zeta.sav, and Köln 2001.sav is as run 1 left it")
        if failed:
            for i, r in sorted(logs.items()):
                with open(os.path.join(os.environ.get("PLAY_LOGS", work), f"stderr{i}.txt"), "w") as f:
                    f.write(r.stderr)
                print(f"run {i} log, play lines:\n" + "\n".join(l for l in r.stderr.splitlines() if "play:" in l or "timed out" in l))
    print("PASS" if not failed else "FAIL")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
