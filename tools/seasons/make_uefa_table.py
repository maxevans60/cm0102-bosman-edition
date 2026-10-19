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
# Where the losers of the Champions League qualifying rounds go in the Europa League, from the first season of
# each rule: the club counts of the Europa League rows ("159 entered + 33 from the Champions League": 15 third
# qualifying round losers to the play-offs, 10 play-off losers to the groups, 8 group thirds) and their format.
# 2021/22 on (slice 24): Q2 losers to Q3, Champions Path Q3 losers to the play-offs, League Path Q3 losers and
# play-off losers to the groups ("21 entered + 37 from the Champions League" = 10 + 3 + 6 + 4 + 6 + 8 thirds);
# the preliminary round and Q1 losers go to the Conference League.
DROPS = {2009: "Q3>PO PO>G", 2018: "PR>Q2 Q1>Q2 Q2>Q3 Q3-CP>PO Q3-LP>G PO>G",
         2021: "Q2-CP>Q3 Q2-LP>Q3 Q3-CP>PO Q3-LP>G PO>G"}
# Slice 24, part b: where the losers of the Champions League ("c:") and the Europa League ("e:") enter the
# Conference League, from the club counts of its rows ("136 entered + 45 from the Champions League and Europa
# League" in 2021/22: the 3 preliminary round and 16 first qualifying round losers of the Champions League, the 8
# third qualifying round and 10 play-off losers of the Europa League, and the 8 Europa League group thirds in the
# knockout round play-offs). The rounds of the Conference League have one path in the game, as the Europa
# League's: all Champions League losers of the first qualifying round go to its second qualifying round (the real
# draw gives one of them a place in the third).
ECL_DROPS = {2021: "c:PR>Q2 c:Q1>Q2 e:Q3>PO e:PO>G"}
LEAGUE_CUP = {"England", "France"}  # the nations whose league cup winner can take a Europa League place
# Slice 24: ecl the Conference League (k), ecl-holder its title holder (K; "G (EL)" a Europa League place, Y).
ACCESS_COMPS = {"cl": "c", "el": "e", "cl-holder": "h", "el-holder": "H", "el-fairplay": "f", "ecl": "k",
                "ecl-holder": "K"}
OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_uefa_table.c")
FIRST, LAST = 2001, 2023
MAX_ROUNDS = 12
ROUND_NAMES = ["QR", "PR", "Q1", "Q2", "Q3", "PO", "R1", "R2", "R3", "R4", "Groups", "Groups 2", "KO PO", "KO"]
# The rounds list writes the knockout round play-offs (2021/22 on) "KO play-offs".
ROUND_ALIASES = {"KO play-offs": "KO PO"}
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
        if r and r.group(1) in ROUND_ALIASES:
            r = re.fullmatch(r"(.+) (\d+)", f"{ROUND_ALIASES[r.group(1)]} {r.group(2)}")
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
    """(groups, clubs a group, meetings, places through a group) or (0, 0, 0, 0). From 2021/22 the winners go to
    the round of 16 and the runners-up to the knockout round play-offs: two through."""
    m = re.search(r"(\d+) groups of (\d+), (single|double) round robin", row["format"])
    if not m:
        return 0, 0, 0, 0
    t = re.search(r"top (two|three) plus", row["format"])
    po = re.search(r"winners to the round of 16; runners-up play the (?:Champions|Europa) League group thirds in "
                   r"knockout round play-offs", row["format"])
    if not t and not po:
        fail(row, "groups with no 'top two|three plus' and no 'winners to the round of 16; runners-up play'")
    if po and not any(n == "KO PO" for n, _ in rounds(row)):
        fail(row, "knockout round play-offs in the text, but no 'KO play-offs' round")
    return int(m.group(1)), int(m.group(2)), 1 if m.group(3) == "single" else 2, THROUGH[t.group(1)] if t else 2


def super_cup(row):
    """(stadium, city, day, month 0 to 11, extra time 1 or 0) of the Super Cup row."""
    m = re.search(r"one match at (.+), ([^,]+), (\d+) (\w+) \d{4};", row["format"])
    if not m or m.group(4) not in MONTHS:
        fail(row, "no 'one match at <stadium>, <city>, <day> <month> <year>;'")
    # "if level: unknown" (2006, the row of 2026/27) keeps the original's extra time.
    no_et = "no extra time; penalties after 90 minutes" in row["format"]
    return m.group(1), m.group(2), int(m.group(3)), MONTHS.index(m.group(4)), 0 if no_et else 1


