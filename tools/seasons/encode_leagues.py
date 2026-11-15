#!/usr/bin/env python3
"""Encodes the play-offs, splits and calendars of the league rows of the rules table (0.6.0 slice 19).

Usage:
    python3 -I tools/seasons/encode_leagues.py --write     write the outputs below
    python3 -I tools/seasons/encode_leagues.py --check     stop on a text the tool does not know, and check that
                                                           the committed outputs are up to date
    python3 -I tools/seasons/encode_leagues.py             a summary

Input: the league rows (a numbered tier, a comp_file of the original) of the 20 parts of
docs/research/rules-0.6.0/, read with make_rules.read_rows(). The text columns are playoffs, split and
real_season. Slice 17's encoder (encode_rules.py) encodes the nation rules; this tool is its league part.

Outputs (committed):
    docs/research/2026-10-18-league-rules-encoded.tsv   one line per league row: the structured values below
    docs/research/2026-10-18-league-rules-gaps.tsv      one line per text the tool could not encode (for Max)
    src/recomp_rt/win/season_league_table.c             the C table of the runtime (season_league_rules)

The structured values (the same words in the TSV and in the C table):
    po_up        the places of the division in a play-off for a place in the tier above (a list of table
                 places, "3-6" or "2,3"), "-" for none. A club of the tier above in the same tie does not count
                 here: it counts in po_down of its own row.
    po_down      the places of the division in a play-off to keep its place (relegation play-off), "-" for none.
    po_other     the play-offs that move no club between tiers: "title" (a title play-off, a finals series or a
                 championship of stage winners), "europe" (a play-off for a European place), "tiebreak" (a
                 match only when clubs are level), joined by "+", or "-".
    po_status    ok, or the reason the text is only partly encoded (the gaps file lists it too).
    split        "-" for one table; "after N: A+B[+C], points P" for a split after N matches into groups of A, B
                 (and C) clubs with the points kept, halved or unknown (P is kept, halved or unknown);
                 "stages", "zones" or "groups" for a season of two tournaments, of zones, or of groups;
                 "gap" when the tool cannot read it.
    calendar     "split" (a season over two years, July to May), "calendar" (one calendar year), or
                 "transition" (a row that holds more or less than one season: Ireland 2002/03, Russia 2010/11 and
                 2011/12).

The structure values (0.7.2 league sizes, slice 1; the engine of the later slices reads them). A row of a file
that is not a league (LEAGUE_NOT: cups, continental competitions, nation rules) has "-" in each:
    meetings     the times each pair of clubs meets in the table (1 to 4): in each half of a season of halves, in
                 each zone, before the split of a split season, in each group of a division of groups. Read from
                 the format text, then the split text ("single", "double", "triple", "quadruple round robin",
                 "each pair four times", or matches and rounds over the club count), else MEETINGS_BY_HAND
                 (docs/research/2026-11-13-league-meetings.tsv, with the evidence of each range).
                 "-": the class's own meetings stay (a season not held, a future row with unknown clubs, an
                 unbalanced schedule). The tool stops on a held league row it can read neither way.
    halves       1: two halves (Apertura and Clausura, two stages), each a table of the meetings; 0: one table.
                 A future row with unknown clubs has "?" in halves, zones, extra and periods (-1 in the C table):
                 its structure is the last known row's (D20).
    zones        "-", or the club counts of the zones of one division joined by "+" ("15+15"); "gap" when the
                 text names zones the tool cannot count.
    extra        rounds beyond meetings x (n' - 1) in the table (a half, a zone): derby or inter-zone rounds
                 ("plus one derby round" 1, "plus two interzonal matches" 2); "?" when the text gives no number.
    periods      the period titles of the season (0 none, "?" unknown).
    split_meet   with a split ("after N: ..."): the meetings in each section, joined by "+" in the order of the
                 split's sections ("1+1", "2+1"), "?" for a section the text does not give; "-" without a split.
    rounds       the rounds of the table (of each half, of each zone): meetings x (n' - 1) + extra (the spec's R
                 is meetings x (n' - 1); the derby and inter-zone rounds come on top), n' the club
                 count (the largest zone) rounded up to even. With an odd count the derby or inter-zone matches
                 first fill the rounds in which a club rests (one a meeting). "-" when the meetings, the count or
                 the extra are not known, or for a division of groups.

The tool stops, with exit code 1, on a text that it neither encodes nor lists in KNOWN_GAPS with a reason, so
a changed research text never goes unseen.
"""
import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import make_rules  # noqa: E402

OUT_TSV = os.path.join(ROOT, "docs", "research", "2026-10-18-league-rules-encoded.tsv")
OUT_GAPS = os.path.join(ROOT, "docs", "research", "2026-10-18-league-rules-gaps.tsv")
OUT_C = os.path.join(ROOT, "src", "recomp_rt", "win", "season_league_table.c")

TSV_COLUMNS = ["id", "season", "comp_file", "tier", "clubs", "po_up", "po_down", "po_other", "po_status", "split",
               "calendar", "meetings", "halves", "zones", "extra", "periods", "split_meet", "rounds"]

# The files of the table with a numbered tier that are not leagues: they get no structure values.
LEAGUE_NOT = {"argentina_rules", "usa_rules", "usa_mls_all_stars", "usa_open_cup", "gre_cup", "gre_super", "pol_cup",
              "pol_lge_cup", "pol_super", "cro_cup", "conmebol_liber", "conmebol_merc", "con_merc_cup", "con_champ",
              "asia_club_champ", "asia_cup_winner", "oceania_club_champ", "european_cup", "uefa_cup"}

# The meetings of the league rows whose texts do not give them: {comp_file: [(first season, last season, meetings,
# evidence)]}, from docs/research/2026-11-13-league-meetings.tsv (each range with its evidence). "-" and "?" keep
# the class's meetings.
MEETINGS_FILE = os.path.join(ROOT, "docs", "research", "2026-11-13-league-meetings.tsv")


def read_meetings_by_hand(path=MEETINGS_FILE):
    out = {}
    with open(path, encoding="utf-8") as fh:
        lines = [x.rstrip("\n").split("\t") for x in fh if x.strip() and not x.startswith("#")]
    if lines[0] != ["comp_file", "first_season", "last_season", "meetings", "evidence"]:
        raise SystemExit("encode_leagues.py: %s: the columns are not those expected" % path)
    for cells in lines[1:]:
        file, first, last, value, why = cells
        value = int(value) if value.isdigit() else value
        if value not in (1, 2, 3, 4, "-", "?") or not why.strip():
            raise SystemExit("encode_leagues.py: %s: bad line %s" % (path, cells))
        out.setdefault(file, []).append((int(first), int(last), value, why))
    return out


