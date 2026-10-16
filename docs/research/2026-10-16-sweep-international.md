# Sweep: international competitions (slice 2, sweep-international)

Date 2026-10-16. Slice 2 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Research only. No code changes.

Scope: the 19 files `world_cup`, `wc_europe_league`, `wc_south_american_league`, `wc_concacaf_cup`, `wc_african_cup`, `wc_asia_league`, `wc_oceania_league`, `euro_champ`, `euro_champ_qual`, `fifa_confed`, `goldcup`, `african_nations`, `asia_nations`, `oceania_nations`, `olympics`, `host_country`, `rb_international`, `national_teams` and `fifa_rankings`. They hold 406 game functions. The other rows of their function map docs are library and graphics code that the first pass placed there (for example 41 graphics functions in `goldcup.cpp.md`). They are out of scope.

Names: `/Users/max/Projects/cm0102/research-0.6.0/sweeps/sweep-international.jsonl`, one line for each of the 406 game functions (model opus). Class names use the file stem, except `goldcup.cpp`, which is `copa_america` (section 2).

Method: the assembly of the clean 3.9.68 original, read from the instruction comments of the recompiled game's generated C and checked with objdump, with call targets, key ids and strings named. All addresses are virtual addresses. "Inference" marks a statement that the code does not prove.

## 1. What build slices 27 to 29 need first

1. The original finds each international competition by its name in the data, once at load (0x60efd0), then builds a fixed class for each key id in `create_world_cup_comps` (0x830620) and `create_other_int_comps` (0x830ac0). A new competition such as the Nations League needs a new key id, a new class and a new line in setup. A data record alone never runs (section 3).
2. Every year cycle is code: a base year and a step per class (section 4). None reads the data. The cycles fit 2001 to 2006 and drift after that: the Confederations Cup runs every two years for ever, the African Cup of Nations every even year, the Copa America every odd year, the Asian Cup in 2004, 2008, 2012.
3. Hosts come from one table of 57 rows built in code (0x5f9970). The World Cup has hosts to 2026, the European Championship to 2028, the African Cup of Nations to 2014, the Copa America to 2009, the Asian Cup to 2016, the Olympics to 2028. Hosts after 2006 (World Cup) and 2004 (European Championship) are drawn at random from three candidates each (section 6).
4. Golden goal is bit 0x80 of the rule flags word of a cup round template (+0xd). Only the World Cup, the European Championship and the Confederations Cup set it (section 7). Removing golden goal from 2004/05 is a change of seven words.
5. A nation's confederation is its continent pointer in the data (nation +0x71). The code adds fixed lists: Australia and New Zealand in the Oceania Nations Cup, Melanesian and Polynesian lists, New Caledonia and Papua New Guinea left out of Oceania World Cup qualifying, and an exact count of 10 Oceania entrants. Australia's move to Asia needs data plus code (section 8).
6. The European Championship qualifiers of the 2002 campaign leave out Belgium and Holland and put in host Portugal (fixed list). The World Cup European section takes at most 50 nations, so with the 2001 data and no European exemption Yugoslavia, the last European nation in table order, is left out (section 10).
7. A 48-team World Cup and the Nations League need new classes. They can reuse League, League_Stage, Cup_Stage, Comp_Stats, HostCountry and the qualifier hand-over (add_qualified_team), and the third-place ranking of the Copa America (section 11).

## 2. Files and classes

| File | Class (sweep name) | Base | Ctor | Vtable | Key comp global | Created at |
|---|---|---|---|---|---|---|
| world_cup.cpp | world_cup | League | 0x92bf50 | 0x9712f0 | 0x9cf79c FIFA World Cup | 0x830802 |
| wc_african_cup.cpp | wc_african_cup | Cup | 0x90f210 | 0x970ad0 | 0x9cf77c | 0x830867 |
| wc_asia_league.cpp | wc_asia_league | League | 0x911bf0 | 0x970b70 | 0x9cf774 | 0x8308ca |
| wc_concacaf_cup.cpp | wc_concacaf_cup | Cup | 0x9149f0 | 0x970c24 | 0x9cf770 | 0x83092d |
| wc_europe_league.cpp | wc_europe_league | League | 0x918200 | 0x970cc4 | 0x9cf780 | 0x830990 |
| wc_oceania_league.cpp | wc_oceania_league | League | 0x91df90 | 0x970d78 | 0x9cf76c | 0x8309f3 |
| wc_south_american_league.cpp | wc_south_american_league | League | 0x9202b0 | 0x970e2c | 0x9cf778 | 0x830a56 |
| olympics.cpp | olympics | League | 0x79f530 | 0x96e1d0 | 0x9cf7a0 | 0x830ca4 |
| euro_champ.cpp | euro_champ | League | 0x57a890 | 0x96a020 | 0x9cf7a4 | 0x830d09 |
| euro_champ_qual.cpp | euro_champ_qual | League | 0x57ce10 | 0x96a0d4 | 0x9cf784 | 0x830d6c |
| african_nations.cpp | african_nations | League | 0x401000 | 0x967270 | 0x9cf7ac | 0x830dcf |
| goldcup.cpp | copa_america | League | 0x5e0660 | 0x96b400 | 0x9cf7a8 Copa America | 0x830e32 |
| oceania_nations.cpp | oceania_nations | League | 0x7988b0 | 0x96e0cc | 0x9cf8a0 | 0x830e95 |
| asia_nations.cpp | asia_nations | League | 0x40e860 | 0x967614 | 0x9cf790 Asian Cup | 0x830ef8 |
| fifa_confed.cpp | fifa_confed | League | 0x58b310 | 0x96a270 | 0x9cf964 | 0x830f64 |
| rb_international.cpp | rb_international | RulingBody | 0x7df180 | 0x96ec34 | n/a | 0x55d4e0 |

