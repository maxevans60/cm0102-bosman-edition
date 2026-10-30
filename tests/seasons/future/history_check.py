#!/usr/bin/env python3
"""History checker for the future runs (step 2 of lead-notes future testing): a saved game in, one TSV of the
history rows of every competition out, then the checks of those rows.

Usage:
    python3 -I tests/seasons/future/history_check.py <save.sav> [<save.sav> ...] [--out <folder>]
        [--rules <competitions.tsv>] [--known <known.tsv>] [--label <name>]

The rows are the ones that the winners tab of each competition shows (0x4991f0 draws winner, runner-up, third
and host): the club comp history and the nation comp history of the save (club_comp_history.dat and
nation_comp_history.dat, 26 bytes a row: +0x00 id, +0x04 competition id, +0x08 u16 label year, +0x0a first,
+0x0e second, +0x12 third, +0x16 host, -1 for none; docs/research/2026-10-17-competition-history-fill.md). A row
with a negative competition is a free row. Club and national side ids index club.dat and then nat_club.dat
(581 bytes a record, +0x53 nation). A nation (nation.dat, 290 bytes) has its continent at +0x71. A competition
(club_comp.dat and nation_comp.dat, 107 bytes, the record id is the competition id) has its continent at +0x59
and its nation at +0x5d.

The save date is the latest date of the news list (news.dat: 100 items of 223 bytes after an 8-byte head, the
s16 day of the year from 0 and the s16 year at +0xc9 of each item). Inference: it matches the test logs of the gate runs (date 182/2028 in the
log, last news 189/2028 at the save after it). The start year and changing rules are the words +8 and +0xc of
bosman_season.dat (docs/research/2026-10-17-season-save-part.md, section 4); a save without that part is a 2001
game with changing rules off.

The edition year of a finals class: its .tmp part holds the word after the bytes ff ff ff ff 07 03 xx xx, seven
zero bytes, one byte and three zero bytes (inference from the parts of a 2028 save: FIFA World Cup 2030, the
classes after their table 2200, the ended cups 2999). An edition in progress writes a longer part, where the
pattern is absent; then the check reads nothing.

Checks (one row of <label>.faults.tsv each, check names in the first column):
    duplicate     a competition holds the same label year twice
    no-host       a row with no host in a competition whose rows have hosts (more than half of its rows)
    confederation a winner or runner-up from another confederation, in a row from the start year on. The
                  confederation is the rules file's, else the competition's continent, else the continent of
                  80% or more of the winners before the start year (at least 3); none: no check
    same-final    the same two finalists more than 2 editions in a row (nation competitions, and club
                  competitions with no nation; with --rules every competition whose kind is not a league)
    gap           a gap in the cycle from the start year on: two editions further apart than the cycle and one
                  year. The cycle is the rules file's, else the most common step of the last 6 editions before
                  the start year
    (a competition whose rules cycle is 0 has ended: it gets none of the three checks below)
    missing       no edition after the last one up to the save date (the last edition + cycle + 1 < the save
                  year), for a competition with rows from the start year on or an international one; not for a
                  competition whose class waits on 2999 (ended) or whose rules end year has passed
    next-edition  the class waits for the year 2199 or 2200 (the idle year of rt_intl_next_year), or for a
                  year more than one cycle and one year after the save date
--rules (default: competitions.tsv next to this file, when it is there; lines with # are notes): per key (column comp) the columns
kind, confed (FIFA or nation: no check, else codes joined by |), has_host (0: no host check, 1: every row from
the start on needs one), cycle (0: the competition ended) and end_year (a year or none). --names (default
history_names.tsv next to this file) gives the key of each competition name of a save; a name with no key keeps
the rules above. --known: a TSV with check, competition, year
(or *) and a note; a fault that matches is printed as "known". Exit code 1 when a fault is not known.
"""
import collections
import csv
import datetime
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "save"))
import savfile  # noqa: E402

