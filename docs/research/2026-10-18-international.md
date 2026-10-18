# International competitions in the original (0.6.0 slice 27)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md`, slice 27, Q248, Q276, Q277, Q288, Q366, Q367. Build brief: `docs/research/build-brief-0.6.0.md`, slice 27. Sweep: `docs/research/2026-10-16-sweep-international.md` (named "sweep" below).

Code: `src/recomp_rt/win/season_intl.h`, `season_intl_rules.c` (editions and rules, no game memory), `season_intl.c` (host table, Australia, the rule kind), `season_intl_table.c` (made by `tools/seasons/make_intl.py`), the sites in `tools/recomp/seasons.py`. Tests: `tests/seasons/test_season_intl.c` (ctest `season_intl`), ctest `season_intl_table`, `tests/seasons/season_intl_run.py`.

## The edition table

`tools/seasons/make_intl.py` reads the rows of `int-world.tsv` and `int-continental.tsv` whose `comp_file` is a finals class of the original (`world_cup`, `euro_champ`, `fifa_confed`, `olympics`, `goldcup`, `african_nations`, `asia_nations`, `oceania_nations`) and whose format is not "not held". Each row is one edition: the season of the row, the edition year that the class holds at +0x40, the team count, the golden goal, the first day and the hosts. Slices 28 and 29 add their competitions to the same table.

- The edition year is the year of the last date of the format. So the 2025 Africa Cup of Nations (21 December 2025 to 18 January 2026) is the edition 2026 of the class, and the 2022 World Cup is 2022.
- Rows with no edition that a class can play are in `SKIP` of the tool with the reason: the 2004 OFC final (part of row 32), the second row of the 2008 OFC round robin, the cancelled 2020 OFC Nations Cup, and the 2028 Africa Cup of Nations, OFC Nations Cup and Copa América (no host, dates or format yet, Q248).
- Q366 moves the Tokyo Olympics (row int-world-251) to 2020/21 on their planned first day, 22 July 2020.
- Q367: the format of Euro 2004 says silver goal, so the edition has no golden goal: full extra time, then penalties.
- Hosts with no automatic place get no host row: Euro 2020 (12 host nations, each had to qualify) and Euro 2028 (no automatic place; two places are kept for hosts that miss out in the qualifying, slice 28).
- Nation names of the table that nation.dat writes another way: Netherlands is Holland, China is China PR (China in later databases), the United Kingdom of the 2012 Olympics is England (inference: the game has no Great Britain team).

## Two paths

A season game is a game with the season code on whose start year is not 2001 or whose changing rules are on (`season_intl_on`). The base game (2001/02, changing rules off) and `RECOMP_SEASON_CODE=off` get none of this slice: every site gives the original's value.

- **At a new game** each finals class takes the first edition of the table whose season is the start season or later. A 2001 start keeps the original's first editions (the table holds the same ones).
- **With changing rules on**, `start_new_season` of each class takes the next edition of the table. After the last edition of a class the object waits with the year 2200, which no game reaches: the Confederations Cup after 2017 (Q248: a competition that ended does not come back).
- **With changing rules off**, the rules of the start season stay (Q232): after the first edition each class adds the original's step.

## The editions (code sites)

Every class keeps its edition year in the word +0x40 of its object (sweep section 2). The research of the sites (constructor, `start_new_season`, `this` in esi):

| Class | Constructor | First instruction after the year loop | start_new_season | Step |
|---|---|---|---|---|
| world_cup | 0x92bf50 | 0x92bfee `push 6` (jump from 0x92bfcf) | 0x92d330 | 0x92d409 +4 |
| euro_champ | 0x57a890 | 0x57a92b `push 1` (from 0x57a90c) | 0x57b750 | 0x57b7ca +4 |
| fifa_confed | 0x58b310 | 0x58b392 `push 1` (from 0x58b389) | 0x58bff0 | 0x58c050 +2 |
| copa_america (goldcup.cpp) | 0x5e0660 | 0x5e06fb `push 1` (from 0x5e06db) | 0x5e1530 | 0x5e1590 +2 |
| african_nations | 0x401000 | 0x401097 `push 1` (from 0x40107a) | 0x401d20 | 0x401d80 +2 |
| asia_nations | 0x40e860 | 0x40e8f5 `push 1` (from 0x40e8d8) | 0x40f720 | 0x40f780 +4 |
| oceania_nations | 0x7988b0 | 0x798952 `push 1` (from 0x798935) | 0x7998e0 | 0x799940 +2 |
| olympics | 0x79f530 | 0x79f5c5 `push 1` (from 0x79f5a8) | 0x7a0450 | 0x7a04b0 +4 |

