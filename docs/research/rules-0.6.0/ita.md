# Rules table: Italy (part `ita`, slice 5)

Files:

- `ita.tsv`: 432 rows, 16 competitions for each of the 27 seasons from 2001/02 to 2027/28 (459 rows before pass 2, which deleted Serie D).
- `ita-clubs.tsv`: 2,095 club rows for the divisions whose real club count differs from the season database, that are new, or that club moves leave short (1,573 before pass 2).

## Method

1. I read the raw wikitext of the English and Italian Wikipedia season pages with `curl` on 2026-10-16. I did not use a summary model for any number in the table.
2. The league structure (clubs, promotion, relegation, play-offs, play-outs) comes from the season pages and from the main pages "Serie A", "Serie B", "Serie C1" and "Serie C2". For the third tier after 2014 it comes from the "Regolamento" sections of the Italian season pages.
3. Cup formats come from the English and Italian Coppa Italia season pages, the Italian "Coppa Italia Serie C" page, the "Supercoppa Italiana" page and each edition page, and the Italian "Supercoppa Serie C" page.
4. Substitutes, bench size, the non-EU signing quota, squad lists and transfer windows come from the "Serie A" page, the Italian season pages, the "List of Italian football transfers" pages and news pages (Calcio e Finanza, LaPresse, Sky Sport, OA Sport). Each row names its pages in `sources`.
5. A script parsed the final table of each group on the Italian season pages and matched each club to the club records of the season database by name. I checked by hand every match where the two names differ.

## Competitions in the table

| comp | comp_file | Seasons held |
|---|---|---|
| nation rules | n/a | all |
| Serie A | ita_ser_a | all |
| Serie B (with promotion play-offs and play-outs) | ita_ser_b | all |
| Serie C1 group A and B (Lega Pro Prima Divisione from 2008/09) | ita_ser_c1a, ita_ser_c1b | 2001/02 to 2013/14 |
| Serie C2 group A, B and C (Lega Pro Seconda Divisione from 2008/09) | ita_ser_c2a, ita_ser_c2b, ita_ser_c2c | 2001/02 to 2013/14 (group C to 2010/11) |
| Serie C group A, B and C (Lega Pro to 2016/17) | new | 2014/15 on |
| Coppa Italia | ita_cup | all |
| Coppa Italia Serie C (Coppa Italia Lega Pro 2008/09 to 2016/17) | ita_c_cup | all except 2020/21 |
| Supercoppa Italiana | ita_super | all |
| Supercoppa di Serie C | ita_c1_super | all except 2019/20 |
| Supercoppa di Lega di Seconda Divisione | new | 2005/06 to 2013/14 |

## Main changes for the build

- Serie A has 18 clubs to 2003/04 and 20 clubs from 2004/05. Tie-break play-offs end after 2004/05 and return in 2022/23 for 1st and 17th place only.
- Serie B has 20, 24 (2003/04), 22 (2004/05 to 2017/18), 19 (2018/19) and 20 (from 2019/20) clubs. Promotion play-offs start in 2004/05 (3rd to 6th) and grow to 3rd to 8th in 2013/14. Play-outs start in 2003/04.
- The third and fourth tiers merge in 2014/15 into one division of three groups (60 clubs in most seasons, 54 in 2015/16, 57 in 2017/18, 59 in 2018/19). Second teams of Serie A clubs enter from 2018/19.
- Coppa Italia: 48 clubs with a group round to 2004/05, 72 clubs in 2005/06 and 2006/07, 42 clubs in 2007/08, 78 clubs from 2008/09 to 2020/21, 44 clubs from 2021/22. The final is a single match from 2007/08. From 2024/25 a draw before the semi-finals goes straight to penalties.
- Supercoppa Italiana: four clubs from the 2023 edition (played in 2023/24) to 2025/26, then two clubs again. Most editions from 2018/19 are in Saudi Arabia.
- Substitutes: 3, then 5 from the June 2020 restart and from 2020/21. Serie A bench: 7, then 12 from 2012/13, then 15 from 2022/23.

## Season databases and club moves

