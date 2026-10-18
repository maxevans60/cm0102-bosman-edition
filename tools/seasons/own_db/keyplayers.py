"""Key players with no record in the new database (Q383; Max 2026-10-17): they go into a copy of the converter's
output, with ratings from an Update Team record where one is near, else from FM's ratings through a mapping that
the data gives.

Usage: python3 -I keyplayers.py <season start year> <converter output> <facts folder> <work folder>
       <folder of Update Team databases> <new folder> [<overrides.tsv>]

The work folder holds match.pkl and leagues.pkl. The facts folder holds people_all.pkl, loans.pkl (optional) and
ratings.pkl (FM's ratings, read by fm2005/read_fm2005.py or fm2007/read_fm2007.py). Run with python3 -I. The new
folder must not exist. A report goes to <work>/keyplayers-report.tsv.

Key players (the rule of the earlier list, kept): FM players (job bit 1) whom match.py did not match, at a club of a
division whose name ends in "Premier Division", "First Division", "Serie A", "Bundesliga" or "Premier League"
(this takes the top divisions and also the English, Scottish and Dutch second tiers), with an FM weekly wage of
1,000 or more, and with no player of the same surname word at that club in the output. A loaned player counts at
his loan club (facts loans.pkl).

Identity: each key player is looked for in every Update Team database (folders whose name gives a month and year)
and in the output: the full or common name the same, the birth day one day either way and the birth year one year
either way after the database's birth shift (measured on Thierry Henry, born 17.8.1977). Players with no such hit
are looked for again by any name word of 4 letters or more.
Decision per player (an overrides row replaces it):
- template: the output holds him as a player with no club (a template player that match.py missed).
- ut: an Update Team record; the nearest season, not older than the season before the start.
- own: no Update Team record.
Overrides (tab-separated, "#" starts a comment): FM index, action (ut, own or skip), source database, source staff
id, positions (12 values, optional), note. The file lives in the work folder (it names FM rows): it is not in the
repo.

Ratings:
- template: the template's.
- ut, the record's season within one season of the start: the record's ratings as they are (as Q384).
- ut, further away: the record's positions and attributes; current and potential ability from FM's (mapped).
- own: positions from FM's ratings (or the overrides row); attributes, current and potential ability from FM's
  (mapped); the other fields of the staff row from a donor (the output's player of the same main positions and
  an age within 3 years whose current ability is nearest).
The mapping (Max: FM's rating is the reference for these players): for each FM value, the median value of the
Update Team ratings of the matched players (match.py) whose FM value is the same (widened one step at a time
until 100 players), made non-decreasing; above and below the values that 100 players have, the map goes on with
slope 1 (value_map). FM's unset ratings (current ability 0) count as no FM rating: a far record then keeps its
Update Team ratings, and an "own" player with none is left out with a message. One map for current
ability, one for potential ability (FM's ranges -10 to -1 map to the median of each range; the potential ability
is at least the current ability), one per attribute. FM's position ratings are facts and go across as they are
(the older FM scheme has the same twelve positions as CM 01/02 in another order).

Dates are written the Update Team way of the output (k = start year - 2001; convert.py store_update_team_way).
At the end the squad lists must agree with the staff rows (convert.check_squad_lists).
"""
import collections
import datetime
import os
import pickle
import re
import shutil
import statistics
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from cmdb import Folder, cstr, names, norm, staff_view, swords  # noqa: E402
from convert import check_squad_lists  # noqa: E402

CUM = [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334]
EMPTY = (31, 1900)
TOP = ("Premier Division", "First Division", "Serie A", "Bundesliga", "Premier League")
MONTHS = {m: i + 1 for i, m in enumerate("January February March April May June July August September October "
                                          "November December".split())}
