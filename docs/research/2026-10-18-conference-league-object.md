# The Conference League as a new competition object (0.6.0 slice 24)

Date 2026-10-18. Research only. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Evidence: objdump of the clean exe (Intel syntax) and REA with Ghidra for function bounds. Data: `clean-3968/Data/club_comp.dat`, `nation_comp.dat`, `index.dat`. "Inference" marks a statement that the code does not prove. Words as in `GLOSSARY.md`.

Max's decision (Q364): the Conference League gets a new competition record. The Cup Winners' Cup record (key id 0x9cf6ec) keeps its name and history. This doc answers "Open" point 1 of `2026-10-18-new-comps.md` (slice 26, branch `claude/slice-26-d5d47b`) for the Conference League, and gives a design that the shared module can take for any def with `record` NULL.

## Summary for the plan

1. The record array is one block of the database. Its layout is fixed: club comps, then nation comps, then 127 (0x7f) blank records for the friendly object. The nation comp records carry their array index as their id in the data (433 to 451 in 3.9.68). So a new record cannot go after the club comps without a change of every nation comp id. It also cannot go after the friendly records, because the friendly code finds them as "the last 0x7f". The one safe place is between the nation comps and the friendly records.
2. The block is made, filled and linked inside `load_database` 0x528540, both at a new game and at a load. `set_key_ids` runs inside it too (0x52d8fa). A record added after it returns would need a new block and a fix of every pointer into the old one. So the runtime makes room inside `load_database` with constant sites (the `PATCHES` mechanism of `tools/recomp/uefa.py`), not with a later copy.
3. The save holds the record array (`save_database` 0x52dad0 from `save_game` 0x82e430 at 0x82e6d6), and a load reads it from the save, not from the data (`load_database` with source 1 at 0x82abf2). The save writes the friendly records as an "extra" part whose count is the header word [0xae2344]. A wrap of `save_database` that sets [0xae2344] to 0x7f + K for the call saves the K new records with the friendly records, in the right order. A load then reads K back from the header.
4. A second object of the Uefa_Cup class needs no vtable copy. Inside the class only `build_entrants` 0x902b90 reads the key id 0x9cf6f0, and it is a direct call (0x90237e, 0x902fb3). The constructor writes the vtable at 0x90232d before every call it makes, so a vtable copy written after the constructor cannot change what the constructor does. The runtime wraps that act on "the UEFA Cup" test the vtable 0x970790 (`is_uc`, slices 22 and 23). They must test the record id instead, so that the Europa League and the Conference League each get their own rules.
5. The fifth object goes into the Europe array with `season_change_add_comp`. No code reads entry 3's array by a fixed address. The Intertoto slot is not needed, and the Intertoto object must stay (rule of `season_comps.h`).

## A. A new competition record

### A.1 The array and its count

| Global | Meaning | Evidence |
|---|---|---|
| [0xae23d0] | the record array (0x6b bytes a record) | carved at 0x529f82 |
| [0xae2378] | the count of all records: club comps N, nation comps M and 0x7f friendly records | 0x529a88..0x529a97 (club comp entry adds n + 0x7f), 0x529aaf..0x529abf (nation comp entry adds n, also to [0xae2380]) |
| [0xae23d8] | the first nation comp record | 0x529f88..0x529fba: start + (count - (M + 0x7f)) * 0x6b |
| [0xae2380] | M, the nation comp count | 0x529aca |
| [0xae2344] | a header word of `index.dat`: the count of the "extra" records in the file (0 in the data, 0x7f in a save) | read at 0x52873b (4 bytes), set to 0x7f at 0x52c687 after a load with allocMode 1, written by the save at 0x53133d |

In 3.9.68 `club_comp.dat` has 433 records with ids 0 to 432, and `nation_comp.dat` has 19 records with ids 433 to 451 (read from the files). `index.dat` starts with two zero words ([0xae2344], [0xae2348]).

### A.2 Where the array is allocated and filled