CLUB, NATION, COMP, CONT, HIST = 0x245, 0x122, 0x6B, 0xC6, 0x1A
IDLE_YEARS = (2199, 2200)
ENDED_YEAR = 2999
EDITION = re.compile(rb"\xff\xff\xff\xff\x07\x03..\x00{7}.\x00\x00\x00(..)", re.S)
PART = re.compile(r"^(.*)_(\d+)\.tmp$")
SEASON = re.compile(r"^\d{4}(-\d{2})? ")
# continent ids of the data (Africa, Asia, Europe, North America, Oceania, South America in every database)
CONF_IDS = ["CAF", "AFC", "UEFA", "CONCACAF", "OFC", "CONMEBOL"]


class LazySav:
    """The members of a .sav, read one at a time (a long game saves 200 MB and more)."""

    def __init__(self, path):
        self.f = open(path, "rb")
        head = self.f.read(savfile.HEADER_SIZE)
        self.version, _, count = struct.unpack_from("<3i", head, 0)
        if self.version not in (1, 2, 3, 4) or not 0 <= count <= savfile.MAX_ENTRIES:
            raise ValueError(f"{path}: not a saved game")
        self.f.seek(0, 2)
        end = self.f.tell()
        ms = []
        for k in range(count):
            o = 12 + k * savfile.ENTRY_SIZE
            off, size = struct.unpack_from("<2I", head, o)
            ms.append((off, size, head[o + 8:o + savfile.ENTRY_SIZE].split(b"\0")[0].decode("latin-1")))
        order = sorted(ms)
        self.members = {}
        for i, (off, size, name) in enumerate(order):
            nxt = order[i + 1][0] if i + 1 < len(order) else end
            self.members[name] = (off, size, nxt - off)

    def names(self):
        return list(self.members)

    def read(self, name):
        off, size, raw = self.members[name]
        self.f.seek(off)
        data = self.f.read(raw)
        return savfile.decode(data, size)[0] if self.version in (2, 4) else data[:size]


def s32(b, o):
    return struct.unpack_from("<i", b, o)[0]


def text(b, o, n=51):
    return b[o:o + n].split(b"\0")[0].decode("latin-1")


