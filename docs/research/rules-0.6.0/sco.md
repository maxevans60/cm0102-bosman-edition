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
