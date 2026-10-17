# Scotland rules table, 2001/02 to 2027/28

Part `sco` of slice 4 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Research date 2026-10-16.

Files:

- `sco.tsv`: 216 rows. Each of the 27 seasons has one `nation rules` row and one row for each of the 7 competitions of the original (`sco_prm`, `sco_first`, `sco_second`, `sco_third`, `sco_fa_cup`, `sco_lge_cup`, `sco_chal_cup`).
- `sco-clubs.tsv`: header only. No club moves are needed (see "Club moves").

## Method

- Wikipedia season pages and main articles, read as raw wikitext with `curl` and grepped. Each league row cites its season page.
- Substitutes, bench size and squad rules come from the league rulebooks. The rulebooks are the SFL constitution (2010, 2012), the SFL League Cup and Challenge Cup rules (2005/06, 2011, 2012), the SPL rules (April 2011, July 2012) and the SPFL rules (March 2014 to July 2026). Most of them are Wayback Machine copies. The September 2022 rulebook is on spfl.co.uk.
- SPFL and Scottish FA news pages for the mid-season substitute changes of January 2022 and the Scottish Cup rules of 2020/21 and 2021/22.
- STV news pages for work permits and governing body endorsements before and after Brexit.
- 2027/28 has no season pages yet. Its rows copy 2026/27 and say so. The SPFL article lists 42 member clubs for 2026/27, so the four divisions keep 12, 10, 10 and 10 clubs.

## Change points

- 2001/02 to 2004/05: SPL 12 clubs with the split after 33 matches, one down. The First Division champion goes up only if it meets the SPL stadium criteria. SFL First Division has 1 up and 2 down, Second Division 2 up and 2 down, Third Division 2 up. No play-offs.
- 2005/06: SFL play-offs start. In each SFL division the champion goes up. Places 2 to 4 play off with the 9th club of the division above (two-legged semi-finals and final). The bottom club goes down. The Third Division has no relegation.
- 2007/08: Scottish Cup semi-final replays end. The Scottish Qualifying Cup ends and junior clubs enter. The SPL enters in R4 (last 32), with a fifth round before the quarter-finals.
- 2010/11 to 2012/13: the SPL team sheet of 18 must include 3 outfield under-21 players.
- 2011/12: two Highland League clubs join the Challenge Cup (32 clubs).
- 2013/14: SPFL. The names change to Premiership, Championship, League One and League Two. The Premiership play-off starts: 11th v the winner of Championship places 2 to 4. SPFL rule C43 bars any homegrown rule, squad cap or under-21 rule. The bench grows to 7 in all four divisions.
- 2014/15: pyramid play-off. The Highland and Lowland League champions play off, and the winner plays the League Two bottom club.
- 2015/16: League Cup and Challenge Cup bench grows from 5 to 7.
- 2016/17: League Cup group stage (8 groups of 5, bonus point for a shoot-out win after a draw). Challenge Cup with Premiership under-20 teams, Highland and Lowland clubs and 2 clubs each from Wales and Northern Ireland (54 clubs). Scottish Cup fourth substitute in extra time.
- 2017/18: 2 League of Ireland clubs join the Challenge Cup. 2018/19: 2 English National League clubs join (58 clubs).
- 2019/20: all four leagues curtailed on points per game (COVID-19). No play-offs. The Challenge Cup final was not played.
- 2020/21: Premiership 5 substitutes from a bench of 9. League One and Two (22 matches with a split) also get 5 from 9, while the Championship (27 matches) keeps 3 from 7. The Challenge Cup is cancelled. The Scottish Cup has no replays. From 1 January 2021 EU players need a governing body endorsement.
- 2021/22: 3 substitutes at the start, then 5 from 9 in all divisions from late January 2022 (League Two had 5 already). League Cup and Challenge Cup have 5 from 9. The Challenge Cup has Scottish clubs only. The Scottish Cup brings back replays before R4.
- 2022/23: 5 substitutes from 9 in all SPFL competitions. Scottish Cup replays only in the preliminary round. League Cup groups are no longer regional. Welsh and Northern Irish clubs return to the Challenge Cup.
- 2023/24: no Scottish Cup replays at all.
- 2024/25: the Challenge Cup goes back to Scottish clubs only (52 clubs).
- 2025/26: Challenge Cup league phase (30 clubs, 6 matches each), with the Championship entering in R2.
- 2026/27: Challenge Cup league phase of 40 clubs with 10 Highland and Lowland guests (50 clubs). The League Cup has 45 clubs.

## Club moves

Every season database holds 12, 10, 10 and 10 clubs in the four Scottish divisions: 3.9.68, October 2003, September 2005, November 2007, September 2008 to September 2012, Autumn 2013, October 2014 to October 2024, November 2020 and December 2025. These counts match the real counts of every season from 2001/02 to 2026/27. No division is new, so `sco-clubs.tsv` has no rows.

