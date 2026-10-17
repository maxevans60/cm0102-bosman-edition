# Republic of Ireland: check pass

Part `ireland`, slice 11c. I checked every filled field in `ireland.tsv` on 17 October 2026. The check covers `first` and `second` of the 42 `yes` rows, `ended` where it holds a month (34 rows), and `held` of the 92 `not held` rows. That gives 210 lines in `ireland.check.tsv`. All pages were read as raw HTML with curl.

## Counts

| Result | Lines |
|---|---|
| agree | 137 |
| disagree | 0 |
| no second source | 73 |

## Second sources

- Premier Division and First Division: the English Wikipedia season pages (`2001–02 League of Ireland` to `2009 League of Ireland`, and the `League of Ireland First Division` season pages) for the tables. The research source is RSSSF. For `ended`, the Wikipedia season text and the Transfermarkt fixture lists (`IR1`, `IR2`).
- FAI Cup: the Wikipedia `FAI Cup` list of finals, the Wikipedia season pages for the final dates, and a BBC Sport article of 28 October 2002 for the interim 2002/03 final.
- League Cup: the Transfermarkt fixture list of each season (`ILIP`) for the finalists and the final date, and its winners list for the missing 2002/03 edition. The research sources are RSSSF and Wikipedia.
- FAI Super Cup and LFA President's Cup (`not held` rows): RSSSF `iersupcuphist.html`. It gives the Super Cup 1998 to 2001 only, and the Leinster FA President's Cup "between 1930 and 2002".
- Leinster Senior Cup: Trinity News (1 October 2023).
- P.J. Casey Cup: leagueofirelandseasons.com. It says the 1962/63 final was the only one.
- Blaxnit, Dublin and Belfast Inter-City, North-South, Texaco and Tyler cups, labels 2002 to 2004: When Saturday Comes, issue 215, January 2005. It says the 2005 Setanta Cup is the first competitive all-Ireland football since the Blaxnit Cup was abandoned 25 years ago.

## Disagreements

None.

## No second source (73 lines)

- Munster Senior Cup, all 14 `first` and `second` fields and the `ended` of label 2004. The only list I found is the row's own source (Wikipedia). The extratime and Munster Express pages are behind a bot check or gone. The Cork City and RTÉ reports that Wikipedia cites give 404, and the Wayback Machine did not answer.
- LFA President's Cup labels 2002 and 2003, `first`, `second` and the `ended` of label 2003. RSSSF names the competition but lists no results.
- Premier Division and First Division label 2002 (2001/02), `ended`. The Wikipedia season pages give no end date, and Transfermarkt has no 2001/02 season.
- Blaxnit Cup, Inter-City Cup labels 2005 to 2009, North-South Cup labels 2005 to 2007, Texaco Cup and Tyler Cup labels 2005 to 2008 (21 lines). The WSC article is from January 2005, so it does not cover later seasons.
- Dublin City Cup, First Division Shield, League of Ireland Shield and Top Four Cup, all labels (30 lines). I found only RSSSF and Wikipedia, and both are already the row's sources. Both say that these cups ended decades before 2002.

## Points of note

1. Leinster Senior Cup: Trinity News says the cup came back in 2009, but Wikipedia gives 2010. Both years are after the last label of the range (2007, the 2006/07 season), so the rows are not affected.
2. Transfermarkt numbers the seasons by start year. Its `saison_id` 2001 is the July 2002 to January 2003 transition season, and it has no 2001/02 league season. I used it only for dates and for the League Cup.
3. Premier Division 2006: Wikipedia marks Shelbourne "(C, R)", champions and then demoted. This matches the row note.
4. For `ended` of the 2004 to 2009 leagues, Wikipedia and Transfermarkt sometimes give a different last day (First Division 2006: 18 November against 19 November, First Division 2009: 7 November against 10 November). They agree on the month.
5. The research rows for the 2002/03 and 2003 seasons (two rows for label 2003) agree with the second sources on both editions.
