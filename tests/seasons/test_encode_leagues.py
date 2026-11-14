#!/usr/bin/env python3
"""Test of the league rules encoder (0.6.0 slice 19, tools/seasons/encode_leagues.py).

Usage: python3 -I tests/seasons/test_encode_leagues.py

The checks: the play-off places of known rows (the English play-offs, the Championship play-offs of 6 from
2026/27, the National League of 6 from 2017/18, Serie B from 2004/05, the Segunda from 2010/11, the Ligue 1
relegation play-off from 2016/17, the 3rd v 16th tie of Germany, the Scottish play-offs from 2005/06 and the
Premiership play-off from 2013/14, a "pos" list of Ireland that plays both ways), that a finals series of the
MLS and a tie-break moves no place, the splits of Scotland, Belgium and Poland, the calendars of Argentina,
Ireland and Russia, that a text the tool does not know stops it, and that the committed outputs are up to date.

0.7.2 league sizes, slice 1: the structure of each league row. Meetings from each form of text (single, double,
triple, quadruple round robin, "each pair four times", "the others four times", matches or rounds over the count,
a split after N matches, home and away), from MEETINGS_FIX and MEETINGS_BY_HAND, "-" for a future row with unknown
clubs and for a cup; halves, zones (from the split text, "N and M clubs", the clubs cell), derby and inter-zone
rounds, periods (from the text, PERIODS_BY_HAND, unknown), the meetings of each split section, the rounds, and the
cells that the check of 13 November 2026 corrected."""
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
for y in range(2005, 2009):
    check(po("tur_second", y) == ("3-6", "-", "-"), "Turkish second tier %d: 3rd v 6th and 4th v 5th, a play-off of 4" % y)
check(po("usa_mls", 2001)[2] == "title" and po("usa_mls", 2001)[:2] == ("-", "-"), "MLS play-offs move no club")
check(po("ita_ser_a", 2001) == ("-", "-", "tiebreak"), "Serie A tie-break")
check(po("aus_nsl", 2005) == ("-", "-", "title"), "A-League finals series")

# --- splits
check(by[("sco_prm", 2001)]["split"] == "after 33: 6+6, points unknown", "Scottish split")
_bel_text = {(x["comp_file"], x["season"]): x for x in e.make_rules.read_rows()}[("bel_first", 2009)]
check(e.encode_split(_bel_text) == "after 30: 6+8, points halved", "Belgian Play-off I, points halved (the text)")
# Slice 10: the nearest form of Play-off I, Play-off II and the relegation play-off (SPLIT_BY_HAND).
check(by[("bel_first", 2009)]["split"] == "after 30: 6+8+2, points halved" and
      by[("bel_first", 2009)]["split_meet"] == "2+1+2", "Belgian play-offs by hand: 6+8+2, meetings 2+1+2")
check(by[("den_prm", 2017)]["split_meet"] == "2+1" and by[("bel_first", 2024)]["split_meet"] == "2+2+2" and
      by[("bel_second", 2016)]["meetings"] == 4 and by[("bel_second", 2016)]["rounds"] == 28,
      "Superliga 2+1, Pro League 2+2+2, First Division B four meetings")
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

# --- structure (0.7.2 league sizes, slice 1)
def st(file, season, col):
    r = by.get((file, season))
    return r[col] if r else None


# meetings, one check per form of the text
check(st("eng_prm", 2001, "meetings") == 2, "double round robin")
check(st("arg_prm", 2001, "meetings") == 1, "single round-robin tournaments (each half)")
check(st("cro_a1", 2007, "meetings") == 3, "33 matches (triple round robin)")
check(st("cro_a1", 2013, "meetings") == 4, "quadruple round robin")
check(st("sco_first", 2001, "meetings") == 4, "36 matches (each pair four times)")
check(st("bel_second", 2020, "meetings") == 4, "each club plays the others four times")
check(st("sco_prm", 2001, "meetings") == 3, "38 matches, a split after 33 matches of 12 clubs: 3")
check(st("bel_second", 2001, "meetings") == 2, "34 matches of 18 clubs: 2")
check(st("cro_a1", 2002, "meetings") == 2, "22 rounds then split, 12 clubs: 2")
check(st("bra_nat_third", 2008, "meetings") == 2, "groups of 5 (home and away)")
check(st("cro_a1", 2006, "meetings") == 3, "MEETINGS_FIX: a third game set by the standings")
check(st("jap_j2", 2001, "meetings") == 4 and st("jap_j2", 2010, "meetings") == 2, "MEETINGS_BY_HAND: J2")
check(st("arg_prm", 2026, "meetings") == "-", "unknown clubs: the class's meetings stay")
check(st("gre_cup", 2010, "meetings") == "-", "a cup has no structure")
check(all(r["meetings"] in (1, 2, 3, 4, "-") for r in rows), "meetings 1 to 4 or -")
held = [r for r in rows if r["comp_file"] not in e.LEAGUE_NOT and r["clubs"] > 0]
kept = {(g[2], g[1]) for g in gaps if g[3] == "meetings"}
check(all(isinstance(r["meetings"], int) or (r["comp_file"], r["season"]) in kept for r in held),
      "every league row with a count has meetings, or a gap line that keeps the class's meetings")
