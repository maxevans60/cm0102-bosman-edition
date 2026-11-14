#!/usr/bin/env python3
"""A league of the size engine plays its real size for one season (0.7.2 league sizes, slice 2; spec
docs/specs/2026-11-13-league-sizes-0.7.2-spec.md, Testing seam 2, D4 and D5). Made from swe_prm_run.py (issue 106).

Usage:
    python3 tests/seasons/league_size_run.py <cm0102_recomp> <game folder> <file> [--year <season>]
        [--last <season> | --to <season>] [--keep <folder>] [--reuse] [--timeout <seconds>]
    python3 tests/seasons/league_size_run.py --list
    python3 tests/seasons/league_size_run.py --self-test

The run starts a new game in a season (default: the year of the file in ROWS) with all nations, changing rules on,
and plays to the end of --last (default the start year: RECOMP_SEASON_JUMP, the rules report of
tests/seasons/rules_report_check.py). The row of the file and season in the encoded rules table
(docs/research/2026-10-18-league-rules-encoded.tsv, slice 1) gives the clubs n and the meetings m; the rounds are
R = m x (n' - 1), n' the count rounded up to even. RECOMP_SIZE_LOG=1 (src/recomp_rt/win/season_sizes.c) writes the
lines that the checks read. The checks:
    report   the row of the file in the start report holds n clubs and m meetings (the first season only: a
             calendar-year league holds edition Y + 1 in later reports);
    table    a table of n rows when n is more than the class allocates (league_sizes.py HELD);
    dates    a template of R dates for n clubs: the dates ascend, each new date ("+") is a Wednesday or an earlier
             start in steps of 7 days, none is in the largest gap of the class's template (the winter break) or in a
             gap over 14 days, the last date is the class's last date, and with fewer rounds than the class's
             (D5) no date is new and the first date is the class's first;
    played   at the start of the next edition (League::free_table 0x687970) every club of a table of n clubs played
             m x (n - 1) matches, the matches per club (+0xa7);
    expect   the rules report check lists no clubs difference of the file (the first season);
    box      no message box.
A run takes about 20 to 40 minutes a season. A two-year league ends its table after the end report of its season:
use --last <year + 1> for its played check. --reuse checks the logs in --keep again. Exit code 1 on a failure.
"""
import datetime
import os
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
    "por_second": (2014, 2015, "Segunda Liga, 24 clubs (the class holds 18): a template of 46 dates"),
    "por_second@2008": (2008, 2009, "Segunda Liga, 16 clubs: fewer dates, the season ends on its usual day (D5)"),
    "swe_prm": (2025, 2026, "Allsvenskan, 16 clubs (issue 106, also swe_prm_run.py)"),
    # Slice 17 (D11): the class plays one meeting on 27 dates and a play-off of the top 4. Rows are keyed one year
    # before the real season (row 2003 is the 2004 Série A).
    "bra_nat_first@2003": (2003, 2004, "Série A, 24 clubs in two meetings: 46 dates, no play-off"),
    "bra_nat_first": (2010, 2011, "Série A, 20 clubs in two meetings: 38 dates, no play-off"),
    "bra_nat_second": (2006, 2007, "Série B, 20 clubs in two meetings: 38 dates, no play-off"),
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
    import rules_report_check as rrc
    report = os.path.join(work, "report.tsv")
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    fails = [f"{x.strip()}" for x in log.split("\n") if "recomp: message box from" in x]
    clubs, meetings = encoded_row(file, year)
    m = meetings or 2
    rounds = rounds_of(clubs, m)
    if clubs < 2:
        return fails + [f"{file} {year}: no row in {os.path.basename(ENCODED)}"]
    start = None
    with open(report, encoding="latin-1") as f:
        cols = None
        for line in f:
            p = line.rstrip("\n").split("\t")
            if p[0] == "columns":
                cols = p[1:]
            elif cols and p[0] == "comp" and len(p) == len(cols) + 1:
                row = dict(zip(cols, p[1:]))
                if row["file"] == file and row["season"] == str(year) and row["phase"] == "start":
                    start = row
    if not start:
        fails.append(f"{file} {year}: no row in the start report")
    elif (int(start["clubs"]), int(start["rounds"])) != (clubs, m):
        fails.append(f"{file} {year}: start report clubs and meetings {(int(start['clubs']), int(start['rounds']))}, "
                     f"want {(clubs, m)}")
    held = league_sizes.HELD.get(file, 0)
    if clubs > held and not any(f"{file} table of {clubs} rows" in x for x in log.split("\n")):
        fails.append(f"{file} {year}: no log line of a table of {clubs} rows (the class allocates {held})")
    fails += [f"{file}: {x.split(': ', 1)[1]}" for x in log.split("\n")
              if x.startswith(f"recomp: league sizes: {file} template mode ")]
    fails += [f"{file}: {x.split(': ', 1)[1]}" for x in log.split("\n")
              if x.startswith(f"recomp: league sizes: {file} dates: no room")]
    tmpl = templates(log, file)
    want = [t for t in tmpl if len(t[1]) != rounds]
    if not tmpl:
        fails.append(f"{file} {year}: no template line")
    elif not want:
        fails.append(f"{file} {year}: every template has the class's {len(tmpl[0][1])} dates, want {rounds}")
    for base, cls, new in want:
        if new is None:
            fails.append(f"{file} template {base}: the class's {len(cls)} dates stay, want {rounds}")
        else:
            fails += template_faults(f"{file} template {base}", cls, new, rounds,
                                     EARLIER_BY_FILE.get(file, EARLIER))
    done = [p for p in played(log, file) if p[1] == clubs]
    if not done:
        fails.append(f"{file}: no finished table of {clubs} clubs in the run (a two-year league needs --last "
                     f"{year + 1})")
    for ed, n, matches, lo, hi in done:
        if not (matches == lo == hi == m * (n - 1)):
            fails.append(f"{file} {ed}: {n} clubs, {matches} matches, played {lo} to {hi}, want {m * (n - 1)} each")
    fails += no_playoff_faults(log, file, m)
    if os.path.exists(report):
        rep_clubs = []
        seasons, comps = rrc.read_report(report, rep_clubs)
        diffs = (set(rrc.differences(seasons, comps, rrc.make_rules.read_rows()))
                 | rrc.division_differences(comps, rep_clubs))
        diffs = sorted(d for d in diffs if d[1] in ("clubs", "division_clubs") and file in d[2] and
                       str(year) in str(d[0]))
        fails += [f"{file} {year}: rules report difference: {d[2]}" for d in diffs]
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
    if encoded_row("nor_prm", 2009) != (16, 2) or encoded_row("por_second", 2014) != (24, 2):
        bad.append(("encoded", encoded_row("nor_prm", 2009), encoded_row("por_second", 2014)))
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
        code, _report = rrc.run(program, game, last, year, db, ["all"], [], work, timeout,
                                {"changing_rules": "on"}, {"RECOMP_SIZE_LOG": "1"})
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
