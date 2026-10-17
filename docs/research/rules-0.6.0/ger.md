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

## Pass 2

Date: 2026-10-17. Decision agent. Rules: Q331 (no new tiers), Q288, Q279, Q280, Q289, Q290 and Q297 to Q327 of the spec. Row ids are kept. `rules_lint.py`: 0 problems before and after.

### 1. No new tiers (Q331)

The original's lowest German tier is the Regionalliga (`ger_regional`, tier 3). From 2008/09 the 3. Liga is tier 3 and the Regionalliga is tier 4. The Regionalliga rows from 2008/09 leave the table.

- Deleted from `ger.tsv`: 148 rows, 351 to 203.
  - Regionalliga Nord and Regionalliga Süd, 2008/09 to 2027/28: 40 rows.
  - Regionalliga West, Nordost, Südwest and Bayern, all 27 seasons: 108 rows. These groups run only at tier 4 from 2008/09. Their rows before 2008/09 were `not held`, so the competitions never run at a tier in the game.
- Deleted from `ger-clubs.tsv`: 1,508 club rows (every Regionalliga list from 2008/09), 1,925 to 417.
- `ger_regional` has rows only up to 2007/08. From 2008/09 the game does not run it. Tier 3 is the 3. Liga (`comp_file` = `new`) from 2008/09. Its 2001/02 to 2007/08 rows stay `not held`.
- The 3. Liga promotes from the Regionalliga, which is not in the game. The build takes the clubs that go up to the 3. Liga from the German clubs with no division.
- No German competition needs a nation that a game cannot select (Q288).
- The conflict on ger-164 (Regionalliga Südwest 2013/14) and the consistency items on ger-238 to ger-255 are void, because their rows are deleted.

### 2. Conflicts and open points

| Row | Field | Old | New | Decision |
|---|---|---|---|---|
| ger-82, 2007/08 Regionalliga Nord | down | 0 | 9 | second |
| ger-83, 2007/08 Regionalliga Süd | down | 1 | 8 | second |

- ger-82 and ger-83: in real life, places 11 and lower dropped from tier 3 to the tier-4 Regionalliga. The game drops tier 4 (Q331), so these clubs leave the game's divisions. RSSSF marks them "Relegated", and the Wikipedia table marks them "Qualification to Regionalliga" (tier 4 from 2008/09). Sportfreunde Siegen (Süd) went to the Oberliga after insolvency. URLs: https://www.rsssf.org/tablesd/duit08.html and https://en.wikipedia.org/wiki/2007%E2%80%9308_Regionalliga (both read on 2026-10-17). The `down` source of both rows now cites RSSSF too.
- ger-4 and ger-18 (open point of "Loose ends"): both rows use one reading, and they stay as they are. In those seasons the number of Regionalliga relegation places depended on how many clubs of that region came down from the 2. Bundesliga. ger-4 (2001/02 Nord) has 3 places because one Nord club came down. ger-18 (2002/03 Süd) has 5 places because SSV Reutlingen and SV Waldhof Mannheim came down. Their licence refusals later reprieved Stuttgarter Kickers and Sportfreunde Siegen, and that is in `notes` under Q272.
- 2009 Supercup (ger-116) and 2008 T-Home Supercup (ger-103): both matches took place in real life, so the rows stay as played matches.
- Non-EU limit 2001/02 to 2005/06 and bench 7 before 2014/15: no change in this pass. The check found that the 2004/05 and 2005/06 values agree with dfl.de. The second-source pass takes the rest.
- 2027/28: the rows keep the current structure. The Regionalliga reform vote of 18 December 2026 touches tier 4 only, so it no longer changes this table.

### 3. Club moves (Q279, Q280, Q290)

