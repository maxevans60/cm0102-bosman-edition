# Second-source check: int-world

Check of `int-world.tsv` (324 rows) for release 0.6.0. Research date 2026-10-16. The verdicts are in `int-world.check.tsv`, one line per filled fact field.

## Method

- Every first source of this part is English Wikipedia. No second source in this check is a Wikipedia edition.
- Fields checked per row: `name`, `clubs`, `playoffs`, `format`, `subs_used`, `bench`, `registration` and `notes`. Fields with `n/a` are not listed. The `name` of the `host_country` rows is a label, so it is not listed.
- World Cup qualifying 2002 to 2022: RSSSF qualifying pages (`rsssf.org/tables/<year>q.html`), read raw with `curl`. A script grouped every dated match by zone and by season (July to June) and gave the first date, last date and match count. The script then checked every `not held` qualifier row against these counts.
- World Cup qualifying 2026 and parts of 2022: the FIFA match list (`api.fifa.com/api/v3/calendar/matches`, competition 520, one season id per zone). The times are UTC, so a late match in the Americas or Samoa shows the next day. Slots, entrants and withdrawals for 2026: ESPN.
- Finals: RSSSF pages for the World Cup (`<year>f.html`), the Confederations Cup (`tablesi/intconcup.html`) and the Olympics (`tableso/ol<year>f.html` and `ol<year>q.html`).
- Rules (squads, substitutes, bench), hosts, membership and renames: FIFA, UEFA, OFC, Al Jazeera, CBS Sports, beIN Sports, Flashscore, AP and other papers. Some of these were read through search-result extracts, not the full page. The notes say so where it matters.
- FIFA name changes: archived copies of the fifa.com ranking table on the Wayback Machine, grepped for the team names.
- `subs_used` = 3 in qualifying to 2019/20: IFAB allowed more than three only from 2020 (temporary five-substitute option). The use of five in each qualifying competition from 2020/21 was not found, so those fields are `no second source`.

## Counts

| Verdict | Fields |
|---|---|
| agree | 1020 |
| no second source | 237 |
| found | 11 |
| disagree | 11 |
| total | 1279 |

Most `no second source` lines are `bench` = `unknown` in qualifying rows, the five-substitute question from 2020/21, the 2030 qualifying formats, and notes that say "no change found".

## Disagreements

| Row | Field | First value | Second value | First URL | Second URL |
|---|---|---|---|---|---|
| int-world-3 | format | UEFA groups from 2 September 2000 | First match 16 August 2000 (Estonia v Andorra, group 2); last group match 27 October 2001 | en.wikipedia.org/wiki/2002_FIFA_World_Cup_qualification_(UEFA) | rsssf.org/tables/2002q.html |
| int-world-119 | notes | 2010/11 Olympic qualifiers: South American Youth Championship and U-21 Euro only | AFC Olympic qualifying also started this season (first round from 23 February 2011) | en.wikipedia.org/wiki/Football_at_the_2012_Summer_Olympics_–_Men's_tournament | rsssf.org/tableso/ol2012q.html |
| int-world-172 | format | 32 CONMEBOL matches in 2015/16 | 30 matches (6 matchdays of 5, 8 October 2015 to 29 March 2016) | en.wikipedia.org/wiki/2018_FIFA_World_Cup_qualification_(CONMEBOL) | rsssf.org/tables/2018q.html |
| int-world-206, 218, 230, 242 | notes | Next finals: 2023 | Next finals: the 2022 World Cup (20 November to 18 December 2022, season 2022/23) | no URL on the row | rsssf.org/tables/2022f.html |
| int-world-248 | clubs | 8 (Tonga withdrew) | 6 completed: Cook Islands (after one annulled match) and Vanuatu (before playing) also withdrew because of COVID-19 | en.wikipedia.org/wiki/2022_FIFA_World_Cup_qualification_(OFC) | rsssf.org/tables/2022q.html |
| int-world-270 | format | CAF first round: 9 groups of 6 | 8 groups of 6 and 1 group of 5 (Eritrea withdrew); 104 matches on matchdays 1 to 4 | en.wikipedia.org/wiki/2026_FIFA_World_Cup_qualification_(CAF) | api.fifa.com season 288282; global.espn.com story 40297462 |
| int-world-282 | clubs | 54 in groups | 53 (Eritrea withdrew) | en.wikipedia.org/wiki/2026_FIFA_World_Cup_qualification_(CAF) | global.espn.com story 40297462 |
| int-world-294 | clubs | 54 in groups | 53 (Eritrea withdrew) | en.wikipedia.org/wiki/2026_FIFA_World_Cup_qualification_(CAF) | global.espn.com story 40297462 |

