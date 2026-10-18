#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_cups_table.c from tools/seasons/cup_formats.tsv (0.6.0 slice 21).

Usage:
    python3 -I tools/seasons/make_cups_table.py            write season_cups_table.c
    python3 -I tools/seasons/make_cups_table.py --check    exit 1 when season_cups_table.c is not the file that
                                                           cup_formats.tsv would give

cup_formats.tsv has one row for each cup and season in which the format of its rounds changes. The facts come
from the "format" column of the cup rows of the rules table (docs/research/rules-0.6.0/<part>.tsv). Columns:
    file      the source file of the cup class of the original (eng_fa_cup, as comp_file in the rules table)
    season    the start year of the first season of the format (2016 for 2016/17), from 2002 on: the base game
              (2001/02) keeps the original's rounds
    rounds    how many rounds the class makes (cup+0x3c). The game applies the row only when the cup holds
              this many rounds, so a wrong class stops at run time with a log line and does nothing
    types     one word for each round, first round first: the way the round is played
                  .   keep the round as the original makes it
                  E   single match, extra time then penalties on the day, no replay
                  P   single match, penalties after 90 minutes, no replay
                  R   single match, a level match is replayed once (the replay has extra time and penalties)
                  RP  as R, a level replay goes straight to penalties
                  L   two legs, away goals count (extra time and penalties after the second leg)
                  LN  two legs, no away goals rule (extra time and penalties after the second leg)
                  LP  two legs, no away goals rule, penalties after the second leg, no extra time
                  LH  two legs, away goals count only after extra time (the League Cup rule)
    sentence  the news sentence of the change, plain words (no "<" and no "%")
A row holds the whole format of its cup from that season on: the game takes the latest row of a cup that is not
later than the season. The round fields that the types set are in src/recomp_rt/win/season_cups.c and in
docs/research/2026-10-17-cup-round-record.md. A bad row stops the tool with exit code 1.
"""
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
SRC = os.path.join(HERE, "cup_formats.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_cups_table.c")
WORDS = {".": ".", "E": "E", "P": "P", "R": "R", "RP": "S", "L": "L", "LN": "N", "LP": "Q", "LH": "H"}
# The cup classes whose create_round_table the game wraps (src/recomp_rt/win/season_kind_cups.c).
CLASSES = {"eng_fa_cup", "eng_cc_cup", "eng_fa_trophy", "sco_fa_cup", "bel_fa_cup", "fra_cup", "fra_lge_cup", "nor_cup",
           "spa_cup", "tur_cup", "rus_cup", "ita_cup", "hol_cup"}   # word to the letter in C
FIRST, LAST = 2002, 2099


def fail(text):
    raise SystemExit("make_cups_table.py: " + text)


def text():
    with open(SRC, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    if not rows or list(rows[0].keys()) != ["file", "season", "rounds", "types", "sentence"]:
        fail("cup_formats.tsv: the first line must name file, season, rounds, types, sentence")
    out = []
    seen = set()
    for n, r in enumerate(rows, 2):
        where = f"cup_formats.tsv line {n}"
        if None in r.values() or None in r:
            fail(f"{where}: a row needs 5 columns (file, season, rounds, types, sentence) separated by tabs")
        if r["file"] not in CLASSES:
            fail(f"{where}: {r['file']!r} is not a cup class that the game wraps; the classes are "
                 f"{', '.join(sorted(CLASSES))} (add the wrap in season_kind_cups.c first)")
        if not r["season"].isdigit() or not FIRST <= int(r["season"]) <= LAST:
            fail(f"{where}: the season must be a start year from {FIRST} to {LAST}")
        if not r["rounds"].isdigit() or not 1 <= int(r["rounds"]) <= 32:
            fail(f"{where}: bad rounds {r['rounds']!r}")
        words = r["types"].split()
        if len(words) != int(r["rounds"]) or any(w not in WORDS for w in words):
            fail(f"{where}: types must be {r['rounds']} words of {' '.join(WORDS)}")
        s = r["sentence"]
        if not s or not s.isascii() or any(c in s for c in '"\\<%') or len(s) > 200:
            fail(f"{where}: the sentence must be plain ASCII text of at most 200 characters without a quote, "
                 f"backslash, < or %")
        key = (r["file"], int(r["season"]))
        if key in seen:
            fail(f"{where}: a second row for {r['file']} in {r['season']}")
        seen.add(key)
        out.append((r["file"], int(r["season"]), int(r["rounds"]), "".join(WORDS[w] for w in words), s))
    out.sort()
    lines = [
        "/* The round formats of the domestic cups by season (season_cups.h). Made by tools/seasons/make_cups_table.py",
        " * from tools/seasons/cup_formats.tsv. Do not edit by hand: change the table and run the tool. */",
        '#include "season_cups.h"',
        "",
        "const SeasonCupRow season_cup_rows[] = {",
    ]
    for file, season, rounds, types, s in out:
        lines.append(f'    {{"{file}", {season}, {rounds}, "{types}", "{s}"}},')
    lines += ["};", f"const int season_cup_row_count = {len(out)};", ""]
    return "\n".join(lines)


def main(argv):
    want = text()
    if "--check" in argv:
        have = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if have != want:
            print("season_cups_table.c is not the file that cup_formats.tsv gives: run make_cups_table.py")
            return 1
        print("season cups table matches cup_formats.tsv")
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(want)
    print("wrote " + OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
