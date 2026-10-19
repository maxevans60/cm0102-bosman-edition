#!/usr/bin/env python3
"""48-team World Cup check (0.6.0 slice 29): the 2026 finals, their qualifying and the play-off tournament in a
season jump.

Usage:
    python3 tests/seasons/season_wc48_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--jobs n]
        [--keep <folder>] [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py (England, the quick result engine) with the
test logs RECOMP_SEASON_INTL_LOG=1 and RECOMP_SEASON_CHANGE_LOG=1. The runs:
    2023    October 2023, 2023/24 to 2026/27, changing rules on: the full 2026 qualifying, the play-off
            tournament, the finals of 48, the host news, then the 2030 cycle
    2025    December 2025, 2025/26 to 2027/28, changing rules on: the finals of 48 from a start after the
            qualifying began (the draw fill, Q388)
    off     October 2023, 2023/24 to 2026/27, changing rules off: the rule edition of the start season is the
            2026 World Cup, so the finals have 48 teams too

The checks (each run):
    - the jump ends with code 0 and the report holds the end of the last season
    - the log lines of LINES (the draw of 48 in 12 groups, the round of 32, the host slot of the United States,
      the seeding day)
    - QUALIFIERS: the qualifiers that reach add_qualified_team for the 2026 World Cup, by confederation (the three
      hosts come from the host row): UEFA 16, CONMEBOL 6, CONCACAF 3, CAF 9, AFC 8, OFC 1 at least, 2 more from
      the play-off tournament, 45 in all
    - no draw after a full qualifying is short (log "no fill"), no play-off without a team
    - on: the host news of the 2026 World Cup names three hosts
A run takes about 25 to 35 minutes. --jobs (default 2) runs that many at the same time. --reuse checks the
logs already in the --keep folder again without a new run.
"""
import concurrent.futures
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402
import season_intl_run as sir  # noqa: E402

RUNS = {
    "2023": (2023, 2026, "October 2023", "on"),
    "2025": (2025, 2027, "December 2025", "on"),
    "off": (2023, 2026, "October 2023", "off"),
}
LINES = {
    "2023": ["world_cup 2026 draw of 48 teams in 12 groups", "world_cup 2026 round of 32",
             "world_cup 2026 seeds on 1 April", "wc play-off 2026: seeds", "wc play-off 2026: finals",
             "wc qualifying 2026: path A", "wc qualifying 2026: Europe draws", "world_cup 2026 host"],
    "2025": ["world_cup 2026 draw of 48 teams in 12 groups", "world_cup 2026 round of 32"],
    "off": ["world_cup 2026 draw of 48 teams in 12 groups", "world_cup 2026 round of 32"],
}
QUALIFIERS = {"2023": 2026, "off": 2026}
BASE = {"UEFA": 16, "CONMEBOL": 6, "CONCACAF": 3, "CAF": 9, "AFC": 8, "OFC": 1}
NEWS = {"2023": "The 2026 World Cup plays in"}
BASE_ENV = {"RECOMP_STAFF_RESERVE": "23000", "RECOMP_SEASON_INTL_LOG": "1", "RECOMP_SEASON_CHANGE_LOG": "1"}
QUALIFIER = re.compile(r"season intl: world_cup (\d+) qualifier (.*) \((\w+|\?)\)$")


def check(name, start, last, rules, work):
    report = os.path.join(work, "report.tsv")
    if not os.path.exists(report):
        return [f"{name}: no report ({work})"]
    seasons, _ = rrc.read_report(report)
    fails = []
    if not seasons or seasons[-1]["season"] != last or seasons[-1]["phase"] != "end":
        fails.append(f"{name}: the report does not end with the end of {last}/{(last + 1) % 100:02d}")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read().split("\n")
    for text in LINES.get(name, []):
        if not any(text in x for x in log):
            fails.append(f"{name}: no log line {text!r}")
    for x in log:
        if "no fill (full qualifying)" in x and "world_cup" in x:
            fails.append(f"{name}: a short list after a full qualifying: {x.strip()}")
        if "season intl: wc play-off" in x and " none" in x:
            fails.append(f"{name}: a play-off without a team: {x.strip()}")
        if "no room for the knockout" in x or "no knockout stage" in x or "no paths" in x:
            fails.append(f"{name}: {x.strip()}")
    year = QUALIFIERS.get(name)
    if year:
        count = {}
        teams = []
        for x in log:
            m = QUALIFIER.search(x)
            if m and int(m.group(1)) == year:
                count[m.group(3)] = count.get(m.group(3), 0) + 1
                teams.append(m.group(2))
        extra = 0
        for conf, want in BASE.items():
            got = count.get(conf, 0)
            if got < want or (conf == "UEFA" and got != want):
                fails.append(f"{name}: {conf} gives {got} qualifiers for {year}, the table {want}")
            extra += max(0, got - want)
        if sum(count.values()) != 45 or extra != 2:
            fails.append(f"{name}: {sum(count.values())} qualifiers for {year} ({count}), 45 with 2 from the "
                         f"play-off tournament")
        if len(set(teams)) != len(teams):
            fails.append(f"{name}: a nation qualifies twice for {year}")
    want = NEWS.get(name)
    if want:
        news = [x for x in log if "season change: news" in x and want in x]
        if not news:
            fails.append(f"{name}: no host news {want!r}")
        elif "3 host nations" not in news[0]:
            fails.append(f"{name}: the host news does not name three hosts: {news[0].strip()}")
    return fails


def one(program, game, name, keep, reuse):
    start, last, database, rules = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-season-wc48-{name}-")
    if not reuse:
        code, _ = rrc.run(program, game, last, start, sir.database_folder(database), ["England"], [], work, 10800,
                          {"changing_rules": rules}, BASE_ENV)
        if code != 0:
            return [f"{name}: the run ended with code {code} ({work})"]
    return check(name, start, last, rules, work)


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = argv[argv.index("--only") + 1].split(",") if "--only" in argv else list(RUNS)
    jobs = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 2
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
