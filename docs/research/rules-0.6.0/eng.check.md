# England rules table: second-source check

Check of `eng.tsv` (459 rows) and a spot check of `eng-clubs.tsv`. Research date 2026-10-16. The first agent's files are not changed.

## Method

- Every filled fact field of every row has one line in `eng.check.tsv` (3,002 lines). Fields with `n/a` have no line.
- The second source never comes from Wikipedia. Main sources, all read on 2026-10-16:
  - RSSSF season pages `rsssf.org/tablese/engYY.html` (2001/02 to 2008/09) and `eng20YY.html` (2009/10 to 2025/26). A script read each final table: the club count and the dividing lines for promotion, play-off and relegation places.
  - FCHD league pages (`fchd.info/lghist/conf*.htm`, `nat*.htm`, `isth*.htm`, `south*.htm`, `npl*.htm`) for 2001/02 to 2021/22: table sizes, play-off rounds, and the clubs that left each division after the season.
  - FA National League System allocations for steps 1 to 4 (2022/23, 2023/24, 2024/25, 2025/26 and 2026/27). Some are image PDFs. I read them as page images.
  - News pages and club sites for substitutes, cups, work permits and windows (Sky Sports, TWTD, Pink Un, Portugal Resident, PFA, Swansea City, Cambridge United, Cardiff City, Basildon Standard, Fan Banter, The42, the FA, SI, Chelsea FC, Liverpool FC, Premier League, Mondaq, Littleton Chambers, Inside World Football, Shoot and others). Each line gives its URL.
- A season with no second table (2026/27 in progress, 2027/28) gets `no second source`. The exception is a fact that an announcement covers.
- The temporary files are in a `mktemp -d` folder in the session scratchpad.

## Counts

| Verdict | Lines |
|---|---|
| agree | 2,036 |
| no second source | 922 |
| disagree | 31 |
| found | 13 |

Most `no second source` lines are substitutes and bench sizes of cups and Conference North and South, FA Trophy formats, and the up and play-off fields of the step 3 divisions.

## Disagreements

### 1. Conference `down` does not give the number of places (17 lines)

The brief defines `down` as direct places. `eng.md` says the Conference value is the nominal number of places. The values are smaller in many seasons because the first agent counted only clubs that the Wikipedia legend marks as relegated or reprieved. Reprieved clubs are often not marked. Examples: Chester and Dartford in 2013/14, Southport in 2010/11, Altrincham in 2007/08.

National League (step 1), RSSSF dividing line:

| Row | Season | First agent | RSSSF places |
|---|---|---|---|
| eng-62 | 2004/05 | 2 | 3 |
| eng-72 | 2005/06 | 1 | 2 |
| eng-92 | 2007/08 | 3 | 4 |
| eng-112 | 2009/10 | 2 | 4 |
| eng-122 | 2010/11 | 3 | 4 |
| eng-132 | 2011/12 | 3 | 4 |
| eng-152 | 2013/14 | 2 | 4 |
| eng-212 | 2019/20 | 4 | 3 (only 3 relegated because of Bury) |

Conference North and South:

| Row | Season | First agent | Second source |
|---|---|---|---|
| eng-64 | 2004/05 South | 2 | FCHD: 3 left (Margate, Redbridge, Hornchurch folded) |
| eng-83 | 2006/07 North | 2 | RSSSF: 3 places; FCHD: 3 left |
| eng-84 | 2006/07 South | 1 | RSSSF: 3 places |
| eng-94 | 2007/08 South | 2 | RSSSF: 3 places |
| eng-104 | 2008/09 South | 1 | RSSSF: 3 places; FCHD: 3 left |
| eng-113 | 2009/10 North | 1 | FCHD: 2 left (Farsley Celtic expunged, Northwich) |
| eng-123 | 2010/11 North | 2 | FCHD: 3 left (Ilkeston expelled, Redditch, Stafford) |
| eng-153 | 2013/14 North | 2 | FCHD: 3 left (Histon, Vauxhall Motors, Workington) |

RSSSF URLs: `https://www.rsssf.org/tablese/eng05.html`, `eng06`, `eng07`, `eng08`, `eng09`, `eng2010`, `eng2011`, `eng2012`, `eng2014`, `eng2020`. FCHD URLs: `https://www.fchd.info/lghist/conf2005.htm`, `conf2010`, `conf2011`, `conf2014`. First source: the Wikipedia season page of each row.

