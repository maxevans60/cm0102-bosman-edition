# Idle club competitions and the Club World Championship years (0.6.0 slice 26)

Date 2026-10-18. Research only. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Evidence: capstone disassembly of the clean exe (the `cm0102` venv) and REA with Ghidra. "Inference" marks a statement that the code does not prove. Months in the date calls are 0-based: `Date::Date` 0x549f70 (day, month, year, weekday) gets (20, 5, 2001) for the update day of entry 2 at 0x8312a0, and that day is 20 June (day 170, `2026-10-17-season-change.md`).

Classes in this doc:

| Class | Constructor | Base | Vtable | Key id | Entry | Year step |
|---|---|---|---|---|---|---|
| World_Club_Champ (FIFA Club World Championship) | 0x929140 | League | 0x97119c | 0x9cf7bc | 2 | +2 at 0x929d9e |
| World_Club_Cup (Intercontinental Cup) | 0x92b4b0 | Cup | 0x971250 | 0x9cf6e4 | 2 | +1 at 0x92be3e |
| Inter_Amer_Cup | 0x632080 | Cup | 0x96bad8 | 0x9cf95c | 2 | +1 at 0x6329ae |
| Con_Merc_Cup (Merconorte Cup) | 0x4c16f0 | League | 0x968b40 | 0x9cf68c | 4 | +1 at 0x4c2023 |
| Conmebol_Merc (Copa Mercosur) | 0x4c46c0 | League | 0x968ca8 | 0x9cf698 | 5 | +1 at 0x4c50b3 |
| Asia_Cup_Winner | 0x40cbe0 | Cup | 0x967574 | 0x9cf650 | 6 | +1 at 0x40d691 |
| Asia_Super_Cup | 0x410650 | Cup | 0x9676c8 | 0x9cf7d0 | 6 | +1 at 0x410d00 |
| Oceania_Club_Champ | 0x796fd0 | League | 0x96e018 | 0x9cf7cc | 7 | +2 at 0x797dbe |

All eight vtables have `all_stages_complete` 0x48ce10 at +0xc and the shared "return 0" 0x48ce70 at +0x2c.

## 1. Recommended idle method

Use the state that the original itself uses between two editions of the Club World Championship and the Oceania Club Championship: keep the object, and put its season year +0x40 at the year of the next edition, so that its start date or draw dates lie in the future. Do not remove the object, and do not skip `start_new_season` without a year fix.

1. **In a game, for a season S with no edition:** in the `before` hook of the competition's `start_new_season` (slice 16 registry), set +0x40 to (next edition year minus the class's year step). `start_new_season` then adds its step and builds the rules, the round table and the start date of the next edition with that year. Until that date the object is not complete, so the pass does not call `start_new_season` again (section 3.2), and it makes no fixtures (section 2).
2. **At a new game, for a first season with no edition:** wrap the constructor and call the original constructor with the year of the first edition as its year argument (thiscall, `ret 8`: year, then the record pointer). The creators push the game year `[0xae2c92]` (for example 0x83118b..0x8311b8 for the Club World Championship). The constructor then builds every date from that year. Change the argument, not +0x40 after the constructor: the League classes store their start date as a day of the year with a weekday shift for the year of the constructor (`set_up_rules`, for example 0x929c36..0x929c52), so a later change of +0x40 keeps the old weekday (inference: a shift of up to three days).
3. **Copa Mercosur only:** block its own call of `start_new_season`. `ConmebolMerc::update_for_date` 0x4c5c90 calls vtable +0x8 on 1 June of every year when +0x40 is not the game year (Date(1, 5, year) at 0x4c5ca4..0x4c5cb1, compare at 0x4c5cd3, call at 0x4c5ce4). A waiting object has a future year, so each 1 June would add 1 again. This call is outside the pass, and the slice 16 wraps pass it through. The wrap of 0x4c5040 must refuse it while the object waits for a planned edition.

Why not the other ways:

- **Skip `start_new_season` and change nothing.** The object stays complete with the old year. That is safe for the idle season itself (section 3.2). But the next call adds only the step, so the dates of the next edition can lie in the past. A League class then never draws: its update compares the start date with today for equality (Club World Championship 0x92a73d..0x92a74f, Mercosur 0x4c5d2c..0x4c5d3d, Merconorte 0x4c2ce5..0x4c2cf6), so `+0x45` stays 0, `check_complete` stays false (0x689af1) and the competition stops for ever. A Cup class draws a past round at once (catch-up at 0x51fa45..0x51fa7d), with match dates in the past (unknown result). So a skip needs the same year fix as point 1, at the resume.
- **Remove the object.** Section 4.3 lists the code that breaks.

Cup classes and stale entrants. A Cup class picks its clubs in the constructor and in `start_new_season` (World_Club_Cup 0x92b53e and 0x92be50, Inter_Amer_Cup 0x63210e and 0x6329c0, Asia_Cup_Winner 0x40cca9 and 0x40d69d, Asia_Super_Cup 0x4106da and 0x410d12). With a year ahead, the clubs come from the season before the gap (inference). The League classes pick their clubs on the start date (for example `WorldClubChamp::update_for_date` 0x92a6b0 calls `pick_entrants` 0x92a8b0), so they get fresh clubs. For the Intercontinental Cup the Copa Libertadores writes its new winner into the cup when its final ends (section 4.2), so only the European club is stale. A kind that wants fresh clubs for a Cup class can skip `start_new_season` in the idle season and set +0x40 to (edition minus 1) in the `before` hook of the season of the edition. That keeps the object complete for a year (section 3.2), with the Libertadores write into a finished cup (section 4.2).

## 2. What each constructor does at a new game (question 1a)

Every constructor first tries to load the object from its CM3_TEMP file: `League::load` 0x687b10 (vtable +0x18 of the League classes) or `Cup::load_from_disk` 0x51fc00. The file name holds the record name and the record id (`"%s_%d.tmp"`, 0x99b838, with flag 1). A new game has no file, so the constructor goes on. Inference: at a load of a saved game the creators run again and the constructors read their state from these files, and the loaded +0x40 replaces the constructor's year (0x687b10 reads +0x40 at line 0x11f7 of its assert list).

No constructor makes fixtures. Fixtures go to the fixture manager `[0xae2a58]` only on a draw or start date:

- League classes: on the start date, `update_for_date` (vtable +0x14) picks the clubs, builds the groups and calls `League::create_fixtures` 0x6835c0, which gives each match to the fixture manager (Club World Championship 0x92a6b0, Mercosur 0x4c5d43..0x4c5db3, Merconorte 0x4c2cfc.., Oceania 0x797b40..). Then it sets +0x45 to 1 (0x92a87d, 0x4c5ead).
- Cup classes: `Cup::update_for_date` 0x51f890 draws a round with `Cup::draw_round` 0x51c410 when its draw date is today or past (0x51f9ef, 0x51fa7d), and `Cup::set_up_round` 0x51c800 makes the fixtures of a round (0x51fb1f). Exception: `AsiaSuperCup::start_new_season` calls `set_up_round` itself (0x410dfe).

