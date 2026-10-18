# Play-offs, splits and calendar changes in existing divisions (0.6.0 slice 19)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (slice 19; Q247, Q249, Q272 to Q282, Q284, Q292, Q337, Q338). Brief: `docs/research/build-brief-0.6.0.md`, slice 19. Code: `tools/seasons/encode_leagues.py`, `src/recomp_rt/win/season_league.h`, `season_league_table.c`. Tests: `tests/seasons/test_encode_leagues.py` (ctest `season_encode_leagues`), the rules report (`tests/seasons/rules_report_check.py`).

## The encoding

The play-off, split and calendar columns of the rules table are text. `tools/seasons/encode_leagues.py` reads the 2538 league rows (a numbered tier and a `comp_file` of the original) and writes one line per row to `2026-10-18-league-rules-encoded.tsv`:

- `po_up`: the places of the division in a play-off for a place in the tier above ("3-6", "2,3"). A club of the tier above in the same tie counts in `po_down` of its own row.
- `po_down`: the places of the division in a play-off to keep its place.
- `po_other`: a play-off that moves no club between tiers: `title` (a finals series, a championship of stage winners), `europe`, `tiebreak` (a match only when clubs are level).
- `split`: "after N: A+B, points P", or `stages` (Apertura and Clausura), `zones`, `groups`, or `gap`.
- `calendar`: `split` (July to May), `calendar` (one year) or `transition` (Ireland 2002/03, Russia 2010/11 and 2011/12).

How the tool reads a text: it splits it into clauses at ";" and at the end of a sentence. The places of the division are the places before the other side of a tie ("16th v 2. Bundesliga 3rd" gives 16), a range at the start ("3rd to 6th"), a "pos" list ("pos 2,3,12") or "top N". The words promotion and relegation give the direction. A clause with both words or neither goes by place: a range by its first place, a list place by place, the top half up and the bottom half down. A place in the top half of a top division is a title play-off. Words for Europe, a title or a tie-break make the clause `po_other`. 120 texts that this does not read right are encoded by hand in `BY_HAND` and `BY_START` (each read against its row), and 12 forms with no encoding are in `KNOWN_GAPS` with the reason. The tool stops on any other text, so a changed research text never goes unseen.

Result: 967 rows with play-off places, 169 with a split, 631 calendar-year rows. `2026-10-18-league-rules-gaps.tsv` lists 264 texts (of 261 rows) that the tool reads only in part or not at all, each with its reason:

| Reason | Lines |
|---|---|
| the places of each group or zone of a division of groups | 83 |
| the period winners of Belgium and Holland (the game has no periods) | 54 |
| a reserve team leaves its place to the next club (Spain, Portugal, Croatia) | 28 |
| a stage of groups (Brazilian Série C, see format) | 19 |
| places of the groups after a split (Croatia, Denmark, Greece, Japan 2026) | 18 |
| the winners of zones or series (a second stage that the game does not play) | 15 |
| the table of averages of Argentina | 11 |
| the Serie B gap rule (no play-off when the 3rd leads the 4th by more than 9 points) | 9 |
| the play-off places are not given in the text | 9 |
| the split cannot be read (no round count, or sizes in words) | 7 |
| other single rules (a licence, a final of tournament winners, a regional rule) | 11 |

## The rules report

The `playoffs` difference of `rules_report_check.py` compared whether a division has play-off places with whether the text is not "none". So the MLS, the A-League and the J-League finals, the European play-offs and the Italian tie-break all showed as differences, and a wrong count did not. It now compares the promotion play-off places (+0xbf) and the relegation play-off places (+0xc0) of the division with the counts of `po_up` and `po_down`. For a text that the tool reads only in part, it compares only whether the division has play-off places.

## The original's play-off code

Evidence: the instruction listing of the translated code (`build/recomp/gen/fn_*.c`) and the vtables of `clean-3968/cm0102.exe`. A research agent read the functions on 2026-10-18; its notes are summed up here with the addresses.

