# Rules table: Holland and Belgium (slice 6, part hol-bel)

Date 2026-10-16. Research only. Brief: `docs/research/research-brief-0.6.0.md`.

Files:

- `hol-bel.tsv`: 351 rows, 13 competitions x 27 seasons (2001/02 to 2027/28).
- `hol-bel-clubs.tsv`: 2,158 club rows for every real division whose club count or structure differs from its season database.
- This file: method, decisions, missing clubs and unknowns.

## Scope

| comp | comp_file | Rows |
|---|---|---|
| nation rules (Holland) | n/a | 27 |
| Eredivisie | hol_prm | 27 |
| Eerste Divisie | hol_first | 27 |
| Dutch third tier: Hoofdklasse to 2009/10, Topklasse 2010/11 to 2015/16, Tweede Divisie from 2016/17 | new | 27 |
| KNVB Cup | hol_cup | 27 |
| Johan Cruyff Shield | hol_super | 27 |
| nation rules (Belgium) | n/a | 27 |
| Belgian top flight | bel_first | 27 |
| Belgian second tier | bel_second | 27 |
| Belgian third tier | bel_third | 27 |
| Belgian fourth tier | unknown | 27 |
| Belgian Cup | bel_fa_cup | 27 |
| Belgian Super Cup | bel_super | 27 |

- The databases hold four Belgian tiers (data comps 0 to 3 and 429 to 432) but the function map has no `bel_fourth` file. I could not tell which file runs the fourth-tier divisions, so `comp_file` is `unknown` for those rows. Slice 1 can settle it.
- The Belgian fifth tier from 2016/17 (Third Amateur Division, Division 3) is out of scope. The original has four Belgian tiers.
- The Dutch Hoofdklasse before 2010/11 had no promotion to the Eerste Divisie. Its rows say so. I listed no clubs for it.

## Method

- Wikipedia season pages, raw wikitext read on 2026-10-16. The league tables (`Sports table` module) gave club counts, outcomes and club lists. Lead and format sections gave the rules.
- Dutch-language Wikipedia gave the Belgian second tier to 2008/09, the third tier to 2015/16, the fourth tier to 2015/16, the 2017/18 Second Amateur Division and the 2016/17 to 2019/20 top-flight club lists. English pages hold these tables only in part or not at all.
- Substitutes and bench: KNVB regulations (2020 decision, 2025/26 regulations), the Eredivisie news page, Belgian news pages (seneweb 2020, Sporza 2022), the Wikipedia article on substitutes for the three-substitute rule of 1995, and Super Cup match rules.
- Nation rules: IND, Everaert, VI and Voetbalprimeur for the Dutch salary criterion; Inside World Football for the Belgian homegrown quota; a 2020 Belgian Chamber bill for the Belgian non-EU minimum salary; the Wikipedia transfer window article.
- Club moves: the database of each season start is the one in the spec table. October 2003 is for 2003/04, September 2005 for 2005/06, and so on. 2001/02 and 2002/03 use 3.9.68. 2004/05, 2006/07, 2026/27 and 2027/28 have no database and no club list.
- Club names were matched to database clubs by a normalised name comparison and an alias list. I checked each match below exact by hand. A club matches only inside its own nation.

## Decisions to check (inference)

1. **Spare club pool.** Q244 says to take clubs "that have no division". I counted clubs in division -1 and in "A Lower Division" (357, 433) as the pool. "A Lower Division" is a placeholder division with no league. Pool sizes: Holland 105 to 173 clubs, Belgium 12 to 118 clubs, by database.
2. **Belgian reform of 2016.** The databases keep the old layout: First 18, Second 18, Third A and B 16 each, Fourth A to D 16 each. The real pyramid from 2016/17 has First Division A 16, First Division B 8, First Amateur Division 16 and Second Amateur Division 3 x 16. The club list maps each real division to a suggested database division in `db_division` of the real club rows: tier 3 to "Belgian Third Division A", tier 4 groups to Fourth A, B and C. "Belgian Third Division B" and "Belgian Fourth Division D" then stay empty. From 2024/25 Division 1 has two groups (VV 16, ACFF 12). I mapped them to Third A and Third B. Max decides the mapping.
3. **Belgian top flight 16 clubs.** In the 16-club seasons (2009/10 to 2019/20, 2023/24 to 2025/26) the database still has 18 clubs in the top flight. Two clubs must move down. The club list shows which.
4. **Jong teams.** The databases name Dutch reserve teams in several ways ("Ajax Amsterdam U21", "Jong AFC Ajax", "PSV Eindhoven U21"). Belgian U23 teams appear as "KAA Gent B", "KRC Genk B", "Club Brugge KV II", "RSC Anderlecht B". I matched them by alias. "Jong Westerlo" (2024/25) matched "Jong Westel", which I take to be the same club with a cut name.
5. **Renamed clubs.** Some database records changed name across databases (for example id 7938 is "FC Nieuwkerken Sint-Niklaas", then "Sportkring Sint-Niklaas"). I matched by the name in that season's database. KRC Harelbeke (2016) matched "SW Harelbeke" or "Racing Harelbeke". RWDM47 matched "RWD Molenbeek (2015)" or "RWDM Brussels". Treat these as inference.
6. **Bench 12 in Dutch professional football** is sourced for 2016/17, 2017/18 (Shield rules), 2019/20 to 2020/21 (KNVB 2020) and 2025/26 (KNVB regulations). 2018/19 and 2021/22 to 2024/25 are inference from the seasons around them, and the rows say so.
7. **Substitutes before 2020.** "3" rests on the Laws of the Game (three since 1995). I found no league rule that sets a lower number.

