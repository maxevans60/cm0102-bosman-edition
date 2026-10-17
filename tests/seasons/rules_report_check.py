#!/usr/bin/env python3
"""Rules report check (0.6.0 slice 12, Q265): the competitions that the game holds in each season, compared
with the rules table.

Usage:
    python3 tests/seasons/rules_report_check.py <report> [--expected <name>] [--update-expected]
        [--data <data folder>] [--start-only]
        compares a report with the rules table and lists each difference (--data: the club checks against the
        database of the run)
    python3 tests/seasons/rules_report_check.py --run <cm0102_recomp> <game folder> [--year y] [--last y]
        [--database <folder>] [--nations england|all|all,-<name>,...|<name,name,...>]
        [--extra conference,regional,c2]
        [--keep <folder>] [--timeout s] [--start-only] [--update-expected]
        plays the season jump, then compares its report

Runs:
    per slice   --year X --last X+1: two seasons from the start in X (about 6 minutes with England alone)
    release     the base game from 2001 with --last 2027: 27 seasons (about 65 minutes with England alone)
    start only  --year X --start-only: the report of the first day of a season game, then the program ends
                (RECOMP_RULES_REPORT_STOP=start): the club moves and names of that start (slice 15). Its
                expected file has the name <year>-<nations>-start

The run: a new game (Arsenal, as in tests/smoke/smoke_run.py) with changing_rules = off, then Go on Holiday.
RECOMP_SEASON_JUMP=<last> turns the match engine off, so the original's quick result engine plays every match
in the real day loop. RECOMP_RULES_REPORT writes the competitions at the start of each season (the first day,
then 1 August) and at its end (1 June of the next year) (src/recomp_rt/win/season_report.c). The run ends after
the end report of the last season. --year starts the game in that season (RECOMP_START_SEASON) with the
database folder. --nations picks the leagues on the Select League(s) screen: england (the default
selection), all (Select All), all,-france (Select All, then France out), or names of the first 16 rows
(Argentina to Norway). --extra adds the lower
divisions of the screen: the English Conference, the German regional divisions and Serie C2. --keep keeps the
report and the log there.

The checks that fail the test (exit code 1):
    - the run ends with code 0 and its report holds the start and end of every season from the start year to
      the last year
    - every competition of the report has a known class (its vtable is in the class table of the build)
    - base game (start 2001): the name of each competition record is the name of that record in
      club_comp.dat or nation_comp.dat of the game folder, and each English division holds as many clubs as
      club.dat puts in it
    - a difference with the rules table that the expected file does not list. The expected files are
      tests/seasons/rules_report_expected/<start year>-<nations>.tsv: one line per difference (season, kind,
      text). A slice that brings a rule removes its lines with --update-expected; a difference that has gone
      is listed, and does not fail the check.

The differences, by kind:
    clubs, subs_used, bench     the game's value at the start and at the end differ from the count of the row
    up, down                    a division's promotion places (+0xbe) or relegation places (+0xc1) differ from
                                the row (a row with a numbered tier); the row's count can also hold the
                                play-off winner (+0xbf) or the play-out losers (+0xc0)
    playoffs                    a division has play-off places and the row has none, or the other way round
    name                        the game's name differs from the table's name of that season and from its shown
                                form (tools/seasons/make_names.py shown_name)
    no_row                      the game holds a competition whose file has no row in that season
    not_held                    the game holds a competition (its year is the season, one year before or one
                                after) that the table marks not held in that season
    idle                        the game's object waits for a later edition (its year is later), but the
                                table holds the competition in that season
    club_list                   (a season game, its start season) a division with a club list of the club moves
                                table (tools/seasons/make_clubs.py) whose listed clubs that the database holds
                                are not all in it, or that holds other clubs too: the division keeps its count
                                (slice 18 brings the real count), or the list lacks clubs of the database
    not_in_game                 a row with no competition in the game, of a nation (or body, such as UEFA)
                                that has a numbered tier in the report, or of a file of the report

The row of a competition: the rows of its file in that season; of those, a row with this file alone before a
row of two files; a row that is not a qualifying path ("qualifying" in comp); then the k-th row for the k-th
object of the same file (the Regionalliga groups).
"""
import collections
import os
import struct
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
EXPECTED_DIR = os.path.join(HERE, "rules_report_expected")
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import make_rules  # noqa: E402
import make_names  # noqa: E402
import make_clubs  # noqa: E402

