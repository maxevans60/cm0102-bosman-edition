# The Copa America class as a Gold Cup, and a class for the Finalissima (0.6.0 slice 28, part C)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses, read with capstone. Brief: `slice28-research-brief.md` (scratchpad). Related: `2026-10-18-new-comps.md` (the module), `2026-10-18-international.md` (slice 27), `src/recomp_rt/win/season_intl_copa.c`, `season_intl.c`, `season_intl_rules.c`, `season_kind_club_world.c`, `tools/recomp/seasons.py`.

Key id names come from `docs/research/data-map/globals.tsv`. A key id holds a record index: the creator passes `[0xae23d0] + [key] * 0x6b` as the record (0x830e17..0x830e2e), and the finals tests compare `[record + 0]` with the key (0x76d948..0x76d992). So for the Copa America, `[[this + 4]]` equals `[0x9cf7a8]`.

## 1. What the build needs first

1. The class can hold a Gold Cup of 12 (3 groups of 4, two best thirds, quarter-finals: the editions 2005 to 2017) and, through slice 27's code, of 16. It cannot hold the 2002 and 2003 editions (4 groups of 3).
2. Nearly every Copa America fact of the class is in a direct call or an inline instruction, not in a vtable slot. A vtable copy alone changes almost nothing. The build needs object tests at sites and in wraps.
3. The object test must use the record (`[this + 4]`, written at 0x5e06b9), not the vtable. The constructor writes the class vtable at 0x5e06af and calls the stage builder directly at 0x5e073d, before `make` can write a copy.
4. Slice 27's year sites (0x5e06fb, 0x5e1590) and date shift (wraps of 0x5e12d0 and 0x5e0900) use the class Copa America (class index 3, `INTL_COPA`), not the object. A second object takes the Copa America's years and dates in a season game. Slice 27's 16-team wraps test the vtable (`copa16`), so a second object with a vtable copy takes the 12-team path.
5. Finalissima: `Eur_Super_Cup` (ctor 0x57a100, vtable 0x969f80, size 0xb2) is the cheapest class. Its picker 0x57a500 already takes two history winners by key id, and a wrap that reads the Euro (0x9cf7a4) and Copa America (0x9cf7a8) records gives the Finalissima pair. Change its round table (slot 15, 0x57a3c0): date, rule flags and venue flags.

## 2. The vtable 0x96b400 (League, 45 slots)

Base = the League vtable 0x96d020 (45 slots, `2026-10-16-sweep-europe.md`). Compared slot by slot.