# halves, zones, extra rounds
check(st("arg_prm", 2001, "halves") == 1 and st("arg_prm", 2014, "halves") == 0, "Argentina: halves, then one table")
check(st("arg_prm", 2024, "zones") == "15+15", "two zones of 15")
check(st("arg_second", 2020, "zones") == "17+18", "two zones of 17 and 18 clubs")
check(st("por_second_b", 2001, "zones") == "20+20+20", "three zones from the clubs cell (Norte 20, Centro 20, Sul 20)")
check(st("eng_prm", 2001, "zones") == "-", "no zones")
check(st("arg_prm", 2014, "extra") == 1 and st("arg_prm", 2024, "extra") == 2, "a derby round, two inter-zone rounds")
check(st("arg_second", 2025, "extra") == "?", "inter-zone matches with no number: a gap")
# periods
check(st("bel_second", 2001, "periods") == 3, "three periods")
check(st("hol_first", 2005, "periods") == 6 and st("hol_first", 2010, "periods") == 4, "six, then four periods")
check(st("hol_first", 2001, "periods") == 0 and st("bel_second", 2007, "periods") == 0, "no periods")
check(st("hol_first", 2003, "periods") == "?", "periods unknown")
check(st("bel_second", 2016, "periods") == 2, "PERIODS_BY_HAND: First Division B, two periods")
# the meetings of the split sections
check(st("nir_prm", 2010, "split_meet") == "1+1", "after 33, 5 more matches each in sixes: one meeting")
check(st("wel_first", 2012, "split_meet") == "2+2", "play each other twice more")
check(st("wel_first", 2026, "split_meet") == "1+1+1", "sixes, four and six: once more")
check(st("fin_prm", 2024, "split_meet") == "2+1", "top 6 a double round, bottom 6 a single round")
check(st("gre_prm", 2022, "split_meet") == "2+1", "play-off double, play-out single round robin")
check(st("eng_prm", 2001, "split_meet") == "-", "no split")
# rounds
check(st("eng_prm", 2001, "rounds") == 38, "20 clubs, 2 meetings: 38 rounds")
check(st("arg_prm", 2014, "rounds") == 30, "30 clubs, 1 meeting and a derby round: 30 rounds")
check(st("arg_prm", 2024, "rounds") == 16, "a zone of 15, 1 meeting and 2 inter-zone rounds: 16 rounds")
check(st("sco_prm", 2001, "rounds") == 33, "the rounds before the split")
check(st("fra_cfa", 2001, "rounds") == "-", "a division of groups has no one table")
# the cells of the check of 13 November 2026
check(st("tur_second", 2023, "clubs") == 18, "TFF 1. Lig 2023/24: 18 played")
check(all(by[(f, 2027)]["clubs"] == -1 for f in ("ita_ser_a", "ita_ser_b", "ita_ser_c1a")), "Italy 2027/28: unknown")
check(st("kor_league", 2012, "split") == "after 26: 7+7, points kept", "K League 2013: split after 26 rounds")
check(st("fin_prm", 2020, "split") == "-", "Veikkausliiga 2020: the split was cancelled")
check(st("bel_first", 2022, "split").endswith("points halved"), "Pro League 2022/23: points halved")
check(st("bel_second", 2007, "po_up") == "2-5", "Belgian second tier 2007/08: a final round of 2nd to 5th")
check(st("arg_prm", 2026, "halves") == "?" and st("arg_prm", 2026, "zones") == "?", "a future row: structure unknown")
check(st("usa_mls", 2008, "meetings") == "-", "MLS 2008: 30 games of 15 clubs is unbalanced (matches over clubs - 1)")
check(st("jap_j1", 2025, "zones") == "10+10" and st("jap_j1", 2025, "rounds") == 18, "J1 2025: two regions of 10")
check(st("swe_second", 2001, "rounds") == "-" and st("tur_second_b", 2003, "rounds") == "-",
      "divisions of regional groups have no one table")
check(st("bel_third", 2001, "rounds") == "-" and st("bra_nat_third", 2001, "rounds") == "-",
      "groups in the clubs cell (A 16, B 16) or the format (regional groups): no one table")
check(st("usa_mls", 2026, "meetings") == 1 and st("usa_mls", 2026, "rounds") == "-", "MLS 2026: conferences, no one table")
check(st("tur_second", 2023, "rounds") == 34, "18 (19 entered; ...): one table of 18")
check(st("arg_second", 2001, "zones") == "gap", "ZONES_FIX: the Apertura of 25, then zones")
check(st("den_prm", 2020, "split_meet") == "2+2", "home and away (10 rounds) in sixes")
bad_zones = [(r["comp_file"], r["season"]) for r in rows if r["zones"] not in ("-", "gap", "?") and r["clubs"] > 0
             and sum(int(x) for x in r["zones"].split("+")) != r["clubs"]]
check(not bad_zones, "the zones add up to the count: %s" % bad_zones[:5])
table = {(x["comp_file"], x["season"]): x for x in e.make_rules.read_rows()}
check(table[("hol_first", 2019)]["up"] == 0 and table[("hol_first", 2019)]["down"] == 0
      and table[("hol_prm", 2019)]["down"] == 0, "Eerste Divisie and Eredivisie 2019/20: abandoned, no club up or down")
check(table[("bel_second", 2001)]["down"] == 0 and table[("bel_second", 2002)]["down"] == 0
      and table[("bel_second", 2020)]["down"] == 0, "Belgian second tier: no sporting relegation 2001/02, 2002/03, 2020/21")
check("sources disagree" in table[("usa_mls", 2006)]["playoffs_text"]
      and "sources disagree" in table[("usa_mls", 2007)]["playoffs_text"], "MLS 2007 and 2008 play-off rule")
fake_format = {"format_text": "the league of the moon", "split_text": "n/a", "notes": "", "clubs": 10,
               "comp_file": "eng_conf", "season": 2001, "format": "text"}
check(e.encode_meetings(fake_format) is None, "a format text the tool does not know is not encoded")

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
