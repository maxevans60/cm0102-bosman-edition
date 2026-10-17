#!/usr/bin/env python3
"""Rules report check (0.6.0 slice 12, Q265): the competitions that the game holds in each season, compared
with the rules table.

Usage:
    python3 tests/seasons/rules_report_check.py <report>
        compares a report with the rules table and lists each difference
    python3 tests/seasons/rules_report_check.py --run <cm0102_recomp> <game folder> [--last y] [--year y]
        [--database <folder>] [--keep <folder>] [--timeout s]
        plays the season jump, then compares its report

The run: a new game (Arsenal, England selected, as in tests/smoke/smoke_run.py) with changing_rules = off,
then Go on Holiday. RECOMP_SEASON_JUMP=<last> (default 2027) turns the match engine off, so the original's
quick result engine plays every match in the real day loop, and RECOMP_RULES_REPORT writes the competitions
at the start and at each 1 August (src/recomp_rt/win/season_report.c). --year starts the game in that season
(RECOMP_START_SEASON) with the database folder. --keep keeps the report and the log there.

The checks that fail the test (exit code 1):
    - the run ends with code 0 and its report holds every season from the start year to the last year
    - every competition of the report has a known class (its vtable is in the class table of the build)
    - base game (start 2001): the name of each competition record is the name of that record in
      club_comp.dat or nation_comp.dat of the game folder, so the report reads the original's competitions;
      and each league with clubs in club.dat holds as many clubs as club.dat puts in its division
The differences between the report and the rules table do not fail the test: this slice changes no rule.
The script lists them by kind:
    clubs, subs_used, bench     the game's value differs from a count of the table row
    name                        the game's name differs from the table's name of that season
    no_row                      the game holds a competition whose file has no row in that season
    not_held                    the game holds a competition (its year is the season, one year before or one
                                after) that the table marks not held in that season
    idle                        the game's object waits for a later edition (its year is later), but the
                                table holds the competition in that season
    not_in_game                 a row with no competition in the game, of a nation (or body, such as UEFA)
                                that has a numbered tier in the report, or of a file of the report: a new
                                competition, or a file that the game does not run
"""
import collections
import csv
import os
import struct
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import make_rules  # noqa: E402

COLS = ["season", "slot", "record", "file", "class", "kind", "vtable", "year", "clubs", "rounds", "matches",
        "subs_used", "bench", "name"]
RECORD = 0x6b


def read_report(path):
    seasons, comps = [], []
    with open(path, encoding="latin-1") as f:
        lines = f.read().split("\n")
    if not lines or not lines[0].startswith("#bosman-rules-report\t1"):
        raise SystemExit(f"{path}: not a rules report")
    for line in lines[1:]:
        if not line:
            continue
        cells = line.split("\t")
        if cells[0] == "season":
            seasons.append({"season": int(cells[1]), "date": cells[2], "start": int(cells[3]), "rules": cells[4]})
        elif cells[0] == "comp":
            if len(cells) != len(COLS) + 1:
                raise SystemExit(f"{path}: a comp line with {len(cells)} cells")
            c = dict(zip(COLS, cells[1:]))
            for k in ("season", "record", "year", "clubs", "rounds", "subs_used", "bench"):
                c[k] = int(c[k])
            comps.append(c)
    return seasons, comps


