# Germany: second-source check for 0.6.0 (part `ger`)

Date: 2026-10-16. Check of `ger.tsv`, `ger-clubs.tsv` and `ger.md`. I did not change those files. Result: `ger.check.tsv`, 1,693 rows, one per filled fact field.

## Method

- Every filled fact field of every row got a verdict. Fields with `n/a` or `see nation rules` were skipped. For a `not held` row, the `format` field carries the check and `clubs` is not repeated.
- The first sources are mostly Wikipedia, plus goal.com, bundesliga.com, dfb.de and se-legal.de. No second source is a Wikipedia edition, and none is the same publisher as the first source of that fact.
- League tables: I read the RSSSF Germany season pages (`rsssf.org/tablesd/duit02.html` to `duit2026.html`) for the Bundesliga, the 2. Bundesliga, the 3. Liga, the Regionalliga up to 2008/09 and the Regionalliga promotion play-offs from 2012/13. For the Regionalliga from 2008/09, and as a cross-check before that, I read the final tables of fussballdaten.de for every group and season.
- I checked the `down` field of the Regionalliga by comparing the club lists of one season with the next season (all Regionalliga groups, 2. Bundesliga, 3. Liga). The first notes say `down` counts relegated clubs "withdrawals included". I used that definition, so a club that withdrew counts.
- Cups: RSSSF cup pages and the RSSSF list of Super and Liga Cup finals, the DFB-Pokal rules on dfb.de, and diefalsche9.de for the current entrants.
- Rules: dfl.de and Tagesspiegel (foreign players), the BeschV on gesetze-im-internet.de (work permits), Sport1, LigaInsider, nau.ch and goal.com (substitutes and bench), goal.com, echo24, zdfheute and bundesliga.com (transfer windows).
- Values in the RSSSF tables that differ from the rule places because of a licence refusal or a reprieve count as agree. The note says so.

## Counts

| Verdict | Rows |
|---|---|
| agree | 1,251 |
| no second source | 426 |
| disagree | 14 |
| found | 2 |

Most `no second source` rows are fields that the first agent marked `unknown` and that I also could not find: Regionalliga substitutes and bench (all seasons), 3. Liga bench, 3. Liga substitutes from 2021/22, League cup and Supercup substitutes and bench, League cup entry rules, and squad registration. The others are the 2027/28 rows, the 2026/27 Regionalliga rotation and relegations, the DFB-Pokal bench before 2019/20, and DFB-Pokal substitutes in 2018/19 and from 2021/22.

## Disagreements

