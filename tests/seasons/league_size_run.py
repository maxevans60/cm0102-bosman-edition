#!/usr/bin/env python3
"""A league of the size engine plays its real size for one season (0.7.2 league sizes, slice 2; spec
docs/specs/2026-11-13-league-sizes-0.7.2-spec.md, Testing seam 2, D4 and D5). Made from swe_prm_run.py (issue 106).

Usage:
    python3 tests/seasons/league_size_run.py <cm0102_recomp> <game folder> <file> [--year <season>]
        [--last <season> | --to <season>] [--nations all|<name,name,...>|all,-<name>,...] [--keep <folder>] [--reuse]
        [--timeout <seconds>]
    python3 tests/seasons/league_size_run.py --list
    python3 tests/seasons/league_size_run.py --self-test

The run starts a new game in a season (default: the year of the file in ROWS) with all nations (--nations as in
tests/seasons/rules_report_check.py: a list keeps England; "all,-<name>" leaves a nation out), changing rules on,
and plays to the end of --last (default the start year: RECOMP_SEASON_JUMP, the rules report of
tests/seasons/rules_report_check.py). Each edition (+0x40) is checked against its own row of the encoded rules
table (docs/research/2026-10-18-league-rules-encoded.tsv, slice 1): a calendar-year league plays edition Y by the
row of season Y - 1 (its real season is Y), any other league by the row of season Y. The row gives the clubs n and
the meetings m; the rounds are R = m x (n' - 1), n' the count rounded up to even. RECOMP_SIZE_LOG=1
(src/recomp_rt/win/season_sizes.c) writes the lines that the checks read. The checks:
    report   every row of the file in the rules report (start and end of each season) holds the n clubs and m
             meetings of its edition, and m x (n - 1) matches a club once they are set;
    table    a table of n rows when n is more than the class allocates (league_sizes.py HELD);
    dates    a template of R dates for n clubs: the dates ascend, each new date ("+") is a Wednesday or an earlier
             start in steps of 7 days, none is in the largest gap of the class's template (the winter break) or in a
             gap over 14 days, the last date is the class's last date, and with fewer rounds than the class's
             (D5) no date is new and the first date is the class's first;
    played   at the start of the next edition (League::free_table 0x687970) every club of a table of n clubs played
             m x (n - 1) matches, the matches per club (+0xa7);
    expect   the rules report check lists no clubs difference of the file (the first season);
    box      no message box.
A message box ends the run at once (stop_on_box) and fails it. A run takes about 20 to 40 minutes a season. A two-year league ends its table after the end report of its season:
use --last <year + 1> for its played check. --reuse checks the logs in --keep again. Exit code 1 on a failure.
"""
import datetime
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
sys.path.insert(0, os.path.join(ROOT, "tools", "recomp"))
import league_sizes  # noqa: E402

ENCODED = os.path.join(ROOT, "docs", "research", "2026-10-18-league-rules-encoded.tsv")

# The runs of each slice: file: (start year, last season, what it shows). Slice 2: the pilots.
ROWS = {
    "nor_prm": (2009, 2010, "Eliteserien, 16 clubs (the class holds 14): a calendar year with a summer break"),
    "nor_prm@2008": (2008, 2012, "Eliteserien 14 in 2008, 16 from 2009 (a forum report: 14 still in 2012)"),
    "por_second": (2014, 2015, "Segunda Liga, 24 clubs (the class holds 18): a template of 46 dates"),
    "por_second@2008": (2008, 2009, "Segunda Liga, 16 clubs: fewer dates, the season ends on its usual day (D5)"),
    "swe_prm": (2025, 2026, "Allsvenskan, 16 clubs (issue 106, also swe_prm_run.py)"),
    # Slice 15: the Argentine Primera. Edition Y plays the row whose real season begins with Y, else row Y - 1
    # (season_sizes_edition): editions 2014 and 2015 play row 2014, 2016 row 2016, 2025 row 2024 (the 2025 season).
    "arg_prm@2013": (2013, 2014, "Primera 2013/14 (row 2013, 20 in two halves), then 30 in 2014/15: the size step "
                                 "takes 10 clubs of the second division, which takes clubs of the third"),
    "arg_prm": (2014, 2015, "Primera 2014/15 (row 2014): one table of 30, 29 rounds and the derby round"),
    "arg_prm@2016": (2016, 2017, "Primera 2016/17 (row 2016): one table of 30 and the derby round, then 28 (the step)"),
    "arg_prm@2025": (2025, 2026, "Primera row 2024 (the 2025 season): Apertura and Clausura of two zones of 15 with "
                                 "the rival and drawn inter-zone rounds"),
}


