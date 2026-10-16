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
| U5 | 2009/10 | Champions and non-champions split from Q3. The regulations name them Champions Path and League Path from 2016/17 |
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
