# Sweep: continental and world club competitions (slice 2, sweep-europe)

Date 2026-10-16. Slice 2 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Research only. No code changes.

Scope: the 21 files `european_cup`, `uefa_cup`, `intertoto_cup`, `eur_super_cup`, `uefa_seeding`, `rb_europe`, `world_club_champ`, `world_club_cup`, `con_champ`, `con_merc_cup`, `conmebol_liber`, `conmebol_merc`, `conmebol_seeding`, `asia_club_champ`, `asia_cup_winner`, `asia_super_cup`, `oceania_club_champ`, `inter_amer_cup`, `rb_south_america`, `rb_asia` and `rb_oceania`. The function map lists 328 addresses for them.

Names: `/Users/max/Projects/cm0102/research-0.6.0/sweeps/sweep-europe.jsonl`, one line for each of the 328 addresses (model opus).

Method: the dossiers in the function map work folder, the assembly of the clean 3.9.68 exe (capstone), the vtables read from `.rdata`, and a scan of every instruction of these files for immediates from 1990 to 2030. All addresses are virtual addresses. "Inference" marks a statement that the code does not prove.

## 1. How the original picks its club competitions

### 1.1 By name, once, at data load

1. `set_competition_id_by_name` (0x60efd0, index.cpp) compares a club_comp long name (record +4) with 351 fixed strings. The compare is `_stricmp` (0x951c50), so case does not matter.
2. A match stores the record index in a global. The index of a record is its position in club_comp.dat. The caller loop is at 0x60425d. It runs over `DAT_00ae2378 - 0x7f` records.
3. Every global starts at -1 (0x602251 and on).
4. The game finds a record as `index * 0x6b + DAT_00ae23d0`.

The globals of this sweep:

| Global | Names that match |
|---|---|
| 0x9cf6e8 | UEFA European Champions League, UEFA Champions League, European Champions Cup |
| 0x9cf6ec | European Cup Winners Cup |
| 0x9cf6f0 | UEFA Cup |
| 0x9cf6f4 | European Super Cup |
| 0x9cf6f8 | Inter-toto Cup |
| 0x9cf6e4 | World Club Cup, Intercontinental Cup |
| 0x9cf7bc | FIFA Club World Championship |
| 0x9cf95c | inter-american cup |
| 0x9cf72c | CONCACAF Champions Cup |
| 0x9cf730 | CONCACAF Cup Winners Cup |
| 0x9cf68c | CONCACAF Merconorte Cup, Merconorte Cup |
| 0x9cf63c | South American Copa Libertadores |
| 0x9cf698 | South American Copa Mercosur |
| 0x9cf640, 0x9cf644, 0x9cf648 | South American Super Cup, South American CONMEBOL Cup, South American Recopa |
| 0x9cf64c | Asian Champions Cup, Asian Club Championship |
| 0x9cf650 | Asian Cup Winners Cup, Asian Cup Winners' Cup |
| 0x9cf7d0 | Asian Super Cup |
| 0x9cf7cc | Oceania Club Championship, Oceania Champions Cup |

Nation and club globals come from two more name matchers in index.cpp: nations at 0x60d810 and clubs at 0x6146b0. The seeding code uses them (section 3).

### 1.2 Which objects exist

1. The table at 0xb63d60 holds 35 entries of 0x48 bytes (see `2026-10-14-game-creation-inventory.md`). Entries 2 to 7 are the continents. `key_nation_build_table` (0x667150) writes a creator function into field +8 of each.
2. The creators are in Setup.cpp:

| Entry | Creator | Objects, in array order | Count |
|---|---|---|---|
| 2 (world) | 0x830fe0 | World_Club_Champ (0x9cf7bc), World_Club_Cup (0x9cf6e4), Inter_Amer_Cup (0x9cf95c) when that record exists | 2 or 3 (0x8310b3, 0x8310bc) |
| 3 (Europe) | 0x8312e0 | European_Cup (0x9cf6e8), Uefa_Cup (0x9cf6f0), Intertoto_Cup (0x9cf6f8), Eur_Super_Cup (0x9cf6f4) | 4 (0x8313bd, array 0x10 bytes at 0x8313bb) |
| 4 (North America) | 0x831650 | Con_Champ (0x9cf72c), Con_Merc_Cup (0x9cf68c) | 2 |
| 5 (South America) | 0x8318f0 | Conmebol_Liber (0x9cf63c), Conmebol_Merc (0x9cf698) | 2 |
| 6 (Asia) | 0x831ba0 | Asia_Club_Champ (0x9cf64c), Asia_Cup_Winner (0x9cf650), Asia_Super_Cup (0x9cf7d0) | 3 |
| 7 (Oceania) | 0x831eb0 | Oceania_Club_Champ (0x9cf7cc) | 1 |

