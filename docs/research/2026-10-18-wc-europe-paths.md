# World Cup European qualifying with Nations League paths (0.6.0 slice 28, part D)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses, read with capstone from the PE. Brief: scratchpad `slice28-research-brief.md`. Method and the shared Cup facts: `docs/research/2026-10-18-nations-league-class.md` (named "class doc" below). Slice 27: `src/recomp_rt/win/season_intl_qual.c`, `season_intl.c`, block "World Cup qualifying 2006 to 2022" of `tools/recomp/seasons.py`. Slice 28 Euro paths: `src/recomp_rt/win/season_nl_euro.c`.

Class `wc_europe_league`: ctor 0x918200, thiscall (year, record) ret 8, vtable 0x970cc4 (written at 0x91823a). The only maker is 0x830990 in `create_other_int_comps`, with the game year [0xae2c92] (0x830963) and the record of key id 0x9cf780 (0x83096f). The World Cup object is `[0xadadfc][0x9cf79c]` (0x9189d2..0x9189de).

## 0. What the build needs first

1. **The 2022 campaign is +0x40 = 2020.** Slice 27's gate (`wc_year`, +0x40 + 2) gives 2022. In a 2022 game with no European host and no holder (slice 27), the exempt count +0xf6 is 0, so the class has 10 groups of 5, stage indices +0xff = 8, +0x100 = 9 (runners-up table), +0x101 = 10 (play-off), and 11 stage slots (0x91a0c2..0x91a0d0, 0x918348).
2. **The class has the same shape as `euro_champ_qual`** (class doc): the same round template layout, the same Cup_Stage play-off, the same slot 17 rule. The `season_nl_euro.c` method carries over: a wrap of the template 0x9185c0 and a wrap of the play-off builder 0x919910, gated on the vtable 0x970cc4 and the campaign 2020.
3. **The real group days move the end of the groups past the Asian tie.** Slice 27 calls `get_asia_play_off_team` 0x91def0 from Asia stage 12 (0x913e42, about 27 October 2021: slice 27 inference from 0x912482). 0x91def0 reads `stage[+0x101]` with no null test (0x91df07, 0x91df18). With the last group day on 14 to 16 November 2021 the play-off stage does not exist on that day. A wrap of 0x91def0 must return 0 for the path campaign. Slice 27's partner code then takes the CONMEBOL team or its fallback (season_intl_qual.c, `rt_wcq_asia_partner`).
4. **Path winners reach the World Cup only because slice 27's date shift moves the World Cup draw** (section 5). The path build must have the same gate as that shift (`season_intl_on`).

## 1. The vtable 0x970cc4 and the year

Slots that the season needs ("own" = not the League 0x96d020 pointer):

| Slot | Off | Function | Note |
|---|---|---|---|
| 2 | +0x08 | 0x919430 start_new_season | own; already wrapped by slice 16 (`start_919430`, season_starts.c) |
| 3 | +0x0c | 0x48ce10 Comp::all_stages_complete | base |
| 5 | +0x14 | 0x91a500 update_for_date | own |
| 8, 9 | +0x20, +0x24 | 0x91a9f0, 0x91ae80 load and save extra data | own |
| 10 | +0x28 | 0x919620 advance_stage | own |
| 11 | +0x2c | 0x48ce70 | base (gives 0, so the season pass calls slot 2) |
| 15 | +0x3c | 0x9185c0 create_schedule (round template) | own |
| 17 | +0x44 | 0x918980 update_staff_records_for_status | own |
| 23 | +0x5c | 0x91b1b0 set_predicted_positions | own |
| 26 | +0x68 | 0x91b510 set_club_progress | own |
| 30 | +0x78 | 0x91e1c0 (gives the World Cup record of 0x9cf79c) | own |
| 35 | +0x8c | 0x9191e0 set_up_rules | own |
| 37 | +0x94 | 0x91a7b0 update_positions_and_status | own |

