# Club moves and names at a new game (0.6.0 slice 15)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md` (Q225, Q226, Q237, Q238, Q244, Q279, Q290, Q335). Code: `tools/seasons/make_names.py`, `tools/seasons/make_clubs.py`, `src/recomp_rt/win/season_names.c` and `.h`, `src/recomp_rt/win/season_clubs.c` and `.h`, the club lines of `season_report.c`, the club and name checks of `tests/seasons/rules_report_check.py` and `tests/seasons/season_core_run.py`. Lists: `docs/research/club-moves-0.6.0/lists.tsv` (each club list and what became of it) and `missing.tsv` (the real clubs that no database record matches).

## Which games

Only a season game gets names and club moves: a game whose start year is not 2001, or whose changing rules are on. The base game (2001/02, changing rules off) and the test switch `RECOMP_SEASON_CODE=off` keep the data as it is.

## Names (Q226, Q238)

`make_names.py` turns the `name` column of the rules table into `Seasons/names.tsv` (4,713 rows): season, source file of the class, object number, long name, short name. The rows of a file in a season are those that `rules_report_check.py` pick_row takes, in the same order, so the k-th object of a file gets the k-th name.

The shown name is the research name with four changes, so that a screen shows a plain name:

- a part in brackets at the end goes, unless it names a confederation or a play-off ("Primera División (Liga BBVA)" gives "Primera División");
- a name in quotes at the end goes;
- the year of an edition goes ("2012 Africa Cup of Nations" gives "Africa Cup of Nations"; "UEFA Euro 2012" gives "UEFA European Championship"), because the object of a competition can wait for another edition than the row's;
- a name over 51 characters is cut after a whole word.

The short name (26 characters) is the long name when it fits, else a shorter form ("FIFA World Cup qualification (UEFA)" gives "WC qualifying (UEFA)", "Football League Championship" gives "Championship"). `make_names.py --list` prints them all.

Where the names go: the competition record holds the long name at +4 (52 bytes) and the short name at +56 (27 bytes) (`2026-10-13-database-inventory.md`). The objects are those of the rules report: `g_comp_object_table` [0xadadfc], then the 35 league table entries at 0xb63d60; the class of an object comes from its vtable (`season_comp_classes`).

When: at the end of `build_comp_object_table` 0x8386f0. A new game calls it at 0x829640 in `create_game` 0x828110, after the competitions are made and before the run to the start date (0x5cc850 at 0x829fd9); a load calls it at 0x82c5f6 in `load_game_data` 0x82a9c0. A new game takes the names of the start season. A load takes the start season with changing rules off, else the season of the game date (a season starts on 1 July); slice 16 gives later names through `season_names_apply`.

The save: the original finds each competition by its long name at every load (`match_club_comp_key_id` 0x60efd0, called from `set_key_ids` 0x601a40 at 0x52d8fa in `load_database` 0x528540; `load_game_data` reaches it through 0x524d10 at 0x82abf2), and a save holds the records (`club_comp.dat`, `nation_comp.dat`). A renamed record would lose its key id at the load. So the save wrap (`save_game` 0x82e430 in `season.c`) puts the database names back before the save writes and the season names in again after it. A save of a season game holds the database names; the load gives the names again. `season_core_run.py` checks both.

Slice 16 calls `season_names_get(comp_file, season)` (NULL when the table holds no name) and `season_names_apply(season)`.

## Club moves (Q225, Q237, Q244, Q279, Q290)

### The table

