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
    # Open item 250: the Welsh Premier has no chain (WelFirst::start_new_season 0x925cf0); its row's relegation places go
    # to no division and the step brings it to the count of its next row.
    "wel_first@2009": (2009, 2010, "Welsh Premier, 18 clubs, then 12 with a split: 6 down to no division"),
    "wel_first@2025": (2025, 2026, "Welsh Premier, 12 clubs with a split, then 16: 2 down, 6 up from no division"),
    # Slice 17 (D11): the class plays one meeting on 27 dates and a play-off of the top 4. Rows are keyed one year
    # before the real season (row 2003 is the 2004 Série A).
    "bra_nat_first@2003": (2003, 2004, "Série A, 24 clubs in two meetings: 46 dates, no play-off"),
    "bra_nat_first": (2010, 2011, "Série A, 20 clubs in two meetings: 38 dates, no play-off"),
    "bra_nat_second": (2006, 2007, "Série B, 20 clubs in two meetings: 38 dates, no play-off"),
    # Slice 14 (D8): calendar-year leagues, edition Y plays row Y - 1. J1 plays halves and the final only in the rows
    # of two stages; its table holds both halves (season_sizes_asia.c).
    "jap_j1": (2005, 2006, "J1, 18 clubs in one table: 34 dates, no halves and no final"),
    "jap_j1@2015": (2015, 2017, "J1, 18 clubs in two halves of 17 rounds and the final, then one table in 2017"),
    "jap_j1@2024": (2024, 2025, "J1, 20 clubs in one table: 38 dates"),
    "jap_j2": (2010, 2011, "J2, 19 clubs in two meetings: 38 dates (the class plays 4 meetings of 12)"),
    "kor_league": (2013, 2014, "K League, 14 clubs: 26 rounds, then sections of 7 and 7 that meet twice"),
    "kor_league@2003": (2003, 2004, "K League, 12 clubs in four meetings: 44 dates, 17 weeks earlier at most, "
                                    "then two stages as one table in two meetings, 12 of the row's 13 (no club to add, D12)"),
    # Gap 45: row 2025 (2026) splits after 33 into 6 and 6 that meet once more, 38 in all (2026 K League 1 page).
    # Gap 47: the K League alone sends its row's places to no division (edition 2024 plays row 2023: 1 down), and the
    # step takes one Korean club with no division back up to 12.
    "kor_league@2024": (2024, 2025, "K League 1, 12 clubs: 1 down to no division, 1 up from no division"),
    "kor_league@2026": (2026, 2026, "K League 1, 12 clubs: 33 rounds, then sections of 6 and 6 that meet once more"),
    # Slice 13: the NIFL Championship (the class holds 10 in 4 meetings on 36 dates) and the League of Ireland First
    # Division (the class takes 9 to 12; rows are keyed one year before the real season, row 2013 is the 2014 season).
    "nir_first": (2016, 2017, "NIFL Championship, 12 clubs: 22 rounds, then sections of 6 and 6 that meet twice"),
    "nir_first@2022": (2022, 2023, "NIFL Championship, 12 clubs: 33 rounds, then sections of 6 and 6 once more"),
    "nir_first@2020": (2020, 2021, "NIFL Championship, 12 clubs: the announced 22 rounds of 2020/21, no split (gap 42)"),
    "ire_first": (2013, 2014, "League of Ireland First Division, 8 clubs in four meetings: 28 dates (D5)"),
    "ire_first@2017": (2017, 2018, "League of Ireland 2017: 3 down, the champion up, no play-off; 8 to 10 (item 251)"),
    "ire_first@2008": (2008, 2009, "League of Ireland 2008: 3 down, the champion up, no play-off; 10 to 12, Premier "
                       "12 to 10 (item 251)"),
    # Slice 15: the Argentine Primera. Edition Y plays the row whose real season begins with Y, else row Y - 1
    # (season_sizes_edition): editions 2014 and 2015 play row 2014, 2016 row 2016, 2025 row 2024 (the 2025 season).
    "arg_prm@2013": (2013, 2014, "Primera 2013/14 (row 2013, 20 in two halves), then 30 in 2014/15: the size step "
                                 "takes 10 clubs of the second division, which takes clubs of the third"),
    "arg_prm": (2014, 2015, "Primera 2014/15 (row 2014): one table of 30, 29 rounds and the derby round"),
    "arg_prm@2016": (2016, 2017, "Primera 2016/17 (row 2016): one table of 30 and the derby round, then 28 (the step)"),
    "arg_prm@2025": (2025, 2026, "Primera row 2024 (the 2025 season): Apertura and Clausura of two zones of 15 with "
                                 "the rival and drawn inter-zone rounds"),
    # Slice 17 (D11): the class plays one meeting on 27 dates and a play-off of the top 4. Rows are keyed one year
    # before the real season (row 2003 is the 2004 Série A).
    "bra_nat_first@2003": (2003, 2004, "Série A, 24 clubs in two meetings: 46 dates, no play-off"),
    "bra_nat_first": (2010, 2011, "Série A, 20 clubs in two meetings: 38 dates, no play-off"),
    "bra_nat_second": (2006, 2007, "Série B, 20 clubs in two meetings: 38 dates, no play-off"),
    # Gap rows 56 and 57: the Série B play-offs of the rules table (season_sizes_bra.c).
    "bra_nat_second@2003": (2003, 2004, "Série B 2003 and 2004, 24 clubs: a play-off of the top 8, the two finalists "
                                        "go up"),
    "bra_nat_second@2025": (2025, 2026, "Série B 2025 (4 up), then 2026: 2 up and the ties 3rd v 6th and 4th v 5th, "
                                        "both winners go up"),
}