Both callers use allocMode 1: `create_game` 0x828110 calls `load_database(path, 1, 0)` through 0x524d50 at 0x8282a7 (source 0: the files), `load_game_data` 0x82a9c0 calls `load_database(save, 1, 1)` through 0x524d10 at 0x82abf2 (source 1: the save stream, 0x932c30). allocMode is kept in [0xae2340] (0x5289bc).

In `load_database` 0x528540 (0x528540..0x52d97b), with allocMode 1:

1. 0x52873b: the header word [0xae2344] is read from `index.dat` (file or save).
2. The size pass (switch from 0x52990e): the club comp entry adds n + 0x7f records (0x529a90 `lea edx, [ecx+eax+0x7f]`, 0x529a94 `add eax, 0x7f`). All tables are then carved from one block (0x529f02..0x52a046): club comps at [0xae23d0] (0x529f82), nation comps at [0xae23d8] (0x529f8e `lea esi, [edx+0x7f]`, 0x529fba).
3. The blank loop 0x52a060..0x52a1ab: records count - 1 down to count - 0x7f get id = index (0x52a081), empty names (+0x04, +0x38, +0x53, +0x57, +0x58 cleared), -1 at +0x59, +0x5d, +0x61, +0x65 and 0 at +0x69. The loop end is `cmp eax, 0x7f` at 0x52a1a3.
4. The readers: club comps by 0x54feb0 (raw 0x6b-byte records, the id from the file). The nation comps at [0xae23d8] (0x52bbad..0x52bbc4). Then, when [0xae2344] > 0 (0x52bbd4..0x52bbdb), a second read of [0xae2344] records into index count - 0x7f (0x52bc15 `lea eax, [edx-0x7f]`, read at 0x52bc46). This is the friendly part of a save.
5. 0x52c687: [0xae2344] = 0x7f.
6. The id pass `resolve_database_indexes` 0x5315a0 at 0x52d1aa links +0x59 continent, +0x5d nation and the colours of every record up to [0xae2378] (port: `database_links.c`, loop 0x532d27, bound ANY), and the comp history +4 to its record.
7. `set_key_ids` 0x601a40 at 0x52d8fa. The club comp name matcher (loop 0x604241..0x604271, `match_club_comp_key_id` 0x60efd0) runs over [0xae2378] - 0x7f records, so over club and nation comps.

### A.3 What reads the count and the index

| Reader | What it does with the count | Effect of K new records before the friendly records |
|---|---|---|
| `build_comp_object_table` 0x8386f0 | [0xadadf8] = [0xae2378], `calloc(count * 4, 1)` into [0xadadfc] (0x8386f0..0x83870e), `table[record id] = object` with no bound test (0x838788..0x8387be) | The table covers the new records with no change |
| Friendly constructor 0x5c210a | friendly +0xaf = count - 0x7f, the first friendly record | Stays right: the friendly records are still the last 0x7f |
| 0x5c29de, 0x5c672c (`is friendly record`, used by `Club_History::record_comp_season` at 0x442f44 and `Cup::update_staff_records_for_status` at 0x51991a) | a record id >= count - 0x7f is a friendly one | A new record is below that, so it is a real competition |
| Name matcher 0x604248, 0x60426b | loops count - 0x7f records | Covers the new records. A name that is none of the 351 strings sets no key id |
| `database_load_language_names` 0x5339a0 (from `Config::Config` 0x5d7694 and `Config::apply_and_save` 0x5d85bd) | checks club comps + nation comps of the language file == count - 0x7f (0x533e96..0x533ea1, else the box "Database.cpp" line 0x1e68 at 0x535890), then writes name i at index i - n + count - 0x7f (0x534aff, 0x534c46) | Breaks: the check fails, and without the check the names would land K records late. Needs constant sites |
| `eng_lng_load_tables` 0x544fc0 (lazy, from `nation_get_english_name` 0x54545c and the other English name getters) | the same check (0x5451c1..0x5451d4, else the box line 0x4df1 at 0x5453a4) | Breaks the same way. Needs a constant site |
| `comp_get_english_name` 0x5455a0 | in a language mode [0x9b5984] other than 0 and 1, reads the name of the English table by record id (0x545622..0x54563e), no upper bound | Reads past the table for a new record. Needs a wrap that returns the record's own name. Which builds use mode 2 or more: unknown |
| `verify_history_files` 0x5f29d6, `verify_background_config` 0x41bd80 | search all records by name | Harmless |
| Bound tests 0x447625, 0x53162d, 0x59cd29, 0x7645e9, 0x7836dd, 0x79d423, 0x7aebac, 0x84a396, 0x88c76e | id below or at the count | A new id passes, as a real competition |
| `Club_History::get_league_comp_ids` 0x443770, `get_cup_comp_ids` 0x443a20 | arrays sized by [0xae2378] (0x4437a3, 0x443ad5) | Covered |

