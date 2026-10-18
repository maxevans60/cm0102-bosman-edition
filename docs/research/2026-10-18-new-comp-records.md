# New competition records (0.6.0 slice 26)

Date 2026-10-18. Research only. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Method: capstone disassembly of the clean exe (the `cm0102` venv), byte scans of `.text` for calls and immediates, and reads of the `.dat` files of the shipped data and of 45 Update Team databases in `~/Library/Application Support/cm0102/Databases`. "Inference" marks a statement that the code does not prove.

The question: the module of `season_comps.h` must add a club competition record (0x6b bytes) that the database lacks, for example "UEFA Europa Conference League", so that a new competition object has its own name, record id, history, [0xadadfc] slot and save file `<name>_<id>.tmp`.

Terms in this document: C is the count of club_comp.dat records, N the count of nation_comp.dat records, F = 0x7f the friendly records, k the new records.

## Recommended design

1. **Place.** Put the k new records after the last nation comp and before the friendly records, at indices C + N to C + N + k - 1. No index that game data holds moves. Only the friendly records move up by k, and they are blank at a new game. The May 2026 Update Team database has the same form in its files: 4 nation comps more (Gold Cup 453 to CONCACAF Nations League 456) after the 19 of 3.9.68.
2. **Counts.** Add k to [0xae2378] and to [0xae2380]. The nation count matters only to the loader, the writer and the free of Database.cpp, so the database writer then saves the new records with the nation comps.
3. **Time.** Only at the database load of a new game (0x524d50), in a wrap of the id pass 0x5315a0, before the original runs. At that time the records still hold ids, and no pointer into the array exists. Never at the load of a save: the save holds the new records itself (section 2.6).
4. **Memory.** The array lies inside the one block at [0xae23a4], so it cannot grow in place. Allocate (C + N + k + F) * 0x6b bytes with the game's malloc 0x944e46, copy the C + N records, write the new records, write the friendly records again at their new indices, and set [0xae23d0], [0xae23d8], [0xae2378] and [0xae2380]. The free 0x528080 does not free this array (it only clears the pointer, 0x528234), so the runtime frees it with free 0x9452ca in a wrap of 0x528080.
5. **Hide the new records from three functions**: set_key_ids 0x601a40, database_load_language_names 0x5339a0 and eng_lng_load_tables 0x544fc0. They check or use `[0xae2378] - 0x7f` as C + N (section 2.4). A wrap subtracts k from [0xae2378] for the call and adds it back after.
6. **The season part** keeps k and the index and data name of each new record, so that a load knows k before the loader runs (season.c reads the part before 0x82a9c0) and the module finds its records.
7. **The record** (section 4): id = index, long name, short name, abbreviation "", bytes +0x57 and +0x58 = 0, the continent id at +0x59, the nation id at +0x5d (-2 for none), the colour ids at +0x61 and +0x65, the level at +0x69. Write ids, not pointers: the id pass then makes the pointers.

## 1. The array at [0xae23d0]

### 1.1 Who loads it

`database_load` 0x528540 is cdecl (path, mode, stream). It has two callers, `database_load_files` 0x524d50 (stream 0) and `database_load_stream` 0x524d10 (stream 1, the save at 0x82abf2). All five call sites pass mode 1: 0x81ce89, 0x82742d, 0x8275d0, 0x82828b and 0x82abef. Mode 1 is the "shared" mode, kept in [0xae2340] (0x5289bc). So the branch at 0x5289c1 always goes to the sizing pass at 0x52990e. The other branch (0x5289c7, one allocation per table, 0x528c01 club comps and 0x528c37 nation comps) is unreachable, and so is its renumber loop of the nation comp ids at 0x52d1b4..0x52d1d8.

The sizing pass walks the 0x16 entries of index.dat (0x43 bytes each from 0xae1a80, count at +0x37, loop end at 0x529cba):