COLS = ["season", "phase", "slot", "record", "file", "class", "kind", "vtable", "year", "clubs", "rounds",
        "subs_used", "bench", "matches", "up", "playoff_up", "playoff_down", "down", "flags", "above", "below",
        "name"]
INTS = ["season", "record", "year", "clubs", "rounds", "subs_used", "bench"]
LEAGUE_INTS = ["matches", "up", "playoff_up", "playoff_down", "down", "above", "below"]
RECORD = 0x6b
NATION_ROWS = ["Argentina", "Australia", "Belgium", "Brazil", "Croatia", "Denmark", "England", "Finland", "France",
               "Germany", "Greece", "Holland", "Italy", "Japan", "Northern Ireland", "Norway"]
EXTRA = {"conference": 320, "regional": 385, "c2": 451}     # y of the row; the extra division is at x 675


def read_report(path, clubs=None):
    """(season lines, comp lines); club lines go into clubs when a list is given."""
    seasons, comps = [], []
    with open(path, encoding="latin-1") as f:
        lines = f.read().split("\n")
    if not lines or not lines[0].startswith("#bosman-rules-report\t2"):
        raise SystemExit(f"{path}: not a rules report of format 2")
    for line in lines[1:]:
        if not line:
            continue
        cells = line.split("\t")
        if cells[0] == "season":
            seasons.append({"season": int(cells[1]), "phase": cells[2], "date": cells[3], "start": int(cells[4]),
                            "rules": cells[5]})
        elif cells[0] == "club":
            if clubs is not None and len(cells) == 6:
                clubs.append({"season": int(cells[1]), "phase": cells[2], "division": int(cells[3]),
                              "club": int(cells[4]), "name": cells[5]})
        elif cells[0] == "comp":
            if len(cells) != len(COLS) + 1:
                raise SystemExit(f"{path}: a comp line with {len(cells)} cells")
            c = dict(zip(COLS, cells[1:]))
            for k in INTS:
                c[k] = int(c[k])
            for k in LEAGUE_INTS:
                c[k] = int(c[k]) if c[k] != "-" else None
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


def pick_row(found, file, k):
    """The row of the k-th object of a file in a season (module docstring)."""
    alone = [r for r in found if r["comp_file"] == file] or found
    main = [r for r in alone if "qualifying" not in r["comp"].lower()] or alone
    return main[min(k, len(main) - 1)]


