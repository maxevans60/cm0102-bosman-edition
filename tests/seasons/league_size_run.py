#!/usr/bin/env python3
"""A league of the size engine plays its real size for one season (0.7.2 league sizes, slice 2; spec
docs/specs/2026-11-13-league-sizes-0.7.2-spec.md, Testing seam 2, D4 and D5). Made from swe_prm_run.py (issue 106).

Usage:
    python3 tests/seasons/league_size_run.py <cm0102_recomp> <game folder> <file> [--year <season>]
        [--last <season> | --to <season>] [--nations all|<name,name,...>|all,-<name>,...] [--keep <folder>] [--reuse]
        [--timeout <seconds>] [--extra conference,regional,c2] [--save-in-split]
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
A league whose row has a split (slice 7, D7; src/recomp_rt/win/season_split.c) plays its table to N matches a club,
then sections of the row: its template holds R dates and the most rounds of a section, the matches per club are N
and those of the largest section, every club of a finished table played N and the matches of its own section, and
a split line names the row's sections and meetings, the dates of the sections are the last dates of the template
line, and at the end of the table (a split end line, in table order) the places after match N are each place once,
each section's clubs stand together above the next section's, and each club played N and its own section's matches. --save-in-split plays to the first split, saves after the first section date,
loads that save in a new program and plays on (leg0 and leg1 in the work folder): the loaded league goes on after
its split and its table finishes.
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
    # Slice 9: Italy. All four classes are sequential (flag 0x40): the template holds exactly the rounds.
    "ita_ser_a": (2005, 2006, "Serie A, 20 clubs (the class holds 18): 38 dates of a template of 34"),
    "ita_ser_b@2003": (2003, 2004, "Serie B, 24 clubs (the class holds 20): 46 dates of a template of 38"),
    "ita_ser_b": (2018, 2019, "Serie B, 19 clubs: 38 dates, a rest round each round"),
    "ita_ser_c1a": (2004, 2005, "Serie C1/A, 19 clubs (the class holds 18): 38 dates of a template of 34"),
    "ita_ser_c1a@2012": (2012, 2013, "Serie C1/A 17 and C1/B 16: 34 and 30 dates, the play-out rows of the count"),
    # Slice 8: England and France.
    "eng_conf": (2010, 2011, "Conference, 24 clubs (the class holds 22): a template of 46 dates"),
    "fra_third@2010": (2010, 2011, "National, 21 clubs (the class holds 20): a mirrored template of 42 dates"),
    "fra_third": (2014, 2015, "National, 18 clubs: a mirrored template of 34 dates, the season ends on its usual day "
                              "(D5)"),
    # Slice 7, the split engine (D7): the table to N matches, then sections of the row.
    "nir_prm": (2010, 2011, "IFA Premiership, 12 clubs (the class holds 10): 33 rounds, then sections of 6 and 6"),
    "wel_first": (2012, 2013, "Welsh Premier, 12 clubs: 22 rounds, then sections of 6 and 6 that meet twice"),
    "wel_first@2026": (2026, 2027, "Welsh Premier, 16 clubs: 30 rounds, then sections of 6, 4 and 6"),
    # Slice 17 (D11): the class plays one meeting on 27 dates and a play-off of the top 4. Rows are keyed one year
    # before the real season (row 2003 is the 2004 Série A).
    "bra_nat_first@2003": (2003, 2004, "Série A, 24 clubs in two meetings: 46 dates, no play-off"),
    "bra_nat_first": (2010, 2011, "Série A, 20 clubs in two meetings: 38 dates, no play-off"),
    "bra_nat_second": (2006, 2007, "Série B, 20 clubs in two meetings: 38 dates, no play-off"),
    # Slice 13: the NIFL Championship (the class holds 10 in 4 meetings on 36 dates) and the League of Ireland First
    # Division (the class takes 9 to 12; rows are keyed one year before the real season, row 2013 is the 2014 season).
    "nir_first": (2016, 2017, "NIFL Championship, 12 clubs: 22 rounds, then sections of 6 and 6 that meet twice"),
    "nir_first@2022": (2022, 2023, "NIFL Championship, 12 clubs: 33 rounds, then sections of 6 and 6 once more"),
    "ire_first": (2013, 2014, "League of Ireland First Division, 8 clubs in four meetings: 28 dates (D5)"),
    # Slice 15: the Argentine Primera. Edition Y plays the row whose real season begins with Y, else row Y - 1
    # (season_sizes_edition): editions 2014 and 2015 play row 2014, 2016 row 2016, 2025 row 2024 (the 2025 season).
    "arg_prm@2013": (2013, 2014, "Primera 2013/14 (row 2013, 20 in two halves), then 30 in 2014/15: the size step "
                                 "takes 10 clubs of the second division, which takes clubs of the third"),
    "arg_prm": (2014, 2015, "Primera 2014/15 (row 2014): one table of 30, 29 rounds and the derby round"),
    "arg_prm@2016": (2016, 2017, "Primera 2016/17 (row 2016): one table of 30 and the derby round, then 28 (the step)"),
    "arg_prm@2025": (2025, 2026, "Primera row 2024 (the 2025 season): Apertura and Clausura of two zones of 15 with "
                                 "the rival and drawn inter-zone rounds"),
    # Slice 16 (D10): the Argentine second division. The class plays 25 clubs: an Apertura of one meeting, then three
    # Clausura zones of 8 and a Reducido. Every row but 2001/02 plays one table of its count and meetings, on the dates
    # of both class halves (25 and 14).
    "arg_second": (2010, 2011, "Primera B Nacional 2010/11, 20 clubs in two meetings: 38 dates, no class stages"),
    "arg_second@2016": (2016, 2017, "Primera B Nacional 2016/17, 23 clubs in two meetings: 46 dates, then 25 in one "
                                    "meeting (the size step)"),
    # Slice 17 (D11): the class plays one meeting on 27 dates and a play-off of the top 4. Rows are keyed one year
    # before the real season (row 2003 is the 2004 Série A).
    "bra_nat_first@2003": (2003, 2004, "Série A, 24 clubs in two meetings: 46 dates, no play-off"),
    "bra_nat_first": (2010, 2011, "Série A, 20 clubs in two meetings: 38 dates, no play-off"),
    "bra_nat_second": (2006, 2007, "Série B, 20 clubs in two meetings: 38 dates, no play-off"),
}

# The extra divisions of the new game screen that a file needs (rules_report_check.EXTRA): the game plays the
# Conference only when the screen selects it.
EXTRA = {"eng_conf": ["conference"]}


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


def encoded_split(file, season):
    """The split of the encoded row of a file in a season (the after-table rule): (after, [(size, meetings)]) or
    None for one table or a split the engine does not play (unknown section meetings)."""
    best = None
    with open(ENCODED, encoding="utf-8") as f:
        cols = f.readline().rstrip("\n").split("\t")
        for line in f:
            row = dict(zip(cols, line.rstrip("\n").split("\t")))
            if row.get("comp_file") != file or not row.get("season", "").isdigit():
                continue
            s = int(row["season"])
            if s <= season and row.get("clubs", "").isdigit() and (best is None or s > best[0]):
                best = (s, row.get("split", "-"), row.get("split_meet", "-"))
    if not best or not best[1].startswith("after "):
        return None
    head, sizes = best[1].split(",")[0][len("after "):].split(": ")
    meet = best[2].split("+")
    if not all(m.isdigit() for m in meet):
        return None
    return int(head), list(zip(map(int, sizes.split("+")), map(int, meet)))


def split_numbers(sp):
    """(rounds after the split: the most of a section, the fewest and the most matches of a club after it)."""
    rounds = max(m * (n + (n & 1) - 1) for n, m in sp)
    matches = [m * (n - 1) for n, m in sp]
    return rounds, min(matches), max(matches)


def split_dates(log, file):
    """{edition: [dates]} of the split dates lines of a file (the probe at the split, season_sizes.c)."""
    out = {}
    tag = f"recomp: league sizes: {file} split dates "
    for line in log.split("\n"):
        if line.startswith(tag):
            head, _, rest = line[len(tag):].partition(": ")
            if head.isdigit():
                out[int(head)] = [d for d, _a in parse_dates(rest.split()[1:])]
    return out


def split_ends(log, file):
    """{edition: [(place after match N, played)]} in table order, of the split end lines of a file."""
    out = {}
    tag = f"recomp: league sizes: {file} split end "
    for line in log.split("\n"):
        if line.startswith(tag):
            head, _, rest = line[len(tag):].partition(": ")
            try:
                out[int(head)] = [tuple(int(x) for x in w.split(":")) for w in rest.split()]
            except ValueError:
                pass
    return out


def end_faults(name, rows, after, sp):
    """The faults of a table at its end after a split: each section holds its count of clubs by their places after
    match N, the table keeps each section together and above the next, and each club played N and its own section's
    matches. The places after match N need not be each place once at the end: League::sort_table 0x6827d0 writes
    each row's place again at index played - 1 at every sort (0x682ada..0x682b0f), so a club that has not yet played
    its first section match (a later kick-off on the day, or a postponed match) gets its place within its section
    at match N again. That place stays in its section, as the order sorts by section first (season_split.c)."""
    fails = []
    places = [p for p, _x in rows]
    total = sum(n for n, _m in sp)
    if len(places) != total or any(not 0 <= p < total for p in places):
        return [f"{name}: the places after match {after} are not places of a table of {total}: {places}"]
    bounds, top = [], 0
    for n, m in sp:
        bounds.append((top, top + n, after + m * (n - 1)))
        top += n
    secs = [next(k for k, (a, b, _w) in enumerate(bounds) if a <= p < b) for p in places]
    counts = [secs.count(k) for k in range(len(sp))]
    if counts != [n for n, _m in sp]:
        return [f"{name}: the sections hold {counts} clubs by the places after match {after}, want "
                f"{[n for n, _m in sp]}: {places}"]
    if secs != sorted(secs):
        fails.append(f"{name}: the table does not keep the sections in order: {secs}")
    for (p, played), k in zip(rows, secs):
        if played != bounds[k][2]:
            fails.append(f"{name}: the club of place {p + 1} after match {after} played {played}, want {bounds[k][2]}")
    return fails


def splits(log, file):
    """{edition: (after, [sizes], [meetings])} of the split lines of a file, and the editions loaded after one."""
    out, loaded = {}, set()
    tag = f"recomp: league sizes: {file} split "
    for line in log.split("\n"):
        if not line.startswith(tag):
            continue
        head, _, rest = line[len(tag):].partition(": ")
        if not head.isdigit():
            continue
        if rest.startswith("after "):
            p = rest.replace(",", "").split()
            try:
                out[int(head)] = (int(p[1]), [int(x) for x in p[3].split("+")], [int(x) for x in p[5].split("+")])
            except (ValueError, IndexError):
                pass
        elif rest.startswith("loaded"):
            loaded.add(int(head))
    return out, loaded


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
        kind = words[0]
        if kind not in ("class", "new"):
            continue
        dates = parse_dates(words[2:])
        if kind == "class":
            last = [int(head), [d for d, _a in dates], None]
            out.append(last)
        elif kind == "new" and last and last[0] == int(head):
            last[2] = dates
    return out


# Earlier starts of 7 days a class may take (season_template.h SEASON_TEMPLATE_EARLIER, season_sizes_bra.c BRA_EARLIER).
EARLIER = 4
EARLIER_BY_FILE = {"bra_nat_first": 13, "bra_nat_second": 13}


def template_faults(name, cls, new, rounds, earlier=EARLIER):
    """The faults of a new template (D4, D5) against the class's dates. A template with no gap over 7 days has no
    winter break (season_template.c)."""
    fails = []
    if len(new) != rounds:
        return [f"{name}: {len(new)} dates, want {rounds}"]
    days = [d for d, _a in new]
    if any(b <= a for a, b in zip(days, days[1:])):
        fails.append(f"{name}: the dates do not ascend, or two rounds share a date")
    gaps = [(a, b) for a, b in zip(cls, cls[1:])]
    largest = max(((b - a).days for a, b in gaps), default=0)
    if largest <= 7:
        largest = 0
    for d, added in new:
        if not added:
            if d not in cls:
                fails.append(f"{name}: {d} is not a date of the class")
            continue
        if d < cls[0]:
            if (cls[0] - d).days % 7 or (cls[0] - d).days > 7 * earlier:
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


def edition_split(file, edition):
    """The split of the row that an edition plays (edition_row), or None."""
    return encoded_split(file, edition_season(file, edition))


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


# Slice 17 (D11): leagues whose class plays a play-off that the row's meetings leave out (season_sizes_bra.c).
NO_PLAYOFF = {"bra_nat_first", "bra_nat_second"}


def no_playoff_faults(log, file, meetings):
    """With two meetings: the log says the play-off is off, and the club on top of the table gets the title (code 5
    in the league stage, the base handler 0x686940)."""
    if file not in NO_PLAYOFF or meetings < 2:
        return []
    lines = log.split("\n")
    fails = []
    if not any(x.startswith(f"recomp: league sizes: {file} ") and x.endswith("no play-off") for x in lines):
        fails.append(f"{file}: no log line of a season with no play-off")
    if not any(x.startswith(f"recomp: league sizes: {file} ") and " champion " in x for x in lines):
        fails.append(f"{file}: no champion of the table in the log")
    return fails


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
    # A class of DATA_COUNT allocates the rows of the clubs it counts (IreFirst::create_table 0x635910), so it
    # writes no table line.
    held = league_sizes.DATA_COUNT.get(file, league_sizes.HELD.get(file, 0))
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
        else:
            sp = edition_split(file, ed)
            want = edition_counts(file, ed)[1] + (split_numbers(sp[1])[2] if sp else 0)
            if matches != -1 and matches != want:
                fails.append(f"{file}: report {season} {phase}, edition {ed}: {matches} matches a club, want {want}")
    fails += [f"{file}: {x.split(': ', 1)[1]}" for x in lines
              if x.startswith(f"recomp: league sizes: {file} template mode ")
              or x.startswith(f"recomp: league sizes: {file} dates: no room")
              or (x.startswith(f"recomp: league sizes: {file} split") and
                  any(w in x for w in ("no dates", "not made", "not each place")))]
    # The templates of each edition.
    tmpl = templates(log, file)
    if not tmpl:
        fails.append(f"{file} {year}: no template line")
    for base, cls, new in tmpl:
        clubs, m = edition_row(file, base)
        sp = edition_split(file, base)
        rounds = edition_counts(file, base)[0] + (split_numbers(sp[1])[0] if sp else 0)
        if rounds == len(cls) and not sp:
            continue
        if new is None:
            fails.append(f"{file} template {base}: the class's {len(cls)} dates stay, want {rounds}")
        else:
            fails += template_faults(f"{file} template {base}", cls, new, rounds,
                                     EARLIER_BY_FILE.get(file, EARLIER))
        if clubs > held and not any(x.startswith(f"recomp: league sizes: {file} table of {clubs} rows") and
                                    f" {base}, the class makes" in x for x in lines):
            fails.append(f"{file} {base}: no log line of a table of {clubs} rows (the class allocates {held})")
    # The finished tables: every club played the matches of the count of its edition's row.
    done = played(log, file)
    if not done:
        fails.append(f"{file}: no finished table in the run (a two-year league needs --last {year + 1})")
    made, loaded = splits(log, file)
    news = {base: new for base, _c, new in tmpl if new}
    for base, dates in split_dates(log, file).items():
        tail = [d for d, _a in news.get(base, [])][-len(dates):] if dates else []
        if dates != tail:
            fails.append(f"{file} {base}: the section dates {dates} are not the last dates of the template {tail}")
    for ed, n, matches, lo, hi in done:
        clubs, m = edition_row(file, ed)
        m = m or 2
        sp = edition_split(file, ed)
        if not sp:
            want = edition_counts(file, ed)[1]
            if n != clubs or not (matches == lo == hi == want):
                fails.append(f"{file} {ed}: {n} clubs, {matches} matches, played {lo} to {hi}; want {clubs} clubs "
                             f"with {want} each")
            continue
        # A split: every club played the table's matches and those of its section (the fewest and the most).
        after = m * (clubs - 1)
        _r, few, most = split_numbers(sp[1])
        if n != clubs or (matches, lo, hi) != (after + most, after + few, after + most):
            fails.append(f"{file} {ed}: {n} clubs, {matches} matches, played {lo} to {hi}; want {clubs} clubs, "
                         f"{after + most} matches, played {after + few} to {after + most}")
        want = (sp[0], [x for x, _m in sp[1]], [y for _x, y in sp[1]])
        if made.get(ed) != want:
            fails.append(f"{file} {ed}: split line {made.get(ed)}, want after, sections and meetings {want}")
        ends = split_ends(log, file)
        if ed not in ends:
            fails.append(f"{file} {ed}: no split end line")
        else:
            fails += end_faults(f"{file} {ed}", ends[ed], sp[0], sp[1])
    if file == "arg_prm":
        fails += arg_faults(log, year)
    fails += no_playoff_faults(log, file, edition_row(file, year)[1] or 2)
    if os.path.exists(report):
        rep_clubs = []
        seasons, comps = rrc.read_report(report, rep_clubs)
        diffs = (set(rrc.differences(seasons, comps, rrc.make_rules.read_rows()))
                 | rrc.division_differences(comps, rep_clubs))
        diffs = sorted(d for d in diffs if d[1] in ("clubs", "division_clubs") and file in d[2])
        fails += [f"{file} {d[0]}: rules report difference: {d[2]}" for d in diffs]
    print(f"{file}: editions in the report " + ", ".join(sorted({f"{ed}:{n}" for _s, _p, ed, n, _m, _x in rows})) +
          "; finished tables " + ", ".join(f"{ed}:{n}:{lo}-{hi}" for ed, n, _x, lo, hi in done) +
          "; splits " + ", ".join(f"{ed}:{'+'.join(map(str, v[1]))}" for ed, v in sorted(made.items())) +
          ("; loaded after a split " + ", ".join(map(str, sorted(loaded))) if loaded else ""))
    return fails


def save_in_split(program, game, year, last, db, nations, work, timeout):
    """--save-in-split: leg 0 plays to the first split of a league and saves after the first section date
    (RECOMP_SAVE_AFTER_SPLIT=1, src/recomp_rt/win/season_split.c and testrun.c); leg 1 loads that save (soak_run.load_leg) and plays to
    the end of --last. The logs and reports of both legs join in work for the checks of faults. The exit code of
    the first leg that fails, else 0."""
    import rules_report_check as rrc
    import smoke_run
    import soak_run
    leg0, leg1 = os.path.join(work, "leg0"), os.path.join(work, "leg1")
    code, _r = rrc.run(program, game, last, year, db, nations, [], leg0, timeout, {"changing_rules": "on"},
                       {"RECOMP_SIZE_LOG": "1", "RECOMP_SAVE_AFTER_SPLIT": "1"}, stop_on_box=True)
    save = os.path.join(leg0, "user", smoke_run.SAVE_NAME)
    if code != 0 or not os.path.exists(save):
        print(f"FAIL leg 0: code {code}, save {os.path.exists(save)} ({leg0})")
        return code or 1
    soak_run.LEG_ENV["RECOMP_SIZE_LOG"] = "1"
    o = {"program": program, "game": game, "rules": "on", "database": db, "nations": nations, "extra": [],
         "start": year, "last": last, "seed": 1, "timeout": timeout}
    code, _took, box = soak_run.load_leg(o, leg1, save, 0)
    if box:
        print(f"FAIL leg 1: {box}")
        return code or 1
    with open(os.path.join(work, "stderr.txt"), "w", encoding="latin-1") as out:
        for leg in (leg0, leg1):
            out.write(open(os.path.join(leg, "stderr.txt"), encoding="latin-1").read())
    # The report: leg 1 writes a start report of the season it loads in, which leg 0 already wrote. Its lines of a
    # season and phase that leg 0 holds stay out, so no club counts twice.
    with open(os.path.join(work, "report.tsv"), "w", encoding="latin-1") as out:
        seen = set()
        for k, leg in enumerate((leg0, leg1)):
            path = os.path.join(leg, "report.tsv")
            if not os.path.exists(path):
                continue
            mine = set()
            for line in open(path, encoding="latin-1"):
                p = line.split("\t")
                key = (p[1], p[2]) if len(p) > 2 and p[1].isdigit() else None
                if k and (key in seen or (key is None and line.startswith(("#", "columns")))):
                    continue
                if key:
                    mine.add(key)
                out.write(line)
            seen |= mine
    return code


def save_faults(work, file):
    """The checks of --save-in-split: leg 0 split and saved, leg 1 loaded the league after its split, and a table
    of that edition finished in leg 1 (the played check of faults reads it)."""
    leg0 = open(os.path.join(work, "leg0", "stderr.txt"), encoding="latin-1").read()
    leg1 = open(os.path.join(work, "leg1", "stderr.txt"), encoding="latin-1").read()
    made, _l = splits(leg0, file)
    _m, loaded = splits(leg1, file)
    fails = []
    if not made:
        fails.append(f"{file}: no split in leg 0")
    if "recomp: saved " not in leg0 or f"recomp: league sizes: {file} split " not in leg0 or \
            "a test save after section date 1" not in leg0:
        fails.append(f"{file}: leg 0 did not save after the first section date")
    if not loaded or not set(made) & loaded:
        fails.append(f"{file}: leg 1 loaded no edition after its split (split {sorted(made)}, loaded {sorted(loaded)})")
    if not any(ed in made for ed, *_x in played(leg1, file)):
        fails.append(f"{file}: no table of the split edition finished in leg 1")
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
    # The zones or derby line that each fixture line follows (an edition's plan is logged just before its lists).
    plan_of = {}
    last_plan = None
    for x in lines:
        m = re.match(r"recomp: league sizes: arg_prm zones (\d+): list (-?\d+), (\d+) clubs of the list, rivals (\d+) "
                     r"of the list, drawn (\d+) of the list, drawn round (\d+); A ([\d ]+); B ([\d ]+);", x)
        if m:
            last_plan = ("zones", int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)),
                         [int(c) for c in m.group(7).split()], [int(c) for c in m.group(8).split()])
        m = re.match(r"recomp: league sizes: arg_prm derby (\d+): list (-?\d+), (\d+) pairs of the list, derby round "
                     r"(\d+); pairs ([\d\- ]+);", x)
        if m:
            last_plan = ("derby", int(m.group(3)), int(m.group(4)),
                         [tuple(int(c) for c in p.split("-")) for p in m.group(5).split()])
        m = re.match(r"recomp: league sizes: arg_prm fixtures (\d+) (\w+):", x)
        if m:
            plan_of[(int(m.group(1)), m.group(2))] = last_plan
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
        plan = plan_of.get((ed, half if half == "apertura" else "apertura"))
        if zones:
            if not plan or plan[0] != "zones":
                fails.append(f"{name}: no zones line before its fixtures")
                continue
            in_list, rivals, drawn, _at, za, zb = plan[1:]
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
            # A game started in its season: the club moves give the division the clubs of the database's date
            # (December 2025 for a 2025 start), so a club of the list can be missing; at most 2 of each.
            if ed == year and (in_list < clubs - 2 or rivals < zones[0] - 2 or drawn < zones[0] - 2):
                fails.append(f"{name}: {in_list} clubs, {rivals} rivals and {drawn} drawn pairs of the list, want at "
                             f"least {clubs - 2}, {zones[0] - 2} and {zones[0] - 2} (a game started in its season)")
            if not halves and half == "clausura":
                fails.append(f"{name}: a Clausura in a row of one table")
        elif extra:
            if not plan or plan[0] != "derby":
                fails.append(f"{name}: no derby line before its fixtures")
                continue
            from_list, at, pairs = plan[1:]
            derby = {frozenset(p) for p in pairs}
            ids = {c for r in rounds for m in r for c in m}
            for x in ids:
                for y in ids:
                    if x < y and meet.get(frozenset((x, y)), 0) != 1 + (frozenset((x, y)) in derby):
                        fails.append(f"{name}: clubs {x} and {y} meet {meet.get(frozenset((x, y)), 0)} times")
            if at - 1 < len(rounds) and {frozenset(m) for m in rounds[at - 1]} != derby:
                fails.append(f"{name}: round {at} is not the derby round")
            if ed == season == year and from_list < clubs // 2 - 2:
                fails.append(f"{name}: {from_list} derby pairs of the list, want at least {clubs // 2 - 2} (a game "
                             f"started in its season)")
    for ed in range(year, year + 2):
        clubs, _m, zones, extra, _r, _h = encoded_form("arg_prm", edition_season("arg_prm", ed))
        if (zones or extra) and ed not in seen and any(f"arg_prm {ed}:" in x or f" {ed}, the class makes" in x
                                                         for x in lines if "league sizes: arg_prm" in x):
            fails.append(f"arg_prm {ed}: no fixture line of its zones or derby round")
    fails += [f"arg_prm: {x.split(': ', 1)[1]}" for x in lines if "arg_prm fixed list not played" in x]
    # The averages hold the clubs of each finished edition (ArgPrm::update_relegation_averages 0x406570 at its end).
    for ed, n, _x, _lo, _hi in played(log, "arg_prm"):
        if not re.search(rf"recomp: league sizes: arg_prm {ed}: averages of {n} clubs", log):
            fails.append(f"arg_prm {ed}: no averages line of {n} clubs")
    fails += [f"arg_prm: {x.split(': ', 1)[1]}" for x in lines
              if re.search(r"league sizes: step: .* from Argentine Second Division to no division", x)]
    print("arg_prm: fixed lists " + ", ".join(f"{ed} {h}: {len(r)} rounds" for (ed, h), r in sorted(fixtures.items())))
    for x in lines:
        if "league sizes: arg_prm " in x and "not in the division:" in x:
            print("arg_prm: " + x.split(": ", 1)[1].split("; ")[0] + "; not in the division:" +
                  x.split("not in the division:", 1)[1].split(";")[0])
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
    # A template of 7-day gaps only (the Brazilian classes) has no winter break: a Wednesday in a gap is good, and the
    # earlier starts go up to the class's limit.
    week = [d(2010, 8, 1), d(2010, 8, 8), d(2010, 8, 15)]
    wed = [(d(2010, 7, 4), True), (d(2010, 8, 1), False), (d(2010, 8, 4), True), (d(2010, 8, 8), False),
           (d(2010, 8, 11), True), (d(2010, 8, 15), False)]
    checks += [
        (template_faults("weeks", week, wed, 6, 13), False),
        (template_faults("weeks limit", week, wed, 6), False),
        (template_faults("weeks far", week, [(d(2010, 4, 25), True)] + wed[1:], 6, 13), True),
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
    # Slice 9: the Italian rows of the runs.
    for file, season, want in (("ita_ser_a", 2005, (20, 2)), ("ita_ser_b", 2003, (24, 2)), ("ita_ser_b", 2018, (19, 2)),
                               ("ita_ser_c1a", 2004, (19, 2)), ("ita_ser_c1a", 2012, (17, 2)),
                               ("ita_ser_c1b", 2012, (16, 2)), ("ita_ser_a", 2003, (18, 2))):
        if edition_row(file, season) != want:
            bad.append(("encoded", file, season, edition_row(file, season), want))
    import rules_report_check as rrc
    if any(f not in ROWS or x not in rrc.EXTRA for f, xs in EXTRA.items() for x in xs):
        bad.append(("extra", EXTRA))
    if (encoded_split("nir_prm", 2010) != (33, [(6, 1), (6, 1)]) or
            encoded_split("wel_first", 2026) != (30, [(6, 1), (4, 1), (6, 1)]) or
            encoded_split("wel_first", 2012) != (22, [(6, 2), (6, 2)]) or encoded_split("wel_first", 2005) is not None or
            split_numbers([(6, 1), (4, 1), (6, 1)]) != (5, 3, 5) or split_numbers([(6, 2), (6, 2)]) != (10, 10, 10)):
        bad.append(("split", encoded_split("nir_prm", 2010), encoded_split("wel_first", 2026)))
    sl = splits("recomp: league sizes: wel_first split 2026: after 30, sections 6+4+6, meetings 1+1+1, rounds 5+3+5, "
                "5 dates, points kept (Welsh Premier)\nrecomp: league sizes: wel_first split 2026: loaded after the "
                "split (Welsh Premier)", "wel_first")
    if sl != ({2026: (30, [6, 4, 6], [1, 1, 1])}, {2026}):
        bad.append(("splits", sl))
    good_end = [(0, 35), (2, 35), (1, 35), (5, 35), (4, 35), (3, 35), (6, 33), (7, 33), (9, 33), (8, 33), (10, 35),
                (11, 35), (12, 35), (13, 35), (15, 35), (14, 35)]
    sp16 = [(6, 1), (4, 1), (6, 1)]
    mixed = good_end[:5] + [(6, 33), (3, 35)] + good_end[7:]
    short = good_end[:6] + [(6, 35)] + good_end[7:]
    if (end_faults("good", good_end, 30, sp16) or not end_faults("mixed", mixed, 30, sp16) or
            not end_faults("short", short, 30, sp16) or
            end_faults("again", good_end[:7] + [(6, 33)] + good_end[8:], 30, sp16) or
            not end_faults("crossed", good_end[:5] + [(6, 35)] + good_end[6:], 30, sp16) or
            split_ends("recomp: league sizes: wel_first split end 2026: 0:35 1:33", "wel_first") != {2026: [(0, 35), (1, 33)]}):
        bad.append(("end", end_faults("good", good_end, 30, sp16)))
    if encoded_row("bra_nat_first", 2002) != (24, 2) or encoded_row("bra_nat_second", 2004) != (22, 1):
        bad.append(("encoded bra", encoded_row("bra_nat_first", 2002), encoded_row("bra_nat_second", 2004)))
    off = ("recomp: league sizes: bra_nat_first 2011: 2 meetings, no play-off\n"
           "recomp: league sizes: bra_nat_first 2011: champion club 1234 (Série A)")
    if no_playoff_faults(off, "bra_nat_first", 2) or len(no_playoff_faults("", "bra_nat_first", 2)) != 2 or \
            no_playoff_faults("", "bra_nat_first", 1) or no_playoff_faults("", "nor_prm", 2):
        bad.append(("no play-off", no_playoff_faults(off, "bra_nat_first", 2)))
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
        extra = EXTRA.get(file, []) + [x for x in rrc.arg(argv, "--extra", "").split(",") if x]
        if "--save-in-split" in argv:
            code = save_in_split(program, game, year, last, db, nations, work, timeout)
        else:
            code, _report = rrc.run(program, game, last, year, db, nations, extra, work, timeout,
                                    {"changing_rules": "on"}, {"RECOMP_SIZE_LOG": "1"}, stop_on_box=True)
        if code != 0:
            print(f"FAIL {file} {year}: the run ended with code {code} ({work})")
            return 1
    fails = faults(work, file, year, last)
    if "--save-in-split" in argv:
        fails += save_faults(work, file)
    for f in fails:
        print("FAIL", f)
    print(("FAIL" if fails else "PASS") + f" {file} {year} to {last} ({work})")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