- The constructor uses the year after that instruction (team slots, stages, hosts), and `start_new_season` calls the stage builder (vtable +0x8c) after the step, so the new year reaches the stages and the hosts. The step is the only write of +0x40 in each `start_new_season`. No instruction after a step reads its flags before another sets them (read at each site).
- Euro qualifying (0x57ce10) and the six World Cup sections keep the original's four-year cycle: the table's European Championships and World Cups come every four years (Euro 2020 at its planned time, Q277).
- The finals dates are fixed in the class code, not in the host rows (the round dates of the template function vtable +0x3c build `Date::Date` 0x549f70 from the day, month and +0x40). For example the African Cup of Nations plays in February (0x401385), the Asian Cup in October (0x40eba5), the Olympics in September (0x79f875), the Confederations Cup from 29 July (0x58b601). So an edition plays in its year at the class's month. The table's months are not applied: see "Left out".
- World Cup check at 0x92c175: when the year is the game year at the constructor, the class takes 32 teams from its default list (+0x36 = 0x20). A 2022 start meets it, because the class plays in June and the table's 2022 World Cup is in the start year.

## Golden goal (Q367)

Bit 0x80 of the rule flags word of a knockout round is the golden goal (sweep section 7). The round template functions hold the competition in edi (0x92c299 World Cup, 0x57ab99 Euro, 0x58b56f Confederations Cup). The PATCHES of `seasons.py` pass the word through `rt_intl_round_flags` at the ten sites of the sweep (0x92c96b, 0x92ca46, 0x92cb15, 0x92cbb9, 0x57b002, 0x57b0b2, 0x57b150, 0x58b892, 0x58b90b, 0x58b9a5). With changing rules on, the edition's golden flag of the table decides (golden goal only at the 2002 World Cup and the 2003 Confederations Cup; Euro 2004 plays full extra time, then penalties). With changing rules off, the start season's rule stays: golden goal in a game that starts in 2001/02 or 2002/03.

## The World Cup holder

From the 2006 World Cup the holder qualifies like the others (int-world, "Holders' place"). Each qualifying section asks for the holder with `comp_util_get_latest_placed_club` 0x4afd80 (place 0 of the World Cup history) and leaves it out of its pool. `rt_intl_wc_holder` gives no holder in a season game, at the instruction after each call: Europe 0x919f47, South America 0x920e38 and its pool 0x920f18, Africa 0x9107a6, Asia 0x914077, CONCACAF 0x916394, Oceania 0x91fb98. Each caller tests the result for NULL (0x919f50, 0x920e3d, 0x9107af, 0x914080, 0x91639f, 0x91fba1) or only compares with it (0x920ff9). In a season game every section plays for the 2006 World Cup or later, or plays the 2000 campaign, whose holder France is in none of those pools.

`world_cup` `build_team_slots` 0x92da40 puts the holder in slot 0 (0x92da86) and counts it (0x92da89). From 2006 the count does not go up, so the first host takes slot 0 (0x92dac3), and slots 1 and 2 are cleared because 0x92db02 and 0x92db57 compare slot 0 with them. Inference, not traced: the sections' places give 31 qualifiers with one host; a slot that no qualifier fills keeps a nation of the default list (0x92dbd5..0x92dfb3).

## Hosts

The host table (`HostCountry`, global 0xb5e134: +0 the rows, +4 the count as a word) is made by the constructor 0x5f97c0 at a new game (called from `create_game` 0x828fb0) and at a load (0x82bfec). It first tries the temp file (0x5f98ab, `load_temp_file` 0x5fb160); only when that fails does it allocate 57 rows (0x5f98bd, malloc 0x944e46 at 0x5f98c3), call `init_default_table` 0x5f9970 (0x5f9945) and sort the rows by year and decision day (qsort 0x945333 with 0x5fb3f0 at 0x5f995a). The save writes the count and every row (0x5fb320: 0x5fb3bf, 0x5fb3ce), and the load reads them back (0x5fb19e, 0x5fb275), so changed rows and a larger count survive a save.

