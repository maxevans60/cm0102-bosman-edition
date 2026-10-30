#!/usr/bin/env python3
"""ctest of tests/seasons/future/history_check.py on a small fixture save.

Usage:
    python3 -I tests/seasons/future/history_check_test.py

The test writes a saved game of a few kilobytes into a temporary folder: the container of tools/save/savfile.py
(compressed members) with six continents, eleven nations, their national sides, six clubs, five competitions and
their history rows, a news list and the .tmp parts of two finals classes. The game starts in 2025 and the save
is on day 189 of 2040. The rows hold one fault for each check of the checker, and two competitions hold none.
The test runs the checker and compares its faults with the planted list, then runs it again with a rules file
and a known-fault list. All data is made up, so the fixture holds no data of a database.
"""
import csv
import os
import re
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "save"))
import savfile  # noqa: E402

CHECKER = os.path.join(HERE, "history_check.py")
CONTINENTS = ["Africa", "Asia", "Europe", "North America", "Oceania", "South America"]
NATIONS = [("Aland", 2), ("Borduria", 2), ("Carpania", 2), ("Durania", 1), ("Elbonia", 1), ("Freedonia", 3),
           ("Genovia", 3), ("Hatay", 5),
           ("Ruritania", 3), ("Kurlandia", 5), ("Osterlich", 5), ("Old Genovia", -1)]
CLUBS = [("Aland City", 0), ("Aland United", 0), ("Borduria Sport", 1), ("Carpania FC", 2), ("Durania Stars", 3),
         ("Elbonia Rovers", 4)]
SIDE0 = 100                                  # national side ids follow the club ids
# id, name, continent, nation, file
COMPS = [(0, "Aland League", 2, 0, "club"), (1, "Euro Super Cup", 2, -1, "club"),
         (10, "World Finals", -1, -1, "nation"), (11, "Euro Finals", 2, -1, "nation"),
         (12, "Asia Finals", 1, -1, "nation"), (13, "Americas Finals", 3, -1, "nation"),
         (14, "Hatay Cup", 5, -1, "nation"), (15, "Nations Shield", 1, -1, "nation")]


def side(n):
    return SIDE0 + n