MEETINGS_BY_HAND = read_meetings_by_hand()

# Texts whose meetings the reading above gets wrong: {(comp_file, season): (meetings, evidence)}.
MEETINGS_FIX = {
    # "three rounds: double round robin (22 matches), then a third game against each club with pairings set by the
    # standings (33 matches)": three meetings, the third set by the standings.
    ("cro_a1", 2006): (3, "33 matches of 12 clubs in the format cell"),
}

# The period titles of rows whose texts do not give them: {(comp_file, first, last): (periods, evidence)}.
PERIODS_BY_HAND = {
    ("bel_second", 2016, 2019): (2, "First Division B: an opening and a closing period of 14 rounds (check of 13 "
                                    "November 2026, sizes-check-north line 230)"),
}

ORD = r"(\d+)(?:st|nd|rd|th)"

# Texts that the parser below reads wrongly or cannot read, with their encoding by hand: (po_up, po_down,
# po_other, po_status). A text is the exact playoffs_text of the table. Each entry was read against the row.
BY_HAND = {
    # A league of zones or groups: the play-off places are places of a group, not of the one table.
    "Final between Apertura and Clausura winners (winner up); loser v best overall club for 2nd place; "
    "promoción v Primera 17th and 18th": ("-", "-", "-", "partial: a final of the tournament winners (stages)"),
    "As 2002/03, plus a Torneo Reducido for a promoción place": ("-", "-", "-", "partial: a final of the tournament winners (stages)"),
    "As 2003/04; Torneo Reducido of 4th to 7th": ("4-7", "-", "-", "partial: a final of the tournament winners (stages)"),
    "As 2003/04; Torneo Reducido of 3rd to 6th": ("3-6", "-", "-", "partial: a final of the tournament winners (stages)"),
    "Torneo Reducido for the 2nd promotion (Clausura zone winners and next best overall); best two non-promoted "
    "clubs play promoción v Primera 17th and 18th": ("-", "-", "-", "partial: a final of the tournament winners (stages)"),
    "one-match tie-breaker for 3rd place": ("-", "-", "tiebreak", "ok"),
    "Torneo Reducido of 2nd to 13th": ("2-13", "-", "-", "ok"),
    "17th and 18th of the average table play two-leg promotion/relegation ties (promoción) against Primera B "
    "Nacional clubs": ("-", "17-18", "-", "partial: the places of the table of averages, not of the season table"),
    "One-match tie-breaker at a neutral venue when clubs tie for the aggregate-table relegation place (extra time, "
    "then penalties)": ("-", "-", "tiebreak", "ok"),
    "In each tournament: top 8 of each zone play single-leg knockouts from the round of 16":
        ("-", "-", "title", "ok"),
    "In each tournament: top 16 of the tournament table play single-leg knockouts from the round of 16":
        ("-", "-", "title", "ok"),
    "Liguilla Pre-Libertadores (3rd to 6th) and Liguilla Pre-Sudamericana for continental places":
        ("-", "-", "europe", "ok"),
    "Final at a neutral ground between the two zone winners; play-off of the two zone runners-up for Copa "
    "Libertadores entry": ("-", "-", "title+europe", "ok"),
    # Belgium: the final rounds of period winners (no places of the table).
    "17th plays the second division final round for one top-flight place": ("-", "17", "-", "ok"),
    "15th and 16th to relegation play-offs (league cut to 16 for 2009/10)": ("-", "15-16", "-", "ok"),
    "top three of the aggregate table (not promoted) to the Europa League play-offs": ("-", "-", "europe", "ok"),
    "promotion final second leg played August 2020": ("1-2", "-", "-",
                                                      "partial: the winners of the two stages, not table places"),
    "runner-up v top-flight 17th": ("2", "-", "-", "ok"),
    "3rd to 6th play off; winner plays the top flight relegation play-off runner-up": ("3-6", "-", "-", "ok"),
    "promotion play-off winner plays the top-flight last place": ("-", "-", "-",
                                                                  "partial: the play-off places are not given"),
    "promotion play-offs": ("-", "-", "-", "partial: the play-off places are not given"),
    "promotion rounds to the First Amateur Division": ("-", "-", "-", "partial: the play-off places are not given"),
    "top four promotion play-offs": ("1-4", "-", "-", "ok"),
    # Turkey 2005/06 to 2008/09: two ties between places of the one division, the winners play one match for one
    # place (a play-off of 4, as the kind builds it). The tool read only "3rd".
    "3rd v 6th and 4th v 5th, winners meet for the third place": ("3-6", "-", "-", "ok"),
    # Croatia: places of the play-off and play-out groups of a split season.
    "1st of play-off group: promotion play-off; 5th of play-out group: relegation play-off (South)":
        ("1", "12", "-", "partial: places of the split groups (1st of 6, 5th of the play-out group of 6)"),
    "1st of play-off group: promotion play-off; 5th of play-out group: relegation play-off":
        ("1", "12", "-", "partial: places of the split groups (1st of 6, 5th of the play-out group of 6)"),
    "1st of play-off group: promotion play-off": ("1", "-", "-", "partial: the 1st of the play-off group of the split"),
    "2nd (next club if a B team is 1st or 2nd): promotion play-off v HNL 9th; 11th: relegation play-off v "
    "third-level South runner-up": ("2", "11", "-", "partial: the next club when a B team is 1st or 2nd"),
    "promotion play-off (first eligible non-reserve club after the champion); 14th: relegation play-off":
        ("2", "14", "-", "partial: the first club after the champion that is not a reserve team"),
    "11th v the 2. NL runner-up, two legs (relegation play-off)": ("-", "11", "-", "ok"),
    # Denmark.
    "2nd and 3rd play two-legged finals against Superliga relegation play-off clubs.": ("2-3", "-", "-", "ok"),
    "Relegation: the two 3rd-placed clubs of the relegation round play over two legs, loser relegated. European "
    "play-offs as in 2016/17.": ("-", "-", "europe",
                                 "partial: the relegation play-off of the split groups (needs the split kind)"),
    "European play-off: relegation-round winner (7th) plays the lowest championship-round club without a "
    "European place in one match.": ("-", "-", "europe", "ok"),
    "The best eligible runner-up of each group (Thisted FC, Holbæk B&IF) play two legs for one place in the 1st "
    "Division.": ("2", "-", "-", "partial: the runner-up of each group"),
    "Promotion game between the two groups' next-best eligible clubs (table shows 3rd places).":
        ("3", "-", "-", "partial: the 3rd of each group"),
    "Promotion game between the group runners-up.": ("2", "-", "-", "partial: the runner-up of each group"),
    "Promotion game between the group runners-up. Relegation game between the 15th-placed clubs.":
        ("2", "15", "-", "partial: the places of each group"),
    "Promotion game between the group runners-up. Relegation game between the two 16th-placed clubs, loser "
    "relegated.": ("2", "16", "-", "partial: the places of each group"),
    "The two group winners play a two-legged promotion play-off for one place.":
        ("1", "-", "-", "partial: the winner of each group"),
    "14th of each group in relegation play-offs.": ("-", "14", "-", "partial: the places of each group"),
    "11th of each group in play-offs.": ("-", "11", "-", "partial: the places of each group"),
    # Finland.
    "zone relegation groups: 5th and 6th v Kakkonen clubs, two legs": ("-", "5-6", "-",
                                                                       "partial: the places of each zone"),
    "2nd v Veikkausliiga 13th, two legs; 11th-12th v Kakkonen clubs": ("2", "11-12", "-", "ok"),
    "2nd-4th play-off, winner v Veikkausliiga 11th": ("2-4", "-", "-", "ok"),
    "11th v winner of Ykkönen 2nd-4th play-off": ("-", "11", "-", "ok"),
    # France.
    "relegation play-off: the 13th of each group v Division d'Honneur clubs": ("-", "13", "-",
                                                                              "partial: the places of each group"),
    "relegation play-off: the 13th of each group, with the worst 12th and best 14th places, in mini-groups with "
    "Division d'Honneur runners-up": ("-", "12-14", "-", "partial: the places of each group"),
    "relegation play-off: the 13th of each group, the worst 12th and the 2 best 14th v the 22 Division "
    "d'Honneur runners-up": ("-", "12-14", "-", "partial: the places of each group"),
    "relegation play-off: the 13th of each group and the best 12th and 14th places v the 22 Division d'Honneur "
    "runners-up": ("-", "12-14", "-", "partial: the places of each group"),
    "4th v 5th, winner v 3rd, winner v Ligue 1 18th; 18th v National 3rd": ("3-5", "18", "-", "ok"),
    "3rd, 4th and 5th play off for a tie with the Ligue 1 18th; 18th v National 3rd": ("3-5", "18", "-", "ok"),
    "4th v 5th, winner v 3rd, winner v Ligue 1 16th; no relegation play-off": ("3-5", "-", "-", "ok"),
    "4th v 5th, winner v 3rd, winner v Ligue 1 16th; 16th v National 3rd": ("3-5", "16", "-", "ok"),
    "4th v 5th, winner v 3rd, winner v Ligue 1 16th; 16th v the winner of the Ligue 3 play-offs (3rd to 6th), "
    "two legs": ("3-5", "16", "-", "ok"),
    "3rd to 6th play off; the winner plays the Ligue 2 16th over two legs": ("3-6", "-", "-", "ok"),
    # Greece.
    "top 4 of each group: promotion play-off round of 8 for 3 places (2 direct, 3rd via a play-off match)":
        ("1-4", "-", "-", "partial: the places of each group"),
    "top 3 of each group: promotion play-off round of 6 for 2 places; 5th-10th: relegation play-off groups":
        ("1-3", "5-10", "-", "partial: the places of each group"),
    "2nd of play-off round: promotion play-off": ("2", "-", "-", "partial: the 2nd of the play-off round of the split"),
    "group winners: title and promotion play-off": ("1", "-", "title", "partial: the winner of each group"),
    "top 5 of each group: play-off round; 6th-11th: play-out round": ("-", "-", "-",
                                                                       "partial: split rounds (needs the split kind)"),
    "top 5 of each group: play-off round; 6th-10th: play-out round": ("-", "-", "-",
                                                                       "partial: split rounds (needs the split kind)"),
    "top 4 of each group: play-off round; 5th-10th: play-out round": ("-", "-", "-",
                                                                       "partial: split rounds (needs the split kind)"),
    "3rd-6th: promotion play-off group (one place); 13th-16th: relegation play-out group (one place)":
        ("3-6", "13-16", "-", "ok"),
    # Holland: the period winners are not table places.
    "six clubs (period winners) join the Eredivisie 16th and 17th in two groups of four; group winners promoted":
        ("-", "-", "-", "partial: the period winners, not table places"),
    "eight clubs (period winners and best placed) with the Eredivisie 16th and 17th in a 10-club knockout for "
    "two Eredivisie places": ("-", "-", "-", "partial: the period winners, not table places"),
    "Eerste Divisie clubs (period winners and best placed) with the Eredivisie 16th, for one Eredivisie place":
        ("-", "-", "-", "partial: the period winners, not table places"),
    "six clubs (3rd to 8th or period winners) with the Eredivisie 16th; knockout for one Eredivisie place":
        ("3-8", "-", "-", "partial: the period winners take places of 3rd to 8th"),
    "six clubs: two seeded into the quarter-finals, four in the first round, with the Eredivisie 16th; one "
    "Eredivisie place": ("3-8", "-", "-", "partial: the period winners take places of 3rd to 8th"),
    "16th and 17th join the promotion/relegation groups (nacompetitie): two groups of four with six Eerste "
    "Divisie clubs; group winners play in the Eredivisie": ("-", "16-17", "-", "ok"),
    # Ireland and Northern Ireland: a "pos" list of a division.
    "from 2027: bottom club in a play-off with the National League champion (subject to licence)":
        ("-", "-", "-", "ok"),
    # Italy.
    "tie-break play-off match if clubs finish level on points for the title, a European place or a relegation "
    "place": ("-", "-", "tiebreak", "ok"),
    "15th v Serie B 6th, two legs, for one Serie A place; tie-break play-off match if clubs finish level on "
    "points for the title, a European place or a relegation place": ("-", "15", "tiebreak", "ok"),
    "single-match play-off (90 minutes then penalties, neutral venue) if level on points for 1st or 17th place":
        ("-", "-", "tiebreak", "ok"),
    "3rd to 6th, semi-finals and final over two legs; not held if 3rd leads 4th by more than 9 points (3rd then "
    "promoted); aggregate tie goes to the better-placed club":
        ("3-6", "-", "-", "partial: not held if the 3rd leads the 4th by more than 9 points"),
    "play-off winner promoted": ("-", "-", "-", "partial: the play-off places are not given"),
    "relegation play-outs only": ("-", "-", "-", "partial: the play-out places are not given"),
    # Japan and Korea.
    "stage winners meet in a two-legged championship (not held when one club wins both stages)":
        ("-", "-", "title", "ok"),
    "stage winners meet in a two-legged championship; 16th v J2 3rd in a two-legged promotion/relegation series":
        ("-", "16", "title", "ok"),
    "championship stage of 3 to 5 clubs (stage winners and top three overall)": ("-", "-", "title", "ok"),
    "same-rank clubs of the two regions play two-legged ties (extra time, penalties) to set the final order":
        ("-", "-", "title", "partial: ties of the regions (needs the split kind)"),
    "same-rank clubs of the groups play single-match knockout ties to set the final order":
        ("-", "-", "title", "partial: ties of the groups (needs the split kind)"),
    "championship: the two stage winners and the top two of the overall table": ("-", "-", "title", "ok"),
    "championship play-off of the top six": ("-", "-", "title", "ok"),
    "12th v the losing K League 2 play-off finalist, only if Gimcheon Sangmu do not finish last":
        ("-", "12", "-", "partial: only when one club does not finish last"),
    # Northern Ireland, Wales: European play-offs and licences.
    "pos 2 to 6: highest-ranked club with a promotion licence: promotion play-off; pos 11: play-off with the "
    "Championship 2 runner-up if that club holds a licence": ("2", "11", "-", "partial: the first club with a licence"),
    "2nd to 6th plus winner of the 7th to 10th group: European play-offs; 14th: relegation play-off with Cymru "
    "North and South qualifiers": ("-", "14", "europe", "ok"),
    # Norway.
    "14th v 1. divisjon 5th and 1. divisjon 3rd v 4th in single-match semi-finals; the winners meet in a "
    "two-legged final": ("-", "14", "-", "ok"),
    "13th v 1. divisjon 4th, two legs (transition to 16 clubs)": ("-", "13", "-", "ok"),
    # Poland.
    "5th-6th of relegation group: relegation play-offs": ("-", "13-14", "-",
                                                          "partial: the 5th and 6th of the relegation group of 8"),
    "3rd: promotion/relegation play-off v Ekstraklasa 14th (cancelled on 12 June 2009); 13th-14th: relegation "
    "play-off": ("3", "13-14", "-", "ok"),
    # Portugal.
    "15th v 4th of Segunda Liga (3rd non-reserve), two legs, for an 18th place": ("-", "15", "-", "ok"),
    "4th (3rd non-reserve) v 15th of Primeira Liga, two legs": ("4", "-", "-",
                                                                "partial: the 3rd club that is not a reserve team"),
    "16th v 3rd of second tier (non-reserve), two legs": ("-", "16", "-", "ok"),
    "3rd (non-reserve) v 16th of Primeira Liga; 16th v Liga 3 play-off winner; both two legs":
        ("3", "16", "-", "partial: the 3rd club that is not a reserve team"),
    "17th and 18th v Campeonato de Portugal promotion-group runners-up, two legs": ("-", "17-18", "-", "ok"),
    # Russia.
    "a play-off match for the title if the top two are level on points (2002: Lokomotiv beat CSKA 1-0 on 21 "
    "November 2002)": ("-", "-", "tiebreak", "ok"),
    "3rd and 4th play the 14th and 13th of the Premier League over two legs; not held (licensing)":
        ("-", "-", "-", "ok"),
    # Scotland.
    "11th plays the Championship play-off winner (Championship 2nd to 4th) in a two-legged final for a "
    "Premiership place": ("-", "11", "-", "ok"),
    "11th plays the Championship play-off winner (Championship 2nd to 4th) in a two-legged final for a "
    "Premiership place (inference: no change announced)": ("-", "11", "-", "ok"),
    # Spain.
    "3rd to 6th (reserve teams excluded, next club moves up) play off for 1 place: two-legged semi-finals and "
    "two-legged final": ("3-6", "-", "-", "partial: reserve teams leave their place to the next club"),
    "top 4 of each group (16 clubs) play in four cross-regional groups of 4 (home and away, 6 matches); the 4 "
    "group winners go up": ("1-4", "-", "-", "partial: the places of each group"),
    "top 4 of each group (16 clubs) play two-legged ties for 4 places": ("1-4", "-", "-",
                                                                          "partial: the places of each group"),
    "top 4 of each group: group winners play two-legged ties, the 2 winners go up and play for the title; the 2 "
    "losers join the other play-off rounds for the last 2 places": ("1-4", "-", "title",
                                                                    "partial: the places of each group"),
    # Sweden.
    "Group winners play promotion play-offs for Superettan; 10th of each group plays a relegation play-off "
    "against Division 3 clubs.": ("1", "10", "-", "partial: the places of each group"),
    "Group winners play promotion play-offs for Superettan; 5th of each group plays off for Division 1.":
        ("1", "-", "-", "partial: the places of each group (the 5th plays for a place in its own tier)"),
    "Runners-up play the Superettan 13th and 14th over two legs.": ("2", "-", "-",
                                                                    "partial: the runner-up of each group"),
    "Runners-up play the Superettan 13th and 14th; 11th of each group plays a relegation play-off.":
        ("2", "11", "-", "partial: the places of each group"),
    "Runners-up play the Superettan 13th and 14th; 13th of each group plays a relegation play-off against "
    "Division 2 clubs.": ("2", "13", "-", "partial: the places of each group"),
    # Turkey: the play-off for the third promotion place.
    "3rd goes straight to the play-off final; 4th v 7th and 5th v 6th single match at the 4th and 5th, then "
    "two-legged round, neutral final": ("3-7", "-", "-", "ok"),
    # The championship rounds of the MLS and the A-League: a finals series of the class, no place moves.
    "top six to a finals series (elimination finals, semi-finals, preliminary final, grand final)":
        ("-", "-", "title", "ok"),
    "top six play a home-and-away championship series; top two meet in the grand final": ("-", "-", "title", "ok"),
    "top six to a finals series (major and minor semi-finals, two legs, grand final)": ("-", "-", "title", "ok"),
    "top four, page play-off system, grand final": ("-", "-", "title", "ok"),
    "top six, four weeks, double chance, grand final": ("-", "-", "title", "ok"),
    "top six, three weeks, no double chance, grand final": ("-", "-", "title", "ok"),
    "top six; 3rd to 6th play single-leg elimination finals; two-legged semi-finals; grand final":
        ("-", "-", "title", "ok"),
    "top 8 of the single round robin to two-legged quarter-finals, semi-finals and final":
        ("-", "-", "title", "ok"),
    "top 8 of the single round robin to two-legged quarter-finals, semi-finals and final; finalists promoted":
        ("1-8", "-", "-", "ok"),
    "top 8 of the single round robin to two groups of 4 (home and away), top 2 of each to a final group of 4 "
    "(home and away); top 2 promoted": ("1-8", "-", "-", "ok"),
    "3rd to 6th: two-legged ties 3rd v 6th and 4th v 5th, the two winners go up": ("3-6", "-", "-", "ok"),
    "3rd to 6th: two-legged ties 3rd v 6th and 4th v 5th, the two winners go up (inference: copies 2026)":
        ("3-6", "-", "-", "ok"),
    "promotion by play-off stage, see format": ("-", "-", "-", "partial: a stage of groups (see format)"),
}