Key id globals hold record indices (the matcher stores the loop index). An index past the nation comps changes no key id. History rows (comp history +4) are pointers in the game, ids in the save, linked with no upper bound. A row for a new record is safe only while that record has the same index at every load. The design below keeps the index.

### A.4 The save holds the array

`save_game` 0x82e430 calls `save_database` 0x52dad0 at 0x82e6d6 (through 0x524d30). It converts the record pointers to ids in place for all [0xae2378] records (0x52db12..0x52db8f) and links them again after the write (`resolve_database_indexes` at 0x531321 and 0x5313eb). It writes:

1. the club comp part: [0xae2378] - [0xae2344] - [0xae2380] records from [0xae23d0] (0x52f973..0x52f9af), and that count into the part's index entry (0x52f9a8);
2. the nation comp part: [0xae2380] records from [0xae23d8] (0x52fb65..0x52fb96), then, in the same part, [0xae2344] records from index count - [0xae2344] (0x52fbbd..0x52fbe5);
3. the header words [0xae2344] and [0xae2348] (0x531336..0x531356).

So a record added to the array comes back at a load only when the save writes it in one of these parts, and the load puts it at the same index. With K new records and [0xae2344] = 0x7f the club comp part would hold N + K records (K nation comps among them) and the extra part would start K records late. Both are wrong. With [0xae2344] = 0x7f + K for the call, the club part holds N, and the extra part holds the K new records and then the 0x7f friendly records, from index N + M. The header then holds 0x7f + K.

### A.5 Design: K new records between the nation comps and the friendly records

Let K be the number of new records of the game. The new record j (0 to K - 1) has the index N + M + j = [0xae2378] - 0x7f - K + j. Its id (+0) is that index.

**K for a game.**

- Base game and `RECOMP_SEASON_CODE=off`: K = 0, so every site gives the original constant and the save is as before (Q152, Q342).
- A new season game: K = the number of defs of the shared module with `record` NULL (version 1: one, the Conference League). Reserve them in every season game, so that K does not depend on the start year or changing rules. The start year and changing rules are known before `create_game` (`season_set_new_game`, season.h).
- A load: K = header [0xae2344] - 0x7f when the header is above 0x7f, else 0. A save of an earlier build has K = 0, so that game has no Conference League (log line).
- The order of the defs with `record` NULL is append-only. A later build that adds a def gives it record K, so old saves keep their indices.

**Constant sites in `load_database`** (X = 0x7f + K; with K = 0 each is the original value):

| Site | Instruction | New value |
|---|---|---|
| 0x529a90 | `lea edx, [ecx+eax+0x7f]` (count) | X |
| 0x529a94 | `add eax, 0x7f` (bytes of the block) | X |
| 0x529f8e | `lea esi, [edx+0x7f]` (start of the nation comps) | X |
| 0x52a1a3 | `cmp eax, 0x7f` (the blank loop) | X |
| 0x52bc15 | `lea eax, [edx-0x7f]` (start of the extra read) | -X |

At a load the extra read reads X records (the header) into index count - X, so the saved new records come back at their index. At a new game the header is 0, the extra read does not run, and the blank loop writes the new records as blanks with id = index.

