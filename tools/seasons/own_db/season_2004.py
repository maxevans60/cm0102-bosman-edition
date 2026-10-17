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

# The real 2004/05 divisions of other nations, keyed by the template's division name (calendar-year leagues: the
# 2005 season: the project's club tables put calendar year X+1 in game season X/X+1), and the clubs promoted into each, in the order of their finish below. English Wikipedia, one URL
# per list. Club counts are the rows of each page's table. Names follow the template's spelling where it differs.
# Kerkyra (Greece) is not in the template.
REAL = {
    'Italian Serie A': ['Juventus', 'Milan', 'Internazionale', 'Udinese', 'Sampdoria', 'Palermo', 'Messina', 'Roma', 'Livorno', 'Reggina', 'Lecce', 'Cagliari', 'Lazio', 'Siena', 'Chievo', 'Fiorentina', 'Parma', 'Bologna', 'Brescia', 'Atalanta'],   # 20 clubs, https://en.wikipedia.org/wiki/2004–05_Serie_A
    'Italian Serie B': ['Empoli', 'Torino', 'Perugia', 'Treviso', 'Ascoli', 'Hellas Verona', 'Modena', 'Ternana', 'Piacenza', 'Bari', 'AlbinoLeffe', 'Catania', 'Salernitana', 'Arezzo', 'Cesena', 'Crotone', 'Vicenza', 'Triestina', 'Pescara', 'Venezia', 'Catanzaro', 'Genoa'],   # 22 clubs, https://en.wikipedia.org/wiki/2004–05_Serie_B
    'Spanish First Division': ['Barcelona', 'Real Madrid', 'Villarreal', 'Real Betis', 'Espanyol', 'Sevilla', 'Valencia', 'Deportivo La Coruña', 'Athletic Bilbao', 'Málaga', 'Atlético Madrid', 'Zaragoza', 'Getafe', 'Real Sociedad', 'Osasuna', 'Racing Santander', 'Mallorca', 'Levante', 'Numancia', 'Albacete'],   # 20 clubs, https://en.wikipedia.org/wiki/2004–05_La_Liga
    'Spanish Second Division': ['Cádiz', 'Celta de Vigo', 'Alavés', 'Eibar', 'Recreativo', 'Valladolid', 'Gimnàstic', 'Xerez', 'Tenerife', 'Elche', 'Sporting Gijón', 'Murcia', 'Poli Ejido', 'Almería', 'Lleida', 'Racing Ferrol', 'Málaga B', 'Ciudad de Murcia', 'Córdoba', 'Terrassa', 'Salamanca', 'Pontevedra'],   # 22 clubs, https://en.wikipedia.org/wiki/2004–05_Segunda_División
    'French First Division': ['Lyon', 'Lille', 'Monaco', 'Rennes', 'Marseille', 'Saint-Étienne', 'Lens', 'Auxerre', 'Paris Saint-Germain', 'Sochaux', 'Strasbourg', 'Nice', 'Toulouse', 'Ajaccio', 'Bordeaux', 'Metz', 'Nantes', 'Caen', 'Bastia', 'Istres'],   # 20 clubs, https://en.wikipedia.org/wiki/2004–05_Ligue_1
    'French Second Division': ['Nancy', 'Le Mans', 'Troyes', 'Dijon', 'Châteauroux', 'Sedan', 'Guingamp', 'Montpellier', 'Brest', 'Lorient', 'Grenoble', 'Gueugnon', 'Amiens', 'Laval', 'Créteil', 'Reims', 'Le Havre', 'Clermont', 'Niort', 'Angers'],   # 20 clubs, https://en.wikipedia.org/wiki/2004–05_Ligue_2
    'German First Division': ['Bayern Munich', 'Schalke 04', 'Werder Bremen', 'Hertha BSC', 'VfB Stuttgart', 'Bayer Leverkusen', 'Borussia Dortmund', 'Hamburger SV', 'VfL Wolfsburg', 'Hannover 96', 'Mainz 05', '1. FC Kaiserslautern', 'Arminia Bielefeld', '1. FC Nürnberg', "Borussia M'gladbach", 'VfL Bochum', 'Hansa Rostock', 'SC Freiburg'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Bundesliga
    'German Second Division': ['1. FC Köln', 'MSV Duisburg', 'Eintracht Frankfurt', 'TSV 1860 München', 'SpVgg Greuther Fürth', 'Alemannia Aachen', 'Erzgebirge Aue', 'Dynamo Dresden', 'Wacker Burghausen', 'SpVgg Unterhaching', 'Karlsruher SC', '1. FC Saarbrücken', 'LR Ahlen', 'Energie Cottbus', 'Eintracht Trier', 'Rot-Weiß Oberhausen', 'Rot-Weiss Essen', 'Rot-Weiß Erfurt'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_2._Bundesliga
    'Dutch Premier Division': ['PSV', 'Ajax', 'AZ', 'Feyenoord', 'Heerenveen', 'Twente', 'Vitesse Arnhem', 'Roda JC', 'RKC Waalwijk', 'Willem II', 'Utrecht', 'Groningen', 'NEC', 'ADO Den Haag', 'NAC Breda', 'RBC Roosendaal', 'De Graafschap', 'Den Bosch'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Eredivisie
    'Dutch First Division': ['Heracles Almelo', 'Sparta Rotterdam', 'VVV-Venlo', 'FC Zwolle', 'FC Volendam', 'Helmond Sport', 'Stormvogels Telstar', 'FC Dordrecht', 'Cambuur Leeuwarden', 'AGOVV Apeldoorn', 'BV Veendam', 'Excelsior Rotterdam', 'HFC Haarlem', 'MVV', 'FC Eindhoven', 'TOP Oss', 'Go Ahead Eagles', 'FC Emmen', 'Fortuna Sittard'],   # 19 clubs, https://en.wikipedia.org/wiki/2004–05_Eerste_Divisie
    'Portuguese Premier League': ['Benfica', 'Porto', 'Sporting CP', 'Braga', 'Vitória Sport Club', 'Boavista', 'Marítimo', 'Rio Ave', 'Belenenses', 'Vitória Futebol Clube', 'Penafiel', 'Nacional', 'Gil Vicente', 'Académica', 'União de Leiria', 'Moreirense', 'Estoril', 'Beira-Mar'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Primeira_Liga
    'Portuguese Second League': ['Paços de Ferreira', 'Naval 1º Maio', 'Estrela da Amadora', 'Marco', 'Desportivo das Aves', 'Leixões', 'Feirense', 'Maia', 'Olhanense', 'Varzim', 'Felgueiras', 'Ovarense', 'Alverca', 'Portimonense', 'Santa Clara', 'Gondomar', 'Chaves', 'Espinho'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Liga_de_Honra
    'Scottish Premier Division': ['Rangers', 'Celtic', 'Hibernian', 'Aberdeen', 'Heart of Midlothian', 'Motherwell', 'Kilmarnock', 'Inverness Caledonian Thistle', 'Dundee United', 'Livingston', 'Dunfermline Athletic', 'Dundee'],   # 12 clubs, https://en.wikipedia.org/wiki/2004–05_Scottish_Premier_League
    'Scottish First Division': ['Falkirk', 'St Mirren', 'Clyde', 'Queen of the South', 'Airdrie United', 'Ross County', 'Hamilton Academical', 'St Johnstone', 'Partick Thistle', 'Raith Rovers'],   # 10 clubs, https://en.wikipedia.org/wiki/2004–05_Scottish_First_Division
    'Scottish Second Division': ['Brechin City', 'Stranraer', 'Greenock Morton', 'Stirling Albion', 'Forfar Athletic', 'Alloa Athletic', 'Dumbarton', 'Ayr United', 'Arbroath', 'Berwick Rangers'],   # 10 clubs, https://en.wikipedia.org/wiki/2004–05_Scottish_Second_Division
    'Scottish Third Division': ['Gretna', 'Peterhead', 'Cowdenbeath', "Queen's Park", 'Montrose', 'Elgin City', 'Stenhousemuir', 'East Fife', 'Albion Rovers', 'East Stirlingshire'],   # 10 clubs, https://en.wikipedia.org/wiki/2004–05_Scottish_Third_Division
    'Belgian First Division': ['Club Brugge', 'Anderlecht', 'Genk', 'Standard Liège', 'Charleroi', 'Gent', 'La Louvière', 'Sporting Lokeren', 'Germinal Beerschot', 'Lierse', 'Cercle Brugge', 'Westerlo', 'Mouscron', 'Sint-Truiden', 'KFC Brussels', 'Beveren', 'Oostende', 'Mons'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Belgian_First_Division
    'Belgian Second Division': ['Zulte-Waregem', 'Roeselare', 'Geel', 'Antwerp', 'Waasland', 'Tubize', 'Kortrijk', 'Ronse', 'Beringen-Heusden-Zolder', 'Deinze', 'Hamme', 'Virton', 'Eupen', 'Union', 'Dessel', 'Visé', 'Maasmechelen', 'Aalst'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_in_Belgian_football
    'Turkish Premier Division': ['Fenerbahçe', 'Trabzonspor', 'Galatasaray', 'Beşiktaş', 'Gençlerbirliği', 'Denizlispor', 'Ankaraspor', 'Konyaspor', 'Gaziantepspor', 'Çaykur Rizespor', 'Malatyaspor', 'Samsunspor', 'MKE Ankaragücü', 'Kayserispor', 'Diyarbakirspor', 'Sakaryaspor', 'İstanbulspor', 'Akçaabat Sebatspor'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Süper_Lig
    'Greek National A Division': ['Olympiakos Piraeus', 'Panathinaikos', 'AEK Athens', 'Skoda Xanthi', 'PAOK', 'Aigaleo City', 'Iraklis', 'Chalkidona', 'Kallithea', 'Ionikos', 'Panionios', 'Apollon Kalamarias', 'OFI', 'Aris', 'Ergotelis', 'Kerkyra'],   # 16 clubs, https://en.wikipedia.org/wiki/2004–05_Alpha_Ethniki
    'Austrian Premier Division': ['Rapid Wien', 'Grazer AK', 'FK Austria Vienna', 'Pasching', 'Mattersburg', 'Wacker Tirol', 'Sturm Graz', 'Admira Wacker Mödling', 'Austria Salzburg', 'Bregenz'],   # 10 clubs, https://en.wikipedia.org/wiki/2004–05_Austrian_Football_Bundesliga
    'Swiss National Division A': ['Basel', 'Thun', 'Grasshopper', 'Young Boys', 'Zürich', 'Neuchâtel Xamax', 'St. Gallen', 'Aarau', 'FC Schaffhousen', 'Servette'],   # 10 clubs, https://en.wikipedia.org/wiki/2004–05_Swiss_Super_League
    'Danish Premier Division': ['Brøndby', 'Football Club København', 'Midtjylland', 'AaB', 'Esbjerg fB', 'OB', 'Viborg FF', 'Silkeborg IF', 'AGF', 'Football Club Nordsjaelland', 'Herfølge Boldklub', 'Randers'],   # 12 clubs, https://en.wikipedia.org/wiki/2004–05_Danish_Superliga
    'Norwegian Premier Division': ['Vålerenga', 'Start', 'Lyn', 'Lillestrøm', 'Viking', 'Brann', 'Rosenborg', 'Tromsø', 'Odd Grenland', 'Ham-Kam', 'Fredrikstad', 'Molde', 'Aalesund', 'Bodø/Glimt'],   # 14 clubs, https://en.wikipedia.org/wiki/2005_Tippeligaen
    'Swedish Premier Division': ['Djurgårdens IF', 'IFK Göteborg', 'Kalmar FF', 'Hammarby IF', 'Malmö FF', 'Helsingborgs IF', 'IF Elfsborg', 'BK Häcken', 'Örgryte IS', 'Halmstad BK', 'Gefle IF', 'Landskrona BoIS', 'GIF Sundsvall', 'Assyriska FF'],   # 14 clubs, https://en.wikipedia.org/wiki/2005_Allsvenskan
    'Finnish Premier Division': ['MYPA', 'HJK', 'Tampere United', 'Haka', 'FC Inter', 'FC Lahti', 'AC Allianssi', 'FC KooTeePee', 'TPS', 'KuPS', 'FF Jaro', 'IFK Mariehamn', 'RoPS', 'TP-47'],   # 14 clubs, https://en.wikipedia.org/wiki/2005_Veikkausliiga
    'Russian Premier Division': ['CSKA Moscow', 'Spartak Moscow', 'Lokomotiv Moscow', 'Rubin Kazan', 'FK Moskva Moscow', 'Zenith St. Petersburg', 'Torpedo Moscow', 'Dinamo Moscow', 'Shinnik Yaroslavl', 'Tom Tomsk', 'Saturn', 'Amkar Perm', 'Rostov', 'Krylia Sovetov Samara', 'Alania Vladikavkaz', 'Terek Grozny'],   # 16 clubs, https://en.wikipedia.org/wiki/2005_Russian_Premier_League
    'Czech First Division': ['Sparta Prague', 'Slavia Prague', 'Teplice', 'Sigma Olomouc', 'Slovan Liberec', 'Jablonec', 'Baník Ostrava', 'Drnovice', 'Marila Příbram', 'Tescoma Zlín', 'Brno', 'Blšany', '1.FC Synot', 'Mladá Boleslav', 'Dynamo České Budějovice', 'Opava'],   # 16 clubs, https://en.wikipedia.org/wiki/2004–05_Czech_First_League
    'Yugoslav First Division': ['Partizan', 'Crvena Zvezda', 'Zeta', 'OFK Belgrade', 'Zemun', 'Budućnost Podgorica', 'Hajduk Kula', 'Vojvodina', 'Železnik', 'Smederevo', 'Obilić', 'Radnički Beograd', 'Borac Čačak', 'Čukarički', 'Sutjeska', 'Hajduk Belgrade'],   # 16 clubs, https://en.wikipedia.org/wiki/2004–05_First_League_of_Serbia_and_Montenegro
    'Welsh Premier Division': ['Total Network Solutions', 'Rhyl', 'Bangor City', 'Haverfordwest County', 'Caersws', 'Carmarthen Town', 'Cwmbran Town', 'Aberystwyth Town', 'Welshpool Town', 'Newtown', 'Porthmadog', "Connah's Quay Nomads", 'Port Talbot Town', 'Llanelli', 'Caernarfon Town', 'Airbus UK', 'NEWI Cefn Druids', 'Afan Lido'],   # 18 clubs, https://en.wikipedia.org/wiki/2004–05_Welsh_Premier_League
    'Northern Irish League Premier Division': ['Glentoran', 'Linfield', 'Portadown', 'Dungannon Swifts', 'Limavady United', 'Coleraine', 'Lisburn Distillery', 'Ballymena United', 'Institute', 'Newry City', 'Cliftonville', 'Loughgall', 'Larne', 'Ards', 'Crusaders', 'Omagh Town'],   # 16 clubs, https://en.wikipedia.org/wiki/2004–05_Irish_Premier_League
    'Irish Premier Division': ['Cork City', 'Derry City', 'Shelbourne', 'Drogheda United', 'Longford Town', 'Bohemians', 'Bray Wanderers', 'Waterford United', 'University College Dublin', "St Patrick's Athletic", 'Shamrock Rovers', 'Finn Harps'],   # 12 clubs, https://en.wikipedia.org/wiki/2005_League_of_Ireland_Premier_Division
}
PROMOTED = {
    'Italian Serie A': ['Palermo', 'Cagliari', 'Livorno', 'Messina', 'Atalanta', 'Fiorentina'],   # https://en.wikipedia.org/wiki/2003–04_Serie_B ; Serie B finish order; Fiorentina = play-off winner
    'Italian Serie B': ['Arezzo', 'Catanzaro', 'Cesena', 'Crotone'],   # https://en.wikipedia.org/wiki/2004–05_Serie_B ; promoted from Serie C; page order, finish order not shown
    'Spanish First Division': ['Levante', 'Getafe', 'Numancia'],   # https://en.wikipedia.org/wiki/2003–04_Segunda_División
    'French First Division': ['Saint-Étienne', 'Caen', 'Istres'],   # https://en.wikipedia.org/wiki/2003–04_Ligue_2
    'German First Division': ['1. FC Nürnberg', 'Arminia Bielefeld', 'Mainz 05'],   # https://en.wikipedia.org/wiki/2003–04_2._Bundesliga
    'German Second Division': ['Rot-Weiß Erfurt', '1. FC Saarbrücken', 'Rot-Weiss Essen', 'Dynamo Dresden'],   # https://en.wikipedia.org/wiki/2004–05_2._Bundesliga ; from Regionalliga; page order, finish order not shown
    'Dutch Premier Division': ['Den Bosch', 'De Graafschap'],   # https://en.wikipedia.org/wiki/2004–05_Eredivisie ; champion, then play-off winner
    'Portuguese Premier League': ['Estoril', 'Vitória Futebol Clube', 'Penafiel'],   # https://en.wikipedia.org/wiki/2003–04_Liga_de_Honra
    'Scottish Premier Division': ['Inverness Caledonian Thistle'],   # https://en.wikipedia.org/wiki/2004–05_Scottish_Premier_League
    'Scottish First Division': ['Airdrie United', 'Hamilton Academical'],   # https://en.wikipedia.org/wiki/2003–04_Scottish_Second_Division
    'Scottish Second Division': ['Stranraer', 'Stirling Albion'],   # https://en.wikipedia.org/wiki/2003–04_Scottish_Third_Division
    'Belgian First Division': ['KFC Brussels', 'Oostende'],   # https://en.wikipedia.org/wiki/2004–05_Belgian_First_Division ; page calls the first club FC Brussels; champion, then play-off winner
    'Turkish Premier Division': ['Sakaryaspor', 'Kayserispor', 'Ankaraspor'],   # https://en.wikipedia.org/wiki/2003–04_TFF_First_League ; 1. Lig page lists Kayseri Erciyesspor (2nd) and B.B. Ankaraspor (3rd); the Super Lig table names them Kayserispor and Ankaraspor
    'Greek National A Division': ['Kerkyra', 'Apollon Kalamarias', 'Ergotelis'],   # https://en.wikipedia.org/wiki/2004–05_Alpha_Ethniki
    'Austrian Premier Division': ['Wacker Tirol'],   # https://en.wikipedia.org/wiki/2004–05_Austrian_Football_Bundesliga ; inferred only: the one 2004-05 club absent from the 2003-04 table; page has no promotion text
    'Swiss National Division A': ['FC Schaffhousen'],   # https://en.wikipedia.org/wiki/2003–04_Swiss_Challenge_League ; champion
    'Danish Premier Division': ['Silkeborg IF', 'Randers'],   # https://en.wikipedia.org/wiki/2003–04_Danish_1st_Division
    'Norwegian Premier Division': ['Start', 'Aalesund'],   # https://en.wikipedia.org/wiki/2005_Tippeligaen
    'Swedish Premier Division': ['Assyriska FF', 'Gefle IF', 'BK Häcken'],   # https://en.wikipedia.org/wiki/2005_Allsvenskan
    'Finnish Premier Division': ['KuPS', 'IFK Mariehamn'],   # https://en.wikipedia.org/wiki/2005_Veikkausliiga (the two clubs new to the table)
    'Russian Premier Division': ['Terek Grozny', 'Tom Tomsk'],   # https://en.wikipedia.org/wiki/2005_Russian_Premier_League
    'Czech First Division': ['Mladá Boleslav', 'Drnovice'],   # https://en.wikipedia.org/wiki/2003–04_Czech_2._Liga
    'Yugoslav First Division': ['Radnički Beograd', 'Hajduk Belgrade', 'Budućnost Podgorica', 'Čukarički'],   # https://en.wikipedia.org/wiki/2004–05_First_League_of_Serbia_and_Montenegro ; four Second League group champions, page order
    'Welsh Premier Division': ['Airbus UK'],   # https://en.wikipedia.org/wiki/2003–04_Cymru_Alliance ; PARTIAL: the table also has Llanelli as new, route not on any page checked
    'Northern Irish League Premier Division': ['Loughgall'],   # https://en.wikipedia.org/wiki/2003–04_Irish_First_Division ; champion
    'Irish Premier Division': ['Finn Harps'],   # https://en.wikipedia.org/wiki/2005_League_of_Ireland_Premier_Division (the club new to the table)
}
