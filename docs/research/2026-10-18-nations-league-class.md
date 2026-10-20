# EuroChampQual as the base of a Nations League (0.6.0 slice 28, part A)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Read with capstone from the PE. Brief: scratchpad `slice28-research-brief.md`. Shared module: `src/recomp_rt/win/season_comps.h`, `docs/research/2026-10-18-new-comps.md`. Round record: `docs/research/2026-10-17-cup-round-record.md` (named "round doc" below).

Class `euro_champ_qual`: ctor 0x57ce10, thiscall (year, record) ret 8, object size 0x102 (operator new at 0x830d15), vtable 0x96a0d4, League vtable 0x96d020 (written at 0x68227e). The only maker is `create_other_int_comps` at 0x830d6c with the record of key id 0x9cf784.

## 0. What the build needs first

1. **The class has 9 groups, not 10.** The draw makes `+0xff + 2` = 9 groups (0x57dabe..0x57dac9). The League object is group 1 (draw group 0), stage[0..7] are groups 2 to 9 (eight calls at 0x57ff28..0x57ff37, bound `cmp edi, 8` at 0x57ff34), stage[8] is the runners-up table of 9 rows (0x57f9bc), stage[9] is the play-off Cup_Stage. The sweep's "10 groups" counted the stage array (10 slots, 0x57d00b), not the groups.
2. **Group sizes 5 and 6 come from `get_group_team_count` 0x57d9d0**, called with the draw group index minus one (0x57e76b `dec al`). One host (+0xf6 = 1, 0x57d9de): indices 0, 2, 5 have 6 (0x57da7b..0x57da9b), the rest 5. So 3 groups of 6 and 6 of 5 = 48 places. Two hosts (0x57d9e5): indices 0, 2, 5, 7 have 6, so 49 places. The pool is 0x33 minus the hosts (0x57f76f): 50 with one host. **With one host (and with no host, which slice 27 makes one host at 0x57f768) the two weakest nations of the pool get no group** (inference from the draw loop 0x57e6f1..0x57e7ce, which stops when every group is full). The slice 27 note "room for 54" is wrong.
3. **Every club picker of the class is a direct call, reached from vtable slots.** A vtable copy that replaces slot 5 (`update_for_date`) never reaches `collect_entrants`, `draw_groups`, `init_group_one_table`, `create_group_stage`, `create_runners_up_stage` or `update_runners_up_table`. A copy that replaces slot 10 (`advance_stage`) never reaches `create_playoff_stage`. So a second object needs no wrap keyed on `this` for those. Wraps are still needed for the two calls in the constructor (section 2, "Constructor traps").
4. **The constructor runs with the original vtable.** `season_comps` writes the copy after the constructor, but the constructor calls `League::load_from_temp_file` 0x687b10 (0x57cf21), which calls vtable +0x20 at 0x687ccd, so a load always reads extra data through `load_extra_data` 0x580130. A new edition calls `set_up_rules` 0x57e8f0 directly (0x57cf30), which calls vtable +0x3c at 0x57e9b1, so the League object's own round template is the Euro qualifying template of 10 rounds. The make function must rebuild +0xba (and the draw date +0xb6) after it writes the copy.
5. **One object with all leagues as groups is easier** than one object per league (section 10).

## 1. The vtable 0x96a0d4 (45 slots)

"Base" = the same pointer as League 0x96d020. "Own" = the class's function (or a shared function that League does not use).

| Slot | Off | Function | Kind |
|---|---|---|---|
| 0 | +0x00 | 0x57d080 scalar deleting destructor | own |
| 1 | +0x04 | 0x57ffd0 add_result | own |
| 2 | +0x08 | 0x57eb20 start_new_season | own |
| 3 | +0x0c | 0x48ce10 Comp::all_stages_complete | base |
| 4 | +0x10 | 0x689ad0 League::check_complete | base |
| 5 | +0x14 | 0x57fd60 update_for_date | own |
| 6 | +0x18 | 0x687b10 League::load_from_temp_file | base |
| 7 | +0x1c | 0x6891c0 League::save_to_temp_file | base |
| 8 | +0x20 | 0x580130 load_extra_data | own |
| 9 | +0x24 | 0x580410 save_extra_data | own |
| 10 | +0x28 | 0x57ec60 advance_stage | own |
| 11 | +0x2c | 0x57eb10 skip_season_reset | own |
| 12 | +0x30 | 0x90fbe0 (shared with WcAfricanCup; calls `NationalTeams::clear_competition_for_comp_clubs` 0x775420 with the record) | own |
| 13 | +0x34 | 0x583470 shared no-op | base |
| 14 | +0x38 | 0x57e9d0 write_progress_text | own |
| 15 | +0x3c | 0x57d190 create_schedule (round template) | own |
| 16 | +0x40 | 0x686670 League::announce_team_status (calls +0x44 at 0x6866ea) | base |
| 17 | +0x44 | 0x57d5b0 update_staff_records_for_status | own |
| 18 | +0x48 | 0x57d700 build_fate_news_text | own |
| 19 | +0x4c | 0x48dfa0 Comp::get_club_label_text | base |
| 20 | +0x50 | 0x48e180 Comp::get_table_layout_type | base |
| 21 | +0x54 | 0x68a3e0 League::get_match_importance | base |
| 22 | +0x58 | 0x48e1c0 Comp::collect_club_list | base |
| 23 | +0x5c | 0x5806d0 set_predicted_positions | own |
| 24 | +0x60 | 0x48e360 Comp::counts_for_fixture_clash | base |
| 25 | +0x64 | 0x5bb550 return 1 | base |
| 26 | +0x68 | 0x580980 set_club_progress | own |
| 27 | +0x6c | 0x583470 shared no-op | base |
| 28 | +0x70 | 0x580b50 get_qualification_status | own |
| 29 | +0x74 | 0x580b80 get_qualification_status_change | own |
| 30 | +0x78 | 0x57d060 get_finals_comp (record of key 0x9cf7a4) | own |
| 31 | +0x7c | 0x48f2d0 Comp::set_fixture_substitutes | base |
| 32 | +0x80 | 0x68abd0 League::contains_club | base |
| 33 | +0x84 | 0x68ac40 League::update_projected_positions | base |
| 34 | +0x88 | 0x5dce00 return false | base |
| 35 | +0x8c | 0x57e8f0 set_up_rules | own |
| 36 | +0x90 | 0x68aad0 League::points_for_result | base |
| 37 | +0x94 | 0x580010 update_positions_and_status | own |
| 38 | +0x98 | 0x684790 League::has_automatic_promotion | base |
| 39 | +0x9c | 0x6847b0 League::has_relegation | base |
| 40 | +0xa0 | 0x687a20 League::get_club_array | base |
| 41 | +0xa4 | 0x68aa30 League::record_final_positions | base |
| 42 | +0xa8 | 0x4011e0 League::get_fixed_draw_slot | base |
| 43 | +0xac | 0x4011f0 shared no-op ret 8 | base |
| 44 | +0xb0 | 0x689c20 League::process_promotion_relegation_chain | base |