- `goldcup.cpp` runs the Copa America. Its ctor gets the Copa America row (0x830e17), its entrants are the South American nations and two guests (0x5e1cd0), and its host rows are those of Copa America (0x5fa189). No CONCACAF Gold Cup exists in the original.
- Ctor convention of every class: thiscall (this, short year, ClubComp* record) ret 8. The year is the game year (0xae2c92). The record is `0xae23d0 + row * 0x6b`.
- Object fields used by all: +0x04 record, +0x08 Comp_Stats, +0x0c stage array, +0x2c stage count, +0x30 current stage, +0x36/+0x38 slot count and capacity (finals), +0x3e teams per table, +0x40 year (word), +0x49 bench size (7 in every class), +0xb1 table, +0xba schedule, +0xbe..+0xc8 rule bytes, +0xee pool array, +0xf2 pool size, +0xf6 exempt count (qualifiers).

## 3. How the original picks the international competitions

By name, then by fixed code.

1. At load, `set_competition_id_by_name` (0x60efd0) compares the long name (+4) of each comp row with fixed strings, with `_stricmp` (0x951c50). The first equal name of a chain stores the row in a global. The loop is at 0x604241; it runs over the club comps and the nation comps, which follow the club comps in the same table (rows 433 to 451 in the clean data), and leaves out the 127 reserve rows.
2. The international globals and their names:

| Global | Names |
|---|---|
| 0x9cf76c | World Cup Oceania Qualifying Section |
| 0x9cf770 | World Cup CONCACAF Qualifying Section |
| 0x9cf774 | World Cup Asian Qualifying Section |
| 0x9cf778 | World Cup South American Qualifying Section |
| 0x9cf77c | World Cup African Qualifying Section |
| 0x9cf780 | World Cup European Qualifying Section |
| 0x9cf784 | European Championship Qualifying |
| 0x9cf788 | Asian Cup Qualifying (no class) |
| 0x9cf78c | Football Tournament of the Asian Games, Asian Games Football Tournament (no class) |
| 0x9cf790 | Asian Cup |
| 0x9cf794 | 'B' International (no class) |
| 0x9cf798 | Under 21 International (no class) |
| 0x9cf79c | FIFA World Cup |
| 0x9cf7a0 | Football Tournament of the Olympic Games, Olympic Games |
| 0x9cf7a4 | European Football Championship |
| 0x9cf7a8 | Copa America |
| 0x9cf7ac | African Cup of Nations |
| 0x9cf8a0 | oceania nations cup |
| 0x9cf964 | confederations cup |

3. `create_world_cup_comps` (0x830620) always builds the World Cup and the six sections, in the order World Cup, Africa, Asia, CONCACAF, Europe, Oceania, South America. `create_other_int_comps` (0x830ac0) always builds the Olympics, European Championship, its qualifiers, African Cup of Nations, Copa America, Oceania Nations Cup and Asian Cup, and builds the Confederations Cup only when its key id is not -1 (0x830f0e; else it lowers the count at 0x830f89).
4. Each object goes into `comp_objects[row]` (0xadadfc). Other code reaches a competition through its key id, for example the World Cup through 0x9cf79c.
5. Hazard: setup does not test the key id of the always-built competitions. A database without one of those names gives the ctor `0xae23d0 + (-1) * 0x6b` (inference: a bad record pointer and a crash or garbage).
6. A data record such as "UEFA Nations League" gets no key id (no name string in 0x60efd0) and no object. It can never run without new code. A renamed record (for example "European Championship" without "Football") loses its key id.
7. Key ids with no class: Asian Cup Qualifying (the Asian Cup has no qualifiers, section 5), Asian Games, 'B' International and Under 21 International. The last three appear in the lists of `national_teams` (0x76b63a, 0x76b6fe, 0x76cc2d).

## 4. Year cycles

The year field +0x40 is the edition year (finals) or the first year of the campaign (qualifiers). Every class adds its step in `start_new_season`.

