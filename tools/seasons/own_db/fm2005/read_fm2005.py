"""Read the facts of Football Manager 2005's database (data/db of the disc) into pickles for the own_db converter.

Usage: python3 -I read_fm2005.py <FM2005 db folder> <facts folder>

The db folder holds client_db.dat, server_db.dat and people_db.dat, copied from the disc as data. Nothing from the
disc runs. Facts only: names, dates of birth, nations, clubs, jobs, contracts, final tables and competition
winners. No rating, attribute, ability or position is read.

Format (file version 0xf0; plain binary, little-endian, text UTF-16LE with a u32 length; a date is a u16 day of
the year from 0 and a u16 year, 1900 for no date). Found in the files with known people and clubs:
- Name record (client_db): u32 n, n chars, u16 0, u16, i32 nation, u32 index, u32 uid, u8 0. Three tables:
  first names (in two runs), surnames, common names.
- Person (people_db): u32 first, u32 surname, i32 common (-1 none), u16 day, u16 year, u8, u32 nation; bytes +33 to
  +37 are 00 00 00 6c 07. At +38: u32 team id, u8 job (bit flags: 1 player, 2 coach, 12 physio, 14 scout,
  16 manager, 20 assistant manager), u16 day, u16 year (joined), 00. After it the contract (the date +83 for most people; optional blocks move it): u32 person
  id, 00, u32 club, u32 weekly wage, u16 day, u16 year (end), 02.
- Club record (client_db): ..., long name, short name, u8 k kits, 7 bytes per kit, u32 club id, u32 id2, then the
  next record (type byte, i32 nation). A club's nation is therefore read at the end of the club before it.
- Nation records: the same shape, after the club table and before the first name table.
- Server club record: after the kits, u8 n and n u32 team ids (first team first).
- STARTING_CLUB_HISTORY (server_db), 30 bytes: u8 0, u32 competition, u32 team, u16 season end year, u8 position,
  u8 played, won, drawn, lost, u16 goals for, against, points, u32 id, u32 uid.
- COMP_HISTORY (server_db), 39 bytes: u8 0, u32 competition, u16 year, 6 u32 teams (first, second, third, ...),
  u32 id, u32 uid.

Checks printed at the end: Rooney (Manchester United, joined 31.8.2004), Mourinho (manager of Chelsea), the
2003/04 Premier League top three.
"""
import collections
import datetime
import os
import pickle
import struct
import sys


def need(cond, msg):
    if not cond:
        sys.exit("read_fm2005: " + msg)


def ustr(d, o, most=100):
    """(text, end) of a u32-length UTF-16 string at o whose text is followed by u16 0, or None."""
    if o + 4 > len(d):
        return None
    n = struct.unpack_from("<I", d, o)[0]
    if not 1 <= n <= most:
        return None
    e = o + 4 + 2 * n
    if e + 2 > len(d) or d[e:e + 2] != b"\0\0":
        return None
    try:
        t = d[o + 4:e].decode("utf-16-le")
    except UnicodeDecodeError:
        return None
    return (t, e + 2) if t.isprintable() else None


def name_tables(c):
    """Runs of name records whose index goes up by 1: [(start, rows)], row = (text, nation, index)."""
    def rec(o):
        s = ustr(c, o)
        if not s or s[1] + 15 > len(c):
            return None
        e = s[1]
        nat, idx = struct.unpack_from("<iI", c, e + 2)
        return s[0], nat, idx, e + 15
    out, o, L = [], 0, len(c)
    while o < L - 30:
        r = rec(o)
        if r:
            r2 = rec(r[-1])
            if r2 and r2[2] == r[2] + 1:
                start, rows = o, []
                while r and (not rows or r[2] == rows[-1][2] + 1):
                    rows.append(r)
                    o = r[-1]
                    r = rec(o)
                out.append((start, rows))
                continue
        o += 1
    return [t for t in out if len(t[1]) >= 50]


