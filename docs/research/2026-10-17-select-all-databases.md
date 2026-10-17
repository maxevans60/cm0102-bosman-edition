# Select All with the season databases

Date 2026-10-17. 0.6.0 work. The question: why do some Update Team season databases stop at the error box "Database.cpp 1844" when every league is selected (Select All), and how does the runtime fix it without a change to the database files or to the base game.

## Result

- 7 of the 21 season databases stop at "Database.cpp 1844" with Select All: November 2007, September 2008, September 2009, September 2010 ODB, September 2011 ODB, September 2012 and Autumn 2013. The start year does not matter.
- No single record breaks the check. The new game needs more blank staff rows than the database load reserves. The load reserves 23,000 rows. These 7 databases need 23,664 to 27,156.
- Fix: the reserve is a run-time value, `rt_staff_reserve` (`tools/recomp/staff_reserve.py`, `src/recomp_rt/win/staff_reserve.c`). It is 23,000 for the official data sets and 46,000 for a custom data set (`database = folder:<name>`). The base game allocates and plays as before: the same-save checks against 0.5.0 pass.
- The fix adds no club and no player. It lets the original make the coaches, scouts, physios and chairmen that it already makes for every other database. The database files on disk do not change.
- With the fix, all 21 databases pass the staff step. With a 2001 start all 21 reach the first news screen and holiday. In its own start year every database then stops at a second box, "fra_first..cpp 108". That box comes from the French First Division size, not from the staff reserve (see "Second stop").

## The assert

The box text is "Error: v3.9.68 Database.cpp 1844". Line 1844 is 0x734.

`database_generate_extra_non_playing_staff` 0x527240 (Database.cpp, called only from `database_trim_staff_tables` 0x524d70 in game creation) makes extra non-playing staff for clubs of the selected nations. It has four loops over the club table (0x245 bytes a club):

| Loop | Club slots | New staff job (+0x3d) | Count per club | Limit check | Box line |
|---|---|---|---|---|---|
| 1 | 5 at club +0x19f | 8 | `(rep + 500) * 5 / 10000`, 1 to 5 | 0x52747b `cmp [esp+0x10], 0x59d8` | 0x65f (1631) |
| 2 | 7 at club +0x1b3 | 9 | `(rep + 500) * 7 / 10000`, 1 to 7 | 0x52777c | 0x6aa (1706) |
| 3 | 3 at club +0x1cf | 10 | `(rep + 500) * 3 / 10000`, 1 to 3 | 0x527a77 | 0x6f5 (1781) |
| 4 | club +0xcf empty | 5 (also +0x28 = 5) | 1 | 0x527d43 `cmp ebx, 0x59d8` | 0x734 (1844), box at 0x527e4c |

`rep` is the club word at +0x80. Loops 1 to 3 fill only the empty slots of a club whose nation is selected (nation +0x11c not 0, through club +0x53) and for which a test through 0x540a50 fails (its meaning is not read here). Loop 4 adds one staff member to each club with an empty +0xcf. Inference from the job value 5 and the count of one per club: loop 4 makes chairmen. The job meanings of 8, 9 and 10 are not proven here.

Each new staff member takes the next blank row: index `staff_count - n - 17` for the n-th new row (0x527489..0x527494). These are the reserve rows of the database load. The counter n is shared by the four loops. When n reaches 23,000 (0x59d8), the loop that runs shows its box and the function returns 0, so game creation stops.

## The reserve

`load_database` 0x528540 sizes the tables from index.dat:

- staff: count + 16 + 0x5f05 (24,325 = 23,000 + 1,325) at 0x529b39, or + 0x52d when the load is not a new database (the flag at `cStack_81d`, a load of a saved game),
- non-players: count + 16 + 0x59d8 (23,000) at 0x529bd1,
- then the 1,325 spare staff rows are taken off the count again (0x52a04b),
- the 23,000 blank rows get their default values in the loop at 0x52a1c2 (`cmp esi, 0x59d8`), each with its own non-player row (`docs/research/2026-10-14-game-creation-inventory.md`, section 2).

The database loads at "Loading database", before Select League(s). So the reserve size is fixed before the player selects any nation, and it cannot depend on the selection.

