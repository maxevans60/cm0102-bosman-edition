#!/usr/bin/env python3
"""Season change check (0.6.0 slice 16, Q232, Q258): the July step and its news item.

Usage:
    python3 tests/seasons/season_change_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>]

Each run is a season jump of tests/seasons/rules_report_check.py over one season change (start in X, last
season X+1), with changing_rules on, the test log RECOMP_SEASON_CHANGE_LOG=1 and RECOMP_SEASON_NAMES=table.
The second variable makes the stand-in of slice 15's names table give the rules table name of a season when it
differs from the name of the season before (src/recomp_rt/win/season_names_stub.c), so the run has real
changes of name before slice 15 is on main. The runs (the era boundaries of the build brief):
    2002    3.9.68 data from 2002/03, across 2002/03 to 2003/04
    2008    September 2008, across 2008/09 to 2009/10 (the UEFA Cup becomes the Europa League)
    2020    November 2020, across 2020/21 to 2021/22
    2023    October 2023, across 2023/24 to 2024/25
    off     September 2008 with changing_rules off: no change and no news
    load    September 2008: the game saves 352 days after the start (mid July 2009, after the change of 21 June),
            then a new program loads that save and plays 2 days on (RECOMP_SAVE_AFTER_DAYS) and saves again.
            The save of a game with a changed name must load: the game finds its key competitions by name
            (0x60efd0), and without the names in the data the load stops at the box "index.cpp 5809". Both
            saves must hold the news text and the changed name in their season part (bosman_season.dat,
            version 2)

The checks (each run):
    - the jump ends with code 0
    - on: the competitions whose rules table name changes from X to X+1 (first row of the file) have the new
      name at the end of X+1 in the rules report (1 June: the international entries change as late as 15 and
      28 August, after the start report of 1 August), and every other competition keeps its name
    - on: the news log lists exactly one sentence "<old name> is now called <new name>." for each of them,
      for season X+1, and the news item of X+1 is sent once (no news item when no name changes)
    - off: the report names at the end of X+1 are the names at the end of X, and the log has no change
A run takes about 6 to 8 minutes. --jobs (default 3) runs that many at the same time. --reuse checks the
reports already in the --keep folder again without a new run.
"""
import collections
import concurrent.futures
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
RUNS = {
    "2002": (2002, None, "on"),
    "2008": (2008, os.path.join(DB, "September 2008"), "on"),
    "2020": (2020, os.path.join(DB, "November 2020"), "on"),
    "2023": (2023, os.path.join(DB, "October 2023"), "on"),
    "off": (2008, os.path.join(DB, "September 2008"), "off"),
    "load": (2008, os.path.join(DB, "September 2008"), "on"),
}
LOAD_NEWS = b"UEFA Cup is now called UEFA Europa League."


def season_part(path):
    sys.path.insert(0, os.path.join(HERE, "..", "..", "tools", "save"))
    import savfile
    return savfile.Sav(path).read_member("bosman_season.dat")


def load_and_save(program, game, save, out, days, env):
    """Loads save (the first in the list) and plays days game days on: the game is on holiday, so no wait for
    the news screen. Returns the result and the path of the new save, or None."""
    sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
    import fixtures
    import smoke_run
    user = os.path.join(out, "user")
    os.makedirs(user, exist_ok=True)
    os.replace(save, os.path.join(user, os.path.basename(save)))
    fixtures.write_settings(user, "classic", [])
    steps = fixtures.LOAD_STEPS[:-1] + ["900000 quit"]
    r, _ = fixtures.run(program, game, "classic", user, smoke_run.scaled_steps("classic", out, steps),
                        dict(env, RECOMP_SAVE_AFTER_DAYS=str(days)), 1200)
    with open(os.path.join(out, "stderr.txt"), "w") as f:
        f.write(r.stderr)
    saved = os.path.join(user, smoke_run.SAVE_NAME)
    return r, saved if os.path.exists(saved) else None


