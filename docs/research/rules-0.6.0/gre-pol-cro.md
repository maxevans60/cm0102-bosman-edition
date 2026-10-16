# Rules table part gre-pol-cro: Greece, Poland and Croatia (slice 7)

Research for release 0.6.0. Date: 2026-10-16. Files:

- `gre-pol-cro.tsv`: 486 rows, 18 competitions x 27 seasons (2001/02 to 2027/28).
- `gre-pol-cro-clubs.tsv`: 1,634 club rows for the divisions whose real size differs from the season database.
- This file: method, decisions, missing clubs and unknowns.

## Scope

| comp_file | Competition | Notes |
|---|---|---|
| gre_prm | Alpha Ethniki, Super League Greece, Super League 1 (tier 1) | 14, 16, 18, then 14 with a split from 2019/20 |
| gre_second | Beta Ethniki, Football League, Super League 2 (tier 2) | one group, or the North group in two-group seasons |
| new (comp `league group 2`) | second group of the Greek tier 2 | 2013/14, 2014/15, 2021/22 to 2025/26 |
| gre_cup | Greek Football Cup | league-phase format from 2025/26 |
| gre_super | Greek Super Cup | held 2007/08, then from 2025/26 |
| n/a | Greece nation rules | |
| pol_first | I liga (Ekstraklasa), Ekstraklasa from 2008/09 (tier 1) | 16, 14, 16, 18 |
| pol_second | II liga, I liga from 2008/09 (tier 2) | 20, then 18 |
| pol_cup | Polish Cup | |
| pol_lge_cup | Puchar Ligi Polskiej 2001/02, Puchar Ekstraklasy 2006/07 to 2008/09 | not held in other seasons |
| pol_super | Polish Super Cup | |
| n/a | Poland nation rules | |
| cro_a1 | Prva HNL, HNL from 2022/23 (tier 1) | 16, 12, 16, 12, 10 |
| cro_a2a | Druga HNL North to 2005/06, then the single Druga HNL, Prva NL from 2022/23 (tier 2) | |
| cro_a2b | Druga HNL South | not held from 2006/07 |
| cro_cup | Croatian Football Cup | 48 clubs |
| new (comp `super cup`) | Croatian Football Super Cup | the original game has none |
| n/a | Croatia nation rules | |

Out of scope: the third tiers (Gamma Ethniki, Polish II liga from 2008/09, Croatian 3. HNL groups). The original runs no league code for them.

## Method

- English Wikipedia season pages, read raw (`action=raw`) on 2026-10-16. A script read the sports-table templates (club order, result keys) and the round tables of the cups. Where a season page holds its table in a template, the template page is the source.
- RSSSF season pages for the Polish second tier 2002/03 to 2006/07, which have no English Wikipedia page.
- Polish Wikipedia for the 2001/02 Polish League Cup and the Polish top-flight name (I liga to 2007/08, Ekstraklasa from 2008/09).
- Foreign-player rules: a comparative quota paper dated 30 May 2008 (rdes.it), the HFF transfer regulations of June 2022, the HNS competition rules of 2020, and news pages (insideworldfootball.com, sportowefakty.wp.pl, prawosportowe.pl, soccerway.com, index.hr, gameofthepeople.com). The PDFs were read as text with pdftotext. The news pages were read through WebFetch with a request for verbatim quotes.
- `up` and `down` are direct places. Where the table key shows extra demotions or withdrawals, the field holds the planned count and the notes give the real outcome. Where the planned count is not clear, the field is `unknown` and the notes give the real outcome.
- Club matching: real names matched to database names by a script (normalised tokens, fuzzy match, a short list of name overrides, reserve teams only to reserve records). Matches below the threshold are left empty.

## Decisions taken in the table

