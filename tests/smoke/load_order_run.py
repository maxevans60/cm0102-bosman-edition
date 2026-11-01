#!/usr/bin/env python3
"""Load list order test (0.9.0): the Load Game list shows the newest saved game first.

Usage:
    python3 tests/smoke/load_order_run.py <cm0102_recomp> <game folder>

The user folder holds two copies of a saved game (fixtures start.sav), "A first.sav" and "Z second.sav", with
different file times. The test clicks the first row of the Load Game list and reads the trace (RECOMP_TRACE) for the
saved game that the game opens last, which is the one it loads. The original lists names in order, so the first row
would always be "A first.sav". Run 1 makes Z the newer file and expects Z. Run 2 makes A the newer file and expects A,
so the test shows that it sees the order. The saved games stay outside git (tests/fixtures/fixtures.py).
"""
import os
import re
import shutil
import sys
import tempfile
import time

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "fixtures"))
import fixtures  # noqa: E402


def play(program, game, user, steps):
    r, _ = fixtures.run(program, game, "classic", user, steps, {"RECOMP_TRACE": "1"}, 180)
    return r


def loaded_save(r):
    """The name of the saved game that the game opened last."""
    names = re.findall(r"CreateFileA\([^>]*-> [^,]*/([^/,]+\.sav), ", r.stderr)
    return names[-1] if names else None


def setup(work, name, z_is_newer):
    user = os.path.join(work, name)
    os.makedirs(user)
    fixtures.write_settings(user, "classic", [])
    now = time.time()
    for save, age in (("A first.sav", 1000 if z_is_newer else 0), ("Z second.sav", 0 if z_is_newer else 1000)):
        path = os.path.join(user, save)
        shutil.copyfile(os.path.join(fixtures.DEFAULT_DIR, "start.sav"), path)
        os.utime(path, (now - age, now - age))
    return user


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    if not os.path.exists(os.path.join(fixtures.DEFAULT_DIR, "start.sav")):
        print("no start.sav: see tests/fixtures/fixtures.py")
        return 2
    steps = fixtures.LOAD_STEPS + ["2000 quit"]
    failed = []

    def check(ok, text):
        print(("ok   " if ok else "FAIL ") + text)
        if not ok:
            failed.append(text)

    with tempfile.TemporaryDirectory(prefix="cm0102-loadorder-") as work:
        for name, z_newer, want in (("newer-z", True, "Z second.sav"), ("newer-a", False, "A first.sav")):
            r = play(program, game, setup(work, name, z_newer), steps)
            got = loaded_save(r)
            check(r.returncode == 0 and got == want, f"{name}: the first row loads {want} (loaded {got}, code {r.returncode})")
    print("PASS" if not failed else "FAIL")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