# CM 01/02 player row +0x1b to +0x44, one attribute per byte, by the FM name of the same attribute (found by the
# correlation of each byte with FM2005's attributes over matched players: each byte goes clearly best with one FM
# attribute). None: no FM attribute (+0x41 is 0 for most players).
CM_ATTRS = ["Acceleration", "Aggression", "Agility", "Anticipation", "Balance", "Bravery", "Consistency", "Corners",
            "Crossing", "Decisions", "Dirtiness", "Dribbling", "Finishing", "Flair", "Free Kicks", "Handling",
            "Heading", "Important Matches", "Injury Proneness", "Jumping", "Leadership", "Left Foot", "Long Shots",
            "Marking", "Movement", "Natural Fitness", "One On Ones", "Pace", "Passing", "Penalties", "Positioning",
            "Reflexes", "Right Foot", "Stamina", "Strength", "Tackling", "Teamwork", "Technique", None,
            "Versatility", "Vision", "Work Rate"]
# CM 01/02 positions (+0x0f, 12 bytes): goalkeeper, sweeper, defender, defensive midfielder, midfielder, attacking
# midfielder, attacker, wing back, right, left, centre, free role. FM's older scheme lists the free role before
# the sides.
FM_OLD = ["Goalkeeper", "Sweeper", "Defender", "Defensive Midfielder", "Midfielder", "Attacking Midfielder",
          "Attacker", "Wing Back", "Right Side", "Left Side", "Central", "Free Role"]


def i32(r, o):
    return struct.unpack_from("<i", r, o)[0]


def cm_date(d):
    return (CUM[d.month - 1] + min(d.day, 28 if d.month == 2 else 31) - 1, d.year)


def put_date(r, o, dy):
    struct.pack_into("<hHi", r, o, dy[0], dy[1], 0)


def set_count(ib, table, count, offset=None):
    for i in range(8, len(ib) - 66, 67):
        if i32(ib, i + 51) == table:
            struct.pack_into("<i", ib, i + 55, count)
            if offset is not None:
                struct.pack_into("<i", ib, i + 59, offset)
            return
    sys.exit(f"keyplayers: index.dat has no table {table}")


def dbseason(name):
    """Season start year of an Update Team database from its folder name ("September 2005" -> 2005), or None."""
    m = re.match(r"(\w+) (\d{4})", name)
    if not m or (m.group(1) not in MONTHS and m.group(1) != "Autumn"):
        return None
    mo = 10 if m.group(1) == "Autumn" else MONTHS[m.group(1)]
    return int(m.group(2)) if mo >= 7 else int(m.group(2)) - 1


def value_map(pairs, need=100):
    """{FM value: mapped value} from (FM value, CM value) pairs. A value that at least `need` pairs have (dense) maps
    to the median CM value of the pairs with that value, widened one step at a time until `need` pairs, made
    non-decreasing. Above the highest dense value and below the lowest the map goes on with slope 1 from the last
    dense value, so that the few top players are not pulled down to the median of a wide window."""
    by = collections.defaultdict(list)
    for a, b in pairs:
        by[a].append(b)
    dense = [v for v in by if len(by[v]) >= need]
    if not dense:
        dense = [max(by, key=lambda v: len(by[v]))]
    lo, hi = min(dense), max(dense)
    out, best = {}, None
    for v in range(lo, hi + 1):
        w, got = 0, list(by.get(v, ()))
        while len(got) < need and w < 200:
            w += 1
            got += by.get(v - w, []) + by.get(v + w, [])
        m = round(statistics.median(got))
        best = m if best is None else max(best, m)
        out[v] = best
    return out


def mapped(vmap, v):
    """The mapped value; beyond the dense values, slope 1 from the nearest end."""
    lo, hi = min(vmap), max(vmap)
    if v > hi:
        return vmap[hi] + v - hi
    if v < lo:
        return vmap[lo] - (lo - v)
    return vmap[v]


def fm_positions(r):
    """CM 01/02 position bytes from an FM rating: the older scheme when it rates a position 15 or more, else
    derived from the position grid (FM2005 and FM2007 keep both)."""
    pos = r["pos"]
    old = [pos.get(n, 0) for n in FM_OLD]
    if max(old) >= 15:
        return [max(1, v) for v in old]
    g = lambda *k: max(pos.get(n, 0) for n in k)  # noqa: E731
    grid = [pos.get("Goalkeeper", 0), pos.get("Sweeper", 0), g("D L", "D C", "D R"), g("DM C"),
            g("M L", "M C", "M R"), g("AM L", "AM C", "AM R"), g("ST C"), g("WB L", "WB R"),
            g("D R", "M R", "AM R", "WB R"), g("D L", "M L", "AM L", "WB L"), g("D C", "DM C", "M C", "AM C", "ST C"),
            pos.get("Free Role", 0)]
    return [max(1, v) for v in grid]