**Constant sites of the language files** (count - 0x7f becomes count - X, so the language names go to the first N + M records only): 0x533e9c `add edx, -0x7f`, 0x534aff `lea edi, [ecx+edx-0x7f]`, 0x534c46 `lea edi, [edx+eax-0x7f]`, 0x5451cf `add ecx, -0x7f`. Inference: `uefa.py` has only immediates of `sub` and `cmp`; whether the patch tool handles a `lea` displacement must be checked first.

**K at the sites.** The sites run before 0x52c687 sets [0xae2344] to 0x7f. The runtime takes X from the header at the first site (header >= 0x7f: X = header; else X = 0x7f + K of the new game) and keeps it for the game, because [0xae2344] reads 0x7f after the load.

**Save.** Wrap `save_database` 0x52dad0: in a game with K > 0, set [0xae2344] = 0x7f + K for the call, then set it back to 0x7f.

**The fields of the record.** At a new game the record is a blank (the blank loop) until the module makes the object. At that point the runtime copies +0x37 to +0x6a from the UEFA Cup record ([0xae23d0] + [0x9cf6f0] * 0x6b, already linked, so +0x59 continent and the colours are pointers) and writes the long name (+0x04, 51 bytes) and the short name (+0x38, 26 bytes) through `season_change_rename` (its bound test is "below [0xae2378]"). The id (+0) stays the index. The UEFA Cup record in 3.9.68 (index 328): +0x37 0xff, +0x52 0xff, +0x53..+0x56 0, +0x57 0xba, +0x58 0x0d, continent 2 (Europe), nation -2 (no nation), colours 17 and 16, +0x69 18. The meaning of +0x57, +0x58 and +0x69 is not known (the port's header says "level" for +0x69, an inference). Copy them as they are. The record then holds the European continent, which `WorldClubChamp::pick_entrant` and the menus use (inference). The next save writes it, the next load reads it.

**What breaks past the friendly records.** A record at index >= count - 0x7f is a friendly record to 0x5c29de and 0x5c672c, so `Club_History::record_comp_season` and the staff records treat it as a friendly. A record after the last 0x7f moves the friendly records: friendly +0xaf (0x5c210a) and the blank loop (0x52a068) then point at the wrong records, and the save's extra part starts one record late. So no record may go after the nation comps without the sites above, and none after the friendly records at all.

## B. A second object of the Uefa_Cup class

Constructor 0x9022e0, thiscall (year as a word at 0x902308, record pointer at 0x902310), `ret 8` at 0x9024bf. Size 0xb3 (operator new at 0x8314bf in `create_european_club_comps` 0x8312e0).

### B.1 What each function reads

| Function | Call | Fixed globals | Reads only `this`? |
|---|---|---|---|
| Constructor 0x9022e0 | direct | the seeding object [0xde1f58] (0x90236c) | Writes the vtable 0x970790 at 0x90232d before every call below. Calls load 0x51fc00 (direct, 0x90235f), `update_season` 0x9035a0 (0x902377), `build_entrants` (direct, 0x90237e), slot 15 through the original vtable (0x9023cd), `map_fixture_tree` 0x518790 (direct, 0x9023d8), Comp_Stats 0x49ee70 (0x90242e), `rank_teams` 0x5223a0 (direct, 0x9024a2) |
| `build_entrants` 0x902b90 | direct only (0x90237e, 0x902fb3) | **0x9cf6f0** (0x902c45): takes the clubs with +0x1db equal to it, entry byte +4 = club +0x1df (0x902c3f..0x902c9d); the club table [0xae23b4], [0xae235c], [0xae2350] for the fillers (0x902cb9..) | No: a second object takes the Europa League's clubs |
| slot 0, destructor 0x9024d0 | vtable | none | yes |
| slot 1, 0x633290 (Intertoto process_fixture) | vtable | the seeding [0xde1f58] for `add_nation_points` 0x9058b0 (0x6332ca, 0x6332df) | yes, apart from the coefficient points (inference: right for the Conference League, which counts for the coefficient) |
| slot 2, `start_new_season` 0x902ee0 | vtable | the seeding [0xde1f58] (0x902fa1) | Frees +0xa7, +0x14, +0xa3, +0xad, archives Comp_Stats (0x902f8c), year +1 (0x902f91), seeding (0x902fac), `build_entrants` (direct, 0x902fb3), slot 35 (0x902ff0), slot 37 (0x90302d), slot 23 (0x9030a0) |
| slots 8, 9, 0x903300, 0x903340 | vtable | none | yes (+0xb2) |
| slot 15, `create_round_table` 0x9025c0 | vtable | none (dates by 0x521e60, 0x521eb0) | yes |
| slot 17, 0x9030e0 | vtable | none itself | Sets club +0x1db = -1 for status 1, 2 and -1 (0x9030e8..0x9030f4), then `Cup::update_staff_records_for_status` 0x519890. That reads [0x9cf6f0] at 0x51997c only to write the UEFA Cup id into the per-club table [0xae2d14] when this cup is the key cup [0x9cf750] or a domestic cup (0x5198da..0x519988). Inference: not for a European record |
| slot 22, 0x903120 | vtable | none | yes |
| slot 35, `set_up_rules` 0x9030b0 | vtable | none | Prize 3,000,000 (+0x28, 0x9030c3), slot 15 |
| Base slots 5 (0x51f890), 6, 7, 12, 23, 37 | vtable | Cup base | yes for the class. Slices 22 and 23 wrap 0x51f890, 0x51fc00, 0x519a90, 0x5223a0, 0x518790 |

So the class has one function that reads the key id, and it is a direct call. A vtable copy cannot reach it, and the constructor's calls go through the original vtable anyway.

### B.2 The runtime wraps that need an object test

The wraps of slices 22 and 23 find "the UEFA Cup" by the vtable 0x970790 (`is_uc`, `groups_layout`, `season_uefa_cup.c` 113..118; `rd32(cup) == UC_VTABLE` in `cup_update_for_date`, `season_uefa_qual.c` 812; `season_uefa_report_clubs` 904). A second object with that vtable would get the Europa League's format, its drops from the Champions League (`uc_fill_drops`) and its entrants (`season_uefa_uc_entrants`). With a vtable copy they would not act at all after the constructor, and would act during it.

Recommendation: keep the vtable 0x970790 (no copy, so the rules report knows the class) and give the wraps a test by record: the Europa League is the object whose record id ([[obj + 4]]) equals [0x9cf6f0], the Conference League the one whose record id is its new record. The record pointer +4 is set at 0x90231d, before the first call, so the test also holds inside the constructor. Wraps to change: 0x902b90 (entrants), 0x9025c0 (round table), 0x51fc00 (stage array), 0x5a8f60 (advance stage), 0x519a90 (honours), 0x9030e0 (slot 17), 0x51f890 (drops), 0x5223a0 (rank), 0x518790 (fixture tree, if it acts on the UEFA Cup), and `season_uefa_report_clubs`.

### B.3 Club +0x1db and a club that drops into the third cup

- Set: the seeding sets -1 for every European club (`collect_european_clubs` 0x904040) and gives each place the cup's record id (original 0x90430e and on; the runtime's `season_uefa_seeding.c` 107 writes [0x9cf6e8] or [0x9cf6f0]). The Champions League hand-over writes [0x9cf6f0] (0x585a6a). Slot 17 of the UEFA Cup writes -1 for a club that goes out (0x9030f4).
- Read: `build_entrants` (0x902c3f..0x902c4c). Outside the class: `EUROPE_RULES::get_ineligibility_code` 0x5832e0 applies the European rules only for the Intertoto, UEFA Cup, Champions League and Cup Winners' Cup ids (0x583334..0x583352; any other id goes to the ruling body's rule 0x8e8a60). `SQUAD_MANAGER::process_euro_registration` 0x85a9b0 registers squads for the three cups by index (0x85aa18..0x85aa28). Domestic cups skip clubs with the UEFA Cup id (`HolCup::select_entrants` 0x5f4bed, `ScoLgeCup::collect_entrants` 0x7f04e7, `IreSuperCup::create_table` 0x63b1de).
- A club that drops from the Europa League into the Conference League needs: +0x1db = the Conference record id, +0x1df its entry code, a slot in the Conference League's entry table (+0xa7) and its list (+0x14, +0x36), as `uc_fill_drops` does for the Champions League losers. Slot 17 of the Europa League sets -1 when the club goes out (0x9030f4), so the hand-over must write after it. Inference: the hand-over in the wrap of 0x51f890 runs before the original update of that day, as slice 23's does, and so finds the losers of the rounds decided on earlier days.
- Inference from 0x5832e0 and 0x85a9b0: Conference League matches use the ruling body's rules with no European squad list. A wrap of 0x5832e0 that treats the Conference id as the UEFA Cup id would give the European rules. Not needed for the matches to play.