`League::update_for_date` 0x6847c0 (called at the end of slot 5, 0x57ffac) calls +0x10 (0x6849be), +0x28 advance_stage (0x6849e0) and +0x30 (0x684a8f).

## 2. The functions: what they read and write

Direct calls with their callers (all inside the class unless named):

| Function | Called from |
|---|---|
| get_group_team_count 0x57d9d0 | 0x57e774 (draw), 0x57e7eb (group 1 table), 0x57ed62 (group stage) |
| draw_groups 0x57dab0 | 0x57fe3c (update_for_date) |
| init_group_one_table 0x57e7e0 | 0x57fe43 |
| create_group_stage 0x57ed10 | 0x57ff2e, 0x580fd2 (2002 history) |
| create_playoff_stage 0x57ef50 | 0x57ecf5 (advance_stage) |
| collect_entrants 0x57f600 | 0x57fe35 |
| create_runners_up_stage 0x57f920 | 0x57ff50, 0x580feb |
| update_runners_up_table 0x57fa10 | 0x57f9ee, 0x57ff9c, 0x5831ae, 0x5831c8 (add_historic_fixture) |
| mark_qualified_teams 0x580040 | 0x580033 (slot 37) |
| create_groups_and_results 0x580bd0 | 0x57d025 (constructor) |
| add_historic_fixture 0x582ef0 | about 51 calls inside 0x580bd0 (from 0x581036) |
| set_up_rules 0x57e8f0 | 0x57cf30 (constructor), and slot 35 from start_new_season 0x57ebef |
| EuroChamp::add_qualified_team 0x57c120 | 0x57d609, 0x57d6a0 (slot 17 only) |

Per function (reads, then writes outside the object):