# The same for long texts, by their start.
BY_START = {
    # Portugal: the 16th of each zone plays to stay in the division (liguilha, poule).
    "16th of each zone to a three-club play-off": ("-", "16", "-", "partial: the places of each zone"),
    # Denmark: the relegation ties of the qualifying groups of a split season.
    "Relegation: first a two-legged cross round, the 3rd of each qualifying group v the 4th of the other.":
        ("-", "-", "europe", "partial: the relegation play-off of the split groups (needs the split kind)"),
    "Relegation: 3rd-placed clubs of the two qualifying groups play over two legs":
        ("-", "-", "europe", "partial: the relegation play-off of the split groups (needs the split kind)"),
    # Italy: a title play-off and a relegation play-off only when clubs are level on points.
    "play-off if level on points for 1st or 17th place": ("-", "-", "tiebreak", "ok"),
}

# Texts with no encoding at all, with the reason (for Max). A key is the start of the text.
PERIOD = "the period winners of a season of periods: the game has no periods"
ZONES = "the winners of zones or series: places of a second stage that the game does not play"
KNOWN_GAPS = {
    "third division play-off: three period winners per group": PERIOD,
    "final round: three period winners (four clubs before 2005/06)": PERIOD,
    "final round: runner-up and three period winners": PERIOD,
    "runner-up and three period winners play two-legged qualifying ties": PERIOD,
    "seven clubs for one Eredivisie place": PERIOD,
    "series winners to championship play-off": ZONES,
    "promotion-series runners-up play each other": ZONES,
    "zone winners to a play-off": ZONES,
    "5 series winners and the 3 best runners-up": ZONES,
    "second phase: top 3 of each subgroup in promotion groups": ZONES,
    "lowest-placed Açores club of the three zones": "a rule of one region of the nation",
    "Final of the two zone winners": ZONES,
    # Slice 20b (Q520): the Serie C groups of 2014/15 on take the files of C1/A, C1/B and C2/A. Their play-offs are
    # one national phase of the clubs of all three groups and the cup winner: not a play-off of one league.
    "28 clubs: ": "a national phase of the clubs of the three Serie C groups: not a play-off of one league",
}