def history():
    """[(competition, year, first, second, third, host)] and the planted faults {(check, competition, year)}."""
    rows, want = [], set()
    for y in range(2010, 2041):                                       # a clean league
        rows.append((0, y, 0, 1, -1, -1))
    for y in range(2015, 2025):                                       # a club cup, then 3 finals of one pair
        rows.append((1, y, 2 + y % 2, 0 + y % 2, -1, -1))
    for y in (2026, 2027, 2028):
        rows.append((1, y, 0, 3, -1, -1))
    want.add(("same-final", "Euro Super Cup", 2028))
    for y in range(2029, 2041):
        rows.append((1, y, y % 3, 3, -1, -1))
    for y in range(1990, 2035, 4):                                    # World Finals: no host in 2034, then none
        rows.append((10, y, side(y % 8), side((y + 3) % 8), side((y + 5) % 8), side(y % 7) if y < 2034 else -1))
    want |= {("no-host", "World Finals", 2034), ("missing", "World Finals", 2038),
             ("next-edition", "World Finals", 2200)}
    for y in range(1996, 2041, 4):                                    # Euro Finals: 2028 twice
        rows.append((11, y, side(y % 3), side((y + 1) % 3), side((y + 2) % 3), side(y % 3)))
    rows.append((11, 2028, side(2), side(0), side(1), side(1)))
    want.add(("duplicate", "Euro Finals", 2028))
    for y in range(2003, 2040, 4):                                    # Asia Finals: a European runner-up in 2031
        rows.append((12, y, side(3 + y % 2), side(1) if y == 2031 else side(4 - y % 2), -1, side(3)))
    want.add(("confederation", "Asia Finals", 2031))
    for y in list(range(2015, 2027, 2)) + [2031, 2033, 2035, 2037, 2039]:   # Americas Finals: 2027 and 2029 lost
        rows.append((13, y, side(5), side(6 if y // 2 % 2 else 8), -1, side(5)))
    want.add(("gap", "Americas Finals", 2031))
    for y in range(2015, 2040):                                       # Hatay Cup: clean, its class ended
        a, b = ((7, 9), (9, 10), (10, 7))[y % 3]
        rows.append((14, y, side(a), side(b), -1, -1))
    for y in (2020, 2024):                                            # Nations Shield: a European cup on
        rows.append((15, y, side(1), side(2), -1, -1))                # an Asian record (rules run only)
    return rows, want


# Nation changes (nations.tsv): the save is on day 189 of 2040 with changing rules on.
NATION_CHANGES = ("kind\tseason\tfrom\tnation\tto\tsource\n"
                  "rename\t2024\t09-01\tHatay\tHatay Republic\tdue in the start season\n"
                  "rename\t2030\t09-01\tRuritania\tNeo-Ruritania\tdue, a live record\n"
                  "rename\t2030\t09-01\tOld Genovia\tGenovia\tdue, a dead record (no continent)\n"
                  "rename\t2040\t09-01\tKurlandia\tCourland\tnot due before 1 September 2040\n"
                  "confed\t2035\t09-01\tDurania\tUEFA\tdue, still in AFC\n"
                  "confed\t2020\t02-01\tElbonia\tAFC\tdue, in AFC\n")
NATION_FAULTS = {("nation-name", "Hatay", 2024), ("nation-name", "Ruritania", 2030),
                 ("nation-confed", "Durania", 2035)}
NATION_FAULTS_OFF = {("nation-name", "Hatay", 2024)}             # changing rules off: only to the start season


def records(changing=1):
    def pad(b, n):
        return b + bytes(n - len(b))

    cont = b"".join(pad(struct.pack("<i", i) + n.encode(), 0xC6) for i, n in enumerate(CONTINENTS))
    nat = bytearray()
    for i, (n, c) in enumerate(NATIONS):
        r = bytearray(0x122)
        struct.pack_into("<i", r, 0, i)
        r[4:4 + len(n)] = n.encode()
        struct.pack_into("<i", r, 0x71, c)
        nat += r

    def club(cid, name, nation):
        r = bytearray(0x245)
        struct.pack_into("<i", r, 0, cid)
        r[4:4 + len(name)] = name.encode()
        struct.pack_into("<i", r, 0x53, nation)
        return bytes(r)

    clubs = b"".join(club(i, n, nat_) for i, (n, nat_) in enumerate(CLUBS))
    sides = b"".join(club(side(i), n, i) for i, (n, _) in enumerate(NATIONS))
    comp = {"club": bytearray(), "nation": bytearray()}
    for cid, name, c, n, part in COMPS:
        r = bytearray(0x6B)
        struct.pack_into("<i", r, 0, cid)
        r[4:4 + len(name)] = name.encode()
        struct.pack_into("<ii", r, 0x59, c, n)
        comp[part] += r
    rows, want = history()
    want |= NATION_FAULTS
    hist = {"club": bytearray(), "nation": bytearray()}
    files = {cid: part for cid, _, _, _, part in COMPS}
    for i, (cid, y, a, b, c, h) in enumerate(rows):
        hist[files[cid]] += struct.pack("<iiHiiii", 1000 + i, cid, y, a, b, c, h)
    news = bytearray(8 + 100 * 223)
    struct.pack_into("<ii", news, 0, 100, 3)
    for k, (d, y) in enumerate([(180, 2040), (189, 2040), (20, 2040)]):
        struct.pack_into("<hh", news, 8 + k * 223 + 0xC9, d, y)

    def part(year):                                                   # a finals class waiting on its edition year
        return (b"x.tmp\0" + bytes(40) + b"\xff\xff\xff\xff\x07\x03\x50\x00" + bytes(7) + b"\x01" + bytes(3)
                + struct.pack("<H", year) + bytes(40))

    members = [("continent.dat", cont), ("nation.dat", bytes(nat)), ("club.dat", clubs), ("nat_club.dat", sides),
               ("club_comp.dat", bytes(comp["club"])), ("nation_comp.dat", bytes(comp["nation"])),
               ("club_comp_history.dat", bytes(hist["club"])), ("nation_comp_history.dat", bytes(hist["nation"])),
               ("news.dat", bytes(news)), ("bosman_season.dat", struct.pack("<4i", 1, 16, 2025, changing)),
               ("World Finals_10.tmp", part(2200)), ("Asia Finals_12.tmp", part(2043)),
               ("Euro Finals_11.tmp", b"\0" * 64), ("Americas Finals_13.tmp", b"\0" * 64),
               ("Euro Super Cup_1.tmp", b"\0" * 64), ("Hatay Cup_14.tmp", part(2999))]
    return members, want


def write_sav(path, members):
    header = bytearray(savfile.HEADER_SIZE)
    struct.pack_into("<3i", header, 0, 2, 0, len(members))
    body = bytearray()
    for k, (name, data) in enumerate(members):
        o = 12 + k * savfile.ENTRY_SIZE
        struct.pack_into("<2I", header, o, savfile.HEADER_SIZE + len(body), len(data))
        header[o + 8:o + 8 + len(name)] = name.encode("latin-1")
        body += savfile.encode(data)
    with open(path, "wb") as f:
        f.write(header + body)


def run(args):
    r = subprocess.run([sys.executable, "-I", CHECKER] + args, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def faults(folder, label):
    with open(os.path.join(folder, label + ".faults.tsv"), newline="", encoding="utf-8") as f:
        return {(r["check"], r["competition"], int(r["year"])): r["known"] for r in csv.DictReader(f, delimiter="\t")}


def renames_match():
    """The rename rows of nations.tsv against season_intl_renames (season_intl_rules.c)."""
    src = open(os.path.join(ROOT, "src", "recomp_rt", "win", "season_intl_rules.c"), encoding="latin-1").read()
    code = set()
    for season, old, new in re.findall(r'\{(\d{4}), "([^"]*)", ((?:"[^"]*"\s*)+),', src):
        name = "".join(re.findall(r'"([^"]*)"', new))
        code.add((int(season), old, bytes(name, "latin-1").decode("unicode_escape").encode("latin-1")
                  .decode("latin-1")))
    with open(os.path.join(HERE, "nations.tsv"), encoding="utf-8") as f:
        rows = {(int(r["season"]), r["nation"], r["to"])
                for r in csv.DictReader((l for l in f if not l.startswith("#")), delimiter="\t")
                if r["kind"] == "rename"}
    if not code or rows != code:
        return [f"nations.tsv renames differ from season_intl_renames: only in the file {sorted(rows - code)}, "
                f"only in the code {sorted(code - rows)}"]
    return []


def main():
    work = tempfile.mkdtemp(prefix="history-check-")
    members, want = records()
    sav = os.path.join(work, "fixture.sav")
    write_sav(sav, members)
    bad = []
    empty = os.path.join(work, "none.tsv")
    open(empty, "w").write("check\tcompetition\tyear\tnote\n")
    nations = os.path.join(work, "nations.tsv")
    with open(nations, "w", encoding="utf-8") as f:
        f.write("# a note line\n" + NATION_CHANGES)

    code, out = run([sav, "--out", work, "--label", "plain", "--known", empty, "--nations", nations])
    got = faults(work, "plain")
    if set(got) != want:
        bad.append(f"plain run: missed {sorted(want - set(got))}, extra {sorted(set(got) - want)}")
    if code != 1:
        bad.append(f"plain run: exit code {code}, not 1 for new faults")
    if "start 2025, save day 189 of 2040" not in out:
        bad.append("plain run: start year or save date not read: " + out.splitlines()[0])
    with open(os.path.join(work, "plain.history.tsv"), encoding="utf-8") as f:
        lines = f.read().splitlines()
    rows, _ = history()
    if len(lines) != len(rows) + 1:
        bad.append(f"history TSV: {len(lines) - 1} rows, the save holds {len(rows)}")
    if not any(l.startswith("10\tnation\tWorld Finals\t2002\tCarpania (Carpania)\t") and
               l.endswith("\tUEFA\tCONCACAF") for l in lines):
        bad.append("history TSV: the World Finals 2002 row is not as written")

    # A rules file: World Finals ends in 2034 (no missing fault), Euro Super Cup is a league (no same-final
    # fault), Americas Finals keeps its gap. Known list: the Euro Finals duplicate.
    rules = os.path.join(work, "rules.tsv")
    with open(rules, "w", encoding="utf-8") as f:
        f.write("# a note line\ncomp\tname\tkind\tconfed\thas_host\tcycle\tnext_year\tend_year\n"
                "americas\tAmericas Finals\tintl_finals\tCONCACAF\t1\t2\t2041\tnone\n"
                "world\tWorld Finals\tintl_finals\tFIFA\t1\t4\t2038\t2034\n"
                "esc\tEuro Super Cup\tleague\tUEFA\t0\t1\t2041\tnone\n"
                "shield\tNations Shield\tnations_league\tUEFA\t0\t4\t2028\tnone\n")
    names = os.path.join(work, "names.tsv")
    with open(names, "w", encoding="utf-8") as f:
        f.write("# a note line\npattern\tcomp\nAmericas Finals\tamericas\n(The )?World Finals\tworld\n"
                "Euro Super Cup\tesc\nNations Shield\tshield\n")
    known = os.path.join(work, "known.tsv")
    with open(known, "w", encoding="utf-8") as f:
        f.write("check\tcompetition\tyear\tnote\nduplicate\tEuro Finals\t*\tplanted\n")
    code, out = run([sav, "--out", work, "--label", "rules", "--rules", rules, "--names", names, "--known", known,
                     "--nations", nations])
    got = faults(work, "rules")
    want2 = (want - {("missing", "World Finals", 2038), ("same-final", "Euro Super Cup", 2028)}) | \
        {("comp-continent", "Nations Shield", 2040)}
    if set(got) != want2:
        bad.append(f"rules run: missed {sorted(want2 - set(got))}, extra {sorted(set(got) - want2)}")
    if got.get(("duplicate", "Euro Finals", 2028)) != "known":
        bad.append("rules run: the known Euro Finals duplicate is not marked known")

    # Only known faults: exit code 0.
    with open(known, "a", encoding="utf-8") as f:
        for c, comp, y in sorted(want2):
            f.write(f"{c}\t{comp}\t{y}\tplanted\n")
    code, out = run([sav, "--rules", rules, "--names", names, "--known", known, "--nations", nations])
    if code != 0:
        bad.append(f"known run: exit code {code} with only known faults\n{out}")

    # Changing rules off: only the nation changes up to the start season are due.
    off = os.path.join(work, "off.sav")
    write_sav(off, records(changing=0)[0])
    run([off, "--out", work, "--label", "off", "--known", empty, "--nations", nations])
    got = {k for k in faults(work, "off") if k[0].startswith("nation-")}
    if got != NATION_FAULTS_OFF:
        bad.append(f"changing rules off: nation faults {sorted(got)}, not {sorted(NATION_FAULTS_OFF)}")

    # nations.tsv holds the renames of the season code.
    bad += renames_match()

    for b in bad:
        print("FAIL  " + b)
    print(f"{'ok  ' if not bad else 'FAIL'}  history checker: {len(want)} planted faults found, rules and "
          f"known list applied (work folder {work})")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