- **Constructor 0x57ce10.** Year: `+0x40` = smallest y from (year - 1) with (y - 1990) % 4 = 0 (0x57ce4c..0x57cedf). Fixed bytes: +0x42 = 4, +0xf6 = 1, +0xff = 7, +0x100 = 8, +0x101 = 9 (0x57ce6b..0x57ceb0), +0xd9 = 0x24, +0x51 = 0xf. Results list +0x10 (LLIST 0x68d020). Load from file (0x57cf21), else set_up_rules (0x57cf30), Comp_Stats 0x5ce bytes (0x57cf97). +0x2c = 10 (0x57d00b), stage array 0x28 bytes (0x57cffe, 0x57d012). **Trap:** when the year argument is greater than +0x40 (0x57d01a), it calls `create_groups_and_results` 0x580bd0, which writes the 2002 groups of 49 fixed key nations and their 2002 and 2003 results, whatever the year. That happens for year arguments 2003, 2007, ..., 2023 (y - 1 = +0x40).
- **create_schedule 0x57d190** (slot 15). Reads +0x40, +0xff, +0x100, +0x101, +0xf6. Section 4.
- **update_staff_records_for_status 0x57d5b0** (slot 17). Stage below +0x101 and status 7: staff record `add_qualified_for_round_record` 0x86bdd0 (on 0xdd7f00, value 0x64) with the record of 0x9cf7a4, `add_qualified_team` on `[0xadadfc][0x9cf7a4]` (0x57d609), `NationalTeams::add_qualified_nation` 0x7779b0 (on 0xb74318). Play-off stage and status 1: the same staff record (0x28) and `add_qualified_team` (0x57d6a0). Status 2: `add_knocked_out_record` 0x86c000 and `clear_current_competition` 0x775000.
- **build_fate_news_text 0x57d700** (slot 18). Group news through `comp_util_write_group_qualify_news` 0x4b4590, tie news through 0x4b0b80. Links name the Euro record of 0x9cf7a4 (0x57d83c, 0x57d8cc, 0x57d992).
- **get_group_team_count 0x57d9d0.** Reads +0xf6 only; any other value shows box "euro_champ_qual.cpp 581" (0x57da3d) and returns 5.
- **draw_groups 0x57dab0.** Section 5.
- **init_group_one_table 0x57e7e0.** +0x3e = size(-1); table +0xb1 of 0x3b-byte rows; +0xa7 = (teams - 1) * +0x3c (matches per team, 0x57e892); rows from draw slots 0..5 (+0xf7); `send_group_draw_news` 0x684230.
- **set_up_rules 0x57e8f0** (slot 35). Rule bytes +0xc2 = 3 (win points), +0xc3 = 1 (draw), +0xc4 = 0xa, +0xc5..+0xc8 = 1, 2, 4, 5 (tie-break list, inference), +0xbe = 1, +0xbf = 1 (inference: direct places and play-off places of a group, see section 6), +0x3c = 2 (home and away), +0x49 = 7 (bench). Draw date `Date::Date(10, 2, +0x40, 5)` = Saturday nearest 10 March of the campaign year, kept as day +0xb6 and year offset +0xb8 = 0 (0x57e913..0x57e9aa). Template: vtable +0x3c (-1, &+0xa9, &+0x3a, 0) into +0xba (0x57e9b1).
- **write_progress_text 0x57e9d0** (slot 14). Texts 0x99b800, 0x9a751c, 0x9c48a4 by stage code 0x3e8..0x3f5 and status 7 or 1.
- **skip_season_reset 0x57eb10** (slot 11). Writes +0xb5 = 1, returns 1.
- **start_new_season 0x57eb20** (slot 2). Section 7.
- **advance_stage 0x57ec60** (slot 10). Box "line 0x38b" when +0x30 >= +0x2c - 1. Else +0x30 + 1; at +0x100 or +0x101 it calls create_playoff_stage (0x57ece8..0x57ecf5).
- **create_group_stage 0x57ed10** (group i, list, count) ret 0xc. Section 3.
- **create_playoff_stage 0x57ef50.** Section 6.
- **collect_entrants 0x57f600.** Section 5.
- **create_runners_up_stage 0x57f920.** League_Stage of 9 rows, no template (0x57f980..0x57f9c1), stage index +0x100, rules 1, 2, 5, 0; +0x30 = +0x100; fills it at once (0x57f9ee).
- **update_runners_up_table 0x57fa10.** For the 9 groups (-1 to 7, bound 0x20 at 0x57fcb3): sort the group, take the runner-up (row 1), build a `SubLeague` 0x88c6d0 of the group's top four from the results list +0x10 in a date window (first to last group round + 1 week, or from 1 June 2002 for the 2002 campaign, 0x57fa5a), copy the runner-up's row into runners-up row g, sort the runners-up table.
- **update_for_date 0x57fd60** (slot 5). Section 6.
- **add_result 0x57ffd0** (slot 1). Keeps a copy of each result of a stage up to +0xff in +0x10, then League::add_result.
- **update_positions_and_status 0x580010** (slot 37). Base, then once (+0xfb) when check_complete: **mark_qualified_teams 0x580040** reads stage[+0x101] (+0xa7 entries, byte +5 = 1) and sets status 7 in the group rows. It reads `[+0xc][+0x101]+0x3e` with no null test (0x580097..0x58009a).
- **load_extra_data / save_extra_data 0x580130 / 0x580410** (slots 8, 9). The results list +0x10 (0x4f-byte items) and two more fields (inference from the map: seeding byte and the flag +0xfb; not traced).
- **set_predicted_positions 0x5806d0** (slot 23). Sorts the pool +0xee by `compare_clubs_by_reputation_desc_nulls_last` 0x4b07d0, calls `NationalTeams::release_competitions_of_continent` 0x775070 (Europe, 1) at 0x580787, then `Comp_Stats::set_club_final_position` 0x4a2540 in bands of 9 (codes 1, 10, 13, 16, 19, 28, 37, 46 by rank, two tables for +0xf6 = 1 or 2) and `NationalTeams::set_current_competition` 0x775220 for every pool nation. The pool must still exist: update_for_date calls it at 0x57ff59, before it frees +0xee (0x57ff63).
- **set_club_progress 0x580980** (slot 26). Group position p becomes code 1 + 9 * (p - 1) (jump tables 0x580b00, 0x580b18, 0x580b30) in Comp_Stats entry +0x73, +0x74, +0x75; play-off 1 and 2 become 0xd and 0x10.
- **get_qualification_status 0x580b50, change 0x580b80** (slots 28, 29). Comp_Stats stats 0x12 and 0x13 against 14.
- **create_groups_and_results 0x580bd0, add_historic_fixture 0x582ef0.** The 2002 campaign only; add_historic_fixture accepts fixture years 2002 and 2003 only (0x582f4a, 0x582f54).

Key id globals: 0x9cf7a4 (Euro finals) at 0x57d060, 0x57d5cb, 0x57d5fa, 0x57d619, 0x57d662, 0x57d691, 0x57f6a5, 0x57d83c..0x57d992. 0x9cf784 only in the maker (0x830d51). Europe 0x9cfa10 at 0x57f6da, 0x57f82e, 0x580762. Game year [0xae2c92] at 0x57eba9 and in the maker. Today [0xae2c90] at 0x57f32b. Start date [0xdd6ec8] at 0x57f30c. Hosts: `HostCountry::find_hosts` 0x5fa730 on [0xb5e134] with key 0x9cf7a4 and year +0x40 + 2 (0x57f6ac).

## 3. Groups

