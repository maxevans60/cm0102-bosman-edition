# Club table users and new club records (0.6.0 slice 15b)

Date 2026-10-19. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Task: about 40 new club records (0x245 bytes each) must join the club table at a new game, before the competitions are made, each with a squad. A saved game must stay loadable.

Terms: "club table" is [0xae23b4], "club count" is [0xae235c] (clubs and national teams), "national teams" are the last 2 x [0xae2350] rows (nation count 213, nat_club.dat 426 rows), [0xae23b8] points to the first of them, [0xae2360] is their count. "Row" is the index in the club table. Each club holds its own row at +0x00 after the load and after the trim.

Data counts of the 3.9.68 files: club.dat 13,214 rows, nat_club.dat 426 rows (2 x 213 nations), stadium.dat 8,917, colour.dat 34, city.dat 6,682. eng.lng holds 13,640 club name rows (13,214 + 426), 0x4f bytes each, from file offset 0x20 (header of 8 ints: 13640, 6, 19, 213, 6682, 433, 8917, 543).

## 1. Time line of a new game

`create_game` 0x828110, calls in order (assembly):

- 0x8282a7 `0x524d50`, the load `load_database` 0x528540 (only when the nation table is empty, the start-new-game path loads earlier, see `2026-10-14-start-new-game-inventory.md`).
- 0x8282ec 0x5d8480 settings, 0x8282f1 0x541e60 squad trim of senior and B teams, 0x82836c 0x539750 tactics, 0x828371 0x539ae0 comp history.
- 0x828389 `database_trim_staff_tables` 0x524d70 (slice 15 wraps its start, `season_clubs.c`).
- 0x828395 `database_trim_club_table` 0x5267c0.
- 0x8283c3, 0x8283d1 0x839920 (Greek divisions, only when Greece is selected).
- 0x8283d9 0x544970 (moves the coaches, scouts, physios and manager of a B team to its senior club, through 0x540a50).
- 0x8283ec malloc(club count x 4) into [0xae2d1c], then 0x82846c 0x53db40(1): copies each club's +0x00 into [0xae2d1c] and sorts it (qsort comparator 0x540020). So 0x53db40 runs at a new game, after both trims.
- 0x828474 0x830520 start date, 0x828485 0x54cab0.
- 0x828759: [0xae2d18] = club count, 0x828769 malloc(count x 6) into [0xae2d14], each row {-1, 0, 0} (0x8287f8..0x82880e).
- 0x82882f..: staff +0x39 set from the squad lists (inventory 3c).
- 0x828894 0x535a80 Player Initialisation, 0x8288ae 0x532e00 (fake players, only for "Use Real Players: No").
- Then the 44 steps that make the competitions (inventory 3e), and 0x829fd9 0x5cc850.

### The load 0x528540

- Signature: 0x528540(source, shared, from_stream). A data load is `0x524d50(dir, shared)` = 0x528540(dir, shared, 0). A saved game is `0x524d10(stream, ...)` = 0x528540(stream, 1, 1), called at 0x82abf2 in `load_game_data` 0x82a9c0 after 0x528080 frees the tables (0x82abea). So a saved game takes the shared branch too.
- Shared branch ([0xae2340] = shared != 0, 0x529912..): the club count is the sum of the index entries of table 0 and table 1 (0x529935.. case 0 and case 1), table 1 also adds to [0xae2360]. The byte size is summed in one local, and the club table is ONE malloc of exactly (clubs + national teams) x 0x245 bytes ([0xae23b4], pseudocode line "DAT_00ae23b4 = _malloc(pcStack_830)" near 0x529dc0). No spare club rows.
- [0xae23b8] = [0xae23b4] + ([0xae235c] - [0xae2360]) x 0x245, after the allocations.
- The branch with shared == 0 (0x5289c7..) mallocs table 0 and table 1 separately (case 0 and case 1 at the top of the switch). Neither the new game nor a saved game takes it (callers above). Inference: unused by the game.
- The only other array that the loader sizes by the club count is [0xae23fc], and only when the index has the language block: 0x52889b reads a count (-1 means none), 0x5288a8 mallocs count x 4, 0x5288b2 stores [0xae23fc], 0x52893a reads count x 4 bytes. The shipped index.dat has no block, so [0xae23fc] is NULL after a data load (start-new-game inventory step 5).
- The id pass 0x5315a0 is called at 0x52d1aa, inside the loader. After it every club pointer field is a pointer.
- At the end of a data load (`cStack_81d` == 0, pseudocode lines 2462..2503): rows from count - nations to count (the second 213 national team rows) get reputation +0x80 = half the reputation of the row that 0x53b3d0 gives (called at 0x52d854), every club whose division +0x57 has the name "Reserve" (0x9b5b40) gets division NULL.
- `set_key_ids` 0x601a40 is called at 0x52d8fa, after the id pass. It matches the name of every club row 0 .. count-1, national teams included (loop 0x604385..0x6043a7, matcher 0x6146b0(name, row)). The club key ids are row numbers (0x9d00a4..0x9d0524).