Slot 1 is `EuroChampQual::add_result` 0x57ffd0 (shared with 0x96a0d4). Slot 12 is 0x90fbe0 (shared). The full dump of 45 slots matches the class doc table except slots 0, 2, 5, 8, 9, 10, 14, 15, 17, 18, 23, 26, 28 to 30, 35, 37 (WC Europe functions) and slot 11 (base here, own in EuroChampQual).

**Year +0x40.** The ctor writes the year argument (0x918240). When (year - 2000) % 4 = 1 it takes year - 1 (0x918251..0x9182a1), then adds 1 until (y - 2000) % 4 = 0 (0x9182a5..0x9182d9), at least 2004 (0x9182db..0x9182e6). So game years 2019, 2020 and 2021 give 2020, and 2022 gives 2024. For the 2022 World Cup the class holds **2020**.

**start_new_season 0x919430.** Frees the table, the template +0xba, the results list +0x10 and the stages, archives Comp_Stats (0x919494), adds 4 until +0x40 >= the game year (0x9194a6..0x9194c2), clears +0x45 and +0xfb, builds the pool (0x9194cf), a new stage array of +0x2c slots (0x91950c..0x919546), then slot 35 (0x919557). The season pass calls it on entry 0's update day (25 July, season-change doc) when slot 3 is true and slot 11 gives 0 (0x5d5cc6..0x5d5d1c). After paths that end in March 2022 that is 25 July 2022, and +0x40 becomes 2024, as in the original (inference).

## 2. The group round template (slot 15, 0x9185c0)

Thiscall (stage as a byte, short *rounds, short *code, int *is_league), ret 0x10. Group stage when stage <= +0xff (0x9185d2): is_league 1 (0x9185eb), rounds 10 (0x91860e), code 0x3e9 + stage (0x918608), 10 records of 0x41 bytes (`push 0x28a` 0x9185f8, malloc 0x944e46 at 0x918616). Each round: `league_round_set_date` 0x68a160 (cdecl: rec, index, day, month from 0, year offset, weekday, flag, year +0x40, 0) and `league_round_set_match_slot` 0x68a1c0 (rec, index, 0, 0, -1, -1, -1, 0).

| Round | Original day | Year offset | Weekday | Flag | Call |
|---|---|---|---|---|---|
| 0 | 2 September | 0 | 5 Sat | 1 | 0x918668 |
| 1 | 6 September | 0 | 2 Wed | 2 | 0x918691 |
| 2 | 7 October | 0 | 5 | 1 | 0x9186bb |
| 3 | 11 October | 0 | 2 | 2 | 0x9186e5 |
| 4 | 24 March | 1 | 5 | 1 | 0x918710 |
| 5 | 28 March | 1 | 2 | 2 | 0x91873b |
| 6 | 6 June | 1 | 2 | 2 | 0x918766 |
| 7 | 2 September | 1 | 5 | 1 | 0x918791 |
| 8 | 6 September | 1 | 2 | 2 | 0x9187bc |
| 9 | 7 October | 1 | 5 | 1 | 0x9187e7 |

The same days as EuroChampQual 0x57d190. The groups end on the Saturday nearest 7 October 2021.

**Real 2021 matchdays** (all year offset 1 from 2020): MD1 24 to 25 March, MD2 27 to 28 March, MD3 30 to 31 March, MD4 1 to 2 September, MD5 4 to 5 September, MD6 7 to 8 September, MD7 8 to 9 October, MD8 11 to 12 October, MD9 11 to 13 November, MD10 14 to 16 November. Source: the matchday table of goal.com and https://en.wikipedia.org/wiki/2022_FIFA_World_Cup_qualification_(UEFA) (search summary read 2026-10-18; MD1, MD3, MD10 and the September and October windows confirmed there; the single days of MD2, MD5, MD6 and MD9 need a second source).