# Files whose table holds both halves of a row of halves, so it plays twice the row's meetings (slice 14,
# src/recomp_rt/win/season_sizes_asia.c): J1, and the K League stages as one table.
HALVES_IN_TABLE = {"jap_j1", "kor_league"}

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
                m = int(row["meetings"]) if row.get("meetings", "").isdigit() else 0
                if file in HALVES_IN_TABLE and row.get("halves") == "1":
                    m *= 2
                best = (s, int(row["clubs"]), m)
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


# Earlier starts of 7 days a class may take (season_template.h SEASON_TEMPLATE_EARLIER, season_sizes_bra.c BRA_EARLIER,
# season_sizes_asia_rows.c KOR_EARLIER).
EARLIER = 4
EARLIER_BY_FILE = {"bra_nat_first": 13, "bra_nat_second": 13, "kor_league": 17}


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


def bra_playoff(file, ed):
    """Gap rows 56 and 57: the Série B play-off of edition ed (row ed - 1): "top8" for rows 2001 to 2004 (the two
    finalists of the top 8 go up), "ties" from row 2025 (3rd v 6th and 4th v 5th, both winners go up), else None."""
    if file != "bra_nat_second":
        return None
    row = ed - 1
    return "top8" if 2001 <= row <= 2004 else "ties" if row >= 2025 else None


def playoff_faults(log, file, editions):
    """Each finished edition with a Série B play-off of the rules table: the play-off was drawn on its places, and two
    clubs went up from it (season_sizes_bra.c log lines)."""
    lines = log.split("\n")
    fails = []
    for ed in sorted(editions):
        kind = bra_playoff(file, ed)
        if not kind:
            continue
        head = f"recomp: league sizes: {file} {ed}: "
        drawn = head + "play-off of places " + ("1 to 8" if kind == "top8" else "3 to 6") + " drawn"
        if not any(x.startswith(drawn) for x in lines):
            fails.append(f"{file} {ed}: no play-off of places {'1 to 8' if kind == 'top8' else '3 to 6'}")
        ups = [x for x in lines if x.startswith(head) and " goes up" in x and "no play-off" not in x]
        if len(ups) != 2:
            fails.append(f"{file} {ed}: {len(ups)} clubs went up from the play-off, want 2")
    return fails


_DOWN = {}


def row_down(file, season):
    """The relegation places of the rules row of a file and season (the leading number of its down text), or -1."""
    if not _DOWN:
        sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
        import make_rules
        for r in make_rules.read_rows():
            if r.get("comp_file") and str(r.get("season", ""))[:4].isdigit():
                m = re.match(r"\s*(\d+)", str(r.get("down", "")))
                _DOWN[(r["comp_file"], int(str(r["season"])[:4]))] = int(m.group(1)) if m else -1
    return _DOWN.get((file, season), -1)


# A division with no chain (open item 250, season_sizes.c lone_exchange): its row's relegation places go to no
# division at its season change.
# Gap 47: the K League too (no K League 2 in the game). A division alone sends down only as many clubs as its nation's
# clubs with no division can replace (season_size_lone_down), and the log says so when it sends fewer.
LONE = {"wel_first", "kor_league"}