| Competition | Rule in the ctor | Base constant | First edition in a 2001 game | Step | Real cycle (for slice 10) |
|---|---|---|---|---|---|
| World Cup | round up to (year-1994)%4==0 | 0x92bf99 | 2002 | +4 (0x92d409) | 2002, 2006, ..., 2026 with 48 teams |
| WC Europe | (year-2000)%4==1 moves back one, round up to (year-2000)%4==0, at least 2004 | 0x918251, min 0x9182db | campaign 2004 (World Cup 2006) | +4 | same |
| WC South America | as Europe | 0x920306, min 0x92036d | campaign 2004 | +4 | from 2007 for 2010 |
| WC Asia | as Europe | 0x911c4b, min 0x911cb7 | campaign 2004 | +4 | longer from 2007 |
| WC CONCACAF | odd year back one, round up, no minimum | 0x914a91 | campaign 2000 (World Cup 2002), running | +4 | |
| WC Africa | as CONCACAF | 0x90f275 | campaign 2000, running | +4 | |
| WC Oceania | as CONCACAF | 0x91dfeb | campaign 2000, starts at once in 2001 (0x91f486) | +4 | |
| European Championship | round up to (year-1992)%4==0 | 0x57a8d6 | 2004 | +4 (0x57b7ca) | 2004, 2008, 2012, 2016, 2021, 2024, 2028 |
| Euro qualifying | year-1, round up to (year-1990)%4==0 | 0x57ce59 | campaign 2002 (Euro 2004) | +4 (0x57ebb4) | |
| Confederations Cup | next odd year, never 2001 | 0x58b376, 0x58b385 | 2003 | +2 (0x58c050) | 2003, 2005, 2009, 2013, 2017, then none |
| Copa America | round up to (year-1996)%2==1 | 0x5e069a | 2001 | +2 (0x5e1590) | 2001, 2004, 2007, 2011, 2015, 2016, 2019, 2021, 2024 |
| African Cup of Nations | round up to (year-1996)%2==0 | 0x401048 | 2002 | +2 (0x401d80) | even years to 2012, then 2013, 2015, 2017, 2019, 2022, 2024, 2025 |
| Asian Cup | round up to (year-1996)%4==0 | 0x40e89a | 2004 | +4 (0x40f780) | 2004, 2007, 2011, 2015, 2019, 2024 |
| Oceania Nations Cup | at least 2002, round up to (year-1996)%2==0 | 0x7988e9, 0x798922 | 2002 | +2 (0x799940) | 2002, 2004, 2008, 2012, 2016, then none until 2024 |
| Olympics | round up to (year-1996)%4==0 | 0x79f56a | 2004 | +4 (0x7a04b0) | 2004, 2008, 2012, 2016, 2021, 2024 |

The "real cycle" column is a hint for slice 10, not a sourced fact. A new start year (2003 to 2026) needs the base constants or a per-season table: a start in 2005 gives a Confederations Cup in 2005 and 2007, an Asian Cup in 2008, a Copa America in 2005.

## 5. Entrants, qualifying and stages

### World Cup (0x92bf50)

- 32 slots (0x92bffa), 8 groups of 4, then a 16-team Cup_Stage (0x92d770): winners against runners-up of the paired group.
- `build_team_slots` (0x92da40) fills the slots: slot 0 is the holder, the latest placed nation of the World Cup history (0x92da72). Then the hosts from HostCountry (0x92daa9). With no host it puts Iceland and Hungary (0x92db89, 0x92dbaa), with one host China (0x92db62). Slots 3 to 31 get a fixed default list of 29 key nations (0x92dbd5 to 0x92dfb3) that the qualifiers overwrite, because qualified nations are added at the slot count (0x92e740).
- For 2002 (0x92dfc4) it adds the nations that had qualified by July 2001: Costa Rica, two of United States, Honduras and Mexico at random (0x92e079, 0x92e18e), Cameroon, Nigeria, Senegal, Tunisia and South Africa.
- `draw_groups` (0x92e940): for 2002 (0x92eb46) the real draw of 32 key nations in group order (France, Senegal, Uruguay, Denmark, ...). Other years: seeded pots of 8 (0x92f230) and a random draw.
- Qualified nations reach the slots through `world_cup::add_qualified_team` (0x92e740), called from each section's `update_staff_records_for_status`.

### Qualifying sections

| Section | Base | Pool | Format | Places and play-offs |
|---|---|---|---|---|
| Europe (0x918200) | League, 9 to 11 stages by exempt count | continent Europe, at most 50 (0x919e41, 0x91a04a), without European hosts of year+2 and a European holder (0x919e30); pool size 50 minus exempt | groups of 5 or 6 (0x918dd0), drawn in pots by strength (0x918ec0) | group winners qualify; best runners-up in a 6 or 8 team play-off (0x919a8a); one runner-up plays the Asian section (0x919c10) |
| South America (0x9202b0) | League | continent South America, at most 10 (0x920f1c), without hosts and holder | one double round robin | top places qualify; the play-off place meets the Oceania winner (0x920ad2); places depend on the exempt counts +0xee and +0xef |
| CONCACAF (0x9149f0) | Cup, 7 stages | continent North America, at most 35 (0x9163f4), ranked into byes and preliminary rounds; for the 2000 campaign a fixed list of 35 key nations (0x916479) | three two-legged preliminary rounds, three semi-final groups of four, a final group of six | the final group's leaders qualify (inference: three) |
| Africa (0x90f210) | Cup, 4, 5 or 7 stages | continent Africa, at most 52 (0x910896), without hosts and holder | two-legged first round of 50, 48 or 46 (0x910133), then 4 or 6 groups, then a last two-legged round when nations are exempt | group winners qualify (0x90fc92) |
| Asia (0x911bf0) | League, 13 stages | continent Asia, at most 44 found, 40 kept (0x9140b9, 0x9141dd) | 10 first-round groups, two final groups of five (0x913740) | final group winners qualify; the runners-up play off (stage 11), the winner plays Europe (stage 12, 0x913d50) |
| Oceania (0x91df90) | League, 3 stages | continent Oceania, exactly 10 minus exempt (0x91fa95), without New Caledonia (0x91fc89) and Papua New Guinea (0x91fc93); fails when the count differs (0x91fce0) | two groups of five, a two-legged final | the winner plays the South American play-off nation (0x91f800) |