### `database_trim_staff_tables` 0x524d70

- Its first call is `database_generate_extra_non_playing_staff` 0x527240 (pseudocode line 97, before the allocation of 0x2a2). So a club in the table at the start of 0x524d70 gets the extra coaches, scouts, physios and a manager like every other club of a selected nation.
- It then trims the staff table and fixes the staff pointers of every club row 0 .. count-1: +0x9b..+0xaf (6), +0xbf, +0xc3 (3), +0xcf, +0xd3, +0xd7 (50), +0x19f (5), +0x1b3 (7), +0x1cf (3) (pseudocode lines 598..745). Its loop bound is [0xae235c], so new rows are covered.
- It reallocs in place: staff to (count x 0x6e + 0x52d x 0x6e), players to (count + 0x52d) x 0x46, non-players to count x 0x44, preferences to count x 0x34 (lines 998..1001), with the assert that realloc returns the same block. The 1,325 (0x52d) spare staff and player rows stay allocated.

### `database_trim_club_table` 0x5267c0

- It works in place. Steps:
  1. A temporary map old row -> new address (operator_new(count x 4) at the start) and a keep flag per row (0x945501(count, 4)).
  2. Per nation (0x945501(nations, 0xd0)): the best clubs of each nation by reputation +0x80, up to 15 (database size option 1 or 2), for nations without the selected flag and not in a fixed list of key nations (England 0x9cf2e4 and 14 others).
  3. Keep flags for every club that a staff row names at +0x39 or +0x24, for the clubs of comp history +0x0a, +0x0e, +0x12 (bounds 0 .. count-1), and with database size 0 for the club of a staff history row of a selected nation.
  4. For each row in order: kept when `0x60ba80(club id, new row)` returns 1 (a club key id: it rewrites that key id to the new row), when flagged, when the row is a national team (`club id >= count - 2 x nations`), or when its nation is selected (nation +0x11c), except an English club with reputation < 501 when a flag is set. A kept row gets the next new address.
  5. Fixes every club pointer through the map, using the old row read from the target's +0x00: club +0xb3, +0xb7, +0xbb, staff +0x39, +0x24, preferences +0x04..+0x18 (6), staff history +0x0a, comp history +0x0a, +0x0e, +0x12, +0x16. A dropped club gives NULL.
  6. Mallocs a new [0xae23fc] of (kept rows) x 4, copies each kept row down to its new address (0x91 dwords and 1 byte), writes [0xae23fc][new row] = old row, and writes the new row into the club's +0x00.
  7. Sets [0xae235c] = kept rows, reallocs the club table to kept x 0x245 (0x945667) and asserts the same address (line 0x4ca), sets [0xae23b8] = table + (count - [0xae2360]) x 0x245, logs "Club table has %d entries".
- So the new ids are the new rows, given in table order. The pointer fix of step 5 depends on +0x00 of each target holding its old row. A new club must hold its own row at +0x00 when 0x5267c0 runs.
- [0xae2360] is not changed: all national teams are kept.
- Unknown: whether the old [0xae23fc] is freed when it was set (not seen in the pseudocode).

## 2. Who reads the club table and the club count

Method: Ghidra xrefs of [0xae23b4] (4,512 references) and [0xae235c] (about 700), each mapped to the procedure that holds it (procedure starts from `list_procedures`, names from `function-map/names.tsv`).