def encoded_row(file, season):
    """(clubs, meetings) of the encoded row of a file in a season, by the after-table rule (the last row before)."""
    best = None
    with open(ENCODED, encoding="utf-8") as f:
        cols = f.readline().rstrip("\n").split("\t")
        for line in f:
            row = dict(zip(cols, line.rstrip("\n").split("\t")))
            if row.get("comp_file") != file or not row.get("season", "").isdigit():
                continue
            s = int(row["season"])
            if s <= season and row.get("clubs", "").isdigit() and (best is None or s > best[0]):
                best = (s, int(row["clubs"]), int(row["meetings"]) if row.get("meetings", "").isdigit() else 0)
    return (best[1], best[2]) if best else (0, 0)


def rounds_of(clubs, meetings):
    return meetings * (clubs + (clubs & 1) - 1)


def encoded_form(file, season):
    """(clubs, meetings, zones, extra, rounds, halves) of the encoded row of a file in a season, by the after-table
    rule; zones is the list of zone counts ([] for none), rounds the encoded rounds (0 when not given)."""
    best = None
    with open(ENCODED, encoding="utf-8") as f:
        cols = f.readline().rstrip("\n").split("\t")
        for line in f:
            row = dict(zip(cols, line.rstrip("\n").split("\t")))
            if row.get("comp_file") != file or not row.get("season", "").isdigit():
                continue
            s = int(row["season"])
            if s <= season and row.get("clubs", "").isdigit() and (best is None or s > best[0]):
                best = (s, row)
    if not best:
        return (0, 0, [], 0, 0, 0)
    row = best[1]
    num = lambda k: int(row[k]) if row.get(k, "").isdigit() else 0  # noqa: E731
    zones = [int(z) for z in row.get("zones", "-").split("+") if z.isdigit()]
    return (num("clubs"), num("meetings"), zones, num("extra"), num("rounds"), num("halves"))


def want_counts(file, season):
    """(rounds of the template, matches of a club) of a file's row: m x (n' - 1) and m x (n - 1), plus the derby or
    inter-zone rounds of the row; with zones, those of a zone (each half the same)."""
    clubs, m, zones, extra, rounds, _h = encoded_form(file, season)
    m = m or 2
    if zones:
        return (rounds or m * (max(zones) + (max(zones) & 1) - 1) + extra, m * (max(zones) - 1) + extra)
    if extra and clubs % 2 == 0:
        return (m * (clubs - 1) + extra, m * (clubs - 1) + extra)
    return (rounds_of(clubs, m), m * (clubs - 1))


_REAL = {}


def real_season(file, season):
    """The real_season text of the rules row of a file and season (make_rules.read_rows), or ""."""
    if not _REAL:
        sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
        import make_rules
        for r in make_rules.read_rows():
            if r.get("comp_file") and str(r.get("season", ""))[:4].isdigit():
                _REAL[(r["comp_file"], int(str(r["season"])[:4]))] = r.get("real_season", "")
    return _REAL.get((file, season), "")