# The forms of a whole text that mean no play-off.
NONE_TEXTS = {"none", "n/a", "see split", "see format"}


def ordinals(s):
    return [int(x) for x in re.findall(ORD, s)]


def places_of(clause):
    """The table places of this division that a clause names, or None."""
    s = clause.strip()
    m = re.match(r"pos\s+([\d,\s]+?)(?:\s+to\s+(\d+))?\s*:", s)
    if m:
        if m.group(2):
            return list(range(int(m.group(1)), int(m.group(2)) + 1))
        return [int(x) for x in m.group(1).replace(" ", "").split(",") if x]
    # The places before the other side of the tie ("16th v 2. Bundesliga 3rd", "9th plays off with ...").
    head = re.split(r"\s(?:v|plays?|with|against|join|joins|to|meet)\s|:", s, maxsplit=1)[0]
    m = re.match(ORD + r"\s*(?:-|to|–)\s*" + ORD, s)
    if m:
        return list(range(int(m.group(1)), int(m.group(2)) + 1))
    m = re.match(r"top\s+(\d+)", s)
    if m:
        return list(range(1, int(m.group(1)) + 1))
    found = ordinals(head)
    if found and re.match(ORD, s):
        return found
    # "Torneo Reducido of 2nd to 9th": a range inside the clause, before any other side of a tie.
    m = re.search(r"\bof\s+" + ORD + r"\s*(?:-|to)\s*" + ORD, s.split(" v ")[0])
    if m:
        return list(range(int(m.group(1)), int(m.group(2)) + 1))
    return None