- Stage array +0xc, 10 slots of 4 bytes (0x57cffe), count +0x2c = 10 (0x57d00b), current stage +0x30. Group stages: the League object (index -1) and stage[0..7]. The last group stage index is +0xff = 7, the runners-up stage +0x100 = 8, the play-off +0x101 = 9 (0x57cea2..0x57ceb0).
- Immediates that fix the group count: +0xff = 7 (0x57cea2) and `add bl, 2` (0x57dac9) give 9 groups; `cmp edi, 8` (0x57ff34) gives 8 stages; League_Stage of 9 rows (0x57f9bc); loop bound 0x20 (0x57fcb3); pots 6 (0x57e7c8); draw slots 6 a group (`mov cl, 6` 0x57dabb, `lea ecx, [edi + edi*2]` 0x57e788). Sizes: 6 at 0x57da9b, 5 at 0x57da6e and 0x57da8e.
- `create_group_stage` 0x57ed10 (group i, list, count): template = vtable +0x3c (i, &rounds, &code, 0) (0x57ed49). When list is 0 it takes size(i) teams from draw group i + 1 (`+0xf7 + (i + 1) * 24`, 0x57edbe..0x57eddb). League_Stage 0xee bytes (0x57ede1), ctor 0x68b100, 22 arguments (ret 0x58), pushed at 0x57ee0b..0x57ee5a:

  | Arg | Value | Field of the stage |
  |---|---|---|
  | 1 | this (parent) | |
  | 2 | team count | +0x3e |
  | 3 | team list | rows +0xb1 (0x68b375) |
  | 4 | 2 | +0x3c, matches per pair |
  | 5 | record +4 | +4 |
  | 6 | template | copied to +0xba (0x68b26f..0x68b2d9); 0 makes one empty round |
  | 7 | round count | +0xa9 |
  | 8, 9, 10 | +0xc2, +0xc3, +0xc4 | same fields |
  | 11 | &+0xc5 (4 bytes) | +0xc5..+0xc8 |
  | 12 | &+0xbe (4 bytes) | +0xbe..+0xc1 |
  | 13 | year +0x40 | +0x40 |
  | 14 | group index i | +0x44 |
  | 15 | round code (0x3e9 + i) | +0x3a |
  | 16 | +0x51 (0xf) | +0x51 |
  | 17 | 4 | +0x42 |
  | 18 | 0 | +0xab |
  | 19 | +0xd9 (0x24) | +0xd9 |
  | 20 | -1 | +0xa7 |
  | 21 | 0 | +0x28 |
  | 22 | 2 | +0xe1 |

  Then +0x30 = i and `send_group_draw_news` (0x57ef25..0x57ef2b). The stage then calls `League::create_fixtures` 0x6835c0 (0x68b3b9).
- **An odd group works.** `create_fixtures` rounds the team count up to even (0x6835e9..0x6835fc), so one team rests each round. The class already plays groups of 5 this way (10 rounds for 5 and for 6 teams). A group of 3 needs 6 rounds, a group of 4 also 6 rounds (inference from the same rule: n + n % 2 - 1 rounds a leg).

## 4. The round templates

**Group template** (stage <= +0xff, 0x57d1a2): rounds = 10 (0x57d1de), code 0x3e9 + stage (0x57d1d8), is_league 1, 10 records of 0x41 bytes (0x28a, 0x57d1c8). Each round: `league_round_set_date` 0x68a160 (cdecl: rounds, index, day, month from 0, year offset, weekday, flag byte, year, dword) and `league_round_set_match_slot` 0x68a1c0 (rounds, index, slot 0, -1, -1, -1, 0).

| Round | Date (nearest weekday) | Year | Flag | Address |
|---|---|---|---|---|
| 0 | 2 September, Saturday | +0 | 1 | 0x57d238 |
| 1 | 6 September, Wednesday | +0 | 2 | 0x57d261 |
| 2 | 7 October, Saturday | +0 | 1 | 0x57d28b |
| 3 | 11 October, Wednesday | +0 | 2 | 0x57d2b5 |
| 4 | 24 March, Saturday | +1 | 1 | 0x57d2e0 |
| 5 | 28 March, Wednesday | +1 | 2 | 0x57d30b |
| 6 | 6 June, Wednesday | +1 | 2 | 0x57d336 |
| 7 | 2 September, Saturday | +1 | 1 | 0x57d361 |
| 8 | 6 September, Wednesday | +1 | 2 | 0x57d38c |
| 9 | 7 October, Saturday | +1 | 1 | 0x57d3b7 |

Weekday 0 is Monday (`Date::Date` 0x549f70 moves to the nearest given weekday). Round record 0x41 bytes (0x68a160): +0 word day of year, +2 word year offset from the year argument (0x68a1a2), +4 byte flag, +5..+0x3c eight 7-byte match slots (0x68a3b9..0x68a3c9), +0x3d dword. Inference: flag 1 is a weekend round and 2 a midweek round (it follows the weekday in every call).

**A Nations League template** of 6 rounds needs: operator new 0x944e46 of 6 * 0x41, `*round_count = 6`, six `league_round_set_date` calls with year offset 0 and months 8, 9, 10 (September to November), and the same slot call. Groups of 4 and groups of 3 both take 6 rounds (section 3). League_Stage takes the round count from the template caller (arg 7), so no immediate in League_Stage changes. A group of 3 plays 4 matches a team in 6 rounds. The "4 matchdays" of the brief do not fit a double round robin of 3.

**Play-off template** (stage = +0x101, 0x57d3e9): 1 round of 0x68 bytes (0x57d412), code 0, is_league 0. Draw 11 October +1 Wednesday (`cup_set_round_draw_date` 0x521e60 at 0x57d4a3), first leg 11 November +1 Saturday (`cup_set_round_match_date` 0x521eb0 at 0x57d4ba). Fields: +7 = 0xa0 round id, +0xd = 0, +0xf = 3, +0x17 = 0xa, +0x21 = 2 legs, +0x22 = 4 days to the second leg, +0x18 = 6 or 8, +0x1a = 3 or 4 ties, +0x1c = 6 or 8 (one host 0x57d500, two hosts 0x57d524, else box "line 0x11b" 0x57d597). Stage +0x100 returns 0 (no template, 0x57d3e3).

## 5. Entrants and the draw