def edition_season(file, edition):
    """The season of the row that an edition (+0x40) plays (season_sizes.c season_sizes_edition): row Y when its real
    season begins with Y, else row Y - 1 when its real season begins with Y, else Y."""
    def starts(season):
        t = real_season(file, season)
        return t[:4] == str(edition) and not t[4:5].isdigit()
    if starts(edition):
        return edition
    if starts(edition - 1):
        return edition - 1
    return edition


def parse_dates(words):
    """[(date, added)] of the words of a template line."""
    out = []
    for w in words:
        added = w.endswith("+")
        out.append((datetime.date.fromisoformat(w.rstrip("+")), added))
    return out


def templates(log, file):
    """[(base, class dates, new dates or None)] of the template lines of a file."""
    out, last = [], None
    tag = f"recomp: league sizes: {file} template "
    for line in log.split("\n"):
        if not line.startswith(tag):
            continue
        head, _, rest = line[len(tag):].partition(": ")
        words = rest.split()
        if len(words) < 2 or not head.isdigit():
            continue
        kind, dates = words[0], parse_dates(words[2:])
        if kind == "class":
            last = [int(head), [d for d, _a in dates], None]
            out.append(last)
        elif kind == "new" and last and last[0] == int(head):
            last[2] = dates
    return out


def template_faults(name, cls, new, rounds):
    """The faults of a new template (D4, D5) against the class's dates."""
    fails = []
    if len(new) != rounds:
        return [f"{name}: {len(new)} dates, want {rounds}"]
    days = [d for d, _a in new]
    if any(b <= a for a, b in zip(days, days[1:])):
        fails.append(f"{name}: the dates do not ascend, or two rounds share a date")
    gaps = [(a, b) for a, b in zip(cls, cls[1:])]
    largest = max(((b - a).days for a, b in gaps), default=0)
    for d, added in new:
        if not added:
            if d not in cls:
                fails.append(f"{name}: {d} is not a date of the class")
            continue
        if d < cls[0]:
            if (cls[0] - d).days % 7 or (cls[0] - d).days > 28:
                fails.append(f"{name}: earlier start {d} is not a week step before {cls[0]}")
            continue
        if d.weekday() != 2:
            fails.append(f"{name}: new date {d} is not a Wednesday")
        for a, b in gaps:
            if a < d < b and ((b - a).days > 14 or (b - a).days == largest):
                fails.append(f"{name}: new date {d} in the gap {a} to {b} ({(b - a).days} days, the winter break)")
    if days and days[-1] != cls[-1]:
        fails.append(f"{name}: last date {days[-1]}, the class's last is {cls[-1]}")
    if rounds < len(cls):
        if any(a for _d, a in new):
            fails.append(f"{name}: a new date with fewer rounds than the class's")
        if days and days[0] != cls[0]:
            fails.append(f"{name}: first date {days[0]}, the class's first is {cls[0]}")
    return fails


def played(log, file):
    """[(edition, clubs, matches, lowest, highest)] of the played lines of a file."""
    out = []
    tag = f"recomp: league sizes: {file} played "
    for line in log.split("\n"):
        if line.startswith(tag):
            try:
                ed, rest = line[len(tag):].split(": ", 1)
                p = rest.replace(",", "").split()
                out.append((int(ed), int(p[0]), int(p[2]), int(p[5]), int(p[7])))
            except (ValueError, IndexError):
                pass
    return out


def calendar_year(file):
    """1 when the encoded rows of a file are of a calendar-year league (column calendar)."""
    with open(ENCODED, encoding="utf-8") as f:
        cols = f.readline().rstrip("\n").split("\t")
        for line in f:
            row = dict(zip(cols, line.rstrip("\n").split("\t")))
            if row.get("comp_file") == file:
                return row.get("calendar") == "calendar"
    return False


def edition_row(file, edition):
    """(clubs, meetings) of the row that an edition (+0x40) plays (edition_season): a calendar-year league plays
    edition Y by the row of season Y - 1 (its real season is Y), any other league by the row of season Y."""
    return encoded_row(file, edition_season(file, edition))


def edition_counts(file, edition):
    return want_counts(file, edition_season(file, edition))