def differences(seasons, comps, rows):
    """A sorted list of (season, kind, text)."""
    by_season_file = collections.defaultdict(list)
    for r in rows:
        for f in r["comp_file"].split(";"):
            by_season_file[(r["season"], f)].append(r)
    # One entry per object and season: the start and end lines together.
    objects = collections.OrderedDict()
    for c in comps:
        objects.setdefault((c["season"], c["slot"]), {})[c["phase"]] = c
    out = set()
    files_in_report = {c["file"] for c in comps}
    nations = set()
    held = collections.defaultdict(set)
    nth = collections.Counter()
    for (season, _), phases in objects.items():
        c = phases.get("start") or phases["end"]
        both = [x for x in (phases.get("start"), phases.get("end")) if x]
        held[season].add(c["file"])
        found = by_season_file.get((season, c["file"]), [])
        k = nth[(season, c["file"])]
        nth[(season, c["file"])] += 1
        if not found:
            out.add((season, "no_row", f"{c['file']} ({c['name']}): no table row"))
            continue
        r = pick_row(found, c["file"], k)
        nations |= {x["nation"] for x in found if isinstance(x["tier"], int)}
        label = f"{c['file']} ({c['name']}), row {r['id']}"
        table_held = "not_held" not in (r["clubs"], r["format"])
        # The year of an object is the season (club competitions), the year of the final (the World Cup holds
        # 2002 in 2001/02) or the first year of a qualifying campaign (2000 for the 2002 World Cup). An object
        # with a later year waits for a later edition (the World Cup holds 2006 in 2002/03).
        if not any(season - 1 <= x["year"] <= season + 1 for x in both):
            if table_held:
                out.add((season, "idle", f"{label}: the game waits for {c['year']}, the table holds it"))
            continue
        if not table_held:
            out.add((season, "not_held", f"{label}: the game holds {c['year']}, the table says not held"))
        for key in ("clubs", "subs_used", "bench"):
            values = sorted({x[key] for x in both})
            if isinstance(r[key], int) and r[key] not in values:
                game = "/".join(str(v) for v in values)
                out.add((season, key, f"{label}: game {game}, table {r[key]} ({r[key + '_text'][:80]})"))
        if c["kind"] == "league" and isinstance(r["tier"], int):     # a division, not a group tournament
            for key, extra in (("up", "playoff_up"), ("down", "playoff_down")):
                want = r[key]
                if want == "na" and c[key] == 0:
                    continue
                accept = {c[key], c[key] + (1 if c[extra] else 0), c[key] + c[extra]}
                if not isinstance(want, int) or want not in accept:
                    out.add((season, key, f"{label}: game {c[key]} (+{c[extra]} play-off places), "
                                          f"table {want} ({r[key + '_text'][:80]})"))
            table_po = r["playoffs"] not in ("na", "none", "unknown")
            game_po = bool(c["playoff_up"] or c["playoff_down"])
            if r["playoffs"] != "unknown" and table_po != game_po:
                out.add((season, "playoffs", f"{label}: game {c['playoff_up']} up and {c['playoff_down']} down "
                                             f"play-off places, table {r['playoffs_text'][:80]!r}"))
        shown = make_names.shown_name(r["name"])
        if shown and c["name"] not in (r["name"], shown):
            out.add((season, "name", f"{label}: game {c['name']!r}, table {r['name']!r}"))
    for s in sorted({x["season"] for x in seasons}):
        for r in rows:
            if r["season"] != s or r["comp_file"] == "na" or "not_held" in (r["clubs"], r["format"]):
                continue
            files = r["comp_file"].split(";")
            if any(f in held[s] for f in files):
                continue
            if r["nation"] in nations or any(f in files_in_report for f in files):
                out.add((s, "not_in_game", f"{r['id']} {r['comp_file']} {r['name']!r} ({r['nation']})"))
    return sorted(out)


