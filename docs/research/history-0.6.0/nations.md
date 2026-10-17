# Nation competitions: history 2002 to 2009

Part `nations` of slice 11c. The file `nations.tsv` holds 72 rows: 9 competitions, label years 2002 to 2009. 22 rows are `held = yes`, 50 rows are `not held`, and no row is `unknown`. The sources are the English Wikipedia articles of each edition, read raw on 2026-10-17. The check pass must use a second publisher.

## Label convention

All nine competitions use the year of the tournament. The 3.9.68 rows prove it:

- FIFA World Cup 1998: France, Brazil, Croatia, host France.
- Copa America 2001: Colombia, Mexico, Honduras, host Colombia.
- Confederations Cup 2001: France, Japan, Australia, host Japan.
- African Cup of Nations 2002: Cameroon, Senegal, Nigeria, host Mali. The final was in February 2002.

The 2008 OFC Nations Cup ran from October 2007 to November 2008. It has the label 2008 from its official name. Its `real_season` is `2007-08`. The 2007 South Pacific Games served as its first round, so 2007 is `not held`.

## Third place

- The data fills third with the winner of the third place match, or the bronze medal match at the Olympic Games and the Asian Games.
- The European Championship has no third place match since 1984. The 3.9.68 rows for 1996 and 2000 leave third empty. Euro 2004 and Euro 2008 have `none`.
- The 2004 and 2008 OFC Nations Cups had no third place match. Both rows have `none`. The tables put New Zealand (2004) and Fiji (2008) third. The 3.9.68 row for OFC 1996 has New Zealand as third, and that edition had no third place match. So the data can also fill third from a losing semi-finalist. This is an inference.

## Hosts

- The 3.9.68 row for Euro 2000 has Belgium, the first-named host. I follow that rule for the World Cup 2002 (South Korea), Euro 2008 (Austria) and the Asian Cup 2007 (Indonesia, the first of four hosts).
- Point of doubt: the 3.9.68 row for the African Cup of Nations 2000 has Nigeria, the second-named host after Ghana. So the data does not use one rule for all competitions.
- The 2008 OFC Nations Cup had no fixed venue. Its host is `none`.
- The 2004 OFC Nations Cup had its final group in Adelaide and a two-legged final in Honiara and Sydney. The host is Australia.

## Names

- China: the 3.9.68, October 2003 and September 2005 data use `China PR`. The later databases use `China`. The rows use `China PR`, the 3.9.68 spelling.
- Ivory Coast, South Korea, United States and Iran match the national sides file.
- All names match the national sides file. No name is `not in data`.
- The Olympic Games and the Asian Games are under-23 tournaments. The data writes the senior side names, as in the 3.9.68 Olympic row for 2000 (Cameroon).

## Database rows that disagree

None inside the label years. The rows of October 2003 and September 2005 for the African Cup of Nations 2002 (Cameroon, Senegal, Nigeria, Mali) agree.

Outside the range, the 3.9.68 rows leave third empty for two years that had a third place match:

- African Cup of Nations 1996: Zambia beat Ghana for third.
- African Cup of Nations 1998: DR Congo beat Burkina Faso for third.

## Ended

The `ended` column holds the month of the final, from the dates in each article's infobox. The 2004 OFC Nations Cup ended with the second leg of the final in October 2004. The 2008 OFC Nations Cup ended with the last round robin match in November 2008.
