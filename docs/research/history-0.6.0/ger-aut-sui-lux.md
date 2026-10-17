# History research: Germany, Austria, Switzerland, Luxembourg

Part `ger-aut-sui-lux`, slice 11c. Research date 2026-10-17. The results are in `ger-aut-sui-lux.tsv`: 90 rows, 16 competitions, no `unknown` rows. 13 rows are `not held`.

## Label conventions

### Season-end year

The label year is the year in which the season ends. These 3.9.68 rows prove it:

- German First Division: 2001 FC Bayern München, FC Schalke 04 (2000/01). 2002 BV Borussia Dortmund, Bayer 04 Leverkusen (2001/02).
- German Second Division: 2001 1.FC Nürnberg, Borussia M'gladbach (2000/01). 2002 Hannover 96, Arminia Bielefeld (2001/02).
- German Cup: 2001 FC Schalke 04, 1.FC Union Berlin (final May 2001). 2002 FC Schalke 04, Bayer 04 Leverkusen (final May 2002).
- German Regional Division North: 2001 1.FC Union Berlin, SV Babelsberg 03 (2000/01). 2002 VfB Lübeck, TSV Eintracht Braunschweig (2001/02).
- German Regional Division South: 2001 Karlsruher SC, VfB Stuttgart Amateure (2000/01). 2002 SV Wacker Burghausen, SV Eintracht Trier 05 (2001/02).
- Austrian Premier Division: 2000 FC Tirol Innsbruck, SK Sturm Graz (1999/2000). 2001 FC Tirol Innsbruck, SK Rapid Vienna (2000/01).
- Austrian FA Cup: 2000 Grazer AK, SV Salzburg (1999/2000). 2001 FC Austria Kärnten, FC Tirol Innsbruck (2000/01).
- Swiss Cup: 1997 FC Sion, FC Lucerne (1996/97). 2000 FC Zürich, Lausanne-Sports (1999/2000).
- Luxembourg National Division: 1999 AS La Jeunesse d'Esch-Alzette (1998/99). 2000 F91 Dudelange (1999/2000).
- Luxembourg Cup: 1998 CS Grevenmacher, Avenir Beggen (1997/98). 2000 AS La Jeunesse d'Esch-Alzette, FC Mondercange (1999/2000).

### Calendar year of the match

These competitions are played in July or August before the season starts. The label year is the year of the match.

- German League Cup (DFB-Ligapokal): 3.9.68 holds 2000 FC Bayern München, Hertha BSC Berlin, and 2001 Hertha BSC Berlin, FC Schalke 04. These are the finals of July 2000 and July 2001.
- Austrian Super Cup: 3.9.68 holds 2000 Grazer AK, FC Tirol Innsbruck, and 2001 FC Austria Kärnten, FC Tirol Innsbruck. These are the matches of 30 June 2000 and 7 July 2001.

### Austrian First Division and Swiss National Division A: inference

The 3.9.68 rows of these two leagues are not consistent. The early rows follow the season-end year. The later rows are one year early.

Austrian First Division, 3.9.68 against the real champions:

| label | 3.9.68 | real champion of the season that ends in the label year |
|---|---|---|
| 1995 | Grazer AK | Grazer AK (1994/95) |
| 1996 | FC Linz | FC Linz (1995/96) |
| 1997 | SC Austria Lustenau | Austria Lustenau (1996/97) |
| 1998 | SW Bregenz | Vorwärts Steyr (1997/98) |
| 1999 | VfB Admira Wacker Mödling | Schwarz-Weiß Bregenz (1998/99) |
| 2000 | FC Austria Kärnten | Admira Wacker (1999/2000) |

Swiss National Division A, 3.9.68 against the real champions:

| label | 3.9.68 | real champion of the season that ends in the label year |
|---|---|---|
| 1995 | Grasshopper Club Zurich | Grasshopper (1994/95) |
| 1996 | FC Sion | Grasshopper (1995/96) |
| 1997 | Grasshopper Club Zurich | Sion (1996/97) |
| 1998 | Servette FC | Grasshopper (1997/98) |
| 1999 | FC St. Gallen | Servette (1998/99) |
| 2000 | Grasshopper Club Zurich | St. Gallen (1999/2000) |

The data probably lost one season (1997/98 Steyr in Austria, 1995/96 Grasshopper in Switzerland), and the rows after the loss moved down one year. This is an inference. I used the season-end year for both leagues for these reasons:

- The other 14 competitions in this part use the season-end year or the match year.
- The September 2005 Swiss row (label 2005, FC Basel) is the 2004/05 champion, which fits the season-end year.
- With the start year, label 2005 would be 2005/06, a season that ends after the September 2005 game starts. The brief allows that only for calendar-year competitions.

If the check agent or the lead decides for the start year instead, the values move by one label:

- Swiss: 2002 Grasshopper, 2003 Basel, 2004 Basel, 2005 Zürich (2005/06, runner-up Basel).
- Austrian: 2002 SV Mattersburg, 2003 FC Wacker Tirol, 2004 SV Ried, 2005 SCR Altach (2005/06).

With both conventions, one season between the last 3.9.68 row and label 2002 has no row. These are 2000/01 or 2001/02 for the two leagues, and 2000/01 for both Luxembourg competitions and the Swiss Cup. These labels (2001) are outside `targets.tsv`, so I did not add rows for them.

### Places used

The 3.9.68 rows of Austrian First Division and Swiss National Division A fill only `first`. I filled `first` and `second` for all club competitions, as the brief says.

## The `ended` column

Every `held yes` row has the month of its last match. Points to note for the game start dates:

- German League Cup finals were in late July or early August: 2002-08, 2003-07, 2004-08, 2005-08, 2006-08, 2007-07. A database that starts in July does not have the edition of its own start year.
- Austrian Super Cup matches were in July (2002 to 2004). The 2008 match was on 22 October 2008, after the start of the September 2008 database.
- German Regional Division 2003 to 2005 and the Austrian FA Cup 2003 and 2005 ended in June.
- Austrian Premier Division 2008 and Swiss Cup 2004 ended in April.

Sources for `ended`: the English Wikipedia season pages (Bundesliga infobox dates, cup final dates), fussballdaten.de (last match day of 2. Bundesliga and Austrian Bundesliga, and the club fixture lists of the regional league champions), transfermarkt.com fixture lists (Erste Liga, Swiss league) and RSSSF (Luxembourg). The Luxembourg league ended with a championship round: 12 May 2002 and 25 May 2003 by RSSSF. The transfermarkt Luxembourg lists miss that round, so I did not use them.

## Points of doubt

- Austrian Super Cup 2008: Rapid Wien beat cup winner SV Horn 7-1 on 22 October 2008. English Wikipedia counts it as a Supercup win. German Wikipedia lists the match but leaves it out of its record table. I wrote it as `held yes` and put the doubt in `notes`.
- Austrian Super Cup 2006: an all-star game (Bundesliga against Erste Liga) took the place of the Supercup. It does not map, so the row is `not held`.
- Austrian FA Cup 2008 (2007/08): only second-tier and lower clubs played because of Euro 2008. The winner and finalist are SV Horn and SV Feldkirchen. German Wikipedia shows a final over two legs (12 and 26 May 2008, 3-2 on aggregate). English Wikipedia gives one score, 2-1, which is the second leg.
- German League Cup 2008: a one-off Supercup (Borussia Dortmund against Bayern) took the place of the Ligapokal. It is a different competition, so the row is `not held`.
- German Regional Division South 2004: FC Bayern München Amateure won the league but could not go up. Erfurt (second) and 1. FC Saarbrücken were promoted.
- German Regional Division East and West/Southwest: both leagues ended in 2000 when the third tier went from four groups to two (Nord and Süd). The data holds the new groups as North and South. Nothing maps to East or West/Southwest, so labels 2002 to 2005 are `not held`. The regional league North of 2008 to 2012 is outside the target labels.
- Salzburg: I used the 3.9.68 name SV Salzburg for Red Bull Salzburg (2006 to 2009), as the brief says. Later databases call the club Red Bull Salzburg.
- Tirol: FC Wacker Tirol (2004 First Division champion) is the club founded in 2002 after FC Tirol Innsbruck folded. I used the name FC Wacker Tirol from the September 2005 data. It is not the same entry as FC Tirol Innsbruck.