In a 2001 game the European, South American and Asian sections start with the 2004 campaign. Their 2002 loaders (`load_2002_campaign` 0x91b760 and 0x9216d0, with the real groups and fixtures of 2000 and 2001) only run when the game year is at least the campaign year (0x91846f), which never happens in 2001 (inference from the ctor flow).

### European Championship (0x57a890) and qualifying (0x57ce10)

- 16 slots (0x57bd93): the hosts from HostCountry, then a default list of 14 key nations (0x57bf2f) that the qualifiers overwrite (0x57c120). Pot 1 holds the hosts and the holder (0x57c710). 4 groups of 4, then an 8-team Cup_Stage.
- Qualifying: pool of 51 minus the European hosts of year+2 (0x57f76f), at most 2 hosts (0x57f6fa), no holder exemption. 10 groups of 5 or 6, group winners and the best runner-up qualify, the other runners-up play two-legged play-offs (0x57ef50).
- 2002 campaign: `draw_groups` (0x57dab0) uses a fixed list of 49 key nations when the year is 2002 (0x57dbb5); `load_2002_campaign` (0x580bd0) uses the same 49 with the real fixtures and results of 2002 and 2003 when the game starts later. The play-off pairs are fixed key nations (0x57f33f).

### Finals without qualifiers

| Competition | Teams | Entrants |
|---|---|---|
| Confederations Cup (0x58b310) | 8 (0x58c6bc) | host, World Cup holder, holder, and the latest winners of the European Championship, Copa America, African Cup of Nations, Oceania Nations Cup and Asian Cup (0x58c6b0). A missing or repeated winner is replaced from a fixed list of 11 (Brazil, Italy, Spain, Australia, France, Germany, Argentina, United States, Canada, Mexico, Jamaica) or at random (0x58ccb0). |
| Copa America (0x5e0660) | 12 (0x5e1cde) | 2001: fixed 12 (0x5e1d65; Honduras in, Argentina out). Other years: hosts, every South American nation and two guests: Japan and Mexico in 1999, Canada and Mexico otherwise (0x5e1fe5). The count must be 12 (0x5e207c). |
| African Cup of Nations (0x401000) | 16 (0x4022d3) | hosts, then the strongest African nations (0x4022c0). No qualifiers. |
| Asian Cup (0x40e860) | 12 (0x40fe02) | hosts, then the strongest Asian nations (0x40fdf0). No qualifiers. |
| Oceania Nations Cup (0x7988b0) | 6 (0x799e42) | Australia and New Zealand by key id, then the best Melanesian (Papua New Guinea, Solomon Islands, New Caledonia, Fiji, Vanuatu; 0x53d900) and Polynesian nations (Tonga, American Samoa, Tahiti, Cook Islands, Samoa; 0x53d940). |
| Olympics (0x79f530) | 16 under-21 teams (0x7a09f6) | 2000: fixed list (0x7a09f0). Other years: hosts, then the strongest by continent to running totals of 5 (Europe), 8 (Africa), 10 (North America), 12 (South America), 15 (Asia), 16 (Oceania) (0x7a0eb6 to 0x7a100a). |

The Copa America and the Asian Cup take the two best third-placed nations into the quarter-finals through a ranking League_Stage (0x5e18a0, 0x40fa90).

## 6. Hosts (host_country.cpp)

- `HostCountry::init_default_table` (0x5f9970) builds 57 rows (0x5f98bd) of 0x22 bytes: +0 comp row, +4 year (0 = every year), +6/+0xa host pair 1, +0xe/+0x12 pair 2, +0x16/+0x1a pair 3, +0x1e choice (0 to 2 = chosen pair, 0xfe = not yet chosen, 0xfd = fixed), +0x1f date, +0x21 team count. It then moves each date by a random -35 to +34 days (0x5fa664). The table is saved in the CM3_TEMP host file (0x5fb320) and loaded from it (0x5fb160).
- `HostCountry::find_hosts` (0x5fa730): thiscall (this, comp_row, short year, int* host1, int* host2, int allow_undecided) ret 0x14, returns the host count. An undecided row returns its first pair only with allow_undecided. `HostCountry::find_next_hosts` (0x5fa7d0) ignores the year and returns the first decided row of the competition.
- `HostCountry::daily_update` (0x5fa850), called each day by `game_main_day_loop`, picks a candidate pair at random for each undecided row and sends the host news (0x5faec0).
- The rows (nations by key id):

