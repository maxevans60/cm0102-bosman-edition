# Rules table: Spain (part `spa`, slice 5)

Date 2026-10-16. Research only. Files: `spa.tsv` (216 rows), `spa-clubs.tsv` (912 rows), `spa.reconcile.tsv`, this note.

## Method

- Scope: the original's `spa_first`, `spa_second`, `spa_second_b`, `spa_lower`, `spa_cup`, `spa_super`, plus `spain_rules` for the nation rules row. One new competition: Segunda Federación (tier 4 from 2021/22, `comp_file` = `new`).
- Eight rows per season, 2001/02 to 2027/28: nation rules, La Liga, Segunda, tier 3, tier 4 (new), tier 4 or 5 (Tercera), Copa del Rey, Supercopa.
- Tier 3 keeps `spa_second_b` for all seasons: Segunda B to 2020/21, then Primera División RFEF (2021/22) and Primera Federación (2022/23 on). The build must turn 4 groups of 20 into 2 groups of 20 from 2021/22.
- Tercera rows use `spa_lower` as a stand-in. This is an inference: the original holds the clubs below Segunda B in one pool (423 clubs in 3.9.68). It runs no 17 or 18 regional groups.
- Facts come from raw Wikipedia wikitext (`action=raw`) and grep, read on 2026-10-16, and from RFEF rule books (PDF). Each row's `sources` column names the page for each field.
- 2026/27 uses the season pages that already exist. 2027/28 has no season pages: its rows copy 2026/27 and say so in `notes` (inference).

## Main sources

- Season pages: `YYYY–YY La Liga`, `Segunda División`, `Segunda División B`, `Primera División RFEF` or `Primera Federación`, `Segunda División RFEF` or `Segunda Federación`, `Tercera División` or `Tercera Federación`, `Copa del Rey`, and `YYYY Supercopa de España` (en.wikipedia).
- General pages: La Liga, Segunda División, Segunda División B, Primera, Segunda and Tercera Federación, La Liga play-offs, Supercopa de España, EU status (football).
- es.wikipedia "Primera División de España": 25 licences, non-EU history, 3 substitutes from 1995/96, 5 from mid 2019/20.
- RFEF rule books for Primera and Segunda: 2021/22, 2022/23, 2023/24, 2025/26 (bench 12, 5 substitutes, 3 non-EU in Primera, 2 in Segunda, 25 licences, 2025/26 windows). Primera Federación: 2023/24, 2025/26, 2026/27 (bench 12). Segunda Federación 2023/24 (bench 7, 5 substitutes).
- Club sites and press: FC Barcelona (June 2020: 23-man squads and 5 substitutes, 18 before), Calcio e Finanza (2020/21 kept 23 and 5), Goal (5 substitutes in 2021/22 including the Copa), Reuters via Yahoo and OK Diario (UK players non-EU from 1 January 2021).

## Change points found

| Season | Change |
|---|---|
| 2004/05 | Tercera from 17 to 18 groups; Segunda B play-offs from four mini-leagues of 4 to two-legged knock-out |
| 2005/06 | Primera non-EU places from 4 (3 on the pitch) to 3 |
| 2006/07 | Copa: round of 32 becomes two-legged; Segunda B play-out sends 2 clubs down (1 before) |
| 2008/09 | Segunda B group winners' route to promotion |
| 2010/11 | Segunda play-offs: 2 direct + 1 from 3rd to 6th |
| 2018/19 | Supercopa a single match (Tangier) |
| 2019/20 | Copa: 125 clubs, single matches to the quarter-finals; Supercopa 4 clubs in January (Jeddah); 5 substitutes and bench 12 from the June 2020 restart; Segunda B stopped, no relegation |
| 2020/21 | Segunda B 102 clubs in subgroups (transition); UK players non-EU from 1 January 2021; Supercopa in Spain |
| 2021/22 | Primera RFEF (2 x 20) and Segunda RFEF (5 x 18) start; Tercera becomes tier 5; Copa semi-finals lose away goals |
| 2022/23 | Names Primera, Segunda and Tercera Federación; Tercera groups of 16 |
| 2024/25 | Supercopa: a draw after 90 minutes goes straight to penalties |
| 2025/26 | Copa preliminary round two-legged; Primera Federación drops the title final |
| 2026/27 | Supercopa in Istanbul (February 2027); Tercera play-off drawn across groups from round 1 |

## Club moves

