# Competitions of national teams in the original (0.6.0 slice 28, part B)

Date 2026-10-18. Research only. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Method: capstone disassembly of the clean exe (the `cm0102` venv), a Unicorn run of `Friendly::build_international_date_table` 0x5ca4f0 with stub allocators for its table, and a dump of `club_comp.dat`, `nation_comp.dat` and the two history files of `clean-3968/Data` and of `test-fixtures/databases/October 2018`. "Inference" marks a statement that the code does not prove. Brief: `slice28-research-brief.md` (scratchpad of the lead). Related: `2026-10-18-new-comps.md`, `2026-10-18-new-comps-load.md`, `2026-10-16-sweep-international.md`.

## 1. What the build needs first

1. **The code does not ask "is this a competition of national teams?".** It asks two things. The clubs: `database_is_national_team` 0x53b900 (club id at least clubs minus 2 x nations) and `club_is_national_team` 0x5409b0 (at least clubs minus nations: the under-21 rows). The fixture type: a byte that `comp_active_key_nation_of_fixture` 0x48dd70 gives from the competition record when the fixture goes into the fixture store (`FIXTURE_MANAGER::insert_fixture` 0x5ab230, call at 0x5ab2e3, list index at 0x5ab2f0..0x5ab301). Type 0 is the World Cup and its six sections, type 1 the list of `0x669ae0` (section 2). Every other record gets the index of a league table entry, 2 or more.
2. **Four features of the national teams read only types 0 and 1**: the call-up two weeks before a fixture (`NationalTeams::daily_update` 0x76b100, types at 0x76b207 and 0x76b221), the release of a player on the day (`NationalTeams::get_international_duty_club` 0x7757d0 through `FIXTURE_MANAGER::find_fixtures` 0x5acbe0, types {0, 1} at 0x5acc43..0x5acc4d), the guard that keeps friendlies off the dates of other matches (`Friendly::arrange_international_friendlies` 0x5cbdf0, types 0..1 at 0x5cbfbf..0x5cbfcb) and the FIFA ranking weight (0x58cf70, section 4).
3. **A record that no key id list holds gets type 2 or more.** Record 344 ("CONCACAF Cup Winners Cup", continent North America) gets 4, the index of the CONCACAF club entry (0x669470, section 2). Records 444 and 445 (Asian Cup Qualifying, Asian Games) get 6, the Asian club entry. So a new competition of national teams in a spare record, club comp or nation comp, has: no call-ups except at the add of a fixture that is already less than two weeks away (0x5aab15), no release of players and no friendly guard. In a record that is not a tournament (344, 444) it also gets an error box "fifa_rankings.cpp" line 0xde for each match (0x58d157..0x58d1a8). The record's place in the array does not matter (section 6).
4. **Recommended fix for the build: wrap `0x669ae0`** (cdecl (ClubComp* record), returns int; its only callers are 0x48deb6 in 0x48dd70 and 0x669512 in 0x669470) to return 1 for the record ids of the new competitions. Then their fixtures are type 1, like the Euro qualifiers and the international friendlies: call-ups, release, friendly guard and ranking weight 1.1 all work with no other change. The 10 other callers of 0x669470 (for example the season text 0x6697f4, Cup 0x519940, League 0x684896) then see entry 1 too. Inference: right, because entry 1 holds the Copa America and the Euro. Not read in detail.
5. **Do not make the Nations League a tournament** (section 3): leave `is_tournament` 0x76d940 and its eight inline copies alone. A tournament holds its squad from 10 days before its first fixture until the team goes out (0x76c304..0x76c3c9, ends at 0x86af1a, 0x86bc70, 0x86c1af, 0x86c368), so a league stage from September to November would hold the players all autumn. It also takes a squad of 23 (0x76d4b0). The Gold Cup and the Finalissima are finals: the same tests apply to them (section 3, last point).
6. **Asian Cup Qualifying (record 444, key id 0x9cf788) is free**: no code reads its key id except the load (0x606e39) and the name matcher (0x610267), and no history row holds it in either database. **Asian Games (record 445, key id 0x9cf78c) is not free**: nine sites read it as a finals tournament, and both databases hold its history (13 and 20 rows).
7. **Friendly dates are a fixed table**, not the fixtures of the competitions (section 7). Matches of a new competition of type 1 keep friendlies away within 4 days. Of any other type they do not.
8. **The latest winner** of a competition is `comp_util_get_latest_placed_club` 0x4afd80 (cdecl (record pointer, place 0 to 2), returns Club* or 0). For the Finalissima: the record pointers `[0xae23d0] + [0x9cf7a4] * 0x6b` (Euro) and `+ [0x9cf7a8] * 0x6b` (Copa America), place 0, as the Confederations Cup does (0x58cdc3..0x58cdda).

## 2. How a fixture becomes international (question 1)

### 2.1 The fixture type

`comp_active_key_nation_of_fixture` 0x48dd70, cdecl (ClubComp* record, Fixture* fixture or 0), returns a byte:

1. Friendly or reserve competition (`Friendly::is_friendly_competition` 0x5c6710 true, or record id = key id 0x9cf684, 0x48dd81..0x48dd92), with a fixture: when the home club (+0x1c) or the away club (+0x20) is a national team (0x53b900 at 0x48ddc2, 0x48ddd6), it returns 1 (0x48dec2). So an international friendly is type 1. Else the nation of both clubs, and its entry +0x14 (0x48ddec..0x48de54).
2. Record +0x5d (nation pointer) set: the entry of that nation, its +0x14 byte, else 3 (0x48de5c..0x48de9a).
3. No nation: `0x669a10` true gives 0 (0x48dea3..0x48deb1), `0x669ae0` true gives 1 (0x48deb6..0x48dec4), else `0x669470` (0x48dec9).

The lists (record id compared with key id globals):

| Function | Returns 1 for | Addresses |
|---|---|---|
| 0x669a10 | World Cup 0x9cf79c and the sections 0x9cf77c, 0x9cf774, 0x9cf770, 0x9cf780, 0x9cf76c, 0x9cf778 | 0x669a8a..0x669acf |
| 0x669ae0 | Olympics 0x9cf7a0, Euro 0x9cf7a4, Euro qualifying 0x9cf784, African Cup of Nations 0x9cf7ac, Copa America 0x9cf7a8, 'B' International 0x9cf794, Under 21 International 0x9cf798, Oceania Nations Cup 0x9cf8a0, Confederations Cup 0x9cf964, Asian Cup 0x9cf790 | 0x669b5a..0x669bb7 |

The Asian Games (0x9cf78c) and Asian Cup Qualifying (0x9cf788) are in neither list.

`0x669470` (cdecl (record), returns the entry index): nation +0x5d set gives its raw entry index or 4 (0x66947e..0x6694a7); the World Cup list gives 0 (0x669528..0x669564); the 0x669ae0 list gives 1 (0x669511..0x66951e); the Intercontinental Cup 0x9cf6e4 or the Club World Championship 0x9cf7bc give 2 (0x66956e..0x6695b6); else the first entry with no nation (+0) whose continent (+4) equals record +0x59 (0x669584..0x6695a4), else 3. Entries 0, 1 and 2 have +0 and +4 set to 0 (0x667219..0x66721f, 0x6672ad..0x6672b3, 0x66734a..0x667350), so a record with a continent never matches them. Entry 3 has Europe (creator 0x8312e0), 4 North America (0x831650), 5 South America (0x8318f0), 6 Asia (0x831ba0), 7 Oceania (0x831eb0), 8 the next continent (0x832100), from the build of the table (0x6673e4..0x667887; continent keys 0x9cfa10, 0x9cfa14, 0x9cfa1c, 0x9cfa0c, 0x9cfa18).

The type is the list index of the fixture store: `[store + (day * 3 + slice) * 4] + type * 4` (0x5ab2f0..0x5ab301; the same in `get_fixture_list` 0x5abfbf..0x5abfda). How many type lists a day holds: unknown (the store constructor 0x5a9cf0 was not read).

### 2.2 The call-up (0x76bb9e)

`NationalTeams::daily_update` 0x76b100 (thiscall (this, Date* today), ret 4; called each day):

- It takes today + 2 weeks (`Date::add_weeks` 0x54c770 with 2 at 0x76b126..0x76b12b) and, for slices 0 to 2 (0x76b55b..0x76b567), the fixture lists of type 0 (0x76b207) and type 1 (0x76b221) of that day.
- For each fixture it calls `select_squads_for_fixture` 0x76bab0 (0x76b30e, 0x76b447). For a fixture whose record is not a finals tournament (inline list 0x76b31a..0x76b384, section 3), whose home club is not an under-21 side (0x5409b0 at 0x76b38e) and whose +0x40 byte is set, it counts the called-up players per club with `record_club_call_ups` 0x7771c0 (0x76b3a7, 0x76b3b4; B or under-21 squads by fixture +0x4d bits 0x100 and 0x200, 0x76b3b9..0x76b409). The clash code (`reschedule_clashing_club_fixtures` 0x7777e0, 0x76b579) reads those counts.

`NationalTeams::select_squads_for_fixture` 0x76bab0 (thiscall (this, Fixture*), ret 4) reads only the fixture and the clubs:

- It returns when fixture +0x40 is 0 and +0x4e bit 1 is set (0x76bad1..0x76badc), or when the home club is an under-21 side and +0x4e bit 2 is set (0x76bae2..0x76baf6), or when the fixture date minus 2 weeks (`Date::sub_weeks` 0x54cab0, 0x76bb35) is after today (0x76bb3a..0x76bb5a).
- The squad record of the nation of each club (`[this + 4] + nation id * 0x6e`, 0x76bb65..0x76bb76; slots of 0x14 bytes at +0x32 senior, +0x46 B, +0x5a under-21).
- **0x76bb9e**: `days_since` 0x54cdb0 from the end date of the senior slot (+0x3a) to today `cmp eax, 0xe`. When that is at most 14 days and the slot is picked (+0x43), the squad stays and its window grows to this fixture (0x76bbaa..0x76bc09). Else `select_squad` 0x76c120 picks a new one (0x76bc10..0x76bc23). The same for B (0x76bc6c, +0x57) and under-21 (0x76bd26, +0x6b), and for the away side (0x76bdd4 on).
- The only competition test here: the under-21 squad of an Olympics fixture (record id = 0x9cf7a0) is picked even when the nation's under-21 flag `[this + 0x36][nation]` is set (0x76bd3a..0x76bd54).