- `collect_entrants` 0x57f600: +0xf6 = the European hosts of the Euro of year +0x40 + 2 (find_hosts 0x57f6ac, continent test 0x57f6d0..0x57f6e4), box at more than 2 (0x57f6fa). Pool +0xee of 0x33 - hosts (0x57f76f). It takes nations in table order ([0xae23a8], 0x122 bytes, count [0xae2350]) whose continent is Europe and that are not hosts, up to the pool size (0x57f825..0x57f875), box "line 0x506" when there are fewer (0x57f8af). Then `qsort` 0x945333 with `compare_clubs_by_reputation_desc_nulls_last` 0x4b07d0 (0x57f8f5).
- `draw_groups` 0x57dab0: draw grid +0xf7 of 9 * 6 dwords (0x57dad7..0x57db3d). For year 2002 (`cmp word [esi+0x40], 0x7ce` at 0x57dbb5) it writes 49 fixed key nations into the pool and their fixed slots (49 calls of `database_get_national_team` 0x53b3d0 from 0x57dbc1 to 0x57e6af, last slot 53 at 0x57e6d8). Other years: 6 pots (0x57e7c8). In each pot it shuffles the group order with 20 random swaps (0x57e705..0x57e736), then gives the next pool nation to each group in that order that still has room (0x57e746..0x57e7c0). So pot k is the nations of rank 9k + 1 to 9k + 9 by reputation, one a group.
- Where a group's teams come from: draw grid group 0 for the League object (init_group_one_table), group i + 1 for stage[i].

## 6. Daily update, stages, the play-off

- `update_for_date` 0x57fd60 reads today (arg), the draw date (+0xb6 day, +0xb8 + +0x40 year, 0x57fd96..0x57fdb7). Only on that exact day, with +0x45 = 0 and the full-update argument set (0x57fdfb..0x57fe2d): collect_entrants, draw_groups, init_group_one_table, `create_fixtures` 0x6835c0, sort, create_group_stage 0..7 (0x57ff28), free +0xf7, create_runners_up_stage, vtable +0x5c, free the pool, +0x45 = 1. Other days: when the record has a fixture today (`fix_man_comp_has_fixture_on_date` 0x5ae6d0 at 0x57ff91) it refreshes the runners-up table. Then `League::update_for_date` 0x6847c0.
- Stages advance through the base: 0x6847c0 calls +0x28 at 0x6849e0 when the stages are complete. After the draw +0x30 = 8 (0x57f9eb). advance_stage moves to 9 and builds the play-off.
- `create_playoff_stage` 0x57ef50:
  1. Direct places for runners-up: 3 with one host, 1 with two (0x57f014..0x57f037). The best runners-up of stage[+0x100] get status 7 in their group row, vtable +0x40 (status 7), and their group gets +0xbe = 2, +0xbf = 0 (0x57f09f..0x57f0d6). Inference: +0xbe and +0xbf are the direct and play-off places that `League::update_positions_and_status` 0x684cd0 uses; group winners get status 7 that way.
  2. Play-off teams N = 6 (one host) or 8 (two hosts) (0x57f115..0x57f124): the last N rows of the runners-up table (0x57f17d..0x57f1b3).
  3. Each group row: a play-off team gets status 1 and staff record 0xa0 (0x57f48f..0x57f4b0); a row without status 7 gets `add_failed_to_qualify_record` 0x86c1d0 and `clear_current_competition` (0x57f227..0x57f261).
  4. Template vtable +0x3c (+0x101) (0x57f2fa).
  5. **Fixed pairs** (0x57f30c..0x57f434) when the start date [0xdd6ec8] is after today (the run before the start of a new game): the list is 8 key nations 0x9cf45c, 0x9cf2e4, 0x9cf35c, 0x9cf2c0, 0x9cf474, 0x9cf4f0, 0x9cf2dc, 0x9cf4dc (names not read), and `or byte [edi+0xe], 2` sets bit 0x200 of +0xd (fixed bracket, round doc) at 0x57f437. Else the list of step 2.
  6. Cup_Stage 0xb2 bytes, ctor 0x522e00, 15 arguments (ret 0x3c): this, N, list, round count, record, template, year, stage +0x101, 4, 0, 0x14, link mode (1 for the fixed pairs at 0x57f46b, 0 at 0x57f506), 0, 0, 0. Stored in stage[+0x101] (0x57f4d1, 0x57f541), `Cup::set_up_round` 0x51c800 (0), +0x30 = +0x101.
  7. Winners reach the Euro through slot 17 (status 1 in the play-off stage, `add_qualified_team` 0x57d6a0), called by announce_team_status 0x686670.
  - A loop at 0x57f296..0x57f2e1 calls slot 26 for rows of `edi`, which is then the last group, with count (group size - N) <= 0. Inference: it never runs.

## 7. start_new_season 0x57eb20 and skip_season_reset 0x57eb10

start_new_season: +0x4c = 0, `free_table`, free the template +0xba, delete stages 0..+0x30 through slot 0 (0x57eb58..0x57eb6e), +0x30 = -1, clear the results list, free the pool. Year: **adds 4 until +0x40 >= the game year [0xae2c92]** (0x57eba3..0x57ebcc; no step when it is already there). +0x45 = 0, +0xb5 = 0, +0xfb = 0, `Comp_Stats::archive_season_and_reset` 0x4a1c50 (1), then slot 35 (0x57ebef). The stage array and +0x2c stay. skip_season_reset writes +0xb5 = 1 and returns 1.

## 8. One leg against two legs (Euro finals knockout 0x57bb40)

`EuroChamp::create_knockout_stage` 0x57bb40 calls the template (stage 3, 0x57bc4f) and Cup_Stage (0x57bc9c) with this, 8, list, round count, record, template, year, 3, 4, code, 0x14, 1, 0, 0, 0. The arguments are the play-off's arguments with N = 8 and link mode 1. **The legs come from the template only**, per round (round doc, section 1):

| Field | Euro QF (0x57aff4..0x57b037) | Euro qual play-off (0x57d4bf..0x57d4f0) |
|---|---|---|
| +0x21 legs | 1 | 2 |
| +0x0d flags of the first match | 0x383 (pens, extra time, golden goal, match-day list, fixed bracket) | 0 |
| +0x0f flags of the deciding match | 0 | 3 (extra time and pens after the second leg) |
| +0x22 days to second leg | 0 | 4 |
| +0x07 round id | 0x78 (QF), SF 0x82 (0x57b0a2), F 0x96 (0x57b137) | 0xa0 |

