#!/usr/bin/env python3
"""The 2001 rows of the Campeonato Paraense and the Campeonato Cearense in a 2001 game (0.6.2 open item 189,
season_history_start.c).

Usage:
    python3 tests/seasons/state_rows_2001_run.py <cm0102_recomp> <game folder> [--keep <folder>]

Real life: one champion and one runner-up for each edition. In this game the region records of 3.9.68 play the
championships of Para and Ceara (season_bra_states.c), and their data rows are of other states, one a year: Alagoas
under the Paraense (ASA / CSA 2001), Amazonas under the Cearense (Atletico Rio Negro AM / Nacional AM 2001). At the
creation the game writes its own 2001 row of each (League::record_season_honours 0x684640, 0x4afce0 at 0x684771), and
the data row of 2001 goes. Before item 189 the data row stayed (the rule of item 176 for rows of another state), so
2001 held two rows.

One new game as the run-set row a2001to2300: Tier A, every nation, seed 1, changing rules on, the 3.9.68 database,
a 2001/02 start, 10 days, a save. The checks: the run ends with code 0 and shows no message box; each of the two
records holds one row of 2001, and its winner is a club of the record's state (the game's row).
"""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "future"))
import history_check  # noqa: E402
import soak_run  # noqa: E402

DAYS = 10
# the game's own champions of 2001 come from the record's state; the data rows of 3.9.68 are of these other states
DATA_ROWS = {"Campeonato Paraense": {"Agremiação Sportiva Arapiraquense", "Centro Sportivo Alagoano"},
             "Campeonato Cearense": {"Atlético Rio Negro Clube (AM)", "Nacional Futebol Clube (AM)"}}


def rows_2001(save):
    """{record name: [(winner, runner-up) of each row labelled 2001]} for the two records."""
    game = history_check.Game(save)
    out = {name: [] for name in DATA_ROWS}
    for cid, c in game.comps.items():
        if c["name"] in out:
            out[c["name"]] += [(game.club(r[2])[0], game.club(r[3])[0]) for r in game.rows
                               if r[0] == cid and r[1] == 2001]
    return out


def check(rows):
    fails = []
    for name, rs in rows.items():
        print(f"      {name} 2001: {'; '.join(f'{a} / {b}' for a, b in rs) or 'no row'}")
        if len(rs) != 1:
            fails.append(f"{name} holds {len(rs)} rows of 2001, not 1")
        elif rs[0][0] in DATA_ROWS[name] or rs[0][1] in DATA_ROWS[name]:
            fails.append(f"{name} 2001 is the data row of another state, not the game's row")
        else:
            print(f"ok    {name}: one row of 2001, the game's")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    keep = argv[argv.index("--keep") + 1] if "--keep" in argv else tempfile.mkdtemp(prefix="cm0102-state2001-")
    os.environ["RECOMP_TIER_A"] = "1"
    o = {"program": program, "game": game, "rules": "on", "database": None, "seed": 1, "start": 2001, "last": 2011,
         "nations": ["all"], "extra": [], "timeout": 5400}
    work = os.path.join(keep, "leg")
    code, took, box = soak_run.new_game_leg(o, work, DAYS)
    fails = []
    if code != 0 or box:
        fails.append(f"the run ended with code {code}{', box ' + box if box else ''} ({work})")
    save = os.path.join(work, "user", "samesave.sav")
    if not os.path.exists(save):
        fails.append("no save after the days")
    else:
        print(f"      run {took:.0f} s")
        fails += check(rows_2001(save))
    for f in fails:
        print("FAIL ", f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 0 if not fails else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
