# Season change and news (0.6.0 slice 16)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `src/recomp_rt/win/season_change.c` and `.h`, `season_starts.c` (made by `tools/seasons/make_start_wraps.py`), `season_kind_names.c`, `season_names_stub.c` (until slice 15 is on main), the season part and the season text in `season.c`, the patches of the season text in `tools/recomp/seasons.py`. Test: `tests/seasons/season_change_run.py`.

## What the slice does

With `changing_rules` on, each competition takes the rules of its new season at its season change, and the player gets one news item for each season in which the rules change (Q232, Q258). With `changing_rules` off, the runtime only calls the original (Q232). `RECOMP_SEASON_CODE=off` turns it off too (Q342).

The first rule kind is the competition name (Q226, Q238), from the names table of slice 15 (`season_names_get`). The substitutes and the bench are not in this slice: see "Substitutes and bench".

## Where the season changes

There is no single "new season" step (`2026-10-16-sweep-season-end.md`, section 2.1). `game_season_update_pass` 0x5d5ae0 runs at the end of every day (called from `game_main_day_loop` at 0x5cda0b, and at 0x5ccaf2 in the run to the start date). For each of the 35 league table entries (0xb63d60, 0x48 bytes each):

- 0x5d5bc8: when the game date (day of the year, 0xae2c90) is the entry's update day +0x15, and the entry's nation is selected (+0x11c at 0x5d5be5) or the entry has no nation, and +0x35 is not the game year (0x5d5bf3), the pass adds the year step +0x1d to +0x35 and +0x44 (0x5d5c05, 0x5d5c0f) and sets the start and end dates.
- 0x5d5cab..0x5d5dca: then for each competition of the entry (+0xc count, +0x10 array) whose `all_stages_complete` (vtable +0xc) is true and vtable +0x2c is 0, it calls `game_update_league_attendances` 0x5d6280 and `start_new_season` (vtable +0x8 at 0x5d5d1c).
- A top league's `start_new_season` starts its lower leagues (for example `eng_prm::start_new_season` 0x574c10) and adds 1 to each competition's season year +0x40.

The runtime wraps the pass and every `start_new_season` of the competition classes: slot +0x8 of each vtable of the class table (`make_comp_classes.py`), 174 distinct functions, written out as RT_WRAP lines in `season_starts.c` by `tools/seasons/make_start_wraps.py` (ctest `season_starts` checks that the committed file matches the class table; RT_WRAP stops the build when an address is not a translated function). Seven classes have the shared "return 0" 0x5a8f60 in that slot, which other vtables use for other slots, so the tool leaves it out. The hooks act only while a pass with changing rules on runs; a call of `start_new_season` outside the pass passes through. A competition whose `start_new_season` runs has a real new season (`started` 1). A competition of an entry on its update day whose `start_new_season` does not run has none (`started` 0): an unfinished competition (`all_stages_complete` false), or an international one that waits for its next edition (in July 2003 the European Championship object already holds 2004; the 2010 qualifiers on 25 July 2009).

An earlier form of this slice noted +0x40 of every object before the pass and compared after it. The reviews found that it can miss objects made in the pass and read freed ones, and cannot run before `start_new_season`; the wraps replace it.

**The season that starts** comes from the game date. The update days of a game (log of the base game, England selected, `RECOMP_SEASON_CHANGE_LOG=1`):

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

So a change from January to September of year Y starts season Y: a split season on 20 or 21 June, a calendar-year edition Y in January or February (the rules table puts edition Y in the row of season Y, Q273 to Q276). A change from 1 October starts season Y + 1: the calendar-year leagues of entries 24 and 15 start their next edition (the december run of the test checks it). The entry's own +0x35 is not the season: the international entries 0 and 1 step by 2 (2008 to 2010 on 25 July 2009), and a December entry moves it a year late.

The change applies only to a season after the start year. The run from 1 January to the start date of a new game (`game_run_to_start_date` 0x5cc850) passes update days too: for example the Copa America object moves from 2001 to 2003 in the creation of a 2001/02 game. Those changes are not season changes of the game.

## The season point

The season point of season S is the first pass from 1 June to 30 September of year S in which an entry has its update day: 20 June in a game with England (entries 2 and 14), as the European season starts. The season hooks run there, and the news item of S is posted there. A calendar-year league that changes before it (January or February of S, or 27 November or 27 December of S - 1) gets its rules on its own date, and its sentences wait for the item of S. A sentence after the point (the international entries on 25 July and 15 August) goes into the text of the item that is already posted: the text is read when the item is shown. The last season point is kept in the save part, so a load between two changes of one season does not run the season hooks twice.

