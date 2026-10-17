#!/usr/bin/env python3
"""Makes src/recomp_rt/win/seasons_table.c from the signatures table (0.6.0 slice 14).

Usage:
    python3 -I tools/seasons/make_db_table.py            write seasons_table.c
    python3 -I tools/seasons/make_db_table.py --check    exit 1 when seasons_table.c is not the file that the
                                                         table would give

The table is docs/research/rules-0.6.0/databases-signatures.tsv. The game recognises a known database by the
sizes and SHA-256 of club.dat and staff.dat (Q271), so the C file keeps only those and the name, style,
season and role of each row. The stop rule: a row that the tool cannot read (a role it does not know, a
hash that is not 64 hex digits, a name that is not plain) stops it with exit code 1.
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
TSV = os.path.join(ROOT, "docs", "research", "rules-0.6.0", "databases-signatures.tsv")
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "seasons_table.c")

ROLES = {"official version": "SEASONS_ROLE_OFFICIAL_VERSION", "official base": "SEASONS_ROLE_OFFICIAL_BASE",
         "season database": "SEASONS_ROLE_SEASON", "extra version": "SEASONS_ROLE_EXTRA"}
NOT_SEASON = "SEASONS_ROLE_NOT_SEASON"
NAME = re.compile(r"^[A-Za-z0-9][A-Za-z0-9. ]*$")
HEX = re.compile(r"^[0-9a-f]{64}$")


def rows():
    with open(TSV, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            name = r["database"]
            if not NAME.match(name):
                raise SystemExit(f"make_db_table.py: the name {name!r} is not a plain name")
            role = ROLES.get(r["role"], NOT_SEASON if r["role"].startswith("not a season") else None)
            if role is None:
                raise SystemExit(f"make_db_table.py: {name}: unknown role {r['role']!r}")
            if r["data_season"] == "none":
                season = 0
            else:
                m = re.match(r"^(\d{4})/\d{2}$", r["data_season"])
                if not m:
                    raise SystemExit(f"make_db_table.py: {name}: bad season {r['data_season']!r}")
                season = int(m.group(1))
            for k in ("club_sha256", "staff_sha256"):
                if not HEX.match(r[k]):
                    raise SystemExit(f"make_db_table.py: {name}: bad {k}")
            yield name, r["style"], season, role, int(r["club_size"]), int(r["staff_size"]), r["club_sha256"], r["staff_sha256"]


def text():
    lines = [
        "/* The known databases (seasons_db.h). Made by tools/seasons/make_db_table.py from",
        " * docs/research/rules-0.6.0/databases-signatures.tsv. Do not edit by hand: change the table and run the tool. */",
        '#include "seasons_db.h"',
        "",
        "const SeasonKnownDb seasons_known_dbs[] = {",
    ]
    n = 0
    for name, style, season, role, cs, ss, ch, sh in rows():
        lines.append(f'    {{"{name}", "{style}", {season}, {role}, {cs}u, {ss}u,')
        lines.append(f'     "{ch}",')
        lines.append(f'     "{sh}"}},')
        n += 1
    lines += ["};", "", f"const int seasons_known_count = {n};", ""]
    return "\n".join(lines)


def main(argv):
    want = text()
    if "--check" in argv:
        have = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if have != want:
            print("make_db_table.py: seasons_table.c is not the file that the signatures table gives. "
                  "Run tools/seasons/make_db_table.py.", file=sys.stderr)
            return 1
        print("seasons_table.c matches the signatures table")
        return 0
    with open(OUT, "w", encoding="utf-8") as f:
        f.write(want)
    print(f"wrote {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
