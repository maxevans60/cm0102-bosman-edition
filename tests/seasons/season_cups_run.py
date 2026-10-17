#!/usr/bin/env python3
"""Domestic cup check (0.6.0 slice 21): the round formats of the cups by season.

Usage:
    python3 tests/seasons/season_cups_run.py <cm0102_recomp> <game folder> [--only <name>,...] [--keep <folder>]
        [--reuse]

Each run is a season jump of tests/seasons/rules_report_check.py with England, Belgium, France and Norway and the
Conference selected (the cups of these nations hold rows of tools/seasons/cup_formats.tsv). The rules report
writes a "cup" line for each cup object at each start and end of a season: its rounds as legs:replays:first:
deciding (src/recomp_rt/win/season_report.c). The runs:
    base       the base game (3.9.68 data, 2001/02, changing rules off): the originals of the cups. Every other run
               takes its expected rounds from these, by applying the table rows (the same rule as
               season_cups_apply in src/recomp_rt/win/season_cups.c)
    s2020      November 2020, changing rules off: the cups hold the formats of 2020/21 for the whole season
    s2024      October 2024, changing rules off: the formats of 2024/25
    c2001      3.9.68 data, changing rules on, 2001/02 to 2002/03: only the Norwegian Cup changes
    c2015      October 2015, changing rules on, 2015/16 to 2016/17: the FA Cup changes
    c2017      October 2017, changing rules on, 2017/18 to 2018/19: the FA Cup and the League Cup change
    c2020      November 2020, changing rules on, 2020/21 to 2022/23: the formats of 2021 and 2022
The checks (each run, every cup line): the rounds are the expected ones for the season, and the news of each season
holds exactly the sentence of each row that starts in that season for a cup that the run holds (nothing in the
first season, nothing with changing rules off). A run takes 5 to 15 minutes. --reuse checks the reports in the
--keep folder again without a new run.
"""
import collections
import csv
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
import rules_report_check as rrc  # noqa: E402

DB = "/Users/max/Projects/cm0102/test-fixtures/databases"
NATIONS = ["England", "Belgium", "France", "Norway"]
# The French first division of the databases from 2008 on has 20 clubs and the game stops at the start with the box
# "fra_first..cpp 108" (the league size is slice 18's), so those runs leave France out and its two cups are not
# tested from 2008 on.
NATIONS_LATE = ["England", "Belgium", "Norway"]
EXTRA = ["conference"]
# name: (start year, database folder or None, changing rules, last season)
RUNS = {
    "base": (2001, None, "off", 2001),
    "s2020": (2020, os.path.join(DB, "November 2020"), "off", 2020),
    "s2024": (2024, os.path.join(DB, "October 2024"), "off", 2024),
    "c2001": (2001, None, "on", 2002),
    "c2015": (2015, os.path.join(DB, "October 2015"), "on", 2016),
    "c2017": (2017, os.path.join(DB, "October 2017"), "on", 2018),
    "c2020": (2020, os.path.join(DB, "November 2020"), "on", 2022),
}
NEWS = re.compile(r"recomp: season change: news (\d{4})/\d\d: (.*)$")
NOT_APPLIED = re.compile(r"recomp: cups: .* not applied")


