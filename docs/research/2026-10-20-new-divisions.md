# New divisions (0.6.0 slice 20)

Date 2026-10-20. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (Q232, Q247, Q290, Q291, Q331, Q332, Q363). Code: `src/recomp_rt/win/season_divisions.c` and `.h`, `tools/recomp/new_divisions.py`, small blocks in `season_clubs.c`, `season_sizes.c`, `tools/seasons/make_clubs.py`, `tests/seasons/rules_report_check.py`.

## Parts

The research of 2026-10-19 and 2026-10-20 read the code of each part (one research file per part, outside the repo). Each part is medium to large:

| Part | Design | Size |
|---|---|---|
| 20a Germany | the GerRegional object holds the 3. Liga from 2008/09 | medium to large: 8 wraps and 2 sites |
| 20b Italy | groups A, B, C in the C1a, C1b and C2a objects; a new start and chain code for Serie A | large |
| 20c Spain, Portugal | Primera Federación and Liga 3 in the Segunda B objects, fewer groups | medium to large |
| 20d Greece | a second GreSecond object with a new record (slice 26's module), a cup wrap | medium to large |
| 20e Brazil | 15 state championships with a picker by state, 12 new records | large, a session of its own (reported to the lead) |

Only a season game (`season_game_changes`) gets any of it. The base game and `RECOMP_SEASON_CODE=off` keep the original (Q152, Q342). `RECOMP_NEW_DIVISIONS=off` is the test switch in a season game, `RECOMP_DIVISION_LOG=1` prints a line for each count and date template.

## 20a Germany: the 3. Liga

### The original

- `create_german_comps` 0x8343f0 makes 5 objects when bit 0x04 of nation +0x11c is set (the extra division flag, `test byte [edx+0x11c], 4` at 0x8344d5), else 4. The fifth is `GerRegional` (constructor 0x5dcbd0, record key [0x9cf564] "German Regional", made at 0x83467c). So Q363 holds by itself: the 3. Liga exists only with the flag, as the Regionalliga.
- One object, two groups. The object's table is the North group: `GerRegional::create_table` 0x5dd7f0 takes the clubs whose division (+0x57) is German Regional and whose group tag (+0x60) is North [0x9cf55c] (0x5dd8d5..0x5dd8f1), 18 of them (store at 0x5dd809; 19 when +0x40 is 2000, 0x5dd801). A club after the count gets the box ger_regional.cpp 332 (0x14c at 0x5dd998). The South group is a child stage made by `create_south_stage` 0x5ddc50 (exactly 18 clubs, box 528 with fewer at 0x5dde2a), called by the constructor at 0x5dcdcd and by `start_new_season` 0x5dda90 at 0x5ddc24.
- The stage is read with no test by `GerFirst::process_promotion_relegation_chain` 0x5db6c0 (0x5db73f, 0x5db78a: 0x689c80 with the stage at 0x5db79c), the exchange 0x5db980 (second pass, 0x5dba5d), `get_club_array` 0x5ddff0 (36 slots, 0x5de005..0x5de018; 0x5de0d5) and `set_predicted_positions` 0x5de130 (0x5de202). The destructor (0x5dce80) and `start_new_season` (0x5ddacc) free the stages up to +0x30 only.
- `GerFirst::start_new_season` 0x5db4a0: the chain (vtable +0xb0), then the exchange 0x5db980 when the regional object exists (else `ger_first_exchange_regional_clubs_unplayed` 0x5db7b0), then the split 0x5dbbd0 (36 clubs by latitude into 18 North and 18 South; box ger_first.cpp 1049 at 0x5dbcd4 with fewer), then +0x40 + 1 (0x5db52a).
- The exchange takes new clubs from a pool of up to 200 German clubs (nation [0x9cf314]) with no division or in A Lower Division [0x9cf764], whose senior side (0x540a50) is not in German Regional or A Lower Division (0x5db9bc..0x5dba17). Relegated rows go to A Lower Division through 0x6831a0 (0x5dba9e) and lose their tag (0x5dbaad). The pool is sorted by 0x4afbd0 (0x5dbb6f) and the first clubs come in through 0x6830b0 (0x5dbbae).
- `GerRegional::create_schedule` 0x5dcef0: D = 34 (0x5dcf12); the writes of 0x68a160 fill dates 0 to 33 (the last at 0x5dd7a6, date index 0x21).
- `GerRegional::set_up_rules` 0x5dd9f0: meetings 2 (0x5dda00), +0xbe 2 (0x5dda2f), +0xc1 4 (0x5dda4c).
- `GerSecond::set_up_rules` sets its lower division +0x20 to the regional record only with the flag (0x5df441..0x5df457).

### What the game does

In a season whose rules table has one `ger_regional` row (the 3. Liga, 2008/09 on; after 2027/28 the last row stays, Q248):

1. **Rules table (Q520).** The 3. Liga rows of 2008/09 on take comp_file `ger_regional`, and the Regionalliga rows of those seasons `n/a` (below the 3. Liga: Q331). The names table then gives the GerRegional object the name "3. Liga" (`season_change_rename` of the "German Regional" record).
2. **Count.** `rt_ger_regional_clubs` at 0x5dd801 and 0x5dd809 (`tools/recomp/new_divisions.py`): the clubs of the division, from 8 to 24. A new game puts the 20 clubs of the list into it.
3. **Dates.** A wrap of 0x5dcef0 makes a template of 2 (n - 1) dates: the 34 of the class, and each extra round on the Wednesday after a Saturday whose next date is at least a week later, spread over the season.
4. **Places.** A wrap of 0x5dd9f0: +0xbe the row's up and the play-off place, +0xc1 the row's down. The play-off of the 3rd against the 16th of the 2. Bundesliga has no code (slice 19 plays a play-off inside one league), so its place goes up and down directly (Q521). The 2. Bundesliga with the 3. Liga below gets the same count down (`season_divisions_rules`, called after slice 18's places), so the pairs of 0x689c80 stay even.
5. **No South group.** A wrap of 0x5ddc50 makes no stage: the slot is cleared, +0x30 stays -1, so save, load and destructor see none. Wraps of 0x5ddff0 and 0x5de130 give the one table (positions 1, 2, 3, ...). A wrap of the chain 0x5db6c0 makes the same calls less the stage.
6. **Season change.** A wrap of the exchange 0x5db980 (when the season that starts has the 3. Liga): the relegated rows leave to A Lower Division as in the original (Q522: the German Cup takes its lower clubs from there, 0x5d94c6, and not clubs with no division, 0x5d94ad). At the change from two groups (2007/08 to 2008/09) the clubs below the row's count leave by table position, North and South in turn; the clubs that came down from the 2. Bundesliga stay. Then the pool fills the division to the row's count, with the original's pool and sort. A wrap of the split 0x5dbbd0 gives every club of the division the North tag.
7. **Club moves at a new game.** The 3. Liga lists go to "German Regional Division North" (`make_clubs.py`). With the flag, South takes 0 clubs: they go to no division (Q290). Without the flag, the 3. Liga list does not apply and North and South keep 18 each, because the unplayed exchange 0x5db7b0 and the split need 36 (`season_divisions_class_size`, `season_divisions_list_on`).

With changing rules off the start season stays (Q232): a 2008 start plays the 3. Liga in every season, a 2007 start the Regionalliga in every season.

### The box ger_first.cpp 1049

The research of 2026-10-19 found that with the flag off 0x5db7b0 moves a fixed 4 clubs up (0x5db8d8, 0x5db8e8), so a 2. Bundesliga with 2 relegation places would leave the pool at 34 and stop the split. Slice 18 already keeps the 2. Bundesliga's places when its lower division is not a size-table division (`open_neighbour`, commit 6587a7c), so it keeps 4 down. The flag-off test run below checks it.

## Decisions

- **Q520:** a new division that lives in the object of an old one takes that object's comp_file in the rules table from its first season; the old division's rows of those seasons become `n/a` with a note. Done for the 3. Liga (as Q291 does for Spain and Portugal).
- **Q521:** a play-off against the division above, which slice 19's code cannot play, becomes a direct place up and down (slice 19's nearest form). 3. Liga: 3 up, 2. Bundesliga 3 down.
- **Q522:** clubs relegated from the 3. Liga go to A Lower Division, as the original's Regionalliga does, so the German Cup still takes them. The clubs of the ended South group at a new game go to no division (Q290).

## Tests

To come with the first build of each part.
