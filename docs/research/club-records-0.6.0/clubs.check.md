# Club records 0.6.0: second-source check (slice 15b)

Date 2026-10-19. `clubs.tsv` holds the 52 rows of `rows.tsv` (39 clubs): the tier 1 and 2 rows of `../club-moves-0.6.0/missing.tsv` of the nations of Q359 and Q361, without the MLS (Q404), the B teams (slice 15 fills them) and "new (gre second, group 2)" (no class, slice 20). Five research parts (`parts/out-A..E.tsv`, `parts/check-A..E.md`) made the rows with the brief `research-brief.md`. This file joins the five check files. Each line names the facts that a page of a second publisher agrees with, disagrees with or does not give.

## Summary

- Every row is right: each club played in the division of its row in that real season (all five parts).
- A second source for 47 of 52 rows. No second source: Glebe Rangers (4 rows) and Warrenpoint Town (1 row). Not every second source gives every fact: the division of Ethnikos Piraeus 2007/08 to 2009/10 has the club archive only, and Dergview and Killymoon Rangers have second sources of 2013, not of the row season.
- Stadium: a second source for most rows. Capacity, colours and founded year: mostly one source.
- Colours come from the kits of today in most rows (inference for the row season).
- Two database divisions of slice 15's table are wrong: Rudeš 2009/10 and Dugopolje 2010/11 stand in "Croatian Second Division North", but the Croatian second tier had one group in those seasons. The game keeps the database's North and South classes, so the rows stay in North (the class that the table maps).

## Decisions (slice 15b, recommended answers taken)

- Q504: a fact with one source stays in the record, with "no second source" in `notes`. Glebe Rangers and Warrenpoint Town have no second source that could be read on the day (the RSSSF Northern Ireland pages, worldfootball.net, transfermarkt, soccerway and the Wayback Machine refused the requests). Their division comes from the row of slice 15's table, which has its own source.
- Q360 applies: Drwęca played 2005/06 as KS Finishparkiet Nowe Miasto Lubawskie (a sponsor name, two sources). The record is "Drweca Nowe Miasto Lubawskie".
- Letters with no Windows-1252 form (ę, ł, đ, č, ć, ğ, ı, İ, ş) are written as the plain letter. Š and Ž have a Windows-1252 form, but the database style of Croatian clubs drops them ("NK Rudes"), so the records drop them too.
- An unknown capacity: the record takes the median capacity of the stadiums of the division (inference rule of the design doc).

## Check per club

### Part A

# Check A: Argentina, Japan, South Korea, Australia

- Club Atlético Paraná (2015): agrees: name, division, founded 1907, colours red and white (transfermarkt); not given: city, stadium for 2015 (transfermarkt gives Estadio Pedro Mutio as the current ground), capacity.
- Central Córdoba de Santiago del Estero (2015): agrees: name, division, founded 1919, colours black and white (transfermarkt); disagrees: stadium (transfermarkt gives the current ground Único Madre de Ciudades, which opened after 2015); not given: capacity for 2015.
- Matsumoto Yamaga (2012): agrees: division J2 2012, home city and ground Matsumoto (J.League data site, 21 home games), name (transfermarkt 2012 table); disagrees: founded (transfermarkt 1964, Wikipedia 1965); not given: capacity.
- Iwaki FC (2026/27): agrees: division J2 2026/27, ground Hawaiians Stadium Iwaki (J.League data site, 19 home games; transfermarkt 26/27 table), founded 2012, colours red and navy (transfermarkt); disagrees: capacity (Wikipedia club infobox 5,600, season and stadium pages 5,066); not given: capacity in the second source.
- Daegu FC (2003, 2004, 2006): agrees: division 2003 and 2004 (RSSSF, transfermarkt 2003), division 2006 (RSSSF), city and stadium 2003 and 2004 (RSSSF), founded 2002, colour sky blue (transfermarkt); disagrees: capacity (RSSSF 65,754, Wikipedia 66,422 current); not given: stadium 2006 in a second source.
- Incheon United (2004, 2006): agrees: division 2004 and 2006 (RSSSF), city and stadium 2004 (RSSSF), founded 2003, colours blue and black (transfermarkt); disagrees: capacity (RSSSF 50,256, Wikipedia 49,084 current); not given: stadium 2006 in a second source.
- Gyeongnam FC (2006): agrees: division 2006 (RSSSF), founded 2006, colours red and black (transfermarkt); not given: city, stadium and capacity for 2006 in a second source.
- Gangwon FC (2009, 2010): agrees: division 2009 and 2010 (transfermarkt), founded 2008, orange as a colour (transfermarkt); not given: stadium and capacity for those seasons, second colour.
- Gwangju FC (2011): agrees: division 2011, founded 2010, stadium Gwangju World Cup Stadium (transfermarkt current ground), colour yellow; not given: capacity for 2011, second colour.
- Central Coast Mariners (2005/06): agrees: division, name, city Gosford (RSSSF), founded 2004 and colours yellow and navy (transfermarkt); not given: stadium, capacity.
- Queensland Roar (2005/06): agrees: division, name, city Brisbane (RSSSF), colour orange (transfermarkt); disagrees: founded (Wikipedia 1957 as Hollandia-Inala, transfermarkt 2004); not given: stadium, capacity, second colour for 2005/06.

