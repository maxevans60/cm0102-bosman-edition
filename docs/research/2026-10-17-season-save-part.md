# The season part of a save (0.6.0 slice 11)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `src/recomp_rt/win/season.c`.

## Result

- A save keeps the start year and `changing_rules` of its game in a new part, `bosman_season.dat`, in the save container.
- The runtime writes the part **only** when the start year is not 2001 or `changing_rules` is on. A base game save (2001/02, changing rules off) has no part, so it is byte for byte the save of 0.5.0.
- A save without the part loads as 2001 with changing rules off. This covers every save of 0.5.0 and earlier.
- The original never reads the part (section 2). The runtime adds it on the host side after the original's save returns, and reads it on the host side before the original's load runs (section 3). Neither step touches the game's memory.

## 1. The container

Source file zip.cpp of the original. `tools/save/savfile.py` reads and writes the same format.

| Item | Evidence |
|---|---|
| Header of 0x416ec bytes: word version, word archive id, word count, then 1000 rows of 0x10c bytes (word offset, word size, name of 260 bytes) | read at 0x932816..0x9328f3 (12 bytes, then count rows), written whole by `zclose` 0x932b00 |
| Version 1 and 3 store a part as it is. Version 2 and 4 store it with the byte run code. The reader maps 1 to 3 and 2 to 4 in memory | 0x93282f..0x93284a |
| The byte run code: a byte below 0x81 is one byte, a byte b of 0x81 or more repeats the next byte b - 0x80 times | `zread` 0x932c30, `zwrite` 0x932e10 |
| The size in a row is the size before the code | `zread` stops at the size; `savfile.py` |

## 2. The original skips a part that it does not know

### 2.1 A part opens by its exact name

`zopen` 0x9326e0 (archive path, part name, write flag, archive id, archive type):

- Read mode: it reads the header (0x9328a3..0x9328f8), then compares the name with the name of each row from 0 to count - 1, byte by byte with case (0x93293b..0x932981). With a match it seeks to the row's offset (0x9329b6..0x9329c4). Without one it prints "zopen(): File %s not found" (0x9329e8) and returns 0.
- So each read of the load gets the part it names and never looks at another part.

### 2.2 The load reads named parts and the `*.tmp` parts only

`load_game_data` 0x82a9c0 (cdecl, path of the save; callers 0x81bee7, 0x81c53a, 0x81f360):

- 0x82ac50: `zget_matching_size` 0x933860 with `*.tmp` (string 0xa813ac): the size of the temporary tables, for the free disk check. It counts only the rows whose name matches (0x9339b7 calls the matcher 0x933a00).
- 0x82acbc: `zextract_matching_files` 0x933320 with `*.tmp`: it copies the names of rows 0 to count - 1 (0x93345d..0x9334a5, an empty name for the other rows) and extracts a part only when the matcher 0x933a00 says yes (0x9334c6, 0x9334ce).
- 0x82ad15 and later: `zopen` of named parts, for example `general.dat` (string 0x9c7b84).

The matcher 0x933a00 (pattern, name): an equal string matches (0x933a53..0x933a82). For a pattern `*.` it moves the name pointer to the last `.` of the name (0x933a9b..0x933ac4, `strchr` 0x946540 for a later `.`), then compares the rest of the pattern with the rest of the name, one character at a time, through `toupper` 0x90c2f0 (0x933aca..0x933ae8). A difference returns 0 (0x933a47). So `*.tmp` matches only names that end in `.tmp`. `bosman_season.dat` ends in `.dat`: the load never extracts it and never counts it.

### 2.3 Other readers of a save

- `check_saved_game_file` 0x838e10 calls `zget_archive_id` 0x9331c0 (0x838e8d) and tests the archive id 0x16 (0x838e97). It reads the header only. The new part does not change the archive id.
- The original itself added a part in a later version: `save_fifa_transfer_rules` 0x8395b0 adds a part (name at 0xa82518) with `zopen` in write mode (0x8395fd), and `load_fifa_transfer_rules` 0x839760 reads it by name. An older save without that part loads. This is the same pattern.

### 2.4 Inference

A save with the part loads in the original and in 0.5.0 as a 2001/02 game: they ignore the part. This was not run in the original; it follows from 2.1 to 2.3.

## 3. The writer and the reader in the runtime

### 3.1 Which writer

`save_game` 0x82e430 (cdecl: path, flags; callers 0x7617d8 Auto Save, 0x76276a, 0x827b71; the test saves of `testrun.c` call it too):

