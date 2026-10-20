### Slice 30a, release test (Opus), after the last merge and before slice 30
Waits for: every 0.6.0 slice on main (spec "Blocked by: all except 30"). Runs on the final main build only. Max's decision of 2026-10-19 (O12): after this slice, 0.6.0 goes out first as a public beta (a GitHub pre-release and a forum post, about a week), then as the 0.6.0 release. Max approves both publishes himself.

Rule: any error box of the game is a fail. Keep every log and run folder in `/Users/max/Projects/cm0102/run-logs/release-test-0.6.0/`, one folder per check, and list each result in `docs/research/<date>-release-test-0.6.0.md`.

Checks:
1. **Every start season.** Each season database from 2001/02 to 2025/26 (the season database table), with changing rules on and off, played on holiday through at least two season changes. Reuse `tests/seasons/rules_report_check.py` (`--year X --last X+2`) and `season_change_run.py`.
2. **Long games.** Several games from 2001/02 to 2027/28 with changing rules on, each with another seed, all nations loaded (Select All). Write this soak script; it plays the season jump to 2027/28 and checks for boxes and a save at each season change.
3. **Save and load at risky moments.** A save just before the July season change, in the middle of the European cups, and after the draws. Each save loads and plays on for at least 30 days. Reuse `uefa_formats_run.py` (load run), `season_history_run.py` (load run) and `tests/smoke/save_folder_run.py`.
4. **Screen sweep.** For each season, open the tables, schedules and history of every competition. Write this sweep script; a box, a broken date (year outside the season) or an empty table where the rules give clubs is a fail.
5. **Old saves.** The 0.5.x saves and Max's Yeovil save (`/Users/max/Projects/cm0102/test-fixtures/yeovil.sav`) load and play as before. The base game gives the same saves as 0.5.1: `smoke_run.py --same-save` (start.sav, prematch.sav, a new game) with `--ref-build` a 0.5.1 build, and the full same-save set of the same-save-check skill.
6. **Windows and Linux.** The MinGW Windows build and the Linux AppImage (Colima on) run the smoke set and one season change each, besides the Mac.

7. **Every season to 2027, all nations (Max, 2026-10-20, option 4).** (a) Every start season 2001/02 to 2025/26 with its tile's database, all nations loaded (Select All), to July 2027 with the quick season jump (the no-match-engine results of rules_report_check --run), changing rules on and off: 50 runs, with a save and load at each July change and a stall timeout. (b) Full play (match engine, holiday), all nations, two seasons each, for 5 starts across the eras: 2002/03, 2006/07 (own database), 2013/14, 2021/22, 2025/26. Run (a) and (b) on main now; repeat (a) on the final main as the gate before the beta. Measured cost of the quick jump with all nations: about 100 minutes a season (slice 24, 2026-10-20), so a 2001 start to 2027 takes about 45 hours. The test report says honestly that full match play covered 5 starts, not 25.

Write only what is missing (the soak in check 2 and the sweep in check 4). Run the checks overnight in parallel within the machine limits in `docs/HANDOVER.md` (game limit, load, build queue, HOLD file, no stale games).

Report: write `docs/release/0.6.0-test-report.md`, one page: every check with its result (runs, seasons covered, nations, faults found and fixed) and an honest list of what was not tested. Also one line per check with PASS or FAIL and the log folder, each box with its file, line and the season, and a list of what must be fixed before the beta.
