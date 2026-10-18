#!/usr/bin/env python3
"""Nations League check (0.6.0 slice 28): the UEFA Nations League in a season jump.

Usage:
    python3 tests/seasons/season_nl_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py (England, the quick result engine) with the
test log RECOMP_SEASON_NL_LOG=1. The runs:
    2018    October 2018, 2018/19 to 2019/20, changing rules on: the 2018-19 edition from the real leagues,
            the finals in June 2019, the 2020-21 leagues from the results (draw in March 2020)
    2020    November 2020, 2020/21 to 2021/22, changing rules on: the 2020-21 edition, finals in June 2021 (Q277),
            the 2022 World Cup qualifying on its 2021 matchdays and its three paths in March 2022
    2022    October 2022, 2022/23 to 2023/24, changing rules on: the 2024-25 edition (draw February 2024)
    2024    October 2024, 2024/25, changing rules on: the 2024-25 edition and its finals
    off     October 2018, 2018/19 to 2019/20, changing rules off: the 2018-19 shape again for 2020
    save    October 2018, changing rules on: the jump saves after 430 days (September 2019, after the 2018-19
            finals and before the 2020-21 draw) and stops; a second game loads that save in a new user folder and
            jumps to the end of 2019/20. The leagues of its 2020-21 draw must be those of run 2018 (the same
            seed and clicks give the same 2018-19 results), so promotion and relegation hold across a save and a
            load. Run 2018 first (or with it, in --keep).

The checks (each run):
    - the jump ends with code 0 and the report holds the end of the last season
    - the object is made for the first edition of the table from the start season (log "season nl: <edition>")
    - the first draw puts each nation of the real leagues (docs/research/rules-0.6.0/nations-league-tiers.tsv)
      that the log names in its real league and group, and no league holds more nations than the edition
    - each finals line names four nations, all of League A of that edition's draw
    - a nation that moves between editions moves one league, and the next draw puts it there
A run takes about 6 to 10 minutes. --jobs (default 1) runs that many at the same time. --reuse checks the
logs already in the --keep folder again without a new run.
"""
import concurrent.futures
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import make_nl  # noqa: E402
import rules_report_check as rrc  # noqa: E402
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
import custom_database_run as cdr  # noqa: E402
import fixtures  # noqa: E402
import smoke_run  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
# name: (start year, database, changing rules, last season, first edition, editions drawn in the run)
RUNS = {
    "2018": (2018, "October 2018", "on", 2019, "2018-19", ["2018-19", "2020-21"]),
    "2020": (2020, "November 2020", "on", 2021, "2020-21", ["2020-21", "2022-23"]),
    "2022": (2022, "October 2022", "on", 2023, "2024-25", ["2024-25"]),
    "2024": (2024, "October 2024", "on", 2024, "2024-25", ["2024-25"]),
    "off": (2018, "October 2018", "off", 2019, "2018-19", ["2018-19", "2018-19"]),
    "save": (2018, "October 2018", "on", 2019, "2018-19", ["2018-19", "2020-21"]),
}
SAVE_AFTER = 430
# Log lines each run must hold: the play-off paths (season_nl_euro.c, season_nl_wc.c).
LINES = {"2018": ["euro 2020 qualifying groups from 21 March 2019", "euro 2020 path A (", "euro 2020 path D (",
                  "euro 2020 seeding on day"],
         "2020": ["world cup 2022 qualifying groups from 24 March 2021", "world cup 2022 path A:",
                  "world cup 2022 path C:"],
         "2022": ["euro 2024 qualifying groups from 23 March 2023", "euro 2024 path C ("]}
NO_LINES = ["no path play-off", ": no paths ("]
BASE_ENV = {"RECOMP_STAFF_RESERVE": "23000", "RECOMP_SEASON_NL_LOG": "1"}
DRAW = re.compile(r"recomp: season nl: (\S+) draw league ([A-D]) group (\d+):(.*)$")
FINALS = re.compile(r"recomp: season nl: (\S+) finals:(.*)$")
MADE = re.compile(r"recomp: season nl: (\S+) \((\d+) groups\), draw day")
MOVE = re.compile(r"recomp: season nl: (.+) goes from league ([A-D]) to ([A-D])$")


def database_folder(name):
    hits = sorted(d for d in os.listdir(DB) if d == name or d.startswith(name + " "))
    if not hits:
        raise SystemExit(f"season_nl_run.py: no database folder {name!r} in {DB}")
    return os.path.join(DB, hits[0])


def real_leagues(edition):
    """nation name (each "/" name of the table) -> (league, group) of the real draw."""
    out = {}
    for t in make_nl.tiers({("UEFA", edition)}):
        for name in t["nation"].split("/"):
            out[name.encode("latin-1", "replace").decode("latin-1")] = ("ABCD"[t["league"]], t["group"])
    return out