## The registry

`season_change.h` holds the interface for slices 17 to 29, with one example for each hook. A rule kind is a table of functions; NULL leaves a hook out:

```c
typedef struct {
    int season; uint32_t comp; const char *file; int entry; uint32_t nation; int started;
} SeasonChange;
typedef struct {
    const char *name;
    void (*new_game)(int start_year);       /* after a new game is made */
    void (*loaded)(int start_year);         /* after a saved game is loaded */
    void (*season)(int season);             /* at the season point */
    void (*entry)(const SeasonChange *c);   /* each entry on its update day, before the pass */
    void (*before)(const SeasonChange *c);  /* before start_new_season of a competition */
    void (*after)(const SeasonChange *c);   /* after it, or with started 0 for one that waits */
} SeasonRuleKind;
```

A slice adds a line to `season_rule_kinds` in `season_change.c`. `entry` and `nation` give the scope by league table entry and by nation (entry +0, 0 for a continental or world entry). The entry of a lower league is the entry of the top league whose `start_new_season` starts it. The `entry` hook runs before the pass, so `season_change_add_comp` can put a competition that starts in season S into the entry's array (a new array with the game's operator new 0x944cf1, the old one back with operator delete 0x944c94, as the pass itself allocates at 0x5d5d74 and frees at 0x5d5d6a). `set_up_rules` (vtable +0x8c) and `League::create_fixtures` 0x6835c0 run inside `start_new_season`, so a kind that sets a field they read uses `before`, and a field they overwrite needs `after` and its own fix of what they made. A kind applies the rule of season S from its own table and calls `season_change_news` with one sentence for each change the player sees. A kind that finds no row changes nothing, so after the last row of the table the rules stay (Q248). `season_change_comps`, `season_change_entry_comps` and `season_change_file` give the competition objects and their source files.

A test kind (`RECOMP_SEASON_CHANGE_TEST=1`, off without it) logs the season hook and adds a sentence for each competition that starts from 1 October, for the december run.

## The news item

The game's own news code makes the item, as `HostCountry::post_host_news` 0x5faec0 does (0x5faf66, 0x5faf81, 0x5fb049):

- `NewsItem::NewsItem` 0x77bbe0 (thiscall: type, flag): +0 type, +4 flag, +5 fifty slots of 4 bytes cleared at 0x77bc29, +0xcd the game date (0x77bc3b: s16 day of the year, s16 year). An item has 0xdf bytes: the fixture store copies 0xdf (0x5aaed0), 0x6842a8 allocates 0xdf, and `News::send_to_all_managers` writes the scope byte +0xde (0x786802). The runtime keeps its item in 0x100 bytes of runtime memory.
- `NewsItem::set_slot` 0x785790: slot 0 holds the mark "BOSN", slot 1 the season.
- `News::send_to_all_managers` 0x786780 on the news store [0xb7431c]: `News::deliver_to_manager` 0x785d40 copies the item into each human manager's inbox.

The item is in runtime memory (`win_static_alloc`), not on the game's heap. The type is new: 0x300. `NewsItem::get_category` 0x785670 puts types 1 to 999 in category 2 (0x785684). The news screen's filters (`news_screens_build_news_screen` 0x7881e0, 0x7885ce..0x7886d7) are All, Messages (1), Competitions (2), Injuries and Bans (3), Contracts and Media (4), Transfers (5), Jobs (6) and Records (7), so the item is in Competitions. The original makes types up to 0x2f in that range (the pushes before the 226 calls of 0x77bbe0). The runtime answers the four functions that read the type, for this type only:

- `NewsItem::write_text` 0x77bc80 (manager, text, full_text, flag): `Text::clear` 0x4afa40, then `Text::append_string` 0x4ae770 with the headline (full_text 0) or the body. Return 1.
- `NewsItem::get_action_button` 0x782b70: no button (label empty, handler 0, return 0).
- `NewsItem::is_important_for` 0x784eb0: 0.
- `NewsItem::auto_respond` 0x784100: nothing.

The news screen build calls only `write_text`, `get_action_button` and `NewsItem::get_date` 0x789280 with an item (its call list), and it compares no type. A search of the compares with the first type of each range (0x3e8, 0x7d0, 0xbb8, 0xfa0, 0x1388, 0x1770) finds news types only in these four functions and `get_category`; the other hits are money and counts. So no switch of the original meets type 0x300.

The text: headline "New rules for the 2009/10 season", body "These changes come in with the 2009/10 season:" and one sentence per line. The sentences of one season go into one item, posted at the season point. The text of one season holds at most 8 KB; a sentence that does not fit is left out, so the save part never grows too large.