def uc_groups(row):
    """groups(row), and a stop when the rounds list names a group stage that the text does not describe."""
    g = groups(row)
    if any(n == "Groups" for n, _ in rounds(row)) and not g[0]:
        fail(row, "the rounds name a group stage, but no 'N groups of M, single|double round robin'")
    return g


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


def away_goals(row):
    """1 when away goals decide a level two-legged tie, 0 with 'no away goals rule' (U9, 2021/22 on)."""
    return 0 if "no away goals rule" in row["format"] else 1


NATIONS = os.path.join(ROOT, "src", "recomp_rt", "win", "season_nation_table.c")


def game_nations():
    """The nation names of the nations table (season_nation.h), which the game uses to find a nation's record."""
    with open(NATIONS, encoding="utf-8") as f:
        return set(re.findall(r'^\s*\{"([^"]+)", 0x[0-9a-f]+u, \d+\},', f.read(), re.M))


def suspended(row):
    """The nations of the access list row with no places ("Russia suspended: no places"), comma separated. A name
    that the nations table does not hold stops the tool: the game could not find it."""
    names = re.findall(r"([A-Z][a-z]+(?: [A-Z][a-z]+)*) suspended: no places", row["notes"])
    known = game_nations()
    for n in names:
        if n not in known:
            fail(row, f"suspended nation {n!r} is not in {os.path.relpath(NATIONS, ROOT)}")
    return ",".join(names)


def ko_po(row):
    return sum(c for n, c in rounds(row) if n == "KO PO")


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
            if r["comp_file"] == "uefa_seeding" and r["comp"] == "access list":
                by["access"] = r
            if r["comp_file"] == "new" and r["comp"] == "Conference League":
                by["ecl"] = r
            if r["comp_file"] in ("european_cup", "uefa_cup", "intertoto_cup", "eur_super_cup"):
                if r["comp_file"] in by:
                    fail(r, f"two {r['comp_file']} rows in {key}")
                by[r["comp_file"]] = r
        for f in ("european_cup", "uefa_cup", "intertoto_cup", "eur_super_cup"):
            if f not in by:
                raise SystemExit(f"make_uefa_table.py: no {f} row in {key}")
        cl, uc, uic = by["european_cup"], by["uefa_cup"], by["intertoto_cup"]
        ecl = by.get("ecl")
        if ecl and ecl["format"].startswith("not held"):
            ecl = None
        for x in (uc, ecl):             # one away goals rule for the three cups (the game has one per season)
            if x and away_goals(x) != away_goals(cl):
                fail(x, f"away goals rule {away_goals(x)}, the Champions League row has {away_goals(cl)}")
        for x in (uc, ecl):
            if x and any(n == "KO PO" for n, _ in rounds(x)) and not groups(x)[0]:
                fail(x, "knockout round play-offs with no group stage")
        names = [n for n, _ in rounds(cl)]
        if "Groups" not in names or "KO" not in names:
            fail(cl, "the Champions League rounds have no Groups or no KO")
        out.append({
            "season": season,
            "ids": f"{cl['id']} {uc['id']} {uic['id']} {by['eur_super_cup']['id']}",
            "sc": super_cup(by["eur_super_cup"]),
            "cl_clubs": first_int(cl, "clubs"), "cl_rounds": rounds(cl),
            "uc_rounds": rounds(uc), "uc_groups": uc_groups(uc),
            "uic": intertoto(uic),
            "uc_ko_po": ko_po(uc), "away_goals": away_goals(cl),
            "suspended": suspended(by["access"]) if "access" in by else "",
            "ecl_rounds": rounds(ecl) if ecl else [], "ecl_groups": groups(ecl) if ecl else (0, 0, 0, 0),
            "ecl_ko_po": ko_po(ecl) if ecl else 0,
        })
    return out


