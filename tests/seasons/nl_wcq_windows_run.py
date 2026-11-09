#!/usr/bin/env python3
"""The CONCACAF Nations League keeps out of the CONCACAF World Cup qualifying windows (0.6.2 item 209, season_nl.c,
season_nl_held_after in season_nl_rules.c).

Usage:
    python3 tests/seasons/nl_wcq_windows_run.py <cm0102_recomp> <game folder> [--keep <folder>]

After its table (last edition 2026-27) the CONCACAF Nations League repeated every second year with the matchdays
of September to November. The game's CONCACAF section holds the campaign year W - 2 for the World Cup of year W
(0x914a91) and plays from March of W - 2 to November of W - 1, so the 2032 edition met the 2034 qualifying in
every autumn window: in a replay of 2031-2033 (run-logs/0.6.2-s203/rep-ag) 30 Nations League matches were
clash-moved and more went to days that are not FIFA days. In the 2023-26 cycle the league phases were in the
autumns of 2023 and 2024 and the qualifying started in March 2024: they never shared a window. After item 209 an
edition starts only in a year whose autumn the qualifying leaves free (W - 4 and W - 3): 2027, 2030 and 2031.

One new game in 2021/22 on the October 2021 database (the December 2025 database has no CONCACAF Nations League
record), Tier A, England's leagues, seed 1, changing rules on, played
to 30 November 2032. The checks: the leg ends with code 0 and no message box; the Nations League has league-phase
matches (September to November) in 2027, 2030 and 2031 and none in 2028, 2029 and 2032; and from 2027 on no month of
September to November holds matches of both the Nations League and the World Cup qualifying (CONCACAF), and no
Nations League match (the March finals too) is within 3 days of a qualifier of the same side. On the
code before item 209 the 2028 and 2032 editions are played and share the autumn of 2032 with the qualifying: the
test fails there.

The years of the table are not checked: the 2024-25 edition (real days, September to November 2024) meets the
game's 2026 qualifying, which plays in the autumn of 2024 where the real one played in March and June 2024 and in
2025. That is the qualifying's days, not the Nations League's (a separate open item).
"""
import collections
import datetime
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "future"))
import history_check  # noqa: E402
import soak_run  # noqa: E402

DATABASES = "/Users/max/Projects/cm0102/test-fixtures/databases"
START = datetime.date(2021, 7, 1)
END = datetime.date(2032, 11, 30)
FROM = 2027                    # the first year after the table (last edition 2026-27)
HELD = (2027, 2030, 2031)
NOT_HELD = (2028, 2029, 2032)
NL = "CONCACAF Nations League"


def is_wcq(name):
    return name in ("FIFA World Cup qualification (CONCACAF)", "World Cup CONCACAF Qualifying Section")


def autumn_months(game, name):
    # name: a test of the competition name
    """(year, month) -> match count of a competition's matches in September to November."""
    out = collections.Counter()
    fx = history_check.fixtures(game)
    for cid, c in game.comps.items():
        if not name(c["name"]):
            continue
        for f in fx.get(cid, []):
            d = datetime.date.fromordinal(f[0])
            if 9 <= d.month <= 11:
                out[(d.year, d.month)] += 1
    return out


def near_pairs(game):
    """Nations League matches within 3 days of a qualifier of the same side (any month, the March finals too)."""
    fx = history_check.fixtures(game)
    nl, wcq = [], []
    for cid, c in game.comps.items():
        to = nl if c["name"] == NL else wcq if is_wcq(c["name"]) else None
        if to is not None:
            to += [(f[0], f[1], f[2]) for f in fx.get(cid, [])]
    days = collections.defaultdict(list)
    for d, h, a in wcq:
        days[h].append(d), days[a].append(d)
    out = []
    for d, h, a in nl:
        if d >= datetime.date(FROM, 1, 1).toordinal() and any(abs(d - q) <= 3 for t in (h, a) for q in days[t]):
            out.append(datetime.date.fromordinal(d).isoformat())
    return sorted(set(out))


def check(save):
    game = history_check.Game(save)
    nl, wcq = autumn_months(game, lambda n: n == NL), autumn_months(game, is_wcq)
    for name, months in ((NL, nl), ("World Cup qualifying (CONCACAF)", wcq)):
        print(f"      {name}: " + ", ".join(f"{y}-{m:02d} {n}" for (y, m), n in sorted(months.items())))
    fails = []
    years = {y for y, _ in nl}
    missing = [y for y in HELD if y not in years]
    extra = sorted(y for y in NOT_HELD if y in years)
    if missing:
        fails.append(f"no Nations League league phase in {', '.join(map(str, missing))}")
    if extra:
        fails.append(f"a Nations League league phase in {', '.join(map(str, extra))} (the qualifying's autumns)")
    shared = sorted(k for k in nl if k in wcq and k[0] >= FROM)
    if shared:
        fails.append("months with matches of both: " + ", ".join(f"{y}-{m:02d}" for y, m in shared))
    near = near_pairs(game)
    if near:
        fails.append("Nations League matches within 3 days of a qualifier of the same side: " + ", ".join(near))
    if not fails:
        print(f"ok    league phases in {', '.join(map(str, sorted(years)))}, no autumn month shared with the qualifying")
    return fails


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    keep = argv[argv.index("--keep") + 1] if "--keep" in argv else tempfile.mkdtemp(prefix="cm0102-nl-wcq-")
    os.environ["RECOMP_TIER_A"] = "1"
    os.environ["RECOMP_SEASON_NL_LOG"] = "1"
    o = {"program": program, "game": game, "rules": "on", "seed": 1, "start": 2021, "last": 2033,
         "database": os.path.join(DATABASES, "October 2021"), "nations": ["England"], "extra": [], "timeout": 7200}
    w = os.path.join(keep, "leg")
    code, took, box = soak_run.new_game_leg(o, w, (END - START).days)
    print(f"      leg to {END}: {took:.0f} s")
    save = os.path.join(w, "user", "samesave.sav")
    fails = []
    if code != 0 or box:
        fails.append(f"the leg ended with code {code}{', box ' + box if box else ''}")
    elif not os.path.exists(save):
        fails.append("no save")
    else:
        fails += check(save)
    for f in fails:
        print("FAIL ", f)
    print("PASS" if not fails else f"FAILED: {len(fails)}")
    return 0 if not fails else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
