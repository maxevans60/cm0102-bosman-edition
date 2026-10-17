"""Football Manager 2007 (file version 0x042c): the ratings of each player, keyed by the people_all list index.

Usage: python3 -I rating_link.py <db700 folder> <people_all.pkl> <out ratings.pkl>
Library: ratings(db_folder, people_all) -> {people_all index: dict(ca, pa, pos, roles, attrs, pd, rid)}

Pure reader, no FM data inside. The files are untrusted input: every read is bounds-checked.

read_fm2007.py calls common_ratings() and writes ratings.pkl into the facts folder, with the names that
extra_fm2005.py uses. The ratings serve only as the reference for key players with no Update Team record (Q383,
Max 2026-10-17); keyplayers.py maps them. Checks: Henry 188/193, Rooney 179, Cech 181 (GK 20), Terry 182 (DC 20);
current ability correlates 0.80 with November 2007 over 81,948 matched players.

Layout, read from the game's own loaders in the Mac binary (fm_i386, read as data with objdump):
- people_db.dat holds the PERSON table. A player is a type 2 record (ACTUAL_PLAYER, loader 0x8067ba) or a type 3
  record (ACTUAL_PLAYER_AND_NON_PLAYER, 0x93b47c). The record is: u32 record id, type byte, the ACTUAL_PLAYER block,
  (type 3 only: the NON_PLAYER block, 0x84ce4e), then the ACTUAL_PERSON block (0x83d804), which starts with the name
  block that people_all's `off` points at.
- ACTUAL_PLAYER block: 6 s16 (+0xa, +0xc, +0xe, current ability +0x10, potential ability +0x12, one value the loader
  discards), u32 PLAYING_DATA id (+0xb0), then more fields (see _player_block).
- PLAYING_DATA table: u32 count, then per row: type byte 0, 79 bytes, u32 id, u32 unique id (loader 0x852d9a). The row
  with id k is row k. Byte i of the 79 is what the game reads as attribute i (0x7fd982 reads PLAYING_DATA+0x1c+i).
"""
import os
import pickle
import struct
import sys

PD_ROW = 88
MAX_BLOCK = 2000          # the search for the record start, in bytes before the name block
NAME_GAP = 6              # the last fields of the block (a date and a u32) lie right before the name block

POS = {0: "GK", 1: "SW", 12: "DL", 13: "DC", 14: "DR", 15: "DMC", 23: "WBL", 24: "WBR", 16: "ML", 17: "MC", 18: "MR",
       19: "AML", 20: "AMC", 21: "AMR", 22: "ST"}
ROLES = {2: "defender", 3: "defensive midfielder", 4: "midfielder", 5: "attacking midfielder", 6: "attacker",
         7: "wing back", 8: "free role", 9: "right side", 10: "left side", 11: "central"}
ATTRS = {25: "Crossing", 26: "Dribbling", 27: "Finishing", 28: "Heading", 29: "Long Shots", 30: "Marking",
         31: "Off The Ball", 32: "Passing", 33: "Penalty Taking", 34: "Tackling", 35: "Creativity", 36: "Handling",
         37: "Aerial Ability", 38: "Command Of Area", 39: "Communication", 40: "Kicking", 41: "Throwing",
         42: "Anticipation", 43: "Decisions", 44: "One On Ones", 45: "Positioning", 46: "Reflexes", 47: "First Touch",
         48: "Technique", 49: "Left Foot", 50: "Right Foot", 51: "Flair", 52: "Corners", 53: "Teamwork",
         54: "Work Rate", 55: "Long Throws", 56: "Eccentricity", 57: "Rushing Out", 58: "Tendency To Punch",
         59: "Acceleration", 60: "Free Kicks", 61: "Strength", 62: "Stamina", 63: "Pace", 64: "Jumping",
         65: "Influence", 66: "Dirtiness", 67: "Balance", 68: "Bravery", 69: "Consistency", 70: "Aggression",
         71: "Agility", 72: "Important Matches", 73: "Injury Proneness", 74: "Versatility", 75: "Natural Fitness",
         76: "Determination", 77: "Composure", 78: "Concentration"}


