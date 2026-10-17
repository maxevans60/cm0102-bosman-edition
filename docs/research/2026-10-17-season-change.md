# Season change and news (0.6.0 slice 16)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `src/recomp_rt/win/season_change.c` and `.h`, `season_kind_names.c`, `season_names_stub.c` (until slice 15 is on main), the season part in `season.c`. Test: `tests/seasons/season_change_run.py`.

## What the slice does

With `changing_rules` on, each competition takes the rules of its new season at its season change, and the player gets one news item for each season in which the rules change (Q232, Q258). With `changing_rules` off, the runtime only calls the original (Q232). `RECOMP_SEASON_CODE=off` turns it off too (Q342).

The first rule kind is the competition name (Q226, Q238), from the names table of slice 15 (`season_names_get`). The substitutes and the bench are not in this slice: see "Substitutes and bench".

## Where the season changes

There is no single "new season" step (`2026-10-16-sweep-season-end.md`, section 2.1). `game_season_update_pass` 0x5d5ae0 runs at the end of every day (called from `game_main_day_loop` at 0x5cda0b, and at 0x5ccaf2 in the run to the start date). For each of the 35 league table entries (0xb63d60, 0x48 bytes each):

- 0x5d5bc8: when the game date (day of the year, 0xae2c90) is the entry's update day +0x15, and the entry's nation is selected (+0x11c at 0x5d5be5) or the entry has no nation, and +0x35 is not the game year (0x5d5bf3), the pass adds the year step +0x1d to +0x35 and +0x44 (0x5d5c05, 0x5d5c0f) and sets the start and end dates.
- 0x5d5cab..0x5d5dca: then for each competition of the entry (+0xc count, +0x10 array) whose `all_stages_complete` (vtable +0xc) is true and vtable +0x2c is 0, it calls `game_update_league_attendances` 0x5d6280 and `start_new_season` (vtable +0x8 at 0x5d5d1c).
- A top league's `start_new_season` starts its lower leagues (for example `eng_prm::start_new_season` 0x574c10) and adds 1 to each competition's season year +0x40.

The runtime wraps the pass. Before it, it notes +0x40 of every competition object (the table [0xadadfc] with [0xadadf8] slots and the entry arrays, as the rules report reads them). After it, an object whose +0x40 moved had its season change on that day, and so has each object in the competition array of an entry whose +0x35 moved. The second case holds the international competitions that wait for their next edition: in July 2003 the European Championship object already holds 2004 and does not move, but its entry starts the 2003/04 season (first test run, 2026-10-17: without this case nine names of the 2003/04 rows did not come in).

**The season that starts** is the year of the game date. The update days of a game (log of the base game, England selected, `RECOMP_SEASON_CHANGE_LOG=1`):

| Entry | Update day (day of the year) | Date | +0x35 step |
|---|---|---|---|
| 5 | 9 | 10 January | 1 |
| 4 | 51 | 21 February | 1 |
| 2 | 170 | 20 June | 1 (Inter-American Cup, Intercontinental Cup) |
| 14 | 170 | 20 June | 1 (England) |
| 3 | 171 | 21 June | 1 (UEFA competitions) |
| 7 | 201 | 21 July | 1 |
| 0 | 205 | 25 July | 2 (World Cup and its qualifiers) |
| 1 | 226 | 15 August | 2 (Copa America, African and Asian Cups) |
| 6 | 239 | 28 August | 1 |

A run with calendar-year nations selected (September 2010 ODB; England, Argentina, Australia, Brazil, Japan, Norway, Finland, Denmark; first pass) shows more entries: 8 (day 212, 31 July), 9 (200), 11 (11, 12 January), 13 (170), 22 (24), and two late ones, 24 (day 330, 27 November, 3 competitions) and 15 (day 360, 27 December, 3 competitions). The step of +0x35 needs +0x35 to differ from the game year (0x5d5bf3), so a December entry that holds 2010 does not move it in December 2010, while its competitions still start their next season (the competition loop at 0x5d5c5b does not depend on the step).

So a change from January to September of year Y starts season Y: a split season on 20 or 21 June, a calendar-year edition Y in January or February (the rules table puts edition Y in the row of season Y, Q273 to Q276). A change from 1 October starts season Y + 1: the calendar-year leagues of entries 24 and 15 start their next edition (inference from the update days and the calendar seasons of those nations; no test run crosses a December change yet). The entry's own +0x35 is not the season: the international entries 0 and 1 step by 2 (2008 to 2010 on 25 July 2009), and a December entry moves it a year late.

The change applies only to a season after the start year. The run from 1 January to the start date of a new game (`game_run_to_start_date` 0x5cc850) passes update days too: for example the Copa America object moves from 2001 to 2003 in the creation of a 2001/02 game. Those changes are not season changes of the game.

## The registry

`season_change.h` holds the interface for slices 17 to 29. A rule kind is a table of a name and two functions:

```c
typedef struct {
    const char *name;
    void (*season)(int season);                               /* once per new season */
    void (*comp)(uint32_t obj, const char *file, int season); /* once per competition that changes */
} SeasonRuleKind;
void season_change_news(const char *fmt, ...);
```

A slice adds a line to `season_rule_kinds` in `season_change.c`. The kind applies the rule of the new season from its own table and calls `season_change_news` with one sentence for each change the player sees. A kind that finds no row changes nothing, so after the last row of the table the rules stay (Q248). `season_change_comps` and `season_change_file` give the competition objects and their source files.

## The news item

The game's own news code makes the item, as `HostCountry::post_host_news` 0x5faec0 does (0x5faf66, 0x5faf81, 0x5fb049):

