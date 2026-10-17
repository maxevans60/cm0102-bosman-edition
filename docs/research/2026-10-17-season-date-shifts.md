# Right ages and dates in every season (slice 11b)

Date 2026-10-17. Slice 11b of `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (Q344). Code: `src/recomp_rt/win/season_dates.c` and `season_dates.h`, the table `season_dates_table.c`. Tools: `tools/seasons/measure_date_shifts.py` (measures), `tools/seasons/make_dates_table.py` (C table). Data: `docs/research/rules-0.6.0/databases-date-shifts.tsv`, `databases-date-shifts-last-rows.tsv` and `databases-date-shifts-careers.tsv`. Test: `tests/seasons/season_dates_run.py`.

## Result

The Update Team databases keep the dates of a game that starts in 2001. The shift is not one number per database. It differs by field, and some databases mix shifted and unshifted rows inside one table. The game now adds a shift for each field at the database load of a new game in another season. With the shifts, every checked person of every season database has the real year of birth and the right age on the first day, and the last row of the competition history is the season before the start with the real champions.

Main facts:

- Dates of birth of the season databases from November 2007 on are early by the data season minus 2001 (November 2007: 6, December 2025: 24). September 2005 is early by 3, not 4. October 2003 is not early (shift 0).
- Contract ends are in the years of a 2001 start in every Update Team database, October 2003 and September 2005 too. The most common contract end years are 2002 to 2004 in all of them, as in the 3.9.68 data. Their shift is the data season minus 2001.
- Officials have shift 0 up to 2019/20 and 1 from 2020/21 for the referees of the 3.9.68 data. The referees that a database added later have early years with no common rule, and keep them.
- Competition history: from the 2010/11 databases on, every row is early by the data season minus 2001. Before that, the rows keep their real years, and the database writes the last champions into the row of the year 2001. Only that row moves, for the competitions that the database updated.
- Careers (staff history): from 2009/10 on, all careers follow the dates of birth. In September 2005 and in the 2007/08 and 2008/09 databases, most careers are the 3.9.68 careers with their real years, and only the careers that the database rewrote (the well-known players) are early. A list per database names the careers that keep their years.
- The award history (staff competition history) of the Update Team databases holds name ids that do not resolve to the right people. A year shift cannot make it right (Q350: no shift makes more than 1 % of the winners right), so it does not move.

## The fields

The data scan read every 16-bit value of every table of the 3.9.68 data and listed the offsets where more than 2 % of the rows hold a year (1850 to 2030). The table lists every such field. The record sizes and the order of the tables are those of the loader 0x528540 (`docs/research/2026-10-13-database-inventory.md`).

| Field | File | Offset | Evidence | Shifts |
|---|---|---|---|---|
| Date of birth | staff.dat | +0x10 (year +0x12) | The age 0x53ab90 subtracts the year at +0x12 from the game year and compares the day at +0x10. Data: Henry 17.8.1977 (day 228) | yes, birth |
| Age | staff.dat | +0x18 | Written each day of the run-up from 0x53ab90 (0x5cc850, the loop over [0xae23bc] that stores the result at +0x18). The file holds a year or 0 there | no: the game sets it before any screen |
| National job date | staff.dat | +0x29 | Written by the leave function 0x53c190 for a national job (`database_links.c`). Year 0 in every file row | yes, contract (when set) |
| National contract end | staff.dat | +0x31 (year +0x33) | 0x4c7560 (contract init) reads +0x31 and +0x33 for a staff row with a national club (+0x24): an empty date (31, 1900) gets a random end after the game year | yes, contract |
| Date joined | staff.dat | +0x3e (year +0x40) | Written by 0x53c190 for a club move. The staff pass compares it with 1 January 1998 (0x52d2d1). Data: Henry 3.8.1999 at Arsenal | yes, joined |
| Contract end | staff.dat | +0x46 (year +0x48) | 0x4c7560 reads +0x46 and +0x48: an empty date gets a random end, and an end within 8 weeks of the game date moves on by 52 weeks (the loop at 0x4c776d). Data: Henry 30.6.2004 | yes, contract |
| Official date of birth | officials.dat | +0x0c (year +0x0e) | Data: a year in 100 % of the rows, 1900 for an empty date | yes, officials |
| Career year | staff_history.dat | +0x08 | Data: the year where the season starts (Lampard 2001 Chelsea = 2001/02). The id pass links +0x04 to the staff row and +0x0a to the club (0x532b5b) | yes, careers, except the kept careers |
| Competition history year | club_comp_history.dat, nation_comp_history.dat | +0x08 | Data: the year where the season ends (3.9.68: 2002 Arsenal = 2001/02). One array in memory, club rows then nation rows (0xae23d4, count 0xae237c includes both). The id pass links +0x04 to the competition (0x532c11) | yes, competition history |
| Award history year | staff_comp_history.dat | +0x08 | Data: each place has 16 bytes from +0x0a (first name id, second name id, staff id, club id) | no (see the result) |

No other table has a year field: club.dat (the hits at +0x80 are the reputation, 500 times a byte), nat_club.dat, nation.dat (+0x8e is a reputation), stadium.dat, city.dat, the player and non-player tables, the competitions and the name tables. A date is 8 bytes: s16 day of the year, u16 year, u32 leap flag. The data writes the days of a common year with the flag 0 in every year (Gerrard 30.5.1980 is day 149), so a shift changes the year only, and the day and month stay as the screens show them. The year 1900 marks an empty date and never moves.

## How the shifts are measured

`measure_date_shifts.py` compares each database with the 3.9.68 data of the game folder, whose dates are real (the base game).

- Birth: the people with the same first name, second name and day of birth in both (names seen twice are left out). The shift is the most common difference of the years. In every database 96 % to 100 % of the matches agree. Well-known people confirm it: Rooney (real 1985) is 1982 in September 2005 and 1979 in November 2007.
- Joined: the matched people at the same club in both. October 2003 and September 2005 keep the real dates (shift 0, 66,472 and 57,897 matches, all agree). From November 2007 on almost every joined date is 2000 or 2001, so the shift is the contract shift.
- Officials: as birth, with the officials table.
- Contract: no rows to compare. The shift is the data season minus 2001. The most common contract end years of each database are listed in the table: they are those of a 2001 start in every database.
- Careers: each matched person's rows (year, club and the three bytes after the club) against his 3.9.68 rows. A career keeps its years when more rows match without the shift than with it. September 2005, November 2007, February 2008, September 2008, December 2008 and March 2009 have 25,505 to 70,220 such careers. They are in the careers file as one bit per staff row. From 2009/10 on fewer than 70 per database remain, and all careers move.
- Competition history: the rows (competition, year, first club) against the 3.9.68 rows. From 2010/11 on the best shift equals the birth shift (3,000 to 3,800 matching rows). Before that the best shift is 0, and the last champions are in the 2001 row. A competition counts as updated when the database has a 2001 row and the 3.9.68 data does not have the competition, or has another first club in 2001, or has a 2002 row that the database dropped. The ids of those competitions are in the last-rows file.
- The last champions of the English Premier Division and the European Champions Cup after the shift are in the table. All are the real champions of the season before the start.

## Shifts of each database

Years added. "Last rows": the shift of the 2001 history row and the number of competitions that it applies to. Every row of all 49 known databases is in `databases-date-shifts.tsv`.

| Database | Season | Birth | Joined | Contract | Officials | Careers (kept rows) | Competition history | Last rows | Birth matches (share) | Contract years | Last champions after the shift |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 3.9.68 | 2001/02 | 0 | 0 | 0 | 0 | 0 (0) | 0 | - | 117619 (1.00) | 2003 2004 2002 | English Premier Division 2002 Arsenal; European Champions Cup 2001 FC Bayern München |
| October 2003 | 2003/04 | 0 | 0 | 2 | 0 | 0 (0) | 0 | +2 for 74 | 115391 (1.00) | 2003 2004 2002 | English Premier Division 2003 Manchester United; European Champions Cup 2003 AC Milan |
| September 2005 | 2005/06 | 3 | 0 | 4 | 0 | 3 (70220) | 0 | +4 for 103 | 111731 (1.00) | 2003 2004 2002 | English Premier Division 2005 Chelsea; European Champions Cup 2005 Liverpool |
| November 2007 | 2007/08 | 6 | 6 | 6 | 0 | 6 (45760) | 0 | +6 for 53 | 84202 (0.96) | 2004 2003 2005 | English Premier Division 2007 Manchester United; European Champions Cup 2007 AC Milan |
| September 2008 | 2008/09 | 7 | 7 | 7 | 0 | 7 (28727) | 0 | +7 for 98 | 60607 (0.96) | 2004 2003 2005 | English Premier Division 2008 Manchester United; European Champions Cup 2008 Manchester United |
| September 2009 | 2009/10 | 8 | 8 | 8 | 0 | 8 (0) | 0 | +8 for 96 | 53440 (0.96) | 2004 2003 2005 | English Premier Division 2009 Manchester United; European Champions Cup 2009 F.C. Barcelona |
| September 2010 ODB | 2010/11 | 9 | 9 | 9 | 0 | 9 (0) | 9 | - | 51504 (0.96) | 2004 2003 2002 | English Premier Division 2010 Chelsea; European Champions Cup 2010 Internazionale |
| September 2011 ODB | 2011/12 | 10 | 10 | 10 | 0 | 10 (0) | 10 | - | 38785 (0.96) | 2003 2004 2002 | English Premier Division 2011 Manchester United; European Champions Cup 2011 F.C. Barcelona |
| September 2012 | 2012/13 | 11 | 11 | 11 | 0 | 11 (0) | 11 | - | 26515 (0.97) | 2002 2003 2004 | English Premier Division 2012 Manchester City; European Champions Cup 2012 Chelsea |
| Autumn 2013 | 2013/14 | 12 | 12 | 12 | 0 | 12 (0) | 12 | - | 19574 (0.97) | 2002 2001 2003 | English Premier Division 2013 Manchester United; European Champions Cup 2013 FC Bayern München |
| October 2014 | 2014/15 | 13 | 13 | 13 | 0 | 13 (0) | 13 | - | 15475 (0.97) | 2002 2001 2000 | English Premier Division 2014 Manchester City; European Champions Cup 2014 Real Madrid C.F. |
| October 2015 | 2015/16 | 14 | 14 | 14 | 0 | 14 (0) | 14 | - | 11165 (0.97) | 2002 2001 2003 | English Premier Division 2015 Chelsea; European Champions Cup 2015 F.C. Barcelona |
| October 2016 | 2016/17 | 15 | 15 | 15 | 0 | 15 (0) | 15 | - | 9914 (0.97) | 2002 1999 2001 | English Premier Division 2016 Leicester City; European Champions Cup 2016 Real Madrid C.F. |
| October 2017 | 2017/18 | 16 | 16 | 16 | 0 | 16 (0) | 16 | - | 8715 (0.96) | 2002 2001 2003 | English Premier Division 2017 Chelsea; European Champions Cup 2017 Real Madrid C.F. |
| October 2018 | 2018/19 | 17 | 17 | 17 | 0 | 17 (0) | 17 | - | 7858 (0.96) | 2002 2001 2003 | English Premier Division 2018 Manchester City; European Champions Cup 2018 Real Madrid C.F. |
| October 2019 | 2019/20 | 18 | 18 | 18 | 0 | 18 (0) | 18 | - | 5835 (0.97) | 2002 2001 2003 | English Premier Division 2019 Manchester City; European Champions Cup 2019 Liverpool |
| November 2020 | 2020/21 | 19 | 19 | 19 | 1 | 19 (0) | 19 | - | 5890 (0.96) | 2002 2003 2001 | English Premier Division 2020 Liverpool; European Champions Cup 2020 FC Bayern München |
| October 2021 | 2021/22 | 20 | 20 | 20 | 1 | 20 (0) | 20 | - | 6136 (0.97) | 2003 2002 2001 | English Premier Division 2021 Manchester City; European Champions Cup 2021 Chelsea |
| October 2022 | 2022/23 | 21 | 21 | 21 | 1 | 21 (0) | 21 | - | 6225 (0.97) | 2002 2003 2001 | English Premier Division 2022 Manchester City; European Champions Cup 2022 Real Madrid C.F. |
| October 2023 | 2023/24 | 22 | 22 | 22 | 1 | 22 (0) | 22 | - | 5936 (0.97) | 2003 2002 2001 | English Premier Division 2023 Manchester City; European Champions Cup 2023 Manchester City |
| October 2024 | 2024/25 | 23 | 23 | 23 | 1 | 23 (0) | 23 | - | 6003 (0.97) | 2002 2001 2003 | English Premier Division 2024 Manchester City; European Champions Cup 2024 Real Madrid C.F. |
| December 2025 | 2025/26 | 24 | 24 | 24 | 1 | 24 (0) | 24 | - | 6213 (0.97) | 2001 2002 2003 | English Premier Division 2025 Liverpool; European Champions Cup 2025 Paris Saint-Germain |

| Extra database | Season | Birth | Joined | Contract | Officials | Careers (kept rows) | Competition history | Last rows | Birth matches (share) |
|---|---|---|---|---|---|---|---|---|---|
| 3.9.60 | 2001/02 | 0 | 0 | 0 | 0 | 0 (0) | 0 | - | 89265 (1.00) |
| 3.9.65 | 2001/02 | 0 | 0 | 0 | 0 | 0 (0) | 0 | - | 92314 (1.00) |
| February 2008 | 2007/08 | 6 | 6 | 6 | 0 | 6 (41273) | 0 | +6 for 54 | 79217 (0.96) |
| December 2008 | 2008/09 | 7 | 7 | 7 | 0 | 7 (27282) | 0 | +7 for 58 | 57016 (0.96) |
| March 2009 | 2008/09 | 7 | 7 | 7 | 0 | 7 (25505) | 0 | +7 for 95 | 56381 (0.96) |
| April 2010 SIM | 2009/10 | 8 | 8 | 8 | 0 | 8 (0) | 0 | +8 for 99 | 46435 (0.96) |
| February 2010 ODB | 2009/10 | 8 | 8 | 8 | 0 | 8 (0) | 0 | +8 for 94 | 52408 (0.96) |
| June 2010 | 2009/10 | 9 | 8 | 8 | 0 | 9 (0) | 0 | +8 for 94 | 51904 (0.96) |
| April 2011 SIM | 2010/11 | 9 | 9 | 9 | 0 | 9 (0) | 9 | - | 36954 (0.96) |
| December 2010 SIM | 2010/11 | 9 | 9 | 9 | 0 | 9 (0) | 9 | - | 38156 (0.96) |
| February 2011 ODB | 2010/11 | 9 | 9 | 9 | 0 | 9 (0) | 9 | - | 50856 (0.96) |
| March 2012 ODB | 2011/12 | 10 | 10 | 10 | 0 | 10 (0) | 10 | - | 37170 (0.96) |
| March 2012 SIM | 2011/12 | 10 | 10 | 10 | 0 | 10 (0) | 10 | - | 33998 (0.96) |
| March 2013 | 2012/13 | 11 | 11 | 11 | 0 | 11 (0) | 11 | - | 25474 (0.97) |
| March 2014 | 2013/14 | 12 | 12 | 12 | 0 | 12 (0) | 12 | - | 17187 (0.97) |
| April 2015 | 2014/15 | 13 | 13 | 13 | 0 | 13 (0) | 13 | - | 14704 (0.97) |
| April 2016 | 2015/16 | 14 | 14 | 14 | 0 | 14 (0) | 14 | - | 10966 (0.97) |
| April 2017 | 2016/17 | 15 | 15 | 15 | 0 | 15 (0) | 15 | - | 9816 (0.97) |
| March 2018 | 2017/18 | 16 | 16 | 16 | 0 | 16 (0) | 16 | - | 8685 (0.96) |
| March 2019 | 2018/19 | 17 | 17 | 17 | 0 | 17 (0) | 17 | - | 7861 (0.96) |
| March 2020 | 2019/20 | 18 | 18 | 18 | 0 | 18 (0) | 18 | - | 5905 (0.97) |
| April 2021 | 2020/21 | 19 | 19 | 19 | 1 | 19 (0) | 19 | - | 6027 (0.96) |
| April 2022 | 2021/22 | 20 | 20 | 20 | 1 | 20 (0) | 20 | - | 6262 (0.97) |
| April 2023 | 2022/23 | 21 | 21 | 21 | 1 | 21 (0) | 21 | - | 5849 (0.97) |
| April 2024 | 2023/24 | 22 | 22 | 22 | 1 | 22 (0) | 22 | - | 6018 (0.97) |
| May 2026 | 2025/26 | 24 | 24 | 24 | 1 | 24 (0) | 24 | - | 6081 (0.96) |

## When the shift runs

`season_dates.c` wraps `database_load_files` 0x524d50, the load of the database files of a new game (0x528540 with the file source; callers 0x81ce00, 0x827110 and 0x828110). The shift runs when it returns, for the start year of that moment. So the whole loader runs first on the dates as the database stores them, as in a 2001 start: the reads, the staff conversion, the id pass 0x5315a0 and the staff pass 0x52d20a with its 1998 compare at 0x52d2e1. That compare then keeps its meaning relative to the start year. The fix of the Select All stop (Database.cpp line 1844, another slice) runs inside the loader, so before this shift. A saved game loads through 0x524d10 (the stream source) and is not touched: it holds the shifted data. Only the rows of the files move (the counts of index.dat at 0xae1a80), never the reserve rows.

Order with the Choose Season screen (tested, `second` run): the screen calls `season_set_new_game` before Setup Game returns, so the first Start New Game (0x81ce00) loads in the chosen year. 0x81ce00 loads only when no nation is loaded (it tests [0xae23a8] at 0x81ce06). After Back from Select League(s) the data stays in memory. Before this round, a second start in another season kept the data of the first choice (the October 2019 data in a 2010/11 game, in the database of the first tile) and stopped with a message box. Now a wrap of 0x81ce00 frees the tables with `database_free_all_tables` 0x528080 (it sets [0xae23a8] to 0) when the season or the database changed since the load, so the start loads the data of its own season and database. As a second guard, every change of a year is kept in a journal, and the creation of the game (0x828110, season.c) makes the shift again from the stored dates when the start year differs from the year of the load.

Nothing moves when the start year is 2001 (every database plays as in 0.5.0, the Update Team way) or with the test switch `RECOMP_SEASON_CODE=off`.

## The rules for a game in season N

- A known Update Team database in its own season (N = D, the data season of the signatures table): the measured shifts of the table.
- A known database in another season (for example October 2019 named 2010/11): the whole database moves by N minus D on top of its shifts. Every person keeps the age that the database gives him, and no history row lies after the start. Real dates of birth would put children of the 2019 squads into a 2010/11 game, and the 2019 champions into its history.
- The official data sets (3.9.60, 3.9.65, 3.9.68; D = 2001): dates of birth, joined dates, careers and history keep their real years. The contract ends and national job dates move by N minus 2001, so on the 2002/03 tile the 3,630 contracts that end in 2002 move to 2003 and do not end in the run-up.
- A Custom database: its shift d is measured at the load, and also for Setup Game, against the game's own Data. The game first checks that its own Data is the 3.9.68 data (signature). Then D = 2001 + d and the rules of the Update Team databases apply: dates of birth, joined dates and careers d; contracts by the contract rule; history d only when D is 2010 or later; officials +1 when D is 2020 or later; then N minus D on top. Below 2,000 matches (or 80 % that agree) nothing moves, and Setup Game shows "Ages could not be checked" under the database. The files are opened without regard to case, as files.c does.
- Contract ends (+0x46) and national job dates (+0x29, +0x31), per row. Some databases hold contract ends in real years beside the ends of a 2001 start (October 2019: 1,343 ends from 2019 to 2023; September 2008: 145 ends from 2009; September 2011: 92 ends from 2011). The ends of a 2001 start reach 2008 (3.9.68: 65 ends in 2008, 9 in 2009). So an end in the data season or later, and in 2009 or later, is a real year and moves by N minus D only; every other end moves by N minus 2001. No end lies more than 6 years after N: a later one becomes N + 6 (the outliers such as 2200 and 3076 too).
- Leap days: a date with the leap flag set that moves into a common year uses the days of the common table, and 29 February becomes 28 February. No year goes below 1901, so no date becomes the empty year 1900.

## What stays wrong (data, not code)

- Careers that a database did not update end in 2001 (for example Lampard in September 2005). The game cannot know the missing seasons.
- In October 2003 and September 2005, a player who moved after 2001 has the joined date 2001 (Rooney at Manchester United in September 2005). The other joined dates there are real, so they do not move.
- Before the 2010/11 databases, the competition history has no rows between 2001 and the season before the start.
- The award history (Q350): the rows whose winner names resolve to the 3.9.68 winners of the same award were compared for each shift from -2 to +29. September 2008: 8 of 1,070 rows with names match at +7 (5 at +6); September 2012: 8 of 793 at +7; October 2019: 4 of 700 at +7; December 2025: 5 of 695 at +8; September 2005: 17 of 1,602 at 0. No shift makes more than 1 % of the winners right, so no rule makes the winners right, and the years stay.
- Officials: the referees of the 3.9.68 data keep their real dates (0, +1 from 2020/21). The referees that a database added later have early years that differ per referee (October 2019: Michael Oliver 1973, real 1985; Anthony Taylor 1964, real 1978; September 2010: Howard Webb 1964, real 1971). No rule per database fits them, so they keep the years of the data.
- On the 2002/03 tile the 3.9.68 data has 3,630 contracts that end in 2002. They end in the run-up from 1 January 2002 to the start, as 571 contracts that end in 2001 do in the base game.

## Tests

`tests/seasons/season_dates_run.py <program> <game> <test-fixtures/databases>`: for each season database and the 2002/03 tile, a new game in its season. The checks: at least five well-known people with the real year of birth and the real age on the first day; the last English Premier Division row with the real champions of the season before the start; 95 % of the contract ends and 80 % of the national job dates from N - 2 to N + 6; the latest joined date in N - 1 or N and a quarter of them there (not for October 2003, September 2005 and 3.9.68, whose joined dates are real); the latest career year from N - 2 to N. Each field check fails when the shift of that field is left out (from the 2007/08 databases on). Also: `second` (a start in 2019/20, Back, the 2010/11 tile, a start again: the 2010/11 data and dates), October 2019 in a 2001/02 game moves nothing, October 2019 with `RECOMP_SEASON_CODE=off` moves nothing, and the 2010/11 game gives the same save twice. `ctest -R season_dates_table` checks that the C table is the one that the tables give.

Results of the first round on 2026-10-17: `season_dates_run.py --jobs 3` passed for all 21 season databases and the 2002/03 tile (9 to 13 of the 17 people right in each, the real last champions in each), and for base, off and again. The same-save checks with `--runs again,season-code-off --jobs 3 --ref-build <0.5.0>` passed for a new game, `start.sav`, `prematch.sav` and `yeovil.sav` (7 days), and `--match --runs again,season-code-off` passed. The age is the one of 0x53ab90: it compares the day of the year, so in a leap year a birthday on the first day of the game can count one day early, as in the original.
