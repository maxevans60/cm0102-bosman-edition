#!/usr/bin/env python3
"""The first round of AFC World Cup qualifying (0.6.2 item 196, season_afc_first.c).

Usage:
    python3 tests/seasons/afc_first_round_run.py <cm0102_recomp> <game folder> [--keep <folder>] [--cycles 1|2]

Real life: the AFC played a two-leg first round of its weakest nations before the groups (2006: 14 teams, 2018 and
2022: 12, 2026: 20; docs/research/rules-0.6.0/int-world.tsv, rows wc_asia_league). Brunei played it in 2018, 2022
and 2026. The original's pool builder 0x913f60 reads the Asian nations in nation-table order and stops after 44
less its exempt count, so a nation at the end of the table (Brunei, in the 3.9.68 data) never plays. With changing
rules on, every Asian nation enters, and the weakest 2 x (nations - 40) play two-leg ties from October of the
campaign year; the winners take the last places of the group draw (25 March of the next year).

One new game, Tier A, England's leagues, seed 1, changing rules on, the 3.9.68 database, a 2001/02 start:
- leg 1 to 11 October 2004 (the 2006 cycle's first round is under way), a save;
- leg 2 loads it and saves one day later: the Asian section's part of the save is the same (save and load in the
  middle of the first round give the same game);
- leg 3 loads the save of leg 1 and plays to 31 March 2005 (after the group draw), a save;
- with --cycles 2, leg 4 loads that and plays to 31 March 2009 (the 2010 cycle).

The checks: every leg ends with code 0 and no message box; in each cycle Brunei plays two matches of the Asian
section between 15 September of the campaign year and the group draw (25 March); the section's matches of that
time are two-leg ties (each pair of teams meets twice). On main before item 196 Brunei plays no match: the test
fails there.
"""
import datetime
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "future"))
import history_check  # noqa: E402
import soak_run  # noqa: E402

START = datetime.date(2001, 7, 1)          # 3.9.68, 2001/02: the game's first day (soak_run's leg counts from it)
MIDDLE = datetime.date(2004, 10, 11)
NATION = "Brunei"
SECTION = "World Cup Asian Qualifying Section"


def first_round(save, campaign):
    """[(day, home name, away name)] of the Asian section from 15 September of campaign to 24 March after it."""
    game = history_check.Game(save)
    ids = {cid for cid, c in game.comps.items() if c["name"] == SECTION}
    lo = datetime.date(campaign, 9, 15).toordinal()
    hi = datetime.date(campaign + 1, 3, 24).toordinal()
    name = lambda t: game.club(t)[1] or game.club(t)[0]     # noqa: E731
    return sorted((f[0], name(f[1]), name(f[2])) for cid in ids for f in history_check.fixtures(game).get(cid, [])
                  if lo <= f[0] <= hi)


def check_cycle(save, campaign):
    fails = []
    fx = first_round(save, campaign)
    pairs = {}
    for _, h, a in fx:
        pairs[frozenset((h, a))] = pairs.get(frozenset((h, a)), 0) + 1
    brunei = [f for f in fx if NATION in f[1:]]
    days = sorted({datetime.date.fromordinal(d).isoformat() for d, _, _ in fx})
    print(f"      {campaign + 2}: {len(fx)} first-round matches, {len(pairs)} ties, days {', '.join(days)}")
    for d, h, a in brunei:
        print(f"      {datetime.date.fromordinal(d).isoformat()} {h} v {a}")
    if len(brunei) != 2:
        fails.append(f"{campaign + 2}: {NATION} plays {len(brunei)} first-round matches, not 2")
    bad = [sorted(p) for p, n in pairs.items() if n != 2]
    if not pairs or bad:
        fails.append(f"{campaign + 2}: ties that do not meet twice: {bad}")
    if not fails:
        print(f"ok    {campaign + 2}: {NATION} plays a two-leg first-round tie")
    return fails


def section_parts(save):
    sav = history_check.LazySav(save)
    return {n: sav.read(n) for n in sav.names() if n.startswith(SECTION)}


def leg_fails(name, code, box, save):
    fails = []
    if code != 0 or box:
        fails.append(f"leg {name} ended with code {code}{', box ' + box if box else ''}")
    if not os.path.exists(save):
        fails.append(f"leg {name}: no save")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    keep = argv[argv.index("--keep") + 1] if "--keep" in argv else tempfile.mkdtemp(prefix="cm0102-afc1-")
    cycles = int(argv[argv.index("--cycles") + 1]) if "--cycles" in argv else 1
    os.environ["RECOMP_TIER_A"] = "1"
    o = {"program": program, "game": game, "rules": "on", "database": None, "seed": 1, "start": 2001, "last": 2011,
         "nations": ["England"], "extra": [], "timeout": 7200}
    saved = lambda work: os.path.join(work, "user", "samesave.sav")      # noqa: E731
    fails = []

    w1 = os.path.join(keep, "leg1")
    code, took, box = soak_run.new_game_leg(o, w1, (MIDDLE - START).days)
    print(f"      leg 1 to {MIDDLE}: {took:.0f} s")
    fails += leg_fails("1", code, box, saved(w1))
    if not fails:
        w2 = os.path.join(keep, "leg2")
        code, took, box = soak_run.load_leg(o, w2, saved(w1), 1)
        fails += leg_fails("2", code, box, saved(w2))
        if not fails:
            a, b = section_parts(saved(w1)), section_parts(saved(w2))
            if not a or a != b:
                fails.append(f"the Asian section's part differs after a save and a load ({sorted(a)}, {sorted(b)})")
            else:
                print(f"ok    save and load in the first round: {', '.join(sorted(a))} the same")
        w3 = os.path.join(keep, "leg3")
        end = datetime.date(2005, 3, 31)
        code, took, box = soak_run.load_leg(o, w3, saved(w1), (end - MIDDLE).days)
        print(f"      leg 3 to {end}: {took:.0f} s")
        fails += leg_fails("3", code, box, saved(w3))
        if not fails:
            fails += check_cycle(saved(w3), 2004)
        if not fails and cycles > 1:
            w4 = os.path.join(keep, "leg4")
            end4 = datetime.date(2009, 3, 31)
            code, took, box = soak_run.load_leg(o, w4, saved(w3), (end4 - end).days)
            print(f"      leg 4 to {end4}: {took:.0f} s")
            fails += leg_fails("4", code, box, saved(w4))
            if not fails:
                fails += check_cycle(saved(w4), 2008)
    for f in fails:
        print("FAIL ", f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 0 if not fails else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
