#!/usr/bin/env python3
"""Brazil's state championships by state (0.6.0 slice 20e, Q332; docs/research/2026-10-20-brazil-states.md).

Usage:
    python3 tests/seasons/bra_states_run.py <program> <game folder> [--only a,b] [--jobs n] [--keep folder]

Runs (Brazil selected, RECOMP_BRA_STATES_LOG=1):
    2001-on   the 3.9.68 data, 2001/02 with changing rules on, to 2002/03
    2014-on   October 2014, to 2015/16
    2021-on   October 2021 (no state code in the city names: the tables of Q600), to 2022/23
    2001      the base game (changing rules off): no line of this slice (Q152)
    load      October 2014, a save in March 2015 inside the state season, loaded and played on 60 days

A run passes when it ends with no error box, each of the 15 state objects that is made picks only clubs of its
state, no club is in two of the 15 state championships of one year (a region object that keeps its region logs its
clubs with the state "RG"), the three region objects take their state, and the championships of the next calendar
year pick again (the season changed).
"""
import collections
import concurrent.futures
import os
import re
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
# name: (start year, database folder or None, last season, changing rules, days before a save or 0)
RUNS = {
    "2001-on": (2001, None, 2002, "on", 0),
    "2014-on": (2014, "October 2014", 2015, "on", 0),
    "2021-on": (2021, "October 2021", 2022, "on", 0),
    "2001": (2001, None, 2002, "off", 0),
    "load": (2014, "October 2014", 2014, "on", 250),
}
REGIONS = {"Brazilian North State Championship": "PA", "Brazilian Northeast State Championship": "CE",
           "Brazilian Central State Championship": "MT"}
PICK = re.compile(r"recomp: brazil states: (.+?) (\d{4}): (\d+) clubs of ([A-Z]{2}) \(row (-?\d+), (\d+) in the data\):(.*)")
BOX = re.compile(r"message box from 0x[0-9a-f]+: (.*)")


def check_log(name, text, base):
    fails = []
    boxes = BOX.findall(text)
    fails += [f"{name}: message box {b}" for b in boxes]
    picks = [(m.group(1), int(m.group(2)), int(m.group(3)), m.group(4), [c.strip() for c in m.group(7).split(";") if c.strip()])
             for m in PICK.finditer(text)]
    if base:
        if "brazil states" in text:
            fails.append(f"{name}: the base game logs this slice")
        return fails
    if not picks:
        return fails + [f"{name}: no state championship picked its clubs"]
    by_year = collections.defaultdict(dict)
    for record, year, n, state, clubs in picks:
        by_year[year][record] = (state, clubs)
        if record in REGIONS and REGIONS[record] != state and state != "RG":   # RG: the region object kept its region
            fails.append(f"{name}: {record} {year} takes {state}")
        if len(clubs) != n or len(set(clubs)) != n:
            fails.append(f"{name}: {record} {year}: {n} clubs, the list holds {len(clubs)} ({len(set(clubs))} distinct)")
    for year, objects in sorted(by_year.items()):
        seen = {}
        for record, (state, clubs) in objects.items():
            for c in clubs:
                if c in seen and seen[c] != record:
                    fails.append(f"{name}: {year}: {c} in {seen[c]} and {record}")
                seen[c] = record
        print(f"  {name}: {year}: {len(objects)} state championships, {sum(len(v[1]) for v in objects.values())} clubs")
    if len(by_year) < 2 and name != "load":
        fails.append(f"{name}: the state championships picked in one year only (the season did not change)")
    return fails


def run_one(program, game, name, keep):
    year, db, last, rules, save_days = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-bra-states-{name}-")
    folder = os.path.join(DB, db) if db else None
    env = {"RECOMP_BRA_STATES_LOG": "1"}
    if save_days:
        env["RECOMP_SAVE_AFTER_DAYS"] = str(save_days)
    code, _ = rrc.run(program, game, last, year, folder, ["Brazil"], [], work, 7200,
                      options_more={"changing_rules": rules}, env_more=env)
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    if code != 0:
        return [f"{name}: the run ended with code {code} ({work})"]
    fails = check_log(name, log, rules == "off")
    if save_days:
        fails += run_load(program, game, work)
    return fails


def run_load(program, game, work):
    sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
    import fixtures
    import smoke_run
    first = os.path.join(work, "user", smoke_run.SAVE_NAME)
    if not os.path.exists(first):
        return [f"load: the first run made no save ({work})"]
    again = os.path.join(work, "again")
    user = os.path.join(again, "user")
    os.makedirs(user, exist_ok=True)
    shutil.copyfile(first, os.path.join(user, "states.sav"))
    fixtures.write_settings(user, "classic", [])
    steps = fixtures.LOAD_STEPS[:-1] + ["900000 quit"]
    table = os.path.join(os.path.dirname(os.path.abspath(program)), "seasons", "rules.tsv")
    r, _ = fixtures.run(program, game, "classic", user, smoke_run.scaled_steps("classic", again, steps),
                        {"RECOMP_SAVE_AFTER_DAYS": "60", "RECOMP_RULES_TABLE": table, "RECOMP_BRA_STATES_LOG": "1",
                         "RECOMP_SCREEN_LOG": "1"}, 2400)
    with open(os.path.join(again, "stderr.txt"), "w") as f:
        f.write(r.stderr)
    fails = [f"load: message box {b}" for b in BOX.findall(r.stderr)]
    if r.returncode != 0 or not os.path.exists(os.path.join(user, smoke_run.SAVE_NAME)):
        fails.append(f"load: the loaded game did not play on and save (code {r.returncode}, {again})")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only")
    names = only.split(",") if only else list(RUNS)
    jobs = int(rrc.arg(argv, "--jobs", 1))
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    failed = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        for f in [pool.submit(run_one, program, game, n, keep) for n in names]:
            failed += f.result()
    for text in failed:
        print("FAIL  " + text)
    print("PASS" if not failed else f"FAILED: {len(failed)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
