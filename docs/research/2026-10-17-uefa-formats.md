# UEFA club competition formats by season (0.6.0 slice 22)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `src/recomp_rt/win/season_uefa.h`, `season_uefa.c` (Champions League, news, report), `season_uefa_cup.c` (UEFA Cup and Intertoto Cup), `season_uefa_lookup.c`, `season_uefa_table.c` (made by `tools/seasons/make_uefa_table.py`), `tools/recomp/uefa.py` (constant sites). Tests: ctest `season_uefa`, `season_uefa_table`, and the rules report (`tests/seasons/rules_report_check.py`, kind `format`).

## What the slice does

| Change point | Seasons | What the game plays |
|---|---|---|
| U1 | 2003/04 on | Champions League: one group stage of 8 groups of 4, then a knockout round of 16 (in place of the second group stage and the quarter-final draw of the original) |
| U2 | 2004/05 on | UEFA Cup: Q1, Q2 and a first round of 80, then 8 groups of 5 that meet once, then a round of 32 with the top three of each group and the 8 Champions League group thirds, then R16, QF, SF and the final |
| U3 | 2006 to 2008 | Intertoto Cup: 3 rounds, the 11 winners of the third round enter the UEFA Cup second qualifying round |

The format of a season comes from the table, not from tests of years. `season_uefa_row(S)` gives the row: none for the base game (start year 2001, changing rules off) and with `RECOMP_SEASON_CODE=off`; with changing rules on the row of S; with changing rules off the row of the start year (Q232). After the last row (2008/09 in this slice) the last row applies (Q248). Slices 23 to 25 raise `LAST` in the tool and add their seasons.

## The table

`tools/seasons/make_uefa_table.py` reads `docs/research/rules-0.6.0/uefa.tsv`: for each season the `european_cup`, `uefa_cup` and `intertoto_cup` rows. It takes the "Rounds (clubs):" list of the format (names Q1, Q2, Q3, QR, PO, R1 to R4, Groups, Groups 2, KO), the UEFA Cup group text ("N groups of M, single|double round robin") and the Intertoto round count and winners ("the 11 third-round winners enter the UEFA Cup second qualifying round"). A row it cannot read stops it. The ctest `season_uefa_table` checks that the committed C file is the one the table gives.

## Champions League (U1)

The original (EuropeanCup, vtable 0x96a1d0, Cup base) holds 13 stages: array +0xc, count +0x2c = 13 (0x583820), current +0x30 (-1 is the qualifying rounds of the Cup base). `advance_stage` 0x5851d0 draws phase 1 into stages 0 to 7 (0x585290), phase 2 into 8 to 11 (0x585ab0) and the quarter-finals into 12 (0x586330).

Why the new knockout stage is in slot 8 and not in slot 12: Cup base code walks the stages up to +0x30 with no NULL test (Cup::update_for_date 0x51f8da and 0x51f957, Cup::check_complete 0x521e2b, Cup::save_cup_to_disk 0x521573, Club_History::record_comp_season 0x44313b, the competition screens 0x49d49b, 0x48fe5d, 0x4950fc, menubar 0x764661), and Club_History reads a stage's +0x44 as its slot (0x443051). So the format holds 9 stages: groups in 0 to 7, the knockout (Cup_Stage, 16 clubs, 4 rounds, +0x44 = 8) in 8. The save keeps +0x2c and +0x30 (0x521535, 0x521541; load 0x520c48, 0x520c56), so a count of 9 marks the format in a loaded game.

The format is chosen when the class asks for its qualifying round table (create_round_table 0x583c50 with stage -1: from the constructor at 0x5838d4 and from set_up_rules 0x858e70 inside start_new_season 0x584d60, after the year moves). Wraps for a count of 9:

- `advance_stage`: at stage 8 the runtime draws the round of 16: each group runner-up meets the winner of another group, of another nation when a draw is found (club +0x53, the field 0x585bab compares); the runner-up is first in the tie (inference: the second club plays the second leg at home). Each club gets what phase 2 gave: bookings reset (0x7e9180), a coefficient point (0x9058b0), 1,200,000 (0x586065). The group thirds go to the UEFA Cup and the fourths are out, as 0x586282..0x58630b.
- The round table of the knockout: the original's 3 rounds (create_round_table stage 12) with a round of 16 before them, made from the quarter-final's record: code 20 ("Second Round": codes 10 to 150 are taken, 0x4b4850), flag 0x200 of +0xd (the ties are the list's pairs; the draw 0x51c410 skips the random draw at 0x51c5e4), 16 clubs, all new. First legs on 20 February, second legs 14 days later (+0x22), the weeks of the original's second group stage (0x58415a). The quarter-final loses flag 0x200, so its draw is random (map_fixture_tree 0x518dc6).
- `rank_teams` 0x586900 (predicted positions by bands, the clubs sorted by strength 0x4c08f0): the knockout bands come from the 4-round table, and the 2 bands of 4 of phase 2 (0x586aef) go.
- The stage argument 8 of `set_club_progress` 0x586c00, `build_fate_news_text` 0x584550 and `update_staff_records_for_status` 0x584ae0 becomes 12, the knockout of the original.
- The two reads of the knockout slot with offset 0x30 (0x584ad3 in record_season_honours, 0x584cf2 in the staff records) read `rt_uefa_cl_ko_offset` (0x20 for 9 stages).

## UEFA Cup (U2)

The original (Uefa_Cup, vtable 0x970790) is a Cup of one stage (+0x2c = 0 at 0x902344). Its entry table +0xa7 has 145 slots (0x902ba7): 118 seeded clubs (0x902cb0), qualifying clubs first (sort 0x522190 by entry byte = club +0x1df), then 3 slots for the Intertoto winners (count - 0x1b, 0x633deb), 16 for the Champions League third qualifying round losers (count - 0x18, 0x585a02) and 8 for the Champions League group thirds (count - 8, 0x586286). A round takes +0x1c new clubs from slot +0x1e.

The format keeps the entry table. Its round table (wrap of 0x9025c0, stage -1, from the constructor 0x9023cd and set_up_rules 0x9030b0) has Q1 (code 250), Q2 (260) and R1 (10). R1 and Q2 come from the row; Q1 is set so the three rounds take every slot before the thirds: Q1/2 + Q2/2 + R1 = count - 8. With 145 slots that gives Q1 50, Q2 64, R1 80 (2004/05), Q1 52 with Q2 62 (2005/06). The row's own Q1 count (62 to 74) needs another number of seeded clubs, which the original's place brackets fix at 118: see "Not done". When the UEFA Cup has clubs of the original's extra round (+0xb2, 0x902c85) the original format stays.

Then the object holds 9 stages, as the Champions League: 8 League_Stage groups (0x68b100: 5 clubs, 1 meeting, 5 match days on 21 October to 16 December; create_fixtures 0x6835c0 adds a resting place for an odd count, 0x683c1b) and a Cup_Stage of 32 clubs and 5 rounds (codes 30, 40, 0x78, 0x82, 0x96; 17 February to the final on 18 May). The group draw takes the 40 R1 winners (entry byte +5 = 1, as 0x585383), sorted by seed (0x586f60) into 5 pots of 8, one club of each pot a group, no two clubs of a nation in a group when a draw is found. The knockout draw: the 8 winners and 8 runners-up are seeded; the 8 group thirds and the 8 Champions League thirds (from their slots) meet them, not their own group, not their nation when a draw is found. The fourths and fifths are out (+0x1db = -1).

Wraps: slot 10 of the UEFA Cup is the shared empty function 0x5a8f60 (a `ret` of many vtables); its wrap acts only for a UEFA Cup object with 9 stages. Cup::load_from_disk 0x51fc00 makes the stage array of a UEFA Cup first (the constructor makes none), in a season game only. Cup::record_season_history 0x519a90 forwards a UEFA Cup with groups to its knockout stage, as 0x584ad0 does. Slot 17 0x9030e0 calls the Cup base 0x519890, which does nothing for a cup with stages (0x5198a3); the wrap clears +0x2c for that call, so the title holder (0x519974) and the staff records stay. A round index past the round table is not read there (0x5199e0).

## Intertoto Cup (U3)

The original (Intertoto_Cup, vtable 0x96bb8c) plays 5 rounds (0x633300): 40, 32 and 24 clubs in rounds 1 to 3 with 40, 12 and 8 new clubs of its 60 entrants. In this format the first three rounds are the cup: 40, 36 and 22 clubs with 40, 16 and 4 new clubs; the 11 winners of the third round (dates of the original: 17 and 24 July) go to the last 11 new slots of the UEFA Cup's Q2. The UEFA Cup's entry table grows to 153 slots (161 are allocated, 0x902b9d): the wrap of build_entrants 0x902b90 moves the seeded clubs after Q1 and the 20 seeded Q2 clubs up by 11 and makes the list +0x14 153 entries long (0x902dff makes it count entries). The hand-over (update_for_date 0x633d40) reads `rt_uefa_uic_back` and `rt_uefa_uic_winners` at 0x633deb and 0x633df3. With 153 slots the sizes are Q1 66, Q2 64 (20 seeded, 11 winners), R1 80.

## News (Q258)

The kind `uefa` of the season change writes one sentence for each change of format in a season with changing rules on, for example "The Champions League now has one group stage of 8 groups and a knockout round of 16."

## Rules report

`season_report.c` writes a `uefa` line for the Champions League and the UEFA Cup at each report: stage count, current stage, clubs of the group stages in stages 0 to 7 and after them, clubs and rounds of the knockout stage. The check compares the end line of each season with the "Rounds (clubs):" list of the row (kind `format`).

## Not done, for Max

1. The entry counts of the access list (Champions League 72 to 76 clubs, UEFA Cup Q1 62 to 74, Intertoto 49 to 50). The game keeps the original's place brackets by rank (`UefaSeeding::set_place_counts` 0x903cc0, `assign_champions_league_places` 0x904210, built from registers at 0x904219..0x9042ce) and the exact entrant counts the classes check (Champions League 71 at 0x584faa and the fill check 0x5851a1, UEFA Cup 118 seeded, Intertoto 60 at 0x6336db). The access list rows give places only for the 21 nations of the league table, so the brackets of the other nations need the rank rules of each season (`uefa.md`, "Points for Max" 1). Recommended: keep the original's brackets and counts to 2008/09 and encode the brackets with slice 23.
2. The name of the Champions League round of 16: the round names of the original have no "Round of 16" (0x4b4850). It shows as "Second Round". Recommended: keep it; a new name needs a new round code and a check of every compare of round codes.
3. Dates: the group stages of both cups keep the original's weeks where they can (Champions League groups to 7 November).