- `NewsItem::NewsItem` 0x77bbe0 (thiscall: type, flag): 0xde bytes, +0 type, +4 flag, +5 fifty slots of 4 bytes cleared at 0x77bc29, +0xcd the game date (0x77bc3b).
- `NewsItem::set_slot` 0x785790: slot 0 holds the mark "BOSN", slot 1 the season.
- `News::send_to_all_managers` 0x786780 on the news store [0xb7431c]: `News::deliver_to_manager` 0x785d40 copies the item into each human manager's inbox.

The item is in runtime memory (`win_static_alloc`), not on the game's heap. The type is new: 0x300. `NewsItem::get_category` 0x785670 puts types 1 to 999 in category 2 (0x785684). The news screen's filters (`news_screens_build_news_screen` 0x7881e0, 0x7885ce..0x7886d7) are All, Messages (1), Competitions (2), Injuries and Bans (3), Contracts and Media (4), Transfers (5), Jobs (6) and Records (7), so the item is in Competitions. The original makes types up to 0x2f in that range (the pushes before the 226 calls of 0x77bbe0). The runtime answers the four functions that read the type, for this type only:

- `NewsItem::write_text` 0x77bc80 (manager, text, full_text, flag): `Text::clear` 0x4afa40, then `Text::append_string` 0x4ae770 with the headline (full_text 0) or the body. Return 1.
- `NewsItem::get_action_button` 0x782b70: no button (label empty, handler 0, return 0).
- `NewsItem::is_important_for` 0x784eb0: 0.
- `NewsItem::auto_respond` 0x784100: nothing.

The news screen build calls only `write_text`, `get_action_button` and `NewsItem::get_date` 0x789280 with an item (its call list), and it compares no type.

The text: headline "New rules for the 2009/10 season", body "These changes come in with the 2009/10 season:" and one sentence per line. The sentences of one season go into one item. A later day of the same season (another league table entry) adds its sentences to that item's text and sends no new item.

## The text in the save

The item in news.dat holds only its slots (200 bytes), too small for a list. The text goes in the season part `bosman_season.dat` (`2026-10-17-season-save-part.md`): version 2 adds after the 16 bytes of version 1 a count, then for each season its year, the length and the text, then a count of changed names and for each the record's name in the data and its name now (51 bytes each, see "Names"). A game with no news text and no changed name still writes version 1, so every save of slice 11 stays as it was. A reader of version 1 reads a version 2 part (its first fields keep their meaning). A save of a game with changing rules always has the part (`part_needed`), so the text is there whenever an item is.

## Names

The names kind (`season_kind_names.c`) writes the name of `season_names_get(file, season)` into the long name of the competition record (+4, 51 bytes with the 0) when it differs, and adds "<old name> is now called <new name>." to the news. The rules table is UTF-8. The game's text is one byte per character (the data files are Windows-1252), so `season_change_latin1` turns each two-byte UTF-8 character up to U+00FF into one byte (the first run showed "Copa AmÃ©rica" without it).

**A changed name must not break a load.** `set_key_ids` 0x601a40 runs at every load of the data, also of a saved game. It finds the key competitions by name: `match_club_comp_key_id` 0x60efd0 (cdecl: name, index) compares each record's long name with fixed strings (`_stricmp`; the loop over the club comps and then the nation comps at 0x60425d; `2026-10-16-sweep-europe.md`, 1.1 and 1.4). A save holds the names of now. The first save after the 2009 change ("UEFA Europa League") stopped the load with the box "v3.9.68 index..cpp 5809" (an "Unable to find the %s index" assert of 0x601a40, 0x6043b4..0x6043f6 and the same pattern after it); the same game with changing rules off loaded. So every change of a name goes through `season_change_rename`, which keeps the record's name in the data and its name now (in the season part), and the runtime wraps 0x60efd0: a name that the season code changed is matched by its name in the data. Slice 15 must change names through the same function.

## Substitutes and bench

The brief asks for them here only if that is simple. It is not:

- `start_new_season` makes the new season's fixtures (`League::create_fixtures` 0x6835c0, from 0x574c10) before the pass returns, and `Comp::set_fixture_substitutes` 0x48f2fa..0x48f32d packs +0x49 and +0x4a into a fixture (`2026-10-17-rules-report.md`). Inference, not read in the code: the packing runs when the fixture is made, so a change after the pass misses the fixtures that are already made.
- Some classes override the packing (`AusNsl::set_fixture_substitutes` 0x4124e0).
- Whether the match engine holds 5 substitutes and 9 on the bench is not proven.

So the substitutes and the bench stay with slice 17, which can register a kind and change the fixtures too.

## Unknowns

- What `season_names_get` gives when a name goes back to the database name. The names kind writes a name only when the lookup gives one, so a NULL keeps the name the record has.
- The short name (+0x38 of the record) stays as it is. The names table gives one name.
- No test run crosses a change on 27 November or 27 December, so the rule "from 1 October, season Y + 1" is an inference from the update days.
- The fixtures screen of the base game shows "Season 2009/0" for 2009/10 (seen in the load test). This slice does not touch that screen. Why it shows "/0" is unknown: a later slice can look at it.

## Tests

`tests/seasons/season_change_run.py` (module docstring): season jumps across 2002/03 to 2003/04, 2008/09 to 2009/10, 2020/21 to 2021/22 and 2023/24 to 2024/25 with changing rules on and the stand-in names, one with changing rules off, and the load run. Results are in the slice report.

Screenshot (outside the repo): `/Users/max/Projects/cm0102/shots/0.6.0-slice16/news-item.png`, the item in the Competitions filter of a loaded 2008/09 game in July 2009.