- Every season database holds Serie A 18, Serie B 20 and five groups of 18 for C1/A, C1/B, C2/A, C2/B and C2/C. No database follows the real structure after 2004/05.
- `ita-clubs.tsv` lists every real club of:
  - Serie A for each database season from 2005/06 (20 against 18);
  - Serie B for 2003/04 (24), 2005/06 to 2017/18 (22) and 2018/19 (19);
  - Lega Pro Seconda Divisione 2010/11 and 2011/12, and Lega Pro Prima Divisione 2012/13 and 2013/14 (group sizes differ);
  - Serie C groups A to C from 2014/15 (a new division).
- In 2011/12 to 2013/14 the database group C2/C has 18 clubs but the real division has no group C. The build must decide where these clubs go.
- From 2014/15 the database has 90 clubs in the five third-tier groups against about 60 real clubs. The club moves put the real 60 into the three new groups. The other 30 database clubs have no real division in that season.
- Some matched clubs sit in the database Serie D. In December 2025 these are Ospitaletto, Dolomiti Bellunesi, Pineto, Livorno, Forlì, Guidonia and Casarano, all real 2025/26 Serie C clubs.
- SPAL matches the database record "Società Polisportiva Ars et Labor". Matera in 2010/11 matches "Materasassi".

## Missing clubs (for Max)

Each of these divisions lacks one club after the club moves. The season database holds no record of the club.

| Season | Division | Club |
|---|---|---|
| 2020/21 | Serie C girone C | Trapani |
| 2025/26 | Serie C girone B | Rimini |

Both clubs were excluded during that season.

## Unknowns

- The exact non-EU signing quota for 2003/04 to 2007/08. The rule changed in August 2004, June 2005, June 2006 and June 2007, but I found no page with the numbers.
- Transfer window dates for 2001/02 to 2005/06, and some opening or closing dates for 2006/07, 2008/09 and 2011/12. The winter 2026/27 window is not published yet.
- The bench size in Serie B from 2022/23, in the third tier before 2016/17, in Serie D and in the super cups.
- The number of entrants in the Coppa Italia Serie C for 2001/02 to 2003/04 and its round format before 2021/22.
- The format of the Supercoppa di Lega di Seconda Divisione.
- Serie D relegation numbers. I did not take Serie D club lists, because the original has no Serie D competition. Max decides if Serie D comes in.

## Inferences (marked in the rows)

- A Serie A bench of 7 before 2012/13 comes from the Italian 2012-13 season page ("12 instead of 7"). The 7 in Serie B before 2012 is inferred from a September 2012 report that Serie B kept 7.
- Serie B went to 12 on the bench from 2017/18. This is inferred from a July 2017 report that the league asked for 12 "from next season".
- Serie B used 5 substitutes from 2020/21. I found no report of the Serie B vote.
- The non-EU quota of two signings a season after 2013/14, and the 25-player list after 2015/16, are assumed to stay the same.
- Rows for 2027/28 copy 2026/27 where no change is announced.
- The play-out positions in Serie C1 and C2 groups (14th v 17th, 15th v 16th) and in Serie C from 2019/20 (16th to 19th).
- Where a source page and a later page disagree, the row gives both values in its text. Examples are the Serie B play-off gap (9 or 14 points) and the play-out gap (4 or 5 points).

## Reconcile

Date 2026-10-16. Each `disagree` and `found` line of `ita.check.tsv` got a third source from a publisher other than the first and second, or the primary record. I read every page today with `curl` or `pdftotext`. The decisions are in `ita.reconcile.tsv`.

| Decision | Count |
|---|---|
| second | 34 |
| first | 8 |
| conflict | 2 |
| accepted (`found`) | 40 |

Primary records read: FIGC C.U. 97/A (2004/05), 232/A (2005/06), 73/A (Seconda Divisione 2010/11), 22/A (2017/18), 40/A (2019/20), 25/A (2026/27), 14/A (July 2002), 194/A (June 2004), 44/A (August 2017), 269/A (April 2025) and 245/A (May 2026). Third publishers: LiveSicilia, L'Unità, Sky Sport, Calcio e Finanza, Goal, Calciomercato.com, fantacalcio.it, 90min, TuttoMercatoWeb, La Sicilia, Canal 26 and Il Piccolo.