| Class | Constructor builds | Start or draw |
|---|---|---|
| World_Club_Champ | League base 0x682200, year +0x40 moved to the first odd year from 2003 (0x92919c..0x9291bd), 3 stage slots (0x9291e2), Comp_Stats 0x49ee70, `set_up_rules` 0x929bb0: rules, round table +0xba from slot 15 0x929390, start date Date(5, 1, year, 5) into +0xb6, +0xb8 = 0 | draw about 5 February of +0x40 |
| World_Club_Cup | Cup base 0x518640, `pick_qualifiers` 0x92b8b0 (two clubs into +0xa7), round table slot 15 0x92b770 into +0xa3, `Cup::map_fixture_tree` 0x518790, Comp_Stats, `Cup::rank_teams` 0x5223a0 | draw 10 October |
| Inter_Amer_Cup | the same shape: `select_teams` 0x632480, round table 0x632340, 0x518790, Comp_Stats, 0x5223a0 | draw 10 October |
| Con_Merc_Cup | League base, stage array, `set_up_rules` 0x4c1ed0: start Date(1, 7, year) (0x4c1f02..0x4c1f59), round table, Comp_Stats | 1 August |
| Conmebol_Merc | League base, 5 stage slots (0x4c4740), `set_up_rules` 0x4c4f50: start Date(15, 5, year) (0x4c4f84..0x4c4fe0), prize fund 2,000,000 (0x4c500e), round table, Comp_Stats | 15 June |
| Asia_Cup_Winner | Cup base, `select_qualified_clubs` 0x40d830, team entries 0x40d4d0, round table, Comp_Stats, zone stage 0x40ddf0 (a Cup_Stage 0x522e00), 0x518790, `rank_teams` 0x40e3b0 | 1 August |
| Asia_Super_Cup | Cup base, `select_teams` 0x410a50, round table 0x410910, 0x518790, Comp_Stats, 0x5223a0 | draw 4 July |
| Oceania_Club_Champ | League base, year code 0x797007..0x79705c, Comp_Stats, `set_up_rules` 0x7979b0: start Date(20, 7, year, 5) (0x7979d0..0x797a39) | about 20 August |

What removes fixtures: nothing removes the fixtures of a competition as a whole. They leave the fixture manager when they are played. `FIXTURE_MANAGER::remove_fixture` 0x5abc30 removes one fixture (callers not traced). So an edition that has drawn cannot be cancelled cleanly. Idle a competition before its draw or start date.

## 3. When the runtime does not call `start_new_season` (question 1b)

### 3.1 The pass, 0x5d5cab..0x5d5dca

For each object of the entry's array (+0xc count, +0x10 array, null elements skipped at 0x5d5cbe):

1. vtable +0xc `all_stages_complete` (0x5d5ccb). False skips the object.
2. vtable +0x2c (0x5d5cdb), 0 for all eight classes (0x48ce70).
3. If `0x4b0700(record id)` is true (0x5d5cee), `game_update_league_attendances` 0x5d6280 along the +0x20 chain.
4. `start_new_season`, vtable +0x8 (0x5d5d1c).
5. `Discipline::start_new_season_for_comp` 0x556210 with the record (0x5d5d29). It calls slot 0 of the ruling body of object +0x50 when +0x50 is not 0xff. The Club World Championship (0x9291c3) and the Intercontinental Cup (0x92b525) have 0xff. The Mercosur has 2 (0x4c4711).
6. Once a day, a new `simulated_stats` object 0x84b940 (0x5d5d58..0x5d5dbb).

Steps 5 and 6 run after the call at 0x5d5d1c whatever the wrap does. So with a skipped `start_new_season` the ruling body of the Mercosur still starts its new season (inference: its card counts reset, which is harmless).

### 3.2 The object after a skip

`all_stages_complete` 0x48ce10 is true when +0x1c is -1 (no parent), vtable +0x10 `check_complete(1, 1)` is true, and every competition along +0x20 is complete. `League::check_complete` 0x689ad0 needs a table (+0xb1), +0xb5 = 0 and +0x45 = 1 (drawn), then the finished flag +0xdd. `Cup::check_complete` 0x521e00 needs +0xb1 (finished). Only `start_new_season` clears these (for example +0x45 = 0 at 0x929db9 and 0x4c50c0, +0xb1 = 0 at 0x92be4a and 0x6329ba). So after a skip:

- `all_stages_complete` stays true. The pass calls `start_new_season` again on every update day (the wrap must refuse it each time).
- The object makes no fixtures. The League update needs today to equal the start date of the old year. The Cup update draws nothing while +0xb1 is set (0x51f9f9..0x51fa01).
- `update_for_date` still runs every day for every object of `g_comp_object_table` (`game_update_comps_for_date` 0x5ce060, call at 0x5ce088). The season end work runs once only: `League::update_for_date` 0x6847c0 and `Cup::update_for_date` (0x51f909..0x51f92f) do the history (`Club_History::record_comp_season` 0x442e80), honours (vtable +0x30) and records (0x4a19f0) only while +0x4c is 0, then set it to 1. Nothing repeats.
- Club history screens: `Club_History::get_cup_history` 0x443520 lists an object as a current competition only when `all_stages_complete` is false (0x4435d7..0x4435dc). A skipped object counts as finished. Inference: a waiting object of method 1 counts as current, the same as the Club World Championship of the original in an even year.
- News and screens: no code of the pass posts news for it. Inference: the competition screens show the last edition with its old year.

### 3.3 Two self-starts outside the pass

- Copa Mercosur, 1 June: see section 1, point 3.
- Asian Super Cup, 25 June: `AsiaSuperCup::update_for_date` 0x410e20 calls vtable +0x8 when the cup is complete and today is 25 June (0x410e34..0x410e72). `AsiaSuperCup::start_new_season` 0x410c50 does its work only when the game month is June (`Date::get_month` 0x54cfb0, compare at 0x410c64). So the call of the pass on 28 August is a no-op for this class, and its real season change is the 25 June call. A waiting object is not complete, so the self-start does not happen. A skipped (complete) object restarts there unless the wrap refuses it.

## 4. Other code that reads these objects (question 1d)

### 4.1 By key id or record only (safe for an idle or removed object)

| Reader | What it reads |
|---|---|
| `WorldClubChamp::pick_entrant` 0x92ae60 | See below |
| `WorldClubCup::pick_qualifiers` 0x92b8b0 | Europe: the latest Champions League winner from the competition history (0x9cf6e8, 0x92bbd5..0x92bbfd) when Europe has a selected nation (0x92b960) or +0x40 is 2001 (0x92b970), else a random European club above reputation 5000 (0x92b97c..0x92bbce) |
| `AsiaSuperCup::select_teams` 0x410a50 | The latest Asian Cup Winners' Cup winner from the history (0x410b42..0x410b5c) |
| `FIXTURE_MANAGER::find_fixture_clashes` 0x5ad780 | Compares a fixture's competition id with 0x9cf7bc (0x5ad92e) |
| `match_day_can_quick_simulate` 0x6b66a0 | Compares the competition id with 0x9cf7bc and 0x9cf6e4 (0x6b67a1, 0x6b67ae) |
| `key_nation_index_of_comp` 0x669470 | Compares with 0x9cf6e4 and 0x9cf7bc (0x669570, 0x66957a) |
| `NewsItem::write_text` 0x77bc80 | The record name of 0x9cf7bc (0x780046, 0x780167) |
| `HostCountry::init_default_table` 0x5f9970, `redraw_world_club_and_confed_hosts` 0x5fab70, `alternate_oceania_club_champ_host` 0x5face0 | Host table rows by id (section 5.2) |
| `set_key_ids` 0x601a40, `match_club_comp_key_id` 0x60efd0 | The ids at load |

**How `pick_entrant` tests that a continental competition runs.** It does not test the object. For a continent it finds the continental league table entry (entry +4 the continent, entry +0 no nation, loop 0x92ae86..0x92aead) and takes the record at entry +0x1e (0x92aeb4). When there is none, it takes the Asian Super Cup record for Asia (0x92aebf..0x92aec7), the Oceania Club Championship record for Oceania (0x92aece..0x92aed6), the CONCACAF Champions Cup record for North America (0x92aedd..0x92aee5). A sixth argument other than -1 gives another record (the second European pick passes the UEFA Cup, 0x92aa8b). Then, when `key_nation_continent_has_selected_nation` 0x6699d0 is true for the continent (0x92af42) or +0x40 is 2001 (0x92af4e), it takes the club in place 0 of the latest history row of that record (`comp_util_get_latest_placed_club` 0x4afd80, 0x92af59), or place 1 when that club is already picked (0x92af80). Else it takes a random club of the continent from reputation 5000 down in steps of 1500 (0x92afaf..0x92b093). So an idle or removed continental competition gives its last winner in the history, which can be some seasons old (inference: a stale entrant, no crash). Which code writes entry +0x1e is unknown.