## Database rows that disagree

Only rows inside the target labels are listed. Many disagreements hold the result of one or two seasons earlier, which suggests the databases were not updated.

| database | competition | label | database holds | real |
|---|---|---|---|---|
| October 2003 | German Cup | 2003 | FC Schalke 04, Bayer 04 Leverkusen (2001/02) | FC Bayern München, 1.FC Kaiserslautern |
| November 2007 | German Cup | 2007 | FC Bayern München, Eintracht Frankfurt (2005/06) | 1.FC Nürnberg, VfB Stuttgart |
| February 2008 | German Cup | 2007 | FC Bayern München, Eintracht Frankfurt (2005/06) | 1.FC Nürnberg, VfB Stuttgart |
| April 2010 SIM | German Cup | 2009 | FC Bayern München, Borussia Dortmund (2007/08) | SV Werder Bremen, Bayer 04 Leverkusen |
| February 2010 ODB | German Cup | 2009 | correct clubs, but third and host hold `T` | third and host empty |
| November 2007 | German First Division | 2007 | FC Bayern München, SV Werder Bremen (2005/06) | VfB Stuttgart, FC Schalke 04 |
| February 2008 | German First Division | 2007 | FC Bayern München, SV Werder Bremen (2005/06) | VfB Stuttgart, FC Schalke 04 |
| April 2010 SIM | German First Division | 2009 | FC Bayern München, SV Werder Bremen, FC Schalke 04 (2007/08) | VfL Wolfsburg, FC Bayern München |
| December 2008 | German First Division | 2008 | the same row twice | one row |
| February 2010 ODB | German First Division | 2009 | correct clubs, but host holds `T` | host empty |
| October 2003 | German Second Division | 2003 | Hannover 96, Arminia Bielefeld (2001/02) | SC Freiburg, 1.FC Köln |
| September 2005 | German Second Division | 2005 | 1.FC Nürnberg, Arminia Bielefeld (2003/04) | 1.FC Köln, MSV Duisburg |
| April 2010 SIM | German Second Division | 2009 | Borussia M'gladbach, TSG Hoffenheim 1899, 1.FC Köln (2007/08) | SC Freiburg, FSV Mainz 05 |
| February 2010 ODB | German Second Division | 2009 | correct clubs, but host holds `T` | host empty |
| October 2003 | German Regional Division North | 2003 | VfB Lübeck, TSV Eintracht Braunschweig (2001/02) | FC Erzgebirge Aue, VfL Osnabrück |
| October 2003 | German Regional Division South | 2003 | SV Wacker Burghausen, SV Eintracht Trier 05 (2001/02) | SpVgg Unterhaching, SSV 2000 Jahn Regensburg |
| September 2005 | German Regional Division South | 2005 | FC Rot-Weiss Erfurt, 1.FC Saarbrücken (the promoted clubs of 2003/04) | Kickers Offenbach, Sportfreunde Siegen |
| September 2005 | Austrian Premier Division | 2005 | Grazer AK, FK Austria Vienna (2003/04) | SK Rapid Vienna, Grazer AK |
| November 2007 | Austrian Premier Division | 2007 | FK Austria Wien, Red Bull Salzburg (2005/06) | Red Bull Salzburg, SV Ried |
| February 2008 | Austrian Premier Division | 2007 | FK Austria Wien, Red Bull Salzburg (2005/06) | Red Bull Salzburg, SV Ried |
| September 2008 | Austrian Premier Division | 2008 | FK Austria Wien, Red Bull Salzburg (2005/06) | SK Rapid Wien, Red Bull Salzburg |
| December 2008 | Austrian Premier Division | 2008 | FK Austria Wien, Red Bull Salzburg (2005/06) | SK Rapid Wien, Red Bull Salzburg |
| March 2009 | Austrian Premier Division | 2008 | FK Austria Wien, Red Bull Salzburg (2005/06) | SK Rapid Wien, Red Bull Salzburg |
| September 2009 | Austrian Premier Division | 2009 | FK Austria Wien, Red Bull Salzburg (2005/06) | Red Bull Salzburg, SK Rapid Wien |
| February 2010 ODB | Austrian Premier Division | 2009 | FK Austria Wien, Red Bull Salzburg (2005/06) | Red Bull Salzburg, SK Rapid Wien |
| April 2010 SIM | Austrian Premier Division | 2009 | FK Austria Wien, Red Bull Salzburg (2005/06) | Red Bull Salzburg, SK Rapid Wien |
| June 2010 | Austrian Premier Division | 2009 | FK Austria Wien, Red Bull Salzburg (2005/06) | Red Bull Salzburg, SK Rapid Wien |
| September 2005 | Austrian First Division | 2005 | FC Wacker Tirol (2003/04) | SV Ried |
| October 2003 | Swiss National Division A | 2003 | FC Basel (2001/02) | Grasshopper Club Zurich |

