# Slice 26 in parts (0.6.0, FIFA and other club competitions)

Date 2026-10-18. The build brief splits slice 26 into lettered parts after the research is read. This doc says what part a does, what is left and what the later parts need. Rules table: `rules-0.6.0/club-world.tsv`. Research of part a: `2026-10-18-club-world-idle.md`.

## Part a: the seasons of the original's competitions (built)

Rule kind `club_world` (`src/recomp_rt/win/season_kind_club_world.c`), registered in `season_change.c`. For each class of the original with a season that the table does not hold, the object stays and its season year +0x40 is the year of its next edition (the state that the original uses between two Club World Championship editions). A competition that the table never holds again waits for the year 2999.

| Class | Seasons with an edition (table) | What the game does now |
|---|---|---|
| World_Club_Champ (FIFA Club World Championship) | 2005/06 to 2024/25, every year | First edition 2005/06; one edition a year, drawn on 1 July (was 5 February, every second year); none after 2024/25 |
| World_Club_Cup (Intercontinental Cup) | to 2004/05 | Waits from 2005/06 for ever |
| Inter_Amer_Cup | none | Waits from the new game |
| Con_Merc_Cup (Merconorte Cup), Conmebol_Merc (Copa Mercosur), Asia_Cup_Winner | 2001/02 only | Wait from 2002/03 |
| Asia_Super_Cup | 2001/02 and 2002/03 | Waits from 2003/04 |
| Asia_Club_Champ | every season but 2022/23 | Skips 2022/23 |
| Oceania_Club_Champ | 2004/05 to 2019/20, 2021/22 on | One edition a year from 2004/05; skips 2020/21 |

Slice 16 offers `season_change_skip()` (the competition keeps its finished season). Part a does not use it: after a gap a skipped League class keeps the start date of its old year, so it never draws again (`2026-10-18-club-world-idle.md`, section 1), and the Oceania Champions Cup and the Asian Club Championship come back after a gap. A competition that waits for its next edition is also not complete, so the pass does not ask it again each season.