- 840 procedures read [0xae23b4]. By source file: transfer_manager.cpp 122, contract_manager.cpp 37, manager_manager.cpp 30, media.cpp 25, fin_prm.cpp 24, transfer_screens.cpp 22, Database.cpp 22, shortlist_manager.cpp 18, player_regen.cpp 12, national_teams.cpp 12, friendly.cpp 12, then about 120 files with 1 to 9 each (every league and cup class, the screens, news, awards).
- 98 of the 840 also read the nation count [0xae2350]. Inference: they hold the national team boundary `count - 2 x nations` inline (Database.cpp 10, find_screens.cpp 6, about 60 league and cup files 1 to 3 each, Setup.cpp 0x820d40 and 0x827110, player_regen.cpp 0x7acfc0 and 0x7ad0e0, national_teams.cpp 0x7777e0, index.cpp 0x601a40).
- [0xae2360] (national team count) is read only by 0x528540, 0x5267c0 and 0x52dad0. [0xae23b8] only by 0x528540 and 0x52dad0, written by 0x528540, 0x5267c0 and 0x528080 (data map, Database.cpp.md).

### Groups

Loader and trims (0x528540, 0x5315a0, 0x524d70, 0x527240, 0x5267c0, 0x541e60, 0x544970, 0x53db40, 0x601a40, 0x5339a0):
- Every loop of these reads the count at run time (section 1). A larger count with the new rows before the national teams breaks none of them, when the new rows hold their own row at +0x00, their ids are below count - 2 x nations, and every field holds a valid pointer or NULL.
- 0x601a40 runs inside the load (0x52d8fa). Club key ids it sets are rows. A move of the national team rows after it leaves a key id that names a national team row pointing at the wrong row. Whether any club key id names a national team is unknown (the matcher 0x6146b0 was not read, it runs over all rows, national teams included, 0x604385..0x6043a7).

Save and load of a game:
- `save_database` 0x52dad0 turns every pointer back into an id (club fields from +0x53 to +0x1cf, pseudocode lines 81..345 of 0x52dad0), writes table 0 (clubs) as [0xae235c] - [0xae2360] rows from [0xae23b4] and table 1 (national teams) as [0xae2360] rows from [0xae23b8] (index entry +4 = the row count, writer 0x54e5d0 in the blocks with assert lines 0x1166 and 0x1198). No count is fixed.
- At its end it writes the language block: 1 byte, 0x1cc bytes, the club count and [0xae23fc] as count x 4 bytes, then [0x9b5984] (0x531454..0x5314fc), then calls 0x5315a0(1) to make pointers again.
- `load_game_data` 0x82a9c0 loads with 0x528540(stream, 1, 1) (0x82abf2). The loader sums the saved counts of table 0 and table 1 into [0xae235c] and reads [0xae23fc] back with its saved count (0x52889b..0x52893a).
- So a save with 40 more club rows loads, when [0xae23fc] has one entry per row. With fewer entries the save writes count x 4 bytes from a shorter block (heap bytes past its end). Inference: a larger count breaks nothing here.

National team lookups (0x53b3d0, 0x53b4f0, 0x53b900, 0x53c190 at 0x53c25d and 0x53c306, 0x53bd10 at its end, 0x542740 at 0x542804, 0x5339a0):
- All take the national teams as the last 2 x nations rows of the count. A larger count breaks them only when the national teams are not the last rows.

Loops over all clubs (most of the 840: the league classes that take clubs by division, the cups, transfer and contract managers, screens):
- They loop 0 .. [0xae235c]-1 and test the club's own fields. Inference from the 0x56ecb0 example (slice 15 doc) and the loops read here: a new row with a division, a nation and a reputation joins them like any club. Not every one of the 840 was read.

Arrays sized by the club count:
- At creation, after both trims: [0xae2d1c] count x 4 (0x8283ec, filled by 0x53db40), [0xae2d14] count x 6 and [0xae2d18] = count (0x828759..0x82880e). Both are made after 0x5267c0, so they take the final count.
- [0xae23fc] (made by 0x5267c0, section 1). Before 0x5267c0 no other array sized by the club count exists in the load (the loader allocates only the club table by it on a data load).
- Inference: arrays that the 44 steps or later code size by the club count are made after the trims and take the final count.

## 3. National teams

