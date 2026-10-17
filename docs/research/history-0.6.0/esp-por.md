# History research: Spain and Portugal (part esp-por)

Date 2026-10-17. Rows: 101 in `esp-por.tsv` (53 Portugal, 48 Spain). Of these, 92 are `yes` and 9 are `not held`. No row is `unknown`.

Sources: RSSSF season pages (`tablesp/portYY.html`, `tabless/spanYY.html`), the RSSSF lists of champions, cup finals and super cup finals, Spanish Wikipedia season pages of the Segunda División B, and English Wikipedia pages for the Portuguese third and fourth tiers. All pages were read raw with curl on 2026-10-17.

## Label conventions, with the 3.9.68 rows that prove them

### Leagues and cups: the year the season ends

- Portuguese Premier League 2002 = Sporting / Boavista. That is the 2001/02 table (RSSSF port02).
- Spanish First Division 2002 = Valencia / Deportivo. That is the 2001/02 table (RSSSF span02).
- Portuguese Cup 2002 = Sporting / Leixões. The final was on 12 May 2002.
- Spanish Cup 2002 = Deportivo / Real Madrid. The final was in March 2002.
- Portuguese Second League 2002 = Moreirense / Académica, the 2001/02 table.
- Spanish Second Division 2002 = Atlético de Madrid / Racing de Santander, the 2001/02 table.

### Super cups: the calendar year of the August match

- Spanish Super Cup 2000 = Deportivo / Espanyol (August 2000). 2001 = Real Madrid / Zaragoza (19 and 22 August 2001).
- Portuguese Super Cup 1998 = Porto / Braga (August and September 1998). 2001 = Porto / Boavista (4 August 2001).
- Portuguese Super Cup 2000 = Sporting / Porto. This edition began in August 2000 and ended with a replay in May 2001. The label is the year of the first match.
- RSSSF labels each Portuguese edition by the season before it (for example "2001/02" for August 2002). The data label is the year of the match, so label 2002 = the RSSSF "2001/02" edition.

### Spanish Second Division B1 to B4: Grupo I to IV

The data's B1 to B4 are the official Grupo I to IV, and the label is the season end year. The 3.9.68 rows of 2002 match the 2001/02 groups exactly:

- B1 = Barakaldo / Cultural Leonesa (Grupo I).
- B2 = Barcelona B / Espanyol B (Grupo II).
- B3 = Real Madrid B / Valencia B (Grupo III).
- B4 = Motril / Ceuta (Grupo IV).

First and second are the top two of the regular-season group table. They are not the results of the promotion play-off. RSSSF and Spanish Wikipedia give the same group numbers and the same top two for all four seasons.

### Portuguese Second Division B North, Central, South: Zona Norte, Centro, Sul

The label is the season end year. The 3.9.68 rows of 2000 (Marco / Famalicão, Ovarense / Vilafranquense, Nacional / Portimonense) match the 1999/00 zone tables on RSSSF. The rows of 2001 (Moreirense / Famalicão, UD Oliveirense / Covilhã, Portimonense / União Madeira) match 2000/01.

### Portuguese Third Division: the national championship final

The data holds one row per year: the winner and the loser of the III Divisão national final. The 3.9.68 rows of 1997 to 2000 match the list of finals on English Wikipedia:

- 1997: Dragões Sandinenses / Lourinhanense.
- 1998: Vilafranquense / Freamunde.
- 1999: Vianense / Lusitânia.
- 2000: Paredes / Seixal.

The 2000/01 season had no final, and the data has no 2001 row. This agrees with the convention.

## The `ended` column

`ended` is the month of the final or of the last league round. It comes from the RSSSF round dates and final dates, and from the RSSSF cup and super cup lists. For the Spanish Second Division B groups, `ended` is the end of the regular phase (May), because first and second come from the regular table. The play-offs continued into June. The Spanish Wikipedia pages give 19 May 2002, 18 May 2003 and 29 May 2005, and RSSSF gives round 38 on 14 to 16 May 2004.

These values are inferences, and the `notes` field says so:

- Portuguese Second Division B, all zones, labels 2002 to 2005: `YYYY-05`. No page that I read gives the date of the last zone round.
- Portuguese Second League 2003: `2003-06`. The top tier ended on 1 June 2003. In 2001/02, 2003/04 and 2004/05 both tiers ended on the same day.
- Portuguese Third Division 2002: `2002-06`. I found no date for the final. The series ended in May 2002.

All these editions ended before the earliest start (October 2003) of a database that needs them, so a one-month error has no effect on which rows the game adds.

## Points of doubt

1. **Portuguese Second Division B South 2002.** In the real table, Sporting B (a reserve side) was second on 72 points and Camacha was third on 69. The 3.9.68 row shows Camacha. I wrote Sporting Clube de Portugal B. Maybe the original data skips reserve sides. I don't know. The Spanish B-group rows of 3.9.68 do show reserve sides as runners-up (for example Real Madrid B in 1998 and Sevilla B in 1999).
2. **Portuguese Second Division B, labels 2006 to 2009.** From 2005/06 the third tier was played in four series, A to D, with a promotion play-off. The series mix regions (Madeira clubs in A and B, Azores clubs in B and D), so the three zones have no one-to-one map. I marked North, Central and South `not held` for these years and wrote the series winners and the overall champion in `notes`. Série D is the southern series, so it could stand in for South, but I did not use it. That map is an inference.
3. **Portuguese Third Division 2002.** The only source I found for the 2001/02 final (Mafra 1-0 Dragões Sandinenses) is English Wikipedia. The RSSSF 2001/02 page does not have series B to F or a final. From 2002/03 there was no national final, so labels 2003 to 2005 are `not held`.
4. **Ties on points.** Some top-two places were decided on head-to-head. The `notes` field records each case: Spanish Second Division 2004, 2005 and 2009, Spanish First Division 2007, B4 2002, B4 2005, and Portuguese North 2002.
5. **Third Division 1996 (outside the range).** The 3.9.68 row has Juventude as the losing finalist. The Wikipedia list has Alcanenense (Fafe won 1-0).

## Names I could not match

- Lorca Deportiva CF (B3 2004, second place). This club was founded in 2002. The data has only `Lorca C.F.`, and I did not match the two. The row has `Lorca Deportiva` and `not in data`.

The data has two names for Estoril. I used `Grupo Desportivo Estoril-Praia`, the 3.9.68 spelling, and not `Estoril Praia Futebol` from the later databases. For Almería I used `U.D. Almería` (3.9.68), and not `U.D. Almería S.A.D.` or `Union Deportiva Almería`.

## Database rows that disagree with the real results

### Wrong club or wrong result

| Database | Competition | Label | Database holds | Real |
|---|---|---|---|---|
| 3.9.68 | Portuguese Second Division B South | 2002 | União Madeira / Camacha | União Madeira / Sporting B (see doubt 1) |
| September 2005 | Portuguese Cup | 2005 | Esporte Clube Vitória (a Brazilian club) / Benfica | Vitória Futebol Clube (Setúbal) / Benfica |
| December 2008 | Portuguese Cup | 2008 | Sporting / Belenenses | Sporting / Porto |
| December 2008 | Spanish First Division | 2008 | Real Madrid / Barcelona | Real Madrid / Villarreal |
| April 2010 SIM | Spanish Cup | 2009 | Valencia / Getafe | Barcelona / Athletic Bilbao |
| February 2010 ODB, June 2010 | Spanish Second Division | 2004 | Levante / Numancia | Levante / Getafe (Getafe second on head-to-head) |
| February 2010 ODB, June 2010 | Spanish Second Division | 2009 | Xerez / Tenerife | Xerez / Zaragoza (Zaragoza second on head-to-head) |

### Results from an earlier season under a later label

These rows repeat a row from before the start, with the label moved forward.

