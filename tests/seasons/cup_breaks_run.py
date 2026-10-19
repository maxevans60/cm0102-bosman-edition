#!/usr/bin/env python3
"""Cup break check (0.6.0 slice 21b): domestic cups that the rules table does not hold in some seasons.

Usage:
    python3 tests/seasons/cup_breaks_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--keep <folder>]
        [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py with changing rules on or off and the nations
of its cups selected (src/recomp_rt/win/season_kind_cup_breaks.c, docs/research/2026-10-18-cup-breaks.md).

The box fixes on main (euro_finals.c, 7a99f82) let a start in a later season play through the finals whose
qualifying began before the start. The "start" runs check a new game in a later season and end at the first start
report or at their time limit, whichever comes first.

The runs (Select All plays about 10 seconds a game day, so the played runs select few nations):
    r2001   3.9.68 data, changing rules on, 2001/02 to 2008/09, Greece and Northern Ireland: the Greek Super Cup
            plays 2001/02 (a 2001 game keeps the original's first season), skips 2002/03 to 2006/07, comes back
            and plays in 2007/08, and skips again in 2008/09; the Northern Irish Charity Shield skips every season
            from 2002/03
    f2008   September 2008, changing rules on, 2008/09 to 2010/11, Italy and Holland: the knockout stage of the
            Coppa Italia takes the format of 2008 (single matches to the quarter-finals), the KNVB Cup final is two
            legs in 2009/10 and one match again in 2010/11 (rows of tools/seasons/cup_formats.tsv)
    x2008   September 2008, changing rules on, 2008/09, Select All without France (the French first division of
            that database stops the game, a known slice 18 matter): the Copa del Rey plays the round of 32 over two
            legs, the Turkish Cup its quarter-finals and semi-finals, the Russian Cup single matches
    s2020   November 2020, changing rules off, 2020/21 to 2021/22, Belgium, Holland, Italy, Northern Ireland and
            Norway: the cups that the table does not hold in 2020/21 stay off for the whole game (Q232), also after
            the 2021 season change, with no fixture in any club's list
    n2008   September 2008, changing rules on, 2008/09 to 2009/10, Germany and Brazil: the German League Cup and
            the Copa dos Campeoes wait for ever across a season change, with no fixture
    s2008   start run, September 2008, changing rules off, Select All without France (the French first division of
            that database stops the game, a known slice 18 matter): the cups that never come back (Welsh Premier
            Cup, German League Cup, Irish Super Cup, Copa dos Campeoes) wait for ever with no fixtures
    r2019   October 2019, changing rules on, 2019/20 to 2022/23, the five nations of s2020: the Belgian and Dutch
            super cups, the Coppa Italia Serie C, the Northern Irish League Cup (2020/21) and the Norwegian Cup
            (2020) skip one season and come back; at the new game the Supercoppa di Serie C waits for 2020 and the
            Northern Irish Charity Shield for 2022
The checks of each run:
    - a played run ends with code 0 and its report holds the end of its last season; a start run holds the start
      report of its first season (a time limit after that is no fault);
    - the log holds the required events of the run, and every line "waits for", "is not held in" (a skip) and
      "resumes in" agrees with the rules table (season_cups.h rule); every cup of the kind that the report holds
      and whose first edition the table does not hold has its "waits for" line with the next edition;
    - no cup of the kind holds a year that the table does not hold, at any start or end report (a skipped cup keeps
      the year of its last edition, a waiting cup holds the year of its next edition);
    - a cup that never skipped starts the edition of the season of the change (log "season S, year Y": Y + 1 = S);
    - a cup that resumes in season E plays: its start_new_season runs again in season E + 1;
    - no fixture of 2999 or later reaches the fixture store, and none is made inside a constructor that waits
      for ever (log "fixture added" of the add_fixture wrap); the years of the store are logged at the new game
      and each season point ("fixture years");
    - with changing rules on, the news of each season holds one sentence for each cup of the run whose table
      status changes in it (sentences of other kinds, such as the club competitions of slice 26, do not count);
      with changing rules off, none.
--reuse checks the reports in the --keep folder again without a new run.
"""
import collections
import csv
import os
import re
import signal
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
NEVER = 2999
NATIONS = ["Belgium", "Holland", "Italy", "Northern Ireland", "Norway"]
# The classes of the kind (season_kind_cup_breaks.c).
FILES = ["bel_super", "bra_champ_cup", "fra_lge_cup", "ger_lge_cup", "gre_super", "hol_super", "ire_leinster_cup",
         "ire_lge_cup", "ire_munster_cup", "ire_pres_cup", "ire_super_cup", "ita_c1_super", "ita_c_cup",
         "jap_emp_cup", "jap_j_cup", "jap_super", "kor_league_cup", "kor_super_cup", "nir_charity", "nir_lge_cup",
         "nor_cup", "pol_lge_cup", "pol_super", "sco_chal_cup", "swe_cup", "usa_open_cup", "wel_cup", "wel_lge_cup",
         "wel_prm_cup"]
