"""Season settings: 2006/07 from Football Manager 2007 facts and the November 2007 Update Team database.

FM2007's final tables are those of 2005/06 (season end year 2006). Sources of every fact that FM2007 lacks are
given next to it (read 2026-10-17)."""
START = 2006                        # first season of the new database (2006/07)
TEMPLATE = "November 2007"          # the template's name in databases-date-shifts.tsv
FILL = "September 2005"             # second ratings source for FM players that the template lacks
HIST_SRC = "September 2010 ODB"     # competition history 2000/01 to 2005/06 where FM lacks it
AFTER_START = ["Asian Games Football Tournament"]   # played in December 2006, after the start

ENGLAND_NATION = 61                 # the template's nation id of England
ENGLAND_DIVISIONS = {7: "English Premier Division", 8: "English First Division", 9: "English Second Division",
                     10: "English Third Division"}
ENGLAND_FEEDER = 99
# (automatic promotions, relegations) of each division in 2005/06; one automatic promotion from the Conference.
ENGLAND_MOVES = {7: (0, 3), 8: (2, 3), 9: (2, 4), 10: (3, 2)}
FEEDER_AUTO = 1
# Play-off winners of 2005/06, by FM competition id, with the template's club names.
PLAYOFF_WINNERS = {
    8: "Watford",                   # https://en.wikipedia.org/wiki/2006_Football_League_Championship_play-off_Final
    9: "Barnsley",                  # https://en.wikipedia.org/wiki/2006_Football_League_One_play-off_Final
    10: "Cheltenham Town",          # https://en.wikipedia.org/wiki/2006_Football_League_Two_play-off_Final
    99: "Hereford United",          # https://en.wikipedia.org/wiki/2006_Conference_National_play-off_Final
}

# Clubs whose FM2007 and template names do not match (FM long name -> template long name). FM2007 also has an
# older "Erciyesspor" with no players; the 2006/07 Süper Lig club is Kayseri Erciyesspor.
CLUB_FIX = {"Borussia Mönchengladbach": "Borussia M'gladbach", "Kayseri Erciyesspor": "Erciyesspor"}
SUCCESSORS = {}