### Open points

- Iwaki FC row: the calendar rule maps game season 2025 to real 2026, which is the half-year J2/J3 100 Year Vision League. The row source is 2026/27 J2, and Iwaki FC plays in it. Max to confirm that 2026/27 is the intended season.
- Korean names: the older database uses the McCune-Reischauer style (Taejon, Pusan, Chonbuk). Decide whether Daegu FC stays "Daegu" or becomes "Taegu".
- Queensland Roar founded year: 1957 or 2004.
- Matsumoto Yamaga founded year: 1965 or 1964.
- Capacities for Paraná, Central Córdoba, Gyeongnam, Gangwon, Gwangju, Central Coast and Queensland Roar have no second source for that season.
- Gangwon FC second colour for 2009 and 2010 is an inference (White).

### Part B

- Kerkyra (2016): agrees: name (RSSSF, Kerkyra AO), division (RSSSF, ESPN), city and stadium (ESPN match of 10 September 2016 at Kerkyra Stadium); disagrees: capacity on Wikipedia itself (3,000 season page, 2,685 club page); not given: capacity, colours, founded.
- Ethnikos Piraeus (2007, 2008, 2009): agrees: name, city and stadium Elliniko (archived club site), capacity 9,000 for 2008 and 2009 (club site); disagrees: Hellinikon Stadium page gives 10,000; not given: division, colours, founded, capacity 2007/08 (unknown).
- Ethnikos Piraeus (2010): agrees: name, division, forced relegation (RSSSF grk2011), stadium and capacity 9,000 (archived club site); disagrees: none; not given: colours, founded.
- Iraklis Psachna (2011): agrees: name (RSSSF Iraklis Psachnon), division (RSSSF grk2012); disagrees: none; not given: city, stadium, capacity, colours, founded.
- Vataniakos (2013): agrees: name, division, Northern Group (RSSSF grk2014); disagrees: none; not given: city, stadium, capacity, colours, founded.
- Aiolikos (2023): agrees: name (RSSSF Aiolikos Mytilinis, FotMob Aiolikos Mitilini), division (RSSSF, FotMob); disagrees: capacity on Wikipedia itself (3,000, 2,850, 3,300); not given: stadium, capacity, colours, founded.
- Drwęca Nowe Miasto Lubawskie (2005): agrees: division and 18th place (RSSSF pol06, 90minut), founded 1919, white and blue colours, capacity 2,000, city (90minut); disagrees: official name that season was KS Finishparkiet Nowe Miasto Lubawskie (90minut and Polish Wikipedia), RSSSF uses Drwęca; not given: stadium name on 90minut.