- Tiers 1 and 2: every season database has 18 clubs in the Bundesliga and 18 in the 2. Bundesliga, the real count in every season. No rows.
- Tier 3, 2001/02 to 2007/08: the lists of Süd 2002/03, Nord 2005/06 and Nord 2007/08 (19 clubs each) stay. The other seasons have the real counts.
- Tier 3 from 2008/09: the databases hold 36 clubs in German Regional Division North and South, and the real 3. Liga has 20. The 20-club 3. Liga list of each season (2008/09 to 2025/26, 360 rows) stays. Under Q290 the two database Regionalliga divisions have no real tier from 2008/09, so their clubs are in the pool. The `db_division` of these rows is now a valid source.
- Club rows added: 0. Club matches changed: 1. TuS Koblenz 2010/11 (3. Liga) is now club 6059 ("TuS Rot-Weiß Koblenz" in September 2010 ODB, German Regional Division South). Evidence: club 6059 is in German Second Division in the databases of November 2007 to June 2010, the seasons in which TuS Koblenz played in the 2. Bundesliga. From Autumn 2013 the same id carries the name "TuS Koblenz", and from October 2019 the other Koblenz club has its own id (9288). This is an inference from the database files. The real TuS Rot-Weiß Koblenz is another club (https://de.wikipedia.org/wiki/TuS_Koblenz, read on 2026-10-17).
- Missing clubs: none. All 51 missing clubs of the first pass were in deleted Regionalliga lists, and TuS Koblenz is now matched.

### 4. Reserve and B teams (Q289)

Reserve teams listed for the build: 0. Every reserve team in a kept list has a club record in its season's database: Bayern Munich II, VfB Stuttgart II, Werder Bremen II, Borussia Dortmund II, Mainz 05 II, SC Freiburg II, Hannover 96 II and TSG Hoffenheim II in the 3. Liga, and the reserve teams of the 2002/03, 2005/06 and 2007/08 Regionalliga lists.

### Files of other passes

`ger.check.tsv`, `ger.check.md` and `ger.reconcile.tsv` are unchanged. Their lines on deleted rows (the ids in section 1) no longer apply. The second-source agent skips them.

## Second source, pass 2

Date: 2026-10-17. File: `ger.check2.tsv`. Input: the 426 lines of `ger.check.tsv` with verdict `no second source`. 222 lines name rows that pass 2 deleted, and I skipped them. 133 lines name a field that holds `unknown` or an inference (registration, and `subs_used` and `bench` with `unknown`), and I skipped them. 15 lines name the DFB-Pokal `bench` of 2001/02 to 2015/16 (ger-10 to ger-192). The row notes mark that value as an inference from final line-ups, so I skipped them too. 56 lines remained. Each second source is from another publisher than the first source. I read each page on 2026-10-17.

| Verdict | Lines |
|---|---|
| agree | 51 |
| disagree, settled | 1 |
| disagree, still split | 0 |
| no second source | 4 |

Main second sources:

- League cup 2001 to 2007 (name, clubs, format): fussballarchiv.de.tl (official names, entry rules per edition), fussballdaten.de (6 teams and the rounds of each edition) and Horizont of 10 July 2007 (2007 entry rule and the Premiere-Ligapokal name).
- DFB-Pokal `bench` and `subs_used`: DFB Durchführungsbestimmungen of December 2016, September 2022, October 2023 and July 2024, DFB Offizielle Mitteilungen 4/2018, the DFB-Pokal implementing rules 2025/26, Sport1 (August 2019) and BDFL (August 2021).
- 2027/28 rows (ger-339 to ger-341, ger-350, ger-351): the DFL Spielordnung of 4 August 2026, the DFL Lizenzordnung Spieler of 1 July 2026, bundesliga.com, dfb.de and fussballdaten.de. These rows carry the current rules forward. The second source confirms the 2026/27 rule, and states no change for 2027/28.

### Change to ger.tsv

- ger-338 (2026/27 nation rules) `windows`. Old: "winter from 1 January 2027, end date not announced on 2026-10-16 (standard rule: 31 January)". New: "winter 1 January to 1 February 2027, ending at 20:00". Decision: second. The DFL Lizenzordnung Spieler (Stand 1 July 2026, footnote to § 4) gives the decision of 10 June 2026. 31 January 2027 is a Sunday, so the window ends on Monday 1 February. The summer part agrees with fussballdaten (1 September 2026, 20:00). `notes` and `sources` of the row now cite https://media.bundesliga-gruppe.de/sites/2/2026/07/Lizenzordnung-Spieler-LOS-2026-07-01-Stand.pdf. This is primary rule text, so I did not look for a third publisher.

### Other findings

- ger-37 and ger-50 (league cup 2003 and 2004): the 9th-placed club took the last place, because places 6 to 8 played in the UI Cup. The `format` value "next club in" fits this reading. I did not change it.
- DFB-Pokal bench: the team sheet grew from 18 to 20 players in 2019/20 (Sport1, and DFB text of July 2019). This agrees with bench 7 up to 2018/19.

### No second source

- ger-13 and ger-26 `foreign` (5 non-European players, 2001/02 and 2002/03). dfl.de gives the limit from 2004/05 only. A newsgroup thread gives 2003/04 only.
- ger-335 and ger-348 `subs_used` (DFB-Pokal 2026/27 and 2027/28). No DFB-Pokal rule text for 2026/27 was found. The 2025/26 text gives five.

`rules_lint.py`: ger.tsv 0 problems before and after.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after.

- Added 40 rows, ger-352 to ger-391: `ger_regional` Regionalliga Nord and Regionalliga Süd, 2008/09 to 2027/28, with `format` = `not held`. The Regionalliga ended as tier 3 after 2007/08 and became tier 4 (Q331). Each pair follows the 3. Liga row of its season. Source: https://en.wikipedia.org/wiki/Regionalliga (levels 3 from 1994 to 2008, 4 from 2008), read on 2026-10-17.
