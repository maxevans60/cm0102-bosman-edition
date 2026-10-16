# Sweep: season end, year constants and transfer rules (0.6.0 slice 1, part sweep-rules)

Date 2026-10-16. Research only. Original: `clean-3968/cm0102.exe` 3.9.68. Addresses are virtual addresses. Evidence is the disassembly (llvm objdump and REA with Ghidra). "Inference" marks a conclusion that I did not prove. Proven names are in `/Users/max/Projects/cm0102/research-0.6.0/sweeps/sweep-rules.jsonl` (26 functions). The `rb_*` and `*_rules` files belong to the sweep-rulebooks part. This doc only cites them.

## 1. What slices 11, 12, 16 and 17 need first

- **The season change is per league table entry, not one global step.** `game_season_update_pass` (0x5d5ae0) runs at the end of every day. Each of the 35 entries has its own update day (+0x15, mostly 20 or 21 June). On that day the entry's competitions call `start_new_season` (vtable +0x8). There is no single "new season" function to hook.
- **`start_new_season` runs only for a finished competition.** The pass calls it only when `Comp::all_stages_complete` (vtable +0xc, 0x48ce10) is true, and that needs `check_complete` (vtable +0x10) to see every match played. A jump with no days played therefore needs match results first (section 2.4).
- **Three year counters exist.** The game date year `0xae2c92`, the entry season years (entry +0x35 start, +0x44 end), and each competition's season year (competition +0x40). The first-season branches test competition +0x40 or `0xae2c92`.
- **The start date comes from the league table, built from the game date.** The game date is set to 1 Jan 2001 at two places (0x81b938, 0x8304ec). Slice 11 must change both and the Europe clamp at 0x83058a (section 3.1).
- **Fixed 2001/02 data is in the code.** The Italian leagues build the real 2001/02 fixture list with absolute dates (2,216 calls). World Cup qualifiers of Europe and South America hold absolute 2001 fixtures. A moved start year leaves these dates in 2001 and 2002 (section 3.3).
- **Transfer windows repeat each year.** They are day and month records with no year, so they need no year change. Per-nation rules are 28 objects found by the club's nation pointer (section 4).
- **The new FIFA transfer rules come in on 31 Aug 2001** (`0xae2330`, set at 0x524cd2). After a moved start they are in force from the first day (section 3.4).

## 2. The end-of-season step (task 1)

### 2.1 Order of events