Row (0x22 bytes): +0 the key id of the competition (the value of its key global, compared with the record at 0x5f99cf), +4 the year (word, 0 = every year), +6/+0xa, +0xe/+0x12, +0x16/+0x1a three host pairs (nation ids, the index into [0xae23a8] with 0x122 bytes a record, 0x92dacc; -1 for none), +0x1e the choice (0 to 2 the chosen pair, 0xfd fixed, 0xfe not decided, 0xff bidding open), +0x1f the decision day (word), +0x21 the years by which the row moves on 1 January once it lies in the past (`daily_update` 0x5fa850, 0x5faafa; its only reader).

`find_hosts` 0x5fa730 takes the first row of the competition with the year or with year 0 and returns the count of hosts of the chosen pair (pair 1 for a fixed row). It cannot return a third host.

The wrap of `init_default_table` (season game only) writes, for each edition of the table from the start year of the World Cup, European Championship, African Cup of Nations, Olympics, Copa America, Asian Cup and Confederations Cup: the comp key id, the year, pair 1 the first two hosts that the data holds (by name), the other pairs none, choice fixed, no move. An edition without a row of the original gets a new row (the table grows; new rows array with malloc 0x944e46, the old one back with free 0x9452ca), with the first day of the finals as its decision day. The constructor sorts them after the return. The original's 57 random draws of the decision days stay. An edition whose hosts have no automatic place (Euro 2020, Euro 2028) gets a row with no host, so `find_hosts` gives 0 and the class goes on without a host (sweep section 6).

- The Confederations Cup has one row of every year (row 56) whose host `redraw_world_club_and_confed_hosts` 0x5fab70 draws at random. Its year becomes 2200 in a season game, so the rows of the table come first, and the wrap of 0x5fab70 puts the table's hosts back after the draw.
- A row of the original that lies in the past moves on by its cycle and takes a random host. A row whose move would land on an edition of the table (the 1999 Copa America row on 2011, the 2010 African Cup of Nations row on 2026) stops where it is.
- Rows of the original for years without an edition stay as they are (for example the World Cup rows after 2026 that the original makes by its move).
- The Oceania Nations Cup calls no `find_hosts`.
- Host news: `post_host_news` 0x5faec0 posts only for rows that `daily_update` opens or decides (choice 0xfe and 0xff). A fixed row gets no news. See "Left out".

## Australia to Asia

The confederation of a nation is the continent pointer at nation +0x71; its +0 is the continent id, compared with the key continents (Asia 0x9cfa0c, Oceania 0x9cfa18; for example 0x914037..0x914047 in the Asian section). Australia joined the AFC on 1 January 2006 (int-world-49).

- A season game from 2006/07 moves Australia before the original makes the game (`season_intl_at_create`, from the wrap of `create_game` in `season.c`), so every competition takes it as an Asian nation. The new pointer is the continent record of an Asian nation of the data; the save keeps it.
- With changing rules on and an earlier start, the rule kind moves it on the first update day from 1 January 2006 (the `entry` hook, entry 5 on 10 January), with the news sentence "Australia now plays in the Asian Football Confederation." A 2005/06 start with changing rules off keeps Australia in Oceania (Q232).
- World Cup Oceania section: it takes exactly 10 entrants (+0xf3 from 0x91fbda) and leaves out New Caledonia (key nation 0x9cf530, 0x91fc89) and Papua New Guinea (0x9cf41c, 0x91fc93). With Australia in Asia, New Caledonia comes in (a FIFA member from 2004 that played the 2010 qualifying), so the count stays 10.
- Oceania Nations Cup (`pick_entrants` 0x799e30): Australia and New Zealand by key id (0x799f9c, 0x799fc1; 0x9cf224 is Australia, 0x9cf3f4 New Zealand), then the two best Melanesian and Polynesian nations. With Australia in Asia the wrap puts the Oceania nation with the highest reputation that is not in the six in Australia's place (reputation at nation +0x8e, an inference from `2026-10-17-season-date-shifts.md`).
- Host row of the World Cup Oceania section: 0x5fab70 swaps its host between Australia and New Zealand; with Australia in Asia the wrap sets New Zealand.

