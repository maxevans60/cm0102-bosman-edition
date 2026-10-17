"""Season settings: 2004/05 from Football Manager 2005 facts and the September 2005 Update Team database.

FM2005's final tables are those of 2003/04 (season end year 2004). Sources of every fact that FM2005 lacks are
given next to it (read 2026-10-17)."""
START = 2004                        # first season of the new database (2004/05)
TEMPLATE = "September 2005"         # the template's name in databases-date-shifts.tsv
FILL = "October 2003"               # second ratings source for FM players that the template lacks (Q382)
HIST_SRC = "September 2010 ODB"     # competition history 2001/02 to 2003/04 where FM lacks it
AFTER_START = ["Olympic Games"]     # national competitions of the start year played after the start (August 2004)

ENGLAND_NATION = 61                 # the template's nation id of England
# FM2005 competition id -> template division, top to bottom; the Conference feeds the Third Division.
ENGLAND_DIVISIONS = {7: "English Premier Division", 8: "English First Division", 9: "English Second Division",
                     10: "English Third Division"}
ENGLAND_FEEDER = 99
# (automatic promotions, relegations) of each division in 2003/04; one automatic promotion from the Conference.
ENGLAND_MOVES = {7: (0, 3), 8: (2, 3), 9: (2, 4), 10: (3, 2)}
FEEDER_AUTO = 1
# Play-off winners of 2003/04, by FM competition id, with the template's club names.
PLAYOFF_WINNERS = {
    8: "Crystal Palace",            # https://en.wikipedia.org/wiki/2004_Football_League_First_Division_play-off_Final
    9: "Brighton and Hove Albion",  # https://en.wikipedia.org/wiki/2004_Football_League_Second_Division_play-off_Final
    10: "Huddersfield Town",        # https://en.wikipedia.org/wiki/2004_Football_League_Third_Division_play-off_Final
    99: "Shrewsbury Town",          # https://en.wikipedia.org/wiki/2004_Football_Conference_play-off_Final
}

# Clubs whose FM2005 and template names do not match (FM long name -> template long name). Randers FC was
# formed in 2003 from Randers Freja (https://en.wikipedia.org/wiki/Randers_FC).
CLUB_FIX = {"Borussia Mönchengladbach": "Borussia M'gladbach", "Randers Football Club": "Randers Freja FC"}

# Clubs of FM2005's 2003/04 tables that closed in summer 2004 -> the FM club that took their place.
# Wimbledon moved and became Milton Keynes Dons in June 2004 (https://en.wikipedia.org/wiki/Milton_Keynes_Dons_F.C.);
# Telford United folded and AFC Telford United was formed (https://en.wikipedia.org/wiki/AFC_Telford_United).
SUCCESSORS = {"Wimbledon": "Milton Keynes Dons", "Telford United": "AFC Telford United"}

# The real 2004/05 top divisions of other nations, keyed by the template's division name.
REAL = {}
PROMOTED = {}
