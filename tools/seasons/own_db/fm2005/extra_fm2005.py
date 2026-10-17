"""Loans and ratings from Football Manager 2005's database (file version 0xf0). Data only, python3 -I safe.
read_fm2005.py calls it and writes loans.pkl and ratings.pkl into the facts folder. The ratings serve only as the
reference for key players with no Update Team record (Q383, Max 2026-10-17); keyplayers.py maps them.

loans(db_folder, people_all, clubs, teammap) -> {people_all index: dict(club=<FM club id>, end=<date or None>,
    start=<date or None>)}
ratings(db_folder, people_all) -> {people_all index: dict(ca, pa, pos={name: v}, attrs={name: v}, row=<index>)}

people_all, clubs and teammap are the facts pickles of read_fm2005.py (lists and dicts, already loaded).

Layouts (found in the FM2005 Mac data editor's loaders, read as data: PLAYING_DATA::load, ACTUAL_PLAYER::load,
PLAYING_DATA::get_ability_name; checked with Riquelme's loan to Villarreal and the ratings of Henry, Rooney,
Cech, Terry and Fabregas; current ability correlates 0.87 with September 2005 over 84,143 matched players):
- STARTING_LOAN, server_db.dat: u32 count right after the last COMP_HISTORY row, then 27-byte rows:
  u8 0, u32 person id (the contract pid), u32 team id (loan club, map with teammap), u16 day + u16 year (start),
  u16 day + u16 year (end), u8 flag, u8 squad number, u32 row id, i32 -1.
- ACTUAL_PLAYER block of a PERSON record, people_db.dat: u8 type (2 player, 3 player and non-player), s16 current
  reputation, s16 home reputation, s16 world reputation, s16 current ability, s16 potential ability (-10..-1 are
  ranges), u32 PLAYING_DATA row, u32 -1, ... The name block of the same person follows 105 or 121 bytes later
  (other lengths for some records).
- PLAYING_DATA, people_db.dat: the third table; u32 count, then 89-byte rows: u8 0, 79 ability bytes (index =
  ability number below), u8 extra flag, u32 row id, i32 -1.
"""
import datetime
import os
import struct

# Ability numbers and names, from PLAYING_DATA::get_ability_name in the FM2005 Mac data editor (number 22 has no
# name there; it is the central striker slot, editor id "attc").
POSITIONS = ["Goalkeeper", "Sweeper", "Defender", "Defensive Midfielder", "Midfielder", "Attacking Midfielder",
             "Attacker", "Wing Back", "Free Role", "Right Side", "Left Side", "Central",
             "D L", "D C", "D R", "DM C", "M L", "M C", "M R", "AM L", "AM C", "AM R", "ST C", "WB L", "WB R"]
ATTRS = ["Crossing", "Dribbling", "Finishing", "Heading", "Long Shots", "Marking", "Movement", "Passing",
         "Penalties", "Tackling", "Vision", "Handling", "Aerial Ability", "Command Of Area", "Communication",
         "Kicking", "Throwing", "Anticipation", "Decisions", "One On Ones", "Positioning", "Reflexes",
         "First Touch", "Technique", "Left Foot", "Right Foot", "Flair", "Corners", "Teamwork", "Work Rate",
         "Long Throws", "Eccentricity", "Rushing Out", "Tendency To Punch", "Acceleration", "Free Kicks",
         "Strength", "Stamina", "Pace", "Jumping", "Leadership", "Dirtiness", "Balance", "Bravery", "Consistency",
         "Aggression", "Agility", "Important Matches", "Injury Proneness", "Versatility", "Natural Fitness",
         "Determination", "Composure", "Concentration"]
# Abilities the game stores relative to current ability (PLAYING_DATA::is_absolute_ability is false for them).
RELATIVE = {n for i, n in enumerate(ATTRS, 25) if (25 <= i <= 48 and i != 33) or i in (77, 78)}


def displayed(ca, v):
    """The value FM2005 shows for a relative ability: ACTUAL_PLAYER::convert_relative_ability."""
    w, x = ca / 200.0, (ca + 2.0 * v) / 10.0
    f = 0.5 + (1 - w) * (10 + x) / 1.75 + w * (5 + x) / 1.5
    return int(min(max(f, 1.0), 20.0))


def _date(d, o):
    day, y = struct.unpack_from("<HH", d, o)
    if y in (0, 1900) or day > 365:
        return None
    return datetime.date(y, 1, 1) + datetime.timedelta(days=day)