def lone_downs(log, file):
    """{edition: clubs} of the lines of a file's clubs that went down to no division."""
    out = {}
    for m in re.finditer(r"recomp: league sizes: %s \(.*?\) (\d+): .* goes down to no division" % re.escape(file), log):
        out[int(m.group(1))] = out.get(int(m.group(1)), 0) + 1
    return out


def lone_places(log, file):
    """{edition: relegation places} that sizes_rules wrote for a lone file (the last line of each edition). The class
    writes none (0), so an edition with no line shows 0."""
    out = {}
    for m in re.finditer(r"recomp: league sizes: %s \(.*?\) (\d+): relegation places (\d+) to no division"
                         % re.escape(file), log):
        out[int(m.group(1))] = int(m.group(2))
    return out


# Test switch RECOMP_TEST_LONE_HUMAN=1 (season_sizes.c lone_exchange): the worst club of a lone exchange counts as a
# club with a human manager. It stays, the next worst club goes in its place, and the news says so.
def lone_human_faults(log, file, year, last):
    fails = []
    stays = {}
    for m in re.finditer(r"recomp: league sizes: %s \(.*?\) (\d+): (.+?) stays, its manager is a player: (.+?) goes "
                         r"down in its place" % re.escape(file), log):
        stays.setdefault(int(m.group(1)), []).append((m.group(2), m.group(3)))
    downs = {}
    for m in re.finditer(r"recomp: league sizes: %s \(.*?\) (\d+): (.+?) goes down to no division" % re.escape(file),
                         log):
        downs.setdefault(int(m.group(1)), []).append(m.group(2))
    for ed in range(year, last):
        if not downs.get(ed):
            continue
        if not stays.get(ed):
            fails.append(f"{file} {ed}: no club stayed with the test switch RECOMP_TEST_LONE_HUMAN")
            continue
        for kept, went in stays[ed]:
            if kept in downs[ed] or went not in downs[ed]:
                fails.append(f"{file} {ed}: {kept} stays and {went} goes, but the clubs down are {downs[ed]}")
            if not re.search(r"recomp: season change: news \d+/\d+: %s stays in the " % re.escape(kept), log):
                fails.append(f"{file} {ed}: no news line that {kept} stays")
    return fails


def lone_guards(log, file):
    """{edition: clubs that go down} of the guard lines of a file (fewer than its relegation places). The count is
    worked out again from the line's places, pool and counts (season_size_lone_down), so a wrong guard fails: -1."""
    out = {}
    for m in re.finditer(r"recomp: league sizes: %s \(.*?\) (\d+): relegation places (\d+), (\d+) go down: "
                         r"(\d+) clubs with no division can come up, (\d+) clubs to (-?\d+)" % re.escape(file), log):
        places, down, pool, have, want = (int(m.group(k)) for k in range(2, 7))
        most = 0 if want < 0 else max(0, pool + have - want)
        out[int(m.group(1))] = down if down == min(places, most) else -1
    return out


def split_bounds(log, file):
    """{edition: [[(best, worst)] in table order]} of the split bounds lines of a file (season_split.c rt_split_bounds:
    the best and worst places that each row can still reach after the split)."""
    out = {}
    for m in re.finditer(r"recomp: league sizes: %s split bounds (\d+) \d+:((?: \d+-\d+)+)" % re.escape(file), log):
        out.setdefault(int(m.group(1)), []).append([tuple(int(x) for x in w.split("-")) for w in m.group(2).split()])
    return out


def bounds_faults(name, lines, sizes):
    """The faults of the bounds lines of a split: each row's best and worst places lie in the places of its section
    (the rows are in table order, so the sections are the runs of the sizes)."""
    fails = []
    for rows in lines:
        if len(rows) != sum(sizes):
            fails.append(f"{name}: bounds of {len(rows)} rows, want {sum(sizes)}")
            continue
        top = 0
        for size in sizes:
            for i in range(top, top + size):
                best, worst = rows[i]
                if not top + 1 <= best <= worst <= top + size:
                    fails.append(f"{name}: row {i + 1} may reach places {best} to {worst}, its section holds "
                                 f"{top + 1} to {top + size}")
            top += size
    return fails


def encoded_halves(file, season):
    """1 when the encoded row of a file in a season (the after-table rule) plays two halves."""
    best = None
    with open(ENCODED, encoding="utf-8") as f:
        cols = f.readline().rstrip("\n").split("\t")
        for line in f:
            row = dict(zip(cols, line.rstrip("\n").split("\t")))
            if row.get("comp_file") != file or not row.get("season", "").isdigit():
                continue
            s = int(row["season"])
            if s <= season and row.get("clubs", "").isdigit() and (best is None or s > best[0]):
                best = (s, row.get("halves") == "1")
    return bool(best and best[1])