| Competition | Year | Candidates (choice) |
|---|---|---|
| World Cup | 1998 | France (chosen) |
| | 2002 | South Korea and Japan (chosen) |
| | 2006 | Germany (chosen) |
| | 2010 | Brazil, Argentina, South Africa (random) |
| | 2014 | Sweden and Norway, Spain, Switzerland (random) |
| | 2018 | Mexico, Uruguay, Canada (random) |
| | 2022 | Italy, Germany, England (random) |
| | 2026 | Brazil, Argentina, United States (random) |
| European Championship | 2000 | Holland and Belgium (chosen) |
| | 2004 | Hungary and Austria, Portugal, Spain (pair 2, Portugal, chosen) |
| | 2008 | Scotland and Wales, Sweden, Switzerland (random) |
| | 2012 | France, Norway, Denmark (random) |
| | 2016 | Republic of Ireland, Switzerland, Yugoslavia (random) |
| | 2020 | Switzerland, Belgium, Holland (random) |
| | 2024 | Italy, Germany, England (random) |
| | 2028 | Greece, Spain, France (random) |
| African Cup of Nations (step 2, fixed) | 2000 to 2014 | Ghana and Nigeria, Egypt, Sudan, Tunisia, Cameroon, Nigeria, Morocco, Algeria |
| Olympics (step 4, fixed) | 2000 to 2028 | Australia, Greece, Germany, Italy, Japan, Mexico, Canada, Spain |
| Copa America (step 2, fixed) | 1999 to 2009 | Paraguay, Colombia, Peru, Venezuela, Argentina, Brazil |
| Asian Cup (step 4, fixed) | 2000 to 2016 | Lebanon, Singapore, Japan, China, South Korea |
| Confederations Cup | every year | South Korea, redrawn by `HostCountry::pick_club_world_champ_hosts` (0x5fab70) |
| World Cup Oceania section | every year | Australia and New Zealand, swapped by 0x5fab70 |

- 0x5fab70 runs only from `WorldClubChamp::start_new_season` (0x929d40), so every second year and only while the Club World Championship object exists. It gives the Club World Championship row (0x9cf7bc) and the Confederations Cup row a random nation with nation byte +0x85 above 12 (not Oceania, not key ids 0x9cf2e4 or 0x9cf488), and swaps the World Cup Oceania section host between Australia and New Zealand. The Oceania club championship host alternates in `HostCountry::alternate_oceania_club_champ_host` (0x5face0), from `OceaniaClubChamp::start_new_season`.
- After the last row a competition has no host: find_hosts returns 0. The World Cup then puts Iceland and Hungary in the host slots (0x92db89). The others go on without a host.
- The ACN row of 2002 holds Egypt (the real host was Mali) and the Copa America row of 2001 Colombia, which the fixed 2001 list matches. Slice 27 replaces all rows from the rules table.

## 7. Golden goal and the round rule flags

- Each cup round template has a rule flags word at +0xd (round records are 0x68 bytes). The match takes them through `Cup::apply_round_rules_to_match` (0x521d60) into match +0x38.
- Bits, proven by the rules text of the fixture screen (0x45e0b5 to 0x45e2f1): 0x1 penalty shoot-out, 0x2 extra time, 0x80 golden goal (0x45e112, text at 0x98f440), 0x20 US-style shoot-out, 0x800 the higher seed wins. The away-goals text shows when a digit test of the competition id passes (0x45e0b1) and 0x4 and 0x4000 are both clear (0x45e0b9); the meaning of 0x4 and 0x4000 is unknown. 0x100 and 0x200: unknown.
- The match engine tests bit 0x80 at 0x6d4250 (match_eng.cpp, called at 0x706ea0, 0x70ae31, 0x70c4dc).

| Competition | Round | Address | Word (2-byte immediate) |
|---|---|---|---|
| World Cup | last 16, quarter-final, semi-final | 0x92c96b, 0x92ca46, 0x92cb15 | 0x383 |
| | third place | 0x92cbb9 | 0x83 |
| | final | 0x92cc63 | 0x3 (no golden goal) |
| European Championship | quarter-final, semi-final | 0x57b002, 0x57b0b2 | 0x383 |
| | final | 0x57b150 | 0x283 |
| Confederations Cup | semi-final | 0x58b892 | 0x383 |
| | third place, final | 0x58b90b, 0x58b9a5 | 0x83 |
| Copa America | quarter-final, semi-final | 0x5e0cfd, 0x5e0da6 | 0x103 |
| | third place, final | 0x5e0e3b | 0x3 |
| African Cup of Nations | quarter-final, semi-final | 0x40174c, 0x4017fc | 0x303 |
| | third place, final | 0x401897, 0x40193e | 0x203 |
| Asian Cup | quarter-final; later rounds | 0x40eedb; 0x40ef85, 0x40f019 | 0x303; 0x3 |
| Oceania Nations Cup | semi-final; later rounds | 0x798dfc, 0x79918c; 0x798e73, 0x799291 | 0x303; 0x3 |
| Olympics | quarter-final, semi-final; later rounds | 0x79fc2d, 0x79fcd9; 0x79fd74 | 0x303; 0x3 |
| CONCACAF section | preliminary rounds | 0x914ea9 | 0x200 |
| African section | first round | 0x90f6ba | 0x400 |
| Asian section | play-off | 0x91239c | 0x3 |