class Bad(Exception):
    pass


def need(cond, msg):
    if not cond:
        raise ValueError("rating_link: " + msg)


def _u(d, fmt, o):
    if o < 0 or o + struct.calcsize(fmt) > len(d):
        raise Bad(o)
    return struct.unpack_from(fmt, d, o)


def _b(d, o):
    if not 0 <= o < len(d):
        raise Bad(o)
    return d[o]


def _player_block(d, s):
    """Parse the ACTUAL_PLAYER block from its type byte at s (loader 0x8067ba, file version 0x042c).
    Return (end offset, the six s16 values, PLAYING_DATA id). Raise Bad on a sub-list this reader does not decode."""
    o = s + 1
    vals = _u(d, "<6h", o)
    o += 12
    pd = _u(d, "<i", o)[0]
    o += 4                       # u32 +0xb0: PLAYING_DATA id
    o += 4 + 1 + 1               # u32 +0x20, u8 +0x44, u8 +0x47
    o += 2 + 2 + 2               # u16 +6, +8, +4
    o += 1 + 2 + 2 + 1 + 1       # u8, u16 +0x1c, u16 +0x1e, u8 +0x49, s8 +0x45
    o += 2 + 2 + 1 + 2 + 2       # u16 +0x18, +0x1a, u8 +0x48, u16 +0x14, +0x16
    o += 4                       # u32 +0x24
    o += 9                       # 0x7fc66e: 9 bytes (0x7fae04), u8 n, n x 6 bytes (0x7fb884),
    n = _b(d, o)                 # s8, u32, u32, u8, four dates
    o += 1 + 6 * n + 1 + 4 + 4 + 1 + 16
    o += 1 + 4 + 8 + 4 + 4       # u8 +0x46, u32 +0x3c, 8 bytes +0xb4, u32 +0x38, date +0x4c
    if _b(d, o):                 # s8 flag: career stats, 8 x u16 (0xa18842)
        o += 16
    o += 1
    if _b(d, o):                 # s8 flag: 0x9265ca
        o += 1
        for _ in range(3):       # injuries (0x83e7d0), list 0x92e13c, flag 0x926b74: not decoded
            if _b(d, o):
                raise Bad("9265ca")
            o += 1
        n = _b(d, o)             # days in foreign nations, n x (u32, u32)
        o += 1 + 8 * n
    else:
        o += 1
    if _b(d, o):                 # s8 flag: MLS data, 19 bytes (0x868794)
        o += 19
    o += 1
    if _b(d, o):                 # u8 n: booking counts (0xb96490), not decoded
        raise Bad("bookings")
    o += 1
    if _b(d, o):                 # s8 flag: object +0x30 (0x7fa5f4), not decoded
        raise Bad("x30")
    o += 1
    o += 4 + 4                   # date, u32 +0x34
    return o, vals, pd


def _non_player_block(d, o):
    """The NON_PLAYER block of a type 3 record (0x84ce4e): 5 u16, 4 u8, u32, u32, two flags (objects not decoded)."""
    o += 10 + 4 + 4 + 4
    if _b(d, o) or _b(d, o + 1):
        raise Bad("non-player objects")
    return o + 2