def j1_faults(log, file, editions):
    """J1 (slice 14, D8): an edition of a row of halves plays the halves of its count and their final or a double
    winner; an edition of one table plays no half and its top club is the champion."""
    if file != "jap_j1":
        return []
    lines = log.split("\n")
    fails = []
    for ed in sorted(editions):
        tag = f"recomp: league sizes: jap_j1 {ed}: "
        mine = [x[len(tag):] for x in lines if x.startswith(tag)]
        clubs, m = edition_row(file, ed)
        if encoded_halves(file, ed - 1 if calendar_year(file) else ed):
            if not any(x.startswith("halves and the final") for x in mine):
                fails.append(f"jap_j1 {ed}: no log line of a season of halves")
            if any(x.startswith("one table") for x in mine):
                fails.append(f"jap_j1 {ed}: a season of halves played one table")
            half = m * (clubs - 1) // 2
            if half != 15 and not any(x.startswith(f"half of {clubs} clubs, {half} matches") for x in mine):
                fails.append(f"jap_j1 {ed}: no half of {clubs} clubs with {half} matches a club")
            rounds = rounds_of(clubs, m)
            bound = [x for x in mine if x.startswith("second half from round ")]
            if not bound or any(not x.startswith(f"second half from round {rounds // 2 + 1} of {rounds} ") for x in bound):
                fails.append(f"jap_j1 {ed}: second half lines {bound}, want from round {rounds // 2 + 1} of {rounds}")
            if not any(x == "final of the halves" or x == "double winner, no final" for x in mine):
                fails.append(f"jap_j1 {ed}: neither a final of the halves nor a double winner")
        else:
            if not any(x.startswith("one table, no halves and no final") for x in mine):
                fails.append(f"jap_j1 {ed}: no log line of a season of one table")
            if any(x.startswith(("half of", "final of the halves", "second half from", "double winner")) for x in mine):
                fails.append(f"jap_j1 {ed}: a season of one table made a half or a final")
            if not any(x.startswith("champion club ") for x in mine):
                fails.append(f"jap_j1 {ed}: no champion of the table in the log")
    return fails