1. **Greek tier 2 in two groups.** The database has one Greek second division. In two-group seasons the `gre_second` row holds the North group and a `new` row holds the South group. In one-group seasons the `new` row says `not held`.
2. **Croatian tier 2.** The database has two second divisions (North and South). From 2006/07 the real tier 2 is one league. `cro_a2a` holds it and `cro_a2b` says `not held`. The 16 database clubs of the South division then have no real division. Max decides where they go.
3. **Super cups that a double cancels.** The Croatian Super Cup is not played when one club wins the double. Those seasons keep the format with "(real: not played this season)", because the game decides the double itself. The Greek Super Cup was abolished from 2008/09 to 2024/25, so those rows say `not held`. The Polish Super Cup plays the cup runner-up after a double from 2016 (the 2013 match was not played after a double).
4. **Substitutes.** 3 used to 2019/20. 5 from 2020/21. Only Poland has a league source for 2020/21. For Greece and Croatia the 5 is an inference from the IFAB rule. Bench size is `unknown` except Croatia 2020/21 (7 named substitutes, HNS rules 2020).
5. **Transfer windows.** From 2002/03 to 2022/23 the rows give the FIFA frame (two registration periods, 12 weeks and 4 weeks at most). From 2023/24 the rows give the RSTP March 2023 frame (8 to 12 weeks and 4 to 8 weeks, 16 weeks together), and the national dates where a federation or league page gives them (see Reconcile). 2001/02 is `unknown`.
6. **2027/28.** No season pages exist yet. The rows carry 2026/27 and say "inference".
7. **Database names.** The two databases with an extra word in the folder name appear as "October 2003" and "September 2005".

## Main changes by season

- Greece tier 1: 14 (2001/02), 16 (2002/03 to 2012/13), 18 (2013/14, 2014/15), 16 (2015/16 to 2018/19), 14 from 2019/20. Super League from 2006/07. European play-off mini-league of 2nd to 5th from 2007/08 to 2016/17. Split from 2019/20: top 6 and bottom 8; from 2024/25 top 4, 5th to 8th and bottom 6.
- Greece tier 2: Beta Ethniki to 2009/10 (14, 16, 18), Football League 2010/11 to 2018/19 (18, 21, two groups 2013/14 and 2014/15, 18, 16), Super League 2 from 2019/20 (12, two groups 2021/22 to 2025/26, one group of 16 in 2026/27).
- Greek Cup: two-legged rounds to 2004/05; single rounds with replays 2005/06 to 2010/11; group stage 2014/15 to 2018/19; Super League clubs only in 2020/21; league phase from 2025/26.
- Poland tier 1: two groups of 8 then championship and relegation groups (2001/02), 16, 14 (2003/04, 2004/05), 16, split 30 + 7 from 2013/14 to 2019/20 (points halved to 2016/17), 18 from 2021/22.
- Polish League Cup: 2001/02 and 2006/07 to 2008/09 only.
- Polish Cup: final two legs 2001/02 to 2005/06 and 2012/13; quarter-finals and semi-finals two legs to 2017/18; all single matches from 2018/19.
- Croatia tier 1: 16 (2001/02), 12 with a split after 22 rounds (2002/03 to 2005/06), 12 with 33 rounds (2006/07 to 2008/09), 16 (2009/10 to 2011/12), 12 (2012/13), 10 with 36 rounds from 2013/14.
- Croatian tier 2: two groups to 2005/06, one league from 2006/07, Prva NL of 12 from 2022/23, 16 in 2026/27.
- Croatian Cup: final two legs to 2013/14 and in 2023/24 and 2024/25; quarter-finals single from 2015/16; semi-finals single from 2017/18.
- Foreign players: Greece Super League 5 non-EU (2007/08), HFF limit of 8 non-EU (5 over 23, 5 on the match sheet) in 2020/21 to 2022/23, 7 over 23 and 5 on the field from 2026/27. Poland: no Ekstraklasa limit to 2014/15, 3 non-EU on the pitch in 2015/16, 2 from 2016/17 to 2018/19, no limit from 2019/20 with a youth-player rule (one on the pitch 2019/20 to 2021/22, 3000 minutes 2022/23 to 2024/25, no penalty from 2025/26). Croatia: 4 foreigners fielded (2007/08), 6 then 8 foreigners (2016/17), 6 (2020/21), 6 non-EU (2025/26), 5 non-EU from 2026/27.

