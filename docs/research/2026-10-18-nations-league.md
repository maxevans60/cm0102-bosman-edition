# The Nations Leagues and the Euro qualifying paths (0.6.0 slice 28)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md`, slice 28, Q169, Q248, Q277, Q278, Q288, Q368. Build brief: `docs/research/build-brief-0.6.0.md`, slice 28.

Research of this slice:

- `2026-10-18-nations-league-class.md` (named "class doc"): the class `euro_champ_qual` as the base of a Nations League.
- `2026-10-18-national-team-comps.md` ("nations doc"): how the game treats a competition of national teams, the free records.
- `2026-10-18-gold-cup-class.md` ("gold cup doc"): the class `copa_america` for a Gold Cup, and the class for a Finalissima.
- `rules-0.6.0/nations-league-tiers.tsv`: the real league and group of each nation in each edition (two sources a row; both Wikipedia pages, the edition page and the league page).
- `rules-0.6.0/nations-league-dates.tsv`: the draw, matchday, finals and play-off dates of each UEFA edition.

Code: `src/recomp_rt/win/season_nl.h`, `season_nl_rules.c` (the rules, no game memory), `season_nl_table.c` (made by `tools/seasons/make_nl.py`), `season_nl.c` (the game). Tests: `tests/seasons/test_season_nl.c` (ctest `season_nl`), ctest `season_nl_table`, `tests/seasons/season_nl_run.py`.

## The rules (no game memory)

- **Editions.** `make_nl.py` holds one line per edition with the sizes, group counts and moves that the row's text gives, and checks them against the row (season, the total of the clubs column, each size in the format). The 2022-23 editions start on 1 and 2 June 2022, so their season is 2021 (the season of the first match, Q276).
- **Overall ranking.** By league, then group place, then points, goal difference and goals, then nation id (UEFA regulations, overall ranking). Inference: the results against the fourth of a group of four are not left out, because the game's table has no such column. The game reads the points of the table (row +0x19, the overall table of the comparator 0x683466) and leaves goal difference and goals at 0 (their row fields are not read).
- **Leagues of the next edition.** The group winners go up, the last of each group goes down where the edition says so, and the lowest of the ranking where a play-out decides (2 of League C in 2020-21, 1 in 2022-23, 2 in 2024-25; the game plays no play-out). Then each league fills to the size of the next edition from the ranking. The test gives the real 2020-21 leagues from the 2018-19 places.
- **Play-off paths.** Each path takes the group winners of its league that did not qualify, then the other nations of its league by the ranking; the places that a league cannot fill go to the best free nations of the ranking, path A first. Semi-finals 1 v 4 and 2 v 3. The test gives the four real Euro 2020 paths and their semi-finals from the 2018-19 ranking.

## The game

See the head of `season_nl.c` for the design: one `euro_champ_qual` object on the record "Asian Cup Qualifying" in entry 1; the League object is group A1 and the stages are the other groups; the last stage is the finals of four.

### Code sites

| Site | What | Evidence |
|---|---|---|
| wrap 0x57f600 | collect_entrants: nothing for the Nations League | class doc 2, called at 0x57fe35 |
| wrap 0x57dab0 | draw_groups: the groups of the edition into the grid +0xf7 | grid read at 0x57edbe..0x57eddb and 0x57e8a3 |
| wrap 0x57d9d0 | get_group_team_count: the size of each drawn group | called at 0x57e7eb, 0x57ed62 |
| wrap 0x57f920, 0x57fa10 | no runners-up stage or table; +0x30 = the last group | 0x57ff50, 0x57ff9c |
| INSNS 0x57ff34 | `cmp edi, 8`: G - 1 group stages | 0x57ff28..0x57ff37 |
| wrap 0x669ae0 | the record is international (fixture type 1) | nations doc 1, point 4 |
| vtable copy slot 2 | start_new_season: keep the tables, move to the next edition | the pass calls +0x8 at 0x5d5d1c |
| slot 5 | update_for_date: the draw of a new edition, then 0x57fd60 | 0x57fd60 draws only on its draw day (0x57fe0e..0x57fe2d) |
| slot 10 | advance_stage: the finals of four | base update calls +0x28 at 0x6849e0 |
| slot 12 | history from the finals stage, then 0x90fbe0 | Euro thunk 0x40f590; Cup slot 12 0x519a90 |
| slot 15 | round templates: six matchdays, the finals | 0x57d190, league_round_set_date 0x68a160 |
| slots 14, 17, 18, 23, 26, 28, 29, 30, 37 | the League base (0x96d020): no Euro qualification, news, predictions or status 7 | class doc 10 |

### The save

No new field. The object saves and loads as every League object (League::save 0x6891c0 writes the stage count +0x2c and the stages up to +0x30 at 0x6898dc..0x6899f1; the constructor returns after a load at 0x57cf28). The module makes the object again at a load from its file (season_comps.c). After an edition ends, start_new_season keeps its tables, so the leagues of the next edition come from them at the next draw, also after a save and a load.

## Decisions for Max to confirm

1. **Records.** The UEFA Nations League takes the record "Asian Cup Qualifying" (record 444 in 3.9.68 and in every Update Team database checked), the only nation competition record that no code reads and that has no history (nations doc 5.5). The record gets the name of the table through the module. The CONCACAF Nations League, the Gold Cup and the Finalissima need new records, which the shared module cannot make yet (season_comps open point 1): not built in this slice.
2. **Not a tournament.** The Nations League is not in the tournament lists of `national_teams` (0x76d940 and the eight inline lists): a tournament holds its squad from 10 days before the first match until the team goes out (0x76c31b), which would keep players from their clubs from September to November (nations doc 3.4). It counts as an international competition (0x669ae0, ranking weight 1.1 as the Euro qualifiers).
3. **Play-outs and promotion play-offs** (League C play-outs 2022 and 2024; the A/B and B/C play-offs of 2025 and 2027; the 2024-25 and 2026-27 quarter-finals) are not played. The lowest of the ranking go down in place of the play-out losers; the play-off and quarter-final places keep their league and the finals take the four League A group winners.
4. **The finals.** Semi-finals A1 v A4 and A2 v A3 (no draw), and no third place match.
5. **The 2020-21 finals** play in June 2021 (2 and 6 June), the planned time before Euro 2020 moved to 2021 (Q277).
6. **A game that starts after an edition's league phase** (for example 2021/22, before the 2020-21 finals) starts with the next edition. The finals of the edition before are not played.
7. **Russia** plays to 2020-21 and is left out from 2022-23, as it was in real life (Q293, Q365).
8. **After the table (2028-29 on)** the 2026-27 edition is played again every two years (Q248: the rules stay). With changing rules off the edition of the start season is played again every two years (Q232).
