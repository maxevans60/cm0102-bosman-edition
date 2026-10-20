# Italy's single Serie C of three groups, 2014/15 on (0.6.0 slice 20b)

Date 2026-10-20. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses, read in the recompiled C of `build/recomp/gen` (each instruction of the original is a comment there). Research: `slice-20-research-ita.md` in the shared notes folder of the 0.6.0 lead (the class table, the chain, the regroups, the cups). Spec: slice 20, Q290, Q363. Rows: `rules-0.6.0/ita.tsv` ("new" rows "Serie C group A..C" from 2014/15; C1 and C2 not held from 2014/15). Lists: `rules-0.6.0/ita-clubs.tsv` (36 lists, 2014/15 to 2025/26).

Code: `src/recomp_rt/win/season_divisions_ita.c` and `.h` (the Italian part of slice 20's new divisions module), an Italy block in `tools/recomp/new_divisions.py`, one dispatch line in `season_divisions_class_size`, small hooks in `season_sizes.c` (cap), `season_comps.c` (one def), `season_change.c` (one kind), `tools/seasons/make_clubs.py` (targets). Test: `tests/seasons/ita_serie_c_run.py`.

## The era

Rows (Q520, as slice 20): from 2014/15 the rows "Serie C group A", "B" and "C" take the comp_file of the object they play in (`ita_ser_c1a`, `ita_ser_c1b`, `ita_ser_c2a`), and the C1 and C2 rows of those seasons are `n/a` with a note. So slice 18's sizes and places, slice 15's names table (the groups are named "Lega Pro girone A" to 2016/17 and "Serie C girone A" from 2017/18) and the rules report read them as the rows of these objects, with no mapping in code.

A season S is in the era when its rules season has the three rows "Serie C group A", "B" and "C" (held, 2 to 20 clubs). The rules season is S with changing rules on and the start year with changing rules off (Q232). The base game and `RECOMP_SEASON_CODE=off` have no rules season (`season_game_changes`), so they never enter the era (Q152, Q342).

## The objects (Q620)

Groups A, B and C are the objects of Serie C1/A (ItaSerC1a, key id [0x9cf578]), C1/B ([0x9cf57c]) and C2/A ([0x9cf580]). Each class picks its clubs by the key id of its own record (0x64b71d, 0x64f8cd, 0x653b3d), so each group plays the clubs whose division is its record.

- `create_italian_comps` 0x835320 always makes C1/A and C1/B, and C2/A, C2/B and C2/C only with the extra division flag (bit 4 of nation +0x11c, 0x835407). Q363: the single Serie C follows C1, so it always exists.
- Without the flag, slice 26's module makes the C2/A object (def `season_ita_group_c`, entry 21, record "Italian Serie C2/A", operator new 0xee and constructor 0x652e90 thiscall (year, record), as 0x835696..0x8356ce). Entry 21 is Italy's: `key_nation_build_table` 0x667150 stores the Italian nation at 0xb64348 = 0xb63d60 + 21 x 0x48. With the flag the original's object exists, so the make function returns 0 (at a new game and at a load). The def's file is the class's own file (`ita_ser_c2a`), so `season_change_file` and the rule kinds still read it as C2/A.
- C2/B and C2/C (with the flag) end in the era with year 2999, the shared rule for a competition that never returns (Q395). Their constructors stop with "league..cpp 1020" when a table has no club, so the club moves give each one club (C2/B and C2/C count 1 in `season_ita_division_size` with the flag), and `create_table` (0x657f50, 0x65c0e0) takes it (`rt_ita_c2_end_clubs` at 0x657f62 and 0x65c0f2). The new game hook of the rule kind then sets year 2999. Their fixed fixture lists of 2001 (0x658750, 0x65c8e0) write 306 entries into a block for one club, so in the era the builders make no list (+0xea 0); `create_fixtures` reads the list only when its year is the league's (0x68382e..0x68383d). At the change into the era the chain ends them. They are never started again (below), and the rules report shows no edition of them (an object whose year is outside the season gives no line).
- Names: from the names table (Q520). A small rule kind writes the news of the change into the era.

## Sizes: groups of 20 with 38 match days

| What | Original | Era |
|---|---|---|
| Table block of C1/A, C1/B, C2/A | `push 0x426` (18 rows of 0x3b) at 0x64b67b, 0x64f82b, 0x653a9b | 20 rows (`rt_ita_table_bytes`) |
| Count of C1/A and C1/B | slice 18's store (`rt_league_clubs`), cap 18 | cap 20 (`season_ita_cap`) |
| Count of C2/A | `mov word ptr [edi + 0x3e], 0x12` at 0x653aa2 | the clubs of its division, at most 20 (`rt_ita_c2a_clubs`) |
| Match days | `create_schedule` stage -1 (C1/A 0x64ad80, C1/B 0x64ef30, C2/A 0x6531a0): D 0x22 (0x64adbd), block 0x8a2 (0x64adb8), 34 calls of `league_round_set_date` 0x68a160 and `league_round_set_match_slot` 0x68a1c0 | for more than 18 clubs: 2 x (n - 1) match days, 38 for 20 or 19 |

The 38 match days (Q621): the original's 34 of C1/A (Sundays from 5 September to 7 May, weekday 6 time 1; Wednesday 6 January weekday 3 time 2; Tuesday 5 April weekday 2 time 2; read from 0x64ae06..0x64b3a0), plus Sundays 22 and 29 August and the Wednesdays 27 October and 9 March. The same calendar serves every season, as the original's does.

A new count sets the matches of a club (+0xa7) back to -1, so `League::check_complete` 0x689ad0 counts them again (as slice 18).

## The season change

`ItaSerA::start_new_season` 0x641700 already has slice 16's generated wrap (`season_starts.c`), so the era replaces the Italian functions it calls, each only in the era of the season that starts (ItaSerA's year + 1: it adds 1 at 0x641789, after the chain):