The two-legged play-offs of the European section, Euro qualifying, Oceania section and African section write 3 at template +0xf (0x918905, 0x57d4d5, 0x91e6e9, 0x90fba9). The meaning of +0xf: unknown.

## 8. A nation's confederation, and Australia to Asia

- The confederation of a nation is the continent pointer at nation +0x71, read from nation.dat. Every pool in this sweep compares it with a key continent (Europe 0x9cfa10, Africa 0x9cfa08, Asia 0x9cfa0c, North America 0x9cfa14, Oceania 0x9cfa18, South America 0x9cfa1c).
- Moving Australia's continent to Asia in the data puts it in the Asian section (0x913f60), the Asian Cup (0x40fdf0) and the Asian Olympic quota. It then breaks or stays wrong in code:
  1. The Oceania section needs exactly 10 Oceania entrants (0x91fa95, 0x91fce0). With Australia gone it finds 9 and fails. The count must come from the data or a table.
  2. The Oceania Nations Cup puts Australia in by key id (0x799f9c), whatever its continent.
  3. HostCountry rows 43 and 44 hold Australia for the Oceania club championship and the Oceania section; 0x5fab70 and 0x5face0 swap Australia and New Zealand.
  4. `national_teams::fill_squad` (0x76c120) uses wider thresholds for Australia in the Oceania Nations Cup, Confederations Cup and Oceania section (0x76c187 to 0x76c236).
  5. Outside this sweep: `oceania_club_champ` (0x798220), `rb_australia` (0x7db5b0), `league_region_matches_filter` (0x839340), `friendly_cup_table_init` (0x5ca4f0) and `world_club_champ` (0x92a8b0) name Australia or the Oceania continent.

## 9. Nation names in the code

- 0x9b12a4 is the string "Yugoslavia" (11 bytes with the zero). Two code sites use it: `nation_name_to_id_store` (0x60ee83), which sets KEY nation Yugoslavia (0x9cf51c) by name, and a credits list at 0x4e920f (the nation of a data researcher; the name string next to it, at 0x9b1290, is pushed at 0x4e925d). A database that renames the nation to Serbia and Montenegro or Serbia leaves 0x9cf51c at -1, and the load shows "Unable to find the YUGOSLAVIA index" (0x606182, 0x60d780). The hand port leaves that box out (key_index.h).
- KEY nation Yugoslavia is read in this sweep by the World Cup default list (0x92de11), the 2002 European section (0x91b760) and the Euro 2002 lists (0x57dab0, 0x580bd0), and outside it by Database (0x524f50), nationality adjectives (0x6181d0) and the shortlist manager (0x844f8c, 0x847059).
- Each nation in code is a key id from `nation_name_to_id_store` (0x60d810), 209 globals from 0x9cf200 to 0x9cf544. Every fixed list above (2002 draws, default slots, hosts, guests) reads those globals.

## 10. Nations left out

- Euro qualifying, 2002 campaign: the fixed 49 (0x57dbc1 to 0x57e698, and 0x580bd0) lack Belgium and Holland and hold Portugal, the 2004 host. Why: unknown. Inference: the lists were made for a draw that treated the Euro 2000 hosts as exempt.
- Euro qualifying, later campaigns: the pool is every European nation except the hosts, up to 51 minus the host count. With the 2001 data (51 European nations) nobody else is left out. A database with more European nations (for example after Montenegro, Serbia, Kosovo and Gibraltar) cuts the last ones in table order (inference from 0x57f837).
- World Cup European section: the pool takes the first 50 European nations in table order that are not exempt (0x91a04a). In the 2001 data Yugoslavia is row 209, the last European nation, so it is left out whenever no European nation is exempt. With one exemption all 50 others are in, and the pool size of 49 drops the weakest after the sort (inference from 0x918fdf).
- Oceania section: New Caledonia and Papua New Guinea, always (0x91fc89, 0x91fc93).
- Wales is in every list (Euro 2002 at 0x57dbf5, WC Europe 2002 at 0x91b855, Euro defaults at 0x57bd80).

## 11. Nations League and the 48-team World Cup

- 48-team World Cup: `world_cup` cannot hold it. Its sizes are immediates (32 slots 0x92bffa, 8 groups 0x92ea9d, 16-team knockout 0x92d7a5, 7 group stages 0x92d52c), its schedule has a fixed kick-off table per group (jump table 0x92ccd0), its knockout pairs groups A to H in fixed order (0x92d770), and `set_predicted_positions` makes 32 fixed calls (0x92f680). A new class can reuse League, League_Stage, Cup_Stage, Comp_Stats, `HostCountry::find_hosts` (three hosts need a third pair field), the qualifier hand-over through an add-qualified call, and the third-place ranking of `copa_america::create_knockout_stage` (0x5e18a0) for the eight best thirds. The sections push into the World Cup by key comp FIFA World Cup, so a new class on the same key keeps them working if it offers the same add call (0x92e740 is called directly, not through the vtable).
- Nations League: no class fits. It needs leagues A to D, groups of 3 or 4, promotion and relegation between editions (no international competition keeps a tier per nation), finals of four, and play-off paths into Euro qualifying. Reusable: the group draw by pots and the group stages of `euro_champ_qual` (0x57dab0, 0x57ed10), its play-off Cup_Stage (0x57ef50), and `add_qualified_team` of the finals. It also needs a key id and a setup line (section 3), and a decision whether it counts as a tournament in `national_teams::is_tournament` (0x76d940), `get_squad_limits` (0x76d430) and the FIFA ranking weights (0x58cf70).
- Euro qualifying with Nations League paths needs a new `create_playoff` (0x57ef50 builds plain two-legged ties from runners-up).