### 4.2 By object, through `g_comp_object_table` [0xadadfc] or an entry array

| Reader | What it does | Idle object | Removed object |
|---|---|---|---|
| `ConmebolLiber` vtable +0x44 at 0x4c4080 (the function map puts it in 0x4c3f40) | When the Libertadores winner is known (fate 1, 0x4c4182), takes `[0xadadfc][0x9cf6e4]` with no null check (0x4c41a0..0x4c41ab), writes the winner into its club list +0xa7 slot 0 or +6 (0x4c41ae..0x4c41d0), archives its Comp_Stats with `Comp_Stats::archive_season_and_reset(1)` 0x4a1c50 (0x4c41d6) and calls its `rank_teams` (0x4c41e1) | Works. The write goes into the waiting or old list. What the archive adds to the cup's history when it runs on a cup that is not playing is unknown | Crash when the slot is cleared, or when +0xa7 is freed |
| `EngFaCup::collect_entrants` 0x56fab0 | Walks the entry 2 array (count `[0xb63dfc]`, array `[0xb63e00]`) and reads element +4 with no null check (0x56fb08..0x56fb0e). When the Club World Championship +0x40 equals the FA Cup +0x40 (0x56fb26), collects its English clubs (vtable +0x58) into FA Cup +0xb3/+0xb4 (0x56fb89..0x56fbb6) | Years differ, so no effect | A null element in the array crashes. A lower count is safe |
| `asia_cup_winner_club_in_club_championship` 0x40e7b0 | Takes element 0 of the entry 6 array as the Asian Club Championship and reads its stage 0 and +0xa7 with no null checks (0x40e802..0x40e821) | Not affected | Breaks if the order of entry 6 changes |
| `game_update_comps_for_date` 0x5ce060, `game_run_to_start_date` 0x5cc850 | Call vtable +0x14 of each object of the table, null skipped (0x5ce07b, 0x5cc9a4, 0x5ccabf) | Runs daily | A cleared slot is skipped |
| `Club_History::get_cup_history` 0x443520 | Null-checked (0x4435cb) | See 3.2 | Safe |
| `comp_screens_add_related_comps_menu` 0x49d070, `menubar_build` 0x75db00 | Walk the entry arrays, null-checked (0x49d156, 0x75f68d) | Shown | Not shown |
| `key_nation_save_all_comps` 0x6692f0, `key_nation_delete_all_comps` 0x669290 | Save (vtable +0x1c) and free each object of the entry arrays | Saved | Not saved and not freed (a leak) |
| `build_comp_object_table` 0x8386f0 | Fills `g_comp_object_table` from the entry arrays only (0x838788..0x8387be) | Kept | Not in the table after it runs |

### 4.3 Why removal is the most risky way

1. A lower count in the entry array leaves the object out of the save (0x6692f0). Inference: at a load the creator makes the object again (the counts are fixed in the creator, for example 2 or 3 at 0x8310b3 and 0x8310bc), finds no temp file and builds a new edition with the year of the load.
2. A cleared slot of `g_comp_object_table` crashes the Libertadores write for the Intercontinental Cup (0x4c41ab).
3. A null element in the entry 2 array crashes the FA Cup (0x56fb0b). A changed order in entry 6 breaks the Asian Cup Winners' Cup (0x40e802).

## 5. The Club World Championship years (question 2)

### 5.1 The code

First edition loop, `WorldClubChamp::WorldClubChamp` 0x929140. +0x40 holds the constructor's year (0x92918b). The loop adds 1 while the year is even or below 2003:

```
0x92919c: 66 8b 46 40         mov ax, word ptr [esi + 0x40]
0x9291a0: 0f bf d0            movsx edx, ax
0x9291a3: 81 e2 01 00 00 80   and edx, 0x80000001
0x9291a9: 79 05               jns 0x9291b0
0x9291ab: 4a                  dec edx
0x9291ac: 83 ca fe            or edx, 0xfffffffe
0x9291af: 42                  inc edx
0x9291b0: 74 06               je 0x9291b8          ; even: add 1
0x9291b2: 66 3d d3 07         cmp ax, 0x7d3        ; 2003
0x9291b6: 7d 07               jge 0x9291bf         ; odd and >= 2003: done
0x9291b8: 40                  inc eax
0x9291b9: 66 89 46 40         mov word ptr [esi + 0x40], ax
0x9291bd: eb dd               jmp 0x92919c
```

Step, `WorldClubChamp::start_new_season` 0x929d40:

```
0x929d9e: 66 83 46 40 02      add word ptr [esi + 0x40], 2
```

Start date, `WorldClubChamp::set_up_rules` 0x929bb0 (Date::Date is thiscall: day, month, year, weekday, so the pushes run from weekday to day):

```
0x929c32: 66 8b 46 40         mov ax, word ptr [esi + 0x40]
0x929c36: 6a 05               push 5               ; weekday
0x929c38: 50                  push eax             ; year
0x929c39: 6a 01               push 1               ; month: February
0x929c3b: 6a 05               push 5               ; day 5
0x929c41: e8 2a 03 c2 ff      call 0x549f70
0x929c4b: 66 89 9e b8 00 00 00  mov word ptr [esi + 0xb8], bx   ; year offset 0
0x929c52: 66 89 8e b6 00 00 00  mov word ptr [esi + 0xb6], cx   ; day of the year
```

Round table, `WorldClubChamp::create_schedule` 0x929390 (vtable +0x3c). All rounds have year offset 0:

- Groups (stages 0 and 1, `league_round_set_date` 0x68a160): 29 July (0x929435), 2 August (0x929460), 5 August (0x92948c).
- Knockout (stage 2, `cup_set_round_draw_date` 0x521e60 and `cup_set_round_match_date` 0x521eb0): draw 7 August, match 9 August (0x9296e9, 0x929700); draw 10 August, match 12 August for the two last rounds (0x92975f, 0x929776, 0x9297f5, 0x92980c).

The update, `WorldClubChamp::update_for_date` 0x92a6b0: before the start date it returns without the League update (0x92a718 to 0x92a857). On the start date, with +0x45 = 0 and the second argument 0, it picks the 12 clubs, predicts positions, draws the groups, makes the fixtures and sets +0x45 = 1 (0x92a746..0x92a87d).

So in the original: an edition with year E (odd, from 2003) draws about 5 February of E and plays from 29 July to 12 August of E. It is complete after 12 August. On 20 June of E + 1 the pass calls `start_new_season`, which sets E + 2. The game has an edition in July and August of every odd year from 2003, in game season E (E/E+1, after the season point of 20 June E).

### 5.2 What an annual edition from year Y0 needs