**A wrap** of 0x9185c0 for a group stage of the 2020 campaign does what `nl_euro_qual_template` does for 0x57d190: write is_league 1, rounds 10, code 0x3e9 + stage, malloc 10 * 0x41, and call `season_nl_league_round` for ten days with year +0x40 and offset 1, then `rt_ret_stdcall(rec, 4)`. Round count 10 stays, so League_Stage needs no other change (arg 7, class doc section 3). A group of 5 rests one team a round (class doc section 3), so 10 rounds fit groups of 5 and 6. The runners-up table reads its date window from the template's first and last round (+0xba, 0x91a261..0x91a2b0), so it follows the new days.

The draw date is not in the template: set_up_rules 0x9191e0 sets `Date::Date(25, 2, +0x40, 5)` (pushes 0x919203..0x919216, call 0x919269), the Saturday nearest 25 March 2020, into +0xb6 (0x919275) and +0xb8 = 0 (0x919290). The real draw was 7 December 2020 (same sources). update_for_date draws only on that exact day (0x91a5ab..0x91a5bd) and returns before League::update_for_date on earlier days (0x91a57f, 0x91a599). A game that starts after the draw day never plays this campaign (inference from that test). With the real draw day a game that starts in July or August 2020 still plays it (inference). Moving the draw needs a wrap of 0x9191e0 (slot 35; direct call from the ctor at 0x9183a1, through the vtable from start_new_season at 0x919557) that writes +0xb6 and +0xb8 after the original. Optional for the brief.

## 3. Groups, sizes, draw, end