## Missing clubs (Q244)

Real clubs that the script found in no club record of that nation in the season database. Some may exist under another name. Max decides.

| Season | Division | Clubs |
|---|---|---|
| 2001/02 | gre_second | Apollon Smyrnis, Panegialios |
| 2002/03 | cro_a2a | Omladinac Novo Selo Rok, Sloga Nova Gradiška, Valpovka |
| 2002/03 | cro_a2b | Inker Zaprešić, TŠK Topolovac |
| 2002/03 | pol_second | Aluminium Konin, MCP Bielsko-Biala |
| 2003/04 | cro_a1 | Inter Zaprešić |
| 2003/04 | cro_a2a | Međimurje, Valpovka, Sloga Nova Gradiška |
| 2003/04 | pol_second | Aluminium Konin |
| 2005/06 | cro_a2a | Čakovec, Graničar Županja |
| 2005/06 | cro_a2b | Naftaš Ivanić Grad |
| 2005/06 | pol_second | Zawisza Bydgoszcz, Heko Czermno, Drwęca Nowe Miasto Lubawskie |
| 2007/08 | gre_second | Kerkyra |
| 2009/10 | cro_a2a | Rudeš |
| 2009/10, 2010/11 | gre_second | Panetolikos |
| 2010/11 | cro_a2a | Dugopolje |
| 2011/12 | gre_second | Iraklis Psachna |
| 2013/14 | gre_second, group 2 | Vataniakos; Panegialios, Episkopi |
| 2014/15 | gre_prm | OFI |
| 2014/15 | group 2 | Enosi Ermionida, Alimos |
| 2015/16 to 2017/18 | cro_a2a | Dinamo Zagreb II (and Hajduk Split II in 2017/18) |
| 2016/17 | gre_prm | Kerkyra |
| 2016/17, 2017/18 | gre_second | Aiginiakos |
| 2020/21 | gre_second | O.F. Ierapetra |
| 2021/22 to 2025/26 | gre_second and group 2 | Olympiacos B (four seasons), Aiolikos, Tilikratis, Asteras Tripolis B (two seasons), Hellas Syros |

Other points for Max:

- 145 matched clubs come from the nation's "Lower Division" record, not from clubs with no division. Clubs with no division exist only in some databases: Poland 23 to 41 in November 2007 to September 2012, Croatia 3 to 7 in November 2007 to September 2011, Greece 0 or 1. In other seasons Q244 cannot be met as written.
- Short divisions: the database has 16 Croatian first-division clubs against 12 or 10 real clubs from 2002/03 (except 2009/10 to 2011/12), and 14 Greek first-division clubs against 16 or 18 real clubs from 2007/08 to 2018/19. The clubs file lists the real clubs. The surplus database clubs need a division.
- Greek tier 2 in 2021/22 to 2025/26 has 20 to 34 real clubs against 16 in the database, and B teams that the database lacks.
- Club matches are by name. A reviewer should check the pairs with a changed club name (for example Dospel Katowice to GKS Katowice, GKS Bogdanka to Górnik Łęczna, Sesvete to NK Radnik Sesvete).

## Unknowns

- Greek, Polish and Croatian bench sizes in nearly all seasons.
- Foreign-player rules for Greece 2001/02 to 2006/07 and 2009/10 to 2019/20, Poland 2001/02 to 2006/07, Croatia 2001/02 to 2006/07, 2008/09 to 2014/15, 2017/18 to 2019/20 and 2021/22 to 2024/25. The original game's values (Greece 4 non-EU, Poland 5, Croatia 4) come from the repo doc `docs/research/2026-10-16-season-change-points.md`, not from a web source.
- Work permits and squad registration: no source found, except the Greek non-EU declaration dates (2020/21 to 2022/23).
- National transfer window dates, except Greece 2025/26 and Poland (Ekstraklasa) 2023/24 winter to 2025/26.
- Cup entrant counts for the Polish Cup except 2014/15 (86) and 2015/16 (68).
- Planned relegation counts for several seasons with demotions or withdrawals (notes give the real outcome).
- The year in which the Greek over-23 non-EU cap moved from 5 to 6.

