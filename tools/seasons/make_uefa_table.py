#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_uefa_table.c, the formats of the UEFA club competitions by season (0.6.0
slice 22; slices 23 to 25 add their seasons to the same table).

Usage:
    python3 -I tools/seasons/make_uefa_table.py            write season_uefa_table.c
    python3 -I tools/seasons/make_uefa_table.py --check    exit 1 when season_uefa_table.c is not the file that
                                                           the rules table would give
    python3 -I tools/seasons/make_uefa_table.py --print    print the encoded rows

Input: docs/research/rules-0.6.0/uefa-access.tsv (slice 23: the access list by association rank from 2009/10,
read_access) and docs/research/rules-0.6.0/uefa.tsv, the rows of the seasons FIRST to LAST. Each season gives one row of
the C table (season_uefa.h) from four rows of the part:
    european_cup    the Champions League row (not the qualifying path rows): the "Rounds (clubs):" list of its
                    format and the club count
    uefa_cup        the UEFA Cup row: the rounds list, and "N groups of M, single|double round robin"
    intertoto_cup   the club count, the round count ("Five rounds" or "Three rounds") and the winners that
                    enter the UEFA Cup ("the 3 final winners enter the UEFA Cup first round", "the 11
                    third-round winners enter the UEFA Cup second qualifying round"), or "not held"
    eur_super_cup   "one match at <stadium>, <city>, <day> <month> <year>;" (slice 23: the venue of each year)
A round of the rounds list is a name and a club count: Q1, Q2, Q3, QR (qualifying), R1 to R4, PO, Groups (the
first group stage), Groups 2 (a second group stage) and KO (the first knockout round after the groups; the
UEFA Cup row of 2006/07 writes it R3). From 2009/10 a qualifying round can have two paths, each a round of
its own in the list: "Q3 CP" and "Q3 LP" (Champions League), "Q2 CP" and "Q2 MP" (Europa League). The UEFA
Cup group text also names the clubs that go through: "top two|three plus 8 Champions League group thirds". The stop rule: a row that the tool cannot read stops it with exit code
1 and names the row. After LAST the game keeps the rules of LAST (Q248) until a later slice adds rows.
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
PART = os.path.join(ROOT, "docs", "research", "rules-0.6.0", "uefa.tsv")
ACCESS = os.path.join(ROOT, "docs", "research", "rules-0.6.0", "uefa-access.tsv")
ACCESS_FIRST = 2009                 # Q369: the original's place brackets to 2008/09, the access list from 2009/10
PLACE_TOKENS = {"G", "PO", "PO-CP", "PO-LP", "Q3", "Q3-CP", "Q3-LP", "Q2", "Q2-CP", "Q2-LP", "Q1", "PR"}
ACCESS_COMPS = {"cl": "c", "el": "e", "cl-holder": "h", "el-holder": "H", "el-fairplay": "f"}
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_uefa_table.c")
FIRST, LAST = 2001, 2020
MAX_ROUNDS = 12
ROUND_NAMES = ["QR", "PR", "Q1", "Q2", "Q3", "PO", "R1", "R2", "R3", "R4", "Groups", "Groups 2", "KO"]
# A qualifying round split in two paths (2009/10 on): "Q3 CP" (Champions Path) and "Q3 LP" (League Path) of the
# Champions League, "Q2 CP" and "Q2 MP" (the main path) of the Europa League. Q296: the names of every season.
PATHS = ["CP", "LP", "MP"]
THROUGH = {"two": 2, "three": 3}
MONTHS = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October",
          "November", "December"]
UIC_ENTRY = {"first round": "R1", "second qualifying round": "Q2"}
WORDS = {"Three": 3, "Five": 5}


def fail(row, text):
    raise SystemExit(f"make_uefa_table.py: {row['id']} ({row['season']}): {text}")