1. club_comp.dat: `[0xae2378] += count + 0x7f` (0x529a8a..0x529a97). The size grows by (count + 0x7f) * 0x6b (0x529a94..0x529aa8).
2. nation_comp.dat: `[0xae2378] += count` and `[0xae2380] += count` (0x529ab1..0x529aca).

So [0xae2378] = C + N + F, and [0xae2380] = N.

### 1.2 Size and allocation

One block holds most tables: `GlobalAlloc(0, size)` in mode 1 (import 0x9670f0, call at 0x529ddc), into [0xae23a4] (0x529ded). The layout code 0x529ef2..0x52a01b gives each table its place in that block:

| Global | Value | Evidence |
|---|---|---|
| [0xae23d0] | the start of the comp records, after the staff comps | 0x529f6f..0x529f82 |
| [0xae23d8] | [0xae23d0] + (count - N - 0x7f) * 0x6b = [0xae23d0] + C * 0x6b | 0x529f88..0x529fba |
| [0xae23ec] | the colour table, right after the last friendly record | 0x529fb8..0x529fc3 |

So the order in memory is: C club comps, then N nation comps, then F friendly records, in one array of 0x6b-byte records. The history table [0xae23d4] is a separate malloc (0x529e57..0x529e62) and the free uses free for it (0x528118), which is why 0x527f60 can grow it with realloc.

After the allocation the loader writes the F friendly records as blank records, from index count - 1 down to count - 0x7f (loop 0x52a060..0x52a1ab):

| Field | Value | Evidence |
|---|---|---|
| +0x00 | the index | 0x52a081 |
| +0x04, +0x38, +0x53, +0x57, +0x58 | byte 0 | 0x52a09d, 0x52a0ba, 0x52a0d7, 0x52a0f4, 0x52a111 |
| +0x59, +0x5d, +0x61, +0x65 | -1 | 0x52a12e..0x52a185 |
| +0x69 | word 0 | 0x52a1a6 |

Then the reader 0x54feb0 reads club_comp.dat into [0xae23d0] (0x52bb05) and nation_comp.dat into [0xae23d8] (0x52bbc4). When the index header field [0xae2344] is above 0, it reads [0xae2344] more records from the nation_comp part into the friendly records at index [0xae2378] - 0x7f (0x52bbd4..0x52bc46). In the shipped index.dat the header is 8 zero bytes, so a new game reads no friendly records. At the end of the reads, mode 1 sets [0xae2344] = 0x7f (0x52c687). Then come the id pass 0x5315a0 (0x52d1aa), the staff pass, and set_key_ids 0x601a40 (0x52d8fa). All of it happens inside 0x528540, at a new game and at the load of a save.

### 1.3 The counts

| Global | Value | 3.9.68 |
|---|---|---|
| [0xae2378] | C + N + F | 433 + 19 + 127 = 579 |
| [0xae2380] | N | 19 |
| [0xae2344] | F after a load (0x52c687), the friendly records that the save holds | 127 |

### 1.4 What a record holds

The fields come from the blank records (1.2), the friendly writer 0x5c5bf4..0x5c5cc8, the id pass 0x532d16..0x532de1 and the data:

| Offset | Size | Meaning | Evidence |
|---|---|---|---|
| +0x00 | int | record id | id pass reads it as the index, 0x532c25 (history), 0x5315d6.. (clubs); 0x8387a1 |
| +0x04 | char[0x34] | long name, 50 characters and the 0 at most (season_change.c) | friendly 0x5c5c28; matcher 0x604258 |
| +0x38 | char[0x1b] | short name | friendly 0x5c5c4e |
| +0x53 | char[4] | abbreviation ("D2", "PRM", "Fr"; "" for the continental cups) | friendly 0x5c5c64 copies "Fr" (0x9cd0f0) |
| +0x57 | byte | unknown. In the data the bytes +0x57 and +0x58 cycle through 186, 13, 173, 240 from record to record, which looks like editor memory (inference) | blank 0 at 0x52a0f4 |
| +0x58 | byte | unknown, as +0x57 | blank 0 at 0x52a111 |
| +0x59 | int then pointer | continent: id in the file, pointer into [0xae23a4] (stride 0xc6) after the id pass. Below 0 gives 0 | 0x532d50..0x532d77 |
| +0x5d | int then pointer | nation: pointer into [0xae23a8] (stride 0x122) | 0x532d27..0x532d4c |
| +0x61 | int then pointer | colour: pointer into [0xae23ec] (stride 0x3a) | 0x532d7b..0x532da4 |
| +0x65 | int then pointer | colour: as +0x61 | 0x532da8..0x532dd1 |
| +0x69 | s16 | level of the competition: Premier Division 18, UEFA Cup 18, European Champions Cup 20, World Cup 20, Belgian Fourth Division 1, a friendly 0 | compared at 0x79d447 and 0x79d457 |

The database writer turns the four pointers back into ids (`[pointer]`, or -1 for none) before it writes (0x52db12..0x52db8f), and calls the id pass again after the write.

Inference on +0x61 and +0x65: they are the colours of the competition. The friendly copies the colour fields of its host club (club +0x83 and +0x87, 0x5c5caf..0x5c5cc8) into them.

### 1.5 Is record +0 always the index?

In the data, yes. In the shipped 3.9.68 data every club_comp record has id = index (0 to 432), and every nation_comp record has id = C + index (433 to 451). The same holds in 3.9.60 (C = 390), 3.9.65 (C = 431), the two own databases (Summer 2004, Summer 2006) and all 45 Update Team databases on disk (C = 433 or 434, N = 19, May 2026 N = 23). Script check of the files, 2026-10-18.

The code does not renumber in mode 1. It relies on the equality:

1. The id pass makes a pointer as `[0xae23d0] + id * 0x6b` from a raw id (history 0x532c25, club +0x57 0x531635, +0x5b 0x53168c, +0x60 0x5316c4).
2. set_key_ids stores the **index** in each key id global (0x60425d passes esi, the loop counter; the matcher stores it, for example 0x60efec). Code then reads `[0xadadfc][key id]`. `build_comp_object_table` 0x8386f0 writes each object at its **record +0** (0x8387a1..0x8387a6). The two agree only when id = index.
3. The loader and the friendly write id = index for the friendly records (0x52a081, 0x5c5ca3).

So a new record must have id = its index.

### 1.6 The last 0x7f records belong to the friendly object: proved

1. The loader reserves them after the nation comps (1.1, 1.2) and blanks them (0x52a060..0x52a1ab).
2. `Friendly::Friendly` 0x5c1f80 sets the word +0xaf to `[0xae2378] - 0x7f` (0x5c2105..0x5c210d), the index of the first friendly record. It has 0x7f slots of 0x69 bytes from +0xb1 (array constructor at 0x5c1fdb with 0x7f and 0x69).
3. A friendly tournament writes its own record at index +0xaf + slot (slot byte at tournament +0x64): id, long name, short name, "Fr", continent, nation and two colours (0x5c5bf4..0x5c5cc8).
4. `0x5ca030` (called by 0x8386f0 at 0x8387c6) writes the friendly object into [0xadadfc] at key id 0x9cf668 (the record "Friendly", index 72) and each tournament object of a used slot at index +0xaf + slot (0x5ca050..0x5ca07a).
5. `0x5c29b0` and `0x5c6710` treat a record with id below `[0xae2378] - 0x7f` as a real competition and a record at or above it as a friendly slot (0x5c29d6..0x5c29e3, 0x5c6723..0x5c672c).
6. The matcher loop of set_key_ids runs over `[0xae2378] - 0x7f` records (0x604241..0x604273), so it never sees the friendly records.

## 2. Readers of the count and of record indices