**Places and statuses.** `League::update_positions_and_status` 0x684cd0 (vtable +0x94) reads +0xbe..+0xc1 as one dword (0x684db0) and marks a zone for each place: +0xbe places from the top (zone 0), then +0xbf (zone 1), 0x684e11..0x684e7b; +0xc1 places from the bottom (zone 3), then +0xc0 (zone 2), 0x684e8e..0x684ef5. The status byte of a row (+0x37) becomes 5 champion (0x685149), 0 promoted (0x6851a5), 1 promotion play-off place (0x685213), 2 relegation play-off place (0x685281), 3 relegated (0x68525e) or 0xfe nothing to play for (0x685327). `league_promote_and_relegate` 0x689c80 moves rows of status 3 down and 0 or 5 up (0x689d1a..0x689e2c). Rows of status 1 and 2 never move.

**When a play-off is played.** `League::update_for_date` 0x6847c0 calls `check_complete(0, 0)` (0x684994). When the table is complete and the stage index +0x30 is below the stage count +0x2c less 1, it calls `advance_stage` (vtable +0x28) at 0x6849e0. The season of the competition ends only when every stage is complete (`check_complete(1, 1)`, 0x689bbc..0x689be8), so `start_new_season` waits for the play-off.

**England.** `EngFirst::advance_stage` 0x572ff0 calls `create_playoff_stage` 0x573080 when +0x30 becomes 0 (0x573073); EngSecond 0x5769e0 to 0x576a70, EngThird 0x578330 to 0x5783c0. The builders read +0xbf only as the array size (0x57309f) and the club count of the Cup_Stage (0x57317b, ctor 0x522e00). The clubs are fixed rows: 6th, 3rd, 5th, 4th (EngFirst 0x5730f3..0x573126, EngSecond 0x576ae4..0x576b17), 7th, 4th, 6th, 5th (EngThird 0x578434..0x578464). So +0xbf must stay 4 on these classes. The result handler +0x44 0x576c50 gives the final winner status 0 (0x576d9e) and the losers 0xfe (0x576dc2).

**The other builders** (each hard-codes its club count):

| Builder | Clubs | Reads the statuses | Note |
|---|---|---|---|
| `ScoFirst::create_playoff_group` 0x7ef340 | 2nd and 3rd, and the SPL club of the relegation play-off place | status 1 (0x7ef3e1), SPL status 2 (0x7ef48a) | only in the 1999 rules: +0xbf 2 when the year is 1999 (0x7eee5d, 0x7eee9a), +0x2c 1 (0x7ee022, 0x7eef75), ScoPrm +0xc0 1 (0x7f297a, 0x7f299b) |
| `BelSecond::create_playoff_group` 0x420230 | 4 period winners, topped up from the table (0x42028e) | status 1 | League_Stage of 4 |
| `BelThird::create_promotion_playoff_stage` 0x423880 | 3 + 3 period winners and the BelSecond 16th (0x423a80) | status 1 | Cup_Stage of 7 |
| `HolFirst::create_play_off_groups` 0x5f6720 | 6 HolFirst clubs (0x5f677e) and the HolPrm clubs of status 2 (0x5f68ad) | yes | two League_Stages of 4 |
| `ItaSerB::create_tiebreak_playoff_stage` 0x646bb0 | clubs level on points across a zone line (0x646c61) | no | a tie-break, not a promotion play-off |
| `SpaSecondB::create_promotion_playoff_groups` 0x857570 | the top 4 of each of the 4 groups (0x8576cf..0x8576fb) | no | four League_Stages of 4 |
| `SweSecond::create_relegation_play_off_stage` 0x892bb0 | 6 clubs of status 2 and 12 random clubs of the tier below (0x892c41..0x892d4e) | status 2 | Cup_Stage of 18 |
| `NorFirst::create_play_off_stage` 0x790e20 | the last own row of status 1 (0x790e97), the NorPrm row of status 2 (0x790f3c) | yes | Cup_Stage of 2 |
| `FinFirst::create_play_off_stages` 0x594080 | fixed rows (0x594295..0x5942ce) | no | Cup_Stage of 4 |
| `PolSecond::create_play_off_stage` 0x7cc290 | own status 1 rows (0x7cc30a), PolFirst status 2 rows (0x7cc34f) | yes | Cup_Stage of 4 |
| `IreFirst::create_promotion_playoff_stage` 0x635dc0 | 1 + 1 (0x635e37, 0x635edc) | yes | Cup_Stage of 2 |
| `NirFirst::create_playoff_stage` 0x78c0a0 | 1 + 1 (0x78c117, 0x78c1bc) | yes | Cup_Stage of 2 |
| `RusPrm::create_title_playoff_stage` 0x7eba90 | 1st and 2nd when level on points (0x7ebabb) | no | a title decider |
| `UsaMls::create_playoff_stage` 0x9094c0 | 3 division winners and the best 5 others (0x9095b7, 0x909692) | no | championship, no promotion |
| Italian C1 and C2 builder (C1a 0x64ba70) | fixed rows 2nd to 5th (0x64bb1a..0x64bb43) | +0xbf as the count (0x64bb99) | +0xbf must stay 4 |

