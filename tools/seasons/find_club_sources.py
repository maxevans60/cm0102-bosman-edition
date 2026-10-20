#!/usr/bin/env python3
"""Finds the squad source of each new club record (0.6.0 slice 15b, Q359): the nearest season database that holds
the club.

Usage:
    python3 -I tools/seasons/find_club_sources.py --databases <folder> --out <sources.tsv>

Input:
    docs/research/club-records-0.6.0/clubs.tsv         the 52 rows of the research (season, club, ...)
    docs/research/club-records-0.6.0/source-names.tsv  the names that each club has in the databases (hand table)
    docs/research/rules-0.6.0/databases-signatures.tsv the known databases (neutral name, season, club.dat SHA-256)
    <folder>/<any name>/club.dat, staff.dat, ...        the unpacked databases; each folder is known by the SHA-256
                                                        of its club.dat, never by its folder name
Output, one row per research row: season, club, source (the neutral name of the database, "" for none), club_id
(the record id there, -1 for none), players (players at that club there), distance (seasons between the row and
the database).

Rule (Q503): the season database (role "season database") whose data season is nearest to the row's season and
that holds a club of the nation of the row with one of the names of source-names.tsv; on a tie the later one.
A club whose names column is "=" is held by the season's own database (the clubs file of slice 15 names its
record): source "=", no new record. A club that is in no season database gets no source: the game fills its squad (design doc, "Squads").
The output holds no database content: names of databases, record ids and counts only.
"""
import argparse
import csv
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
RECORDS = os.path.join(REPO, "docs", "research", "club-records-0.6.0")
SIGNATURES = os.path.join(REPO, "docs", "research", "rules-0.6.0", "databases-signatures.tsv")

# The nation of a research row, as nation.dat names it (+4).
NATIONS = {"South Korea": ("Korea Republic", "South Korea"), "Republic of Ireland": ("Republic Of Ireland", "Ireland")}


def text(rec, off, n=51):
    return rec[off:off + n].split(b"\0")[0].decode("cp1252", "replace")


def table(folder, tid, size):
    """The rows of table tid of index.dat (file, offset, count), each `size` bytes."""
    idx = open(os.path.join(folder, find(folder, "index.dat")), "rb").read()
    for i in range(8, len(idx) - 66, 67):
        name = idx[i:i + 51].split(b"\0")[0].decode("latin1")
        t, count, off, _ver = struct.unpack_from("<iiii", idx, i + 51)
        if t == tid:
            data = open(os.path.join(folder, find(folder, name)), "rb").read()
            return [data[off + k * size:off + (k + 1) * size] for k in range(count)]
    return []


def find(folder, name):
    for f in os.listdir(folder):
        if f.lower() == name.lower():
            return f
    raise SystemExit(f"{folder}: no {name}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--databases", required=True)
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    sigs = {r["club_sha256"]: r for r in csv.DictReader(open(SIGNATURES), delimiter="\t")
            if r["role"] == "season database"}
    dbs = {}                                    # neutral name -> (data season, folder)
    for f in sorted(os.listdir(a.databases)):
        path = os.path.join(a.databases, f)
        try:
            club = open(os.path.join(path, find(path, "club.dat")), "rb").read()
        except (SystemExit, NotADirectoryError):
            continue
        s = sigs.get(hashlib.sha256(club).hexdigest())
        if s:
            dbs[s["database"]] = (int(s["data_season"][:4]), path)
    names = {r["club"]: [n for n in r["database_names"].split(";") if n]
             for r in csv.DictReader(open(os.path.join(RECORDS, "source-names.tsv"), encoding="utf-8"), delimiter="\t")}
    rows = list(csv.DictReader(open(os.path.join(RECORDS, "clubs.tsv"), encoding="utf-8"), delimiter="\t"))
    held = {}                                   # database -> {(nation, club name): club id}
    for db, (_season, path) in dbs.items():
        nations = {struct.unpack_from("<i", r, 0)[0]: text(r, 4) for r in table(path, 4, 290)}
        held[db] = {(nations.get(struct.unpack_from("<i", r, 0x53)[0], ""), text(r, 4)): struct.unpack_from("<i", r, 0)[0]
                    for r in table(path, 0, 0x245)}
    players = {}

    def count(db, club_id):
        if (db, club_id) not in players:
            staff = table(dbs[db][1], 6, 0x6e)
            players[(db, club_id)] = sum(1 for r in staff if struct.unpack_from("<i", r, 0x39)[0] == club_id
                                         and struct.unpack_from("<i", r, 0x61)[0] != -1)
        return players[(db, club_id)]

    with open(a.out, "w", encoding="utf-8") as out:
        out.write("season\tclub\tsource\tclub_id\tplayers\tdistance\n")
        for r in rows:
            season, nation = int(r["season"]), (r["nation"],) + NATIONS.get(r["nation"], ())
            if r["club"] not in names:
                raise SystemExit(f"source-names.tsv has no row for {r['club']}")
            if names[r["club"]] == ["="]:          # the season's own database holds the club (slice 15 table)
                out.write(f"{season}\t{r['club']}\t=\t-1\t0\t0\n")
                continue
            best = None
            for db, (s, _path) in dbs.items():
                ids = [held[db][(n, c)] for n in nation for c in names[r["club"]] if (n, c) in held[db]]
                if ids:
                    key = (abs(s - season), -s)
                    if best is None or key < best[0]:
                        best = (key, db, ids[0])
            if best:
                out.write(f"{season}\t{r['club']}\t{best[1]}\t{best[2]}\t{count(best[1], best[2])}\t{best[0][0]}\n")
            else:
                out.write(f"{season}\t{r['club']}\t\t-1\t0\t\n")


if __name__ == "__main__":
    sys.exit(main())
