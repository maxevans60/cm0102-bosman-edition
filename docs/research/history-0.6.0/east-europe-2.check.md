# East Europe 2: check pass

Part `east-europe-2` (Russia, Yugoslavia, Greece, Turkey). I checked every filled field in `east-europe-2.tsv` on 2026-10-17. All pages were read raw (wikitext through the Wikipedia API, HTML with curl) and grepped. The results are in `east-europe-2.check.tsv`.

## Counts

| Result | Lines |
|---|---|
| agree | 360 |
| disagree | 0 |
| no second source | 54 |

The 414 lines are 135 `first`, 135 `second`, 133 `ended` and 11 `held` fields. The two `ended` values that are `unknown` (Turkish 2. Division 2002 and 2003) are not filled, so they have no line. No row uses `third` or `host`.

## Second sources

The research used RSSSF for almost every fact. So the check uses other publishers:

- English Wikipedia season articles: Russian Premier League, First Division and Cup, Alpha Ethniki and Super League Greece, Beta Ethniki, Greek Cup and Cup finals, Greek Super Cup, Süper Lig, the Turkish second level, Turkish Cup, and the Yugoslav and Serbia and Montenegro First League, Cup and Second League.
- Russian Wikipedia: the Second Division (PFL) season articles for every zone from 2002 to 2009, and the Premier and First Division articles for the season dates.
- Serbian Wikipedia: the First League articles (season dates) and the South group articles of 2001/02 and 2003/04 (round dates).
- Transfermarkt: Süper Lig and Turkish second level fixtures for the last match days, and the Greek second level table and fixtures of 2005/06.
- RSSSF, only for the facts whose first source is Wikipedia: Greek Cup 2006/07 `ended` (`grk07.html`, final of 5 May 2007).

Wikipedia in English, Russian and Serbian count as one publisher. No line uses Wikipedia to check a Wikipedia fact.

## Disagreements

None.

## No second source (54 lines)

1. Russian Second Division zones, `ended`, 42 lines. The Russian Wikipedia articles give only the dates of the whole season (for example 27 March to 17 November 2002), not the last match day of each zone. Transfermarkt holds the East zone tables but no match dates for these seasons. Each zone month in the research file is inside the season range of its year.
2. Yugoslav Second Division groups, `ended`, 12 lines: East 2002 and 2003, North 2002 to 2005, South 2003 and 2005, West 2002 to 2005. I found no match dates outside RSSSF. For South 2005 (Montenegro group), English Wikipedia shows that the league ended before the relegation play-off of 8 June 2005, but not the month. The West 2002 value (2002-08, a replayed match) is in this list.

## The start-year claim for Greek and Turkish rows in 3.9.68

The claim is correct. I compared the 3.9.68 rows of labels 1995 to 2001 in `db-rows.tsv` with the Wikipedia lists of champions and cup finals:

- Greek National A Division: every label Y from 1995 to 2001 holds the champion and runner-up of season Y/Y+1. Label 1995 Panathinaikos / AEK Athens is 1995/96. The season-end reading would need Panathinaikos / Olympiakos (1994/95). Label 2001 Olympiakos / AEK Athens is 2001/02, and 2000/01 was Olympiakos / Panathinaikos.
- Greek Cup: the winners of labels 1995 to 2001 match the seasons 1995/96 to 2001/02. The runners-up of labels 1995 and 1996 are swapped, as the research says. The 1995/96 final was AEK Athens 7-1 Apollon Athens. The 1996/97 final was AEK Athens against Panathinaikos, won on penalties.
- Greek National B Division: the champions of labels 1995 to 2001 (Kavala, Panionios, Aris, Trikala, Athinaikos, Aigaleo, PAS Giannina) are the champions of 1995/96 to 2001/02. The 1994/95 champion was Paniliakos.
- Turkish Premier Division: labels 1995 to 2000 match the seasons 1995/96 to 2000/01, champion and runner-up. Label 1995 Fenerbahçe / Trabzonspor is 1995/96. The 1994/95 champion was Beşiktaş.
- Turkish FA Cup: labels 1995 to 2000 match the finals of 1995/96 to 2000/01, winner and losing finalist.

## Points of note

1. English Wikipedia's Second League tables for 2001/02 to 2003/04 cite RSSSF `joeg02` to `joeg04` as their source. So the Yugoslav group `first` and `second` lines that agree are checks against another publisher, but probably not against independent data. The Russian, Greek and Turkish second sources do not cite RSSSF in the tables that I used.
2. The Turkish 2. Division `ended` values of labels 2002 and 2003 are `unknown` in the research file. Transfermarkt gives the last match days as 25 May 2003 and 16 May 2004.
3. The Ural and Volga rows of 2003 to 2008 hold the merged Ural-Povolzhye zone. The names agree with that zone. Whether to fill both rows or mark one `not held` is a mapping decision that a source cannot check.
4. Name matches that the research marks as inferences agree with the real clubs: Spartak-MZhK Ryazan (Central 2006), Lukoil Chelyabinsk (Ural-Povolzhye 2003 and 2004), FC Moscow (Cup 2006/07). Whether the data names are the same clubs is not something these pages can show.
5. The Greek A Division `ended` values of 2007/08 to 2009/10 are the last regular match day. English Wikipedia gives 20 April 2008, 26 April 2009 and 11 April 2010, which agree.