# name: (start year, database folder or None, changing rules, last season, nations, start run, time limit in
# seconds, required log events). An event: ("waits", file, year), ("skip", file, edition), ("resumes", file,
# edition), or a knockout table that takes a row of cup_formats.tsv: ("format", file, season, rounds changed).
RUNS = {
    "r2001": (2001, None, "on", 2008, ["Greece", "Northern Ireland"], False, 5400, {
        ("skip", "gre_super", 2002), ("skip", "gre_super", 2003), ("skip", "gre_super", 2004),
        ("skip", "gre_super", 2005), ("skip", "gre_super", 2006), ("resumes", "gre_super", 2007),
        ("skip", "nir_charity", 2002), ("skip", "nir_charity", 2007),
    }),
    "f2008": (2008, os.path.join(DB, "September 2008"), "on", 2010, ["Italy", "Holland"], False, 5400, {
        ("format", "ita_cup", 2008, 4), ("format", "ita_cup", 2009, 4), ("format", "ita_cup", 2010, 4),
        ("format", "hol_cup", 2009, 1), ("format", "hol_cup", 2010, 0),
    }),
    "x2008": (2008, os.path.join(DB, "September 2008"), "on", 2008, ["all", "France"], False, 7200, {
        ("format", "spa_cup", 2008, 1), ("format", "tur_cup", 2008, 2), ("format", "rus_cup", 2008, 0),
    }),
    "s2020": (2020, os.path.join(DB, "November 2020"), "off", 2021, NATIONS, False, 2400, {
        ("waits", "bel_super", NEVER), ("waits", "hol_super", NEVER), ("waits", "ita_c_cup", NEVER),
        ("waits", "nir_lge_cup", NEVER), ("waits", "nir_charity", NEVER), ("waits", "nor_cup", NEVER),
    }),
    "n2008": (2008, os.path.join(DB, "September 2008"), "on", 2009, ["Germany", "Brazil"], False, 3600, {
        ("waits", "ger_lge_cup", NEVER), ("waits", "bra_champ_cup", NEVER),
    }),
    "s2008": (2008, os.path.join(DB, "September 2008"), "off", 2008, ["all", "France"], True, 1800, {
        ("waits", "wel_prm_cup", NEVER), ("waits", "ger_lge_cup", NEVER), ("waits", "ire_super_cup", NEVER),
        ("waits", "bra_champ_cup", NEVER),
    }),
    "r2019": (2019, os.path.join(DB, "October 2019"), "on", 2022, NATIONS, False, 3000, {
        ("waits", "ita_c1_super", 2020), ("waits", "nir_charity", 2022),
        ("skip", "bel_super", 2020), ("resumes", "bel_super", 2021),
        ("skip", "hol_super", 2020), ("resumes", "hol_super", 2021),
        ("skip", "ita_c_cup", 2020), ("resumes", "ita_c_cup", 2021),
        ("skip", "nir_lge_cup", 2020), ("resumes", "nir_lge_cup", 2021),
        ("skip", "nor_cup", 2020), ("resumes", "nor_cup", 2021),
    }),
}
OPT_IN = set()
FORMAT = re.compile(r"recomp: cups: (ita_cup|hol_cup|spa_cup|tur_cup|rus_cup) \(0x[0-9a-f]+\) season (\d+), "
                    r"row \d+: rounds changed (\d+)")
WAITS = re.compile(r"recomp: cup_breaks: (\w+) waits for (\d+) \(edition (\d+) not held")
SKIP = re.compile(r"recomp: cup_breaks: (\w+) is not held in (\d+)")
RESUMES = re.compile(r"recomp: cup_breaks: (\w+) resumes in (\d+)")
FIXTURE_ADDED = re.compile(r"recomp: cup_breaks: fixture added for")
FIXTURE_YEARS = re.compile(r"recomp: cup_breaks: fixture years ([^:]*):((?: -?\d+)*)")
STARTS = re.compile(r"recomp: cup_breaks: (\w+) season (\d+), year (-?\d+), held (-?\d+)")
NEWS = re.compile(r"recomp: season change: news (\d{4})/\d\d: (.*)$")
KIND_NEWS = re.compile(r"The .* (is played again|is no longer played|is not played)")


