#!/usr/bin/env python3
"""Test of the nation rules encoder (0.6.0 slice 17, tools/seasons/encode_rules.py).

Usage: python3 -I tests/seasons/test_encode_rules.py

The checks: the window forms of the research texts (two windows, three windows, a range over the new year,
"end of season" for England, the Italian "four weeks"), the texts that must stay gaps (one window only, a
window for domestic moves only, a stray date), the foreign player forms (Turkish "5+1+2", Brazilian squad
limit, a limit on the pitch, a limit in a kind the game's record does not hold), Brexit from the UK nations'
texts, that every gap has a known reason, that every row of the 27 nations is encoded or is in the gaps
file, and that the committed outputs are up to date."""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import encode_rules as e  # noqa: E402

failures = 0


def check(cond, text):
    global failures
    if not cond:
        failures += 1
        print("FAIL " + text)


def windows(nation, text):
    return e.parse_windows(nation, text)


# --- windows
check(windows("England", "summer: end of season to 31 Aug (1 Sep when 31 Aug is a weekend or bank holiday); winter: 1-31 Jan")
      == ("ok", "1.6-31.8,1.1-31.1", ""), "England 2002 form")
check(windows("England", "summer: 14 Jun to 1 Sep 2023; winter: 1 Jan to 1 Feb 2024") == ("ok", "14.6-1.9,1.1-1.2", ""),
      "two windows with years")
check(windows("England", "summer 1-10 Jun and 16 Jun-1 Sep 2025; winter 1-31 Jan 2026")
      == ("ok", "1.6-10.6,16.6-1.9,1.1-31.1", ""), "three windows")
check(windows("Spain", "1 July to 1 September 2022; 2 to 31 January 2023") == ("ok", "1.7-1.9,2.1-31.1", ""),
      "Spain: a range with one month name")
check(windows("Russia", "two registration periods (Premier League regulation 2003): 25 February to 13 March and 1 July "
                        "to 31 August 2003; the mid-season period lasted about 9 weeks")
      == ("ok", "25.2-13.3,1.7-31.8", ""), "Russia: two ranges with 'and'")
check(windows("Italy", "summer 1 July-31 August 2012; winter from 3 January 2013, four weeks")
      == ("ok", "1.7-31.8,3.1-30.1", ""), "Italy: four weeks")
check(windows("Italy", "summer to 31 August 2007; winter 3-31 January 2008") == ("ok", "1.7-31.8,3.1-31.1", ""),
      "Italy: summer to a date")
check(windows("Sweden", "no transfer windows (two windows introduced 15 November 2004; inference that moves were free "
                        "before)") == ("ok", "none", ""), "no windows")
check(windows("Brazil", "unknown")[0] == "keep", "unknown keeps")
check(windows("Greece", "two registration periods: one between seasons (max 12 weeks) and one mid-season (max 4 weeks); "
                        "exact dates: unknown")[0] == "keep", "exact dates unknown keeps")
check(windows("Belgium", "summer 1 July to 6 September")[0] == "gap", "one window only is a gap")
check(windows("Brazil", "5 January to 3 March; complementary domestic window 4 to 27 March; 20 July to 11 September")
      == ("gap", "", "partial"), "domestic window is a gap")
check(windows("Germany", "Summer 2020: 1 July, then 15 July to 5 October; winter 1 to 31 January, moved to the next bank "
                         "working day if needed")[0] == "gap", "a stray date is a gap")
check(windows("USA", "x")[0] == "gap", "an unknown form is a gap")

# --- foreign players
def foreign(nation, season, text):
    st, vals, reason, permit = e.parse_foreign(nation, season, text)
    return st, vals, reason, permit


check(foreign("Turkey", 2001, "Süper Lig: 5+1+2 (5 on the pitch, 1 on the bench, 2 in the stand)")[:2]
      == ("ok", ("keep", 6, "keep", 8)), "Turkey 5+1+2")
check(foreign("Turkey", 2013, "Süper Lig: 6+0+4: at most 10 foreigners signed, at most 6 on the match sheet")[:2]
      == ("ok", ("keep", 6, "keep", 10)), "Turkey 6+0+4")
check(foreign("Turkey", 2022, "Süper Lig: 14 foreigners registered, at most 8 on the pitch (planned 12+7 cancelled)")[:2]
      == ("ok", ("keep", "keep", "keep", 14)), "Turkey 14 registered")
check(foreign("Turkey", 2007, "Süper Lig: 6+1 at the start, 6+2 from the winter break; Goal adds a later 6+2+2")[0:3:2]
      == ("gap", "mid_season"), "Turkey changes in mid-season")
check(foreign("Brazil", 2023, "Série A match squad: at most 9 foreign players; no on-pitch limit stated")[:2]
      == ("ok", ("keep", 9, "keep", "keep")), "Brazil squad limit")
check(foreign("England", 2001, "no nationality quota in any division; EU/EEA players free to move")[:2]
      == ("ok", ("none", "none", "keep", "keep")), "England no quota")
check(foreign("England", 2021, "no nationality quota; every non-UK player (EU included) needs a GBE")[3] == "brexit",
      "England 2021: Brexit")
check(foreign("Spain", 2005, "Primera: 3 non-EU players registered in the first team, all may play together")[0:3:2]
      == ("gap", "hardcoded"), "Spain: hard-coded caps")
check(foreign("Italy", 2011, "Serie A: two non-EU signings from abroad per season")[0:3:2] == ("gap", "new_code"),
      "Italy: quota of signings")
check(foreign("Russia", 2008, "Premier League: at most 6 foreigners on the pitch (6+5)")[0:3:2] == ("gap", "pitch"),
      "Russia: a limit on the pitch")
check(foreign("Croatia", 2025, "HNL: 6 non-EU players per team per match; Ukrainian players counted as domestic")[0:3:2]
      == ("gap", "kind"), "Croatia: non-EU limit against a foreign slot")
check(foreign("Norway", 2001, "unknown")[0] == "keep", "unknown keeps")

# --- permits and registration
check(e.parse_permit("England", "GBE points system for all non-UK players (15 points)", "keep") == "brexit", "GBE")
check(e.parse_permit("Scotland", "all players without UK status need a governing body endorsement (points system)",
                     "keep") == "brexit", "Scotland")
check(e.parse_permit("Germany", "Non-EU players need a national work visa", "keep") == "keep", "Germany")
check(e.classify_registration_gap("England", "no squad size or home-grown rule") is None, "no rule is no gap")
check(e.classify_registration_gap("Spain", "25 first-team licences") == "new_code", "squad cap needs code")

# --- the whole table
rows, gaps, problems = e.encode()
check(not problems, "problems: %s" % problems)
check(len(rows) == 27 * 27, "%d rows, 729 expected" % len(rows))
for r in rows:
    e.check_windows(r)
check(all(g[3] in e.GAP_REASONS for g in gaps), "every gap has a known reason")
check({"windows", "foreign", "work_permit", "registration"} >= {g[2] for g in gaps}, "gap columns")
check(sum(1 for r in rows if r["windows"] not in ("keep",)) > 80, "windows are encoded for the nations that give dates")
check(subprocess.run([sys.executable, "-I", os.path.join(ROOT, "tools", "seasons", "encode_rules.py"), "--check"],
                     cwd=ROOT).returncode == 0, "the committed outputs are up to date (encode_rules.py --check)")

if failures:
    print("%d checks failed" % failures)
    sys.exit(1)
print("ok")
