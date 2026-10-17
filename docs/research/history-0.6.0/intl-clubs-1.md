# International club competitions: history 2002 to 2009

Part `intl-clubs-1`, 16 club competitions (Europe, South America, CONCACAF and world), label years 2002 to 2009. The file `intl-clubs-1.tsv` has 162 rows: 108 `yes`, 54 `not held`, 0 `unknown`. All pages were read on 2026-10-17 as raw HTML with curl. Every name in `first` and `second` is a full name in a `clubs/<nation>.tsv` file. The `ended` column gives the month of the last match of each edition, from the edition page or the final page.

## Label conventions, with the 3.9.68 rows that prove them

Season-end year (label Y = season Y-1/Y, final in May). This applies to the European Champions Cup and the UEFA Cup.

- European Champions Cup 2001 FC Bayern München, Valencia C.F. = the May 2001 final.
- European Champions Cup 1999 Manchester United, FC Bayern München = the May 1999 final.
- UEFA Cup 2001 Liverpool, C.D. Alavés = the May 2001 final.

Calendar year of the match or the tournament. This applies to the European Super Cup, the Intercontinental Cup, the FIFA Club World Championship, the Copa Libertadores, the Copa Mercosur, the Recopa and the CONCACAF Champions Cup.

- European Super Cup 2000 Galatasaray SK, Real Madrid C.F. = the August 2000 match in Monaco (the start of the 2000/01 season).
- Intercontinental Cup 2001 FC Bayern München, Boca Juniors = the November 2001 match.
- FIFA Club World Championship 2000 Sport Club Corinthians Paulista, Clube de Regatas Vasco da Gama = the January 2000 tournament.
- South American Copa Libertadores 2001 Boca Juniors, Club Deportivo Cruz Azul and 2002 Club Olimpia, Associação Desportiva São Caetano = the 2001 and 2002 editions.
- South American Copa Mercosur 2001 San Lorenzo de Almagro, Clube de Regatas do Flamengo = the 2001 edition.
- South American Recopa 1998 Cruzeiro Esporte Clube, River Plate = the edition named 1998. The label is the edition name.
- CONCACAF Champions Cup 2000 Los Angeles Galaxy, Club Deportivo Olimpia = the 2000 edition.

The CONCACAF Champions League replaced the Champions Cup from 2008/09 and runs over two calendar years. I label it by the year of the final: label 2009 = 2008/09 (final April 2009). The September 2009, February 2010 ODB and June 2010 databases do the same. This gives no clash, because the last Champions Cup (label 2008) ended in April 2008.

The Inter-Toto Cup is in none of the 3.9.68 rows. It is held only in the September 2008, March 2009 and 2009/10 databases, with labels 1995 to 2000. Their last row (label 2000) holds one club in `first` and nothing in `second`. I don't know if these databases hold one row or several rows per label year, because `db-rows.tsv` has no Inter-Toto rows and `gaps.tsv` shows the last row only. I label each edition by its calendar year (the summer of the competition). This is an inference from the first label 1995, the year of the first UEFA edition.

## How the Inter-Toto rows are written

- 2002 to 2005: three finals a year, each winner a joint winner. Three rows per label, `part` = `Winner 1` to `Winner 3`, `first` = the winner, `second` = the losing finalist.
- 2006 to 2008: no final. The 11 third-round winners were joint winners, and the club with the best UEFA Cup run got the trophy. Eleven rows per label, `part` = `Winner 1` to `Winner 11`, with the outright winner as `Winner 1` (2006 Newcastle United, 2007 Hamburger SV, 2008 Sporting Clube de Braga). `second` = the third-round opponent.
- 2009: `not held`. The 2008 edition was the last.

If the game can show only one row per year, use `Winner 1`. For 2002 to 2005, `Winner 1` is the first final in the table on the Wikipedia page, which is not a ranking.

## Competitions that no longer exist

