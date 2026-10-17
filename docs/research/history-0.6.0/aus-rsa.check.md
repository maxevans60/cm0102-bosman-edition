# Check pass: aus-rsa

Date 2026-10-17. I checked every filled field of `aus-rsa.tsv` (first, second, ended, and held for `not held` rows) against a page of another publisher than the row's source. I read the raw pages with curl and grep. An `ended` value of `unknown` is not a fact, so I did not check it (3 rows).

## Counts

| Result | Fields |
|---|---|
| agree | 138 |
| disagree | 0 |
| no second source | 30 |
| total | 168 |

## Second sources used

- Australian rows (sources Wikipedia and socceraust): RSSSF season pages `tablesa/ausYY.html` and the champions list `tablesa/auschamp.html`. RSSSF names a summer state season by its end year, so its "New South Wales 2003" Premier League is the 2002/03 season.
- NSW Premier League `ended` (source Wikipedia): archived socceraust fixtures pages, which give the date of each grand final.
- South African rows (source RSSSF): English Wikipedia season, cup and final pages.

## Disagreements

None.

## No second source (30 fields)

- `ended` of NSW State League (8), Queensland (4), South Australia (3) and Western Australia (4). The first source is socceraust. RSSSF gives the results but no dates.
- South African Charity Cup: `second` for 2002 and 2003, and `ended` for 2003. Wikipedia lists only the winners.
- South African Premier Division `ended` (4). The Wikipedia season pages give no dates.
- South African League Cup 2002 `ended`. No date on the Wikipedia page.
- `held` of South African First Division 2002 and 2003, and of South African Superbowl 2002. The row already cites both RSSSF and Wikipedia, and I found no third publisher.

## Points to note

- The South African rows cite `rsssf.org/tablesz/zaf2002.html` to `zaf2005.html`. On 2026-10-17 these URLs return 404. The RSSSF season pages are `zaf02.html` to `zaf05.html`, and the facts on them match the rows. The sources column needs a fix.
- Two dates in the notes are one day off from Wikipedia, but the month is the same, so `ended` agrees. League Cup 2003: the notes say 6 December, Wikipedia says 7 December. ABSA Cup 2004: the notes say 13 June, Wikipedia says 12 June.
- The Wikipedia page of the SAA Supa 8 2003 gives the final as "2 October 2004" in its match box. The same page gives the date range "9 August to 21 September 2003". I took 21 September 2003, which agrees with `2003-09`.
- Name matches are the research agent's decisions (for example Queensland Lions as Brisbane Lions, Wollongong FC as Wollongong Wolves, Spirit FC as GHFA Spirit FC). I checked that the real club is the same. I did not check the choice of database name.
