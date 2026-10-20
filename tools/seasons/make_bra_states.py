#!/usr/bin/env python3
"""The state of a Brazilian club (0.6.0 slice 20e, Q600): the city and club tables and their C table.

Usage:
    python3 -I tools/seasons/make_bra_states.py --measure <folder of databases>
                    write docs/research/bra-city-states.tsv from the databases of MEASURE
    python3 -I tools/seasons/make_bra_states.py
                    write src/recomp_rt/win/season_bra_states_table.c from bra-city-states.tsv
    python3 -I tools/seasons/make_bra_states.py --check
                    exit 1 when season_bra_states_table.c is not the file that the tsv would give

The data has no state field. The stadium's city of a Brazilian club (club +0x69 stadium id, stadium +0x38 city
id, city name at +4) ends with the state code, "Fortaleza (CE)", in the Update Team databases to March 2020. The
newer databases drop the code. --measure reads the databases of MEASURE (the Update Team ones from 2005 to 2020)
and keeps each city name, and each club name, that always comes with one state. A name with two states is left
out. Read the databases as data only.
"""
import collections
import csv
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
TSV = os.path.join(ROOT, "docs", "research", "bra-city-states.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_bra_states_table.c")
sys.path.insert(0, os.path.join(HERE, "own_db"))

# The databases of --measure: Update Team databases with the state code in the city names (2026-10-20: 79 to 99
# percent of the Brazilian clubs).
MEASURE = ["September 2005 Helvete", "November 2007", "February 2008", "September 2008", "March 2009",
           "September 2009", "September 2010 ODB", "September 2011 ODB", "September 2012", "Autumn 2013",
           "October 2014", "October 2015", "October 2016", "October 2017", "October 2018", "October 2019",
           "March 2020"]
STATES = {"AC", "AL", "AM", "AP", "BA", "CE", "DF", "ES", "GO", "MA", "MG", "MS", "MT", "PA", "PB", "PE", "PI", "PR",
          "RJ", "RN", "RO", "RR", "RS", "SC", "SE", "SP", "TO"}
SUFFIX = re.compile(r"^(.*?)\s*\(([A-Z]{2})\)\s*$")


def fail(text):
    raise SystemExit("make_bra_states.py: " + text)


def brazil_clubs(path):
    """(club long name, city name) of each Brazilian club of a database folder."""
    from cmdb import Folder, cstr
    g = lambda r, o: struct.unpack_from("<i", r, o)[0]  # noqa: E731
    f = Folder(path)
    brazil = [g(r, 0) for r in f.rows(4) if cstr(r, 4, 51) == "Brazil"]
    if len(brazil) != 1:
        fail(f"{path}: no nation Brazil")
    city = {g(r, 0): cstr(r, 4, 51) for r in f.rows(21)}
    stadium = {g(r, 0): g(r, 0x38) for r in f.rows(5)}
    for r in f.rows(0):
        if g(r, 0x53) == brazil[0]:
            yield cstr(r, 4, 51), city.get(stadium.get(g(r, 0x69), -1), "")


def measure(folder):
    cities, clubs = collections.defaultdict(set), collections.defaultdict(set)
    for name in MEASURE:
        path = os.path.join(folder, name)
        if not os.path.isdir(path):
            fail(f"{path} is missing")
        for club, city in brazil_clubs(path):
            m = SUFFIX.match(city)
            if m and m.group(2) in STATES:
                cities[m.group(1)].add(m.group(2))
                clubs[club].add(m.group(2))
    rows = [("city", n, next(iter(s))) for n, s in cities.items() if len(s) == 1 and n]
    rows += [("club", n, next(iter(s))) for n, s in clubs.items() if len(s) == 1 and n]
    rows.sort()
    with open(TSV, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["kind", "name", "state"])
        w.writerows(rows)
    print(f"wrote {TSV}: {sum(r[0] == 'city' for r in rows)} cities, {sum(r[0] == 'club' for r in rows)} clubs")


def c_string(s):
    """A C string of the name in the game's code page 1252."""
    out = []
    for b in s.encode("cp1252"):
        out.append(chr(b) if 32 <= b < 127 and chr(b) not in '"\\?' else f"\\{b:03o}")
    return '"' + "".join(out) + '"'


def text():
    with open(TSV, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    for r in rows:
        if r["kind"] not in ("city", "club") or r["state"] not in STATES or not r["name"]:
            fail(f"bad row {r}")
    lines = ["/* Made by tools/seasons/make_bra_states.py from docs/research/bra-city-states.tsv. Do not",
             " * edit. The names are in code page 1252, as the game holds them. */",
             '#include "season_bra_states.h"', "",
             "const SeasonBraName season_bra_names[] = {"]
    for r in rows:
        lines.append(f"    {{{1 if r['kind'] == 'club' else 0}, {c_string(r['name'])}, \"{r['state']}\"}},")
    lines += ["};", f"const int season_bra_name_count = {len(rows)};", ""]
    return "\n".join(lines)


def main(argv):
    if "--measure" in argv:
        measure(argv[argv.index("--measure") + 1])
        return 0
    want = text()
    if "--check" in argv:
        have = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if have != want:
            print(f"{OUT} is not up to date: run tools/seasons/make_bra_states.py")
            return 1
        print("season_bra_states_table.c matches bra-city-states.tsv")
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(want)
    print(f"wrote {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
