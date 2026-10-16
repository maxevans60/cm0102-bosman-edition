# Second-source check: Wales, Northern Ireland, Republic of Ireland (slice 4)

Check of `wal-nir-irl.tsv` and `wal-nir-irl-clubs.tsv`, done on 2026-10-16. The check rows are in `wal-nir-irl.check.tsv`. The first agent's files are not changed.

## Method

1. The main second source is RSSSF, which is another publisher than Wikipedia. I downloaded the RSSSF season page of each nation for every season it has: Wales 2001/02 to 2024/25 (`tablesw/walYY.html`), Northern Ireland 2001/02 to 2024/25 (`tablesn/nilYY.html`) and Ireland 2001/02 to 2025 (`tablesi/ierYY.html`, with `ier03a.html` for 2003). I read them as raw text with `curl`. No summary model was used for them.
2. A script pulled the league tables, the promotion and relegation marks and the notes from each page. I compared each league row by hand with that output and read the raw page for every doubtful case.
3. For cups, a script listed the rounds, counted the ties per round and found replays, extra time and penalties. Entrant counts come from the round lists (ties times two, minus the winners that the next round takes in). Where a list was incomplete, the count is not used.
4. Other pages read today: the FAW announcement of the 2026/27 Cymru Premier format (https://faw.cymru/?p=18768), the Bosman judgment on EUR-Lex, FIFA circular 1853 on registration periods, a Lewis Silkin note on the 2026/27 GBE criteria, and a leagueofireland.ie transfer news page of July 2024.
5. `n/a` fields are not checked. A field that stays `unknown` gets `no second source` with the note "still unknown".
6. Names: a name counts as `agree` only when the same name is on the RSSSF page.
7. RSSSF has no page yet for Wales and Northern Ireland 2025/26 or Ireland 2026. Those rows and the future rows have no second source, except the Cymru Premier format of 2026/27 on, which the FAW page confirms.

## Counts

| Verdict | Fields |
|---|---|
| agree | 1434 |
| disagree | 40 |
| found | 92 |
| no second source | 2115 |
| total | 3681 |

Per field:

| Field | agree | disagree | found | no second source |
|---|---|---|---|---|
| clubs | 214 | 4 | 33 | 206 |
| up | 96 | 2 | 16 | 26 |
| down | 156 | 8 | 6 | 46 |
| playoffs | 164 | 6 | 12 | 39 |
| split | 166 | 6 | 18 | 31 |
| format | 357 | 14 | 7 | 216 |
| name | 124 | 0 | 0 | 470 |
| subs_used | 0 | 0 | 0 | 457 |
| bench | 0 | 0 | 0 | 457 |
| foreign | 65 | 0 | 0 | 16 |
| work_permit | 14 | 0 | 0 | 67 |
| registration | 0 | 0 | 0 | 81 |
| windows | 78 | 0 | 0 | 3 |

Most `no second source` fields are substitutes, bench, squad registration and names, plus the cups that RSSSF does not cover (Setanta Sports Cup, Charity Shield, President's Cup, FAI Super Cup, Leinster and Munster Senior Cups).

## Disagreements

| id | season | competition | field | first value | first URL | second URL | second source says |
|---|---|---|---|---|---|---|---|
| wal-nir-irl-16 | 2001/02 | Irish Premier Division | down | 2 (pos 11,12) | https://en.wikipedia.org/wiki/2001%E2%80%9302_League_of_Ireland_Premier_Division | https://www.rsssf.org/tablesi/ier02.html | RSSSF: 3 relegated (pos 10 Dundalk, 11 Galway United, 12 Monaghan United); pos 9 Longford play-off. First value 2 (pos 11,12) |
| wal-nir-irl-17 | 2001/02 | Irish First Division | clubs | 10 | https://en.wikipedia.org/wiki/2001%E2%80%9302_League_of_Ireland_First_Division | https://www.rsssf.org/tablesi/ier02.html | RSSSF: 9 clubs (St Francis merged into St Patricks). First value 10 |
| wal-nir-irl-88 | 2004/05 | Northern Irish Cup | format | single-match knockout with replays in early rounds; extra time and penalties in replays (inference from round lists) | https://en.wikipedia.org/wiki/2004%E2%80%9305_Irish_Cup | https://www.rsssf.org/tablesn/nil05.html | RSSSF: replays in the fifth and sixth rounds, quarter-finals and a semi-final replay (Apr 5). First value: replays in early rounds |
| wal-nir-irl-95 | 2004/05 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2005_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier05.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-111 | 2005/06 | Northern Irish League First Division | down | 2 (pos 11,12) | https://en.wikipedia.org/wiki/2005%E2%80%9306_Irish_First_Division | https://www.rsssf.org/tablesn/nil06.html | RSSSF: Omagh Town closed, so 12th Ballymoney United stayed; only pos 11 Ballyclare Comrades relegated. First value 2 (pos 11,12) |
| wal-nir-irl-116 | 2005/06 | Irish Premier Division | playoffs | pos 11: relegation play-off | https://en.wikipedia.org/wiki/2006_League_of_Ireland_Premier_Division | https://www.rsssf.org/tablesi/ier06.html | RSSSF: 11th Waterford v Dundalk play-off only set the placings; the FAI decided the 2007 places by licence. First value: relegation play-off |
| wal-nir-irl-120 | 2005/06 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2006_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier06.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-142 | 2006/07 | Irish First Division | up | 0 | https://en.wikipedia.org/wiki/2007_League_of_Ireland_First_Division | https://www.rsssf.org/tablesi/ier07.html | RSSSF: pos 1 Cobh Ramblers promoted. First value 0 |
| wal-nir-irl-142 | 2006/07 | Irish First Division | playoffs | pos 1: European play-off; pos 2,3: promotion/relegation play-off | https://en.wikipedia.org/wiki/2007_League_of_Ireland_First_Division | https://www.rsssf.org/tablesi/ier07.html | RSSSF: pos 2 Finn Harps and pos 3 Dundalk in the promotion play-off; no European play-off. First value: pos 1 European play-off, pos 2,3 promotion/relegation play-off |
| wal-nir-irl-145 | 2006/07 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2007_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier07.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-160 | 2007/08 | Northern Irish League Premier Division | down | 2 (pos 15,16) | https://en.wikipedia.org/wiki/2007%E2%80%9308_Irish_Premier_League | https://www.rsssf.org/tablesn/nil08.html | RSSSF: 5 relegated for the cut to 12 clubs (pos 5 Portadown, 11 Donegal Celtic, 13 Larne, 15 Limavady United, 16 Armagh City). First value 2 (pos 15,16) |
| wal-nir-irl-161 | 2007/08 | Northern Irish League First Division | up | 1 (pos 4) | https://en.wikipedia.org/wiki/2007%E2%80%9308_Irish_First_Division | https://www.rsssf.org/tablesn/nil08.html | RSSSF: pos 3 Bangor promoted (only applicant). First value pos 4 |
| wal-nir-irl-161 | 2007/08 | Northern Irish League First Division | down | 3 (pos 10,11,12) | https://en.wikipedia.org/wiki/2007%E2%80%9308_Irish_First_Division | https://www.rsssf.org/tablesn/nil08.html | RSSSF: pos 2 Dundela (failed criteria) and pos 10,11,12 relegated, 4 in all. First value 3 (pos 10,11,12) |
| wal-nir-irl-185 | 2008/09 | Northern Irish League Premier Division | down | 1 (pos 12) | https://en.wikipedia.org/wiki/2008%E2%80%9309_IFA_Premiership | https://www.rsssf.org/tablesn/nil09.html | RSSSF: pos 11 Bangor demoted (left the licensing scheme); pos 12 Dungannon Swifts went into the play-off and stayed. First value pos 12 |
| wal-nir-irl-185 | 2008/09 | Northern Irish League Premier Division | playoffs | pos 11: promotion/relegation play-off | https://en.wikipedia.org/wiki/2008%E2%80%9309_IFA_Premiership | https://www.rsssf.org/tablesn/nil09.html | RSSSF: pos 12 Dungannon Swifts played the play-off in place of 11th Bangor. First value pos 11 |
| wal-nir-irl-192 | 2008/09 | Irish First Division | playoffs | pos 2: promotion/relegation play-off | https://en.wikipedia.org/wiki/2009_League_of_Ireland_First_Division | https://www.rsssf.org/tablesi/ier09.html | RSSSF: pos 2,3 promotion play-off and pos 12 Kildare County relegation play-off. First value pos 2 only |
| wal-nir-irl-195 | 2008/09 | Irish League Cup | clubs | 24 | https://en.wikipedia.org/wiki/2009_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier09.html | RSSSF 2009 (EA Sports Cup): 2 preliminary ties, 11 first-round ties, 8 second-round ties, about 29 entrants. First value 24 |
| wal-nir-irl-270 | 2011/12 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2012_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2012.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-295 | 2012/13 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2013_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2013.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-306 | 2013/14 | Welsh second tier south | down | 3 (pos 14,15,16) | https://en.wikipedia.org/wiki/2013%E2%80%9314_Welsh_Football_League_Division_One | https://www.rsssf.org/tablesw/wal2014.html | RSSSF: 4 relegated (pos 13 Cwmbran Celtic, 14 Tata Steel, 15 Aberbargoed Buds, 16 West End); first value 3 (pos 14,15,16) |
| wal-nir-irl-316 | 2013/14 | Irish Premier Division | playoffs | pos 11: European play-off | https://en.wikipedia.org/wiki/Template:2014_League_of_Ireland_Premier_Division_table | https://www.rsssf.org/tablesi/ier2014.html | RSSSF: pos 11 UCD in the relegation play-off. First value: European play-off |
| wal-nir-irl-320 | 2013/14 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2014_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2014.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-345 | 2014/15 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2015_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2015.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-358 | 2015/16 | Welsh League Cup | clubs | 20 | https://en.wikipedia.org/wiki/2015%E2%80%9316_Welsh_League_Cup | https://www.rsssf.org/tablesw/wal2016.html | RSSSF: 12 first-round ties plus 4 byes (TNS, Prestatyn, Port Talbot, Bala) = 28. First value 20 |
| wal-nir-irl-370 | 2015/16 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2016_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2016.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-386 | 2016/17 | Northern Irish League First Division | split | none | https://en.wikipedia.org/wiki/2016%E2%80%9317_NIFL_Championship | https://www.rsssf.org/tablesn/nil2017.html | RSSSF shows a Promotion Group and a Relegation Group after the first stage (top six and bottom six). First value none |
| wal-nir-irl-395 | 2016/17 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2017_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2017.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-406 | 2017/18 | Welsh second tier south | down | 2 (pos 15,16) | https://en.wikipedia.org/wiki/2017%E2%80%9318_Welsh_Football_League_Division_One | https://www.rsssf.org/tablesw/wal2018.html | RSSSF: pos 14 Monmouth Town and 15 Caerau (Ely) relegated (no Tier 2 certificate); 16th Ton Pentre stayed. First value pos 15,16 |
| wal-nir-irl-411 | 2017/18 | Northern Irish League First Division | playoffs | pos 2: promotion play-off; pos 11: relegation play-off | https://en.wikipedia.org/wiki/2017%E2%80%9318_NIFL_Championship | https://www.rsssf.org/tablesn/nil2018.html | RSSSF: pos 2 Newry City and pos 3 H&W Welders in the promotion play-off; pos 11 Dergview relegation play-off. First value pos 2 only for promotion |
| wal-nir-irl-411 | 2017/18 | Northern Irish League First Division | split | none | https://en.wikipedia.org/wiki/2017%E2%80%9318_NIFL_Championship | https://www.rsssf.org/tablesn/nil2018.html | RSSSF shows a Promotion Group and a Relegation Group after the first stage (top six and bottom six). First value none |
| wal-nir-irl-420 | 2017/18 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2018_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2018.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-436 | 2018/19 | Northern Irish League First Division | split | none | https://en.wikipedia.org/wiki/2018%E2%80%9319_NIFL_Championship | https://www.rsssf.org/tablesn/nil2019.html | RSSSF shows a Promotion Group and a Relegation Group after the first stage (top six and bottom six). First value none |
| wal-nir-irl-445 | 2018/19 | Irish League Cup | format | single-match knockout; penalties if level | https://en.wikipedia.org/wiki/2019_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2019.html | RSSSF: drawn ties went to extra time, then penalties. First value: penalties if level |
| wal-nir-irl-456 | 2019/20 | Welsh second tier south | down | 3 (pos 6,15,16) | https://en.wikipedia.org/wiki/2019%E2%80%9320_Cymru_South | https://www.rsssf.org/tablesw/wal2020.html | RSSSF: pos 14 Taffs Well, 15 Cwmamman United, 16 Caerau (Ely) relegated; first value pos 6,15,16 (6th is Llanelli Town). Note: Taffs Well is in the RSSSF 2021/22 Cymru South table |
| wal-nir-irl-470 | 2019/20 | Irish League Cup | format | abandoned after the first round (COVID-19) | https://en.wikipedia.org/wiki/2020_League_of_Ireland_Cup | https://www.rsssf.org/tablesi/ier2020.html | RSSSF: abandoned during the preliminary round; two preliminary ties not played. First value: abandoned after the first round |
| wal-nir-irl-488 | 2020/21 | Northern Irish Cup | clubs | 26 | https://en.wikipedia.org/wiki/2020%E2%80%9321_Irish_Cup | https://www.rsssf.org/tablesn/nil2021.html | RSSSF: 16 first-round ties (32 clubs), 6 of them awarded as walkovers. First value 26 |
| wal-nir-irl-494 | 2020/21 | Irish Senior Challenge Cup | format | single-match knockout; extra time and penalties; no replays (inference from round lists) | https://en.wikipedia.org/wiki/2021_FAI_Cup | https://www.rsssf.org/tablesi/ier2021.html | RSSSF: quarter-final Finn Harps 3-3 Dundalk replayed on Sep 21 (Dundalk 3-1 aet). First value: no replays |
| wal-nir-irl-511 | 2021/22 | Northern Irish League First Division | split | none | https://en.wikipedia.org/wiki/2021%E2%80%9322_NIFL_Championship | https://www.rsssf.org/tablesn/nil2022.html | RSSSF shows a Promotion Group and a Relegation Group after the first stage (top six and bottom six). First value none |
| wal-nir-irl-536 | 2022/23 | Northern Irish League First Division | split | none | https://en.wikipedia.org/wiki/2022%E2%80%9323_NIFL_Championship | https://www.rsssf.org/tablesn/nil2023.html | RSSSF shows a Promotion Group and a Relegation Group after the first stage (top six and bottom six). First value none |
| wal-nir-irl-561 | 2023/24 | Northern Irish League First Division | split | none | https://en.wikipedia.org/wiki/2023%E2%80%9324_NIFL_Championship | https://www.rsssf.org/tablesn/nil2024.html | RSSSF shows a Promotion Group and a Relegation Group after the first stage (top six and bottom six). First value none |

Notes on the disagreements:

- The League of Ireland Cup rows say "penalties if level". RSSSF shows extra time before penalties in every season it lists. This affects 11 rows.
- The NIFL Championship split is `none` in the rows for 2016/17 to 2023/24, but the notes file of the first agent says the league has had a split since 2016/17. RSSSF agrees with the notes file.
- 2001/02 League of Ireland: the relegation of three clubs and the 9-club First Division fit together. The Premier Division went from 12 to 10 clubs with one club promoted.
- Some RSSSF tables mark clubs as relegated that its next season still lists (Wales 2001/02, Cymru Alliance 2006/07 and 2008/09, Cymru South 2019/20). I took the next-season table as the actual outcome and marked these `agree`, except Cymru South 2019/20, where the first value names the 6th club.

## Found values

| id | season | competition | field | value | URL |
|---|---|---|---|---|---|
| wal-nir-irl-6 | 2001/02 | Welsh Football League Division One | clubs | 19 clubs (RSSSF Welsh Division 1 table) | https://www.rsssf.org/tablesw/wal02.html |
| wal-nir-irl-6 | 2001/02 | Welsh Football League Division One | up | 0: RSSSF note says no team from the southern second level was promoted | https://www.rsssf.org/tablesw/wal02.html |
| wal-nir-irl-6 | 2001/02 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal02.html |
| wal-nir-irl-6 | 2001/02 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal02.html |
| wal-nir-irl-7 | 2001/02 | Welsh Cup | format | extra time and penalties if level after 90 minutes, no replays (RSSSF note in the 2001/02 Welsh Cup section) | https://www.rsssf.org/tablesw/wal02.html |
| wal-nir-irl-8 | 2001/02 | Welsh League Cup | format | six regional groups of three; group winners and two best runners-up to single-leg quarter-finals (replay if level after 90 min); two-legged semi-finals; final at a neutral venue (RSSSF) | https://www.rsssf.org/tablesw/wal02.html |
| wal-nir-irl-12 | 2001/02 | Irish League Second Division | clubs | 13 clubs (Cookstown United withdrew, matches annulled), RSSSF Second Division table | https://www.rsssf.org/tablesn/nil02.html |
| wal-nir-irl-12 | 2001/02 | Irish League Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil02.html |
| wal-nir-irl-20 | 2001/02 | League of Ireland Cup | format | first round, intermediate round, quarter-finals, semi-finals, two-legged final (RSSSF) | https://www.rsssf.org/tablesi/ier02.html |
| wal-nir-irl-31 | 2002/03 | Welsh Football League Division One | clubs | 18 clubs | https://www.rsssf.org/tablesw/wal03.html |
| wal-nir-irl-31 | 2002/03 | Welsh Football League Division One | up | 0: RSSSF says no team promoted from Welsh League Division One | https://www.rsssf.org/tablesw/wal03.html |
| wal-nir-irl-31 | 2002/03 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal03.html |
| wal-nir-irl-31 | 2002/03 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal03.html |
| wal-nir-irl-32 | 2002/03 | Welsh Cup | format | final Barry 2-2 Cwmbran, aet, 4-3 pen confirmed; no replays in the RSSSF round lists (inference) | https://www.rsssf.org/tablesw/wal03.html |
| wal-nir-irl-33 | 2002/03 | Welsh League Cup | format | two-legged preliminary and first rounds, second round, two-legged semi-finals, final (RSSSF round list) | https://www.rsssf.org/tablesw/wal03.html |
| wal-nir-irl-37 | 2002/03 | Irish League Second Division | clubs | 16 clubs | https://www.rsssf.org/tablesn/nil03.html |
| wal-nir-irl-37 | 2002/03 | Irish League Second Division | up | 8 (pos 1-8), league restructure | https://www.rsssf.org/tablesn/nil03.html |
| wal-nir-irl-37 | 2002/03 | Irish League Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil03.html |
| wal-nir-irl-44 | 2002/03 | FAI Cup | clubs | about 41 entrants for the 2003 cup (inference from RSSSF round lists; the 2002-03 cup about 42) | https://www.rsssf.org/tablesi/ier03a.html |
| wal-nir-irl-56 | 2003/04 | Welsh Football League Division One | clubs | 18 clubs | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-56 | 2003/04 | Welsh Football League Division One | up | 1 (pos 1 Llanelli) | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-56 | 2003/04 | Welsh Football League Division One | down | 3 (pos 16,17,18) | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-56 | 2003/04 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-56 | 2003/04 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-57 | 2003/04 | Welsh Cup | format | no replays in the RSSSF round lists, ties decided by extra time and penalties (inference) | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-58 | 2003/04 | Welsh League Cup | format | two-legged preliminary and first rounds, second round, semi-finals, final (RSSSF round list) | https://www.rsssf.org/tablesw/wal04.html |
| wal-nir-irl-62 | 2003/04 | Irish Second Division | clubs | 12 clubs (Intermediate League Second Division) | https://www.rsssf.org/tablesn/nil04.html |
| wal-nir-irl-62 | 2003/04 | Irish Second Division | up | 2 (pos 1,2) | https://www.rsssf.org/tablesn/nil04.html |
| wal-nir-irl-62 | 2003/04 | Irish Second Division | playoffs | pos 12: relegation play-off | https://www.rsssf.org/tablesn/nil04.html |
| wal-nir-irl-62 | 2003/04 | Irish Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil04.html |
| wal-nir-irl-81 | 2004/05 | Welsh Football League Division One | clubs | 18 clubs | https://www.rsssf.org/tablesw/wal05.html |
| wal-nir-irl-81 | 2004/05 | Welsh Football League Division One | up | 1 (pos 2 Grange Quins; champion Ton Pentre failed the criteria) | https://www.rsssf.org/tablesw/wal05.html |
| wal-nir-irl-81 | 2004/05 | Welsh Football League Division One | down | 2 (pos 17,18) | https://www.rsssf.org/tablesw/wal05.html |
| wal-nir-irl-81 | 2004/05 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal05.html |
| wal-nir-irl-81 | 2004/05 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal05.html |
| wal-nir-irl-87 | 2004/05 | Irish Second Division | clubs | 12 clubs | https://www.rsssf.org/tablesn/nil05.html |
| wal-nir-irl-87 | 2004/05 | Irish Second Division | up | 2 (pos 1,2) | https://www.rsssf.org/tablesn/nil05.html |
| wal-nir-irl-87 | 2004/05 | Irish Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil05.html |
| wal-nir-irl-95 | 2004/05 | League of Ireland Cup | clubs | 23 (inference from RSSSF round list) | https://www.rsssf.org/tablesi/ier05.html |
| wal-nir-irl-106 | 2005/06 | Welsh Football League Division One | clubs | 18 clubs | https://www.rsssf.org/tablesw/wal06.html |
| wal-nir-irl-106 | 2005/06 | Welsh Football League Division One | up | 0: no teams promoted to League of Wales | https://www.rsssf.org/tablesw/wal06.html |
| wal-nir-irl-106 | 2005/06 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal06.html |
| wal-nir-irl-106 | 2005/06 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal06.html |
| wal-nir-irl-112 | 2005/06 | Irish Second Division | clubs | 11 clubs | https://www.rsssf.org/tablesn/nil06.html |
| wal-nir-irl-112 | 2005/06 | Irish Second Division | up | 2 (pos 1,2) | https://www.rsssf.org/tablesn/nil06.html |
| wal-nir-irl-112 | 2005/06 | Irish Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil06.html |
| wal-nir-irl-120 | 2005/06 | League of Ireland Cup | clubs | 24 (inference from RSSSF round list) | https://www.rsssf.org/tablesi/ier06.html |
| wal-nir-irl-131 | 2006/07 | Welsh Football League Division One | clubs | 19 clubs | https://www.rsssf.org/tablesw/wal07.html |
| wal-nir-irl-131 | 2006/07 | Welsh Football League Division One | up | 1 (pos 1 Neath Athletic) | https://www.rsssf.org/tablesw/wal07.html |
| wal-nir-irl-131 | 2006/07 | Welsh Football League Division One | down | 3 (pos 17,18,19) | https://www.rsssf.org/tablesw/wal07.html |
| wal-nir-irl-131 | 2006/07 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal07.html |
| wal-nir-irl-131 | 2006/07 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal07.html |
| wal-nir-irl-137 | 2006/07 | Irish Second Division | clubs | 12 clubs | https://www.rsssf.org/tablesn/nil07.html |
| wal-nir-irl-137 | 2006/07 | Irish Second Division | up | 2 (pos 1,2) | https://www.rsssf.org/tablesn/nil07.html |
| wal-nir-irl-137 | 2006/07 | Irish Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil07.html |
| wal-nir-irl-144 | 2006/07 | FAI Cup | clubs | 42 (inference from RSSSF round lists) | https://www.rsssf.org/tablesi/ier07.html |
| wal-nir-irl-145 | 2006/07 | League of Ireland Cup | clubs | 25 (inference from RSSSF round list; byes for Derry, Sligo, Cork, Kerry League and others) | https://www.rsssf.org/tablesi/ier07.html |
| wal-nir-irl-156 | 2007/08 | Welsh Football League Division One | clubs | 18 clubs | https://www.rsssf.org/tablesw/wal08.html |
| wal-nir-irl-156 | 2007/08 | Welsh Football League Division One | up | 0: Goytre United and Dinas Powys did not apply | https://www.rsssf.org/tablesw/wal08.html |
| wal-nir-irl-156 | 2007/08 | Welsh Football League Division One | down | 3 (pos 16,17,18) | https://www.rsssf.org/tablesw/wal08.html |
| wal-nir-irl-156 | 2007/08 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal08.html |
| wal-nir-irl-156 | 2007/08 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal08.html |
| wal-nir-irl-162 | 2007/08 | Irish Second Division | clubs | 12 clubs | https://www.rsssf.org/tablesn/nil08.html |
| wal-nir-irl-162 | 2007/08 | Irish Second Division | up | 4 (pos 1-4) | https://www.rsssf.org/tablesn/nil08.html |
| wal-nir-irl-162 | 2007/08 | Irish Second Division | split | none (single table) | https://www.rsssf.org/tablesn/nil08.html |
| wal-nir-irl-181 | 2008/09 | Welsh Football League Division One | clubs | 18 clubs | https://www.rsssf.org/tablesw/wal09.html |
| wal-nir-irl-181 | 2008/09 | Welsh Football League Division One | up | 0: champion Aberaman Athletic refused entry to the Welsh Premier | https://www.rsssf.org/tablesw/wal09.html |
| wal-nir-irl-181 | 2008/09 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal09.html |
| wal-nir-irl-181 | 2008/09 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal09.html |
| wal-nir-irl-187 | 2008/09 | IFA Interim Intermediate League | clubs | 12 clubs (IFA Interim League, one-season league for clubs that failed the Championship criteria) | https://www.rsssf.org/tablesn/nil09.html |
| wal-nir-irl-187 | 2008/09 | IFA Interim Intermediate League | split | none (single table) | https://www.rsssf.org/tablesn/nil09.html |
| wal-nir-irl-193 | 2008/09 | A Championship | playoffs | promotion/relegation play-off with the First Division 12th (Kildare County), RSSSF 2009 | https://www.rsssf.org/tablesi/ier09.html |
| wal-nir-irl-314 | 2013/14 | NIFL Cup | clubs | 42 (RSSSF: 10 first-round ties plus 22 clubs in round 2) | https://www.rsssf.org/tablesn/nil2014.html |
| wal-nir-irl-339 | 2014/15 | NIFL Cup | clubs | 41 (RSSSF: 9 first-round ties plus 23 clubs in round 2) | https://www.rsssf.org/tablesn/nil2015.html |
| wal-nir-irl-355 | 2015/16 | Cymru Alliance | clubs | 16 clubs | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-355 | 2015/16 | Cymru Alliance | up | 1 (pos 2 Cefn Druids; champion Caernarfon Town had no licence) | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-355 | 2015/16 | Cymru Alliance | down | 1 (pos 16 Rhayader Town) | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-355 | 2015/16 | Cymru Alliance | playoffs | none listed | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-355 | 2015/16 | Cymru Alliance | split | none (single table) | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-356 | 2015/16 | Welsh Football League Division One | clubs | 16 clubs | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-356 | 2015/16 | Welsh Football League Division One | up | 1 (pos 1 Cardiff Metropolitan University) | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-356 | 2015/16 | Welsh Football League Division One | down | 4 (pos 13,14,15,16) | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-356 | 2015/16 | Welsh Football League Division One | playoffs | none listed | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-356 | 2015/16 | Welsh Football League Division One | split | none (single table) | https://www.rsssf.org/tablesw/wal2016.html |
| wal-nir-irl-364 | 2015/16 | NIFL Cup | clubs | 40 (RSSSF: 8 first-round ties plus 24 clubs in round 2) | https://www.rsssf.org/tablesn/nil2016.html |
| wal-nir-irl-389 | 2016/17 | NIFL Cup | clubs | 36 (RSSSF: 4 first-round ties plus 28 clubs in round 2) | https://www.rsssf.org/tablesn/nil2017.html |
| wal-nir-irl-394 | 2016/17 | FAI Cup | clubs | 40 (inference from RSSSF round lists) | https://www.rsssf.org/tablesi/ier2017.html |
| wal-nir-irl-414 | 2017/18 | NIFL Cup | clubs | 36 (RSSSF: 4 first-round ties plus 28 clubs in round 2) | https://www.rsssf.org/tablesn/nil2018.html |
| wal-nir-irl-419 | 2017/18 | FAI Cup | clubs | 40 (inference from RSSSF round lists) | https://www.rsssf.org/tablesi/ier2018.html |
| wal-nir-irl-439 | 2018/19 | NIFL Cup | clubs | 36 (RSSSF: 4 first-round ties plus 28 clubs in round 2) | https://www.rsssf.org/tablesn/nil2019.html |
| wal-nir-irl-464 | 2019/20 | NIFL Cup | clubs | 36 (RSSSF: 4 first-round ties plus 28 clubs in round 2) | https://www.rsssf.org/tablesn/nil2020.html |
| wal-nir-irl-589 | 2024/25 | NIFL Cup | clubs | 38 (RSSSF: 6 preliminary ties plus 26 clubs in round 1) | https://www.rsssf.org/tablesn/nil2025.html |

Counts marked "inference" come from the RSSSF round lists, not from a stated number.

## Clubs file spot-check

I compared the club list of one season per division with the RSSSF table of that season:

- 2010/11 Welsh Premier Division (wal2011)
- 2005/06 Cymru Alliance (wal06)
- 2012/13 and 2017/18 Welsh Football League Division One (wal2013, wal2018)
- 2019/20 and 2022/23 Cymru North (wal2020, wal2023)
- 2022/23 Cymru South (wal2023)
- 2003/04 Northern Irish Premier Division (nil04)
- 2008/09 IFA Championship (nil09)
- 2011/12 Championship 2 (nil2012)
- 2018/19 NIFL Premier Intermediate League (nil2019)
- 2009 League of Ireland Premier Division (ier09)
- 2012 League of Ireland First Division (ier2012)
- 2009 A Championship, both groups (ier09)

All lists match. Name differences are spelling only (Llanfairpwll and Llanfair PG, SD Galway and Salthill Devon). Cymru North 2019/20 lists 17 clubs, and RSSSF ranks 16 because Rhyl was excluded in April 2020.

I also listed every database match where the real name and the database name differ. Most are sponsor names or renamings. Doubtful matches:

- Caerau (Ely), 2010/11 and 2011/12: matched to "Caerau F.C." (2118 in the April 2010 and April 2011 databases). Caerau F.C. may be the club from Caerau near Maesteg, which is a different club from Caerau (Ely) of Cardiff. From 2013/14 the match is "Caerau" (4700). Max should check this.
- Newry City from 2016/17: matched to the old Newry Town record (7916, named Newry City in the April 2015 database). The present Newry City AFC is a new club formed in 2013 after the old club folded. This is a fair alias for the game, but it is not the same club.
- Barry Town United and Y Rhyl 1879 are also successor clubs matched to the records of the old clubs.
- UCD has two ids (11923 and 3532). The ids belong to different databases, so this is correct.

## Sources read today

- RSSSF season pages: https://www.rsssf.org/resultsp00.html, https://www.rsssf.org/resultsp2010.html and https://www.rsssf.org/resultsp2020.html list them all.
- https://faw.cymru/?p=18768
- https://eur-lex.europa.eu/legal-content/EN/TXT/HTML/?uri=CELEX:61993CJ0415
- https://digitalhub.fifa.com/m/4db9566315c26398/original/Circular-1853_RSTP-categorisation-of-clubs-and-registration-periods_EN.pdf
- https://www.lewissilkin.com/insights/2026/06/15/immigration-and-football-significant-gbe-criteria-changes-for-2026-27-102n2rh
- https://www.leagueofireland.ie/news/transfer-latest-league-ireland/
