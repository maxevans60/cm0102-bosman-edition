"""Read the facts of Football Manager 2007's database (db/700 of the disc's setup.jar) into pickles for the own_db
converter.

Usage: python3 -I read_fm2007.py <FM2007 db700 folder> <facts folder>

The db700 folder holds client_db.dat, server_db.dat and people_db.dat, unpacked from setup.jar as data with
unpack_setup_jar.py. Nothing from the disc runs. The files are untrusted input: every read is bounds-checked, and a
file that does not have the expected shape stops the script. Facts only: names, dates of birth, nations, clubs,
jobs, contracts, final tables and competition winners. No rating, attribute, ability or position is read.

Output (the facts folder): nations, clubs, people_all, teammap, clubhist and comphist .pkl.

Format (file version 0x042c; plain binary, little-endian, text UTF-16LE with a u32 length; a date is a u16 day of
the year from 0 and a u16 year, 1900 for no date). Found in the files with known people and clubs:
- Name record (client_db): u32 n, n chars, u16 0, u8, u16, i32 nation, u32 index, u32 uid, u8 0. Four runs of
  1,000 or more: first names (in two runs), surnames, common names.
- Person (people_db): u32 first, u32 surname, i32 common (-1 none), u32 n and n chars of the full name (then u16 0
  when n > 0), u16 day, u16 year, u8, u32 nation. The 6 bytes before the person are 6c 07 ff ff ff ff for a player
  and ff ff ff ff 00 00 for a non-player.
- Join block, in the 80 bytes after the nation: 6c 07, u32, u8 job (bit flags: 1 player, 2 coach, 12 physio, 14
  scout, 16 manager, 20 assistant manager), u16 day, u16 year (joined), 00.
- Contract block, up to 400 bytes after the nation: 64, u32 person id, 00, u32 club, u32 weekly wage, u16 day, u16
  year (end, 2006 to 2016 or 1900), 02.
- Club record (client_db): long name, short name, u8 k kits, 7 bytes per kit, u32 club id, u32 id2, u8 0, then the
  next record's i32 nation. A club's nation is therefore read at the end of the club before it. The first club
  (KS Albpetrol Patos) has nation 126 (Albania).
- Nation records: the same shape, between the end of the club table and the first name table.
- Server club record: the long and short name, the kits, then u8 n and n u32 team ids (first team first).
- STARTING_CLUB_HISTORY (server_db), 30 bytes: u8 0, u32 competition, u32 team, u16 season end year, u8 position,
  u8 played, won, drawn, lost, u16 goals for, against, points, u32 id, u32 uid.
- COMP_HISTORY (server_db), 39 bytes: u8 0, u32 competition, u16 year, 6 u32 teams (first, second, third, ...),
  u32 id, u32 uid.

Checks printed at the end: Rooney (Manchester United), Mourinho (manager of Chelsea), the 2005/06 Premier League
top three.
"""
import collections
import datetime
import os
import pickle
import re
import struct
import sys

PLAYER, STAFF = bytes.fromhex("6c07ffffffff"), bytes.fromhex("ffffffff0000")
NATION_SCAN_FROM = 0x2f0000   # the nation scan starts inside the last club records, as the first reader did
FIRST_CLUB, FIRST_CLUB_NATION = "KS Albpetrol Patos", 126
PREMIER, CHELSEA_TEAM, SEASON = 7, 543, 2006   # anchors: the 2005/06 Premier League table and winners


def need(cond, msg):
    if not cond:
        sys.exit("read_fm2007: " + msg)


def u(fmt, d, o):
    """struct.unpack_from with a bounds check (a negative offset would read from the end)."""
    need(0 <= o and o + struct.calcsize(fmt) <= len(d), f"read {fmt} at {o:#x} is outside the file")
    return struct.unpack_from(fmt, d, o)


def byte(d, o):
    need(0 <= o < len(d), f"byte at {o:#x} is outside the file")
    return d[o]


def ustr(d, o, most):
    """(text, end) of a u32-length UTF-16 string at o whose text is followed by u16 0, or None."""
    if o < 0 or o + 4 > len(d):
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
    """Runs of name records whose index goes up by 1: [(start, rows)], row = (text, index, end)."""
    def rec(o):
        s = ustr(c, o, 100)
        if not s or s[1] + 16 > len(c):
            return None
        e = s[1] - 2
        return s[0], struct.unpack_from("<I", c, e + 9)[0], e + 18
    out, o, L = [], 0, len(c)
    while o < L - 30:
        r = rec(o)
        if r:
            r2 = rec(r[-1])
            if r2 and r2[1] == r[1] + 1:
                start, rows = o, []
                while r and (not rows or r[1] == rows[-1][1] + 1):
                    rows.append(r)
                    o = r[-1]
                    r = rec(o)
                out.append((start, rows))
                continue
        o += 1
    return [t for t in out if len(t[1]) >= 1000]