def report_rows(report, file):
    """The rows of a file in the rules report: [(season, phase, edition, clubs, meetings, matches)]."""
    out, cols = [], None
    with open(report, encoding="latin-1") as f:
        for line in f:
            p = line.rstrip("\n").split("\t")
            if p[0] == "columns":
                cols = p[1:]
            elif cols and p[0] == "comp" and len(p) == len(cols) + 1:
                row = dict(zip(cols, p[1:]))
                if row["file"] == file:
                    out.append((int(row["season"]), row["phase"], int(row["year"]), int(row["clubs"]),
                                int(row["rounds"]), int(row["matches"])))
    return out


def faults(work, file, year, last):
    """The faults of a run, edition by edition: each edition that the report, the template lines and the played
    lines show is checked against its own row (edition_row)."""
    import rules_report_check as rrc
    report = os.path.join(work, "report.tsv")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    lines = log.split("\n")
    fails = [f"{x.strip()}" for x in lines if "recomp: message box from" in x]
    if edition_row(file, year)[0] < 2:
        return fails + [f"{file} {year}: no row in {os.path.basename(ENCODED)}"]
    held = league_sizes.HELD.get(file, 0)
    # The report: every edition of the run holds the clubs and meetings of its row, and the matches of the count.
    rows = report_rows(report, file) if os.path.exists(report) else []
    if not any(r[0] == year and r[1] == "start" for r in rows):
        fails.append(f"{file} {year}: no row in the start report")
    for season, phase, ed, n, meet, matches in rows:
        clubs, m = edition_row(file, ed)
        m = m or meet
        if (n, meet) != (clubs, m):
            fails.append(f"{file}: report {season} {phase}, edition {ed}: clubs and meetings {(n, meet)}, want "
                         f"{(clubs, m)}")
        elif matches != -1 and matches != edition_counts(file, ed)[1]:
            fails.append(f"{file}: report {season} {phase}, edition {ed}: {matches} matches a club, want "
                         f"{edition_counts(file, ed)[1]}")
    fails += [f"{file}: {x.split(': ', 1)[1]}" for x in lines
              if x.startswith(f"recomp: league sizes: {file} template mode ")
              or x.startswith(f"recomp: league sizes: {file} dates: no room")]
    # The templates of each edition.
    tmpl = templates(log, file)
    if not tmpl:
        fails.append(f"{file} {year}: no template line")
    for base, cls, new in tmpl:
        clubs, m = edition_row(file, base)
        rounds = edition_counts(file, base)[0]
        if rounds == len(cls):
            continue
        if new is None:
            fails.append(f"{file} template {base}: the class's {len(cls)} dates stay, want {rounds}")
        else:
            fails += template_faults(f"{file} template {base}", cls, new, rounds)
        if clubs > held and not any(x.startswith(f"recomp: league sizes: {file} table of {clubs} rows") and
                                    f" {base}, the class makes" in x for x in lines):
            fails.append(f"{file} {base}: no log line of a table of {clubs} rows (the class allocates {held})")
    # The finished tables: every club played the matches of the count of its edition's row.
    done = played(log, file)
    if not done:
        fails.append(f"{file}: no finished table in the run (a two-year league needs --last {year + 1})")
    for ed, n, matches, lo, hi in done:
        clubs, m = edition_row(file, ed)
        want = edition_counts(file, ed)[1]
        if n != clubs or not (matches == lo == hi == want):
            fails.append(f"{file} {ed}: {n} clubs, {matches} matches, played {lo} to {hi}; want {clubs} clubs with "
                         f"{want} each")
    if file == "arg_prm":
        fails += arg_faults(log, year)
    if os.path.exists(report):
        rep_clubs = []
        seasons, comps = rrc.read_report(report, rep_clubs)
        diffs = (set(rrc.differences(seasons, comps, rrc.make_rules.read_rows()))
                 | rrc.division_differences(comps, rep_clubs))
        diffs = sorted(d for d in diffs if d[1] in ("clubs", "division_clubs") and file in d[2])
        fails += [f"{file} {d[0]}: rules report difference: {d[2]}" for d in diffs]
    print(f"{file}: editions in the report " + ", ".join(sorted({f"{ed}:{n}" for _s, _p, ed, n, _m, _x in rows})) +
          "; finished tables " + ", ".join(f"{ed}:{n}:{lo}-{hi}" for ed, n, _x, lo, hi in done))
    return fails