## Finals that start before their qualifying ends

A game that starts in a later season meets finals whose qualifying began before the start. The sections of a 2005 start play the 2004 campaign from July 2005 and cannot end before the World Cup draw of 9 December 2005: `WorldCup::seed_teams` 0x92f230 finds fewer than 32 teams (+0x36 against 0x20 at 0x92f24c) and stops at the box "world_cup.cpp 1438" (0x92f2b1). A run of the main build with September 2005 stops at the same box, so this is older than this slice. A 2003 start (October 2003) stopped at the Euro 2004 draw of February 2004 with the box "comp_stats.CPP 1664": `EuroChamp::run_group_draw` 0x57c1d0 calls `set_predicted_positions` 0x57c9f0, which passes an empty slot to `Comp_Stats::set_club_final_position` 0x4a2540 (the assert at 0x4a257c).

In a season game the wraps of `seed_teams` and `run_group_draw` fill each empty slot before the draw: first the default nations that the slot builders wrote in the slots after the count (0x92dbd5..0x92dfb3, 0x57bf2f), then the nations with the highest reputation (nation +0x8e, s16: 9500 for Brazil and France in the 3.9.68 nation.dat) that are not in yet, European nations only for the Euro. Teams are compared by their own nation (club +0x53). `national_team` 0x53b3d0 gives `club[nation id + clubs - 2 * nations]`, but in a season database that row can be an empty club with no nation: a 2019 start (October 2019) gave an empty row for Denmark while the qualified Denmark team was another club (club 2598 of 3007, read in the test log). An early form of the fill put that empty club in beside Denmark, and the game stopped in May 2020 in `NationalTeams::select_squad` 0x76c120 and `release_overage_u21_players` 0x76d880 (boxes "Database.cpp 13126" and "13160"; a 2003 start the same with the Czech Republic). So the fill takes a club only when its nation field names the nation. Why the row is empty in that database is not known (a question for the club table work of slices 15 and 15b). A qualifier that ends after a full draw stays out: `add_qualified_team` (0x92e740, 0x57c120, thiscall (team), ret 4) would stop at its box when the slots are full (0x92e74a, 0x57c12a). The 2002 World Cup keeps its own draw (0x92f240).

**Open: an empty team in the Euro finals** (the lead gave it to the staff reserve fix session on 2026-10-18). With the fill above, a 2003 start (October 2003) and a 2019 start (October 2019) still stop in late May of the Euro year (23 May 2004, 20 May 2020) at the box "Database.cpp 13160" (`database_get_national_u21_team` 0x53b4f0 with no nation, from `NationalTeams::release_overage_u21_players` 0x76d880). The test log of the 2019 run read the fixture: its first word is 0x1c1 (record 449, in the range of the nation competitions, rows 433 to 451 in the clean data: inference, the European Championship), its date 17 June 2020, its teams Denmark and an empty club with no nation, which is the row that `national_team` gives for Denmark (club 2598 of 3007). All 16 draw slots held clubs with a nation at the draw. Where the Euro takes the empty club after the draw is unknown. The main build cannot reach this point (it stops at the Euro draw, see above), so it is not known whether this slice causes it. The 2005 and 2017 starts (World Cups 2006 and 2018, no Euro finals in the run) pass.

Two more stops of later starts, both in a season game only:

- **A Euro without a host.** `EuroChamp` `build_team_slots` 0x57bd80 stops at the box "euro_champ.cpp 837" when `find_hosts` (0x57be34) gives no host (0x57be3d to 0x57becc). Euro 2020 and Euro 2028 have no host place in the table, so a 2017 start stopped there at the new game. The INSNS entry at 0x57be3d goes on to the default nations (0x57bf2f) instead.
- **Euro qualifying without a host.** `EuroChampQual::get_group_team_count` 0x57d9d0 sizes the groups by the hosts that the pool leaves out (+0xf6): 1 (0x57d9de) or 2 (0x57d9e5), else the box "euro_champ_qual.cpp 581" (0x57da3d). With no host it now takes the sizes of 1 host (room for 54; the pool holds at most 51, 0x57f76f).

## Finals dates