def club_records(folder):
    """{id: long name} of club.dat in a data folder (581 bytes a record: id +0, long name +4)."""
    path = os.path.join(folder, "club.dat")
    out = {}
    if os.path.exists(path):
        data = open(path, "rb").read()
        for i in range(len(data) // 581):
            r = data[i * 581:(i + 1) * 581]
            out[struct.unpack_from("<i", r, 0)[0]] = r[4:56].split(b"\0")[0].decode("latin-1")
    return out


def division_records(folder):
    """{long name: id} of club_comp.dat in a data folder."""
    path = os.path.join(folder, "club_comp.dat")
    out = {}
    if os.path.exists(path):
        data = open(path, "rb").read()
        for i in range(len(data) // RECORD):
            r = data[i * RECORD:(i + 1) * RECORD]
            out.setdefault(r[4:56].split(b"\0")[0].decode("latin-1"), struct.unpack_from("<i", r, 0)[0])
    return out


def club_differences(seasons, comps, clubs, data_dir):
    """The club moves of the start season (slice 15): for each division with a club list of the table
    (tools/seasons/make_clubs.py) whose league is in the report, the listed clubs that the database holds
    must be the clubs of that division at the start. One difference line per division that differs."""
    start = seasons[0]
    if start["start"] == 2001 and start["rules"] == "off":
        return set()                                   # the base game moves no club
    season = start["season"]
    records = division_records(data_dir)
    names = club_records(data_dir)
    in_report = {c["record"] for c in comps if c["season"] == season and c["kind"] == "league"}
    # By name: the trim of the club table (database_trim_club_table 0x5267c0) gives the clubs new ids, so the
    # ids of the report are not those of club.dat. The club table holds each name once per nation; a name in
    # two nations counts once here, which the check accepts.
    game = collections.defaultdict(set)
    for c in clubs:
        if c["season"] == season and c["phase"] == "start":
            game[c["division"]].add(c["name"])
    held = set(names.values())
    lists = collections.OrderedDict()
    for s, division, club_id, club, _ in make_clubs.build()[0]:
        if s == season:
            lists.setdefault(division, []).append(names.get(club_id) if names.get(club_id) == club else club)
    out = set()
    for division, listed in lists.items():
        rec = records.get(division)
        if rec is None or rec not in in_report:
            continue
        found = [n for n in listed if n in held]
        have = game[rec]
        kept = [n for n in found if n in have]
        others = len(have - set(found))
        if len(kept) != len(found) or others:
            out.add((season, "club_list", f"{division}: {len(kept)} of {len(found)} listed clubs that the database "
                                          f"holds ({len(listed)} listed), {others} other clubs, {len(have)} in all"))
    return out


def expected_path(name):
    return os.path.join(EXPECTED_DIR, name + ".tsv")


def read_expected(name):
    path = expected_path(name)
    if not os.path.exists(path):
        return None
    out = set()
    with open(path, encoding="utf-8") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            s, kind, text = line.rstrip("\n").split("\t", 2)
            out.add((int(s), kind, text))
    return out


def write_expected(name, diffs, seasons):
    os.makedirs(EXPECTED_DIR, exist_ok=True)
    old = read_expected(name) or set()
    keep = {d for d in old if d[0] not in seasons}           # seasons that this report does not hold
    with open(expected_path(name), "w", encoding="utf-8") as f:
        f.write("# Differences between the rules report and the rules table that the check accepts. Made by\n"
                "# tests/seasons/rules_report_check.py --update-expected. Columns: season, kind, text.\n")
        for s, kind, text in sorted(keep | set(diffs)):
            f.write(f"{s}\t{kind}\t{text}\n")
    print(f"expected differences written: {expected_path(name)}")


def check(report, game=None, last=None, expected=None, update=False, data_dir=None, start_only=False):
    """Prints the checks and the differences. Returns the failure texts."""
    clubs = []
    seasons, comps = read_report(report, clubs)
    rows = make_rules.read_rows()
    failed = []
    if not seasons:
        return ["the report holds no season"]
    start = seasons[0]["season"]
    got = [(s["season"], s["phase"]) for s in seasons]
    if last is not None:
        want = [(start, "start")] if start_only else [(y, p) for y in range(start, last + 1) for p in ("start", "end")]
        if got != want:
            failed.append(f"the report holds {got[:2]}...{got[-2:]} ({len(got)} parts), not the start and end of "
                          f"{start} to {last}")
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
            # Only the English divisions: the inference about +0x57 comes from them. Other nations hold
            # several objects of one class (stages) and groups, so a division count does not fit them.
            if (c["season"] == 2001 and c["phase"] == "start" and c["kind"] == "league" and c["file"].startswith("eng_")
                    and divisions.get(c["record"])):
                if c["clubs"] != divisions[c["record"]]:
                    failed.append(f"base game: {c['file']} holds {c['clubs']} clubs, club.dat has "
                                  f"{divisions[c['record']]} in division {c['record']}")
    per = collections.Counter((c["season"], c["phase"]) for c in comps)
    print(f"report: {len(got)} parts, {got[0][0]}/{(got[0][0] + 1) % 100:02d} to {got[-1][0]}/"
          f"{(got[-1][0] + 1) % 100:02d}")
    for s in seasons:
        print(f"  season {s['season']}/{(s['season'] + 1) % 100:02d} {s['phase']:5} on {s['date']}: "
              f"{per[(s['season'], s['phase'])]} competitions")
    diffs = differences(seasons, comps, rows)
    if data_dir and clubs:
        diffs = sorted(set(diffs) | club_differences(seasons, comps, clubs, data_dir))
    kinds = collections.Counter(k for _, k, _ in diffs)
    print(f"differences with the rules table: {len(diffs)} ({', '.join(f'{k} {n}' for k, n in sorted(kinds.items()))})")
    if expected:
        held_seasons = {s["season"] for s in seasons}
        if update:
            write_expected(expected, diffs, held_seasons)
        known = read_expected(expected)
        if known is None:
            failed.append(f"no expected file {expected_path(expected)}: run with --update-expected")
        else:
            new = [d for d in diffs if d not in known]
            gone = sorted(d for d in known if d[0] in held_seasons and d not in set(diffs))
            for s, k, text in new:
                print(f"  NEW   {s}/{(s + 1) % 100:02d}  {k:12} {text}")
            for s, k, text in gone:
                print(f"  GONE  {s}/{(s + 1) % 100:02d}  {k:12} {text}")
            print(f"against {expected_path(expected)}: {len(new)} new, {len(gone)} gone")
            if new:
                failed.append(f"{len(new)} differences that the expected file does not list")
    else:
        for s, k, text in diffs:
            print(f"  {s}/{(s + 1) % 100:02d}  {k:12} {text}")
    return failed


def league_steps(nations, extra):
    """The new game steps of tests/smoke/smoke_run.py with other leagues selected."""
    import smoke_run
    steps = smoke_run.without_intro_click([s for s in smoke_run.STEPS if "dump" not in s and "quit" not in s])
    if nations == ["England"] and not extra:
        return steps
    points = []                                                     # a step's time counts from the last wait
    if nations[0] == "all":
        points.append("592 155")                                    # Select All
        for n in nations[1:]:                                       # then a click takes a nation out again
            points.append(f"390 {188 + 22 * NATION_ROWS.index(n)}")
    else:
        for n in nations:
            if n != "England":
                points.append(f"390 {188 + 22 * NATION_ROWS.index(n)}")
    for e in extra:
        points.append(f"675 {EXTRA[e]}")
    picks = [f"{2000 + 1500 * i} click {p}" for i, p in enumerate(points)]
    many = nations != ["England"]
    out = []
    for s in steps:
        if s == "2500 click 705 572" and not picks[0] in out:      # Select League(s): Next
            out += picks + [f"{2000 + 1500 * len(picks) + 1000} click 705 572"]
            if many:
                out += ["wait screen:0x81e8d0", "2500 click 705 572"]   # Select Start Season: Next (England)
            continue
        if many and s in ("wait typing", "wait first-news"):
            s += " 1800000"                                         # the game is made after the start season
        if many and s == "1000 click 155 163":
            s = "2000 click 155 188"                                # Select Team: Arsenal, below the Nation box
        out.append(s)
    return out


def run(program, game, last, year, database, nations, extra, keep, timeout, start_only=False):
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
    steps = league_steps(nations, extra)
    report = os.path.join(work, "report.tsv")
    table = os.path.join(os.path.dirname(os.path.abspath(program)), "seasons", "rules.tsv")
    env = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
               RECOMP_USER_DIR=user, RECOMP_FIXED_CLOCK="1", RECOMP_SEASON_JUMP=str(last), RECOMP_SCREEN_LOG="1",
               RECOMP_RULES_REPORT=report, RECOMP_RULES_TABLE=table,
               **({"RECOMP_RULES_REPORT_STOP": "start"} if start_only else {}),
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


def arg(argv, name, default=None):
    return argv[argv.index(name) + 1] if name in argv else default


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    update = "--update-expected" in argv
    if argv[1] != "--run":
        failed = check(argv[1], expected=arg(argv, "--expected"), update=update, data_dir=arg(argv, "--data"),
                       start_only="--start-only" in argv)
    else:
        program, game = os.path.abspath(argv[2]), os.path.abspath(argv[3])
        year = int(arg(argv, "--year", 0)) or None
        last = int(arg(argv, "--last", 2027))
        database = os.path.abspath(arg(argv, "--database")) if "--database" in argv else None
        nations = arg(argv, "--nations", "england")
        words = [x.strip().lower().replace("_", " ") for x in nations.split(",")]
        names = [next(n for n in NATION_ROWS if n.lower() == w.lstrip("-")) for w in words if w != "all"]
        nations = (["all"] + names) if words[0] == "all" else names
        if nations[0] != "all" and "England" not in nations:
            nations = ["England"] + nations                      # Arsenal: England stays selected
        extra = [x for x in arg(argv, "--extra", "").split(",") if x]
        keep = os.path.abspath(arg(argv, "--keep")) if "--keep" in argv else None
        timeout = int(arg(argv, "--timeout", 7200))
        start_only = "--start-only" in argv
        if start_only:
            last = year or 2001
        code, report = run(program, game, last, year, database, nations, extra, keep, timeout, start_only)
        failed = [] if code == 0 else [f"the run ended with code {code}"]
        name = f"{year or 2001}-{nations[0].lower()}" + "".join(
            f"{'-no' if nations[0] == 'all' else '-'}{n.lower().replace(' ', '_')}" for n in nations[1:]) + \
            "".join(f"-{e}" for e in extra) + ("-start" if start_only else "")
        if os.path.exists(report):
            failed += check(report, game if not database else None, last, name, update,
                            data_dir=database or os.path.join(game, "Data"), start_only=start_only)
        else:
            failed.append("the run wrote no report")
    for text in failed:
        print("FAIL  " + text)
    print("PASS" if not failed else f"FAILED: {len(failed)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