| Index | Offset | Function | Base or own |
|---|---|---|---|
| 0 | +0x00 | 0x5e0820 scalar deleting destructor (calls 0x5e0840) | own (base 0x6822e0) |
| 1 | +0x04 | 0x685d30 | base |
| 2 | +0x08 | 0x5e1530 start_new_season | own (base 0x682760) |
| 3 | +0x0c | 0x48ce10 | base |
| 4 | +0x10 | 0x689ad0 | base |
| 5 | +0x14 | 0x5e2120 update_for_date | own (base 0x6847c0, which it calls at 0x5e231d) |
| 6 | +0x18 | 0x687b10 League load | base |
| 7 | +0x1c | 0x6891c0 League save | base |
| 8 | +0x20 | 0x48ceb0 | base |
| 9 | +0x24 | 0x48cea0 | base |
| 10 | +0x28 | 0x5e1620 advance_stage | own (base 0x5a8f60) |
| 11 | +0x2c | 0x48ce70 | base |
| 12 | +0x30 | 0x40f590 jump to vtable +0x30 of stage[2] | own, shared with the Asian Cup (0x967644); slice 27 wraps it |
| 13 | +0x34 | 0x583470 | base |
| 14 | +0x38 | 0x48cab0 | base |
| 15 | +0x3c | 0x5e0900 round template | own (base 0x682650) |
| 16 | +0x40 | 0x686670 | base |
| 17 | +0x44 | 0x5e13b0 stage result, staff records | own (base 0x686940) |
| 18 | +0x48 | 0x5e0f50 result news text | own (base 0x48c6d0) |
| 19 | +0x4c | 0x48dfa0 | base |
| 20 | +0x50 | 0x48e180 | base |
| 21 | +0x54 | 0x68a3e0 | base |
| 22 | +0x58 | 0x48e1c0 | base |
| 23 | +0x5c | 0x5e2340 predicted positions | own (base 0x68a850) |
| 24 | +0x60 | 0x48e360 | base |
| 25 | +0x64 | 0x5bb550 | base |
| 26 | +0x68 | 0x5e2500 final positions | own (base 0x48e380) |
| 27 | +0x6c | 0x583470 | base |
| 28 | +0x70 | 0x4011c0 | base |
| 29 | +0x74 | 0x4011d0 | base |
| 30 | +0x78 | 0x5dce00 | base |
| 31 | +0x7c | 0x48f2d0 | base |
| 32 | +0x80 | 0x68abd0 | base |
| 33 | +0x84 | 0x68ac40 | base |
| 34 | +0x88 | 0x5dce00 | base |
| 35 | +0x8c | 0x5e12d0 set_up_rules (stage builder) | own (base 0x6826f0) |
| 36 | +0x90 | 0x68aad0 | base |
| 37 | +0x94 | 0x684cd0 | base |
| 38 | +0x98 | 0x684790 | base |
| 39 | +0x9c | 0x6847b0 | base |
| 40 | +0xa0 | 0x687a20 | base |
| 41 | +0xa4 | 0x68aa30 | base |
| 42 | +0xa8 | 0x4011e0 | base |
| 43 | +0xac | 0x4011f0 | base |
| 44 | +0xb0 | 0x689c20 | base |

## 3. The functions of the class (0x5e0660..0x5e2580)

The class is 0x5e0660..0x5e2580. The rows of `goldcup.cpp.md` from 0x5e25d0 on are graphics code (sweep-international, scope note). Callers are from a scan of every `call`/`jmp` rel32 and every data word in the image.

