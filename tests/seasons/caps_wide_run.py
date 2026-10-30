#!/usr/bin/env python3
"""International caps above 255 (0.6.2 slice 12, fault 30, Q25; src/recomp_rt/win/caps_wide.h).

Usage:
    python3 tests/seasons/caps_wide_run.py wide <cm0102_recomp> <game folder> --keep <folder> [--days 60]
    python3 tests/seasons/caps_wide_run.py load <cm0102_recomp> <game folder> --keep <folder> --save <in.sav>
        [--days 1]

wide: sets the caps byte (staff +0x22) of every player of start.sav of the test fixtures (17 July 2001) with at least
one cap to 255, loads it, goes on holiday and saves --days game days later (tests/fixtures/fixtures.py play). The
default of 60 days takes in the internationals of August and September 2001. The checks:
    - at least one edited player got a cap: the runtime logs "recomp: caps: staff <id> has <n> caps"
    - every edited player keeps the byte 255 (the original's byte wrapped to 0 at the 256th cap)
    - the season part of the end save is version 3 and holds exactly the logged players, each with his last logged
      count, 256 or more
    - no edited player gets a first-cap history row (the log line "recomp: caps: first cap of staff <id>", and the
      rows of type 6 in staff history.tmp grow only by the number of those lines)
    - no message box, and the program ends with code 0
The end save stays in <keep>/wide/out/samesave.sav for the load check. On a build without the fix (main) the first
three checks fail: this is the proof that the run finds the fault.

load: loads --save, a save made on holiday (tests/seasons/soak_run.py load_leg), plays --days game days and saves.
The checks: no box and code 0; no player's caps byte falls, and a byte rises by at most one (a cap on those days);
the caps above 255 of the input's season part (version 3) are all in the end save's part, none lower. With a save of
0.6.1 (no caps field) the end part must hold no caps unless a player passed 255 in those days. A build of 0.6.1
format (main before this slice) that loads a save of version 3 must load it too: it sees the byte 255.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "..", "tools", "save"))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))
import savfile  # noqa: E402
import soak_run  # noqa: E402
import fixtures  # noqa: E402

STAFF_SIZE, CAPS = 0x6e, 0x22
HISTORY_SIZE, HISTORY_TYPE, FIRST_CAP = 57, 0x0c, 6     # Staff_Records::add_first_cap_record 0x86cc50: type 6
HOLIDAY_STEPS = ["2500 click 45 165", "4500 click 150 427 | 199 474", "7000 click 705 572"]
FRIENDLY_ENTRY = 20
ADD, FIRST = "recomp: caps: staff ", "recomp: caps: first cap of staff "


def season_part(sav):
    """(version, {staff id: caps}) of the season part bosman_season.dat (src/recomp_rt/win/season.c), or (0, {})."""
    if not any(m.name == "bosman_season.dat" for m in sav.members):
        return 0, {}
    d = sav.read_member("bosman_season.dat")
    form, length = struct.unpack_from("<II", d, 0)
    if form < 3:
        return form, {}
    at = 16
    news = struct.unpack_from("<I", d, at)[0]
    at += 4
    for _ in range(news):
        at += 12 + struct.unpack_from("<I", d, at + 8)[0]
    at += 4 + struct.unpack_from("<I", d, at)[0] * (4 + 51)
    at += 4                                             # the season point
    at += 4 + struct.unpack_from("<I", d, at)[0] * FRIENDLY_ENTRY
    count = struct.unpack_from("<I", d, at)[0]
    caps = {}
    for i in range(count):
        sid, n = struct.unpack_from("<II", d, at + 4 + 8 * i)
        caps[sid] = n
    if form == 3 and at + 4 + 8 * count != length:
        raise SystemExit(f"season part: {length - at - 4 - 8 * count} bytes after the caps field")
    return form, caps


def caps_bytes(sav):
    """{staff id: caps byte} of every staff record."""
    st = sav.read_member("staff.dat")
    return {struct.unpack_from("<i", st, r * STAFF_SIZE)[0]: st[r * STAFF_SIZE + CAPS]
            for r in range(len(st) // STAFF_SIZE)}


def first_cap_rows(sav):
    h = sav.read_member("staff history.tmp")
    return sum(1 for r in range(len(h) // HISTORY_SIZE) if h[r * HISTORY_SIZE + HISTORY_TYPE] == FIRST_CAP)


def log_lines(path):
    adds, firsts = {}, []
    for line in open(path, encoding="latin-1"):
        if line.startswith(FIRST):
            firsts.append(int(line[len(FIRST):].split()[0]))
        elif line.startswith(ADD) and " has " in line:
            sid, n = line[len(ADD):].split(" has ")
            adds[int(sid)] = int(n.split()[0])
    return adds, firsts


def box_in(path):
    return [x.strip() for x in open(path, encoding="latin-1") if "message box" in x][:1]


def wide(argv):
    program, game = os.path.abspath(argv[0]), os.path.abspath(argv[1])
    keep = os.path.abspath(soak_run.arg(argv, "--keep"))
    days = int(soak_run.arg(argv, "--days", 60))
    work = os.path.join(keep, "wide")
    os.makedirs(work, exist_ok=True)
    sav = savfile.Sav(os.path.join(fixtures.DEFAULT_DIR, "start.sav"))
    staff = bytearray(sav.read_member("staff.dat"))
    edited = set()
    for r in range(len(staff) // STAFF_SIZE):
        if staff[r * STAFF_SIZE + CAPS] >= 1:
            staff[r * STAFF_SIZE + CAPS] = 255
            edited.add(struct.unpack_from("<i", staff, r * STAFF_SIZE)[0])
    sav.write_member("staff.dat", staff)
    save = os.path.join(work, "start.sav")
    if os.path.exists(save):
        os.remove(save)
    sav.save(save)
    rows_before = first_cap_rows(sav)
    steps = os.path.join(work, "steps.txt")
    with open(steps, "w") as f:
        f.write("\n".join(HOLIDAY_STEPS) + "\n")
    out = os.path.join(work, "out")
    code = fixtures.play(program, game, work, "start.sav", steps, out, "classic", {}, days)
    log = os.path.join(out, "stderr.txt")
    fails = []
    if code != 0:
        fails.append(f"the run failed (code {code})")
    if box_in(log):
        fails.append(f"a message box: {box_in(log)[0]}")
    end_path = os.path.join(out, "samesave.sav")
    if not os.path.exists(end_path):
        return fails + ["no save at the end"]
    end = savfile.Sav(end_path)
    adds, firsts = log_lines(log)
    form, part = season_part(end)
    after = caps_bytes(end)
    rows_after = first_cap_rows(end)
    print(f"{len(edited)} players edited to 255 caps; {len(adds)} of them got caps (log), the season part is version "
          f"{form} with {len(part)} players; {len(firsts)} first caps logged; first-cap history rows {rows_before} "
          f"before, {rows_after} after")
    for sid in sorted(adds)[:5]:
        print(f"  staff {sid}: byte {after.get(sid)}, logged {adds[sid]}, season part {part.get(sid)}")
    if not adds:
        fails.append("no edited player got a cap (no 'recomp: caps: staff ... has ... caps' line)")
    low = sorted(s for s in edited if after.get(s, 255) != 255)
    if low:
        fails.append(f"{len(low)} edited players do not keep the byte 255, for example staff {low[0]}: {after[low[0]]}")
    if form != 3 and adds:
        fails.append(f"the season part is version {form}, not 3")
    if part != adds:
        fails.append(f"the season part {sorted(part.items())[:4]} is not the logged caps {sorted(adds.items())[:4]}")
    if any(n < 256 for n in part.values()):
        fails.append("a stored count is below 256")
    again = sorted(set(firsts) & edited)
    if again:
        fails.append(f"{len(again)} edited players got a second first cap, for example staff {again[0]}")
    if rows_after - rows_before != len(firsts):
        fails.append(f"the first-cap history rows grew by {rows_after - rows_before}, the log has {len(firsts)}")
    return fails


def load(argv):
    program, game = os.path.abspath(argv[0]), os.path.abspath(argv[1])
    keep, save = os.path.abspath(soak_run.arg(argv, "--keep")), os.path.abspath(soak_run.arg(argv, "--save"))
    days = int(soak_run.arg(argv, "--days", 1))
    o = {"program": program, "game": game, "rules": "on", "database": None, "last": 2399, "seed": 1, "timeout": 3600}
    work = os.path.join(keep, "load-" + os.path.basename(os.path.dirname(os.path.dirname(save))))
    code, took, box = soak_run.load_leg(o, work, save, days)
    print(f"run: code {code} after {took:.0f} s, log {os.path.join(work, 'stderr.txt')}")
    fails = []
    if code != 0:
        fails.append(f"the program ended with {code}" + (f" ({box})" if box else ""))
    end_path = os.path.join(work, "user", "samesave.sav")
    if not os.path.exists(end_path):
        return fails + ["no save at the end"]
    start, end = savfile.Sav(save), savfile.Sav(end_path)
    (f0, p0), (f1, p1) = season_part(start), season_part(end)
    b0, b1 = caps_bytes(start), caps_bytes(end)
    adds, _ = log_lines(os.path.join(work, "stderr.txt"))
    fell = sorted(s for s in b0 if s in b1 and b1[s] < b0[s])
    jump = sorted(s for s in b0 if s in b1 and b1[s] > b0[s] + 1)
    rose = sum(1 for s in b0 if s in b1 and b1[s] == b0[s] + 1)
    print(f"{len(b0)} staff; caps bytes: {rose} rose by one, {len(fell)} fell, {len(jump)} rose by more; season part "
          f"version {f0} with {len(p0)} caps above 255, then version {f1} with {len(p1)}")
    top = max(b0, key=lambda s: (p0.get(s, b0[s]), -s))
    print(f"  most caps: staff {top}, {p0.get(top, b0[top])} before, {p1.get(top, b1.get(top))} after")
    if fell:
        fails.append(f"{len(fell)} caps bytes fell, for example staff {fell[0]}: {b0[fell[0]]} to {b1[fell[0]]}")
    if jump:
        fails.append(f"{len(jump)} caps bytes rose by more than one")
    lost = sorted(s for s in p0 if p1.get(s, 0) < p0[s])
    if lost:
        fails.append(f"{len(lost)} caps above 255 are lost or lower after the load, for example staff {lost[0]}")
    new = sorted(set(p1) - set(p0) - set(adds))
    if new:
        fails.append(f"{len(new)} players have caps above 255 that no cap of this run gave, for example staff {new[0]}")
    return fails


def main(argv):
    if len(argv) < 4 or argv[1] not in ("wide", "load") or "--keep" not in argv:
        print(__doc__)
        return 2
    fails = (wide if argv[1] == "wide" else load)(argv[2:])
    for f in fails:
        print(f"FAIL: {f}")
    print("FAIL" if fails else "PASS")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