### 2.1 set_key_ids 0x601a40

1. The nation names first (0x604220..0x60423c), then the matcher `match_club_comp_key_id` 0x60efd0 for each record below `[0xae2378] - 0x7f` (0x604241..0x604273), with the index as its second argument. So it matches club comps and nation comps alike.
2. When the data language [0x9b5984] is not 0, it first opens eng.lng (0x603c52..0x603c82) to match the English names. Inference: it reads them with `eng_lng_load_tables` 0x544fc0, which checks the counts of the file against `[0xae2378] - 0x7f` (0x5451c1..0x5451d4).
3. With a new record that the matcher can see, a name equal to one of the 351 fixed strings moves that key id to the new record. With [0xae2378] lowered by k for the call (design point 5), the matcher sees what the original sees.

### 2.2 build_comp_object_table 0x8386f0

`[0xadadf8] = [0xae2378]` (0x8386f0..0x8386fb), `calloc(count * 4, 1)` (0x838706), then `table[record +0] = object` for each object of each entry array, with no bounds check (0x838788..0x8387be), then 0x5ca030 (1.6 point 4). A new record at index C + N + j < count gets its slot with no change.

### 2.3 The friendly code

1.6 above. All friendly code finds its records from +0xaf, which the constructor takes from the count at the time the game is made (0x5c210d), after the database load. With the design, count - 0x7f is still the first friendly record.

### 2.4 The language names

`database_load_language_names` 0x5339a0 (callers `Config::Config` 0x5d7694 and 0x5d85bd) returns at once when the asked language equals the data language [0x9b5984] (0x5339d5..0x5339dd). The loader sets [0x9b5984] to 0 when index.dat has no language block (0x528a30), as for every database folder. Else it reads the .lng of the game's Data and:

1. checks `lng club comps + lng nation comps == [0xae2378] - 0x7f` (0x533e94..0x533ea1), with the other counts. A mismatch gives the error box with line 0x1e68 (0x53585d..0x5358b7) and writes no name;
2. writes club comp name i at index i (0x53556f..0x5355bc);
3. writes nation comp name i at index `i - N + [0xae2378] - 0x7f` (0x534aed..0x534aff, and 0x534c3a..0x534c46 when the buffer is not allocated). So it puts the nation comps right before the friendly records.

With design point 5 (count lowered by k), the check passes and the nation names land on C to C + N - 1, as in the original. Without it, the check fails.

Context: eng.lng holds 433 club comps and 19 nation comps (header ints 0x1b1 and 0x13). The database folders hold no .lng. So inference: for every Update Team database with C = 434 a language change already fails this check today.

### 2.5 The comp history tables

The rows of club_comp_history.dat and nation_comp_history.dat hold a record id at +4. In 3.9.68 the club rows hold 0 to 432 and the nation rows 433 to 451, so the nation rows use the index in the whole array, not the index in nation_comp.dat. The id pass makes the pointer `[0xae23d0] + id * 0x6b` (0x532c11..0x532c38). In April 2021 (C = 434) the nation rows hold 436 to 452: the database makers renumbered them when they added a club comp.

Other data that holds raw comp ids: club +0x57, +0x5b and +0x60 (id pass 0x53161f..0x5316d6). In 3.9.68 club.dat they hold only ids below 433, and nat_club.dat holds none. A struct with a comp id at +0xc that `officials_manager_compare_comp_level` 0x79d410 reads as an index (0x79d419, 0x79d435). Which struct this is: unknown (officials.dat +0xc does not hold comp ids).

### 2.6 Save and load of the data

The .sav holds the whole database:

1. `save_game` 0x82e430 calls 0x524d30 (at 0x82e6d6), which calls the writer `0x52dad0(ctx, 1, ...)` (0x524d41).
2. The writer writes the part `index.dat` (string 0x9b5c48): [0xae2344], [0xae2348] and the 0x16 entries with the counts of now (0x531336..0x53136b).
3. The part `club_comp.dat`: `[0xae2378] - [0xae2344] - [0xae2380]` records from [0xae23d0], and that count into its entry (0x52f973..0x52f9af).
4. The part `nation_comp.dat`: [0xae2380] records from [0xae23d8] (0x52fb65..0x52fb96), then, when [0xae2344] > 0, the [0xae2344] friendly records from index `[0xae2378] - [0xae2344]` in the same part (0x52fbbd..0x52fbe5).
5. The load reads them back through 0x524d10 into 0x528540 (1.2), before the id pass and before set_key_ids.

So a record added in memory is saved and comes back at a load, before set_key_ids, if the counts say where it is. With the design ([0xae2380] + k), point 3 writes C records, point 4 writes N + k records and then the friendly records. At the load the sizing gives the same layout, and every id equals its index again.

### 2.7 Code that loops over all records

| Function | Use | Evidence |
|---|---|---|
| `verify_background_config` 0x41bc20 | finds a name of a config file in all records | 0x41bd80..0x41bdba |
| `verify_history_files` 0x5f2840 | the same | 0x5f29d6..0x5f2a19 |
| `Club_History::get_league_comp_ids` 0x443770, `get_cup_comp_ids` 0x443a20 | arrays of count entries | 0x4437a3, 0x443a53, 0x4438a9 |
| bounds checks | `id >= 0 && id <= count` or `< count` | 0x447625, 0x59cd29, 0x7645e9, 0x7836dd, 0x79d423, 0x7aebac, 0x84a396, 0x88c76e |
| loops over [0xadadf8] | daily update and others | load doc section 4 |

None of them splits club comps from nation comps by index. The nation globals [0xae23d8] and [0xae2380] have readers only in Database.cpp: the free 0x528080, the loader 0x528540 and the writer 0x52dad0 (every immediate reference lies between 0x52823c and 0x52fb6d). A screen that lists all competitions by record index: none found among these readers.

## 3. Where a new record can go

| Place | What moves | Verdict |
|---|---|---|
| After the last club comp (index C) | the N nation comps. Their ids at +0, the nation history rows (ids 433 to 451), any club field or other data with a nation comp id, and the nation comp ids of the slice 11b date table (`last_ids`, "club comps, then nation comps") would all need + k. The language pass also fails: the count check, and lowering the count puts the nation names on the wrong records (2.4 point 3) | Possible (April 2021 does it in its files), but many holders of an index |
| After the last nation comp, before the friendly records | only the friendly records, which are blank at a new game | **Safest.** May 2026 does it in its files |
| At the end of the whole array, after the friendly records | the friendly block would no longer end at the count: the friendly start `count - 0x7f` (0x5c210d) and the slot tests (0x5c29de) then point one record too high | No |
| In place of a record that no game data uses | nothing | Version 1 of the module. Only where such a record exists (design doc table) and its history may mix |

### Steps (new game)

1. In the wrap of `database_load_files` 0x524d50 (season_dates.c), set a flag "new game data load" around the original call. The wrap of 0x5315a0 acts only when the flag is set and only once per load. Reason: the writer 0x52dad0 also calls 0x5315a0 at each save (0x52e81a, 0x52e88e, 0x531321, 0x5313eb, 0x531526), and the save load calls it through 0x524d10.
2. Decide k: the defs with no spare record that the game can hold (season game only, Q152, Q232). The base game makes none.
3. In the wrap of 0x5315a0, before the original: read C + N = [0xae2378] - 0x7f and N = [0xae2380]. Allocate the new array with 0x944e46 (rt_callback). Copy records 0 to C + N - 1. Write the k new records. Write the F friendly records at C + N + k onwards as the loader does (1.2 table, id = new index). Set [0xae23d0] = new, [0xae23d8] = new + C * 0x6b, [0xae2378] += k, [0xae2380] += k. Keep the address for the free.
4. Call the original id pass. It turns the ids of the new records into pointers with the others.
5. set_key_ids runs next (0x52d8fa), in its wrap with the count lowered by k.
6. The friendly object is made later, at the creation of the game, with +0xaf = C + N + k.

