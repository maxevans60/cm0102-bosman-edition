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

## Pass 2

Date 2026-10-17. Decision agent. `club-world.tsv` now has 405 rows. Row ids of the kept rows are unchanged. The lint gives 0 problems, as before.

### Competitions removed (Q288)

A competition of the original stays, also when it holds clubs of nations that a game cannot select (CONCACAF Champions Cup, Copa Libertadores, Oceania Champions League, Club World Cup). A new competition stays only when clubs of the game's nations take part. I deleted the 108 rows (4 x 27) of these new competitions:

- Leagues Cup: Q288 names it. It needs Liga MX clubs.
- CONCACAF League: Central American and Caribbean clubs only. No US club took part.
- AFC Cup: clubs of the developing AFC nations only. Japan, South Korea and Australia had no places.
- OFC Professional League: 7 of its 8 clubs are from nations that a game cannot select. South Melbourne cannot qualify through it for other competitions.

Kept new competitions: Copa Sudamericana, Recopa Sudamericana, AFC Champions League Two and the FIFA Intercontinental Cup. Argentine, Brazilian, Japanese, Korean, Australian or European clubs take part in each.

Deleted ids: 10, 11, 14, 19, then every 19th id from each of these to 513 (the `leagues_cup`, `concacaf_league`, `afc_cup` and `ofc_pro_league` rows). No clubs file exists for this part, so no club rows were deleted.

### Notes added for the removed competitions

- CONCACAF Champions League 2018 to 2023 (313, 332, 351, 370, 389, 408): the places of the CONCACAF League winner (to 2021) and of its top clubs (2022, 2023). Inference: the build fills them with Central American and Caribbean clubs from the data, as the original does for those nations.
- CONCACAF Champions Cup 2024 to 2028 (427, 446, 465, 484, 503): the three Leagues Cup places. Open point for Max, see below.
- FIFA Intercontinental Cup 2026 and 2027 (478, 497): inference: the OFC entrant is the OFC Champions League winner, as to 2025.

### Season of the 2026 OFC Champions League (Q276, Q277)

The 2026 edition starts with qualifying on 31 January 2026. By Q276 its season is 2025/26, not 2026/27. I moved its values from 493 to 474. The competition proper was played in August 2026, after the season end. By Q277 the game plays the whole edition at the normal time of the competition in 2025/26. Rows 493 (2026/27) and 512 (2027/28) now hold the 2027 and 2028 editions, with `unknown` values. Their season is an inference: qualifying early in the year, as in 2026. Sources: https://en.wikipedia.org/wiki/2026_OFC_Champions_League and https://pina.com.fj/2026/01/30/ofc-mens-champions-league-2026-qualifying-ready-for-kick-off-in-samoa/

Warning for the second-source agent: the check lines of 474 and 493 were written for the old content of those rows. Check 474 against the 2026 edition. The check lines of 493 and 512 now refer to `unknown` values.

### Late editions (Q277)

Each row stays in the season of its first match (Q276). The game plays it at the normal time of the competition in that season, with no special dates. I added a note to these rows:

- 346 and 349: Copa Libertadores and Copa Sudamericana 2020, finals in January 2021.
- 351 and 370: CONCACAF Champions League 2020 (finished December 2020) and 2021 (April to October).
- 355, 374 and 393: AFC Champions League 2020 (to December 2020), 2021 (from April) and 2022 (final May 2023).
- 363, 382 and 401: Club World Cup 2020, 2021 and 2022, played in February of the next year.
- 369: Recopa 2021, played in April.
- 398: OFC Champions League 2022, June to August.
- 360: OFC Champions League 2020, knockout stage cancelled. The game plays it to the end (Q273).

### Conflicts and open points

The reconcile left no conflicts. The points listed above under "For Max", "Reconcile" and "Loose ends" are settled by what happened:

- Copa Sudamericana entry rounds 2002 to 2009 (26 to 159): the rows already give the real entry stages from RSSSF (https://www.rsssf.org/sacups/sudamcup04.html and the other sudamcup pages).
- Club World Cup host place (116, 135, 401): the rows already name the club that took the host place (https://www.rsssf.org/tablesf/fifa-wcc08.html).
- Central Coast in the 2026 OFC final is the Solomon Islands club (https://www.scoop.co.nz/stories/CU2608/S00140.htm).
- Season mapping: it agrees with Q276.
- Campeones Cup: it is in the arg-usa part, and it needs a Liga MX club (Q288).

Open for Max:

1. The three Leagues Cup places in the CONCACAF Champions Cup from 2024. The real rule gives them to the Leagues Cup top three, MLS or Liga MX. The game has no Leagues Cup. I don't know which rule Max wants.
2. The J.League Cup / Copa Sudamericana Championship (2008 to 2019) has clubs of game nations only. It was not researched in pass 1. Max decides whether it comes in.

## Second source, pass 2

Date 2026-10-17. Second-source agent. The lines are in `club-world.check2.tsv` (301 lines). I took each check line with verdict `no second source` whose row still exists and whose field holds a value. I skipped 159 lines of deleted rows, 448 lines whose field is `unknown`, and 8 lines whose value is an inference (the format of 484, 497, 498, 501, 503 and 509, the places of 509, the notes of 512). I also checked every filled field of row 474 against the 2026 edition. The lint gives 0 problems, as before.

| Verdict | Lines |
|---|---|
| agree | 119 |
| disagree, settled | 4 |
| disagree, still split | 0 |
| no second source | 178 |

Sources: RSSSF for the OFC club counts. The OFC, CONMEBOL-calendar, AFC-draw and FIFA releases came through oceaniafootball.com, pina.com.fj, oceaniafootballcenter.org, theawayend.co, insideworldfootball.com, thepeninsulaqatar.com, qatar-tribune.com, en.vff.org.vn, inside.fifa.com, concacaf.com, jfa.jp and national newspapers. Transfermarkt and worldfootball.net show a bot check, so I did not use them. The web search limit of the session ran out near the end.

### Changes to club-world.tsv

Each change has its URLs in `sources`. Row order and ids are the same.

1. club-world-387 (Copa Sudamericana 2022), `subs_used`: 5 is now "5, a sixth in extra time". The 2022 Manual de Clubes allows a sixth substitution in extra time of the single final, for both cups (442.perfil.com and opovo.com.br).
2. club-world-479 (Copa Libertadores 2027), `format`: the start is now 3 February 2027, not 2 February (tribunadejundiai.com.br and alairelibre.cl, from the CONMEBOL calendar of 25 August 2026).
3. club-world-482 (Copa Sudamericana 2027), `format`: the start is now 3 March 2027, not 2 March (same pages).
4. club-world-490 (ACL Two 2026-27), `format` places: now "JPN 1, KOR 1 + 1, AUS 1 + 1". Gangwon and Adelaide United lost the ACL Elite preliminary stage and play in the ACL Two groups (en.vff.org.vn and qatar-tribune.com).

### Points for the maintainer

- Row 474 agrees in every filled field.
- Club count reading: row 452 (ACL Two 2024-25) says 31 because Mohun Bagan was withdrawn and its matches were made void. Row 471 (2025-26) says 32, but Mohun Bagan was withdrawn again on 30 September 2025. Both rows follow a page. One rule for both rows is needed.
- Found values that I did not write: the 2027 Recopa legs are on 17 and 24 February 2027 (row 483). The 2027 CONCACAF Champions Cup has 27 clubs (row 484, concacaf.com).
- FIFA names Auckland FC, the OFC Professional League champion, as the OFC club in the 2026 FIFA Intercontinental Cup. Row 478 keeps the pass-2 inference (Q288) for the game.
- Row 482: CONMEBOL gave the 2027 Sudamericana final to Santa Cruz de la Sierra, but no page that I read names the stadium.
- Most `no second source` lines are substitutes and bench size. No page of another publisher that I could read states the rule of that edition. The other lines are method notes, code facts and the 2027 and 2028 editions.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after. Every page below was read on 2026-10-17.

- Rows 452 and 471 (AFC Champions League Two 2024-25 and 2025-26), `clubs`: one rule for both. `clubs` counts the clubs at the start (32 in the group stage). Withdrawals go in `notes`. Row 452 changed from 31 to 32. Mohun Bagan Super Giant was withdrawn in both seasons and its matches were made void (Al Jazeera, 7 October 2024, https://www.aljazeera.com/sports/2024/10/7/indian-club-mohun-bagan-out-of-afc-competition-after-missing-match-in-iran, and The Away End, 30 September 2025, https://theawayend.co/2025/09/30/mohun-bagan-withdrawn-afc-champions-league-two-2025-26/).
- Row 483 (Recopa Sudamericana 2027), `format`: unknown changed to two legs on 17 and 24 February 2027 (CONMEBOL calendar as reported by https://www.alairelibre.cl/futbol/copa-libertadores/conmebol-calendario-sudamericana-formato-ida-vuelta/).
- Row 484 (CONCACAF Champions Cup 2027), `clubs`: unknown changed to 27. `format` now gives the places by route and the byes to the Round of 16 (Caribbean Cup winner, Leagues Cup winner, highest-ranked Liga MX champion, MLS Cup winner). Rounds and legs stay unknown. Source: https://www.concacaf.com/competitions/champions-cup/news/2027-concacaf-champions-cup-qualified-clubs. The open point on the three Leagues Cup places (Q288) stays open.
- Row 478 (FIFA Intercontinental Cup 2026), checked: FIFA names Auckland FC, winner of the inaugural OFC Professional League, as the OFC club. It played Al-Ahli in Jeddah on 26 August 2026 (https://inside.fifa.com/news/al-ahli-auckland-fc-kick-off-intercontinental-cup-2026-jeddah). The row keeps the pass-2 inference for the game (Q288), and the note now gives the real entrant.

## Second source, retry

Date: 2026-10-17. Second-source retry agent. The earlier pass ran out of web searches. I took the 178 lines of `club-world.check2.tsv` with verdict `no second source` and tried again. I read regulation PDFs first: the IFAB Laws of the Game (downloads.theifab.com), FIFA Club World Cup regulations (fifa.com and digitalhub.fifa.com, through archive.org where needed), CONMEBOL regulations and club manuals (conmebol.com), AFC regulations (the-afc.com) and CONCACAF regulations (concacaf.com). I updated each line in place. The lint gives 0 problems for `club-world.tsv`, as before.

| Verdict of the 178 lines | Lines |
|---|---|
| agree | 96 |
| disagree, settled | 6 |
| disagree, still split (conflict) | 3 |
| no second source | 73 |

New totals of `club-world.check2.tsv`: 215 agree, 13 disagree, 73 no second source.

The 73 lines that stay `no second source`:

- 43 method notes, code facts and inferences. No outside page applies.
- 16 substitutes or bench lines with no regulation text found: the bench of rows 58, 75, 78, 97, 116, 135, 249, 255, 344 and 365, the substitutes of rows 313 and 382, and both fields of rows 325 and 363. Six of these lines now carry a note that says what the page read does and does not state.
- 14 lines of the 2027 and 2028 editions (club counts, names, places). Row 507 now carries a note: the AFC Entry Regulations for 2027/28 give the East ranks 1, 2 and 5 the place patterns of the row, but do not name the nation of each rank.

For the substitutes of 2004 to 2016 where no competition rule text was found, the second source is Law 3 of the IFAB Laws of the edition in force. It caps every official competition at three substitutes.

### Changes to club-world.tsv

Each change has the regulation URL in `sources` and a line in `club-world.reconcile.tsv`. Row order and ids are the same.

1. club-world-311 (Copa Sudamericana 2018), `subs_used`: 3 is now "3, a fourth in extra time of the final" (regulations 2018, art. 90).
2. club-world-332 (CONCACAF Champions League 2019), `subs_used`: 3 is now "3, a fourth in extra time" (regulations 2019).
3. club-world-336 (AFC Champions League 2019), `subs_used`: 3 is now "3, a fourth in extra time" (regulations 2019, art. 13.3).
4. club-world-389 (CONCACAF Champions League 2022), `subs_used`: 5 is now "5, a sixth in extra time" (regulations 2022, 12.8.5).
5. club-world-393 (AFC Champions League 2022), `subs_used`: 5 is now "5, a sixth in extra time" (regulations 2022, art. 17.3 and 10.3).
6. club-world-368 (Copa Sudamericana 2021), `subs_used`: 5 is kept. The club manual of March 2021 says 3, but the match report of the final (Gazeta Esportiva) shows five substitutions for each team in a match without extra time. The report is added to `sources`.

The notes of rows 311, 332, 336, 389 and 393 now say that the bench comes from the final's match rules and the substitutions from the regulations.

### Conflicts

- club-world-351 (CONCACAF Champions League 2020), `subs_used` and `bench`: the row says "5, a sixth in extra time" and 12. The regulations of January 2020 say a fourth substitute in extra time and seven substitutes on the bench. The final was played in December 2020 in Orlando after the pause. No page that I read states the rules of the December 2020 matches. Inference: the IFAB temporary five-substitute option explains the row's value. Values kept.
- club-world-355 (AFC Champions League 2020), `bench`: the row says 10. The regulations of October 2019 say seven players on the bench. The 2020 matches resumed in Qatar with five substitutions (The Peninsula, 13 September 2020), but no page that I read states the bench. Value kept.

### Points for the maintainer

- The 2026 Recopa regulations give 12 substitutes on the bench (art. 67), but the line-up clause of the same text says "11 titulares y 9 suplentes" in a list of 23. Row 464 keeps 12.
- Transfermarkt and worldfootball.net still block scripted reads. Archive.org was offline at times during the session.
