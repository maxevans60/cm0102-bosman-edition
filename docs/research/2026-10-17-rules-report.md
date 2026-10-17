# Rules table reader, season jump and rules report (0.6.0 slice 12)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `tools/seasons/make_rules.py`, `tools/seasons/make_comp_classes.py`, `src/recomp_rt/win/season_rules.c` and `.h`, `src/recomp_rt/win/season_report.c`, `tests/seasons/rules_report_check.py`, `tests/seasons/test_season_rules.c`, `tests/seasons/rules_report_expected/`.

## The rules table in the download

`make_rules.py` reads the 20 parts of `docs/research/rules-0.6.0/` (7,101 rows) and writes one file, `Seasons/rules.tsv` of the download (4.5 MB). The build writes the same file to `build/seasons/rules.tsv`. The format and the value forms are in the docstring of the tool. The structured columns become values: the season is its start year, `comp_file` is a file name of the original without `.cpp`, and the counts (clubs, up, down, subs_used, bench) are a number or a code (na, unknown, not_held, see_each, as_cl, text). Each count and each code column keeps its text in `<name>_text`. The other columns stay as text for slice 17. The tool stops on a row it cannot read and names the part, the line and the column. The tool and the reader have the same limits (seasons 2001 to 2099, counts up to 100,000), so a bad table stops the build and never reaches a game.

The game reads the file once at the start (`season_rules_init`, called in `main.c` after `season_init`). The reader uses host memory only. It drops a `\r` at the end of a line. A missing or damaged file gives an empty table and a line on stderr. No rule uses the table yet. The Windows check (`tools/windows/check_installer.py --manifest`) checks that the installer packs `Seasons/rules.tsv` and the season zips: the packed folder against the manifest (size and SHA-256), and the makensis log (`-V4`) against the manifest (path and size).

## The season jump

`RECOMP_SEASON_JUMP=<year>` and `RECOMP_RULES_REPORT=<file>` are test variables. A jump value that is not a year from 2001 to 2099 stops the program at the start with code 2. The test steps start a new game (Arsenal) and go on holiday, so the days pass with no input. The jump plays every day: a jump with no days played is not possible, because the end-of-season step runs only for a competition whose matches are all played, and prize money, history and records come from the day loop (`2026-10-16-sweep-season-end.md`, section 2.4).

It uses the original's own switch for a game with no match engine:

- `create_game_from_setup_file` 0x827110 reads `ENABLE_MATCH_ENGINE=` (0xa812c4) and writes the byte 0xa5a390: 1 for "YES" (0x8277a6), 0 for "NO" (0x8277cf). The exe data holds 1.
- `MATCH_DAY::play_matches` 0x6b4360 reads it at 0x6b439b, 0x6b4797 and 0x6b4a29. At 0x6b4a29 a match of a human club takes the deferred path 0x6b4df5 only when the byte is 1. With 0 the match is played with the others.
- `CM3_MATCH_ENGINE::init_match` 0x6b7f20 sets its quick flag when the byte is 0 (0x6b85fb..0x6b8604), so every match goes to `CM3_MATCH_ENGINE::quick_simulate_match` 0x6beeb0.
- `match_stats_get_record_bucket` 0x729bd0 reads it at 0x729c32: for a match whose argument has byte +0x4c set, it returns bucket 3 only when the byte is 1 (0x729c3d). With 0 that match goes to another bucket of the match statistics (the meaning of +0x4c and of the buckets is unknown).
- The results go through `match_day_apply_match_result` 0x6b5f00 into each competition, as on a normal day. So the tables, prize money, history, records and the end-of-season step (`game_season_update_pass` 0x5d5ae0) run in the real day loop.

The runtime writes 0 to the byte at the first human turn, after the game is made.

## The rules report

The runtime writes one `season` line and one `comp` line for each competition object twice in each season:

- **start:** at the first human turn of the game, then at the first human turn on or after 1 August;
- **end:** at the first human turn on or after 1 June of the next year, before the update days of the league table entries (mostly 20 or 21 June). A national team tournament makes its team list after the start: the World Cup holds 0 clubs on 14 July 2001 and 4 on 1 June 2002 (inference: the club count of one group).

The jump ends after the end report of the last season. The objects come from `g_comp_object_table` [0xadadfc] ([0xadadf8] slots, one per club competition record, 0x8386f0) and from the competition arrays of the 35 league table entries at 0xb63d60 (+0xc count, +0x10 array, 0x5d5cab). The fields:

| Column | Where | Evidence |
|---|---|---|
| record, name | object +4: the competition record; record +0 id, +4 long name | 0x689b66, 0x48f2df; `2026-10-13-database-inventory.md` |
| file, class, kind | object +0: the vtable, looked up in the class table of the build | `make_comp_classes.py` (constructors, for example 0x5733d0 in EngPrm::EngPrm) |
| year | +0x40, s16 | `start_new_season` adds 1 (0x574c10) |
| clubs | +0x3e, s16 | League::check_complete 0x689b3d; cups set it in the constructor (EngCcCup 0x56c32d: 92) |
| rounds | +0x3c, s16: a league's meetings of each pair, a cup's round count | 0x689b27..0x689b36; Cup::map_fixture_tree 0x51879c |
| subs_used, bench | +0x4a and +0x49, bytes | Comp::Comp 0x48c5c9 (5 named, 3 used); Comp::set_fixture_substitutes 0x48f2fa..0x48f32d |
| matches | +0xa7, s16, leagues only | 0x689b1d |
| up | +0xbe, leagues only: promotion places | League::has_automatic_promotion 0x684790; EngFirst::set_up_rules 0x572dc8 (2), EngPrm 0x574bb8 (0) |
| playoff_up | +0xbf: promotion play-off places (inference from the values) | EngFirst 0x572dce (4), EngThird 0x578104 (4), EngPrm 0x574bbe (0) |
| playoff_down | +0xc0: places next to both ends that are neither safe nor direct (inference: relegation play-off places) | League::update_positions_and_status 0x6852b3..0x6852e4 adds it to +0xbe at the top and +0xc1 at the bottom; 0 in England |
| down | +0xc1: relegation places | League::has_relegation 0x6847b0; EngPrm 0x574bca (3) |
| flags | +0xd9, u16: bit 1 automatic promotion, bit 0x20 status marks | 0x68479a, 0x6852a0 |
| above, below | +0x1c and +0x20, s32: the record ids of the divisions above and below, -1 for none | EngFirst 0x572dea and 0x572df6 store the key ids [0x9cf5bc] and [0x9cf5c4]; EngPrm 0x574bd0 stores -1 |

The game holds no split or format field that is common to all leagues: a split season is code of its class (for example the Scottish leagues). The report writes the rounds (+0x3c) and the matches per club (+0xa7), which give the format of a round robin. For a qualifying league with groups, +0x3e is the club count of one table, not the entrant count (inference from the values: 5 for the European Championship qualifying).

The class table: a constructor `X::X` of the function map stores its vtable after the call of its base constructor. The tool reads that store and follows the first call down to Comp::Comp 0x48c580, League::League 0x682200 or Cup::Cup 0x518640 for the kind. For classes that another function makes inline (BIntComp and Under21IntComp in ReserveComp::create_sub_comps 0x7e61c0), it takes any stored vtable whose slot +0xc is Comp::all_stages_complete 0x48ce10 and whose slot 0 is a scalar deleting destructor of the function map. 188 classes.

## The check and the expected differences

`rules_report_check.py` compares each competition with the row of its file and season: a row of this file alone before a row of two files, a row that is not a qualifying path, then the k-th row for the k-th object of the same file. It compares the clubs, substitutes and bench (a difference only when the start and the end value both differ), and for a division (a row with a numbered tier) the promotion and relegation places and the play-offs. The row's count may also hold the play-off winner or the play-out losers. The kinds of difference are in the docstring of the check.

Each run has a file of expected differences, `tests/seasons/rules_report_expected/<start year>-<nations>.tsv`. A difference that the file does not list fails the check. A slice that brings a rule runs the check with `--update-expected` and commits the shorter file. A difference that has gone is listed and does not fail.

## Runs

| Run | Command | Time (load 12 to 24 from other runs) |
|---|---|---|
| Per slice | `python3 tests/seasons/rules_report_check.py --run build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 --year 2010 --database "/Users/max/Projects/cm0102/test-fixtures/databases/September 2010 ODB" --last 2011` | 379 to 413 s (two seasons, England) |
| Release | `python3 tests/seasons/rules_report_check.py --run build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 --last 2027 --timeout 10800` | 3,380 s (27 seasons, England) |
| More nations, 2010 | `... --year 2010 --database "<September 2010 ODB>" --nations argentina,australia,belgium,brazil,croatia,denmark,finland,germany,greece,holland,italy,japan,northern_ireland,norway --last 2010` | 1,655 s (one season) |
| All nations, base game | `... --run build/cm0102_recomp /Users/max/Projects/cm0102/clean-3968 --nations all --extra conference,regional,c2 --last 2001` | 3,428 s (one season, 175 competitions) |

The per-slice run takes the start year of the slice's own seasons: for example `--year 2003` with the October 2003 database for slice 22.

## Nations

The test steps select leagues on the Select League(s) screen (`--nations`, `--extra`). A step's time counts from the last wait, so the clicks have rising times. Results on 2026-10-17:

- **Base game, 3.9.68 data, 2001:** Select All with the Conference, the German regional divisions and Serie C2 loads and plays (all 27 nations): 175 competitions, one season in 3,428 s. Expected file `2001-all-conference-regional-c2.tsv`. Each English division holds the clubs that club.dat gives it, and every record name is that of the data.
- **September 2010 ODB, 2010:**
  - Select All stops at the box Database.cpp 1844 (0x527ea3, in `database_generate_extra_non_playing_staff`), as slice 11 found. Select All without France stops there too.
  - France with the seven nations after it stops at fra_first.cpp 108 (`FraFirst::FraFirst`): Ligue 1 wants 20 clubs, as slice 11 found.
  - Argentina to Norway without France (14 nations and England) loads and plays: 109 competitions, one season in 1,655 s. Expected file `2010-england-argentina-...-norway.tsv`.
  - The 11 nations below Norway (Republic of Ireland, South Korea, Poland, Portugal, Russia, Scotland, Spain, Sweden, Turkey, United States, Wales) with England load too. The screen shows only the first 16 rows, so the step takes the other rows out after Select All (`--nations all,-argentina,...`).
  - So the stop at 1844 comes from the number of nations together, not from one nation (inference: each half loads, the whole does not). Another agent works on it.

## Base game report against the 2001/02 rows

The report of the base game (start 2001, 3.9.68 data, changing rules off) matches the original's data: each record name is the name in club_comp.dat or nation_comp.dat, and each English division holds the club count that club.dat gives it (20, 24, 24, 24). Against the 2001/02 rows of the rules table (England, the default selection):

- **Names (21):** every competition has the database name, not the real name of the season, for example "English Premier Division" for "FA Premier League", "European Champions Cup" for "UEFA Champions League". Slice 15 gives the names.
- **Club counts (7):** the Champions League object holds 55, the table 72 (32 in the group stage). The World Cup, the African Cup of Nations and the Copa America hold 0 at the start and 4 at the end (tables 32, 16, 12). The World Cup qualifying sections hold their own values: Africa 50 (table 25 in the second round), CONCACAF 24 (6 in the final round), Oceania 5 (1).
- **Bench (1):** the World Cup names 7 substitutes, the table 12.
- **Edition timing (6):** the European, Asian and South American World Cup qualifying objects already wait for the 2006 campaign (year 2004), while the table holds the 2002 campaign. The European Championship qualifying (2002), the Oceania Nations Cup (2002) and the Inter-American Cup (2001) are objects of the season that the table marks not held.
- **Not in the game (3):** the Football Conference and the FA Trophy (no object with the default selection; `--extra conference` makes them), and the UEFA access list (uefa_seeding is not a competition object).
- **No row (4):** Friendly, Reserve, 'B' International and Under 21 International.

The English divisions agree with the table in clubs (20, 24, 24, 24), promotion, relegation and play-off places, substitutes used (3) and named (5). The release run to 2027/28 lists 1,647 differences, all in `2001-england.tsv`. They are the rules that later slices bring (for example 5 substitutes and 7 or 9 on the bench from 2020/21, no Intertoto Cup after 2008, the Conference League from 2021/22) and the database names.

## Same-save checks

Run on 2026-10-17 after the merge of main (`7df5c0f`), with `G=/Users/max/Projects/cm0102/clean-3968` and `REF=/Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp`. The jump variables are unset in these runs, so the report and the jump change nothing. The table read at the start uses host memory only.

| Command | Result |
|---|---|
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --ref-build $REF` | PASS: every run, ref-build equal to 0.5.0 |
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save prematch.sav --ref-build $REF` | PASS, ref-build equal to 0.5.0 |
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --ref-build $REF` (new game) | PASS, ref-build equal to 0.5.0 |
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save yeovil.sav --days 7 --ref-build $REF` | ref-build equal to 0.5.0, again equal. The `off` run differs in `Friendly League_453.tmp`. The main build (`/Users/max/Projects/cm0102/cm0102-mac/build/cm0102_recomp`) gives the same `off` difference, so it does not come from this slice |
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --match --ref-build $REF` | PASS, ref-build equal to 0.5.0. A run beside five other test sets failed in every option run while `again` was equal (commentary waits count real time); the run alone passed |
| `python3 tests/seasons/season_core_run.py build/cm0102_recomp $G --only season` (before the merge) | PASS: the 2010/11 game twice gives the same save |
| `cd build && ctest` | 13 of 13 pass, `season_rules` among them |

## Unknowns

- The meaning of +0x3e for each group competition and qualifying class.
- The meaning of +0xbf and +0xc0 is an inference from the values and from 0x6852b3..0x6852e4. A league with a relegation play-off (for example Serie B) would show it.
- The club.dat field +0x57 is the division of a club by inference from the data (20 clubs in division 7, 24 in each of 8, 9 and 10). The check uses it for the base game only.
- Why every nation together stops the September 2010 ODB at Database.cpp 1844 while each half loads (see Nations).