The constant 0x59d8 is in the code at these six places only (0x52747f, 0x527780, 0x527a7b, 0x527d45, 0x529bd4, 0x52a1c4, a byte search of the .text section). 0x5f05 is only at 0x529b3c. No other code holds the reserve size.

## Reserve rows used with Select All

From the test log of `staff_reserve.c` (it counts the reserve rows that are not blank after 0x527240). Each database in its own start year. The counts per loop are the rows with job 8, 9, 10 and the rest.

| Database | Start | Rows used | Job 8 | Job 9 | Job 10 | Chairmen | Over 23,000 |
|---|---|---|---|---|---|---|---|
| October 2003 | 2003 | 22,809 | 5,572 | 8,033 | 5,419 | 3,785 | no |
| September 2005 | 2005 | 22,908 | 5,585 | 8,039 | 5,434 | 3,850 | no |
| November 2007 | 2007 | 23,664 | 5,731 | 8,325 | 5,672 | 3,936 | yes |
| September 2008 | 2008 | 24,483 | 5,845 | 8,515 | 5,941 | 4,182 | yes |
| September 2009 | 2009 | 24,636 | 5,891 | 8,396 | 6,033 | 4,316 | yes |
| September 2010 ODB | 2010 | 24,926 | 5,961 | 8,470 | 6,101 | 4,394 | yes |
| September 2011 ODB | 2011 | 25,779 | 6,123 | 8,712 | 6,251 | 4,693 | yes |
| September 2012 | 2012 | 27,156 | 6,484 | 9,139 | 6,601 | 4,932 | yes |
| Autumn 2013 | 2013 | 24,818 | 6,074 | 8,181 | 6,130 | 4,433 | yes |
| October 2014 | 2014 | 22,758 | 5,514 | 7,486 | 5,610 | 4,148 | no |
| October 2015 | 2015 | 21,820 | 5,283 | 7,093 | 5,346 | 4,098 | no |
| October 2016 | 2016 | 21,844 | 5,289 | 7,091 | 5,372 | 4,092 | no |
| October 2017 | 2017 | 21,841 | 5,379 | 7,181 | 5,464 | 3,817 | no |
| October 2018 | 2018 | 22,245 | 5,485 | 7,327 | 5,588 | 3,845 | no |
| October 2019 | 2019 | 19,488 | 4,771 | 6,540 | 4,929 | 3,248 | no |
| November 2020 | 2020 | 19,324 | 4,745 | 6,449 | 4,915 | 3,215 | no |
| October 2021 | 2021 | 14,499 | 3,435 | 4,960 | 3,703 | 2,401 | no |
| October 2022 | 2022 | 14,902 | 3,564 | 4,959 | 3,828 | 2,551 | no |
| October 2023 | 2023 | 14,776 | 3,541 | 4,911 | 3,865 | 2,459 | no |
| October 2024 | 2024 | 15,187 | 3,645 | 4,982 | 3,970 | 2,590 | no |
| December 2025 | 2025 | 15,542 | 3,703 | 5,042 | 4,049 | 2,749 | no |

The 3.9.68 data with Select All uses 22,623 rows. Inference: the original sized the reserve for its own data.

In every failing database, loops 1 to 3 together stay below 23,000 (September 2012: 22,224). So the counter reaches 23,000 in loop 4, and the box is always line 1844. This agrees with the box that the slice 11 agent saw.

Why these 7: their clubs have more empty staff slots than the reserve holds. The club count alone does not decide it: the 7 hold 12,967 to 14,489 clubs (3.9.68: 13,214), and the databases from October 2014 hold 10,149 to 11,851. The per-loop counts above show the empty slots directly.

## The fix

`tools/recomp/staff_reserve.py` replaces the constant at the seven sites with `rt_staff_reserve` (rt.h, rt.c). The build stops when a site no longer holds its constant (`translate.py`, `widescreen.py` merge).

| Site | Instruction | New value |
|---|---|---|
| 0x529b39 | `lea ecx, [eax + ebp + 0x5f05]` | `rt_staff_reserve_size() + 0x52d` |
| 0x529bd1 | `lea eax, [eax + ebp + 0x59d8]` | `rt_staff_reserve_size()` |
| 0x52a1c2 | `cmp esi, 0x59d8` | `rt_staff_reserve` |
| 0x52747b, 0x52777c, 0x527a77 | `cmp dword [esp + 0x10], 0x59d8` | `rt_staff_reserve` |
| 0x527d43 | `cmp ebx, 0x59d8` | `rt_staff_reserve` |

