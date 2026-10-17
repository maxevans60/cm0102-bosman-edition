# UEFA club competitions, rules 2001/02 to 2027/28

Part `uefa` of slice 9 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Research only. Date of all page reads: 2026-10-16.

Table: `uefa.tsv`, 216 rows, 8 rows per season for 27 seasons. No clubs file: club moves do not apply to this part.

## Rows per season

| comp | comp_file | Notes |
|---|---|---|
| access list | uefa_seeding | Entry places of the 21 UEFA nations of the league table, ranking year, access list link, seeding rules |
| Champions League | european_cup | Format, club counts, ties, subs, bench, squad list, seeding |
| Champions League qualifying, Champions Path | european_cup | `not held` to 2008/09. New code in the original's file |
| Champions League qualifying, League Path | european_cup | `not held` to 2008/09 |
| UEFA Cup / Europa League | uefa_cup | UEFA Cup to 2008/09, Europa League from 2009/10 |
| Conference League | new | `not held` to 2020/21 |
| Intertoto Cup | intertoto_cup | `not held` from 2009/10. `real_season` holds the summer year |
| Super Cup | eur_super_cup | The August match at the start of the season. `real_season` holds the year |

The six nations of the league table outside UEFA (Argentina, Australia, Brazil, Japan, South Korea, United States) get no places. The access list row says so.

## Method

1. Season formats and club counts: the Wikipedia season page of each competition, read as raw wikitext. The script took the "Teams entering in this round" access table and the infobox counts. The round lists in `format` are a short form of that table.
2. Rules (subs, bench, squad list, winter registration, away goals, extra time): the UEFA regulations PDF of each season, mirrored at kassiesa.net/uefa/files. Text was searched with pdftotext and read in context.
3. Entry places per association: the access list pages at kassiesa.net. 2001/02 to 2006/07 use the qualification pages (teams per country). 2007/08 to 2026/27 use the access list tables. 2027/28 uses the 2027/28 access list PDF.
4. Intertoto places 2001 to 2006: the clubs that took part, counted from the Wikipedia season page. 2007 and 2008: the access list.
5. Super Cup: the Wikipedia page of each match ("Match rules" list) and the main UEFA Super Cup page.
6. Coefficient method by period: kassiesa.net/uefa/calc.html.

## How to read the access list row

- Each nation: `Nation (rank N): CL n [slot@round ...] UC or EL n [...] ECL n [...] UIC n [...]`.
- Slots from 2007/08: CH champion, RU or N2 runner-up, N3 to N7 league position, CW cup winner, LC league cup winner, FP fair play place, EPS European Performance Spot.
- Slots 2001/02 to 2006/07 keep the source codes: CL1 to CL4 the league entrants for the Champions League in order, UC1 to UC4 the league entrants for the UEFA Cup, CW, LC, FP, and IT for a club that entered the UEFA Cup through the Intertoto Cup. `extra [...]` holds FP and IT places, which are not part of the association allocation.
- Rounds: G group stage, LP league phase, R1 first round, Q1 to Q4 qualifying rounds, PO play-off round. In the source pages for 2009/10 to 2017/18, Q4 and NCQ4 or CHQ4 are the play-off round. NC or nc means the non-champions path, CH or ch the champions path, lp the League Path.
- The lists are the final published lists, after the title-holder rebalancing of that season. A `+` marks a slot that rebalancing moved. A nation can therefore show one place fewer or more than its rank gives (for example England 2012/13 has its fourth club in the Europa League). The rank bracket rules ("Associations 1 to 3 each have four teams") are on each Wikipedia season page, section "Association team allocation".
- Ranking year: season Y/Y+1 uses the association ranking of year Y-1.

## Change points confirmed