### Open points
- Drwęca 2005: decide between Drweca Nowe Miasto Lubawskie and the sponsor name Finishparkiet Nowe Miasto Lubawskie.
- Kerkyra 2016: name form (AO Kerkyra, AOK Kerkyra, Kerkira), capacity (3,000 or 2,685), colour2 is an inference, founded 1968 or the merger date.
- Ethnikos 2007 to 2009: no second source for the division. 2007/08 capacity unknown. Capacity 9,000 or 10,000. Main kit colour not confirmed.
- Iraklis Psachna 2011, Vataniakos 2013, Aiolikos 2023: stadium and capacity have no second source. Iraklis Psachna colour2 unknown.

### Part C

- Valpovka (2002, 2003): agrees: name (RSSSF "Valpovka Valpovo"), city, division (North group both seasons); disagrees: none; not given: stadium, capacity, colours, founded (RSSSF does not give them; capacity unknown in all sources).
- TŠK Topolovac (2002): agrees: city, division (RSSSF South group 2002/03, relegated); disagrees: name (RSSSF "TŠK Naftaš Topolovac"); not given: stadium, capacity, colours, founded.
- Međimurje (2003): agrees: name, city, division (RSSSF North 2003/04, winners), founded 2003 (transfermarkt), stadium SRC Mladost (transfermarkt); disagrees: capacity (transfermarkt 6,000 seats, Wikipedia 8,000); not given: colours.
- Čakovec (2005): agrees: name, city, division (RSSSF North 2005/06), stadium SRC Mladost (transfermarkt); disagrees: capacity (transfermarkt 6,000 seats, Wikipedia 8,000); not given: colours, founded.
- Naftaš Ivanić Grad (2005): agrees: name, city, division (RSSSF South 2005/06), stadium Zelenjak (transfermarkt "SRC Zelenjak"); disagrees: capacity (Wikipedia season page 2,000, hr.wikipedia 500, transfermarkt 0 seats); not given: colours, founded (no source gives it).
- Rudeš (2009): agrees: name, city, division (RSSSF and transfermarkt: single Druga HNL 2009/10), founded 1957 (Nogometni leksikon); disagrees: stadium and capacity (transfermarkt SC Rudes 2,000, today's data; Wikipedia season page Stadion u Sigetu 1,000); not given: colours.
- Dugopolje (2010): agrees: name, city, stadium, capacity 5,200, division (transfermarkt 10/11 second tier, RSSSF single Druga HNL 2010/11); disagrees: founded (transfermarkt 30/11/1951, Wikipedia 1952); not given: colours.

### Open points

- Rudeš 2009 and Dugopolje 2010: the database division "Croatian Second Division North" is wrong. Druga HNL had no groups in 2009/10 and 2010/11.
- TŠK Topolovac 2002: the 2002/03 ground is not confirmed. Park Grofova and 1,500 are today's facts (inference). The RSSSF name "TŠK Naftaš Topolovac" needs a check.
- Valpovka: capacity unknown. The 2002/03 name was "Valpovka - Satnica" by one source only.
- Rudeš 2009: stadium "Stadion u Sigetu" from one source only.
- Naftaš Ivanić Grad: capacity sources disagree. Founded unknown.
- Colours of all clubs come from today's kits (inference).
- Š and Ž have Windows-1252 forms, but the database style strips them ("NK Rudes", "NK HASK Zagreb"). The names follow the database style.

### Part D

- Gençlerbirliği OFTAŞ (2006/07): agrees: name, city Ankara, division and 1st place (RSSSF tur07), stadium Cebeci Inönü (Transfermarkt match sheets); disagrees: none; not given: capacity, colours, founded (en and tr Wikipedia disagree: 1949 or 1947).
- Sibir Novosibirsk (2007): agrees: name, city, division, 3rd place (RSSSF rus2-07), stadium Spartak and capacity 12500 (Transfermarkt, current figure); disagrees: none; not given: colours, founded.
- Spartak-MZhK Ryazan (2007): agrees: name, city, division, withdrawal on 23 July 2007 (RSSSF), stadium CSK (Transfermarkt match sheets); disagrees: Transfermarkt shows Agrokomplekt for the first two home games and as club stadium; not given: 2007 capacity, colours, founded.
- Spartak Nizhny Novgorod (2006): agrees: name, city, move from Chelyabinsk, division, 18th and relegated (RSSSF rus2-06), stadium Lokomotiv (Transfermarkt match sheets); disagrees: none; not given: capacity, colours, founded.
- Krasnodar (2009): agrees: name, city, division, 10th place, entry after refusals (RSSSF rus2-09), stadium Kuban for most home games (Transfermarkt match sheets); disagrees: Transfermarkt shows some home games at Trud Stadium and some marked OZON Arena; not given: 2009 capacity, colours, founded.
- Nizhny Novgorod (2009): agrees: name, city, division, 13th place, entry after refusals (RSSSF rus2-09), stadium Severny (Transfermarkt match sheets); disagrees: capacity (Wikipedia 3180, Transfermarkt 4500 current); not given: colours, founded.

### Open points

- Kit colours for all six rows have one source only (Wikipedia, mostly current kits). Sibir and Krasnodar colours are inference.
- Gençlerbirliği OFTAŞ: founded year 1949 or 1947 is open. Capacity 15000 of Cebeci Inönü has one source.
- Spartak-MZhK Ryazan: 2007 capacity of CSK Stadium is unknown. The club withdrew in July 2007.
- Krasnodar: capacity 35200 has one source. Home games were split between Kuban and Trud.
- worldfootball.net blocked the requests with a bot check, so Transfermarkt and RSSSF are the second sources.

### Part E

- Wexford Youths (2006, real 2007): agrees: name, division (RSSSF ier07); disagrees: none; not given: city, stadium, capacity, colours, founded
- Glebe Rangers (2008, 2009, 2010, 2011): agrees: none (no second publisher page could be read); disagrees: none; not given: all facts
- Dergview (2008, 2010, 2011): agrees: name, stadium (Darragh Park), city (Castlederg) in BBC June 2013, a later season; disagrees: none; not given: capacity, colours, founded, division of the row seasons
- Killymoon Rangers (2008): agrees: name, stadium (Mid-Ulster Sports Arena), city (Cookstown) in Glentoran site and armaghi.com, both 2013; disagrees: none; not given: 2008/09 ground, capacity, colours, founded, division
- Warrenpoint Town (2011): agrees: none (no second publisher page could be read); disagrees: none; not given: all facts
- Galway (2013, real 2014): agrees: name Galway FC, stadium (Eamonn Deacy Park), city (Galway Advertiser 2013), division and promotion (RSSSF ier2014); disagrees: none; not given: capacity, colours, founded
- Cabinteely (2014, real 2015): agrees: name, stadium (Stradbrook), division (the42 March 2015, balls.ie); disagrees: none; not given: capacity, colours, founded
- Treaty United (2020, real 2021): agrees: name, city (Limerick), stadium (Markets Field), division (the42 March 2021); disagrees: capacity (season table 5,000, club page 4,500 today); not given: colours, founded
- Kerry (2022, real 2023): agrees: name, city (Tralee), stadium (Mounthawk Park) (Irish Examiner Feb 2023), division (RSSSF ier2023); disagrees: none; not given: capacity, colours, founded

### Open points

- All 14 rows are right: each club played in that division in that real season.
- Glebe Rangers and Warrenpoint Town have no second source. RSSSF Northern Ireland pages, worldfootball.net, transfermarkt, soccerway and the Wayback Machine could not be read on the day.
- Dergview and Killymoon second sources date from 2013, not the row seasons.
- Killymoon Rangers 2008/09 ground is an inference. The club page names Beechway Pitch or the Mid Ulster Sports Arena.
- Capacity unknown for Galway 2014 and Cabinteely 2015.
- Colours for every club come from the current Wikipedia infobox, except Wexford Youths (club page text). No colour has a second source.
- Galway founded year: 2013 (new club Galway F.C.) is an inference. Galway United claims 1937.
