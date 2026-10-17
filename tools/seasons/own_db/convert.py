"""Build a CM 01/02 3.9.68 season database from an Update Team template (ratings, positions, names, key names)
and FM facts (who plays and works where, contracts, the final tables of the season before).

Usage: python3 -I convert.py <season start year> <template folder> <facts folder> <work folder> <history folder>
       <new folder>

Run match.py and leagues.py first (they write match.pkl and leagues.pkl into the work folder). The season settings
are season_<year>.py. The template and the history folder (an Update Team database whose competition history
covers the seasons up to the start, settings HIST_SRC) are read only. The new folder must not exist. It gets the
20 .dat files and index.dat, and no .cfg file: the game reads a file from a database folder only when the folder
has it (src/recomp_rt/win/files.c), so it uses its own .cfg files. The work runs in real years. The last step
stores the dates the Update Team way, as for a 2001 start (Q381): see store_update_team_way.

What changes:
- Dates. The template stores the dates of a 2001 start, early by a number of years per field (the date shift
  research: birth, joined, contract, careers except the kept careers). They get their real years. Rows of the
  start season and later go: careers from the start year, competition history after the start year.
- People. A template person matched to an FM person (match.py) gets the FM club and job, joined date and
  contract end. A matched person whom FM has at no club, or at a club that the template lacks, has no club.
  An unmatched template player at a club that FM covers has no club: FM does not have him there.
  His date of birth is FM's (the template's is sometimes one or two years out).
  Ratings, positions and every other field of the template stay.
- Clubs. Current division and last division and position from leagues.pkl. Manager (+0xcf) and assistant
  manager (+0xd3) follow the staff. The squad list (+0xd7, 50 ids) is rebuilt for each club that has one; the
  staff lists lose people who left.
- Competition history. The template's last champions (its 2001 rows of the competitions it updated) go. The
  seasons from 2000/01 to the one before the start come from FM where it has them, else from the history folder,
  for club and national competitions.
"""
import datetime
import os
import pickle
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from cmdb import Folder, cstr  # noqa: E402

from seasons import settings, template_shifts, kept_careers, last_row_comps, repair_clubs  # noqa: E402

S_ = START = SH = HIST_SHIFT = None   # set in main from the season settings and the date shift research
AFTER_START = set()             # national competitions of the start year played after the start (filled in main)
DAT_FILES = ["city.dat", "club.dat", "club_comp.dat", "club_comp_history.dat", "colour.dat", "common_names.dat",
             "continent.dat", "first_names.dat", "index.dat", "nat_club.dat", "nation.dat", "nation_comp.dat",
             "nation_comp_history.dat", "officials.dat", "second_names.dat", "stadium.dat", "staff.dat",
             "staff_comp.dat", "staff_comp_history.dat", "staff_history.dat"]
JOBS = {16: 5, 20: 6, 2: 8, 34: 8, 48: 8, 14: 9, 12: 10}     # FM job -> 3.9.68 job (FM2005 and FM use the same codes)
CUM = [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334]
EMPTY = (31, 1900)


def check(cond, msg):
    if not cond:
        sys.exit("convert: " + msg)


def cm_date(d):
    """A real date as the 3.9.68 data writes it: day of a common year, the year, flag 0."""
    if d is None:
        return EMPTY
    day = CUM[d.month - 1] + min(d.day, 28 if d.month == 2 else 31) - 1
    return (day, d.year)


def put_date(rec, off, dy):
    struct.pack_into("<hHi", rec, off, dy[0], dy[1], 0)


def shift(rec, off, n):
    y = struct.unpack_from("<H", rec, off)[0]
    if y >= 1901:
        struct.pack_into("<H", rec, off, y + n)


def set_count(buf, table, count):
    for i in range(8, len(buf) - 66, 67):
        if struct.unpack_from("<i", buf, i + 51)[0] == table:
            struct.pack_into("<i", buf, i + 55, count)
            return
    check(False, f"index.dat has no table {table}")


