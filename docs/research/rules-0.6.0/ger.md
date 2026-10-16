# Germany: rules table for 0.6.0 (slice 6, part `ger`)

Date: 2026-10-16. Research only. Files: `ger.tsv` (351 rows, 13 per season for 2001/02 to 2027/28) and `ger-clubs.tsv` (1,925 club rows).

## Method

- I read the raw wikitext of the English Wikipedia season pages for the Bundesliga, 2. Bundesliga, 3. Liga, Regionalliga and DFB-Pokal, 2001–02 to 2026–27, and their table templates. A script read the club lists and the promotion and relegation marks from the standings tables.
- Rules come from the main articles (Bundesliga, 2. Bundesliga, 3. Liga, DFB-Pokal, DFL-Ligapokal, Franz Beckenbauer Supercup) and the German articles Fußball-Bundesliga, Ausländerregelung, Wechselperiode and 3. Fußball-Liga. Some facts come from goal.com, bundesliga.com, dfb.de and se-legal.de.
- Most first sources are Wikipedia. The second-source check must use other publishers.
- Club moves: a name matcher compared each real club with the German clubs of that season's database. I checked every partial match by hand and removed the wrong ones.

## Rows per season

| comp | comp_file | Seasons held |
|---|---|---|
| Bundesliga | ger_first | all |
| 2. Bundesliga | ger_second | all |
| 3. Liga | new | 2008/09 on |
| Regionalliga Nord | ger_regional | all (tier 3 to 2007/08, tier 4 from 2008/09) |
| Regionalliga Süd | ger_regional | 2001/02 to 2011/12 |
| Regionalliga West | new | 2008/09 on |
| Regionalliga Nordost, Südwest, Bayern | new | 2012/13 on (Bayern 2020/21: the 2019 to 2021 season continued, row ger-256) |
| DFB-Pokal | ger_cup | all |
| League cup (Ligapokal) | ger_lge_cup | 2001/02 to 2007/08 |
| Supercup | new | 2008/09 and 2009/10 (unofficial), 2010/11 on |
| nation rules | n/a | all |

The rows for the original's two Regionalliga groups keep `ger_regional`. Regionalliga Nord continues as a tier-4 group. The build decides if that file runs the tier-4 groups.

## Change points

- **2008/09:** 3. Liga with 20 clubs. Regionalliga drops to tier 4 with 3 groups (Nord, West, Süd), champions promoted. Relegation play-offs return: Bundesliga 16th v 2. Bundesliga 3rd, 2. Bundesliga 16th v 3. Liga 3rd, two legs. The 2. Bundesliga goes from 3 up and 4 down to 2 up and 2 down plus the play-offs. DFB-Pokal: top 4 of the 3. Liga enter, and reserve teams are out. There is no Ligapokal. An unofficial T-Home Supercup took place.
- **2007/08:** this was the transition season. Regionalliga places 1 and 2 went up, and places 3 to 10 went into the new 3. Liga.
- **2010/11:** the DFL-Supercup became official. It has no extra time.
- **2012/13:** 5 Regionalliga groups. The 5 champions and the Südwest runner-up play 3 two-legged ties for 3 promotion places.
- **2018/19:** the 3. Liga relegates 4 clubs. The Regionalliga promotes 3 clubs direct and 1 through a play-off. From 2020/21, West and Südwest always go up direct. Nord, Nordost and Bayern take turns for the third place. The ger.tsv rows give the order.
- **2019/20:** the bench goes from 7 to 9 in the Bundesliga, the 2. Bundesliga and the DFB-Pokal. Clubs may use 5 substitutes from the May 2020 restart.
- **2020/21:** 5 substitutes in the Bundesliga, the 2. Bundesliga and the DFB-Pokal. The 3. Liga used 3 to matchday 19, then 5. The 2019 to 2021 Regionalliga Bayern season ran on, so the group has no 2020/21 row. Regionalliga Nord played in 2 sub-groups in 2020/21 and 2021/22.
- **2021/22:** the relegation play-offs have no away goals rule.
- **2022/23:** the DFL made 5 substitutes permanent.
- **2025/26:** the Supercup became the Franz Beckenbauer Supercup.
- **2027/28:** no change is decided. The DFB plans an extraordinary Bundestag on 18 December 2026 to vote on the Kompass model. That model has 4 Regionalliga groups of 20 clubs and promotes all 4 champions. In the plan, 2027/28 is a qualification season and the 4 groups start in 2028/29. The 2027/28 rows keep the current rotation (Bayern direct, Nord v Nordost), and that is an inference. Max decides if the reform goes in after the vote.