| Database | Competition | Label | Database holds (real season) | Real for the label |
|---|---|---|---|---|
| October 2003 | Portuguese Premier League | 2003 | Sporting / Boavista (2001/02) | Porto / Benfica |
| October 2003 | Portuguese Cup | 2003 | Sporting / Leixões (2001/02) | Porto / União de Leiria |
| October 2003 | Portuguese Second League | 2003 | Moreirense / Académica (2001/02) | Rio Ave / Alverca |
| October 2003 | Portuguese Second Division B North | 2003 | Marco / Leixões (2001/02) | Leixões / Lousada |
| October 2003 | Portuguese Second Division B Central | 2003 | Covilhã / Pombal (2001/02) | Feirense / Estrela Portalegre |
| October 2003 | Portuguese Second Division B South | 2003 | União Madeira / Camacha (3.9.68 2002 row) | Estoril / Mafra |
| October 2003 | Spanish Second Division | 2003 | Atlético de Madrid / Racing (2001/02) | Murcia / Zaragoza |
| October 2003 | Spanish Second Division B1 | 2003 | Barakaldo / Cultural Leonesa (2001/02) | Universidad de Las Palmas / Zamora |
| October 2003 | Spanish Second Division B2 | 2003 | Barcelona B / Espanyol B (2001/02) | Real Unión / Barakaldo |
| October 2003 | Spanish Second Division B3 | 2003 | Real Madrid B / Valencia B (2001/02) | Castellón / Barcelona B |
| October 2003 | Spanish Second Division B4 | 2003 | Motril / Ceuta (2001/02) | Algeciras / Málaga B |
| September 2005 | Portuguese Second League | 2005 | Moreirense / Académica (2001/02) | Paços de Ferreira / Naval |
| September 2005 | Portuguese Second Division B North | 2005 | Marco / Leixões (2001/02) | Vizela / Dragões Sandinenses |
| September 2005 | Portuguese Second Division B Central | 2005 | Covilhã / Pombal (2001/02) | Covilhã / Mafra |
| September 2005 | Portuguese Second Division B South | 2005 | União Madeira / Camacha (3.9.68 2002 row) | Barreirense / Pinhalnovense |
| September 2005 | Spanish Second Division | 2005 | Levante / Alavés (2003/04 champion, 2004/05 third) | Cádiz / Celta |
| September 2005 | Spanish Second Division B1 | 2005 | Pontevedra / Ferrol (2003/04) | Real Madrid B / Universidad de Las Palmas |
| September 2005 | Spanish Second Division B2 | 2005 | Atlético B / Real Madrid B (2003/04) | Ponferradina / Real Unión |
| September 2005 | Spanish Second Division B3 | 2005 | Lleida / Gimnástica Torrelavega (2003/04 winner, wrong second) | Alicante / Hércules |
| September 2005 | Spanish Second Division B4 | 2005 | Lanzarote / Pájara Playas de Jandía (2003/04) | Sevilla B / Conquense |
| September 2008 | Spanish Second Division | 2008 | Valladolid / Almería (2006/07) | Numancia / Málaga |
| November 2007, February 2008 | Portuguese Second League | 2007 | Santa Clara / Varzim (2000/01) | Leixões / Vitória de Guimarães |
| September 2008, December 2008, March 2009 | Portuguese Second League | 2008 | Santa Clara / Varzim (2000/01) | Trofense / Rio Ave |
| November 2007, February 2008, September 2008, December 2008, March 2009 | Portuguese Second Division B North | 2007, 2008 | Moreirense / Famalicão (2000/01) | not held (four series) |
| Same five databases | Portuguese Second Division B Central | 2007, 2008 | UD Oliveirense / Covilhã (2000/01) | not held (four series) |
| Same five, plus September 2009, April 2010 SIM, February 2010 ODB, June 2010 | Portuguese Second Division B South | 2007 to 2009 | Portimonense / União Madeira (2000/01) | not held (four series) |

### Name differences only (same club, same result)

- February 2008, Spanish Second Division 2007: `Union Deportiva Almería`. February 2010 ODB, June 2010 and September 2008: `U.D. Almería S.A.D.`. The 3.9.68 name is `U.D. Almería`.

All other database rows in the range agree with the results.