3. A creator makes its objects only when `key_nation_continent_has_selected_nation` (0x6699d0) is true: at least one selected nation of that continent is in the key nation table. Else the count is 0.
4. `setup_player_index_table` (0x8386f0) then fills `DAT_00adadfc`: one pointer for each club_comp record, at the record id. Code finds a competition object as `DAT_00adadfc[global]`.
5. Each object gets the game year at creation (`DAT_00ae2c90` high word, stored at object +0x40).

### 1.3 Every season, no choice

The set of competitions never changes during a game. Each object moves its own year at the start of its next season:

- plus 1: European_Cup 0x584e3b, Uefa_Cup 0x902f91, Intertoto_Cup 0x6339be, Eur_Super_Cup 0x57a76e, World_Club_Cup 0x92be3e, Inter_Amer_Cup 0x6329ae, Con_Champ 0x4c14e2, Con_Merc_Cup 0x4c2023, Conmebol_Liber 0x4c39f1, Conmebol_Merc 0x4c50b3, Asia_Club_Champ 0x40b7e1, Asia_Cup_Winner 0x40d691, Asia_Super_Cup 0x410d00
- plus 2: World_Club_Champ 0x929d9e, Oceania_Club_Champ 0x797dbe

So the Intertoto Cup, the Copa Mercosur and the Merconorte Cup run for ever.

### 1.4 What this means for 0.6.0

1. A new club_comp record, such as "UEFA Europa Conference League", never runs. No matcher string names it and no creator makes an object for it.
2. A rename in club_comp.dat before load breaks the match. For example, "UEFA Europa League" leaves 0x9cf6f0 at -1, and the European creator then builds the UEFA Cup on record -1 (inference: a crash or a corrupt competition). The names table of Q238 must apply after 0x60efd0 has run.
3. The Cup Winners' Cup record (0x9cf6ec) is matched but no object exists. Only `Eur_Super_Cup::pick_clubs` reads it, in 1999 (0x57a5f2).
4. The record "UEFA Nations League" (455 in May 2026) is a nation_comp record. These files do not create national-team competitions. See the sweep-international part.

## 2. Classes, vtables and slots

### 2.1 Class of each competition

| Class | File | Base | Vtable | Object size (operator new) |
|---|---|---|---|---|
| European_Cup | european_cup.cpp | Cup | 0x96a1d0 | 0xb2 |
| Uefa_Cup | uefa_cup.cpp | Cup | 0x970790 | 0xb3 |
| Intertoto_Cup | intertoto_cup.cpp | Cup | 0x96bb8c | 0xb6 |
| Eur_Super_Cup | eur_super_cup.cpp | Cup | 0x969f80 | 0xb2 |
| World_Club_Cup | world_club_cup.cpp | Cup | 0x971250 | 0xb2 |
| Inter_Amer_Cup | inter_amer_cup.cpp | Cup | 0x96bad8 | 0xb2 |
| Con_Champ | con_champ.cpp | Cup | 0x968aa0 | 0xb2 |
| Asia_Club_Champ | asia_club_champ.cpp | Cup | 0x9674d4 | 0xba |
| Asia_Cup_Winner | asia_cup_winner.cpp | Cup | 0x967574 | 0xba |
| Asia_Super_Cup | asia_super_cup.cpp | Cup | 0x9676c8 | 0xb2 |
| World_Club_Champ | world_club_champ.cpp | League | 0x97119c | 0xf6 |
| Con_Merc_Cup | con_merc_cup.cpp | League | 0x968b40 | 0xf6 |
| Conmebol_Liber | conmebol_liber.cpp | League | 0x968bf4 | 0xf6 |
| Conmebol_Merc | conmebol_merc.cpp | League | 0x968ca8 | 0xf6 |
| Oceania_Club_Champ | oceania_club_champ.cpp | League | 0x96e018 | 0xf6 |