def arg_fixtures(log):
    """{(edition, half): [[(home, away)] a round]} of the fixture lines of the Argentine Primera."""
    out = {}
    tag = "recomp: league sizes: arg_prm fixtures "
    for line in log.split("\n"):
        if line.startswith(tag):
            head, _, rest = line[len(tag):].partition(":")
            ed, half = head.split()
            out[(int(ed), half)] = [[tuple(int(x) for x in m.split("-")) for m in r.split()]
                                    for r in rest.split("|")[1:]]
    return out


def arg_round_faults(name, rounds, n):
    fails = []
    for i, r in enumerate(rounds):
        clubs = [c for m in r for c in m]
        if len(r) != n // 2 or len(set(clubs)) != len(clubs):
            fails.append(f"{name} round {i + 1}: {len(r)} matches, {len(clubs) - len(set(clubs))} clubs twice")
    return fails


def arg_faults(log, year):
    """The Argentine Primera (slice 15): each edition with a fixed list plays the form of its row. Zones: the clubs of
    each zone of the zones line meet once in a half, each club meets two clubs of the other zone (its rival and a
    drawn club) once each, the Clausura is the Apertura with the venues reversed, and in the start season every club,
    rival and drawn pair comes from the research list. Derby round: every pair meets once, the derby pairs twice,
    all in the derby round of the list."""
    fails = []
    lines = log.split("\n")
    fixtures = arg_fixtures(log)
    zone_lines = {}
    derby_lines = {}
    for x in lines:
        m = re.match(r"recomp: league sizes: arg_prm zones (\d+): list (-?\d+), (\d+) clubs of the list, rivals (\d+) "
                     r"of the list, drawn (\d+) of the list, drawn round (\d+); A ([\d ]+); B ([\d ]+);", x)
        if m:
            zone_lines.setdefault(int(m.group(1)), []).append(
                (int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)),
                 [int(c) for c in m.group(7).split()], [int(c) for c in m.group(8).split()]))
        m = re.match(r"recomp: league sizes: arg_prm derby (\d+): list (-?\d+), (\d+) pairs of the list, derby round "
                     r"(\d+); pairs ([\d\- ]+);", x)
        if m:
            derby_lines.setdefault(int(m.group(1)), []).append(
                (int(m.group(3)), int(m.group(4)), [tuple(int(c) for c in p.split("-")) for p in m.group(5).split()]))
    seen = set()
    for (ed, half), rounds in sorted(fixtures.items()):
        season = edition_season("arg_prm", ed)
        clubs, _m, zones, extra, want_rounds, halves = encoded_form("arg_prm", season)
        name = f"arg_prm {ed} {half}"
        seen.add(ed)
        fails += arg_round_faults(name, rounds, clubs)
        if len(rounds) != want_counts("arg_prm", season)[0]:
            fails.append(f"{name}: {len(rounds)} rounds, want {want_counts('arg_prm', season)[0]}")
        meet = {}
        for r in rounds:
            for h, a in r:
                meet[frozenset((h, a))] = meet.get(frozenset((h, a)), 0) + 1
        if zones:
            z = zone_lines.get(season)
            if not z:
                fails.append(f"{name}: no zones line of row {season}")
                continue
            in_list, rivals, drawn, _at, za, zb = z[-1]
            if len(za) != zones[0] or len(zb) != zones[1]:
                fails.append(f"{name}: zones of {len(za)} and {len(zb)}, want {zones}")
            for grp in (za, zb):
                for i, x in enumerate(grp):
                    for y in grp[i + 1:]:
                        if meet.get(frozenset((x, y)), 0) != 1:
                            fails.append(f"{name}: zone clubs {x} and {y} meet {meet.get(frozenset((x, y)), 0)} times")
            for x in za + zb:
                other = zb if x in za else za
                inter = [y for y in other if meet.get(frozenset((x, y)))]
                if len(inter) != extra or any(meet[frozenset((x, y))] != 1 for y in inter):
                    fails.append(f"{name}: club {x} meets {len(inter)} clubs of the other zone, want {extra} once each")
            if half == "clausura":
                ap = fixtures.get((ed, "apertura"), [])
                if [[(a, h) for h, a in r] for r in ap] != rounds:
                    fails.append(f"{name}: not the Apertura with the venues reversed")
            if ed == year and season == year - 1 and (in_list, rivals, drawn) != (clubs, zones[0], zones[0]):
                fails.append(f"{name}: {in_list} clubs, {rivals} rivals and {drawn} drawn pairs of the list, want "
                             f"{clubs}, {zones[0]} and {zones[0]} (a game started in its season)")
            if not halves and half == "clausura":
                fails.append(f"{name}: a Clausura in a row of one table")
        elif extra:
            d = derby_lines.get(season)
            if not d:
                fails.append(f"{name}: no derby line of row {season}")
                continue
            from_list, at, pairs = d[-1]
            derby = {frozenset(p) for p in pairs}
            ids = {c for r in rounds for m in r for c in m}
            for x in ids:
                for y in ids:
                    if x < y and meet.get(frozenset((x, y)), 0) != 1 + (frozenset((x, y)) in derby):
                        fails.append(f"{name}: clubs {x} and {y} meet {meet.get(frozenset((x, y)), 0)} times")
            if at - 1 < len(rounds) and {frozenset(m) for m in rounds[at - 1]} != derby:
                fails.append(f"{name}: round {at} is not the derby round")
            if ed == season == year and from_list != clubs // 2:
                fails.append(f"{name}: {from_list} derby pairs of the list, want {clubs // 2} (a game started in its "
                             f"season)")
    for ed in range(year, year + 2):
        clubs, _m, zones, extra, _r, _h = encoded_form("arg_prm", edition_season("arg_prm", ed))
        if (zones or extra) and ed not in seen and any(f"arg_prm {ed}:" in x or f" {ed}, the class makes" in x
                                                         for x in lines if "league sizes: arg_prm" in x):
            fails.append(f"arg_prm {ed}: no fixture line of its zones or derby round")
    if year in seen:
        print("arg_prm: fixed lists " + ", ".join(f"{ed} {h}: {len(r)} rounds" for (ed, h), r in sorted(fixtures.items())))
    else:
        fails.append(f"arg_prm {year}: no fixture line")
    return fails