def kind_of(clause):
    s = clause.lower()
    if "europ" in s or "uefa" in s or "conference league" in s:
        return "europe"
    if "tie-break" in s or "tie break" in s or "level on points" in s:
        return "tiebreak"
    if "title" in s or "grand final" in s or "finals series" in s:
        return "title"
    if "relegation" in s and "promotion" not in s:
        return "down"
    if "promotion" in s and "relegation" not in s:
        return "up"
    return "either"


def fmt(places):
    if not places:
        return "-"
    places = sorted(set(places))
    if places == list(range(places[0], places[-1] + 1)) and len(places) > 1:
        return "%d-%d" % (places[0], places[-1])
    return ",".join(str(p) for p in places)


def encode_playoffs(r, last_clubs=None):
    """(po_up, po_down, po_other, po_status), or None when the tool does not know the text. last_clubs: the count
    of the last row of the file with one, for a row with unknown clubs (the after-table rule)."""
    code, text = r["playoffs"], r["playoffs_text"].strip()
    if code in ("na", "none", "unknown") or text.lower() in NONE_TEXTS:
        return "-", "-", "-", "ok"
    if code in ("see_format", "see_split"):
        return "-", "-", "-", "ok"
    if text in BY_HAND:
        return BY_HAND[text]
    for start, value in BY_START.items():
        if text.startswith(start):
            return value
    for start, reason in KNOWN_GAPS.items():
        if text.startswith(start):
            return "-", "-", "-", "gap: " + reason
    # MLS: a count of clubs in the championship play-offs.
    if r["comp_file"] == "usa_mls":
        return "-", "-", "title", "ok"
    if re.match(r"as \d{4}", text.lower()):
        return None                      # "as 2026/27": the tool reads the earlier row (encode below)
    clubs = r["clubs"] if isinstance(r["clubs"], int) else last_clubs if r["clubs"] == "unknown" else None
    up, down, other = [], [], []
    status = "ok"
    for clause in re.split(r";|(?<=[a-z)])\.\s", text):
        clause = re.sub(r"\(.*?\)", "", clause).strip().rstrip(".")
        if not clause:
            continue
        low = clause.lower()
        if re.search(r"of each (group|zone|series|subgroup)|of the (play-off|relegation|play-out) group", low):
            status = "partial: the places of each group or zone"
        kind = kind_of(clause)
        places = places_of(clause)
        if kind in ("europe", "title", "tiebreak"):
            other.append(kind)
            continue
        if places is None:
            continue                     # a clause that names no place of the division: how a tie is played

        if kind == "either":
            # A promotion/relegation play-off, or a text with neither word: a place in the top half plays for
            # the tier above, a place in the bottom half to stay. A range goes by its first place ("2nd to 9th"
            # of a league of 16 is a promotion play-off), a list ("pos 2,3,12") place by place.
            if clubs is None:
                return None
            ranged = re.match(ORD + r"\s*(?:-|to|–)\s*" + ORD, clause) or re.match(r"top\s+\d+", clause)
            groups = [places] if ranged else [[p] for p in places]
            for g in groups:
                if min(g) <= clubs // 2 and r["tier"] != 1:
                    up.extend(g)
                elif min(g) <= clubs // 2:
                    other.append("title")
                else:
                    down.extend(g)
            continue
        if kind == "up" and r["tier"] == 1:
            other.append("title")
            continue
        (up if kind == "up" else down).extend(places)
    if not (up or down or other):
        return None
    other = "+".join(sorted(set(other))) or "-"
    return fmt(up), fmt(down), other, status


