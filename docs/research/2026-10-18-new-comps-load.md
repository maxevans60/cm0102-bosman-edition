# New competition objects across a save and a load (0.6.0 slice 26)

Date 2026-10-18. Research only. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Evidence: capstone disassembly of the clean exe (the `cm0102` venv) and REA with Ghidra (pseudocode of `Cup::load_from_disk` 0x51fc00 and `League::load` 0x687b10). "Inference" marks a statement that the code does not prove.

The question: the runtime makes a competition object that the original never makes (for example a Copa Sudamericana), puts it into the array of a league table entry (entry +0xc count, +0x10 array, entries at 0xb63d60, 0x48 bytes) with `season_change_add_comp`, and must keep it across a save and a load.

## 1. Result for the module design

1. **The load point.** At a load, the creators at entry +8 run again and make only the original's objects, with fixed counts (section 3). The runtime must make its own objects again after `key_nation_create_all_comps` 0x669180 returns (call at 0x82c5aa) and before `build_comp_object_table` 0x8386f0 runs (call at 0x82c5f6). Between the two calls there are only the progress box and disk checks (0x82c5b7..0x82c5f1). A wrap of 0x8386f0 that does the work before it calls the original is the simplest hook. 0x8386f0 has two callers, `create_game` 0x828110 (at 0x829640) and `load_game_data` 0x82a9c0 (at 0x82c5f6), so the wrap must know that a load runs (season.c already wraps 0x82a9c0).
2. **No separate load call.** Every constructor calls its own load with flag 1 before it builds anything (Conmebol_Merc 0x4c471f, World_Club_Cup 0x92b52f). The load takes the file name from the record that the constructor got. So the runtime only does operator new 0x944cf1 with the class size, calls the constructor (thiscall, year, record pointer, `ret 8`) and adds the object to its entry. The load then replaces the year +0x40 (Cup 0x520288, League 0x6883c3) and the record pointer +4 (Cup 0x5207f3, League 0x688541..0x68854d) from the file.
3. **The save needs no change.** `key_nation_save_all_comps` 0x6692f0 saves every object of every entry array, and the save then packs every `*.tmp` file of CM3_TEMP (section 2). An added object in an entry array is saved as an original one is.
4. **The runtime must keep its own list.** The original does not record which objects an entry holds. The save part (`bosman_season.dat`) must hold, for each added object: the entry, the class (constructor and operator new size) and the record id. Inference: a record id is enough to find the file, because the file name is the record name and id.
5. **The table [0xadadfc] is a second owner.** The daily update `game_update_comps_for_date` 0x5ce060 calls vtable +0x14 only for the objects of the table (0x5ce071..0x5ce088), not of the entry arrays. `season_change_add_comp` (season_change.c line 191) writes only the entry array. An object added in a game, after 0x8386f0 ran, is not in the table: it gets `start_new_season` from the season pass (0x5d5d1c walks the entry array) but no `update_for_date`, so no draw and no start date. The runtime must also write `[0xadadfc][record id] = object` when it adds an object outside the load path.

## 2. Save (question 1)

### 2.1 Who calls what

`save_game` 0x82e430:

1. 0x82e4d4..0x82e513: it copies the CM3_TEMP folder (`0x90bde0("CM3_TEMP", "")`, string 0x987da0) into a stack buffer (esp + 0x3ec).
2. 0x82f139: `key_nation_save_all_comps` 0x6692f0. It walks the 35 entries (0xb63d6c to 0xb64744, step 0x48). For each entry with an array (+0x10 not 0), it calls vtable +0x1c with the argument 1 for each object that is not null (0x669306..0x66931c). It always returns 1 (0x66932a).
3. 0x82f538 or 0x82f56d: `zadd_matching_files` 0x933610 with the folder buffer and `"*.tmp"` (0xa813ac). The branch picks archive type 3 or 4 from the option bit 0x8000 of `[0xae3138] + 4` (0x82f517). 0x933610 makes the path with `_makepath` 0x945b13 (0x933635), walks the folder with findfirst 0x4c0aa0 (0x933649) and findnext 0x4c0b50 (0x93373a), and for each file opens it "rb" (0x9336a1), adds a part with `zopen` 0x9326e0 in write mode (0x9336c4) and copies it with `zwrite` 0x932e10 (0x933701).

So the save takes **every** `*.tmp` file in CM3_TEMP, not a fixed list. Inference from the stack offsets: the folder at 0x82f526 (esp + 0x3f4 after two pushes) is the CM3_TEMP buffer of step 1.