## Missing clubs (for Max)

Real clubs with no record in that season's database. All other real clubs have a record, often in another division.

| Season | Division | Clubs with no record |
|---|---|---|
| 2010/11 | Topklasse Sunday | VVSB |
| 2011/12 | Topklasse Saturday | SVZW, Montfoort |
| 2011/12 | Topklasse Sunday | WKE, VVSB |
| 2012/13 | Topklasse Sunday | WKE, VVSB |
| 2013/14 | Topklasse Sunday | VVSB, Leonidas, WKE |
| 2014/15 | Topklasse Saturday | Ajax (amateurs) |
| 2014/15 | Topklasse Sunday | VVSB, WKE, Hercules, EDO |
| 2015/16 | Topklasse Sunday | VVSB, WKE |
| 2016/17 | Tweede Divisie | VVSB |
| 2005/06 | Belgian Fourth Division A | VG Oostende, RRC Péruwelz |
| 2009/10 | Belgian Fourth Division A | Football Couillet-La Louvière |
| 2012/13 | Belgian Fourth Division B | RUS Genly-Quévy 89, RSC Templeuvois |
| 2012/13 | Belgian Fourth Division C | Merksem-Antwerpen Noord SC, K. Ternesse VV Wommelgem |
| 2016/17 | Second Amateur Division ACFF | Waremme |
| 2017/18 | Second Amateur Division VV B | SC City Pirates Antwerpen |
| 2018/19 | Second Amateur Division VV B | City Pirates, Heur-Tongeren |
| 2023/24 | Division 2 ACFF | Rochefort |
| 2024/25 | Division 2 VV B | Jong STVV |

- Several of these clubs exist in other databases (VVSB as "VVSB Noordwijkerhout", Leonidas, Hercules, Templeuvois, Genly-Quévy, Ternesse, VG Oostende). So a fix by data is possible, but no club records may be added without Max (Q244).
- The Topklasse and Tweede Divisie are new divisions. The database has no Dutch third tier, so every club in those lists must come from the pool.

## Database oddities seen

- October 2003 puts Excelsior in the Dutch Premier Division. Real 2003/04: Eerste Divisie.
- October 2018 has KSK Tongeren with no nation (-1). November 2020 has the same id as "KFC Heur Tongeren".
- The Belgian fourth-tier counts in November 2020 to December 2025 vary (13 to 26 per group). They do not follow the real 16 or 18.

## Unknowns

- Dutch foreign player quota: no source found. Non-EU players need a work permit with a salary criterion (150% of the average Eredivisie salary). Amounts found for 2018/19, 2023/24, 2024/25 and 2025/26 only. Start year of the criterion unknown.
- Belgian non-EU minimum salary: only the 2020 figure (EUR 84,896). Belgian squad registration before 2011: unknown.
- Bench size: Dutch professional football before 2016/17; all Belgian leagues and cups; Dutch and Belgian amateur tiers.
- Transfer window dates: only Netherlands 2025/26 and Belgium 2023/24 (summer). Other seasons say "two FIFA windows from 2002/03". 2001/02 unknown.
- KNVB Cup entrants for 2011/12, 2014/15, 2015/16, 2025/26 and 2026/27.
- Hoofdklasse club counts before 2006/07 (no page fetched).
- Belgian third tier 2023/24 relegation; Belgian fourth tier 2019/20 relegations.
- 2027/28: no season pages. Q248 keeps the 2026/27 rules. Belgium: the 2026/27 page reports that the big clubs want 14 or fewer clubs in the long term, with no decision.