## Foreign players and other nation rules

- 2001/02 to 2003/04: no limit on EU or UEFA players. Up to 5 non-European players may play (de.wikipedia Fußball-Bundesliga).
- 2004/05: 5 non-European players under contract. 2005/06: 4 (de.wikipedia Ausländerregelung).
- 2006/07 on: no foreign limit. Each club must hold 12 German licensed players under contract, of whom 4 (2006/07), 6 (2007/08) and then 8 were trained at a German club.
- Windows: 1 July to 31 August and 1 to 31 January. 2020 summer: 1 July, then 15 July to 5 October. 2022 summer: to 1 September. From 2023/24 the end days follow dates agreed with other big leagues (see Reconcile).
- Work permits: I found no points system. The source describes current residence law only.

## Disagreements for Max

1. **Non-EU limit.** The English Bundesliga article says that a matchday squad may hold no more than 5 non-EU players, with no date. The German sources say the limit ended in 2006/07. The 2003/04 DFB-Pokal page cites a maximum of 3 non-EU players in a cup match. The original game uses 3 non-EU for Germany. The German Bundesliga article says 5 non-European players "since 2001". A second source must decide the 2001/02 to 2005/06 value and whether the limit counted players under contract or players in a match.
2. **Bench 7 before 2014/15** is an inference. The DFB-Pokal finals of 2002 to 2012 list 7 substitutes per side, and goal.com says the bench was 7 before 2019/20.

## Unknowns

- Substitutes and bench of the Regionalliga, in all seasons.
- Bench of the 3. Liga, and its substitutes from 2021/22.
- Ligapokal and Supercup substitutes and bench, and extra time in the Ligapokal.
- Squad registration limits before 2006/07.
- 2019/20 Regionalliga Bayern relegations (the season ran to 2021). 2026/27 relegations (the season is in progress). 2027/28 club counts.

## Club moves

- Database per season: the starting table of the spec. 2004/05, 2006/07, 2026/27 and 2027/28 have no database, so they have no club rows.
- In every database, the Bundesliga and the 2. Bundesliga have 18 clubs, the real count, so they have no rows. Regionalliga Nord and Süd have 18 + 18 clubs.
- Up to 2007/08, rows exist only where the real count differs: Süd 2002/03 (19), Nord 2005/06 (19), Nord 2007/08 (19).
- From 2008/09, the databases put the 3. Liga clubs and some Regionalliga clubs into the two old Regionalliga divisions (36 clubs). The 3. Liga and every Regionalliga group are new divisions, so every real club has a row.
- `db_division` shows where the database holds the club now. Under Q244, only clubs with no division (`A Lower Division`, `none`, `Reserve`) may fill a short division. Many real clubs are in the two old Regionalliga divisions. The build must move them.
- Accepted name changes (the same club under another name in the database): Rot Weiss Ahlen = LR Ahlen, Türkiyemspor Berlin = Berliner FC Türkiyemspor, Weiche Flensburg = ETSV Weiche, SV Elversberg = SpVgg Elversberg, FC Viktoria Köln = SCB Viktoria Köln, SV Bergisch Gladbach 09 = SSG Bergisch-Gladbach. The last two are inferences.

### Missing clubs (no club in the season's database)