| Step | When | Function | What it does | Evidence |
|---|---|---|---|---|
| 1 | Every slice | `League::update_for_date` 0x6847c0 (vtable +0x14) | Calls `check_complete(0,0)`. When the league is complete and +0x4c is 0: `club_history_apply_season_records` 0x442e80, `club_records_update_season_records` 0x447430 (league divisions only), `record_season_honours` (vtable +0x30), `comp_screens_update_round_records` 0x4a19f0, then +0x4c = 1. With more stages left it calls vtable +0x28 instead | 0x684994, 0x6849fa, 0x684a19, 0x684a8f, 0x684a99, 0x684a9e |
| 2 | When the last match is in the table | `League::check_complete` 0x689ad0 | Complete when every row has played (clubs - 1) x rounds matches (row +0x11 against +0xa7). First time: `League::pay_prize_money` 0x682e10, then finished flag +0xdd = 1 | 0x689b30, 0x689b51, 0x689ba4, 0x689c06 |
| 3 | End of each day (slice 2) | `game_main_day_loop` 0x5cccd0 calls `game_season_update_pass` 0x5d5ae0 | See steps 4 and 5. Order in the day: discipline, UEFA seeding 0x9035a0, awards, per-player update 0x5ce2d0, host country 0x5fa850, then this pass, then form 0x5d5f20, national teams 0x76b100, finance, regens, transfers | 0x5cd9c5..0x5cdb0f |
| 4 | Day before an entry's update day | `game_season_update_pass` | `Club_records::update_club_records_from_season` 0x447880, `player_stats_refresh_nation_players` 0x7c1350, `FinanceTable::end_season_for_nation` 0x59cdd0 (season-end club update and finance roll-over for the clubs of one nation), `staff_records_remove_player_from_teams` 0x86d6e0 (0x447880, 0x7c1350 and 0x86d6e0 still have first-pass names) | 0x5d5e21 branch |
| 5 | Entry's update day (+0x15) | `game_season_update_pass` | If the entry's nation is selected (nation +0x11c) and entry +0x35 is not the game year: +0x35 and +0x44 += step +0x1d, start date +0x2a = date(+0x32, +0x34, +0x35, +0x37), end date +0x39 = date(+0x41, +0x43, +0x44, +0x46). Entry 3 only: clears byte +0x6d of each record of table `0xae23bc` (meaning unknown). Then for each competition of the entry (+0xc count, +0x10 array) where vtable +0xc is true and vtable +0x2c is 0: `game_update_league_attendances` 0x5d6280 along the +0x20 chain (it sets each club's average attendance and its limits from final position, promotion, relegation and division reputation; it does not touch wages), `start_new_season` (vtable +0x8), `discipline_dispatch_by_id_slot` 0x556210, and once a day a new `simulated_stats` object 0x84b940 | 0x5d5bc8..0x5d5c4f, 0x5d5c66, 0x5d5cab.. |
| 6 | Inside `start_new_season` (top league of a nation) | for example `eng_prm::start_new_season` 0x574c10 | +0x4c = 0, promotion and relegation (vtable +0xb0), new club order (0x5750a0 or 0x574e60), `League::free_table`, archive the season stats (`Comp_Stats::archive_season_and_reset` 0x4a1c50), **competition year +0x40 += 1**, `set_up_rules` (vtable +0x8c), table, `League::create_fixtures` 0x6835c0, `League::sort_table`, predictions, division tiers 0x68a980, then `start_new_season` of each lower league, `review_club_managers`, `OfficialsManager::renew_officials` 0x79cee0 | 0x574c20..0x574e49 |
| 7 | Inside step 6 | `eng_prm::process_promotion_relegation_chain` 0x5752e0 | `record_final_positions` (vtable +0xa4) of all five leagues, then `league_promote_and_relegate` 0x689c80 for each pair. The base `League::process_promotion_relegation_chain` 0x689c20 does the same along the +0x20 chain | 0x575301..0x57548d, 0x689c2f..0x689c60 |
| 8 | 31 Dec to 1 Jan | `game_main_day_loop` | Game date + 1 day after slice 2 (0x5cdc39). When `0xae2c92` changes: `FIXTURE_MANAGER::set_year` 0x5aa2d0 on `0xae2a58` | 0x5cd246..0x5cd25f |

Other leagues follow the same pattern. `ita_ser_a::start_new_season` 0x641700 has the same order (0x641713 promotion, 0x641789 year + 1, 0x64181c fixtures, 0x64194f.. lower leagues). The league sweep lists the overrides of 114 files (`start_new_season`, `process_promotion_relegation_chain`).

International competitions move their year by the edition step in `start_new_season`:

- +2: African Nations 0x401d80, Confederations Cup 0x58c050, Gold Cup 0x5e1590, Oceania club 0x797dbe and nations 0x799940, World Club Championship 0x929d9e.
- +4: Asian Cup 0x40f780, European Championship 0x57b7ca, Olympics 0x7a04b0, World Cup 0x92d409.
- A computed step: the qualifiers (0x57ebb9, 0x9133ec, 0x9194af, 0x91efd9, 0x9214c2, 0x910505, 0x9160ee).

Every other competition uses `inc` (169 sites). This is slice 2 work. I list it here because these steps are real-year rules.

### 2.2 Competition re-creation

Competitions are not made again at the season change. Each object is made once at a new game by the setup function of its league table entry (`Setup.cpp`, for example `create_english_comps` 0x833750 calls the Premier League constructor at 0x83394b, `create_world_club_comps` calls 0x929140 at 0x8311b8). `start_new_season` resets the same object: table, fixtures, rules and year. Inference: a competition that starts later in real life (Conference League, Nations League) needs a new object at game creation that stays idle until its first season, or a new hook in step 5.

### 2.3 Inputs of the step

- Game date `0xae2c90` (day of year at +0, year at +2) and day slice `0xae2c88`.
- League table entries at `0xb63d60` (35 x 0x48 bytes): +0x8 setup function, +0xc competition count, +0x10 competition array, +0x15 update day of year, +0x1d year step, +0x2a start date, +0x32..+0x37 start day, month, year, weekday, +0x39 end date, +0x41..+0x46 end day, month, year, weekday. The `create_*_comps` functions also write +0x1b = 2001 and +0x26 = 0. I found no reader of +0x1b (unknown use).
- For each competition: table rows (played count +0x11 per row), +0xa7 games per club, +0xdd finished, +0x4c season-ended, +0x40 season year, +0x1c parent, +0x20 next stage.
- Club records, finance and player stats tables (step 4).

### 2.4 What a jump with no days played needs (Q265)

The step **needs match results**. Without them `check_complete` stays false, the competition never sets +0xdd, and `game_season_update_pass` skips `start_new_season` for it. Promotion and relegation also read the final positions and the status that `League::update_positions_and_status` 0x684cd0 sets from the table.

A jump flag needs, for each season:

1. A result for every fixture of every competition, added through `add_result` (vtable +0x4) so that the table rows, played counts and status bytes are set. Cups need a winner of every tie so that their `check_complete` is true.
2. One call of each competition's `update_for_date` (vtable +0x14) after the last result, so that prize money, history, records and honours run (steps 1 and 2).
3. The day before and the update day of each entry: `game_season_update_pass` with the game date set to those days (steps 4 and 5).
4. The calendar year change: `FIXTURE_MANAGER::set_year` with the new year (step 8).

Inference: the cheapest jump runs the real day loop with a quick-result stub in place of the match engine. A loop that only calls step 5 skips the prize money, history and records, so the report would differ from a played season.

## 3. The year constants (task 2)

Classes: **F** = first-season branch (tests a season or game year against the start year), **S** = start-date constant (an absolute date or year of the 2001/02 calendar), **R** = real-year rule (a real calendar year or a cycle), **N** = not a year.

### 3.1 Where the start date of a new game is set

| Address | Function | What | Class |
|---|---|---|---|
| 0x81b938 | `cm_main_entry_setup` 0x81b890 | Game date `0xae2c90` = 1 Jan 2001 at program start | S |
| 0x8304ec | `cleanup_world_state_and_delete_tmp` 0x82ff40 | Game date = 1 Jan 2001 again after a game ends | S |
| 0x667150 | `key_nation_build_table` | Entry start and end dates from the year of the game date (`0xae2c92`). Calendar-year leagues use year + 1 | uses game year |
| 0x830520 | `setup_pick_start_date` | Start date `0xdd6ec8` = +0x2a of the first selected entry. For a European nation, when the year is 2001 (0x83058a) and the date is after 1 Sep 2001, it uses 1 Sep 2001 (0x830597, 0x8305db) | S |
| 0x828480 | `create_game` 0x828110 | Start date minus 4 weeks | none |
| 0x5cc850 | `game_run_to_start_date` | Runs every day from 1 Jan of the year to the start date | none |

So the start year is the year of the game date when `key_nation_build_table` runs. Slice 11 must set the game date to 1 Jan of the chosen year at both S sites and move the 2001 test and date at 0x83058a, 0x830597 and 0x8305db.

### 3.2 Every compare, subtract or move of 2001, 2002 or 2003

99 sites. Field +0x40 is the competition season year. `0xae2c92` is the game year. `[reg+0x2]` is the year of a date.

| File | Address | Test | Meaning | Class |
|---|---|---|---|---|
| bel_super.cpp | 0x421513 | +0x40 == 2001 | Ballot scheme first season | F |
| bel_third.cpp | 0x4224ec | +0x40 == 2001 | Club slots and size 16 first season | F |
| club_screens.cpp | 0x45582d, 0x45f979 (+ push 2001 at 0x45583c, 0x45f980) | year == 2001, builds a 2001 date | Screen text for the first season (inference) | F |
| club_screens.cpp | 0x45f4e9 | year == 2001 | Fixture list panel (meaning unknown) | F |
| club_screens.cpp | 0x483322 | date year < 2001 | Date before the game start | S |
| conmebol_seeding.cpp | 0x4c64fe | game year > 2002 | Seeding from the third game season. Real change or not: unknown | F |
| eng_fa_cup.cpp | 0x56fc5f | +0x40 == 2001 | FA Cup round list | F |
| european_awards.cpp | 0x5836c4 | date year == 2001 | Skip awards in the first season | F |
| fifa_confed.cpp | 0x58b385 | year == 2001 then year + 1 | 2001 edition was before the game start | S |
| fifa_confed.cpp | 0x58cd13 | +0x40 == 2002 | Player pick (meaning unknown) | F |
| fin_prm.cpp | 0x59b56b | mov 2002, else start-date year + 1 | Contract expiry window | S |
| fix_man.cpp | 0x5aee52 | 2001 + bit | Fixture manager's two calendar years | S |
| fix_man.cpp | 0x5af218, 0x5af243, 0x5af288, 0x5af2b9 | [+0x28] == 2001 | Slot pair of the 2001 year (inference) | S |
| fra_cup.cpp | 0x5bab83, 0x5bac2f, 0x5bace5 | +0x40 - 2001 | Entrants 184 in 2001, 182 later | F |
| fra_first.cpp | 0x5bb6a7, 0x5bc2bd, 0x5bc41c, 0x5bc8ae | +0x40 == 2001 | Ligue 1 18 clubs first season, 20 later | F |
| fra_lge_cup.cpp | 0x5bd237 | +0x40 - 2001 | Team list | F |
| fra_second.cpp | 0x5bf0aa | +0x40 == 2001 | Rules first season | F |
| fra_super.cpp | 0x5bf6c6 (+ push 2001 at 0x5bf6d3) | +0x40 == 2001 | Fixed 2001 match date | F, S |
| fra_third.cpp | 0x5c070a | +0x40 == 2001 | Rules first season | F |
| friendly.cpp | 0x5c3065 | game year == 2001 | Friendlies of the first pre-season | F |
| ger_cup.cpp | 0x5d92b4 (+ push 0x5d92bd) | +0x40 == 2001 | Fixed 2001 round dates | F, S |
| ger_first.cpp | 0x5d9d31 (+ push 0x5d9d3b) | +0x40 == 2001 | Fixed 2001 schedule | F, S |
| ger_first.cpp | 0x5db23e | +0x40 == 2001 | Team table, size 18 | F |
| goldcup.cpp | 0x5e1d65 | +0x40 == 2001 | Team list | F |
| host_country.cpp | 0x5fa89b | date year == 2001 | Skip in the first season | F |
| inter_amer_cup.cpp | 0x632540, 0x63266c | +0x40 == 2001 | Qualifier lists | F |
| international_awards.cpp | 0x632e30 | date year == 2001 | Awards first season | F |
| kor_league.cpp | 0x66b1f1 (+ push 0x66b1fb) | +0x40 == 2002 | Fixed 2002 schedule (calendar year) | F, S |
| match_official.cpp or match_pl.cpp | 0x6eb28d | game year >= 2002 | Non-playing staff in the match engine (meaning unknown) | F |
| Menubar.cpp | 0x7647f7, 0x76488d, 0x764f7e | cx == 2001, 2003, eax == 2002 | Menu command ids in a chain with 2000 to 2023 | N |
| news.cpp | 0x78164b, 0x781671, 0x781697, 0x7835fb, 0x783602, 0x785102, 0x78510d, 0x785118 | eax == 2000 to 2003 | News type ids in a switch (inference) | N |
| oceania_club_champ.cpp | 0x797042 | +0x40 < 2001 then + 1 | First edition | S |
| oceania_nations.cpp | 0x7988e9, 0x798918 | < 2002 then 2002, parity from 1996 | First edition 2002, every two years | S, R |
| old_international_awards.cpp | 0x79e8a0 | date year == 2001 | Awards first season | F |
| pol_lge_cup.cpp | 0x7ca7c3, 0x7cad3f | +0x40 == 2001 | Rounds, size 36 first season | F |
| pol_second.cpp | 0x7cb4b8, 0x7cbdfd, 0x7cbf4f | +0x40 == 2001 | Schedule, size 20, rules | F |
| por_second_b.cpp | 0x7d240c | +0x40 == 2001 | Size 20 first season | F |
| Setup.cpp | 0x83058a | start date year == 2001 | Europe clamp | S |
| Setup.cpp | 0x830fb0, 0x8312b0, 0x83161e, 0x8318c2, 0x831b69, 0x831e7c, 0x8320c9, 0x8371d1 (and 0x833ca3) | mov entry +0x1b, 2001 | Entry year word, use unknown | S |
| spa_second_b.cpp | 0x856cdc | +0x40 == 2001 | Size 20 first season | F |
| spain_rules.cpp | 0x8595ad, 0x8595b3 | year == 2001 or 2002 | Non-EU limit 3, else 2 (sweep-rulebooks) | R |
| squad_manager.cpp | 0x85b83d (+ date 1 Jul 2003 at 0x85b897) | date year < 2003 | Meaning unknown | S |
| stadium.cpp | 0x85c407 | year < 2003 | Meaning unknown | S |
| transfer_manager.cpp | 0x8c7aa9 | game year == 2001 | Player turns down offer, first season | F |
| tur_first.cpp | 0x8fddce | +0x40 == 2001 | Size 18 first season | F |
| uefa_seeding.cpp | 0x9038af, 0x903b6f, 0x903e1a, 0x903e32, 0x903e64, 0x903e73, 0x903e8c, 0x903eee, 0x9059b2 | game year == 2001 | First-season pots and places | F |
| uefa_seeding.cpp | 0x9041ec | [+0x439] == 2001 | Read euro.cfg only in the first season | F |
| wc_europe_league.cpp | 0x91dbc4 | fixture year 2000 to 2001 | Fixed 2002 World Cup qualifiers | S |
| wc_oceania_league.cpp | 0x91f486 | date year == 2001 | Fixed 2001 qualifiers | S |
| wc_south_american_league.cpp | 0x9223ab | fixture year 2000 to 2001 | Fixed qualifiers | S |
| world_club_champ.cpp | 0x9291b2 | < 2003 then + 1 | First edition 2003 | S, R |
| world_club_champ.cpp | 0x92af4e, 0x92b24b | +0x40 == 2001 | Club pick | F |
| world_club_cup.cpp | 0x92b970, 0x92ba9c | +0x40 == 2001 | Qualifiers | F |
| world_cup.cpp | 0x92dfc4, 0x92eb46, 0x92f240 | +0x40 == 2002 | Fixed 2002 World Cup (teams, groups) | S |
| year_ratings.cpp | 0x931ae9 | game year == 2001 | Ratings first season | F |
| (library) | 0x93e9b4 | cx == 2000, 2001 | Glyph code | N |

### 3.3 Pushes of 2001, 2002 and 2003 (2,553 sites)

| Group | Sites | Callee | Meaning | Class |
|---|---|---|---|---|
| Italian leagues (`ita_ser_a` 0x642a70, `ita_ser_b` 0x647230, `ita_ser_c1a` 0x64be90, `ita_ser_c1b` 0x650100, `ita_ser_c2a` 0x6545c0 and 0x656570, `ita_ser_c2b` 0x658750, `ita_ser_c2c` 0x65c8e0) | 2,216 | `fixed_fixture_set_by_club_names` 0x68aff0 | Real 2001/02 fixtures with absolute dates. Built only in each constructor (for example 0x6407c5), so only in the first season | S |
| World Cup qualifiers, Europe | 162 | 0x91db60 | Fixed 2001 fixtures | S |
| World Cup qualifiers, South America | 40 | 0x922350 | Fixed 2001 fixtures | S |
| `key_nation_build_table` | 33 | `Date::Date` 0x549f70 | Update day (+0x15) of each entry. Only the day-of-year word is kept, so 2001 only sets the leap rule | S |
| Setup.cpp `create_*_comps` | 35 | 0x549f70 | Update day +0x15 per entry (20 June mostly) | S |
| Setup.cpp game date | 2 | `Date::set` 0x54a130 | 1 Jan 2001 (section 3.1) | S |
| Setup.cpp Europe clamp | 2 | 0x549f70 | 1 Sep 2001 | S |
| Rule change date | 1 (0x524cd2) | 0x549f70 | 31 Aug 2001, new FIFA transfer rules | R |
| Award update day | 2 (0x413385, 0x4140e4) | 0x549f70 | 20 June 2001 (day of year) | S |
| Club history | 2 (0x44426f, 0x444296) | 0x549f70 | 1 Feb 2001 and 20 June 2001 against the game date | S |
| Contract offer | 1 (0x4dc134) | 0x549f70 | 1 June 2002 (inference: earliest contract end) | S |
| Manager manager | 1 (0x69e84d) | 0x549f70 | 23 July 2003, meaning unknown | S |
| First-season fixed dates (fra_super, ger_cup, ger_first, kor_league) | 4 | `cup_set_round_match_date` 0x521eb0, `league_round_set_date` 0x68a160 | Inside the F branches above | S |
| Text links, widgets, news items | about 60 | `Text::add_link` 0x4ae8a0, `display_add_area_*` 0x8040a0 and 0x804100, `NewsItem::NewsItem` 0x77bbe0, `langlib_sprintf` 0x66f4e0 | Ids, not years | N |

### 3.4 Other year constants on year fields

These are not 2001 to 2003 but test a year field. They matter for later seasons.

| Address | Test | Meaning | Class |
|---|---|---|---|
| 0x572d03, 0x574b32, 0x576742, 0x578082 | game year > 1999 | English prize funds (Premier League 12,000,000 else 5,000,000) | R, always true |
| 0x56fb89, 0x57a5f2 | +0x40 > 1999 | FA Cup, European Super Cup | R, always true |
| 0x7ee022 .. 0x7f4f3b (12 sites, sco_*) | +0x40 vs 1999 | Scottish leagues and League Cup | R, always true |
| 0x57dbb5, 0x57fa5a, 0x582f4a, 0x582f54 | year vs 1998, 1999 | Euro qualifiers | R |
| 0x5dcf0c, 0x5dd7fb, 0x5dd9fa, 0x5de005 | +0x40 vs 2000 | German regional leagues | R, always true |
| 0x7a09f0, 0x7d31c0, 0x7d322a, 0x914ea1, 0x914f4f, 0x915010, 0x916479 | +0x40 vs 2000 | Olympics, Portuguese Super Cup, CONCACAF qualifiers | R |
| 0x859e26 | game year == 1998 | Squad rebuild | R, never true |
| 0x85da2b | game year >= 2004 | `stadium_attendance_update` returns 0 before 2004 | R, becomes true in a game |
| 0x579c8d | date 19 June 1998 | England loan period | R, always past |
| 0x8595c1, 0x85959f | year 2000 | Spain rules | R |

Compares with 2000 elsewhere are money or attribute values, not years.

### 3.5 Reads of the game year global

`0xae2c92` is read at 520 sites in 208 functions. 15 sites compare it with a constant (all in 3.2 and 3.4). 31 sites pass it to a date constructor (season dates in `key_nation_build_table`, `Setup.cpp` and the transfer window code). One site divides by it (0x4ce135, contract wage). Inference from the sites read: the rest compute ages, years at a club or record ages by subtraction. Most reads are in Setup.cpp (108), key_nation.cpp (87), player_regen.cpp (34), manager_manager.cpp (25) and transfer_manager.cpp (17). Relative tests in the transfer code: 0x8c6f86 (years since a record >= 5, then the club's nation goes to the staff record, inference: residence), 0x8c7387 (records older than 4 years dropped), 0x8ca089 (years at the club). None of these is tied to 2001. I did not classify each of the other reads one by one.

### 3.6 The new FIFA transfer rules date

- `init_transfer_rule_change_date` 0x524cd0 sets `0xae2330` = 31 Aug 2001. `0x9b5980` = 1 in the data section turns the feature on.
- `game_main_day_loop` 0x5cdb2c: when the feature is on, the rules are not yet in force (`0xae233c` = 0) and the game date is later than `0xae2330`, it sets `0xae233c` = 1 and `0xa83608` = 5 (0x5cdb5d, 0x5cdb64) and calls `news_post_transfer_system_change` 0x58a890 with 1. On the day 25 days before the change date it calls the same function with 0 (0x5cdb85..0x5cdbb6). Inference: an advance news item. `game_run_to_start_date` does the same at 0x5ccbe8. `load_game_data` checks it at load (0x82cde9).
- `load_fifa_transfer_rules` 0x839760 reads both flags from a save.
- Readers of `0xae233c`: contract manager 0x4ce1c6, staff contracts 0x8650cd, 0x86315d, 0x86824d, transfer fee 0x8d0853, dispute 0x569e60, 0x56a18b, screens. Inference: they switch contracts and fees to the post-Bosman FIFA rules. The meaning of state `0xa83608` is unknown.

## 4. Transfer rules (task 3)

### 4.1 Objects and lookup

- `TRANSFER_MANAGER::setup_transfer_rules` 0x8d26b0 makes 28 rules objects of 0x19 bytes and stores them at TRANSFER_MANAGER +0x8ac to +0x918. Object 0 is the European object (0x583280, continent `0x9cfa10`, scope 2). Objects 1 to 27 are the national objects in this order: Argentina, Australia, Belgium, Brazil, Croatia, Denmark (inference, 0x555460), England, Finland, France, Germany, Greece, Holland, Ireland, Italy, Japan, Norway, Northern Ireland, Poland, Portugal, Russia, then the rest up to Wales and Korea (0x8d2dce, 0x8d2e11). It then copies every window edge into a table at +0x8a4 (count +0x8a8) and sorts it with 0x8e5590.
- `TRANSFER_RULES::TRANSFER_RULES` 0x8e84a0: vtable 0x970458, +0x4 window array (12-byte records), +0x8 window count, +0x9 flag, +0xa nation pointer, +0xe continent pointer, +0x12 scope (1 nation, 2 continent, 4 world), +0x13 = 50, +0x14 = 30. These are the foreign player signing caps: `TRANSFER_RULES::can_sign_player` 0x8e8560 reads both, `TRANSFER_RULES::can_make_offer` 0x8e8800 reads +0x13. The sweep-rulebooks doc reads +0x14 as the non-EU cap (section 1, point 4).
- Lookup: a linear scan of the 28 pointers for rules +0xa equal to the club's nation (club +0x53). Examples: `find_open_window_date` 0x8cf4c7, `rules_can_end_loan` 0x8d054e, the restriction lookup 0x8cf6b0 (see `2026-10-16-foreign-and-tactics-positions.md`). A club whose nation has no object gets no rule.
- Base vtable 0x970458 slots: +0xc `TRANSFER_RULES::can_make_offer` 0x8e8800, +0x10 `TRANSFER_RULES::can_sign_player` 0x8e8560, +0x18 `get_ineligibility_code` 0x8e8a60, +0x1c fill squad restrictions, +0x20 can loan, +0x24 can end loan, +0x28 loan period 0x8e8bd0, +0x2c `post_deadline_news` 0x8e9080, +0x30 `write_deadline_news_text` 0x8e91b0. England's vtable is 0x969f4c with its own slots +0x0, +0x8, +0x18, +0x1c (0x579c50), +0x20, +0x28.

### 4.2 Transfer windows

- `TRANSFER_RULES::find_open_window` 0x8e8e50 tests a date against each window with `transfer_window_contains_date` 0x8e8c40. A rules object with no windows is always open.
- `transfer_window_contains_date` builds the start and end dates for the date's own year with `transfer_window_get_date` 0x40a7d0 (argentina_rules.cpp, sweep-rulebooks). A window whose start month is later than its end month runs over the new year. The window records hold day, month and other bytes but **no year**, so windows repeat every year.
- England has one window, written in its constructor at 0x579c03..0x579c2f: start month 5 (June), end month 2 (March), end day byte 26. Inference: open from June to the March deadline. The other bytes of the record are not decoded.
- `TRANSFER_MANAGER::find_next_window_open_date` 0x8cf470 steps forward up to 365 days for the next open date. `TRANSFER_RULES::can_sign_player` 0x8e8560 and `TRANSFER_RULES::can_make_offer` 0x8e8800 test a deal against the windows. `TRANSFER_RULES::get_days_until_window_closes` 0x8e8ed0 counts the days to the end of the open window (365 when none is open). `TRANSFER_MANAGER::update_transfer_window_news` 0x8c6bc0 reads the sorted edge table and posts the deadline news.
- Year tests: none in the window code. `get_days_until_window_closes` reads the game year only to build the dates (0x8e8f50).

### 4.3 Foreign players and squad registration

- The per-nation filler (vtable +0x1c) writes a 42-byte restriction record: +0..+2 foreign limits, +3..+5 non-EU limits, +0x16 and +0x17 age limit. Full table of fillers and readers: `2026-10-16-foreign-and-tactics-positions.md`. England's filler is 0x579c50 (+5 = 3).
- `transfer_manager_check_matchday_squad_limit` 0x8cf880 applies the match squad limits of the restriction record: it counts the non-EU, foreign or over-age players of a matchday squad. `TRANSFER_RULES::can_sign_player` 0x8e8560 and `TRANSFER_RULES::can_make_offer` 0x8e8800 apply the signing caps at rules +0x13 and +0x14. `transfer_manager_check_squad_limits` 0x8cd450 is not a foreign player rule: it keeps a club within 50 players and its staff slots. (Opus names of the function map, checked against the code on 2026-10-16.)
- European registration of 25 players is in squad_manager.cpp (0x85b05a, 0x85b0be, cited from the change-points doc).
- Year tests: Spain 0x8595ad and 0x8595b3 (sweep-rulebooks). None in transfer_manager.cpp or transfer_offer.cpp.

### 4.4 Work permits

- `TRANSFER_MANAGER::player_needs_work_permit` 0x8c7560: true when the club's nation byte +0x76 is 14, the player is not exempt (`transfer_offer_is_blocked` 0x53d740, first-pass name), and he has no permit in the sorted list at +0x93a (count +0x93e, 14-byte records, comparator 0x8e55c0). Inference: +0x76 is a nation group code, and 14 marks the work permit nations. Which nations have 14 is in nation.dat (not read).
- `TRANSFER_MANAGER::add_work_permit` 0x8c7070 adds a permit. `TRANSFER_MANAGER::update_work_permit_on_move` 0x8c7e20 stamps the game year in the record (+0x8, store at 0x8c7f4a). `TRANSFER_MANAGER::update_work_permits` 0x8c7330 (test at 0x8c7387): a permit that is 5 or more game years old sets the staff record's nation field +0x1e to the permit's nation (record +0xa) and leaves the list. The function map reads +0x1e as the second nationality (not proven here). A newer permit leaves the list when the staff record has no player record (+0x61 = 0).
- No real-year test in the permit code. A Brexit rule (R10) needs new code.

### 4.5 Files with no transfer rules

- `ruling_body.cpp` (19 functions) holds discipline: bans, suspensions, points. No transfer, window or registration code. Its one year read (0x7e7f84) ages discipline entries.
- `new_transfer_rule_screens.cpp` (2 functions) shows the transfer regulations information screen (0x77b850) and its command dispatch (0x77bb30). No rule checks.
- `comp.cpp`: `Comp::set_fixture_substitutes` 0x48f2d0 writes the substitutes word (+0x4d) from two nibbles of the competition (cited in the change-points doc). No transfer code.

## 5. Corrections to earlier docs

- `2026-10-16-season-change-points.md` says key_nation.cpp has 4 sites of `push 2001` for "league table season labels". There are 33, and they set the season update day of each entry (+0x15).
- The same doc lists news.cpp 0x781671 as a test of 2002 and 2003. Inference: these are news type ids, not years.
- The same doc says the Setup.cpp 2001 sites are "new game set-up of the 35 leagues". They are the update days (+0x15) and a year word (+0x1b) per entry.
- First-pass names that are wrong: 0x830520 is the start date picker (`setup_pick_start_date`), not `setup_check_team_control_slots`. 0x5d5ae0 is the season update pass. 0x48ce10 is the completion test. 0x5aa2d0 sets the fixture manager's year.

## 6. Unknowns

- The use of entry +0x1b (2001) and of the table at `0xae23bc` that entry 3 clears.
- The meaning of `0xa83608` (state 5 after the FIFA rules change).
- The meaning of rules bytes +0x13 (50) and +0x14 (30), and of the window record bytes other than the months and the end day.
- Which nations have work permit code 14 at nation +0x76.
- The 2003 tests at 0x85b83d and 0x85c407 and the 2002 test at 0x6eb28d.
- Whether cups use the same +0xdd finished flag. I read only the league path.
