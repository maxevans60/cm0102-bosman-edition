#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_nl_table.c: the editions of the Nations Leagues and the real leagues of each
edition (0.6.0 slice 28, season_nl.h).

Usage:
    python3 -I tools/seasons/make_nl.py            write season_nl_table.c
    python3 -I tools/seasons/make_nl.py --check    exit 1 when season_nl_table.c is not the file that the
                                                   tables would give (ctest season_nl_table)

The editions: EDITIONS below, one per row of docs/research/rules-0.6.0/int-continental.tsv whose comp is a
Nations League and whose format gives a league phase. The sizes, group counts and moves are read from the
row's clubs, format, up and down text by hand and written here, because the text has no one form; the tool
checks each edition against its row (season, the total of the clubs column, each size in the format text).
The real leagues: docs/research/rules-0.6.0/nations-league-tiers.tsv (edition, confed, league, group, nation,
two sources). The nation names become the names of the game's nation.dat (NAMES), with the other names that a
database or a rename of slice 27 can give after a "/".
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
RULES = os.path.join(ROOT, "docs", "research", "rules-0.6.0")
TIERS = os.path.join(RULES, "nations-league-tiers.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_nl_table.c")

# row id: (confed, edition, season, sizes, groups, up_winners, down_last, down_lowest)
# down_last: 1 per league whose group last places go down. down_lowest: more nations of a league that go
# down, the lowest of the ranking (a play-out that the game does not play: decision for Max, see the doc).
EDITIONS = {
    # "League A 12, B 12, C 15, D 16; each league 4 groups of 3 or 4"; up "group winners of B, C, D promoted";
    # down "none in the end (format revamp)".
    "int-continental-190": ("UEFA", "2018-19", 2018, (12, 12, 15, 16), (4, 4, 4, 4), 1, (0, 0, 0, 0),
                            (0, 0, 0, 0)),
    # "League A, B, C 16 each (4 groups of 4), D 7 (groups of 4 and 3)"; up "group winners of B, C, D"; down
    # "last-placed teams of A and B groups; 2 League C teams by play-outs".
    "int-continental-212": ("UEFA", "2020-21", 2020, (16, 16, 16, 7), (4, 4, 4, 2), 1, (1, 1, 0, 0),
                            (0, 0, 2, 0)),
    # "League A, B, C 16 (4 groups of 4), D 7"; up "group winners of B, C, D"; down "last-placed teams of A
    # and B groups; 1 League C team by play-out". 55 entered, 54 played (Russia).
    "int-continental-234": ("UEFA", "2022-23", 2022, (16, 16, 16, 7), (4, 4, 4, 2), 1, (1, 1, 0, 0),
                            (0, 0, 1, 0)),
    # "League A, B, C 16 (4 groups of 4), D 6 (2 groups of 3)"; up "group winners of B, C, D; play-off
    # winners"; down "last-placed teams of A and B; two lowest League C teams".
    "int-continental-256": ("UEFA", "2024-25", 2024, (16, 16, 16, 6), (4, 4, 4, 2), 1, (1, 1, 0, 0),
                            (0, 0, 2, 0)),
    # "League A, B, C 16 (4 groups of 4), D 6 (2 groups of 3)"; up "group winners of B and C, all League D
    # teams"; down "two worst last-placed League A teams". The next edition (2028-29, three leagues of 18) is a
    # proposal after the table (Q248).
    "int-continental-278": ("UEFA", "2026-27", 2026, (16, 16, 16, 6), (4, 4, 4, 2), 1, (0, 0, 0, 0),
                            (2, 0, 0, 0)),
    # "League A 12 (4 groups of 3), B 16, C 13"; up "group winners of B (to A) and C (to B)"; down
    # "last-placed teams of A groups (to B) and B groups (to C)".
    "int-continental-204": ("CONCACAF", "2019-20", 2019, (12, 16, 13), (4, 4, 4), 1, (1, 1, 0), (0, 0, 0)),
    # "league phase to 28 March 2023 (League A 12 teams, structure unchanged from 2019-20: inference)"; up
    # "group winners of B and C"; down "none (no relegation from 2022-23)". The league phase starts 2 June
    # 2022 (row int-continental-226, season 2021/22).
    "int-continental-237": ("CONCACAF", "2022-23", 2022, (12, 16, 13), (4, 4, 4), 1, (0, 0, 0), (0, 0, 0)),
    # "League A 16: 12 in two groups of 6 (4 matches each), top 4 plus 4 top-ranked teams to quarter-finals;
    # League B 16 (4 groups of 4); League C 9 (3 groups of 3)"; up "group winners of B and C, best League C
    # runner-up"; down "5th and 6th of A groups, 4th of B groups".
    "int-continental-248": ("CONCACAF", "2023-24", 2023, (16, 16, 9), (2, 4, 3), 1, (0, 1, 0), (4, 0, 0)),
    # As 2023-24 (League A 16 with 4 byes, B 16, C 9 from the moves); up "group winners of B; group winners of
    # C and the best C runner-up"; down "5th and 6th of A groups ...; 4th of B groups".
    "int-continental-259": ("CONCACAF", "2024-25", 2024, (16, 16, 9), (2, 4, 3), 1, (0, 1, 0), (4, 0, 0)),
    "int-continental-281": ("CONCACAF", "2026-27", 2026, (16, 16, 9), (2, 4, 3), 1, (0, 1, 0), (4, 0, 0)),
}