def playing_table(p):
    """(offset of the first row, row count) of the PLAYING_DATA table."""
    pat = struct.pack("<II", 1, 2) + b"\0"
    k = p.find(pat)
    while k >= 0:
        t = k - 80 - PD_ROW - 4
        if t >= 0:
            n = struct.unpack_from("<I", p, t)[0]
            first = t + 4
            if 0 < n and first + n * PD_ROW <= len(p) and all(
                    p[first + i * PD_ROW] == 0 and struct.unpack_from("<I", p, first + i * PD_ROW + 80)[0] == i
                    for i in (0, 1, n // 2, n - 1)):
                return first, n
        k = p.find(pat, k + 1)
    need(False, "no PLAYING_DATA table in people_db.dat")


def find_record(p, off, npd):
    """Start (type byte offset), type, values, PLAYING_DATA id of the player record whose name block is at off."""
    hits = []
    for s in range(off - NAME_GAP - 49, max(off - MAX_BLOCK, 4), -1):
        t = p[s]
        if t not in (2, 3):
            continue
        try:
            e, vals, pd = _player_block(p, s)
            if t == 3 and e < off:
                e = _non_player_block(p, e)
        except Bad:
            continue
        if e != off:
            continue
        rid = struct.unpack_from("<I", p, s - 4)[0]
        if rid < 1000000 and 0 <= pd < npd and all(-100 <= v <= 200 for v in vals):
            hits.append((s, t, vals, pd, rid))
    return hits


def ratings(db_folder, people_all):
    """{people_all index: dict(ca, pa, pos={name: v}, roles={name: v}, attrs={name: v}, pd=row id, rid=record id)}.
    people_all is the list of person dicts (key `off`, `pid`) or the path of people_all.pkl. Values are raw: 1 to 20
    for positions and attributes, 0 where FM leaves a value unset; pa can be negative (FM's -5 .. -95 ranges)."""
    if isinstance(people_all, (str, os.PathLike)):
        with open(people_all, "rb") as f:
            people_all = pickle.load(f)
    path = os.path.join(db_folder, "people_db.dat")
    with open(path, "rb") as f:
        p = f.read()
    need(p[:6] == b"\x02\x01tad." and struct.unpack_from("<H", p, 6)[0] == 0x042c, "people_db.dat is not version 0x042c")
    first, npd = playing_table(p)
    out = {}
    for i, x in enumerate(people_all):
        off = x.get("off")
        if not isinstance(off, int) or not 64 <= off < first:
            continue
        hits = find_record(p, off, npd)
        if len(hits) > 1 and x.get("pid") is not None:
            hits = [h for h in hits if h[4] == x["pid"]] or hits
        if len(hits) != 1:
            continue
        s, t, vals, pd, rid = hits[0]
        row = p[first + pd * PD_ROW + 1:first + pd * PD_ROW + 80]
        need(len(row) == 79, "PLAYING_DATA row outside the file")
        out[i] = dict(ca=vals[3], pa=vals[4], pos={k: row[j] for j, k in POS.items()},
                      roles={k: row[j] for j, k in ROLES.items()}, attrs={k: row[j] for j, k in ATTRS.items()},
                      pd=pd, rid=rid, type=t)
    return out



GRID = {"GK": "Goalkeeper", "SW": "Sweeper", "DL": "D L", "DC": "D C", "DR": "D R", "DMC": "DM C", "WBL": "WB L",
        "WBR": "WB R", "ML": "M L", "MC": "M C", "MR": "M R", "AML": "AM L", "AMC": "AM C", "AMR": "AM R", "ST": "ST C"}
OLD = {"defender": "Defender", "defensive midfielder": "Defensive Midfielder", "midfielder": "Midfielder",
       "attacking midfielder": "Attacking Midfielder", "attacker": "Attacker", "wing back": "Wing Back",
       "free role": "Free Role", "right side": "Right Side", "left side": "Left Side", "central": "Central"}
RENAME = {"Off The Ball": "Movement", "Penalty Taking": "Penalties", "Influence": "Leadership", "Creativity": "Vision"}


def common_ratings(db_folder, people_all):
    """ratings() with the position and attribute names of extra_fm2005.py: {index: dict(ca, pa, pos, attrs)}."""
    out = {}
    for i, r in ratings(db_folder, people_all).items():
        pos = {GRID[k]: v for k, v in r["pos"].items()}
        pos.update({OLD[k]: v for k, v in r["roles"].items()})
        out[i] = dict(ca=r["ca"], pa=r["pa"], pos=pos, attrs={RENAME.get(k, k): v for k, v in r["attrs"].items()})
    return out

if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.exit(__doc__.split("\n\n")[1])
    r = ratings(sys.argv[1], sys.argv[2])
    with open(sys.argv[3], "wb") as f:
        pickle.dump(r, f)
    print(len(r), "players with ratings")