def check_squad_lists(clubs, staff):
    """Every squad list entry (club +0xd7, 50 ids) must name a player whose staff club (+0x39) is that club. The
    computer's team pick 0x6dbc50 joins a club's list with the list of its linked B team (0x540a50) and stops with
    "match_man..cpp 1225" (check at 0x6dd3a4) when one player comes twice."""
    bad = []
    for r in clubs:
        c = struct.unpack_from("<i", r, 0)[0]
        for k in range(50):
            t = struct.unpack_from("<i", r, 0xd7 + 4 * k)[0]
            if t >= 0 and (t >= len(staff) or struct.unpack_from("<i", staff[t], 0x39)[0] != c):
                bad.append((c, t))
    check(not bad, f"{len(bad)} squad list entries name a player of another club or none, "
                   f"for example {bad[:5]}")


def store_update_team_way(T, k):
    """Q381 (Max, 2026-10-17): the dates as an Update Team database of the start season stores them, early by
    k = start year - 2001, so the database works in three ways: as a known season database (its signature row
    carries these shifts), as a Custom database (the game measures the birth shift k against its own data and
    adds it back, src/recomp_rt/win/season_dates.c plan_for) and in the original game, which always starts in
    2001. Dates of birth (+0x10, age +0x18), joined dates (+0x3e), contract ends (+0x46) and national job dates
    (+0x29, +0x31), and the career years (staff_history +8), move k years back. A contract end that would land in
    2010 or later keeps its real year: the game reads an end of 2010 or later as a real year (REAL_CONTRACT_FROM).
    Competition history and officials keep their real years (the rules for a database before 2010/11)."""
    if k <= 0:
        return
    for r in T.rows(6):
        for off in (0x12, 0x18, 0x40):
            shift(r, off, -k)
        for off in (0x2b, 0x33, 0x48):
            y = struct.unpack_from("<H", r, off)[0]
            if y >= 1901 and y - k < 2010:
                struct.pack_into("<H", r, off, y - k)
    sh = T.raw["staff_history.dat"]
    for o in range(0, len(sh) - 16, 17):          # the file holds exactly the rows (step 4 wrote it)
        shift(sh, o + 8, -k)
    print(f"dates stored the Update Team way: {k} years early")