### 2.2 What the save functions write

| Class base | Vtable +0x18 (load) | Vtable +0x1c (save) | Evidence |
|---|---|---|---|
| Cup (0x9693c8) | `Cup::load_from_disk` 0x51fc00 | `Cup::save_cup_to_disk` 0x5210f0 | vtable words at 0x9693c8 + 0x18, + 0x1c |
| League (0x96d020) | `League::load` 0x687b10 | `League::save` 0x6891c0 | vtable words at 0x96d020 + 0x18, + 0x1c |

All 15 continental and world classes of `2026-10-16-sweep-europe.md` section 2.1 keep these four base slots (vtable words read for each class). Only Uefa_Cup (0x903300, 0x903340) and Intertoto_Cup (0x633eb0, 0x633f50) override +0x20 and +0x24, the extra state that the base load (0x51fc00 calls +0x20) and the base save (0x521255 calls +0x24) call. The others have the shared 0x48ceb0 and 0x48cea0.

The file name, the same in both save functions (Cup 0x5210fe..0x521193, League 0x6891ce..0x68926b) and both loads:

1. `sprintf(buf, "%s", record + 4)`: the long name of the record (0x521113, 0x6891e3).
2. Each `/` and `.` in it becomes `_` (0x521139..0x52114c).
3. With flag 1: `sprintf(this + 0x53, "%s_%d.tmp", buf, [record + 0])`, the record id (0x52114e..0x521162, 0x689226..0x68923a). The name is kept in the object at +0x53 (0x50 bytes).
4. With flag 0: the same pattern with the year +0x40 (0x52117e, 0x689256), and only when +0x45 is not 0 (0x521175, 0x68924d). The League load with flag 0 opens `"%s_%d.bak"` (0x9f1624) instead. Inference: flag 0 is a copy of a past edition. The game-save path uses flag 1 only.
5. The file is `CM3_TEMP\<name>`, mode "wb" (0x9a7f28, 0x5211ba..0x5211ce, 0x68926e..0x689282).

What the Cup save writes, in the order of the matching load 0x51fc00 (pseudocode): the name (0x50 bytes, 0x521228), a Comp_Stats flag and the Comp_Stats object (0x52124b, `0x4ac4e0`), the extra state of vtable +0x24 (0x521255), then the fields +0x28, +0x52, +0x51, +0x4b, +0x4c, +0x24, +0x49, +0x4a, +0x45, +0xb1, +0x3c, +0x3e (0x521261..0x5212fc and on), and in the load also +0x40 (year), +0xab, +0xac, +0x42, +0x3a, +0x1c, +0x20, the array +0x18 (0x13 bytes per row, count +0x34), the record index, the round table +0xa3 (0x68 bytes per round, count +0x3c), the club list +0xa7 (6 bytes per club, count +0x3e), the round arrays +0xad, the stage count +0x2c, the current stage +0x30, +0x44, each stage (type 1 a `Cup_Stage` 0x5230e0 of 0xb2 bytes, type 2 a `League_Stage` 0x68b4f0 of 0xee bytes, both read from the same file) and the array +0x14 (count +0x36). The League save and load have the same shape with the League fields (load 0x687b10: +0xe2, +0xe4, +0xe1, +0x28, ...). The stages are in the parent's file, not in their own files.

### 2.3 Would an added object's file be saved with no change?

Yes, with these conditions:

1. Its record id differs from that of every other object. Two objects of one record write one file name, and the second save overwrites the first (from 2.2 step 3).
2. A Cup object must have +0xa3, +0xa7 and +0xad set. Else the save shows the error box "Cup.cpp" line 0x147e and writes no file (0x521196..0x5211b4, 0x521724..0x521772). The constructors build all three, or the load reads them.
3. Class fields after the base fields are saved only through vtable +0x20 and +0x24. Conmebol_Merc keeps its 20 invited clubs at +0xee and +0xf2 (0x4c585e, 0x4c5872), and its slots +0x20 and +0x24 are the shared ones, so that list is not saved. Inference: it is used only on the draw day, so this is the original's own behaviour for the Copa Mercosur.

The save function returns nothing (`ret 4` at 0x521721, 0x52178a), so a failed write is only the box, and `key_nation_save_all_comps` still returns 1.