## Missing clubs and items for Max

- No division lacks clubs.
- The pyramid play-off (2014/15 on) needs the Highland League and Lowland League champions. These are tier 5 divisions, which the brief does not ask for. Decide whether the game draws these two clubs from Scottish clubs that have no division, or whether League Two has no relegation.
- The League Cup (2016/17 on) has 1 to 3 Highland or Lowland League clubs. The Challenge Cup has Premiership B or under-20/21 teams, Highland and Lowland clubs and guests from Wales, Northern Ireland, the Republic of Ireland and England (2016/17 to 2019/20, 2022/23 and 2023/24). The original builds the Challenge Cup from league clubs only, up to 30. Decide which guests to model.
- The Highland League, Lowland League and the B teams are not in the table.

## Unknowns

- SPL bench size before 2010/11, and SFL bench and substitute rules before 2010/11. The SFL rows give "5 (inference)" from SFL rule 104 of 2010.
- Scottish Cup bench size before 2021/22, and the Scottish Cup substitute rule from 2022/23. A 2025/26 tie was replayed for a sixth substitution, which suggests five. Wikipedia states a bench of up to 7 with no date.
- Scottish Cup entrant counts before 2023/24 (only the 2023/24, 2025/26 and 2026/27 counts are stated).
- League Cup substitutes in 2020/21 after the rulebook of 30 July 2020.
- Foreign player limits before 2013/14. No quota was found, and the original has none for Scotland.
- Squad registration rules before 2013/14, apart from the SPL under-21 rule.
- Transfer window dates, and the moved summer window of 2020/21.
- Any change for 2027/28.

## Notes for the build

- The game's own names in the data stay "Scottish Premier Division", "Scottish First Division" and so on in every database. The names table needs the SPL and SFL names until 2012/13, then the SPFL names.
- Sponsor names, such as CIS Insurance Cup, Betfred Cup, Premier Sports Cup, IRN-BRU Cup or KDM Evolution Trophy, are on the season pages. The table uses the plain names.

## Reconcile

Reconcile date 2026-10-16. The full list is in `sco.reconcile.tsv`. I changed 18 fields in `sco.tsv` and added each URL to `sources`. I changed no other field.

| Decision | Rows |
|---|---|
| first | 0 |
| second | 7 |
| conflict | 0 |
| found value accepted | 11 |
| found value rejected | 0 |

How I decided:

- sco-158 format: the Inverness Courier is a third publisher. It agrees with the Scottish FA: most Highland League clubs enter in the second preliminary round, two in round one.
- sco-159 subs_used: I found no third publisher outside the SPFL and its clubs. The SPFL competition page of October 2020 gives five substitutes, nine on the teamsheet and a sixth in knockout extra time. The rulebook of 30 July 2020 is older.
- Scottish Cup bench, 2022/23 to 2026/27: I found no third publisher. I read the primary record, rule 13.1 of each season's rules PDF. It gives nine, nine, eight, nine and nine.

There are no conflicts.

Items for Max that the reconcile did not change, because they are in other fields:

- sco-159 bench is still 7. The same SPFL page gives a maximum of nine on the teamsheet in 2020/21.
- sco-214 (2027/28) bench is still "up to 7". The 2026/27 rule gives nine. This is an inference.
- sco-190 format still says "unknown entrants". The clubs field is now 130.


## Source fill

Method: the lint of 2026-10-16 listed each filled fact field without a `field=URL` entry. For each one, I read the pages the row already cites today (curl, text search for the value). If no cited page states the value, I read the general page of the competition. I added `field=URL` to `sources` only where the page text holds the value. I changed no values.

Sources added: 80 fields (split 79, subs_used 1).

- `split` = none on the second to fourth tiers cites the page that states that the teams play each other four times (season page, or the general page of the division). For sco-101, sco-108 and sco-109 the season page gives a single final table with 36 games for each of the 10 clubs. For sco-165 and sco-173 the season page gives the match grids for matches 1 to 18 and 19 to 36.
- sco-214 `subs_used` = "5 (inference: copies 2026/27)" cites the 2026-27 Scottish Cup rules, rule 13.1: "not more than five may play in the match".

Disagreements: none. Fields with no source: none.

## Pass 2

Date 2026-10-17. Decision agent. Rules: Q268 to Q331 in the spec. Row ids and row order are unchanged. `rules_lint.py` gives 0 problems for `sco.tsv`, as before.

### 1. Tiers and nations (Q331, Q288)

