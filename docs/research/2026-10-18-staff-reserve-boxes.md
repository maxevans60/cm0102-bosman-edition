# Two error boxes in the season databases

Date 2026-10-18. 0.6.0 work. The question: why do new games of September 2008 and October 2023 stop at the boxes "news..cpp 7797" (23.8.2008) and "Database..cpp 13126" (9.2.2024) on main, and how does the runtime fix them without a change to the base game.

## Result

- Neither box comes from the size of the staff reserve. Both are faults of the original that the season databases reach.
- "news..cpp 7797": a national squad call-up news item lists the same player three times. Its text writer then reads past the item and stops at slot 50. The staff reserve of 46,000 rows (doc `2026-10-17-select-all-databases.md`) only moves memory, so the game goes another way. Inference: with 23,000 rows that game makes no such squad, which is why `RECOMP_STAFF_RESERVE=23000` plays on.
- "Database..cpp 13126": the European Championship finals of 2024 get 15 teams for 16 places. The group draw puts a row with no club in the last group, and the group draw news reads that row as a club.
- Fixes: two `RT_WRAP` functions that act only where the original would stop at the box. `src/recomp_rt/win/call_up_news.c` and `src/recomp_rt/win/euro_finals.c`.

## How the runs were made

- The run of both seasons: `python3 tests/seasons/rules_report_check.py --run build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 --year 2008 --last 2009 --database "/Users/max/Projects/cm0102/test-fixtures/databases/September 2008"`, and the same with `--year 2023 --last 2024` and October 2023. England is selected, the reserve is the build's own (46,000 for September 2008, 23,000 for October 2023).
- Before the fix, on this branch at main's code: September 2008 stops on day 235 of 2008 (23 August) at "Error: v3.9.68 news..cpp 7797", box call 0x7858a3. October 2023 stops on day 40 of 2024 (9 February) at "Error: v3.9.68 Database..cpp 13126", box call 0x53b4a7. So the October 2023 box has nothing to do with the reserve: its reserve is 23,000, as in the original.
- `RECOMP_SCREEN_LOG=1` now prints the values on the stack that look like return addresses of the game's code after each message box (`waits.c`, "message box stack"). It is a guess list, not a call chain. The callers below come from test wraps that logged the return address and the arguments, removed after the research.

## Box 1: news..cpp 7797

### The check

`NewsItem::get_slot` 0x785830 (news.cpp) takes a signed byte slot (0x785830). It returns the dword at item + 5 + 4 * slot when the slot is 0 to 49 (0x78583c `jl`, 0x78583e `cmp al, 0x32`, 0x785845). Otherwise it shows the box with line 0x1e75 = 7797 (0x78587b) and returns 0. An item holds 50 slots (`NewsItem::set_slot_string` 0x7858c0 caps at 0x32, 0x785989).

### The caller

The box came from 0x76e440, the return address of the call at 0x76e43b in `NationalTeams::write_call_up_news_text` 0x76e3b0 (national_teams.cpp). The item was type 29 (0x1d). Its slots at the box:

| Slot | Value | Meaning |
|---|---|---|
| 4 | 3 | the count of players (written at 0x76e24a) |
| 10, 14, 18 | 0xcfb | the staff id of each player, the first of four slots per player (0x76e2ce..0x76e324) |
| 11 to 13, 15 to 17, 19 to 21 | 0xbb8, 0x2afb, 0 | the other three slots of each player |
| 22 to 49 | 0 | empty |

So the item lists one player three times.

### Why the reader runs off the item

`write_call_up_news_text` 0x76e3b0 reads the count from slot 4 (0x76e400, byte at 0x76e404). For a count of 1 to 3 (0x76e418 `cmp al, 3`, `jg`) it reads four slots for each player from slot 10 (0x76e422 `mov bl, 0xa`, 0x76e434..0x76e47c). After each group it compares the staff id with the ids it already holds (0x76e4c5..0x76e4dc). For a repeat it lowers its counter again (0x76e4e4 `dec cl`) and counts the repeat (0x76e4e6). The slot number goes up by 4 in every case. The loop ends when the counter reaches the count (0x76e4f4).

With the item above: slot 10 gives a new player, slots 14 and 18 repeat it, slot 22 gives a new "player" with id 0, and every later group gives id 0 again, a repeat. The counter never reaches 3, and the read of slot 50 shows the box.

### Where the repeat comes from