## Points for the build slices

- Eredivisie: 18 clubs every season. Direct relegation 1 to 2018/19, 2 from 2020/21. 2019/20 abandoned, no relegation.
- Promotion play-offs: groups of four to 2004/05; a 10-club knockout 2005/06 to 2018/19; 16th plus six Eerste Divisie clubs from 2020/21. The original's `hol_first` already builds the 2001 group format. `HolFirst::create_play_off_groups` (`0x5f6720`, called by `HolFirst::advance_stage` at stage 4) tops the period winners up to six Eerste Divisie clubs from the table. It adds the two Eredivisie clubs with play-off status 2 and builds two groups of four. Stage 4 is comp code 0x418 and stage 5 is 0x419, with 6 rounds each (`HolFirst::create_schedule`, `0x5f5790`). An earlier version of this note said "groups of eight". That came from a wrong first-pass function name.
- Eerste Divisie: 18, 19 or 20 clubs by season. Jong teams from 2013/14. No relegation to 2008/09. From 2017/18, no relegation to the Tweede Divisie for first teams.
- Belgian top flight: 18 to 2008/09, 16 with play-offs 2009/10 to 2019/20, 18 in 2020/21 to 2022/23, 16 in 2023/24 to 2025/26, 18 with no play-offs from 2026/27 (announced February 2025 and confirmed on the 2026/27 page).
- Belgian second tier: 18 or 19 clubs to 2014/15, 17 in 2015/16, 8 in 2016/17 to 2021/22 (two tournaments to 2019/20), 12 in 2022/23 with U23 teams and a split, 16 in 2023/24 and 2024/25, 17 in 2025/26, 15 in 2026/27.
- Belgian Cup: two-legged quarter-finals 2002/03 to 2014/15; two-legged semi-finals every season except 2020/21.
- Five substitutes: Netherlands from 2020/21; Belgium from 1 January 2021.

## Reconcile

Date: 2026-10-16. File: `hol-bel.reconcile.tsv`. Third sources are transfermarkt (KNVB Cup round lists of every season, Belgian second-tier club lists), NOS (2017 Johan Cruyff Shield), Sporza (2021/22 First Division B), the Pro League communication of June 2020 (through web.archive.org), KNVB news (summer window 2025) and the KNVB regulations 2025/26. The `up` and `down` decisions use the brief rule: places by the rules of the season, with actual outcomes in `notes`.

| Decision | Fields |
|---|---|
| first | 2 |
| second | 10 |
| conflict | 2 |
| found accepted | 11 |
| found rejected | 1 |

### KNVB Cup: one method

Every KNVB Cup row now counts the clubs of the main draw. The main draw starts at the round that the KNVB calls the first round. The amateur qualifying rounds (from 2016/17, plus one match in 2005/06) are in `format`, with the total of clubs in all rounds. Clubs in Europe that enter in the second round count in the main draw. Each row cites the transfermarkt round list of its season.

| Season | Main draw | Season | Main draw |
|---|---|---|---|
| 2001/02 | 86 | 2014/15 | 85 |
| 2002/03 | 86 | 2015/16 | 85 |
| 2003/04 | 88 | 2016/17 | 64 |
| 2004/05 | 85 | 2017/18 | 64 |
| 2005/06 | 86 | 2018/19 | 64 |
| 2006/07 | 88 | 2019/20 | 60 |
| 2007/08 | 87 | 2020/21 | 60 |
| 2008/09 | 88 | 2021/22 | 59 |
| 2009/10 | 88 | 2022/23 | 59 |
| 2010/11 | 93 | 2023/24 | 59 |
| 2011/12 | 92 | 2024/25 | 58 |
| 2012/13 | 92 | 2025/26 | 58 |
| 2013/14 | 82 | 2026/27 | 58 |

