# Scotland, Wales, Northern Ireland: check pass

Part `scot-wal-nir`. I checked every filled field in `scot-wal-nir.tsv` on 2026-10-17. All pages were read raw with curl. Each check page comes from another publisher than the row's source.

## Counts

| Result | Lines |
|---|---|
| agree | 349 |
| disagree | 0 |
| no second source | 36 |

The 385 lines are:

- 254 `first` and `second` fields of the 127 `yes` rows. All agree.
- 9 `held` fields of the `not held` rows. All agree.
- 122 `ended` fields. 86 agree and 36 have no second source.

I did not check the 5 `ended` fields that hold `unknown` (Northern Irish League First Division 2003, Scottish Second Division 2002 and 2003, Scottish Third Division 2002 and 2003). No row uses `third` or `host`.

## Second sources

- Rows from RSSSF, checked on Wikipedia: the Scottish and Welsh league season pages, the Irish League, Irish Premier League, Irish First Division, IFA Premiership and IFA Championship season pages, the lists of Scottish Cup, Scottish League Cup and Welsh Cup finals, the Welsh League Cup page, the single Scottish final pages (dates), the Welsh Cup and Welsh League Cup season pages from 2004/05 (dates) and the FAW Premier Cup page (2008/09 not held).
- Rows from Wikipedia, checked on RSSSF: the Northern Irish season pages `nilYY` (Irish Cup, League Cup and their dates), the County Antrim Shield list `nilantrimshieldhist`, the Gold Cup list `nilgoldcuphist`, the Scottish season pages (Challenge Cup, with `scot2010` for the 2009/10 final) and the Welsh season pages `walYY` (FAW Premier Cup finals).
- BBC Sport match reports for the Welsh Cup finals of 2003 and 2004 (dates).

## Disagreements

None.

## No second source (36 `ended` fields)

- County Antrim Shield 2003, 2005, 2006, 2008, 2009. The RSSSF list gives no dates. The RSSSF season pages give no date for 2004/05 and 2005/06, and do not show the 2002/03, 2007/08 and 2008/09 finals.
- Northern Irish League Premier Division 2002 to 2006, and First Division 2002 and 2004 to 2009. The Wikipedia season pages give no last match date. worldfootball.net refused the request.
- Scottish First Division 2002 to 2006, 2008, 2009. Scottish Second Division 2004 to 2009. Scottish Third Division 2004, 2005. The Wikipedia season pages give no last league match date, or only play-off dates.
- Welsh Cup 2002. Welsh League Cup 2002, 2003, 2004. No Wikipedia season page, and I found no other page.

## Points of note

1. Inference only, not counted as agree: the Wikipedia Third Division pages give the last Scottish Football League match day as 29 April 2006, 28 April 2007, 26 April 2008 and 9 May 2009. The four divisions usually share a last day. If they did, the First and Second Division `ended` values for 2006, 2008 and 2009 also agree.
2. Club names on the check pages differ in form only: Rangers and Celtic for Glasgow Rangers and Glasgow Celtic, St Mirren for St. Mirren, Llanelli for Llanelli AFC. Wikipedia gives The New Saints for the Welsh League Cup winner of 2005/06, and RSSSF gives Newry City for the League Cup runner-up of 2008/09. They are the same clubs as the data names in the rows.
3. The Scottish Premier League 2004/05 infobox gives the last date as 21 May 2005. The row holds only the month, and the month agrees.
4. Gold Cup: RSSSF ends its list at 2000/01 and says the cup was apparently not held after 2001. That agrees with all eight `not held` rows.