## C. The Europe array and entry 3

- The creator writes count 4 (0x8313bd) and `calloc(0x10, 1)` (0x8313bb, 0x8313c4): European_Cup, Uefa_Cup (new 0xb3 at 0x8314bf), Intertoto_Cup, Eur_Super_Cup.
- Entry 3 lies at 0xb63e38 (0xb63d60 + 3 * 0x48). A grep of the whole disassembly finds no instruction with entry 3's +0xc (0xb63e44) or +0x10 (0xb63e48) as a fixed address. The writes at 0x6673fe..0x6674e4 are `key_nation_build_table` setting +0, +4, +8 and the dates. The readers of the arrays are loops over all entries: save 0x6692f0, free 0x669290, `build_comp_object_table` 0x8386f0, the season pass 0x5d5ae0 (start_new_season at 0x5d5d1c), the menus 0x49d070 and 0x75db00 (null-checked, club world doc). Other code finds a European object by key id through [0xadadfc] (for example 0x585a02, 0x633deb, 0x85aa2e).
- So a fifth object can go in with `season_change_add_comp(3, obj)`, and it shows in the menus that walk the arrays. The Intertoto slot is not needed. The Intertoto object must stay in slot 2 (shared rule: never remove an object). Not searched: a fixed index into the array through a register that holds entry 3 (unknown, none seen).