## The text in the save

The item in news.dat holds only its slots (200 bytes), too small for a list. The text goes in the season part `bosman_season.dat` (`2026-10-17-season-save-part.md`). Version 2 adds after the 16 bytes of version 1: the news texts (season, posted, length, text), the changed names (record index and name in the data) and the last season point (format in `season_change.h`). A game with none of them still writes version 1, so every save of slice 11 stays as it was. A later version keeps these fields first. A reader takes them from version 2 and every later version and refuses a load when it cannot read them, because the changed names are needed to load the data; it never goes on without them. The slice 11 test writes a version 3 part with these fields and 8 more bytes.

## Names

The names kind (`season_kind_names.c`) writes the name of `season_names_get(file, season)` into the long name of the competition record (+4, 51 bytes with the 0) when it differs, and adds "<old name> is now called <new name>." to the news. The rules table is UTF-8. The game's text is one byte per character (the data files are Windows-1252), so `season_change_latin1` turns each two-byte UTF-8 character up to U+00FF into one byte (the first run showed "Copa AmÃ©rica" without it).

**A changed name must not break a load.** `set_key_ids` 0x601a40 runs at every load of the data, also of a saved game. It finds the key competitions by name: `match_club_comp_key_id` 0x60efd0 (cdecl: name, index) compares each record's long name with fixed strings (`_stricmp`; the loop over the club comps and then the nation comps at 0x60425d; `2026-10-16-sweep-europe.md`, 1.1 and 1.4). A save holds the names of now. The first save after the 2009 change ("UEFA Europa League") stopped the load with the box "v3.9.68 index..cpp 5809" (an "Unable to find the %s index" assert of 0x601a40, 0x6043b4..0x6043f6 and the same pattern after it); the same game with changing rules off loaded. So every change of a name goes through `season_change_rename`, which keeps the record's name in the data by the record's index (from [0xae23d0], 0x6b bytes a record; the club comps, then the nation comps), and the runtime wraps 0x60efd0: the name pointer of the loop at 0x60425d is the record +4, so its index finds the name in the data. A second change of the same record keeps the first name in the data. The two other callers (0x60400e, 0x6041b7) pass names that they read from a file and never match a record. Slice 15 must change names through the same function.

## The season text (Q358)

`key_nation_format_club_season` 0x669630 and `key_nation_format_comp_season` 0x6697c0 write a season with "%d/%d" (0x9eb024) and (year + 1) % 10 (0x6696d3..0x6696e6, 0x669869..0x669876), so 2009/10 shows "2009/0". 27 calls reach 0x669630 (club history tabs, the fixtures tab 0x45f9ae, printouts, records, staff history, the TV rights news). With the season code on, the patches of `seasons.py` give % 100 and "%d/%02d" (in runtime memory): "2009/10", "2001/02". With `RECOMP_SEASON_CODE=off` they give the original's 10 and 0x9eb024. Q358 accepts that the base game text changes ("2001/2" becomes "2001/02").

## Substitutes and bench

The brief asks for them here only if that is simple. It is not:

- `start_new_season` makes the new season's fixtures (`League::create_fixtures` 0x6835c0, from 0x574c10) before the pass returns, and `Comp::set_fixture_substitutes` 0x48f2fa..0x48f32d packs +0x49 and +0x4a into a fixture (`2026-10-17-rules-report.md`). Inference, not read in the code: the packing runs when the fixture is made, so a change after the pass misses the fixtures that are already made.
- Some classes override the packing (`AusNsl::set_fixture_substitutes` 0x4124e0).
- Whether the match engine holds 5 substitutes and 9 on the bench is not proven.

So the substitutes and the bench stay with slice 17, which can register a kind and change the fixtures too.

## Unknowns

- What `season_names_get` gives when a name goes back to the database name. The names kind writes a name only when the lookup gives one, so a NULL keeps the name the record has.
- The short name (+0x38 of the record) stays as it is. The names table gives one name.

## Tests

`tests/seasons/season_change_run.py` (module docstring): season jumps across 2002/03 to 2003/04, 2008/09 to 2009/10, 2020/21 to 2021/22 and 2023/24 to 2024/25 with changing rules on and the stand-in names, one with changing rules off, the load run (a save after the change loads, returns from holiday, shows the fixtures screen and the news item, plays on with Continue Game and saves again) and the december run. Results are in the slice report.

Screenshots (outside the repo): `/Users/max/Projects/cm0102/shots/0.6.0-slice16/news-item.png` (the item in the Competitions filter of a loaded 2008/09 game in July 2009) and `fixtures-2009.png` (the fixtures screen with "2009/10").
