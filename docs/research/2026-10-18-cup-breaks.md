# Domestic cups, part 21b (0.6.0)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Part 21a is in `2026-10-17-cup-round-record.md`; its list "Formats the code cannot hold" is the scope of this part. Code: `src/recomp_rt/win/season_kind_cup_breaks.c` (kind `cup_breaks`), `season_cup_breaks.c` (the part that reads only the rules table), more classes in `season_kind_cups.c`, rows in `tools/seasons/cup_formats.tsv`. Tests: ctest `season_cup_breaks` (`tests/seasons/test_season_cup_breaks.c`), `tests/seasons/cup_breaks_run.py`, `tests/seasons/season_cups_run.py`.

## 1. Cups that end or pause

### The classes

29 classes of the original have a cup or super cup row with `not_held` in the rules table: `bel_super`, `bra_champ_cup`, `fra_lge_cup`, `ger_lge_cup`, `gre_super`, `hol_super`, `ire_leinster_cup`, `ire_lge_cup`, `ire_munster_cup`, `ire_pres_cup`, `ire_super_cup`, `ita_c1_super`, `ita_c_cup`, `jap_emp_cup`, `jap_j_cup`, `jap_super`, `kor_league_cup`, `kor_super_cup`, `nir_charity`, `nir_lge_cup`, `nor_cup`, `pol_lge_cup`, `pol_super`, `sco_chal_cup`, `swe_cup`, `usa_open_cup`, `wel_cup`, `wel_lge_cup`, `wel_prm_cup`. The club competitions outside UEFA are slice 26's (`season_kind_club_world.c`), the Intertoto Cup and the international competitions are other slices'.

What the code of these classes does (read for each class):

- Every `start_new_season` adds 1 to +0x40 before it calls `set_up_rules` or `set_up_rounds` (vtable +0x8c): FraLgeCup 0x5bd4be, KorLeagueCup 0x66c971, KorSuperCup 0x66df2e, WelPrmCup 0x928261, IreSuperCup 0x63b5b0, IreLgeCup 0x6377d1, IrePresCup 0x6398be, IreLeinsterCup 0x636cbe, IreMunsterCup 0x63916e, BraChampCup 0x426320, JapEmpCup 0x6629ee, JapJCup 0x665c5e, JapSuper 0x66657e, NorCup 0x78fc2e, ItaC1Super 0x63cb3e, ItaCCup 0x63d7f1, NirCharity 0x78a4ae, NirLgeCup 0x78d1d1, ScoChalCup 0x7ed1ce, WelCup 0x924cee, WelLgeCup 0x926911.
- No function of these files calls vtable +0x8 (`start_new_season`), nor do `cup.cpp`, `league.cpp` or `comp.cpp`. So, unlike the Copa Mercosur and the Asian Super Cup (club world doc, section 3.3), no class starts itself outside the season pass.
- Every constructor is thiscall (year, record) with `ret 8` and calls `Cup::Cup` 0x518640 or `League::League` 0x682200 first.
- Six League-based classes make the fixtures of an edition at once: their constructor and their `start_new_season` call `League::create_fixtures` 0x6835c0 (WelPrmCup at 0x927970 and 0x9282f8; KorLeagueCup, IreLgeCup, ItaCCup, NirLgeCup, WelLgeCup the same). Cup classes make the fixtures of a round only on its draw and match dates.
- Every Cup class picks its entrants in `start_new_season` (FraLgeCup 0x5bd4d0).

### Edition and row

The edition of a cup is its year +0x40. A split-season cup plays edition Y in season Y/Y+1, row Y. A calendar-year cup plays edition Y in the calendar year Y (NorCup draws its first round on 7 April of +0x40, `create_round_table` 0x78f3cb; JapJCup, KorSuperCup, UsaOpenCup the same), and the rules table puts that edition in row Y - 1 (`rules-0.6.0/club-world.md`, "How editions map to game seasons"; for example `nor_cup` row 2019/20 holds the real 2020 cup). `season_cups_calendar` reads this from the table: a file whose first row (2001/02) holds the real season "2002" alone is a calendar-year cup. The Irish classes play from August to May in the original (IrePresCup draws in June, plays in August, 0x6395f4, 0x639612), so they are split-season cups; their rows from 2003/04 hold the real calendar years 2004 on, which the game plays from August 2003 to May 2004.

The season S of the hook (slice 16) is the edition that `start_new_season` makes: a log of the jump from 2019 (`RECOMP_SEASON_CHANGE_LOG=1`, lines `cup_breaks: <file> season S, year Y`) shows Y + 1 = S for every cup that did not skip a season, the Norwegian Cup too (its change on 27 November 2019 is season 2020).