1. **The first edition.** Change `74 06` at 0x9291b0 to `90 90` (no parity test), and the immediate `d3 07` at 0x9291b4 to Y0 (little-endian). The loop then gives max(constructor year, Y0). The year of a later rule change can come from the constructor wrap of section 1 instead.
2. **The step.** Change the byte `02` at 0x929da2 to `01`.
3. **The start date. This is required with step 1.** An edition E is complete on 12 August E. The next update day is 20 June E + 1, and step 1 gives E + 1, whose start date (5 February E + 1) is already past. The draw needs today to equal the start date (0x92a73d..0x92a74f), so the competition would never draw again and never be complete again: it stops after one edition. Move the start date between 21 June and 28 July: for example month byte `01` at 0x929c3a to `06` (July) and day byte `05` at 0x929c3c to `01` (1 July, then the weekday 5 shift of 0x549f70). Inference: at a new game the run to the start date calls vtable +0x14 with the second argument 0 (0x5cc9a8..0x5cc9ae), so a draw date before the game's first day still runs in the run-up.
4. **Other places that assume odd years.** I found none other in the code that I read:
   - `HostCountry::redraw_world_club_and_confed_hosts` 0x5fab70 (called at 0x929db0) gives every host row with the Club World Championship id (0x5fac7e..0x5facc3) a random nation with nation +0x85 of 13 or more, not England (0x9cf2e4), not 0x9cf488, and not an Oceania nation (0x5faba2..0x5fabe6). It does not read the year.
   - The host row of `HostCountry::init_default_table` has year 0 (id at 0x5fa343, year word at 0x5fa351 from bp, which is 0 from 0x5f999d). `pick_entrants` asks `HostCountry::find_hosts` 0x5fa730 with +0x40 and the default flag 1 (0x92a944..0x92a961). Inference: the year 0 row answers every year.
   - First-season branches: `pick_entrant` tests +0x40 = 2001 (0x92af4e, 0x92b24b). Never true from 2003.
   - Side effect: the FA Cup reads the English clubs of a Club World Championship with its own year (0x56fb26, section 4.2). With an edition every year, this happens every year (inference: an English club in it gets the FA Cup treatment of +0xb3/+0xb4 each season).

### 5.3 World_Club_Cup (Intercontinental Cup)

`WorldClubCup::start_new_season` 0x92bda0: `0x92be3e: 66 ff 46 40  inc word ptr [esi + 0x40]`, then new clubs (0x92be50), rounds (vtable +0x8c) and tree (vtable +0x94). It is already annual. Round table 0x92b770: draw 10 October (0x92b834), match 1 December (0x92b84f), both year offset 0. An edition with year Y plays in December Y, game season Y. To stop it after a last year, idle it with method 1 and keep the object (the Libertadores write of section 4.2 needs it).

### 5.4 Oceania_Club_Champ

Step: `0x797dbe: 66 83 46 40 02  add word ptr [esi + 0x40], 2`, then a new host (`alternate_oceania_club_champ_host` 0x5face0, at 0x797dd7) and the rules (vtable +0x8c at 0x797de0).

First-year code of the constructor 0x796fd0:

```
0x79700d: 81 e2 01 00 00 80   and edx, 0x80000001   ; parity of the constructor year
0x797035: 79 05               jns 0x79703c
0x797037: 4a                  dec edx
0x797038: 83 ca fe            or edx, 0xfffffffe
0x79703b: 42                  inc edx
0x79703c: 75 20               jne 0x79705e          ; odd: keep
0x79703e: 66 8b 46 40         mov ax, word ptr [esi + 0x40]
0x797042: 66 3d d1 07         cmp ax, 0x7d1         ; 2001
0x797046: 7d 16               jge 0x79705e          ; even and >= 2001: keep
0x797048: 40                  inc eax
0x797049: 66 89 46 40         mov word ptr [esi + 0x40], ax
...                                                 ; parity test again, je 0x79703e at 0x79705c
```

So the constructor keeps any year from 2001, odd or even. Only an even year below 2001 moves to the next odd year. The function map's "odd year after 2000" is right only below 2001. The parity of the editions follows the start year of the game.

Dates: start (draw) Date(20, 7, year, 5), about 20 August of +0x40 (0x7979d0..0x797a39). Groups 10, 12 and 14 January of +0x40 + 1 (0x7972d1, 0x7972fc, 0x797328), knockout 16 to 19 January of +0x40 + 1 (0x797502..0x79761f). An edition with year Y plays in January Y + 1, game season Y. It is complete on 19 January Y + 1. The update day is 21 July. With the step `02` at 0x797dc2 changed to `01`, the new start date (20 August Y + 1) lies after the update day, so an annual edition needs only that byte (inference from the dates, not run).