Group stages are `League_Stage` objects (0x68b100, 0xee bytes). Knockout stages are `Cup_Stage` objects (0x522e00). The parent keeps its stages in the array at +0xc, the stage count at +0x2c and the current stage at +0x30.

The rule books `RbEurope` (0x96eb34), `RbAsia` (0x96ea14), `RbOceania` (0x96ed74) and `RbSouthAmerica` (0x96ee34) are ruling bodies on the RulingBody base (0x7e7760). They set the card bans of continental club matches. Europe and Oceania ban after a red card or every third booking. Asia and South America ban after a red card or every second booking.

### 2.2 Slots that a competition overrides

Cup base vtable 0x9693c8 has 40 slots. League base vtable 0x96d020 has 45. Slot n is at offset 4n.

| Slot (offset) | Base method | Overridden by |
|---|---|---|
| 0 (0x00) | scalar deleting destructor | all |
| 1 (0x04) | Cup::process_fixture | European_Cup, Uefa_Cup and Intertoto_Cup (coefficient points) |
| 2 (0x08) | Cup::start_new_season (0x518780) or League::start_new_season | all. This is the new-season step: new year, new entrants |
| 5 (0x14) | update_for_date | Intertoto_Cup, Con_Champ, Asia_Super_Cup and every League-based class (draw on the start date) |
| 8, 9 (0x20, 0x24) | Comp load and save of extra state | Uefa_Cup and Intertoto_Cup (field +0xb2) |
| 10 (0x28) | pre_exit_noop | every multi-stage class: advance_stage |
| 12 (0x30) | record_season_history | European_Cup (forwards to stage 12), shared thunks for others |
| 14 (0x38) | Comp::write_progress_text (0x48cab0) | European_Cup |
| 15 (0x3c) | create_round_table or create_schedule | all |
| 17 (0x44) | result finance and staff records | all multi-stage classes, Uefa_Cup, Intertoto_Cup |
| 18 (0x48) | Comp::build_fate_news_text | all multi-stage classes, Intertoto_Cup |
| 22 (0x58) | Comp::collect_club_list (0x48e1c0) | European_Cup, Uefa_Cup, Intertoto_Cup |
| 23 (0x5c) | rank_teams or set_predicted_positions | all multi-stage classes |
| 24 (0x60) | Comp::allows_fixture_clash (0x48e360): true when the round code is under 1000 and ends in zero, so the fixture checker accepts a date clash | the three UEFA cups (always false: no date clash allowed) |
| 26 (0x68) | set the club's stage codes in Comp_Stats (+0x73..+0x75) | all multi-stage classes |
| 35 (0x8c) | set_up_rounds or set_up_rules | all. Shared 0x858e70 when no prize fund is set |

Three shapes:

1. A one-round cup (super cups, World_Club_Cup, Inter_Amer_Cup): constructor, slots 0, 2, 15, 35 and a club picker.
2. A multi-stage Cup (European_Cup, the Asian cups): adds slots 10, 17, 18, 23, 26 and stage builders.
3. A League with stages (Libertadores, Mercosur, Merconorte, Oceania, Club World Championship): slots 0, 2, 5, 10, 12, 15, 17, 18, 23, 26, 35. Slot 5 picks the clubs and draws the groups on the start date.

### 2.3 Can a new competition reuse a class and slot?

1. The classes read fixed globals, not their own record. `European_Cup::fill_entrants` (0x584fa0) takes clubs with `+0x1db == DAT_009cf6e8`. `Uefa_Cup::build_entrants` (0x902b90) takes `DAT_009cf6f0`. `Intertoto_Cup::build_entrants` (0x6336c0) takes `DAT_009cf6f8`. A second object of one class would take the same clubs. So no class can run a second competition without a code change.
2. The cups also write into each other at fixed offsets. The Champions League puts its third qualifying round losers at UEFA Cup slot count-0x18 (0x585290). The Intertoto Cup puts its three winners at count-0x1b to count-0x18 (0x633deb).
3. The Europe array has a fixed count of 4 (0x8313bd). A Conference League needs a fifth slot or the Intertoto slot (array index 2) from 2009/10, when the Intertoto Cup stops. Reuse of the slot is possible. Reuse of the Intertoto class is not (inference from points 1 and 2).
4. A Conference League object needs: a comp id global, a Cup-based class with the Uefa_Cup slots (0, 1, 2, 8, 9, 15, 17, 22, 24, 35), a place function in the seeding, and entry slots for the clubs that drop from the other cups.
5. The 2024 league phase needs a new stage shape. The original's group stages are fixed groups of four (`League_Stage` with 4 clubs, 0x585290 and 0x585ab0).
6. Nation competitions such as the Nations League are not in these files. No class here fits them.