These rows agree with the result: German Cup October 2003 is wrong but September 2005 (2005), September 2008, December 2008, March 2009 (2008), June 2010 and September 2009 (2009) agree. German First Division October 2003, September 2005, the three 2008 rows (their extra third place, FC Schalke 04, is real), June 2010 and September 2009 agree. German Second Division September 2008, March 2009, June 2010 and September 2009 agree. Swiss National Division A September 2005 agrees under the season-end convention.

The 3.9.68 rows before 2002 that disagree with the real results:

- Austrian First Division 1998 to 2000 and Swiss National Division A 1996 to 2000: one year early (see the tables above).
- Swiss Cup 1999: 3.9.68 holds Grasshopper Club Zurich as winner and Lausanne-Sports as finalist. Lausanne-Sport won the 1998/99 final 2-0.

## Names not in the data

- SV Feldkirchen (Austrian FA Cup 2008, finalist). Feldkirch in the data is a different club in Vorarlberg.
- FC CeBra 01 (Luxembourg Cup 2005, finalist).

All other names match a full name in `clubs/<nation>.tsv`. Where the name changed, I used the 3.9.68 spelling (for example FK Austria Vienna, SK Rapid Vienna, SV Salzburg, BV Borussia Dortmund, TSG Hoffenheim, FC Austria Kärnten).

## Sources

All pages were read with `curl` on 2026-10-17 and parsed from the raw HTML.

- English Wikipedia: List of German football champions, 2. Bundesliga, DFB-Pokal, DFB-Ligapokal, Regionalliga Nord, List of Austrian football champions, Austrian Cup, Austrian Supercup, 2. Liga (Austria), List of Swiss football champions, Swiss Cup, Luxembourg National Division, Luxembourg Cup.
- German Wikipedia: Fußball-Regionalliga Süd, Fußball-Regionalliga Nordost (1994 bis 2000), Fußball-Regionalliga West/Südwest, ÖFB-Supercup, ÖFB-Cup 2007/08, Österreichische Fußballmeisterschaft 2001/02 to 2004/05.
- For `ended` only: English Wikipedia season pages, fussballdaten.de, transfermarkt.com and RSSSF, as above.

All sources for the places are Wikipedia. The check pass needs a second publisher (RSSSF, fussballdaten.de, the governing bodies). worldfootball.net and kicker.de blocked plain requests on the day.