| Row | Field | First value | Second value | Second URL |
|---|---|---|---|---|
| ger-82 (2007/08 RL Nord) | down | 0 | 9 (places 11 to 19 marked "Relegated" to the new tier-4 Regionalliga) | https://www.rsssf.org/tablesd/duit08.html |
| ger-83 (2007/08 RL Süd) | down | 1 | 8 (places 11 to 18 marked "Relegated" to tier 4) | https://www.rsssf.org/tablesd/duit08.html |
| ger-116 (2009/10 Supercup) | format | not held | unofficial Super Cup on 20 July 2009, Werder Bremen 2-1 VfL Wolfsburg | https://www.rsssf.org/tablesd/duitsupcuphist.html |
| ger-162 (2013/14 RL West) | down | 2 | 3 (Lippstadt, Velbert relegated; Leverkusen II withdrew) | https://www.fussballdaten.de/regionalliga/west/2014/tabelle/ |
| ger-164 (2013/14 RL Südwest) | down | 2 | 3 (Pfullendorf, SSV Ulm 1846, Eintracht Frankfurt II) | https://www.fussballdaten.de/regionalliga/suedwest/2014/tabelle/ |
| ger-175 (2014/15 RL West) | down | 3 | 4 (places 15 to 18: Uerdingen, Bochum II, Siegen, Hennef) | https://www.fussballdaten.de/regionalliga/west/2015/tabelle/ |
| ger-176 (2014/15 RL Nordost) | down | 1 | 2 (VFC Plauen, insolvent and last; Union Berlin II withdrew) | https://www.fussballdaten.de/regionalliga/nordost/2015/tabelle/ |
| ger-202 (2016/17 RL Nordost) | down | 1 | 2 (RB Leipzig II and FC Schönberg 95 left; 18th-placed Neustrelitz stayed) | https://www.fussballdaten.de/regionalliga/nordost/2017/tabelle/ |
| ger-251 (2020/21 RL Nord) | down | 0 | 1 (VfL Wolfsburg II left; no table relegation) | https://www.fussballdaten.de/regionalliga/nord/2021/tabelle/ |
| ger-264 (2021/22 RL Nord) | down | 0 | 5 (Lüneburger SK, HSC Hannover, Heider SV, Altonaer FC 93, FC Oberneuland) | https://www.fussballdaten.de/regionalliga/nord/2022/tabelle/ |
| ger-279 (2022/23 RL West) | down | 2 | 3 (Kaan-Marienborn, Wattenscheid 09, Straelen) | https://www.fussballdaten.de/regionalliga/west/2023/tabelle/ |
| ger-299 (2023/24 rules) | windows | 1 July to 31 August; 1 to 31 January | 1 July to 1 September 2023; 1 January to 1 February 2024 | https://www.goal.com/de/meldungen/transferfenster-bundesliga-2023-24-offen-schluss/bltf46cf4c4b511c7d9 |
| ger-312 (2024/25 rules) | windows | 1 July to 31 August; 1 to 31 January | summer closed Friday 30 August 2024, 20:00; winter closed 3 February 2025 | https://www.echo24.de/sport/fussball/werder-bremen-transfers-finale-so-laeuft-deadline-day-bundesliga-transferfenster-schluss-sommer-wechsel-transfermarkt-zr-93268504.html and https://www.zdfheute.de/sport/fussball-deadline-day-wintertransfer-2025-100.html |
| ger-325 (2025/26 rules) | windows | 1 July to 31 August; 1 to 31 January | extra window 1 to 10 June 2025 for the FIFA Club World Cup; windows close at 20:00 from 2025/26 | https://www.bundesliga.com/de/bundesliga/news/dfl-transferfenster-juni-prasidium-beschluss-fifa-klub-wm-31542 |

The first URL for each Regionalliga row is the first agent's Wikipedia season page, given in `ger.tsv`. The first URL for the windows rows is https://de.wikipedia.org/wiki/Wechselperiode, and for ger-116 it is https://en.wikipedia.org/wiki/Franz_Beckenbauer_Supercup.

Notes on the disagreements:

- ger-82 and ger-83 are a question of reading, not of fact. In 2007/08 the clubs placed 11th and lower stayed in a league called Regionalliga, but that league dropped to tier 4. The first agent counts 0 and 1 because only those clubs left the Regionalliga by name. RSSSF counts every club that dropped a tier. Max decides which reading the game uses.
- The `down` values of ger-162 to ger-279 omit clubs that withdrew or were not placed in the next season. The first notes say withdrawals are included, so these values do not match the first agent's own definition. The first agent's own `ger-clubs.tsv` agrees with the second source: none of the clubs named above is in the next season's list.
- The window rule "an end on a non-bank day moves to the next bank working day" does not explain 2023/24 or 2024/25. Inference: from 2022 the DFL chose end days that match other big leagues (zdfheute says England, Spain, Italy and France also closed on 3 February 2025).

## Found values

| Row | Field | Value | URL |
|---|---|---|---|
| ger-243 (2019 to 2021 RL Bayern) | down | 1: VfR Garching is the only club of the 17 that is not in Regionalliga Bayern 2021/22 or the 3. Liga. No table relegation. The reason is not confirmed. | https://www.fussballdaten.de/regionalliga/bayern/2022/tabelle/ |
| ger-103 (2008 T-Home Supercup) | subs_used | 5 per side, as used in the match (friendly-type rules) | https://www.rsssf.org/tablesd/duitsupcuphist.html |

## Non-EU limit, 2001/02 to 2005/06