def date(d, o):
    day, y = struct.unpack_from("<HH", d, o)
    if y in (0, 1900) or day > 365:
        return None
    try:
        return datetime.date(y, 1, 1) + datetime.timedelta(days=day)
    except (ValueError, OverflowError):
        return None


def people(p, F, S, C):
    out, o, L = [], 0, len(p)
    while o < L - 120:
        fn, sn, cn = struct.unpack_from("<IIi", p, o)
        if fn in F and sn in S and (cn == -1 or cn in C):
            day, yr = struct.unpack_from("<HH", p, o + 12)
            if 1900 <= yr <= 1995 and day < 366 and p[o + 33:o + 38] == b"\0\0\0\x6c\x07":
                j = o + 38
                x = dict(fn=F[fn], sn=S[sn], cn=C.get(cn), full="", dob=date(p, o + 12),
                         nat=struct.unpack_from("<I", p, o + 17)[0], team=struct.unpack_from("<I", p, j)[0],
                         job=p[j + 4], joined=date(p, j + 5), club=None, wage=None, cend=None, pid=None)
                x["off"] = o
                x["role"] = "player" if x["job"] & 1 else "staff"
                out.append(x)
                o += 38
                continue
        o += 1
    # The contract block lies after the join block; optional blocks before it change its offset (+83 from the date
    # for most people, +87 for some). Search the record up to the next person.
    for n, x in enumerate(out):
        q = x["off"] + 12
        end = min(out[n + 1]["off"] if n + 1 < len(out) else len(p), q + 400)
        for k in range(q + 40, end - 18):
            if p[k + 4] == 0 and p[k + 17] == 2:
                club, wage, d_, y_ = struct.unpack_from("<IIHH", p, k + 5)
                if (2004 <= y_ <= 2014 or y_ == 1900) and d_ < 366 and club < 100000 and wage < 10 ** 7:
                    x.update(pid=struct.unpack_from("<I", p, k)[0], club=club, wage=wage, cend=date(p, k + 13))
                    break
    return out


def clubs_of(c):
    """club id -> dict(long, short, nat, id2, off). Nation: see the module doc."""
    clubs, o, L = {}, 0, len(c)
    while o < L - 20:
        a = ustr(c, o, 80)
        if a:
            b = ustr(c, a[1], 80)
            if b:
                k = c[b[1]]
                t = b[1] + 1 + 7 * k
                if 0 <= k <= 8 and t + 14 <= L and c[t + 8] == 0:
                    cid, id2 = struct.unpack_from("<II", c, t)
                    nxt = struct.unpack_from("<i", c, t + 9)[0]
                    if cid < 200000 and -1 <= nxt < 400:
                        clubs.setdefault(cid, dict(long=a[0], short=b[0], id2=id2, off=o, nxt=nxt))
                        o = t + 13
                        continue
        o += 1
    order = sorted(clubs, key=lambda i: clubs[i]["off"])
    for i in clubs.values():
        i["nat"] = None
    for a, b in zip(order, order[1:]):
        clubs[b]["nat"] = clubs[a]["nxt"]
    return clubs


def nations_of(c, start, end):
    nat = {}
    for o in range(start, end):
        a = ustr(c, o, 60)
        if not a:
            continue
        b = ustr(c, a[1], 60)
        if not b:
            continue
        k = c[b[1]]
        t = b[1] + 1 + 7 * k
        if 1 <= k <= 4 and t + 9 <= len(c):
            nid = struct.unpack_from("<I", c, t)[0]
            if nid < 400 and c[t + 8] == 0:
                nat.setdefault(nid, (a[0], b[0]))
    return nat


def teams_of(s, clubs):
    def enc(w):
        return struct.pack("<I", len(w)) + w.encode("utf-16-le") + b"\0\0"
    tm, pos = {}, 0
    for cid in sorted(clubs):
        cl = clubs[cid]
        pat = enc(cl["long"]) + enc(cl["short"])
        i = s.find(pat, pos)
        if i < 0:
            i = s.find(pat)
        if i < 0:
            continue
        e = i + len(pat)
        t = e + 1 + 7 * s[e]
        n = s[t]
        if 1 <= n <= 8:
            for j in range(n):
                tm[struct.unpack_from("<I", s, t + 1 + 4 * j)[0]] = cid
            pos = i
    return tm