- **Pool** (`build_entry_pool` 0x919e30, from the ctor 0x918339 and start_new_season 0x9194cf): exempt +0xf6 = European hosts of year +0x40 + 2 (`find_hosts` 0x5fa730 at 0x919ee3, continent test 0x919f11) plus the holder (0x919f42, off in a season game by slice 27's 0x919F47). Pool size +0xf2 = 0x32 - exempt (0x919e41, 0x919f89). The pool array is 0xc8 bytes = 50 pointers (0x919fa9). It takes European nations in table order up to `cmp ebx, 0x32` (0x91a04a), returns 0 with no box when it finds fewer than +0xf2 (0x91a08c), then sorts by `compare_clubs_by_reputation_desc_nulls_last` 0x4b07d0 (0x91a0b0).
- **Groups:** exempt 0 gives +0xff = 8, so 10 groups (draw count +0xff + 2 at 0x918ee3): the League object and stage[0..8] (loop at 0x91a6e8..0x91a701, count 0xa at 0x91a6cf). Draw grid 6 slots a group (0x918ed1), group order list of 10 bytes on the stack (0x918f18..0x918f45), dealt until +0xf2 nations (0x918fdf).
- **Sizes** (`get_group_size` 0x918dd0, jump table 0x918eac): exempt 0 gives 5 for every group (0x918e9e). So 10 x 5 = 50. Exempt 1: 6 for indices 2, 4, 6. Exempt 2: 6 for 2, 4, 7. Exempt 3: 6 for all but index 2.
- **Real 2022:** 55 nations, 10 groups, 5 of 6 (F to J) and 5 of 5 (A to E). The class plays 10 of 5 and leaves out the 5 weakest nations of a 55-nation database (inference from the cap at 0x91a04a). The count of European nations in each season database: unknown. The real shape needs a wrap of 0x918dd0 and a pool of 55 (0x919e41, 0x919f89, 0x919fa9, 0x91a04a). Slice 27 lists these caps as left out. Not needed for the brief's paths.
- **Slice 27 changes** of this class: E1 (0x918923 in the template, 0x919a7e in the play-off) makes exempt 0 play a play-off of 6; E2 and E3 (0x919b4f, 0x919b5e) send no runner-up to Asia. The holder site 0x919F47 and the 2002 data gate 0x918473 also touch this class. Group sizes and counts are not changed.
- **Group winners** qualify by the rules +0xbe = 1, +0xbf = 0 (0x91924c, 0x919253): status 7, then slot 17 (0x91899b..0x9189e1). Unlike EuroChampQual there are no direct places for runners-up.
- **End of groups:** the last group round (9). The runners-up table (`create_runners_up_stage` 0x91a140: League_Stage of +0xff + 2 = 10 rows at 0x91a184, stage index +0x100, code 0x3fc at 0x91a1bb, +0x30 = +0x100 at 0x91a218) ranks the 10 runners-up by a SubLeague of the group's top four (0x91a300, `cmp edi, 4` 0x91a32c, `SubLeague` 0x88c6d0 with 4 at 0x91a392). The real rule left out only the results against the sixth of a group of six (inference from the regulations, not read here).

## 4. The play-off

- **When:** `advance_stage` 0x919620 adds 1 to +0x30 (0x9196a2) and calls `create_play_off_stage` 0x919910 at +0x100 or +0x101 (0x9196a8..0x9196b5). Only caller of 0x919910: 0x9196b5. After the draw +0x30 = 9 (0x91a218). League::update_for_date 0x6847c0 calls slot 10 when the stages are complete (class doc section 6), so the play-off is built when the groups end, with +0x30 = 10.
- **Picks (original):** with +0x101 not 0xff: N = 8 for exempt 0 or 1, else 6 (0x919a7e..0x919a94; slice 27 E1 makes exempt 0 give 6). It takes N + 1 rows of the runners-up table in rank order (`get_table_row` 0x6826d0 at 0x919b35), picks one at random for Asia (0x90cfc0 at 0x919b4f, slice 27 E2) and moves the last into its place (0x919b6a). Group rows of the list get status 1 (0x919bf8) and staff record `0x86bdd0` (team, record, 0, 0xa0, 0x1e) (0x919c03..0x919c3e). Other rows without status 7 get `add_failed_to_qualify_record` 0x86c1d0 (team, record, 0x41c, 0x64) and `clear_current_competition` 0x775000 (0x919c45..0x919c75).
- **Template** (stage +0x101, 0x918819): 1 round of 0x68 bytes (0x918842), code 0, is_league 0. Draw `cup_set_round_draw_date` 0x521e60 at 0x9188d3: Wednesday nearest 11 October, year offset 1. Match `cup_set_round_match_date` 0x521eb0 at 0x9188ea: Saturday nearest 11 November, offset 1. +7 = 0xa0, +0xd = 0, +0xf = 3, +0x17 = 0xa, +0x21 = 2 legs, +0x22 = 4 days (0x9188ef..0x918916). +0x18/+0x1a/+0x1c = 8/4/8 for exempt 0 or 1 (0x918954), else 6/3/6 (0x918934). Stage +0x100 gives 0 (0x918813).
- **Cup_Stage:** vtable +0x3c (+0x101) at 0x919d00, operator new 0xb2 (0x919d0a), ctor 0x522e00 at 0x919d53 with (this, N, list, rounds, record +4, template, year +0x40, +0x101, 4, code, 0x14, link 0, 0, 0, 0) (pushes 0x919d2d..0x919d50). Stored in stage[+0x101] (0x919d71), box "line 0x49f" when null (0x919dc5), `Cup::set_up_round` 0x51c800 (0) at 0x919dfd, +0x30 = +0x101 (0x919e09). Link mode 0 means random pairs (class doc 12.1).
- **Winners into the World Cup:** Cup::process_fixture 0x51a150 sends each result to Cup_Stage vtable +0x40 `Cup::add_team_result_news` 0x519690, which calls vtable +0x44 of the record's object (class doc 12.1). Slot 17 0x918980 with stage = +0x101 (0x918a36): status 1 gives staff record 0x32 (0x918a70), `world_cup::add_qualified_team` 0x92e740 (0x918a84) and `NationalTeams::add_qualified_nation` 0x7779b0 (0x918aa8); status 2 gives `add_knocked_out_record` 0x86c000 and `clear_current_competition` (0x918ab4..0x918ae6); other statuses give 0 (0x918aeb). Group stages: status 7 gives the same three calls with 0x64 (0x9189cd, 0x9189e1, 0x918a10). After the last match `mark_play_off_qualifiers` 0x91a7e0 (from slot 37 at 0x91a7d3) sets status 7 in the group rows of each team with +5 = 1 in the play-off team table (0x91a960..0x91a991). It reads `stage[+0x101]` with no null test (0x91a947).
- **Two rounds, status 1:** the same as EuroChampQual. Cup::process_fixture gives +5 = 1 only to the winner of a tie with no next tie (0x51af95..0x51afac); a semi-final winner goes to +0x40 with 0 (0x51b126) and a loser with -1 (0x51b23e). Slot 17 does nothing for 0 and -1 (0x918aeb). So **only the three path final winners get status 1 and `add_qualified_team`**. A semi-final loser gets no knocked-out record and no `clear_current_competition` (inference, as in the class doc). `mark_play_off_qualifiers` marks only the three final winners.

**The path play-off for 2022** (a wrap of 0x919910 for campaign 2020; other campaigns call the original):
1. Picks: the 10 runners-up (row 1 of each sorted group, as `qualified`/`group_rows` in season_nl_euro.c) and the 2 best 2020-21 Nations League group winners that are not in the top two of their group (`season_nl_held_results(2020, ...)` and the overall ranking, `season_nl_rank`). Real: Austria and Czech Republic. Without the Nations League tables (a game that started after that edition): the 2 best nations of the groups that are not in the top two (inference, as `group_rows`).
2. Seeds: the 6 best runners-up of stage[+0x100] (`get_table_row` order) are seeded and at home; the other 4 runners-up and the 2 Nations League nations are not. Real draw (26 November 2021, not confirmed by the search; second source needed): A Scotland v Ukraine, Wales v Austria; B Russia v Poland, Sweden v Czech Republic; C Italy v North Macedonia, Portugal v Turkey. A fixed order S1 v U?, S2 v U? per path, or a random draw of the unseeded, is a choice for Max.
3. Status: each of the 12 group rows gets status 1 and record 0x86bdd0 (team, record, 0, 0xa0, 0x1e). Every other row without status 7 gets 0x86c1d0 and 0x775000, as 0x919c45..0x919c75.
4. Template through vtable +0x3c (+0x101 = 10): the 0x9185c0 wrap gives 2 cup rounds of 0x68 bytes, as `path_round` in season_nl_euro.c. Round 0: +7 = 0x82, +0xd = 3, +0x21 = 1, +0x18 = 12, +0x1a = 6, +0x1c = 12, match 24 March, year offset 2. Round 1: +7 = 0x96, +0xd = 0x203 (bit 0x200: ties 2k and 2k + 1 go to final tie k, 0x5245c3), +0x21 = 1, +0x18 = 6, +0x1a = 3, +0x1c = 0, match 29 March, offset 2. Weekday -1 keeps the exact day. The draw date of each record can be before the build day (class doc 12.3).
5. Cup_Stage 0x522e00 as at 0x919d53 with N = 12, the list in pair order and **link mode 1**. Store at stage[+0x101], `Cup::set_up_round` (0), +0x30 = +0x101.
6. Winners: no change. Slot 17 and `mark_play_off_qualifiers` do the rest.

Real note: the path A semi-final Scotland v Ukraine and final Wales v Ukraine moved to 1 and 5 June 2022 (UEFA page in the search, final on 5 June). A path final on 5 June would come after the game's World Cup seeding day (section 5) and its winner would stay out. Keeping 29 March for all three paths is the safe choice.

## 5. The World Cup draw and late qualifiers

- `WorldCup::set_up_rules` 0x92d200: seeding `Date::Date(15, 11, +0x40, -1)` (pushes 0x92d234..0x92d247, call 0x92d290) into +0x18e (0x92d2a1) with year offset +0x190 = -1 (0x92d2b4). Draw `Date::Date(27, 11, +0x40, -1)` (call 0x92d2bd) into +0xb6 (0x92d2c9), +0xb8 = -1 (0x92d2e2). Original for 2022: seeding 15 December 2021, draw 27 December 2021.
- `WorldCup::update_for_date` 0x92f1a0 tests each day exactly: `seed_teams` 0x92f230 on +0x18e and +0x190 + +0x40 (0x92f1ad..0x92f1d6), `run_group_draw` 0x92e7f0 on +0xb6 and +0xb8 + +0x40 while +0xb1 is empty (0x92f1db..0x92f20e).
- **Slice 27 date shift:** 0x92d200 is wrapped (`wc_stages`, season_intl.c line 577). Every `Date::Date` in it moves by the days from 12 June (class_first, season_intl_rules.c line 144) to 20 November 2022 (season_intl_table.c line 66): 161 days. The year offsets -1 are immediates and do not move. So the seeding becomes 25 May 2022 and the draw 6 June 2022 (inference: computed, not tested). The real draw was 1 April 2022. **Path finals on 29 March 2022 come before the seeding.**
- **A late qualifier:** slice 27 wraps `seed_teams` 0x92f230 (`wc_seed_teams`): it fills every slot with no qualifier from the default nations, then by reputation, and sets the count to 32. Slice 27 wraps `add_qualified_team` 0x92e740 (`wc_add_qualified`): when the count +0x36 reaches the room +0x38 the team stays out. So a qualifier after 25 May 2022 stays out. With `season_intl_on` false there is no shift, the seeding is 15 December 2021, and the path winners would stay out (they would arrive after it). The total of 13 European places (10 + 3) is the same as slice 27's play-off of 6 (E1).

## 6. The smallest set of wraps

Gate for all: `season_comps_on()` and `season_intl_on()`, vtable 0x970cc4 at +0, campaign +0x40 = 2020. Other objects and campaigns call the original.

| Site | Kind | What |
|---|---|---|
| 0x9185c0 create_schedule (slot 15) | new RT_WRAP | Group stages (stage <= +0xff): 10 rounds on the real 2021 days, offset 1. Play-off stage (+0x101): 2 cup rounds, 12 then 6, single matches 24 and 29 March, offset 2, final with 0x203. |
| 0x919910 create_play_off_stage | new RT_WRAP | The 10 runners-up and 2 Nations League nations, statuses and records, Cup_Stage of 12 with link mode 1. |
| 0x91def0 get_asia_play_off_team | new RT_WRAP | Give 0 for the 2020 campaign (no European runner-up for Asia, and no read of a stage[+0x101] that is not built yet). Slice 27's call at 0x913e42 goes through f_0091def0 (rt.h line 285), so the wrap catches it (inference). |
| 0x9191e0 set_up_rules (slot 35) | optional RT_WRAP | Draw day 7 December 2020: +0xb6 day of the year, +0xb8 = 0, after the original. |

Group winners direct: no wrap (+0xbe = 1, slot 17). Winners into the World Cup: no wrap (slot 17, 0x918a84). The World Cup dates: no wrap (slice 27's shift).

Sites that other slices own (do not wrap or patch them again):
- Slice 27 INSNS in this class: E1 0x918923 (inside 0x9185c0), E1 0x919a7e, E2 0x919b4f, E3 0x919b5e (inside 0x919910), the holder 0x919F47, the 2002 gate 0x918473. The new wraps sit on the function entries, and the original body o_ keeps these patches for other campaigns (inference from rt.h line 285: the recompiler writes the translation as o_<addr>). For campaign 2020 they no longer run.
- Slice 27 wraps: 0x92d200, 0x92c290, 0x549f70, 0x92f230, 0x92e740; INSNS 0x913E42 (Asia stage 12 calls 0x91def0).
- Slice 16: 0x919430 start_new_season (`start_919430`, season_starts.c).
- Slice 28 Euro: 0x57d190, 0x57ef50 (other class).

Open: whether `stage[+0x101]` of an unbuilt stage is 0 or garbage (the stage array is a plain malloc 0x944e46 at 0x91838c and 0x919541, not cleared): unknown, so the 0x91def0 wrap is needed in either case. The Asian stage 12 date with the real Asian calendar: unknown (slice 27 inference only).