# Names of the tiers file -> names of the game's nation.dat (3.9.68 and the Update Team databases), with the
# other names after "/" (a later database, or a rename of slice 27, season_intl.c).
NAMES = {
    "Netherlands": "Holland",
    "Bosnia and Herzegovina": "Bosnia-Herzegovina",
    "Czech Republic": "Czech Republic/Czechia",
    "Czechia": "Czech Republic/Czechia",
    "North Macedonia": "FYR of Macedonia/North Macedonia",
    "Macedonia": "FYR of Macedonia/North Macedonia",
    "Turkey": "Turkey/Türkiye",
    "Türkiye": "Turkey/Türkiye",
    "Antigua and Barbuda": "Antigua & Barbuda",
    "Saint Kitts and Nevis": "St Kitts & Nevis",
    "Saint Vincent and the Grenadines": "St Vincent & The Grenadines",
    "Trinidad and Tobago": "Trinidad & Tobago",
    "Turks and Caicos Islands": "Turks and Caicos Islands",
    "Curaçao": "Curaçao/Netherlands Antilles",
    "Saint Martin": "Saint-Martin",
    "United States Virgin Islands": "US Virgin Islands",
    "U.S. Virgin Islands": "US Virgin Islands",
}
LEAGUES = "ABCD"

# Rows whose format does not give every size: the sizes are checked against the row named here. 2022-23:
# "structure unchanged from 2019-20: inference" (row 237). 2024-25 and 2026-27: League C 9 from the moves of
# 2023-24 (3 winners and the best runner-up up, 4 down from B), as row 248 gives it.
SIZES_FROM = {
    "int-continental-237": "int-continental-204",
    "int-continental-259": "int-continental-248",
    "int-continental-281": "int-continental-248",
}


def fail(text):
    raise SystemExit("make_nl.py: " + text)


def read(name):
    with open(os.path.join(RULES, name), newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def editions():
    rows = {r["id"]: r for r in read("int-continental.tsv")}
    out = []
    for row, (confed, ed, season, sizes, groups, up, down, lowest) in EDITIONS.items():
        r = rows.get(row)
        if not r:
            fail(f"{row}: no such row")
        if int(r["season"][:4]) != season and not (row == "int-continental-237" and season == 2022):
            fail(f"{row}: season {r['season']}, the tool gives {season}")
        total = re.match(r"^(\d+)", r["clubs"])
        if not total or int(total.group(1)) not in (sum(sizes), sum(sizes) + 1):
            fail(f"{row}: clubs {r['clubs']!r}, the sizes give {sum(sizes)}")
        fmt = rows[SIZES_FROM.get(row, row)]["format"]
        for n in sizes:
            if str(n) not in fmt and str(n) not in r["clubs"]:
                fail(f"{row}: the size {n} is not in the format")
        out.append({"confed": confed, "edition": ed, "season": season, "sizes": sizes, "groups": groups, "up": up,
                    "down": down, "lowest": lowest, "row": row})
    out.sort(key=lambda e: (e["confed"] != "UEFA", e["confed"], e["season"]))
    return out


def tiers(known):
    if not os.path.exists(TIERS):
        return []
    out = []
    with open(TIERS, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            key = (r["confed"], r["edition"])
            if key not in known or r["league"] not in LEAGUES:
                continue                                # the CONCACAF qualifying round, a league "Q"
            group = int(r["group"]) if r["group"].strip().isdigit() else 0
            name = r["nation"].strip()
            out.append({"confed": r["confed"], "edition": r["edition"], "league": LEAGUES.index(r["league"]),
                        "group": group, "nation": NAMES.get(name, name)})
    out.sort(key=lambda t: (t["confed"] != "UEFA", t["confed"], t["edition"], t["league"], t["group"],
                            t["nation"]))
    return out


def c_str(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def text():
    eds = editions()
    known = {(e["confed"], e["edition"]) for e in eds}
    lines = [
        "/* The editions of the Nations Leagues and the real leagues of each edition (0.6.0 slice 28, season_nl.h).",
        " * Made by tools/seasons/make_nl.py from docs/research/rules-0.6.0/int-continental.tsv and",
        " * nations-league-tiers.tsv: do not edit by hand. */",
        '#include "season_nl.h"',
        "",
        "const SeasonNlEdition season_nl_editions[] = {",
    ]
    for e in eds:
        def arr(v):
            v = list(v) + [0] * (4 - len(v))
            return "{" + ", ".join(str(x) for x in v) + "}"
        lines.append(f'    {{{c_str(e["confed"])}, {c_str(e["edition"])}, {e["season"]}, {len(e["sizes"])}, '
                     f'{arr(e["sizes"])}, {arr(e["groups"])}, {e["up"]}, {arr(e["down"])}, {arr(e["lowest"])}, '
                     f'{c_str(e["row"])}}},')
    lines += [
        "};",
        "const int season_nl_edition_count = (int)(sizeof season_nl_editions / sizeof season_nl_editions[0]);",
        "",
        "const SeasonNlTier season_nl_tiers[] = {",
    ]
    t = tiers(known)
    for x in t:
        lines.append(f'    {{{c_str(x["confed"])}, {c_str(x["edition"])}, {x["league"]}, {x["group"]}, '
                     f'{c_str(x["nation"])}}},')
    if not t:
        lines.append('    {"", "", 0, 0, ""},')
    lines += [
        "};",
        f"const int season_nl_tier_count = {len(t)};",
        "",
    ]
    return "\n".join(lines)


def main(argv):
    body = text()
    if "--check" in argv:
        old = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if old != body:
            print("make_nl.py: season_nl_table.c differs from the tables; run tools/seasons/make_nl.py")
            return 1
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(body)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