## 6. Update days and the months of the rounds (question 3)

The update days are the dates of the creators: 20 June (entry 2, 0x831298..0x8312b6), 21 February (entry 4), 10 January (entry 5), 28 August (entry 6), 21 July (entry 7) (`2026-10-16-sweep-europe.md`, section 5; `2026-10-17-season-change.md`). By the rule of slice 16, a match from July to December of year Y and a match from January to June of year Y + 1 are both in game season Y (Y/Y+1).

| Class | Update day | Rounds for an object with year Y (+0x40) | Evidence | Game season |
|---|---|---|---|---|
| World_Club_Champ | 20 June | Draw about 5 Feb Y. Groups 29 Jul, 2 Aug, 5 Aug Y. Knockout 9 and 12 Aug Y | 0x929c36, 0x929435..0x92980c | Y. Odd Y from 2003 |
| World_Club_Cup | 20 June | Draw 10 Oct Y, match 1 Dec Y | 0x92b834, 0x92b84f | Y |
| Inter_Amer_Cup | 20 June | Draw 10 Oct Y, match 26 Jan Y+1 | 0x632404, 0x63241d | Y |
| Con_Merc_Cup | 21 February | Start 1 Aug Y. Groups 9 Sep, 23 Sep, 7 Oct, 21 Oct, 4 Nov, 18 Nov Y. Semi-final 25 Nov, final 9 Dec Y | 0x4c1f59, 0x4c1a00..0x4c1c40 | Y |
| Conmebol_Merc | 10 January, and its own call on 1 June | Start 15 Jun Y. Groups 29 Jul, 19 Aug, 2 Sep, 16 Sep, 30 Sep, 14 Oct Y. Knockout 28 Oct, 11 Nov, 2 Dec Y | 0x4c4fe0, 0x4c49d0..0x4c4ca3, 0x4c5ca4 | Y |
| Asia_Cup_Winner | 28 August | One stage: draw 1 Aug Y, matches 23 Aug, 20 Sep Y and 24 Mar Y+1. The other stage: 15 Apr to 26 May Y+1 | 0x40d2b6..0x40d3d6, 0x40d06e..0x40d1c8 | Y |
| Asia_Super_Cup | 28 August (no-op), real start on 25 June | Draw 4 Jul Y, match 4 Aug Y | 0x4109d4, 0x4109ee, 0x410e3d | Y |
| Oceania_Club_Champ | 21 July | Start about 20 Aug Y. Groups 10 to 14 Jan Y+1. Knockout 16 to 19 Jan Y+1 | 0x797a39, 0x7972d1..0x79761f | Y. Every second year |

Inference: which branch of `AsiaCupWinner::create_round_table` 0x40cf90 is the zone stage and which is the finals stage. The function map says zone stage then finals stage. The first draw (1 August) of the Asian Cup Winners' Cup is before its update day (28 August). After a season change on 28 August the draw date of the new year is past, and the Cup update draws it at once (0x51fa45..0x51fa7d). What happens to the past match date of 23 August is unknown. A season jump with a log of this object's +0x40 would settle it.

## 7. Unknowns

- Who writes league table entry +0x1e, which `pick_entrant` reads (0x92aeb4).
- What `Comp_Stats::archive_season_and_reset` 0x4a1c50 adds to the history when the Libertadores write (0x4c41d6) runs on an Intercontinental Cup that does not play that season.
- Whether `Cup::load_from_disk` 0x51fc00 reads +0x40 (the League load does, 0x687b10). A constructor wrap that changes the year argument assumes that the load replaces it.
- The callers of `FIXTURE_MANAGER::remove_fixture` 0x5abc30.
- The result for the Asian Cup Winners' Cup of a first match date before its update day (section 6).
- The effect of FA Cup +0xb3 and +0xb4 (0x56fbaf, 0x56fbb6).