- European Cup Winners Cup: last edition 1998/99. Labels 2002 to 2009 are `not held`.
- CONCACAF Cup Winners Cup: held from 1991 to 1998, and the last three editions were not finished. Labels 2002 to 2005 are `not held`.
- Inter-American Cup: last edition 1998. `not held`.
- South American Super Cup (Supercopa Libertadores): last edition 1997. `not held`.
- South American CONMEBOL Cup: last edition 1999. `not held`.
- Merconorte Cup: discontinued after 2001. `not held`.
- Intercontinental Cup: last edition December 2004. Labels 2005 to 2009 are `not held`. The FIFA Club World Championship replaced it.
- FIFA Club World Championship: the 2001 edition was cancelled. Labels 2002 to 2004 are `not held`. It came back in December 2005 (named FIFA Club World Cup from 2006).
- South American Recopa: not held from 1999 to 2002. Label 2002 is `not held`.

## Points of doubt

1. Copa Mercosur and Copa Merconorte. Both ended after 2001, and the Copa Sudamericana replaced them from 2002. I map the Copa Sudamericana to South American Copa Mercosur, because the September 2005 database holds the 2003 Sudamericana final (ADFP Club Cienciano, River Plate) under that name. Nothing is left for the Merconorte Cup, so it is `not held`. This is an inference. Another view is that the CONMEBOL Cup or the Merconorte Cup is the predecessor.
2. Spelling. I follow the 3.9.68 full names when a club has a different full name in the later databases. Examples: Zenith St. Petersburg (later Zenit St. Petersburg), Larisa (later A.E Larisas), Maccabi Petach Tikva (later Maccabi Petach-Tikva), Ethnikos Achnas (later Ethnikos Achna), Dacia (MOL) (later Dacia Chisinau), Vetra Rudiskes (later Vetra Vilnius), Kispest-Honvéd FC (later Budapest Honvéd FC), Saturn Ramenskoye (later Saturn Moscow Oblast). The game must match these names to the club of each database.
3. UNAM (Pumas) is Club de Fútbol Universidad in 3.9.68 (short name Pumas) and Pumas UNAM in the later databases.
4. FC Riga (2008 Inter-Toto opponent of IF Elfsborg) is matched to FK Riga in the Latvia list. I think it is the same club, but I did not prove it.
5. Several databases put `T` in the `third` and `host` fields of their newest row (February 2010 ODB: European Champions Cup, UEFA Cup, CONCACAF Champions Cup). Club competitions do not use these fields. I ignore them.
6. The label 2009 editions of the FIFA Club World Championship (December 2009) and the Copa Mercosur, mapped to the Copa Sudamericana (December 2009), ended after the September 2009 databases start. The `ended` column shows this.
7. Outside the range: the Inter-Toto label 2000 row is Hamburger SV in the September 2008 and March 2009 databases. The real 2000 winners are Udinese, Celta Vigo and VfB Stuttgart. VfB Stuttgart is in the 2009/10 databases.

## Database rows that disagree with the real result

| Database | Competition | Label | Holds | Real |
|---|---|---|---|---|
| October 2003 | European Super Cup | 2003 | Real Madrid C.F., Feyenoord | AC Milan, Futebol Clube do Porto (August 2003). The row holds the 2002 result. |
| September 2005 | European Super Cup | 2005 | Real Madrid C.F., Feyenoord | Liverpool, CSKA Moscow (August 2005). The row holds the 2002 result. |
| September 2005 | South American Copa Libertadores | 2005 | Corporación Deportiva Once Caldas, Boca Juniors | São Paulo Futebol Clube, Clube Atlético Paranaense (July 2005). The row holds the 2004 result. |
| September 2005 | South American Copa Mercosur | 2005 | ADFP Club Cienciano, River Plate | Copa Sudamericana 2005: Boca Juniors, Club de Fútbol Universidad (December 2005, after the start). The row holds the 2003 result. |
| September 2005 | CONCACAF Champions Cup | 2005 | Liga Deportiva Alajuelense, Deportivo Saprissa | Deportivo Saprissa, Club de Fútbol Universidad (May 2005). The row holds the 2004 result. |
| April 2010 SIM | CONCACAF Champions Cup | 2009 | Club Deportivo Pachuca, Club Deportivo Cruz Azul | Club de Fútbol Atlante, Club Deportivo Cruz Azul (2008/09). The row holds the 2009/10 result (final April 2010). |

All other rows in `db-rows.tsv` inside the range agree. The difference between Zenit and Zenith St. Petersburg is a spelling, not a disagreement.

## Names that I could not match

None. FC Moscow and Bnei Sakhnin are in the later databases only, not in 3.9.68.
