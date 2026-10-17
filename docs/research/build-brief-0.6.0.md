# Brief for the 0.6.0 build sessions

Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (Decisions, with the research decisions Q268 to Q339). Read it first, then `GLOSSARY.md` (use its words) and `docs/HANDOVER.md`. For the REA tools and their traps, read `docs/research/agent-brief.md`. Build only your own slice. Function map names (`docs/research/function-map/`) are hints: read a function's code before you rely on it, and cite its address.

## Shared facts
- Repo `/Users/max/Projects/cm0102/cm0102-mac`, GitHub `maxevans60/cm0102-bosman-edition`. You work in your own worktree on your own branch. **First run `git merge main`**: a worktree can be cut from an older commit. Commit on your branch. Do not push. Do not merge into `main`: the lead merges after review.
- The original: `/Users/max/Projects/cm0102/clean-3968/cm0102.exe` (3.9.68). Game files: `/Users/max/Projects/cm0102/clean-3968`. Never use the fan pack in `/Users/max/Downloads/Game`.
- Build: `cmake -S . -B build -DCM0102_EXE=/Users/max/Projects/cm0102/clean-3968/cm0102.exe -DRECOMP_PYTHON=/Users/max/.venvs/cm0102/bin/python`, then `cmake --build build -j 6 --target cm0102_recomp`. Game code changes go through the checked mechanisms (`RT_WRAP`, `tools/recomp/callbacks.py`, `tools/recomp/callsites.py`, `INSNS` entries as in `tools/recomp/widescreen.py`): the build must stop if an address no longer matches.
- Builds to compare with (do not rebuild them):
  - main: `/Users/max/Projects/cm0102/cm0102-mac/build/cm0102_recomp`
  - 0.5.0: `/Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp` (tag v0.5.0). The base game rule compares against this one.