### Method

- **In a game** (changing rules on, or the hooks of the start season): in the before hook of season S, a cup whose edition S the table does not hold calls `season_change_skip()`. The cup keeps its finished edition and its year, makes no fixtures and plays no matches (club world doc, section 3.2). The pass asks again each season. When the table holds the cup again, the before hook finds the year of the last edition (lower than S - 1) and sets +0x40 to S - 1, so `start_new_season` builds edition S with fresh entrants and dates in the future.
- **At a new game** of a season game (start year not 2001): the constructor builds the first edition from its year argument. When the table does not hold that edition, the wrap gives the constructor the year of the next edition that the table holds, and the cup waits (club world doc, section 1). A cup that the table never holds again gets the year 2999. For that object the wrap of `League::create_fixtures` 0x6835c0 makes no fixture and returns 1 (the callers test the result, 0x927975), so no fixture of the year 2999 reaches a club's list. Such a cup never completes, so the pass never starts it again.
- A 2001 start keeps the original's 2001/02 (the Leinster Cup and the Northern Irish Charity Shield, not held in 2001/02, play in a 2001 game with changing rules on, and stop in 2002/03).
- News (Q258): one sentence in the season in which the table stops or starts holding the cup ("The X is not played in the 2020/21 season.", "The X is no longer played.", "The X is played again in the 2021/22 season."; calendar-year cups say "in 2020").

### Decision for Max: a skip, not a wait, for a gap in a game

The lead's rule for a cup that comes back was the wait of slice 26 (+0x40 at the next edition before `start_new_season`). For these classes that would make the next edition at once: the six League-based classes would put its fixtures into the clubs' fixture lists a season or more early (`League::create_fixtures` inside `start_new_season`, for example 0x9282f8), and every class would pick its entrants a season early (0x5bd4d0). The skip with a year fix at the return has neither problem, and it is the same state for a cup that never returns. At a new game the wait is the only way (no `start_new_season` runs for a new object); a League-based cup with a one-season gap at the start (Korean League Cup 2003, Irish League Cup 2002/03, Coppa Italia Serie C, Northern Irish and Welsh League Cups 2020/21) then holds the fixtures of its next edition from the start of the game. Recommended answer: keep the skip in a game and the wait at a new game.

### Not checked in a game run

The run `r2019` of `cup_breaks_run.py` checks Belgium, Holland, Italy, Northern Ireland and Norway. The Irish, Welsh, Korean, Scottish, Polish, Swedish, Brazilian and American cups need Select All in the test steps (their nations are not in the 16 rows that the steps click); their classes follow the same code (section "The classes").

## 2. Round counts that differ from the class

A row of `cup_formats.tsv` gives the rounds counted from the final, as many as the class makes. The real early rounds that the class does not have are for clubs that the game does not hold (the Highland and Lowland League clubs in Scotland, the fifth tier in Spain), so the class keeps its round count and its entrants.

- **Scottish Cup from 2007:** part 21a already has the rows (replays to the quarter-finals, then on the day).
- **Spanish Cup from 2019:** single matches to the quarter-finals, semi-finals over two legs (`spa_cup` 2019), with no away goals from 2021 (`spa_cup` 2021). SpaCup `create_round_table` 0x84d220 (vtable 0x96f9c8 +0x3c) is wrapped.
- **A round that becomes two legs needs room** before the draw of the next round. The new check `room_for_second_leg` in `season_kind_cups.c` refuses a whole row when the second leg (match date plus the days) is not before the next draw (log "no room for a second leg"). The dates are the record fields +0x00 and +0x03 with the year offsets +0x02 and +0x05 from the cup's year (`Cup::update_for_date` 0x51fa2a..0x51fa30).
- **Not held, decision for Max (needs new round dates):**
  - Spanish Cup 2006/07 to 2018/19 (the round of 32 over two legs): SpaCup round 2 plays in early January and round 3 draws on 5 January (0x84d220, calls of 0x521eb0 and 0x521e60 for rounds 2 and 3), so a second leg falls after the next draw.
  - Turkish Cup quarter-finals and semi-finals over two legs (2005/06 to 2010/11, 2012/13 to 2019/20, 2021/22 to 2023/24, 2026/27): TurCup 0x8fc670 plays the quarter-final on 31 January and draws the semi-final on 6 February, plays the semi-final on 14 February and draws the final on 15 February.
  - Russian Cup two-leg rounds (2003/04 to 2006/07 from the round of 32, 2018/19 quarter-finals and semi-finals): RusCup 0x7e9530 plays round 1 on 12 October and draws round 2 on 13 October; the quarter-final plays on 4 April and the semi-final draws on 5 April.
  - Recommended answer: a later pass moves the draw and match dates of the next rounds when a round becomes two legs (the dates are in the round record, and the fixture manager already moves a fixture that clashes, `FIXTURE_MANAGER::add_fixture` 0x5aa680). Until then these cups keep single matches.