Paths: at a new game of a season game (start year not 2001, or changing rules on) the constructors get the year of the first edition; with changing rules on the `before` hook of `start_new_season` moves the year at each season change, and the `after` hook adds one news sentence when the table starts or stops holding a competition. With changing rules off the rules of the start season stay (Q232): a competition that the start season does not hold never comes, and the Club World Championship keeps the form of the start season. The base game and `RECOMP_SEASON_CODE=off` are unchanged (every patch gives the original's value outside a season game).

Test: `tests/seasons/club_world_check.py` (its docstring gives the runs and checks).

Not covered by the tests: the CONCACAF classes (entry 4 is made only when the United States is selected, and the United States is below row 16 of the Select League(s) screen, so only Select All selects it).

## Left for later parts

### 26b: club counts and formats of the original's competitions

**Built (2026-10-18): the Club World Championship knockout (Form K).** Research: `2026-10-18-club-world-formats.md`. In a season game an edition with 6 or 7 clubs in the table (2005/06 to 2023/24) is one Cup_Stage made on the start date (1 July): for 7 clubs a play-off of the host champion and the OFC winner, quarter-finals with the AFC, CAF and CONCACAF winners, the UEFA and CONMEBOL winners from the semi-finals, the final and the third place match; for 6 clubs the quarter-finals of the AFC, CAF, CONCACAF and OFC winners (2020/21: the host champion in place of the OFC winner). The clubs are the original's picks (`pick_entrants` 0x92a8b0) in round order. The matches keep the original's weeks (1 to 12 August), not December: the fixture checks then stay as they are (Q277, a late edition plays at its normal time). Left out: the fifth place match (no class of the original has one) and the rule of 2011/12 on of penalties with no extra time in the place matches. Code: the part b block of `season_kind_club_world.c`. Tests: `club_world_check.py` runs 2004-on, 2007 and 2007-load.

The rules report reads the parent's club count +0x3e, which is 0 for Form K, so its `clubs` line for this class says 0 against 6 or 7. That line stays in the expected files: the report needs another count for a class whose clubs are all in a stage (owner of `season_report.c`: slice 12).

**Not built yet:**

- Form G, the 32-club edition of 2024/25 (8 groups of 4 and a knockout from the round of 16, with the Libertadores builders, doc section 4). That season plays the original's 12-club form.

- The Club World Championship host comes from `HostCountry::pick_club_world_champ_hosts` 0x5fab70 (random): the host of the table needs the host table that slice 27 owns.
- Copa Libertadores: 38 clubs from 2005, 47 from 2017 with a calendar to November, a single final from 2019, no away goals from 2022 (`Conmebol_Seeding` 0x4c5fe0, places at 0x4c6168 and 0x4c617a, `assign_libertadores_places` 0x4c6430).
- CONCACAF: Champions Cup of 8 to 2008 (0x4c11a0), Champions League 2008/09 to 2023, Champions Cup of 27 from 2024; the three Leagues Cup places go to the next MLS clubs in the league table (Q333).
- Asian Club Championship to AFC Champions League from 2002/03 (entrants 0x40b950), autumn to spring from 2023/24, ACL Elite from 2024/25. Australian clubs: Oceania to 2005, the ACL from 2007. Australia's confederation is slice 27's.
- Oceania: 8 clubs (0x798220). The table has more clubs from 2005.

### 26c: new competitions

Copa Sudamericana (2002/03 on), Recopa Sudamericana (2003/04 on, with gaps), FIFA Intercontinental Cup (2024/25 on), AFC Champions League Two (2024/25 on), Suruga Bank Championship (2008/09 to 2019/20, Q334). The build brief gives them to the shared new competitions module (`season_comps.c`); no branch holds it on 2026-10-18.

Facts for the module design (read from the data on 2026-10-18; the record index is the position in `club_comp.dat`):

| Record | Index (3.9.68, September 2009, October 2024, December 2025) | Object in the original |
|---|---|---|
| South American CONMEBOL Cup | 60 | none (key id 0x9cf644, sweep-europe 1.1) |
| South American Recopa | 61 | none (key id 0x9cf648, matched at 0x60f799) |
| CONCACAF Cup Winners Cup | 344 | none (key id 0x9cf730) |
| Afro-Asian Champions Cup | 373 | none |
| PrepreLibertadores Cup, PreLibertadores | 218, 219 | unknown |

Inference: a new competition can take a record that the data already holds and no object uses, renamed through `season_change_rename` (the Recopa its own record; the Copa Sudamericana the CONMEBOL Cup record, the competition that it replaced in 2002). That needs no new record in the array at [0xae23d0] and no new key id. An object that waits for ever (part a: the Copa Mercosur, the Merconorte Cup, the Asian Cup Winners' Cup, the Intercontinental Cup, the Inter-American Cup) is saved and loaded like every object, so a kind could give it a new record and a new club picker in place of a new object, vtable and save. The classes read their key id globals, not their record (sweep-europe 2.3), so each reused class needs its club picker and its record reads changed. This is a design choice for the module owner and the lead.

### 26d: substitutes and bench

The `subs_used` and `bench` lines (Club World Championship 5 substitutes from 2020/21, bench 12) are slice 17's kind (substitutes and bench of every part). Part a does not change them.

## For Max

1. Two editions in one season (Recopa in 2006/07, 2008/09, 2014/15 and 2016/17, Copa Sudamericana in 2016/17, separated by `||`). Recommended answer: play both, the second as a second edition of the same object after the first ends. It applies when part c builds these competitions.
2. A competition that ends for good (the Intercontinental Cup after 2004/05, the Inter-American Cup) still shows in the competition menus with its last edition. Recommended answer: keep it in the menus, so its history stays easy to find.