## 3. Seeding, places per nation and coefficients

### 3.1 UEFA seeding object

`Uefa_Seeding` is created at step 8 of game creation (0x903360, 0x698 bytes, pointer `DAT_00de1f58`). The constructor first waits for the game CD. Layout:

| Offset | Size | Content |
|---|---|---|
| +0 | 60 x 18 | One entry for each European nation |
| entry +0 | 4 | nation id |
| entry +4 | 4 | nation record pointer |
| entry +8 | 4 | float: five-season coefficient total |
| entry +0xc | 1 | Champions League places |
| entry +0xd | 1 | UEFA Cup places |
| entry +0xe | 1 | Intertoto places |
| entry +0xf | 2 | points this season |
| entry +0x11 | 1 | clubs entered this season |
| +0x438 | 1 | nation count |
| +0x439 | 2 | year of the last update |
| +0x43b | 4 | list of up to 30 English Intertoto volunteers |
| +0x43f | 1 | volunteer count |
| +0x440 | 50 x 12 | domestic cup id and two losing semi-finalists (0x905790) |

It is saved in `uefa_seeding.tmp` (load 0x905230, save 0x905510).

### 3.2 The yearly update

`Uefa_Seeding::update_season` (0x9035a0) runs from each UEFA cup's constructor and new-season step. When the game year differs from +0x439 it runs, in order:

1. `roll_coefficients` (0x903b50). Each nation record holds six doubles at +0xe0 to +0x108. The update shifts them down one place and writes last season's points divided by clubs entered into +0x108. The total is the sum of the five doubles +0xe0 to +0x100. Then it sorts the nations by total (0x905cb0) and writes the rank into nation +0x1df. In 2001 there is no shift (0x903b6f), so the first season uses the database values (inference: the doubles come from nation.dat).
2. `set_place_counts` (0x903cc0), by rank (0 is the top nation):

| Rank | CL places (+0xc) | UEFA Cup places (+0xd) |
|---|---|---|
| 0 to 2 | 4 | 3 |
| 3 to 5 | 3 | 3 |
| 6 to 7 | 2 | 4 |
| 8 to 14 | 2 | 2 |
| 15 to 20 | 1 | 3 |
| 21 to 47 | 1 | 2 |
| 48 to 49 | 0 | 1 |

Three nations get one more UEFA Cup place (fair play). In 2001 they are Finland, Slovakia and Belarus. In later years they are random, never Italy. Intertoto places (+0xe): the first 16 nations that are not fixed cases get 2, the next 27 get 1, the rest 0. Fixed cases: Liechtenstein, San Marino, Moldova, Andorra and Bosnia-Herzegovina 0, Scotland 1, England 1 after 2001. In 2001 only: France 4, Germany 3, Portugal, Norway, Spain and Russia 0, Italy 1. Belgium gets 2 in 2000 (0x903eaa).

3. `collect_european_clubs` (0x904040) makes a 12-byte record for each European club, sets club +0x1db to -1, and sorts by nation, division, league position and reputation (0x905ce0). In 2001 it calls `apply_euro_cfg_order` (section 3.4).
4. `assign_champions_league_places` (0x904210). The holder takes a group place. Then each nation's clubs by league position go into entry round 1 (groups), 2 (third qualifying round), 3 (second) or 4 (first). Capacities 16, 18, 19, 18 (0x904252 to 0x904261). The holder cuts the groups to 15 (0x904340). Places by rank, as (round 1, 2, 3, 4):

| Rank | Places |
|---|---|
| 0 to 2 | 2, 2, 0, 0 |
| 3 to 5 | 2, 1, 0, 0 |
| 6 to 8 | 1, 1, 0, 0 |
| 9 | 1, 0, 1, 0 |
| 10 to 14 | 0, 1, 1, 0 |
| 15 | 0, 1, 0, 0 |
| 16 to 28 | 0, 0, 1, 0 |
| 29 and below | 0, 0, 0, 1 |

