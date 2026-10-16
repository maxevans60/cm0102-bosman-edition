# Holland and Belgium: second-source check for 0.6.0 (part `hol-bel`)

Date: 2026-10-16. This file checks `hol-bel.tsv`, `hol-bel-clubs.tsv` and `hol-bel.md`. I did not change those files. The result is `hol-bel.check.tsv`, with 1,946 rows: one for each filled fact field of the 351 rows.

## Method

- Every filled fact field got a verdict. I skipped fields with `n/a`. A field that the first agent marked `unknown` gets `found` when I found the value, and `no second source` when I did not.
- The first sources are Wikipedia (English and Dutch), plus knvb.nl, sporza.be, insideworldfootball.com, vi.nl, seneweb.com, everaert.nl, dekamer.be, voetbalprimeur.nl and ind.nl. No second source is a Wikipedia edition. A script compared the publisher of each second URL with the first URL of that fact. When they matched, the verdict became `no second source` (4 fields, all KNVB 2019/20 bench and substitute facts).
- League tables: RSSSF season pages, all read raw on 2026-10-16. Netherlands `tablesn/ned02.html` to `ned2026.html`, Belgium `tablesb/belg02.html` to `belg2026.html`. I checked club counts, direct places, play-off places, splits and names from the final tables and their notes.
- Dutch amateur third tier: RSSSF amateur pages `ned-amat02.html` to `ned-amat2010.html` for the Hoofdklasse. StatsCrew season pages (`l-NETDZA`, `l-NETDZO`, `l-NETTWE`) for the Topklasse club counts 2010/11 to 2015/16 and the Tweede Divisie 2016/17 to 2023/24.
- Cups: RSSSF season pages (KNVB Beker from 2007/08, Belgian Cup from 2005/06), the RSSSF lists of cup finals and super cup finals. I counted KNVB Cup entrants with a script that collects every club name in the cup section. Spelling variants can add one or two names, and the notes say where I found a duplicate.
- Rules: the KNVB substitute decision of 29 July 2020 (it quotes the earlier rule), NOS (2 February 2021), eredivisie.nl, DH (dhnet.be, December 2020, January 2023, August 2023, July 2024), Sporza (2023, 2025, 2026), goal.com (transfer windows), Voetbalprimeur (2025 window), Flashscore (non-EU salary), Bleacher Report (2008, foreign players) and the KNVB invallersbepalingen page for 2026/27.
- Rows for 2026/27 and 2027/28 have no finished season page. Most of their fields are `no second source`. The exceptions are the 2026 Johan Cruyff Shield and Belgian Super Cup, which RSSSF lists, and the 2026/27 Belgian top flight, which Sporza describes.

## Counts

| Verdict | Rows |
|---|---|
| agree | 949 |
| no second source | 971 |
| disagree | 14 |
| found | 12 |

Most `no second source` rows are rule fields: bench (286) and substitutes (277) in nearly every competition, the Dutch foreign quota, work permits and squad registration. The others are the 2026/27 and 2027/28 rows, the Topklasse 2012/13 to 2015/16 outcomes, the Tweede Divisie 2019/20 and 2020/21 outcomes, the Belgian third and fourth tiers from 2010/11 to 2015/16 and from 2020/21 (no RSSSF table), and the cup formats before 2007/08 (KNVB Cup) and before 2005/06 (Belgian Cup).

## Disagreements