The original runs four Scottish tiers (`sco_prm` to `sco_third`). The table holds only these four leagues, the three cups and the nation rules. No row is below tier 4, and no row needs a nation that a game cannot select. The Challenge Cup guests come from Wales, Northern Ireland, the Republic of Ireland and England, which the original runs. Rows deleted: 0. Club rows deleted: 0. The Highland League, the Lowland League and the B-team leagues stay out of the table (tier 5).

### 2. Conflicts and open points

The reconcile left 0 conflicts. The three items for Max of the reconcile are already in the rows: sco-159 bench 9, sco-214 bench "up to 9 (inference)", sco-190 with 130 entrants. The open points of "Missing clubs and items for Max" above are settled by what happened in real life:

- Pyramid play-off (Q284): the `sco_third` rows from 2014/15 keep the real play-off. A note in each row says that the game takes two Scottish clubs with no division as the Highland and Lowland League champions. In every database from October 2014 the real champions are in the spare division "A Lower Division", not in "none". By Q279 the spare division is part of the pool. The real champions of each database season are in `sco-clubs.tsv`. Sources: https://en.wikipedia.org/wiki/Highland_Football_League and https://en.wikipedia.org/wiki/Lowland_Football_League (champions lists and play-off results).
- Cup guests (Q285): the game models the real guests of each season. `sco-clubs.tsv` lists every League Cup and Challenge Cup entrant that is not in the database's four Scottish divisions, for each database season. The counts agree with the `clubs` field of each row (for example 2016/17 Challenge Cup: 30 + 12 B teams + 12 guests = 54). Sources: the Wikipedia season page of each cup, and the League Cup group stage page from 2016/17.
- 2019/20 Scottish Cup (sco-150): the semi-finals (31 October and 1 November 2020) and the final (20 December 2020) are a late edition. The game plays them at their normal time (Q277). Source: https://en.wikipedia.org/wiki/2019–20_Scottish_Cup
- 2019/20 Challenge Cup (sco-152): the final was not played. The game plays it (Q273).
- 2020/21 Challenge Cup (sco-160) stays `not held`, because it was cancelled in real life.

### 3. Club moves (Q279, Q280, Q290)

All 22 season databases hold 12, 10, 10 and 10 clubs in the four Scottish divisions. This agrees with the real count of every season. No tier needs a club move, so no tier leaves a gap below it. Tier club rows added: 0.

### 4. Reserve and B teams (Q289)

No Scottish tier from 1 to 4 has a B team in any season. The Challenge Cup has Premiership under-20 teams (2016/17, 2017/18), under-21 teams (2018/19, 2019/20) and B teams (2021/22 to 2025/26). `sco-clubs.tsv` lists 103 such entries, one per team per season (division "Challenge Cup B team"). The database holds 15 of them: Celtic (2019/20 to 2025/26), Rangers (2021/22 to 2025/26) and Heart of Midlothian (2022/23 to 2025/26), all in the division "Reserve". The other 88 entries have no record. The build adds them as new club records.

### Rows added to `sco-clubs.tsv`

| Division | Rows | No database record |
|---|---|---|
| pyramid play-off (Highland League champion) | 12 | 0 |
| pyramid play-off (Lowland League champion) | 12 | 2 |
| League Cup guest | 25 | 1 |
| Challenge Cup guest | 105 | 6 |
| Challenge Cup B team | 103 | 88 |
| Total | 257 | 97 |

