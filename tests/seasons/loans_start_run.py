#!/usr/bin/env python3
"""The loans of a season database at the start of a new game (open item 229).

Usage:
    python3 tests/seasons/loans_start_run.py <cm0102_recomp> <game folder> <folder of databases> [--keep <folder>]
        [--jobs n]

<folder of databases> is test-fixtures/databases: it needs "October 2026" and "December 2025" as the Update Team
gives them (with player_setup.cfg). Each run starts a new game (England, Arsenal, changing rules on), plays one day
and saves. The loans are counted in contract.dat of the save (CONTRACT_MANAGER::load_from_disk 0x4db5c0: int n1, n1
rows of 0x15, int n2, n2 contracts of 0x50, +0 the staff index (the load writes it, 0x4db5c0), +4 the club id, +0x2d
and +0x2f the end day and year, +0x35 the type in its low 6 bits): a loan is a contract of type 6, next to the
player's main contract (type 2).

The checks:
    y2026       October 2026 in 2026/27: more than 100 loans; Jack Grealish has a contract with Manchester City and
                a loan with Everton that ends in 2027; the log line "recomp: player setup: the dates of" fires.
    y2026-orig  the same with RECOMP_PLAYER_SETUP_DATES=original: no loans (the fault of 0.7.0 and 0.7.1).
    y2025       December 2025 in 2025/26: more than 100 loans, Grealish on loan at Everton to 2026.
    y2001       October 2026 in 2001/02 (no season game): the same loans as y2001-orig, where the switch keeps the
                original; nothing moves (no log line).
At most --jobs (2) games run at the same time.
"""
import collections
import concurrent.futures
import os
import shutil
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "tests", "smoke"))
import season_core_run as scr  # noqa: E402
import smoke_run  # noqa: E402

LOG = "recomp: player setup: the dates of"
LOAN = 6                                    # contract type of a loan (contract +0x35, low 6 bits)
failures = 0


def check(cond, text):
    global failures
    print(("ok    " if cond else "FAIL  ") + text, flush=True)
    if not cond:
        failures += 1


def find_db(dbs, name):
    """The folder of the database name under dbs: the folder itself or its one subfolder (a zip's folder)."""
    d = os.path.join(dbs, name)
    if os.path.exists(os.path.join(d, "staff.dat")):
        return d
    subs = [os.path.join(d, s) for s in os.listdir(d) if os.path.isdir(os.path.join(d, s))]
    return subs[0] if len(subs) == 1 else d


def loans(save):
    """(number of loans, {(staff index, name): [(club name, end year)]} of the players on loan)."""
    files = smoke_run.save_files(save)
    b = files["contract.dat"][0]
    n1 = struct.unpack_from("<i", b, 0)[0]
    o = 4 + n1 * 0x15
    n2 = struct.unpack_from("<i", b, o)[0]
    o += 4
    rows = [(struct.unpack_from("<i", b, o + i * 0x50)[0], struct.unpack_from("<i", b, o + i * 0x50 + 4)[0],
             struct.unpack_from("<H", b, o + i * 0x50 + 0x2f)[0], b[o + i * 0x50 + 0x35] & 0x3f) for i in range(n2)]
    per = collections.defaultdict(list)
    for staff, club, end, kind in rows:
        if staff >= 0:
            per[staff].append((club, end, kind))
    on_loan = {s: [(c, y) for c, y, _ in cs] for s, cs in per.items() if any(k == LOAN for _, _, k in cs)}

    def names(part):
        d = files[part][0]
        return {struct.unpack_from("<i", d, i + 0x33)[0]: d[i:i + 51].split(b"\0")[0].decode("latin-1")
                for i in range(0, len(d) - 59, 60)}
    first, second, common = names("first_names.dat"), names("second_names.dat"), names("common_names.dat")
    cd = files["club.dat"][0]
    clubs = {struct.unpack_from("<i", cd, i)[0]: cd[i + 4:i + 55].split(b"\0")[0].decode("latin-1")
             for i in range(0, len(cd) - 580, 581)}
    sd = files["staff.dat"][0]
    out = {}
    for s, cs in on_loan.items():
        f, sn, cm = struct.unpack_from("<iii", sd, s * 0x6e + 4)
        out[(s, common.get(cm) or f"{first.get(f, '?')} {second.get(sn, '?')}")] = [(clubs.get(c, c), y) for c, y in cs]
    return len(on_loan), out


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game, dbs = (os.path.abspath(a) for a in argv[:3])
    keep = os.path.abspath(argv[argv.index("--keep") + 1]) if "--keep" in argv else None
    jobs = int(argv[argv.index("--jobs") + 1]) if "--jobs" in argv else 2
    work = keep or tempfile.mkdtemp(prefix="cm0102-loans-")
    os.makedirs(work, exist_ok=True)
    oct26, dec25 = find_db(dbs, "October 2026"), find_db(dbs, "December 2025")
    runs = [("y2026", 2026, oct26, {}), ("y2026-orig", 2026, oct26, {"RECOMP_PLAYER_SETUP_DATES": "original"}),
            ("y2025", 2025, dec25, {}), ("y2001", 2001, oct26, {}),
            ("y2001-orig", 2001, oct26, {"RECOMP_PLAYER_SETUP_DATES": "original"})]

    def one(r):
        name, year, db, env = r
        if os.path.exists(os.path.join(work, name)):
            shutil.rmtree(os.path.join(work, name))
        path, err, t = scr.play(program, game, work, name, {"changing_rules": "on"}, scr.new_game_steps(), 1,
                                env_extra={"RECOMP_START_SEASON": str(year), **env}, database=db)
        return name, path, err
    res = {}
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        for name, path, err in ex.map(one, runs):
            check(path is not None, f"{name}: the game starts and saves")
            res[name] = (loans(path) if path else (0, {}), err or "")
    try:
        (n26, l26), e26 = res["y2026"]
        g = next((v for (_, n), v in l26.items() if n == "Jack Grealish"), [])
        check(n26 > 100, f"y2026: {n26} loans at the start of 2026/27")
        check(sorted(c for c, _ in g) == ["Everton", "Manchester City"] and ("Everton", 2027) in g,
              f"y2026: Jack Grealish has a contract with Manchester City and a loan at Everton to 2027 ({g})")
        check(LOG in e26, "y2026: the log line of the date move fires")
        (n26o, _), e26o = res["y2026-orig"]
        check(n26o == 0 and LOG not in e26o, f"y2026-orig: no loans with RECOMP_PLAYER_SETUP_DATES=original ({n26o})")
        (n25, l25), _ = res["y2025"]
        g = next((v for (_, n), v in l25.items() if n == "Jack Grealish"), [])
        check(n25 > 100 and ("Everton", 2026) in g, f"y2025: {n25} loans, Grealish at Everton to 2026 ({g})")
        (n01, l01), e01 = res["y2001"]
        (n01o, l01o), _ = res["y2001-orig"]
        check(n01 > 100 and l01 == l01o and LOG not in e01,
              f"y2001: a 2001 start keeps the original ({n01} loans, {n01o} with the switch, no log line)")
    finally:
        if not keep:
            shutil.rmtree(work, ignore_errors=True)
    print(f"{failures} failure(s)")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