- Before 2016/17 the KNVB Cup had no qualifying rounds, except one match in 2005/06 (AFC v Be Quick '28). Amateur clubs qualified through the district cups and their league places. Their first round, which held only amateur clubs, is part of the main draw. The `format` text says where the professional clubs enter.
- Changed counts: 2005/06 87 to 86, 2010/11 88 to 93, 2016/17 63 to 64, 2019/20 64 to 60, 2020/21 64 to 60, 2022/23 46 to 59, 2023/24 110 to 59, 2024/25 110 to 58. 2011/12, 2014/15, 2015/16, 2025/26 and 2026/27 were unknown and now have values.
- 2019/20 and 2020/21: Wikipedia gives 64 in the infobox, but the first round had 28 matches and 4 clubs had a bye to the second round. The count is 60.
- 2027/28 stays unknown.

### Other changes to hol-bel.tsv

- hol-bel-86 (2007/08 Belgian top flight): `down` 2 (17th, 18th), `playoffs` none.
- hol-bel-87 (2007/08 Belgian second tier, 19 clubs): `down` 2 (18th, 19th), 17th to the third division play-off. `playoffs`: runner-up and three period winners, no top-flight club.
- hol-bel-100 (2008/09 Belgian second tier, 19 clubs): `down` 2 (18th, 19th), 15th to 17th to the third division final round. `playoffs`: two-legged qualifying ties, then a group of four with the top-flight 15th and 16th.
- 12 other second-tier rows (hol-bel-9 to hol-bel-178) copy the play-off text. Their phrase "the top-flight 17th (2005/06 to 2008/09)" now says 2005/06 and 2006/07 only.
- hol-bel-108 (2009/10 Hoofdklasse): `up` 24 direct places. `playoffs` now holds the seven play-off places (31 clubs up in all).
- hol-bel-214 (2017 Johan Cruyff Shield): value kept. `notes` records that RSSSF and a later KNVB article say extra time was played.
- hol-bel-256 and hol-bel-269: `down` kept at 1. `notes` give the outcomes.
- hol-bel-313: summer window 2025 now 23 June to 2 September (KNVB statement).
- Found values written: Hoofdklasse 2001/02 to 2005/06 `clubs` 84, Netherlands 2007/08 and 2008/09 `foreign`.
- `hol-bel-clubs.tsv`: no change. No decision touched a club list.

### Conflicts for Max

1. **hol-bel-256, 2020/21 Belgian First Division B, `down`.** First value 1 (8th). Second value: no relegation. Only Wikipedia states the rule that the bottom club goes down. The Pro League text of June 2020 sets the 8-club format but gives no number of relegation places. Club NXT finished 8th and, as a U23 team, could not go down. The amateur leagues were abandoned, so no club went down. Proposal: keep 1, with the outcome in notes (done).
2. **hol-bel-313, Netherlands 2025/26, `windows`.** Three opening dates: 11 June (first agent, cited to the KNVB regulations, which give no dates), 16 June (Voetbalprimeur, 3 June 2025) and 23 June (KNVB news, 3 September 2025). The close date, 2 September 23:59, is settled. Proposal: 23 June, the KNVB statement (now in the TSV).

### Other findings

- hol-bel-329 (2026/27 Tweede Divisie `subs_used`) stays unknown. The KNVB page gives 5 substitutes in 4 moments for category A, but it does not name the Tweede Divisie.
- hol-bel-313 winter window "3 to 31 January" has no source that states it. KNVB news says the winter 2026 window closed on 3 February 23:59 (https://www.knvb.nl/node/71461, found in a search summary, not read raw). Not changed.
- The 2001/02 and 2002/03 transfermarkt round lists hold only 46 clubs. They lack the group stage. The counts of 86 rest on Wikipedia only.

## Loose ends

- hol-bel-313 (Netherlands 2025/26) `windows`: the winter window is now 2 January to 3 February 2026 23:59. The KNVB news of 4 February 2026 says clubs could sign players until 3 February 23:59 (https://www.knvb.nl/node/71461, read raw today). Voetbalprimeur gives the opening date 2 January 2026 (https://www.voetbalprimeur.nl/nieuws/1706663/wanneer-opent-en-sluit-de-winterse-transfermarkt.html). It gives the close as "dinsdag 2 februari", but 2 February 2026 was a Monday and 3 February a Tuesday. I kept the KNVB close date. The opening date has one source only.

## Source fill

Date: 2026-10-16. A source-fill pass added `field=URL` entries to `sources` for filled fact fields that had none. It changed no values and kept row order and ids. Each URL was read on this date, and the value was checked against the page text.

Method:

- For `name`, the pass took the pages that the row already cites, season pages first, and kept the first page that contains the name. For a name with a note in brackets, the base name had to match. For an Italian group (girone), the page also had to name that group.
- For a value of `none` in `split` or `playoffs`, the source is the season page with one league stage and no championship or relegation round.

Fields filled: 297 `name`.

- Rows hol-bel-329 and hol-bel-340 to hol-bel-351 (2026/27 and 2027/28 with no season page) and hol-bel-193 (2015/16 fourth tier) cite the main competition page. That page gives the current name.
- hol-bel-219 (2017/18) cites the English season page of the Belgian Second Amateur Division. The cited Dutch page uses the Dutch name.

Disagreements: none found.

Lint after the pass: 0 problems.