## D. The seeding and the order of a season change

- `Uefa_Seeding::update_season` 0x9035a0 runs from the constructors and `start_new_season` of the three cups (0x583881, 0x584e56, 0x63304d, 0x6339df, 0x902377, 0x902fac) and works once a year (+0x439 holds the year). A Uefa_Cup-class Conference object calls it too (0x902377 in its constructor, 0x902fac in its `start_new_season`), and then `build_entrants`.
- The season pass calls `start_new_season` of each finished object of entry 3 in array order. Whichever object runs first runs the seeding. Each object then takes its entrants after it. So the Conference object in slot 4 gets its clubs after the seeding of each year with no change of order.
- At its first edition the module makes the object in the entry hook, before the cups of the entry start (season_change.h). Its constructor then runs the seeding of the new year itself (the year differs from +0x439) and takes its entrants at once. Inference: the seeding gives the same places whichever cup runs it, because it takes no argument from the cup (push 0 at 0x902376 and 0x902fab).
- What the fourth cup needs: the seeding wrap (slice 23, wrap of 0x904210) gives Conference places (+0x1db = its record id, +0x1df codes apart from those of the other cups, as slice 23 keeps codes 11 to 16 apart from 1 to 9). The Conference wrap of `build_entrants` takes the clubs with its id and code, as `season_uefa_uc_entrants` does.

## E. History and honours

- `comp_util_add_history_record` 0x4afce0 takes the record from the object (+4, 0x4afd51) and the game year (0x4afd57). It reuses an empty row or grows the history (0x527f60 at 0x4afd2c). It needs nothing from the record but its place in the array.
- `comp_util_get_latest_placed_club` 0x4afd80 (the title holder) sorts the history (0x945333, compare 0x540780) and searches it (0x946464). With no row for the record it returns 0 with no box (0x4afdd7 to 0x4afe9e). So a Conference League with no final yet has no holder; slice 23's seeding treats a holder of 0 as "none".
- `Club_History::record_comp_season` 0x442e80 tests the record for a friendly one (0x442f44) and a league division (0x4b0700 at 0x442f62), and walks the stages up to +0x30 (0x44313b). A new record below count - 0x7f is a cup of the normal kind.
- `Club_History::get_cup_history` 0x443520 walks [0xadadfc] with a null test (club world doc).
- A history row for the new record is saved as its id and linked again at a load (id pass, bound ANY). It points at the right record because the design keeps the index.
- Unknown: whether the competition history screens show a record with no rows without a box. Not read. The first build opens the Conference League's history and honours screens before its first final and after it.