`national_teams_post_call_up_news` 0x76e0b0 makes one item for each run of squad players of one club (0x76e1d5 `cmp [eax+0x39], edi`). It reads the squad of 50 player pointers (club +0xd7, 0x76e11f, 0x76e135) and writes up to 3 players into the item (0x76e1ae, 0x76e2c8..0x76e324). `NationalTeams::select_squad` 0x76c120 calls it at 0x76d3dc. `national_teams_remove_duplicate_squad_entries` 0x76c060 clears a repeated player in the squad of club +0xd7 (0x76c0b1..0x76c0b5), but its callers run it after the squad is chosen (0x76c02f, 0x76c03d in `NationalTeams::select_squads_for_fixture` 0x76bab0, after its call of 0x76c120 at 0x76c021). So the original knows that a squad can hold a player twice, and it sends the call-up news before it takes the repeats out.

Unknown: why `select_squad` picks the same player three times in this game. Inference: it happens in the original too, with any data, when its pick goes that way. The item writer, the reader and the clean-up are the original's code.

### Why the staff reserve seemed to matter

With `RECOMP_STAFF_RESERVE=23000` the same run plays on (slice 16 report). The reserve does not take part in the squad pick: the 46,000 rows only change the size of the staff and non-player tables, so the blocks after them sit elsewhere. The original's results depend on where its memory blocks sit (Q152, HANDOVER). Inference: with 23,000 rows the game makes another squad on that day. This is the same as a screen that opens, not a fault of the reserve.

### The fix

`RT_WRAP(0076e3b0)` in `call_up_news.c` runs the reader's loop on the item's slots first (same order, same repeat test). Only when that loop would read slot 50 or more, the reader gets for this call a count equal to the number of different staff ids among the first `count` groups, and the old count goes back into the item after the call. The text then names each player once. Every other item reads as in the original, and the item bytes in memory and in the saved game do not change. With `RECOMP_SCREEN_LOG=1` the wrap prints "call-up news lists a player more than once".

In the fixed September 2008 run the wrap acted on 18 calls of the text writer (the same item drawn more than once, inference) and the jump reached the end of 2009/10 with no box.

## Box 2: Database..cpp 13126

### The check

0x53b3d0 (Database.cpp, `database_get_national_team`) takes a nation pointer. It returns the senior national team club: the club table 0xae23b4 + (nation id + `g_club_count` 0xae235c - 2 * `g_nation_count` 0xae2350) * 0x245 (0x53b4c2..0x53b4e0). For a null pointer it shows the box with line 0x3346 = 13126 (0x53b407) and returns 0.

### The callers

- 0x53b3d0 was called from 0x600c02 in `human_manager_is_human_club` 0x600a50. For a club that `club_is_national_team` 0x5409b0 finds (club id at least `g_club_count` - `g_nation_count`, 0x540a2a..0x540a40), that function goes on with the national team of the club's nation (club +0x53, 0x600bf2..0x600c09). The club here was no club: pointer 0x7247bb9b, id 65,536,000, nation 0.
- 0x600a50 was called from 0x7875fd in `news_club_has_human_manager` 0x7875e0, called from 0x684293 in `League::send_group_draw_news` 0x684230. That function reads the club pointer of each table row (+0xb1, rows of 59 bytes, 0x684270..0x684289) for the club count at +0x3e.
- The league was the "European Football Championship" of 2024 (vtable 0x96a020, the EuroChamp class), a group of 4 with Italy, Denmark, Slovenia and the bad pointer.

### Why the group has a row with no club

`EuroChamp::set_up_team_list` 0x57bd80 makes 16 slots of 6 bytes at +0x14 (0x57bd89..0x57bda1), count +0x36 = 0, size +0x38 = 16 (0x57bd93). It asks HostCountry for the hosts (`find_hosts` 0x5fa730 at 0x57be34) and adds one or two at the count with flag 1 (0x57be4d, 0x57be97). Then it writes 14 default key nations in slots 2 to 15 (0x57bf2f..0x57c104), without a change of the count. `EuroChamp::add_qualified_team` 0x57c120 writes each qualifier at the count with flag 6 and raises the count (0x57c1ad..0x57c1bf). `EuroChamp::update_for_date` 0x57c620 then calls `EuroChamp::seed_teams` 0x57c710 on the seeding date (+0x13e, 0x57c698) and `EuroChamp::run_group_draw` 0x57c1d0 on a later date (0x57c6ee). `seed_teams` copies the count of teams (0x57c79f..0x57c7c1), sorts them, puts the hosts and the holder first, and writes the count of seeds at +0xee, 5 bytes each (0x57c9ac..0x57c9d8). `EuroChamp::draw_groups` 0x57c320 (from 0x57c1db) always takes 16 seeds (0x57c3b7..0x57c46b, the first four as group heads, then 12 more). So when the count is below 16, the last seeds hold no club, and `EuroChamp::create_group_table` 0x57b530 (0x57c1e5) makes a group row from it.

