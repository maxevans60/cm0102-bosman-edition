#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_intl_table.c: the editions of the international finals (0.6.0 slice 27).

Usage:
    python3 -I tools/seasons/make_intl.py            write season_intl_table.c
    python3 -I tools/seasons/make_intl.py --check    exit 1 when season_intl_table.c is not the file that the
                                                     rules table would give (ctest season_intl_table)

The rows are those of docs/research/rules-0.6.0/int-world.tsv and int-continental.tsv whose comp_file is a
finals class of the original and whose format is not "not held". One edition per row:
    file        the source file of the class (world_cup, euro_champ, ...)
    season      the season of the row (2003 for 2003/04)
    year        the edition year that the class holds at +0x40: the year of the last date of the format
                ("19 January to 10 February 2002" gives 2002, "21 December 2025 to 18 January 2026" 2026)
    teams       the first count of the clubs column
    golden      1 when the format gives a golden goal (Q367: the Euro 2004 silver goal is 0)
    month, day  the first day of the finals, 0 when the format gives none
    hosts       the host nations of the format, "/" between names, ";" between hosts. The names are those of
                the game's nation.dat where the two differ (HOST_NAMES), with the other name a database
                can use after a "/" (China PR and China)
    row         the id of the row
A row that the tool cannot read stops it with exit code 1. A row of SKIP has no edition the classes can play:
the reason is next to it. Slices 28 and 29 add their competitions to the same table.
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
RULES = os.path.join(ROOT, "docs", "research", "rules-0.6.0")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_intl_table.c")

# comp_file of the table: the source file of the class (sweep-international section 2).
FILES = {
    "world_cup.cpp": "world_cup",
    "fifa_confed.cpp": "fifa_confed",
    "olympics.cpp": "olympics",
    "euro_champ": "euro_champ",
    "african_nations": "african_nations",
    "asia_nations": "asia_nations",
    "oceania_nations": "oceania_nations",
    "goldcup": "goldcup",
}

# Rows with no edition that a class plays.
SKIP = {
    "int-continental-43": "the two-legged final of the 2004 OFC Nations Cup, part of the edition of row 32",
    "int-continental-76": "the 2008 OFC Nations Cup is one edition over two seasons: row 87 holds it",
    "int-continental-208": "the 2020 OFC Nations Cup was cancelled",
    "int-continental-293": "the 2028 Africa Cup of Nations has no host and no dates yet",
    "int-continental-296": "the 2028 OFC Nations Cup has no host and no dates yet",
    "int-continental-297": "the 2028 Copa America has no host and no format yet",
}

# The edition year and first day where the format gives no date range that the tool reads.
YEARS = {
    "int-continental-32": (2004, 5, 29),    # "final stage from 29 May 2004", final in October 2004 (row 43)
    "int-continental-87": (2008, 0, 0),     # home-and-away round robin to 19 November 2008
    "int-continental-238": (2023, 6, 0),    # "June to July 2023": the planned time, exact days unknown (Q277)
}

# Rows that a decision of Max moves (season, year, month, day).
MOVED = {
    "int-world-251": (2020, 2020, 7, 22),   # Q366: the Tokyo Olympics play on their planned dates in 2020/21
}

# Rows whose hosts have no automatic place, so the host table gets none (the classes put a host in).
NO_HOST_PLACES = {
    "int-continental-199": "Euro 2020: 12 host nations, each had to qualify",
    "int-continental-287": "Euro 2028: no host qualifies automatically; two places are kept for hosts that "
                           "miss out in the qualifying (slice 28)",
}

# Names of the table -> names of nation.dat (clean 3.9.68 data; later databases write China).
HOST_NAMES = {
    "Netherlands": "Holland",
    "China": "China PR/China",
    "United Kingdom": "England",
    "United Kingdom (Great Britain team)": "England",
}

MONTHS = {m: i + 1 for i, m in enumerate(
    ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October",
     "November", "December"])}
