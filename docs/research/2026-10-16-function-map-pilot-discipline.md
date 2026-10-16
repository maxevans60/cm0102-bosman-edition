# Function map pilot: discipline.cpp

Date: 2026-10-16. Original: CM 01/02 3.9.68 (`clean-3968/cm0102.exe`, image base 0x400000). Research only. No code changes.

Method: capstone disassembly through `tools/recomp/analysis.py`, string references, assert line numbers (the constant pushed before the call to 0x90d130 with the `discipline.cpp` path at 0x9b8f18), and a byte scan of the image for direct calls and pointers to each entry. The `this` value at every outside call site was checked. No pseudocode tool was used. Scripts are in the session scratchpad under `pilot/`.

All member functions are MSVC `__thiscall` with `this` in ecx and the callee popping its arguments. The `this` value at all outside call sites is the global `g_discipline` at 0xae28f0.

## Functions

| Address | Proposed name | Convention, arguments | Purpose | Key evidence | Confidence |
|---|---|---|---|---|---|
| 0x5554a0 | `Discipline::Discipline` | thiscall (file), ret 4 | Constructor. Sets up the appeal list, opens `ban_history.tmp` in CM3_TEMP, allocates the per-player table, loads `discipline.dat` or creates the rule sets. | Called from Setup.cpp 0x828110 straight after `operator new(0x2a)`. Asserts at lines 124 and 5635. Calls 0x555860 and 0x55d420. | high |
| 0x555790 | `Discipline::~Discipline` | thiscall, ret 0 | Destructor. Frees the ban array, the nation index and the player table, deletes the 43 rule sets and the history file, destroys the appeal list. | Called from 0x82ff40 on `[0xae28f0]` before `operator delete`. Loop count 0x2b. | high |
| 0x555860 | `Discipline::load` | thiscall (file), ret 4 | Reads `discipline.dat`: player table (0x1d bytes per player), rule sets, ban array, nation index and appeal list. | String `discipline.dat`, reads via 0x932c30, asserts at lines 218 to 323. | high |
| 0x555dd0 | `Discipline::save` | thiscall (file), ret 4 | Writes the same blocks to `discipline.dat` and calls each rule set's save (0x7e7b80). | Called from the Setup.cpp save routine 0x82e430. Writes via 0x932e10. Lines 363 to 439. | high |
| 0x556210 | `Discipline::notify_competition` | thiscall (comp_id*), ret 4 | Calls virtual slot 0 of the rule set that the competition uses. | Called from 0x5d5ae0 ("Updating game data") after a competition virtual call. Reads comp `+0x50`. | low |
| 0x556240 | `Discipline::daily_update` | thiscall (date*), ret 4 | Calls virtual slot 1 with the date on all 43 rule sets. | Called from 0x5cc850 and 0x5cccd0 with `&0xae2c90` (game date). | medium |
| 0x556270 | `Discipline::clear_bans_invalid_for_club` | thiscall (staff*, arg2, club*), ret 0xc | For each player slot, drops the ban when the rule set's nation or continent does not match the new club. | Called from the transfer completion routine 0x8c9720. Compares rule set `+0x17` and `+0x1b` with club `+0x53`. Assert line 569. | medium |
| 0x5563f0 | `Discipline::hold_bans_on_transfer` | thiscall (staff*), ret 4 | Moves bans that do not apply at the destination club into the appeal/held list and clears the player slots. | Callers 0x8bf400 and transfer_manager.cpp 0x8c2180. Uses 0x4cf6d0 on 0xae1050. Assert line 608. | medium |
| 0x556570 | `Discipline::restore_held_bans` | thiscall (staff*), ret 4 | Puts held bans back into the player slots after a move, then drops slots that do not apply to the new club. | Called from contract_manager.cpp 0x4d07e0. Reads held list at `+0x1c`. | medium |
| 0x5567d0 | `Discipline::drop_held_bans` | thiscall (staff_id*), ret 4 | Removes every held-list entry for one player and decrements the player's held count. | Called from 0x8c9720 next to 0x556270. | medium |
| 0x556830 | `Discipline::remove_player` | thiscall (staff_id), ret 4 | Clears the player's history, releases all four ban slots, removes held entries and resets the 29-byte player record to 0xff. | Called from 0x7abac0. Calls 0x55e1d0 and `rep stosd` with 0xff. | medium |
| 0x556940 | `Discipline::add_ban` | thiscall (staff_id, BanRecord*), ret 8 | Extends an existing ban for the player, or copies the record into a free slot of the ban array, registers it with each rule set and adds it to the nation index. | Callers in many rule-set methods (0x7db0ca and others). Grows the array by 100 up to 0x7d00. Asserts at lines 1025, 1040, 1086. | high |
| 0x556c70 | `BanRecord::operator=` | thiscall (const BanRecord&), ret 4 | Copies a 20-byte ban record field by field, including the date and three flag bits. | Called only from 0x556940. | high |
| 0x556d00 | `Discipline::release_ban` | thiscall (staff_id, short ban_index), ret 8 | Unregisters the ban from each rule set (0x7e8590), removes the nation index entry for slot 3, marks the record free. | Called from 12 discipline functions. `bsearch` with comparator 0x55e5f0. Assert line 1149. | high |
| 0x556e90 | `Discipline::update_ban_progress` | thiscall (staff_id, char rule_set, int count_match), ret 0xc | Counts a served match or checks the end date of a timed ban. When served, appends it to the history file and releases it. | Called from 0x557120 and rule-set methods 0x7e7b80 to 0x7e88d0. Calls 0x58e340 and 0x54c5b0 (date plus days). | medium |
| 0x557000 | `Discipline::apply_appeal_result` | thiscall (staff_id, char rule_set, 6 more args), ret 0x20 | Reduces a ban by a number of matches or days, or lifts it, and releases it when nothing is left. | Only caller is manager_manager.cpp 0x6abef0, which walks a list. | low |
| 0x557120 | `Discipline::serve_international_bans` | thiscall (club*), ret 4 | Finds all players in the nation index whose key equals the team's nation and counts a served match for each. | Called from rule-set method 0x7e7ff0. Key read from club `+0x53`. | medium |
| 0x557280 | `Discipline::post_ban_news` | thiscall (staff*, BanRecord*, arg3), ret 0xc | Builds a news item of type 0x7d2 with the ban details and the game date, and adds it to the news list. | Called from 0x55e300 and rule-set methods. 0x77bbe0 with 0x7d2, 0x785790 field writes, news manager 0xb7431c. | medium |
| 0x5576e0 | `Discipline::write_ban_news_text` | thiscall (news*, text_out, arg3), ret 0xc | Text generator for the ban news item: "banned for N matches", scope, and a match report sentence with the score line. Adds the appeal note. | Ban and score-line strings, "You have appealed against this decision". Called from news text dispatcher 0x77bc80. | high |
| 0x55b8b0 | `Discipline::write_further_ban_news_text` | thiscall (news*, text_out, arg3), ret 0xc | Text generator for the extended-ban news item ("has been given a further ban"). Falls back to 0x5576e0. | Strings "further ban". Called from 0x77bc80. | high |
| 0x55ca00 | `Discipline::process_match` | thiscall (Fixture*, result), ret 8 | Passes the match to the competition's rule set, or for friendlies and some cups to both clubs' league rule sets (0x7e7ff0). | Called from match_day.cpp 0x6b5f00. Also referenced from a .data table at 0xaab0e7. Reads fixture `+0x1c`, `+0x20`. | medium |
| 0x55cae0 | `Discipline::get_slot_value` | thiscall (staff_id, char slot, char rule_set), ret 0xc | Returns the ban index in a player slot when the slot's rule set matches, else -1. | Callers are rule-set methods 0x7e82e0, 0x7e85b0, 0x7e88d0. | medium |
| 0x55cb20 | `Discipline::set_slot` | thiscall (staff_id, char slot, char rule_set, int value), ret 0x10 | Writes rule set and value into a player slot. | Rule-set callers 0x7df1c0, 0x7e7b80, 0x7e82e0, 0x7e9050. | medium |
| 0x55cb60 | `Discipline::find_held_ban` | thiscall (staff_id, char slot), ret 8 | Returns the held-list entry for a player and slot, or null. | Rule-set callers. Walks list at `+0x1c`. | medium |
| 0x55cbc0 | `Discipline::get_ban_history` | thiscall (staff_id), ret 4 | Returns the history file result (0x58f120) for the player's head and tail record. Returns 0 for an invalid player. | Called from the player screen 0x885820 that has the " Bans" tab. | medium |
| 0x55cc20 | `Discipline::describe_ban` | thiscall (BanRecord*, char* buf, int len), ret 0xc | Formats "N match {scope} ban (starts {date})" or "{length} {scope} ban". | Strings at the call, buffer 0xc8 from 0x885820. Assert line 5123. | high |
| 0x55ce40 | `Discipline::get_scope_text` | thiscall (const char rule_sets[4], char* buf, int len, bool flag, int n), ret 0x14 | Writes "domestic", "continental", "world-wide" or "global" for a ban's rule sets. | Strings. Asserts `len >= 70`, line 5237. | high |
| 0x55d0a0 | `Discipline::get_reason_text` | thiscall (reason, char* buf, int len), ret 0xc | Writes the ban reason text: red card, yellow cards, points, misconduct, illegal substance, invalid passport. | Strings. Asserts `len >= 15`, line 5390. Callers 0x7e88d0 and 0x885820. | high |
| 0x55d270 | `Discipline::is_player_banned` | thiscall (staff*, comp_id*, a, b), ret 0x10 | Asks rule set 0 then the competition's rule set (0x7e85b0). If banned, removes the player from the club's selection slots at club `+0x1e0`. Returns 1 or 0. | 17 callers in team and match code (0x45cbe0, 0x4849f0, 0x725f70 and more). | medium |
| 0x55d360 | `Discipline::get_competition_ban` | thiscall (staff*, comp_id*), ret 8 | Returns the rule set query 0x7e8d60 for this player and competition, or 0. | Called from 0x4849f0. | low |
| 0x55d3c0 | `Discipline::get_slot_text` | thiscall (staff*, char slot, char* buf, int len), ret 0x10 | Asks the slot's rule set (0x7e88d0) for a text line into the buffer. | Called from staff_screens.cpp 0x870650 with a 0xc8 buffer. | medium |
| 0x55d420 | `Discipline::create_rule_sets` | thiscall (file), ret 4 | Allocates the 43-entry rule set table and constructs one 0x23-byte rule set object per entry, each from the file. | `calloc(0x2b, 4)`, 43 `new(0x23)` with constructors 0x7db100 to 0x7e43c0. Called from the constructor and `load`. Line 5662. | high |
| 0x55e1d0 | `Discipline::clear_ban_history` | thiscall (staff*), ret 4 | Asserts the player and his player data exist, then passes the history head and tail to 0x58f110. | Asserts at lines 5876 and 5880 on null and on staff `+0x61`. Callers 0x556830, 0x7a59e0, 0x7abac0. | medium |
| 0x55e300 | `Discipline::handle_referee_assault` | thiscall (MatchPlayer*, a, b), ret 0xc | When offence code 12 is set, drops current bans, adds a 90 or 180 day world-wide ban with reason 3 (Misconduct), posts news and may trigger a club reaction (0x758de0). | Called from rule-set method 0x7e7ff0 with `this` = `[0xae28f0]`. Calls 0x556940, 0x557280, 0x556d00. Reason 3 is "Misconduct" in 0x55d0a0. Corrected after the check below: the pilot named it `handle_drug_test_failure`. | high (corrected) |
| 0x55e620 | `discipline_appeal_news_action` | cdecl (news_id, arg2, arg3), ret 0 | Action handler for the "Appeal" button on a ban news item. Unpacks the news fields and opens the appeal confirm dialog. | Pointer stored by 0x782b70 next to the "Appeal" string. Callbacks 0x55ea10 ("Appeal against the N game ban") and 0x55ed00. Reads the fields that 0x557280 writes. | medium |