The list in the October 2023 game:

- after `set_up_team_list`: count 1, host Italy (the 2024 host is drawn at random from the HostCountry candidates, sweep `2026-10-16-sweep-international.md` section 6), slot 1 empty, the defaults in slots 2 to 15,
- the qualifiers: 10 from 0x57d60e and 4 from 0x57d6a5 (in `EuroChampQual::update_staff_records_for_status` 0x57d5b0), Italy among them,
- at the draw: count 15 for 16 places, Italy in slots 0 and 9.

So 1 host and 14 qualifiers leave one seed free, and one group gets a row with no club. Inference: the qualifying makes 14 qualifiers, the number for two hosts, and the host row then picks one host. Unknown: how this qualifying was set up in a game from 2023 (`EuroChampQual::collect_entrants` 0x57f600 takes at most 2 hosts, 0x57f6fa, and `create_groups_and_results` 0x580bd0 loads the 2002 campaign when the game starts later). The rules of the Euro qualifying belong to slices 27 and 28 of 0.6.0.

### The fix

`RT_WRAP(0057c710)` in `euro_finals.c` acts before the seeding only when the count is below the size. A first try at `run_group_draw` 0x57c1d0 was too late: the list was full, but the seeds were already made from 15 teams, and the box came again. It takes out a team that is in the list twice (the first stays, so the host keeps slot 0 and flag 1), then fills the free places with the national teams of the 14 default key nations (the key ids of 0x57bf2f..0x57c0e3, in that order) that are not in the list, with flag 0 as the defaults have. The national team comes from the key id as `set_up_team_list` gets it (nation table 0xae23a8 + key * 0x122, then the row of 0x53b3d0). A full list seeds and draws as in the original. With `RECOMP_SCREEN_LOG=1` the wrap prints "European Championship finals with 15 of 16 teams, now 16".

## Base game

Both wraps change nothing when the original does not stop at the box: the call-up reader acts only when it would read slot 50, and the finals list only when it is short. A short list always gives a row with no club, so the base game cannot have one and play on. The same-save checks against the main build pass (see Checks).

## Checks

Commands from the repo, with `G=/Users/max/Projects/cm0102/clean-3968` and `MAIN=/Users/max/Projects/cm0102/cm0102-mac/build/cm0102_recomp`:

- `python3 tests/seasons/season_boxes_run.py build/cm0102_recomp $G`: September 2008 and October 2023, each over its first season, England selected, fail on any message box. On this branch: both runs end the season with code 0 and no box ("FAILED: none"). With `$MAIN`: "news..cpp 7797" on day 235 of 2008 and "Database..cpp 13126" on day 40 of 2024, and no end report (the games hang after the box; they were stopped by hand).
- The two-season jumps of the first section on this branch: September 2008 to the end of 2009/10 and October 2023 to the end of 2023/24, code 0, no box.
- Same-save checks of the base game against main: `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --runs normal,again,season-code-off --ref-build $MAIN --jobs 2`, the same with `--save prematch.sav` and without `--save` (a new game). All three PASS: `again`, `season-code-off` (`RECOMP_SEASON_CODE=off`) and `ref-build` equal `normal`.
- `python3 tests/smoke/update_team_sweep.py build/cm0102_recomp $G /Users/max/Projects/cm0102/test-fixtures/databases --select-all --jobs 2 --timeout 2400`: 21 of 21 pass, each at the expected stop "fra_first..cpp 108" with "result 1", and with the reserve rows of the table in `2026-10-17-select-all-databases.md`.

## Unknowns

- Why `select_squad` 0x76c120 picks one player three times.
- How the Euro 2024 qualifying gets 14 qualifiers in a game from 2023, and whether Italy as host of 2024 is right (the real host is Germany). Both belong to the international slices (27, 28).
- Other news writers of national_teams.cpp that read groups in the same way (for example 0x76ff40, 0x7703f0) are not checked.