A single match with extra time and penalties: +0x21 = 1, +0x0d = 3, +0x20 = 0 (round doc, section 3). Bit 0x80 (golden goal) passes through `rt_intl_round_flags` in slice 27.

## 9. Final positions of a group

`League::sort_table` 0x6827d0 sorts the 0x3b-byte rows of +0xb1 in place (qsort with `league_compare_table_rows_h2h` 0x683410 at 0x68289d). After the sort, `League::get_table_row` 0x6826d0 (index) gives position index + 1; row +0 is the club, +0x37 the status byte (7 qualified, 1 play-off). The class reads positions that way in create_playoff_stage (0x57f078) and update_runners_up_table (0x57fb4a, 0x57fb53). Group tables: the League object and stage[0..7], each a League_Stage with its own +0xb1. The history keeps only the band codes of set_club_progress (Comp_Stats +0x73..+0x75). start_new_season frees every table and stage (section 7), so promotion and relegation must read the tables before it: in a before hook or in a slot 2 wrap.

## 10. A second object and the Nations League

What a second object of the class does wrong (each with the site):

1. Year rounded to the Euro cycle (0x57ce59) and steps of 4 (0x57ebb4): Nations League editions are two years apart. Year arguments 2020 and 2024 become +0x40 = 2022 and 2026 (inference from the rounding), and odd ones may start the 2002 history (0x57d01a).
2. Entrants: the Euro hosts (0x57f6ac), pool cap 0x33 (0x57f76f, drops nations past 51; 55 UEFA members from 2016), the box below the cap (0x57f8af).
3. Draw: 9 groups of 5 or 6 by reputation pots (0x57dab0, 0x57d9d0); the 2002 fixed list when +0x40 is 2002 (0x57dbb5).
4. Template: 10 rounds over two years (0x57d190), play-off of 6 or 8 in October and November of year + 1.
5. Runners-up table of 9 and play-off (0x57f920, 0x57ef50).
6. Qualifiers go into the Euro finals (0x57d609, 0x57d6a0); the finals box when full (0x57c12a). Staff records and news name the Euro (0x57d5cb, 0x57d83c).
7. `NationalTeams` (0xb74318): `release_competitions_of_continent` (Europe) at 0x580787 and `set_current_competition` for each pool nation (0x580952). A nation has a current and a queued competition (0x775220). Inference, not tested: a Nations League draw clears or pushes back the Euro or World Cup qualifying of every European nation.
8. Prediction and progress codes in bands of 9 (0x5806d0, 0x580980, 0x580b50).
9. mark_qualified_teams reads stage[+0x101] with no null test (0x580097): an object with no stage at +0x101 crashes after the last match (slot 37).
10. get_finals_comp gives the Euro record (slot 30). Who reads slot 30: unknown.
11. The load and the first template use the original vtable (section 0, point 4).

**One object or four.** The code makes one object with all leagues as groups easier:
- One record. The module needs one spare record a def (new-comps doc), and only about five exist. Four leagues would take four, plus one save file each.
- League_Stage takes any team count (arg 2) and any template, and an odd count works (section 3). The group index +0x44 and code +0x3a of each stage can carry the league and group.
- The stage array is a plain malloc (0x57d012) walked up to +0x30 (0x57d0f5, 0x57eb4e). A make function can give a larger array and +0x2c. The 2018/19 edition has 16 groups (League object + 15 stages) plus the finals stage. How `League::save_to_temp_file` 0x6891c0 and the load walk the stages (by +0x2c or +0x30): not read; check before a larger array.
- Comp_Stats capacity for 55 nations: unknown (0x49ee70 not read).

**The smallest set** for one object (vtable copy of 45 slots):
- Slot 5 `update_for_date`: own draw on its own date, from the stored league of each nation: the League object and League_Stage per group with the 6-round template (no call to 0x57f600, 0x57dab0, 0x57e7e0, 0x57ed10, 0x57f920, 0x57fa10). Then `League::update_for_date` 0x6847c0.
- Slot 15 `create_schedule`: 6 rounds September to November of year +0 (and the finals template).
- Slot 35 `set_up_rules`: draw date and rules; the make function calls it again after it writes the copy (point 4 of section 0).
- Slot 10 `advance_stage`: the finals of four (Cup_Stage, single legs) and no play-off of the class.
- Slot 17, 18, 14: no `add_qualified_team`, own news and stage labels.
- Slot 23, 26, 28, 29: own predictions and position codes, and no `release_competitions_of_continent`.
- Slot 37: no mark_qualified_teams (or keep stage[+0x101] valid).
- Slot 2 `start_new_season`: step 2, save the final positions first (promotion and relegation), then build the next edition.
- Slots 8, 9: keep them (the load uses 0x580130 in the constructor) and keep the league of each nation in a store of the slice, or in a wrap of 0x580130 and 0x580410 keyed on the record.
- Wraps keyed on `this` (or on the record +4): `create_groups_and_results` 0x580bd0 (constructor 0x57d025) unless the make passes a safe year; the first `set_up_rules` call 0x57cf30 if the make cannot rebuild +0xba.

## 11. Euro qualifying play-off paths (Euro 2020 shape)

