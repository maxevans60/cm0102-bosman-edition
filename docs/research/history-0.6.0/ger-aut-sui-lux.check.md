# Germany, Austria, Switzerland, Luxembourg: check pass

Part `ger-aut-sui-lux`, slice 11c. Check date 2026-10-17. The check covers every filled field in `ger-aut-sui-lux.tsv`: 90 rows, 242 fields. 76 rows are `held yes` (first, second and ended). 14 rows are `not held` (held). The research `.md` says 13 `not held` rows. The file holds 14.

The research source for the places is Wikipedia. The second source is RSSSF for every place. For `ended`, the second source is RSSSF, or a transfermarkt.de club fixture list when the research used RSSSF or RSSSF gives no date. All pages were read raw with curl and parsed on the day. worldfootball.net, weltfussball.de and kicker.de refused plain requests.

## Counts

| Result | Fields |
|---|---|
| agree | 240 |
| disagree | 0 |
| no second source | 2 |

## No second source

- Austrian Super Cup, label 2008, `ended` 2008-10. RSSSF lists the match (Rapid 7-1 SV Horn) but gives no date. The transfermarkt Rapid fixture list for 2008/09 does not hold it.
- Luxembourg National Division, label 2002, `ended` 2002-05. The research source is RSSSF (title round, last round 12 May). The transfermarkt list leaves out the spring 2002 league rounds, and the Wikipedia season page has no dates.

## Second sources

- Germany: RSSSF season pages `tablesd/duit02.html` to `duit09.html` (Bundesliga, 2. Bundesliga, Regionalliga, cup finals), `duitcuphist.html` (cup) and `duitsupcuphist.html` (Liga Cup finals 1997 to 2007, then unofficial Super Cups 2008 and 2009).
- Austria: `tableso/oost02.html` to `oost09.html` and `oostsupcuphist.html`.
- Switzerland: `tablesz/zwit02.html` to `zwit05.html`, `zwitchamp.html` and `zwitcuphist.html`.
- Luxembourg: `tablesl/lux02.html` to `lux05.html` and `luxcup02.html` to `luxcup05.html`.
- transfermarkt.de club fixture lists for these `ended` values: German Cup 2003 (Bayern), Regionalliga Nord 2003 (Aue), Regionalliga Süd 2003 (Unterhaching) and 2005 (Offenbach), Luxembourg National Division 2003 and all four Luxembourg Cup finals.

## Notes on agreed fields

- Austrian Super Cup 2008: RSSSF lists Rapid 7-1 SV Horn but calls the 2008 edition unofficial. It says that the competition stopped after 2004. The research row is `held yes` with a doubt in `notes`. The clubs agree. Whether the row belongs in the history is for the lead to decide.
- Austrian Super Cup 2005, 2006, 2007 and 2009 `not held`: RSSSF says that the competition stopped after 2004 and lists no match in those years.
- German League Cup 2008 and 2009 `not held`: the RSSSF Liga Cup list ends with the 2007 final. RSSSF lists unofficial Super Cups in July 2008 (Dortmund 2-1 Bayern) and July 2009 (Werder Bremen 2-1 Wolfsburg). These are a different competition.
- German Regional Division East and West/Southwest `not held`: the RSSSF pages for 2001/02 to 2004/05 have only Regionalliga Nord and Süd at the third level.
- Austrian FA Cup 2008: RSSSF has it as the ÖFB Amateur Cup 2007/08. The final was over two legs, SV Horn 1-1 SV Feldkirchen on 12 May and SV Feldkirchen 1-2 SV Horn on 26 May. This agrees with the research.
- 3.9.68 Swiss Cup 1999: RSSSF gives Lausanne-Sports 2-0 Grasshopper Club. The 3.9.68 row swaps winner and finalist, as the research `.md` says.

## Label convention: Austrian First Division and Swiss National Division A

The research uses the season-end year for both leagues. The values in the rows are correct for the seasons that the research names. The convention needs a decision by the lead. I compared the 3.9.68 rows with the RSSSF champions lists.

Swiss National Division A (RSSSF `zwitchamp.html`):

| label | 3.9.68 | RSSSF season that starts in the label year | RSSSF season that ends in the label year |
|---|---|---|---|
| 1995 | Grasshopper Club Zurich | 1995/96 Grasshopper | 1994/95 Grasshopper |
| 1996 | FC Sion | 1996/97 Sion | 1995/96 Grasshopper |
| 1997 | Grasshopper Club Zurich | 1997/98 Grasshopper | 1996/97 Sion |
| 1998 | Servette FC | 1998/99 Servette | 1997/98 Grasshopper |
| 1999 | FC St. Gallen | 1999/2000 St. Gallen | 1998/99 Servette |
| 2000 | Grasshopper Club Zurich | 2000/01 Grasshopper | 1999/2000 St. Gallen |

All six Swiss rows fit the start year. The research explains the rows as a lost 1995/96 season. That explanation is not necessary, because Grasshopper won both 1994/95 and 1995/96. The brief says to find the convention from the 3.9.68 rows of 1997 to 2002. By that rule, the Swiss league uses the start year.

Austrian First Division (RSSSF `oost95.html` to `oost01.html`, second level): the champions are 1994/95 Grazer AK, 1995/96 FC Linz, 1996/97 Austria Lustenau, 1997/98 Vorwärts Steyr, 1998/99 Schwarz-Weiß Bregenz, 1999/2000 Admira/Wacker and 2000/01 FC Kärnten. The 3.9.68 rows 1995 to 1997 fit the season-end year, and the rows 1998 to 2000 fit the start year. Vorwärts Steyr (1997/98) is not in the data. This confirms the research table. Three of the four rows from 1997 to 2000 fit the start year.

Both 3.9.68 lists end with the 2000/01 champion at label 2000. With the research choice (season-end year for the new rows), the seasons run on without a gap: 2000/01 at label 2000, then 2001/02 at label 2002, and label 2001 stays empty. With the start year, 2001/02 belongs at label 2001, which is outside `targets.tsv`. Label 2005 would then be 2005/06, which ends after the September 2005 start. The research choice is defensible, but it does not follow the brief's rule for these two leagues. If the lead picks the start year, the research `.md` gives the moved values. I checked them against RSSSF:

- Swiss: 2002 Grasshopper (2002/03), 2003 Basel (2003/04), 2004 Basel (2004/05), 2005 FC Zürich (2005/06).
- Austrian: 2002 SV Mattersburg (2002/03), 2003 FC Wacker Tirol (2003/04), 2004 SV Ried (2004/05), 2005 SC Rheindorf Altach (2005/06, runner-up LASK Linz).