def read_rows():
    """{file: [(season, rounds, types, sentence)]} of tools/seasons/cup_formats.tsv, by season."""
    out = collections.defaultdict(list)
    with open(os.path.join(ROOT, "tools", "seasons", "cup_formats.tsv"), newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            out[r["file"]].append((int(r["season"]), int(r["rounds"]), r["types"].split(), r["sentence"]))
    for rows in out.values():
        rows.sort()
    return out


def apply_type(word, rnd):
    """The rule of season_cups_apply in Python. rnd = [legs, replays, first, deciding, days]."""
    legs, replays, first, deciding = rnd[:4]
    keep = first & ~0x4007
    if word == "E":
        first, replays, legs = keep | 3, 0, 1
    elif word == "P":
        first, replays, legs = keep | 1, 0, 1
    elif word == "R":
        first, deciding, replays, legs = keep, 3, 1, 1
    elif word in ("L", "LN", "LH"):
        first = keep | (0x4000 if word == "LH" else 0)
        deciding = 3 | (4 if word == "LN" else 0x4000 if word == "LH" else 0)
        replays = 0
        legs = 2
    return [legs, replays, first, deciding, rnd[4]]


def parse_cups(report):
    """[(season, phase, slot, file, [rounds as lists of legs, replays, first, deciding])] from the cup lines."""
    out = []
    with open(report, encoding="latin-1") as f:
        for line in f:
            cells = line.rstrip("\n").split("\t")
            if cells[0] != "cup":
                continue
            rounds = []
            for word in cells[6].split(" "):
                legs, replays, first, deciding = word.split(":")
                rounds.append([int(legs), int(replays), int(first, 16), int(deciding, 16), 0])
            out.append((int(cells[1]), cells[2], cells[3], cells[4], rounds))
    return out


def expected(rows, originals, file, season):
    """The rounds of a cup in a season: the original with the latest row not later than the season."""
    rounds = [list(r) for r in originals[file]]
    best = None
    for row in rows.get(file, []):
        if row[0] <= season:
            best = row
    if best and best[1] == len(rounds):
        rounds = [apply_type(w, r) if w != "." else r for w, r in zip(best[2], rounds)]
    return rounds


def key(rounds):
    return [r[:4] for r in rounds]


def check_run(name, program, game, keep, originals, rows, reuse=False):
    year, database, rules, last = RUNS[name]
    nations = NATIONS if database is None else NATIONS_LATE
    work = os.path.join(keep, name) if keep else tempfile.mkdtemp(prefix=f"cm0102-cups-{name}-")
    report = os.path.join(work, "report.tsv")
    if reuse and os.path.exists(report):
        code = 0
    else:
        code, report = rrc.run(program, game, last, year, database, nations, EXTRA, work, 10800,
                               {"changing_rules": rules}, {"RECOMP_SEASON_CHANGE_LOG": "1"})
    if code != 0:
        return [f"{name}: the run ended with code {code} ({work})"], None
    cups = parse_cups(report)
    fails = []
    files = sorted({c[3] for c in cups})
    if name == "base":
        for _, phase, _, file, rounds in cups:
            originals.setdefault(file, rounds)
        wanted = {"eng_fa_cup", "eng_cc_cup", "eng_fa_trophy", "bel_fa_cup", "fra_cup", "fra_lge_cup", "nor_cup"}
        if wanted - set(files):
            fails.append(f"{name}: no cup line for {sorted(wanted - set(files))}")
        print(f"{name}: originals of {len(files)} cups: {', '.join(files)}")
        return fails, files
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read().split("\n")
    if [x for x in log if NOT_APPLIED.search(x)]:
        fails.append(f"{name}: a row was not applied because the cup holds another round count: "
                     f"{[x for x in log if NOT_APPLIED.search(x)][:3]}")
    changed = 0
    for season, phase, slot, file, rounds in cups:
        if file not in originals or file not in rows:
            continue          # a cup with no row keeps what the game makes (the World Cup sections change by edition)
        want = expected(rows, originals, file, season if rules == "on" else year)
        later = expected(rows, originals, file, season + 1) if rules == "on" and phase == "end" else want
        # A calendar-year cup (the Norwegian Cup) makes its next edition in January, before the end report.
        if key(rounds) != key(want) and key(rounds) != key(later):
            fails.append(f"{name}: {file} {season}/{(season + 1) % 100:02d} {phase}: rounds {key(rounds)}, "
                         f"expected {key(want)}")
        if key(want) != key(originals[file]):
            changed += 1
    news = collections.Counter((int(m.group(1)), m.group(2)) for m in (NEWS.search(x) for x in log) if m)
    want_news = collections.Counter()
    if rules == "on":
        for file in files:
            for row in rows.get(file, []):
                if year < row[0] <= last and row[1] == len(originals[file]):
                    want_news[(row[0], row[3])] += 1
    sentences = {r[3] for rs in rows.values() for r in rs}
    got = collections.Counter({k: v for k, v in news.items() if k[1] in sentences})
    for k in sorted(set(want_news) - set(got)):
        fails.append(f"{name}: the news of {k[0]} lacks: {k[1]}")
    for k in sorted(set(got) - set(want_news)):
        fails.append(f"{name}: the news of {k[0]} has an extra line: {k[1]}")
    if rules == "off" and any("season change" in x for x in log):
        fails.append(f"{name}: changing rules off, but the log shows a season change")
    print(f"{name}: {len(cups)} cup lines, {changed} with a changed format, {sum(got.values())} news lines, "
          f"{len(fails)} problems ({work})")
    return fails, files


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = rrc.arg(argv, "--only", ",".join(RUNS)).split(",")
    keep = os.path.abspath(rrc.arg(argv, "--keep")) if "--keep" in argv else None
    rows = read_rows()
    originals = {}
    fails = []
    if "base" not in only:
        only = ["base"] + only          # the originals come first
    for name in [n for n in RUNS if n in only]:
        f, _ = check_run(name, program, game, keep, originals, rows, "--reuse" in argv)
        fails += f
        if name == "base" and f:
            break
    for t in fails:
        print("FAIL  " + t)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
