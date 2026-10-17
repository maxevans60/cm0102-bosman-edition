#!/usr/bin/env python3
"""Competition history between 2001 and the start season (0.6.0 slice 11c, Q347).

Usage:
    python3 tests/seasons/season_history_run.py <cm0102_recomp> <game folder> <folder of databases>
        [--only <season or database>,...] [--jobs n] [--extras] [--no-extra-runs]

For each season database before 2010/11 (and the 3.9.68 data on the 2002/03 tile) a new game starts in its
season and plays to the first news screen. The test log RECOMP_HISTORY_LOG (season_history.c) gives, at the
database load and at the first human turn, every history row from 1999 on of the competitions below. At the
first turn the game holds only the rows of the selected nations (England in these runs). The checks:
    plan       for each competition, every label year from 2002 to the start year that the plan of that
               database fills (docs/research/history-0.6.0/plan.tsv) holds exactly the researched first and
               second clubs, and no other row.
    england    the English Premier Division holds the real champion of every season from 2001/02 to the season
               before the start (an independent list, as in season_dates_run.py).
Also:
    off        September 2005 in 2005/06 with RECOMP_SEASON_CODE=off: no row is written (the 2002 to 2004 labels
               of the English Premier Division stay empty).
    again      the September 2009 game twice: the saves are byte for byte the same.
At most --jobs (3) games run at the same time.
"""
import collections
import concurrent.futures
import csv
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import season_dates_run as sdr  # noqa: E402

PLAN = os.path.join(ROOT, "docs", "research", "history-0.6.0", "plan.tsv")
COMPS = ["English Premier Division", "English FA Cup", "European Champions Cup", "Scottish Premier Division",
         "Norwegian Premier Division", "Italian Serie A", "FIFA World Cup"]
EPL = "English Premier Division"


def plan_rows():
    """{database: {competition: {label: set of (first, second)}}} and {database: start year} of the plan."""
    want = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict(set)))
    starts = {}
    with open(PLAN, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            starts[r["database"]] = int(r["start_year"])
            c = want[r["database"]][r["competition"]]
            if r["action"] == "clear":
                c.setdefault(int(r["label"]), set())
            else:
                c[int(r["label"])].add((r["first"], r["second"]))
    return want, starts


def parse(log, when):
    """{competition: {label: list of (first, second)}} from the history log of `when` (load or turn)."""
    out = {}
    for m in re.finditer(r"recomp: history log " + when + r": (.+?):((?: \d+ [^;]*;)*)\n", log):
        rows = collections.defaultdict(list)
        for x in re.finditer(r" (\d+) (.*?) / (.*?);", m.group(2)):
            rows[int(x.group(1))].append((x.group(2), x.group(3)))
        out[m.group(1)] = rows
    return out


def check(label, start, got, want, turn=False):
    """At the load every competition of the plan; at the first turn those that the game kept (it keeps only the
    rows of the selected nations, 0x539ae0)."""
    bad, n = [], 0
    for comp in COMPS:
        if comp not in want or (turn and not got.get(comp)):
            continue
        rows = got.get(comp, {})
        for y in range(2002, start + 1):
            if y not in want[comp]:
                continue
            w = want[comp][y]
            have = set(rows.get(y, []))
            n += 1
            if have != w or len(rows.get(y, [])) != len(w):
                bad.append(f"{comp} {y}: holds {sorted(have) or 'nothing'}, the plan {sorted(w) or 'nothing'}")
    eng = got.get(EPL, {})
    for y in range(2002, start + 1) if EPL in want else ():
        first = {a for a, b in eng.get(y, [])}
        if first != {sdr.CHAMPIONS[y]}:
            bad.append(f"{EPL} {y}: champion {sorted(first) or 'none'}, real {sdr.CHAMPIONS[y]}")
    return n, bad


def run_one(program, game, work, label, year, folder, env=None):
    env = dict(env or {}, RECOMP_HISTORY_LOG=";".join(COMPS))
    save, log, secs = sdr.play(program, game, work, label, year, folder, env)
    return save, log, secs


def main(argv):
    if len(argv) < 4:
        print(__doc__)
        return 2
    program, game, dbs_dir = (os.path.abspath(a) for a in argv[1:4])
    jobs_n = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 3
    only = argv[argv.index("--only") + 1].split(",") if "--only" in argv else None
    want, starts = plan_rows()
    dbs = sdr.databases(dbs_dir)
    runs = []
    for r in sdr.season_rows():
        if r["role"] not in ("season database", "extra version") or r["database"] not in starts:
            continue
        if r["role"] == "extra version" and "--extras" not in argv:
            continue
        year = starts[r["database"]]
        if only and str(year) not in only and r["database"] not in only:
            continue
        if r["database"] not in dbs:
            print(f"skip  {r['database']}: not in {dbs_dir}")
            continue
        runs.append((r["database"], year, r["database"], None))
    if not only or "2002" in only or "3.9.68" in only:
        runs.append(("3.9.68", 2002, None, None))
    if "--no-extra-runs" not in argv:
        if "September 2005" in dbs:
            runs.append(("off", 2005, "September 2005", {"RECOMP_SEASON_CODE": "off"}))
        if "September 2009" in dbs:
            runs.append(("again", 2009, "September 2009", None))
    work = tempfile.mkdtemp(prefix="season-history-")
    failed = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs_n) as ex:
        futs = {}
        for label, year, name, env in runs:
            folder = sdr.neutral_copy(work, name, dbs[name][1]) if name else None
            futs[ex.submit(run_one, program, game, work, label, year, folder, env)] = (label, year, name)
        results = {}
        for f in concurrent.futures.as_completed(futs):
            label, year, name = futs[f]
            save, log, secs = f.result()
            results[label] = save
            got, turn = parse(log, "load"), parse(log, "turn")
            if label == "off":
                got = turn
                eng = got.get(EPL, {})
                filled = [y for y in (2002, 2003, 2004) if eng.get(y)]
                ok = "season history:" not in log and not filled and EPL in got
                print(f"{'ok  ' if ok else 'FAIL'}  off: {'no rows written' if ok else 'rows written ' + str(filled)}"
                      f" ({secs:.0f} s)")
                failed += not ok
                continue
            db = "September 2009" if label == "again" else (name or "3.9.68")
            if not got or not turn:
                print(f"FAIL  {label}: no history log ({secs:.0f} s)\n{log[-1500:]}")
                failed += 1
                continue
            n, bad = check(label, year, got, want[db])
            n2, bad2 = check(label, year, turn, want[db], turn=True)
            bad += ["at the first turn: " + b for b in bad2]
            line = re.search(r"recomp: season history: .*", log)
            print(f"{'ok  ' if not bad else 'FAIL'}  {label} ({year}/{(year + 1) % 100:02d}): {n} label years "
                  f"checked at the load, {n2} at the first turn; {line.group(0)[24:] if line else 'no fill line'} ({secs:.0f} s)")
            for b in bad:
                print("      " + b)
            failed += bool(bad)
        if "again" in results:
            first = results.get("September 2009")
            if first and results["again"]:
                same = open(first, "rb").read() == open(results["again"], "rb").read()
                print(f"{'ok  ' if same else 'FAIL'}  again: the two September 2009 saves are "
                      f"{'the same' if same else 'different'}")
                failed += not same
            else:
                print("FAIL  again: a save is missing")
                failed += 1
    print(f"{len(runs)} runs, {failed} failed; work folder {work}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
