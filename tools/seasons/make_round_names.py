#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_round_names_table.c, the real round names by competition and season (0.6.0,
Q370; slice 22 brings the first rows, slices 23 to 29 add theirs).

Usage:
    python3 -I tools/seasons/make_round_names.py            write season_round_names_table.c
    python3 -I tools/seasons/make_round_names.py --check    exit 1 when the C file is not the one the table gives

Input: docs/research/round-names.tsv, one row per name, exactly these columns:
    comp_file       the source file of the original's class (european_cup): a file of the function map
                    (docs/research/function-map/<file>.cpp.md), so a competition the game has; or the file of a
                    new competition ("new_conference_league", a string of src/recomp_rt/win/season_comps.c or
                    a header there)
    round_code      the code of the round record (+7): a multiple of 10 from 10 to 990 (the code + 1 to + 5 names
                    the replays, legs and decider)
    first_season    the start year of the first season with the name: the season of the real rename
    sets            the name sets of the round name function 0x4b4850, joined with ",": long (flag 0),
                    plural (flag 3) and abbreviated (flag 1)
    name            at most 40 characters in the game's one-byte code (no character over U+00FF, no '"', '\\' or
                    '<'), or "-": from first_season the round takes the original's name again (the end of a name)
    source          where the name comes from
The stop rule: a row it cannot read, two rows for one competition, code, season and set, or an extra or missing
column stops it with exit code 1 and names the line.
"""
import csv
import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
TSV = os.path.join(ROOT, "docs", "research", "round-names.tsv")
FUNCTION_MAP = os.path.join(ROOT, "docs", "research", "function-map")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_round_names_table.c")
COLUMNS = ["comp_file", "round_code", "first_season", "sets", "name", "source"]
SETS = {"long": 1, "plural": 2, "abbreviated": 4}


def fail(n, text):
    raise SystemExit(f"make_round_names.py: line {n}: {text}")


def rows():
    files = {os.path.basename(p)[:-len(".cpp.md")].lower() for p in glob.glob(os.path.join(FUNCTION_MAP, "*.cpp.md"))}
    win = os.path.join(ROOT, "src", "recomp_rt", "win")
    for p in [os.path.join(win, "season_comps.c")] + glob.glob(os.path.join(win, "*.h")):
        with open(p, encoding="utf-8") as f:
            files |= set(re.findall(r'"(new_[a-z_]+)"', f.read()))
    out, seen = [], set()
    with open(TSV, newline="", encoding="utf-8") as f:
        lines = list(csv.reader(f, delimiter="\t"))
    if not lines or lines[0] != COLUMNS:
        fail(1, f"the columns are not {COLUMNS}")
    for n, cells in enumerate(lines[1:], start=2):
        if len(cells) != len(COLUMNS):
            fail(n, f"{len(cells)} columns, not {len(COLUMNS)}")
        r = dict(zip(COLUMNS, cells))
        if r["comp_file"] not in files:
            fail(n, f"comp_file {r['comp_file']!r} is not a file of the function map")
        if not r["round_code"].isdigit() or int(r["round_code"]) % 10 or not 10 <= int(r["round_code"]) <= 990:
            fail(n, f"round_code {r['round_code']!r} is not a multiple of 10 from 10 to 990")
        if not r["first_season"].isdigit() or not 2001 <= int(r["first_season"]) <= 2099:
            fail(n, f"first_season {r['first_season']!r}")
        words = r["sets"].split(",")
        if not words or any(w not in SETS for w in words) or len(set(words)) != len(words):
            fail(n, f"sets {r['sets']!r} is not a list of long, plural and abbreviated")
        name = r["name"]
        if not name or len(name) > 40 or any(ord(c) > 0xff or c in '"\\<' for c in name):
            fail(n, f"name {name!r}")
        if not r["source"].strip():
            fail(n, "no source")
        code, first = int(r["round_code"]), int(r["first_season"])
        for w in words:
            key = (r["comp_file"], code, first, w)
            if key in seen:
                fail(n, f"a second row for {key}")
            seen.add(key)
        out.append((r["comp_file"], code, first, sum(SETS[w] for w in words), name))
    return out


def c_string(s):
    return '"' + "".join(c if ord(c) < 0x80 else "\\%03o" % ord(c) for c in s) + '"'


def text():
    lines = ["/* The real round names (season_round_names.h). Made by tools/seasons/make_round_names.py from",
             " * docs/research/round-names.tsv, where each row has its source. Do not edit by hand. */",
             '#include "season_round_names.h"', "", "const SeasonRoundName season_round_name_rows[] = {"]
    for file, code, first, sets, name in rows():
        lines.append(f'    {{"{file}", {code}, {first}, {sets}, {"NULL" if name == "-" else c_string(name)}}},')
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