| Row | Field | First value | Second value | First URL | Second URL |
|---|---|---|---|---|---|
| hol-bel-86 (2007/08 Belgian top flight) | down | 1 (18th); 17th to the second division final round | 2: 17th Sint-Truiden and 18th FC Brussels both relegated directly | https://en.wikipedia.org/wiki/2007%E2%80%9308_Belgian_First_Division | https://www.rsssf.org/tablesb/belg08.html |
| hol-bel-86 | playoffs | 17th plays the second division final round | no top-flight club in the 2008 final round. RSSSF notes that the 17th took part only in 2005/06 and 2006/07 | same | https://www.rsssf.org/tablesb/belg08.html |
| hol-bel-87 (2007/08 Belgian second tier) | down | 2; 16th to the third division play-off | 19 clubs: 18th and 19th relegated, 17th to the play-off | https://nl.wikipedia.org/wiki/Tweede_klasse_2007-08_%28voetbal_Belgi%C3%AB%29 | https://www.rsssf.org/tablesb/belg08.html |
| hol-bel-87 | playoffs | final round with the top-flight 17th (2005/06 to 2008/09) | final round of the runner-up and three period winners, no top-flight club | same | https://www.rsssf.org/tablesb/belg08.html |
| hol-bel-100 (2008/09 Belgian second tier) | down | 2; 16th to the third division play-off | 19 clubs: 18th and 19th relegated, 15th to 17th to the play-off | https://nl.wikipedia.org/wiki/Tweede_klasse_2008-09_%28voetbal_Belgi%C3%AB%29 | https://www.rsssf.org/tablesb/belg09.html |
| hol-bel-100 | playoffs | final round with the top-flight 17th | group of four: top-flight 15th and 16th plus two second-tier qualifiers | same | https://www.rsssf.org/tablesb/belg09.html |
| hol-bel-108 (2009/10 Hoofdklasse) | up | top four of each group (24) plus six play-off winners | 31 clubs marked promoted: 24 plus 7 via play-offs (DTS '35 and Excelsior Maassluis both went up because ARC was promoted) | https://en.wikipedia.org/wiki/2009%E2%80%9310_Hoofdklasse | https://www.rsssf.org/tablesn/ned-amat2010.html |
| hol-bel-122 (2010/11 KNVB Cup) | format | 88 entrants | 93 clubs named in all rounds | https://en.wikipedia.org/wiki/2010%E2%80%9311_KNVB_Cup | https://www.rsssf.org/tablesn/ned2011.html |
| hol-bel-200 (2016/17 KNVB Cup) | format | 63 entrants | 64 clubs from round 1 (about 103 with the amateur qualifying rounds) | https://en.wikipedia.org/wiki/2016%E2%80%9317_KNVB_Cup | https://www.rsssf.org/tablesn/ned2017.html |
| hol-bel-214 (2017/18 Johan Cruyff Shield) | format | 90 minutes then penalties | 2017 match Feyenoord v Vitesse 1-1 after extra time, 4-2 on penalties | https://en.wikipedia.org/wiki/2017_Johan_Cruyff_Shield | https://www.rsssf.org/tablesn/nedsupcuphist.html |
| hol-bel-256 (2020/21 First Division B) | down | 1 (8th) | no relegation, as the amateur leagues were abandoned | https://en.wikipedia.org/wiki/2020%E2%80%9321_Belgian_First_Division_B | https://www.rsssf.org/tablesb/belg2021.html |
| hol-bel-269 (2021/22 First Division B) | down | 1 (8th) | no club marked relegated. 8th Virton stayed and Mouscron folded after the season | https://en.wikipedia.org/wiki/2021%E2%80%9322_Belgian_First_Division_B | https://www.rsssf.org/tablesb/belg2022.html |
| hol-bel-278 (2022/23 KNVB Cup) | format | 46 entrants | 59 clubs from round 1, about 86 with the qualifying rounds | https://en.wikipedia.org/wiki/2022%E2%80%9323_KNVB_Cup | https://www.rsssf.org/tablesn/ned2023.html |
| hol-bel-313 (2025/26 Netherlands rules) | windows | 11 June to 2 September and 3 to 31 January | summer window 16 June to 1 September 2025 (article of 3 June 2025; the deadline later moved to 2 September) | https://www.knvb.nl/downloads/bestand/29888/reglementen-betaald-voetbal-seizoen-2025-26versie-januari-2026 | https://www.voetbalprimeur.nl/nieuws/1291478/wanneer-begint-en-eindigt-de-zomerse-transferperiode.html |

Notes on the disagreements:

- The KNVB Cup entrant counts in the first file use two methods. Some seasons count all clubs including the amateur qualifying rounds (2009/10, 2012/13, 2013/14, 2023/24, 2024/25). Others count the main draw only (2016/17 to 2021/22). The build should use one method. The RSSSF counts for both are in the TSV notes.
- The 2025/26 window conflict is between the KNVB regulations (first source) and a news article written before the season. The KNVB text is the stronger source. I list it because the two values differ.

## Found values

| Row | Field | Value | URL |
|---|---|---|---|
| hol-bel-4, 17, 30, 43, 56 (Hoofdklasse 2001/02 to 2005/06) | clubs | 84: six groups of 14 (Saturday A to C, Sunday A to C) | https://www.rsssf.org/tablesn/ned-amat02.html to ned-amat06.html |
| hol-bel-79, 92 (Netherlands 2007/08, 2008/09) | foreign | no limit on foreign players as of 2008 | https://bleacherreport.com/articles/68191-foreigners-in-football |
| hol-bel-135 (2011/12 KNVB Cup) | format | 92 entrants | https://www.rsssf.org/tablesn/ned2012.html |
| hol-bel-174 (2014/15 KNVB Cup) | format | 85 entrants | https://www.rsssf.org/tablesn/ned2015.html |
| hol-bel-187 (2015/16 KNVB Cup) | format | 85 entrants | https://www.rsssf.org/tablesn/ned2016.html |
| hol-bel-317 (2025/26 KNVB Cup) | format | about 110 entrants | https://www.rsssf.org/tablesn/ned2026.html |
| hol-bel-329 (2026/27 Tweede Divisie) | subs_used | 5 in 4 moments (3 in play, 1 at half-time), adult category A (inference that this covers the Tweede Divisie) | https://www.knvb.nl/node/48989 |

Values still not found:

- A Dutch foreign-player quota for any season other than 2007/08 and 2008/09. No source names a quota. Searches found only the non-EU salary criterion.
- Belgian bench sizes for any league or cup. No source found.

## Agreements with a caveat

These are `agree` in the TSV. The note in the TSV gives the detail.

- Club counts where a club withdrew or went bankrupt during the season. The first file gives the count at the start, and RSSSF confirms it: 2002/03 Belgian top flight (Lommel), 2009/10 Belgian top flight (Mouscron), 2009/10 and 2012/13 Eerste Divisie, 2005/06 and 2024/25 Belgian second tier, 2010/11 Topklasse Sunday (Dijkse Boys).
- Belgian second tier 2024/25 and 2025/26, `down` = 2. RSSSF shows only one club below the line, because U23 teams cannot go down. Sporza (5 August 2025) confirms that 16th and 17th go down in the normal case.
- Netherlands 2025/26 work permit: Flashscore gives EUR 600,000, a rounded figure. The first file gives EUR 608,000.
- Transfer windows "two FIFA windows from 2002/03": goal.com confirms the general start date. It is not specific to the Netherlands or Belgium.
- Belgian squad registration 2022/23 to 2024/25: DH confirms six players trained in Belgium on the match sheet, and Sporza confirms eight in the squad. No second source gives the start year 2011.

## Club list spot checks (`hol-bel-clubs.tsv`)

I compared the club lists of one season per division with the RSSSF final tables (StatsCrew for one Topklasse season). Differences in spelling only (for example "Moeskroen" and "R. Excel Mouscron") count as a match.

| Season | Division | Result |
|---|---|---|
| 2003/04 | Eerste Divisie | 19 of 19 match |
| 2022/23 | Eerste Divisie | 20 of 20 match |
| 2010/11 | Topklasse Sunday | 16 match; Dijkse Boys withdrew (RSSSF note) |
| 2011/12 | Topklasse Saturday | 16 of 16 match |
| 2013/14 | Topklasse Saturday | 16 of 16 match (StatsCrew) |
| 2017/18 | Tweede Divisie | 18 of 18 match |
| 2010/11 | Belgian First Division (16 clubs) | 16 of 16 match |
| 2017/18 | Belgian First Division A | 16 of 16 match |
| 2018/19 | Belgian First Division B | 8 of 8 match |
| 2024/25 | Belgian Pro League | 16 of 16 match |
| 2023/24 | Challenger Pro League | 16 of 16 match |
| 2009/10 | Belgian Second Division | 19 of 19 match |
| 2009/10 | Belgian Third Division A | 19 of 19 match |
| 2005/06 | Belgian Fourth Division A | 17 of 17 match |
| 2016/17 | Belgian First Amateur Division | 16 of 16 match |
| 2016/17 | Second Amateur Division A, B and C | 48 of 48 match |
| 2022/23 | Belgian National Division 1 | 20 of 20 match |
| 2024/25 | Belgian Division 1 VV and ACFF | 16 and 12 match |

No second source was available for the Belgian Division 2 groups (2020/21 to 2025/26), Third Division B in the 2010s and Fourth Division B and C 2012/13. RSSSF has no table for these, and worldfootball.net blocked automatic reading.

Two RSSSF points support the first file's missing-club list. Waremme played in the 2016/17 Second Amateur Division ACFF (RSSSF "R Stade Waremmien FC"), and VVSB played in the 2010/11 Topklasse Sunday.