Points that agree only after a note:

- int-world-128: RSSSF prints 23 to 27 November 2011 for the OFC first round. The FIFA match list gives kick-offs at 01:00 and 03:30 UTC on 23 to 27 November, which is 22 to 26 November in Samoa (UTC-11). The row is right.
- int-world-223 and 235: RSSSF prints the AFC second-round restart as May-June 2020. The FIFA match list gives 2021, with one match (Qatar v Bangladesh) on 4 December 2020. The rows are right. The RSSSF page has a typo.
- int-world-125 and 233: the CONCACAF `clubs` value counts the teams left at the start of the season, including the seeds that enter later.

## Found values

| Row | Field | Value | URL |
|---|---|---|---|
| int-world-2, 50, 98, 194 | bench | 12 (the bench before 2022: "12 available in previous tournaments") | lastwordonsports.com/football/2022/06/24/26-player-squad-fifa-world-cup/ |
| int-world-254 | bench | 15 substitutes and 11 officials on the bench (FIFA, 23 June 2022) | cbssports.com/soccer/news/fifa-approves-26-man-squads-for-2022-world-cup-as-teams-will-have-three-extra-players-in-qatar |
| int-world-290 | subs_used | 5 in normal time, one more in extra time | beinsports.com/en-us/soccer/fifa-world-cup-2026/articles/how-many-substitutions-are-allowed-in-fifa-world-cup-2026-matches-2026-04-22 |
| int-world-35 | notes | 2004 Olympic qualifying started in 2002/03: CAF preliminary round 8 to 24 November 2002, AFC first round from 5 April 2003. OFC (January 2004), CONMEBOL (January 2004) and the CONCACAF final round (February 2004) fall in 2003/04 | rsssf.org/tableso/ol2004q.html |
| int-world-244 | notes | Brazil v Argentina (suspended 5 September 2021, re-set for 21 September 2022) was declared void and never played | rsssf.org/tables/2022q.html |
| int-world-193 | notes | Eswatini: the old fifa.com ranking table shows "Swaziland" in the ranking of 16 August 2018 and "Eswatini" in the ranking of 20 September 2018. The newer fifa.com ranking page shows Swaziland up to the ranking of 7 February 2019 and Eswatini from the ranking of 4 April 2019. The association renamed itself on 1 July 2018 | web.archive.org/web/20181006204509/https://www.fifa.com/fifa-world-ranking/ranking-table/men/index.html |
| int-world-205 | notes | North Macedonia: "FYR Macedonia" in the FIFA ranking of 4 April 2019, "North Macedonia" in the ranking of 14 June 2019. FIFA changed the name between these two releases | web.archive.org/web/20190702120030/https://www.fifa.com/fifa-world-ranking/ranking-table/men/ |
| int-world-181 | notes | Czechia: date still unknown. The FIFA ranking table still says "Czech Republic" in the archived copy of 1 June 2021, so FIFA changed the name after June 2021 | web.archive.org/web/20210601000417/https://www.fifa.com/fifa-world-ranking/ranking-table/men/ |

Türkiye (int-world-253): date not found. The archived FIFA ranking page of 1 June 2021 still says "Turkey". Later archived copies are built by script and hold no team names. Al Jazeera says the UN received Turkey's letter on Wednesday 1 June 2022. The row says "letter of 31 May 2022". These can both be true (letter date and receipt date), so the verdict is `agree`.

## Not found

- 2030 qualifying of CONMEBOL, CAF, AFC and OFC (rows 304, 306 to 308, 316, 318 to 320): no published format. The FIFA match list has no 2030 qualifier from September to mid-October 2026. One report (Blueprint) put a CAF preliminary round in March 2026, but the FIFA match list has no such match. Colombian media expect CONMEBOL to start in March 2027. CONMEBOL has not confirmed this.
- Substitutes at the 2020 and 2024 Olympics, at the Confederations Cups and in qualifying from 2020/21.
- The draw dates of the inter-confederation play-offs (2002 to 2022). RSSSF does not give them.
- The AFC (23 March 2005) and OFC (17 April 2005) endorsement dates for Australia. The OFC page gives only the effective date, 1 January 2006.