## The two gap functions

Both belong to discipline.cpp.

- 0x55e300 runs as a member of the discipline object. Its caller loads `this` from 0xae28f0. It calls only discipline helpers and uses the ban record layout.
- 0x55e620 decodes the news item that `post_ban_news` builds and opens the appeal dialog. Its two callbacks 0x55ea10 and 0x55ed00 also follow it and carry the appeal strings. The first display.cpp assert is at 0x55f1c0 and the display globals start at 0x55efe0.

## Data

### Discipline object

Global pointer `g_discipline` at 0xae28f0. Size 0x2a (42) bytes.

| Offset | Type | Use |
|---|---|---|
| +0x00 | `RuleSet**` | Table of 43 rule set objects (0x23 bytes each, with a vtable). Index 0 is asked first in every ban check. |
| +0x04 | `BanRecord*` | Ban array, 20 bytes per record. |
| +0x08 | short | Ban array capacity, grows by 100, maximum 32000. |
| +0x0a | short | Ban array used count. |
| +0x0c | `{int staff_id, int nation_id}*` | Nation index for bans in slot 3, sorted with comparator 0x55e5f0. |
| +0x10 | short | Nation index capacity, grows by 20. |
| +0x12 | short | Nation index count. |
| +0x14 | `PlayerDiscipline*` | Per-player table, 29 bytes each, one entry per staff (count at 0xae2364). |
| +0x18 | object* | Ban history file over `ban_history.tmp` (constructor 0x58e280, destructor 0x58e330, append 0x58e340). |
| +0x1c | list (14 bytes) | Held bans of players in transfer, 10-byte entries `{int staff_id, char slot, char rule_set, int value}`. |