## 12. Year and size constants

Bytes = size of the immediate in the instruction.

| Address | Value | Bytes | Meaning |
|---|---|---|---|
| 0x92bf99 | 0x7ca (1994) | 4 | World Cup base year |
| 0x92d409 | 4 | 1 | World Cup step |
| 0x92bffa | 0x20 | 2 | World Cup slots |
| 0x92c063 | 8 | 4 | World Cup stage count |
| 0x92d0fe | 4 | 2 | World Cup group size |
| 0x92ea9d | 8 | 2 | World Cup groups |
| 0x92d52c | 7 | 1 | World Cup last group stage |
| 0x92d7a5 | 0x40 | 1 | World Cup knockout buffer (16 teams) |
| 0x92dfc4 | 0x7d2 (2002) | 2 | World Cup 2002 qualified nations |
| 0x92eb46 | 0x7d2 | 2 | World Cup 2002 draw |
| 0x918251 | 0x7d0 (2000) | 4 | WC Europe base |
| 0x9182db | 0x7d4 (2004) | 4 | WC Europe earliest campaign |
| 0x919e41 | 0x32 | 4 | WC Europe pool size |
| 0x91a04a | 0x32 | 1 | WC Europe pool cap |
| 0x919ed7 | 2 | 1 | WC Europe hosts of year+2 |
| 0x918348, 0x918355, 0x918362, 0x91836f | 0xb, 0xa, 0xa, 9 | 4 | WC Europe stage counts by exempt count 0 to 3 |
| 0x919a8a, 0x919a94 | 6, 8 | 4 | WC Europe play-off teams |
| 0x91dbba, 0x91dbc4 | 0x7d0, 0x7d1 | 2 | WC Europe 2002 results years |
| 0x920306 | 0x7d0 | 4 | WC South America base |
| 0x92036d | 0x7d4 | 4 | WC South America earliest campaign |
| 0x920f1c | 0xa | 4 | WC South America size |
| 0x9223a0, 0x9223ab | 0x7d0, 0x7d1 | 2 | WC South America 2002 results years |
| 0x914a91 | 0x7d0 | 4 | WC CONCACAF base |
| 0x914a2a | 7 | 4 | WC CONCACAF stages |
| 0x9163f4 | 0x23 | 4 | WC CONCACAF entrants |
| 0x916479, 0x914ea1 | 0x7d0 | 2 | WC CONCACAF 2000 campaign list and rounds |
| 0x90f275 | 0x7d0 | 4 | WC Africa base |
| 0x9107e8 | 0x32 | 4 | WC Africa pool size |
| 0x910896 | 0x34 | 1 | WC Africa pool cap |
| 0x910133, 0x91013f, 0x910147 | 0x32, 0x30, 0x2e | 2 | WC Africa first round size |
| 0x90f345, 0x90f34e, 0x90f357 | 4, 7, 5 | 4 | WC Africa stage counts |
| 0x911c4b | 0x7d0 | 4 | WC Asia base |
| 0x911cb7 | 0x7d4 | 4 | WC Asia earliest campaign |
| 0x911cf6 | 0xd | 4 | WC Asia stages |
| 0x9140b9 | 0x2c | 4 | WC Asia pool cap |
| 0x9141dd | 0x28 | 1 | WC Asia kept |
| 0x91dfeb | 0x7d0 | 4 | WC Oceania base |
| 0x91e076 | 3 | 4 | WC Oceania stages |
| 0x91fa95 | 0xa | 4 | WC Oceania exact entrants |
| 0x91ec0e | 5 | 2 | WC Oceania group size |
| 0x91f486 | 0x7d1 | 2 | WC Oceania start at once in 2001 |
| 0x57a8d6 | 0x7c8 (1992) | 4 | Euro base |
| 0x57b7ca | 4 | 1 | Euro step |
| 0x57bd93 | 0x10 | 2 | Euro slots |
| 0x57a951 | 4 | 4 | Euro stages |
| 0x57b53f | 4 | 2 | Euro group size |
| 0x57ce59 | 0x7c6 (1990) | 4 | Euro qualifying base |
| 0x57d00b | 0xa | 4 | Euro qualifying stages |
| 0x57f76f | 0x33 | 4 | Euro qualifying pool |
| 0x57f6fa | 2 | 1 | Euro qualifying most hosts |
| 0x580e3c | 0x33 | 4 | Euro qualifying 2002 pool |
| 0x57dbb5, 0x57fa5a, 0x582f4a | 0x7ce (2002) | 2 | Euro qualifying 2002 lists and results |
| 0x582f54 | 0x7cf (2003) | 2 | Euro qualifying 2003 results |
| 0x58b376 | 0x80000001 | 4 | Confed odd years |
| 0x58b385 | 0x7d1 | 2 | Confed skips 2001 |
| 0x58c050 | 2 | 1 | Confed step |
| 0x58c6bc | 8 | 4 | Confed teams |
| 0x58cd13 | 0x7d2 | 2 | Confed 2002 substitutes |
| 0x5e069a | 0x7cc (1996) | 4 | Copa America base (odd years) |
| 0x5e1590 | 2 | 1 | Copa America step |
| 0x5e1cde | 0xc | 4 | Copa America teams |
| 0x5e1d65 | 0x7d1 | 2 | Copa America 2001 list |
| 0x5e1fe5 | 0x7cf (1999) | 2 | Copa America 1999 guests |
| 0x401048 | 0x7cc | 4 | ACN base (even years) |
| 0x401d80 | 2 | 1 | ACN step |
| 0x4022d3 | 0x10 | 2 | ACN teams |
| 0x4023b3 | 0x32 | 1 | ACN nations scanned |
| 0x40e89a | 0x7cc | 4 | Asian Cup base |
| 0x40f780 | 4 | 1 | Asian Cup step |
| 0x40fe02 | 0xc | 2 | Asian Cup teams |
| 0x40ff24 | 0x64 | 1 | Asian Cup nations scanned |
| 0x7988e9, 0x798918 | 0x7d2 | 2 | OFC earliest edition |
| 0x798922 | 0x7cc | 4 | OFC base (even years) |
| 0x799940 | 2 | 1 | OFC step |
| 0x799e42 | 6 | 2 | OFC teams |
| 0x79f56a | 0x7cc | 4 | Olympics base |
| 0x7a04b0 | 4 | 1 | Olympics step |
| 0x7a09f0 | 0x7d0 | 2 | Olympics 2000 list |
| 0x7a09f6 | 0x10 | 4 | Olympics teams |
| 0x7a0eb6 to 0x7a100a | 5, 8, 0xa, 0xc, 0xf, 0x10 | 2 | Olympics continent totals |
| 0x5f98bd | 0x39 | 2 | host rows |
| 0x5f99bb, 0x5f99c0, 0x5f99e1 | 0x7ce, 8, 4 | 4, 4, 1 | host rows World Cup: first year, rows, step |
| 0x5f9c09 | 0x7d0 | 4 | host rows Euro first year (8 rows, step 4) |
| 0x5f9eb7, 0x5f9ee2 | 0x7d0, 2 | 4, 1 | host rows ACN first year, step |
| 0x5f9ff2 | 0x7d0 | 4 | host rows Olympics first year |
| 0x5fa177, 0x5fa1a9 | 0x7cf, 2 | 4, 1 | host rows Copa America first year, step (6 rows) |
| 0x5fa525, 0x5fa52f | 0x7d0, 5 | 4, 4 | host rows Asian Cup first year, rows |
| 0x5faf3c | 0x7d6 (2006) | 2 | host news special case |
| 0x76d4b0, 0x76d4cf, 0x76d4df | 0x17, 0x18, 0x1a | 1 | squad sizes |
| 0x830a8f | 0x7cc | 2 | World Cup group record +0x1b (meaning unknown) |
| 0x830fb0 | 0x7d1 | 2 | other international group record +0x1b (meaning unknown) |
| 0x830a63, 0x830f8d | 0x7d1 | 4 | setup dates 25 July 2001 and 15 August 2001 (meaning unknown) |

