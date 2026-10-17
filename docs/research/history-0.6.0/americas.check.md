# Check pass: americas (Argentina, Mexico, United States)

Part `americas`, slice 11c. I checked `americas.tsv` (63 rows) field by field. The lines are in `americas.check.tsv`. Every row has `held = yes`, so there are no `held` lines. The record uses only `first` and `second` for these club competitions, so there are no `third` or `host` lines. I also checked `ended` on every row.

## Counts

189 lines (63 rows, three fields each):

- agree: 175
- disagree: 0
- no second source: 14

## Disagreements

None.

## Lines with no second source

- Mexican Second Division 2002 and 2003: `first`, `second` and `ended`. RSSSF mex03 and mex04 have no Segunda División section. The only page with these results is es.wikipedia, which is the row's own source.
- PrepreLibertadores Cup 2002: `ended`. The Wikipedia page gives no match dates.
- American Major League 2004: `ended`. RSSSF usa04 gives the MLS Cup result without a date.
- American A-League 2002 to 2007: `ended`. The RSSSF season pages give the play-off result without a date.

## Points for the research agent

- Mexican Second Division 2002, `ended` 2003-06: RSSSF mex03 shows that Tepic, the losing finalist in this row, played the Primera A promotion and relegation play-off from 28 May 2003. If Tepic qualified for that play-off through the Segunda final, the final ended in May 2003 or earlier. This is an inference. I did not mark it as a disagreement.
- PrepreLibertadores Cup: the Wikipedia page "Pre-Libertadores tournament" lists a Mexican preliminary round, the "Selectivo Pre Pre-Libertadores". Its two qualifiers for 1998 to 2002 are the same pairs as the 3.9.68 rows for 1998 to 2001 and the research row for 2002. The data name probably refers to this round. The 2002 clubs are the same in both rounds, but the `ended` month may be different for the preliminary round. The page also says that the tournament was held from 1998 to 2002. This supports the note that label 2003 (InterLiga 2004) is a mapping and not the same competition.
- 11 source URLs in `americas.tsv` return HTTP 404 today: `rsssf.org/tablesa/arg2003.html` to `arg2009.html` (the `ended` source of the Argentine Premier Division rows), and `rsssf.org/tablesm/mex2003.html` to `mex2006.html` (the `ended` source of the Mexican First Division rows). The RSSSF pages that exist are named `arg03.html` and `mex03.html`. The values are correct (they agree with other pages), but the sources must be fixed.

## Label conventions against the 3.9.68 rows

- Argentine Premier Division, label Y = Apertura Y and Clausura Y+1: the rule holds. en.wikipedia season tables: 1996–97 Apertura River Plate, Independiente and Clausura River Plate, Colón (3.9.68 label 1996). 2000–01 Boca Juniors, River Plate and San Lorenzo, River Plate (label 2000). 2001–02 Racing, River Plate and River Plate, Gimnasia (LP) (label 2001).
- Argentine Second Division, label Y = season Y/Y+1: the rule holds. RSSSF arg2champ gives the champions of 1995/96 to 2001/02 as Huracán Corrientes, Argentinos Juniors, Talleres, Instituto, Huracán, Banfield and Olimpo. These are the 3.9.68 `first` clubs for labels 1995 to 2001.
- Mexican First Division, label Y = Invierno or Apertura Y and Verano or Clausura Y+1: the rule holds. RSSSF mexchamp: 1996–97 Santos Laguna v Necaxa and Guadalajara v Toros Neza (label 1996). 1997–98 Cruz Azul v León and Toluca v Necaxa (label 1997). 2001–02 Pachuca v Tigres and América v Necaxa (label 2001).
- Mexican First Division A, label Y = season Y/Y+1: the rule holds for the winners. RSSSF mex2champ: 1996–97 Tigres v Atlético Hidalgo (label 1996), 1997–98 Pachuca promoted (label 1997), 1998–99 Unión de Curtidores promoted (label 1998), 2001–02 Real San Luis promoted (label 2001). The 3.9.68 rows for 1999 and 2000 name other clubs, as the research `.md` says.
- Mexican Second Division: es.wikipedia gives 2001–02 Cihuatlán 2-0 Astros de Ciudad Juárez, the 3.9.68 label 2001 row. I found no other publisher for this level before 2004/05.
- PrepreLibertadores Cup, label Y = played in Y: the rule holds. Wikipedia "Pre-Libertadores tournament": 1998 Monterrey, Necaxa. 1999 Atlas, América. 2000 Cruz Azul, Atlante. 2001 América, Morelia. These are the 3.9.68 rows.

## Method

I read the raw pages with curl and the Wikipedia `action=raw` wikitext, then searched them. Wikipedia pages checked RSSSF sources, and RSSSF pages checked Wikipedia sources. For `ended`, if the row's source was RSSSF, I used en.wikipedia or es.wikipedia (season and tournament pages, InterLiga 2004, Final de Ascenso 2002-03 and 2003-04). If the row's source was Wikipedia, I used RSSSF (arg2-03 to arg2-2010, arg2010, usa02 to usa09, usacuphist). worldfootball.net blocked the requests with a bot check.