Recommendation: use the number of places below the RSSSF dividing line. Keep reprieves and demotions in the notes.

### 2. League Two 2019/20 `down` (eng-211)

First agent: 2 (nominal). RSSSF `eng2020.html`: one place below the line. Only Macclesfield went down because Bury's expulsion took a place.

### 3. National League 2020/21 `clubs` (eng-222)

First agent: 24. RSSSF `eng2021.html`: Macclesfield Town was wound up in September 2020, before the season. 23 clubs started, and Dover Athletic later withdrew. `eng-clubs.tsv` also lists Macclesfield Town for 2020/21. Remove it there.

### 4. National League North and South 2020/21 `up` and `playoffs` (eng-223, eng-224)

First agent: 1 promoted and the 2nd to 7th play-offs. FCHD `nat2021.htm`: the division was voided in February 2021 and no club went up. Write `none` in both fields.

### 5. Southern League Premier 2026/27 `clubs` (eng-286)

First agent: `0 + 0`. FA allocations 2026/27 (`https://www.thefa.com/-/media/thefacom-new/files/competitions/2026-27/nls/nls-1-to-4-club-allocations-2026-27---v1-140526.ashx`): 22 + 22. The 2027/28 row (eng-296) copies the same error. I marked it `no second source` because 2027/28 has no allocation yet.

### 6. Championship 2019/20 `subs_used` (eng-209)

First agent: 3. Swansea City (`https://www.swanseacity.com/news/efl-confirms-return-five-substitute-rule-remainder-2020-21-season`): the EFL allowed five substitutions from the June 2020 restart to the end of 2019/20. Write it like the Premier League row: 3, then 5 after the restart. The bench after the restart is not stated.

### 7. Football League matchday squad (eng-9, eng-10, eng-11)

This is the point that `eng.md` raises. First agent: 4 home-grown players in a 16-man squad for 2009/10 and 2010/11, and 6 in an 18-man squad from 2011/12. TWTD, July 2011 (`https://www.twtd.co.uk/amp/news/18796/substitute-numbers-cut`): the Football League went to 7 named substitutes in summer 2009, and cut to 5 for 2011/12. Some clubs could not name "a full 18-man party". So the squad was 18 in 2009/10 and 2010/11 and 16 in 2011/12. The squad sizes in the home-grown source look reversed. The bench columns of the league rows are right. Rows from 2012/13 to 2016/17 (18 men) fit the bench. I found no second source for the number 6.

### 8. Transfer windows

- eng-18 (2018/19): the first agent says League One and League Two stayed open to 31 August. Sky Sports (`https://www.skysports.com/football/news/11231990/two-transfer-deadline-days-in-august`): all EFL clubs voted to close on 9 August 2018 at 5pm, with loans to 31 August.
- eng-19 (2019/20): the row repeats "9 Aug 2018". The Lancashire Telegraph, March 2019 (`https://www.thisislancashire.co.uk/sport/17518131.efl-introduce-transfer-window-change-for-championship-clubs/`): Championship deadline 8 August 2019.
- eng-26 (2026/27): the summer agrees (15 June to 1 September 2026). The winter window is 1 January to 1 February 2027 (`https://www.premierleague.com/en/news/4664145/when-does-summer-2026-premier-league-transfer-window-open-and-close`). The first agent writes 1 to 31 January.

### 9. Work permit 2020/21 (eng-20)

The first agent writes that the exceptions panel took players with 12 to 14 points. Littleton Chambers (`https://littletonchambers.com/post-brexit-football-player-recruitment-requirements/`) gives 10 to 14 points, January 2021 window only. The other parts agree.

## Found values