- La Liga (20) and Segunda (22) match the database count in every season. Segunda B (4 x 20) matches from 2001/02 to 2019/20, except 2012/13 (81 clubs, group III of 21) and 2013/14 (79 clubs, group I of 19).
- Club moves are listed for: Segunda B 2012/13 and 2013/14 (added in the reconcile, source StatsCrew group pages); Segunda B 2020/21 (102 real clubs, 80 in the database); Primera RFEF or Federación 2021/22 to 2025/26 (40 real clubs, 80 in the database divisions B1 to B4); Segunda RFEF or Federación 2021/22 to 2025/26 (90 clubs, new).
- The databases from October 2021 on put the 40 tier-3 clubs and about 40 tier-4 clubs into B1 to B4. Most other tier-4 clubs sit in "Spanish Lower Division". Some sit in "A Lower Division" (a pool shared with other nations) or "Reserve".
- December 2025 still holds 14 clubs of 2024/25 in B1 to B4 that went down to Tercera Federación in 2025 (for example Compostela, Cornellà, Linense). October 2024 holds 6 such clubs.
- Matching: exact name, then a reviewed name map, then a fuzzy match (all fuzzy pairs reviewed by hand). `db_club_id` is per database: ids differ between databases.
- Inferences in the name map, for Max to check: Mérida AD as "C.P. Mérida", Salamanca CF UDS as "U.D. Salamanca", Extremadura UD and CD Extremadura as "Extremadura C.F.", CD El Ejido 2022/23 as "Club Polideportivo El Ejido", Rayo Cantabria as "Racing Club de Santander B" (2021/22, 2022/23). The database reuses the record of an older club with the same town name.
- Reconcile additions: UD Logroñés 2012/13 as "C.D. Logroñés" (the old CD Logroñés record) and Yeclano Deportivo 2012/13 as "Yeclano C.F." are inferences of the same kind. 2012/13: the database holds all 80 real B1 to B4 clubs, and Orihuela CF (group III) comes from "Spanish Lower Division". 2013/14: "U.D. Salamanca" is in B1 of the database but did not play (dissolved in 2013).
- Ourense CF (the former Ponte Ourense) is not the successor of CD Ourense. Its match to "C.D. Ourense" is cleared in 2022/23, 2023/24, 2024/25 and 2025/26.

## Missing clubs (Max decides)

Segunda Federación 2025/26 (December 2025): no database record for UD Ourense, Reus FC Reddis, CD Castellón B, UE Porreres, Puente Genil FC.

Ourense CF has no database record of its own (no "Ponte Ourense" record in the October 2022 to December 2025 databases): Segunda Federación group 1 in 2022/23 and 2023/24, Primera Federación group 1 in 2024/25 and 2025/26. The "C.D. Ourense" record fits UD Ourense better (fan successor of CD Ourense, founded 2014). All other real clubs of the listed divisions have a record.

Tercera División and Tercera Federación (about 320 to 360 clubs a season) are not listed club by club. The original has no league at this level, only the `spa_lower` pool. Max decides if 0.6.0 needs these groups.

## Unknowns and conflicts

- Non-EU limit 2001/02 to 2004/05: es.wikipedia says the 2005 change cut the places from four to three, but the same page says Bosman cut them to three in 1995. The original's code (`SPAIN_RULES::fill_squad_restrictions` 0x859570, tests at 0x8595AD) gives the First Division and the competition with key id 0x9cf740 a non-EU limit of 4 in 2000 and 3 in every later season. Other Spanish competitions get 3 in 2001 and 2002 and 2 later. For the First Division, 3 agrees with the 1995 figure of the page (checked 2026-10-16; an earlier version of this note gave the limit of the other competitions).
- Segunda non-EU limit before 2008 and tier 3 to 5 limits after 2020: unknown.
- Bench before June 2020: 7 is an inference from the 18-man squad called the norm in June 2020. No source for 2001/02 to 2018/19.
- Segunda B, Tercera, Copa and Supercopa bench sizes: unknown. Substitutes there are inferences from the national rule.
- Segunda B 16th-place play-out: 1 club down in 2001/02 to 2005/06 and 2012/13 (reconcile). Other seasons keep 2, as the check found.
- 2020/21 Segunda B relegations to tier 5: 27 in the tables, 26 on the Tercera División RFEF page.
- Promotion counts from Tercera: known for 2003/04 (18), 2004/05 (17) and 2024/25 on (27). Other seasons unknown.
- Transfer windows: exact dates only for 2025/26 and 2026/27.
- Work permits: no source found; field set to unknown.
- Start date of the Cotonou (ACP) equal treatment in Spain: unknown; listed as of 2018.

## Reconcile

Date 2026-10-16. Each `disagree` row of `spa.check.tsv` got a third source from a publisher other than the first and second sources, read today. Each `found` value was checked against its page. The decisions are in `spa.reconcile.tsv`.

| Decision | Count |
|---|---|
| second | 18 |
| first | 1 |
| conflict | 0 |
| accepted (`found`) | 21 |