- `database_get_national_team` 0x53b3d0: returns [0xae23b4] + (nation +0x00 + [0xae235c] - 2 x [0xae2350]) x 0x245 (asserts at lines 0x3346, 0x334d, 0x3354 for no nation, no club table, no nation table). Confirmed.
- `database_get_national_u21_team` 0x53b4f0 (port: `national_team_b`, database_links.c): row nation +0x00 + count - nations, the second group of 213.
- 0x53b900: returns `club +0x00 >= [0xae235c] - 2 x [0xae2350]` (assert line 0x349f for no club). So the test reads the club's +0x00, not its address.
- 0x53bd10 (add staff to a club) uses the same test on +0x00 to choose staff +0x24/+0x28 (national) or +0x39/+0x3d (club), at its end.
- 0x5267c0 keeps a row as a national team by the same test on +0x00 (section 1 step 4).

What holds a pointer to a national team row at the start of 0x524d70 (all fields are pointers then, after 0x5315a0 at 0x52d1aa):
- staff +0x24 (national club) and its job +0x28. 0x53bd10 writes +0x24 for a national team (end of 0x53bd10).
- staff +0x39: only for a club, for a national team 0x53bd10 writes +0x24. In the 3.9.68 files no staff row names a national team at +0x39 (count over staff.dat, section 6).
- club +0xb3, +0xb7, +0xbb of any row (rivals, a national team's rivals can be national teams).
- preferences +0x04..+0x18 (6 clubs).
- staff history +0x0a.
- comp history +0x0a, +0x0e, +0x12, +0x16. The nation comp history rows (the last [0xae2384] rows of the same array, [0xae23dc]) name national teams.
- [0xae23b8].
- This is the list of club pointers that 0x5267c0 fixes when rows move (section 1 step 5). That function moves the national teams down at every new game, so the game itself knows no other club pointer that must survive a move (inference).
- Club key ids 0x9d00a4..0x9d0524 hold rows, not pointers (0x60ba80). See section 2 for the open point.
- Rows computed from the count: 0x53b3d0, 0x53b4f0, 0x53b900 and the inline tests. They stay right when the national teams stay the last 2 x nations rows.

## 4. Names indexed by club row

- `club_get_english_name` 0x545880: when the language [0x9b5984] is 0 or 1 it returns club +0x04. Else it builds the name table [0xae2408] once (0x544fc0) and returns row [0xae23fc][club +0x00] x 0x4f of its block [0xae2408]+0x20, or with [0xae23fc] NULL row club +0x00 when it is below the table count.
- `club_get_english_short_name` 0x545770 reads [0xae23fc] in the same way (xref 0x5457f2, not read in full).
- [0x9b5984] is 0 after a data load (0x528a30, start-new-game inventory step 5). The port keeps the language at 0 (select-leagues spec). So in the port a club's name and short name come from +0x04 and +0x38 of its record.
- The .lng files hold 13,640 club rows (13,214 clubs and 426 national teams), 0x4f bytes each: long name at +0x00, short name at +0x34 (layout from 0x5339a0 and eng.lng row 0). No row has an empty long name (check of eng.lng).
- `database_load_language_names` 0x5339a0 (only when the settings language differs from [0x9b5984], start-new-game inventory, 0x5d8480):
  - with [0xae23fc] NULL (before 0x5267c0) it renames club row i from .lng row i for i below the .lng club count
  - with [0xae23fc] set (after 0x5267c0) it renames club row i, for i below [0xae235c], from .lng row [0xae23fc][i] (0x533ecb..0x534150).
  - So a new club needs an entry in [0xae23fc] that is a valid .lng row (below 13,640), or a language change reads past the .lng block. Any valid row gives the new club that row's name after a language change.
- `set_key_ids` 0x601a40 reads [0xae23fc] at 0x603e45 when it reads eng.lng (only with [0x9b5984] != 0, 0x603c52).
- What a new club needs: its name at +0x04 (51 bytes) and short name at +0x38 (26 bytes), and an entry in [0xae23fc] after 0x5267c0. 0x5267c0 writes that entry itself (the row of the club at trim time). After 0x5267c0 the entry of a moved national team is its row in the enlarged table, not its .lng row, and the entry of a new club is the old row of a national team. Inference: the port must rewrite these entries after 0x5267c0 (national teams: minus the number of new rows, new clubs: a chosen valid row), or a language change gives wrong names.

## 5. Stadium, colours, city

- At the start of 0x524d70 club +0x69 and +0x6e (stadiums), +0x83..+0x97 (6 colours) and +0x53 (nation), +0x57, +0x5b, +0x60 (comps) are pointers (id pass 0x5315a0, the save turns them back into ids, 0x52dad0 lines 81..180).
- A club has no city field. Its city is stadium +0x38 (database.h, id pass 0x53228c).
- Many clubs share a colour record (34 colour rows for 13,214 clubs). Inference: a new club can point to an existing stadium and existing colour records, the save writes the id of the record, so a shared record saves and loads.
- The data load allocates the stadium and colour tables with no spare rows: they sit in one block with the continents, comps, nations (base [0xae23a4]: comps, then [0xae23ec] colours = + comps, [0xae23a8] nations, [0xae23b0] stadiums = nations + 213 x 0x122, loader pseudocode near 0x529e25 and the lines that set [0xae23b8]). Only the club comps have 127 spare rows (0x52a068).

## 6. Where and how to add the records

Facts that decide it:
- The data load makes the club table with no spare rows (section 1). Any point needs a larger block.
- In the files the national teams have ids 13,214..13,639 (nat_club.dat +0x00 of rows 0 and 425), so their ids already count from the start of the shared table. 351 staff rows name a national team at file +0x24, none at +0x39 (count over staff.dat).
- The national teams must stay the last 2 x 213 rows (section 3). So every new row goes before them, and every reference to a national team moves by the number of new rows (K).
- `database_trim_senior_and_b_team_squads` 0x541e60 runs at 0x8282f1, before 0x524d70. It sets club +0x244 (inventory 3c). A club added at the start of 0x524d70 misses it.
- `set_key_ids` 0x601a40 runs at the end of the load (0x52d8fa). A move after it can leave a key id on a wrong row (section 2).
- The staff reserve is sized at the load (0x529b39, 0x529bd1, `rt_staff_reserve_size`), and 3.9.68 with Select All leaves only 377 blank rows (select-all doc). New players taken from the reserve need a larger reserve, and that size must be known at the load. This holds for every insertion point.

### Option A: at the start of 0x524d70 (the inference to test)

Steps: grow the block to (count + K) x 0x245 with the game's allocator, move the 426 national team rows K rows down, write their new rows into +0x00, fix the pointers, fill the K rows, [0xae235c] += K, [0xae23b8] = table + (count - 426) x 0x245, set +0x244 of each new club as 0x541e60 would, add K to every club key id 0x9d00a4..0x9d0524 that holds a national team row, after 0x5267c0 rewrite [0xae23fc] (section 4).

Pointers to fix:
- when the block moves (a realloc that grows can move it): every pointer into the club table, by the distance of the move, when it does not move: only those into the national team rows, by K x 0x245
- staff +0x39 and +0x24 (all staff rows, reserve and manager rows too)
- club +0xb3, +0xb7, +0xbb (all rows, the national teams too)
- preferences +0x04..+0x18
- staff history +0x0a
- comp history +0x0a, +0x0e, +0x12, +0x16 (club and nation comp history rows)
- [0xae23b8]
- any club pointer that port code holds at that time (the slice 15 club moves run in the same wrap, unknown which pointers they keep).

### Option B: in the load, before the id pass (0x52d1aa)

Steps, at a data load of a season game only (from_stream 0, never for a saved game):
1. At the shared sizing (case 0 of the switch at 0x529935..), add K x 0x245 to the club table size and K to [0xae235c]. [0xae23b8] is then computed K rows later.
2. After the reads and before 0x5315a0: write the K new records at rows 13,214..13,214+K-1 as file records (ids, -1 for none), with +0x00 = own row.
3. Add K to every club id >= 13,214 in: national team +0x00, club +0xb3/+0xb7/+0xbb, staff +0x24 and +0x39, preferences +0x04..+0x18, staff history +0x0a, comp history +0x0a..+0x16. (Same field list as A, as ids.)
4. The id pass, the staff pass, 0x601a40, 0x541e60, 0x527240, both trims and the language index then treat the new clubs as file clubs. The id pass checks their ids (a bad id gives NULL and a log line).
- To verify before a build: that the reader of table 1 writes to [0xae23b8] and not to table + (table 0 count) x 0x245 (the readers were not read), and that the season and the club list are known at the data load (slice 11b already changes dates at the end of the load, which suggests they are).

### Option C: reuse rows that nothing names (alternative, for Max)

3,390 of the 13,214 club rows are named by no staff (+0x39, +0x24, preferences), no club rival and no history row (count over the 3.9.68 files). Overwriting K of them, of nations that no league uses, needs no larger table, no move and no pointer fix (only the key id check by 0x60ba80). It removes K small real clubs from the world, so it is a design choice. Not checked: key ids and squads of those rows.

### Recommendation

Option B. Reasons:
- It moves no memory, so no pointer can go stale, including pointers that port code holds.
- The fixes are id shifts on a known list of fields, before the game's own id pass checks them.
- 0x601a40 (key ids), 0x541e60 (+0x244), 0x527240 (coaches, scouts, physios, manager), the trims (new ids, [0xae23fc]) and the save all run after it unchanged.
- A has the same field list plus a block move, a key id fix and the +0x244 step, at a point where pointers are live.
- The staff reserve has to grow at the load in every option, so B needs nothing that A does not.
If the data load cannot know the season's clubs, use A with the full list above.
In both: after 0x5267c0, set [0xae23fc] of each moved national team to its .lng row (value - K) and of each new club to a valid .lng row (section 4).

## 7. What a club in a league with a squad needs

Club fields that the code read here uses:
- +0x00 own row (0x5267c0, 0x53b900, 0x53bd10, 0x53db40).
- +0x04 name, +0x37 byte, +0x38 short name, +0x52 byte (0x5339a0 sets +0x37 and +0x52, meaning of the two bytes unknown).
- +0x53 nation: 0x5267c0 keeps a club of a selected nation (nation +0x11c), 0x527240 makes staff only for a club of a selected nation, England with reputation 500 or less gets no extra staff (select-all doc).
- +0x57 division: a league takes its clubs by it (slice 15 doc, 0x56ecb0), 0x527240 loop 4 needs it for a manager. +0x5b last division (cups, slice 15 doc step 5). +0x60 comp (meaning unknown).
- +0x64 byte: 0x7a6f10 caps a regen's wage at 5000 when it is 3 (meaning unknown).
- +0x69, +0x6e stadiums, +0x83..+0x97 colours (section 5).
- +0x80 reputation: 0x5267c0 ranks by it, 0x527240 counts the coaches (5 x (rep + 500) / 10000), scouts (x 7) and physios (x 3), step 0x23 of create_game gives a random value when it is 0 or less (inventory 3c).
- +0xb3..+0xbb rivals (NULL allowed: 0x5267c0 tests for NULL).
- +0xbf chairman, +0xc3 directors (3), +0xcf manager, +0xd3 assistant, +0xd7 squad (50), +0x19f coaches (5), +0x1b3 scouts (7), +0x1cf physios (3). 0x527240 fills the empty coach, scout and physio slots and an empty manager slot (job 8, 9, 10 and 5) for a club of a selected nation. So a new club needs only its players, the game adds the rest.
- +0x1db: create_game sets -1 for every club (inventory 3c). +0x1e0 team (20) and +0x230: unknown.
- +0x244: set by 0x541e60 (1 when 0x540a50 finds a league with a nation).
- +0x6d: create_game step 0x23 sets it (0 for "Bright..." and "Wimble...", else 1).
- +0x73..+0x7f: inference money values, create_game step 35 sets club finance fields (inventory 3e). Unknown which file values they need.
- Inference: build each new record from a copy of a real club of the same nation and division, then set the known fields. The unknown bytes then hold values that the game already plays with.

How a player links to his club:
- club +0xd7[i] = the staff row (first empty slot of 50), staff +0x39 = the club, staff +0x3d = 11 (player, 12, 13, 15 are player and manager, assistant or coach). `database_add_staff_to_club` 0x53bd10(club, staff, job, date, set_link, as_player) writes the list slot and, with set_link, +0x39/+0x3d (or +0x24/+0x28 for a national team). It refuses a staff row at or above staff count - 16 unless the job is 5 (the 16 manager rows).
- The reverse: `database_leave_club` 0x53c190 (port `database_leave_club`) clears the slot of the job and, with unlink, +0x39.
- create_game sets staff +0x39 for every squad member whose +0x39 is NULL (0x82882f..). The squad list is the master, +0x39 follows it.
- staff +0x3e is the date of the last move (0x53c190 writes it), +0x46 the contract end (read by 0x4c7560). +0x4e and +0x52: 0x7a6f10 sets +0x4e = 0 and +0x52 = 0x4d0c60(...) for a regen (inference: contract values).
- 0x541e60 trims a squad of more than 50 and keeps at least 3 players with player +0x0f > 14 (inventory 3c).

A staff row that becomes a player:
- staff +0x5f is the kind of row: 1 non-player data, 2 player data, 3 both, 6 blank reserve row (0x524d70 tests: +0x69 for kinds 1 and 3, +0x61 for 2 and 3).
- 0x524d70 drops every row with +0x5f = 6 (first test of its keep loop over the staff table, pseudocode after the per-nation pass: map entry 0). A reserve row made into a player must get +0x5f = 2 (or 3) before 0x524d70, or the trim drops it.
- 0x524d70 keeps a staff row whose nation (+0x1a), second nation (+0x1e) or club nation (+0x39 -> +0x53) is selected (keep tests near 0x5255d5), among others. A player of a new club of a selected nation is kept.
- The player row: staff +0x61 must point to a player row (table 10, 0x46 bytes). A reserve row has none (+0x61 -1). The player table has 1,325 (0x52d) allocated rows past its count (0x52a04b, kept by the realloc of 0x524d70). Unknown whether other code uses those rows (the regen pool 0x7a8350 is a candidate).
- The preferences row: a file staff row has one (+0x65 = its row, 0x52caa1), a reserve row has NULL, and the preferences table has no spare rows (staff count of the file + 16). Unknown whether a player with NULL +0x65 is safe.
- The non-player row: each reserve row has its own (+0x69, 0x52a1c2). A player row with +0x5f = 2 does not need it (inference from the kind values).
- Names: +0x04, +0x08, +0x0c point to first, second and common name rows (60 bytes each: name, id, nation id, use count). The name tables have only 16 spare rows each (the manager rows, 0x52add4). A new name needs an existing row with that text, or more rows at the load.
- Also needed: +0x10 birth date, +0x18 year, +0x1a nation. Player attributes come from the player row, 0x535a80 (Player Initialisation) then normalises the attributes of every staff (0x538bc0, 0x538ec0, 0x539490 and the vector functions 0x53a610..0x53aa10), it has no club loop.
- The 16 manager rows must stay the last 16 rows of the staff table (0x53bd10 test, 0x527240 index count - n - 17). So new staff rows cannot be added by a larger staff count, they come from the reserve, and the reserve size must grow by the rows taken (staff_reserve.c).

## 8. Squad fill at a new game

- No code found that adds generated players to a club with too few. Checked: 0x541e60 only removes players over 50, 0x544970 only moves non-players from a B team to its senior club, 0x527240 makes only jobs 8, 9, 10 and 5, 0x535a80 has no club loop (no read of [0xae235c]) and only sets attributes and names, 0x532e00 only shuffles for "Use Real Players: No", the PLAYER_REGEN constructor 0x7a5380 (step 36) builds the retirement list. No string about a minimum squad in the binary (string search). Not every one of the 840 club table readers was read, so this is "none found".
- The function that makes one new young player: `PLAYER_REGEN::regenerate_as_youth_player` 0x7abac0, thiscall (ECX = the PLAYER_REGEN object [0xb7434c], one stack argument: the staff row to reuse), ret 4 (0x7acfad). It needs a staff row with no club (+0x39 NULL, 0x7abaec) and a flag 0x30 in the per-staff info row of [0xae2d0c] (0x4f bytes a row, made at create_game 0x8284ad), and it uses game objects such as [0xb74318] (0x7abaf7). Its only caller is `process_regen_queue` 0x7a65d0. It reuses a retired staff row and places the player at a club itself (`player_regen_sign_for_club` 0x7a6f10, which uses the contract and wage code). So it cannot run before the competitions are made.
