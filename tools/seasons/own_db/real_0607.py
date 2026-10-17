"""The real 2006/07 top divisions (source: the 2006-07 season pages of English Wikipedia, read 2026-10-17). Spellings follow the template where its name differs.
Keyed by the template's club_comp name. England comes from FM2007's 2005/06 tables and the play-off winners."""
REAL = {
    'German First Division': ['Alemannia Aachen', 'Arminia Bielefeld', 'Bayer Leverkusen', 'Bayern Munich', 'VfL Bochum',
        'Borussia Dortmund', "Borussia M'gladbach", 'Eintracht Frankfurt', 'Energie Cottbus', 'Hamburger SV', 'Hannover 96',
        'Hertha BSC', 'Mainz 05', '1. FC Nürnberg', 'Schalke 04', 'VfB Stuttgart', 'Werder Bremen', 'VfL Wolfsburg'],
    'French First Division': ['Auxerre', 'Bordeaux', 'Le Mans', 'Lens', 'Lille', 'Lorient', 'Lyon', 'Marseille', 'Monaco', 'Nancy',
        'Nantes', 'Nice', 'Paris Saint-Germain', 'Rennes', 'Saint-Étienne', 'Sedan', 'Sochaux', 'Toulouse', 'Troyes', 'Valenciennes'],
    'Dutch Premier Division': ['PSV', 'Ajax', 'AZ', 'Twente', 'Heerenveen', 'Roda JC', 'Feyenoord', 'Groningen', 'Utrecht', 'NEC',
        'NAC Breda', 'Vitesse', 'Sparta Rotterdam', 'Heracles Almelo', 'Willem II', 'Excelsior', 'RKC Waalwijk', 'ADO Den Haag'],
    'Portuguese Premier League': ['Académica', 'Beira-Mar', 'Belenenses', 'Benfica', 'Boavista', 'Braga', 'Desportivo das Aves',
        'Estrela da Amadora', 'Marítimo', 'Nacional', 'Naval 1º de Maio', 'Paços de Ferreira', 'Porto', 'Sporting', 'União de Leiria',
        'Vitória Futebol Clube'],
    'Belgian First Division': ['Anderlecht', 'Standard Liège', 'Club Brugge', 'Gent', 'Genk', 'Germinal Beerschot', 'Zulte-Waregem',
        'Lokeren', 'Westerlo', 'FC Brussels', 'Charleroi', 'Roeselare', 'Mouscron', 'Cercle Brugge', 'Sint-Truiden', 'Beveren',
        'Lierse', 'Mons'],
    'Turkish Premier Division': ['Fenerbahçe', 'Beşiktaş', 'Kayserispor', 'Erciyesspor', 'Galatasaray', 'Konyaspor', 'Trabzonspor',
        'Ankaragücü', 'Ankaraspor', 'Gençlerbirliği', 'Bursaspor', 'Denizlispor', 'Gaziantepspor', 'Sakaryaspor', 'Sivasspor',
        'Antalyaspor', 'Rizespor', 'Manisaspor'],
    'Greek National A Division': ['AEK Athens', 'AEL', 'Apollon Kalamarias', 'Aris', 'Atromitos', 'Aigaleo', 'Ergotelis', 'Ionikos',
        'Iraklis', 'Kerkyra', 'OFI', 'Olympiakos Piraeus', 'Panathinaikos', 'Panionios', 'PAOK', 'Skoda Xanthi'],
    'Austrian Premier Division': ['Red Bull Salzburg', 'Ried', 'Mattersburg', 'Rapid Wien', 'Pasching', 'Austria Wien', 'Sturm Graz',
        'Rheindorf Altach', 'Wacker Innsbruck', 'Liebherr GAK'],
    'Swiss National Division A': ['Zürich', 'Basel', 'Sion', 'Young Boys', 'St. Gallen', 'Grasshopper', 'Thun', 'Luzern', 'Aarau',
        'Schaffhausen'],
    'Danish Premier Division': ['Aalborg', 'Horsens', 'Brøndby', 'Esbjerg fB', 'København', 'Midtjylland', 'Nordsjælland', 'OB',
        'Randers', 'Silkeborg', 'Vejle', 'Viborg'],
    'Italian Serie A': ['Ascoli', 'Atalanta', 'Cagliari', 'Catania', 'Chievo', 'Empoli', 'Fiorentina', 'Inter', 'Lazio', 'Livorno',
        'Messina', 'Milan', 'Palermo', 'Parma', 'Reggina', 'Roma', 'Sampdoria', 'Siena', 'Torino', 'Udinese'],
}

# Promoted in 2006, in their 2005/06 second-tier order, for leagues where FM2007 has no 2005/06 table
# (France: 2005-06 Ligue 2, Wikipedia).
PROMOTED = {
    'French First Division': ['Valenciennes', 'Sedan', 'Lorient'],
}