When a round is full the place moves to the next round down.

5. `assign_uefa_cup_places` (0x904580). The holder takes a place. Each nation's first place goes to its cup winner (the runner-up if the winner has a place, or a semi-finalist from +0x440). The second goes to its league cup winner. The rest go by league position. Places by rank, as (first round, qualifying round): 0 to 5: 3, 0. 6 to 7: 4, 0. 8 to 14: 1, 1. 15 to 16: 1, 2. 17 to 20: 0, 3. 21 to 47: 0, 2. 48 and below: 0, 1. Fair play nations get one more qualifying place. The holder's nation loses one first-round place.
6. `assign_intertoto_places` (0x904e30) cycles through the nations until 60 clubs have a place (0x905003). Nations ranked 0 to 7 enter round 1, 8 to 11 round 2, the others round 3. England's club comes from the volunteer list or at random from the Premier Division (0x903890).
7. `post_qualifiers_news` (0x905f00).

Each club's European competition is club field +0x1db (a comp record id) and its entry round is +0x1df.

### 3.3 Points

- `European_Cup::process_fixture` (0x583b10) and `Intertoto_Cup::process_fixture` (0x633290, also the UEFA Cup's slot 1) add 2 points for a win and 1 to each club for a draw through `add_nation_points` (0x9058b0).
- Each club that reaches a Champions League group phase adds 1 more point (0x585290 and 0x585ab0).
- `get_nation_places` (0x905c60) gives `League::update_positions_and_status` (0x684cd0) the CL and UEFA Cup place counts of a nation. So the league tables mark European places from the seeding.

### 3.4 euro.cfg and the first season

1. `validate_euro_cfg` (0x905d40) reads euro.cfg at game set-up and reports nations or clubs that the database lacks.
2. `apply_euro_cfg_order` (0x9059e0) runs only when the seeding year is 2001 (0x9041ec). A `*Nation` line selects that nation's block of sorted clubs. Each club line moves that club to the front of the block, in file order. So the listed clubs take the nation's places in file order.
3. The header order does not set the nation ranking. The code scans its own nation list for each header (inference from 0x905a45 to 0x905ab1). The ranking comes from the coefficients.
4. With a moved start year this is a first-season branch. It must test "first season", not 2001, or a 2010 start ignores euro.cfg (see Q239).

### 3.5 Registration deadlines

`check_registration_deadlines` (0x9058e0) resets European squad flags on 5 August, 1 September and 2 February of the game year (`squad_manager_reset_player_flags` 0x85a9b0). The third is skipped in 2001.

### 3.6 Other confederations

- `Conmebol_Seeding` (0x4c5fe0, 0x75 bytes, `DAT_00ae1000`, file `conmebol_seeding.tmp`) holds up to 11 nations: the CONMEBOL nations and Mexico. Libertadores places: Brazil and Argentina 4, Venezuela and Mexico 1, others 3 (0x4c6168, 0x4c617a).
- `assign_libertadores_places` (0x4c6430): the holder first. Brazil's places until 2002 go to Palmeiras and Cruzeiro (club globals 0x9d0274, 0x9d026c), from 2003 to the Brazilian Cup finalists and the Brazilian Champions Cup winner (+0x71). The rest go by league position.
- The Copa Mercosur takes a fixed list of 20 clubs (0x4c6890), set by name in index.cpp:
  - Argentina: River Plate, Boca Juniors, Independiente, San Lorenzo, Vélez Sarsfield, Rosario Central
  - Brazil: Palmeiras, Cruzeiro, São Paulo, Corinthians, Flamengo, Vasco da Gama, Grêmio
  - Chile: Colo Colo, Universidad de Chile, Universidad Católica
  - Uruguay: Peñarol, Nacional
  - Paraguay: Olimpia, Cerro Porteño
- The Merconorte Cup takes 16 clubs from Mexico, Colombia, Peru, Bolivia and the American Major League (0x4c2560).
- The CONCACAF Champions Cup takes 8 clubs, with the US Open Cup and American Major League clubs (0x4c11a0).
- The Asian cups take the champions or cup winners of a fixed list of Asian nations (0x40b950, 0x40d830).
- The Oceania Club Championship takes 8 clubs: the Australian NSL champion and clubs of New Zealand, Fiji, Tahiti and others (0x798220).
- The World Club Cup takes the Champions League and Libertadores winners. When a continent does not run, it takes a random club above reputation 5000 from it, except in 2001 (0x92b8b0).
- The Club World Championship takes 12 clubs (0x92a8b0): 2 each from Africa, Asia, Europe, North America and South America, 1 from Oceania and 1 from the host nation. The second European pick passes the UEFA Cup record (0x9cf6f0). `WorldClubChamp::pick_entrant` (0x92ae60) takes the continental winner when that competition runs, else a random high-reputation club of the continent or nation.

## 4. Formats in the original

| Competition | Stages and rounds | Entrants | Evidence |
|---|---|---|---|
| Champions League | 3 qualifying rounds, 8 groups of 4 (6 rounds), 4 groups of 4 (6 rounds), quarter-final, semi-final, final | 71 entrants, 32 in phase 1 | 0x583c50, 0x584fa0, 0x585290, 0x585ab0, 0x586330 |
| UEFA Cup | optional qualifying round, rounds 1 to 4, quarter-final, semi-final, one-match final | 145 (118 seeded plus 24 from the CL and 3 Intertoto) | 0x9025c0, 0x902b90 |
| Intertoto Cup | 5 rounds, June to August, 3 winners to the UEFA Cup | 60 | 0x633300, 0x6336c0, 0x633d40 |
| European Super Cup | 1 match | 2 | 0x57a3c0, 0x57a500 |
| World Club Cup | 1 match | 2 | 0x92b770, 0x92b8b0 |
| Inter-American Cup | 1 round | 2 | 0x632340 |
| Club World Championship | 3 groups of 4 (3 rounds). The 3 group winners and the best runner-up go to a 4-club knockout stage of 3 rounds: semi-finals, third place match and final (round order and legs not read). Odd years from 2003 | 12 | 0x92a8b0, 0x929aa0, 0x929e60, 0x92a140, 0x929390 |
| CONCACAF Champions Cup | 3 knockout rounds | 8 | 0x4c0f00, 0x4c11a0 |
| Merconorte Cup | 4 groups of 4 (6 rounds). The 4 group winners go to a 4-club knockout stage of 2 rounds: semi-final and final (legs not read) | 16 | 0x4c1960, 0x4c1de0, 0x4c2140, 0x4c2330, 0x4c2560 |
| Copa Libertadores | 8 groups of 4 (6 rounds). The top 2 of each group go to a 16-club knockout of 4 rounds | 32 | 0x4c31c0, 0x4c3f40, 0x4c37a0, 0x4c3b10, 0x4c3d10, 0x4c4410 |
| Copa Mercosur | 5 groups of 4 (6 rounds). The 5 group winners and the 3 best runners-up go to a knockout of 3 rounds from the quarter-finals | 20 | 0x4c4930, 0x4c5850, 0x4c4e60, 0x4c51d0, 0x4c53d0, 0x4c5c90 |
| Asian Club Championship | 2 two-round qualifying stages (zones of 16), 2 groups (3 rounds), 3-round final stage | 32 | 0x40ae90, 0x40b620, 0x40b950 |
| Asian Cup Winners' Cup | 3-round knockout stages (zones of 16), then a 4-club finals stage | 32 | 0x40cf90, 0x40d4d0, 0x40d830, 0x40e030 |
| Asian Super Cup | 1 round | 2 | 0x410910, 0x410a50 |
| Oceania Club Championship | 2 groups of 4 (3 rounds). The top 2 of each group go to a 3-round final stage from the semi-finals. Every second year | 8 | 0x797230, 0x798220, 0x7978c0, 0x797e80, 0x798050 |

Prize funds (+0x28): UEFA Cup 3,000,000 (0x9030c3), CONCACAF Champions Cup 2,000,000, Asian club cups 4,000,000. Champions League payments: 200,000 for a group win and 100,000 to each club for a group draw (0x583baa to 0x583c28), 1,800,000 a club for phase 1 (0x585803), 1,200,000 for phase 2 (0x586065), knockout round prizes 1,600,000 and 2,000,000 (0x5843bd, 0x584459).

## 5. Year constants

All year tests in the 21 files and in the six continent creators. Size is the size of the immediate.

| Address | Size | Value | Function | Meaning |
|---|---|---|---|---|
| 0x9038af | 2 | 2001 | Uefa_Seeding::pick_english_intertoto_club | No English Intertoto pick in 2001 |
| 0x903b6f | 2 | 2001 | Uefa_Seeding::roll_coefficients | No coefficient shift in 2001 |
| 0x903e1a, 0x903e32, 0x903e64, 0x903e73, 0x903e8c | 2 each | 2001 | Uefa_Seeding::set_place_counts | Fixed 2001 Intertoto places |
| 0x903eaa | 2 | 2000 | Uefa_Seeding::set_place_counts | Belgium Intertoto places in 2000 |
| 0x903eee | 2 | 2001 | Uefa_Seeding::set_place_counts | Fixed 2001 fair play nations |
| 0x9041ec | 2 | 2001 | Uefa_Seeding::collect_european_clubs | Read euro.cfg in the first season |
| 0x9059b2 | 2 | 2001 | Uefa_Seeding::check_registration_deadlines | Skip the February deadline in 2001 |
| 0x584489 | 2 | 2000 | European_Cup::create_round_table | Other final date option in 2000 |
| 0x584495 | 4 | 2000 | European_Cup::create_round_table | The year passed with that option |
| 0x57a5f2 | 2 | 1999 | Eur_Super_Cup::pick_clubs | Cup Winners' Cup winner in 1999, UEFA Cup winner after |
| 0x632540, 0x63266c | 2 each | 2001 | Inter_Amer_Cup::pick_clubs | First-season club choice |
| 0x92b970, 0x92ba9c | 2 each | 2001 | World_Club_Cup::pick_clubs | First-season club choice |
| 0x92af4e, 0x92b24b | 2 each | 2001 | World_Club_Champ::find_club_for_slot | First-season club choice |
| 0x9291b2 | 2 | 2002 | World_Club_Champ::World_Club_Champ | First edition: odd year after 2002 |
| 0x797042 | 2 | 2001 | Oceania_Club_Champ::Oceania_Club_Champ | An even year below 2001 moves to an odd year |
| 0x4c64fe | 2 | 2002 | Conmebol_Seeding::assign_libertadores_places | Fixed Brazilian clubs until 2002 |
| 0x831607, 0x83161e | 4, 2 | 2001 | create_european_club_comps | Entry date 21 June 2001 and entry year |
| 0x831299, 0x8312b0 | 4, 2 | 2001 | create_world_club_comps | Entry date 20 June 2001 and entry year |
| 0x8318ac, 0x8318c2 | 4, 2 | 2001 | create_north_american_club_comps | Entry date 21 February 2001 and entry year |
| 0x831b53, 0x831b69 | 4, 2 | 2001 | create_south_american_club_comps | Entry date 10 January 2001 and entry year |
| 0x831e65, 0x831e7c | 4, 2 | 2001 | create_asian_comps | Entry date 28 August 2001 and entry year |
| 0x8320b2, 0x8320c9 | 4, 2 | 2001 | create_asian_club_comps (Oceania) | Entry date 21 July 2001 and entry year |

Values in the range that are not years: 2000, 2005, 2007 and 2010 pushed to `Text::add_link` are link types (for example 0x58466d, 0x584697). 0x902d03 compares nation field +0x8e with 2000 (inference: a nation strength value). 0x92b001 compares a list count with 2000. Pushes of 1999 and 2025 before 0x90d130 are assert line numbers.

## 6. Size and round-count constants

| Address | Size | Value | Meaning |
|---|---|---|---|
| 0x8313bd | 4 | 4 | European competitions in the Europe entry |
| 0x8313bb | 1 | 0x10 | Bytes of the Europe array |
| 0x8310b3, 0x8310bc | 4 | 2, 3 | World competitions without and with the Inter-American Cup |
| 0x583820 | 4 | 0xd | Champions League stages |
| 0x585255, 0x58525a | 1 | 8, 0xc | First stage of phase 2 and of the knockout |
| 0x584faa | 4 | 0x47 | Champions League entrants (71) |
| 0x585097, 0x5850cc, 0x5850ff, 0x585137 | 1 | 0x60, 0x6c, 0x72, 0x6c | Entrant caps in bytes for entry rounds 1 to 4 (16, 18, 19, 18 clubs) |
| 0x5849a3 | 2 | 0x37 | Champions League qualifying slots (55) |
| 0x583c9b, 0x5842a8 | 4 | 3 | Rounds of the qualifying stage and of the knockout |
| 0x583ee1, 0x5840a0 | 2 | 6 | Rounds of a group |
| 0x583eda, 0x584099 | 4 | 0x43d | Base stage id of the groups |
| 0x5852b4 | 1 | 0x20 | Clubs in phase 1 |
| 0x904256, 0x90425b, 0x904260, 0x904265 | 1 | 16, 18, 19, 18 | CL place capacities by entry round |
| 0x904344 | 1 | 15 | Group capacity when the holder has a place |
| 0x9043cb to 0x904431 | 1 | 3, 6, 9, 10, 15, 16, 29 | CL rank brackets |
| 0x9046fb to 0x904758 | 1 | 6, 8, 15, 17, 21, 48 | UEFA Cup rank brackets |
| 0x904e47, 0x904e58 | 1 | 8, 12 | Intertoto entry round brackets |
| 0x905003, 0x9050b3 | 2 | 0x3c | Intertoto clubs (60) |
| 0x902bab | 2 | 0x91 | UEFA Cup entrant slots (145) |
| 0x902b9d | 4 | 0xa1 | UEFA Cup list allocation (161) |
| 0x902cb2, 0x902cef, 0x902d7e, 0x902da6 | 1 | 0x76 | UEFA Cup seeded cap (118) |
| 0x633dee | 1 | 0x1b | UEFA Cup slots from the end for Intertoto winners |
| 0x6336db | 2 | 0x3c | Intertoto entrants |
| 0x9030c6 | 4 | 3,000,000 | UEFA Cup prize fund |
| 0x9291e5 | 4 | 3 | Club World Championship stages |

The place tables of section 3.2 are built from registers and bytes on the stack at 0x904219 to 0x9042ce (CL) and 0x9045b6 to 0x9045fc (UEFA Cup). A table change there is a code change, not a byte patch.

## 7. Notes on the names

- 21 addresses in european_cup.cpp, and 0x4c2070, 0x4c4b40 and 0x4c5420, are not function starts. They lie inside a real function and nothing calls them. Data words in 0x967000 to 0x967270 and 0x985600 to 0x9858a0 point at the European ones (purpose unknown). They are named `fragment_in_<function>`.
- The real functions in that range are `EuropeanCup::draw_phase1_groups` (0x585290 to 0x585aa1) and `EuropeanCup::draw_phase2_groups` (0x585ab0 to 0x58632c). The function map names the fragments `EuropeanCup::draw_phase1_groups_*` and `EuropeanCup::draw_phase2_groups_*`.
- The gap 0x587220 to 0x58aa60 between european_cup.cpp and fifa_confed.cpp holds news stories (inference: a news events file without asserts). They carry european_cup.cpp. The FA meeting screen functions 0x58acd0 to 0x58b0f0 carry fifa_confed.cpp.
- The gap 0x4c6d40 to 0x4c74f0 holds the static Date initialisers and the constructor and release of `CONTRACT_MANAGER`. They carry contract_manager.cpp.
- The CD wait loops in `Uefa_Seeding::Uefa_Seeding`, `Conmebol_Seeding::Conmebol_Seeding` and `CONTRACT_MANAGER::CONTRACT_MANAGER` are part of the constructors.
- The rule book names agree with the slice 1 sweep (`sweep-t1a`) for Europe and Asia.

## 8. Unknowns

- What the data words at 0x967000 and 0x985600 that point into european_cup.cpp are for.
- The exact meaning of nation field +0x8e.
- Whether the six coefficient doubles of a nation record are read from nation.dat or computed at load. Not traced.
- The meaning of every field of a round record (0x68 bytes) beyond dates, prize at +0x5c and the fields that Cup.cpp names.
- How the Club World Championship host is chosen. Partly answered by the function map: `WorldClubChamp::start_new_season` 0x929d40 calls `HostCountry::pick_club_world_champ_hosts` 0x5fab70. That function gives the host rows of the Club World Championship (0x9cf7bc) and the Confederations Cup (0x9cf964) a random nation with nation byte +0x85 above 12, except two fixed nations (key ids 0x9cf2e4 and 0x9cf488) and Oceania nations. `HostCountry::find_hosts` 0x5fa730 only reads the row. The meaning of nation +0x85 is not traced.