Stale files: the load deletes `*.tmp` in CM3_TEMP (`0x90cd50("*.tmp")` at 0x82ac45, with CM3_TEMP at 0x90cd5e) before it extracts the save. During a game nothing deletes them, so the file of a removed object stays in CM3_TEMP and goes into each later save (inference: harmless, because no constructor asks for it).

## 3. Load (question 2)

`load_game_data` 0x82a9c0, in order:

| Step | Call | What it does |
|---|---|---|
| 1 | 0x82ac45 `0x90cd50("*.tmp")` | Deletes the temp files of CM3_TEMP |
| 2 | 0x82ac50 `zget_matching_size` 0x933860 | Size of the `*.tmp` parts, for the disk check |
| 3 | 0x82acbc `zextract_matching_files` 0x933320 with `*.tmp` | Extracts every `*.tmp` part. So every saved comp file is on disk from here |
| 4 | 0x82ad15 and on | `general.dat` and the other named parts (game date, tables) |
| 5 | 0x82b845 `key_nation_build_table` 0x667150 | Builds the league table again, with the creators at entry +8 |
| 6 | 0x82b8ab 0x669140 | Numbers the selected entries (+0x14 at 0x66915f) |
| 7 | 0x82b8fc..0x82c548 | Managers made again, among them `Uefa_Seeding` 0x903360 (0x82bc93) and `ConmebolSeeding` 0x4c5fe0 (0x82bd0c), each from its own temp file |
| 8 | 0x82c5aa `key_nation_create_all_comps` 0x669180 | For each entry with no nation, or with a selected nation (+0x11c, 0x669196), calls the creator `[entry + 8](entry)` (0x6691a7) and sets the dates +0x22 and +0x31 (0x6691c7..0x6691ff). A creator that returns 0 gives the box "key_nation.cpp" line 0x1dc |
| 9 | 0x82c5b7..0x82c5f1 | Disk checks and progress box only |
| 10 | 0x82c5f6 `build_comp_object_table` 0x8386f0 | Fills [0xadadfc] (section 4) |
| 11 | 0x82c644..0x82cd4b | More managers: 0x859aa0, `ReserveComp` 0x7e5fc0 and `create_sub_comps` 0x7e61c0 (0x82c6fc, writes into [0xadadfc]), ..., `load_fifa_transfer_rules` 0x839760 (0x82cd4b) |

Each creator sets the count and makes a new array with fixed contents. For South America (0x8318f0): when `key_nation_continent_has_selected_nation` 0x6699d0 is true (0x8319ba), it writes count 2 into +0xc (0x8319ce), allocates the array with `calloc(2, 4)` 0x945501 (0x8319d5) into +0x10 (0x8319df), and makes Conmebol_Liber and Conmebol_Merc with `new(0xf6)` and the records of the key ids 0x9cf63c and 0x9cf698 (0x831a7a..0x831b24). The year argument is the game year `[0xae2c92]` (0x831a96), which step 4 set from the save. The count does not come from the save: no code in steps 1 to 10 reads a count of an entry array (inference from the creators, which write fixed counts).

Each constructor then loads its object from its temp file: the constructor sets +4 to the record and +0x40 to the year, then calls the load with 1 (Conmebol_Merc 0x4c46f9..0x4c471f, World_Club_Cup 0x92b4e7..0x92b52f). The load returns 0 when the file does not open (Cup 0x51fc00 pseudocode, League 0x687b10 pseudocode), and then the constructor builds a new edition (Conmebol_Merc 0x4c472c on, World_Club_Cup 0x92b53c on).

So for an added object:

1. Its file is on disk after step 3.
2. No creator makes it in step 8, and step 8 replaces the array of its entry.
3. The runtime makes it after step 8 and before step 10: new, constructor (the constructor loads it), `season_change_add_comp(entry, object)`. Then 0x8386f0 puts it into [0xadadfc].
4. If the file is missing, the constructor makes a new edition with the game year and no error. Inference: the runtime should check that `CM3_TEMP\<name>_<id>.tmp` exists before it makes the object, or accept a new edition.

The array type: the creators use `calloc` 0x945501, `key_nation_delete_all_comps` 0x669290 frees the array with `free` 0x9452ca (0x6692cb). Operator new 0x944cf1 calls 0x944e58 and operator delete 0x944c94 calls `free` 0x9452ca (0x944c98). So the operator new array of `season_change_add_comp` is on the same heap and is freed correctly.

