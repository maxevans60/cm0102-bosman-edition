# England rules table, 2001/02 to 2027/28

Part `eng` of slice 4 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Research date 2026-10-16.

Files:

- `eng.tsv`: 459 rows. Each of the 27 seasons has 17 rows:
  - one `nation rules` row
  - the 10 competitions of the original (`eng_prm`, `eng_first`, `eng_second`, `eng_third`, `eng_conf`, `eng_fa_cup`, `eng_cc_cup`, `eng_auto_cup`, `eng_fa_trophy`, `eng_charity`)
  - 3 new competitions (`Conference North`, `Conference South`, `Conference League Cup`)
  - the 3 step 3 divisions that the database holds with no code (Isthmian, Southern and Northern Premier League Premier Divisions, database divisions 358, 359 and 360).
- `eng-clubs.tsv`: 1,371 rows of real clubs for each season start that has a season database, with the database club that matches each one.

## Method

- Wikipedia season pages, read as raw wikitext through the MediaWiki API on 2026-10-16. A script read each league table (`Module:Sports table`) for the club count, the result legend and the club names. Each league row cites its season page, or the table template that the page uses (`Template:<season> National League table` and the like).
- Cup formats come from the season pages and the main articles of each cup. Substitutes and bench size of the four cups come from the final of each season, where the page records them ("Seven named substitutes, of which up to three may be used"). Those rows say so in `notes`. The rule of the earlier rounds is an inference from the final.
- League substitutes come from news pages of each change (Sky Sports, Football365, 90min, club sites, Inside World Football, The Non-League Football Paper). Work permits come from the FA (2015, 2023) and a law firm page (2020). Transfer windows come from the Transfer window article, the Week (2018), Brentford FC (2020) and premierleague.com (2020).
- 2026/27 has season pages. 2027/28 has none. Its rows copy 2026/27 and say so.
- `down` of the Conference, Conference North and Conference South rows gives the relegation places by the rules of each season (changed by the reconcile, see the end of this file). The note gives the outcome: reprieved, demoted, folded and expelled clubs.

## Change points

- 2001/02: Third Division sends 1 club down. The Conference sends 1 club up and has no play-offs. No transfer windows. Football League Trophy has 56 entrants, Conference clubs among them.
- 2002/03: FIFA transfer windows start. Conference sends 2 up (champion and a play-off winner of places 2 to 5). Third Division sends 2 down. Trophy has 60 entrants (12 Conference clubs). Charity Shield is now the Community Shield.
- 2004/05: names change to Championship, League One and League Two. Conference North and South start with 22 clubs each. Their play-off winners met in one final for one place in 2004/05 only. The Conference Cup runs for one season.
- 2006/07: Conference National grows from 22 to 24 clubs. Trophy back to 48 League clubs.
- 2007/08: Conference National is now Conference Premier. Conference League Cup runs in 2007/08 and 2008/09.
- 2008/09: Premier League bench 5 to 7.
- 2009/10: Football League bench 5 to 7. Football League home-grown rule: 4 in the matchday squad.
- 2010/11: Premier League 25-man squad with 8 home-grown players.
- 2011/12: Football League bench back to 5 for one season. 2012/13: bench 7 again.
- 2015/16: new work permit criteria by FIFA ranking band. Conference names change to National League, National League North and South.
- 2016/17: Football League is now the EFL. The EFL Trophy takes 16 academy sides and a group stage (64 teams). FA Cup quarter-final replays end, and a fourth substitute comes in extra time.
- 2017/18: National League and North and South play-offs grow to 6 clubs (quarter-finals and semi-finals single matches).
- 2018/19: Premier League and Championship summer window closes before the season (2018/19 and 2019/20). League Cup has no extra time before the final and no away goals in the semi-finals. FA Cup fifth-round replays end.
- 2019/20: COVID-19. League One, League Two and the National League end on points per game. Step 3 is abandoned. Bury are expelled before the season, so League One has 23 clubs. Premier League uses 5 substitutes from 9 after the restart.
- 2020/21: window 27 July to 5 October 2020. FA Cup has no replays. FA Cup uses 5 substitutes from 9. EFL goes to 5 substitutes on 20 November 2020. North and South are voided in February 2021. Brexit: from 1 January 2021 every non-UK player needs a GBE, and no EU players under 18 can be signed. League Cup semi-finals are single matches.
- 2021/22: Premier League 3 from 9. EFL 3 from 7. FA Cup replays only in the first and second rounds. The National League has 23 clubs and North and South have 22 and 21. FA Trophy draws go straight to penalties.
- 2022/23: Premier League 5 from 9. EFL 5 from 7. North and South grow to 24 clubs each. FA Cup replays up to the fourth round.
- 2023/24: Championship 5 from 9. GBE route outside the points for 4 players (Premier League, Championship) or 2 (League One, League Two), from 14 June 2023.
- 2024/25: no FA Cup replays from the first round proper (six-year agreement). National League 5 substitutes from 7. National League Cup returns with 16 National League clubs and 16 under-21 sides.
- 2025/26: summer window in two parts (1 to 10 June, 16 June to 1 September 2025).
- 2026/27 (announced): Championship play-offs for places 3 to 8. 5th v 8th and 6th v 7th play single-leg eliminators, then semi-finals with 3rd and 4th. Step 3 play-offs also grow to 6 clubs.