1. The chain (vtable +0xb0 = 0x642410, thiscall (1) ret 4):
   - the statuses of each table (vtable +0xa4 of A, B and the groups, as 0x642423..0x642465);
   - A and B, 4 and 4 in pairs (as 0x64248e);
   - B and each group in pairs (as 0x6424b6 and 0x6424de). `league_promote_and_relegate` 0x689c80 moves the rows of status 3 of the upper league and of status 0 or 5 of the lower one, and in pairs a row moves only with a partner. So B's relegated clubs go to the groups whose rows go up, in the order A, B, C. Slice 18's wrap of 0x689c80 handles counts that differ;
   - the relegated rows (status 3) of each group go to the Serie D data division [0x9cf58c] (`0x6831a0` thiscall (club, record, 1) ret 0xc, as `exchange_serie_d_clubs` 0x642317..0x6423e6 does for C2). Q620: the original's own lowest tier pool, so the pool never runs dry;
   - at the change into the era (the season before was not in it): the clubs of C2/B and C2/C that their table does not relegate join group C's record, the relegated go to the Serie D, and both objects end.
2. `exchange_serie_d_clubs` 0x6421c0, `exchange_serie_c2_clubs_unplayed` 0x641fc0 and `regroup_serie_c1_clubs` 0x642630 do nothing in the era (0x641fc0 runs the regroup when there is no C2/A object).
3. The regroup (in place of `regroup_serie_c2_clubs` 0x642790, which reads 54 clubs and splits by 18):
   - the clubs of the three group records;
   - too few for the rows: clubs of the Serie D data division, shuffled with 0x4afbd0 (as 0x6422de), then Italian clubs with no division by reputation (Q622);
   - too many: the lowest reputations go to the Serie D;
   - sorted by latitude (`database_compare_clubs_by_latitude` 0x53fd40 with the game's qsort, as 0x642826..0x642833); the first go to group C, then B, then A (C2/C was the first of 0x64284a, the southern group);
   - a human manager whose club changes group gets the news of `regroup_serie_c1_clubs` (0x6426e5..0x642745: 0x7875e0, 0x77bbe0 (0x13, 0), 0x785790 twice, 0x7861e0).
4. Two patched loads of the C2/A object in 0x641700 (`new_divisions.py`):
   - 0x6418d5, the tier block (`League::limit_club_division_tier` 0x68a980 with (1, 3, -3, 1) on C2/A, C2/B, C2/C, else on C1/A, C1/B): in the era `rt_ita_tier_c2` calls it on C2/A and gives 0, so the block does C1/A and C1/B (0x641915). All three groups get it.
   - 0x641986, the start block (vtable +8 of C2/A, C2/B, C2/C, 0x64198d..0x6419b7): in the era `rt_ita_start_c2` starts C2/A and gives 0, so C2/B and C2/C are never started (0x6419ba). A C2/A that slice 26's module made in this pass already holds the new season, so it is started only when its year is older. When the regroup changed its count after the module made it, its year goes back one and it starts again, so its table takes the clubs of the group.
   Outside the era both give the object, so the original runs.

## A new game of the era

- Slice 15's club moves: the lists "Lega Pro girone A..C" and "Serie C girone A..C" go to the database divisions Italian Serie C1/A, C1/B and C2/A (`make_clubs.py` `TARGETS`; before, `SKIP_NEW`). The counts of these divisions are the group rows (`season_divisions_class_size`, which slice 20 calls in the club moves); C2/B and C2/C get 0, and the club moves read 0 as "the division ends" (its clubs go to no division, Q290). The season databases of 2014/15 on still hold the original's five groups of 18 and a Serie D of 177 to 204 clubs (October 2014, October 2016, December 2025, read from `club.dat`).
- Two clubs of the lists are not in their databases (one in 2020/21 group C, one in 2025/26 group B): slice 15 fills their places by its rules.

## The cups in the era

- `ItaCCup::select_entrants` 0x63dfe0 (Coppa Italia Serie C) sets 90 entrants (0x63dff4) and reads all 90 slots (0x63e109..). It takes the Italian clubs of C1/A to C2/C (0x63e094..0x63e107). The era's groups hold 60, and the constructor stopped at "Database..cpp 16398" (0x63ccce). Q625: a wrap makes the clubs that are short Serie D clubs by reputation, which count as C2/B for the call and go back to the Serie D after it.
- `ItaCup::select_entrants` 0x63faf0 (Coppa Italia) compares a club's last division with the C1/B id (0x63fc79, 0x63fcf9). In the era `rt_ita_cup_c1b` makes the C2/A id count as C1/B, so group C clubs take the lower places. Its second loop goes up through the last positions until it has 32 clubs and has no end (0x63fd22..0x63fd30). Q626: at 0x63fd28, from position 40 with fewer than 32 clubs, `rt_ita_cup_fill` fills the list from the group clubs by reputation.
- The cup reads each club's last division (+0x5b) and final place (+0x5f), and its sort 0x640590 reads the stats of that division (a club with no stats there stopped at "comp_stats.CPP 1664"). The cup can start before or after Serie A in the season change pass. So the chain and a wrap of 0x63faf0 both mark the clubs of each group whose table still holds the last season (its first row has played, row +0x11, as 0x689b51 reads it).

## Out of this part

- Promotion play-offs and play-outs of the groups (slice 19). Places come from slice 18's `set_up_rules` wrap with the rows.
- The real format of `ItaCCup` 0x63dfe0 (60 entrants of the groups, ita-235) and `ItaC1Super` (two C1 winners; from 2014/15 a triangular): slice 21. `ItalyAwards` 0x660ec0 gives division awards to C2 only with the flag (0x660f5b): without the flag group C gets none.
- The two-group C2 seasons 2011/12 to 2013/14 with the flag (C2/C not held): `regroup_serie_c2_clubs` still reads 54 clubs; not changed here.

## Decisions (recommended answers taken; for Max to confirm)

- Q620: groups A, B and C live in the objects of C1/A, C1/B and C2/A; the groups' relegated clubs go to the Serie D data division and the clubs that come up come from it, as the original's own lowest tier exchange (0x6421c0).
- Q621: 38 match days: the original's 34 plus 22 and 29 August and the Wednesdays 27 October and 9 March, every season.
- Q622: when the Serie D cannot fill the groups, Italian clubs with no division by reputation fill them; with too many clubs the lowest reputations go to the Serie D.
- Q623: C2/B and C2/C end in the era with year 2999 and no clubs (Q395), with or without the flag; they stay in the menus as empty leagues.
- Q624: the groups are split by latitude, the south first (group C), as the original's regroups do. The real groups are drawn by the league each summer.
- Q625: until slice 21, the Coppa Italia Serie C takes the clubs it is short of 90 from the Serie D by reputation.
- Q626: when the Coppa Italia finds fewer than 32 lower clubs by position 40, the group clubs fill its list by reputation.