Note on the header text of season_change.h: the allocation at 0x5d5d74 in the season pass is an 8-byte object for `[0xdd7ee4]` (0x84b940, freed at 0x5d5d6a after 0x84be10), not an entry array.

## 4. The table [0xadadfc] (question 3)

`build_comp_object_table` 0x8386f0:

1. Count `[0xadadf8] = [0xae2378]`, the number of club_comp records (0x8386f0..0x8386fb).
2. Array `calloc(count * 4, 1)` 0x945501 into [0xadadfc] (0x838700..0x83870e). On failure the box "Setup.cpp" line 0x3896.
3. For each object of each entry array, not null: `table[*(object->record)] = object`, the index is the record id at record +0 (0x838788..0x8387be). There is no bounds check.
4. Then `0x5ca030` on the friendly object `[0xae2c74]` (0x8387c6): it writes the friendly object at the index of key id 0x9cf668 (0x5ca045) and up to 0x7f friendly sub-competitions at indices from friendly +0xaf (0x5ca050..0x5ca07a).

Other writers of the table: friendly code (0x5c5dcb, 0x5c5f13, a clear at 0x5c859f), `ReserveComp::create_sub_comps` 0x7e61c0 (key ids 0x9cf684, 0x9cf794, 0x9cf798 at 0x7e61ea, 0x7e6250, 0x7e62aa), and `free_game_data` clears the pointer and count (0x82ff97, 0x82ff9d) after it frees the array (0x82ff8f). So the table is filled from the entry arrays, the friendly object and the reserve competition.

Who reads it: 627 instructions load [0xadadfc]. Almost all take `table[key id]`. The loops over the whole table (count [0xadadf8]) are: the daily update 0x5ce060, the run to the start date (0x5cc98e, 0x5ccaa9), 0x5cd2fe (vtable +0x14), `Club_History::get_cup_history` (0x4435b0, 0x443697, null-checked per the club world doc), 0x79c18d and 0x859dcc (both call 0x4b0700 with the index). The season pass follows the chain +0x20 of a competition through the table (0x5d5d00..0x5d5d15) for attendances.

An object of a record with no key id global:

1. No `table[key id]` reader finds it, because no key id holds its record id. So code of the original that reads a fixed competition does not see it.
2. The loops include it: it gets `update_for_date` every day (0x5ce088). This is what it needs to play.
3. Its field +0x20 must be -1 or a valid record id, because the season pass reads `table[+0x20]` (0x5d5d00..0x5d5d10). Inference: the Comp base constructor 0x48c580 sets it. Not read.
4. Its record id must be below [0xae2378], or step 3 of 0x8386f0 writes past the array. Inference from 0x60425d (the name matcher runs over `[0xae2378] - 0x7f` records) and 0x5ca050 (0x7f friendly slots): the last 0x7f club_comp records belong to the friendly object. A new record must not use one of those indices. Where the runtime gets the record (a record of the database, or a record it adds) is outside this research. Unknown.

## 5. A new object of an existing class (question 4)

Both base constructors are plain thiscall with no arguments and call the Comp base 0x48c580: `Cup::Cup` 0x518640 (vtable 0x9693c8, +0xab, +0xac = 0, +0x43 = 1) and `League::League` 0x682200 (vtable 0x96d020, many fields, +0xd9 = 3, +0xc = 0, +0xa3 = 0). Neither reads a global.

| Class | new size | Base | Constructor | From the record argument | From key ids or fixed lists |
|---|---|---|---|---|---|
| Conmebol_Merc | 0xf6 (0x831ad2 reuses the size of 0x831a7a) | League 0x682200 (0x4c46e3) | 0x4c46c0, `ret 8` (0x4c483f) | +4 (0x4c46f9), the file name, the history lookups through +4 (0x4c5973, 0x4c59c2, 0x4c5a22 in 0x4c5850) | The entrants: `ConmebolSeeding::is_mercosur_invited_club` 0x4c6890 (called at 0x4c5906) compares each club id with 20 fixed club key ids (0x9d00ac..0x9d027c, 0x4c689d..0x4c6947). The class code 0x4c46c0..0x4c5f00 reads no competition key id |
| Conmebol_Liber | 0xf6 (0x831a7a) | League | 0x4c2f50 (called at 0x831ac3) | The entrants: `select_qualified_clubs` 0x4c3f40 takes the clubs whose +0x1db equals its own record id `[[this + 4]]` (0x4c3fae..0x4c3fca) | `[0xadadfc][0x9cf6e4]` for the Intercontinental Cup (0x4c41a0, club world doc 4.2) |
| World_Club_Cup | 0xb2 (0x831235 and on) | Cup 0x518640 (0x92b4d3) | 0x92b4b0, `ret 8` (0x92b678) | +4 (0x92b4ed), the file name | `pick_qualifiers` 0x92b8b0 (0x92b53e): the Champions League key id 0x9cf6e8 (0x92ba42, 0x92bbd5) and the Libertadores key id 0x9cf63c (0x92bb71, 0x92bc67, 0x92bd21) |

