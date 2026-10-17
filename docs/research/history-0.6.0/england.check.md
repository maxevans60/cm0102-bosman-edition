# England: check pass

Part `england`. I checked every filled field in `england.tsv` on 2026-10-17. All pages were read as raw HTML with curl. No source is Wikipedia.

## Counts

| Result | Lines |
|---|---|
| agree | 210 |
| disagree | 0 |
| no second source | 0 |

The 210 lines are 204 `first` and `second` fields of the 102 `yes` rows, and 6 `held` fields of the `not held` rows. No row uses `third` or `host`.

## Second sources

- RSSSF: Charity Shield, FA Cup and League Cup history lists, and the season pages `eng02` to `eng09` for the top four tiers.
- Football Club History Database: FA Trophy and Football League Trophy finals, and the Conference, Isthmian, Southern and Northern Premier League tables. The Conference pages from 2004/05 have the Northern and Southern Divisions.
- Football Conference official history page: the Bob Lord Challenge Trophy (the Conference League Cup) to 2000/01, the Conference Cup 2004/05 and the Setanta Shield 2007/08 and 2008/09.

## Disagreements

None.

## Points of note

1. Conference Cup label convention. The governing body page confirms the season-start convention of the 3.9.68 rows. The real finals are 1995/96 Bromsgrove Rovers v Macclesfield Town, 1996/97 Kidderminster Harriers v Macclesfield Town, 1997/98 Morecambe v Woking, 1998/99 Doncaster Rovers v Farnborough Town and 1999/2000 Doncaster Rovers v Kingstonian. The 3.9.68 rows hold these at labels 1995 to 1999. So the 2004 Woking, 2007 Aldershot Town and 2008 AFC Telford United rows agree with that convention.
2. A small error in `england.md`. It says the last 3.9.68 label before 2002 is 2000. The 3.9.68 rows in `db-rows.tsv` end at 1999. The 2000/01 final (Chester City v Kingstonian) is not in the 3.9.68 rows. The value 2000 comes from `targets.tsv`. This does not change any result.
3. Conference Cup label 2009 (2009/10) `not held`. The only second source I found is an AFC Telford United match programme feature. It says the Setanta Shield was never contested after 2008/09. This is a weak source, but it agrees.
4. Isthmian Premier label 2005 `not held`. RSSSF `eng05` gives Conference South at tier 6 and the Isthmian Premier at tier 7 in 2004/05. These facts agree. The decision to map nothing to the Isthmian Premier is an inference in `england.md`, and a source cannot check it.
5. Other label conventions agree with the 3.9.68 rows. RSSSF gives the Charity Shield 2000 Chelsea v Manchester United and 2001 Liverpool v Manchester United, which are the August matches of those years. The season-end rows (Premier Division 2002 Arsenal, FA Trophy 2002 Yeovil Town, Vans Trophy 2002 Blackpool, Conference 2002 Boston United) agree with RSSSF and the Football Club History Database.
