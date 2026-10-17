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

## Pass 2

Decision agent, 2026-10-17. It applies the research decisions Q268 to Q331 of the spec. Row ids are kept. `tools/research/rules_lint.py` gives 0 problems for `eng.tsv` (9 before).

### No new tiers (Q331)

The lowest tier of the original's English files is `eng_conf` (tier 5). These rows are deleted, 27 seasons each, 135 rows in all:

- Conference North and Conference South (tier 6, `new`)
- Isthmian, Southern and Northern Premier League Premier Divisions (tier 7, database divisions 358 to 360 with no code)

`eng.tsv` now has 324 rows: 12 per season. The 9 lint problems were all in deleted step 3 rows. The Conference League Cup rows stay. The cup is a real competition of tier 5. Its tier 6 entrants of 2004/05, 2007/08 and 2008/09 are clubs with no division in the game.

`eng-clubs.tsv`: the 893 rows of Conference North, Conference South, National League North and National League South are deleted. 477 club rows remain.

Conflicts 3, 4 and 6 of the Reconcile section (eng-133, eng-134, eng-213, eng-214, eng-223, eng-224) and the conflict on eng-153 go with their rows. The six near-name matches of the Club moves section were all in deleted North and South rows.

### Conflicts and open points

1. League Two and National League 2019/20 (eng-211, eng-212). Settled by Q278. The season began on 3 August 2019. Bury were expelled on 27 August 2019. The cuts came after the season start, so `down` stays 2 and 4. Source: https://en.wikipedia.org/wiki/2019%E2%80%9320_EFL_League_One
2. National League 2020/21 (eng-222). Settled at 3. Macclesfield Town were expelled on 29 September 2020. The season began on 3 October 2020 with a maximum of 3 relegation places. Dover Athletic stopped playing from 30 January 2021, during the season, so Q278 keeps 3. Source: https://en.wikipedia.org/wiki/2020%E2%80%9321_National_League
3. Football League matchday squad (eng-9 to eng-12). Settled by the bench sizes. The bench was 7 in 2009/10 and 2010/11, 5 in 2011/12 and 7 again from 1 June 2012. The squads were 18, 18, 16 and 18. `registration` now says 4 home-grown in 18 (2009/10, 2010/11), 4 in 16 (2011/12) and 6 in 18 (from 2012/13). The home-grown numbers for 18 in 2009/10 to 2010/11 and the rise to 6 in 2012/13 are inferences. No page states them. Sources: https://en.wikipedia.org/wiki/2009%E2%80%9310_Football_League_Championship, https://en.wikipedia.org/wiki/2011%E2%80%9312_Football_League_Championship, https://en.wikipedia.org/wiki/2012%E2%80%9313_Football_League_Championship
4. FA Cup fifth-round replays (eng-400). Settled at 2018/19. The FA removed them on 18 December 2018, before the fifth round of that edition. Source: https://www.thefa.com/news/2018/dec/18/fa-cup-fifth-round-replays-181218
5. National League substitutes 2020/21 and 2021/22. 2020/21 (eng-222) is 3 used from 5 named (reconcile). 2021/22 (eng-232) stays 3 from 5 as an inference: Wrexham asked for more substitutes during 2021/22, and no rule change is reported. Source: https://fanbanter.co.uk/wrexham-boss-phil-parkinson-makes-a-plea-to-the-national-league/
6. Foreign players. The game uses the real rule: no nationality quota, with the work permit and from 2021 the GBE as the limit. The original's rule of 3 non-EU players goes. Sources: the `foreign` and `work_permit` entries of each nation rules row.
7. Bury 2019/20. Q282 removes Bury from a 2019/20 start. League One plays with 23 clubs (eng-210). eng-210 keeps 4 relegation places by the rules. In real life Bury counted as the fourth. The one-off transition of Q281 must give 24 clubs in League One for 2020/21.
8. Conference North and South substitutes from 2020/21: the rows are deleted.

### Club moves (Q279, Q280, Q290)

Every database season has 20, 24, 24, 24 and 22 clubs in tiers 1 to 5. Real counts differ only here:

- tier 5, 2007/08 to 2025/26 (24 clubs, 23 in 2020/21 and 2021/22): already listed
- tier 3, 2019/20 (23 clubs, Bury out): already listed

The extra tier 5 clubs come from the step 3 divisions and "A Lower Division". These are no longer game tiers, so no gap opens below tier 5. No club for tiers 1 to 5 comes from a higher database division. No club row is added. No division lacks clubs.

### Reserve and B teams (Q289)

English tiers 1 to 5 hold no reserve or B teams in any season, so none are listed. Two cups use academy sides: the EFL Trophy from 2016/17 (16 sides) and the National League Cup from 2024/25 (16 under-21 sides). The brief limits this task to tiers, so these sides are not listed. Max decides whether the two cups get them as new club records.

## Second source, pass 2

Second-source agent, 2026-10-17. The work list is every check line with verdict `no second source` whose row still exists and whose field holds a value. 469 lines name deleted rows and 94 lines hold `unknown`. The 51 lines of the 2027/28 rows (eng-27, eng-288 to eng-292, eng-454 to eng-459) are skipped: their values are carried forward as inferences, and that season has no page yet. 308 lines remain. They are in `eng.check2.tsv`. Every page was read on 2026-10-17.

Counts:

- agree: 228
- disagree: 3, all settled (decision `second`), none still split
- no second source: 77

Sources:

- Cup finals, bench and substitutes: the ESPN match data of each final of the FA Cup, League Cup, Football League Trophy and Community Shield. It lists the named substitutes and the substitutes used. A `subs_used` line agrees only when a club used that many. Where no club reached the limit, the line stays `no second source`.
- Football League Trophy format 2001/02 to 2015/16 and FA Trophy format and name 2001/02 to 2015/16: the season pages of the Football Club History Database (fchd.info). The entrant counts match: 56 in 2001/02, 60 in 2002/03 to 2005/06, 48 with 16 byes from 2006/07. These pages also confirm the two-leg area finals, which the rows mark as an inference.
- FA Trophy from 2016/17: thefa.com news and draw pages, the Non-League Paper and club sites.
- 2026/27 leagues: the BBC Sport tables (club count, promotion, play-off and relegation places) and ESPN match data (bench and substitutes).
- Transfer windows: 90min, goal.com, Liverpool FC and premierleague.com.

Changes to `eng.tsv` (row order and ids kept; `rules_lint.py` gives 0 problems, as before):

1. eng-22 `windows`: the generic rule is replaced by `summer: end of season to 1 Sep 2022; winter: 1-31 Jan 2023`. The deadline was Thursday 1 Sep 2022, which the generic rule does not give. Second: https://www.90min.com/posts/summer-transfer-window-2022-open-close-dates. Third: https://www.footballtransfers.com/en/transfer-news/uk-premier-league/2022/02/does-summer-transfer-window-open-close
2. eng-23 `windows`: now `summer: 14 Jun to 1 Sep 2023; winter: 1 Jan to 1 Feb 2024`. Second: https://www.90min.com/posts/premier-league-confirms-transfer-window-dates-2023-24-season. Third: https://www.premierleague.com/en/news/3485999
3. eng-24 `windows`: now `summer: 14 Jun to 30 Aug 2024; winter: 1 Jan to 3 Feb 2025`. Second: https://www.liverpoolfc.com/news/2024-25-premier-league-transfer-window-dates-confirmed. Third: https://www.premierleague.com/news/4012986
4. eng-209 `bench`: `7 (after the restart: unknown)` is now `7 (9 after the restart)`. The ESPN data of two Championship matches after the June 2020 restart (20 Jun and 22 Jul 2020) show 9 named substitutes for each club. Matches before the stop show 7.

The old `windows=` sources of eng-22 to eng-24 gave only the generic rule, so the new URLs replace them.

Points for Max (no change made):