| Point | Season | Fact |
|---|---|---|
| U1 | 2003/04 | One group stage and a first knockout round (round of 16) |
| U2 | 2004/05 | UEFA Cup groups: 8 groups of 5, single round robin, 4 matches each |
| U3 | 2008 | Last Intertoto Cup. From 2006 it had 3 rounds and 11 winners to UEFA Cup Q2 |
| U4 | 2009/10 | Europa League, 12 groups of 4 |
| U5 | 2009/10 | Champions and non-champions split from Q3. The regulations name them Champions Path and League Path from 2015/16. The table uses these names in every season (Q296) |
| U6 | 2013 | Super Cup leaves Monaco (Prague) |
| U7 | 2018/19 | Associations 1 to 4 get 4 places, all direct to the group stage |
| U8 | 2021/22 | Conference League, Europa League groups cut to 32 |
| U9 | 2021/22 | Away goals rule ends in all three competitions |
| U10 | 2024/25 | League phase of 36 in all three. CL and EL 8 matches, Conference League 6 matches. Two European Performance Spots |
| U11 | 2006/07 to 2008/09 | Locally trained places on List A: 4, then 6, then 8 (max 2, 3, 4 association-trained) |
| extra | 2002/03 and 2003/04 | Silver goal extra time |
| extra | 2001/02 | Golden goal in the finals |
| extra | 2018/19 | A 4th substitute in extra time of knockout matches. 12 on the bench for the final |
| extra | 2020/21 | 5 substitutes and 12 on the bench from the group stage. 5 substitutes and 12 on the bench everywhere from 2021/22 |
| extra | 2023 | Super Cup goes to penalties after 90 minutes, no extra time |

## Seeding

Seeding rules per season are in the `notes` of the Champions League row and the access list row.

- Club coefficient: club points of five seasons plus 50% of the association coefficient to 2003/04, plus 33% from 2004/05 to 2008/09, plus 20% from 2009/10 to 2017/18. From 2018/19: club points or 20% of the association coefficient, whichever is higher.
- Group stage pot 1 from 2015/16: the title holder and the champions of the top 7 associations. From 2018/19: both title holders and the champions of the top 6.
- League phase from 2024/25: 4 pots of 9 by club coefficient, title holder top seed.

## Unknowns

- 2002/03 regulations: the PDF has no text layer. Subs and bench come from the 2003 final pages. Squad list rules, winter registration and away goals in extra time are `unknown`.
- 2027/28: no regulations and no Europa League or Conference League season page yet. Rules fields are `unknown`. The Champions League row uses the 2027/28 season page. Places come from the PDF access list.
- 2027/28 entry rounds: read from the column positions of the PDF. This is an inference. The club counts per competition are certain.
- Intertoto Cup: no regulations read. Subs, bench and squad rules are `unknown`. "Away goals and penalties" comes from the results on the season pages.
- Super Cup 2006: no match rules on the page. Super Cup 2024 to 2026: subs and bench not on the pages.
- 2007/08 UEFA Cup total: the page gives only 80 for the first round.

## Points for Max

1. The access list row gives the final list of each season, after title-holder moves. If the game needs the plain allocation by rank, use the rank bracket rules on the Wikipedia season pages, which the access list sources link.
2. Russia: suspended from 2022/23. The 2027/28 PDF still lists Russia. The 2027/28 Champions League page says Russia is suspended.
3. The 2001/02 first qualifying round has 20 champions on Wikipedia and 22 on kassiesa.net. The table uses Wikipedia.
4. 2019/20: the quarter-finals onward were single matches (Lisbon and Germany, August 2020). 2020/21: qualifying ties were single matches. The `format` field says so. Whether the game should copy these one-off seasons is a decision.

## Sources

- Wikipedia season pages: `https://en.wikipedia.org/wiki/<YYYY>%E2%80%93<YY>_UEFA_Champions_League`, `_UEFA_Cup`, `_UEFA_Europa_League`, `_UEFA_Europa_Conference_League`, `_UEFA_Conference_League`, `<YYYY>_UEFA_Intertoto_Cup`, `<YYYY>_UEFA_Super_Cup`, and `UEFA_Super_Cup`, `Golden_goal`, `Homegrown_Player_Rule_(UEFA)`.
- UEFA regulations: `https://kassiesa.net/uefa/files/<YYYY>-uefa-cl-rules.pdf` (to 2008) and `<YYYY>-<YY>-uefa-cl|el|ecl|co-rules.pdf` (from 2009/10).
- Access lists: `https://kassiesa.net/uefa/history/qual<YYYY>.html` (2001 to 2006), `access<YYYY>.html` (2007 to 2017), `AccessList<YYYY>.html` (2018 to 2025), `https://kassiesa.net/uefa/AccessList2026.html`, `https://kassiesa.net/uefa/files/2027-28-uefa-access-list.pdf`.
- Season format summaries: `https://kassiesa.net/uefa/history/index<YYYY>.html`.
- Each row's `sources` field gives the exact URL for each field.

## Reconcile