Decisions:

- Serie B play-off gap. From 2013/14 to 2016/17 the 9-point rule stays, and only clubs within 14 points of the 3rd take part (2 to 6 clubs). From 2017/18 the 3rd goes up directly only with a lead of more than 14 points, and the play-offs always have 3rd to 8th. The rows from 2013/14 to 2027/28 now say this. The 2027/28 row copies 2026/27 as an inference.
- Serie B play-out gap. The FIGC texts for 2004/05 and 2005/06 say "more than 4 points", so the first file stands. The RSSSF sentences for 2004/05 to 2006/07, 2009/10 and 2010/11 only describe the actual gap of each season. No rule text for 2006/07 was found. Its row keeps "over 4" as an inference.
- Seconda Divisione 2010/11. C.U. 73/A cut relegation to 4 clubs: 2 in group A (17 clubs), 1 in groups B and C (16 clubs). The play-out is 16th v 15th and is not held if the gap is more than 5 points.
- Prima Divisione 2013/14. The play-offs had 2nd to 9th of each group, with one-leg quarter-finals. I found no third publisher. The bracket on the season page agrees with Transfermarkt.
- Serie D club counts use the count at the season start: 162 in 2016/17 (Chieti excluded later) and 172 in 2021/22 (FC Messina excluded later).
- Squad list. The free group is "under 22" in 2020/21 to 2024/25 and "under 23" from 2025/26. The 2021/22 and 2027/28 rows follow.
- Transfer windows: summer 2018 closed on 17 August (first), summer 2023 on 1 September, winter 2024/25 on 3 February (first). Summer 2026 opened on 29 June for Serie A. Winter 2026/27 is 2 January to 1 February 2027.
- Serie C 2019/20 used 5 substitutes in the June and July 2020 play-offs and play-outs. The Coppa Italia Serie C 2019/20 final was one match on 27 June 2020.
- Supercoppa Italiana: the 2023/24 final was on 22 January 2024. The 2026/27 match is Inter v Lazio at San Siro on 22 December 2026, with extra time and penalties.
- All `found` values except one are accepted. They fill the Supercoppa di Lega di Seconda Divisione, the Coppa Italia Serie C rounds, bench sizes from C.U. 44/A, the Serie C squad list of 2020/21 and the Serie D bench of 2021/22.

No change to `ita-clubs.tsv`. No decision changes a club list.

### Conflicts for Max

1. Non-EU rule 2001/02 (`ita-1`, `foreign`). L'Unità of 4 May 2001 and Sky Sport report that the Corte Federale dropped the caps (3 in the match list, 5 registered) in May 2001. But FIGC C.U. 194/A of June 2004 still prints NOIF art. 40.7 as unchanged. That text allows a Serie A club at most 5 non-EU players from foreign federations, with 3 in the match list, and a Serie B club 1. C.U. 14/A of July 2002 also derogates from art. 40. I don't know if the cap on players from foreign federations still applied in 2001/02. The row keeps "no limit" with a note. Max decides between "no limit" and "5 registered and 3 in the match list for players from foreign federations".
2. Substitutes in Lega Pro from 2017/18 (`ita-288`, `subs_used`, and the Serie C rows of 2017/18 to 2019/20). FIGC C.U. 44/A (4 August 2017) gives 3 substitutes in all professional leagues. Calcio e Finanza (22 August 2017) and Il Piccolo (21 August 2017) report a Lega Pro trial of 5 substitutes from 2017/18, in the league, the play-offs, the play-outs and the Supercoppa. I don't know if the trial ran in 2018/19 and 2019/20. The Supercoppa di Serie C 2017/18 value stays `unknown`. The Serie C rows of 2017/18 to 2019/20 still say 3, with a note on the 2019/20 rows.

Open point, not a conflict: the rows for 2015/16 to 2019/20 call the free group of the Serie A list "under-21". The 2020 and 2022 texts call the same age cut "under 22". The label may differ while the rule is the same. I did not change these rows.

## Loose ends

Pass of 2026-10-16 over the free age group of the Serie A squad list in 2015/16 to 2019/20. I did not find the FIGC or Lega Serie A texts of these seasons in a readable form. The rows now follow press reports from each season that quote the rule. The age cut changed in 2018/19, so the open point above was a real change and not a label difference.