Inference: no code makes a pointer into the comp array between the reads and the id pass. Every reference to [0xae23d0] in Database.cpp is listed (0x528236 to 0x532daa), and the ones before 0x52d1aa are the layout, the blank records and the reads.

### Steps (load of a save)

1. Nothing to grow: the save holds the records (2.6). The load frees the old tables first (0x82abea calls 0x528080), so the wrap of 0x528080 frees the old new-game array.
2. The season part gives k before the load, for the wraps of 0x601a40, 0x5339a0 and 0x544fc0.
3. Do not grow at a load. The friendly records then hold live tournaments: their ids, their history rows, their object files `<name>_<id>.tmp` and the friendly +0xaf would all move. A def that needs a new record and finds none at a load gets no object, with a log line.

### Start screen

0x81ce00 loads the data only when no nation is loaded and keeps it after Back. The `start_new_game` wrap (season_dates.c) frees the tables when the season or the folder changed. It must also free them when k changed (for example the changing rules switch on Setup Game), so that the next Start New Game loads with the right records. The create path 0x828110 loads the data itself only when none is loaded (0x82825c, call 0x8282a7).

## 4. What the runtime writes in a new record

Write before the id pass, so the four reference fields hold ids:

| Field | Value | Why |
|---|---|---|
| +0x00 | its index, C + N + j | 1.5 |
| +0x04 | the long name of the data, at most 50 characters and the 0, the rest 0 | the matcher, the screens, the save file name (load doc 2.2) |
| +0x38 | the short name, at most 26 characters and the 0 | screens (friendly 0x5c5c4e writes it) |
| +0x53 | "" (as the continental cups) or a code of 3 characters | friendly 0x5c5c64 |
| +0x57, +0x58 | 0 | as the blank records (0x52a0f4, 0x52a111). Meaning unknown |
| +0x59 | the continent id (2 for Europe as UEFA Cup 328, 5 for South America as Copa Libertadores 58), or -2 | id pass 0x532d64 |
| +0x5d | the nation id for a national competition (for example 3. Liga), -2 for a continental one | id pass 0x532d3b |
| +0x61, +0x65 | colour ids, for example those of the record that the competition follows (UEFA Cup 17, 16), or -2 | id pass 0x532d8f, 0x532dbc |
| +0x69 | a level below that of the senior cup (UEFA Cup 18, Inter-Toto Cup 8) | 0x79d447. The other readers of +0x69: unknown |

Inference: copying +0x59 to +0x69 from the record of the competition that the new one follows (UEFA Cup for the Conference League) is the simplest rule, with a lower +0x69.

Slice 11c note: `season_history_fill` finds a competition only through history rows that already point at it (`index_comps`), so a new record gets no rows from the history table until the fill can also find a record by name.

## 5. Unknowns

- The meaning of the bytes +0x57 and +0x58, and every reader of +0x57, +0x58 and +0x69 other than 0x79d410.
- The struct that 0x79d410 reads with a comp id at +0xc.
- Whether the Bosman Edition lets the player choose a language other than that of the data. If not, 0x5339a0 never passes 0x5339dd and its wrap is only for safety.
- Whether `eng_lng_load_tables` 0x544fc0 places the nation names as 0x5339a0 does. Only its count check (0x5451c1..0x5451d4) was read.
- Whether the friendly load 0x5c8a00 reads +0xaf from its file. It does not matter for the design (no grow at a load), but it does for any grow at a load.
- Whether the runtime's wrap of 0x5315a0 sees the call from inside the recompiled 0x528540. Inference: yes, as other wraps of called functions work (for example 0x8386f0 in the load doc).