def date(d, o):
    day, y = u("<HH", d, o)
    if y in (0, 1900) or day > 365:
        return None
    try:
        return datetime.date(y, 1, 1) + datetime.timedelta(days=day)
    except (ValueError, OverflowError):
        return None


def people(p, F, S, C):
    """Every person: names, full name, date of birth, nation, role, job, joined date and the contract block."""
    found, o, L = [], 0, len(p)
    while o < L - 120:
        fn, sn, cn, sl = struct.unpack_from("<IIiI", p, o)
        if fn in F and sn in S and (cn == -1 or cn in C) and sl < 60:
            q = o + 16 + 2 * sl + (2 if sl else 0)
            if q + 9 <= L:
                day, yr = struct.unpack_from("<HH", p, q)
                if 1900 <= yr <= 1995 and day < 366:
                    nat = struct.unpack_from("<I", p, q + 5)[0]
                    full = p[o + 16:o + 16 + 2 * sl].decode("utf-16-le", "replace")
                    try:
                        dob = datetime.date(yr, 1, 1) + datetime.timedelta(days=day)
                    except (ValueError, OverflowError):
                        dob = None
                    found.append(dict(off=o, fn=F[fn], sn=S[sn], cn=C.get(cn), full=full, dob=dob, nat=nat, q=q))
                    o = q + 9
                    continue
        o += 1
    P = [x for x in found if x["off"] >= 6 and p[x["off"] - 6:x["off"]] in (PLAYER, STAFF)]
    for i, x in enumerate(P):
        x["role"] = "player" if p[x["off"] - 6:x["off"]] == PLAYER else "staff"
        q = x["q"]
        end = min(P[i + 1]["off"] if i + 1 < len(P) else q + 400, q + 400, L)
        x["job"] = x["joined"] = x["club"] = x["wage"] = x["cend"] = x["pid"] = None
        m = re.search(rb"(?s)\x6c\x07....(.)(....)\x00", p[q + 9:q + 80])
        if m:
            x["job"] = m.group(1)[0]
            x["joined"] = date(p, q + 9 + m.start(2))
        for k in range(q + 9, end - 20):
            if p[k] == 0x64 and p[k + 5] == 0:
                club, wage, day, yr = struct.unpack_from("<IIHH", p, k + 6)
                if (2006 <= yr <= 2016 or yr == 1900) and day < 366 and p[k + 18] == 2:
                    x["pid"] = struct.unpack_from("<I", p, k + 1)[0]
                    x["club"], x["wage"], x["cend"] = club, wage, date(p, k + 14)
                    break
    return P


def clubs_of(c):
    """club id -> dict(long, short, nat, id2, off, nat_tail). nat_tail is the nation of the next record."""
    clubs, o, L = {}, 0, len(c)
    while o < L - 20:
        a = ustr(c, o, 80)
        if a:
            b = ustr(c, a[1], 80)
            if b:
                k = byte(c, b[1])
                t = b[1] + 1 + 7 * k
                if 0 <= k <= 8 and t + 14 <= L and c[t + 8] == 0:
                    cid, id2 = struct.unpack_from("<II", c, t)
                    nat = struct.unpack_from("<i", c, t + 9)[0]
                    if cid < 200000 and -1 <= nat < 400:
                        clubs.setdefault(cid, dict(long=a[0], short=b[0], nat=nat, id2=id2, off=o))
                        o = t + 13
                        continue
        o += 1
    need(clubs, "no club records")
    order = sorted(clubs, key=lambda k: clubs[k]["off"])
    need(clubs[order[0]]["long"] == FIRST_CLUB, f"the first club is not {FIRST_CLUB}")
    prev = {b: clubs[a]["nat"] for a, b in zip(order, order[1:])}
    for k in order:
        clubs[k]["nat_tail"] = clubs[k]["nat"]
    for k in order:
        clubs[k]["nat"] = prev.get(k, FIRST_CLUB_NATION)
    return clubs


def nations_of(c, start, end):
    need(0 <= start < end <= len(c), "nation scan range is outside the file")
    nat = {}
    for o in range(start, end):
        a = ustr(c, o, 60)
        if not a:
            continue
        b = ustr(c, a[1], 60)
        if not b:
            continue
        k = byte(c, b[1])
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
        t = e + 1 + 7 * byte(s, e)
        n = byte(s, t)
        if 1 <= n <= 8:
            for j in range(n):
                tm[u("<I", s, t + 1 + 4 * j)[0]] = cid
            pos = i
    return tm