## Club moves

The database of each season keeps the 2001/02 layout: Conference 22, Isthmian 24, Southern 22, Northern Premier 23 (24, 24 and 2 from October 2022). The databases put Conference North and South clubs in divisions 358 to 360 and in "A Lower Division".

`eng-clubs.tsv` lists every real club of these divisions for each season with a database:

- Conference North and South: 2005/06 and 2007/08 to 2025/26 (new divisions).
- Conference Premier and National League: 2007/08 to 2025/26 (24 or 23 real clubs, 22 in the database). Macclesfield Town is not in the 2020/21 list (wound up in September 2020, before the season).
- League One 2019/20: 23 real clubs. The October 2019 database still holds Bury FC in League One (club id 2015). Max decides whether the game removes Bury for a 2019/20 start.

Every listed club has a database club with the same name or a near name. No division lacks clubs. 33 matches come from "A Lower Division" (357), the database's pool of clubs with no playing division. Six matches use a near name and need a check:

| Season | Real club | Database club |
|---|---|---|
| 2007/08 | Bromley | Bromley Town (could be another club) |
| 2010/11 | Hyde | Hyde United |
| 2017/18 | Darlington | Darlington 1883 |
| 2018/19 | Hereford | Hereford United |
| 2019/20 | Farsley Celtic | Farsley AFC |
| 2025/26 | Merthyr Town | Merthyr Tydfil |

The step 3 divisions are lower divisions with no code. Their real club counts differ from the database counts in most seasons. Their clubs are not listed.

No database exists for 2004/05, 2006/07, 2026/27 or 2027/28, so those seasons have no club list.

## Missing clubs

None.

## Disagreements and open points for Max

- Fifth-round FA Cup replays: `2026-10-16-season-change-points.md` (L7) gives 2019/20. The 2018/19 FA Cup page says they ended in 2018/19. The table uses 2018/19.
- Football League matchday squad: the home-grown source says the squad grew from 16 to 18 in 2011/12. The 2011/12 Championship page says the bench fell from 7 to 5 that season, and the 2009 bench change gives a squad of 18 from 2009/10. The `registration` column follows the home-grown source. The bench column follows the bench sources.
- Conference North and South substitutes from 2020/21: the 2024 change to 7 named and 5 used covers Step 1 only. The Step 2 rule is unknown.
- National League substitutes in 2020/21 and 2021/22 (COVID-19 years): unknown.
- The original's foreign rule for England is "3 non-EU". Real England has no quota. The limit is the work permit or the GBE.

## Unknowns

- FA Cup entrant counts before 2005/06 (not recorded in the table).
- Substitutes and bench of the FA Cup for 2013/14 and 2015/16, the League Cup for 2015/16 and 2025/26 onward, the Football League Trophy from 2012/13, the FA Trophy for most seasons, the Charity Shield 2001, the Conference League Cup and the step 3 divisions.
- Football League Trophy area finals over two legs before 2016/17: an inference, not checked on each page.
- Club counts of the step 3 divisions in 2026/27 and 2027/28.

## Out of scope

Points deductions for administration (Football League from 2004/05), the Premier League winter break, VAR, concussion substitutes and financial rules.

## Reconcile

A third agent settled the 31 `disagree` lines and 13 `found` lines of `eng.check.tsv` on 2026-10-16. Every decision is in `eng.reconcile.tsv` (55 lines).