# The real 2006/07 top divisions of other nations, keyed by the template's division name. English Wikipedia, one
# URL per list. Names follow the template's spelling where it differs. Kerkyra, AEL (Greece) and Pasching (Austria)
# are not in the template.
REAL = {
    'German First Division': ['Alemannia Aachen', 'Arminia Bielefeld', 'Bayer Leverkusen', 'Bayern Munich', 'VfL Bochum', 'Borussia Dortmund', "Borussia M'gladbach", 'Eintracht Frankfurt', 'Energie Cottbus', 'Hamburger SV', 'Hannover 96', 'Hertha BSC', 'Mainz 05', '1. FC Nürnberg', 'Schalke 04', 'VfB Stuttgart', 'Werder Bremen', 'VfL Wolfsburg'],   # 18 clubs, https://en.wikipedia.org/wiki/2006–07_Bundesliga
    'French First Division': ['Auxerre', 'Bordeaux', 'Le Mans', 'Lens', 'Lille', 'Lorient', 'Lyon', 'Marseille', 'Monaco', 'Nancy', 'Nantes', 'Nice', 'Paris Saint-Germain', 'Rennes', 'Saint-Étienne', 'Sedan', 'Sochaux', 'Toulouse', 'Troyes', 'Valenciennes'],   # 20 clubs, https://en.wikipedia.org/wiki/2006–07_Ligue_1
    'Dutch Premier Division': ['PSV', 'Ajax', 'AZ', 'Twente', 'Heerenveen', 'Roda JC', 'Feyenoord', 'Groningen', 'Utrecht', 'NEC', 'NAC Breda', 'Vitesse', 'Sparta Rotterdam', 'Heracles Almelo', 'Willem II', 'Excelsior', 'RKC Waalwijk', 'ADO Den Haag'],   # 18 clubs, https://en.wikipedia.org/wiki/2006–07_Eredivisie
    'Portuguese Premier League': ['Académica', 'Beira-Mar', 'Belenenses', 'Benfica', 'Boavista', 'Braga', 'Desportivo das Aves', 'Estrela da Amadora', 'Marítimo', 'Nacional', 'Naval 1º de Maio', 'Paços de Ferreira', 'Porto', 'Sporting', 'União de Leiria', 'Vitória Futebol Clube'],   # 16 clubs, https://en.wikipedia.org/wiki/2006–07_Primeira_Liga
    'Belgian First Division': ['Anderlecht', 'Standard Liège', 'Club Brugge', 'Gent', 'Genk', 'Germinal Beerschot', 'Zulte-Waregem', 'Lokeren', 'Westerlo', 'FC Brussels', 'Charleroi', 'Roeselare', 'Mouscron', 'Cercle Brugge', 'Sint-Truiden', 'Beveren', 'Lierse', 'Mons'],   # 18 clubs, https://en.wikipedia.org/wiki/2006–07_Belgian_First_Division
    'Turkish Premier Division': ['Fenerbahçe', 'Beşiktaş', 'Kayserispor', 'Erciyesspor', 'Galatasaray', 'Konyaspor', 'Trabzonspor', 'Ankaragücü', 'Ankaraspor', 'Gençlerbirliği', 'Bursaspor', 'Denizlispor', 'Gaziantepspor', 'Sakaryaspor', 'Sivasspor', 'Antalyaspor', 'Rizespor', 'Manisaspor'],   # 18 clubs, https://en.wikipedia.org/wiki/2006–07_Süper_Lig
    'Greek National A Division': ['AEK Athens', 'AEL', 'Apollon Kalamarias', 'Aris', 'Atromitos', 'Aigaleo', 'Ergotelis', 'Ionikos', 'Iraklis', 'Kerkyra', 'OFI', 'Olympiakos Piraeus', 'Panathinaikos', 'Panionios', 'PAOK', 'Skoda Xanthi'],   # 16 clubs, https://en.wikipedia.org/wiki/2006–07_Super_League_Greece
    'Austrian Premier Division': ['Red Bull Salzburg', 'Ried', 'Mattersburg', 'Rapid Wien', 'Pasching', 'Austria Wien', 'Sturm Graz', 'Rheindorf Altach', 'Wacker Innsbruck', 'Liebherr GAK'],   # 10 clubs, https://en.wikipedia.org/wiki/2006–07_Austrian_Football_Bundesliga
    'Swiss National Division A': ['Zürich', 'Basel', 'Sion', 'Young Boys', 'St. Gallen', 'Grasshopper', 'Thun', 'Luzern', 'Aarau', 'Schaffhausen'],   # 10 clubs, https://en.wikipedia.org/wiki/2006–07_Swiss_Super_League
    'Danish Premier Division': ['Aalborg', 'Horsens', 'Brøndby', 'Esbjerg fB', 'København', 'Midtjylland', 'Nordsjælland', 'OB', 'Randers', 'Silkeborg', 'Vejle', 'Viborg'],   # 12 clubs, https://en.wikipedia.org/wiki/2006–07_Danish_Superliga
    'Italian Serie A': ['Ascoli', 'Atalanta', 'Cagliari', 'Catania', 'Chievo', 'Empoli', 'Fiorentina', 'Inter', 'Lazio', 'Livorno', 'Messina', 'Milan', 'Palermo', 'Parma', 'Reggina', 'Roma', 'Sampdoria', 'Siena', 'Torino', 'Udinese'],   # 20 clubs, https://en.wikipedia.org/wiki/2006–07_Serie_A
}
# Promoted in 2006, in their 2005/06 second-tier order, where FM2007 has no 2005/06 table.
PROMOTED = {
    'French First Division': ['Valenciennes', 'Sedan', 'Lorient'],   # https://en.wikipedia.org/wiki/2005–06_Ligue_2
}