**A class with no builder.** SpaFirst, SpaSecond, FraFirst, FraSecond, GreSecond, GerSecond, PorSecond and EngConf have `advance_stage` +0x28 = 0x5a8f60 (a bare return), the base +0x44 0x686940 and the base +0x94 0x684cd0. Places set at +0xbf or +0xc0 there only label rows with status 1 or 2 (no news: 0x686940 sends news for 5, 0 and 3 only). The rows do not move and the season completes. With the direct places cut to leave room for the play-off winner, one club fewer goes up or down. So a play-off cannot be added by a count: it needs a builder.

**The Scottish split.** `ScoPrm::update_for_date` 0x7f2d20 calls `ScoPrm::create_split_stages` 0x7f2d90 when +0x3c is 3 (0x7f2d2d) and every club has played 33 (0x7f2d56). Rows 0 to 5 and 6 to 11 each get a League_Stage of 6 clubs and 1 meeting (0x7f2e90..0x7f2ed7) with dates from vtable +0x3c with 0 or 1 (0x7f2e63, 0x7f2f41). The stages are deleted at once (0x7f2ef5, 0x7f2fd3): inference, the constructor adds the fixtures to ScoPrm's own list. The clubs stay in the one table, so points carry over. Then +0x3c becomes 4 (0x7f2feb) and the table order takes the comparator 0x7f3220 (top six above the bottom six; the load sets it again, 0x7f0bf1..0x7f0c08). The function is not virtual and needs the split dates of the class's +0x3c, so another class needs new code.

## What this slice applies

The play-offs that the original plays stay as they are in every season: the English Championship, League One and League Two (4 clubs), the Norwegian, Irish and Northern Irish ties, the Dutch, Belgian, Swedish, Finnish, Polish and Italian C1 and C2 play-offs. Slice 18 keeps the row's promotion and relegation counts: where the class has play-off places, the row's count may hold the play-off winner (`season_sizes_rules`); where it has none, the direct places take the row's count. So a play-off of the table on a class with no builder plays in its nearest form: the play-off place goes to the next club of the table (Serie B 2004/05: 3 up directly). This is Max's recommended answer to the stop of the brief ("the nearest form only where the code cannot").

This slice changes no game code. A new play-off, a split on another class and a calendar move all need new code of the size below. The rules report shows each place where the game and the table differ (`playoffs` lines), so the work left is visible in the expected files.

## Design: a play-off on a class with no builder

Inference from the evidence above; not built.