def encode_split(r):
    code, text = r["split"], r["split_text"].strip()
    if code != "text":
        return "-"
    low = text.lower()
    if low in ("single table",):
        return "-"
    if "not played" in low:
        return "-"
    m = re.search(r"after (?:the first )?(\d+) (?:matches|rounds)", low) or re.match(r"(\d+) rounds, then", low)
    sizes = []
    for a, b in re.findall(r"top (four|six|seven|eight)[^;,]*?(?:and|,)\s*(?:(?:the|a)\s+)?bottom (four|six|seven|eight)", low):
        sizes = [a, b]
    m3 = re.search(r"groups of (\w+), (\w+) and (\w+)", low)
    if m3 and not sizes:
        sizes = list(m3.groups())
    words = {"four": 4, "six": 6, "eight": 8, "seven": 7}
    sizes = [words.get(x, x) for x in sizes]
    if not sizes:
        g = re.findall(r"(?:group|round) of (\d+)|round of (\d+)", low)
        sizes = [int(a or b) for a, b in g]
    if not sizes:
        g = re.findall(r"(?:top|bottom) (\d+)|(\d+)(?:st|nd|rd|th) ?(?:to|-) ?(\d+)(?:st|nd|rd|th)", low)
        for a, b, c in g:
            sizes.append(int(a) if a else int(c) - int(b) + 1)
    points = "halved" if "halved" in low else "kept" if ("points kept" in low or "points carried" in low
                                                         or "kept in full" in low) else "unknown"
    if m and sizes:
        return "after %s: %s, points %s" % (m.group(1), "+".join(str(int(x)) for x in sizes), points)
    if "apertura" in low or "torneo inicial" in low or "two stages" in low or "first and second stage" in low:
        return "stages"
    if "zone" in low or "region" in low:
        return "zones"
    if "group" in low and not m:
        return "groups"
    if m or "split" in low:
        return "gap"
    return "-"


def encode_calendar(r):
    rs = r["real_season"]
    if re.fullmatch(r"\d{4}", rs):
        return "calendar"
    if re.fullmatch(r"\d{4}[/–-]\d{2}", rs):
        return "split"
    return "transition"


NUMBERS = {"one": 1, "once": 1, "two": 2, "twice": 2, "three": 3, "four": 4, "five": 5, "six": 6, "a third": 1}
ROBIN = {"single": 1, "double": 2, "triple": 3, "quadruple": 4}


def meetings_of_text(text, clubs):
    """The meetings of a format or split text, or None. Words first, then a count of matches or rounds."""
    low = text.lower()
    m = re.search(r"\b(single|double|triple|quadruple)[ -]round[ -]robin", low)
    if m:
        return ROBIN[m.group(1)]
    m = re.search(r"(?:each pair|the others|each other) (once|twice|three times|four times)(?! more)", low)
    if m:
        return {"once": 1, "twice": 2, "three times": 3, "four times": 4}[m.group(1)]
    if "home and away" in low:
        return 2
    if not isinstance(clubs, int) or clubs < 3:
        return None
    m = (re.search(r"after (?:the first )?(\d+) (matches|rounds|games)", low)
         or re.search(r"(\d+)[ -](rounds|matches|games|match)(?:,? then| plus| first stage)", low)
         or re.search(r"(\d+) (matches|rounds|games)", low))
    return per_pair(int(m.group(1)), m.group(2), clubs) if m else None


def per_pair(n, unit, clubs):
    """The meetings that n matches of a club (clubs - 1 a meeting) or n rounds (n' - 1 a meeting) give, or None."""
    d = clubs + clubs % 2 - 1 if unit.startswith("round") else clubs - 1
    return n // d if d > 0 and n % d == 0 and 1 <= n // d <= 4 else None


def encode_meetings(r):
    """(meetings, evidence) of a league row: an int, or "-" with the reason; None when the tool cannot read it."""
    key = (r["comp_file"], r["season"])
    if key in MEETINGS_FIX:
        return MEETINGS_FIX[key]
    if r["format"] == "not_held" or r["clubs"] == "not_held":
        return "-", "not held"
    for text in (r["format_text"], r["split_text"]):
        m = meetings_of_text(text, r["clubs"])
        if m:
            return m, "text"
    m = re.search(r"(\d+) (matches|rounds|games)", r["notes"].lower())
    if m and isinstance(r["clubs"], int) and r["clubs"] > 2 and per_pair(int(m.group(1)), m.group(2), r["clubs"]):
        return per_pair(int(m.group(1)), m.group(2), r["clubs"]), "notes"
    for first, last, value, why in MEETINGS_BY_HAND.get(r["comp_file"], ()):
        if first <= r["season"] <= last:
            return ("-", "no source settles it: " + why) if value == "?" else (value, why)
    if r["clubs"] == "unknown":
        return "-", "unknown clubs"
    return None


def encode_halves(split):
    return 1 if split == "stages" else 0


# Zone texts that the reading below gets wrong: {(comp_file, season): (zones, reason)}.
ZONES_FIX = {
    # "Apertura (25 clubs) and Clausura in 3 zones of 8": one table of 25, then zones of 8 that leave a club out.
    ("arg_second", 2001): ("gap", "the Apertura is one table of 25, the Clausura three zones of 8"),
}