def rounds(row):
    """[(name, clubs)] of the "Rounds (clubs):" list of a format."""
    m = re.search(r"Rounds \(clubs\): ([^.]*)\.", row["format"])
    if not m:
        fail(row, "no 'Rounds (clubs):' list")
    out = []
    for part in m.group(1).split(","):
        r = re.fullmatch(r"\s*(.+?) (\d+)\s*", part)
        words = r.group(1).split(" ") if r else []
        path_round = len(words) == 2 and words[0] in ("PR", "Q1", "Q2", "Q3", "PO") and words[1] in PATHS
        if not r or (r.group(1) not in ROUND_NAMES and not path_round):
            fail(row, f"round {part!r} is not a name of ROUND_NAMES (or a round and a path) and a count")
        name = r.group(1)
        if name == "R3" and out and out[-1][0] == "Groups":
            name = "KO"                     # 2006/07: the round of 32 after the groups
        out.append((name, int(r.group(2))))
    if len(out) > MAX_ROUNDS:
        fail(row, f"more than {MAX_ROUNDS} rounds")
    return out


def first_int(row, column):
    m = re.match(r"\s*(\d+)", row[column])
    if not m:
        fail(row, f"{column} {row[column]!r} does not start with a count")
    return int(m.group(1))


def groups(row):
    """(groups, clubs a group, meetings, places through a group) or (0, 0, 0, 0)."""
    m = re.search(r"(\d+) groups of (\d+), (single|double) round robin", row["format"])
    if not m:
        return 0, 0, 0, 0
    t = re.search(r"top (two|three) plus", row["format"])
    if not t:
        fail(row, "UEFA Cup groups with no 'top two|three plus'")
    return int(m.group(1)), int(m.group(2)), 1 if m.group(3) == "single" else 2, THROUGH[t.group(1)]


def super_cup(row):
    """(stadium, city, day, month 0 to 11) of the Super Cup row."""
    m = re.search(r"one match at (.+), ([^,]+), (\d+) (\w+) \d{4};", row["format"])
    if not m or m.group(4) not in MONTHS:
        fail(row, "no 'one match at <stadium>, <city>, <day> <month> <year>;'")
    return m.group(1), m.group(2), int(m.group(3)), MONTHS.index(m.group(4))


def intertoto(row):
    """(clubs, rounds, winners, entry round) or None when not held."""
    if row["format"].startswith("not held"):
        return None
    m = re.match(r"(Three|Five) rounds", row["format"])
    w = re.search(r"the (\d+) (?:final|third-round) winners enter the UEFA Cup (first round|second qualifying "
                  r"round)", row["format"])
    if not m or not w:
        fail(row, "no round count or no winners that enter the UEFA Cup")
    return first_int(row, "clubs"), WORDS[m.group(1)], int(w.group(1)), UIC_ENTRY[w.group(2)]


