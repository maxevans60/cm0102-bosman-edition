# Brazil's state championships by state (0.6.0 slice 20e)

Date 2026-10-20. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: slice 20, Q283, Q332, Q363.
Research: shared notes `slice-20-research-bra.md` (slice 20 session). "Inference" marks a statement that the code
does not prove.

## What the original does

- `create_brazilian_comps` 0x832980 makes 17 objects: Série A, B, C, 12 state championships, the Copa do Brasil and
  the Champions Cup (object count 0x11 at 0x832a56). Three state objects hold a region: North (BraRegNorth, key id
  [0x9cf994]), Northeast (BraRegNortheast, [0x9cf9a4]) and Central (BraRegCentral, [0x9cf998]).
- A state class picks its clubs in `create_table` (North 0x435d80, Northeast 0x436d90, Central 0x430ef0): every club
  whose state record (club +0x60) is the class's key id, up to the class's count (+0x3e = 10, 16, 15 at 0x435d96,
  0x436da6, 0x430f06). It returns true only with exactly the count. The constructor calls it directly (BraRegNorth
  0x435340 at 0x435400), then `League::create_fixtures` 0x6835c0 (0x43543a).
- The round table of a state class (vtable +0x3c, `create_schedule`; North 0x435640): stage -1 is the league,
  18 rounds of 0x41 bytes for North (malloc 0x492 at 0x435685), each with a date (0x68a160) and three slots
  (0x68a1c0). Later stages are the semi-finals and the final (North), or a knockout of 8 (Central 0x4307b0).
- Inside the state classes only `create_table` reads the key id (0x435e2d, 0x436e3d, 0x430f9d). Other readers of
  the 12 key ids: `BraChampCup::select_entrants` 0x426d80, `BraCup::select_entrants` 0x427f00 (clubs by club +0x60),
  `bra_nat_first_exchange_state_league_clubs` 0x42b130 (club +0x60 between a championship and its lower record),
  `bra_nat_first_fill_brazilian_cup_entrants` 0x42b760 (the tables of the 12 objects, each read only to its +0x3e:
  0x42b8fe, 0x42b926), `bra_nat_first_fill_champions_cup_entrants` 0x42bba0 (the 12 champions) and
  `BrazilAwards::daily_update` 0x43ebf3. Each of them reads the original 12 objects by key id.

## The state of a club (Q600)

The data has no state field. The stadium's city (club +0x69 stadium id, stadium +0x38 city id) ends with the state
code, "Fortaleza (CE)", in the Update Team databases to March 2020 (79 to 99 percent of Brazilian clubs), and almost
never from November 2020 (2 to 3 percent). Measured on the test databases, 2026-10-20.

- **Q600.** The state of a club: the suffix "(XX)" of its city in the database that runs; else its city name in the
  city table `docs/research/bra-city-states.tsv` (the city names with a suffix in the Update Team
  databases from 2005 to 2020: public geography, one state a name); else its club name in the same table's club part;
  else no state. With the tables, 77 to 80 percent of the Brazilian clubs of the databases from November 2020 get a
  state (November 2020 380 of 475, October 2021 301 of 374, October 2024 313 of 391, May 2026 262 of 370).

## Design

- The 15 objects take their clubs by state. The three composite objects become the championship of one state:
  North the Paraense (PA), Northeast the Cearense (CE), Central the Mato-Grossense (MT). The 12 new objects: from the
  North region Maranhense (MA), Amazonense (AM), Piauiense (PI), Acriano (AC); Northeast Potiguar (RN), Paraibano
  (PB), Alagoano (AL), Sergipano (SE); Central Brasiliense (DF), Capixaba (ES), Sul-Mato-Grossense (MS),
  Tocantinense (TO).
- A wrap of each composite class's `create_table` picks the clubs of the object's state when the object's record is
  one of the 15 (the three composite records and the 12 new ones), else it calls the original. A wrap of
  `create_schedule` gives stage -1 a round table for the count.
- The 12 new objects: slice 26's module (`season_comps.h`), a new record each (`like` the region's championship
  record, so its continent, nation and level), made with the North class constructor 0x435340 (size 0xee): the
  semi-finals and final format. No vtable copy: the wraps tell the objects by record.

## What the runs showed

- The constructor asks for the round table of stage -1 before it calls `create_table` (a test log of the schedule
  wrap: the three region objects asked with no clubs picked yet). So the schedule wrap plans the count itself, and
  `create_table` takes the count the round table was made for.
- The state objects hold the game year in +0x40: 2001 in a 2001/02 game, whose state championships play in the
  first months of 2002 (round dates in year +0x40 + 1). The count is that of the row of season +0x40.
- 11 January 2002, the first state match day, stopped at the box "Area..cpp 507" in the day fixtures screen
  (`build_day_fixtures_screen` 0x717db0): `Area::layout` 0x403350 stops when an area wants more than 30 rows on a page
  (+0xbac, 0x4033ec), and with more rows than a page it makes a scrollbar (0x4034bb..0x4034c7). The base game (changing
  rules off) passes the same day. The wrap of 0x403350 gives such an area 30 rows a page in a season game, so the
  screen scrolls (Q609). The new objects also start their rounds 0 to 3 days earlier by state, at most 4 a day (Q610).

## Decisions (recommended answers taken, for Max to confirm)

- **Q600** the state source (above).
- **Q601 Count.** The table's club count of the season (the nearest earlier known row, Q248), capped at the clubs of
  the state that the database holds. With no known count: the clubs of the state, at most 16.
- **Q602 Which clubs.** The clubs of the state with the region's championship record (club +0x60) first, then those
  with the region's lower division record, then those with no state record; within each, the highest reputation
  first.
- **Q603 Too few clubs.** A format needs 4 clubs (semi-finals) or 8 (the Central class's knockout of 8). A new object
  is not made when its state has fewer than 4 clubs in the database. A composite object whose state has fewer than its
  minimum keeps the original's region and count.
- **Q604 Format.** Each state plays its class's format with the count: the league stage of its class's meetings
  (double round robin for the North class, single for Northeast and Central, as the original), then the class's
  semi-finals and final (Central: its knockout of 8). The league match days spread evenly between the template's first
  and last league day.
- **Q605 Moves.** The 12 new championships have no promotion or relegation. This slice writes no club +0x60, so the
  original's exchange 0x42b130 works on the original records as before.
- **Q606 Cups.** The Copa do Brasil and the Champions Cup keep the original's 12 objects and records. The places
  that come from the three composite objects now come from one state each. The clubs of the 12 new states keep their
  region record, so `BraCup::select_entrants` 0x427f00 still finds them by record.
- **Q607 Records.** Each new record copies the region championship record's ids and level (`like`).
- **Q608 No championship.** Clubs of Amapá, Roraima and Rondônia, and clubs with no state: no state championship
  (they keep their national division).
- **Q609 Day screen.** A screen area of more than 30 rows a page shows 30 and scrolls, in a season game only.
- **Q610 Match days.** The 12 new championships start 0 to 3 days before the North template's first day, by state,
  so that no more than four of them play on one day.
- Changing rules off: the start season's counts for the whole game (Q232). The base game and
  `RECOMP_SEASON_CODE=off`: nothing changes (Q152).

## Tests (planned)

- ctest `season_bra_states`: the state of a city name, a suffix, a club name.
- `tests/seasons/bra_states_run.py`: starts 2001 (rules on), 2014 (October 2014), 2021 (October 2021) with Brazil:
  every one of the 15 objects that is made holds only clubs of its state and its count, plays to a champion, and
  the season changes; a load inside the state season; the base 2001 game logs nothing of this slice.
- Same-save set (Q342).
