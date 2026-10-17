# Nation competitions: check pass

Part `nations`. I checked every filled field in `nations.tsv` on 2026-10-17. I read all pages as raw HTML with curl. The first source is the English Wikipedia. Every second source is RSSSF.

## Counts

| Result | Lines |
|---|---|
| agree | 160 |
| disagree | 0 |
| no second source | 0 |

The 160 lines are the `first`, `second`, `third`, `host` and `ended` fields of the 22 `yes` rows (110 lines), and the `held` field of the 50 `not held` rows.

## Second sources

- RSSSF overview pages of each competition: the list of editions (for the `not held` rows), the finals, and the host country of the African Cup of Nations, the Copa America, the European Championship and the World Cup.
- RSSSF edition pages: the third place match, the final with its date, and the venues.

## Disagreements

None.

## Points of note

1. RSSSF writes `China` for the Asian Cup 2004. The row has `China PR`, the 3.9.68 spelling. This is the same side.
2. Two hosts or more. RSSSF names South Korea first for the World Cup 2002, Austria first for Euro 2008 and Indonesia first for the Asian Cup 2007. The rows agree with the first-named host rule in `nations.md`.
3. Confederations Cup hosts. The RSSSF pages give the venues only. All venues are in France (2003), Germany (2005) and South Africa (2009). So the hosts agree.
4. Oceania Nations Cup 2004 and 2008, third `none`. RSSSF shows no third place match in either edition. But the RSSSF final placings put New Zealand third in 2004 and Fiji third in 2008. The 3.9.68 row for 1996 fills third without a third place match (see `nations.md`). So `none` agrees with the facts, but the data convention can also give New Zealand and Fiji. The research agent must decide this.
5. Oceania Nations Cup 2007 `not held`. The RSSSF overview lists no 2007 edition. The 2008 edition starts on 17 October 2007. The role of the 2007 South Pacific Games as a first round is not on the RSSSF pages that I read.