- eng-27 (2027/28) still carries the generic window rule. The rule is wrong for 2022/23 to 2024/25. Max decides whether 2027/28 copies the dates of 2026/27.
- eng-26 (2026/27, settled in Reconcile at a 1 Sep 2026 deadline): https://www.premierleague.com/news/4237397 gives the summer 2026 close as Monday 31 August. 31 August 2026 is a bank holiday. This line is outside this pass and is not changed.
- eng-439 (FA Trophy 2024/25): one Non-League Paper page says that the Oxford City v Woking quarter-final went to 120 minutes. The club report has an injury-time equaliser followed by a shoot-out, so the row (penalties after 90 minutes) stands.
- Conference League Cup 2022/23 and 2023/24 (`not held`): club pages call the 2024 National League Cup a new competition. No page states outright that no cup was played in those seasons, so these lines stay `no second source`.

Lines still with no second source (77):

- `subs_used` of finals where no club used the full number (Community Shield in most years to 2024, FA Cup 2022/23 to 2025/26 and others)
- FA Trophy bench and substitutes (eng-307, eng-331, eng-379 to eng-409, eng-421, eng-439, eng-445), and FA Trophy format and name of 2019/20 and 2020/21
- Football League Trophy format of 2017/18 to 2023/24 and 2025/26
- the `split` and `format` lines of the 2026/27 leagues, League One 2026/27 `subs_used`, the EFL home-grown rule of 2012/13 to 2014/15 and 2016/17, the FA Cup 2026/27 bench, substitutes and format, and the EFL Cup 2026/27 format

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after. No value changed.