def record_names(game):
    """{(file, id): name} of club_comp.dat and nation_comp.dat (0x6b bytes a record: id, long name at +4)."""
    out = {}
    for name in ("club_comp.dat", "nation_comp.dat"):
        path = os.path.join(game, "Data", name)
        if not os.path.exists(path):
            continue
        data = open(path, "rb").read()
        for i in range(len(data) // RECORD):
            r = data[i * RECORD:(i + 1) * RECORD]
            out[(name, struct.unpack_from("<i", r, 0)[0])] = r[4:56].split(b"\0")[0].decode("latin-1")[:51]
    return out


def division_counts(game):
    """{division id: club count} of club.dat (581 bytes a record). Inference from the data: the int at +0x57
    is the club's division (the club_comp id): 20 clubs hold 7 (English Premier Division) and 24 each hold
    8, 9 and 10 in the 3.9.68 data, as the English divisions of 2001/02."""
    path = os.path.join(game, "Data", "club.dat")
    out = collections.Counter()
    if os.path.exists(path):
        data = open(path, "rb").read()
        for i in range(len(data) // 581):
            out[struct.unpack_from("<i", data, i * 581 + 0x57)[0]] += 1
    return out


def differences(seasons, comps, rows):
    """A list of (kind, season, text)."""
    by_season_file = collections.defaultdict(list)
    for r in rows:
        for f in r["comp_file"].split(";"):
            by_season_file[(r["season"], f)].append(r)
    out = []
    files_in_report = {c["file"] for c in comps}
    nations = set()
    held = collections.defaultdict(set)
    for c in comps:
        held[c["season"]].add(c["file"])
        found = by_season_file.get((c["season"], c["file"]), [])
        if not found:
            out.append(("no_row", c["season"], f"{c['file']} ({c['name']}): no table row"))
            continue
        nations |= {r["nation"] for r in found if isinstance(r["tier"], int)}
        # The first row that is a competition (not the qualifying rows of the same file).
        r = found[0]
        label = f"{c['file']} ({c['name']}), row {r['id']}"
        table_held = "not_held" not in (r["clubs"], r["format"])
        # The year of an object is the season (club competitions), the year of the final (the World Cup holds
        # 2002 in 2001/02) or the first year of a qualifying campaign (2000 for the 2002 World Cup). An object
        # with a later year waits for a later edition (the World Cup holds 2006 in 2002/03).
        if not c["season"] - 1 <= c["year"] <= c["season"] + 1:
            if table_held:
                out.append(("idle", c["season"], f"{label}: the game waits for {c['year']}, the table holds it"))
            continue
        if not table_held:
            out.append(("not_held", c["season"], f"{label}: the game holds {c['year']}, the table says not held"))
        for k in ("clubs", "subs_used", "bench"):
            if isinstance(r[k], int) and r[k] != c[k]:
                out.append((k, c["season"], f"{label}: game {c[k]}, table {r[k]} ({r[k + '_text']})"))
        if r["name"] and r["name"] != "n/a" and r["name"] != c["name"]:
            out.append(("name", c["season"], f"{label}: game {c['name']!r}, table {r['name']!r}"))
    for s in sorted({x["season"] for x in seasons}):
        for r in rows:
            if r["season"] != s or r["comp_file"] == "na" or "not_held" in (r["clubs"], r["format"]):
                continue
            files = r["comp_file"].split(";")
            if any(f in held[s] for f in files):
                continue
            if r["nation"] in nations or any(f in files_in_report for f in files):
                out.append(("not_in_game", s, f"{r['id']} {r['comp_file']} {r['name']!r} ({r['nation']})"))
    return out


def check(report, game=None, last=None):
    """Prints the checks and the differences. Returns the failure texts."""
    seasons, comps = read_report(report)
    rows = make_rules.read_rows()
    failed = []
    if not seasons:
        return ["the report holds no season"]
    start = seasons[0]["season"]
    got = [s["season"] for s in seasons]
    if last is not None and got != list(range(start, last + 1)):
        failed.append(f"the report holds the seasons {got[0]} to {got[-1]} ({len(got)}), not {start} to {last}")
    unknown = sorted({c["vtable"] for c in comps if c["file"] == "?"})
    if unknown:
        failed.append(f"competitions with an unknown class: {', '.join(unknown)}")
    if game and start == 2001:
        names = record_names(game)
        for c in comps:
            if c["season"] != 2001:
                continue
            want = names.get(("club_comp.dat", c["record"])), names.get(("nation_comp.dat", c["record"]))
            if c["name"] not in want:
                failed.append(f"base game: {c['slot']} {c['file']} record {c['record']} is {c['name']!r}, "
                              f"the data has {want[0]!r} or {want[1]!r}")
        divisions = division_counts(game)
        for c in comps:
            if c["season"] == 2001 and c["kind"] == "league" and divisions.get(c["record"]):
                if c["clubs"] != divisions[c["record"]]:
                    failed.append(f"base game: {c['file']} holds {c['clubs']} clubs, club.dat has "
                                  f"{divisions[c['record']]} in division {c['record']}")
                else:
                    print(f"base game: {c['file']} holds {c['clubs']} clubs, as club.dat")
    per = collections.Counter(c["season"] for c in comps)
    print(f"report: {len(got)} seasons, {got[0]}/{(got[0] + 1) % 100:02d} to {got[-1]}/{(got[-1] + 1) % 100:02d}, "
          f"{min(per.values())} to {max(per.values())} competitions a season")
    for s in seasons:
        print(f"  season {s['season']}/{(s['season'] + 1) % 100:02d} on {s['date']}: {per[s['season']]} competitions")
    diffs = differences(seasons, comps, rows)
    kinds = collections.Counter(k for k, _, _ in diffs)
    print(f"differences with the rules table: {len(diffs)} ({', '.join(f'{k} {n}' for k, n in sorted(kinds.items()))})")
    for k, s, text in sorted(diffs, key=lambda d: (d[1], d[0], d[2])):
        print(f"  {s}/{(s + 1) % 100:02d}  {k:12} {text}")
    return failed


def run(program, game, last, year, database, keep, timeout):
    import smoke_run
    import custom_database_run as cdr
    work = keep or tempfile.mkdtemp(prefix="cm0102-rules-report-")
    os.makedirs(work, exist_ok=True)
    user, home, shots = (os.path.join(work, d) for d in ("user", "home", "shots"))
    for d in (user, home, shots):
        os.makedirs(d)
    options = {"display": "classic", "changing_rules": "off", "faster_days": "on"}
    env_extra = {}
    if year:
        env_extra["RECOMP_START_SEASON"] = str(year)
    if database:
        cdr.copy_data(database, os.path.join(user, "Databases", os.path.basename(database)))
        options["database"] = "folder:" + os.path.basename(database)
    with open(os.path.join(user, "settings.txt"), "w") as f:
        for k, v in options.items():
            f.write(f"{k} = {v}\n")
    steps = smoke_run.without_intro_click([s for s in smoke_run.STEPS if "dump" not in s and "quit" not in s])
    report = os.path.join(work, "report.tsv")
    table = os.path.join(os.path.dirname(os.path.abspath(program)), "seasons", "rules.tsv")
    env = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
               RECOMP_USER_DIR=user, RECOMP_FIXED_CLOCK="1", RECOMP_SEASON_JUMP=str(last),
               RECOMP_RULES_REPORT=report, RECOMP_RULES_TABLE=table,
               RECOMP_SCRIPT="; ".join(smoke_run.scaled_steps("classic", shots, steps)), **env_extra)
    start = time.time()
    with open(os.path.join(work, "stderr.txt"), "w") as log:
        try:
            r = subprocess.run([program, "--game", game, "-seed", "1"], env=env, stdout=subprocess.DEVNULL,
                               stderr=log, timeout=timeout)
            code = r.returncode
        except subprocess.TimeoutExpired:
            code = "timeout"
    print(f"run: code {code} after {time.time() - start:.0f} s, log {os.path.join(work, 'stderr.txt')}")
    return code, report


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    if argv[1] != "--run":
        failed = check(argv[1])
    else:
        program, game = os.path.abspath(argv[2]), os.path.abspath(argv[3])
        last = int(argv[argv.index("--last") + 1]) if "--last" in argv else 2027
        year = int(argv[argv.index("--year") + 1]) if "--year" in argv else None
        database = os.path.abspath(argv[argv.index("--database") + 1]) if "--database" in argv else None
        keep = os.path.abspath(argv[argv.index("--keep") + 1]) if "--keep" in argv else None
        timeout = int(argv[argv.index("--timeout") + 1]) if "--timeout" in argv else 7200
        code, report = run(program, game, last, year, database, keep, timeout)
        failed = [] if code == 0 else [f"the run ended with code {code}"]
        if os.path.exists(report):
            failed += check(report, game if not database else None, last)
        else:
            failed.append("the run wrote no report")
    for text in failed:
        print("FAIL  " + text)
    print("PASS" if not failed else f"FAILED: {len(failed)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