def read_rows():
    with open(PART, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    out = []
    for season in range(FIRST, LAST + 1):
        key = f"{season}/{(season + 1) % 100:02d}"
        by = {}
        for r in rows:
            if r["season"] != key or "qualifying" in r["comp"].lower():
                continue
            if r["comp_file"] in ("european_cup", "uefa_cup", "intertoto_cup", "eur_super_cup"):
                if r["comp_file"] in by:
                    fail(r, f"two {r['comp_file']} rows in {key}")
                by[r["comp_file"]] = r
        for f in ("european_cup", "uefa_cup", "intertoto_cup", "eur_super_cup"):
            if f not in by:
                raise SystemExit(f"make_uefa_table.py: no {f} row in {key}")
        cl, uc, uic = by["european_cup"], by["uefa_cup"], by["intertoto_cup"]
        names = [n for n, _ in rounds(cl)]
        if "Groups" not in names or "KO" not in names:
            fail(cl, "the Champions League rounds have no Groups or no KO")
        out.append({
            "season": season,
            "ids": f"{cl['id']} {uc['id']} {uic['id']} {by['eur_super_cup']['id']}",
            "sc": super_cup(by["eur_super_cup"]),
            "cl_clubs": first_int(cl, "clubs"), "cl_rounds": rounds(cl),
            "uc_rounds": rounds(uc), "uc_groups": groups(uc),
            "uic": intertoto(uic),
        })
    return out


def read_access():
    """The access list by association rank (Q369), seasons ACCESS_FIRST to LAST: [(season, comp, from, to,
    places)]. comp: c Champions League, e Europa League (the cup winner's place first), h the Champions League
    title holder, H the Europa League title holder ("PO-LP (CL)" or "G (CL)": a Champions League place, given as
    comp X), f the fair play places. places: the entry round of each place in domestic order (PLACE_TOKENS). A
    rank with no places ("-": Liechtenstein, which has no league) is left out; the game gives that rank the
    places of the next bracket."""
    with open(ACCESS, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    out = []
    for r in rows:
        season = int(r["season"][:4])
        if season < ACCESS_FIRST or season > LAST:
            continue
        r = dict(r, id=f"access {r['season']} {r['comp']} {r['rank_from']}")
        comp = ACCESS_COMPS.get(r["comp"])
        if not comp:
            fail(r, f"comp {r['comp']!r} is not one of {sorted(ACCESS_COMPS)}")
        places = r["places"].strip()
        if places == "-":
            continue
        if places.endswith(" (CL)") and comp == "H":
            comp, places = "X", places[:-5]
        toks = places.split()
        if not toks or any(t not in PLACE_TOKENS for t in toks):
            fail(r, f"places {places!r} hold a token that is not one of {sorted(PLACE_TOKENS)}")
        lo = int(r["rank_from"]) if r["rank_from"] not in ("-", "") else 0
        hi = int(r["rank_to"]) if r["rank_to"] not in ("-", "") else 0
        if comp in "ce" and (lo < 1 or hi < lo):
            fail(r, "no rank bracket")
        out.append((season, comp, lo, hi, places))
    for season in range(ACCESS_FIRST, LAST + 1):
        for comp in "ceh":
            if not any(a[0] == season and a[1] == comp for a in out):
                raise SystemExit(f"make_uefa_table.py: no access list rows {comp} in {season}")
    return out


def c_text(t):
    """A C string in the game's code (Windows-1252), each byte above 127 as an octal escape."""
    out = ""
    for b in t.encode("cp1252"):
        out += f"\\{b:03o}" if b > 127 else ("\\" + chr(b) if chr(b) in '"\\' else chr(b))
    return '"' + out + '"'


def c_rounds(rs):
    items = ", ".join(f'{{"{n}", {c}}}' for n, c in rs)
    return f"{len(rs)}, {{{items}}}"


def text():
    lines = [
        "/* The formats of the UEFA club competitions by season (season_uefa.h). Made by",
        " * tools/seasons/make_uefa_table.py from docs/research/rules-0.6.0/uefa.tsv. Do not edit by hand: change the",
        " * table and run the tool. */",
        '#include "season_uefa.h"',
        "",
        "const SeasonUefa season_uefa_rows[] = {",
    ]
    for r in read_rows():
        g = r["uc_groups"]
        u = r["uic"]
        uic = f"{u[0]}, {u[1]}, {u[2]}, \"{u[3]}\"" if u else '-1, 0, 0, ""'
        lines.append(f"    /* {r['season']}/{(r['season'] + 1) % 100:02d}: rows {r['ids']} */")
        lines.append(f"    {{{r['season']}, {r['cl_clubs']}, {c_rounds(r['cl_rounds'])},")
        sc = r["sc"]
        lines.append(f"     {c_rounds(r['uc_rounds'])}, {g[0]}, {g[1]}, {g[2]}, {g[3]},")
        lines.append(f"     {uic},")
        lines.append(f"     {c_text(sc[0])}, {c_text(sc[1])}, {sc[2]}, {sc[3]}}},")
    lines += ["};", "const int season_uefa_row_count = (int)(sizeof season_uefa_rows / sizeof season_uefa_rows[0]);",
              "", f"/* The access list by association rank from {ACCESS_FIRST}/{(ACCESS_FIRST + 1) % 100:02d} "
              "(docs/research/rules-0.6.0/uefa-access.tsv). */",
              "const SeasonUefaAccess season_uefa_access[] = {"]
    for a in read_access():
        lines.append(f"    {{{a[0]}, '{a[1]}', {a[2]}, {a[3]}, {c_text(a[4])}}},")
    lines += ["};",
              "const int season_uefa_access_count = (int)(sizeof season_uefa_access / sizeof season_uefa_access[0]);",
              ""]
    return "\n".join(lines)


def main(argv):
    if "--print" in argv:
        for r in read_rows():
            print(r)
        return 0
    want = text()
    if "--check" in argv:
        have = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if have != want:
            print(f"make_uefa_table.py: {OUT} is not the file that uefa.tsv gives: run the tool")
            return 1
        print("make_uefa_table.py: season_uefa_table.c matches uefa.tsv")
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(want)
    print(f"written: {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
