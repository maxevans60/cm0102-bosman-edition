# Second-source check: club-world (slice 9)

Date 2026-10-16. This file checks `club-world.tsv` against sources from publishers other than Wikipedia. The rows are in `club-world.check.tsv` (2,132 rows, all 513 ids). The first agent's files are unchanged.

## Method

1. Main second source: RSSSF edition pages, read today with `curl` and grep. One page per edition: `sacups/copaYY`, `sacups/sudamcupYY`, `sacups/recopa`, `sacups/mercosur01`, `sacups/merconorte01`, `tablesc/cacupsYY`, `tabless/subleaguescup`, `tablesa/ascupYY`, `tableso/oceacupYY`, `tablesf/fifa-wccYY`, `tablesf/fifa-icc2024/2025`, `tablest/toyotaYY`, plus the overview pages for each competition.
2. Places per nation for the Libertadores and Copa Sudamericana come from the RSSSF lists of Argentine clubs per edition (`sacups/copalibarg.html`, `sacups/sudamcup-arg.html`) and the RSSSF Brazil lists (`rsssfbrasil.com/miscellaneous/brpartlib.htm`, `brpartsul.htm`).
3. For editions after RSSSF's last page (2025 and 2026), I used conmebol.com, ofcproleague.com, oceaniafootball.com, pina.com.fj, aseanfootball.org, thepeninsulaqatar.com, wam.ae, record.com.mx, soccerway and cbssports.com. I read only the facts that these pages state.
4. Dates are checked against the first and last match on the RSSSF page. Club counts come from the group tables and tie lists. Some of these counts are approximate because the same short name can stand for two clubs, and the note says so.
5. Field `format:places` holds the places-per-nation part of `format`. Field `foreign,registration` covers both unknown fields in one row.

## Counts per verdict

| Verdict | Rows |
|---|---|
| agree | 1,192 |
| disagree | 22 |
| found | 7 |
| no second source | 911 |

Most "no second source" rows are substitutes, bench size, foreign players and registration. RSSSF gives line-ups but not match rules, and I read no regulation PDF. Others are the 2023/24 AFC rows, the 2024/25 to 2027/28 ACL Elite and ACL Two places, the 2027 and 2028 rows and some OFC club counts.

## Disagreements

Each row gives the first value, the second value, and both URLs. The first URL is from the `sources` column of `club-world.tsv`.