A third agent settled the 26 `disagree` lines and the 10 `found` lines of `uefa.check.tsv` on 2026-10-16. Decisions are in `uefa.reconcile.tsv`. Settled values are in `uefa.tsv` with their URLs. Row order and ids are unchanged.

| Decision | Lines |
|---|---|
| second | 21 |
| first | 4 |
| conflict | 1 |
| found, accepted | 9 |
| found, rejected | 1 |

### What changed

- Entry places, 2004/05 and 2006/07: the first parse dropped access list lines marked "(league cup)". England 2004/05, France 2004/05 and France 2006/07 now have 3 UEFA Cup places. The three lines are the only ones of that kind on the 2001 to 2006 pages.
- Fair play places: the 2007 and 2008 access lists show "3 x FP" without nations. uefa.com names them. 2007/08: Sweden, Norway, Finland. 2008/09: England, Germany, Denmark. All UEFA Cup Q1.
- Spain 2015/16 to 2017/18: the first parse gave "none". The rows now come from the access lists. All three seasons: rank 1, CL 4 (3 group stage, 1 play-off). EL 2 (group stage, Q3), then EL 2 (both group stage), then EL 3 (2 group stage, Q3).
- Spain 2012/13 and 2014/15: the first value stays (EL 2). The third Spanish club was the EL title holder on the title-holder place. A note in each row says so.
- UEFA Cup 2006/07 clubs: 155 in all (131 direct entrants plus 24 Champions League clubs). The 2005/06 (144) and 2008/09 (157) wording now says "in all". These totals include the Champions League clubs (inference from the round counts).
- Intertoto tie rules, all years: away goals, then 2x15 extra time (away goals count double), then penalties. The source is the 2007 regulations, Art. 7.01. The results show extra-time ties in 2001, 2002, 2004, 2006, 2007 and 2008. For 2003 and 2005 the rule is an inference.
- Intertoto 2007: subs 3, bench 7, squad list from the 2007 regulations.
- CL and EL 2019/20 subs: 3 (+1 in extra time) to March 2020, max 5 (+1 in extra time) in the August 2020 matches.
- Super Cup 2024, 2025, 2026: subs max 5, bench 12.
- Kept: the 2001/02 UEFA Cup final golden goal (regulations Art. 4.03) and the 2006/07 ranking year 2005 (the access list names the 2005 ranking).
- Rejected: UEFA Cup 2007/08 total 157. The RSSSF page gives the ties per round but does not state the total.

### Conflicts for Max

1. Russia 2027/28 (uefa-209). The access list PDF of 8 September 2026 (kassiesa.net) gives Russia CL 1, EL 1, ECL 2. That list follows the ranking only. UEFA decides the suspension each season. The June 2026 decision covers 2026/27 only, and no decision for 2027/28 was found. The row keeps the PDF places with a note. Decide whether the game gives Russia places in 2027/28.

### Reading question for Max

Title holders count in two different ways in the access list rows. 2001/02 to 2006/07 come from the "teams per country" pages, so a title holder counts with its nation (for example England 2005/06 CL 5). From 2007/08, the rows hold the association allocation, and a title holder on the title-holder place is not counted (for example Spain 2012/13 EL 2, Italy 2007/08). Decide whether the game needs one rule for all seasons.


## Source fill

Method: the lint of 2026-10-16 listed each filled fact field without a `field=URL` entry. For each one, I read the pages the row already cites today (curl, text search for the value). If no cited page states the value, I read the general page of the competition. I added `field=URL` to `sources` only where the page text holds the value. I changed no values.

Sources added: 234 fields (clubs 27, name 207).

- The label rows "UEFA access list and seeding" cite the access list page of the season (kassiesa.net) or the season page of the Champions League, which use the words "access list".
- The path rows for 2001/02 to 2008/09 hold `format` = not held. Their `name` cites a later page that names the path: the 2009-10 qualifying page for the Champions Path and the 2018-19 season page for the League Path.
- Super Cup `clubs` = 2 cites the season page of the match, which states that the cup is contested by the two title holders.

Disagreements (value not changed, no source added, "no source found for name" added to `notes`):

