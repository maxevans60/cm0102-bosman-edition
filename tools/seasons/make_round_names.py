#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_round_names_table.c, the real round names by competition and season (0.6.0,
Q370; slice 22 brings the first rows, slices 23 to 29 add theirs).

Usage:
    python3 -I tools/seasons/make_round_names.py            write season_round_names_table.c
    python3 -I tools/seasons/make_round_names.py --check    exit 1 when the C file is not the one the table gives

Input: docs/research/round-names.tsv, one row per name: comp_file (the source file of the original's class,
european_cup), round_code (the code of the round record, +7: 20, 30, 250, ...; a multiple of 10, because the
code + 1 to + 4 names the replays and legs), first_season (the start year of the first season with the name:
the season of the real rename), set (long, short or both: the two name sets of the round name function
0x4b4850), name (at most 40 characters, in the game's one-byte code: no character over U+00FF) and source.
A row it cannot read stops it with exit code 1.
"""
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
TSV = os.path.join(ROOT, "docs", "research", "round-names.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_round_names_table.c")
SETS = {"long": 1, "short": 2, "both": 3}


def fail(n, text):
    raise SystemExit(f"make_round_names.py: line {n}: {text}")


def rows():
    out = []
    with open(TSV, newline="", encoding="utf-8") as f:
        for n, r in enumerate(csv.DictReader(f, delimiter="\t"), start=2):
            if not r["comp_file"].replace("_", "").isalnum():
                fail(n, f"comp_file {r['comp_file']!r}")
            if not r["round_code"].isdigit() or int(r["round_code"]) % 10 or not 10 <= int(r["round_code"]) <= 990:
                fail(n, f"round_code {r['round_code']!r} is not a multiple of 10 from 10 to 990")
            if not r["first_season"].isdigit() or not 2001 <= int(r["first_season"]) <= 2099:
                fail(n, f"first_season {r['first_season']!r}")
            if r["set"] not in SETS:
                fail(n, f"set {r['set']!r} is not long, short or both")
            name = r["name"]
            if not name or len(name) > 40 or any(ord(c) > 0xff or c in '"\\<' for c in name):
                fail(n, f"name {name!r}")
            if not r["source"].strip():
                fail(n, "no source")
            out.append((r["comp_file"], int(r["round_code"]), int(r["first_season"]), SETS[r["set"]], name))
    return out


def c_string(s):
    return '"' + "".join(c if ord(c) < 0x80 else "\\%03o" % ord(c) for c in s) + '"'


def text():
    lines = ["/* The real round names (season_round_names.h). Made by tools/seasons/make_round_names.py from",
             " * docs/research/round-names.tsv, where each row has its source. Do not edit by hand. */",
             '#include "season_round_names.h"', "", "const SeasonRoundName season_round_name_rows[] = {"]
    for file, code, first, sets, name in rows():
        lines.append(f'    {{"{file}", {code}, {first}, {sets}, {c_string(name)}}},')
    lines += ["};", "const int season_round_name_count = (int)(sizeof season_round_name_rows / "
              "sizeof season_round_name_rows[0]);", ""]
    return "\n".join(lines)


def main(argv):
    want = text()
    if "--check" in argv:
        have = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if have != want:
            print(f"make_round_names.py: {OUT} is not the file that round-names.tsv gives: run the tool")
            return 1
        print("make_round_names.py: season_round_names_table.c matches round-names.tsv")
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(want)
    print(f"written: {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