### Per-player record (29 bytes)

| Offset | Use |
|---|---|
| +0x00 | Four slots of 5 bytes: `char rule_set` (0xff = empty) and `int ban_index`. |
| +0x14 | `int` history head. |
| +0x18 | `int` history tail. |
| +0x1c | `char` count of held bans. |

### Ban record (20 bytes)

| Offset | Use |
|---|---|
| +0x00 | `char` matches in the ban, 0xff for a timed ban. 0xff with +0x02 = -1 marks a free record. |
| +0x01 | `char` matches served. |
| +0x02 | `short` days in a timed ban. |
| +0x04 | date (8 bytes) the ban starts. |
| +0x0c | `char[4]` rule set per scope, 0xff = none. Slot 3 entries go into the nation index. |
| +0x10 | `char` reason code (see `get_reason_text`). |
| +0x11 | `short`, not identified. |
| +0x13 | flags: bit 0 extended ban, bit 1 scope text flag, bit 2 set from a match player byte in 0x55e300. |

### Other globals seen

- 0xae23bc staff array, 110 bytes per staff. 0xae2364 staff count.
- 0xae23b4 club array, 581 bytes per club.
- 0xadadfc competition pointer table. Competition `+0x50` is the rule set index.
- 0xae2c90 game date (day word, year word at 0xae2c92).
- 0xb7431c news manager. 0xae1050 transfer manager (guess).