- With the option bit 0x20000000 of the game options (`[0xae3138] + 4`, tests at 0x82e586 and 0x82f5bd) the save writes `temp_save_file` with the path's folder and ending (string 0xa81cf4, made at 0x82e5cb..0x82e5de). Without the bit it writes the path itself (0x82e5e8..0x82e60e).
- It deletes an old file of that name first (0x82e61c, 0x82e62d), so each save makes a new container.
- It adds the parts with `zopen` in write mode (for example `general.dat` at 0x82e77e) and `zadd_matching_files` 0x933610 (0x82f56d). The last part is the transfer rules part of 0x8395b0 (0x82f5b0).
- With the bit, it deletes the old save (0x82f5e3) and renames the temporary file to the path (0x82f684, `rename` 0x945ba4).
- It returns 1 at 0x82f6af. Every failure returns 0 (for example 0x82e579, 0x82f703).

The rename in `files.c` (MoveFileA) happens only with the bit, so a hook there would miss the saves without it. The runtime therefore wraps 0x82e430 (`RT_WRAP` in `season.c`). When the original returns 1, the file at the path is whole. The wrap then:

1. opens the file on the host (`win_existing_file` in `files.c`: the user folder or the save folder, as the game's own write),
2. checks the header: version 1 to 4, count below 999 (the limit of `zopen`, 0x932a3e), no row of that name yet,
3. writes the part at the end of the file (with the byte run code for version 2 and 4),
4. writes the new row (offset, size 16, name, the other bytes 0) at row `count`,
5. writes count + 1.

The order keeps the file whole: if a write stops before step 5, the file is the old save without the part. A failure prints a message on stderr. The game's memory, its heap and its stack do not change, so the game plays on as before.

### 3.2 The reader

The runtime wraps `load_game_data` 0x82a9c0. Before the original runs, it reads the header and the rows of the file on the host side, finds the row named `bosman_season.dat`, decodes the part and sets the start year and changing rules. Without the row it sets 2001 and off. The load needs the start year at once: it builds the league table from the game year at 0x82b845 (`key_nation_build_table` 0x667150), and the year sites of `tools/recomp/seasons.py` read the start year. A part that cannot be read (a bad version, a length below 16, a year outside 2001 to 2099, changing rules not 0 or 1) gives 2001 and off, with a message.

### 3.3 New game and end of game

- `create_game` 0x828110 (callers 0x81e712, 0x81ebf4, 0x82790b): the wrap sets the values of the new game: the start year of the last `season_set_new_game` (2001 before any call, or `RECOMP_START_SEASON`) and changing rules from that call or from the option `changing_rules`.
- `free_game_data` 0x82ff40: the wrap sets the start year of the next new game and changing rules off before the original runs. The original then sets the game date to 1 January of that year (0x8304ef..0x8304fe, the year constant at 0x8304f1 is a year site). The start screen sets the same date at 0x81b938 (year constant at 0x81b93c).
- `season_set_new_game` on the start screen also sets the game date to 1 January of the new year through `Date::set` 0x54a130, because the league table of the new game takes its season from that year (0x81cf37 in `setup_start_new_game` 0x81ce00). It calls the original only when the year changes, so a base game start never calls it.

## 4. Format, version 1

Little endian, 16 bytes, stored in the container like any part.

| Offset | Size | Field |
|---|---|---|
| +0 | 4 | format version: 1 |
| +4 | 4 | length of the part in bytes: 16 |
| +8 | 4 | start year, 2001 to 2099 |
| +12 | 4 | changing rules: 0 off, 1 on |

Rules for later slices:

- A new field goes after the last field. The length grows. The version grows by 1. No field changes its meaning or its place.
- A reader reads the fields it knows that lie inside the length. A field outside the length takes its old meaning (the value of a game before that field). It ignores bytes after the fields it knows.
- The part is written only when the game is not the base game. A field that a base game needs must not force the part into a base game save.

## 5. Checks

- `tests/seasons/season_core_run.py`: a base game save has no part; with changing rules on the part holds 2001 and 1; a 2010 game's part holds 2010 and 1; the 2010 save loads in a new folder with the year 2010 and saves the part again.
- `tests/smoke/smoke_run.py --same-save` against the 0.5.0 build: equal saves for start.sav, prematch.sav, Yeovil and a new base game. The run `rules-on` of a new game differs only in this part.

## 6. Unknowns

- What the option bit 0x20000000 is called in the game's options screen, and its default. It does not change this design.
- A 0.6.0 save with the part that a 0.5.0 build loads plays as a 2001/02 game with the later database's dates (inference from section 2). No test covers it.