def check_load(program, game, keep):
    """The load run (module docstring)."""
    import shutil
    work = os.path.join(keep, "load") if keep else tempfile.mkdtemp(prefix="cm0102-season-change-load-")
    year, database, _ = RUNS["load"]
    code, _ = rrc.run(program, game, year + 2, year, database, ["England"], [], work, 7200, {"changing_rules": "on"},
                      {"RECOMP_SEASON_CHANGE_LOG": "1", "RECOMP_SEASON_NAMES": "table", "RECOMP_SAVE_AFTER_DAYS": "352"})
    first = os.path.join(work, "user", "samesave.sav")
    if code != 0 or not os.path.exists(first):
        return [f"load: the first run made no save (code {code}, {work})"]
    fails = []
    part = season_part(first)
    if part[:4] != b"\x02\x00\x00\x00" or LOAD_NEWS not in part or b"UEFA Cup\x00" not in part:
        fails.append(f"load: the first save's season part lacks the news text or the name: {part!r}")
    again = os.path.join(work, "again")
    os.makedirs(again, exist_ok=True)
    shutil.copyfile(first, os.path.join(again, "change.sav"))
    r, saved = load_and_save(program, game, os.path.join(again, "change.sav"), again, 2,
                             {"RECOMP_SEASON_CHANGE_LOG": "1"})
    if r.returncode != 0 or not saved:
        fails.append(f"load: the save did not load and play on (code {r.returncode}, {again})")
    else:
        part = season_part(saved)
        if LOAD_NEWS not in part or b"UEFA Cup\x00" not in part:
            fails.append(f"load: the second save's season part lacks the news text or the name: {part!r}")
    print(f"load: {len(fails)} problems ({work})")
    return fails
NEWS = re.compile(r"recomp: season change: news (\d{4})/\d\d: (.*)$")
SENT = re.compile(r"recomp: season change: news item for (\d{4})/\d\d sent$")


def table_names(table_path):
    """{(season, file): name} of the first row of each file in each season (as the stand-in reads it)."""
    out = {}
    with open(table_path, encoding="utf-8") as f:
        lines = f.read().split("\n")
    cols = None
    for line in lines:
        if not line or line.startswith("#"):
            continue
        cells = line.split("\t")
        if cols is None:
            cols = cells
            continue
        r = dict(zip(cols, cells))
        try:
            season = int(r["season"])
        except (KeyError, ValueError):
            continue
        for f in r.get("comp_file", "").split(";"):
            out.setdefault((season, f), r.get("name", ""))
    return out


def check_run(name, program, game, keep, reuse=False):
    year, database, rules = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-season-change-{name}-")
    report = os.path.join(work, "report.tsv")
    if reuse and os.path.exists(report):
        code = 0
    else:
        code, report = rrc.run(program, game, year + 1, year, database, ["England"], [], work, 7200,
                               {"changing_rules": rules},
                               {"RECOMP_SEASON_CHANGE_LOG": "1", "RECOMP_SEASON_NAMES": "table"})
    fails = []
    if code != 0:
        return [f"{name}: the run ended with code {code} ({work})"]
    seasons, comps = rrc.read_report(report)
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read().split("\n")
    news = [(int(m.group(1)), m.group(2)) for m in (NEWS.search(x) for x in log) if m]
    sent = [int(m.group(1)) for m in (SENT.search(x) for x in log) if m]
    end = {c["slot"]: c for c in comps if c["season"] == year and c["phase"] == "end"}
    start = {c["slot"]: c for c in comps if c["season"] == year + 1 and c["phase"] == "end"}
    table = table_names(os.path.join(os.path.dirname(program), "seasons", "rules.tsv"))
    expected = []
    for slot, c in sorted(end.items()):
        n = start.get(slot)
        if not n:
            continue
        before, after = table.get((year, c["file"])), table.get((year + 1, c["file"]))
        changes = rules == "on" and before and after and before != after and c["name"] != after[:50]
        want = after[:50] if changes else c["name"]
        if n["name"] != want:
            fails.append(f"{name}: {c['file']} ends {year + 1} as {n['name']!r}, expected {want!r}")
        if changes:
            expected.append(f"{c['name']} is now called {after[:50]}.")
    got = [t for s, t in news if s == year + 1]
    if collections.Counter(got) != collections.Counter(expected):
        for t in sorted(set(expected) - set(got)):
            fails.append(f"{name}: the news lacks: {t}")
        for t in sorted(set(got) - set(expected)):
            fails.append(f"{name}: the news has an extra line: {t}")
    if [s for s, _ in news if s != year + 1]:
        fails.append(f"{name}: news for another season: {[s for s, _ in news if s != year + 1]}")
    want_sent = [year + 1] if expected else []
    if sent != want_sent:
        fails.append(f"{name}: news items sent {sent}, expected {want_sent}")
    if rules == "off" and any("season change" in x for x in log):
        fails.append(f"{name}: changing rules off, but the log shows a season change")
    print(f"{name}: {len(expected)} changes of name, news items {sent}, {len(fails)} problems ({work})")
    for t in got:
        print(f"    news {year + 1}: {t}")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    jobs = int(rrc.arg(argv, "--jobs", 3))
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    fails = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        for f in [pool.submit(check_load, program, game, keep) if n == "load" else
                  pool.submit(check_run, n, program, game, keep, "--reuse" in argv) for n in only]:
            fails += f.result()
    for t in fails:
        print("FAIL  " + t)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