class Table:
    """The rows of the kind's files in the rules table of the build, with the rule of season_cup_breaks.c."""

    def __init__(self, path):
        self.rows = collections.defaultdict(dict)
        with open(path, encoding="utf-8") as f:
            lines = [x for x in f if not x.startswith("#")]
        for r in csv.DictReader(lines, delimiter="\t"):
            for file in r["comp_file"].split(";"):
                self.rows[file].setdefault(int(r["season"]), r)

    def calendar(self, file):
        rows = self.rows.get(file)
        if not rows:
            return 0
        first = rows[min(rows)]
        real = first["real_season"]
        return int(len(real) == 4 and real.isdigit() and int(real) == int(first["season"]) + 1)

    def held(self, file, edition):
        rows = self.rows.get(file, {})
        s = min(edition - self.calendar(file), 2099)
        while s >= 2001:
            if s in rows:
                r = rows[s]
                return int(r["clubs"] != "not_held" and r["format"] != "not_held")
            s -= 1
        return -1

    def next(self, file, edition):
        h = self.held(file, edition)
        if h < 0:
            return 0
        if h:
            return edition
        last = max(self.rows[file]) + self.calendar(file)
        for e in range(edition + 1, last + 1):
            if self.held(file, e) == 1:
                return e
        return NEVER


def first_edition(table, file, edition, rules):
    """The year that the constructor of a new game gets (season_kind_cup_breaks.c): the edition when the table holds
    it, else the next edition with changing rules on, or 2999 with changing rules off (Q232) or when none comes."""
    n = table.next(file, edition)
    if n and n != edition and rules == "off":
        return NEVER
    return n


def wait_for_room():
    """Starts a game only when fewer than 12 games run on this Mac and the load of the last minute is below 18
    (Max's limit for the 0.6.0 build sessions)."""
    while True:
        games = subprocess.run(["pgrep", "-x", "cm0102_recomp"], capture_output=True, text=True).stdout.split()
        if len(games) < 12 and os.getloadavg()[0] < 18:
            return
        time.sleep(20)


