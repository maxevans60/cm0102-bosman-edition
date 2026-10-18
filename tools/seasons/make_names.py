#!/usr/bin/env python3
"""Makes the names table of the game from the rules table (0.6.0 slice 15, Q226, Q238).

Usage:
    python3 -I tools/seasons/make_names.py --out <names.tsv>     write the table for the game
    python3 -I tools/seasons/make_names.py --check               read the parts, write nothing
    python3 -I tools/seasons/make_names.py --list                print every shown name, for a check by eye

Input: the `name` column of the rules table parts (tools/seasons/make_rules.py reads them).

Output: Seasons/names.tsv of the download, Windows-1252 (the text form of the game's data files), with tabs.
Line 1 is "#bosman-names<TAB>1<TAB><row count>", line 2 the column names, then one row per line:
    season   the start year (2001 for 2001/02)
    file     a source file of the original's class without ".cpp" (eng_first)
    n        0, 1, ...: the name of the n-th competition object of that file in the game (the groups of
             ger_regional, for example). An object after the last n takes the last name.
    long     the long name (51 characters at most: the record holds 52 bytes at +4)
    short    the short name (26 characters at most: 27 bytes at +56)
The game gives each competition object the names of its file and season when a game starts in that season
(src/recomp_rt/win/season_names.c). Slice 16 gives the names of each later season through the same table.

The rows of a file in a season are those of tests/seasons/rules_report_check.py pick_row: the rows whose
comp_file is that file alone first, then rows of two files; of those the rows that are not a qualifying path;
the k-th object takes the k-th of these rows. A row whose name is empty, n/a or unknown gives no name, and
rows of comp_file new or na give none (no class of the original runs them).

The shown name (shown_name): the research name with these changes, so that the screens show a plain name:
    - a part in brackets at the end goes, unless it names a confederation (UEFA, CAF, AFC, CONCACAF, CONMEBOL,
      OFC) or a play-off: "Primera División (Liga BBVA)" gives "Primera División", "Intercontinental Cup
      (Toyota Cup)" gives "Intercontinental Cup";
    - the year of an edition goes: "2012 Africa Cup of Nations" gives "Africa Cup of Nations". The object of a
      competition can wait for another edition than the row's (the "idle" kind of the rules report), so a year
      in a name could be wrong. "UEFA Euro 2012" gives "UEFA European Championship";
    - a name in quotes at the end goes: 'Primera División "Julio H. Grondona"' gives "Primera División";
    - two editions in one row ("2020-21 Finals; 2022-23 UEFA Nations League (start)") give the later one;
    - a name over 50 characters (the limit of slice 16's season_change_rename) loses its part in brackets
      first ("FIFA World Cup qualification (inter-confederation play-offs)" gives "FIFA World Cup
      inter-confederation play-offs"), then it is cut after the last whole word that fits, never inside a
      bracket.
The stop rule also stops the tool on a long name over 50 or a short name over 26 characters.
The short name is the shown name when it has 26 characters or fewer; else shorter words of SHORTER come in
("FIFA World Cup qualification (UEFA)" gives "WC qualifying (UEFA)"); else a known opening goes
("Campeonato Brasileiro Série A" gives "Série A", "League of Ireland Premier Division" gives "Premier
Division"); else the words that fit.

The stop rule: a name that Windows-1252 cannot hold stops the tool with exit code 1.
"""
import argparse
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import make_rules  # noqa: E402

FORMAT = 1
LONG = 50           # slice 16 (season_change_rename) takes 50 characters; the record holds 52 bytes
SHORT = 26
KEEP_BRACKETS = re.compile(r"^(UEFA|CAF|AFC|CONCACAF|CONMEBOL|OFC)$|play-off", re.I)
# Shorter words for a short name, tried in order until the name fits.
SHORTER = [("FIFA World Cup inter-confederation play-offs", "WC play-offs"),
           ("FIFA World Cup qualification", "WC qualifying"),
           ("European Championship qualifying", "Euro qualifying"),
           ("Football at the Summer Olympics", "Olympic Games"),
           ("Supercoppa di Lega di ", "Supercoppa "), ("Supertaça Cândido de Oliveira", "Supertaça"),
           (' "Julio H. Grondona"', "")]
# Openings that a short name drops.
OPENINGS = ["Campeonato Brasileiro ", "Campeonato Nacional de ", "Campeonato ", "League of Ireland ",
            "Northern Ireland Football League ", "Football League ", "Russian Football ", "Irish League ",
            "Lega Pro ", "Belgian ", "J.League ", "FIFA ", "UEFA ", "AFC ", "CAF ", "CONCACAF ", "CONMEBOL ",
            "OFC "]