def self_test():
    """The template checks on made-up templates (no game)."""
    d = datetime.date
    cls = [d(2009, 4, 5), d(2009, 4, 12), d(2009, 5, 31), d(2009, 7, 26), d(2009, 8, 2)]
    good = [(cls[0], False), (d(2009, 4, 8), True), (cls[1], False), (cls[2], False), (cls[3], False),
            (d(2009, 7, 29), True), (cls[4], False)]
    bad_break = [(cls[0], False), (cls[1], False), (cls[2], False), (d(2009, 6, 10), True), (cls[3], False),
                 (cls[4], False)]
    shrink = [(cls[0], False), (cls[2], False), (cls[4], False)]
    late = [(cls[0], False), (cls[1], False), (cls[2], False), (cls[3], False), (cls[4], False),
            (d(2009, 8, 5), True)]
    shared = [(cls[0], False), (cls[0], True), (cls[1], False), (cls[2], False), (cls[3], False), (cls[4], False)]
    thursday = [(cls[0], False), (d(2009, 4, 9), True), (cls[1], False), (cls[2], False), (cls[3], False),
              (cls[4], False)]
    checks = [
        (template_faults("good", cls, good, 7), False),
        (template_faults("shared", cls, shared, 6), True),
        (template_faults("thursday", cls, thursday, 6), True),
        (template_faults("break", cls, bad_break, 6), True),
        (template_faults("shrink", cls, shrink, 3), False),
        (template_faults("late", cls, late, 6), True),
        (template_faults("count", cls, good, 8), True),
    ]
    bad = [(i, f) for i, (f, want) in enumerate(checks) if bool(f) != want]
    line = "recomp: league sizes: nor_prm template 2009: class 2 2009-04-05 2009-04-12"
    line2 = "recomp: league sizes: nor_prm template 2009: new 3 2009-04-05 2009-04-08+ 2009-04-12"
    t = templates(line + "\n" + line2, "nor_prm")
    if not (len(t) == 1 and len(t[0][1]) == 2 and t[0][2][1] == (d(2009, 4, 8), True)):
        bad.append(("templates", t))
    p = played("recomp: league sizes: nor_prm played 2009: 16 clubs, 30 matches, played 30 to 30 (Eliteserien)",
               "nor_prm")
    if p != [(2009, 16, 30, 30, 30)]:
        bad.append(("played", p))
    if (encoded_row("nor_prm", 2009) != (16, 2) or encoded_row("por_second", 2014) != (24, 2) or
            edition_row("nor_prm", 2008) != (14, 2) or edition_row("nor_prm", 2009) != (16, 2) or
            edition_row("por_second", 2014) != (24, 2)):
        bad.append(("encoded", encoded_row("nor_prm", 2009), encoded_row("por_second", 2014)))
    for b in bad:
        print("FAIL self-test", b)
    print(("FAIL" if bad else "PASS") + " league_size_run self-test")
    return 1 if bad else 0