## Open points

- The 43 rule set classes (constructors 0x7db100 to 0x7e43c0) live in another file. Their virtual slots 0 and 1 and the methods 0x7e7ff0, 0x7e82e0, 0x7e85b0, 0x7e88d0, 0x7e8d60 and 0x7e91d0 need their own map before the names of `notify_competition`, `get_competition_ban` and `apply_appeal_result` can be confirmed.
- The meaning of `+0x17` and `+0x1b` in the rule set (nation and continent) rests on club `+0x53` and its `+0x71` field only.
- 0x55cbc0 returns whatever 0x58f120 returns. It could be a count or a list of past bans.
- The second argument of 0x556270 and the third arguments of 0x557280, 0x5576e0 and 0x55b8b0 are not identified.
- The assert at line 5635 inside the constructor suggests an inlined helper defined near line 5635.
- 0x55ea10 and 0x55ed00 are missing from `functions.txt` because they are reached only through pointers. They belong in the discipline.cpp list.
- The .data pointer to 0x55ca00 at 0xaab0e7 was not followed.

## Check against earlier research

The pilot ran blind: the agent could not read `docs/`, `src/` or `tests/`. After it finished, six of its names were compared with functions that earlier research proves from other code paths.

| Address | Earlier research | Pilot name | Result |
|---|---|---|---|
| 0x5554a0 | Constructor of the object at 0xae28f0, 0x2a bytes, in game creation (`2026-10-14-game-creation-inventory.md`, row 16) | `Discipline::Discipline` | agrees |
| 0x556240 | Called once per day with the date during game creation (same file, the day loop) | `Discipline::daily_update` | agrees |
| 0x556940 | Stores the referee-assault ban (`2026-10-14-violent-conduct-inventory.md`, long ban) | `Discipline::add_ban` | agrees |
| 0x556d00 | Clears the player's pending bans in a loop (same section) | `Discipline::release_ban` | agrees |
| 0x55d270 | Player ineligible for a competition, args (staff, competition, 1, 0), ret 0x10, can clear club +0x1e0 (`2026-10-15-assistant-pick-inventory.md`) | `Discipline::is_player_banned` | agrees |
| 0x55e300 | Long ban after a referee assault, offence code 12 (`2026-10-14-violent-conduct-inventory.md`) | `handle_drug_test_failure`, confidence low | wrong |

Five of six agree. The wrong name had the pilot's lowest confidence. The pilot misread reason code 3: in 0x55d0a0 code 3 is "Misconduct" and code 5 is "Use of an illegal substance". The row above carries the corrected name.

## Cost of the pilot

- 35 functions, 12,583 instructions (the game average is 272 instructions a function, the pilot average is 360).
- One Opus agent, 169,913 tokens, 24 tool calls, 443 seconds.
- Per function: about 4,850 tokens and 13 seconds.
- Confidence: 13 high, 18 medium, 4 low (before the correction).