| Season | Division | Clubs |
|---|---|---|
| 2009/10 | Regionalliga Süd | SpVgg Greuther Fürth II, SG Sonnenhof Großaspach |
| 2010/11 | 3. Liga | TuS Koblenz |
| 2010/11 | Regionalliga Süd | SpVgg Greuther Fürth II, TSG 1899 Hoffenheim II, SG Sonnenhof Großaspach, FSV Frankfurt II |
| 2011/12 | Regionalliga Nord | Berliner AK 07 |
| 2011/12 | Regionalliga Süd | SG Sonnenhof Großaspach, SpVgg Greuther Fürth II, 1899 Hoffenheim II, FC Ingolstadt 04 II, FSV Frankfurt II |
| 2011/12 | Regionalliga West | TuS Koblenz |
| 2012/13 | Regionalliga Bayern | FC Ingolstadt 04 II, SpVgg Greuther Fürth II, SV Seligenporten, FC Augsburg II, SC Eltersdorf |
| 2012/13 | Regionalliga Nord | ETSV Weiche, BSV Schwarz-Weiß Rehden |
| 2012/13 | Regionalliga Nordost | Berliner AK 07, 1. FC Union Berlin II |
| 2012/13 | Regionalliga Südwest | TuS Koblenz, 1899 Hoffenheim II, FSV Frankfurt II |
| 2013/14 | Regionalliga Bayern | TSV Rain am Lech |
| 2014/15 | Regionalliga Bayern | VfR Garching |
| 2014/15 | Regionalliga Nord | FT Braunschweig |
| 2014/15 | Regionalliga West | SV Rödinghausen, FC Hennef 05 |
| 2015/16 | Regionalliga Bayern | FC Amberg, TSV Rain am Lech |
| 2015/16 | Regionalliga Nord | SV Drochtersen/Assel |
| 2015/16 | Regionalliga Südwest | TSV Steinbach, SV Saar 05 Saarbrücken |
| 2015/16 | Regionalliga West | SV Rödinghausen |
| 2016/17 | Regionalliga Bayern | VfR Garching |
| 2016/17 | Regionalliga Nord | SV Drochtersen/Assel, 1. FC Germania Egestorf/Langreder |
| 2016/17 | Regionalliga Nordost | RB Leipzig II, FSV Union Fürstenwalde |
| 2016/17 | Regionalliga Südwest | TSV Steinbach, Teutonia Watzenborn-Steinberg |
| 2016/17 | Regionalliga West | SV Rödinghausen |
| 2017/18 | Regionalliga Nord | Eutin 08 |
| 2017/18 | Regionalliga Nordost | Union Fürstenwalde |
| 2019/20 | Regionalliga Nord | HSC Hannover |
| 2025/26 | Regionalliga Nord | FSV Schöningen |
| 2025/26 | Regionalliga Nordost | BFC Preussen, 1. FC Magdeburg II |

TuS Koblenz may be the database's "TuS Rot-Weiß Koblenz". That is another real club, so I did not match it. Max decides.

## Sources (main pages)

- https://en.wikipedia.org/wiki/Bundesliga, https://en.wikipedia.org/wiki/2._Bundesliga, https://en.wikipedia.org/wiki/3._Liga, https://en.wikipedia.org/wiki/Regionalliga, https://en.wikipedia.org/wiki/DFB-Pokal, https://en.wikipedia.org/wiki/DFL-Ligapokal, https://en.wikipedia.org/wiki/Franz_Beckenbauer_Supercup
- Season pages `<YYYY–YY> Bundesliga`, `2. Bundesliga`, `3. Liga`, `Regionalliga`, `DFB-Pokal`, and `2019–2021 Regionalliga Bayern`
- https://de.wikipedia.org/wiki/Fu%C3%9Fball-Bundesliga, https://de.wikipedia.org/wiki/Ausl%C3%A4nderregelung, https://de.wikipedia.org/wiki/Wechselperiode, https://de.wikipedia.org/wiki/3._Fu%C3%9Fball-Liga
- https://www.goal.com/de/meldungen/bundesliga-neun-auswechselspieler-ersatzbank-dfl-regeln/mi84hthlriee1wp7jhtqwvbpz (bench 7 to 9, 2019/20)
- https://www.bundesliga.com/de/bundesliga/news/neu-saison-2022-23-regeln-auswechslungen-spielabsetzungen-transferperiode-20452 (5 substitutes permanent, window to 1 September 2022)
- https://www.dfb.de/news/regionalliga-reform-dfb-praesidium-bereitet-ausserordentlichen-bundestag-vor (Kompass model, 11 September 2026)
- https://se-legal.de/rechtsanwalt/auslaenderrecht-aufenthaltsrecht/visum-und-aufenthaltserlaubnis-fuer-profifussballer/ (work visa)