The sizes of the other classes are in `2026-10-16-sweep-europe.md` section 2.1 and agree with the creators: 0xf6 (0x83116f, 0x831a7a, 0x832034), 0xb2 (0x831235, 0x831468, 0x8317e4), 0xba (0x831d29). A creator loads the size once and reuses the register for the next objects, so the size push is only before the first `new` of each size.

What a constructor sets before its load (and the load overwrites): +4, +0x40 and fixed bytes. Conmebol_Merc: +0x50 = 2, +0x51 = 0xf, +0xee = 0 (0x4c4711..0x4c471b). World_Club_Cup: +0xab, +0x44, +0x2c, +0x30, +0x42, +0x52, +0x49, +0x4a, +0x50, +0xb1 (0x92b503..0x92b529). Without a file it then builds from the record and the key ids: Conmebol_Merc 5 stage slots (0x4c4740), `set_up_rules` 0x4c4f50 (start date and prize fund, no key id), Comp_Stats 0x49ee70. World_Club_Cup the picker 0x92b8b0, round table vtable +0x3c (0x92b586), `Cup::map_fixture_tree` 0x518790, Comp_Stats, `Cup::rank_teams` 0x5223a0.

So:

1. A second Conmebol_Merc object would invite the same 20 fixed clubs.
2. A second Conmebol_Liber object takes the clubs that someone marks with its own record id in club +0x1db. This is the only class here whose entrants follow its record.
3. A second World_Club_Cup object picks the same two winners.

## 6. Club fields (question 5)

Club +0x1db (4 bytes) is a club_comp record id: the club's continental competition of the season. Club +0x1df (1 byte) is its entry round (sweep-europe 3.4). Writers: game creation sets -1 (0x828843), the UEFA seeding (0x904135 and on, 0x90430e..0x905047), the CONMEBOL seeding (-1 for South American clubs at 0x4c6377, then the Libertadores record at 0x4c647a..0x4c66f6 in `assign_libertadores_places` 0x4c6430, from key id 0x9cf63c), the European cups (0x584c05..0x586625, 0x633c6c, 0x633d20, 0x902d42, 0x9030f4). Readers outside these files exist (0x56cd88.., 0x5c3273.., 0x5f49b9.., 0x69a139.., 0x7f04d7.., 0x83cd46, 0x845572.., 0x84d989.., 0x85aa5c). What they do was not read.

Does a new competition need it?

1. For a Conmebol_Liber-class object: yes. Its entrants are the clubs with +0x1db equal to its record id (0x4c3fb7). The runtime must write the field, after the CONMEBOL seeding has written it for the season and before the draw. A club holds one value, so a club in the Libertadores cannot also be marked for the new competition in the same season (from the field size).
2. For a Conmebol_Merc-class or World_Club_Cup-class object: no. They do not read it.
3. Any other club field that names a competition id: unknown. The club layout of `2026-10-13-database-inventory.md` 3.4 has no other field with a known meaning of this kind. Club +0x53 is read as a nation pointer by the CONMEBOL seeding (0x4c634f..0x4c636b), so it is not a competition id.

## 7. Unknowns

- Whether `key_nation_build_table` 0x667150 clears +0xc and +0x10 of the entries. The creators overwrite both when they make objects. The value of +0xc on the branch with no selected nation (0x831b4b) was not read.
- Whether the extraction folder of 0x933320 (esp + 0x5ec at 0x82acae) is CM3_TEMP. Inference: yes, because the constructors open `CM3_TEMP\<name>` and the original's own objects load.
- What the Comp base 0x48c580 sets at +0x20.
- What the readers of club +0x1db in section 6 do.
- How the runtime gets a club_comp record for a new competition, and whether its index can clash with the 0x7f friendly slots.
- The loop at 0x82f58f..0x82f59a in `save_game` (counter to 2, back to 0x82e6ec) was not read.