`FIXTURE_MANAGER::add_fixture` 0x5aa680 also calls `select_squads_for_fixture` (0x5aab15) for a fixture whose two clubs are national teams (0x53b900 at 0x5aaaa0, 0x5aaab0) and whose +0x43 is 0xff. Before that, for a record that is not 'B' or Under 21 International (0x5aaac5..0x5aaad7), with +0x4e & 3 clear, and that is not a tournament (0x76d940 at 0x5aaae6), or a tournament that is also a friendly (0x5aaaf9), it adds the linked B or under-21 fixture (`ReserveComp::add_b_or_under21_fixture` 0x7e6720, 0x5aab09). So the direct call works for any record, but only selects when the fixture is already less than two weeks away.

### 2.3 Release of players

`NationalTeams::get_international_duty_club` 0x7757d0 asks the fixture store for the national side's fixtures from the day before to the day after (0x775842, 0x775855; `find_fixtures` 0x5acbe0 at 0x7758a8). For a national team club `find_fixtures` reads only types 0 and 1 (0x5acc37..0x5acc4d: list {0, 1}, count 2). So a player is away on the day only for fixtures of type 0 or 1.

### 2.4 The match

The match code calls `NationalTeams::record_international_result` 0x76b600 (call at 0x6b60f9) when fixture +0x47 is not 0xfd (0x6b60eb). It returns unless both clubs are national teams (0x53b900 at 0x76b613, 0x76b627). Then: 'B' International (0x9cf794, 0x76b63a) adds B caps, Under 21 International (0x9cf798, 0x76b6fe) adds under-21 caps, the Olympics (0x9cf7a0, 0x76b7e6) adds no ranking, every other record calls `FIFA_RANKINGS::record_match_result` 0x58cf70 (0x76b7f6) and the caps of the senior side.

## 3. Tournament, squad limits (question 2 and 7)

### 3.1 `national_teams::is_tournament` 0x76d940

stdcall (ClubComp* record), ret 4. The callers load ecx with [0xb74318], which it does not read. Returns 1 when the record id (record +0) equals one of nine key ids: World Cup 0x9cf79c, Olympics 0x9cf7a0, Euro 0x9cf7a4, Copa America 0x9cf7a8, African Cup of Nations 0x9cf7ac, Asian Cup 0x9cf790, Oceania Nations Cup 0x9cf8a0, Asian Games 0x9cf78c, Confederations Cup 0x9cf964 (0x76d948..0x76d994). A null record gives 0.