## Reconcile

Date: 2026-10-16. File: `ger.reconcile.tsv`. Third sources are transfermarkt.de (Regionalliga tables and club lists of the next season), Trivela and voetbalkrant (2009 Supercup), Sport1, 90min, ran, zdfheute and Stuttgarter Zeitung (transfer windows). kicker.de and weltfussball.de refused the request. The `down` decisions use the new brief default: direct places by the rules of the season. Withdrawals, expulsions and licence refusals are in `notes`.

| Decision | Rows |
|---|---|
| first | 5 |
| second | 6 |
| conflict | 3 |
| found accepted | 2 |

### Changes to ger.tsv

- ger-175 `down` 3 to 4. ger-264 `down` 0 to 5.
- ger-116 (2009/10 Supercup) is now the unofficial match of 20 July 2009. Other fields that changed to match it: `name`, `clubs` (2), `subs_used` (6 used per side), `bench` (unknown) and `notes`.
- ger-103 `subs_used`: 5 used per side, limit unknown.
- ger-299, ger-312 and ger-325 `windows`: the real dates. The bank-day rule and the "uniform since the 1990s" note are out of these 3 rows.
- ger-243 and ger-256 (Regionalliga Bayern 2019 to 2021): ger-256 was `not held`, but `ger-clubs.tsv` lists 17 clubs for 2020/21. ger-256 now holds the continued season with 17 clubs, up 0, down 1 (VfR Garching, relegated in 2021) and the Bayern side of the play-off with the Nord champion. ger-243 `down` is 0, because nobody went down in 2020.
- ger-162, ger-176, ger-202, ger-251 and ger-279: values kept. The withdrawn clubs are now in `notes`.
- In the 15 changed rows, the note "withdrawals included" now gives the new definition. 72 other rows still carry the old note. The second check agreed with their values, and in the rows that I read, the values are table places. Inference: the old note is wrong in wording only. A global replace is left to the lead.

### Conflicts for Max

1. **ger-82, 2007/08 Regionalliga Nord, `down`.** First value 0, second value 9. Places 11 to 19 (9 clubs) stayed in a league named Regionalliga, which became tier 4 in 2008/09 (transfermarkt club lists). Does a drop from tier 3 to tier 4 count as relegation? If yes, the value is 9. If no, it is 0.
2. **ger-83, 2007/08 Regionalliga Süd, `down`.** First value 1, second value 8. Places 11 to 18 (8 clubs) dropped to tier 4. Sportfreunde Siegen is the only club that left the Regionalliga by name. If a tier drop counts, the value is 8. If not, it is 1.
3. **ger-164, 2013/14 Regionalliga Südwest, `down`.** First value 2, second value 3. Under the new default neither fits. SC Pfullendorf (18th) went down on table place. SSV Ulm 1846 (15th) went down after insolvency. Eintracht Frankfurt II (12th) withdrew. Wormatia Worms (16th) and KSV Baunatal (17th) stayed. I found no rule text that gives the number of relegation places. Proposal: 1, with Ulm and Frankfurt II in notes.

### Other findings

- ger-312: the sources do not agree on the closing time of the 2024 summer window (Sport1 18:00, echo24 20:00). The date is 30 August in both.
- ger-338 (2026/27) still says 31 August. ran says the summer 2026 window closed on Tuesday 1 September 2026 at 20:00. I did not change it because it was outside the check list. https://ran.joyn.de/sports/fussball/deadline-day2026-wann-schliesst-das-transferfenster-in-den-wichtigsten-ligen-178634


## Loose ends

Date: 2026-10-16. I read the English Wikipedia Regionalliga season page of each season today (2001/02 to 2006/07 and 2012/13 to 2025/26), plus the German pages for 2001/02 and 2005/06. For each of the 72 rows with the old note "withdrawals included", I compared `down` with the relegation places in the final table.

Wording: the old note is replaced in all 72 rows by the default text: "down = direct relegation places by the rules of the season; withdrawals, expulsions and licence refusals are in notes". Each row also gives its table places, for example "Places 16 to 18 in the final table". Where a withdrawal, licence refusal or reprieve moved a club, the note names it.

