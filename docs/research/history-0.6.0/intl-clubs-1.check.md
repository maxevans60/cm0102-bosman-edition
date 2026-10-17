# Check pass: intl-clubs-1

Part `intl-clubs-1`, slice 11c. The first source of `intl-clubs-1.tsv` is the English Wikipedia. This check uses RSSSF as the second publisher. I read every page raw with curl on 2026-10-17 and took the values from the final lines, the palmares lists and the match dates.

`intl-clubs-1.check.tsv` has 378 lines. It covers each filled field of the 108 `yes` rows (`first`, `second` and `ended`) and the `held` field of the 54 `not held` rows.

## Counts

| Result | Lines |
|---|---|
| agree | 377 |
| disagree | 1 |
| no second source | 0 |

## Disagreements

1. CONCACAF Champions Cup, label 2009 (2008/09), `ended`. The file holds `2009-04`. RSSSF (`tablesc/cacups09.html`) gives the final legs as 22 April and 12 May 2009. The first source page of the edition (en.wikipedia, 2008-09 CONCACAF Champions League) also says that CONCACAF moved the second leg from 29 April to 12 May 2009, because of the swine flu outbreak in Mexico. The real value is `2009-05`. Winner and runner-up (Atlante, Cruz Azul) agree.

## Notes

1. Recopa Sudamericana: RSSSF labels each edition by the year of the qualifying cups, one year before the year of play. Thus RSSSF "2002 Olimpia 2-0 San Lorenzo (12/7/03)" is label 2003 in the file. All seven editions agree when you allow for this shift. RSSSF shows "1998-2001 not played" in its labels, so no Recopa took place in calendar 2002, and label 2002 `not held` agrees.
2. CONCACAF Champions Cup: RSSSF labels the editions after 2002 as seasons (2002/03 to 2008/09). The file labels them by the end year. Results and months agree for 2002 to 2008.
3. South American Copa Mercosur: the check confirms only the Copa Sudamericana results. The mapping of the Copa Sudamericana to the Copa Mercosur rows is the first agent's inference. RSSSF says that the Copa Mercosur and the Copa Merconorte were abolished after 2001, and that the Copa Pan-Americana of 2002 was planned in their place but cancelled.
4. Inter-Toto Cup: the check confirms each pair of the final round (winner and loser) and the months. RSSSF lists the ties in another order, so the `Winner 1` to `Winner 11` numbers of the file come from the first source only. RSSSF also confirms that 2008 was the final edition.
5. Results decided on penalties, which RSSSF confirms: Intercontinental Cup 2003 (Boca Juniors 1-1 Milan, 3-1 on penalties) and 2004 (Porto 0-0 Once Caldas, 8-7), Copa Libertadores 2002 (Olimpia, 4-2) and 2004 (Once Caldas, 2-0), Copa Sudamericana 2005 (Boca Juniors, 4-3), Recopa 2004 (Cienciano, 4-2), CONCACAF 2007 (Pachuca, 7-6), Champions Cup 2003 (Milan) and 2005 (Liverpool), UEFA Cup 2007 (Sevilla).
6. Copa Sudamericana 2007: RSSSF shows 4-4 on aggregate (América 2-3 Arsenal, Arsenal 1-2 América). Arsenal de Sarandí won on away goals. The away-goals reason is an inference from the scores.