`rt_staff_reserve_size()` (staff_reserve.c) sets the size for the load that runs: 23,000 unless the option `database` is `folder:<name>`, then 46,000. Both sizing sites call it, as the order of the entries in index.dat decides which runs first. The test switch `RECOMP_STAFF_RESERVE=<rows>` (23,000 to 400,000) sets any size. With `RECOMP_SCREEN_LOG=1` a wrap of 0x527240 prints the rows used.

Why 46,000: twice the original. The most that a season database needs is 27,156 (September 2012). Inference from the flag of the sizing branch (`cStack_81d`, game creation inventory): a load of a saved game takes the + 0x52d path at 0x529b42 and skips the blank rows (the jump at 0x52a1bc), so the size does not change a load of a saved game.

Effects:

- The official data sets (3.9.68, 3.9.60, 3.9.65) keep 23,000: the same memory, the same game.
- A custom data set gets 23,000 more blank staff rows and non-player rows (about 4 MB). Its memory layout changes against 0.5.0, so a game with a custom data set does not repeat a 0.5.0 game with the same clicks (Q152 applies to the base game).
- When slice 13 or 14 starts a season database by another path than the option `database = folder:<name>`, `rt_staff_reserve_size()` must also give 46,000 for that path.

## Second stop: the French First Division

With the fix, every one of the 21 databases in its own start year stops at "Error: v3.9.68 fra_first..cpp 108" (box from 0x5bb36d in `FraFirst::FraFirst`). `FraFirst::create_table` 0x5bc2b0 sets the size to 18 when the season (+0x40) is 2001 (0x5bc2bd), else 20. It then counts the clubs whose division (+0x57) is the French First Division and returns 0 when the count is not that size.

Every one of the 21 databases holds 18 clubs in the French First Division and 20 in the Second Division (club.dat +0x57 against club_comp.dat). Inference: the databases are made for a game that starts in 2001/02. So a start year that is not 2001 gets size 20 and stops. This is the known case of `docs/research/rules-0.6.0/fra.md` ("Every season database holds 18 clubs in the French First Division"): the club moves of slice 15 move 2 clubs up for 2002/03 to 2022/23, and slice 18 takes the size from the rules table (18 from 2023/24). It is not a staff reserve fault. Note for the lead: `2026-10-17-first-season-branches.md` (Checks) says that a season database of 2002/03 to 2022/23 has 20 Ligue 1 clubs. The data shows 18 in all 21 databases.

## Checks

Commands from the repo, with `G=/Users/max/Projects/cm0102/clean-3968`, `DB=/Users/max/Projects/cm0102/test-fixtures/databases`, `REF=/Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp`:

- Each database in its own start year: `python3 tests/smoke/update_team_sweep.py build/cm0102_recomp $G $DB --select-all --jobs 3 --timeout 2400`. Result: no "Database.cpp 1844" box. All 21 stop at "fra_first..cpp 108" (table above).
- Every database in 2001: the same with `--start 2001`. Result: 21 of 21 pass. Each game reaches the first news screen and holiday, and each uses the same reserve rows as in its own start year.
- Before the fix: the main build (`/Users/max/Projects/cm0102/cm0102-mac/build/cm0102_recomp`) with September 2012, Select All, 2001: the box "Error: v3.9.68 Database..cpp 1844" from 0x527ea3, the box call of the 0x527e4c block.
- Same-save checks against 0.5.0 (the base game): `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --runs normal,again --ref-build $REF`, the same with `--save prematch.sav`, without `--save` (a new game) and with `--save yeovil.sav --days 7`. Result: all four PASS, the saves equal the 0.5.0 saves byte for byte. `--runs` (new in `smoke_run.py`) plays only the named runs, so that each check plays 3 games on a busy machine. This change has no option, so the option pairs of `SAME_SAVE_RUNS` do not apply.

## Unknowns

- The meaning of jobs 8, 9 and 10 and of club +0xcf. The names coach, scout, physio and chairman are inferences from the slot counts.
- Whether a later season needs blank reserve rows (for example for new staff). The 3.9.68 data with Select All leaves 377 blank rows, so the original does not seem to need many. Not proven.