## 13. What a later start year breaks

1. Every first-edition rule in section 4 gives the next edition after the start year by the 2001 cycle, not the real one.
2. The 2002 branches (0x92dfc4, 0x92eb46, 0x58cd13, 0x57dbb5, 0x5e1d65 and the 2000 and 2001 result years) never run, which is right for 2002/03 on.
3. The HostCountry table ends between 2009 and 2028, and its candidates after 2006 are random.
4. The holder exemption of the World Cup sections stays (0x919f42, 0x920e33), though from 2006 the holder qualified like the others.
5. The WC European cap of 50 and the Euro qualifying pool of 51 cut nations once a database holds more European nations.
6. The Oceania exact count and the fixed Oceania lists break with Australia in Asia.
7. Golden goal stays in the three competitions of section 7.
8. The Confederations Cup goes on every odd year.

## 14. Unknowns

- Meaning of rule flag bits 0x100 and 0x200, of round template +0xf, and of the round codes +7 (0x14, 0x78, 0x82, 0x96, 0x8c, 0xa0, 0x172).
- The exact number of World Cup places of each section (group winners plus play-offs; the counts follow from stage and exempt numbers that were not traced in full).
- Why the Euro 2002 list leaves out Belgium and Holland.
- What the CONCACAF, African and Oceania 2000 campaigns hold at the start of a 2001 game (real state or a fresh campaign).
- Where friendlies between nations are scheduled: `friendly.cpp` (not in this sweep) reads the continents (0x5ca4f0). National squads are called up 14 days before each international fixture (0x76bb9e).
- The fields +0x15, +0x1b and +0x32 to +0x47 of the group records that setup fills (0x8306c0, 0x830a8b).
