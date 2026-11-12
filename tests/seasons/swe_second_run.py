#!/usr/bin/env python3
"""Sweden: the Second Division groups take the clubs that the data holds in them (open item 224; swe_second_groups.c).

Usage:
    python3 tests/seasons/swe_second_run.py <cm0102_recomp> <game folder> [--only d3960,d3968] [--jobs n]
        [--keep <folder>] [--data-3960 <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py: a new 2001/02 game with Sweden alone and its extra
division (the test switch RECOMP_TEST_EXTRA=sweden sets the flag that the player sets on Select League(s)), changing
rules off, played to the end of 2001/02. So the game makes the Swedish Second Division (SweSecond 0x8916f0), plays
the Swedish season of 2001 to its end and starts the season of 2002. The runs:
    d3960  the 3.9.60 data (the default 2001/02 data since 0.6.1): East Svealand holds 11 clubs, as the real 2001
           Division 2 Östra Svealand. Before the fix the game stopped at "swe_second..cpp 656". The log names the group
           and its 11 clubs
    d3968  the 3.9.68 data: every group holds 12, so the groups take the class count and the log names none
The 3.9.65 data holds 13 clubs in East Svealand and still stops at "swe_second..cpp 643" (a group over the class count
does not fit the class's tables; open item 224), so it has no run here.
The 3.9.60 data comes from <game folder>/Data-3.9.60, or from --data-3960.
All runs end with code 0 and show no message box of the game.
"""
import concurrent.futures
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rules_report_check as rrc  # noqa: E402
import uefa_formats_run as ufr  # noqa: E402

GROUP = re.compile(r"recomp: swe_second: (.*) holds (\d+) clubs in the data, the class takes (\d+): the group takes")
WANT = {"d3960": {"Swedish Second Division East Svealand": 11},
        "d3968": {}}


def run(name, program, game, work, data):
    options = {"changing_rules": "off", "database": "3.9.60" if name == "d3960" else "3.9.68"}
    env = {"RECOMP_TEST_EXTRA": "sweden"}
    if data:
        env["RECOMP_DATA_DIR"] = data
    code, _ = rrc.run(program, game, 2001, None, None, ["Sweden"], [], work, 7200, options, env)
    fails = [] if code == 0 else [f"{name}: the jump ended with code {code}"]
    found, log = ufr.boxes(work)
    fails += [f"{name}: message box {b}" for b in found]
    if "extra division of sweden on" not in log:
        fails.append(f"{name}: the Swedish extra division was not set (no Swedish Second Division made)")
    got, lines = {}, 0
    for m in GROUP.finditer(log):                   # the new game and each season rebuild (0x8925a4) log
        lines += 1
        got.setdefault(m.group(1), int(m.group(2)))
        if int(m.group(2)) >= int(m.group(3)):
            fails.append(f"{name}: {m.group(1)} took {m.group(2)} clubs, not fewer than the class count {m.group(3)}")
        if int(m.group(3)) != 12:
            fails.append(f"{name}: the class count is {m.group(3)}, want 12 (0x8921c2)")
    print(f"{name}: groups with their own count at the new game: {got or 'none'} ({lines} log lines)")
    if any(got.get(g) != n for g, n in WANT[name].items()) or (not WANT[name] and got):
        fails.append(f"{name}: groups with their own count {got}, want {WANT[name]}")
    return fails


def main():
    args = sys.argv[1:]
    if len(args) < 2:
        raise SystemExit(__doc__)
    program, game = os.path.abspath(args[0]), os.path.abspath(args[1])

    def opt(flag, default=None):
        return args[args.index(flag) + 1] if flag in args else default

    only = opt("--only", "d3960,d3968").split(",")
    jobs = int(opt("--jobs", "1"))
    keep = opt("--keep")
    data = {"d3960": opt("--data-3960", os.path.join(game, "Data-3.9.60")), "d3968": None}
    base = keep or tempfile.mkdtemp(prefix="cm0102-swe-second-")
    fails = []
    for name in only:
        if data.get(name) and not os.path.isdir(data[name]):
            fails.append(f"{name}: no data folder {data[name]}")
    todo = [n for n in only if n in WANT and not (data.get(n) and not os.path.isdir(data[n]))]
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = {pool.submit(run, n, program, game, os.path.join(base, n), data[n]): n for n in todo}
        for f in concurrent.futures.as_completed(futures):
            fails += f.result()
    for f in fails:
        print("FAIL", f)
    print("PASS" if not fails else f"FAIL ({len(fails)})")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