def run_of(d, anchor, size, idoff):
    """Rows of a fixed-size table around a known row: type byte 0 and ids that go up by 1."""
    need(anchor >= 0, "anchor row not found")
    rid = lambda o: struct.unpack_from("<I", d, o + idoff)[0]  # noqa: E731
    o = anchor
    while d[o - size] == 0 and rid(o - size) + 1 == rid(o):
        o -= size
    rows = []
    while d[o] == 0 and (not rows or rid(o) == rid(o - size) + 1):
        rows.append(o)
        o += size
    return rows


def main(argv):
    need(len(argv) == 2, "usage: read_fm2005.py <FM2005 db folder> <facts folder>")
    src, out = argv
    os.makedirs(out, exist_ok=True)
    rd = lambda n: open(os.path.join(src, n), "rb").read()  # noqa: E731
    c, s, p = rd("client_db.dat"), rd("server_db.dat"), rd("people_db.dat")
    need(c[:6] == b"\x02\x01tad." and c[6] == 0xf0, "client_db.dat is not the FM2005 format")

    T = name_tables(c)
    need(len(T) == 4, f"expected 4 name runs, found {len(T)}")
    F = {r[2]: r[0] for r in T[0][1] + T[1][1]}
    S = {r[2]: r[0] for r in T[2][1]}
    C = {r[2]: r[0] for r in T[3][1]}
    clubs = clubs_of(c)
    nations = nations_of(c, max(x["off"] for x in clubs.values()), T[0][0])   # after the club table
    P = people(p, F, S, C)
    teams = teams_of(s, clubs)
    # A person with no contract block but a team in the join block plays for that team's club (no contract end).
    fall = 0
    for x in P:
        if x["club"] is None and x["team"] in teams and x["joined"]:
            x["club"] = teams[x["team"]]
            fall += 1
    print(f"club from the join block's team: {fall}")
    hist = []
    for o in run_of(s, s.find(struct.pack("<IIHB", 7, 516, 2004, 1)) - 1, 30, 22):
        comp, team, yr, pos, g, w, dr, l, gf, ga, pts = struct.unpack_from("<IIHBBBBBHHH", s, o + 1)
        hist.append((comp, team, yr, pos, g, w, dr, l, gf, ga, pts))
    comphist = []
    for o in run_of(s, s.find(struct.pack("<IHIII", 7, 2004, 516, 544, 593)) - 1, 39, 31):
        comp, yr = struct.unpack_from("<IH", s, o + 1)
        comphist.append((comp, yr, struct.unpack_from("<6I", s, o + 7)))
    for n, v in (("nations", nations), ("clubs", clubs), ("people_all", P), ("teammap", teams),
                 ("clubhist", hist), ("comphist", comphist)):
        with open(os.path.join(out, n + ".pkl"), "wb") as f:
            pickle.dump(v, f)
    print(f"names {len(F)}/{len(S)}/{len(C)}, nations {len(nations)}, clubs {len(clubs)}, people {len(P)} "
          f"({sum(x['club'] is not None for x in P)} with a club), teams {len(teams)}, table rows {len(hist)}, "
          f"competition winners {len(comphist)}")
    print("jobs", collections.Counter(x["job"] for x in P).most_common(10))
    for x in P:
        if (x["sn"], x["dob"] and x["dob"].year) in (("Rooney", 1985), ("Mourinho", 1963)) and x["club"]:
            print(" ", x["fn"], x["sn"], x["dob"], clubs[x["club"]]["long"], "job", x["job"], "joined", x["joined"])
    top = sorted((r for r in hist if r[0] == 7 and r[2] == 2004), key=lambda r: r[3])[:3]
    print("  Premier 2003/04:", [(clubs[teams[r[1]]]["long"], r[10]) for r in top])


if __name__ == "__main__":
    main(sys.argv[1:])