def draws(log):
    """The draws in log order: [(edition, {league: [[nations of group 1], ...]})]."""
    out = []
    for line in log:
        m = DRAW.search(line)
        if not m:
            continue
        ed, league, group, names = m.group(1), m.group(2), int(m.group(3)), m.group(4)
        nations = [n.strip() for n in names.split(";") if n.strip()]
        if not out or (out[-1][0] != ed or (league == "A" and group == 1 and out[-1][1])):
            out.append((ed, {}))
        out[-1][1].setdefault(league, []).append(nations)
    return out


def check(name, work):
    start, _, rules, last, first, editions = RUNS[name]
    fails = []
    report = os.path.join(work, "report.tsv")
    if os.path.exists(report):
        seasons, _ = rrc.read_report(report)
        if not seasons or seasons[-1]["season"] != last or seasons[-1]["phase"] != "end":
            fails.append(f"{name}: the report does not end with the end of {last}/{(last + 1) % 100:02d}")
    else:
        fails.append(f"{name}: no report ({work})")
    path = os.path.join(work, "stderr.txt")
    if not os.path.exists(path):
        return fails + [f"{name}: no log"]
    log = open(path, encoding="latin-1").read().split("\n")
    made = [m.groups() for m in (MADE.search(x) for x in log) if m]
    if not made or made[0][0] != first:
        fails.append(f"{name}: the object is not made for {first} ({made[:1]})")
    eds = {(e["confed"], e["edition"]): e for e in make_nl.editions()}
    seen = draws(log)
    if [d[0] for d in seen] != editions:
        fails.append(f"{name}: the draws are {[d[0] for d in seen]}, expected {editions}")
    for k, (ed, leagues) in enumerate(seen):
        e = eds.get(("UEFA", ed))
        for li, league in enumerate("ABCD"):
            groups = leagues.get(league, [])
            count = sum(len(g) for g in groups)
            if e and count > e["sizes"][li] + (1 if k else 0):
                fails.append(f"{name}: {ed} league {league} holds {count}, the edition {e['sizes'][li]}")
            if e and len(groups) != e["groups"][li]:
                fails.append(f"{name}: {ed} league {league} has {len(groups)} groups, the edition {e['groups'][li]}")
        if k == 0 and rules == "on" or (k == 0 and name == "off"):
            real = real_leagues(ed)
            for league, groups in leagues.items():
                for gi, g in enumerate(groups):
                    for nation in g:
                        want = real.get(nation)
                        if want and want[0] != league:
                            fails.append(f"{name}: {ed}: {nation} in league {league}, the real league is {want[0]}")
                        elif want and want[1] and want[1] != gi + 1:
                            fails.append(f"{name}: {ed}: {nation} in group {league}{gi + 1}, the real group is "
                                         f"{want[0]}{want[1]}")
    for text in LINES.get(name, []):
        if not any(text in x for x in log):
            fails.append(f"{name}: no log line {text!r}")
    for text in NO_LINES:
        for x in log:
            if text in x:
                fails.append(f"{name}: {x.strip()}")
    for x in log:
        if "path" in x and "season nl:" in x and " none;" in x:
            fails.append(f"{name}: a path with a missing nation: {x.strip()}")
    league_a = {ed: {n for g in leagues.get("A", []) for n in g} for ed, leagues in seen}
    finals = [m.groups() for m in (FINALS.search(x) for x in log) if m]
    for ed, names in finals:
        nations = [n.strip() for n in names.split(";") if n.strip()]
        if len(nations) != 4 or "none" in nations:
            fails.append(f"{name}: {ed} finals: {nations}")
        elif ed in league_a and not set(nations) <= league_a[ed]:
            fails.append(f"{name}: {ed} finals: {nations} not all of League A")
    if len(seen) >= 2:
        before = {n: lg for lg, gs in seen[0][1].items() for g in gs for n in g}
        after = {n: lg for lg, gs in seen[1][1].items() for g in gs for n in g}
        for m in (MOVE.search(x) for x in log):
            if not m:
                continue
            nation, a, b = m.groups()
            if abs("ABCD".index(a) - "ABCD".index(b)) != 1:
                fails.append(f"{name}: {nation} moves from {a} to {b}")
            if before.get(nation) != a or after.get(nation) != b:
                fails.append(f"{name}: {nation} moves from {a} to {b}, the draws give {before.get(nation)} and "
                             f"{after.get(nation)}")
    return fails


def leagues_of(log, edition):
    """nation -> league of the draw of an edition in a log."""
    for ed, leagues in draws(log):
        if ed == edition:
            return {n: lg for lg, gs in leagues.items() for g in gs for n in g}
    return {}