- 2004/05 and 2005/06 are settled. The DFL page on the local-player rule says clubs could put 5 players from other continents under contract in 2004/05 and 4 in 2005/06. The plan to go to 3 in 2006/07 was dropped on 21 December 2005, and all limits ended from 2006/07. Tagesspiegel of 4 January 2006 says the same. EU and UEFA players were unlimited "in the years before". This agrees with ger-52 and ger-65, and it is a limit on players under contract.
  - https://www.dfl.de/de/hintergrund/transferwesen/local-player-regelung/
  - https://www.tagesspiegel.de/sport/auslanderbeschrankung-aufgehoben-1289497.html
- 2003/04: only an informal source agrees. It is a newsgroup thread of October 2003. It says Bundesliga clubs could sign and use up to 5 licensed non-UEFA players. It also says the DFB-Pokal allowed only 3 non-UEFA players, and that a club forfeited a 2003/04 cup match because it fielded a 4th. The thread does not settle whether the Bundesliga limit counted the contract or the pitch. I marked ger-39 as agree on this weak source.
  - https://de.rec.sport.fussball.narkive.com/s501dss1/anzahl-auslaender-in-bundesliga-mannschaft-historische-frage
- 2001/02 and 2002/03: I found no second source outside Wikipedia. Verdict: no second source.
- The English Wikipedia claim of 5 non-EU players per matchday squad has no support in any source I read. The 3-player limit of the original game matches the DFB-Pokal rule of 2003/04, not the Bundesliga rule. The 3-player limit for the Bundesliga was planned for 2006/07 but never applied. Inference: the game can use 5 non-UEFA players under contract for 2001/02 to 2004/05, 4 for 2005/06, and 3 on the pitch in the DFB-Pokal for 2001/02 to 2005/06. For 2001/02 and 2002/03 this rests on the first source only.

## Other findings for Max

- Work permits (all seasons, agree): no points system. Since the 2013 BeschV (section 22 no. 4), a non-EU professional needs to be at least 16, earn at least 50% of the pension contribution ceiling, and have the qualification confirmed by the German federation with the DOSB. I did not check the text in force before 2013. https://www.gesetze-im-internet.de/beschv_2013/__22.html
- DFB-Pokal: the DFB rules also give amateur sides home advantage against licensed clubs from the last 16 to the semi-finals, not only in round 1. https://www.dfb.de/dfb-pokal/modus/?m=1%2F
- Bench 7 before 2019/20 (Bundesliga, 2. Bundesliga): LigaInsider (November 2018) confirms 7 until 2018/19. It gives no start year, so the earlier seasons still rest on the first agent's inference.
- 3. Liga 2020/21: Sport1 confirms 3 substitutes at the start of the season. I found no second source for the switch to 5 from matchday 20.
- Regionalliga substitutes: a DFB data-centre report of a Regionalliga Nord match in 2025/26 shows 4 substitutions per side and 7 to 8 named substitutes. The limit was above 3 in that season, but I did not find the rule. https://datencenter.dfb.de/en/data-center/regional-league-north/2025-2026/30/2405516
- 2027/28: I did not give a verdict on these rows. They carry the current rules forward, and the reform vote is on 18 December 2026.

## Spot-check of ger-clubs.tsv

I compared every season and division of `ger-clubs.tsv` (103 lists) with the fussballdaten final table of the same group and season. All club counts and names agree, after these allowances:

- Spelling: for example Würzburger Kickers is "Kickers Würzburg" on fussballdaten.
- Clubs that withdrew in-season are not in the fussballdaten table rows: SSV Ulm 1846 (Süd 2010/11), VfB Lübeck and FC Oberneuland (Nord 2012/13), VFC Plauen (Nordost 2014/15), Türkgücü München (Bayern 2019/20), Türkspor Dortmund (West 2024/25).
- fussballdaten shows one extra row for FC Energie Cottbus in Nord 2017/18, from a play-off match.

One inconsistency in the first agent's files: `ger-clubs.tsv` lists 17 clubs under "2020/21 Regionalliga Bayern" (the 2019 to 2021 season after Türkgücü left), but `ger.tsv` has no 2020/21 Bayern row (ger-256 is `not held`). The build must take those 17 clubs from the 2019/20 row.