- The Turkish and Russian cups have no row: every season that the code can hold without two-leg rounds is the original's single matches. Golden goal (bit 0x80) stays in the classes that have it (TurCup, HolCup); the rules table names no season for its end.

## 3. Coppa Italia and KNVB Cup

`ItaCup` and `HolCup` are League classes with a group stage. The knockout stage is a `Cup_Stage` object (constructor 0x522e00), made by `create_knockout_stage` (ItaCup 0x63f870, call at 0x63fa10; HolCup 0x5f4480, call at 0x5f4755) when the group stage ends. Its round table comes from the class's `create_schedule` (vtable +0x3c: ItaCup 0x63ec50 in vtable 0x96c318, HolCup 0x5f36e0 in vtable 0x96b7b8) with stage 7 (ItaCup, call at 0x63f9c0, 5 rounds written at 0x63ed91) or stage 0x13 (HolCup, call at 0x5f4708, 6 rounds at 0x5f381d), in the same 0x68-byte rounds as the Cup classes, with the same arguments (stage, short *round count, short *first id, int *out; `ret 0x10`). `Cup_Stage` plays them with the Cup code, so the five format fields hold the format. Both functions are wrapped for those stages, and the rows apply as for a Cup class:

- `ita_cup` 2005: the first knockout round single, the rest two legs; 2007: the final a single match; 2008: single matches to the quarter-finals, semi-finals two legs; 2024: penalties after 90 minutes before the semi-finals.
- `hol_cup` 2009: the final over two legs (one season); 2010: a single final again.
- **Not held:** the group stages themselves (the Coppa Italia has none from 2005/06, the KNVB Cup none from 2003/04): see section 4.

## 4. Group stages and four-club super cups: decision for Max

Not built. Each needs a stage object the class does not make in that season:

- Group stage in a knockout cup (Scottish League Cup from 2016/17, EFL Trophy from 2016/17, Turkish Cup 2005/06 to 2010/11, 2012/13 to 2016/17 and 2024/25 to 2025/26, Russian Cup from 2020/21): the Cup classes make knockout rounds only (`create_round_table` and `set_up_rounds` 0x4c15f0). A group stage needs a League stage object before the knockout rounds, as ItaCup makes one (`create_group_stage` 0x63f640) and hands its winners to a `Cup_Stage` (`create_knockout_stage` 0x63f870).
- A group stage that ends (Coppa Italia from 2005/06, KNVB Cup from 2003/04): ItaCup and HolCup always start with the group stage (`create_group_stage` 0x63f640, 0x5f4220).
- Four-club super cups (Supercoppa Italiana 2023/24 to 2025/26, Supercopa de Espana from 2019/20): the classes make one round of two clubs. Semi-finals need a second round and two more entrants (the runners-up of the league and the cup).
- Recommended answer: a later slice builds a shared stage helper (a League group stage that feeds a `Cup_Stage`, as ItaCup does) and uses it for these cups. Until then they play the original's format.

## 5. New cups (slice 26's module): decision for Max

Not built. The module (`season_comps.h`, version 1) takes only a club_comp record that the data holds and no object uses. Every Update Team database checked has about five such records (CONMEBOL Cup, Recopa, CONCACAF Cup Winners Cup, Afro-Asian Champions Cup, European Cup Winners Cup), and slice 26 needs them for its own competitions. The six new domestic cups (Russian Premier League Cup 2003, the Turkish, Russian and Norwegian super cups, the All-Ireland cup, the Conference League Cup) each need a record of their own, so they wait for new records ("Open", point 1, of `2026-10-18-new-comps.md`). Each also needs a club picker for its nation (the original super cup classes read their own nation's key ids). Recommended answer: build them after the module can make new records. Q336 (EFL Trophy academy sides, National League Cup): not easy with version 1 of the module, so left out, as part 21a recommended.

## 6. Round names (Q370)

Not built: the shared table of slice 22 (`season_round_names.c`, `docs/research/round-names.tsv`) is not on main yet.