## Reconcile

Reconcile date 2026-10-16. The full list is in `gre-pol-cro.reconcile.tsv` (53 rows: 28 disputed fields, 19 found values, 5 follow-on fields and 1 removed claim). I changed 44 rows of `gre-pol-cro.tsv`, added each new URL to `sources` and kept row order and ids. Third sources: Kathimerini, Ta Nea, Sportowe Fakty, 90minut.pl, the Ekstraklasa SA site, the FIFA RSTP of March 2023, the Druga HNL competition rules of 2017/18 (archived PDF) and Polish and Croatian Wikipedia. Polish and Croatian Wikipedia are not fully separate publishers from English Wikipedia. I used them only where I found no other text.

| Decision | Fields |
|---|---|
| first | 3 (gre-pol-cro-6 format, 254 down, 368 playoffs) |
| second | 22 (15 `windows` fields, 166 and 167 format, 197 playoffs, 249 up and playoffs, 366 and 367 down) |
| conflict | 3 (66 down, 160 foreign, 361 down) |
| found value accepted | 11 (65, 72, 73, 75, 77, 129, 131, 195, 196, 249, 369, all `down`) |
| found value changed to the planned value | 4 (74, 76, 130, 362 `down`) |
| found value rejected | 4 (250 up, 359 up, 360 up, 363 down) |
| follow-on | 5 (195, 196, 251 playoffs; 250 down and playoffs) |
| claim removed | 1 (459 format) |

How I decided:

- Greek Cup 2006/07 (6): Kathimerini of 16 November 2006 says the round of 16 is one match with a replay only after a draw. RSSSF lists the replays as second legs.
- Transfer windows (158 to 162, 320 to 324, 482 to 486): article 6.2 of the RSTP March 2023 edition sets 8 to 12 weeks and 4 to 8 weeks, 16 weeks together. I did not read a later edition. Where the national body published the dates, the row gives them. Greece 2025/26: HFF executive committee decision reported by Ta Nea (9 April 2025), summer 1 July to 12 September 2025 and winter 2 January to 6 February 2026. Poland: Ekstraklasa SA pages, winter 2024 29 January to 27 February, summer 2024 1 July to 6 September, winter 2025 28 January to 24 February, summer 2025 1 July to 8 September, winter 2026 26 January to 25 February. I found no HFF dates for other seasons and no HNS dates. Croatian press dates for 2025 and 2026 disagree with each other, so I did not use them.
- Polish Cup 2004/05 and 2005/06 (166, 167): 90minut.pl shows the group phase of 2004/05 and the two-legged rounds of both seasons. The Wikipedia season pages agree with this. The first value came from the general cup page.
- Ekstraklasa 2008/09 (197, 251): the 14th was to play the I liga 3rd. Sportowe Fakty (12 June 2009) reports that PZPN cancelled it.
- II liga 2006/07 and 2007/08 (249, 250) and the top level of the same seasons (195, 196): Polish Wikipedia gives the plan of each season (1st and 2nd up, 3rd v top-level 14th, last four down). RSSSF and a 90minut.pl report (5 June 2007) show that the corruption demotions changed the outcome. The rows now hold the plan and the notes the outcome.
- I liga 2011/12 (254): the rule was four down. Only three went down because Ruch Radzionkow withdrew.
- Druga HNL 2015/16 and 2016/17 (366, 367): Croatian Wikipedia says the last two go down, and both RSSSF and the English infobox show 11th and 12th relegated.
- Druga HNL 2017/18 (368): the competition rules (articles 38 and 39) give 2nd v HNL 9th, B teams skipped, and 11th v the 3. HNL South runner-up. RSSSF shows the outcome (3rd Varazdin in the play-off, no relegation play-off).
- Found values: I accepted a value where the RSSSF page states the rule or draws the relegation line. Where the value was only the outcome, I wrote the planned value from the same page or kept `unknown`, with the outcome in `notes`.
- Croatian Super Cup 2027/28 (459): I removed "(real: not played this season)". The result depends on 2026/27.