M = "(" + "|".join(MONTHS) + ")"
RANGE = re.compile(r"(\d{1,2})(?: " + M + r")?(?: (\d{4}))? to (\d{1,2}) " + M + r" (\d{4})")
HOSTS = re.compile(r"^(?:planned )?hosts?:? (.+)$")


def fail(text):
    raise SystemExit("make_intl.py: " + text)


def read(name):
    with open(os.path.join(RULES, name), newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def hosts(row, fmt):
    first = fmt.split(";")[0].strip()
    if first.startswith("no host"):
        return []
    m = HOSTS.match(first)
    if not m:
        fail(f"{row}: no host in the format: {first!r}")
    text = m.group(1)
    inner = re.match(r"^\d+ nations \((.+)\)$", text)
    if inner:
        text = inner.group(1)
    text = re.sub(r" \([^)]*\)", "", text)                  # "Japan (Tokyo)", "Argentina (South Zone)"
    names = [n.strip() for n in re.split(r", | and ", text) if n.strip()]
    if not names:
        fail(f"{row}: no host in the format: {first!r}")
    return [HOST_NAMES.get(n, n) for n in names]


def edition(r):
    fmt = r["format"]
    row = r["id"]
    m = RANGE.search(fmt)
    if row in YEARS:
        year, month, day = YEARS[row]
    elif m:
        day, month = int(m.group(1)), MONTHS[m.group(2) or m.group(5)]
        year = int(m.group(6))
    else:
        fail(f"{row}: no dates in the format: {fmt[:80]!r}")
    teams = re.match(r"^(\d+)", r["clubs"])
    if not teams:
        fail(f"{row}: no team count: {r['clubs']!r}")
    low = fmt.lower()
    golden = 1 if "golden goal" in low and "no golden goal" not in low and "without golden goal" not in low \
        and "golden or silver goal" not in low else 0
    season = int(r["season"][:4])
    if row in MOVED:
        season, year, month, day = MOVED[row]
    host_list = [] if row in NO_HOST_PLACES else hosts(row, fmt)
    return {"file": FILES[r["comp_file"]], "season": season, "year": year, "teams": int(teams.group(1)),
            "golden": golden, "month": month, "day": day, "hosts": host_list, "row": row}


def editions():
    out = []
    for part in ("int-world.tsv", "int-continental.tsv"):
        for r in read(part):
            if r["comp_file"] not in FILES or r["format"].startswith("not held") or r["id"] in SKIP:
                continue
            out.append(edition(r))
    out.sort(key=lambda e: (e["file"], e["year"], e["season"]))
    for a, b in zip(out, out[1:]):
        if a["file"] == b["file"] and a["year"] == b["year"]:
            fail(f"{a['row']} and {b['row']}: two editions of {a['file']} in {a['year']}")
    return out


def text():
    lines = [
        "/* The editions of the international finals (0.6.0 slice 27, season_intl.h). Made by",
        " * tools/seasons/make_intl.py from docs/research/rules-0.6.0/int-world.tsv and int-continental.tsv:",
        " * do not edit by hand. */",
        '#include "season_intl.h"',
        "",
        "const SeasonIntlEdition season_intl_editions[] = {",
    ]
    for e in editions():
        hosts_c = ";".join(e["hosts"]).replace('"', '\\"')
        lines.append(f'    {{"{e["file"]}", {e["season"]}, {e["year"]}, {e["teams"]}, {e["golden"]}, '
                     f'{e["month"]}, {e["day"]}, "{hosts_c}", "{e["row"]}"}},')
    lines += [
        "};",
        "const int season_intl_edition_count = (int)(sizeof season_intl_editions / sizeof season_intl_editions[0]);",
        "",
    ]
    return "\n".join(lines)


def main(argv):
    body = text()
    if "--check" in argv:
        old = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if old != body:
            print("make_intl.py: season_intl_table.c differs from the rules table; run tools/seasons/make_intl.py")
            return 1
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(body)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