`make_clubs.py` turns the 16 clubs files `rules-0.6.0/<part>-clubs.tsv` into `Seasons/clubs.tsv` (18,313 rows): season, database division (long name), club record id, club record name, real name. One list is the rows of one part, season and division label. The database division of each list comes from the table `TARGETS` of the tool, one line per part and label. A first version took the database division that most clubs of a list held; that is wrong for many lists (most of the real Série B clubs of 2008 sit in the database's first division, most J2 clubs of 2010 in the Japanese Administrative Division), so the tool now uses the fixed table. 930 lists are in the table and 360 are left out:

| Part | Lists left out | Reason |
|---|---|---|
| arg-usa | 23 | MLS: the class knows its 12 clubs by their key ids (below) |
| den-swe | 40 | Swedish Division 1 Norra and Södra, Ettan: the real groups do not match the six database divisions |
| ger | 18 | 3. Liga: no class in the original (slice 20) |
| gre-pol-cro | 7 | the new second Greek group: no class (slice 20) |
| hol-bel | 8 | Belgian First Amateur Division and National Division 1: the mapping of Q292 leaves Third Division B empty, which the code cannot hold (slice 18) |
| ita | 36 | the single Serie C and Lega Pro groups: no class (slice 20) |
| nor-fin | 22 | Ykkönen as one group and Ykkösliiga: the database has North and South |
| por | 75 | the Segunda Divisão series, Campeonato Nacional de Seniores and Campeonato de Portugal series (groups that do not match the three database zones), Liga 3 (slice 20) |
| rus-tur | 57 | Turkish Category B groups (slice 18) |
| sco | 53 | cup entrants, guests and the pyramid play-off (slice 21) |
| spa | 20 | the Segunda B subgroups of 2020/21 and Primera Federación (slices 18 and 20) |
| fra | 1 | CFA 2 of 2009/10: see Open |

Four clubs stand in two lists of one season (Brazil 2008, Portugal 2009 to 2011, see `lists.tsv`). The first list keeps them. The research should check these four rows.

### The moves

At a new game in a season game, in memory, at the start of `database_trim_staff_tables` 0x524d70 (called once, at 0x828389 in `create_game`): after the load (0x8282a7, and the date shift of slice 11b at its end) and before the trims of the staff and club tables (0x524d70, `database_trim_club_table` 0x5267c0 at 0x828395) and before the competitions are made. A first version ran after the club trim: there the clubs of the nations that the player did not select are already cut down to a few per nation, so the divisions of those nations looked broken. The database files stay unchanged. A loaded game needs nothing: its save holds the moved clubs.

The division of a club is the pointer at club +0x57 to its club_comp record (`resolve_database_indexes` 0x5315a0 at 0x531625). A league takes its clubs by it, in the order of the club table, and stops at its count: `EngConf::create_table` 0x56ecb0 sets 22 at 0x56ecc2 and takes the clubs whose division is the key id [0x9cf69c] (0x56ed54..0x56ed7f). With fewer clubs the table is not made (0x56ed95..0x56edab). So each division keeps the count that its class takes. In the code that is the club count of the database, except for these classes, which take another count after the first season of the original than the database keeps:

| Class | 2001/02 | Later | Evidence |
|---|---|---|---|
| FraFirst (French First Division) | 18 | 20 | `FraFirst::create_table` 0x5bc2b0: +0x40 == 2001 at 0x5bc2bd, 18 at 0x5bc2c5, 20 at 0x5bc2cd. Every season database holds 18, so a later start stopped at fra_first.cpp 108 |
| PolSecond (Polish Second Division) | 20 | 18 | `PolSecond::create_table` 0x7cbdf0: 0x7cbdfd, 20 at 0x7cbe03, 18 at 0x7cbe14 |

With a start year that is not 2001 these two divisions take the later count (`LATER_SIZE` in `season_clubs.c`). The other classes with a year branch on the count take one count in every start from 2001: `GerRegional::create_table` 0x5dd7f0 (19 only when +0x40 is 2000, else 18), `ScoPrm::create_table` 0x7f2820 (10 only in 1999, else 12). `NorFirst::create_table` 0x7909a0 takes 14 when the data gives 14, else 16.

The steps for each season:

1. Each row finds its club: the record id with the same name and the nation of the division, else the club of that name in that nation (an extra database of a season can hold other ids), else it is left out.
2. Each division of a list keeps its listed clubs up to its count: the clubs that the database has there first, then the others in the order of the list.
3. A club that the database has in a listed division and that no list keeps there leaves the division (no division, Q290).
4. Every division of the nations with a list (or a resized division) is filled to its count, from the highest rank (club_comp +0x69) down: first the clubs that the database had there and that no list keeps, then the nation's clubs with no division, then the clubs of the spare divisions "A Lower Division" and "A Premier Division" (key ids 0x9cf764 and 0x9cf6b8, set at 0x610153 and 0x60fb91 in 0x60efd0), then the clubs of lower divisions that no list keeps. A lower division that gives a club fills in its turn. Inside each group the order is that of the club table.
5. A moved club gets its database division as the division of the last season (+0x5b, 0x5315a0 at 0x531657). `FraLgeCup::collect_entrants` 0x5bd160 needs exactly 54 entrants: 14 French National clubs whose last division is set and is not CFA (0x5bd20f..0x5bd249; 16 when +0x40 is 2001), then Ligue 2 and Ligue 1. Without this step the 2002/03 start stopped at fra_lge_cup.cpp 73 (0x5bca3a).

`RECOMP_CLUB_LOG=1` writes a line per list: real clubs, clubs found in the database, clubs kept, clubs filled, count.

### MLS

`usa_mls_get_club_division` 0x909ec0 knows the 12 MLS clubs of the original by their key ids (0x9d037c..0x9d03a8, compares at 0x909ee5..0x909f6b) and stops at usa_mls.cpp 1819 (0x909f96, box at 0x909fbe) for any other club in the American Major League. A 2010/11 start with the real MLS list stopped there. The MLS lists are left out (slice 18 or 20 must give the class its conferences by club).

### Real clubs that the database lacks (Q335)

1,641 rows of the clubs files have no database record (`missing.tsv`): 1,446 in lists of the table (France 1,192, nearly all in the CFA and CFA 2 lists; Brazil 65; Denmark and Sweden 52; Portugal 26; Greece, Poland and Croatia 23; Wales, Northern Ireland and Ireland 15; Australia, Japan and Korea 13; Italy 9; Russia and Turkey 3; Argentina and the US 2) and 195 in lists left out (Scotland 97). This slice adds no club record. The reason and a decision for Max are in "Open".

## Tests

Seam: the rules report (`tests/seasons/rules_report_check.py`). New: at each season start the report writes the clubs of each division (club lines: division record, club id, club name), and `--start-only` ends the run after the first start report (`RECOMP_RULES_REPORT_STOP=start`, about 3 minutes with 15 nations). In a season game the check compares the clubs of each division with a list of the table with the listed clubs that the database holds (kind `club_list`, by name: the club trim gives clubs new ids). A name is right when it is the table name or its shown form. Expected files: `rules_report_expected/<year>-<nations>-start.tsv`.

### Runs of 2026-10-17

`--start-only` runs of every season with a database (2002/03 with the 3.9.68 data, then the 20 season databases), each with two nation groups: (a) England and the first 15 rows of the Select League(s) screen, Argentina to Norway with France; (b) Select All without those 15 (the other 11 nations and England). 44 runs, 3 at a time, about 3 minutes each. All 44 pass against their expected files. Every run with France selected now reaches the game: before this slice each one stopped at fra_first.cpp 108 (a run of 2009/10 with `RECOMP_CLUB_MOVES=off` still stops there).

- Names: no `name` difference is left in any run. Each season renames 29 to 117 records (log line "names of").
- Club lists: 766 lists were checked (a list counts when its league is in the report), 169 of them exact. The other 597 are `club_list` lines of the expected files: the real count differs from the count of the class (slice 18), or real clubs lack a record.
- 2009/10 group (a) stopped at a box "Disk Space Error" (FILE_LLIST::add_node 0x58e340, file_llist.cpp 161) three times. Without France, or without either the French CFA or the CFA 2 list of that season, it starts. The CFA 2 list of 2009/10 is left out (`SKIP_SEASON` in `make_clubs.py`); then the run passes.

Commands (from the repo, `G=/Users/max/Projects/cm0102/clean-3968`, `D=/Users/max/Projects/cm0102/test-fixtures/databases`):

- `python3 tests/seasons/rules_report_check.py --run build/cm0102_recomp $G --year 2010 --database "$D/September 2010 ODB" --nations argentina,australia,belgium,brazil,croatia,denmark,finland,france,germany,greece,holland,italy,japan,northern_ireland,norway --start-only`
- the same with `--nations all,-argentina,-australia,-belgium,-brazil,-croatia,-denmark,-finland,-france,-germany,-greece,-holland,-italy,-japan,-northern_ireland,-norway`
- for 2002 no `--database` (the 3.9.68 data of the game folder).

Other checks of 2026-10-17, after the merge of main (slices 11b and 11c):

| Command | Result |
|---|---|
| `python3 tests/seasons/season_core_run.py build/cm0102_recomp $G --jobs 3` | PASS: the base game has no names and no club moves; the 2010/11 game renames 29 records and moves clubs, its save holds the database names, it is the same save twice, and the save loads, gives the names again and plays 3 days |
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --jobs 3` | PASS, `season-code-off` equal |
| the same with `--save prematch.sav`, without `--save` (new game), and `--save yeovil.sav --days 7` | PASS each, `season-code-off` equal |
| `cd build && ctest` | 17 of 17 pass, `season_names_table` and `season_clubs_table` among them |

## Open

- **Decision for Max (the lead gives it a number): new club records for real clubs that a database lacks (Q335).** The club table holds the clubs, then the national teams in its last 2 x nations rows (the national team test 0x53b900, the nation's team at `club[nation->id + clubs - 2 * nations]` 0x53b3d0), and many tables point into it. A new record at the end moves the national teams; one before them moves every later index. Both need a study of every user of the club table, and a new club has no players, staff or stadium. Recommended answer: a separate slice that adds the records as the reserve teams of Q289 will, with generated squads, built after slice 18; until then the division takes a club of the pool (step 4). Alternative: use a club of the same nation that is in no list of that season as the record and rename it (quicker, but a real club then disappears).
- **Decision for Max (the lead gives it a number): the shown names.** The research names carry sponsor names in brackets and edition years. The rule above drops them. Recommended answer: keep the rule. Max may prefer the sponsor names of each season ("LaLiga Santander").
- Slice 18: every list whose real count differs from the count of its class (`club_list` lines of the expected files), the MLS, the Belgian mapping of Q292, the Swedish, Finnish, Portuguese, Spanish and Turkish groups, Ligue 1 with 18 clubs from 2023/24 (the class takes 20, so two clubs of the pool fill it).
- Slice 20: 3. Liga, the single Serie C, the second Greek group, Liga 3, Primera Federación.
- Slice 21: the Scottish cup entrants, guests and pyramid play-off, and the academy sides of `eng-academy.tsv` (Q336): cup entrants, not divisions.
- Wembley for a 2003 to 2006 start (slice 11 table 3): not in this slice.
- The Disk Space Error of 2009/10 with both French lower lists: cause unknown.
- Slice 11c (`history-0.6.0/no-winner.tsv`): 10 of its 197 rows name a club of `missing.tsv` (Ersboda SK in six databases, Dalkurd FF in four). They become fillable only when the new club records come in.
- Slice 16 (not on main yet) brings `season_change_rename`. When it is on main, the renames of `season_names_apply` go through that call. Until then a save holds the database names (see Names).