Direct calls (15): 0x51c67b (`Cup::draw_round`), 0x58d0ab and 0x58d133 (FIFA ranking), 0x5aaae6 (add_fixture), 0x5ae086 and 0x5ae198 (`collect_day_clashes`), 0x5d0374 (daily staff update), 0x697dbc (board confidence), 0x6b6cd9 (post-match changes), 0x6d2732 and 0x6d2936 (player reputation), 0x74b48a (media), 0x86af07, 0x86bc59, 0x86c19e, 0x86c357 (`Staff_Records`: the end of a team's duty).

The same nine ids are also compared inline, so a wrap of 0x76d940 does not reach them: `daily_update` 0x76b31a..0x76b384 and 0x76b453..0x76b4bd, `select_squad` 0x76c28d..0x76c2d7, `get_squad_limits` 0x76d464..0x76d4ae, `collect_fixture_opponents` 0x76d6e0..0x76d721, `post_squad_request_news` 0x76f1ff..0x76f22e, `set_current_competition` 0x775287..0x7752cb and 0x7752fc on.

What a tournament changes:

1. `select_squad` 0x76c120 (thiscall (this, Nation*, slot, Date*, Fixture*, forced), ret 0x14) sets its finals flag (0x76c2d9) and adds a duty entry to the list at this +0 (0x76c304..0x76c3c9): the fixture date minus 10 days (0x76c31b, `sub_days` 0x54c8e0) and the word 0x3e8 (0x76c3bf). The entry ends when `Staff_Records` records a win, third place, knock-out or failure to qualify for a national team in a tournament (`end_duty_for_competition` 0x774f30 at 0x86af1a; `end_duty_for_nation_in_competition` 0x774f90 at 0x86bc70, 0x86bc83, 0x86c1af, 0x86c368). Inference: the players stay with the nation from 10 days before the first fixture until the team goes out.
2. No call-up counts per club (`daily_update` skips 0x7771c0 for tournaments, 0x76b31a..0x76b384).
3. No linked B or under-21 fixture (0x5aaae6).
4. Squad sizes 23 (below).

### 3.2 `get_squad_limits` 0x76d430

stdcall (Fixture*, slot, byte* max, byte* second), ret 0x10. One caller: `select_squad` at 0x76c566. With a fixture and its record (+0x14):

| Case | max, second | Address |
|---|---|---|
| Record in the nine tournament ids (inline, 0x76d464..0x76d4ae) | 0x17, 0x17 (23, 23) | 0x76d4b0, 0x76d4b3 |
| Else slot 1 (B squad) | 0x18, 0x16 (24, 22) | 0x76d4cf, 0x76d4d2 |
| Else slot 0 or 2 (senior, under-21) | 0x1a, 0x16 (26, 22) | 0x76d4df, 0x76d4e2 |

With no fixture or no record it goes to 0x76d4ef (not read). So a World Cup squad has 23 and a qualifier or friendly squad 26: the test is the record id.

### 3.3 Other key id tests of the squad pick

- `select_squad`: Australia gets other limits by record: friendly (0x5c6710 at 0x76c1ab) or Oceania Nations Cup (0x76c1b7) 0x8ca and 0x2134, Confederations Cup 0xabe and 0x222e (0x76c1c3..0x76c1d3), World Cup Oceania section 0x8ca and 0x2328, 0x1f40 or 0x2711 by fixture +0x42 (0x76c1dd..0x76c20a), World Cup 0xabe and 0x2710 (0x76c214..0x76c224). Meaning of the numbers: unknown (inference: thresholds of the pick).
- `select_squad` 0x76cc1f..0x76cc3b: for a record that is not the friendly (0x9cf668), 'B' or Under 21 International, it compares the continent of the player's club with the nation's (0x76cc3d..0x76cc59). Inference: the release rule for players at clubs on another continent. A new record goes this way, as a qualifier does.
- The under-21 Olympics test of 0x76bab0 (2.2).

A new competition with a record of its own gets the qualifier path from all of these: 26 and 22, no duty hold, call-up counts, the linked B or under-21 fixture.

### 3.4 Recommendation: the Nations League is not a tournament

Reason in the code: a tournament holds the squad from 10 days before its first fixture (0x76c31b) until `Staff_Records` ends the duty when the team goes out (0x86af1a and the 0x774f90 calls). A League-class group stage that plays in September, October and November would keep the players away from their clubs for the whole autumn. The qualifier path picks a squad for each window: two weeks ahead (0x76b12b), kept for fixtures within 14 days (0x76bb9e), with release for the match days only (0x7757d0) and club fixtures moved when more than two players are away (0x7777e0). That is the shape of a Nations League window.

The finals of four, the Gold Cup and the Finalissima: they would need a wrap of 0x76d940 and of all eight inline copies to be tournaments. Inference: the Gold Cup on the class `copa_america` calls the same `Staff_Records` functions as the Copa America, so its duty would end. Unknown for a Nations League finals stage. The recommendation is that no new competition is a tournament in version 1; the squad is then 26, which the game allows for any qualifier.

## 4. FIFA ranking weights 0x58cf70 (question 2)

`FIFA_RANKINGS::record_match_result` 0x58cf70, thiscall (FIFA_RANKINGS*, Fixture*), ret 4. One caller: 0x76b7f6. It needs both clubs with a nation and a continent (0x58cfa5..0x58cfd7) and a record (+0x14, 0x58cfdd). The same continent gives the factor of continent +0xbe (a double, 0x58cfe8..0x58cffc), else 1.0 (0x58cf8b).

The competition weight (doubles: 0x968940 = 1.5, 0x968928 = 1.25, 0x9689c0 = 1.1, 0x967890 = 1.0):

| Record | Weight | Address |
|---|---|---|
| World Cup 0x9cf79c | 1.5 | 0x58d06c..0x58d07b |
| Type 0 (0x48dd70 with no fixture gives 0: the sections) | 1.25 | 0x58d083..0x58d092 |
| Type not 0, record +0x59 (continent) set and a tournament: the Euro | 1.25 | 0x58d09a..0x58d0cf |
| The same, other tournaments (for example the Asian Cup, the Asian Games) | 1.1 | 0x58d0d1..0x58d0d3 |
| Else type 1 (Euro qualifying, Olympics, Confederations Cup, B, U21) | 1.1 | 0x58d110..0x58d120 |
| Else +0x59 is 0 and a tournament | 1.1 | 0x58d122..0x58d13a |
| Else a friendly (0x5c6710) | 1.0 | 0x58d13c..0x58d14f |
| Anything else | error box "fifa_rankings.cpp" line 0xde, no points | 0x58d157..0x58d1c4 |

Note: an international friendly gives type 1 only with a fixture (2.1, point 1). Here 0x48dd70 gets no fixture (0x58d083, 0x58d113), so a friendly falls to the 1.0 row. A new competition of type 1 that is not a tournament gets 1.1, the weight of Euro qualifying. A new competition with no wrap in a record that is not a tournament (344, 444) gets the error box after every match. In record 445 (Asian Games, a tournament with a continent) it gets 1.1.

## 5. The competition record (question 3)

### 5.1 Layout

0x6b bytes, packed. The field reader `0x54ffd0` (sizes pushed at 0x55001f..0x55017d) and the reset of the 0x7f spare rows (0x52a068..0x52a1a6) give:

| Offset | Size | Field | Evidence |
|---|---|---|---|
| +0 | 4 | record id, the index in the array and in [0xadadfc] | 0x8386f0 (load doc 4), 0x52a081 |
| +4 | 51 | long name | 0x55001f, 0x5356d1 |
| +0x37 | 1 | byte after the long name; the language load writes it and copies it to +0x52 (0x535613..0x535617) | inference: a grammar byte of the name |
| +0x38 | 26 | short name | 0x550048, 0x53570a |
| +0x52 | 1 | the same kind of byte for the short name | 0x535724 |
| +0x53 | 4 | abbreviation | 0x55006a, 0x535774 |
| +0x57, +0x58 | 1 each | unknown; read (0x550093, 0x5500bc) and cleared in spare rows (0x52a0f4, 0x52a111) | data: changing values in both kinds of record, no meaning found |
| +0x59 | 4 | continent (index in the file, pointer in the game; -1 none) | fix-up 0x52db3e..0x52db4d; read as entry +4 at 0x66959e |
| +0x5d | 4 | nation (the same) | fix-up 0x52db25..0x52db34; read at 0x66947e, 0x48de5c |
| +0x61, +0x65 | 4 each | colours (pointers into colour.dat [0xae23ec], 0x3a bytes a row) | 0x532d89..0x532dd1 |
| +0x69 | 2 | a value read as one byte (0x5501a3..0x5501a9) | data: 20 for the World Cup, 1 for B, 2 for a fourth division; inference: reputation |

### 5.2 One array

[0xae23d0] holds the club comps, then the nation comps, then 0x7f spare rows; the count [0xae2378] holds all three. Evidence: the block load sets the nation comp pointer [0xae23d8] to the array plus (count - nation comps - 0x7f) x 0x6b (0x529f88..0x529fba) and adds n + 0x7f to the count (0x529a88..0x529a97; inference: the nation comp case of that load); the spare rows are the last 0x7f (0x52a068..0x52a1a6); the friendly object takes `[0xae2378] - 0x7f` as its first slot (0x5c2105..0x5c210d) and `is_friendly_competition` tests id >= count - 0x7f (0x5c6723..0x5c6733). Data: both databases hold 433 club comps and 19 nation comps with ids 433 to 451, so the spare rows are 452 to 578. The slice 27 test log saw the Euro fixture with the record word 0x1c1 (449).

### 5.3 Club comp records against nation comp records

The fields are the same in both files. Differences in the data:

- Ids 0 to 432 against 433 to 451.
- Bytes +0x37 and +0x52 are 0xff in every club comp and other values in the nation comps (dump of both files, both databases).
- Nation comp records hold no nation (+0x5d -2); the continental ones hold a continent (Euro 2, Euro qualifying 2, Asian Cup Qualifying 1, Asian Games 1, Asian Cup 1, Oceania Nations Cup 4, African Cup of Nations 0); the world ones hold none (-2).
- History: the national team history rows are in nation_comp_history.dat, the club rows in club_comp_history.dat. The game puts both in one array (count [0xae237c], the nation part at its end, 0x52a005..0x52a01b), so 0x4afd80 searches both.

No code read here tests the index range of a record (nation comp or club comp). The national team code tests key ids (sections 2 to 4), the record's nation +0x5d and continent +0x59 (0x48dd70, 0x669470), and the clubs.

### 5.4 Can record 344 carry a competition of national teams?

Yes, with the wrap of 0x669ae0 (section 1, point 4). Without it the fixtures are type 4 (continent North America, entry 4), with the losses of section 1 point 3. Other points:

- Key id 0x9cf730 has no reader besides the load (0x6022e1 set to -1, 0x606c6b missing-name check, 0x60ffcb the name match).
- History: the 3.9.68 data holds 5 rows of club winners for record 344 (1991, 1993, 1994 and two more); October 2018 holds none. A new competition in record 344 shows those club rows in its history in a 3.9.68-based game. Inference: the history screens show them under the new name. Records 60, 61 and 373 hold club rows too (8, 10 and 11 in 3.9.68).
- The record's colours (+0x61, +0x65) are -2 (none) for 344; reputation 10 (3.9.68) or 8 (October 2018).

### 5.5 Readers of the four key ids

| Key id | Record | Readers (other than the load: -1 at 0x602365..0x60237d, missing-name checks 0x606e39..0x606e8d, name match in 0x60efd0) |
|---|---|---|
| 0x9cf788 Asian Cup Qualifying | 444 | none |
| 0x9cf78c Asian Games | 445 | the tournament list: 0x76b372, 0x76b4ab (daily_update), 0x76c2c9 (select_squad), 0x76d4a0 (get_squad_limits), 0x76d719 (collect_fixture_opponents), 0x76d984 (is_tournament), 0x76f226 (post_squad_request_news), 0x7752c3, 0x77532c (set_current_competition) |
| 0x9cf794 'B' International | 433 | 0x5aaac5 (add_fixture), 0x669b86 (entry 1 list), 0x6b6207 (`match_day_is_fixture_of_human_interest`), 0x76b63a (B caps), 0x76cc2d (select_squad), 0x770f9c, 0x774c36 (international report text), 0x7c0eb9 (`PlayerStats::record_match`), 0x7e6214, 0x7e623d (`ReserveComp::create_sub_comps` makes its object), 0x7e688c (`add_b_or_under21_fixture`), 0x7e6f2a (`comp_is_reserve_or_minor_comp`) |
| 0x9cf798 Under 21 International | 434 | 0x5aaad1, 0x669b8e, 0x6b620f, 0x76b6fe (under-21 caps), 0x76cc35, 0x7c0ead, 0x7e627a, 0x7e62a3 (its object), 0x7e6988, 0x7e6f3c |

Free for a new competition:

- **Asian Cup Qualifying (444): free.** No reader, no object (no class, sweep section 3), no history row in either database. It is a nation comp record with continent Asia, so with no wrap its fixtures are type 6. The name match keeps the key id after a rename because `season_change_rename` keeps the data name (season change doc, "Names"); with no reader that does not matter.
- **Asian Games (445): not free.** It has no object, but the nine readers above make any competition in it a tournament (23-man squad, duty hold), it is not in the 0x669ae0 list (type 6), and both databases hold its history (13 rows in 3.9.68, 20 in October 2018).
- 'B' and Under 21 International: not free (objects of `ReserveComp`, 0x7e61c0).

### 5.6 Nation comp records in [0xadadfc]

Yes. `build_comp_object_table` 0x8386f0 makes [0xae2378] slots (all three parts) and writes `table[record +0] = object` for every object of every entry array (load doc section 4). The objects of entries 0 and 1 are on the nation comp records 433 to 451 (their creators pass `0xae23d0 + key id * 0x6b`, sweep section 2), so they are in slots 433 to 451. Then `Friendly::register_comp_objects` 0x5ca030 writes the friendly object at key id 0x9cf668 (0x5ca045) and its 0x7f sub-competitions from slot friendly +0xaf = count - 0x7f (0x5ca050..0x5ca07a). The spare rows begin after the last nation comp, so a new object on record 444 or 344 goes in its own slot below them.

## 6. League table entries 0 and 1 (question 5)

| Entry | Setup (creator at entry +8) | Objects | Update day +0x15 | +0x1b | Step +0x1d |
|---|---|---|---|---|---|
| 0 | `create_world_cup_comps` 0x830620 (set at 0x667225) | count 7 (0x8306f5), array calloc(0x1c, 1) (0x8306e5..0x8306fc): World Cup and the six sections | Date(25, 6, 2001) at 0x830a63..0x830a7a: day 205, 25 July (0x830a8b) | 1996 (0x830a8f) | 2 (0x830a95) |
| 1 | `create_other_int_comps` 0x830ac0 (set at 0x6672b9) | count 8 (0x830b96), array calloc(0x20, 1) (0x830b88..0x830b9d), 7 with no Confederations Cup key id (0x830f89) | Date(15, 7, 2001) at 0x830f8d..0x830f9a: day 226, 15 August (0x830fac) | 2001 (0x830fb0) | 2 (0x830fb6) |

Both set +0x26 to 0 (0x830a99, 0x830fba), +0x32 = 1, +0x34 = 6, +0x35 = game year, +0x37 = 5, +0x41 = 1, +0x43 = 6, +0x44 = game year, +0x46 = 5 (0x8306bb..0x8306ef, 0x830b5b..0x830b92). The update days agree with the log in `2026-10-17-season-change.md` (entry 0 day 205, entry 1 day 226, step 2).

How their objects play: the daily update `game_update_comps_for_date` 0x5ce060 (called at 0x5cd9e9) calls vtable +0x14 `update_for_date` of every object in [0xadadfc] (0x5ce071..0x5ce088), not of the entry arrays. The season pass 0x5d5ae0 calls `start_new_season` (vtable +0x8) through the entry arrays on the update day (season change doc). So an object added to entry 0 or 1 after 0x8386f0 ran also needs its slot in [0xadadfc] (load doc point 5), which `season_comps` writes.

## 7. International friendlies (question 4)

### 7.1 The date table

`Friendly::build_international_date_table` 0x5ca4f0 (thiscall (Friendly*), ret 0; one caller 0x5c2180 in the friendly set-up) allocates 200 rows (0xfa0 bytes, 0x5ca4fa; +0x34f0 = 0xc8 at 0x5ca4ff), writes 110 fixed rows inline and with `0x5cb9b0` (thiscall row, (b0, day, month, year offset, continent, key id), ret 0x18), checks the count 0x6e (0x5cb8f1; else the box "friendly" line 0x1659), shrinks the array to 0x898 bytes (0x5cb96e..0x5cb984) and calls `0x5cc510` (0x5cb98a).

Row (0x14 bytes): +0 weekday code b0, +1 day, +2 month from 0, +3 the Date (8 bytes, set by 0x5cc510), +0xb year offset 0 to 3, +0xc continent id (the value of a key continent: Africa 0x9cfa08, Asia 0x9cfa0c, Europe 0x9cfa10, North America 0x9cfa14, Oceania 0x9cfa18, South America 0x9cfa1c; -1 never seen), +0x10 a qualifying key id or -1.

`0x5cc510` sets each Date with `Date::set` 0x54a130 (day, month, year, weekday b0: it moves the date to the nearest such weekday) for the year `Y + offset`, where Y is the first year from the game year with (Y - 2) % 4 = 0 (a World Cup year, 0x5cc511..0x5cc558), minus 4 when that lies more than 3 years after the game year (0x5cc57d..0x5cc594). It sorts the rows by date (qsort 0x945333 with 0x5cc610, 0x5cc5dd). `update_international_friendlies` 0x5cbb00 does the same again on 1 January (0x5cbb42 on, 0x5cbbdb..0x5cbc4a).

The 110 rows (Unicorn run; day/month, offset in brackets; key id of the row after the colon):

- Africa (24 rows): 10/10 (0), 18/11 (0), 15/12 (0), 12/1, 24/2, 24/3, 28/4, 9/6, 24/7, 18/8, 9/10 (1); World Cup African section: 1/6, 15/6, 1/10, 10/11 (2), 12/1, 24/2, 5/4, 27/4, 7/6, 24/7, 17/8, 28/9, 20/10 (3).
- South America (21): 16/8, 6/9, 7/10, 15/11 (0), 29/3, 26/4, 16/8, 6/9, 7/10 (1); World Cup South American section: 26/4, 26/7, 6/9, 7/10, 15/11 (2), 26/4, 4/6, 26/7, 16/8, 6/9, 7/10, 15/11 (3).
- Oceania (17): 10/10, 18/11, 15/12 (0), 10/1, 24/3, 25/7, 7/9, 9/10, 17/11 (1); World Cup Oceania section: 10/12 (2), 1/2, 10/4, 15/4, 10/7, 17/9, 29/10, 29/11 (3).
- Europe (23): 24/3, 25/4, 15/11 (0), Euro qualifying 2/9, 7/10 (0); 15/11 (1), Euro qualifying 24/3, 25/4, 6/6, 2/9, 6/9 (1); 24/3, 25/4, 15/11 (2), Euro qualifying 7/10 (2); 15/11 (3); World Cup European section 7/10 (2), 24/3, 25/4, 6/6, 2/9, 6/9, 7/10 (3).
- North America (21): 25/8, 18/11, 15/12 (0), 10/1, 23/2, 24/3, 28/4, 7/9, 9/10 (1); World Cup CONCACAF section: 25/7, 25/8, 17/11 (2), 26/1, 23/2, 2/3, 15/3, 20/4, 29/6, 9/9, 1/10, 16/11 (3).
- Asia (4): 9/6, 7/9 (1); World Cup Asian section 4/6, 13/9 (3).

### 7.2 How friendlies are arranged

`Friendly::update_international_friendlies` 0x5cbb00 runs from `Friendly::update_for_date` 0x5c2460 (call at 0x5c257c), once a day (+0x34f4 holds the last day, 0x5cbc97, 0x5cbda9). It takes today + 12 weeks (0x5cbb2d..0x5cbb3d), does nothing while that is before the date at [0xdd6ec8] (0x5cbc52..0x5cbc88; inference: the start date of the game), finds the rows of exactly that date by bsearch (0x946464 at 0x5cbcd3) and their neighbours of the same date (0x5cbce5..0x5cbd94), and calls `arrange_international_friendlies` 0x5cbdf0 with the first row and the count (0x5cbd9e).

`arrange_international_friendlies` 0x5cbdf0 (thiscall (this, rows, count), ret 8):

1. **The guard reads the fixtures.** From the date minus 4 days (0x5cbec2) to the date plus 4 days (0x5cbedf), day by day (0x5cbfee), for slices 0 to 2 and types 0 and 1 (0x5cbf1f..0x5cbfdd), it gets each fixture list (`fix_man_get_fixture_list_for_date` 0x5ae6a0 at 0x5cbf3e) and marks the nation of both clubs as busy (0x5cbf7a, 0x5cbf85).
2. For each nation of its sorted list (+0x34fa, count +0x34fe) that is not busy (0x5cc02f), whose continent id matches one of the rows (0x5cc05e..0x5cc07c), and that is not African when the month is February (row +2 = 1, 0x5cc082..0x5cc0a0), it looks for a free partner further down the list (0x5cc0bc..0x5cc1ef): not busy, a matching continent, not African in February, a reputation band of at least 5 on one side (+0x88, 0x5cc15b..0x5cc170), not a banned pair (0x5cc3e0 at 0x5cc17a), and a random score, rand(500) plus rand(1000) for the same continent, above 400 (0x5cc0d5, 0x5cc18d..0x5cc1a4), and not met before (the lists at +0x34f6, 0x5cc1ae..0x5cc1ed).
3. It makes the friendly between the senior national teams (`database_get_national_team` 0x53b3d0 at 0x5cc213, 0x5cc228; `add_friendly_match` 0x5c6f00 at 0x5cc233; `process_friendly_reply` 0x5c5f40 at 0x5cc241) on the row's date and marks both nations busy (0x5cc24f, 0x5cc255).

Row +0x10 (the qualifying key id) is not read by 0x5cbdf0, 0x5cbb00 or 0x5cc510. These and the destructor (0x5c239c) are the only readers of the table pointer +0x34ec. So the friendly dates are fixed dates of a table by continent, and the clash test is the fixtures of types 0 and 1 within 4 days, read 12 weeks ahead.

### 7.3 Clashes with a new competition

- Fixtures of a new competition of type 1 (with the 0x669ae0 wrap): a nation that plays one within 4 days of a table date gets no friendly on it, provided the fixture is in the store 12 weeks before (inference: League-class fixtures are made at the draw, which for a Nations League lies months before the first window).
- Fixtures of type 2 or more: the guard does not see them. A friendly can fall on the same date or a day beside it. What the store then does (the clash code `resolve_same_day_clash` 0x5ae380, `collect_day_clashes` 0x5adba0) was not read: unknown.
- Fixtures made less than 12 weeks before their date: a friendly that the arranger made earlier stays. Inference: the store's clash code moves one of them, as for other clubs. Not read.
- The table dates are the FIFA windows of 2001 (for example Europe 24 March, 25 April, 6 June, 2 and 6 September, 7 October, 15 November). A Nations League on the same window dates as the Euro qualifying rows plays where the table puts friendlies, so the guard is what keeps them apart.

## 8. Competition history and the latest winner (question 6)

Row of the history array (0x1a bytes, [0xae23d4], count [0xae237c]; club and nation history in one array, 5.3): +0 an int (unknown), +4 the record (pointer in the game), +8 the year (word), +0xa winner, +0xe runner-up, +0x12 third (club pointers), +0x16 an int (the fourth club in the nation rows of the data; unknown).

- **Write**: `comp_util_add_history_record` 0x4afce0, cdecl (record, winner, runner-up, third, extra). It takes the last free row (+4 is 0, 0x4afd05..0x4afd23) or grows the array by 0x14 rows (0x527f60 at 0x4afd2c), and writes the record (0x4afd54), the game year [0xae2c92] (0x4afd57..0x4afd62) and the clubs (0x4afd6a on).
- **Read**: `comp_util_get_latest_placed_club` 0x4afd80, cdecl (record, place), returns Club* or 0. It sorts the array (qsort 0x945333 with `0x540780`, 0x4afd9b: by record id, then by year with the newest first), bsearches a key with the record and year 0xffff, which the compare treats as equal to any year (0x5407b4..0x5407c6; 0x946464 at 0x4afdcb), walks back to the first row of that record (0x4afddd..0x4afe04), and returns +0xa, +0xe or +0x12 for place 0, 1 or 2 (0x4afe07..0x4afe30). No row gives 0 (0x4afe9e). A place above 2 gives the box "comp_util" line 0x196 (0x4afe3b..0x4afe8c).

For the Finalissima: `0x4afd80([0xae23d0] + [0x9cf7a4] * 0x6b, 0)` gives the latest European champion and `0x4afd80([0xae23d0] + [0x9cf7a8] * 0x6b, 0)` the latest Copa America champion, as the national team club. The Confederations Cup does this through `0x58ccb0` (record from the key id at 0x58cdc3..0x58cdd6, call 0x58cdda). The year of the row is the game year when the class wrote it (0x4afd57), not the edition year. Inference: a Finalissima in June 2022 finds Euro 2020 (written in July 2021) and Copa America 2021. The result can be 0 (no row) or a club with no nation (slice 27, "An empty team in the Euro finals"), so the caller tests both. In the data the newest rows are Euro 2000 (record 449, year 2000) and Copa America 2001 (record 450) in 3.9.68.

## 9. Unknowns

- The number of type lists per day of the fixture store (0x5a9cf0 not read), and whether a raw entry index from 0x669470 (type 4 for record 344) shares a list with a nation entry numbered by +0x14.
- What the fixture store does when a friendly and a fixture of type 2 or more fall on one day (0x5ae380, 0x5adba0).
- The meaning of fixture +0x40, +0x42, +0x43, +0x47, +0x4d and +0x4e bits beyond the tests cited.
- The meaning of the Australia numbers in `select_squad` and of record +0x57, +0x58.
- `get_squad_limits` with no fixture (0x76d4ef on).
- The effect of entry 1 on the 10 other callers of 0x669470 for a new record (season text 0x6697f4, Cup 0x519940, 0x519954, 0x519c5f, League 0x684896, 0x49d10d, 0x7690f7, 0x769b1d, 0x77df33, 0x7d7ccb, 0x8d301e).
- Whether the box of 0x58cf70 stops a game that runs unattended. The box 0x5e8290 waits for a click (`waits.c`), then the function returns with no points.