- eng-26 (2026/27), `windows`: the value stands. The Premier League (https://www.premierleague.com/en/news/4664145/when-does-summer-2026-premier-league-transfer-window-open-and-close, read on 2026-10-17) gives the summer 2026 window from Monday 15 June to 23:00 BST on Tuesday 1 September 2026, and the January 2027 window from 1 January to 1 February. The page cited in the second-source section for a 31 August close (https://www.premierleague.com/news/4237397) is about the winter 2026 deadline of Monday 2 February. I added a note to the row.
- eng-27 (2027/28), `windows`: no dates for the summer 2027 or January 2028 windows were announced by 17 October 2026. The value stays an inference, and the row notes say so.

## Pass 3

Pass 3 agent, 2026-10-17. Q336 lets the academy sides of the EFL Trophy (from 2016/17) and the National League Cup (from 2024/25) come in if the build finds it easy. `eng-academy.tsv` lists every academy side of each edition, so that the build can judge the work. `eng.tsv` is not changed.

### Method

- First source: the Wikipedia page of each edition, read as raw wikitext on 2026-10-17. The list comes from the team definitions of the group tables (sides marked as academy sides), and the `team` column keeps the Wikipedia name.
- Second source: a page of another publisher for each edition, read on 2026-10-17 and compared name by name.
- Database match: the database of each season is the one in the spec table. A side gets `db_club_id` and `db_club` only when that database holds an English club record for the side itself (a name such as "Chelsea FC U23", "Southampton FC B" or "Everton Reserves"). The parent club's record never counts.

### Counts

- 224 rows: 11 EFL Trophy editions (2016/17 to 2026/27) and 3 National League Cup editions (2024/25 to 2026/27), 16 academy sides each.
- 192 rows fall in a season with a database. 171 of them have a database record for the side, and 21 do not:
  - 2016/17 (October 2016): all 16. The Reserve division holds only two generic records ("Reserves", "Reserves 2").
  - 2017/18 (October 2017): Middlesbrough, Reading and West Bromwich Albion.
  - 2024/25 (October 2024), National League Cup: Derby County and Sunderland.
- 32 rows (2026/27) have no database, so `db_club_id` is empty.
- 2027/28: the sides are not yet known (`unknown`), so the file has no rows for that season.

The academy records in the databases are in the English division 87 "Reserve". From October 2017 they cover most sides. For the build, this means that 171 sides need no new club record, and 21 sides (plus 2026/27 and 2027/28) need one under Q335 if the cups take academy sides.

### Second source per season

All 14 editions agree: the second publisher names the same 16 sides as Wikipedia.

| Season | Cup | Second source | Result |
|---|---|---|---|
| 2016/17 | EFL Trophy | The League Paper, group stage draw: https://www.theleaguepaper.com/features/featured/5492/premier-league-academies-drawn-in-efl-trophy | agree (16) |
| 2017/18 | EFL Trophy | Oxford United, invited U21 teams: https://www.oufc.co.uk/news/2017/june/checkatrade-u21-teams | agree (16) |
| 2018/19 | EFL Trophy | Newport County, invited U21 teams: https://www.newport-county.co.uk/news/2018/june/arsenal-to-join-checkatrade-trophy-teams | agree (16) |
| 2019/20 | EFL Trophy | Oxford United, invited teams: https://www.oufc.co.uk/news/2019/june/efl-trophy-invited-teams/ | agree (16) |
| 2020/21 | EFL Trophy | Leeds United, Category One invited clubs: https://www.leedsunited.com/en/news/leeds-united-to-participate-in-efl-trophy | agree (16) |
| 2021/22 | EFL Trophy | Port Vale (northern pool of 8): https://www.port-vale.co.uk/news/2021/june/papa-johns-trophy-opponents-revealed/ and Plymouth Argyle (southern pool of 8): https://pafc.co.uk/news/2021/june/papa-johns-trophy-draw | agree (16) |
| 2022/23 | EFL Trophy | Sky Sports, group draw: https://www.skysports.com/football/news/11095/12638924/papa-johns-trophy-2022-23-manchester-united-u21-face-barrow-as-premier-league-invited-clubs-discover-groups | agree (16) |
| 2023/24 | EFL Trophy | EFL, group stage draw (22 June 2023): https://www.efl.com/news/2023/june/efl-trophy-group-stage-draw-confirmed/ | agree (16) |
| 2024/25 | EFL Trophy | EFL: https://efl.com/news/2024/june/24/under-21-teams-and-group-stage-draw-confirmed-for-2024-25-bristol-street-motors-trophy/ | agree (16) |
| 2025/26 | EFL Trophy | EFL: https://efl.com/news/2025/june/25/under-21-teams-and-group-stage-draw-confirmed-for-2025-26-vertu-trophy/ | agree (16) |
| 2026/27 | EFL Trophy | EFL: https://www.efl.com/news/2026/june/23/under-21-teams-and-group-stage-draw-confirmed-for-2026-27-efl-trophy/ | agree (16) |
| 2024/25 | National League Cup | Premier League: https://www.premierleague.com/news/4121350 | agree (16, with Derby County for Reading) |
| 2025/26 | National League Cup | Premier League: https://www.premierleague.com/en/news/4358790/draw-made-for-202526-national-league-cup | agree (16) |
| 2026/27 | National League Cup | Fan Banter, group draw: https://fanbanter.co.uk/2026-27-national-league-cup-group-stage-draw-and-premier-league-2-sides-revealed/ | agree (16) |

The 2021/22 check combines two club pages that each name one regional pool of 8. Club sites and Fan Banter reproduce the competition's own announcement, but they are other publishers than Wikipedia.

### Notes

- 2016/17: the EFL first invited 16 Category One academies (with Arsenal, Liverpool, Manchester City, Manchester United, Newcastle United and Tottenham Hotspur). Those six declined, and Blackburn Rovers, Brighton & Hove Albion, Derby County, Norwich City, Reading and Wolverhampton Wanderers took their places. The list holds the sides that played.
- 2024/25 National League Cup: Reading withdrew on 25 September 2024, before the first match, and Derby County took their place. The list holds Derby County.
- Team names: Wikipedia writes "U21" for every side, and "Manchester City EDS" in 2017/18. The squads were under-23 sides in some seasons (Premier League 2 was an under-23 league until 2021/22). The `team` column keeps the Wikipedia name. The database records use U21, U23, B or Reserves.
- `eng.tsv` cup rows: `clubs` is `n/a` for every cup row, and the `format` entrant counts (64 with 16 academy sides, and 32 with 16 under-21 sides) agree with the list. No row is changed. The notes of eng-441, eng-447, eng-453 and eng-459 say "under-21 sides do not exist in the game". That is true for the base game, but not for the databases from October 2017. The lead decides whether to reword them.