- 2015/16 (ita-239): "under 21", players not yet 21 on 31 December of the previous year (Il Post, 22 July 2015: https://www.ilpost.it/2015/07/22/regola-25-calciatori-serie-a/).
- 2016/17 (ita-256): "under 21" (fantacalcio.it, 18 August 2016: https://www.fantacalcio.it/approfondimenti/numeri/18_08_2016/rose-da-25-giocatori-ed-extracomunitari-ecco-la-situazione-229472).
- 2017/18 (ita-273): "under 21", born on or after 1 January 1996 (fantacalcio.it, 15 July 2017: https://www.fantacalcio.it/approfondimenti/numeri/15_07_2017/le-rose-da-25-delle-big-di-serie-a-la-situazione-attuale-272869).
- 2018/19 (ita-290): "under 22", born on or after 1 January 1996. The article calls it new against the "under 21" of the year before. It also gives at least 8 trained in Italy, at least 4 of them trained in the club (fantacalcio.it, 14 July 2018: https://www.fantacalcio.it/approfondimenti/14_07_2018/le-rose-da-25-delle-big-di-serie-a-la-situazione-attuale-302127).
- 2019/20 (ita-307): "under 22", born on or after 1 January 1997, with the same 8 and 4 (fantacalcio.it, 15 July 2019: https://www.fantacalcio.it/approfondimenti/numeri/15_07_2019/le-rose-da-25-delle-big-di-serie-a-la-situazione-attuale-370334).

The 2016/17 and 2017/18 rows keep the inference "as 2015/16" for the homegrown part. Each row's `registration` source is now the page above.

## Source fill

Date: 2026-10-16. A source-fill pass added `field=URL` entries to `sources` for filled fact fields that had none. It changed no values and kept row order and ids. Each URL was read on this date, and the value was checked against the page text.

Method:

- For `name`, the pass took the pages that the row already cites, season pages first, and kept the first page that contains the name. For a name with a note in brackets, the base name had to match. For an Italian group (girone), the page also had to name that group.
- For a value of `none` in `split` or `playoffs`, the source is the season page with one league stage and no championship or relegation round.

Fields filled: 275 `name`. Every name matched a page that the row already cites.

Disagreements: none found.

Lint after the pass: 0 problems.

## Pass 2

Date: 2026-10-17. This pass applies Max's decisions Q268 to Q331. Row ids stay. Lint after the pass: 0 problems (0 before).

### No new tiers (Q331, Q288)

The original runs Italy down to Serie C2 (`ita_ser_c2a` to `ita_ser_c2c`). Max's own example for Q331 is that the original stops at Serie C, so Serie D does not come in.

- Deleted the 27 Serie D rows (`comp` = `Serie D`, ids ita-12, ita-29, ita-46 and so on to ita-454). `ita-clubs.tsv` held no Serie D club rows.
- The Serie C2 rows already say `not held` from 2014/15, when the real fourth tier of the Lega Pro ended. From 2014/15 the game's lowest Italian tier is Serie C.
- Italy needs no nation that a game cannot select. No row was deleted for that reason.
- The Coppa Italia rows of 2008/09 to 2013/14 keep their 9 Serie D entrants. A cup is not a tier. The build takes these entrants from the database clubs of the spare Serie D division.
- The Supercoppa di Lega di Seconda Divisione stays, because it is the cup of an existing tier (Serie C2).

### Conflicts and open points

1. Non-EU rule 2001/02 (`ita-1`, `foreign`). Settled: Serie A clubs may register at most 5 non-EU players from foreign federations (Serie B 1), with no limit in the match list from May 2001. The FIGC dropped the cap of 3 in the match list on 4 May 2001 (https://sport.sky.it/calciomercato/2015/01/21/mercato_story_nakata_alla_roma_polemica_extracomunitari_juventus). L'Unita of 21 February 2003 says the limit of 5 registered stayed and that all 5 could play from 2001 (https://archivio.unita.news/assets/main/2003/02/21/page_020.pdf). FIGC C.U. 194/A still prints NOIF art. 40.7 with 5 and 1 in June 2004. The Serie B value of 1 comes from the 2004 text only.
2. Substitutes in the third tier from 2017/18. Settled: 5 substitutes, in at most three stoppages plus half-time, in the league, the play-offs, the play-outs and the Supercoppa Serie C.
   - 2017/18: Lega Pro trial (https://www.calcioefinanza.it/2017/08/22/lega-pro-2017-2018-sostituzioni/). Rows ita-281 to ita-283 and ita-288.
   - 2018/19: FIGC authorisation for the 2018/19 season (https://archivio.tuttomercatoweb.com/2018/serie-c-confermate-le-5-sostituzioni-anche-nella-prossima-stagione-1141202). Rows ita-298 to ita-300 and ita-305.
   - 2019/20: FIGC Federal Council authorisation of 31 July 2019 (https://www.tuttoreggina.com/serie-c/serie-c-2019-2020-confermate-le-cinque-sostituzioni-la-nota-51057). Rows ita-315 to ita-317.
   - The authorisations name no Coppa Italia Serie C. Those rows keep 3. The Supercoppa rows before 2017/18 stay `unknown`.
3. Serie D (open point in "Unknowns"). Closed by Q331: Serie D does not come in.
4. The clubs of database group C2/C in 2011/12 to 2013/14 (open point in "Season databases and club moves"). Closed by Q290: they become clubs with no division. In the same way, from 2014/15 the database clubs of the five third-tier groups that have no real Serie C place have no division.

### Club moves (Q279, Q280, Q290)

The first pass listed a tier only where its real club count differed from the database. Where Serie A or Serie B takes clubs from Serie C1, C1 is short, and C1 then takes clubs from C2. This pass lists every real club of each tier that the club moves leave short, or whose database clubs differ from the real clubs. The source is the final table of each group on the Italian Wikipedia season page. A script matched each club to the season database by name, and I checked every match whose names differ. Where a name changed, the club id of a later database confirmed the match (for example Cremapergo and Pergocrema, Cisco Collatino and Cisco Roma).

Added 522 club rows:

| Season | Database | Tiers added |
|---|---|---|
| 2003/04 | October 2003 | C1 groups A and B, C2 groups A to C |
| 2005/06 | September 2005 | C1 groups A and B, C2 groups A to C |
| 2007/08 | November 2007 | C1 groups A and B, C2 groups A to C |
| 2008/09 | September 2008 | Prima Divisione groups A and B, Seconda Divisione groups A to C |
| 2009/10 | September 2009 | Prima Divisione groups A and B, Seconda Divisione groups A to C |
| 2010/11 | September 2010 | Prima Divisione groups A and B |
| 2011/12 | September 2011 | Prima Divisione groups A and B |

The Seconda Divisione of 2012/13 and 2013/14 matches the database groups C2/A and C2/B club for club, so it gets no rows. Serie A 2003/04 matches the database.

Matches to note:

- CuoioCappiano 2003/04 and 2005/06 matches the database record "Cappiano-Romaiano". The club came from the merger of Cappiano Romaiano with Cuoiopelli in June 2003 (https://it.wikipedia.org/wiki/Cuoiopelli_Cappiano_Romaiano). From November 2007 the database has a record "Cuoiopelli" in C2/B, and the rows use it.
- Belluno 2003/04 matches "Seppelfricke Bellunopontalpi" (the same club id is "Belluno" from September 2011).

### Missing clubs (for Max)

These divisions still lack a club after the club moves. The season database holds no record of the club. No club records were added.

| Season | Division | Club |
|---|---|---|
| 2005/06 | Serie C2 girone A | Carpenedolo |
| 2005/06 | Serie C2 girone C | Gallipoli, Rende, Modica |
| 2007/08 | Serie C2 girone A | Caravaggese |
| 2007/08 | Serie C2 girone C | Pescina Valle del Giovenco, Celano |
| 2008/09 | Lega Pro Seconda Divisione girone A | Sambonifacese, Alghero |
| 2020/21 | Serie C girone C | Trapani |
| 2025/26 | Serie C girone B | Rimini |

The November 2020 database holds "Trapani Calcio" with nation -1 and no division. The December 2025 database holds "Rimini FC" with nation -1 in the division "A Lower Division". These records are not Italian clubs by the database, so the pass did not take them (Q244). Max decides.

### Reserve and B teams (Q289)

Every reserve team that a tier needs is in its season database: Juventus U23 (Juventus Next Gen from 2022/23) from 2018/19, Atalanta U23 from 2023/24, Milan Futuro in 2024/25 and Inter U23 in 2025/26. The build adds no reserve team for Italy.

### Part rules

No part rule of the brief applies to Italy. Late editions already play in their normal slot (Q277).

## Second source, pass 2

Date: 2026-10-17. This pass took every line of `ita.check.tsv` with the verdict `no second source` and looked for a page of another publisher. It skipped 96 lines of rows that pass 2 deleted (Serie D), 282 lines whose field holds `unknown`, 6 lines whose value starts with `unknown` and 126 lines whose value is an inference. That leaves 374 lines, written to `ita.check2.tsv`. Lint after the pass: 0 problems (0 before).

| Verdict | Lines |
|---|---|
| agree | 237 |
| disagree, settled | 6 |
| disagree, still split | 2 |
| no second source | 129 |

Main second sources:

- Substitutes (3 to 2020, then 5): Virgilio Sport (https://sport.virgilio.it/sostituzioni-nel-calcio-come-funzionano-e-come-sono-cambiate-nel-tempo-794125) and La Sicilia, 5 June 2020, for the five substitutions to the end of 2019/20 (https://www.lasicilia.it/news/sport/786256/la-serie-a-come-la-serie-c-cinque-sostituzioni-ma-in-tre-slot.html).
- Coppa Italia Serie C names: RSSSF (https://rsssf.org/tablesi/italcup2hist.html). Supercoppa di Serie C names: RSSSF (https://rsssf.org/tablesi/ital3supcuphist.html).
- Serie A squad size with no cap to 2014/15: Il Post, 22 July 2015 (https://www.ilpost.it/2015/07/22/regola-25-calciatori-serie-a/).
- Serie C promotion in 2026/27: Diretta.it.

Changes to `ita.tsv`:

1. Serie A tie-break play-offs, rows ita-376 (2023/24), ita-393, ita-410, ita-427 and ita-444. The first file had one match at a neutral venue for 1st and 17th place. Sky Sport (9 July 2023) and Calcio e Finanza (7 July 2023) agree that from 2023/24 the relegation play-off is two legs, with the better club by head-to-head at home in the second leg. The title play-off is one match at the home of the better club. Both play 90 minutes, then penalties. Sky Sport gives the same rule in May 2025. Decision: second. The `playoffs` field and its sources are new in these five rows. Row ita-359 (2022/23) keeps one match at a neutral venue.
2. Rows ita-427 (2026/27) and ita-444 (2027/28) also say that no play-off is held when a club in it plays a UEFA club final. Head-to-head then decides. Source: Calcio e Finanza, 26 May 2026 (https://www.calcioefinanza.it/?p=227469).

Other disagreements:

- Supercoppa di Serie C, 2016/17 (ita-271). RSSSF says the cup kept the Prima Divisione name to 2017. Sky Sport, 27 May 2017, calls the 2017 edition the Supercoppa di Lega Pro. Decision: first. No change.
- Supercoppa di Serie C, 2014/15 and 2015/16 (ita-237, ita-254). Conflict. RSSSF gives the Prima Divisione name to 2017. I found no third publisher that names these two editions. No Prima Divisione existed from 2014/15, so this is a question of reading. The value "Supercoppa di Lega Pro" stays. Max decides.

Lines that still have no second source: the transfer windows (each page found confirms only one window of the season), the non-EU quotas, the Serie A squad list of 2018/19 to 2021/22, the Supercoppa names before 2008, the Seconda Divisione Supercoppa, the "Lega Pro girone" names, the cup club counts of 2004/05 to 2008/09 (Transfermarkt blocks scripted reads with a human check), the bench of 15 in Serie C and in the cups from 2023/24, the Serie B bench of 7, the Serie C2 rows of 2011/12 to 2013/14, and the rows of 2027/28, for which no page exists yet.