- uefa-68 (2009/10), uefa-76 (2010/11): `name` "UEFA Champions League qualifying, League Path". The season pages call the route "non-champions path". The 2009-10 and 2010-11 regulations do not use the word "path".
- uefa-84 (2011/12), uefa-100 (2013/14), uefa-108 (2014/15): same `name`. The season pages say "League route". The regulations of these seasons do not use the word "path".
- uefa-92 (2012/13): same `name`. The season page says "League route" and "non-champions path".
- uefa-83 (2011/12), uefa-99 (2013/14), uefa-107 (2014/15): `name` "UEFA Champions League qualifying, Champions Path". The season pages say "Champions route". The regulations of these seasons do not use the word "path".
- The first regulations that use "League path" and "Champions path" are those of 2015-16 (https://kassiesa.net/uefa/files/2015-16-uefa-cl-rules.pdf). The row notes say 2016/17. Max decides if the label stays for 2009/10 to 2014/15.

## Pass 2

Decision pass of 2026-10-17, by the decisions Q268 to Q331 of the spec. Row ids and row order are unchanged. Lint: 9 problems before, 0 after.

### Tiers, nations, clubs (Q331, Q288, Q279, Q280, Q290, Q289)

- No row is deleted. Each competition of this part is a UEFA club competition. None needs a nation that a game cannot select. Q331 applies to national tiers, so the Conference League (`comp_file` = `new`) stays.
- Club moves and reserve teams do not apply to this part. No clubs file.

### Two legs in 2019/20 and 2020/21 (Q294)

- uefa-146, uefa-149: the sentence on single matches in Lisbon and Germany leaves `format`. The quarter-finals and semi-finals play two legs, as in the rest of the row. The real event is in `notes`.
- uefa-154, uefa-157: the sentence on single-match qualifying ties leaves `format`. The real event is in `notes`. uefa-155 and uefa-156 get the same note.
- The Champions League preliminary round (a mini-tournament of single matches) stays. It is the usual format of that round from 2018/19.
- Subs in 2019/20 keep the real rule change of August 2020 (max 5) by Q293.
- uefa-160: the 2020 Super Cup plays in its normal August slot (Q277). Note added.

### Path names (Q296)

- uefa-68, 76, 83, 84, 92, 99, 100, 107, 108: the names Champions Path and League Path stay. The note "no source found for name" is replaced by a Pass 2 note. `sources` gets `name=docs/specs/2026-10-16-seasons-0.6.0-spec.md (Q296)`, because the name is a decision and the real label of these seasons differs.
- Path rows: "The regulations name the paths from 2016/17" becomes 2015/16, with the 2015-16 regulations URL. Change point U5 says the same.

### Russia (Q293)

- Russia has no places from 2022/23 to 2026/27 already. 2021/22 keeps its places, because the suspension came during the season (Q278).
- uefa-209 (2027/28): Russia's places leave the row. The latest UEFA decision is that of the Executive Committee in June 2026. It amends the 2026/27 list to match the "current suspension of Russian national teams and clubs" (https://www.euronews.com/my-europe/2026/06/07/uefa-extends-ban-on-russian-national-teams-and-clubs-from-competitions-for-202627-season). No decision lifts it, and the 2027/28 Champions League page says Russia is suspended. The PDF places are kept in `notes`. The rounds of the other nations after UEFA rebalances the list are `unknown`.

### Title holders: one rule for all seasons

Rule, as in real life: a title holder that enters on the title-holder place is not counted in its nation, and the domestic place it held is vacated. A title holder that uses its own domestic place counts as that place. The rows from 2007/08 follow it already. Each access list row now states the rule. Four rows change:

| Row | Nation | Before | After | Source |
|---|---|---|---|---|
| uefa-9 (2002/03) | Spain | CL 4 with CL3@TH | CL 3. Real Madrid (3rd) on the title-holder place | https://kassiesa.net/uefa/history/qual2002.html |
| uefa-17 (2003/04) | Italy | CL 4 with CL3@TH | CL 3. AC Milan (3rd) on the title-holder place | https://kassiesa.net/uefa/history/qual2003.html |
| uefa-33 (2005/06) | England | CL 5 | CL 4. Liverpool (5th) on the title-holder place in Q1 | https://kassiesa.net/uefa/history/qual2005.html |
| uefa-49 (2007/08) | Italy | CL 4 | CL 3. AC Milan (4th) on the title-holder place | https://kassiesa.net/uefa/history/access2007.html |

In each case, kassiesa.net states that the vacated place went to another association by rebalancing (Scotland 2002/03, Switzerland 2003/04 and 2007/08) or that the holder entered in addition to the four English clubs (2005/06).

### Other open points

- 2001/02 Champions League Q1: 20 clubs (Wikipedia). RSSSF lists 10 ties (https://www.rsssf.org/ec/ec200102.html). Settled: 20.
- Points for Max 2 and 4 and the conflict and reading question of the Reconcile section are settled above.

## Second source, pass 2

Second-source pass of 2026-10-17. The file is `uefa.check2.tsv`. `uefa.tsv` is unchanged, because no line came out as `disagree`. Lint: 0 problems before and after.

The check file has 265 lines with the verdict `no second source`. 48 lines are out of scope, because the field holds `unknown` or an inference. These are the Intertoto rules, the 2002/03 registration and windows, the Super Cup tie rule of 2006 and 2027, the Intertoto tie rule of 2003 and 2005, and the 2027/28 rules fields. The other 217 lines are in `uefa.check2.tsv`.

| Verdict | Lines |
|---|---|
| agree | 50 |
| disagree, settled | 0 |
| disagree, still split | 0 |
| no second source | 167 |

### Agree

- Path names 2015/16 to 2026/27 (24 lines): the Champions League regulations of each season use "champions path" and "league path".
- Seeding with half the association coefficient, and the title holder as top seed (9 lines): Champions League and UEFA Cup regulations 2001/02 to 2003/04. The 2002/03 Champions League PDF has no text layer, so its pages were read as images. The text layer of the 2002/03 UEFA Cup PDF is shifted by 29 code points, and decoding it gives readable text.
- Subs 3 and bench 7 for 2002/03, Champions League and UEFA Cup (4 lines): the 2002/03 regulations.
- Super Cup 2007 (3 lines): subs 3, bench 7, and extra time then penalties, from the 2007 Super Cup regulations (uefa.com).
- Super Cup 2005 bench 5: the UEFA line-up sheet lists 5 substitutes for each club. This is the line-up of the match, not a rule text.
- Super Cup 2018 bench 12: FourFourTwo, 27 March 2018.
- Windows 2022/23 and 2025/26, Champions League: the limit of 3 new players for the knockout matches, from Sky Sports and The Soccer Era. Neither page covers the 2 new players in qualifying.
- 2027/28 Champions League rounds and club counts (6 lines): the access list PDF of 8 September 2026. It lists Russia and Ukraine in Q1. Once Russia leaves and Ukraine moves to Q2, as the Wikipedia note says, its counts give the row values: 83 clubs, Q1 30, Champions Path Q2 24, Q3 12, PO 10, League Path Q2 6, Q3 8, PO 4.

### No second source (167 lines)

- Windows (49 lines): no page outside UEFA states the rule for those seasons. uefa.com pages do not count, because UEFA publishes the regulations.
- Path names 2009/10 to 2014/15 and 2027/28 (15 lines): 9 of them hold the Q296 decision, so no source can confirm them. 3 are on Wikipedia only. 3 are 2027/28 names, and no 2027/28 regulations exist yet.
- Super Cup subs, bench and tie rule (51 lines): Wikipedia is the only source for those years. UEFA has online Super Cup regulations for 2007 and 2026 only.
- Champions League and Europa League subs and bench 2005/06 to 2010/11 (14 lines): the only source is the regulations. The Wikipedia final pages of these seasons have no list of match rules.
- Intertoto unused places (7 lines), 2026/27 pots (2 lines), 2027/28 seeding notes (2 lines), and the 2027/28 Europa League, Conference League and access list fields (27 lines).

### For the lead

- The 2002/03 regulations can fill the `unknown` registration and windows fields of uefa-10 and uefa-13. For the Champions League, read the image PDF as pictures. For the UEFA Cup, decode the shifted text. In the UEFA Cup file, Art. 6 gives a deadline of 31 January 2003 for later registrations. I did not write these values, because this pass checks only filled fields.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after.

- uefa-10 (Champions League 2002/03) and uefa-13 (UEFA Cup 2002/03): `registration` and `windows` filled from the 2002/03 regulations, Art. 15.04, 15.05 and 15.08. Registration: list A of at most 25 players, and list B for young club-trained players (born on or after 1 January 1981), who do not count in the 25. Windows: at most 3 new players registered by 31 January 2003 for the rest of the competition. The wording is the same as the 2001/02 rows. I read the Champions League pages as images (PDF pages 22 and 23) and decoded the shifted text layer of the UEFA Cup file. URLs, read on 2026-10-17: https://kassiesa.net/uefa/files/2002-uefa-cl-rules.pdf and https://kassiesa.net/uefa/files/2002-uefa-uc-rules.pdf
