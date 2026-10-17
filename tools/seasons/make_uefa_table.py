#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_uefa_table.c, the formats of the UEFA club competitions by season (0.6.0
slice 22; slices 23 to 25 add their seasons to the same table).

Usage:
    python3 -I tools/seasons/make_uefa_table.py            write season_uefa_table.c
    python3 -I tools/seasons/make_uefa_table.py --check    exit 1 when season_uefa_table.c is not the file that
                                                           the rules table would give
    python3 -I tools/seasons/make_uefa_table.py --print    print the encoded rows

Input: docs/research/rules-0.6.0/uefa.tsv, the rows of the seasons FIRST to LAST. Each season gives one row of
the C table (season_uefa.h) from three rows of the part:
    european_cup    the Champions League row (not the qualifying path rows): the "Rounds (clubs):" list of its
                    format and the club count
    uefa_cup        the UEFA Cup row: the rounds list, and "N groups of M, single|double round robin"
    intertoto_cup   the club count, the round count ("Five rounds" or "Three rounds") and the winners that
                    enter the UEFA Cup ("the 3 final winners enter the UEFA Cup first round", "the 11
                    third-round winners enter the UEFA Cup second qualifying round"), or "not held"
A round of the rounds list is a name and a club count: Q1, Q2, Q3, QR (qualifying), R1 to R4, PO, Groups (the
first group stage), Groups 2 (a second group stage) and KO (the first knockout round after the groups; the
UEFA Cup row of 2006/07 writes it R3). The stop rule: a row that the tool cannot read stops it with exit code
1 and names the row. After LAST the game keeps the rules of LAST (Q248) until a later slice adds rows.
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
PART = os.path.join(ROOT, "docs", "research", "rules-0.6.0", "uefa.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_uefa_table.c")
FIRST, LAST = 2001, 2008
MAX_ROUNDS = 8
ROUND_NAMES = ["QR", "Q1", "Q2", "Q3", "PO", "R1", "R2", "R3", "R4", "Groups", "Groups 2", "KO"]
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
        if not r or r.group(1) not in ROUND_NAMES:
            fail(row, f"round {part!r} is not a name of ROUND_NAMES and a count")
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
    """(groups, clubs a group, meetings) or (0, 0, 0)."""
    m = re.search(r"(\d+) groups of (\d+), (single|double) round robin", row["format"])
    if not m:
        return 0, 0, 0
    return int(m.group(1)), int(m.group(2)), 1 if m.group(3) == "single" else 2


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
            if r["comp_file"] in ("european_cup", "uefa_cup", "intertoto_cup"):
                if r["comp_file"] in by:
                    fail(r, f"two {r['comp_file']} rows in {key}")
                by[r["comp_file"]] = r
        for f in ("european_cup", "uefa_cup", "intertoto_cup"):
            if f not in by:
                raise SystemExit(f"make_uefa_table.py: no {f} row in {key}")
        cl, uc, uic = by["european_cup"], by["uefa_cup"], by["intertoto_cup"]
        names = [n for n, _ in rounds(cl)]
        if "Groups" not in names or "KO" not in names:
            fail(cl, "the Champions League rounds have no Groups or no KO")
        out.append({
            "season": season,
            "ids": f"{cl['id']} {uc['id']} {uic['id']}",
            "cl_clubs": first_int(cl, "clubs"), "cl_rounds": rounds(cl),
            "uc_rounds": rounds(uc), "uc_groups": groups(uc),
            "uic": intertoto(uic),
        })
    return out


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
        lines.append(f"     {c_rounds(r['uc_rounds'])}, {g[0]}, {g[1]}, {g[2]},")
        lines.append(f"     {uic}}},")
    lines += ["};", "const int season_uefa_row_count = (int)(sizeof season_uefa_rows / sizeof season_uefa_rows[0]);",
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
