#!/usr/bin/env python3
"""Test of the league rules encoder (0.6.0 slice 19, tools/seasons/encode_leagues.py).

Usage: python3 -I tests/seasons/test_encode_leagues.py

The checks: the play-off places of known rows (the English play-offs, the Championship play-offs of 6 from
2026/27, the National League of 6 from 2017/18, Serie B from 2004/05, the Segunda from 2010/11, the Ligue 1
relegation play-off from 2016/17, the 3rd v 16th tie of Germany, the Scottish play-offs from 2005/06 and the
Premiership play-off from 2013/14, a "pos" list of Ireland that plays both ways), that a finals series of the
MLS and a tie-break moves no place, the splits of Scotland, Belgium and Poland, the calendars of Argentina,
Ireland and Russia, that a text the tool does not know stops it, and that the committed outputs are up to date."""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "seasons"))
import encode_leagues as e  # noqa: E402

failures = 0


def check(cond, text):
    global failures
    if not cond:
        failures += 1
        print("FAIL " + text)


rows, gaps, problems = e.encode()
check(not problems, "every text is known: %s" % problems[:3])
by = {(r["comp_file"], r["season"]): r for r in rows}


def po(file, season):
    r = by.get((file, season))
    return (r["po_up"], r["po_down"], r["po_other"]) if r else None


# --- play-offs
check(po("eng_first", 2001) == ("3-6", "-", "-"), "Championship play-offs of 4")
check(po("eng_first", 2026) == ("3-8", "-", "-"), "Championship play-offs of 6 from 2026/27")
check(po("eng_conf", 2001) == ("-", "-", "-"), "no Conference play-off in 2001/02")
check(po("eng_conf", 2017) == ("2-7", "-", "-"), "National League play-offs of 6 from 2017/18")
check(po("ita_ser_b", 2001) == ("-", "-", "-"), "no Serie B play-off in 2001/02")
check(po("ita_ser_b", 2004) == ("3-6", "-", "-"), "Serie B play-offs from 2004/05")
check(by[("ita_ser_b", 2004)]["po_status"].startswith("partial"), "the Serie B points gap rule is listed")
check(po("spa_second", 2010) == ("3-6", "-", "-"), "Segunda play-offs from 2010/11")
check(po("fra_first", 2016) == ("-", "18", "-"), "Ligue 1 relegation play-off from 2016/17")
check(po("fra_first", 2023) == ("-", "16", "-"), "Ligue 1 of 18: 16th")
check(po("ger_second", 2008) == ("3", "16", "-"), "2. Bundesliga 3rd up, 16th down")
check(po("ger_first", 2008) == ("-", "16", "-"), "Bundesliga 16th")
check(po("sco_first", 2005) == ("-", "9", "-"), "Scottish First Division 9th from 2005/06")
check(po("sco_second", 2005) == ("2-4", "9", "-"), "Scottish Second Division 2nd to 4th and 9th")
check(po("sco_first", 2013) == ("2-4", "9", "-"), "Championship 2nd to 4th from 2013/14")
check(po("sco_prm", 2013) == ("-", "11", "-"), "Premiership 11th from 2013/14")
check(po("sco_prm", 2012) == ("-", "-", "-"), "no Premiership play-off in 2012/13")
check(po("ire_first", 2009) == ("2-3", "12", "-"), "Irish pos 2,3,12: two up, one down")
check(po("nor_prm", 2001) == ("-", "12", "-"), "Tippeligaen 12th")
check(po("usa_mls", 2001)[2] == "title" and po("usa_mls", 2001)[:2] == ("-", "-"), "MLS play-offs move no club")
check(po("ita_ser_a", 2001) == ("-", "-", "tiebreak"), "Serie A tie-break")
check(po("aus_nsl", 2005) == ("-", "-", "title"), "A-League finals series")

# --- splits
check(by[("sco_prm", 2001)]["split"] == "after 33: 6+6, points unknown", "Scottish split")
check(by[("bel_first", 2009)]["split"] == "after 30: 6+8, points halved", "Belgian Play-off I, points halved")
check(by[("pol_first", 2013)]["split"] == "after 30: 8+8, points halved", "Polish split 2013/14")
check(by[("pol_first", 2017)]["split"] == "after 30: 8+8, points kept", "Polish split 2017/18")
check(by[("den_prm", 2020)]["split"] == "after 22: 6+6, points kept", "Danish split from 2020/21")
check(by[("arg_prm", 2001)]["split"] == "stages", "Apertura and Clausura")
check(by[("eng_prm", 2001)]["split"] == "-", "one table")

# --- calendars
check(by[("arg_prm", 2001)]["calendar"] == "split", "Argentina 2001/02")
check(by[("arg_prm", 2014)]["calendar"] == "calendar", "Argentina 2015")
check(by[("arg_prm", 2016)]["calendar"] == "split", "Argentina back in 2016/17")
check(by[("ire_prm", 2002)]["calendar"] == "transition", "Ireland 2002/03 and 2003")
check(by[("ire_prm", 2003)]["calendar"] == "calendar", "Ireland summer season from 2004")
check(by[("rus_prm", 2010)]["calendar"] == "transition", "Russia 2011/12, first stage")
check(by[("rus_prm", 2012)]["calendar"] == "split", "Russia autumn to spring from 2012/13")

# --- a text the tool does not know stops it
fake = {"playoffs": "text", "playoffs_text": "the winner of the moon cup goes up", "comp_file": "eng_conf",
        "clubs": 22, "tier": 5}
check(e.encode_playoffs(fake) is None, "an unknown text is not encoded")

# --- every gap has a reason, and the outputs are up to date
check(all(g[4] for g in gaps), "every gap has a reason")
done = subprocess.run([sys.executable, "-I", os.path.join(ROOT, "tools", "seasons", "encode_leagues.py"), "--check"])
check(done.returncode == 0, "the committed outputs are up to date")

print("PASS" if not failures else "FAILED: %d" % failures)
sys.exit(1 if failures else 0)
