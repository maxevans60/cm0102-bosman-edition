#!/usr/bin/env python3
"""Load Last Save test (0.9.0): the Setup Game item loads the newest saved game at one click.

Usage:
    python3 tests/smoke/load_last_run.py <cm0102_recomp> <game folder>

The user folder holds two copies of a saved game (fixtures start.sav), "A first.sav" and "Z second.sav", with
different file times. The test clicks the Load Last Save item (classic numbers 445 505) and reads the trace
(RECOMP_TRACE) for the saved game that the game opens last. Run 1 makes Z the newer file and expects Z, run 2 makes A the
newer file and expects A. Both runs must reach the first news screen. Run 3 has no saved game: the click does nothing and
the game stays on Setup Game. The saved games stay outside git (tests/fixtures/fixtures.py).
"""
import os
import shutil
import sys
import tempfile

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "fixtures"))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fixtures  # noqa: E402
import load_order_run as lo  # noqa: E402

# The steps of Load Game up to the setup screen, then the Load Last Save item.
SETUP = fixtures.LOAD_STEPS[:fixtures.LOAD_STEPS.index("1000 click 277 242")]
LAST = SETUP + ["1000 click 445 505", "wait first-news", "2000 quit"]
NONE = SETUP + ["1000 click 445 505", "3000 dump {out}", "4000 quit"]


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    if not os.path.exists(os.path.join(fixtures.DEFAULT_DIR, "start.sav")):
        print("no start.sav: see tests/fixtures/fixtures.py")
        return 2
    failed = []

    def check(ok, text):
        print(("ok   " if ok else "FAIL ") + text)
        if not ok:
            failed.append(text)

    with tempfile.TemporaryDirectory(prefix="cm0102-loadlast-") as work:
        for name, z_newer, want in (("newer-z", True, "Z second.sav"), ("newer-a", False, "A first.sav")):
            r = lo.play(program, game, lo.setup(work, name, z_newer), LAST)
            got = lo.loaded_save(r)
            check(r.returncode == 0 and got == want and "script reached first-news" in r.stderr,
                  f"{name}: Load Last Save loads {want} (loaded {got}, code {r.returncode})")
        user = os.path.join(work, "empty")
        os.makedirs(user)
        fixtures.write_settings(user, "classic", [])
        r = lo.play(program, game, user, [s.replace("{out}", os.path.join(work, "none.bmp")) for s in NONE])
        check(r.returncode == 0 and "script reached first-news" not in r.stderr and lo.loaded_save(r) is None,
              f"empty: with no saved game the click does nothing (code {r.returncode})")
    print("PASS" if not failed else "FAIL")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