def shortages(log, file):
    """{edition: clubs} of the editions that the size step left short with a reason (D12: when the database holds
    too few clubs, the division plays with the clubs it has, and the log names it)."""
    out = {}
    pat = re.compile(rf"recomp: league sizes: step: {re.escape(file)} \(.*\) (\d+): (\d+) clubs, the row (\d+): "
                     r"no club of its nation with no division")
    for line in log.split("\n"):
        m = pat.match(line)
        if m and int(m.group(2)) < int(m.group(3)):
            out[int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
    return out
# Gap 43: an exchange whose real places keep both counts. The lower file: (the upper file, the least clubs that move
# each way). League of Ireland 2011 to 2015 (rows wal-nir-irl-266 to 367): the 12th of the Premier Division goes down,
# the First Division champion goes up, and the 11th meets the winner of the First Division play-off, so 1 or 2 clubs
# move each way and the counts stay 12 and 8.
EXCHANGE = {"ire_first": ("ire_prm", 1, 2011, 2014)}


def exchange_faults(log, file, year):
    """The pair lines of the promotion wrap (season_sizes.c, league_promote_and_relegate 0x689c80) between the upper
    file and this one: as many clubs go down as come up, and at least the least count. Only for a run that starts in
    the seasons of EXCHANGE: in a season of a real size change (edition 2017, 12 to 10) the counts differ."""
    if file not in EXCHANGE or not EXCHANGE[file][2] <= year <= EXCHANGE[file][3]:
        return []
    upper, least = EXCHANGE[file][:2]
    pat = re.compile(rf"recomp: league sizes: {upper} \(.*\) down (-?\d+), {file} \(.*\) up (-?\d+), pairs")
    moves = [(int(m.group(1)), int(m.group(2))) for m in map(pat.match, log.split("\n")) if m]
    if not moves:
        return [f"{file}: no exchange line with {upper}"]
    return [f"{file}: {upper} sends {d} down and {file} {u} up, want the same count of {least} or more"
            for d, u in moves if d != u or d < least]


# Open item 251: the League of Ireland seasons whose rows have no play-off between the divisions and change both
# counts (rows wal-nir-irl-66/67, 166/167, 391/392). An edition is a calendar year and plays the row of its real
# season (edition_season), so the key is the edition: (Premier direct places down, First Division places up). Real
# life: 2004 1 down and 3 up (10 to 12, 12 to 10), 2008 and 2017 the 10th to 12th down and the champion up (12 to 10;
# 10 to 12 and 8 to 10), no play-off. The counts of the next edition are the report's check.
IRL_CHANGE = {2004: (1, 3), 2008: (3, 1), 2017: (3, 1)}


def irl_partner_faults(report, file, year):
    """The other Irish division in the report: each edition of the run holds the clubs of its row (edition_row)."""
    other = {"ire_prm": "ire_first", "ire_first": "ire_prm"}.get(file)
    if not other or not os.path.exists(report):
        return []
    seen = {(ed, n) for _s, _p, ed, n, _m, _x in report_rows(report, other)}
    return [f"{other} {ed}: {n} clubs, want {edition_row(other, ed)[0]}" for ed, n in sorted(seen)
            if year <= ed and edition_row(other, ed)[0] >= 2 and n != edition_row(other, ed)[0]]


def irl_change_faults(log, file, year):
    """In a run of ire_prm or ire_first: the play-off line of the First Division (season_sizes_irl.c) for exactly the
    editions of IRL_CHANGE among the start edition and the next; and when the run starts in one of them, the exchange
    moves the row's places with each club on its own."""
    if file not in ("ire_prm", "ire_first"):
        return []
    lines = log.split("\n")
    nopo = {int(m.group(1)) for m in (re.match(r"recomp: league sizes: ire_first (\d+): no play-off with the Premier "
                                               r"Division$", x) for x in lines) if m}
    fails = [f"ire_first {ed}: a line of no play-off, the row has one" for ed in sorted(nopo)
             if ed in (year, year + 1) and ed not in IRL_CHANGE]
    if year not in IRL_CHANGE:
        return fails
    if year not in nopo:
        fails.append(f"ire_first {year}: no line of a season with no play-off with the Premier Division")
    pat = re.compile(r"recomp: league sizes: ire_prm \(.*\) down (-?\d+), ire_first \(.*\) up (-?\d+), pairs \d+, .*"
                     r": each club moves on its own$")
    moves = [(int(m.group(1)), int(m.group(2))) for m in map(pat.match, lines) if m]
    if IRL_CHANGE[year] not in moves:
        fails.append(f"ire_prm {year}: exchanges {moves}, want {IRL_CHANGE[year][0]} down and {IRL_CHANGE[year][1]} up "
                     "with each club on its own")
    return fails


def faults(work, file, year, last):
    """The faults of a run, edition by edition: each edition that the report, the template lines and the played
    lines show is checked against its own row (edition_row)."""
    import rules_report_check as rrc
    report = os.path.join(work, "report.tsv")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    lines = log.split("\n")
    fails = [f"{x.strip()}" for x in lines if "recomp: message box from" in x]
    short = shortages(log, file)

    def row(ed):
        clubs, m = edition_row(file, ed)
        return (short[ed][0], m) if ed in short else (clubs, m)

    def counts(ed):
        """(rounds, matches of a club) of an edition. The J1 and K League rows of halves or stages are one table of
        both (HALVES_IN_TABLE: row() doubles the meetings) and may hold fewer clubs than the row (D12), so they count
        from row(); the rest from edition_counts (zones and derby rounds)."""
        if file not in HALVES_IN_TABLE:
            return edition_counts(file, ed)
        clubs, m = row(ed)
        return (rounds_of(clubs, m or 2), (m or 2) * (clubs - 1))
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
        clubs, m = row(ed)
        m = m or meet
        if (n, meet) != (clubs, m):
            fails.append(f"{file}: report {season} {phase}, edition {ed}: clubs and meetings {(n, meet)}, want "
                         f"{(clubs, m)}")
        else:
            sp = edition_split(file, ed)
            want = counts(ed)[1] + (split_numbers(sp[1])[2] if sp else 0)
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
        clubs, m = row(base)
        sp = edition_split(file, base)
        rounds = counts(base)[0] + (split_numbers(sp[1])[0] if sp else 0)
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
        clubs, m = row(ed)
        m = m or 2
        sp = edition_split(file, ed)
        if not sp:
            want = counts(ed)[1]
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
        bounds = split_bounds(log, file)
        if ed not in bounds:
            fails.append(f"{file} {ed}: no split bounds line")
        else:
            fails += bounds_faults(f"{file} {ed}", bounds[ed], [x for x, _m in sp[1]])
    if file == "arg_prm":
        fails += arg_faults(log, year)
    if file in LONE:
        downs, guards = lone_downs(log, file), lone_guards(log, file)
        for ed in range(year, last):
            want = guards.get(ed, row_down(file, edition_season(file, ed)))
            if want >= 0 and downs.get(ed, 0) != want:
                fails.append(f"{file} {ed}: {downs.get(ed, 0)} clubs down to no division, want {want}")
            # Lead decision of 15 November: the table shows only the places that the exchange uses.
            shown = lone_places(log, file).get(ed, 0)
            if want >= 0 and shown != downs.get(ed, 0):
                fails.append(f"{file} {ed}: the table shows {shown} relegation places, {downs.get(ed, 0)} go down")
        if os.environ.get("RECOMP_TEST_LONE_HUMAN") == "1":
            fails += lone_human_faults(log, file, year, last)
    fails += no_playoff_faults(log, file, edition_row(file, year)[1] or 2)
    fails += playoff_faults(log, file, {ed for ed, *_x in done})
    fails += j1_faults(log, file, {ed for ed, *_x in done})
    fails += exchange_faults(log, file, year)
    fails += irl_change_faults(log, file, year)
    if year in IRL_CHANGE:
        fails += irl_partner_faults(report, file, year)
    if os.path.exists(report):
        rep_clubs = []
        seasons, comps = rrc.read_report(report, rep_clubs)
        diffs = (set(rrc.differences(seasons, comps, rrc.make_rules.read_rows()))
                 | rrc.division_differences(comps, rep_clubs))
        diffs = sorted(d for d in diffs if d[1] in ("clubs", "division_clubs") and file in d[2])
        # The report check reads the row of the game season; an Irish edition is a calendar year and plays the row of
        # its real season (edition 2017 row 2016), so where the two rows differ the edition checks above decide.
        diffs = [d for d in diffs if not (file in ("ire_prm", "ire_first") and edition_season(file, d[0]) != d[0])]
        # A short edition (shortages) differs from its row by the clubs the database lacks, and the log names it.
        # The report's season of a calendar-year league is the edition before (edition_row).
        def named(d):
            ed = d[0] + (1 if calendar_year(file) else 0)
            return ed in short and f"game {short[ed][0]}, table {short[ed][1]} " in d[2] + " "
        fails += [f"{file} {d[0]}: rules report difference: {d[2]}" for d in diffs if not named(d)]
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
    sh = shortages("recomp: league sizes: step: kor_league (K League) 2004: 12 clubs, the row 13: no club of its "
                   "nation with no division or in a spare division\nrecomp: league sizes: step: kor_league (K League) "
                   "2005: 12 clubs, the row 13\nrecomp: league sizes: step: jap_j2 (J2) 2006: 12 clubs, the row 13: no "
                   "club of its nation with no division or in a spare division\nrecomp: league sizes: step: kor_league (K League) "
                   "2006: 12 clubs, the row 13: the division below has no club to give", "kor_league")
    if sh != {2004: (12, 13)}:
        bad.append(("shortages", sh))
    # Odd sections (the K League 2013 log): a place written again inside its own section passes, one across fails.
    sp7 = [(7, 2), (7, 2)]
    kor_end = [(p, 38) for p in (0, 2, 3, 2, 4, 5, 5, 8, 9, 11, 10, 9, 12, 13)]
    across = [(p, 38) for p in (0, 2, 3, 2, 4, 5, 8, 8, 9, 11, 10, 9, 12, 13)]
    if end_faults("kor", kor_end, 26, sp7) or not end_faults("across", across, 26, sp7):
        bad.append(("end odd", end_faults("kor", kor_end, 26, sp7), end_faults("across", across, 26, sp7)))
    ex = "recomp: league sizes: ire_prm (League of Ireland Premier Division) down %d, ire_first (League of Ireland " \
         "First Division) up %d, pairs 0, expected -1 up -1 down%s"
    if (exchange_faults(ex % (2, 2, ""), "ire_first", 2013) or exchange_faults(ex % (1, 1, ""), "ire_first", 2013) or
            not exchange_faults(ex % (1, 2, ": in pairs"), "ire_first", 2013) or
            not exchange_faults(ex % (0, 1, ": in pairs"), "ire_first", 2013) or
            not exchange_faults(ex % (0, 0, ""), "ire_first", 2013) or not exchange_faults("", "ire_first", 2013) or
            exchange_faults("", "nir_first", 2013) or exchange_faults(ex % (3, 1, ": in pairs"), "ire_first", 2016)):
        bad.append(("exchange", exchange_faults(ex % (1, 2, ": in pairs"), "ire_first", 2013)))
    own = ex % (3, 1, ": each club moves on its own")
    nopo = "recomp: league sizes: ire_first 2017: no play-off with the Premier Division"
    if (irl_change_faults(own + "\n" + nopo, "ire_first", 2017) or irl_change_faults(own + "\n" + nopo, "ire_prm", 2017)
            or len(irl_change_faults("", "ire_prm", 2017)) != 2
            or len(irl_change_faults(ex % (1, 1, ": in pairs") + "\n" + nopo, "ire_first", 2017)) != 1
            or len(irl_change_faults(own, "ire_first", 2017)) != 1
            or len(irl_change_faults(own + "\n" + nopo, "ire_first", 2008)) != 1
            or len(irl_change_faults(nopo.replace("2017", "2013"), "ire_first", 2013)) != 1
            or len(irl_change_faults(nopo.replace("2017", "2009"), "ire_prm", 2008)) != 3
            or irl_change_faults(nopo.replace("2017", "2004"), "ire_prm", 2003)
            or len(irl_change_faults(nopo.replace("2017", "2003"), "ire_prm", 2003)) != 1
            or irl_change_faults(ex % (1, 1, ""), "ire_first", 2013) or irl_change_faults("", "nir_first", 2017)):
        bad.append(("irl change", irl_change_faults("", "ire_prm", 2017)))
    if encoded_row("bra_nat_first", 2002) != (24, 2) or encoded_row("bra_nat_second", 2004) != (22, 1):
        bad.append(("encoded bra", encoded_row("bra_nat_first", 2002), encoded_row("bra_nat_second", 2004)))
    off = ("recomp: league sizes: bra_nat_first 2011: 2 meetings, no play-off\n"
           "recomp: league sizes: bra_nat_first 2011: champion club 1234 (Série A)")
    if no_playoff_faults(off, "bra_nat_first", 2) or len(no_playoff_faults("", "bra_nat_first", 2)) != 2 or \
            no_playoff_faults("", "bra_nat_first", 1) or no_playoff_faults("", "nor_prm", 2):
        bad.append(("no play-off", no_playoff_faults(off, "bra_nat_first", 2)))
    top8 = ("recomp: league sizes: bra_nat_second 2003: play-off of places 1 to 8 drawn on day 335, the two finalists\n"
            "recomp: league sizes: bra_nat_second 2003: Club A wins the play-off final and goes up\n"
            "recomp: league sizes: bra_nat_second 2003: place 5 (Club B) goes up, the loser of the play-off final")
    ties = ("recomp: league sizes: bra_nat_second 2026: play-off of places 3 to 6 drawn on day 330, 3rd v 6th\n"
            "recomp: league sizes: bra_nat_second 2026: place 3 (Club C) goes up, the winner of its play-off tie")
    late = ("recomp: league sizes: bra_nat_second 2004: 9 days to the season change, no time for the play-off; places "
            "1 and 2 go up\n"
            "recomp: league sizes: bra_nat_second 2004: place 1 (Club D) goes up, no play-off\n"
            "recomp: league sizes: bra_nat_second 2004: place 2 (Club E) goes up, no play-off")
    if (playoff_faults(top8, "bra_nat_second", {2003}) or len(playoff_faults(ties, "bra_nat_second", {2026})) != 1 or
            len(playoff_faults(late, "bra_nat_second", {2004})) != 2 or
            playoff_faults(top8, "bra_nat_second", {2006, 2025}) or playoff_faults("", "bra_nat_first", {2003}) or
            [bra_playoff("bra_nat_second", y) for y in (2001, 2002, 2005, 2006, 2025, 2026, 2030)] !=
            [None, "top8", "top8", None, None, "ties", "ties"]):
        bad.append(("bra play-off", playoff_faults(top8, "bra_nat_second", {2003}),
                    playoff_faults(ties, "bra_nat_second", {2026})))
    bl = split_bounds("recomp: league sizes: wel_first split bounds 2026 120: 1-2 1-3 3-3 4-5 4-5\n", "wel_first")
    if (bl != {2026: [[(1, 2), (1, 3), (3, 3), (4, 5), (4, 5)]]} or bounds_faults("b", bl[2026], [3, 2]) or
            len(bounds_faults("b", [[(1, 2), (1, 4), (3, 3), (4, 5), (3, 5)]], [3, 2])) != 2 or
            not bounds_faults("b", [[(1, 1)]], [3, 2])):
        bad.append(("bounds", bl))
    ld = lone_downs("recomp: league sizes: wel_first (Welsh Premier League) 2009: Club A goes down to no division\n"
                    "recomp: league sizes: wel_first (Welsh Premier League) 2009: Club B goes down to no division\n",
                    "wel_first")
    if ld != {2009: 2} or row_down("wel_first", 2009) != 6 or row_down("wel_first", 2025) != 2:
        bad.append(("lone", ld, row_down("wel_first", 2009), row_down("wel_first", 2025)))
    lg = lone_guards("recomp: league sizes: kor_league (K League) 2012: relegation places 2, 1 go down: 0 clubs with "
                     "no division can come up, 13 clubs to 12\nrecomp: league sizes: kor_league (K League) 2013: "
                     "relegation places 2, 2 go down: 0 clubs with no division can come up, 12 clubs to 12\n", "kor_league")
    lp = lone_places("recomp: league sizes: kor_league (K League 1) 2024: relegation places 1 to no division, the class "
                     "gave 0\nrecomp: league sizes: kor_league (K League 1) 2024: relegation places 0 to no division, "
                     "the class gave 1\n", "kor_league")
    hf = lone_human_faults("recomp: league sizes: kor_league (K League) 2013: A stays, its manager is a player: C goes "
                           "down in its place\nrecomp: league sizes: kor_league (K League) 2013: B goes down to no "
                           "division\nrecomp: league sizes: kor_league (K League) 2013: C goes down to no division\n"
                           "recomp: season change: news 2014/15: A stays in the K League: its manager is a player.\n",
                           "kor_league", 2013, 2014)
    hf2 = lone_human_faults("recomp: league sizes: kor_league (K League) 2013: A goes down to no division\n",
                            "kor_league", 2013, 2014)
    if lp != {2024: 0} or hf or len(hf2) != 1:
        bad.append(("lone places and humans", lp, hf, hf2))
    if (lg != {2012: 1, 2013: -1} or row_down("kor_league", 2012) != 2 or row_down("kor_league", 2023) != 1 or
            row_down("kor_league", 2025) != 0 or edition_season("kor_league", 2024) != 2023):
        bad.append(("lone kor", lg, row_down("kor_league", 2012), row_down("kor_league", 2023),
                    row_down("kor_league", 2025), edition_season("kor_league", 2024)))
    # Slice 14: the table of a row of halves holds both halves (twice the row's meetings).
    if (encoded_row("jap_j1", 2014) != (18, 2) or encoded_row("jap_j1", 2004) != (18, 2) or
            encoded_row("kor_league", 2005) != (14, 2) or encoded_row("jap_j2", 2009) != (19, 2) or
            edition_row("jap_j1", 2015) != (18, 2) or not encoded_halves("jap_j1", 2014) or
            encoded_halves("jap_j1", 2004)):
        bad.append(("encoded asia", encoded_row("jap_j1", 2014), encoded_row("kor_league", 2005)))
    one = ("recomp: league sizes: jap_j1 2005: one table, no halves and no final (row 2004)\n"
           "recomp: league sizes: jap_j1 2005: champion club 1234 (the top of the table)")
    halves = ("recomp: league sizes: jap_j1 2015: halves and the final (row 2014), 3 stages\n"
              "recomp: league sizes: jap_j1 2015: half of 18 clubs, 17 matches a club (the class makes 15)\n"
              "recomp: league sizes: jap_j1 2015: second half from round 18 of 34 (the class's 16)\n"
              "recomp: league sizes: jap_j1 2015: final of the halves")
    if (not j1_faults(halves.replace("round 18", "round 16"), "jap_j1", {2015}) or
            not j1_faults(halves.replace("final of the halves", "final of the halves not made"), "jap_j1", {2015}) or
            j1_faults(halves.replace("final of the halves", "double winner, no final"), "jap_j1", {2015})):
        bad.append(("j1 bound", j1_faults(halves.replace("round 18", "round 16"), "jap_j1", {2015})))
    if (j1_faults(one, "jap_j1", {2005}) or j1_faults(halves, "jap_j1", {2015}) or
            len(j1_faults(halves.replace("2015", "2005"), "jap_j1", {2005})) != 3 or
            len(j1_faults(one.replace("2005", "2015"), "jap_j1", {2015})) != 5 or j1_faults("", "nor_prm", {2009})):
        bad.append(("j1", j1_faults(one, "jap_j1", {2005}), j1_faults(halves, "jap_j1", {2015})))
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
