# Check pass: nordic-2 (Norway, Denmark, Finland)

The check covers `nordic-2.tsv` (132 rows). The line table is `nordic-2.check.tsv`. All pages were read on 2026-10-17 with `curl` and parsed from the raw HTML.

## Counts

342 lines: `first` and `second` of the 116 `yes` rows, `ended` of the 94 `yes` rows with a month, and `held` of the 16 `not held` rows. The 22 rows with `ended` = `unknown` have no line for `ended`.

- agree: 302
- disagree: 0
- no second source: 40

## Second sources

Each check page has another publisher than the source of the field.

- Norway (source: Wikipedia): RSSSF Norway, `rsssf.no/<year>/Premier.html`, `First.html`, `Cup.html`, `Second1.html` to `Second4.html`, `index.html` and `summary`.
- Danish Premier Division and Danish Cup (source: Wikipedia): RSSSF `tablesd/den02.html` to `den09.html`.
- Danish First and Second Division (source: Wikipedia): danskfodbold.com, the site of the DBU statisticians, `tabel.php?seasonid=<label>`. The `ended` months of the First Division 2006/07 and 2007/08 come from RSSSF `den07.html` and `den08.html`.
- Finnish Premier Division, First Division and Cup (source: RSSSF): English Wikipedia season pages, the Wikipedia list of Finnish Cup finals, and transfermarkt fixture lists for the `ended` months.
- Finnish Second Division groups (source: Wikipedia): RSSSF `tablesf/fin03.html` to `fin05.html`.

## Disagreements

None.

## No second source (40 lines)

- Norwegian Second Division Groups 1 to 4, `ended`, 2002 to 2005 (16 lines). The source is RSSSF Norway. The Wikipedia season pages have no match dates, and transfermarkt has no data for these groups.
- Finnish Second Division, 2002, `first` and `second` of the four groups (8 lines). RSSSF has no Kakkonen tables for 2002. RSSSF `fin02.html` lists FC Espoo, WJK Varkaus, TPV Tampere and OLS Oulu in the Division One/Two promotion playoffs of October 2002. This fits the four `first` values, but it does not prove the group places.
- Finnish Second Division, `ended`, 2003 to 2005 (12 lines). The source is RSSSF. Wikipedia has no dates, and transfermarkt has no data for these seasons.
- Finnish Premier Division 2002, `ended` (1 line). The transfermarkt list stops at 2 September 2002, and the Wikipedia page has no dates.
- Finnish First Division 2002, 2003 and 2005, `ended` (3 lines). Transfermarkt has no matches for these seasons, and the Wikipedia pages have no dates.

## Notes on lines that agree

- Norwegian Second Division Group 2, 2005: RSSSF Norway names the winner Oslo Øst. Its 2005 summary says that Oslo Øst will play as Manglerud Star Toppfotball in 2006. This supports `FK Oslo Øst` for 2005 (doubt 2 of `nordic-2.md`).
- Norwegian Second Division Groups 5 to 8: RSSSF Norway has pages for Second1 to Second4 only from 2002, and its summaries give "SECOND DIVISION (4x14)".
- Danish Second Division 2004/05: danskfodbold.com gives Kolding FC as the winner. The match to `Kolding Idrætsforening FC` is still the researcher's inference (doubt 6).
- Danish reserve sides: danskfodbold.com writes Esbjerg fB [2], Brøndby IF [2], Ikast FS [FCM 2] and OB [2]. These agree with the `not in data` names of the research.
- Finnish First Division 2002: Wikipedia shows no single Ykkönen table. In the promotion/relegation group the Ykkönen sides place TPS 2nd and Jokerit 4th. This agrees with the researcher's inference (doubt 3).
- `ended` from transfermarkt is the last match that it lists. For 2004 (Premier and First Division) that is 2 October 2004. The month agrees, but the list can be incomplete for older seasons.