| Function | Slot or direct (callers) | What it reads and writes |
|---|---|---|
| 0x5e0660 ctor | direct: 0x830e32 in `create_other_int_comps` 0x830ac0. Size 0xf6 (0x830ddb). Stored in the international entry array, slot 4 (0x830e4a) | League base 0x682200 (0x5e0683). vtable 0x96b400 (0x5e06af), year +0x40 (0x5e06b5), record +4 (0x5e06b9). Rounds the year up until `(year - 1996) % 2 == 1` (0x5e069a `sub edx, 0x7cc`, loop 0x5e06dd..0x5e06f9): odd years. Load 0x687b10 (0x5e0703). Stage array malloc 0xc (0x5e0710), count 3 (0x5e0729). Direct call of set_up_rules 0x5e12d0 (0x5e073d). Comp_Stats 0x5ce bytes (0x5e0773), ctor 0x49ee70 (0x5e0793), at +8. No key id read |
| 0x5e0840 dtor | direct: 0x5e0823 | frees table, schedule, stages, stats |
| 0x5e0900 template | slot 15; called by vtable at 0x5e1386 and by slice 27's knockout | Groups: first round Date day 6, month 6 (6 July) through 0x68a160 (0x5e09a5); index -1..1 groups, 2 knockout (0x5e0912). Knockout: quarter-final draw 15 July (0x5e0c74), match 16 July (0x5e0c8b); final round draw 20 July (0x5e0ea4), match 22 July (0x5e0ec6). Rule flags 0x103 (QF, SF) and 0x3 (third place, final): sweep-international section 7. All years from +0x40 |
| 0x5e0f50 result news | slot 18 | Strings 0x98737c ("qualify for quarter finals"), 0x98732c..0x98713c (win, runners-up, third, fourth, through, knocked out). The competition name comes from the record (`[edi + 4]`, 0x5e0f77). No string names the Copa America |
| 0x5e11e0 group A table | direct: 0x5e2200 | +0x3e = 4, malloc 0xec (4 rows of 0x3b) at 0x5e11e9. Teams `3 * k` of +0xee (0x5e128e, a slice 27 site) |
| 0x5e12d0 set_up_rules | slot 35, and direct from the ctor (0x5e073d); by vtable from 0x5e15a1 | Rule bytes +0x43, +0x49 = 7, +0x4a, +0xbe..+0xc8 (0x5e12f5..0x5e1354). Draw date Date(20, 2, year, 5) = 20 March (0x5e1358) into +0xb6, +0xb8 = 0. Template index -1 through vtable +0x3c (0x5e1386) into +0xba |
| 0x5e13b0 stage news and staff records | slot 17 | Staff records 0x86bdd0, 0x86c1d0, 0x86c000, 0x86a800 (cup winners), 0x86b0b0, 0x86b710 with `[0xdd7f00]` (0x5e13d3..0x5e1515). Knockout index 2 at 0x5e13b5 and 0x5e1437 (slice 27 sites) |
| 0x5e1530 start_new_season | slot 2 (by the season pass) | 0x687970, frees +0xba, Comp_Stats archive 0x4a1c50 (0x5e1567), deletes stages, `add word [esi+0x40], 2` (0x5e1590), set_up_rules by vtable +0x8c (0x5e15a1) |
| 0x5e1620 advance_stage | slot 10 | at stage index 2 (0x5e169f) calls 0x5e18a0 (0x5e16a4) |
| 0x5e16b0 group stage B or C | direct: 0x5e22e5 (loop of 2, 0x5e22eb) | 4 teams with a step of 3 in +0xee (0x5e172e, 0x5e1735) |
| 0x5e18a0 knockout | direct: 0x5e16a4 | ranks the thirds, random draw 0x4afb40, template index 2, Cup_Stage of 8 at stage[2] (slice 27 doc) |
| 0x5e1cd0 entrants | direct: 0x5e21f2 | section 4 |
| 0x5e2120 update_for_date | slot 5 | On the draw day (+0xb6, year +0x40 + +0xb8, 0x5e2155..0x5e21d4) with +0x45 = 0 and a full update: entrants 0x5e1cd0 (0x5e21f2), predicted positions by vtable +0x5c (0x5e21fb), group A 0x5e11e0 (0x5e2200), 0x6835c0, 0x6827d0, groups B and C (0x5e22e5), frees the list +0xee (0x5e22f7), +0x45 = 1. Then League update 0x6847c0 |
| 0x5e2340 predicted positions | slot 23 | 12 fixed calls of 0x4a2540 (0x5e2442..0x5e24dc) |
| 0x5e2500 final positions | slot 26 | group codes below stage 2 (0x5e2516). The word at 0xabcd6f that also matches 0x5e2500 is unaligned (inference: not a pointer) |

History: the class itself writes no history row. It archives Comp_Stats (0x4a1c50 at 0x5e1567) and writes staff records (slot 17). The readers of a Copa America winner take it from the history of the record (`comp_util_get_latest_placed_club` 0x4afd80 with the record pointer, 0x58cdc3..0x58cdda). Inference: the history is keyed by the record, so a second object on its own record has its own history.

## 4. The entrants 0x5e1cd0