Each finals class sets its event dates in its stage builder (vtable +0x8c: World Cup 0x92d200, Euro 0x57b630, Confederations Cup 0x58bdb0, Copa America 0x5e12d0, African Cup of Nations 0x401ab0, Asian Cup 0x40f4c0, Oceania Nations Cup 0x799690, Olympics 0x7a0230; read from the vtables of sweep section 2). The builder makes `Date::Date` 0x549f70 (thiscall (day 1 to 31, month from 0, year, flag), ret 0x10) for its own events (for example the Euro seeding and draw dates at 0x57b6b9 and 0x57b6e4, kept at +0x13e/+0x140 and +0xb6/+0xb8 and read by `update_for_date` 0x57c66c and 0x57c69d) and calls the template function (vtable +0x3c), whose round dates go through `League_Stage` 0x68a160 and the Cup round functions 0x521e60 and 0x521eb0 (arguments: day, month from 0, a year offset, a flag, then the year), which make `Date::Date` too.

In a season game the wraps of the eight builders set a shift while they run: the days from the class's fixed first group day (the first call of 0x68a160 in each template: World Cup 12 June 0x92c335, Euro 9 June 0x57ac35, Confederations Cup 29 July 0x58b601, Copa America 6 July 0x5e09a5, African Cup of Nations 8 February 0x401385, Asian Cup 13 October 0x40eba5, Oceania Nations Cup 10 November in a year with year % 4 = 2 or else 19 June 0x798b70 and 0x798c23, Olympics 13 September 0x79f875) to the first day of the edition in the table. The wrap of `Date::Date` moves every date made in that time by the shift (the weekday flag stays). So the African Cup of Nations 2019 plays from 21 June, the Asian Cup 2011 from 7 January, the 2022 World Cup from 20 November (point 3 of "Points for Max": the leagues do not stop for it), and the 2025 African Cup of Nations from 21 December 2025. An edition without a date in the table (the 2008 OFC round robin) keeps the class's dates. The table holds the start year of each edition for this.

## Nation names

The table of `season_intl.c` holds the renamed nations of the "nation rules" rows of `int-world.tsv`: Yugoslavia to Serbia and Montenegro (2002/03, int-world-13), to Serbia (2006/07, int-world-61), Czech Republic to Czechia (2016/17, int-world-181), Swaziland to Eswatini (2017/18, int-world-193), FYR of Macedonia to North Macedonia (2018/19, int-world-205) and Turkey to Türkiye (2022/23, int-world-253). A rename writes the long name (+4, 51 bytes), the short name (+0x38, 26), the code (+0x53, 4) and the adjective (+0x57, 26; the continent pointer follows at +0x71) of the nation record (read from the 3.9.68 nation.dat, row 209). Every season database keeps the record "Yugoslavia" and adds Serbia, Montenegro, Kosovo, Gibraltar and North Macedonia as new records when it has them (the nation.dat of the 49 databases), so a rename applies only where the data holds the old name and no record has the new name.

- A new game takes every rename up to its start season (from the wrap of `create_game`). With changing rules on, the season hook takes the renames of each later season, with the news sentence "<old> is now called <new>." The start season gives no news.
- A load: `set_key_ids` 0x601a40 finds the key nations by name (`match_nation_key_id` 0x60d810, cdecl (name, index), from the loop at 0x60421a with the record +4). The wrap of 0x60d810 matches a record whose name is a new name of the table by the first old name of its chain ("Serbia" by "Yugoslavia"), when no record holds that first name. So the save needs no new field.

## The European Championship of 24 teams