Third sources: Soccer-Spain season pages and reports (Segunda B play-offs and play-out 2001/02 to 2005/06), StatsCrew group pages (Segunda B 2012/13 and 2013/14), Última Hora (play-out 2012/13), Hora Jaén (Tercera Federación play-off), Digital de León (CD Laguna in group VIII), EFE via Infobae and hsmdeportes.com (Supercopa without extra time), Radio Marca Tenerife (no Primera Federación title final), Periódico de Ibiza (Supercopa 2027). For the 2001/02 and 2002/03 play-out I found no third publisher. I read the primary record instead: the semi-final and final match list on BDFutbol.

The one `first` decision: Tercera Federación 2024/25 clubs stays at 325. RFEF circular 46 planned 324, but a court ruling kept CD Laguna in group VIII, which played with 19 clubs.

Fixes that follow from the decisions (in the same or later rows):

- Play-out notes in spa-4, 12, 20, 28, 36 and 92 now say 1 club went down.
- spa-92 and spa-100 notes point to the new club moves.
- Tercera `up` and `playoffs`: spa-198 (2025/26 RFEF rules, same as 2024/25), spa-206 (2026/27 RFEF rules, new cross-group draw), spa-214 (copy of 2026/27, inference).
- Primera Federación `playoffs`: spa-204 (2026/27 RFEF rules, no title final), spa-212 (copy, inference).
- Supercopa: spa-200 `format` (no extra time, 2026 edition), spa-216 `format` (no extra time by inference; Saudi Arabia again in 2028). The extra-time note is replaced in spa-192, 200, 208 and 216. spa-208 note dates are now 2 to 7 February 2027.
- Bench 12 copied to spa-215 and spa-216 (inference).
- Copa `format` 2001/02 to 2007/08: "entrants unknown" replaced by the count.
- Nation rules spa-185, 193, 201 and 209: the tier 3 to 5 foreign rule of 2023/24 carried as an inference.
- `spa-clubs.tsv`: 160 rows added for Segunda B 2012/13 and 2013/14, and the Ourense CF match cleared in four seasons.

Not changed: Tercera rows 2020/21 to 2023/24 still say "count unknown" for promotion. Hora Jaén says 9 clubs went up through the 2023/24 play-off, but the check did not cover these rows.

No conflict is left for Max. One decision is open to a different reading: spa-190 uses the played count (325), and the RFEF bases give 324. Max can choose 324 if the game should follow the bases.

## Loose ends

- spa-158 (Tercera División 2020/21) `up`: 54 to the new Segunda División RFEF, 3 per group: the top 2 of each promotion group go up direct and one more through a play-off inside the group (AS, 15 September 2020).
- spa-166, 174, 182 (Tercera 2021/22 to 2023/24) `up`: 27, the 18 group champions direct and 9 through the national play-off. The 2nd to 5th of each group play a regional stage (single matches in 2021/22, two legs from 2022/23), and the 18 regional winners play 9 ties. Sources: Marca (8 June 2021) for 2021/22, and the English Wikipedia play-off pages for 2022/23 and 2023/24. The RFEF and regional press pages for 2023 and 2024 were not readable on 2026-10-16, so these two rows have no second publisher.
- spa-150 (2019/20) still says "count unknown" (not in this task).

## Source fill

Date: 2026-10-16. A source-fill pass added `field=URL` entries to `sources` for filled fact fields that had none. It changed no values and kept row order and ids. Each URL was read on this date, and the value was checked against the page text.

Method:

- For `name`, the pass took the pages that the row already cites, season pages first, and kept the first page that contains the name. For a name with a note in brackets, the base name had to match. For an Italian group (girone), the page also had to name that group.
- For a value of `none` in `split` or `playoffs`, the source is the season page with one league stage and no championship or relegation round.

Fields filled:

- `name`: 94 rows.
- `split` = `none`: 113 rows. spa-86 (2011/12 Tercera) cites the season page. That page names "relegation" only in table keys and notes.
- `split` of spa-158 (2020/21 Tercera, second phase by subgroup): the 2020/21 Tercera season page, which lists the first-phase subgroups and the second phase.
- `playoffs` = `none` for the First Division: 27 rows. The La Liga page says that the three lowest clubs go down and that the play-offs are for Segunda clubs only.
- `subs_used` marked as inference (3 to 2019/20, 5 from 2020/21): 28 rows. The Wikipedia page on substitutes gives three from 1995 and five from 2020.
- `bench` of spa-215 and spa-216 (12, the 2026/27 value): the RFEF 2026/27 competition rules (art. 36.1, "no podrá exceder ... de doce futbolistas").

Disagreements: none found.

Lint after the pass: 0 problems.