def load_and_jump(program, game, database, save, work, last, timeout):
    """Loads a save in a new user folder and plays the season jump to the end of `last`, as rules_report_check.run
    does for a new game."""
    user, home, shots = (os.path.join(work, d) for d in ("user", "home", "shots"))
    for d in (user, home, shots):
        os.makedirs(d, exist_ok=True)
    cdr.copy_data(database, os.path.join(user, "Databases", os.path.basename(database)))
    shutil.copyfile(save, os.path.join(user, "nl.sav"))
    with open(os.path.join(user, "settings.txt"), "w") as f:
        f.write(f"display = classic\nchanging_rules = on\nfaster_days = on\ndatabase = folder:{os.path.basename(database)}\n")
    # The save was made on holiday (RECOMP_SAVE_AFTER_DAYS in the jump), so the loaded game goes on by itself and
    # shows no first news to wait for; the jump ends the program at the end of `last`.
    load = smoke_run.without_intro_click(fixtures.LOAD_STEPS)
    steps = [x for x in load if x != "wait first-news"] + ["5400000 quit"]
    table = os.path.join(os.path.dirname(os.path.abspath(program)), "seasons", "rules.tsv")
    env = dict(os.environ, HOME=home, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
               RECOMP_USER_DIR=user, RECOMP_FIXED_CLOCK="1", RECOMP_SEASON_JUMP=str(last), RECOMP_SCREEN_LOG="1",
               RECOMP_RULES_TABLE=table, RECOMP_SCRIPT="; ".join(smoke_run.scaled_steps("classic", shots, steps)),
               **BASE_ENV)
    start = time.time()
    with open(os.path.join(work, "stderr.txt"), "w") as log:
        try:
            code = subprocess.run([program, "--game", game, "-seed", "1"], env=env, stdout=subprocess.DEVNULL,
                                  stderr=log, timeout=timeout).returncode
        except subprocess.TimeoutExpired:
            code = "timeout"
    print(f"run: load and jump: code {code} after {time.time() - start:.0f} s, log {os.path.join(work, 'stderr.txt')}")
    return code


def save_run(program, game, keep, reuse):
    start, database, _, last, _, _ = RUNS["save"]
    base = keep or tempfile.mkdtemp(prefix="cm0102-season-nl-save-")
    first, second = os.path.join(base, "save-a"), os.path.join(base, "save-b")
    db = database_folder(database)
    if not reuse:
        save = os.path.join(first, "user", smoke_run.SAVE_NAME)
        if not os.path.exists(save):                     # a save of an earlier run in --keep is used again
            code, _ = rrc.run(program, game, last, start, db, ["England"], [], first, 7200, {"changing_rules": "on"},
                              dict(BASE_ENV, RECOMP_SAVE_AFTER_DAYS=str(SAVE_AFTER)))
            if code != 0 or not os.path.exists(save):
                return [f"save: the first game did not save (code {code}, {first})"]
        if os.path.exists(second):
            second = tempfile.mkdtemp(prefix="save-b-", dir=base)
        code = load_and_jump(program, game, db, save, second, last, 7200)
        if code != 0:
            return [f"save: the loaded game ended with code {code} ({second})"]
    fails = []
    log_b = open(os.path.join(second, "stderr.txt"), encoding="latin-1").read().split("\n")
    ref = os.path.join(base, "2018", "stderr.txt")
    if not os.path.exists(ref):
        return [f"save: no log of run 2018 in {base} to compare with"]
    log_ref = open(ref, encoding="latin-1").read().split("\n")
    want, got = leagues_of(log_ref, "2020-21"), leagues_of(log_b, "2020-21")
    if not want or not got:
        return [f"save: no 2020-21 draw (run 2018: {len(want)} nations, loaded game: {len(got)})"]
    for n in sorted(set(want) | set(got)):
        if want.get(n) != got.get(n):
            fails.append(f"save: {n} in league {got.get(n)} after the load, {want.get(n)} in run 2018")
    if not any("season nl: 2018-19 finals:" in x for x in open(os.path.join(first, "stderr.txt"), encoding="latin-1")):
        fails.append("save: the first game made no finals before the save")
    return fails


def one(program, game, name, keep, reuse):
    if name == "save":
        return save_run(program, game, keep, reuse)
    start, database, rules, last, _, _ = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-season-nl-{name}-")
    if not reuse:
        code, _ = rrc.run(program, game, last, start, database_folder(database), ["England"], [], work, 7200,
                          {"changing_rules": rules}, BASE_ENV)
        if code != 0:
            return [f"{name}: the run ended with code {code} ({work})"]
    return check(name, work)


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = argv[argv.index("--only") + 1].split(",") if "--only" in argv else list(RUNS)
    jobs = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 1
    keep = os.path.abspath(argv[argv.index("--keep") + 1]) if "--keep" in argv else None
    reuse = "--reuse" in argv
    fails = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        for r in pool.map(lambda n: one(program, game, n, keep, reuse), only):
            fails += r
    for f in fails:
        print("FAIL  " + f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