- Scripted runs: `RECOMP_SCRIPT` steps with wait steps (`src/recomp_rt/win/waits.c`). `RECOMP_SCREEN_LOG=1` prints each screen change. Never write to Max's save folder: use `RECOMP_USER_DIR=<new folder from mktemp -d>`. `tests/fixtures/fixtures.py play` loads a saved test game and can dump screens.
- Saved test games in `/Users/max/Projects/cm0102/test-fixtures/`: `start.sav` (Arsenal 14.7.01), `prematch.sav` (16.7.01, Basel friendly 20.7.01), `yeovil.sav` (20.8.02, a copy of Max's save; never touch his own save).
- Update Team databases: `/Users/max/Projects/cm0102/test-fixtures/databases/<name>/` (49 unpacked folders) and `.../databases/zips/` (the original zips). Content signatures of all 49: `docs/research/rules-0.6.0/databases-signatures.tsv` (sizes and SHA-256 of `index.dat`, `club.dat`, `staff.dat`). Season to database: `docs/research/2026-10-16-season-database-table.md`. A database already loads as a custom data set: copy its folder into `Databases` of the user folder and set `database = folder:<name>` (`dbfolders.c`, `tests/smoke/custom_database_run.py`).
- Rules table: `docs/research/rules-0.6.0/<part>.tsv` (20 parts, one row per league or competition per season, columns in the first line), club moves in `<part>-clubs.tsv`, academy sides in `eng-academy.tsv`. The `.check*`, `.reconcile` and `.md` files are the research evidence.
- Code sweeps: `docs/research/2026-10-16-sweep-season-end.md` (end-of-season step, year constants, transfer rules), `2026-10-16-sweep-rulebooks.md`, `2026-10-16-sweep-europe.md`, `2026-10-16-sweep-international.md`, and `2026-10-16-season-change-points.md`.
- Options: `src/recomp_rt/win/options.c` and `options.h` (one row per option), test `ctest -R options` in `build/`. The `add-option` skill (`.claude/skills/add-option/`) gives the steps.

## Same-save checks (every slice that touches game code)
Use the `same-save-check` skill (`.claude/skills/same-save-check/SKILL.md`). With `G=/Users/max/Projects/cm0102/clean-3968` and `REF=/Users/max/Projects/cm0102/wt-ref-0.5.0/build/cm0102_recomp`:
- **Q342 (Max, 2026-10-17):** from slice 13 on, the Setup Game text moves memory, so the base game is compared with the main build after slice 13, not with 0.5.0. Every slice adds its runs with the test switch `RECOMP_SEASON_CODE=off` (it turns the slice 11 year sites and the later season code off): with the season at 2001/02 and `changing_rules` off, the saves must be equal.
- **Base game against 0.5.0 (slices 11, 12 and 14 only):** `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --ref-build $REF`, the same with `--save prematch.sav`, and `--same-save` without `--save` (a new game, `changing_rules = off`). Every save must equal the 0.5.0 save byte for byte.
- **Yeovil:** the same with `--save yeovil.sav --days 7 --ref-build $REF`. If `--save` does not take a save outside the start list, add that to `smoke_run.py` in its own small block and say so.
- **Repeatability of a moved season:** a new game in 2010/11 (September 2010 ODB) twice with the same steps gives equal saves.
- A new switch adds itself to the `off` run of `SAME_SAVE_RUNS` and a pair `<name>-on` and `<name>-off` to `SAME_SAVE_RUNS` and `MATCH_RUNS`.
- Rule (Q152): no feature may change the game's decisions while the player makes the same clicks. If a design would change the base game's decisions under the same steps, stop and report.

## Slices in this batch

### Slice 11, season core (Opus). One-way door: it fixes the save format.
Research: `2026-10-16-sweep-season-end.md` sections 2 and 3 (start date sites 3.1, the 99 year sites 3.2, the pushes 3.3), `2026-10-16-season-change-points.md` section 2.

1. **Start year at run time.** A new runtime module `src/recomp_rt/win/season.c` and `season.h` holds the start year of the game (default 2001) and `changing_rules`. API for later slices: `season_start_year()`, `season_changing_rules()`, `season_set_new_game(int start_year, int changing_rules)` (called before Start New Game; slice 13 calls it from the Choose Season screen). Until slice 13, a test-only variable `RECOMP_START_SEASON=<year>` sets the start year of a new game.
2. **Start date.** Set the game date to 1 Jan of the start year at 0x81b938 and 0x8304ec, and move the Europe clamp at 0x83058a, 0x830597, 0x8305db with the start year. A 2010/11 start begins in July 2010.
3. **First-season branches (Q170, Q239).** Each F site of section 3.2 compares with the start year in place of 2001 (2002 and 2003 sites with start year + 1 and + 2). With start year 2001 each site must give the same result as today. Write a table in `docs/research/2026-10-17-first-season-branches.md`: each site, what it does, how it moves, and for a structure branch (club counts such as Ligue 1 18 or 20, sizes of 16, 18, 20, entrant counts) the value the rules table gives for later seasons. Where a moved structure branch breaks a later season (for example a first-season size of 18 against a database with 20 clubs), take the later-season path when the start year is not 2001 and list it for slices 15 and 18. The rules table wins over a moved branch (Q239), but slice 11 does not read the table yet.
4. **Fixed 2001/02 dates (S sites, section 3.3).** The Italian fixed fixture lists, the fixed World Cup qualifier fixtures and the other absolute 2001 dates must not put fixtures in the past in a moved season. Find what the code does without them (for example the generic fixture builder) and use that when the start year is not 2001. List what you leave for later slices.
5. **Save field.** The season and `changing_rules` go in the save. Old saves (0.5.0 and earlier, and every base game save) must stay byte for byte as they are. Design: a new part (suggested name `bosman_season.dat`) in the save container (see `save_parts` in `tests/smoke/smoke_run.py`: three words, then rows of 268 bytes) that is written **only** when the start year is not 2001 or `changing_rules` is on. Its absence means 2001 and off. The part starts with a format version and its length, so later slices can add fields. Prove from the original's load code (cite addresses) that the loader skips a part it does not know, or else hook the loader. Prove which writer to use: the runtime can add the part when the game renames `temp_save_file.sav` (`files.c`), or a hook in the save writer. Write the proof and the format in `docs/research/2026-10-17-season-save-part.md`. At a load, set the start year and `changing_rules` from the part (or 2001 and off without it) before any game code runs. At the end of a game (0x8304ec) and at a new game, reset them.
6. **Option.** `changing_rules = on|off`, default on, the default for a new game (Q232, Q245). Add it with the `add-option` skill, to `ctest -R options` and to the same-save lists. The new-game same-save runs of `smoke_run.py` set it off. The rules themselves come in later slices: with this slice, on and off give the same game except the save part.

Test seam: the same-save checks above against 0.5.0 (start.sav, prematch.sav, new game, Yeovil 7 days); a 2010/11 new game with September 2010 ODB that shows a July 2010 date on the first news screen, holidays 14 days without a crash and has no fixture before the start date; the repeatability check; a save of a 2010/11 game that loads again with start year 2010; a test that a base game save has no season part. Acceptance checks: slice 11 in the spec.

### Slice 14, season databases in the download (Sonnet)
Research: `2026-10-16-season-database-table.md`, `rules-0.6.0/databases-signatures.tsv`, `tools/release/make_release.sh`.

1. **Release input.** The release script takes a new `--season-dbs <folder>` (the zips folder above). For each of the 21 season databases of the table (not 2001/02 and 2002/03, which use the official data sets), it writes a new zip that holds only the 20 data files (`dbfolders.c` lists them) in a folder with the neutral name of the signatures file (for example `October 2003`). No readme, no other file and no person's name from the source zip goes in the download. It checks each zip against the signatures file and stops on a mismatch. The zips go in `Seasons/` in the game folder of each download (Mac, Windows, Linux). Report the total size.
2. **First pick.** A runtime function, for example `seasons_db_prepare(season)` in a new `src/recomp_rt/win/seasons_db.c` and `.h`, returns the folder of a season's database: if `Databases/<name>` in the save folder (`win_save_folder`, `dbfolders.h`) holds that database (by signature), use it; else unpack `Seasons/<name>.zip` there once, check the signature and return it. A failed or half unpack leaves no folder that looks whole (unpack to a temporary folder, then rename). Use a small zip reader in C (stored and deflate; the build has zlib if `find_package(ZLIB)` works on all three platforms, else a vendored inflate with its licence). Windows paths and Mac paths both.
3. **Recognition (Q264, Q271).** `seasons_db_identify(folder)` gives the known database of a folder by the sizes and SHA-256 of `club.dat` and `staff.dat` (not `index.dat`, not the folder name), or "custom". A copy of the signatures table goes in the code or in a data file of the download. Slice 13 uses it for the version choice and the Custom database tile.
4. Interface for slice 13 in `seasons_db.h`: the list of 25 seasons with their database name, era, status (official, in the download, unpacked, no database) and the known extra databases present in `Databases` for each season.

Test seam: a C or Python test that unpacks one season zip into a new user folder, checks the signature, and the second call uses the unpacked copy without a new unpack; `tests/smoke/update_team_sweep.py` on the 21 unpacked season databases (each reaches the first news screen, run in parallel, `--jobs 6`); a dry run of the release script with `--season-dbs` (no `--publish`). Never commit the databases or the zips.

### Slice 11b, right ages and dates in every season (Opus), after slice 11
Research: Q344 in the spec; `/Users/max/Projects/cm0102/p2004-work/FINDINGS.md` section 3 (outside the repo, for the facts only): the Update Team databases store dates early (Rooney is born 1979 in November 2007, 1982 in September 2005; real 1985), the staff fields +0x12, +0x18, +0x33, +0x40, +0x48 (when the year is 1901 or later), staff_history +8, club_comp_history and nation_comp_history years, and staff competition history in part. Do not use any outside program's code; find every field yourself from the 3.9.68 load code and the data map (`docs/research/data-map/`).

1. Find every date and year field in the 20 data files whose value depends on the start year (dates of birth, contract start and end, joined dates, history years, first and last caps, club founded years if the game ages them, and so on). Write a table with the field, the file, the offset, the evidence address and whether it shifts.
2. Measure the shift of each database: compare the year of birth of the players present in both the 3.9.68 data and that database (the shift is the most common difference after the real age gap is removed; check with several well-known players), and check the history years against the real champions. Do this for the 21 season databases, the 3.9.68 data on the 2002/03 tile, the 2001/02 versions and the extra databases. Mixed shifts inside one database (the source above says staff competition history is mixed) are measured per table or per row kind. Keep the shifts in the database table of `seasons_db` and in a doc `docs/research/2026-10-17-season-date-shifts.md`.
3. Apply the shift in memory at the database load of a new game, before any game code reads the data, in a new file (for example `src/recomp_rt/win/season_dates.c`). A loaded save needs nothing: it holds the shifted data. The base game has shift 0 and must give the same save as now (`RECOMP_SEASON_CODE=off` must turn it off too). Custom databases: shift by the season the player names, minus the season the data was made for when it can be measured, else 0; write the rule in the doc.
4. Test: for each season database, a game run that starts in its season and checks on screen (player profile) or in memory that at least five known players have the real year of birth and the right age on the first day, and that the club history shows the real last champions. Repeatability of a 2010/11 game.
Another agent fixes the Select All stop at load time in memory (Database.cpp 1844). Keep your code in its own file and call, and say where it runs relative to the load.

### Slice 11c, competition history between 2001 and the start season (Opus), research now, code after slice 11b is on main
Spec: Q347, slice 11c. Research process: the `league-research` skill (`.claude/skills/league-research/`), as for the rules table (Q243, Q328): each fact has a source URL read on the day and a second-source check, and disagreements go to Max.

1. For each season database before 2010/11 (October 2003, September 2005, November 2007, September 2008, September 2009, and the 3.9.68 data on the 2002/03 tile) list the competitions with history rows in `club_comp_history.dat` and `nation_comp_history.dat`, which seasons each one already holds after the slice 11b shift, and which seasons from 2001/02 to the season before the start are missing. Read the record layout from the 3.9.68 load code and the data map, and cite addresses.
2. Research the missing rows: the fields the history rows hold (winner, runner-up, other places if the record holds them) for each missing season. Write them in `docs/research/history-0.6.0/` as TSV with sources, one part per nation or confederation, plus a check file.
3. After slice 11b is on main (`git merge main`, check the log): add the rows in memory at a new game, after the 11b shift, matching clubs and nations by their database names (list every name that does not match). The database files stay unchanged. The base game is untouched and `RECOMP_SEASON_CODE=off` turns it off.
4. Test: for each of these databases, a game run that opens the history of a few competitions (English league and cup, Champions League, a smaller nation) and checks every season up to the start with the right winner; the same-save checks of this brief.

### Slice 12, rules table reader and rules report (Opus), after slice 11 is on main
Research: the rules table parts, `2026-10-16-sweep-season-end.md` section 2.4 (what a jump with no days played needs), the league and cup sweeps.

1. **Table in the download.** A build step (for example `tools/seasons/make_rules.py`) turns the 20 parts into one file for the game, with the structured columns (id, season, nation, comp_file, comp, tier, name, clubs, up, down, playoffs, split, format, subs_used, bench) as values and the other columns kept as text for slice 17. It stops on a row it cannot read. The file goes in the download. The game reads it at the start (`season_rules_*` API in a new runtime module).
2. **Rules report (Q265).** A start flag of the test program (for example `RECOMP_SEASON_JUMP=<last year>` with `RECOMP_RULES_REPORT=<file>`) starts a new game in a season (slice 11 API), then jumps from season to season with no days played up to the named year. The step needs match results: use a quick-result stub in place of the match engine inside the real day loop, as section 2.4 says, so prize money, history and records run. At each season it writes the competitions the game holds (name, file or class, season year +0x40, club count, rounds, substitutes) to the report. A test (`tests/seasons/rules_report_check.py`) compares the report with the rules table and lists each difference.
3. This slice changes no rules. The report of the base game must match the original's competitions and sizes. The jump from 2001/02 to 2027/28 must finish without a crash. The flag must not change a normal game: the same-save checks pass.

### Slice 13, Choose Season screen (Opus), after slices 11 and 14 are on main
Research: the mockup `docs/research/mockups/2026-10-15-season-picker.html` (the third screen, "A3 + both"), spec Decisions "Seasons and databases" and "Eras", `database.c` (Setup Game menu, the free cell for the button), `options_screen.c` (Options screen, Game tab).

- The screen in the chosen layout: 25 tiles under the five era headings in the era colours (Q166, Q240), a status tag on each tile, a star on seasons with saved games, the strip with the chosen season, its database and the start date, greyed tiles for 2004/05 and 2006/07 with "No database yet", the version choice when extra databases of that season are in `Databases` (and 3.9.60 and 3.9.65 for 2001/02), the Custom database tile where the player picks a folder and names its season, the link to the Update Team's downloads page at champman0102.co.uk, and the credit of each database.
- **Credit (Q269):** never write a person's name in the code or any committed file. Every season database shows "Database by the CM 01/02 Update Team" (Q340). Keep the credit text in one table, so Max's answer is a one-line change.
- Setup Game shows the season, its database and the `changing_rules` switch for this game, and has no Database button any more (Q236). An old `database = 3.9.60`, `3.9.65` or `folder:<name>` in the settings file still starts that data set. The Options screen's Game tab shows the default of `changing_rules` (Q259).
- Each tile starts its season database through `seasons_db_prepare` and `season_set_new_game`.
- Player-facing text: plain words, British English, no exclamation marks. Screenshots in classic and widescreen (HD mode is removed, Q267).
- Test seam: a smoke run that opens the screen, picks a tile, starts the game and checks the first news date and the database loaded; one run per era; the same-save checks (a base game started through the screen with `changing_rules` off equals the 0.5.0 new game).

## Files that two slices share
- `options.c`, `options.h`, the `ctest` options cases: slices 11 and 13. Keep each change in its own small block.
- `SAME_SAVE_RUNS` and `MATCH_RUNS` in `smoke_run.py`: slices 11 and 13.
- `database.c`, `dbfolders.c`: slices 13 and 14. Slice 14 adds new files and only small hooks in these.
- `tools/release/make_release.sh` and `CMakeLists.txt`: slices 12 and 14.

## Speed
- Run test games in parallel. If a parallel run fails on timing, rerun it alone before you call it a bug (`uptime`: above about 20 runs fail).
- While you build, run only the test your change touches. Run the full set once at the end (the `same-save-check` skill lists it).
- Send long decompiles to a subagent: REA returns each listing twice.

## Rules
- Never name another modder, their patch or tool in any committed file, comment, commit message or test name. Some source zip and folder names hold a person's name: they never go in the download or the repo.
- Never commit the original's code or data, the databases, saves, screenshots or the logo.
- No hidden value may appear on a screen or feed a shown number.
- Every option is a line in the settings file, read through the existing reader.
- Do not run `rm` with a wildcard or on a path in a variable. Make new folders with `mktemp -d`.
- Test first: write the failing check at the spec's seam, then the code.
- Plain STE English in docs, comments and commit messages. End commit messages with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Research: give every fact its address, mark inferences, write "unknown" rather than guess.
- Tick your slice's acceptance checks in the spec on your branch only when they pass.
- If a decision is Max's, stop and report it with a recommended answer. Do not guess.
- Final reply: at most 300 words: the branch name and last commit, the files you touched, what works, what does not, test results with the exact commands, and screenshot paths (about 800 px wide, only of what changed, outside the repo).