def cut(text, n):
    """The words that fit in n characters; an open bracket at the end goes with its words."""
    if len(text) <= n:
        return text
    words, out = text.split(" "), ""
    for w in words:
        if len(out) + len(w) + (1 if out else 0) > n:
            break
        out = f"{out} {w}" if out else w
    if out.count("(") > out.count(")"):
        out = out[:out.rindex("(")].rstrip()
    return out or text[:n]


def shown_name(name):
    """The long name that the game shows for a research name, or None for no name."""
    t = (name or "").strip()
    if not t or t.lower() in ("n/a", "unknown", "none") or t.lower().startswith("unknown"):
        return None
    if "; " in t:                                      # two editions in one row: the later one
        t = t.split("; ")[-1]
    m = re.match(r"^(.*\S)\s*\(([^()]*)\)$", t)
    if m and not KEEP_BRACKETS.search(m.group(2)):
        t = m.group(1)
    t = re.sub(r'\s*"[^"]*"$', "", t)                 # a name in quotes at the end (a person's name)
    t = re.sub(r"\bEuro (19|20)\d\d\b", "European Championship", t)
    t = re.sub(r"\b(19|20)\d\d([/\u2013-]\d{2,4})?\b", "", t)
    t = re.sub(r"\s+", " ", t).strip(" ,-")
    if len(t) > LONG:                                  # too long: first the part in brackets goes
        t = re.sub(r"\s*\(([^()]*)\)$", lambda b: " " + b.group(1) if "play-off" in b.group(1) else "", t)
        t = t.replace("FIFA World Cup qualification inter-confederation play-offs",
                      "FIFA World Cup inter-confederation play-offs")
    return cut(t, LONG) if t else None


def short_name(long):
    if len(long) <= SHORT:
        return long
    for old, new in SHORTER:
        if old in long:
            long = long.replace(old, new).strip()
            if len(long) <= SHORT:
                return long
    for o in OPENINGS:
        if long.startswith(o) and len(long) - len(o) <= SHORT:
            return long[len(o):]
    for o in OPENINGS:
        if long.startswith(o):
            return cut(long[len(o):], SHORT)
    return cut(long, SHORT)


def ordered_rows(found, file):
    """The rows of a file in a season, in the order of rules_report_check.py pick_row."""
    alone = [r for r in found if r["comp_file"] == file] or found
    return [r for r in alone if "qualifying" not in r["comp"].lower()] or alone


def table(rows):
    """[(season, file, n, long, short)] in a fixed order."""
    by = collections.defaultdict(list)
    for r in rows:
        if r["comp_file"] in ("new", "na"):
            continue
        for f in r["comp_file"].split(";"):
            by[(r["season"], f)].append(r)
    out = []
    for (season, file) in sorted(by):
        for n, r in enumerate(ordered_rows(by[(season, file)], file)):
            long = shown_name(r["name"])
            if long:
                out.append((season, file, n, long, short_name(long)))
    return out


def names_by_key(rows):
    """{(season, file): [long or None, ...]} by n, for the check of the rules report."""
    out = collections.defaultdict(dict)
    for season, file, n, long, _ in table(rows):
        out[(season, file)][n] = long
    return out


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("--out")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--list", action="store_true")
    a = ap.parse_args(argv)
    if not (a.out or a.check or a.list):
        ap.error("give --out, --check or --list")
    rows = make_rules.read_rows()
    out = table(rows)
    for season, file, n, long, short in out:
        try:
            long.encode("cp1252")
        except UnicodeEncodeError:
            print(f"make_names.py: {season} {file} {n}: {long!r} has a character that Windows-1252 cannot hold")
            return 1
        if len(long) > LONG or len(short) > SHORT or long.count("(") != long.count(")"):
            print(f"make_names.py: {season} {file} {n}: {long!r} / {short!r} is too long or has an open bracket")
            return 1
    if a.list:
        for season, file, n, long, short in out:
            print(f"{season}\t{file}\t{n}\t{long}\t{short}")
    if a.check:
        print(f"make_names.py: {len(out)} names from {len(rows)} rows")
    if a.out:
        lines = [f"#bosman-names\t{FORMAT}\t{len(out)}", "season\tfile\tn\tlong\tshort"]
        lines += [f"{s}\t{f}\t{n}\t{lg}\t{sh}" for s, f, n, lg, sh in out]
        os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
        tmp = a.out + ".tmp"
        with open(tmp, "w", encoding="cp1252", newline="\n") as f:
            f.write("\n".join(lines) + "\n")
        os.replace(tmp, a.out)
        print(f"make_names.py: {len(out)} names written to {a.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