## Recommended build plan

1. **Records (shared module, slice 26's file, or a small block of slice 24 if slice 26 is not merged).** Add a `tools/recomp/` patch file with the nine constant sites of A.5 and the runtime functions that give X and -X. Check first that the patch tool takes a `lea` displacement. Wrap `save_database` 0x52dad0 (K > 0: [0xae2344] = 0x7f + K for the call). Wrap `comp_get_english_name` 0x5455a0 to return the record's own name for a new record. Keep X for the game in the runtime (taken at the first site, cleared in `season_comps_reset`).
2. **The def.** One `SeasonCompDef` with `record` NULL, entry 3. The module gives a NULL def the record N + M + j and fills its fields when it makes the object (copy of the UEFA Cup record, `season_change_rename` for the names).
3. **make.** `new(0xb3)` (0x944cf1) and the constructor 0x9022e0 with (year, record), through `rt_callback`. No vtable copy. At a load (`loading` 1) the constructor reads the object from its `.tmp` file, which the save packs as for every object.
4. **Object tests.** Change `is_uc` and the vtable tests of slices 22 and 23 to a test by record id, with a kind (Europa League or Conference League) for each wrap of B.2.
5. **Entrants and places.** The seeding wrap of 0x904210 gives the Conference places from the access list. The wrap of 0x902b90 builds the Conference entry table from them, with slots for the clubs that drop from the Champions League and the Europa League.
6. **Drops.** The wrap of 0x51f890 puts the losers of the decided Europa League rounds into their Conference slots (+0x1db, +0x1df, +0xa7, +0x14, +0x36), after slot 17 has set -1.
7. **Format.** The wraps of 0x9025c0 (round table), 0x5a8f60 (groups and knockout) and 0x519a90, 0x9030e0 (honours, title holder) for the Conference rows of the table.
8. **Report.** Add the Conference object to `season_uefa_report_clubs` and the rules report.
9. **Tests.** A new 2021/22 game and a 2020/21 game with changing rules on, each with a save and a load in the qualifying rounds and in the group stage: the record index, its name, the object, its year and fixtures after the load. A base game save must not change (same-save check, Q152).

## Risks

1. The constant sites are inside `load_database`, the largest function of the game (21,564 bytes). A wrong site moves the nation comps or the friendly records and breaks every competition. The same-save check of the base game shows K = 0 is unchanged, and a check of the record ids after a K = 1 load shows the rest.
2. `database_load_language_names` and `eng_lng_load_tables` are the only other code found with count - 0x7f. A third such site that the grep missed (a count read through a register) would show a language box or a wrong name.
3. Saves of earlier 0.6.0 builds have K = 0: their games never get the Conference League.
4. A Conference club plays with no European squad list and with the domestic cups of some nations not skipping it (B.3). This is a difference to the real rules, not a crash.
5. A wrap that keeps the vtable test acts on the Conference object as the Europa League. Each wrap of B.2 must be changed in the same slice.
6. The history screens for a record with no rows are not read (E).

## Unknowns

- The meaning of record bytes +0x57, +0x58 and the word +0x69 (copied from the UEFA Cup record).
- Which builds use a language mode [0x9b5984] of 2 or more (`comp_get_english_name` 0x5455a0).
- Who writes `CM3_TEMP\eng.lng`, which `eng_lng_load_tables` reads.
- The meaning of the second header word [0xae2348] (`+= ebp` at 0x52c693). Not needed for the design.
- Whether the competition history screens need a history row.
