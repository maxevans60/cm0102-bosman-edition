#!/usr/bin/env python3
"""The Dutch nacompetitie as a knockout (issue 150; src/recomp_rt/win/season_ned_playoff.c).

Usage:
    python3 tests/seasons/ned_playoff_run.py <cm0102_recomp> <game folder> [--only <run>,...] [--keep <folder>]
        [--reuse] [--timeout <seconds>] [--seed <n>]
    python3 tests/seasons/ned_playoff_run.py --list

Each run starts a new game with the Netherlands, changing rules on, plays on holiday with no match engine and saves
after 365 days, in July of the next year (soak_run.py). The checks of each run:
    box      no message box of the game, and the program saves and ends with code 0;
    drawn    the brackets of the season, each with its clubs;
    ties     each tie of the log is the tie of the plan (list places of the drawn line, winners of the round before);
    moves    in the save each bracket's winner plays in the Eredivisie, and the Eredivisie club of the bracket plays in
             the Eerste Divisie when a lower club won; the Eredivisie keeps 18 clubs;
    none     2003/04 plays the groups and 2019/20 no play-off: no knockout line;
    load     the run r2012 saves during the play-off, a new program loads the save and plays the rest.
--seed <n> plays another game (a folder per seed), so that both an Eredivisie club and a lower club win a bracket.
A run takes 5 to 60 minutes. --reuse checks the folders in --keep again.
"""
import os
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(HERE)), "tools", "save"))
import rules_report_check as rrc  # noqa: E402
import soak_run  # noqa: E402

NATIONS = ["Holland"]
LINE = "recomp: ned playoff\t"
CLUB = 581              # club.dat: +0 id, +4 long name (latin-1, 0 ended), +0x57 the division (club_comp id)


def w(t):
    return -t - 1


# The ties of src/recomp_rt/win/season_ned_playoff_plan.c: list places, or w(t) for the winner of tie t before.
TEN = [[(0, 1)], [(w(0), 2), (3, 4)], [(w(0), w(1))]]
SEVEN = [[(0, 1), (2, 3), (4, 5)], [(w(0), w(1)), (w(2), 6)], [(w(0), w(1))]]
LADDER = [[(0, 1), (2, 3)], [(w(0), 4), (w(1), 5)], [(w(0), w(1))], [(w(0), 6)]]
# run: (start year, what it shows, ties, brackets, clubs of a bracket, legs).
RUNS = {
    "n2003": (2003, "2003/04: the groups of four stay", None, 0, 0, 0),
    "n2005": (2005, "2005/06: the first knockout, with six periods", TEN, 2, 5, 2),
    "n2012": (2012, "2012/13: two brackets of five", TEN, 2, 5, 2),
    "n2018": (2018, "2018/19: the last season of two brackets", TEN, 2, 5, 2),
    "n2019": (2019, "2019/20: abandoned, no play-off", None, 0, 0, 0),
    "n2020": (2020, "2020/21: seven clubs, single matches", SEVEN, 1, 7, 1),
    "n2022": (2022, "2022/23: seven clubs, two legs", SEVEN, 1, 7, 2),
    "n2025": (2025, "2025/26: four rounds, the Eredivisie 16th in the final", LADDER, 1, 7, 2),
    "n2026": (2026, "2026/27: seven clubs, two legs", SEVEN, 1, 7, 2),
    "r2012": (2012, "2012/13: a save during the play-off, loaded and played on", TEN, 2, 5, 2),
}
# run: the game days before the save during the play-off (the draw comes about 308 days after the start).
SAVE_IN_PLAYOFF = {"r2012": 318}