def encode_zones(r):
    """The zone sizes of one division joined by "+", "-", or "gap"."""
    if (r["comp_file"], r["season"]) in ZONES_FIX:
        return ZONES_FIX[(r["comp_file"], r["season"])][0]
    text = (r["split_text"] + " " + r["format_text"] + " " + r["clubs_text"]).lower()
    if "zone" not in text and not re.search(r"\bregions? of", text):
        return "-"
    text = re.sub(r"\bregions\b", "zones", text)
    m = re.search(r"(\w+) zones(?: \([^)]*\))? of (\d+)", text)
    if m and (m.group(1).isdigit() or m.group(1) in NUMBERS):
        k = int(m.group(1)) if m.group(1).isdigit() else NUMBERS[m.group(1)]
        return "+".join([m.group(2)] * k)
    m = re.search(r"zones? \((\d+) and (\d+) clubs\)", text)
    if m:
        return m.group(1) + "+" + m.group(2)
    m = re.search(r"(\w+) zones\b.*?\((\d+) each", text)
    if m and m.group(1) in NUMBERS:
        return "+".join([m.group(2)] * NUMBERS[m.group(1)])
    sizes = re.findall(r"\b[A-Z][a-z]+ (\d+)", r["clubs_text"])
    if len(sizes) >= 2:
        return "+".join(sizes)
    return "gap"


def encode_extra(r):
    text = (r["split_text"] + " " + r["format_text"]).lower()
    m = re.search(r"plus (\w+) (?:interzonal|derby|inter-zone)", text)
    if m:
        return NUMBERS.get(m.group(1), int(m.group(1)) if m.group(1).isdigit() else "?")
    if re.search(r"plus (?:interzonal|derby|inter-zone)", text):
        return "?"
    return 0


def encode_periods(r):
    for (file, first, last), (value, _why) in PERIODS_BY_HAND.items():
        if r["comp_file"] == file and first <= r["season"] <= last:
            return value
    text = (r["format_text"] + " " + r["split_text"]).lower()
    if "periods unknown" in text:
        return "?"
    m = re.search(r"\b(two|three|four|five|six) periods", text)
    return NUMBERS[m.group(1)] if m else 0


def encode_split_meetings(r, split):
    """The meetings in each section of a split ("1+1", "2+1"), "?" where the text gives none, "-" with no split."""
    m = re.match(r"after (\d+): ([\d+]+), points", split)
    if not m:
        return "-"
    after, sizes = int(m.group(1)), [int(x) for x in m.group(2).split("+")]
    low = r["split_text"].lower()
    # A text per section: "top 6 play a double round ..., bottom 6 a single round ...", "(double round robin, 10
    # matches) ... (single round robin, 7 matches)".
    each = re.findall(r"\b(single|double) round", low)
    if len(each) == len(sizes) > 1:
        return "+".join(str(ROBIN[x]) for x in each)
    one = None
    w = re.search(r"(?:each other|the others(?: in its group| of its group)?) (once|twice|a third time)(?: more)?", low)
    if w:
        one = {"once": 1, "twice": 2, "a third time": 1}[w.group(1)]
    if one is None:
        t = re.search(r"\((\d+) matches\)", low) or re.search(r"(\d+) in all", low)
        if t and len(set(sizes)) == 1 and sizes[0] > 1 and (int(t.group(1)) - after) % (sizes[0] - 1) == 0:
            one = (int(t.group(1)) - after) // (sizes[0] - 1)
    if one is None and len(set(sizes)) == 1 and sizes[0] > 1:
        e = re.search(r"\((\d+) rounds\)", low)
        if e and int(e.group(1)) % (sizes[0] - 1) == 0:
            one = int(e.group(1)) // (sizes[0] - 1)
        elif "home and away" in low:
            one = 2
    if one is None:
        e = re.search(r"(\d+) more (?:matches|rounds)", low)
        if e and len(set(sizes)) == 1 and sizes[0] > 1 and int(e.group(1)) % (sizes[0] - 1) == 0:
            one = int(e.group(1)) // (sizes[0] - 1)
    if one is not None and 1 <= one <= 4:
        return str(one) if len(sizes) == 1 else "+".join([str(one)] * len(sizes))
    return "?"


def encode_rounds(r, meetings, zones, extra, split):
    if not isinstance(meetings, int) or extra == "?" or split == "groups" or (split == "zones" and zones == "-"):
        return "-"
    if r["comp_file"] == "usa_mls":
        return "-"                       # conferences and an unbalanced schedule: no one table (D17)
    if zones not in ("-", "gap"):
        n = max(int(x) for x in zones.split("+"))
    elif (zones == "-" and isinstance(r["clubs"], int) and "group" not in (r["clubs_text"] + r["format_text"]).lower()
          and not re.search(r"\(\s*[A-Z][\w ]* \d+", r["clubs_text"])):     # "32 (A 16, B 16)": groups
        n = r["clubs"]
    else:
        return "-"
    # With an odd count each club rests one round a meeting; its derby or inter-zone match fills that round
    # (Argentina 2024/25: zones of 15, 14 zone matches and 2 inter-zone matches in 16 rounds).
    rests = meetings if n % 2 else 0
    return meetings * (n + n % 2 - 1) + max(0, extra - rests)