def main(argv):
    if len(argv) not in (6, 7):
        sys.exit(__doc__)
    Y = int(argv[0])
    k = Y - 2001
    final, X, W, DB, dst = (os.path.abspath(a) for a in argv[1:6])
    over_path = argv[6] if len(argv) == 7 else None
    if os.path.exists(dst):
        sys.exit(f"keyplayers: {dst} exists")
    fp = pickle.load(open(os.path.join(X, "people_all.pkl"), "rb"))
    R = pickle.load(open(os.path.join(X, "ratings.pkl"), "rb"))
    M = pickle.load(open(os.path.join(W, "match.pkl"), "rb"))
    L = pickle.load(open(os.path.join(W, "leagues.pkl"), "rb"))
    lp = os.path.join(X, "loans.pkl")
    if os.path.exists(lp):                          # as convert.py: a loaned player plays for the loan club
        for i, ln in pickle.load(open(lp, "rb")).items():
            if ln["club"] in M["clubmap"] and not (ln["start"] and ln["start"] > datetime.date(Y, 8, 31)):
                fp[i] = dict(fp[i], club=ln["club"], joined=ln["start"] or fp[i]["joined"])
    O = Folder(final)

    # 1. The key list.
    comps = {i32(r, 0): cstr(r, 4, 51) for r in O.rows(12)}
    St, cl = staff_view(O)
    have = {(w, x["club"]) for x in St if x["club"] >= 0 for w in swords(x["second"], x["common"])}
    top = {d for d, n in comps.items() if n.endswith(TOP) and not n.startswith("A ")}
    keys = []
    for i, x in enumerate(fp):
        if i in M["pmap"] or x["job"] != 1 or x["club"] is None or not x["dob"] or x["dob"].year <= 1900:
            continue
        t = M["clubmap"].get(x["club"])
        if L["cur"].get(t, -1) not in top or (x["wage"] or 0) < 1000:
            continue
        if any((w, t) in have for w in swords(x["sn"], x["cn"] or "")):
            continue
        keys.append(dict(fm=i, club=t, wage=x["wage"], x=x))
    keys.sort(key=lambda r: (-r["wage"], r["fm"]))
    print(f"{Y}: {len(keys)} key players")

    # 2. Identity in the Update Team databases and in the output.
    def doy(d):
        return CUM[d.month - 1] + min(d.day, 28 if d.month == 2 else 31) - 1

    want, words = collections.defaultdict(list), collections.defaultdict(list)
    for kk in keys:
        x = kk["x"]
        for n in {norm(x["fn"] + " " + x["sn"]), norm(x["cn"] or "")} - {""}:
            want[n].append(kk)
    hits = collections.defaultdict(list)
    folders = [("FINAL", final, Y)] + [(d, os.path.join(DB, d), dbseason(d)) for d in sorted(os.listdir(DB))
                                        if dbseason(d) is not None and os.path.isdir(os.path.join(DB, d))]

    def scan(by_words):
        for name, path, season in folders:
            try:
                F = Folder(path)
                rows = F.rows(6)
            except (SystemExit, OSError, KeyError) as e:
                print(f"  identity search: {name} skipped, not readable ({e})")
                continue
            fn, sn, cn = names(F, 13), names(F, 14), names(F, 15)
            sh = None
            for r in rows:
                if fn.get(i32(r, 4)) == "Thierry" and sn.get(i32(r, 8)) == "Henry" and \
                        abs(struct.unpack_from("<h", r, 0x10)[0] - 228) <= 1:
                    sh = 1977 - struct.unpack_from("<H", r, 0x12)[0]
            if sh is None:
                continue
            pl = {i32(r, 0): r for r in F.rows(10)}
            for r in rows:
                f, s, c = struct.unpack_from("<iii", r, 4)
                full, common = norm(fn.get(f, "") + " " + sn.get(s, "")), norm(cn.get(c, "") if c >= 0 else "")
                if by_words:
                    ks = {id(kk): kk for w in (full + " " + common).split() for kk in words.get(w, ())}
                else:
                    ks = {id(kk): kk for n in {full, common} - {""} for kk in want.get(n, ())}
                if not ks:
                    continue
                d, y = struct.unpack_from("<hH", r, 0x10)
                p = pl.get(i32(r, 0x61))
                for kk in ks.values():
                    if abs(d - doy(kk["x"]["dob"])) <= 1 and abs(y + sh - kk["x"]["dob"].year) <= 1:
                        hits[kk["fm"]].append(dict(db=name, season=season, sid=i32(r, 0), kind=r[0x5f],
                                                   club=i32(r, 0x39), ca=struct.unpack_from("<h", p, 5)[0] if p else None))

    scan(False)
    for kk in keys:
        if not [h for h in hits[kk["fm"]] if h["ca"]]:
            for w in norm(kk["x"]["fn"] + " " + kk["x"]["sn"] + " " + (kk["x"]["cn"] or "")).split():
                if len(w) >= 4:
                    words[w].append(kk)
    if words:
        scan(True)

    over = {}
    if over_path:
        for line in open(over_path, encoding="utf-8"):
            if line.startswith("#") or not line.strip():
                continue
            p = line.rstrip("\n").split("\t")
            over[int(p[0])] = dict(action=p[1], db=p[2], sid=int(p[3]) if p[3] else -1,
                                   pos=[int(v) for v in p[4].split(",")] if len(p) > 4 and p[4] else None,
                                   note=p[5] if len(p) > 5 else "")
    dec = []
    for kk in keys:
        h = hits[kk["fm"]]
        d = over.get(kk["fm"])
        fin = [x for x in h if x["db"] == "FINAL" and x["club"] < 0 and x["kind"] == 2]
        if d and d["action"] == "skip":
            continue
        if fin and not (d and d["action"] in ("ut", "own")):
            d = dict(action="template", db="", sid=fin[0]["sid"], pos=None, note="template player that match.py missed")
        elif not d:
            c = sorted((x for x in h if x["db"] != "FINAL" and x["ca"] and x["ca"] > 0 and x["season"] >= Y - 1),
                       key=lambda x: (abs(x["season"] - Y), x["season"], x["db"]))
            d = dict(action="ut", db=c[0]["db"], sid=c[0]["sid"], pos=None, note="") if c else \
                dict(action="own", db="", sid=-1, pos=None, note="no Update Team record")
        fmr = R.get(kk["fm"])
        if d["action"] == "own" and (fmr is None or fmr["ca"] <= 0):
            print(f"  left out: FM index {kk['fm']} has no Update Team record and no FM rating to map")
            continue
        dec.append((kk, d))

    # 3. Maps from FM's ratings to the Update Team's, from the matched players of the output.
    staff = O.rows(6)
    players = {i32(r, 0): r for r in O.rows(10)}
    pairs = collections.defaultdict(list)
    for n, t in M["pmap"].items():
        r = R.get(n)
        if r is None or t >= len(staff) or staff[t][0x5f] != 2:
            continue
        p = players.get(i32(staff[t], 0x61))
        if p is None:
            continue
        ca, pa = struct.unpack_from("<hh", p, 5)
        if ca <= 0:
            continue
        if r["ca"] <= 0:
            continue                                # FM leaves the rating unset
        pairs["ca"].append((r["ca"], ca))
        if pa > 0:
            pairs["pa" if r["pa"] > 0 else "pa_range"].append((r["pa"], pa))
        for j, a in enumerate(CM_ATTRS):
            if a and a in r["attrs"]:
                pairs[a].append((r["attrs"][a], p[0x1b + j]))
    vm = {key: value_map(v) for key, v in pairs.items() if key != "pa_range"}
    rng = collections.defaultdict(list)
    for a, b in pairs["pa_range"]:
        rng[a].append(b)
    pa_range = {a: round(statistics.median(v)) for a, v in rng.items()}   # FM's ranges -10 to -1, no order

    def map_pa(fca, v):
        """Mapped potential ability, at least the mapped current ability."""
        m = pa_range.get(v, fca) if v < 0 else fca if v == 0 else mapped(vm["pa"], v)
        return max(min(m, 200), fca)
    print("current ability map (FM -> CM):", {v: vm["ca"][v] for v in range(0, 201, 10) if v in vm["ca"]})
    print("potential ability map (FM -> CM):", {v: vm["pa"][v] for v in range(0, 201, 10) if v in vm["pa"]},
          "ranges:", dict(sorted(pa_range.items())))

    # 4. Write the copy.
    shutil.copytree(final, dst)
    T = Folder(dst)
    staff = T.rows(6)
    players = {i32(r, 0): r for r in T.rows(10)}
    clubs = {i32(r, 0): r for r in T.rows(0)}
    cname = {c: cstr(r, 4, 51) for c, r in clubs.items()}
    nat_by_name = {cstr(r, 4, 51): i32(r, 0) for r in T.rows(4)}

    def age_of(d):
        return Y - d.year - ((d.month, d.day) > (7, 1))

    info = {}
    for r in staff:
        p = players.get(i32(r, 0x61)) if r[0x5f] == 2 else None
        if p is None:
            continue
        ca, pa = struct.unpack_from("<hh", p, 5)
        if ca > 0:
            info[i32(r, 0)] = dict(ca=ca, pa=pa, age=Y - struct.unpack_from("<H", r, 0x12)[0] - k,
                                   pos=list(p[0x0f:0x1b]))

    name_rows = {t: T.rows(t) for t in (13, 14, 15)}
    name_by = {t: {} for t in (13, 14, 15)}
    for t in (13, 14, 15):
        for r in name_rows[t]:
            name_by[t].setdefault(cstr(r, 0, 51), i32(r, 0x33))
    new_names = {13: [], 14: [], 15: []}

    def name_id(t, txt, nat):
        if not txt:
            return -1
        if txt not in name_by[t]:
            r = bytearray(60)
            b = txt.encode("latin1", "replace")[:50]
            r[0:len(b)] = b
            nid = len(name_rows[t]) + len(new_names[t])
            struct.pack_into("<ii", r, 0x33, nid, nat)
            r[0x3b] = 1
            new_names[t].append(r)
            name_by[t][txt] = nid
        return name_by[t][txt]

    srcs = {}

    def src(db):
        if db not in srcs:
            F = Folder(os.path.join(DB, db))
            srcs[db] = dict(staff={i32(r, 0): r for r in F.rows(6)}, pl={i32(r, 0): r for r in F.rows(10)},
                            fn=names(F, 13), sn=names(F, 14), cn=names(F, 15),
                            nat={i32(r, 0): cstr(r, 4, 51) for r in F.rows(4)}, season=dbseason(db))
        return srcs[db]

    def set_club(r, x, club):
        struct.pack_into("<i", r, 0x39, club)
        j = x["joined"]
        jd = cm_date(j) if j and j <= datetime.date(Y, 8, 31) else cm_date(datetime.date(Y, 7, 1))
        put_date(r, 0x3e, (jd[0], jd[1] - k))
        c = x["cend"]
        if c and c.year > Y:
            cd = cm_date(c)
            put_date(r, 0x46, (cd[0], cd[1] - k if cd[1] - k < 2010 else cd[1]))
        else:
            put_date(r, 0x46, EMPTY)
        bd = cm_date(x["dob"])
        put_date(r, 0x10, (bd[0], bd[1] - k))

    def main_pos(pos):
        return tuple(j for j in range(8) if pos[j] >= 15)

    n6, n9, n10, n22 = (T.idx[t]["count"] for t in (6, 9, 10, 22))
    new_staff, new_player, new_pref, report = [], [], [], []
    to_list = collections.defaultdict(list)
    for kk, d in dec:
        x, club, fmr = kk["x"], kk["club"], R.get(kk["fm"])
        if fmr is not None and fmr["ca"] <= 0:
            fmr = None                              # FM leaves the rating unset: no reference
        age = age_of(x["dob"])
        fca = max(1, min(200, mapped(vm["ca"], fmr["ca"]))) if fmr else None
        fpa = map_pa(fca, fmr["pa"]) if fmr else None
        row = dict(fm=kk["fm"], name=x["cn"] or f"{x['fn']} {x['sn']}", club=cname.get(club), age=age,
                   wage=x["wage"], action=d["action"], src=d["db"], fm_ca=fmr["ca"] if fmr else "",
                   fm_pa=fmr["pa"] if fmr else "", note=d["note"])
        if d["action"] == "template":
            r = staff[d["sid"]]
            set_club(r, x, club)
            i = info.get(d["sid"], dict(ca="", pa="", pos=[]))
            row.update(ca=i["ca"], pa=i["pa"], how="template ratings", pos=i["pos"])
            if d["sid"] not in to_list[club]:
                to_list[club].append(d["sid"])
            report.append(row)
            continue
        if d["action"] == "ut":
            S_ = src(d["db"])
            sr = bytearray(S_["staff"][d["sid"]])
            sp = bytearray(S_["pl"][i32(sr, 0x61)])
            g = S_["season"] - Y
            sca, spa = struct.unpack_from("<hh", sp, 5)
            if abs(g) <= 1 or fmr is None:
                ca, pa, how = sca, spa, f"Update Team ratings ({d['db']})" + ("" if abs(g) <= 1 else ", no FM rating")
            else:
                ca, pa, how = fca, fpa, f"FM ratings mapped; positions and attributes of {d['db']}"
            if d["pos"]:
                sp[0x0f:0x1b] = bytes(d["pos"])
            nat = nat_by_name.get(S_["nat"].get(i32(sr, 0x1a)), -1)
            nat2 = nat_by_name.get(S_["nat"].get(i32(sr, 0x1e)), -1)
            first = S_["fn"].get(i32(sr, 4), x["fn"])
            second = S_["sn"].get(i32(sr, 8), x["sn"])
            common = S_["cn"].get(i32(sr, 0x0c), "") if i32(sr, 0x0c) >= 0 else ""
            caps = abs(g) <= 1
        else:
            pos = d["pos"] or fm_positions(fmr)
            mp = main_pos(pos)
            ca, pa = fca, fpa
            cand = [(abs(i["ca"] - ca), abs(i["age"] - age), sid) for sid, i in info.items()
                    if main_pos(i["pos"]) == mp and abs(i["age"] - age) <= 3 and i["pa"] > 0]
            if not cand:
                cand = [(abs(i["ca"] - ca), abs(i["age"] - age), sid) for sid, i in info.items() if i["pa"] > 0]
            donor = min(cand)[2]
            sr = bytearray(staff[donor])
            sp = bytearray(players[i32(sr, 0x61)])
            sca = struct.unpack_from("<h", sp, 5)[0]
            sp[0x0f:0x1b] = bytes(pos)
            for j, a in enumerate(CM_ATTRS):
                if a and a in fmr["attrs"] and a in vm:
                    sp[0x1b + j] = max(0, min(20, mapped(vm[a], fmr["attrs"][a])))
            how = f"FM ratings mapped; staff fields of {donor}"
            nat = M["natmap"].get(x["nat"], -1)
            nat2 = -1
            first, second, common = x["fn"], x["sn"], x["cn"] or ""
            caps = False
        sid, pid = n6 + len(new_staff), n10 + len(new_player)
        struct.pack_into("<i", sr, 0, sid)
        struct.pack_into("<iii", sr, 4, name_id(13, first, nat), name_id(14, second, nat), name_id(15, common, nat))
        struct.pack_into("<H", sr, 0x18, 0)
        struct.pack_into("<ii", sr, 0x1a, nat, nat2)
        if not caps:
            sr[0x22] = sr[0x23] = 0
        struct.pack_into("<i", sr, 0x24, -1)
        put_date(sr, 0x29, EMPTY)
        put_date(sr, 0x31, EMPTY)
        set_club(sr, x, club)
        struct.pack_into("<i", sr, 0x4e, x["wage"])
        scale = ca / sca if sca > 0 else 1
        struct.pack_into("<i", sr, 0x52, max(0, int(i32(sr, 0x52) * scale * scale)))
        sr[0x5f] = 2
        struct.pack_into("<iii", sr, 0x61, pid, sid, -1)
        struct.pack_into("<i", sp, 0, pid)
        sp[4] = 0                                   # no squad number: a copied one can clash in the new club
        reps = struct.unpack_from("<hhh", sp, 9)
        struct.pack_into("<hhhhh", sp, 5, ca, pa, *[max(1, min(200, round(v * scale))) for v in reps])
        pref = bytearray(b"\xff" * 52)
        struct.pack_into("<i", pref, 0, sid)
        new_staff.append(sr)
        new_player.append(sp)
        new_pref.append(pref)
        to_list[club].append(sid)
        row.update(ca=ca, pa=pa, how=how, pos=list(sp[0x0f:0x1b]))
        report.append(row)

    # staff.dat: staff rows, non-player rows, player rows, preferences rows, in that order (as fill.py).
    idx = T.idx
    sb = T.raw["staff.dat"]
    o9, o10, o22 = idx[9]["off"], idx[10]["off"], idx[22]["off"]
    b6 = bytes(sb[:n6 * 110]) + b"".join(new_staff)
    b9 = bytes(sb[o9:o9 + n9 * 68])
    b10 = bytes(sb[o10:o10 + n10 * 70]) + b"".join(new_player)
    b22 = bytes(sb[o22:o22 + n22 * 52]) + b"".join(new_pref)
    ib = T.raw["index.dat"]
    set_count(ib, 6, n6 + len(new_staff), 0)
    set_count(ib, 9, n9, len(b6))
    set_count(ib, 10, n10 + len(new_player), len(b6) + len(b9))
    set_count(ib, 22, n22 + len(new_pref), len(b6) + len(b9) + len(b10))
    T.raw["staff.dat"] = bytearray(b6 + b9 + b10 + b22)
    for t, f in ((13, "first_names.dat"), (14, "second_names.dat"), (15, "common_names.dat")):
        cnt = idx[t]["count"]
        T.raw[f] = bytearray(bytes(T.raw[f][:cnt * 60]) + b"".join(new_names[t]))
        set_count(ib, t, cnt + len(new_names[t]))
    T.reindex()

    # Squad lists (club +0xd7, 50 ids, sorted by surname as convert.py does). A player who would make a list longer
    # than 50 leaves the club. A player already in the list is not added again.
    staff = T.rows(6)
    sn = {i32(r, 0x33): cstr(r, 0, 51) for r in T.rows(14)}
    cb = T.raw["club.dat"]
    full = []
    for club, ids in sorted(to_list.items()):
        o = club * 581
        if i32(cb, o) != club:
            sys.exit(f"keyplayers: club row {club} is not at its place")
        cur = [i32(cb, o + 0xd7 + 4 * j) for j in range(50)]
        cur = list(dict.fromkeys(v for v in cur if v >= 0))
        for sid in ids:
            if sid in cur:
                continue
            if len(cur) >= 50:
                full.append((cname[club], sid))
                struct.pack_into("<i", staff[sid], 0x39, -1)
                put_date(staff[sid], 0x46, EMPTY)
                continue
            cur.append(sid)
        cur.sort(key=lambda t: sn.get(i32(staff[t], 0x08), ""))
        for j in range(50):
            struct.pack_into("<i", cb, o + 0xd7 + 4 * j, cur[j] if j < len(cur) else -1)
    check_squad_lists(T.rows(0), T.rows(6))
    for f, b in T.raw.items():
        with open(os.path.join(dst, f), "wb") as h:
            h.write(b)
    cols = ["fm", "name", "club", "age", "wage", "action", "src", "fm_ca", "fm_pa", "ca", "pa", "how", "pos", "note"]
    with open(os.path.join(W, "keyplayers-report.tsv"), "w", encoding="utf-8") as h:
        h.write("\t".join(cols) + "\n")
        for r in report:
            h.write("\t".join(",".join(map(str, r[c])) if c == "pos" else str(r.get(c, "")) for c in cols) + "\n")
    print(f"{Y}: {len(report)} key players written, {len(new_staff)} new rows, "
          f"{sum(len(v) for v in new_names.values())} new names, full squads: {full}")
    for r in report:
        print(f"  {r['name']:<28} {r['club'] or '-':<26} age {r['age']:>2} wage {r['wage']:>6} {r['action']:<8} "
              f"FM {r['fm_ca']}/{r['fm_pa']} -> CA {r['ca']} PA {r['pa']} | {r['how']}")


if __name__ == "__main__":
    main(sys.argv[1:])
