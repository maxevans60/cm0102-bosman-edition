# Rules table reader, season jump and rules report (0.6.0 slice 12)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `tools/seasons/make_rules.py`, `tools/seasons/make_comp_classes.py`, `src/recomp_rt/win/season_rules.c` and `.h`, `src/recomp_rt/win/season_report.c`, `tests/seasons/rules_report_check.py`, `tests/seasons/test_season_rules.c`.

## The rules table in the download

`make_rules.py` reads the 20 parts of `docs/research/rules-0.6.0/` (7,101 rows) and writes one file, `Seasons/rules.tsv` of the download (4.5 MB). The build writes the same file to `build/seasons/rules.tsv`. The format and the value forms are in the docstring of the tool. The structured columns become values: the season is its start year, `comp_file` is a file name of the original without `.cpp`, and the counts (clubs, up, down, subs_used, bench) are a number or a code (na, unknown, not_held, see_each, as_cl, text). Each count and each code column keeps its text in `<name>_text`. The other columns stay as text for slice 17. The tool stops on a row it cannot read and names the part, the line and the column.

The game reads the file once at the start (`season_rules_init`, called in `main.c` after `season_init`). The reader uses host memory only. A missing or damaged file gives an empty table and a line on stderr. No rule uses the table yet.

## The season jump

`RECOMP_SEASON_JUMP=<year>` and `RECOMP_RULES_REPORT=<file>` are test variables. The test steps start a new game (Arsenal, the default league selection) and go on holiday, so the days pass with no input.

The jump needs match results (`2026-10-16-sweep-season-end.md`, section 2.4). It uses the original's own switch for a game with no match engine:

- `create_game_from_setup_file` 0x827110 reads `ENABLE_MATCH_ENGINE=` (0xa812c4) and writes the byte 0xa5a390: 1 for "YES" (0x8277a6), 0 for "NO" (0x8277cf). The exe data holds 1.
- `MATCH_DAY::play_matches` 0x6b4360 reads it at 0x6b439b, 0x6b4797 and 0x6b4a29. At 0x6b4a29 a match of a human club takes the deferred path 0x6b4df5 only when the byte is 1. With 0 the match is played with the others.
- `CM3_MATCH_ENGINE::init_match` 0x6b7f20 sets its quick flag when the byte is 0 (0x6b85fb..0x6b8604), so every match goes to `CM3_MATCH_ENGINE::quick_simulate_match` 0x6beeb0.
- The results go through `match_day_apply_match_result` 0x6b5f00 into each competition, as on a normal day. So the tables, prize money, history, records and the end-of-season step (`game_season_update_pass` 0x5d5ae0) run in the real day loop.

The runtime writes 0 to the byte at the first human turn, after the game is made. With the test load of other runs (load average 20 to 28), one game day takes about 0.25 s.

## The rules report

At the first human turn and at the first human turn on or after 1 August of each later year, the runtime writes one `season` line and one `comp` line for each competition object. The objects come from `g_comp_object_table` [0xadadfc] ([0xadadf8] slots, one per club competition record, 0x8386f0) and from the competition arrays of the 35 league table entries at 0xb63d60 (+0xc count, +0x10 array, 0x5d5cab). The fields:

| Column | Where | Evidence |
|---|---|---|
| record, name | object +4: the competition record; record +0 id, +4 long name | 0x689b66, 0x48f2df; `2026-10-13-database-inventory.md` |
| file, class, kind | object +0: the vtable, looked up in the class table of the build | `make_comp_classes.py` (constructors, for example 0x5733d0 in EngPrm::EngPrm) |
| year | +0x40, s16 | `start_new_season` adds 1 (0x574c10) |
| clubs | +0x3e, s16 | League::check_complete 0x689b3d; cups set it in the constructor (EngCcCup 0x56c32d: 92) |
| rounds | +0x3c, s16: a league's meetings of each pair, a cup's round count | 0x689b27..0x689b36; Cup::map_fixture_tree 0x51879c |
| matches | +0xa7, s16, leagues only | 0x689b1d |
| subs_used, bench | +0x4a and +0x49, bytes | Comp::Comp 0x48c5c9 (5 named, 3 used); Comp::set_fixture_substitutes 0x48f2fa..0x48f32d |

For a qualifying league with groups, +0x3e is the club count of one table, not the entrant count (inference from the values: 5 for the European Championship qualifying). A national team tournament holds 0 clubs on 1 August of its season: its team list is made later (inference).