def encode():
    rows = [r for r in make_rules.read_rows() if isinstance(r["tier"], int) and r["comp_file"] not in ("na", "new")]
    out, gaps, problems = [], [], []
    last, last_clubs = {}, {}
    for r in rows:
        if isinstance(r["clubs"], int):
            last_clubs[r["comp_file"]] = r["clubs"]
        po = encode_playoffs(r, last_clubs.get(r["comp_file"]))
        if po is None and re.match(r"as \d{4}", r["playoffs_text"].lower()) and r["comp_file"] in last:
            po = last[r["comp_file"]]
        if po is None:
            problems.append("%s (%s %d): play-off text not known: %r" % (r["id"], r["comp_file"], r["season"],
                                                                         r["playoffs_text"]))
            continue
        last[r["comp_file"]] = po
        split = encode_split(r)
        e = {"id": r["id"], "season": r["season"], "comp_file": r["comp_file"], "tier": r["tier"],
             "clubs": r["clubs"] if isinstance(r["clubs"], int) else -1, "po_up": po[0], "po_down": po[1],
             "po_other": po[2], "po_status": po[3], "split": split, "calendar": encode_calendar(r),
             "meetings": "-", "halves": "-", "zones": "-", "extra": "-", "periods": "-", "split_meet": "-",
             "rounds": "-"}
        if r["comp_file"] not in LEAGUE_NOT:
            meet = encode_meetings(r)
            if meet is None:
                problems.append("%s (%s %d): meetings not known: format %r, split %r" % (
                    r["id"], r["comp_file"], r["season"], r["format_text"][:80], r["split_text"][:80]))
                continue
            zones, extra = encode_zones(r), encode_extra(r)
            e.update(meetings=meet[0], halves=encode_halves(split), zones=zones, extra=extra,
                     periods=encode_periods(r), split_meet=encode_split_meetings(r, split),
                     rounds=encode_rounds(r, meet[0], zones, extra, split))
            if r["clubs"] == "unknown":
                # A future row with no announcement: its structure is the last known row's (D20, the after-table
                # rule), which season_league_row (season_league.c) takes. "?" here, -1 in the C table.
                e.update(halves="?", zones="?", extra="?", periods="?", split_meet="-", rounds="-")
            if meet[0] == "-" and meet[1] not in ("not held", "unknown clubs") and not meet[1].startswith("unknown"):
                gaps.append((r["id"], r["season"], r["comp_file"], "meetings", "the class's meetings stay: " + meet[1],
                             r["format_text"]))
            for col, bad in (("zones", "gap"), ("extra", "?"), ("periods", "?")):
                if e[col] == bad and r["clubs"] != "unknown":
                    gaps.append((r["id"], r["season"], r["comp_file"], col, "the text does not give it",
                                 r["split_text"] if col != "periods" else r["format_text"]))
            if "?" in str(e["split_meet"]):
                gaps.append((r["id"], r["season"], r["comp_file"], "split_meet",
                             "the text does not give the meetings of each section", r["split_text"]))
        out.append(e)
        if po[3] != "ok":
            gaps.append((r["id"], r["season"], r["comp_file"], "playoffs", po[3], r["playoffs_text"]))
        if split == "gap":
            gaps.append((r["id"], r["season"], r["comp_file"], "split", "the tool cannot read the split",
                         r["split_text"]))
    return out, gaps, problems


def count(places):
    if places == "-":
        return 0
    n = 0
    for part in places.split(","):
        a, _, b = part.partition("-")
        n += (int(b) - int(a) + 1) if b else 1
    return n


def tsv_text(rows):
    lines = ["\t".join(TSV_COLUMNS)]
    for e in rows:
        lines.append("\t".join(str(e[c]) for c in TSV_COLUMNS))
    return "\n".join(lines) + "\n"


def gaps_text(gaps):
    lines = ["id\tseason\tcomp_file\tcolumn\treason\ttext"]
    for g in gaps:
        lines.append("\t".join(str(x).replace("\t", " ") for x in g))
    return "\n".join(lines) + "\n"


def c_list(values):
    """A C initializer of SEASON_LEAGUE_PARTS ints."""
    if len(values) > 4:
        raise SystemExit("encode_leagues.py: more than 4 zones or split sections: %s" % values)
    return ", ".join(str(v) for v in (list(values) + [0] * 4)[:4])


def c_text(rows):
    lines = ["/* The structured league rules: play-off places, splits and calendars (0.6.0 slice 19), and the structure",
             " * of each league row (0.7.2 league sizes, slice 1). Made by",
             " * tools/seasons/encode_leagues.py from the rules table (docs/research/rules-0.6.0): do not edit by",
             " * hand. Values: season_league.h. */",
             '#include "season_league.h"', "",
             "const SeasonLeagueRule season_league_rules[] = {"]
    for e in rows:
        m = re.match(r"after (\d+): ([\d+]+), points (\w+)", e["split"])
        after, groups = (int(m.group(1)), m.group(2).split("+")) if m else (0, [])
        top = int(groups[0]) if groups else 0
        zones = [int(x) for x in e["zones"].split("+")] if e["zones"] not in ("-", "gap", "?") else []
        meet = e["split_meet"].split("+") if e["split_meet"] not in ("-", "?") else []
        lines.append('    {"%s", %d, %d, %d, %d, %d, %d, %d, %d, %d,' % (
            e["comp_file"], e["season"], count(e["po_up"]),
            min(int(x) for x in re.findall(r"\d+", e["po_up"])) if e["po_up"] != "-" else 0,
            count(e["po_down"]), 1 if e["po_status"] == "ok" else 0, after, top,
            {"-": 0, "split": 0, "calendar": 1, "transition": 2}[e["calendar"]] if e["calendar"] != "split" else 0,
            {"kept": 0, "halved": 1, "unknown": 2}[m.group(3)] if m else 0))
        lines.append('     %d, %d, %d, {%s}, %d, %d, {%s}, {%s}, %d},' % (
            e["meetings"] if isinstance(e["meetings"], int) else 0,
            e["halves"] if isinstance(e["halves"], int) else -1 if e["halves"] == "?" else 0,
            -1 if e["zones"] == "?" else len(zones), c_list(zones),
            e["extra"] if isinstance(e["extra"], int) else -1,
            e["periods"] if isinstance(e["periods"], int) else -1,
            c_list([int(x) for x in groups]), c_list([int(x) if x != "?" else 0 for x in meet]),
            e["rounds"] if isinstance(e["rounds"], int) else 0))
    lines += ["};", "const int season_league_rule_count = %d;" % len(rows), ""]
    return "\n".join(lines)


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("--write", action="store_true")
    ap.add_argument("--check", action="store_true")
    args = ap.parse_args(argv)
    rows, gaps, problems = encode()
    if problems:
        for p in problems:
            print("encode_leagues.py: " + p, file=sys.stderr)
        return 1
    texts = {OUT_TSV: tsv_text(rows), OUT_GAPS: gaps_text(gaps), OUT_C: c_text(rows)}
    if not (args.write or args.check):
        print("%d league rows, %d with play-off places, %d with a split, %d calendar-year rows, %d gaps" % (
            len(rows), sum(1 for e in rows if e["po_up"] != "-" or e["po_down"] != "-"),
            sum(1 for e in rows if e["split"].startswith("after")),
            sum(1 for e in rows if e["calendar"] == "calendar"), len(gaps)))
        return 0
    if args.write:
        for path, text in texts.items():
            with open(path, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(text)
            print("wrote " + os.path.relpath(path, ROOT))
        return 0
    bad = 0
    for path, text in texts.items():
        have = open(path, encoding="utf-8").read() if os.path.exists(path) else None
        if have != text:
            print("encode_leagues.py: %s is not up to date: run --write" % os.path.relpath(path, ROOT),
                  file=sys.stderr)
            bad += 1
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
