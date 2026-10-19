#!/usr/bin/env python3
"""Makes the club records table of the game (0.6.0 slice 15b, Q359, Q361): the real top and second tier clubs that
a season database lacks, which the game adds as new club records at a new game in that season.

Usage:
    python3 -I tools/seasons/make_club_records.py --out <club_records.tsv>   write the table for the game
    python3 -I tools/seasons/make_club_records.py --check                    read the files, write nothing

Input (docs/research/club-records-0.6.0/): clubs.tsv (the facts, one row per club and season), sources.tsv (the
squad source of each row, tools/seasons/find_club_sources.py). The database division of a row is the one that the
club moves table gives the same club (tools/seasons/make_clubs.py), so both tables name the club alike.

Output: Seasons/club_records.tsv of the download, Windows-1252, with tabs. Line 1 is
"#bosman-club-records<TAB>1<TAB><row count>", line 2 the column names, then one row per new club:
    season     the start year (2001 for 2001/02)
    division   the long name of the database division of the club (club_comp.dat +4)
    real       the name of the club in the club moves table (its key there)
    name       the long name of the record (51 characters), short the short name (26)
    city, stadium, capacity (0 when unknown), colour1 (shirt), colour2 (text)
    source     the neutral name of the season database of its squad, "" for none; source_club its record id there
The game builds the records in src/recomp_rt/win/season_records.c. Doc: docs/research/2026-10-19-club-records.md.
"""
import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import make_clubs  # noqa: E402

RECORDS = os.path.join(os.path.dirname(os.path.dirname(HERE)), "docs", "research", "club-records-0.6.0")
COLUMNS = ["season", "division", "real", "name", "short", "city", "stadium", "capacity", "colour1", "colour2",
           "source", "source_club"]


def build():
    clubs = list(csv.DictReader(open(os.path.join(RECORDS, "clubs.tsv"), encoding="utf-8"), delimiter="\t"))
    sources = {(int(r["season"]), r["club"]): r
               for r in csv.DictReader(open(os.path.join(RECORDS, "sources.tsv"), encoding="utf-8"), delimiter="\t")}
    moves, _report, _missing = make_clubs.build()
    division = {(r[0], r[4]): r[1] for r in moves if r[2] == -1}
    out, errors = [], []
    for c in clubs:
        season, key = int(c["season"]), make_clubs.plain(c["club"])
        s = sources.get((season, c["club"]))
        if not s:
            errors.append(f"{season} {c['club']}: no row in sources.tsv")
            continue
        if s["source"] == "=":                  # the season's own database holds it (Q505)
            continue
        d = division.get((season, key))
        if not d:
            errors.append(f"{season} {c['club']}: the club moves table has no missing club of that name")
            continue
        cap = c["capacity"]
        row = [season, d, key, make_clubs.plain(c["name"])[:51], make_clubs.plain(c["short_name"])[:26],
               make_clubs.plain(c["city"]), make_clubs.plain(c["stadium"])[:51], cap if cap.isdigit() else "0",
               c["colour1"], c["colour2"], s["source"], s["club_id"] if s["source"] else "-1"]
        if any("\t" in str(x) for x in row) or not row[3] or not row[4]:
            errors.append(f"{season} {c['club']}: an empty name or a tab")
            continue
        out.append(row)
    return out, errors


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("--out")
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args(argv)
    if not (a.out or a.check):
        ap.error("give --out or --check")
    out, errors = build()
    for e in errors:
        print("make_club_records.py:", e)
    if errors:
        return 1
    if a.check:
        print(f"make_club_records.py: {len(out)} records, {len({r[2] for r in out})} clubs, "
              f"{sum(1 for r in out if not r[10])} with no squad source")
        return 0
    make_clubs.write(a.out, [f"#bosman-club-records\t1\t{len(out)}", "\t".join(COLUMNS)], out, game=True)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