The class table: a constructor `X::X` of the function map stores its vtable after the call of its base constructor. The tool reads that store and follows the first call down to Comp::Comp 0x48c580, League::League 0x682200 or Cup::Cup 0x518640 for the kind. For classes that another function makes inline (BIntComp and Under21IntComp in ReserveComp::create_sub_comps 0x7e61c0), it takes any stored vtable whose slot +0xc is Comp::all_stages_complete 0x48ce10 and whose slot 0 is a scalar deleting destructor of the function map. 188 classes.

## Leagues that the report covers

The default league selection of a new game is England. The report holds:

- England: Premier Division, First, Second and Third Division (eng_prm to eng_third), FA Cup, League Cup, Charity Shield and Vans Trophy (eng_fa_cup, eng_cc_cup, eng_charity, eng_auto_cup). The game makes no Conference and no FA Trophy with the default selection (records 93 and 94 of club_comp.dat stay without an object).
- UEFA: Champions League, UEFA Cup, Super Cup and Intertoto Cup (european_cup, uefa_cup, eur_super_cup, intertoto_cup).
- Club competitions between confederations: Intercontinental Cup, Club World Championship and the Inter-American Cup (world_club_cup, world_club_champ, inter_amer_cup).
- National teams: the World Cup and its six qualifying sections, the European Championship and its qualifying, the Copa America, the African Cup of Nations, the Asian Cup, the Oceania Nations Cup, the Confederations Cup and the Olympics.
- Classes with no row in the table: Friendly, the friendly league and cup (mini_league, mini_cup), and the Reserve, 'B' International and Under 21 International objects (record_utils).

34 to 36 competitions a season. No other nation's league is in the report: the jump uses the default selection, as the brief asks.

## Base game report against the 2001/02 rows

The report of the base game (start 2001, 3.9.68 data, changing rules off) matches the original's data: each record name is the name in club_comp.dat or nation_comp.dat, and each English division holds the club count that club.dat gives it (20, 24, 24, 24). Against the 2001/02 rows of the rules table (`rules_report_check.py`, 42 differences):

- **Names (21):** every competition has the database name, not the real name of the season, for example "English Premier Division" for "FA Premier League", "European Champions Cup" for "UEFA Champions League", "Inter-Toto Cup" for "UEFA Intertoto Cup". Slice 15 gives the names.
- **Club counts (7):** the Champions League object holds 55, the table 72 (32 in the group stage). On 14 July 2001 the World Cup holds 0 (table 32), the African Cup of Nations 0 (16) and the Copa America 4 (12). The World Cup qualifying sections hold their own values: Africa 50 (table 25 in the second round), CONCACAF 24 (6 in the final round), Oceania 5 (1).
- **Bench (1):** the World Cup names 7 substitutes, the table 12.
- **Timing of the cycles (6):** the European, Asian and South American World Cup qualifying objects already wait for the 2006 campaign (year 2004), while the table holds the 2002 campaign in 2001/02. The European Championship qualifying (2002), the Oceania Nations Cup (2002) and the Inter-American Cup (2001) are objects of the season that the table marks not held.
- **Not in the game (3):** the Football Conference and the FA Trophy (no object with the default selection), and the UEFA access list (uefa_seeding is not a competition object).
- **No row (4):** Friendly, Reserve, 'B' International and Under 21 International.

The English divisions agree with the table in clubs (20, 24, 24, 24), substitutes used (3) and named (5). The Charity Shield names 7 and uses 5; the table has unknown for 2001/02 and 6 used from 2002/03.

## The jump to 2027/28

`rules_report_check.py --run build/cm0102_recomp <game> --last 2027` played from 14 July 2001 to 1 August 2027 and ended with code 0 after 3,856 s (27 seasons, load average 6 to 28 from other runs). Every season's report shows the year +0x40 of each club competition moved by one each season (the Premier League holds 2027 in 2027/28). The 1,635 differences with the table are the rules that later slices bring: for example 5 substitutes and 7 or 9 on the bench from 2020/21, no Intertoto Cup after 2008, 76 and 81 clubs in the Champions League, the Conference League from 2021/22 and the Europa League names. The English division sizes stay 20, 24, 24, 24, as in the table. No rule changes in this slice, so the report keeps the 2001/02 structures, as the original does.

A jump from 2010 with the September 2010 ODB (`--year 2010 --database ...` `--last 2012`) ended with code 0 after 329 s, with 34 or 35 competitions a season.

## Unknowns

- The meaning of +0x3e for each group competition and qualifying class.
- The club.dat field +0x57 is the division of a club by inference from the data (20 clubs in division 7, 24 in each of 8, 9 and 10). The check uses it for the base game only.
