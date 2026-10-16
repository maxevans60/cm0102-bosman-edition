# Rules table part: club-world (slice 9)

Date 2026-10-16. Research only. Scope: the club competitions of the confederations other than UEFA and the FIFA club competitions, 2001/02 to 2027/28. Rows: `club-world.tsv` (513 rows, 19 competitions x 27 seasons). No clubs file: club moves do not apply to these competitions.

## Which source file runs which competition

The original finds each competition record by its name in club_comp.dat (string compares near 0x60f700 to 0x611160) and stores a pointer in a global. The setup code near 0x8311b8 to 0x83207e passes that pointer and the game year (word at 0xae2c92) to the constructor of each file.

| Source file | Constructor | Competition record (club_comp.dat name) | Global |
|---|---|---|---|
| world_club_champ.cpp | 0x929140 | FIFA Club World Championship | 0x9cf7bc |
| world_club_cup.cpp | 0x92b4b0 | Intercontinental Cup (also matched as "World Club Cup") | 0x9cf6e4 |
| conmebol_liber.cpp | 0x4c2f50 | South American Copa Libertadores | 0x9cf63c |
| conmebol_merc.cpp | 0x4c46c0 | South American Copa Mercosur | 0x9cf698 |
| con_merc_cup.cpp | 0x4c16f0 | Merconorte Cup (also "CONCACAF Merconorte Cup") | 0x9cf68c |
| con_champ.cpp | 0x4c0c40 | CONCACAF Champions Cup | 0x9cf72c |
| inter_amer_cup.cpp | 0x632080 | inter-american cup | 0x9cf95c |
| asia_club_champ.cpp | 0x40aae0 | Asian Club Championship (also "Asian Champions Cup") | 0x9cf64c |
| asia_cup_winner.cpp | 0x40cbe0 | Asian Cup Winners' Cup | 0x9cf650 |
| asia_super_cup.cpp | 0x410650 | Asian Super Cup | 0x9cf7d0 |
| oceania_club_champ.cpp | 0x796fd0 | Oceania Champions Cup (also "Oceania Club Championship") | 0x9cf7cc |

So the Intercontinental Cup runs in world_club_cup.cpp, the Copa Mercosur in conmebol_merc.cpp and the Copa Merconorte in con_merc_cup.cpp.

Other facts from the code:

- world_club_champ moves its year up to the first odd year from 2003 (loop at 0x92919c to 0x9291bd, compare at 0x9291b2). In real life the competition was not held from 2001 to 2004 and was annual from December 2005.
- conmebol_seeding branches on game year above 2002 (0x4c64fe). It reads a seeding file (`conmebol_seeding.tmp` string). Not traced further here.
- "South American Recopa" is looked up (0x60f799, global 0x9cf648), but no constructor uses that global. The original does not run the Recopa.
- club_comp.dat holds African records (African Champions League, CAF Cup, African Cup Winners' Cup, African Super Cup), but the function map has no African club file. The original does not run African clubs, so the CAF Champions League and Confederation Cup are out of scope. They appear only as Club World Cup and FIFA Intercontinental Cup opponents.
- The Inter-American Cup ended in 1998, but the original runs it. Every row says "not held".

## Method

1. Wikipedia edition pages, read today through the MediaWiki API (wikitext). Facts came from the infobox (dates, clubs, host), the team allocation tables (places per nation), the intro and format sections, and the "Match rules" lists of final pages (substitutes and bench).
2. Places per nation come from the `rowspan` cell of each nation in the allocation table, with its "(N + M berths)" text. For older pages without a table, I counted the nation's clubs in the group tables.
3. Places are given only for the nations of the game's league table: Argentina and Brazil (CONMEBOL), the United States (CONCACAF), Japan, South Korea and Australia (AFC, and OFC for Australia until 2005). For the FIFA competitions the row names every club from a game nation.
4. One source publisher (Wikipedia) for every fact. The checker must use another publisher.

## How editions map to game seasons

The brief puts a calendar-year competition of year Y in the row of season (Y-1)/Y. I applied that rule by the start date of the competition proper: an edition goes to the game season (July to June) in which its first match is played. For an edition that starts from January to June, this gives the same result as the brief. For one that starts from July to December (Copa Sudamericana 2002 to 2016, Intercontinental Cup, Club World Cup, Leagues Cup, CONCACAF League), it gives the season in which the game plays it. Example: the 2005 Club World Championship (December 2005) is in 2005/06. Editions with a split label (2002-03 ACL, 2008-09 CCL) go to that season.

Effects:

- The base game season 2001/02 holds the 2001 Copa Mercosur and Merconorte, the 2001 Intercontinental Cup, the 2001-02 Asian club competitions, the 2001 Asian Super Cup and the 2002 Libertadores and CONCACAF Champions' Cup. This matches what the original runs.
- Two Recopa editions start in 2006/07, 2008/09, 2014/15 and 2016/17. Two Copa Sudamericana editions start in 2016/17 (2016 and 2017). These rows hold both editions, separated by `||`.
- 2007/08, 2009/10 and 2015/16 have no Recopa. 2022/23 has no new ACL or AFC Cup edition (move to autumn to spring). 2025/26 has no OFC Champions League competition proper (its 2026 qualifying falls in January 2026).

## Main changes for the build

- Intercontinental Cup: last held December 2004. Not held from 2005/06.
- Club World Championship: not held 2001/02 to 2004/05. Annual from 2005/06, renamed Club World Cup from 2006/07, 7 clubs from 2007/08. Five substitutes from the 2020 edition. Fifth-place match dropped after 2021. 32 clubs in June and July 2025 (season 2024/25), then quadrennial: next in summer 2029 (outside the range).
- FIFA Intercontinental Cup: new, annual from 2024/25.
- Copa Mercosur and Merconorte: last edition 2001. Copa Sudamericana from 2002/03 (no Brazilian clubs in 2002).
- Recopa: back from 2003/04.
- Libertadores: 38 clubs from 2005, 47 clubs and a calendar to November from 2017, single-match final from 2019, no away goals from 2022.
- CONCACAF: Champions' Cup to 2008, Champions League 2008-09 to 2023, Champions Cup with 27 clubs from 2024. CONCACAF League 2017 to 2022. Leagues Cup from 2019, a Champions Cup route from 2023.
- AFC: Asian Club Championship, Cup Winners' Cup and Super Cup end in 2002. AFC Champions League from 2002-03. AFC Cup 2004 to 2023-24. Autumn to spring from 2023-24. ACL Elite and ACL Two from 2024-25. ACL Elite league stage grows to 32 clubs in 2026-27. Australian clubs enter the ACL from 2007.
- OFC: not held 2002 to 2004. Australian clubs only in 2005 (Sydney FC won). OFC Professional League from January 2026, with South Melbourne as an Australian club that cannot qualify for other competitions.

## Not in the table

- CAF club competitions (no code in the original).
- AFC Challenge League (third tier from 2024-25): no game nation enters.
- Campeones Cup (MLS v Liga MX champions) and the J.League Cup / Copa Sudamericana Championship: single matches not researched. Max can decide whether they matter.

## Unknowns

- Foreign player limits and squad registration for every held row: not researched (`unknown`).
- Substitutes and bench: known only where a final page lists match rules. The value is the final's rule. It may differ from the group stage of the same edition.
- Copa Sudamericana places for Argentina and Brazil in 2003, 2004 and 2005.
- AFC places for 2002-03 (clubs named on the page only).
- AFC Cup formats: only clubs and dates. Group and knockout details per year are not in the row.
- The Recopa tie-break from 2014 is an inference from second legs that went to extra time.
- Editions without a page on 2026-10-16: 2028 Libertadores and Copa Sudamericana, 2027 and 2028 CONCACAF Champions Cup, 2027 Leagues Cup, 2027 OFC Champions League, 2027-28 ACL Two, 2027 and 2028 OFC Professional League, details of the 2027 and 2028 Recopa and the 2027 FIFA Intercontinental Cup.
- Exact host venues of the 2007 to 2023 Club World Cups (the row gives the host nation).

## Missing clubs

None. No club moves apply to this part.

## For Max

- The season mapping above differs in wording from the brief for July to December competitions. It keeps each edition in the season in which the game plays it.
- The original runs the Inter-American Cup every season and has no Recopa, Copa Sudamericana, CONCACAF League, Leagues Cup, AFC Cup, ACL Two, FIFA Intercontinental Cup or OFC Professional League code.

## Reconcile

Reconcile date 2026-10-16. The full list is in `club-world.reconcile.tsv`. I changed 25 rows in `club-world.tsv` and added each URL to `sources`. Row order and ids are the same.

| Decision | Rows |
|---|---|
| first | 4 |
| second | 18 |
| conflict | 0 |
| found value accepted | 4 |
| found value rejected | 0 |
| flag settled | 1 |

How I decided:

- The third publisher is mostly Transfermarkt (full match lists). For OFC finals and some single dates, Transfermarkt gives placeholder kick-off times, so I used match reports instead: RNZ, OFC, the Timbers club site, Cooperativa, Arab News, Goal and Scoop.
- In 14 of the 18 `second` decisions, the Wikipedia infobox or lead sentence disagrees with the match boxes or body text on the same page. The match boxes agree with RSSSF and the third publisher.
- 156 (Libertadores 2010, 40 clubs) and 363 (Club World Cup 2020 dates) are reading questions. Both values are true. I kept the first value and added a note.
- 103 (Recopa 2007) and 303 (OFC 2017): match reports confirm the first value.
- Copa Sudamericana places for 2003 to 2005 and AFC Champions League 2002-03 places are now filled. The lines about them in "Unknowns" above are settled.
- OFC Men's Champions League 2026: no Australian club took part. The runner-up Central Coast is Central Coast FC of the Solomon Islands (Scoop, OFC, and the SOL flag in the Wikipedia infobox).

There are no conflicts.

Items for Max that the reconcile did not change, because no check row disputes them:

- club-world-64 (Copa Sudamericana 2004) says Argentine and Brazilian clubs enter in the second round. The RSSSF Argentine list shows Arsenal v Banfield in the first round on 10 August 2004. The same may apply in other years.
- club-world-116 calls the 2007 opener a play-off of the host champion v the OFC winner. The host berth went to Sepahan because Urawa won the AFC Champions League.

## Loose ends

- Copa Sudamericana entry round (club-world-26, 45, 64, 83, 102, 121, 140, 159): the rows said that Argentine and Brazilian clubs entered in the second round. The RSSSF pages sudamcup02 to sudamcup09 show otherwise. In 2002 and 2008 they entered in the first round, after a preliminary round for clubs of other nations (River Plate and Boca Juniors had byes in 2008). From 2003 to 2007 they played national sections from the first stage or phase, and some big clubs entered one stage later (for example Arsenal v Banfield in the 2004 first stage, Boca Juniors and River Plate in the second stage). In 2009 they entered in the first stage. The rows of 2010 to 2016 (second stage) are right and stay.
- Club World Cup host place: row 116 (2007) now says that Sepahan, the AFC runner-up, took the host place because Urawa Red Diamonds won the AFC Champions League. The same case is in row 135 (2008: Adelaide United for Gamba Osaka) and row 401 (2022 edition: Al Ahly for Wydad Casablanca). Sources: RSSSF fifa-wcc07, fifa-wcc08 and fifa-wcc2022.


## Source fill

Method: the lint of 2026-10-16 listed each filled fact field without a `field=URL` entry. For each one, I read the pages the row already cites today (curl, text search for the value). If no cited page states the value, I read the general page of the competition. I added `field=URL` to `sources` only where the page text holds the value. I changed no values.

Sources added: 89 fields (format 20, name 69).

- The Recopa Sudamericana rows had their `format` pages under keys such as `format_2003` and `format_rules`, which the lint does not read. I checked that each season page gives the match dates in the row, and that the general page gives the tie-break. I then added the same pages as `format=URL`. The page of cooperativa.cl for 2007 did not show the dates in text, so I did not add it.
- A name with a part in brackets, for example "Intercontinental Cup (Toyota Cup)", counts as stated when the page holds the main name and the bracket text.

Disagreements: none. Fields with no source: none.