def save_divisions(save):
    """{long name: division id} of the clubs of a save."""
    import savfile
    data = savfile.Sav(save).read_member("club.dat")
    out = {}
    for i in range(len(data) // CLUB):
        rec = data[i * CLUB:(i + 1) * CLUB]
        name = rec[4:rec.index(b"\0", 4)].decode("latin-1")
        out[name] = struct.unpack_from("<i", rec, 0x57)[0]
    return out


def lines_of(log):
    """[cells after the tag] of the knockout lines of a log."""
    out = []
    with open(log, encoding="latin-1") as f:
        for line in f:
            if line.startswith(LINE):
                out.append(line.rstrip("\n").split("\t")[1:])
    return out


def check_ties(plan, entrants, ties, bracket):
    """The faults of the logged ties of one bracket against the plan."""
    failed = []
    alive = {}                  # (round, tie): the two clubs that can come out of the tie
    for r, round_ties in enumerate(plan):
        for t, sides in enumerate(round_ties):
            got = ties.get((r, t))
            if not got:
                failed.append(f"ties: bracket {bracket} round {r} tie {t} not in the log")
                continue
            alive[(r, t)] = got
            for k, side in enumerate(sides):
                want = {entrants[side]} if side >= 0 else set(alive.get((r - 1, -side - 1), ()))
                if got[k] not in want:
                    failed.append(f"ties: bracket {bracket} round {r} tie {t} side {k}: {got[k]}, want {sorted(want)}")
    return failed


def check(name, work, loaded=None):
    """loaded: the folder of the leg that loaded the save of work."""
    year, _what, plan, brackets, clubs, legs = RUNS[name]
    failed = []
    logs = [os.path.join(x, "stderr.txt") for x in (work, loaded) if x]
    for log in logs:
        box = soak_run.first_box(log)
        if box:
            failed.append(f"message box: {box}")
    report = os.path.join(work, "report.tsv")
    if not os.path.isfile(report):
        return failed + ["no report"]
    _seasons, comps = rrc.read_report(report, [])
    start = {c["file"]: c for c in comps if c["season"] == year and c["phase"] == "start"}
    upper_id = start["hol_prm"]["record"] if "hol_prm" in start else None
    lower_id = start["hol_first"]["record"] if "hol_first" in start else None
    save = os.path.join(loaded or work, "user", "samesave.sav")
    if not os.path.isfile(save):
        return failed + ["no save at the end of the run"]
    division = save_divisions(save)
    got = sum(1 for d in division.values() if d == upper_id)
    if got != 18:
        failed.append(f"count: the Eredivisie holds {got} clubs in the save, want 18")
    lines = [x for log in logs for x in lines_of(log) if int(x[1]) == year]
    if loaded:
        after = [x for x in lines_of(logs[1]) if int(x[1]) == year]
        if not any(x[0] == "tie" for x in after) or sum(x[0] == "winner" for x in after) != brackets:
            failed.append(f"load: the loaded game played {len(after)} knockout lines, want its later rounds and finals")
    drawn = [x for x in lines if x[0] == "drawn"]
    winners = [x for x in lines if x[0] == "winner"]
    bad = [x for x in lines if x[0] in ("no knockout", "failed", "no side")]
    if bad:
        failed.append(f"log: {bad[0]}")
    if len(drawn) != brackets:
        failed.append(f"drawn: {len(drawn)} brackets, want {brackets}")
    if len(winners) != brackets:
        failed.append(f"moves: {len(winners)} winners, want {brackets}")
    for x in drawn:
        b = int(x[2].split()[1])
        entrants = x[5:]
        if x[3] != f"{legs} legs":
            failed.append(f"drawn: bracket {b}: {x[3]}, want {legs} legs")
        if len(entrants) != clubs or len(set(entrants)) != clubs:
            failed.append(f"drawn: bracket {b}: clubs {entrants}, want {clubs} different clubs")
            continue
        ties = {}
        for t in lines:
            if t[0] == "tie" and int(t[2].split()[1]) == b:
                ties[(int(t[3].split()[1]), int(t[4].split()[1]))] = (t[5], t[6])
        failed += check_ties(plan, entrants, ties, b)
    for x in winners:
        bracket, winner, verdict, upper, upper_verdict = x[2:7]
        print(f"  {year} {bracket}: {winner} {verdict}, {upper} {upper_verdict}")
        if division.get(winner) != upper_id:
            failed.append(f"moves: {winner} in division {division.get(winner)}, want the Eredivisie")
        want = lower_id if verdict == "goes up" else upper_id
        if division.get(upper) != want:
            failed.append(f"moves: {upper} in division {division.get(upper)}, want {want}")
    return failed


def main(argv):
    if "--list" in argv:
        for name, row in RUNS.items():
            print(f"{name}\t{row[0]}\t{row[1]}")
        return 0
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    only = soak_run.arg(argv, "--only")
    names = only.split(",") if only else list(RUNS)
    keep = os.path.abspath(soak_run.arg(argv, "--keep")) if "--keep" in argv else tempfile.mkdtemp(prefix="nedpo-")
    timeout = int(soak_run.arg(argv, "--timeout", 7200))
    seed = int(soak_run.arg(argv, "--seed", 1))
    bad = 0
    for name in names:
        year = RUNS[name][0]
        database = None if year in rrc.BASE_DATA_YEARS else rrc.season_database(year)
        work = os.path.join(keep, name if seed == 1 else f"{name}-s{seed}")
        days = SAVE_IN_PLAYOFF.get(name)
        loaded = work + "-loaded" if days else None
        if "--reuse" not in argv:
            soak_run.wait_for_room()
            o = {"program": program, "game": game, "start": year, "last": year + 1, "seed": seed, "nations": NATIONS,
                 "database": database, "rules": "on", "extra": [], "timeout": timeout}
            code, secs, _box = soak_run.new_game_leg(o, work, days or 365)
            print(f"{name}: code {code}, {secs / 60:.0f} minutes")
            if code == 0 and days:
                soak_run.wait_for_room()
                save = os.path.join(work, "user", "samesave.sav")
                code, secs, _box = soak_run.load_leg(o, loaded, save, 365 - days)
                print(f"{name}: the loaded leg: code {code}, {secs / 60:.0f} minutes")
            if code != 0:
                print(f"{name}: FAIL: exit {code}")
                bad += 1
        failed = check(name, work, loaded)
        print(f"{name}: {'ok' if not failed else 'FAIL'}")
        for line in failed:
            print(f"  {line}")
        bad += bool(failed)
    print(f"folders: {keep}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