class Game:
    def __init__(self, path):
        sav = LazySav(path)
        self.path = path
        cont = sav.read("continent.dat")
        self.continents = {s32(cont, i * CONT): CONF_IDS[s32(cont, i * CONT)] if 0 <= s32(cont, i * CONT) < 6
                           else text(cont, i * CONT + 4, 26) for i in range(len(cont) // CONT)}
        nat = sav.read("nation.dat")
        self.nations = {}
        for i in range(len(nat) // NATION):
            r = nat[i * NATION:(i + 1) * NATION]
            self.nations[s32(r, 0)] = (text(r, 4), self.continents.get(s32(r, 0x71), ""))
        self.clubs, self.sides = {}, set()
        for part in ("club.dat", "nat_club.dat"):
            data = sav.read(part)
            for i in range(len(data) // CLUB):
                r = data[i * CLUB:(i + 1) * CLUB]
                self.clubs[s32(r, 0)] = (text(r, 4), s32(r, 0x53))
                if part == "nat_club.dat":
                    self.sides.add(s32(r, 0))
        self.comps = {}
        for kind, part in (("club", "club_comp.dat"), ("nation", "nation_comp.dat")):
            data = sav.read(part)
            for i in range(len(data) // COMP):
                r = data[i * COMP:(i + 1) * COMP]
                name = text(r, 4)
                if name:
                    self.comps[s32(r, 0)] = {"kind": kind, "name": name, "continent": s32(r, 0x59),
                                             "nation": s32(r, 0x5d)}
        self.rows = []
        for part in ("club_comp_history.dat", "nation_comp_history.dat"):
            data = sav.read(part)
            for i in range(len(data) // HIST):
                rid, cid, year, a, b, c, h = struct.unpack_from("<iiHiiii", data, i * HIST)
                if cid >= 0 and cid in self.comps:
                    self.rows.append((cid, year, a, b, c, h))
        news = sav.read("news.dat")
        dates = [struct.unpack_from("<hh", news, o) for o in range(8 + 0xC9, len(news) - 3, 223)]
        dates = [(y, d) for d, y in dates if 0 <= d < 366 and 1900 < y < 3000]
        self.date = max(dates) if dates else (0, 0)
        self.start, self.changing = 2001, 0
        if "bosman_season.dat" in sav.members:
            part = sav.read("bosman_season.dat")
            if len(part) >= 16:
                self.start, self.changing = struct.unpack_from("<ii", part, 8)
        self.editions, self.parts = {}, set()
        for n in sav.names():
            m = PART.match(n)
            if not m or int(m.group(2)) not in self.comps:
                continue
            self.parts.add(int(m.group(2)))
            if sav.members[n][1] > 1 << 20:
                continue
            found = EDITION.search(sav.read(n))
            if found:
                self.editions[int(m.group(2))] = struct.unpack("<H", found.group(1))[0]

    def club(self, cid):
        """(name, nation, confederation) of a club or national side id; empty for -1."""
        if cid < 0 or cid not in self.clubs:
            return ("", "", "")
        name, nat = self.clubs[cid]
        nname, conf = self.nations.get(nat, ("", ""))
        return (name, nname, conf)


def tsv(path):
    with open(path, newline="", encoding="utf-8") as f:
        return list(csv.DictReader((l for l in f if not l.startswith("#")), delimiter="\t"))


class Rules:
    """The expected rules of competitions.tsv, found by competition name through history_names.tsv."""

    def __init__(self, path=None, names=None):
        self.rows = {r["comp"]: r for r in tsv(path)} if path else {}
        self.names = [(re.compile(r["pattern"], re.I), r["comp"]) for r in tsv(names)] if path and names else []

    def get(self, name):
        """{} or {key, kind, confs (a set, or None: any), has_host (None, 0 or 1), cycle, end}."""
        bare = SEASON.sub("", name)
        key = next((k for pat, k in self.names if pat.fullmatch(bare)), None)
        r = self.rows.get(key)
        if not r:
            return {}
        confed = r.get("confed", "")
        cycle = number(r.get("cycle"))
        return {"key": key, "kind": r.get("kind", ""), "has_host": number(r.get("has_host")),
                "confs": set(confed.split("|")) if confed and set(confed.split("|")) <= set(CONF_IDS) else None,
                "cycle": cycle or None, "end": number(r.get("end_year")), "ended": cycle == 0}


def read_known(path):
    if not path or not os.path.exists(path):
        return []
    return [r for r in tsv(path) if r.get("check")]


def is_known(known, check, comp, year):
    return any(k["check"] == check and k["competition"] == comp and k["year"] in ("*", str(year)) for k in known)


def number(v):
    try:
        return int(v)
    except (TypeError, ValueError):
        return None


def table(game):
    """[(competition id, kind, name, year, first, second, third, host)] sorted by competition and year; each
    place is (name, nation, confederation)."""
    out = []
    for cid, year, a, b, c, h in game.rows:
        comp = game.comps[cid]
        out.append((cid, comp["kind"], comp["name"], year, game.club(a), game.club(b), game.club(c), game.club(h)))
    return sorted(out, key=lambda r: (r[2], r[0], r[3]))


def cycle_of(years, start):
    """The most common step of the last 6 editions before the start year, or None."""
    before = sorted(y for y in set(years) if y < start)[-7:]
    steps = [b - a for a, b in zip(before, before[1:]) if b > a]
    if not steps:
        return None
    best = collections.Counter(steps).most_common()
    return max(s for s, n in best if n == best[0][1]) if len(steps) > 1 else steps[0]


FRIENDLY = re.compile(r"^(Friendly|Reserve|'B' International|Under 21 International)")


def check(game, rules=None):
    """[(check, competition, year, detail)]."""
    rules = rules or Rules()
    by = collections.defaultdict(list)
    for r in table(game):
        by[r[0]].append(r)
    raw = collections.defaultdict(list)
    for cid, year, a, *_ in game.rows:
        raw[cid].append(a)
    save_year, start = game.date[0], game.start
    faults = []
    for cid, rs in by.items():
        comp = game.comps[cid]
        name = comp["name"]
        if FRIENDLY.match(name):
            continue
        rule = rules.get(name)
        sides = sum(1 for a in raw[cid] if a in game.sides)
        teams = "nation" if sides * 2 > len(raw[cid]) else "club"
        international = comp["nation"] < 0
        years = [r[3] for r in rs]
        game_rows = [r for r in rs if r[3] >= start]
        # the same year twice (from the year before the start: older duplicates are rows of the database)
        for y, n in sorted(collections.Counter(years).items()):
            if n > 1 and y >= start - 1:
                pairs = "; ".join(f"{r[4][0]} / {r[5][0]}" for r in rs if r[3] == y)
                faults.append(("duplicate", name, y, f"{n} rows: {pairs}"))
        # a missing host where the competition has hosts
        hosted = [r for r in rs if r[7][0]]
        if rule.get("has_host") == 1 or (rule.get("has_host") is None and len(hosted) * 2 > len(rs)):
            for r in game_rows:
                if not r[7][0]:
                    faults.append(("no-host", name, r[3], f"{r[4][0]} / {r[5][0]}, no host ({len(hosted)} of "
                                                         f"{len(rs)} rows have one)"))
        # a winner or runner-up from the wrong confederation
        if not international:
            conf = game.nations.get(comp["nation"], ("", ""))[1]
        elif "confs" in rule:
            conf = "|".join(sorted(rule["confs"])) if rule["confs"] else ""
        else:
            conf = game.continents.get(comp["continent"], "")
            older = [r[4][2] for r in rs if r[3] < start and r[4][2]]
            if not conf and len(older) >= 3:
                top, n = collections.Counter(older).most_common(1)[0]
                conf = top if n * 5 >= len(older) * 4 else ""
        # the competition record on another continent than its rules (open item 21: the UEFA Nations League built
        # on the record of the Asian Cup qualifiers, continent Asia)
        record = game.continents.get(comp["continent"], "")
        if rule.get("confs") and record in CONF_IDS and record not in rule["confs"]:
            faults.append(("comp-continent", name, save_year, f"the record has continent {record}, the competition "
                                                              f"is {'|'.join(sorted(rule['confs']))}"))
        if conf:
            for r in game_rows:
                for role, p in (("winner", r[4]), ("runner-up", r[5])):
                    if p[0] and p[2] and p[2] not in conf.split("|"):
                        faults.append(("confederation", name, r[3], f"{role} {p[0]} ({p[1]}, {p[2]}), "
                                                                    f"the competition is {conf}"))
        # the same two finalists more than 2 editions in a row
        finals = rule["kind"] != "league" if rule.get("kind") else (teams == "nation" or international)
        if finals:
            run, last = 0, None
            for r in sorted(rs, key=lambda r: r[3]):
                pair = frozenset((r[4][0], r[5][0])) if r[4][0] and r[5][0] else None
                run = run + 1 if pair and pair == last else 1
                last = pair
                if run == 3 and r[3] >= start:
                    faults.append(("same-final", name, r[3], f"{r[4][0]} and {r[5][0]} in 3 editions to {r[3]}"))
        # a gap in the cycle and editions missing up to the save date: for a competition that the game runs
        # (it has a .tmp part) and that was held near the start
        cyc = rule.get("cycle") or cycle_of(years, start)
        end = rule.get("end")
        edition = game.editions.get(cid)
        ys = sorted(set(years))
        if rule.get("ended"):
            continue                                  # ended by the rules: no later edition is expected
        if cyc and ys and cid in game.parts and ys[-1] >= start - 2 * cyc:
            for a, b in zip(ys, ys[1:]):
                if b >= start and a >= start - cyc and b - a > cyc + 1 and not (end and a >= end):
                    faults.append(("gap", name, b, f"no edition between {a} and {b} (cycle {cyc})"))
            ended = edition == ENDED_YEAR or (end is not None and end < save_year)
            if not ended and ys[-1] + cyc + 1 < save_year:
                faults.append(("missing", name, ys[-1] + cyc,
                               f"last edition {ys[-1]}, cycle {cyc}, the save is in {save_year}"))
        # the next draw or start more than one cycle after the save date
        if edition in IDLE_YEARS:
            faults.append(("next-edition", name, edition, f"the class waits for {edition} (save in {save_year})"))
        elif edition and edition != ENDED_YEAR and cyc and edition > save_year + cyc + 1:
            faults.append(("next-edition", name, edition, f"next edition {edition}, the save is in {save_year}, "
                                                          f"cycle {cyc}"))
    return sorted(faults, key=lambda f: (f[1], f[0], f[2]))


def read_nations(path):
    """The rows of nations.tsv: [{kind, season, from (month, day), nation, to}]."""
    if not path or not os.path.exists(path):
        return []
    out = []
    for r in tsv(path):
        month, day = (int(x) for x in r["from"].split("-"))
        out.append({"kind": r["kind"], "season": int(r["season"]), "from": (month, day), "nation": r["nation"],
                    "to": r["to"]})
    return out


def due(game, change):
    """1 when a change of nations.tsv holds at the save date: with changing rules on from its date, with changing
    rules off only when its season is not after the start season (the data of a later start holds it)."""
    if not game.changing:
        return change["season"] <= game.start
    month, day = change["from"]
    year = game.date[0]
    start = (datetime.date(change["season"], month, day) - datetime.date(change["season"], 1, 1)).days
    return change["season"] <= game.start or (year, game.date[1]) >= (change["season"], start)


def check_nations(game, changes):
    """[(check, nation, season, detail)]: a live record (with a continent) under a name that a due rename
    replaced, and a nation in another confederation than a due move gives. A dead record (no continent) beside
    the live one of its later name is no fault."""
    faults = []
    live = {}
    for nid, (name, conf) in game.nations.items():
        if conf:
            live.setdefault(name, []).append(conf)
    year = game.date[0]
    for c in changes:
        if not due(game, c):
            continue
        if c["kind"] == "rename" and c["nation"] in live:
            faults.append(("nation-name", c["nation"], c["season"],
                           f"a live record named {c['nation']} in {year}; from {c['season']} the name is {c['to']}"))
        elif c["kind"] == "confed":
            for conf in live.get(c["nation"], []):
                if conf != c["to"]:
                    faults.append(("nation-confed", c["nation"], c["season"],
                                   f"{c['nation']} is in {conf} in {year}; from {c['season']} it is in {c['to']}"))
    return faults


def place(p):
    return f"{p[0]} ({p[1]})" if p[0] and p[1] else p[0]


def write(game, label, out, faults, known):
    os.makedirs(out, exist_ok=True)
    with open(os.path.join(out, label + ".history.tsv"), "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["competition_id", "kind", "competition", "year", "winner", "runner_up", "third", "host",
                    "winner_conf", "runner_up_conf"])
        for cid, kind, name, year, a, b, c, h in table(game):
            w.writerow([cid, kind, name, year, place(a), place(b), place(c), place(h), a[2], b[2]])
    with open(os.path.join(out, label + ".faults.tsv"), "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["check", "competition", "year", "known", "detail"])
        for c, name, y, d in faults:
            w.writerow([c, name, y, "known" if is_known(known, c, name, y) else "new", d])


def main(argv):
    args = [a for a in argv[1:]]
    opts = {}
    for k in ("--out", "--rules", "--names", "--known", "--label", "--nations"):
        if k in args:
            i = args.index(k)
            opts[k] = args[i + 1]
            del args[i:i + 2]
    if not args:
        print(__doc__)
        return 2
    default = os.path.join(HERE, "competitions.tsv")
    rules = Rules(opts.get("--rules", default if os.path.exists(default) else None),
                  opts.get("--names", os.path.join(HERE, "history_names.tsv")))
    known = read_known(opts.get("--known", os.path.join(HERE, "history_known.tsv")))
    nations = read_nations(opts.get("--nations", os.path.join(HERE, "nations.tsv")))
    new = 0
    for path in args:
        game = Game(path)
        label = opts.get("--label") if len(args) == 1 and "--label" in opts else \
            re.sub(r"[^A-Za-z0-9.-]+", "_", os.path.relpath(path, os.path.dirname(os.path.dirname(path))))
        faults = sorted(check(game, rules) + check_nations(game, nations), key=lambda f: (f[1], f[0], f[2]))
        if "--out" in opts:
            write(game, label, opts["--out"], faults, known)
        fresh = [f for f in faults if not is_known(known, f[0], f[1], f[2])]
        new += len(fresh)
        y, d = game.date
        print(f"{label}: start {game.start}{'' if game.changing else ' (changing rules off)'}, save day {d} of {y}, {len(game.rows)} history rows, "
              f"{len({r[0] for r in game.rows})} competitions, {len(faults)} faults ({len(fresh)} new)")
        for c, name, yr, detail in faults:
            print(f"    {'known' if is_known(known, c, name, yr) else 'FAULT'}  {c}  {name} {yr}: {detail}")
    return 1 if new else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