| Row | Field | Value | Source |
|---|---|---|---|
| eng-66 | clubs (Southern Premier 2004/05) | 22 | `https://www.fchd.info/lghist/south2005.htm` |
| eng-215 | clubs (Isthmian Premier 2019/20) | 22 | `https://www.fchd.info/lghist/isth2020.htm` |
| eng-216 | clubs (Southern Premier 2019/20) | 22 + 22 | `https://www.fchd.info/lghist/south2020.htm` |
| eng-217 | clubs (NPL Premier 2019/20) | 22 | `https://www.fchd.info/lghist/npl2020.htm` |
| eng-225 | clubs (Isthmian Premier 2020/21) | 22 | `https://www.fchd.info/lghist/isth2021.htm` |
| eng-226 | clubs (Southern Premier 2020/21) | 22 + 20 (the South table at suspension shows 20; check) | `https://www.fchd.info/lghist/south2021.htm` |
| eng-227 | clubs (NPL Premier 2020/21) | 22 | `https://www.fchd.info/lghist/npl2021.htm` |
| eng-222 | subs_used, bench (National League 2020/21) | 3 used, 5 named | `https://fanbanter.co.uk/chesterfield-manager-writes-to-national-league-urging-for-new-rule-ahead-of-2021-22-season/` |
| eng-232 | subs_used, bench (National League 2021/22) | 3 used, 5 named (inference: "in previous years" before 2024, and the AGM vote for 7 failed) | `https://www.basildonstandard.co.uk/sport/24292712.two-changes-announced-national-league-season/` |
| eng-285 | clubs (Isthmian Premier 2026/27) | 22 | FA allocations 2026/27 |
| eng-287 | clubs (NPL Premier 2026/27) | 22 | FA allocations 2026/27 |

## Points from eng.md

- **FA Cup fifth-round replays.** The second source agrees with the first agent's 2018/19. The FA, 18 December 2018 (`https://www.thefa.com/news/2018/dec/18/fa-cup-fifth-round-replays-181218`), removed fifth-round replays "from this season's competition, one season earlier than previously planned". Eurosport says the same. The change-points doc (L7, 2019/20) gives the date of the original plan. Correct the change-points doc.
- **Football League matchday squad.** See disagreement 7.
- **Near-name club matches.** All six are the same club record:
  - Bromley against "Bromley Town" (2007/08): database club 1914 is "Bromley Town" in the November 2007 to April 2011 databases. It is "Bromley" from March 2012 and "Bromley FC" later. The same id moves to the Conference in 2015/16 and to League Two by December 2025, as the real Bromley did. The Feb 2008 database holds no other Bromley. The match is right, and "Bromley Town" is a naming error in the old databases.
  - Hyde (5235: Hyde United, Hyde FC), Darlington (2943: Darlington, then Darlington 1883, Darlington FC), Hereford (4992: Hereford United, then FC Hereford, Hereford FC) and Farsley Celtic (3943: Farsley Celtic and Farsley AFC): one id carries the old club and its successor across the databases. The matches are right. Each is the successor club of the old name (inference from the renames).
  - Merthyr Town (2025/26): the December 2025 database holds one English Merthyr club, 7276 "Merthyr Tydfil", in division 360. An older "Merthyr Town" record (1545) is in no December 2025 data. The match is the only candidate.

## eng-clubs.tsv spot check

Club lists were compared with the FCHD final tables, one season per division type. The names were normalised for case, "AFC/FC" and punctuation.

| Season | Division | FCHD | eng-clubs | Result |
|---|---|---|---|---|
| 2005/06 | Conference South | 22 | 22 | same clubs |
| 2007/08 | Conference Premier | 24 | 24 | same clubs |
| 2007/08 | Conference North | 22 | 22 | same clubs (Leigh RMI, Vauxhall Motors spelling only) |
| 2007/08 | Conference South | 22 | 22 | same clubs |
| 2010/11 | Conference North | 22 | 22 | same clubs |
| 2014/15 | Conference South | 21 | 21 | same clubs |
| 2017/18 | National League North | 22 | 22 | same clubs |
| 2018/19 | National League South | 22 | 22 | same clubs |
| 2019/20 | National League | 24 | 24 | same clubs |
| 2021/22 | National League North | 22 | 22 | same clubs |
| 2021/22 | National League South | 21 | 21 | same clubs |

League One 2019/20 has 23 clubs in RSSSF and in `eng-clubs.tsv`. The only error found is Macclesfield Town in the 2020/21 National League (see disagreement 3).

## Other notes

- **Football League Trophy 2002/03 (eng-306).** RSSSF lists 27 first-round ties and 5 byes (59 clubs). The first agent writes 60. The RSSSF list may lack a tie, so the line is `no second source`.
- **Premier League bench 2021/22 (eng-228).** Only the first source (90min) confirms 9 for 2021/22. Sky Sports (December 2020) confirms 9 from December 2020.
- **Conference North and South substitutes, step 3 substitutes, FA Trophy formats and most cup benches.** I found no second source.
