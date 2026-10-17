#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_history_table.c from docs/research/history-0.6.0/plan.tsv (0.6.0 slice 11c).

Usage:
    python3 -I tools/seasons/make_history_table.py           writes the C table
    python3 -I tools/seasons/make_history_table.py --check   exits 1 when the C table is not the one plan.tsv gives

plan.tsv comes from tools/seasons/history_fill.py --write. For each database before 2010/11 it holds the
competition and label years whose history rows the game clears (`clear`), and the researched rows that it adds
(`add`), with the club names as that database writes them. The names go into C as Latin-1 bytes, the encoding of
the game data.
"""
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
PLAN = os.path.join(ROOT, "docs", "research", "history-0.6.0", "plan.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_history_table.c")


def c_str(s):
    b = s.encode("latin-1")
    out = []
    for ch in b:
        if ch in (0x22, 0x5c):
            out.append("\\" + chr(ch))
        elif 0x20 <= ch < 0x7f and ch != 0x3f:      # '?' is escaped: no trigraphs
            out.append(chr(ch))
        else:
            out.append("\\%03o" % ch)
    return '"' + "".join(out) + '"'


def make():
    with open(PLAN, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    dbs = {}
    for r in rows:
        k = (r["database"], int(r["start_year"]))
        dbs.setdefault(k, {"clear": [], "add": []})[r["action"]].append(r)
    lines = ["/* Made by tools/seasons/make_history_table.py from docs/research/history-0.6.0/plan.tsv. Do not edit. */",
             "/* The competition history of the season databases before 2010/11 (0.6.0 slice 11c, Q347). */",
             '#include "season_history.h"', ""]
    table = []
    for i, ((db, start), v) in enumerate(dbs.items()):
        lines.append(f"static const SeasonHistClear clears_{i}[] = {{")
        for r in v["clear"]:
            lines.append(f"    {{{c_str(r['competition'])}, {int(r['label'])}}},")
        lines.append("};")
        lines.append(f"static const SeasonHistRow rows_{i}[] = {{")
        for r in v["add"]:
            places = ", ".join(c_str(r[f]) for f in ("first", "second", "third", "host"))
            nations = ", ".join(c_str(r[f + "_nation"]) for f in ("first", "second", "third", "host"))
            lines.append(f"    {{{c_str(r['competition'])}, {int(r['label'])}, {{{places}}}, {{{nations}}}}},")
        lines.append("};")
        table.append(f"    {{{c_str(db)}, {start}, clears_{i}, {len(v['clear'])}, rows_{i}, {len(v['add'])}}},")
    lines.append("const SeasonHistDb season_history_dbs[] = {")
    lines += table
    lines.append("};")
    lines.append(f"const int season_history_dbs_count = {len(table)};")
    return "\n".join(lines) + "\n"


def main(argv):
    text = make()
    if "--check" in argv:
        old = open(OUT, encoding="latin-1").read() if os.path.isfile(OUT) else ""
        if old != text:
            print(f"{OUT} is not the table that {PLAN} gives: run tools/seasons/make_history_table.py")
            return 1
        print("season history table matches plan.tsv")
        return 0
    with open(OUT, "w", encoding="latin-1", newline="\n") as f:
        f.write(text)
    print("wrote", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