def main(argv):
    if "--list" in argv:
        for k, (y, last, what) in ROWS.items():
            print(f"{k.split('@')[0]}\t--year {y} --last {last}\t{what}")
        return 0
    if "--self-test" in argv:
        return self_test()
    if len(argv) < 3:
        print(__doc__)
        return 2
    import rules_report_check as rrc
    program, game, file = os.path.abspath(argv[0]), os.path.abspath(argv[1]), argv[2]
    year0, last0, _w = ROWS.get(file, (2025, None, ""))
    year = int(rrc.arg(argv, "--year", str(year0)))
    last = int(rrc.arg(argv, "--last", rrc.arg(argv, "--to", str(last0 or year))))
    keep = rrc.arg(argv, "--keep", None)
    timeout = int(rrc.arg(argv, "--timeout", "7200"))
    work = os.path.abspath(keep) if keep else tempfile.mkdtemp(prefix=f"cm0102-size-{file}-{year}-")
    if "--reuse" not in argv:
        db = rrc.run_database(argv, year, os.path.join(work, "owndb"))
        words = [x.strip().lower().replace("_", " ") for x in rrc.arg(argv, "--nations", "all").split(",")]
        rows = rrc.NATION_ROWS + rrc.NATION_ROWS_BELOW
        names = [next(n for n in rows if n.lower() == w.lstrip("-")) for w in words if w != "all"]
        nations = (["all"] + names) if words[0] == "all" else names
        if nations[0] != "all" and "England" not in nations:
            nations = ["England"] + nations
        code, _report = rrc.run(program, game, last, year, db, nations, [], work, timeout,
                                {"changing_rules": "on"}, {"RECOMP_SIZE_LOG": "1"}, stop_on_box=True)
        if code != 0:
            print(f"FAIL {file} {year}: the run ended with code {code} ({work})")
            return 1
    fails = faults(work, file, year, last)
    for f in fails:
        print("FAIL", f)
    print(("FAIL" if fails else "PASS") + f" {file} {year} to {last} ({work})")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