def _read(folder, name):
    with open(os.path.join(folder, name), "rb") as f:
        d = f.read()
    if d[:6] != b"\x02\x01tad." or d[6] != 0xF0:
        raise ValueError(name + " is not an FM2005 (0xf0) database file")
    return d


def _run_end(d, anchor, size, idoff):
    """End offset of a fixed-size table run through the row at anchor (type byte 0, ids up by 1)."""
    rid = lambda o: struct.unpack_from("<I", d, o + idoff)[0]  # noqa: E731
    o = anchor
    while o + 2 * size <= len(d) and d[o + size] == 0 and rid(o + size) == rid(o) + 1:
        o += size
    return o + size


def loan_rows(db_folder):
    """All STARTING_LOAN rows: (pid, team, start, end, flag, squad number)."""
    s = _read(db_folder, "server_db.dat")
    # COMP_HISTORY (39-byte rows) holds the 2003/04 Premier League row: comp 7, 2004, Arsenal, Chelsea, Man Utd.
    a = s.find(struct.pack("<IHIII", 7, 2004, 516, 544, 593))
    if a < 1:
        raise ValueError("COMP_HISTORY anchor not found")
    o = _run_end(s, a - 1, 39, 31)
    n = struct.unpack_from("<I", s, o)[0]
    o += 4
    if not 0 < n < 100000 or o + 27 * n > len(s):
        raise ValueError("loan table count out of range")
    rows = []
    for i in range(n):
        q = o + 27 * i
        t, pid, team = struct.unpack_from("<BIi", s, q)
        flag, sq, rid = struct.unpack_from("<BBI", s, q + 17)
        if t != 0 or rid != i:
            raise ValueError(f"loan row {i} is not in the expected shape")
        rows.append((pid, team, _date(s, q + 9), _date(s, q + 13), flag, sq))
    return rows


def loans(db_folder, people_all, clubs, teammap, on=datetime.date(2004, 7, 1)):
    """Loans that run on the given day (end date on or after it, or no end date), keyed by people_all index.

    A row is dropped when the loan club is the player's own contract club or the team is unknown."""
    bypid = {}
    for i, x in enumerate(people_all):
        if x.get("pid") is not None:
            bypid.setdefault(x["pid"], []).append(i)
    out = {}
    for pid, team, start, end, flag, sq in loan_rows(db_folder):
        if pid not in bypid or len(bypid[pid]) != 1 or team not in teammap:
            continue
        i = bypid[pid][0]
        club = teammap[team]
        if club not in clubs or club == people_all[i].get("club"):
            continue
        if end is not None and end < on:
            continue
        if end is None and start is None:
            continue
        out[i] = dict(club=club, end=end, start=start)
    return out


def _playing_table(p):
    """Start of the PLAYING_DATA rows: the header's third count, found as the u32 in front of the table, which
    sits a few bytes before (count of the last table, tail) the end minus the two fixed-size tables."""
    n_person, n_pdata, n_play, n_nonplay = struct.unpack_from("<4I", p, 9)
    guess = len(p) - n_nonplay * 43 - n_play * 89
    for start in range(guess, guess - 64, -1):
        if struct.unpack_from("<I", p, start - 4)[0] != n_play:
            continue
        ok = all(p[start + 89 * i] == 0 and struct.unpack_from("<I", p, start + 89 * i + 81)[0] == i
                 for i in (0, 1, n_play // 2, n_play - 1))
        if ok:
            return start, n_play
    raise ValueError("PLAYING_DATA table not found")


def ratings(db_folder, people_all):
    p = _read(db_folder, "people_db.dat")
    start, n_play = _playing_table(p)
    out = {}
    prev = 0
    for i, x in enumerate(people_all):
        o = x["off"]
        lo = max(prev + 40, o - 600, 0)
        prev = o
        for k in range(o - 30, lo, -1):
            if p[k] not in (2, 3):
                continue
            r = struct.unpack_from("<5hIi", p, k + 1)
            if all(0 <= a <= 200 for a in r[:4]) and -10 <= r[4] <= 200 and r[5] < n_play and r[6] == -1:
                q = start + 89 * r[5] + 1
                ab = p[q:q + 79]
                out[i] = dict(ca=r[3], pa=r[4], rep=r[:3], row=r[5],
                              pos={n: ab[j] for j, n in enumerate(POSITIONS)},
                              attrs={n: ab[25 + j] for j, n in enumerate(ATTRS)})
                break
    return out