def club_history(s):
    """STARTING_CLUB_HISTORY rows around the 2005/06 Premier League champion's row."""
    def ok(o):
        return 0 <= o and o + 30 <= len(s) and s[o] == 0 and 1850 <= struct.unpack_from("<H", s, o + 9)[0] <= 2007
    a = s.find(struct.pack("<IIHB", PREMIER, CHELSEA_TEAM, SEASON, 1))
    need(a > 0, "the club history anchor row is not in server_db.dat")
    o = a - 1
    while ok(o - 30) and s[o - 30 + 12] == s[o - 30 + 13] + s[o - 30 + 14] + s[o - 30 + 15]:
        o -= 30
    rows = []
    while ok(o):
        comp, team, yr = struct.unpack_from("<IIH", s, o + 1)
        pos, pl, w, dr, lo = s[o + 11:o + 16]
        gf, ga, pts = struct.unpack_from("<HHH", s, o + 16)
        rows.append((comp, team, yr, pos, pl, w, dr, lo, gf, ga, pts))
        o += 30
    return rows


def comp_history(s):
    """COMP_HISTORY rows around the 2005/06 Premier League row."""
    def ok(o):
        return 0 <= o and o + 39 <= len(s) and s[o] == 0 and 1850 <= struct.unpack_from("<H", s, o + 5)[0] <= 2007
    a = s.find(struct.pack("<IHI", PREMIER, SEASON, CHELSEA_TEAM))
    need(a > 0, "the competition history anchor row is not in server_db.dat")
    o = a - 1
    while ok(o - 39):
        o -= 39
    rows = []
    while ok(o):
        comp, yr = struct.unpack_from("<IH", s, o + 1)
        rows.append((comp, yr, struct.unpack_from("<6I", s, o + 7)))
        o += 39
    return rows


def main(argv):
    need(len(argv) == 2, "usage: read_fm2007.py <FM2007 db700 folder> <facts folder>")
    src, out = argv
    os.makedirs(out, exist_ok=True)

    def rd(n):
        with open(os.path.join(src, n), "rb") as f:
            return f.read()
    c, s, p = rd("client_db.dat"), rd("server_db.dat"), rd("people_db.dat")
    for n, d in (("client_db", c), ("server_db", s), ("people_db", p)):
        need(d[:6] == b"\x02\x01tad." and u("<H", d, 6)[0] == 0x042c, f"{n}.dat is not the FM2007 format")

    T = name_tables(c)
    need(len(T) == 4, f"expected 4 name runs, found {len(T)}")
    F = {r[1]: r[0] for r in T[0][1] + T[1][1]}
    S = {r[1]: r[0] for r in T[2][1]}
    C = {r[1]: r[0] for r in T[3][1]}
    clubs = clubs_of(c)
    nations = nations_of(c, NATION_SCAN_FROM, T[0][0])
    P = people(p, F, S, C)
    teams = teams_of(s, clubs)
    hist, comphist = club_history(s), comp_history(s)
    for n, v in (("nations", nations), ("clubs", clubs), ("people_all", P), ("teammap", teams),
                 ("clubhist", hist), ("comphist", comphist)):
        with open(os.path.join(out, n + ".pkl"), "wb") as f:
            pickle.dump(v, f)
    print(f"names {len(F)}/{len(S)}/{len(C)}, nations {len(nations)}, clubs {len(clubs)}, people {len(P)} "
          f"({sum(x['club'] is not None for x in P)} with a club), teams {len(teams)}, table rows {len(hist)}, "
          f"competition winners {len(comphist)}")
    print("jobs", collections.Counter(x["job"] for x in P if x["club"] is not None).most_common(10))
    for x in P:
        if (x["sn"], x["dob"] and x["dob"].year) in (("Rooney", 1985), ("Mourinho", 1963)) and x["club"] in clubs:
            print(" ", x["fn"], x["sn"], x["dob"], clubs[x["club"]]["long"], "job", x["job"], "joined", x["joined"])
    top = sorted((r for r in hist if r[0] == PREMIER and r[2] == SEASON), key=lambda r: r[3])[:3]
    print("  Premier 2005/06:", [(clubs[teams[r[1]]]["long"], r[10]) for r in top if teams.get(r[1]) in clubs])


if __name__ == "__main__":
    main(sys.argv[1:])