`season_intl_euro.c` and the block "Euro of 24 teams" of `seasons.py`. In `euro_champ` the League object is group A (template index -1), stage[0..2] (+0xc, count +0x2c) are groups B to D and stage[3] is the Cup_Stage of 8 (built in 0x57bb40: QF B1-A2, C1-D2, A1-B2, D1-C2 from `0x6826d0` at 0x57bbbf..0x57bc33; Cup_Stage 0x522e00 with 8 teams at 0x57bc97, stage index 3 at 0x57bc8c, stored at 0x57bcb4, +0x30 = 3 at 0x57bd62). For an edition of 24 teams (Euro 2016 on) stage[0..4] are groups B to F and stage[5] is a Cup_Stage of 16: the stage array and count of the constructor (0x57a949, 0x57a951), the team slots (0x57aa72, 0x57bd8b, 0x57bd93), the draw scans (0x57b60b, 0x57b9cb), the group stages of the draw (0x57c2e5), the stage tests (0x57b8fc, 0x57b1d5, 0x57b334), the reads of the knockout stage (0x57b736, 0x57b255) and a new stage array after the year step of `start_new_season` (0x57b7cf). Wraps: the draw 0x57c320 (4 pots of 6, hosts and the holder first), the seeding 0x57c710 (the pot array at +0xee holds 16 entries only, 0x57c4a5, so a 24-team edition writes none), the predicted positions 0x57c9f0, the final positions 0x57cc30 and the knockout 0x57bb40 (the six thirds ranked as `copa_america::create_knockout_stage` 0x5e18a0 does: a temporary League_Stage 0x68b100, the third rows copied, sorted with 0x6827d0; the round of 16 in the Euro 2016 bracket with the 15-row third-place table of the regulations, read from https://en.wikipedia.org/wiki/UEFA_Euro_2016_knockout_stage, the UEFA regulations not read: second source needed). The template wrap makes groups E and F with the templates of groups A and B (inference: they share their kick-off slots) and the knockout of 16 as four round records (round of 16 copied from the World Cup's, 0x92c95d, value +7 0x14). The editions of 16 teams and the base game take the original path at every site.

Test: a 2015 start (October 2015) draws Euro 2016 with 24 teams in 6 groups (log "euro_champ 2016 draw of 24 teams in 6 groups"). The run then stops on 28 May 2016 at the empty-team stop below, so the knockout of 16 is not tested in a game yet.

## Left out of this slice (for a follow-up)

- **Sizes.** African Cup of Nations of 24 from 2019, Asian Cup of 24 from 2019 and the Copa America of 16 in 2016 and 2024 need new stage shapes (6 groups and four best thirds, then a round of 16): the sizes are immediates and fixed stage tables (ACN 16 at 0x4022d3; Asian Cup 12 at 0x40fe02; Copa America 12 at 0x5e1cde, which must be 12 at 0x5e207c). The table holds the team counts. The OFC Nations Cup of 8 (6 at 0x799e42) and of 4 (2008 round robin) also stay 6.
- **Qualifying formats 2006 to 2022.** Places and formats of each section by the table, the inter-confederation pairings of 2006 to 2022 (the original builds only the 2002 pairings, 0x91f800 and 0x919c10), and the pool caps of 50 (0x919e41, 0x91a04a) and 51 (0x57f76f) for databases with more European nations.
- **Host news.** The table's rows are fixed, so the game posts no host decision news for them.
- **The other Australia sites** that name Australia or the Oceania continent for clubs (`OceaniaClubChamp::select_qualified_clubs` 0x798220, `RbAustralia` 0x7db5b0, `WorldClubChamp::pick_entrants` 0x92a8b0, `Friendly::build_international_date_table` 0x5ca4f0, the Setup filter 0x839340) and `national_teams::fill_squad` 0x76c120: not changed. The club sites belong with slice 26.

## Points for Max

1. **Hosts decided after the table** (World Cups 2030 and 2034, Euro 2032; brief "Stop and report"). Recommended answer: add them as rows under Q248, with two sources. Not built: after the table the original's rows stay.
2. **African Cup of Nations and Asian Cup qualifying** (the original runs none). Built as the recommended answer of the brief: no new qualifying; the finals take the hosts, then the strongest nations, as the original does.
3. **The 2022 World Cup in November and December.** Taken (lead: no stops, 2026-10-18): the World Cup plays on its table dates from 20 November 2022. The leagues do not stop for it yet; the recommended answer for that part is slice 19's calendar kind. Max to confirm.
4. **More than two hosts.** A host row holds two hosts. The Asian Cup 2007 had four (all qualified as hosts), the Africa Cup of Nations 2027 three, the World Cup 2026 three (slice 29). Recommended answer: the first two of the table take the host places, the others qualify as usual.
5. **Teams of finals whose qualifying began before the start** (section above). Lead decision for Max to confirm (2026-10-18): the open draw slots of a qualifying cycle that a moved start cannot finish take the default nations, then the nations with the highest reputation, as built. A later slice can put in the real qualified nations of each edition where the rules table lists them.