Each club is matched by name to the database of that season. Guests from other nations show their nation in `db_division`. Matches by hand: Connah's Quay Nomads (2701), Bohemians (1710, "Bohemian Football Club Dublin"), Waterford (1347, "Waterford United"), Tranent (6745, "Tranent Juniors FC"), Banks O' Dee (9581) and Broomhill 2021/22 (9211, "BSC Glasgow", the club's former name: https://en.wikipedia.org/wiki/Broomhill_F.C._(Scotland)).

### Missing clubs for Max

These real entrants have no club record in the database of their season. No record was added.

- East Kilbride: pyramid play-off 2016/17 and 2018/19, League Cup 2017/18, Challenge Cup 2017/18 and 2018/19 (October 2016, 2017 and 2018 databases).
- University of Stirling: Challenge Cup 2016/17 and 2017/18.
- Cumbernauld Colts and Turriff United: Challenge Cup 2016/17.
- 88 B and youth teams of the Challenge Cup, 2016/17 to 2025/26 (Q289: the build adds them).

### Changes to `sco.tsv`

Only the `notes` field changed, in 36 rows, and sco-150 got one `notes=` source:

- sco-150: late edition note (Q277).
- sco-152: the game plays the final (Q273).
- `sco_third` rows from 2014/15 (sco-109 to sco-213, 14 rows): the pyramid play-off note (Q284).
- League Cup rows 2016/17 to 2025/26 and Challenge Cup rows 2014/15 to 2025/26 except 2020/21 (21 rows): a pointer to the guest and B-team lists (Q285, Q289).

## Second source, pass 2

Date 2026-10-17. Second-source agent. Input: the 508 check lines with verdict `no second source` in `sco.check.tsv`. 155 of them hold `unknown`, `n/a` or an inference, so they get no new check. The other 353 lines are in `sco.check2.tsv`. Each second source is a page of another publisher than the first source of that fact, read today.

| Verdict | Lines |
|---|---|
| agree | 165 |
| disagree, settled | 6 |
| disagree, still split (conflict) | 1 |
| no second source | 181 |

Main second sources:

- IFAB Laws of the Game, Law 3, editions 2001/02 to 2016/17 (theifab.com downloads): "up to a maximum of three substitutes" in any official competition. This covers every `subs_used` = 3 to 2016/17 (113 lines). The 2018/19 and 2019/20 editions keep three for top-division first teams, which covers the Premiership in those two seasons.
- Wikipedia League Cup and Challenge Cup final pages (match rules box) for the cup bench and substitutes. The first source of these facts is the league rulebook.
- BBC Sport tables of October 2026 for the 2026/27 club counts and direct places, the Edinburgh Reporter (22 April 2026) for the 2026/27 names, and Groundhopper Guides for the 38 Premiership matches.
- SPFL news and club pages (Clyde FC, Livingston FC, Airdrieonians FC) for the League Cup entrants of 2023/24 to 2026/27 and the Challenge Cup of 2019/20. The first source of these facts is Wikipedia.
- STV for the 2021/22 Premiership bench and the 2020/21 Challenge Cup name.

### Disagreements

1. sco-111 bench (League Cup 2014/15). First: 5, from the SPFL rules of 1 June 2014, League Cup rule 5.1. Second: the 2015 final page gives "Seven named substitutes". Third: thecelticwiki lists 7 substitutes for each team in the final of 15 March 2015. Decision: conflict. The rule at the start of the season says 5, and the final used 7. No page dates a change. `sco.tsv` keeps 5. Max decides.
2. sco-120 bench (Challenge Cup 2015/16). First: 7. Second: the 2016 final page box says "Five named substitutes", but its own line-ups list 7 for each team. Third: ESPN line-ups of the final (https://africa.espn.com/football/lineups/_/gameId/427895) show 7 for each team. Decision: first. No change.
3. to 6. sco-136, sco-143, sco-144, sco-151 `subs_used` = "3 (plus a fourth in extra time)" (Challenge Cup and League Cup, 2017/18 to 2019/20, as listed in `sco.check2.tsv`). Second: the final pages give "Maximum of three substitutions", with no extra-time rule. No third publisher. Primary record: the SPFL rules of 19 January 2018 and 22 July 2019 allow "an additional substitute, a fourth in total" in extra time in both cups. Decision: first. The final page box is a general template. A Livingston FC report of 8 August 2017 also shows a fourth substitute in a League Cup tie. No change.
7. sco-152 format (Challenge Cup 2019/20). First: "guest clubs enter in R2 or R3; as 2018/19; extra time and penalties". Second: the Airdrieonians FC draw page (26 June 2019) gives penalties after 90 minutes in R1 and R2, and the 8 non-Scottish guests enter in R3. Third: the SPFL R2, R3 and R4 pages say the same, and the SPFL rules of 22 July 2019 (Challenge Cup rule 4.6) give extra time only in the semi-finals and final. Decision: second.

### Changes to `sco.tsv`

- sco-152 `format`: now gives the entrants of each round (R1 24, R2 16 more, R3 18 more, 58 in all) and "a draw after 90 minutes goes to penalties, with extra time only in the semi-finals and final". Three `format=` URLs are added to `sources`.
- No other field changed. Row ids and row order are unchanged. `rules_lint.py` gives 0 problems for `sco.tsv`, as before.

### Notes for the lead

- sco-144 (Challenge Cup 2018/19) says "extra time and penalties". The SPFL rules of January 2018 give extra time in all rounds. The rules of July 2019 drop it before the semi-finals. I did not check the 2018/19 rounds against another publisher.
- `no second source` remains for these values: `subs_used` 3 in 2017/18 (all leagues) and 2018/19 to 2019/20 (tiers 2 to 4), the extra-time substitute of the 2017/18 Scottish Cup and the 2019/20 Challenge Cup, the league benches of every season, `subs_used` 5 in the leagues from 2022/23, the League Cup bench of 2005/06 and 2020/21, the Challenge Cup bench of 2019/20 and from 2021/22, SPFL rule C43 (`foreign` and `registration`), the 2026/27 windows, the 2026/27 Premiership `down`, and the 2026/27 `format` and `split` of tiers 2 to 4.
- 34 of the 181 `no second source` lines are 2027/28 values that copy 2026/27 (an inference). No page states 2027/28 rules yet.