1. At `start_new_season`, for a row with play-off places: set +0x2c to 1 and +0x30 to -1, and the places +0xbe, +0xbf (+0xc0, +0xc1) of the row.
2. `advance_stage`: the class's vtable +0x28 is the shared return 0x5a8f60, so a wrap of that address would change every class that shares it. Give the object a copy of its vtable in game memory with +0x28 and +0x44 pointing to runtime functions (needs a way for an indirect call to reach a runtime function; unknown whether the recompiler has one), or wrap `League::update_for_date` 0x6847c0 and build the stage before its call at 0x6849e0.
3. The builder: take rows +0xbe .. +0xbe + +0xbf - 1, make a Cup_Stage through 0x522e00 as 0x573080 does, with round dates in the format of the class's +0x3c (stage 0). For a tie with a club of another tier (Ligue 1 18th v Ligue 2 3rd), the upper league's status 2 row joins, as in NorFirst 0x790e20.
4. The result handler: the logic of 0x576c50 (code 1: status 0; else 0xfe; the upper club: code 0 stays, else 3), on a wrap of the base +0x44 0x686940 for these objects only.
5. Save and load: unknown whether the save of a competition writes its stages for a class that has none in the original, and whether the class's load reads them. This must be read first (a load that does not read a saved stage breaks the file).
6. The round names through `season_round_names`.

Cost: a new builder, a handler, date records and the save and load. The Serie B, Segunda, Ligue 1 and 2, 2. Bundesliga, National League and the later Scottish play-offs all need it.

## Design: a split on another class

Call 0x7f2d90 with the other league as `this` (it is a thiscall of a League) only when its vtable +0x3c answers stage 0 and 1 with the split dates, its table holds 12 clubs and its +0xa3 takes the comparator. Each class needs its own date branch, so this is new code per class. The rows with "points halved" (Belgium Play-off I to 2019/20, Poland 2013/14 to 2016/17, Turkey 2011/12) also need a write of the points at the split.

## Design: a calendar move

A calendar move changes when a league table entry has its update day (+0x15) and its start and end dates (+0x2a, +0x32..+0x37, +0x39..+0x46, set by `key_nation_build_table` 0x667150 from the game year; calendar-year leagues take year + 1). The fixture template of the class (+0x3c) holds the dates of its own calendar, so a move needs a new template too. The year tests of `finance_get_days_to_season_start` 0x59b56b, `MATCH_PLAYER::setup` 0x6eb28d (Japan) and `FIXTURE_MANAGER` 0x5aee52, 0x5af218..0x5af2b9 (`2026-10-17-first-season-branches.md` table 3) follow the calendar. In `season_change.c` the season of a change comes from its date (a change from 1 October starts season Y + 1), so a move from a calendar year to July (Ireland 2003, Argentina 2016/17) gives one season with two changes, and the news of that season must come once. Not built.

## Decisions for Max (taken with the recommended answer)

- **A play-off that the code cannot hold** (every new play-off of the brief: the Championship of 6 from 2026/27, the National League from 2002/03 and 6 from 2017/18, Serie B from 2004/05, Segunda from 2010/11, Ligue 1 from 2016/17, the 3rd v 16th tie of Germany, the Scottish play-offs from 2005/06 and 2013/14, Q284 and Q337): the nearest form, the play-off place goes to the next club of the table. A later slice with the builder above plays them as the table gives.
- **The Championship play-offs of 6 from 2026/27:** the builder takes exactly 4 clubs, so 3rd to 6th play as before.
- **Splits** (Scotland as before; Belgium from 2009/10 with Play-off I and points halved, Denmark from 2016/17, Greece from 2019/20, Poland 2013/14 to 2019/20, Croatia, Wales from 2010/11, Northern Ireland from 2008/09, Korea, Finland): one table with the row's club count and places, as slice 18 gives.
- **Calendar moves** (Ireland 2002/03 to 2003, Russia 2010/11 and 2011/12, Argentina 2014/15, 2016/17 and 2020/21, Japan 2025/26, the MLS 2026/27): each league keeps the calendar of its class. Russia plays its 2011/12 long season as two normal seasons, with the counts of the rows.
- **Apertura and Clausura** (Argentina to 2012/13) and the Brazilian double round robin from 2003 (slice 18 doc: one meeting at 0x42a5af, 0x42cb83): as the class plays them.
