# Check: France, Belgium, Holland

Check pass of part `fra-bel-ned`, slice 11c. Check date 2026-10-17. The results are in `fra-bel-ned.check.tsv`, one line per filled field.

## Counts

459 fields checked: every filled `first`, `second` and `ended` (not `unknown`) of the 163 `yes` rows, and `held` of the 16 `not held` rows.

- agree: 424
- disagree: 1
- no second source: 34

## Second sources

Each field is checked against a publisher other than the row's source:

- Rows from Wikipedia are checked against RSSSF. These are the French and Belgian top leagues, cups and super cups, the French lower leagues, and the Dutch cup and super cup. The RSSSF pages are the season pages (`belgYY`, `belgcupYY`, `franYY`, `nedYY`, `nedYYext`) and the history lists.
- Rows from RSSSF are checked against Wikipedia. These are the Belgian Second, Third and Fourth Divisions, the Dutch Premier and First Divisions, the French League Cup and the Belgian First Division dates. I used the nl.wikipedia season pages, fr.wikipedia for some Belgian dates and en.wikipedia for the French League Cup.
- worldfootball.net blocks scripted reads, so I did not use it.

## Disagreement

Dutch Premier Division 2006, `second` = AZ. AZ was second in the regular table with 74 points. The KNVB ran play-offs for the European places that season. nl.wikipedia `Eredivisie_2005/06` gives the official final order as 1 PSV and 2 Ajax ("winnaar play-offsfinale"). The RSSSF page `ned06.html`, which is the row's own source, also has an "Official Final Ranking (after playoffs)" of PSV, Ajax, FC Groningen, AZ. Decide which order the data uses. The row follows the regular table.

## No second source (34)

- Belgian Fourth Division A to D 2005, first and second (8). The row's source is nl.wikipedia, and RSSSF has no Vierde Klasse tables for 2004/05.
- French CFA, first, all 8 years. RSSSF gives the group tables but does not name the "champion de France amateur". Every club in the rows was promoted from CFA that season, but that does not prove the title.
- French CFA, ended, 2002, 2005, 2007, 2008, 2009 (5). RSSSF does not give the date of the reserve final. 2003, 2004 and 2006 agree (30 May 2003, 28 May 2004, 3 June 2006).
- Belgian Super Cup, ended, 2002, 2008, 2009 (3). Dutch Super Cup, ended, 2002, 2003, 2006 to 2009 (6). RSSSF gives no date for these matches.
- Belgian Third Division A and B 2009, ended (2). nl.wikipedia says only "het voorjaar van 2009".
- Dutch Cup 2002, ended. Dutch First Division 2008, ended.

## Notes on agreeing fields

- Belgian Second Division 2002, ended: nl.wikipedia gives 5 February 2002 as the end of the season. A 34-round season cannot end then. fr.wikipedia gives May 2002, and that agrees with the row.
- Belgian Third Division A 2008, second: RAA La Louviéroise and RRC Péruwelz both have 54 points, and nl.wikipedia shows both in place 2. La Louvière is listed first.
- Belgian First Division 2009: Anderlecht and Standard were level after 34 rounds, and Standard won the title play-off (21 and 24 May 2009). The row agrees.
- Belgian Super Cup 2006: the match of 22 July 2006 was abandoned, and the replay was on 20 December 2006. The row's `2006-12` agrees. The 2004 match was played on 22 December 2004, which also agrees.
- Belgian League Cup: RSSSF lists finals in 1998, 1999 and 2000 only, so the `not held` rows agree. Belgian Third Division: nl.wikipedia has separate A and B tables in every season from 2001/02 to 2008/09, so the `not held` rows agree.

## Club names marked as inference in the .md

- KSK Hasselt = KSK Kermt-Hasselt: agree. The nl.wikipedia club article says the Kermt-Hasselt merger club of 2001 took the name Koninklijke SK Hasselt in 2006.
- RCS Verviétois = RCS Verviers: agree. The nl.wikipedia club article gives "RCS Verviers" as the club's short name.
- RUS Tournaisienne written as RRC Tournaisien (Fourth Division A 2002): nl.wikipedia confirms RUS Tournaisienne as champion. The row follows the 3.9.68 row by a naming decision, and the .md states it. I counted the result as agree. The lead may still want to review the name.
- The Third Division A and B champions in the notes of the `Belgian Third Division` rows agree with the A and B rows. Those rows are checked above.