63 values are the table places and are unchanged. In 9 rows the value was the actual outcome, and I changed it:

| Row | Old | New | Reason |
|---|---|---|---|
| ger-18, 2002/03 Süd | 3 | 5 | Places 15 to 19. Stuttgarter Kickers and Sportfreunde Siegen were reprieved because SSV Reutlingen and SV Waldhof Mannheim, relegated from the 2. Bundesliga, did not get a Regionalliga licence. |
| ger-165, 2013/14 Bayern | 4 | 3 | Places 17 to 19. The old value counted the loser of the relegation play-off (15th). |
| ger-178, 2014/15 Bayern | 4 | 2 | Places 17 and 18. The old value counted both play-off losers (15th and 16th). |
| ger-186, 2015/16 Nord | 3 | 2 | Places 17 and 18. Goslarer SC 08 (16th) did not apply for an Oberliga licence. |
| ger-191, 2015/16 Bayern | 3 | 2 | Places 17 and 18. The old value counted the play-off loser (15th). |
| ger-240, 2019/20 West | 2 | 0 | Curtailed season, no relegation on sporting criteria. The old value counted TuS Haltern (voluntary) and SG Wattenscheid 09 (administration). |
| ger-241, 2019/20 Nordost | 2 | 0 | Cancelled season, no relegation on sporting criteria. The old value counted Wacker Nordhausen (voluntary) and Rot-Weiß Erfurt (administration). |
| ger-269, 2021/22 Bayern | 4 | 3 | Places 18 to 20. The old value counted the play-off loser (16th). |
| ger-282, 2022/23 Bayern | 5 | 4 | Places 17 to 20. The old value counted the play-off loser (16th). |

The Bayern relegation play-off places are now in `notes` of each Bayern row that has them. The `playoffs` field is unchanged.

Rows where a withdrawn or insolvent club sits inside the relegation places keep the count. Examples are ger-147 (Lübeck and Oberneuland, 2012/13), ger-303 (Teutonia Ottensen, 2024/25) and ger-305 (Türkspor Dortmund and KFC Uerdingen, 2024/25).

ger-338 (2026/27 nation rules): `windows` now says that the summer window ran from 1 July to 1 September 2026 and ended at 20:00. ran.de states the end and gives the reason: the last Monday of August was a bank holiday in England. The winter end of January 2027 was not announced on 2026-10-16. The field gives the standard rule (31 January) and says that the date is not confirmed.

Open point for Max: ger-18 treats Stuttgarter Kickers and Siegen as reprieved, under the brief default. In 2001/02 Nord (ger-4), Preußen Münster stayed because only one Nord club came down from the 2. Bundesliga. That is the size rule of the league, so I counted it as a rules place and not as a reprieve. Check that both rows follow the same reading.

## Source fill

Method: I ran the rules lint and took each filled fact field with no `field=URL` entry. I read each page on 2026-10-16 with curl and checked the value against the page text or league table. For `split` = none and `format` = double round robin, the season page must show one table for the division with every club on 2 x (clubs - 1) matches, and no championship or relegation round. For a stopped season (2019/20, 2020/21), a table with fewer matches counts if the page says the season was stopped. I added sources only. I changed no value, row order or id.

- Sources added: 125 field entries in 125 rows (`name` 90, `playoffs` 33, `split` 2).
- `name` of a competition that was not held in a season (for example 3. Liga before 2008/09): the league article, which gives the name.
- ger-251 (2020/21 Regionalliga Nord, two sub-groups): https://de.wikipedia.org/wiki/Fu%C3%9Fball-Regionalliga_Nord_2020/21 (Gruppe Nord and Gruppe Süd).
- ger-264 (2021/22): https://en.wikipedia.org/wiki/2021%E2%80%9322_Regionalliga (Nord and Süd groups, then a championship round and a relegation round).

No source found: none.

Disagreements: none. Note on a page: the 2008/09 Regionalliga page names a play-off between two Oberliga clubs for a Regionalliga Nord place. The Regionalliga divisions had no play-off of their own, so `playoffs` = none stays (ger-95 to ger-97).