- Count 12 into +0xf2 (0x5e1cde), malloc 0x30 (0x5e1cda) into +0xee.
- Year 2001 (0x5e1d65 `cmp ax, 0x7d1`): a fixed list of 12 key ids (0x5e1d6f..0x5e1f24): Colombia 0x9cf29c, Brazil 0x9cf260, Costa Rica 0x9cf2a8, Chile 0x9cf294, Mexico 0x9cf3d0, Honduras 0x9cf33c, Ecuador 0x9cf2d4, Peru 0x9cf428, Uruguay 0x9cf4f4, Venezuela 0x9cf504, Paraguay 0x9cf420, Bolivia 0x9cf254, each through `national_team` 0x53b3d0.
- Other years: `HostCountry::find_hosts` 0x5fa730 on `[0xb5e134]` with key `[0x9cf7a8]` (0x5e1f55), the year (0x5e1f54), two out slots and flag 1 (0x5e1f46). The hosts go first (0x5e1f71..0x5e1fa0).
- Then every nation in nation order (0x5e1fbd..0x5e206c) until the count is 12 (0x5e1fc8). A nation goes in when its continent `[[nation + 0x71]]` is `[0x9cfa1c]` South America (0x5e1fd7), or it is a guest: Japan 0x9cf36c in 1999 (0x5e1fe5, 0x5e1fed), else Canada 0x9cf280 (0x5e1ffe), and Mexico 0x9cf3d0 in every year (0x5e2009). It is skipped when it is one of the hosts (club +0x53 compare, 0x5e2013..0x5e203a).
- The count must be exactly 12 (0x5e207c), else the error box (line 0x3bc). Then qsort by reputation 0x4b07d0 (0x5e20fe).
- The host rows of the Copa America: 6 rows from 1999 with a step of 2, key `[0x9cf7a8]` (0x5fa177..0x5fa1ac, in `init_default_table` 0x5f9970). Slice 27 replaces them from the edition table.

Slice 27 (`season_intl_copa.c`) wraps 0x5e1cd0 (`copa_entrants`). The wrap runs `entrants16` only when `copa16` passes: season game, vtable 0x96b400, an edition of 16 of `goldcup` (the Copa America file of the table), year 2016 or 2024. It asks find_hosts with `rd32(COPA_KEY)`, takes the CONMEBOL nations by `CONTINENT_SOUTH_AMERICA` 0x9cfa1c, the six guests of its year by name, and fills with `CONTINENT_NORTH_AMERICA` 0x9cfa14.

What a second object needs so that it takes CONCACAF nations:

1. The host key: 0x5e1f55 reads the global, not the object. A site there that reads `[[esi + 4]]` (the record id) gives the same value for the Copa America and its own key for a second object. Its host rows then need that key (slice 27's `init_default_table` wrap writes rows by key).
2. The continent: 0x5e1fd7 reads `[0x9cfa1c]`. North America is `[0x9cfa14]` (`g_key_continent_north_america`).
3. The original loop is not usable for CONCACAF even with the continent changed. It stops at the first 12 nations in nation order (0x5e1fc8), not the 12 strongest, because South America has exactly 10 nations plus 2 guests. CONCACAF has many more nations. So the Gold Cup needs its own entrants in the wrap of 0x5e1cd0 (hosts, then a fixed list or the strongest by reputation, as `entrants16` does).
4. The guests (Canada, Mexico) are CONCACAF, so for a CONCACAF object they add nothing wrong. The year 2001 list and the 1999 guest never run for an odd year from 2005.

## 5. Shapes, cycle and dates

Shapes:

- Original, 12 teams: group A is the League (3 rounds of 4), stage[0..1] are groups B and C (0x5e22e2..0x5e22ee), stage[2] the Cup_Stage of 8 with the group winners, runners-up and the two best thirds (0x5e18a0), then semi-finals, third place and final. Gold Cups 2005 to 2017 have this shape. 2005 to 2013 and 2017 had no third-place match (the rows of `int-continental.tsv`); the template plays one (0x5e0e3b). A change of slot 15 (vtable copy) or a wrap of 0x5e0900 with an object test removes it.
- Slice 27, 16 teams: stage[0..2] groups B to D, stage[3] the Cup_Stage of 8 from the top two of each group, with the brackets of 2016 and 2024 in `editions[]`. The Gold Cups 2019, 2021, 2023 and 2025 have this shape, but `copa16` accepts only the vtable 0x96b400 and the years of `editions[]`. A Gold Cup of 16 needs `copa16` to test the object (record) and its own edition list (brackets) and entrants.
- 2002 and 2003 (4 groups of 3): not possible. The group templates and tables are made for 4 teams (0x5e0946 `mov word [eax], 3` rounds, 0x5e11ee +0x3e = 4, 0x5e11e9 0xec bytes).

Cycle: the ctor rounds to an odd year (0x5e069a..0x5e06f9). The step is +2 (0x5e1590). The Gold Cup is played in odd years from 2003, so the original's cycle fits the Gold Cup in the base game.

Dates: draw 20 March (0x5e1358), groups from 6 July (0x5e09a5), quarter-finals 16 July (0x5e0c8b), final 22 July (0x5e0ec6), all in the year +0x40. The draw day starts the entrants (0x5e21f2).

Slice 27's changes, and whether they test the class or the object:

| Site or wrap | Test |
|---|---|
| 0x5e06fb `rt_intl_first_year(3u, ...)`, 0x5e1590 `rt_intl_next_year(3u, ..., 2u)` (`seasons.py`) | class 3 (Copa America): the edition table of `goldcup`. A second object takes the Copa America's years in a season game |
| wraps of 0x5e12d0 and 0x5e0900, `stage_builder(INTL_COPA, ...)` (`season_intl.c`) | class and year: `season_intl_date_shift(INTL_COPA, +0x40)`. A second object in a year with a Copa America edition (2007, 2011, 2015, 2019, 2021, inference from the table) takes the Copa America's shift. In other years it keeps 6 July |
| sites 0x5e0710, 0x5e0729, 0x5e0f56, 0x5e13b5, 0x5e1437, 0x5e169f, 0x5e22eb, 0x5e172e, 0x5e1735, 0x5e128e, 0x5e1595; wraps 0x5e1cd0, 0x5e2340, 0x5e2500, 0x40f590, 0x5e18a0 | object: `copa16` (vtable 0x96b400 and a Copa America year of 16). In the ctor the vtable is still 0x96b400 (0x5e06af), so the ctor sites see a second object as the Copa America when its year is 2016 or 2024 |

So a second object can take its own cycle and dates only when these sites and wraps test the object. The way: give the Gold Cup def its own class index and table file, and make the sites choose the class from the record (`[[esi + 4]]` against the def's record id). The record is set at 0x5e06b9, before every ctor site. Writing +0x40 after the ctor is not enough: the ctor already built the draw date and the round table for the old year (0x5e073d).

## 6. What a second object of the class does wrong

1. Hosts: Copa America hosts (key `[0x9cf7a8]` at 0x5e1f55).
2. Entrants: South American nations and the guests (0x5e1fd7..0x5e2011), or the fixed 2001 list. With 12 South American and guest nations it plays a second Copa America.
3. Years and dates in a season game: the Copa America's (section 5).
4. History and save: own record, own history and own `*.tmp` file (`2026-10-18-new-comps.md`). No shared write found in the class.
5. News: generic texts with the record's name (0x5e0f77). No fixed Copa America text.
6. The finals tests compare the record id with fixed key ids, and the Copa America id is in them: `national_teams_is_finals_comp` 0x76d940 (0x76d964; 16 callers, among them `Cup::draw_round` 0x51c67b, `FIXTURE_MANAGER::add_fixture` 0x5aaae6, the staff records 0x86af07, 0x86bc59, 0x86c19e, 0x86c357, the match reputation 0x6d2732), `key_nation_is_international_comp` 0x669ae0 (0x669b7e; callers 0x48deb6, 0x669512), and inline lists at 0x6b63b9, 0x76b342, 0x76b47b, 0x76c2a9, 0x76d480 (squad 23/23 for finals, else 26/22), 0x76d6f9, 0x76f206, 0x7752a3, 0x77530c. A second object on another record is not a finals competition in any of them. The build must add its record id (wraps of 0x76d940 and 0x669ae0, sites at the inline lists) or accept the difference.
7. Key nation index: `key_nation_index_of_comp` 0x669470 returns 1 (the international entry) only through 0x669ae0 (0x66951e). Else it matches the record's continent `[record + 0x59]` with the key nation table (0x669597..0x6695a4, inference that +0x59 is the continent). The object lives in the international entry array (Copa America slot 4, 0x830e4a). Inference: put the record id into the 0x669ae0 answer, so that the index and the entry agree.
8. Confederations Cup entrants `FifaConfed::select_teams` 0x58c6b0: the Copa America slot reads the history of record `[0x9cf7a8]` (0x58c9b3, picker 0x58ccb0: winner, else runner-up, else third, 0x58cdc3..0x58ce3d). This stays right with a separate record. The CONCACAF slot is key -1 (0x58c999): Canada before 2002 (0x58cd13), else a random one of United States 0x9cf4f8, Canada, Mexico and Jamaica 0x9cf368 (0x58cd7c). To send the Gold Cup winner, wrap 0x58ccb0 for key -1 with the Gold Cup record's winner (0x4afd80).
9. Slice 27's wraps: `copa16` compares the vtable, not a key id. With a vtable copy a second object takes the original path everywhere except in the ctor (point 3 of section 5). `entrants16` reads `COPA_KEY` for the hosts.

## 7. The Finalissima: a class for one match

Requirement (`int-continental-224`): one match, 1 June 2022 (season 2021/22), Wembley, Euro 2020 winner v 2021 Copa America winner, 90 minutes then penalties, no extra time. Row 268 (2026) is cancelled.

### 7.1 The candidates

Both are Cup classes of one round and two entries. Vtables compared with the Cup base 0x9693c8 (40 slots): both change only slots 0 (destructor), 2 (start_new_season), 15 (round table) and 35 (0x858e70, which calls slot 15 into +0xa3).

| | World_Club_Cup | Eur_Super_Cup |
|---|---|---|
| ctor, size, vtable | 0x92b4b0, 0xb2, 0x971250 | 0x57a100, 0xb2 (0x831584), 0x969f80 |
| picker | 0x92b8b0, direct (0x92b53e, 0x92be50) | 0x57a500, direct (0x57a18e, 0x57a780) |
| what the picker does | Europe test 0x6699d0 (0x92b960), a 2001 branch (0x92b970), a random club with reputation above 5000 (0x92b9a3), then the winners of 0x9cf6e8 and 0x9cf63c | +0x3e = 2, malloc 0xc into +0xa7 (two entries of 6 bytes), entry 0 = winner of record `[0x9cf6e8]` (0x57a5a8), entry 1 = winner of `[0x9cf6ec]` in 1999 or `[0x9cf6f0]` (0x57a5f2..0x57a619), bytes +4, +5, +0xa, +0xb = 0 |
| round table (slot 15) | 0x92b770: draw 10 October (0x92b834), match 1 December, venue flags 4 (0x92b84f), rule flags +0xd = 3 (0x92b86b) | 0x57a3c0: draw 4 July (0x57a484), match 24 August, venue flags 0x8000 (0x57a4a2), rule flags +0xd = 0x83 (0x57a4be) |
| year step | +1 at 0x92be3e | +1 at 0x57a76e |
| slice code on it | ctor wrap 0x92b4b0 in `season_kind_club_world.c`: in a new game it sets the year of every object of the class from the Intercontinental Cup rows | only the per-object start wrap 0x57a6d0 (`season_starts.c`) |

Eur_Super_Cup is the cheaper class. Its picker is the Finalissima picker with two other records. World_Club_Cup has a harder picker and a ctor wrap that would give a second object the Intercontinental Cup's year.

### 7.2 What an Eur_Super_Cup object needs

1. Picker 0x57a500 (direct call): a wrap with a record test (`[[ecx + 4]]` against the Finalissima record id) that writes the national team clubs of the Euro and Copa America winners into the two 6-byte entries of +0xa7: `0x4afd80(record [0x9cf7a4], 0)` and `0x4afd80(record [0x9cf7a8], 0)` (record = `[0xae23d0] + key * 0x6b`, as at 0x57a58e..0x57a5a5). Inference: these history rows hold the national team clubs (the Confederations Cup compares them with `national_team` clubs, 0x58cdf5). A missing winner (an empty history in a season database) makes the original picker fail (0x57a5c0, error line 0xca), so the wrap needs a fallback.
2. Round table, slot 15 (+0x3c, 0x57a3c0): match Date 1 June (day 1, month 5) of +0x40 = 2022, a draw date before it, rule flags +0xd = 0x1 (shoot-out, no extra time; bits 0x1 and 0x2 from sweep-international section 7), venue flags 4. The ctor calls slot 15 through the vtable at 0x57a1d6, after it wrote 0x969f80 (0x57a14d), so a vtable copy does not reach the first round table. Use a wrap of 0x57a3c0 with the record test, or rebuild +0xa3 after the ctor. The tree 0x518790 (0x57a1e1) depends only on the round count (1), so it stays.
3. Venue: 0x8000 gives the stadium of AS Monaco (`[0x9d0264]`, club +0x69, 0x85c346..0x85c37d). Flag 4 (0x85c389) goes to `HostCountry::find_hosts` with the record id `[[fixture + 0x14]]` and the fixture year (0x85c5ff..0x85c61c), and one host gives that nation's stadium `[nation + 0x80]` (0x85c6ee..0x85c730). So a host row with the Finalissima record id, year 2022 and England plays it at England's national stadium. Inference: Wembley only when the data's England record names it. The England cup final branch of flag 4 (0x85c38e..0x85c3fa) skips national teams (0x53b900 at 0x85c3b7).
4. Year and edition: the ctor stores the year argument at +0x40 (0x57a137); `make` passes 2022. After the match the object must not come back (2026 cancelled): `season_change_skip` in a `before` hook (shared rule of `season_comps.h`); else 0x57a76e makes a 2023 edition.
5. Entry: the international entry (array of `create_other_int_comps`, update day from 0x830b5b..0x830b92). Inference: an object made in season 2021/22 plays 1 June 2022 before the next update day.
6. Slots a vtable copy changes: slot 15 for the start_new_season path (0x57a7bd calls slot 35, which calls slot 15), and nothing else. The picker and the ctor's first round table need wraps with a record test.

### 7.3 Can a Cup class hold national teams?

Yes. The CONCACAF and African World Cup sections are Cup classes (Cup::Cup 0x518640 at 0x914a14; vtable 0x970c24 slot 1 is `Cup::process_fixture` 0x51a150) and play national teams.

- Club +0x1db: no read in the Cup code 0x518640..0x523200 (search of the disassembly).
- Nation of the club (+0x53): read in the same-nation draw (0x51d9f0, 0x51dc48) and the news (0x5197f7). For a national team club it is its own nation (slice 27 relies on it: `team_nation`).
- Stadium: flags 0 and 4 handle national teams (0x85c592..0x85c5a2, then the nation's stadium at 0x85c70c..0x85c730).
- The finals tests of section 6, point 6, do not list a new record: the Finalissima is not a finals match for squads and fixture clashes. Unknown: whether `NationalTeams::daily_update` 0x76b100 picks a squad for a fixture of a non-finals record. A test run settles it.

## 8. Unknowns

- The meaning of +0x50 (Eur_Super_Cup 1 at 0x57a175, World_Club_Cup 0xff at 0x92b525) and of round +0x17 (8 and 6).
- Whether the history of a season database holds the Euro 2020 and 2021 Copa America winners at a 2021/22 start.
- The record for each def (not this part's question). A record with no object of the original is needed for each (new-comps doc).