def read_access():
    """The access list by association rank (Q369), seasons ACCESS_FIRST to LAST: [(season, comp, from, to,
    places)]. comp: c Champions League, e Europa League (the cup winner's place first), h the Champions League
    title holder, H the Europa League title holder ("PO-LP (CL)" or "G (CL)": a Champions League place, given as
    comp X), f the fair play places, L a nation whose last Europa League place goes to its league cup winner;
    slice 24: k the Conference League, K its title holder (Y with a Europa League place), M a nation whose last
    Conference League place goes to its league cup winner. places: the entry round of each place in domestic order (PLACE_TOKENS). A
    rank with no places ("-": Liechtenstein, which has no league) is left out; the game gives that rank the
    places of the next bracket."""
    with open(ACCESS, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    out = []
    brackets = {}                       # (season, comp): [(from, to, row)] of every rank row, "-" rows too
    for r in rows:
        season = int(r["season"][:4])
        if season < ACCESS_FIRST or season > LAST:
            continue
        r = dict(r, id=f"access {r['season']} {r['comp']} {r['rank_from']}")
        if r["comp"] in ("cl", "el", "ecl"):
            try:
                brackets.setdefault((season, r["comp"]), []).append((int(r["rank_from"]), int(r["rank_to"]), r))
            except ValueError:
                fail(r, "a rank bracket that is not two numbers")
        comp = ACCESS_COMPS.get(r["comp"])
        if not comp:
            fail(r, f"comp {r['comp']!r} is not one of {sorted(ACCESS_COMPS)}")
        places = r["places"].strip()
        if places == "-" and "Liechtenstein" in r["notes"]:
            continue
        if places == "-":               # slice 24: ranks with no place in this competition (EL from 2021/22)
            if comp != "e" or season < 2021:
                fail(r, "places '-' for a range of ranks: only the Europa League from 2021/22 has ranks with no place")
            places = ""
        if places.endswith(" (CL)") and comp == "H":
            comp, places = "X", places[:-5]
        if places.endswith(" (EL)") and comp == "K":
            comp, places = "Y", places[:-5]
        toks = places.split()
        if (not toks and r["places"].strip() != "-") or any(t not in PLACE_TOKENS for t in toks):
            fail(r, f"places {places!r} hold a token that is not one of {sorted(PLACE_TOKENS)}")
        lo = int(r["rank_from"]) if r["rank_from"] not in ("-", "") else 0
        hi = int(r["rank_to"]) if r["rank_to"] not in ("-", "") else 0
        if comp in "cek" and (lo < 1 or hi < lo):
            fail(r, "no rank bracket")
        out.append((season, comp, lo, hi, places))
        # The Europa League place of a league cup winner (the notes of the bracket: "England: last place to the
        # league cup winner"): a row L with the nation's name. The game knows England and France (LEAGUE_CUP).
        if comp in "ek":
            found = re.findall(r"([A-Z][a-z]+)(?: \(rank \d+\))?: last place to the league cup", r["notes"])
            mentions = len(re.findall(r"(?<!no )league cup", r["notes"], re.I))   # "no league cup place": none
            if mentions != len(found):
                fail(r, f"the notes name {mentions} league cup places, {len(found)} as '<Nation>: last place to the "
                        "league cup'")
            for nation in found:
                if nation not in LEAGUE_CUP:
                    fail(r, f"league cup place of {nation}, not one of {sorted(LEAGUE_CUP)}")
                out.append((season, "L" if comp == "e" else "M", 0, 0, nation))   # M: the Conference League's
    # The brackets of a season and cup follow each other from rank 1: no overlap, no repeat, no gap.
    for (season, comp), bs in sorted(brackets.items()):
        bs.sort(key=lambda b: (b[0], b[1]))
        expect = 1
        for lo, hi, r in bs:
            if lo > hi:
                fail(r, f"rank bracket {lo} to {hi} runs backwards")
            if lo < expect:
                fail(r, f"rank bracket {lo} to {hi} overlaps the bracket before it (ranks to {expect - 1})")
            if lo > expect:
                fail(r, f"no bracket for ranks {expect} to {lo - 1}")
            expect = hi + 1
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
        drops = ""
        for first in sorted(DROPS):
            if r["season"] >= first:
                drops = DROPS[first]
        lines.append(f"     {c_text(sc[0])}, {c_text(sc[1])}, {sc[2]}, {sc[3]}, {c_text(drops)},")
        eg = r["ecl_groups"]
        lines.append(f"     {r['uc_ko_po']}, {r['away_goals']}, {c_text(r['suspended'])},")
        ecl_drops = ""
        for first in sorted(ECL_DROPS):
            if r["season"] >= first and r["ecl_rounds"]:
                ecl_drops = ECL_DROPS[first]
        lines.append(f"     {c_rounds(r['ecl_rounds'])}, {eg[0]}, {eg[1]}, {r['ecl_ko_po']}, {sc[4]},")
        lines.append(f"     {eg[2]}, {eg[3]}, {c_text(ecl_drops)}}},")
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