def main(argv):
    global S_, START, SH, HIST_SHIFT
    check(len(argv) in (6, 7), "usage: convert.py <season> <template> <facts> <work> <history folder> <new folder> "
                               "[<fill folder>]")
    S_ = settings(int(argv[0]))
    START = S_.START
    SH = template_shifts(S_.TEMPLATE)
    HIST_SHIFT = template_shifts(S_.HIST_SRC)["comp_history"]
    src, X, W, hist_src, dst = (os.path.abspath(a) for a in argv[1:6])
    fill_src = os.path.abspath(argv[6]) if len(argv) == 7 else None
    check(not os.path.exists(dst), f"{dst} exists")
    T = Folder(src)
    repair_clubs(T, S_)
    for f in DAT_FILES:
        check(f in T.raw, f"{f} missing in the template")
    idx = T.idx
    check(idx[6]["ver"] == 2 and idx[6]["off"] == 0, "staff table 6 is not version 2 at offset 0")
    M = pickle.load(open(os.path.join(W, "match.pkl"), "rb"))
    L = pickle.load(open(os.path.join(W, "leagues.pkl"), "rb"))
    fp = pickle.load(open(os.path.join(X, "people_all.pkl"), "rb"))
    comphist = pickle.load(open(os.path.join(X, "comphist.pkl"), "rb"))
    teammap = pickle.load(open(os.path.join(X, "teammap.pkl"), "rb"))
    clubmap, pmap = M["clubmap"], M["pmap"]
    n_template = idx[6]["count"]
    if fill_src:
        from fill import fill                       # Q382: FM players that the template lacks, from an older database
        pmap.update(fill(T, Folder(fill_src), fp, M, L, S_))
        idx = T.idx

    staff = T.rows(6)
    n_staff = len(staff)
    g = lambda r, o: struct.unpack_from("<i", r, o)[0]  # noqa: E731

    # 1. Real years for every staff row.
    for r in staff:
        for off, n in ((0x12, SH["birth"]), (0x18, SH["birth"]), (0x2b, SH["contract"]), (0x33, SH["contract"]),
                       (0x40, SH["joined"]), (0x48, SH["contract"])):
            shift(r, off, n)
        d, y = struct.unpack_from("<hH", r, 0x3e)
        if y > START or (y == START and d > 212):
            put_date(r, 0x3e, (181, START))          # a 2001 join of the template lands after the start
    # 2. People: FM club, job, joined date and contract.
    by_t = {t: fp[i] for i, t in pmap.items()}
    covered = set(clubmap.values())                 # template clubs that FM knows
    moved = 0
    for t, x in by_t.items():
        r = staff[t]
        if x["dob"] and x["dob"].year > 1900:
            d = cm_date(x["dob"])
            put_date(r, 0x10, d)                     # FM's date of birth: the template's is sometimes a year out
        kind = r[0x5f]
        if kind == 3:
            continue                                 # player-managers keep their template jobs
        if x["role"] == "staff" and x["job"] not in JOBS and x["club"] is not None:
            continue                                 # a job with no 3.9.68 match: leave the template row
        if x["role"] == "staff" and kind == 2:
            struct.pack_into("<i", r, 0x39, -1)      # a template player whom FM has as staff: no club in the start season
            put_date(r, 0x46, EMPTY)
            moved += 1
            continue
        if x["role"] == "player" and kind == 1:
            continue                                 # FM has him as a player, the template as staff: leave it
        new = clubmap.get(x["club"], -1) if x["club"] is not None else -1
        old = g(r, 0x39)
        struct.pack_into("<i", r, 0x39, new)
        if new == -1:
            put_date(r, 0x46, EMPTY)
        else:
            put_date(r, 0x46, cm_date(x["cend"]) if x["cend"] and x["cend"].year > START else EMPTY)
            j = x["joined"]
            if j and j <= datetime.date(START, 8, 31):
                put_date(r, 0x3e, cm_date(j))
            elif new != old:
                put_date(r, 0x3e, cm_date(datetime.date(START, 7, 1)))
            if x["role"] == "staff":
                r[0x3d] = JOBS[x["job"]]
        moved += new != old
    gone = 0
    for t, r in enumerate(staff):
        if t in by_t or r[0x5f] != 2:
            continue
        if g(r, 0x39) in covered:                   # FM knows the club and does not have him there
            struct.pack_into("<i", r, 0x39, -1)
            put_date(r, 0x46, EMPTY)
            gone += 1
    print(f"people: {len(by_t)} matched, {moved} moved, {gone} unmatched players left their club")

    # 3. Clubs.
    # The game checks that every coach with a club is in the club's coach list (0x527240, Database.cpp line
    # 1583), and the lists have fixed sizes. So the lists are built from the staff rows, and people beyond a
    # list's size, or a second manager or assistant manager, lose the club (FM's people stay first).
    clubs = T.rows(0)
    by_club = {}
    for t, r in enumerate(staff):
        c = g(r, 0x39)
        if c >= 0:
            by_club.setdefault(c, []).append(t)
    sn = {struct.unpack_from("<i", r, 0x33)[0]: cstr(r, 0, 51) for r in T.rows(14)}
    born = lambda t: struct.unpack_from("<H", staff[t], 0x12)[0]  # noqa: E731

    def unemploy(t):
        struct.pack_into("<i", staff[t], 0x39, -1)
        put_date(staff[t], 0x46, EMPTY)

    lists, mgr, ass, cut = {}, {}, {}, 0
    for c, ts in by_club.items():
        fm_first = lambda t: (t not in by_t, t)  # noqa: E731
        players = sorted((t for t in ts if staff[t][0x5f] in (2, 3)), key=lambda t: (t not in by_t, born(t)))
        out = {}
        for key, job, size in (("p", None, 50), ("c", 8, 5), ("s", 9, 7), ("ph", 10, 3), ("m", 5, 1), ("a", 6, 1)):
            group = players if job is None else sorted((t for t in ts if staff[t][0x5f] in (1, 3) and staff[t][0x3d] == job), key=fm_first)
            for t in group[size:]:
                unemploy(t)
                cut += 1
            out[key] = group[:size]
        for key in out:                              # a player-coach cut from one list leaves the others too
            out[key] = [t for t in out[key] if g(staff[t], 0x39) == c]
        lists[c] = out
        mgr[c] = out["m"][0] if out["m"] else -1
        ass[c] = out["a"][0] if out["a"] else -1
    print(f"club lists: {cut} people beyond a list's size or a second manager left their club")
    for r in clubs:
        c = g(r, 0)
        cur = L["cur"].get(c, -1)
        if cur < 0:
            cur = g(r, 0x57)                         # no last division in the template: keep its division
        struct.pack_into("<i", r, 0x57, cur)
        if c in L["lastdiv"]:
            div, pos = L["lastdiv"][c]
            struct.pack_into("<i", r, 0x5b, div)
            r[0x5f] = pos
        else:
            struct.pack_into("<i", r, 0x5b, cur)
        ls = lists.get(c, {})
        for key, off, n in (("p", 0xd7, 50), ("c", 0x19f, 5), ("s", 0x1b3, 7), ("ph", 0x1cf, 3)):
            if key == "p" and all(g(r, 0xd7 + 4 * k) == -1 for k in range(50)):
                continue                             # the template keeps no squad list for this club (a list
                                                     # can start with -1 and hold ids further on)
            ids = sorted(ls.get(key, []), key=lambda t: sn.get(g(staff[t], 0x08), ""))
            for k in range(n):
                struct.pack_into("<i", r, off + 4 * k, ids[k] if k < len(ids) else -1)
        struct.pack_into("<i", r, 0xcf, mgr.get(c, -1))
        struct.pack_into("<i", r, 0xd3, ass.get(c, -1))
    check_squad_lists(clubs, staff)

    # 4. Careers: real years, nothing from the start season on.
    kept, kept_rows = kept_careers(S_.TEMPLATE)
    check(not kept or kept_rows == n_template, "kept careers list is for another staff count")
    sh, _, _, n_hist = T.table(17)                  # checked against the file length
    rows = []
    for o in range(0, n_hist * 17, 17):
        rr = bytearray(sh[o:o + 17])
        who = struct.unpack_from("<i", rr, 4)[0]
        k = kept and 0 <= who < n_template and kept[who >> 3] >> (who & 7) & 1
        if not k:
            shift(rr, 8, SH["staff_history"])
        if struct.unpack_from("<H", rr, 8)[0] >= START:
            continue
        rows.append(rr)
    for i, rr in enumerate(rows):
        struct.pack_into("<i", rr, 0, i)
    T.raw["staff_history.dat"] = bytearray(b"".join(rows))
    set_count(T.raw["index.dat"], 17, len(rows))
    print(f"careers: {idx[17]['count']} -> {len(rows)}")

    # 5. Competition history.
    last = last_row_comps(S_.TEMPLATE)
    ch, nh = T.raw["club_comp_history.dat"], T.raw["nation_comp_history.dat"]
    T.table(19), T.table(20)                       # both tables lie inside their files
    crow = [bytearray(ch[o:o + 26]) for o in range(0, idx[19]["count"] * 26, 26)]
    crow = [r for r in crow if not (struct.unpack_from("<i", r, 4)[0] in last
                                    and struct.unpack_from("<H", r, 8)[0] == 2001)]
    crow = [r for r in crow if struct.unpack_from("<H", r, 8)[0] <= START]
    have = {(struct.unpack_from("<i", r, 4)[0], struct.unpack_from("<H", r, 8)[0]) for r in crow}
    added = 0
    # The seasons 2000/01 to 2005/06 from the September 2010 Update Team database, whose history rows are 9 years
    # early (date shift research). Its clubs map to the template's by long name and nation.
    H = Folder(hist_src)
    tkey = {}
    for tid_ in (0, 1):
        for rr in T.rows(tid_):
            tkey.setdefault((cstr(rr, 4, 51), g(rr, 0x53)), g(rr, 0))
    hmap = {}
    for tid_ in (0, 1):
        for rr in H.rows(tid_):
            hmap[g(rr, 0)] = tkey.get((cstr(rr, 4, 51), g(rr, 0x53)), -1)
    tcomps = {g(rr, 0) for rr in T.rows(12)} | {g(rr, 0) for rr in T.rows(16)}
    AFTER_START.update(g(rr, 0) for rr in T.rows(16) if cstr(rr, 4, 51) in S_.AFTER_START)
    fill = {}
    for rr in H.rows(19) + H.rows(20):
        comp, yr = struct.unpack_from("<iH", rr, 4)
        yr += HIST_SHIFT
        if comp in tcomps and 2001 <= yr <= START and not (yr == START and comp in AFTER_START):
            a, b, c3, d4 = struct.unpack_from("<iiii", rr, 10)
            fill[(comp, yr)] = (hmap.get(a, -1), hmap.get(b, -1), hmap.get(c3, -1) if c3 >= 0 else -1, -1)
    fm_rows = {}
    for comp, yr, v in comphist:
        cm = L["compmap"].get(comp)
        if cm is not None and 2001 <= yr <= START:
            a, b = (clubmap.get(teammap.get(t), -1) if t != 0xffffffff else -1 for t in v[:2])
            if a != -1:
                fm_rows[(cm, yr)] = (a, b, -1, -1)
    fill.update(fm_rows)                                # FM first where it has the season
    nation_comps = {g(rr, 0) for rr in T.rows(16)}
    nadd = []
    for (cm, yr), (a, b, c3, d4) in sorted(fill.items()):
        if (cm, yr) in have or a == -1:
            continue
        r = bytearray(26)
        struct.pack_into("<iiHiiii", r, 0, 0, cm, yr, a, b, c3, d4)
        (nadd if cm in nation_comps else crow).append(r)
        have.add((cm, yr))
        added += 1
    crow.sort(key=lambda r: (struct.unpack_from("<i", r, 4)[0], struct.unpack_from("<H", r, 8)[0]))
    nrow = [bytearray(nh[o:o + 26]) for o in range(0, idx[20]["count"] * 26, 26)]
    nrow = [r for r in nrow if struct.unpack_from("<H", r, 8)[0] <= START] + nadd
    nrow.sort(key=lambda r: (struct.unpack_from("<i", r, 4)[0], struct.unpack_from("<H", r, 8)[0]))
    for i, r in enumerate(crow + nrow):
        struct.pack_into("<i", r, 0, i)
    T.raw["club_comp_history.dat"] = bytearray(b"".join(crow))
    T.raw["nation_comp_history.dat"] = bytearray(b"".join(nrow))
    set_count(T.raw["index.dat"], 19, len(crow))
    set_count(T.raw["index.dat"], 20, len(nrow))
    print(f"competition history: {idx[19]['count']} -> {len(crow)} club rows ({added} added), "
          f"{idx[20]['count']} -> {len(nrow)} nation rows")

    store_update_team_way(T, START - 2001)
    os.makedirs(dst)
    for f in DAT_FILES:
        with open(os.path.join(dst, f), "wb") as h:
            h.write(T.raw[f])
    print("written", dst)


if __name__ == "__main__":
    main(sys.argv[1:])