| id | Field | First (Wikipedia) | Second | Second URL |
|---|---|---|---|---|
| club-world-1 | notes | "Played in December of the season" | 2001 match on 27 November 2001 | https://www.rsssf.org/tablest/toyota01.html |
| club-world-9 | format | two-legged knockout, 3 Feb – 18 Sep 2002 | first match 13 Feb 2002; final a single match on 18 Sep 2002 | https://www.rsssf.org/tablesc/cacups02.html |
| club-world-13 | clubs | 27 | 28 drawn, 4 withdrew | https://www.rsssf.org/tablesa/ascup02.html |
| club-world-13 | format | final stage in Abu Dhabi, 6 and 8 Feb 2002 | Abu Dhabi hosted only the West Asia quarter-final group (6–10 Feb). Semi-finals and final in Tehran, 3 and 5 Apr 2002 | https://www.rsssf.org/tablesa/ascup02.html |
| club-world-16 | format | final over two legs | single final in Doha, 30 Mar 2002 | https://www.rsssf.org/tablesa/ascup02.html |
| club-world-28 | format | start 6 Mar 2003 | first leg 4 Mar 2003 | https://www.rsssf.org/tablesc/cacups03.html |
| club-world-42 | format | preliminary round, 8 groups of 4 (2004) | no preliminary round, 9 groups of 4 | https://www.rsssf.org/sacups/copa04.html |
| club-world-83 | format | start 16 Aug 2005 | first ties from 9 Aug 2005 | https://www.rsssf.org/sacups/sudamcup05.html |
| club-world-103 | format | 2007 Recopa first leg 31 May 2007 | 30 May 2007 | https://www.rsssf.org/sacups/recopa.html |
| club-world-104 | format | start 22 Feb 2007 | first leg 13 Feb 2007 | https://www.rsssf.org/tablesc/cacups07.html |
| club-world-116 | format | third-place and fifth-place matches (2007) | no fifth-place match in 2007 | https://www.rsssf.org/tablesf/fifa-wcc07.html |
| club-world-132 | format | end 4 May 2008 | final second leg 11 May 2008 | https://www.rsssf.org/tableso/oceacup08.html |
| club-world-140 | format | start 30 Jul 2008 | first ties 29 Jul 2008 | https://www.rsssf.org/sacups/sudamcup08.html |
| club-world-142 | format | end 30 Apr 2009 | final heading "Apr 22 & May 12" | https://www.rsssf.org/tablesc/cacups09.html |
| club-world-151 | format | end 2 May 2009 | final second leg 3 May 2009 | https://www.rsssf.org/tableso/oceacup09.html |
| club-world-156 | clubs | 40 (2010 Libertadores) | 38 (6 preliminary ties and 32 in groups) | https://www.rsssf.org/sacups/copa2010.html |
| club-world-265 | format | 2015 preliminary stage in American Samoa | in Tuanaimato, Samoa | https://www.rsssf.org/tableso/oceacup2015.html |
| club-world-303 | format | end 7 May 2017 | final second leg 6 May 2017 | https://www.rsssf.org/tableso/oceacup2017.html |
| club-world-351 | format | rest of the 2020 CCL played as single matches in December | three quarter-final first legs played 10–11 Mar 2020, second legs in December. Only LAFC v Cruz Azul was one leg | https://www.rsssf.org/tablesc/cacups2020.html |
| club-world-363 | format | 4–11 Feb 2021 | match 1 dated 1 Feb 2021 (awarded, Auckland City withdrew) | https://www.rsssf.org/tablesf/fifa-wcc2020.html |
| club-world-408 | format | single-match final (2023 CCL) | two-legged final, 31 May and 4 Jun 2023 | https://www.rsssf.org/tablesc/cacups2023.html |
| club-world-410 | notes | Leagues Cup 2022 cancelled for World Cup congestion | cancelled because of the 2023 format change, and replaced by exhibition games | https://www.rsssf.org/tabless/subleaguescup.html |

The first URLs are on these Wikipedia pages: the edition page of each competition in the row's `sources` cell (for example `2002 CONCACAF Champions' Cup`, `2004 Copa Libertadores`, `2007 FIFA Club World Cup`, `2015 OFC Champions League`). For the end dates in 132, 142, 151 and 303, the RSSSF page itself gives one date. A third source can settle them.

## Found values

| id | Fact | Value | URL |
|---|---|---|---|
| club-world-32 | AFC Champions League 2002–03 places | JPN 2: Kashima Antlers (direct) and Shimizu S-Pulse (via qualifying). KOR 2: Seongnam Ilhwa Chunma (direct) and Daejeon Citizen (via qualifying). The first file leaves out Kashima | https://www.rsssf.org/tablesa/ascup03.html |
| club-world-45 | Copa Sudamericana 2003 places | ARG 7, BRA 12 | https://www.rsssf.org/sacups/sudamcup-arg.html ; http://www.rsssfbrasil.com/miscellaneous/brpartsul.htm |
| club-world-64 | Copa Sudamericana 2004 places | ARG 6, BRA 12 | same |
| club-world-83 | Copa Sudamericana 2005 places | ARG 7, BRA 8 | same |

The check file names the clubs of each nation in the note.

## Other points for the maintainer

- The OFC Men's Champions League 2026 row says AUS 0. A search summary quoting the same publisher (Wikipedia) names Central Coast as the 2026 runners-up. No second publisher confirms it. Check it before the build.
- Merconorte 2001: RSSSF gives the final second leg as 19 Dec in the result list and 20 Dec in the match details. The row (20 Dec) agrees with the details.
- Recopa: the aet second legs of 2014, 2018 and 2021 agree. RSSSF also shows aet second legs in 2023 and 2026, which fit the same inferred rule.