def check_run(name, program, game, keep, table, reuse=False):
    year, database, rules, last, nations, start_run, limit, events = RUNS[name]
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-cup-breaks-{name}-")
    report = os.path.join(work, "report.tsv")
    if reuse and os.path.exists(report):
        code = 0
    elif os.path.exists(work) and keep:
        return [f"{name}: the folder {work} exists; use --reuse to check it again, or another --keep folder"]
    else:
        # A game that stops on a message box waits for ever, so every run has a time limit.
        wait_for_room()
        code, report = rrc.run(program, game, last, year, database, nations, [], work, limit,
                               {"changing_rules": rules}, {"RECOMP_SEASON_CHANGE_LOG": "1"})
    if code != 0 and not (start_run and code == "timeout"):
        return [f"{name}: the run ended with code {code} ({work})"]
    if not os.path.exists(report):
        return [f"{name}: no report ({work})"]
    fails = []
    seasons, comps = rrc.read_report(report)
    if start_run and not any(x["season"] == year and x["phase"] == "start" for x in seasons):
        fails.append(f"{name}: the report has no start of {year}/{(year + 1) % 100:02d}")
    if not start_run and not any(x["season"] == last and x["phase"] == "end" for x in seasons):
        fails.append(f"{name}: the report has no end of {last}/{(last + 1) % 100:02d} (the run stopped early)")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read().split("\n")
    got = set()
    for line in log:
        m = WAITS.search(line)
        if m:
            got.add(("waits", m.group(1), int(m.group(2))))
            want = first_edition(table, m.group(1), int(m.group(3)), rules)
            if want != int(m.group(2)):
                fails.append(f"{name}: {line.strip()}: expected {want}")
        m = SKIP.search(line)
        if m:
            got.add(("skip", m.group(1), int(m.group(2))))
            if table.held(m.group(1), int(m.group(2))) != 0:
                fails.append(f"{name}: {line.strip()}: the table holds it")
        m = RESUMES.search(line)
        if m:
            got.add(("resumes", m.group(1), int(m.group(2))))
            if table.held(m.group(1), int(m.group(2))) != 1:
                fails.append(f"{name}: {line.strip()}: the table does not hold it")
        m = FORMAT.search(line)
        if m:
            got.add(("format", m.group(1), int(m.group(2)), int(m.group(3))))
    for e in sorted(events - got):
        fails.append(f"{name}: no log line for {e}")
    # A cup that never skipped starts the edition of the season of the change.
    skipped, started = set(), collections.defaultdict(set)
    for line in log:
        m = SKIP.search(line)
        if m:
            skipped.add(m.group(1))
        m = STARTS.search(line)
        if m:
            started[int(m.group(2))].add(m.group(1))
            if m.group(1) not in skipped and int(m.group(3)) + 1 != int(m.group(2)):
                fails.append(f"{name}: {m.group(1)} starts season {m.group(2)} with year {m.group(3)}")
    # A cup that resumes plays its edition: it is complete again at the next season change, so the pass calls its
    # start_new_season and the before hook logs it ("season E + 1"). A run checks this for a return before its last
    # season.
    for kind, f, e in sorted(x for x in got if x[0] == "resumes"):
        if e + 1 <= last and f not in started[e + 1]:
            fails.append(f"{name}: {f} resumed in {e} but did not start again in {e + 1} (it did not play)")
    # No club has a fixture of a cup that waits for ever: the wrap of FIXTURE_MANAGER::add_fixture logs each
    # fixture of 2999 or later and each fixture made inside a constructor that waits for ever. The year blocks of
    # the fixture store are logged too (a date query makes a block with no fixture, so they are only shown).
    blocks = [(m.group(1), [int(y) for y in m.group(2).split()]) for m in (FIXTURE_YEARS.search(x) for x in log) if m]
    if not blocks:
        fails.append(f"{name}: no log line 'fixture years'")
    added = [x.strip() for x in log if FIXTURE_ADDED.search(x)]
    if added:
        fails.append(f"{name}: {len(added)} fixtures of a cup that waits for ever, first: {added[0]}")
    # No cup holds an edition that the table does not hold; a new game in a later season waits when the table does
    # not hold the first edition (the constructor gets the start year, log of the runs).
    seen, names = set(), set()
    for c in comps:
        if c["file"] not in FILES:
            continue
        seen.add(c["file"])
        names.add(c["name"])
        # 2999: a cup that never comes back; 2001 in a 2001 game: the original's first season, which stays.
        if c["year"] != NEVER and not (year == 2001 and c["year"] == 2001) and table.held(c["file"], c["year"]) == 0:
            fails.append(f"{name}: {c['file']} holds {c['year']} in {c['season']} {c['phase']}, "
                         f"the table does not hold that edition")
    if year != 2001:
        for f in sorted(seen):
            n = first_edition(table, f, year, rules)
            if n and n != year and ("waits", f, n) not in got:
                fails.append(f"{name}: {f} has no line 'waits for {n}'")
    for f in sorted({e[1] for e in events if e[0] != "format"} - seen):
        fails.append(f"{name}: the report has no {f}")
    # The news of the kind.
    news = collections.Counter((int(m.group(1)), m.group(2)) for m in (NEWS.search(x) for x in log) if m)
    mine = {k: v for k, v in news.items()
            if KIND_NEWS.match(k[1]) and any(k[1].startswith(f"The {n} ") for n in names)}
    if rules == "off" and mine:
        fails.append(f"{name}: changing rules off, but the news has {sorted(mine)[:3]}")
    if rules == "on":
        need = collections.Counter()
        for f in seen:
            for s in range(year + 1, last + 2):
                if s == last + 1 and f not in started[s]:
                    continue              # the change of a calendar-year cup in the last autumn of the run
                now, before = table.held(f, s), table.held(f, s - 1)
                if now >= 0 and before >= 0 and now != before:
                    need[s] += 1
        have = collections.Counter()
        for (s, _), n in mine.items():
            have[s] += n
        for s in sorted(set(need) | set(have)):
            if need[s] != have[s]:
                fails.append(f"{name}: the news of {s} holds {have[s]} cup sentences, expected {need[s]}: "
                             f"{[t for (y, t) in mine if y == s]}")
    print(f"{name}: {len(got)} log events, {len(seen)} cups of the kind, {sum(mine.values())} news lines, "
          f"{len(fails)} problems ({work})")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    # A stop of this script stops its game too: subprocess.run kills the game when an exception leaves it.
    for sig in (signal.SIGTERM, signal.SIGHUP):
        signal.signal(sig, lambda *_: (_ for _ in ()).throw(KeyboardInterrupt()))
    only = rrc.arg(argv, "--only", ",".join(n for n in RUNS if n not in OPT_IN)).split(",")
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    table = Table(os.path.join(os.path.dirname(program), "seasons", "rules.tsv"))
    fails = []
    for name in [n for n in RUNS if n in only]:
        fails += check_run(name, program, game, keep, table, "--reuse" in argv)
    for t in fails:
        print("FAIL  " + t)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