| Decision | Lines |
|---|---|
| second | 33 |
| first | 4 |
| third (neither value) | 1 |
| rule (correction asked by Max) | 11 |
| conflict | 6 |

Method:

- `down` of every Conference, Conference North and Conference South row now gives the relegation places by the rules of the season. Sources: the RSSSF dividing line (2001/02 to 2008/09 for all three divisions, then the top division), the zone in the season page table with reprieved positions counted, and the season page statements ("the bottom three in each of the North and South divisions"). FCHD tables show which clubs stayed up. The outcome (reprieved, demoted, folded, expelled) is in `notes`.
- The check counted departures for North and South in some rows. That gave the right number only where no club was reprieved. Eleven rows that the check agreed with also counted outcomes. They are corrected with decision `rule`.
- Rows 2022/23 onward stay at 4. 2014/15 South stays at 2 (21 clubs after the expulsion of Salisbury City).
- `eng-clubs.tsv`: Macclesfield Town removed from the 2020/21 National League (23 clubs now).
- Southern League Premier South 2020/21 has 20 clubs (Blackfield & Langley resigned, Merthyr Town withdrew for one season). `eng-226` clubs = 22 + 20.
- Step 3 club counts for 2026/27 come from the FA allocation lists (22 in each division). The 2027/28 rows copy them as an inference.

### Conflicts for Max

1. Places cut during the season after a club left (eng-153, eng-211, eng-212). The table keeps the places at the season start: 3 for Conference North 2013/14 (cut to 2 on 4 April 2014 after Vauxhall Motors resigned), 2 for League Two 2019/20 and 4 for the National League 2019/20 (cut to 1 and 3 after the expulsion of Bury). RSSSF shows the cut values. Max decides which one the game uses.
2. National League 2020/21 (eng-222). The value is 3: Macclesfield Town closed before the season, and the league set a maximum of 3 places. RSSSF counts the Dover Athletic withdrawal as one of them and shows 2. No club went down.
3. North and South 2019/20 (eng-213, eng-214). The value 2 comes only from the table legend (21st and 22nd reprieved). No rule text was found. It is an inference.
4. North and South 2011/12 (eng-133, eng-134). The season page says three places. Its table marks two. The value is 3 (inference).
5. Football League matchday squad (eng-9, eng-10, eng-11). The home-grown source gives 4 of 16 for 2009/10 and 2010/11 and 6 of 18 from 2011/12. The bench sources give squads of 18, 18 and 16. The values are kept. The rise to 18 probably belongs to 2012/13 (inference).
6. North and South 2020/21 `up` and `playoffs` (eng-223, eng-224). The values stay at the rules (1 place, 2nd to 7th play-offs). The void is in `notes`. This follows the default rule of 2026-10-16.

## Source fill

Method: I ran the rules lint and took each filled fact field with no `field=URL` entry. I read each page on 2026-10-16 with curl and checked the value against the page text or league table. For `split` = none and `format` = double round robin, the season page must show one table for the division with every club on 2 x (clubs - 1) matches, and no championship or relegation round. For a stopped season (2019/20, 2020/21), a table with fewer matches counts if the page says the season was stopped. I added sources only. I changed no value, row order or id.

- Sources added: 555 field entries in 267 rows.
- Seasons 2001/02 to 2003/04, Second and Third Division (eng-30, 31, 40, 41, 50, 51): the season page of the whole Football League (for example https://en.wikipedia.org/wiki/2001%E2%80%9302_Football_League) gives clubs, up, down and the play-off bracket (two legs, then a final).
- 2026/27 and 2027/28 for Premier League to National League South: no season page gives the format of a season not yet played. I cited the league article, which gives the current format (each club plays every other club twice). The row carries that rule forward.
- 2026/27 Isthmian, Southern and Northern Premier League Premier Divisions: the season page shows one table per division, so it covers `split`.

No source found (added to `notes`):

- eng-285, eng-286, eng-287 (2026/27): `format`. The league articles and the 2026/27 season pages give no match format.
- eng-295, eng-296, eng-297 (2027/28): `split` and `format`. No season page exists yet.

Disagreements: none. Notes on the pages:

- 2019/20 League One (eng-210): the table lists 24 lines with Bury on 0 matches. The row has 23 clubs, which agrees with Bury's expulsion.
- 2020/21 National League (eng-222): the table has 24 lines with 2 clubs on 0 matches. The other 22 clubs played 42 matches each.
