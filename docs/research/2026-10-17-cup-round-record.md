# Domestic cups by season (0.6.0 slice 21)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `src/recomp_rt/win/season_kind_cups.c` (the rule kind), `season_cups.c` and `.h` (the round types), `season_cups_table.c` (made by `tools/seasons/make_cups_table.py` from `tools/seasons/cup_formats.tsv`). Tests: `tests/seasons/test_season_cups.c` (ctest `season_cups`, `season_cups_table`) and `tests/seasons/season_cups_run.py`.

## What the slice does

The rules table gives the format of each domestic cup as text. The cup classes of the original hold five fields for each round: the match flags of the first match, the flags of the deciding match, the replays allowed, the legs and the days to the replay. The slice wraps `create_round_table` of each cup class that has a row (8 classes, `season_kind_cups.c`) and writes these five fields into the table that the function returns, before the caller stores it. The original also makes the table anew at times that are not a season start (the FA Cup table was made again by 1 June of its first season, and a log of every `start_new_season` showed no call in between), so a write after a season start was lost. Whatever makes the table, it holds the format of the season:

- The season of a cup is its season year (`cup+0x40`, set before the table is made) with changing rules on, and the start year of the game with changing rules off.
- The news sentence of a row comes in the `after` hook of the season change, once for the cup that starts the season of the row (changing rules on, the cup holds the row's round count).
- This slice is part 21a. Part 21b holds the rest: cups that start or end, new cups, four-club super cups, group stages, other round counts, the Coppa Italia, the KNVB Cup and the round names (Q370).

`cup_formats.tsv` holds one row for each cup and season in which the format changes, as one word for each round (E, P, R, L, LN, LH, or . to keep the round). A row holds the whole format from its season on. A cup with no row for the season keeps the rounds of the original. The first row is 2002: a game that starts in 2001/02 with changing rules off gets nothing, and the table has no row at or before 2001, so a base game is the same as 0.5.0.

A row is applied only when the cup holds the row's round count (`cup+0x3c`). If it holds another count, nothing is written and the log shows "not applied" (`RECOMP_SEASON_CHANGE_LOG=1`).

The fixtures of a round are made when its draw comes, after the season start, so no fixture holds the old flags. `Cup::map_fixture_tree` 0x518790 reads only +0x0d bits 0x200 and 0x400, +0x1a, +0x1c, +0x1e and +0x82 (the next round). It does not read +0x21 or +0x20, and the type words keep bits 0x200 and 0x400. A saved game keeps its table (`Cup::save_cup_to_disk` 0x5210f0 writes the rounds, `Cup::load_from_disk` 0x51fc00 reads them without a rebuild).

The rounds as the game makes them in 2001/02 (rules report line `cup`, base run of `season_cups_run.py`, legs:replays:first:deciding, flags in hexadecimal):

| Cup | Rounds |
|---|---|
| eng_fa_cup | 0:1:0:3, then six rounds 1:1:0:3, then two rounds 1:0:3:0 |
| eng_cc_cup | 1:0:403:0, 1:0:403:0, 1:0:3:0, 1:0:203:0, 1:0:203:0, 2:0:4200:4003, 1:0:3:0 |
| eng_fa_trophy | 1:2:0:2, 0:2:0:2, 0:2:0:2, 2:1:0:4, 1:1:0:3 |
| bel_fa_cup | 1:0:401:0, 1:0:403:0 twice, 1:0:3:0 twice, 1:1:2:3, 2:0:0:3, 1:1:2:3 |
| fra_cup, fra_lge_cup | every round 1:0:3:0 (9 and 7 rounds) |
| nor_cup | 1:0:403:0 twice, 1:0:3:0, then four rounds with a replay (0:1:2:3, 1:1:2:3 twice, 0:1:2:3) |

## Formats the code cannot hold

Each one has the reason. A cup that is not listed has no row in the rules table that differs from what the original plays, or no round structure that was checked.

- **A cup that is not held in some seasons** (the `not_held` rows of the rules table). Slice 21 builds nothing for them. Two cases:
  - The original has a class and an object, and the table says not held. The object would have to go idle: skip `start_new_season` for the season (the generated wraps in `season_starts.c` cannot veto a call yet), add 1 to the cup's season year (+0x40) so that it resumes in step, and do the same at a new game, where the constructor has already made the first season. Cups: Coupe de la Ligue from 2020, Korean League Cup from 2011, K League Super Cup 2006 to 2024, Welsh Premier Cup from 2008, Irish Super Cup from 2002, Irish League Cup from 2020, Irish Presidents Cup 2003 to 2012, Leinster Cup 2001 to 2008, Copa dos Campeoes from 2002, Asian Super Cup from 2003, Inter-American Cup (every season of the table), the Japanese cups in 2025, the Norwegian Cup in 2019, and one-season gaps in 2019 to 2021 (Belgian and Dutch Super Cups, Italian Serie C cups, Scottish Challenge Cup, Welsh cups, Northern Irish cups). The rules report already lists the Inter-American Cup and the Intertoto Cup as `not_held` differences.
  - The original has no class: the table row has `comp_file` new. A new competition needs a record, a key id, an object, a save and a load (`season_change_add_comp`, and the shared module `season_comps.h`, which is not on main). Cups: Conference League Cup (2004 to 2008, National League Cup from 2024), Russian Premier League Cup 2002, Finnish League Cup, the Turkish, Russian and Norwegian Super Cups, the Serie C2 Supercoppa, the All-Ireland cup, the Australia Cup, the Suruga Bank Cup, the Recopa and the Brazilian regional cups.
- **Four-club super cups** (Supercoppa Italiana 2023 to 2025, Supercopa de Espana from 2019): the classes make one round with two clubs. A semi-final round needs a new round table and fixtures, which `set_up_rounds` makes for the class, not for the season.
- **Group stages in a cup** (Scottish League Cup from 2016, EFL Trophy from 2016, Turkish Cup 2005 to 2010 and 2012 to 2025, Russian Cup from 2020, Finnish Cup groups 2016 to 2020, Scottish Challenge Cup league phase 2025): the cup classes hold knockout rounds only.
- **A round count that differs from the class**: Scottish Cup from 2007 (eight rounds and more, the class makes seven), Spanish Cup from 2005 (nine rounds, the class makes seven), Belgian Cup early rounds (ten rounds, eight), Turkish Cup, Russian Cup, Norwegian Cup 2025 and later, Dutch KNVB Cup rounds. For the Scottish and Belgian cups the rows set only the rounds counted from the final, which the class holds (replays after the quarter-finals, semi-final legs, the final).
- **Italy and Holland** (Coppa Italia, KNVB Cup): `ItaCup` and `HolCup` are league classes with a knockout table in `create_schedule` (0x63ec50, 0x5f36e0), not a round table. The single-match final of the Coppa Italia from 2007, the single-leg rounds from 2008 and the two-leg KNVB Cup final of 2009/10 need a patch of those functions.
- **Entrant counts**: the FA Cup holds 152 entrants (a fixed array, `EngFaCup::collect_entrants` 0x56fab0), the League Cup 92. The cup rows give other counts in some seasons (Scottish Cup 106 to 137, Copa del Rey 80 to 126). Not changed.
- **Cups of a nation the game cannot select** (Q288) are out.
- **Q336 (EFL Trophy academy sides from 2016/17, National League Cup from 2024/25):** not built. It needs a new competition (see the first point), so it is not easy. Recommended answer: leave it out of 0.6.0.
- **Not known**: whether the FA Cup qualifying rounds of 2021/22 had replays (the table keeps the original's `R` for round 0), and the Belgian Cup rounds 1 to 4 that go straight to penalties in 2023/24 and 2024/25 (no row).
- **Not checked in a game run**: `sco_fa_cup` (Scotland is not one of the 16 nations that the test steps can select), the Spanish Cup and the German, Portuguese, Swedish and Danish cups (no row).

## Cup round record research (from the code)

The sections below are the reading of the original's code for this slice. "Inf." marks an inference.

Method: objdump of the original plus REA. Ghidra has no function at 0x51c020 or 0x51f2f0, but both are real entries: they are Cup vtable 0x9693c8 slots +0x98 and +0x9c. "Inf." marks an inference. "Fixture" is the 0x4f-byte match record (copied with `rep movsd` ecx=0x13 at 0x51a803).

## 1. Fields of the round record (table at cup+0xa3, cup+0x3c = number of rounds)

| Off | Size | Meaning | Evidence (reader) |
|---|---|---|---|
| +0x00 | w+b | Draw date: day, year offset (+0x02) | written by cup_set_round_draw_date 0x521ea6; read 0x51fa2a |
| +0x03 | w+b | Match date: day, year offset (+0x05) | written 0x521ef6; read 0x51ce7c, 0x51ce6c |
| +0x06 | b | Kick-off slot, copied to fixture+0x3f | 0x521f05, 0x51ce85 |
| +0x07 | w | Round id (comp_util id: 10, 20 .. 120 QF, 130 SF, 150 F, 220/230 early). Not a team count. Copied to fixture+0x30, +3 for a first leg | 0x51ccf5..0x51ccff |
| +0x09 | w | Round index, copied to fixture+0x34 | 0x51cd04 |
| +0x0b | w | Draw rules, copied to fixture+0x36. Bit 0x1: home-team rule; bit 0x4: keep clubs of one nation apart | 0x51e8b2, 0x51c5d4, 0x51d3d8 |
| +0x0d | w | Match flags of the first match (both legs of a two-leg tie), copied whole to fixture+0x38. Also round bits 0x100, 0x200, 0x400 (see 2) | 0x51cd1d |
| +0x0f | w | Extra match flags for the deciding match: OR'd into the second leg after leg 1, or used as the flags of the first replay | 0x521d76 (from 0x51a6b3), 0x51a9f0 |
| +0x11 | w | Flags of a second replay, or of the play-off after a level two-leg tie | 0x51aa26, 0x51ae44 |
| +0x13 | dw | Venue flags, passed to 0x85c260 when the fixture is made | 0x521f08, 0x51ceb9 |
| +0x17 | b | Copied to fixture+0x3a. 4 before the QF, 6 from the QF in every class. Meaning unknown | 0x51cd0d, 0x51b74a |
| +0x18 | w | Teams in round (inf., FA Cup r0 56, r1 80) | writes only seen |
| +0x1a | w | Ties in round (loop bound, random slot range) | 0x51c4ba, 0x51929d, 0x51cbdf |
| +0x1c | w | New entrants this round (summed over rounds) | 0x51a2b9 |
| +0x1e | w | First entrant index (inf.) | 0x51941f |
| +0x20 | b | Replays allowed. >0 sets fixture+0x4e bit 0x4, which shows "Match replayed if scores are level..." (strings 0x98f294, 0x98f2cc, read 0x45e286). >1 lets a first replay draw again. In a cup with one round (cup+0x3c==1) a draw with replays used up ends with both clubs marked (shared) | 0x51cdf5, 0x51d11a, 0x51a9fd, 0x51a716 |
| +0x21 | b | Legs. 2 = two-leg tie; 0 or 1 = one match | 0x51a48c, 0x51ccee, 0x51cd87, 0x51d10d |
| +0x22 | b | Days from the match to the replay or second leg (Date::add_days) | 0x51a85d, 0x51d169 |
| +0x23 | 8x7 | Match-day list (weekday, slot, code, dword) used with bit 0x100 | cup_set_round_match_day 0x521f10 |
| +0x5b | b | Read once, meaning unknown | 0x521d41 |
| +0x5c | dw | Money paid to both clubs when a match is played | 0x51a2e4 |
| +0x60, +0x64 | dw | Round prize money; inf. winner and loser | pay_round_prize_money 0x522282 |

The record is not zeroed (operator new 0x944e46). The FA Cup never writes +0x11, so it holds heap bytes. No path reads it there.

## 2. Bits of the match flags word (fixture+0x38, fed by +0x0d, +0x0f, +0x11)

| Bit | Meaning | Evidence |
|---|---|---|
| 0x0001 | Penalties if level (after ET if 0x2 is set, else at 90 min). Winner code 4 | engine 0x6bd930 (sets 0x8eb3=4); 0x51c1c9, 0x51f79e |
| 0x0002 | Extra time if level at 90 min (aggregate in a second leg) | 0x6bd930 at period 0x3de; 0x51c10f, 0x51f696 |
| 0x0004 | Away goals rule off | 0x6bd930 (`uVar2 & 4`), 0x51f751 |
| 0x0010 | Legs not aggregated, each leg decided alone (inf.) | 0x51c0a5, 0x51f31d, 0x51a689 |
| 0x0020 | Penalties, second variant (winner code 5, engine 0x8eb3=1). Difference unknown. No listed cup sets it | 0x51c21c, 0x6bd930 |
| 0x0080 | Golden goal in extra time | is_golden_goal_active 0x6d4269 |
| 0x0100 | Round bit: put fixtures on the match-day list | 0x51ec3d, 0x51cf8f |
| 0x0200 | Round bit: no random tree links and no draw news (inf.: fixed bracket) | 0x519279, 0x51c65a |
| 0x0400 | Round bit: seeded draw (sort_round_teams) | 0x51c4c7, 0x5192b2 |
| 0x0800 | Home side goes through if level (winner code 8) | 0x51c323, 0x51f64c |
| 0x1000 | Away side goes through if level | 0x51c349, 0x51f673 |
| 0x2000 | Outcome-code variant for league-style use. Unknown, unused here | 0x51c0c5, 0x51f3b0 |
| 0x4000 | Away goals count only after ET (League Cup rule): no away-goal decision at 90, away goals in ET decide | 0x6bd930, 0x51f6aa, 0x51f755 |

When +0x0d has 0x800, the tie code replaces the whole word by 0x1000 (home level >= away) or 0x800 (0x51cdc1). apply_round_rules 0x521d60 does the opposite mapping and sets 0x1 when levels are equal. No listed cup sets 0x800, so this conflict is untested.

A single match that ends level with no 0x2/0x21/0x800/0x1000 returns 0 from 0x51c020. In a cup with more than one round, process_fixture then always makes a replay (0x51a6e0 to 0x51a803), whatever +0x20 says. Replay 1 gets +0x0f, replay 2 gets +0x11, a third level replay hits an assert (0x51aac5).

## 3. Encoding of the three formats

- Two legs: +0x21 = 2. Both legs are made at once (second leg id +4, date +0x22 days, 0x51d12f). Leg flags come from +0x0d (normally 0). After leg 1, +0x0f is OR'd into the second leg (normally 3 = ET + penalties; add 0x4 for no away goals, 0x4000 for the League Cup rule). If level after both legs and no penalties bit: play-off with +0x11 flags (0x51ac80).
- One match + replay: +0x21 = 1, +0x0d without 0x1/0x2/0x20 (0 or 0x2 for ET only), +0x0f = flags of the replay (3 = ET + penalties), +0x20 = 1 (2 for a second replay with +0x11), +0x22 = days to the replay.
- One match, ET and penalties on the day: +0x21 = 1, +0x0d = 3 (0x83 for golden goal, 0x1 for penalties without ET), +0x20 = 0.

## 4. Per class (all write the rule fields inline with mov; dates and venue through 0x521e60 and 0x521eb0)

Cell = legs : +0x0d / +0x0f, "rN" = +0x20 replays. Rounds in order, first round first.

| Class | create_round_table | N | Rounds |
|---|---|---|---|
| EngFaCup | 0x56f320 | 9 | r0..r6 1:0/3 r1 (r0 legs byte 0); SF, F 1:3/0 |
| EngCcCup | 0x56c570 | 7 | 1:403, 1:403, 1:3, 1:203, 1:203, SF 2:4200/4003 (21 days), F 1:3 |
| EngAutoCup | 0x56ade0 | 5+1 | stage 0: 1:83 x4, area final 2:0/83 (7 days); stage 1: final 1:83 |
| EngFaTrophy | 0x570ec0 | 5 | 1:0/2 (+0x11=3) r2 x3; SF 2:0/4 (+0x11=3) r1; F 1:0/3 r1 |
| ScoFaCup | 0x7ed5d0 | 7 | 1:0/3 r1 x6; F 1:3 |
| ScoLgeCup | 0x7efcb0 | 6 | 1:3, 1:403, 1:3 x4 |
| SpaCup | 0x84d220 | 7 | 1:3 x3 (+0x0b=3); 2:0/3 x3 (7, 7, 14 days); F 1:3 |
| ItaCup | none, League-based; knockout table in ItaCup::create_schedule 0x63ec50 | 5 | 2:400/3 x2, 2:0/3 x3 (final two legs) |
| FraCup | 0x5ba3f0 | 9 | 1:3 x9 |
| FraLgeCup | 0x5bcc40 | 7 | 1:3 x7 |
| GerCup | 0x5d8ee0 | 6 | 1:403 x3, 1:3 x3 |
| HolCup | none, League-based; knockout table in HolCup::create_schedule 0x5f36e0 | 6 | 1:283, 1:83 x5 |
| PorCup | 0x7cdc40 | 8 | 1:2/3 r1 x7; F 1:3 |
| BelFaCup | 0x41d310 | 8 | 1:401, 1:403 x2, 1:3 x2, QF 1:2/3 r1, SF 2:0/3 (14 days), F 1:2/3 r1 |
| TurCup | 0x8fc670 | 7 | 1:83 x7 |
| RusCup | 0x7e9530 | 6 | 1:3, 1:403 x4, F 1:3 |
| NorCup | 0x78f2e0 | 7 | 1:403 x2, 1:3, 1:2/3 r1 x4 (final included) |
| SweCup | 0x88cf40 | 2 / 6 | stage < 7: 1:83 x2; stage 7: 1:83 x5, F 2:0/83 (7 days) |
| DenCup | 0x550c70 | 8 | 1:3 x6, SF 2:0/3 (7 days), F 1:3 |
| FinCup | 0x592510 | 7 | 1:3 x7 |

## 5. When the table is built, saving, and mid-season changes

- Built at construction (EngFaCup 0x56f13b, vtable +0x3c) and again every season: each listed start_new_season frees +0xa3 and the tie arrays, then calls +0x8c set_up_rounds (0x4c15f0 calls create_round_table with stage -1) and +0x94 map_fixture_tree. Calls seen at 0x57049a, 0x56d22e, 0x56b61d, 0x57158d, 0x7edefd, 0x7f0a31, 0x84e06e, 0x5baf3d, 0x5bd50d, 0x5d986d, 0x7ce62d, 0x41dc7d, 0x8fce9d, 0x7e9dfd, 0x78fc7d, 0x88d786, 0x5515ed, 0x592ded. ItaCup and HolCup call their own +0x8c (League slot).
- Saved: save_cup_to_disk writes cup+0x3c records of 0x68 bytes (0x521439..0x521448). load_from_disk reads them back with fread (0x5208c5..0x5208ce) and does not call create_round_table. A rule in create_round_table therefore takes effect at the next season start or in a new game. A save keeps the old table until then.
- Mid-season (inf. from the read sites):
  - +0x0d, +0x0b, +0x21, +0x22 are read when a round's fixtures are made (create_tie_fixtures). A change after that has no effect on fixtures already in the fixture manager, which hold their own flags copy.
  - +0x0f, +0x11 and +0x20 are read when a match is processed. A change takes effect on the next replay or second leg.
  - A switch of +0x21 after the fixtures exist breaks the tie. A two-leg tie already has its second leg in the fixture manager (0x51d10d). process_fixture picks the decider from +0x21 and tie+0x33 (0x51a48c), so a mismatch gives wrong results or asserts.
  - +0x1a, +0x18, +0x1c and the number of rounds size the tie arrays (map_fixture_tree, season start). Do not change them mid-season.
- Safe rule: change the record only inside create_round_table, keyed on the season (cup+0x40 = year, as the FA Cup already does for exemptions via cup+0xb3/+0xb4).