Conflicts for Max:

- gre-pol-cro-66 (Football League 2012/13, down). First value 3 (bottom three), from the prose of the season page. Second value 2: RSSSF and the table key of the same page mark only 20th and 21st, and 19th Pierikos stayed. The league proclamation (epae.org, article 8) has no 2012/13 copy in the web archive. The tier went to two groups in 2013/14, which may explain the change. The value stays 3.
- gre-pol-cro-160 (Greece 2025/26, foreign). First value: at most 5 non-EU on the field, squad cap of 6 non-EU over 23 (Insideworldfootball, July 2026, which says the cap rose from six to seven). Second value: 7 non-EU per squad (Sportmonks, 17 December 2025, no age split). I found no HFF text for 2025/26. The value stays as first.
- gre-pol-cro-361 (Druga HNL 2010/11, down). First value 4 (13th-16th): the season page cites the CFF Assembly of 17 December 2010. Second value 1 (16th): RSSSF quotes a licence rule that only 15th or better may remain. In the end 15th was expelled and 16th dissolved. The value stays 4.

Club matches (`gre-pol-cro-clubs.tsv`):

- 2003/04 cro_a2b, Pula 1856: now 12006 "NK Uljanik". The club history on Wikipedia gives NK Uljanik, then NK Pula 1856, Pula Staro Cesko and Istra 1961. RSSSF 2002/03 lists both Istra Pula and Uljanik Pula, and RSSSF 2003/04 lists Pula 1856 as the promoted group winner. In the October 2003 database 12006 sits in the Croatian First Division, so it is not a club with no division (Q244).
- 2014/15 gre_second, Serres: now 3098 "Ethnikos Gazoros". RSSSF 2014/15: "Ethnikos Gazoros were renamed Serres FC". The English season page links the club as Ethnikos Gazoros.
- 2012/13 gre_second, Ethnikos Gazoros: now 13274 "E.S Ethnikos Gazorou Serres" (September 2012 database). The script had matched it to Ethnikos Patron (3694). I found this one while checking Serres.

Other points:

- gre-pol-cro-452 (Croatian Super Cup 2020/21) keeps "(real: not played this season)". The check found that the cause was no free date, not a double. The row is correct on the outcome.
- The Croatian under-21 rule from 2026/27 (check file) is not in rows 485 and 486. It was not a disagreement, so I left it.

## Source fill

Date: 2026-10-16. A source-fill pass added `field=URL` entries to `sources` for filled fact fields that had none. It changed no values and kept row order and ids. Each URL was read on this date, and the value was checked against the page text.

Method:

- For `name`, the pass took the pages that the row already cites, season pages first, and kept the first page that contains the name. For a name with a note in brackets, the base name had to match. For an Italian group (girone), the page also had to name that group.
- For a value of `none` in `split` or `playoffs`, the source is the season page with one league stage and no championship or relegation round.

Fields filled:

- `name`: 148 rows. The not-held Polish League Cup rows (2002/03 to 2005/06, 2009/10 to 2027/28) cite the Ekstraklasa Cup page, which names the Polish League Cup. The Druga HNL South rows from 2006/07 cite the First Football League (Croatia) page, which names the southern second league.
- `clubs` = 2 for the super cups: 19 rows (Greek and Croatian Super Cup pages, infobox "Teams 2").
- `split` = `none`: 5 rows.
- `format` and `split` of gre-pol-cro-261 (I liga 2018/19): the Polish Wikipedia season page ("34 kolejki ligowe", two halves, no split).

No source found for `name` in 19 rows: gre-pol-cro-109 to gre-pol-cro-120, gre-pol-cro-123 to gre-pol-cro-128 and gre-pol-cro-134. The value "second group of the Greek second tier" is a label for a not-held group, and no page uses it. Each row has "no source found for name" in `notes`.

Disagreements: none found.

Lint after the pass: 19 problems (the 19 names above).