What a new play-off builder replaces in `create_playoff_stage` 0x57ef50 and the template 0x57d190 (inference for the shape; the class sites are read):
- The runners-up places (0x57f014..0x57f0dd: 3 or 1) and the runners-up stage (0x57f920): none in a shape where the top two of each group qualify.
- The team count (0x57f115..0x57f124: 6 or 8) becomes 16, and the list (0x57f17d..0x57f1b3) becomes the Nations League path teams in bracket order (four paths of four: 1 v 4 and 2 v 3, then the final). The fixed-pairs branch (0x57f30c..0x57f434) and bit 0x200 (0x57f437) show the way to a fixed bracket: link mode 1 and the list in pair order. How Cup_Stage links the semi-final winners into the path finals: not read (0x522e00 and the link code).
- The template: 2 cup rounds (0x68 bytes each), semi-finals 16 teams, 8 ties, 16 entrants, +0x21 = 1, +0x0d = 3, round id 0x82; final 8 teams, 4 ties, +0x21 = 1, +0x0d = 3, round id 0x96, March of year + 2 (the play-offs come after the group year); the original's play-off is November of year + 1.
- Slot 17: in the play-off stage status 1 calls `add_qualified_team`. With two rounds in one Cup_Stage, whether a semi-final winner already gets status 1: unknown. Check it, or build the finals as a second stage.
- The stage array: 10 groups (League object + 9 stages) and the play-off fill all 10 slots (0x57cffe) when the runners-up stage goes. The group count immediates (section 3) all change for 10 groups.

## 12. Follow-up: path play-offs and the Euro finals draw

Questions 1 and 2 of the follow-up (ten groups, direct places of runners-up) went to another session and are not answered here.

### 12.1 A play-off Cup_Stage of two rounds

**Cup_Stage ctor 0x522e00** (frame 0x228): +4 record (arg 5, 0x522e40), +0x3c round count (arg 4, 0x522e9d), +0x40 year (arg 7), +0x44 stage index (arg 8, 0x522e5b), +0x42 (arg 9), +0x3a code (arg 10), +0x51 (arg 11), +0x28 (arg 15), +0xa3 the round records, rounds * 0x68 bytes copied from the template (0x522ec2..0x522f32). Team table +0xa7, 6 bytes a team (0x522f30..0x522f46): +0 club, +4 seed byte (from arg 16, else the list index when arg 13 is set, else 0; 0x522fa6..0x522fe3), +5 status byte. Arg 12 (link mode) not 0 calls `Cup_Stage::map_fixture_tree` 0x5241e0 (0x523003); 0 calls the random `Cup::map_fixture_tree` 0x518790 (0x52303d).

**Link mode 1** (`Cup_Stage::map_fixture_tree` 0x5241e0):
- Tie arrays +0xad, one per round, of +0x1a ties of 0x35 bytes (0x524245..0x524285). It clears every team status +5 to 0 (0x524299..0x5242ae).
- Links (0x52459c..0x5248dd): when round r + 1 has bit 0x200 in its +0x0d (`and edx, 0x200` at 0x5245c3, the word at round r + 1 because +0x75 = 0x68 + 0x0d), tie j of round r goes to tie j / 2 of round r + 1 (0x5245d3..0x5245da). Without the bit the next tie is random (0x524648). The first winner takes the home slot (+0xa, link +8 = 1, 0x5246c0..0x524706), the second the away slot (+0xc, link +8 = 0, 0x52470f..0x524755). A last round with round id 0x8c (third place) is left out of the links (0x524580, 0x524bb8).
- Seeding of the entrants (0x5249f2..0x524b96): entrant k of a round with +0x1c entrants starts at tie k / 2 (0x524a20..0x524a27) and takes its home slot (team index +0x1e + k, 0x524afd..0x524b29) or its away slot (0x524b2f..0x524b5b), else the next free tie. When +0x24 is set, a tie whose away team has that nation at club +0x69 swaps home and away (0x524a71..0x524ac6).
- So a list A1, A4, A2, A3, B1, B4, B2, B3, ... gives semi-final ties A1 v A4 (tie 0), A2 v A3 (tie 1), B1 v B4 (tie 2), ..., and with bit 0x200 on the final round the winners of ties 0 and 1 meet in final tie 0, ties 2 and 3 in final tie 1. The first-listed team is at home. The final round must have bit 0x200 in its +0x0d. The play-off of the class sets the bit only on round 0 (`or byte [edi+0xe], 2` at 0x57f437), so a new template must write it in the final round record.

**The status of a semi-final winner.** `Cup::process_fixture` 0x51a150 (it runs to 0x51c00e; the map's size is too short) decides a tie at 0x51aedb:
- When the tie has no next tie (edi = -1 at 0x51af06, the last round): winner +5 = 1, loser +5 = 2 (0x51af95..0x51afac). A third-place round (round id 0x8c) gives 3 and 4 (0x51af63..0x51af8e). Both go to vtable +0x40 with the status byte (0x51afd3, 0x51aff8).
- When the tie has a next tie (0x51b07d): the winner moves into the next tie (0x51b0f5, 0x51b1b4) and goes to vtable +0x40 with its +5 byte, which is still 0 (0x51b126, 0x51b1e6). The loser goes with -1 (0x51b23e, 0x51b5b3).
- Cup_Stage vtable +0x40 is `Cup::add_team_result_news` 0x519690 (vtable 0x969468). It calls vtable +0x44 of the object of the record (`[0xadadfc][record id]`, 0x5196d0..0x51970c) with (team, status, stage +0x44, &code, fixture, arg).
- EuroChampQual slot 17 (0x57d5b0) acts in the play-off stage only on status 1 (`add_qualified_team`, 0x57d65e..0x57d6b3) and 2 (knocked-out record, 0x57d6b6..0x57d6e8). It returns 0 for 0 and -1.
- **So only the final winner gets status 1 and `add_qualified_team`.** A semi-final winner gets 0 and a semi-final loser -1. Neither gets a staff record, and the semi-final loser gets no `clear_current_competition` (inference: it keeps the play-off as its current competition until the season resets). `mark_qualified_teams` 0x580040 reads +5 = 1 of the team table, so it marks the final winners only (0x5800b0).
- The round of a result, for a wrap of 0x57d5b0: argument 5 is the fixture (pushed at 0x5196f9 from 0x51afc8). Fixture +0x34 is the round index and +0x30 the round id (+3 for a first leg) (round doc, +0x07 and +0x09). The stage's round count is +0x3c. The last round is +0x34 = +0x3c - 1. A wrap is not needed for the qualification itself.

**Template of two rounds** in `create_schedule` 0x57d190 for stage +0x101: operator new 0x68 at 0x57d412 becomes 0xd0. `*round_count = 1` at 0x57d414 becomes 2. Round 0: +0x07 = 0x82 (semi-final), +0x0d = 3, +0x21 = 1, +0x18 = 16, +0x1a = 8, +0x1c = 16, +0x1e = 0. Round 1: +0x07 = 0x96 (final), +0x0d = 0x203 (bit 0x200 for the fixed links), +0x21 = 1, +0x18 = 8, +0x1a = 4, +0x1c = 0. The Euro finals use the same values for their semi-final and final (0x57b0a2..0x57b0ed, 0x57b137..0x57b194). Inference, not tested: a round with +0x1c = 0 takes only the winners of the round before.

### 12.2 The Euro finals seeding and draw days

- `EuroChamp::set_up_rules` 0x57b630: seeding day `Date::Date(10, 0, +0x40, 5)` (pushes 0x57b666..0x57b676, call 0x57b6b9): +0x13e = the day number (0x57b6ca), +0x140 = 0 (0x57b6dd). Draw day `Date::Date(10, 1, +0x40, 5)` (pushes 0x57b6c7..0x57b6d3, call 0x57b6e4): +0xb6 (0x57b6f0), +0xb8 = 0 (0x57b709). Both are the Saturday nearest the day, in the Euro year.
- **Both pairs are saved and loaded.** +0x13e and +0x140: `EuroChamp::save_extra_data` 0x57c5d0 (slot +0x24, from 0x57c5e9) and `load_extra_data` 0x57c480 (0x57c4de..0x57c535). +0xb6 and +0xb8: `League::save_to_temp_file` 0x6891c0 (0x689411, 0x689422) and `League::load_from_temp_file` 0x687b10 (0x688172, 0x6881be).
- set_up_rules runs only for a new edition: from the constructor when the load fails (0x57a93c, `jne 0x57aa87` at 0x57a943 skips it after a load; call at 0x57a96c) and from `start_new_season` 0x57b750 through vtable +0x8c (0x57b7f2). After a load the days come from the file.
- `EuroChamp::update_for_date` 0x57c620 tests each day exactly: seeding at +0x13e and +0x140 + +0x40 (0x57c66c..0x57c698, `seed_teams` 0x57c710), draw at +0xb6 and +0xb8 + +0x40 when the table +0xb1 is still empty (0x57c69d..0x57c6ee, `run_group_draw` 0x57c1d0).
- **The safest site** is the slice 27 wrap itself: in `stage_builder` of `season_intl.c` (the wrap of 0x57b630, line 578), write the four words after `original()` returns, for an edition with path play-offs. That covers the new game and `start_new_season`, because the vtable slot points at the wrapped address (inference from RT_WRAP). The words written after the call do not go through the date shift (`date_shift` applies only to `Date::Date` 0x549f70 while the builder runs, season_intl.c line 613). The words hold the day number and the year offset from +0x40. A game saved by that build keeps the moved days through the save. A second wrap of 0x57b630 is not needed. If the slice cannot add to slice 27's block, a wrap of `EuroChamp::update_for_date` 0x57c620 (not wrapped in src) can move the words before it calls the original. That works for old saves too, but it must do so before the original days pass.

### 12.3 The date of the play-off stage

The play-off template is in `create_schedule` 0x57d190, stage +0x101 (0x57d3e9). Both dates take the year +0x40 (0x57d494, 0x57d4a8) and a year offset:

| Date | Call | Pushes (last argument first) | Value now |
|---|---|---|---|
| Draw | `cup_set_round_draw_date` 0x521e60 at 0x57d4a3 | year 0x57d498, weekday `push 2` 0x57d499, year offset `push 1` 0x57d49b, month `push 9` 0x57d49d, day `push 0xb` 0x57d49f, round `push ebx` 0x57d4a1 | Wednesday nearest 11 October, year + 1 |
| First match | `cup_set_round_match_date` 0x521eb0 at 0x57d4ba | venue `push ebx` 0x57d4ac, year 0x57d4ad, match time `push 1` 0x57d4ae, weekday `push 5` 0x57d4b0, year offset `push 1` 0x57d4b2, month `push 0xa` 0x57d4b4, day `push 0xb` 0x57d4b6, round `push ebx` 0x57d4b8 | Saturday nearest 11 November, year + 1 |

The second leg is +0x22 = 4 days later (0x57d4e6). Record fields: draw day +0x00 and year offset byte +0x02 (0x521ea6, 0x521ea9), match day +0x03 and year offset byte +0x05 (0x521ef6, 0x521efe), match time +0x06.

For the paths: round 0 (semi-finals) on 26 March and round 1 (final) on 31 March of the Euro year: day 0x1a and 0x1f, month 2, year offset 2. Weekday -1 keeps the exact day (`Date::Date` 0x549f70 takes -1, function map). Both records need their own draw date before the match (for example 22 November of year + 1, the real Euro 2020 play-off draw, or the day the stage is made). EuroChampQual is not in the date shift of slice 27 (the builder list of season_intl.c line 548 does not hold 0x57e8f0 or 0x57d190). The stage is made when the groups end (advance_stage 0x57ecf5, about October of year + 1). `Cup::set_up_round` (0) runs at once (0x57f5cf), so a draw date before the build day is no problem (inference from the header note on Cup classes, 0x51fa45..0x51fa7d). Not tested: whether the Euro finals draw (12.2) can wait for a play-off that ends after its seeding day. The days of 12.2 must lie after 31 March.
