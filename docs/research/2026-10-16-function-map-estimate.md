# Function map: coverage and cost

Date: 2026-10-16. Original: `clean-3968/cm0102.exe` 3.9.68. Research only. The build did not change.

Question: can every function of the original get a name and a description, and what would it cost? Pilots: `2026-10-16-function-map-pilot-discipline.md` (agents) and `2026-10-16-function-map-pilot-club-screens.md` (dossiers and the Batch API).

## Answer in short

A full map is possible. Link order, assert strings and the exception tables place every function in a source file or between two files. Names need one model pass per function.

The fast route of pilot 2 costs about $0.012 a function. It sends one prepared dossier per function through the Message Batches API and picks the model by size. For the whole game this is about $120 and 2 to 3 hours of wall time. The first route, with Claude Code agents, was about 30 million tokens and 22 agent hours.

## How many functions there are

`tools/recomp/functions.txt` lists 9,241 entries. The build translates 11,706 functions: `recomp.py` adds 2,506 more that only data points to (vtables, callbacks, screen handlers). 41 entries of the list are runtime replacements and are not translated. The map counts the union, 11,747.

| Kind | Functions | Naming work |
|---|---|---|
| Game code (0x401000 to 0x9339ff) | 6,609 | the real work |
| Exception unwind funclets (0x950000 and later) | 3,873 | none: each belongs to one parent function, found through the FuncInfo tables |
| Exception handler stubs (`mov eax, FuncInfo` then `jmp`) | 624 | none: one per parent function |
| Library without asserts (0x933a00 to 0x944c15) | 254 | full work, library not identified |
| C runtime (0x944c16 and later, not 16-byte aligned) | 387 | small: match against the MSVC runtime |

## Step 1: which file each game function is in

The exe holds 392 source path strings, for example the `tactics_screens.cpp` path at 0xA8B94C. Each assert pushes one of them. The linker placed the object files in alphabetical order of file name, with one exception in 392. So a function between two functions of one file belongs to that file.

| Method | Game functions placed | Share |
|---|---|---|
| Assert path string in the function | 2,460 | 37% |
| Link order: between two functions of the same file | 2,361 | 36% |
| Vtable: member or constructor of a class whose other methods assert in one file, inside that file's address window | 473 | 7% |
| Call graph: all callers in one file, inside the gap next to that file | 95 | 1% |
| Placed in one file | 5,389 | 82% |
| Gap between two files: the file is one of the two neighbours | 1,220 | 18% |
| Not placed at all | 0 | 0% |

Checks:

- No placed function sits outside the alphabetical window of the assert functions around it.
- Two first attempts gave wrong results and were dropped. A vtable shared by a base class put derived league methods into `league.cpp`. A caller vote put the 43 rule book files (`rb_*.cpp`) into `discipline.cpp`. Both now need the address window to agree.
- 29 functions reference two path strings (inlined code). They stay with the file of their address window.
- Classes: 301 vtables are referenced by code. 1,892 placed game functions are vtable members or constructors. The per-nation competitions are derived classes of `league.cpp` and `Cup.cpp` with the same vtable slots.
- Files with the most functions: `transfer_manager.cpp` 192, `club_screens.cpp` 132, `Database.cpp` 129, `match_eng.cpp` 120, `comp_screens.cpp` 106, `Setup.cpp` 103.

The table of all 11,747 functions with category, file or two candidate files, and method: `function-map/function_files.tsv` in the shared notes folder of this session (outside the repo). The scripts are in the same folder.

## Step 2: the pilot

discipline.cpp has 33 placed functions and 4 gap functions. One Opus agent named 35 of them blind: it could not read `docs/`, `src/` or `tests/`. Details and the comparison are in the pilot doc.

- Cost: 169,913 tokens, 443 seconds, 24 tool calls.
- Per function: 4,850 tokens and 13 seconds.
- Confidence: 13 high, 18 medium, 4 low.
- Check: five of six names agree with earlier research. The wrong name (0x55e300, a referee-assault ban read as a drug ban) had low confidence.
- The pilot found the discipline object (0xae28f0, 42 bytes), the 29-byte player record and the 20-byte ban record.
- Three of its names depend on the rule book classes in another file. A map of one file is weaker until its neighbours are mapped.

## Scaled estimate, agent route (pilot 1)

One unit is one function at the pilot's depth.

| Part | Functions | Units | Rule |
|---|---|---|---|
| Game functions in files that occur once | 4,245 | 3,849 | full cost. 792 of them are already named or cited in research and count half |
| Per-nation repeat files | 2,364 | 606 | the largest file of each family at full cost (167 functions), the others at 20% to confirm the template |
| Library without asserts | 254 | 254 | full cost |
| C runtime | 387 | 39 | 10%: match to the MSVC runtime |
| Funclets and handler stubs | 4,497 | 0 | named by script from the parent |
| Total | 11,747 | 4,748 | |

The per-nation families: leagues (87 files, 967 functions), cups (60, 508), awards (34, 145), rule books (43, 155), eurocomp (15, 214), intercomp (8, 159), world cup (7, 158), transfer rules (25, 58).

| | Tokens | Agent time |
|---|---|---|
| Naming pass: 4,748 units at 4,850 tokens and 13 seconds | 23 million | 17 hours |
| Review and reconcile across files, 30% | 7 million | 5 hours |
| Total | about 30 million | about 22 hours |

With five agents in parallel, one file each, the wall time is about 5 hours plus briefing and merging. If the per-nation files were mapped one by one, the total would rise by about 11 million tokens.

Limits of the estimate:

- One pilot file only. discipline.cpp has many strings and asserts. Screen and match engine files have fewer, so their cost per function can be higher.
- The game average is 272 instructions a function, the pilot average 360. This can lower the cost per function.
- At pilot depth about one name in ten can be wrong, mostly at low confidence. The review share covers a second look at those.

## Fast route (pilot 2)

Pilot 2 named all 149 functions of `club_screens.cpp` in one batch of 25 minutes for about $1.81. Seven of eight checked names agree with earlier research. Details are in its doc.

| Step | Functions | Cost | Wall time |
|---|---|---|---|
| Ghidra pseudocode of the whole game | 6,862 | none | done (about 40 minutes, once) |
| Dossiers for all files | 6,863 | none | about 15 minutes, by script |
| Wave 1: one batch, model by size | 6,863 game and library functions | about $85 at $0.012 a function | about 1 hour if the batch runs like the pilot. A batch can take up to 24 hours. |
| Wave 2: low-confidence names again, with all wave-1 names as context | about 1,700 (25%) | about $35 | about 20 minutes, direct requests in parallel |
| C runtime (387): Haiku or a name list | 387 | under $1 | minutes |
| Funclets and handler stubs | 4,497 | none | by script, from the parent |
| Docs: one table per file in the pilot 2 format | 392 files | none | by script |
| Total | | about $120 | 2 to 3 hours |

Notes:

- The cost per function comes from `club_screens.cpp`, whose functions average 494 instructions. The game average is 272, so $0.012 is likely high.
- In tokens the fast route is larger than the agent route: about 80 million, half of them cheap cache reads. Each request carries the full pseudocode. Money and time are lower because batch prices are half and Haiku and Sonnet take 60% of the functions.
- The per-nation families are run in full. At $0.012 a function this costs about $28. Copying slot names from the template class is then a check, not a saving.
- Quality to expect: about one name in eight wrong after wave 1, mostly marked low. Wave 2 in the pilot lifted 15 of 37 low names. It had only the names of the same file. With the names of the whole game, wave 2 should do better. That is not measured.

## Cheaper models (pilot 2, second test)

| Model for every function | club_screens.cpp | Whole game, wave 1 (scaled) | Checks right |
|---|---|---|---|
| Haiku | $0.05 | about $2.50 | 5 of 8, wrong names at medium confidence |
| Sonnet | $0.93 | about $43 | 6 of 8 |
| By size | $1.81 | about $85 | 7 of 8 |

The subscription route is in `docs/specs/2026-10-16-function-map-spec.md`. It costs no money and takes several usage windows.

## First pass

### Sweeps

league.cpp and Cup.cpp were swept on 2026-10-16 for 0.6.0 (seasons). Five Opus agents on the subscription named 113 functions: the 62 of league.cpp, 4 gap functions that went to Langlib.cpp, and the 47 of Cup.cpp. Each agent took about 4 minutes and 150,000 to 170,000 tokens. The agents read the League vtable (0x96d020, 45 slots) and the Cup vtable (0x9693c8, 40 slots) against the derived competitions. Confidence: league.cpp 36 high, 23 medium, 3 low; Cup.cpp 20 high, 23 medium, 4 low.

No pilot proves a function of these files. Earlier research cites four of them, and the four sweep names agree with it. Haiku had three of the four wrong.

| Address | Earlier research | Haiku name (confidence) | Opus name (confidence) | Result |
|---|---|---|---|---|
| 0x683ee0 | Called for each new manager's division at game creation. Inference: first news items (game-creation inventory) | `League::process_transfer_offers` (low) | `League::send_fixture_list_news` (medium) | agrees |
| 0x686a70 | Seeds `srand` with year plus seed at 0x686b34 (game-creation inventory) | `League::pair_fixtures_by_rivalry` (medium) | `League::draw_team_order` (medium): shuffles the team order, pairs clubs that share a ground | agrees |
| 0x689c20 | Holds 0x689c6e, a jump in the promotion chain | `league_cascade_remove_child_nodes` (low) | `League::process_promotion_relegation_chain` (medium) | agrees in area. The research gives no purpose, and a Northern League patch fits a promotion chain (inference) |
| 0x6701f0 | Tag pass of the message formatter (start-screen helpers) | `league_name_translate_text` (medium), placed in league.cpp | `langlib_apply_grammar_modifier` (high), placed in Langlib.cpp | agrees |

#### Wave t1a (tier 1, 2026-10-16)

Run `sweep-t1a`: 8 Opus agents named 212 functions of the `*_rules.cpp` files from Argentina to Portugal, `rb_argentina.cpp` to `rb_ireland.cpp`, and their gap functions. Each agent took 3 to 8 minutes and 123,000 to 174,000 tokens. Confidence: 165 high, 44 medium, 3 low.

The agents named the same vtable slots in different ways. After the merge, 56 names were changed to one method name a slot, by the slot index in the exe vtables. Transfer rules (base 0x970458, 13 slots) use `<NATION>_RULES` and ruling bodies (base 0x96f104, 8 slots) use `Rb<Name>`. `BRIEF-file.md` now gives these slot names. Two files keep two classes on purpose: `argentina_rules.cpp` holds three `TRANSFER_RULES` base defaults that many vtables share, and `award_manager.cpp` holds the `AWARD` base methods.

27 functions of the wave are cited. All 27 agree.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x40aa10 | Argentina filler, 4 foreign (foreign-and-tactics) | `fill_rule_slots_invalid` (Haiku) | `ARGENTINA_RULES::fill_squad_restrictions` | agrees |
| 0x412db0 | Australia filler, 6 foreign | `australia_rules_init_defaults_vtable_slot` (Haiku) | `AUSTRALIA_RULES::fill_squad_restrictions` | agrees |
| 0x424f80 | Belgium filler, +6 = 21, +8 = 2 | `nationality_rule_reset` (Haiku) | `BELGIUM_RULES::fill_squad_restrictions` | agrees |
| 0x43f2a0 | Brazil filler, 2 foreign | `brazil_rules_flags_block_reset` (Haiku) | `BRAZIL_RULES::fill_squad_restrictions` | agrees |
| 0x518620 | Croatia filler, 4 foreign | `croatia_rules_init_defaults` (Haiku) | `CROATIA_RULES::fill_squad_restrictions` | agrees |
| 0x579c50 | England filler, 3 non-EU | `england_rules_init_loan_record` (Haiku) | `ENGLAND_RULES::fill_squad_restrictions` | agrees |
| 0x5c1a30 | France filler, 5 or 3 non-EU | `france_rules_set_default_rule_block` (Haiku) | `FRANCE_RULES::fill_squad_restrictions` | agrees |
| 0x5e02d0 | Germany filler, 3 or 5 non-EU | `fill_sentinel_and_set_status_byte` (Haiku) | `GERMANY_RULES::fill_squad_restrictions` | agrees |
| 0x5ed020 | Greece filler, 5 or 4 non-EU | `greece_rules_init_fill_record` (Haiku) | `GREECE_RULES::fill_squad_restrictions` | agrees |
| 0x63c330 | Filler with no limit (Finland, Holland, Ireland and others) | `ireland_rules_reset_slots` (Haiku) | `IRELAND_RULES::fill_squad_restrictions` | agrees |
| 0x6619e0 | Italy filler, +6 = 21, +8 = 5 | `fill_italy_rule_result_and_flag_nation` (Haiku) | `ITALY_RULES::fill_squad_restrictions` | agrees |
| 0x667090 | Japan filler, 3 foreign | `japan_rules_reset_record_slots` (Haiku) | `JAPAN_RULES::fill_squad_restrictions` | agrees |
| 0x794740 | Norway filler, 2 non-EU | `norway_rules_init_slots` (Haiku) | `NORWAY_RULES::fill_squad_restrictions` | agrees |
| 0x7cd960 | Poland filler, 5 foreign | `poland_rules_reset_table` (Haiku) | `POLAND_RULES::fill_squad_restrictions` | agrees |
| 0x7d43c0 | Portugal filler, 3 or 4 non-EU | `portugal_rules_fill_slots` (Haiku) | `PORTUGAL_RULES::fill_squad_restrictions` | agrees |
| 0x579b30 | England rules object (season-change-points) | `loan_rules::loan_rules` (Haiku) | `ENGLAND_RULES::ENGLAND_RULES` | agrees |
| 0x661380 | Italy rules object (season-change-points) | `italy_awards_obj_ctor` (Haiku) | `ITALY_RULES::ITALY_RULES` | agrees |
| 0x413180 | Setup step 17 and a CD check (game-creation and import inventories) | `Award_manager::open_cd_awards` (Haiku) | `AWARD_MANAGER::AWARD_MANAGER` | agrees |
| 0x413540 | Daily call after the discipline update (game-creation inventory) | `award_manager_serialise_or_reset` (Haiku) | `AWARD_MANAGER::process_date` | agrees |
| 0x794ad0 | Setup step 21, global 0xb7433c | `norway_rules_init_slots` (Haiku) | `Notes::Notes` | agrees |
| 0x7da190 | Setup step 44, global 0xb74430 | `media_pundit_list_loader::load_and_export` (Haiku) | `PunditList::PunditList` | agrees |
| 0x425390 | Opens Meet with Board (stadium research) | `belgium_rules_open_requests_screen` (Haiku) | `board_request_screen_open` | agrees |
| 0x4253f0 | Meet with Board screen | `bel_board_meeting_dialog_show` (Haiku) | `board_request_screen_draw` | agrees |
| 0x43f4e0 | Asking price "+" step up (slow screens) | `cash_round_to_unit` (Haiku) | `Cash::step_up` | agrees |
| 0x43f580 | Asking price "-" step down | `cash_round_to_unit` (Haiku) | `Cash::step_down` | agrees |
| 0x5ed200 | Lists an object on the confirm screen (start-screen spec) | `gui_text_fit_draw_row` (Haiku) | `gui_add_text_area_from_text` | agrees in area |
| 0x66e790 | Language choice from the keyboard layout (import inventory) | `set_language_from_keyboard_layout` (Haiku) | `langlib_setup` | agrees |

#### Wave t1b (tier 1 and the start of tier 2, 2026-10-16)

Run `sweep-t1b`: 8 Opus agents named 194 functions: the rest of tier 1 (`rb_italy_cup.cpp` to `rb_wales.cpp`, `russia_rules.cpp` to `wales_rules.cpp`) and the first tier 2 files (`african_nations.cpp`, `asia_club_champ.cpp`, `asia_cup_winner.cpp`, `asia_nations.cpp`, `asia_super_cup.cpp`). Each agent took 2 to 5 minutes and 110,000 to 162,000 tokens. Confidence: 150 high, 44 medium, 0 low.

A new check (`slotcheck.py` in the work folder) gives each vtable slot of League, Cup, the transfer rules and the ruling bodies one method name. It takes the Opus name of the base method, else a fixed name, else the majority of the Opus overrides. It renamed 2 overrides of Comp slot 26 to `set_club_progress`. The Comp slots that the base sweeps did not name now have fixed names: 10 `advance_stage`, 26 `set_club_progress`, 28 `get_qualification_status`, 29 `get_qualification_status_change`. `african_nations.cpp` keeps four base stubs as `Comp::` and `League::`, because the base vtables point to them.

14 functions of the wave are cited. 0x401000 is a false hit: the docs cite 0x401000 as the start of .text. The other 13 agree.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x401000 | Start of .text, not this function | `African_Nations::African_Nations` (Haiku) | `AfricanNations::AfricanNations` | no citation |
| 0x402ac0 | Area init, clamps to 799 by 599 (widescreen and display inventories) | `area_init_bounds_and_tables` (Haiku) | `Area::init` | agrees |
| 0x402d10 | Popup open (input inventory) | `area_refresh_view_pos` (Haiku) | `Area::open_popup` | agrees |
| 0x402d80 | Popup close (input inventory) | `cup_flag_clear_and_release` (Haiku) | `Area::close_popup` | agrees |
| 0x40aae0 | Asian Club Championship constructor (club-world) | `Asia_club_champ::ctor` (Haiku) | `AsiaClubChamp::AsiaClubChamp` | agrees |
| 0x40cbe0 | Asian Cup Winners' Cup constructor | `asia_cup_winner_ctor` (Haiku) | `AsiaCupWinner::AsiaCupWinner` | agrees |
| 0x410650 | Asian Super Cup constructor | `asia_super_cup_init` (Haiku) | `AsiaSuperCup::AsiaSuperCup` | agrees |
| 0x7e29e0 | Only function of the South American ruling body (int-continental) | `rb_south_america_ctor` (Haiku) | `RbSouthAmerica::RbSouthAmerica` | agrees |
| 0x7ec950 | Russia filler, 3 foreign (foreign-and-tactics) | `fill_sentinel_and_set_flag` (Haiku) | `RUSSIA_RULES::fill_squad_restrictions` | agrees |
| 0x7f64c0 | Scotland filler, an age rule | `scotland_rules_fill_slots` (Haiku) | `SCOTLAND_RULES::fill_squad_restrictions` | agrees |
| 0x859400 | Spain rules object (season-change-points) | `spain_awards_ctor_base` (Haiku) | `SPAIN_RULES::SPAIN_RULES` | agrees |
| 0x859570 | Spain filler, 2, 3 or 4 non-EU | `spain_rules_loan_eligibility_flags` (Haiku) | `SPAIN_RULES::fill_squad_restrictions` | agrees |
| 0x859aa0 | Setup step 29, global 0xdd7ef0 (game-creation inventory) | `find_cm_cd_drive` (Haiku) | `find_cm_cd_drive` | agrees |
| 0x9021d0 | Turkey filler, 6 foreign or an age rule | `turkey_rules_fill_option_list_for_player` (Haiku) | `TURKEY_RULES::fill_squad_restrictions` | agrees |

#### Wave w03 (tier 2, 2026-10-16)

Run `sweep-w03`: 8 Opus agents named 172 functions of `con_champ.cpp`, `conmebol_liber.cpp`, `conmebol_merc.cpp`, `conmebol_seeding.cpp` with the `CONTRACT_MANAGER` gap functions, `eur_super_cup.cpp`, `euro_champ.cpp`, `euro_champ_qual.cpp` and `european_cup.cpp`. Each agent took 1 to 4 minutes and 99,000 to 143,000 tokens. Confidence: 138 high, 33 medium, 1 low.

After the merge, 23 names changed: one class name a file in `conmebol_seeding.cpp` (`ConmebolSeeding`), one pattern for the static Date initialisers of `contract_manager.cpp`, and Comp slots 8 and 9 as `load_extra_data` and `save_extra_data`. New fixed slot names: 11 `skip_season_reset`, 14 `write_progress_text`, 30 `get_finals_comp`. Three gap functions of a transfer rules class for Europe (vtable 0x96a188) went to `european_awards.cpp`, the nearer file of the pair. Their source file is probably lost (inference from the file order). Seven addresses are pieces inside a larger function (two in `conmebol_merc.cpp`, five in the phase 1 draw of `european_cup.cpp`). They keep a name with the suffix of their parent.

From this wave on, the brief points the agents to a local copy of the champman0102 forum "Offset Index" thread for field hints.

7 functions of the wave are cited. All 7 agree.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x4c0b50 | findnext in the copy of .lng files (start-new-game inventory) | `find_next_matching_file` (Haiku) | `file_find_next` | agrees |
| 0x4c0c40 | CONCACAF Champions Cup constructor (club-world) | `cup_champ_ctor_thiscall` (Haiku) | `ConChamp::ConChamp` | agrees |
| 0x4c2f50 | Copa Libertadores constructor | `conmebol_liber_ctor` (Haiku) | `ConmebolLiber::ConmebolLiber` | agrees |
| 0x4c46c0 | Copa Mercosur constructor | `PHL_league_object::PHL_league_object` (Haiku) | `ConmebolMerc::ConmebolMerc` | agrees |
| 0x4c5fe0 | Setup step 9, global 0xae1000, 0x75 bytes (game-creation inventory) | `find_cd_and_load_teams` (Haiku) | `ConmebolSeeding::ConmebolSeeding` | agrees |
| 0x4c7040 | Setup step 32, global 0xae1050, 0x3c bytes | `cm_cd_check_and_load_seeding_data` (Haiku) | `CONTRACT_MANAGER::CONTRACT_MANAGER` | agrees |
| 0x5832c0 | Filler with no limit in vtable 0x96a188 (foreign-and-tactics) | `qual_reset_slots_to_minus_one` (Haiku) | `TRANSFER_RULES::fill_squad_restrictions` | agrees |

#### Wave w04 (tier 2, 2026-10-16)

Run `sweep-w04`: 8 Opus agents named 200 functions: the rest of `european_cup.cpp`, `fifa_confed.cpp`, `fifa_rankings.cpp`, `goldcup.cpp`, `host_country.cpp`, `inter_amer_cup.cpp`, `intertoto_cup.cpp`, `key_nation.cpp` and their gap functions. Each agent took 2 to 5 minutes and 110,000 to 167,000 tokens. Confidence: 165 high, 35 medium, 0 low.

Many gap functions hold other code: news stories between `european_cup.cpp` and `fifa_confed.cpp`, the display layer and the GUI drawing code between `goldcup.cpp` and `gre_cup.cpp`, and `FILE_LLIST` methods in `fifa_rankings.cpp`. The brief allows only the two files of the gap, so these sit in the nearer file. One merge fix: three news functions moved to `european_cup.cpp`, so the whole news gap is in one file. Comp slot 22 is now `collect_club_list`. Fifteen addresses in `european_cup.cpp` are pieces inside the phase 2 draw and carry its name with a suffix.

50 functions of the wave are cited. 47 agree, 2 agree in area and 1 citation makes no claim about the function.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x585ab0 | Phase 2 group draw, groups of four (sweep-europe) | `european_cup_group_draw_recurse` (Haiku) | `EuropeanCup::draw_phase2_groups` | agrees |
| 0x586330 | Champions League stage list (sweep-europe) | `european_cup_setup_group_stage` (Haiku) | `EuropeanCup::draw_quarter_finals` | agrees |
| 0x587220 | News gap 0x587220 to 0x58aa60 (sweep-europe) | `shortlist_manager_news_transfer_offer` (Haiku) | `news_create_scout_watching_story` | agrees |
| 0x58a890 | Fixed-date event at game start (game-creation inventory) | `news_post_transfer_rule_change` (Haiku) | `news_post_transfer_system_change` | agrees |
| 0x58aa60 | News gap (sweep-europe) | `news_compensation_for_player_loss` (Haiku) | `news_insurance_payout_text` | agrees |
| 0x58acd0 | FA meeting screen (sweep-europe) | `fifa_confed_open_screen_dialog` (Haiku) | `fa_meeting_open_screen` | agrees |
| 0x58b0f0 | FA meeting screen (sweep-europe) | `european_cup_phase_dispatch_handler` (Haiku) | `fa_meeting_screen_handle_event` | agrees |
| 0x5e2620 | Display init, UpdateWindow (import inventory) | `gre_cup_init_graphics_window` (Haiku) | `graphics_init_display` | agrees |
| 0x5e2cc0 | Display release, DestroyWindow (import inventory) | `goldcup_shutdown_ui_resources` (Haiku) | `graphics_shutdown_display` | agrees |
| 0x5e2e50 | Activation handling (input inventory) | `goldcup_release_pending_objects` (Haiku) | `graphics_restore_lost_surfaces` | agrees in area |
| 0x5e2ea0 | Minimise path on deactivate (input inventory) | `get_global_flag_ae319c` (Haiku) | `graphics_is_full_screen` | agrees |
| 0x5e2eb0 | Full screen flip (display inventory) | `blit_cursor_region_update` (Haiku) | `graphics_flip_rect` | agrees |
| 0x5e30e0 | Screen draw helpers (display inventory) | `gold_cup_render_fade_frames` (Haiku) | `graphics_fade_rect` | agrees |
| 0x5e3680 | Clip helper (widescreen inventory) | `clip_rect_to_surface` (Haiku) | `clip_rect_to_surface` | agrees |
| 0x5e3730 | Clip helper (widescreen inventory) | `goldcup_draw_line_clipped` (Haiku) | `graphics_draw_line` | agrees |
| 0x5e3b50 | Rectangle draw (display inventory) | `draw_clipped_quad_outline` (Haiku) | `graphics_draw_rect` | agrees in area |
| 0x5e3dd0 | Screen image save (display inventory) | `grab_screen_region_bitmap` (Haiku) | `gui_save_screen_image` | agrees |
| 0x5e3fd0 | Screen image restore (display inventory) | `blit_region_to_surface` (Haiku) | `gui_restore_screen_image` | agrees |
| 0x5e40d0 | Picture load (display inventory) | `load_bitmap_cached_or_converted` (Haiku) | `gui_load_rgn_picture` | agrees |
| 0x5e42b0 | Image free (display inventory) | `release_owned_buffer` (Haiku) | `gui_free_image` | agrees |
| 0x5e42e0 | Clears the 800x600 rectangle (display inventory) | `gre_cup_blit_palette_surface` (Haiku) | `gui_darken_rect` | agrees |
| 0x5e4560 | Colours set up at start (area-widget-draw spec) | `gre_init_palette_colours` (Haiku) | `gui_init_palette_colours` | agrees |
| 0x5e4ba0 | Bitmap font set load (hd_text.c) | `load_font_set` (Haiku) | `gui_load_bitmap_font` | agrees |
| 0x5e5060 | Hash escape decode (gui_font.c) | `decode_hash_escaped_string` (Haiku) | `text_decode_hash_escapes` | agrees |
| 0x5e5160 | Line draw (clickable-names inventory) | `gre_cup_render_text_line` (Haiku) | `gui_draw_text_line` | agrees |
| 0x5e5d60 | Gap between two glyph codes (gui_font.c) | `kerning_adjust_pair` (Haiku) | `font_glyph_gap` | agrees |
| 0x5e5df0 | Panel draw (display inventory) | `draw_styled_rect` (Haiku) | `gui_draw_panel` | agrees |
| 0x5e6d80 | Text box draw (display inventory) | `gre_cup_draw_wrapped_text` (Haiku) | `gui_draw_text_box` | agrees |
| 0x5e76f0 | Progress box (display inventory) | `gold_cup_draw_progress_panel` (Haiku) | `gui_draw_progress_box` | agrees |
| 0x5e7840 | Ellipse panel style (area-widget-draw spec) | `goldcup_draw_dashed_ellipse_pass` (Haiku) | `gui_draw_circle` | agrees |
| 0x5e7aa0 | Ellipse panel style (area-widget-draw spec) | `draw_filled_ellipse_pixels` (Haiku) | `circle_plot_points` | agrees |
| 0x5e8170 | Cursor kind cache 0x9cd804 (input inventory) | `set_cursor_mode` (Haiku) | `set_cursor_mode` | agrees |
| 0x5e81e0 | Cursor kind cache (input inventory) | `set_cursor_for_mode` (Haiku) | `reapply_cursor_mode` | agrees |
| 0x5e8280 | Called with 1 at start (import inventory) | `set_gold_cup_global` (Haiku) | `set_software_cursor_flag` | agrees |
| 0x5f97c0 | Setup step 15, global 0xb5e134 (game-creation inventory) | `host_country_find_cd_and_load` (Haiku) | `HostCountry::HostCountry` | agrees |
| 0x5fa850 | Daily update call (game-creation inventory) | `host_country_update_sibling_records` (Haiku) | `HostCountry::daily_update` | agrees |
| 0x5fab70 | Listed by a constant scan only (widescreen inventory) | `host_country_assign_players_from_list` (Haiku) | `HostCountry::pick_club_world_champ_hosts` | no claim |
| 0x632080 | Inter-American Cup constructor (club-world) | `Inter_Amer_Cup::Inter_Amer_Cup` (Haiku) | `InterAmerCup::InterAmerCup` | agrees |
| 0x632340 | Inter-American Cup, 1 round (sweep-europe) | `inter_amer_cup::create_round_table` (slot copy) | `InterAmerCup::create_round_table` | agrees |
| 0x633290 | Intertoto process_fixture, nation points (sweep-europe) | `intertoto_cup::process_fixture` (slot copy) | `IntertotoCup::process_fixture` | agrees |
| 0x633300 | Intertoto Cup rounds (sweep-europe) | `intertoto_cup::create_round_table` (slot copy) | `IntertotoCup::create_round_table` | agrees |
| 0x6336c0 | Intertoto entrants (sweep-europe) | `intertoto_cup_collect_team_slots` (Haiku) | `IntertotoCup::collect_entrants` | agrees |
| 0x633d40 | Intertoto Cup rounds (sweep-europe) | `intertoto_cup::update_for_date` (slot copy) | `IntertotoCup::update_for_date` | agrees |
| 0x667150 | Key nation table build (game-creation inventory) | `key_nation_init_table` (Haiku) | `key_nation_build_table` | agrees |
| 0x669140 | Numbers the league entries, count at 0xb64738 | `key_nation_reset_slots` (Haiku) | `key_nation_number_active_entries` | agrees |
| 0x669180 | Makes the competitions (game-creation inventory) | `key_nation_check_table_entries` (Haiku) | `key_nation_create_all_comps` | agrees |
| 0x6699d0 | Creator runs only when an entry is active (sweep-europe) | `nation_has_active_entry` (Haiku) | `key_nation_continent_has_selected_nation` | agrees |
| 0x669f50 | key_nation.cpp assert (start-new-game inventory) | `key_nation_division_name_format` (Haiku) | `key_nation_extra_division_name` | agrees |
| 0x66a0c0 | Sort order of the 35 league entries | `key_nation_table_init` (Haiku) | `key_nation_sorted_order` | agrees |
| 0x66a100 | qsort comparator of the entries | `key_nation_compare_names` (Haiku) | `key_nation_compare_entries` | agrees |

#### Wave w05 (tier 2, 2026-10-16)

Run `sweep-w05`: 8 Opus agents named 201 functions of `national_teams.cpp`, `national_teams_screens.cpp` (with `Network` gap functions), `oceania_club_champ.cpp`, `oceania_nations.cpp` (with the officials stats screen and `OfficialsManager` gap functions), `olympics.cpp`, `uefa_cup.cpp` and `uefa_seeding.cpp`. Each agent took 2 to 5 minutes and 112,000 to 188,000 tokens. Confidence: 157 high, 43 medium, 1 low. The merge needed no fixes.

43 functions of the wave are cited. 38 agree, 2 agree in area and 3 citations only list the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x76a4e0 | Setup step 23, global 0xb74318 (game-creation inventory) | `national_teams_find_cd_and_init` (Haiku) | `NationalTeams::NationalTeams` | agrees |
| 0x76b100 | Daily update call (game-creation inventory) | `national_teams_process_fixture_round` (Haiku) | `NationalTeams::daily_update` | agrees |
| 0x76c120 | fill_squad, wider thresholds for Australia (sweep-international) | `national_teams_fill_squad_from_pool` (Haiku) | `NationalTeams::select_squad` | agrees |
| 0x76d430 | get_squad_limits (sweep-international) | `national_teams_position_pair_or_assert` (Haiku) | `national_teams_get_squad_size_limits` | agrees |
| 0x76d940 | is_tournament (sweep-international) | `is_national_team_type` (Haiku) | `national_teams_is_finals_comp` | agrees |
| 0x76d9b0 | One of three tests in the player search (crash-fixes) | `player_matches_search_filter` (Haiku) | `national_teams_player_is_eligible` | agrees in area |
| 0x77a7d0 | Network session object created by 0x7ff500 (display inventory) | `network_session_init` (Haiku) | `Network::Network` | agrees |
| 0x77a890 | Network session object (display inventory) | `network_send_list_server_bye` (Haiku) | `Network::~Network` | agrees in area |
| 0x77a9f0 | Every wsock32 caller is in 0x77a7d0 to 0x77aa77 (import inventory) | `Network::handle_list_server_ack` (Haiku) | `Network::host_or_join_game` | agrees |
| 0x77ac00 | Listed as a callee only (display inventory) | `network_release_slot_socket` (Haiku) | `Network::release_slot_socket` | no claim |
| 0x77af20 | Listed as a callee only (display inventory) | `Network::list_server_exchange` (Haiku) | `Network::receive_packet` | no claim |
| 0x77b630 | Listed as a callee of scrman init only (display inventory) | `buffer_alloc_or_fatal` (Haiku) | `net_buffer_init` | no claim |
| 0x796fd0 | Oceania Champions Cup constructor (club-world) | `oceania_club_champ_ctor` (Haiku) | `OceaniaClubChamp::OceaniaClubChamp` | agrees |
| 0x797230 | Oceania Club Championship stages (sweep-europe) | `oceania_club_champ::create_schedule` (slot copy) | `OceaniaClubChamp::create_schedule` | agrees |
| 0x798220 | Takes 8 clubs (sweep-europe) | `oceania_club_champ_seed_qualifiers` (Haiku) | `OceaniaClubChamp::select_qualified_clubs` | agrees |
| 0x7988b0 | Oceania Nations Cup constructor (sweep-international) | `oceania_nations_cup_ctor` (Haiku) | `OceaniaNations::OceaniaNations` | agrees |
| 0x79b580 | Setup step 40, global 0xb74340 (game-creation inventory) | `officials_manager_find_cd_and_load` (Haiku) | `OfficialsManager::OfficialsManager` | agrees |
| 0x79f530 | Olympics constructor (sweep-international) | `olympics_cup_ctor` (Haiku) | `Olympics::Olympics` | agrees |
| 0x7a16d0 | Total physical memory in MB (import inventory) | `os_get_total_physical_memory_mb` (Haiku) | `os_get_total_physical_memory_mb` | agrees |
| 0x7a1710 | Free memory in MB (game-creation inventory) | `get_free_memory_mb` (Haiku) | `os_get_free_memory_mb` | agrees |
| 0x7a1750 | Loads cpuinf32.dll (import inventory) | `detect_cpu_speed_class` (Haiku) | `os_get_cpu_speed_class` | agrees |
| 0x9025c0 | UEFA Cup rounds (sweep-europe) | `uefa_cup::create_round_table` (slot copy) | `UefaCup::create_round_table` | agrees |
| 0x902b90 | build_entrants (sweep-europe) | `uefa_cup_build_qualifier_list` (Haiku) | `UefaCup::build_entrants` | agrees |
| 0x903360 | Setup step 8, global 0xde1f58 (game-creation inventory) | `cm_cd_wait_and_load_league_data` (Haiku) | `UefaSeeding::UefaSeeding` | agrees |
| 0x9035a0 | Daily call (game-creation inventory) | `uefa_seeding_on_season_update` (Haiku) | `UefaSeeding::update_season` | agrees |
| 0x903890 | English Intertoto place (sweep-europe) | `uefa_seeding_pick_team_for_slot` (Haiku) | `UefaSeeding::pick_english_intertoto_club` | agrees |
| 0x903b50 | Places by coefficient (season-change-points) | `uefa_seeding_rotate_and_sort_entries` (Haiku) | `UefaSeeding::roll_coefficients` | agrees |
| 0x903cc0 | Places by coefficient (season-change-points) | `uefa_seeding_assign_pots_and_flags` (Haiku) | `UefaSeeding::set_place_counts` | agrees |
| 0x904040 | collect_european_clubs (sweep-europe) | `uefa_seeding_collect_qualified_teams` (Haiku) | `UefaSeeding::collect_european_clubs` | agrees |
| 0x904210 | Champions League places (season-change-points) | `uefa_seeding_assign_pots` (Haiku) | `UefaSeeding::assign_champions_league_places` | agrees |
| 0x904580 | UEFA Cup places (season-change-points) | `uefa_seeding_assign_qualified_teams` (Haiku) | `UefaSeeding::assign_uefa_cup_places` | agrees |
| 0x904e30 | Intertoto places (season-change-points) | `uefa_seeding_assign_pots` (Haiku) | `UefaSeeding::assign_intertoto_places` | agrees |
| 0x905230 | Load of uefa_seeding.tmp (sweep-europe) | `uefa_seeding_load_tmp_file` (Haiku) | `UefaSeeding::load_from_temp_file` | agrees |
| 0x905510 | Save of uefa_seeding.tmp (sweep-europe) | `uefa_seeding_load_file` (Haiku) | `UefaSeeding::save_to_temp_file` | agrees |
| 0x905790 | Cup semi-final losers table (sweep-europe) | `uefa_seeding_add_qualified_team` (Haiku) | `UefaSeeding::record_cup_semi_final_losers` | agrees |
| 0x9058b0 | add_nation_points (sweep-europe) | `uefa_seeding_add_points_to_entry` (Haiku) | `UefaSeeding::add_nation_points` | agrees |
| 0x9058e0 | check_registration_deadlines (sweep-europe) | `uefa_seeding_dispatch_by_team_pair` (Haiku) | `uefa_check_registration_deadlines` | agrees |
| 0x9059e0 | apply_euro_cfg_order (sweep-europe) | `uefa_seeding_load_euro_config_teams` (Haiku) | `UefaSeeding::apply_euro_cfg_order` | agrees |
| 0x905c60 | get_nation_places (sweep-europe) | `uefa_seeding_lookup_entry_flags` (Haiku) | `UefaSeeding::get_nation_places` | agrees |
| 0x905cb0 | Sort of nations by coefficient (sweep-europe) | `uefa_seeding_compare_float_key` (Haiku) | `uefa_compare_nations_by_coefficient` | agrees |
| 0x905ce0 | Sort of the club records (sweep-europe) | `uefa_seeding_compare_entries` (Haiku) | `uefa_compare_seed_clubs` | agrees |
| 0x905d40 | validate_euro_cfg (sweep-europe) | `load_euro_config` (Haiku) | `validate_euro_cfg` | agrees |
| 0x905f00 | post_qualifiers_news (sweep-europe) | `uefa_seeding_build_qualified_report_news` (Haiku) | `uefa_post_qualifiers_news` | agrees |

#### Wave w06 (tier 2 done, 2026-10-16)

Run `sweep-w06`: 8 Opus agents named 184 functions of the six World Cup qualifying sections (`wc_*.cpp`), `world_club_champ.cpp`, `world_club_cup.cpp` and `world_cup.cpp`. Each agent took 2 to 3 minutes and 116,000 to 157,000 tokens. Confidence: 149 high, 35 medium, 0 low. Tier 2 is done.

An agent showed that Cup slot 2 is `start_new_season`: League and Cup share this Comp slot, and the season change calls it for every competition. The Cup sweep had named it `release_data`. The base row 0x518780 and 13 cup overrides of earlier waves are now `start_new_season`, and the brief gives the slot name.

34 functions of the wave are cited. 32 agree and 2 agree in area.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x90f210 | African section constructor (sweep-international) | `wc_african_cup::ctor` (Haiku) | `WcAfricanCup::WcAfricanCup` | agrees |
| 0x911bf0 | Asian section constructor | `wc_asia_league::wc_asia_league` (Haiku) | `WcAsiaLeague::WcAsiaLeague` | agrees |
| 0x913740 | Two final groups of five (sweep-international) | `wc_asia_league_build_qualifier_stages` (Haiku) | `WcAsiaLeague::create_second_round_groups` | agrees |
| 0x913d50 | Asian section stage list | `wc_asia_league_create_qualifier_cup` (Haiku) | `WcAsiaLeague::create_playoff_tie` | agrees in area |
| 0x913f60 | Asian entrant list by continent | `wc_asia_league::build_qualified_team_list` (Haiku) | `WcAsiaLeague::build_entrant_list` | agrees |
| 0x9149f0 | CONCACAF section constructor | `wc_concacaf_cup_ctor` (Haiku) | `WcConcacafCup::WcConcacafCup` | agrees |
| 0x918200 | European section constructor | `WC_Europe_League::ctor_setup_league` (Haiku) | `WcEuropeLeague::WcEuropeLeague` | agrees |
| 0x918dd0 | European section, stages by exempt count | `wc_europe_league_qualified_stage_result` (Haiku) | `WcEuropeLeague::get_group_size` | agrees |
| 0x918ec0 | European section, group draw | `wc_europe_league_fill_group_slots` (Haiku) | `WcEuropeLeague::draw_groups` | agrees |
| 0x919e30 | European entry pool without hosts and holder | `wc_qualifier_pool_setup` (Haiku) | `WcEuropeLeague::build_entry_pool` | agrees |
| 0x91b760 | 2002 loader with the real groups (sweep-international) | `WC_Europe_League::load_fixtures` (Haiku) | `WcEuropeLeague::load_2002_campaign` | agrees |
| 0x91db60 | Fixed 2001 fixtures (sweep-season-end) | `wc_europe_league_add_fixture_for_group_team` (Haiku) | `WcEuropeLeague::add_fixed_result` | agrees |
| 0x91df90 | Oceania section constructor | `wc_oceania_league_ctor` (Haiku) | `WcOceaniaLeague::WcOceaniaLeague` | agrees |
| 0x91f800 | Oceania section, 3 stages | `wc_oceania_league_create_cup_stage` (Haiku) | `WcOceaniaLeague::create_playoff_stage` | agrees |
| 0x9202b0 | South American section constructor | `wc_south_american_league_ctor` (Haiku) | `WcSouthAmericanLeague::WcSouthAmericanLeague` | agrees |
| 0x9216d0 | 2002 loader with the real groups | `wc_south_american_league::add_fixtures` (Haiku) | `WcSouthAmericanLeague::load_2002_campaign` | agrees |
| 0x922350 | Fixed 2001 fixtures (sweep-season-end) | `wc_south_american_league_qualify_team_fixture` (Haiku) | `WcSouthAmericanLeague::add_fixed_fixture` | agrees |
| 0x9226c0 | Setup step 43, weather.cpp, reads weather.cfg (game-creation inventory) | `find_cm_cd_drive` (Haiku) | `Weather::Weather` | agrees |
| 0x929140 | Made once at a new game (sweep-season-end) | `world_club_champ_init` (Haiku) | `WorldClubChamp::WorldClubChamp` | agrees |
| 0x929390 | Club World Championship, 2 groups of 4 (sweep-europe) | `world_club_champ::create_schedule` (slot copy) | `WorldClubChamp::create_schedule` | agrees |
| 0x929e60 | Club World Championship, 2 groups of 4, 8 clubs (sweep-europe) | `world_club_champ_init_stage_pair` (Haiku) | `WorldClubChamp::create_second_and_third_groups` | agrees in area. Opus finds 12 entrants in three groups, the research gives 2 groups of 4. Not checked |
| 0x92a140 | Final and third place match (sweep-europe) | `world_club_champ_build_final_stage` (Haiku) | `WorldClubChamp::create_knockout_stage` | agrees |
| 0x92a8b0 | Host clubs and continental winners (sweep-europe) | `world_club_champ_init_slots` (Haiku) | `WorldClubChamp::pick_entrants` | agrees |
| 0x92ae60 | Host clubs and continental winners (sweep-europe) | `world_club_champ_find_club_for_team_pick` (Haiku) | `WorldClubChamp::pick_entrant` | agrees |
| 0x92b4b0 | Intercontinental Cup constructor (club-world) | `WorldClubCup::WorldClubCup` (Haiku) | `WorldClubCup::WorldClubCup` | agrees |
| 0x92b770 | World Club Cup, 1 match (sweep-europe) | `world_club_cup::create_round_table` (slot copy) | `WorldClubCup::create_round_table` | agrees |
| 0x92b8b0 | CL and Libertadores winners, fallback above 5000 (sweep-europe) | `WorldClubCup::pick_qualifiers` (Haiku) | `WorldClubCup::pick_qualifiers` | agrees |
| 0x92bf50 | World Cup constructor (sweep-international) | `world_cup::world_cup` (Haiku) | `WorldCup::WorldCup` | agrees |
| 0x92d770 | 16-team Cup_Stage (sweep-international) | `world_cup_stage_setup` (Haiku) | `WorldCup::create_knockout_stage` | agrees |
| 0x92da40 | build_team_slots, holder then hosts (sweep-international) | `world_cup_build_team_slots` (Haiku) | `WorldCup::set_up_team_list` | agrees |
| 0x92e740 | Fills the team slots (sweep-international) | `world_cup_push_entry` (Haiku) | `WorldCup::add_qualified_team` | agrees |
| 0x92e940 | draw_groups, real 2002 draw (sweep-international) | `world_cup_generate_groups` (Haiku) | `WorldCup::draw_groups` | agrees |
| 0x92f230 | Seeded pots of 8 (sweep-international) | `world_cup_build_squad_and_groups` (Haiku) | `WorldCup::seed_teams` | agrees |
| 0x92f680 | Fixed sizes of the World Cup (sweep-international) | `world_cup::set_predicted_positions` (slot copy) | `WorldCup::set_predicted_positions` | agrees |

#### Wave w07 (tier 3, 2026-10-16)

Run `sweep-w07`: 8 Opus agents named 213 functions: the last 3 of tier 2 (`world_cup_awards.cpp`), `Date.cpp` with the Database.cpp gap helpers, `comp.cpp`, `comp_util.cpp` and `contract_manager.cpp`. Each agent took 1 to 5 minutes and 83,000 to 211,000 tokens. Confidence: 121 high, 86 medium, 6 low.

The `comp.cpp` agents named the Comp base vtable 0x9689e8. League and Cup share these slots, so three Cup base names now take the League name of the same slot: 4 `check_complete`, 12 `record_season_honours` and 21 `get_match_importance`. Two cup overrides changed with them. `contract_manager.cpp` keeps one `STAFF_CONTRACT::operator=`, an inline member that the compiler put in this file.

21 functions of the wave are cited. 14 agree, 1 agrees in area and 6 citations only list the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x48c580 | Comp base constructor (sweep-rulebooks) | `comp_record_init` (Haiku) | `Comp::Comp` | agrees |
| 0x48ce10 | Loop over the comp table calling virtual +0x10 (crash-fixes) | `comp_chain_all_done` (Haiku) | `Comp::all_stages_complete` | agrees |
| 0x48d500 | Listed by a constant scan only (widescreen inventory) | `comp_build_fixture_list` (Haiku) | `Comp::build_match_date_list` | no claim |
| 0x48dd70 | Listed as a callee only (holiday inventory) | `match_day_result_code` (Haiku) | `comp_active_key_nation_of_fixture` | no claim |
| 0x48dee0 | Listed as a callee only (holiday inventory) | `comp_find_active_team_slot` (Haiku) | `comp_active_key_nation_of_club` | no claim |
| 0x48e1c0 | Listed by a constant scan only (widescreen inventory) | `collect_unique_team_pointers` (Haiku) | `Comp::collect_club_list` | no claim |
| 0x48ee40 | Promotion refused for a small ground (stadium research) | `news_relegation_promotion_text` (Haiku) | `Comp::write_promotion_relegation_override_news_text` | agrees |
| 0x48f2d0 | Writes the substitutes word (sweep-season-end) | `comp_apply_season_flags_thiscall` (Haiku) | `Comp::set_fixture_substitutes` | agrees |
| 0x48f4a0 | Opens the competition from a clickable name (clickable-names inventory) | `comp_open_scrman_setup_window` (Haiku) | `comp_screens_open_comp_screen` | agrees |
| 0x4b07b0 | Sort compare of two teams by +0x80 (crash-fixes) | `compare_short_field_0x80_descending` (Haiku) | `compare_clubs_by_reputation_desc` | agrees |
| 0x4bffc0 | Text formatter, detail unknown (display inventory) | `comp_util_round_name_text` (Haiku) | `comp_util_format_stage_round_name` | agrees in area |
| 0x4c7560 | Setup step 32, after the constructor (game-creation inventory) | `CONTRACT_MANAGER::initialise_all_staff_contracts` (Haiku) | `CONTRACT_MANAGER::initialise_all_staff_contracts` | agrees |
| 0x4c7ac0 | Daily call with the game date (game-creation inventory) | `CONTRACT_MANAGER::update_daily_contracts` (Haiku) | `CONTRACT_MANAGER::daily_update` | agrees |
| 0x4cc2e0 | Morale word in a column (columns inventory) | `staff_morale_comment_text_write` (Haiku) | `contract_manager_write_morale_text` | agrees |
| 0x4cc3a0 | Morale text and byte for the comparison screen | `compare_staff_morale_comment` (Haiku) | `contract_manager_write_morale_comparison_text` | agrees |
| 0x4d1600 | Listed as a caller only (columns inventory) | `contract_manager_calc_player_offer_value` (Haiku) | `contract_manager_calc_asking_price` | no claim |
| 0x4d4fb0 | Listed by a constant scan only (widescreen inventory) | `contract_manager_generate_news_story` (Haiku) | `contract_manager_write_offer_response_news_text` | no claim |
| 0x54a130 | date_set for the birth date (game-creation inventory) | `date_set_ymd` (Haiku) | `Date::set` | agrees |
| 0x54a2d0 | Date constructor with a weekday (game-creation inventory) | `date_next_weekday_on_or_after` (Haiku) | `Date::get_nearest_weekday` | agrees |
| 0x54cf70 | Display function that picks the month table (crash-fixes) | `date_days_into_month_offset_lookup` (Haiku) | `Date::get_day_of_month` | agrees |
| 0x54d580 | Builds the path and fopens (database inventory) | `db_date_table_teardown_or_register` (Haiku) | `db_files_open_for_read` | agrees |

#### Wave w08 (tier 3, 2026-10-16)

Run `sweep-w08`: 8 Opus agents named 204 functions: the rest of `contract_manager.cpp` (its news senders and text writers), `cup_stage.cpp`, `fix_man.cpp`, `game.cpp`, `league_stage.cpp`, `mini_cup.cpp`, `mini_league.cpp`, `new_transfer_rule_screens.cpp` and `player_regen.cpp`, with their gap functions. Each agent took 1 to 7 minutes and 94,000 to 216,000 tokens. Confidence: 139 high, 64 medium, 1 low. The merge needed no fixes. `main.cpp` holds `LLIST` and `LLIST_NODE` gap functions.

23 functions of the wave are cited. 15 agree, 2 agree in area and 3 citations make no claim about the purpose. The old name `game_update_data_pass` (0x5d5ae0) is now `game_season_update_pass`, as the season-end research names it.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x524cd0 | Sets 0xae2330 to 31 Aug 2001 (sweep-season-end) | `register_cup_stage_atexit_init` (Haiku) | `init_transfer_rule_change_date` | agrees |
| 0x524d10 | Second read path of the loader (database inventory) | `database_load_with_flag_one` (Haiku) | `load_database_from_stream` | agrees |
| 0x524d50 | fread path of the loader (database inventory) | `database_load_wrapper_zero_flag` (Haiku) | `load_database_from_files` | agrees |
| 0x5a9cf0 | Setup step 26, global 0xae2a58 (game-creation inventory) | `fix_man_init_drive_scan` (Haiku) | `FIXTURE_MANAGER::FIXTURE_MANAGER` | agrees |
| 0x5aa2d0 | Called when the year changes (game-creation inventory) | `fix_man_set_current_fixture_week` (Haiku) | `FIXTURE_MANAGER::set_year` | agrees |
| 0x5aaf00 | Daily call (game-creation inventory) | `fix_man_refresh_fixtures_and_cups` (Haiku) | `FIXTURE_MANAGER::daily_update` | agrees |
| 0x5abf10 | Fixture list used by the next-fixture search (holiday inventory) | `fix_man_get_cached_entry` (Haiku) | `FIXTURE_MANAGER::get_fixture_list` | agrees |
| 0x5af5b0 | Listed as a callee only (holiday inventory) | `fix_man_find_side_flag` (Haiku) | `fix_man_get_partner_nation_type` | no claim |
| 0x5af630 | Setup step 39, global 0xae2a60 (game-creation inventory) | `fix_man_clear_fog_slots_and_load` (Haiku) | `FOG_OF_WAR::FOG_OF_WAR` | agrees |
| 0x5cc6b0 | Called by WinMain, calls the main loop (screen-stack inventory) | `friendly_run_wrapper` (Haiku) | `game_run` | agrees |
| 0x5cc6d0 | Called with lpCmdLine (import inventory) | `game_init_from_command_line` (Haiku) | `game_init_from_command_line` | agrees |
| 0x5cc850 | Runs to the start date (game-creation inventory) | `friendly_season_loop` (Haiku) | `game_run_to_start_date` | agrees |
| 0x5cccd0 | Main loop under WinMain (screen-stack inventory) | `friendly_match_day_loop` (Haiku) | `game_main_day_loop` | agrees |
| 0x5cde40 | Listed as a reader of an input global only (input inventory) | `friendly_refresh_pending_matches` (Haiku) | `game_run_human_turn` | no claim |
| 0x5ce2d0 | Part of the day order (sweep-season-end) | `Database::update_game_data_per_player` (Haiku) | `game_daily_staff_update` | agrees in area |
| 0x5d5ae0 | Season update pass (game-creation inventory, sweep-season-end) | `game_update_data_pass` (Haiku) | `game_season_update_pass` | agrees |
| 0x5d5f20 | Called after the season update pass, no purpose given | `player_form_update_sort_by_rating` (Haiku) | `game_update_nation_coefficients` | no claim |
| 0x5d6280 | League table entry update day (sweep-season-end) | `game_update_staff_wages` (Haiku) | `game_update_league_attendances` | agrees in area |
| 0x68d010 | List node free in the list destructor (holiday inventory) | `fastcall_free_wrapper` (Haiku) | `LLIST_NODE::~LLIST_NODE` | agrees |
| 0x77b850 | Transfer regulations information screen (sweep-season-end) | `transfer_rules_info_screen_show` (Haiku) | `transfer_rules_info_screen_display` | agrees |
| 0x77bb30 | Command dispatch of that screen (sweep-season-end) | `transfer_rule_dispatch` (Haiku) | `transfer_rules_info_screen_handle_click` | agrees |
| 0x7a30c0 | Four calls with flags at game creation (game-creation inventory) | `player_setup_cfg_process` (Haiku) | `player_setup_cfg_load` | agrees |
| 0x7a5380 | Setup step 36, global 0xb7434c (game-creation inventory) | `player_regen_load_from_cd` (Haiku) | `PLAYER_REGEN::PLAYER_REGEN` | agrees |

#### Wave w09 (tier 3, 2026-10-16)

Run `sweep-w09`: 8 Opus agents named 201 functions: `ruling_body.cpp` with the reserve competition gap functions, `sub_league.cpp` with staff screen dialogs, and six batches of `transfer_manager.cpp` with training screen comparators. Each agent took 2 to 6 minutes and 101,000 to 213,000 tokens. Confidence: 118 high, 82 medium, 1 low. The merge needed no fixes. `record_utils.cpp` holds three competition classes from the gap, each with its own vtable: `ReserveComp`, `BIntComp` and `Under21IntComp`.

25 functions of the wave are cited. 16 agree, 4 agree in area and 5 citations make no claim about the purpose. The rulebooks research calls ruling body slots 0 and 1 "process pending entries" and "process pending rulings". The sweep names them `start_new_season` and `daily_update` from their callers.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x7e5fc0 | Setup step 27, global 0xb74434 (game-creation inventory) | `cd_check_find_cm_drive` (Haiku) | `ReserveComp::ReserveComp` | agrees |
| 0x7e61c0 | Setup step 30 (game-creation inventory) | `record_utils_init_comp_records` (Haiku) | `ReserveComp::create_sub_comps` | agrees in area |
| 0x7e7d50 | Base slot +0x4, process pending rulings (sweep-rulebooks) | `ruling_body_process_pending_rulings` (Haiku) | `RulingBody::daily_update` | agrees in area |
| 0x7e7ed0 | Base slot +0x0, process pending entries (sweep-rulebooks) | `ruling_body_process_pending_entries` (Haiku) | `RulingBody::start_new_season` | agrees in area |
| 0x7e85b0 | Reader of the ruling body fields (sweep-rulebooks) | `ruling_body_rule_check` (Haiku) | `RulingBody::is_player_banned` | agrees |
| 0x88bc50 | Table of 0x65-byte cells at 0xddf570 (attribute-groups) | `forward_to_utils_string_compare_flagged` (Haiku) | `staff_screens_compare_text_rows` | agrees |
| 0x88c440 | None or one player set (comparison-screen inventory) | `staff_screens_open_compare_players_screen` (Haiku) | `staff_screens_open_compare_players_dialog` | agrees |
| 0x88c460 | Compare screen open path | `sub_league_compare_players_dialog` (Haiku) | `staff_screens_build_compare_players_dialog` | agrees |
| 0x88c660 | Compare screen open path | `get_player_team_or_reject_id` (Haiku) | `staff_screens_handle_compare_players_action` | agrees |
| 0x8bba30 | Setup step 34, global 0xde1f54 (game-creation inventory) | `TRANSFER_MANAGER::TRANSFER_MANAGER` (Haiku) | `TRANSFER_MANAGER::TRANSFER_MANAGER` | agrees |
| 0x8c1870 | Listed near the On Loan widget only (comparison-screen inventory) | `transfer_manager_find_staff_match` (Haiku) | `TRANSFER_MANAGER::is_queued_for_free_transfer` | no claim |
| 0x8c39f0 | Adds 1 to the counter of a bsearch row (crash-fixes) | `transfer_manager_process_offers_for_club` (Haiku) | `TRANSFER_MANAGER::update_appearance_clauses_after_match` | agrees |
| 0x8c6bc0 | Transfer window code (sweep-season-end) | `transfer_manager_check_offer_deadlines` (Haiku) | `TRANSFER_MANAGER::update_transfer_window_news` | agrees |
| 0x8c7070 | Adds a work permit (sweep-season-end) | `transfer_manager_add_work_permit` (Haiku) | `TRANSFER_MANAGER::add_work_permit` | agrees |
| 0x8c9390 | Moves part-exchange players (crash-fixes) | `transfer_manager_process_completed_transfer` (Haiku) | `TRANSFER_MANAGER::finalise_transfer` | agrees |
| 0x8cd450 | Squad limit check (sweep-season-end) | `transfer_manager_check_squad_limits` (Haiku) | `transfer_manager_check_squad_limits` | agrees |
| 0x8cdb80 | In a call chain to srand only (assistant-and-days inventory) | `transfer_manager_check_offer_flags` (Haiku) | `transfer_manager_check_manager_veto` | no claim |
| 0x8cf470 | Steps forward to the next open window date (sweep-season-end) | `transfer_manager_find_open_window_date` (Haiku) | `TRANSFER_MANAGER::find_next_window_open_date` | agrees |
| 0x8cf5e0 | Mentioned in a call graph note only (assistant-pick inventory) | `transfer_manager_dispatch_offer` (Haiku) | `TRANSFER_MANAGER::get_ineligibility_code` | no claim |
| 0x8cf6b0 | Lookup of the rules object, slot +0x1c (foreign-and-tactics) | `transfer_manager_fill_party_slots` (Haiku) | `TRANSFER_MANAGER::fill_squad_restrictions` | agrees |
| 0x8cf7e0 | Reader of the squad restriction bytes (foreign-and-tactics) | `transfer_manager_build_selection_result` (Haiku) | `transfer_manager_get_selection_limit` | agrees |
| 0x8cf880 | Applies competition rules to a fixture (assistant-pick inventory) | `squad_registration_rule_check` (Haiku) | `transfer_manager_check_matchday_squad_limit` | agrees in area |
| 0x8cfc90 | In a call chain to srand only (assistant-and-days inventory) | `transfer_manager_check_offer_acceptance` (Haiku) | `TRANSFER_MANAGER::check_offer_restrictions` | no claim |
| 0x8d26b0 | Per-nation rules objects (season-change-points) | `TRANSFER_MANAGER::setup_transfer_rules` (Haiku) | `TRANSFER_MANAGER::setup_transfer_rules` | agrees |
| 0x8d2fa0 | Rules lookup by fixture (foreign-and-tactics) | `transfer_manager_resolve_party_kind` (Haiku) | `TRANSFER_MANAGER::get_fixture_rules_scope` | agrees |

#### Wave w10 (tier 3 done, tier 4 begins, 2026-10-16)

Run `sweep-w10`: 8 Opus agents named 204 functions: the rest of `transfer_manager.cpp`, `transfer_offer.cpp`, `transfer_screens.cpp`, and the first tier 4 files `arg_prm.cpp`, `arg_second.cpp`, `argentina_awards.cpp`, `aus_nsl.cpp` and `australia_awards.cpp`. Each agent took 2 to 5 minutes and 132,000 to 191,000 tokens. Confidence: 175 high, 29 medium, 0 low. Tier 3 is done.

Before this wave the awards base `NationAwards` (vtable 0x96da10) joined the slot check: 0 `init_awards`, 1 `daily_update`, 3 `on_month_change`, 4 `update_team_of_the_week`. The base 0x768310 and `WorldCupAwards` slot 1 changed from `check_month_change` to `daily_update`. Comp slot 13 is `update_relegation_averages`. `transfer_offer.cpp` keeps two classes: the `TRANSFER_RULES` base methods sit in this file next to `TRANSFER_OFFER`.

26 functions of the wave are cited. 9 agree, 4 agree in area and 13 citations only list the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x403ee0 | Listed as a callee only (display inventory) | `area_scroll_to_position` (Haiku) | `Area::scroll_rows` | no claim |
| 0x4041d0 | Listed as a callee only (display inventory) | `Area::set_colour_masks` (Haiku) | `Area::convert_colour_to_pixel_format` | no claim |
| 0x4093f0 | Average points, Argentine second division (crash-fixes) | `arg_second_build_club_name_table` (Haiku) | `ArgSecond::init_relegation_averages` | agrees |
| 0x412680 | Listed by a constant scan only (widescreen inventory) | `AustraliaAwards::init_award_objects` (Haiku) | `AustraliaAwards::init_awards` | no claim |
| 0x8e5590 | Mentioned near the rules objects only (sweep-season-end) | `compare_transfer_sort_keys` (Haiku) | `transfer_manager_compare_window_dates` | no claim |
| 0x8e55c0 | Work permit code (sweep-season-end) | `compare_optional_int_keys` (Haiku) | `transfer_manager_compare_work_permit_entries` | agrees in area |
| 0x8e7b50 | Reads player data of a staff in an offer (crash-fixes) | `transfer_manager_offer_evaluate_player` (Haiku) | `TRANSFER_OFFER::get_weighted_value` | agrees in area |
| 0x8e8560 | Base rules slot +0x10, squad limits (sweep-season-end, rulebooks) | `transfer_offer_validate_squad_limits` (Haiku) | `TRANSFER_RULES::can_sign_player` | agrees in area. The research uses the Haiku name |
| 0x8e8800 | Base rules slot +0xc, squad limits (sweep-season-end, crash-fixes) | `transfer_offer_check_squad_limits` (Haiku) | `TRANSFER_RULES::can_make_offer` | agrees in area. The research uses the Haiku name |
| 0x8e8a60 | Base rules slot +0x18, eligibility code (sweep-season-end) | `transfer_offer_eligibility_code` (Haiku) | `TRANSFER_RULES::get_ineligibility_code` | agrees |
| 0x8e8bd0 | Mentioned in the England loan rules only (sweep-rulebooks) | `contract_rollover_compute_month_year` (Haiku) | `TRANSFER_RULES::get_loan_period` | no claim |
| 0x8e8c40 | Window record of two 6-byte bounds (sweep-rulebooks) | `date_window_contains_offer_date` (Haiku) | `transfer_window_contains_date` | agrees |
| 0x8e8ed0 | Mentioned near the window search only (sweep-season-end) | `transfer_offer_deadline_check` (Haiku) | `TRANSFER_RULES::get_days_until_window_closes` | no claim |
| 0x8e9080 | Base rules slot, deadline news (sweep-season-end) | `transfer_offer_news_message` (Haiku) | `TRANSFER_RULES::post_deadline_news` | agrees |
| 0x8e91b0 | Base rules slot, deadline news text (sweep-season-end) | `transfer_deadline_news_post` (Haiku) | `TRANSFER_RULES::write_deadline_news_text` | agrees |
| 0x8e9760 | Listed as a link site only (clickable-names inventory) | `transfer_screens_build_bid_panel` (Haiku) | `transfer_screens_build_bid_screen` | no claim |
| 0x8f24c0 | Listed as a link site only (clickable-names inventory) | `transfer_screens_show_bid_response` (Haiku) | `transfer_screens_build_offer_changed_screen` | no claim |
| 0x8f4160 | Listed in a set of tested ids only (comparison-screen inventory) | `transfer_screens_open_comparison_screen` (Haiku) | `transfer_screens_open_shortlist_dialog` | no claim |
| 0x8f41b0 | The comparison prompt screen (comparison-screen inventory) | `transfer_screens_open_comparison_screen` (Haiku) | `transfer_screens_open_comparison_prompt` | agrees |
| 0x8f4210 | Build of the prompt screen | `transfer_screens_compare_prompt_dialog` (Haiku) | `transfer_screens_build_comparison_prompt` | agrees |
| 0x8f44d0 | Input of the prompt screen | `transfer_screens_dispatch_by_status` (Haiku) | `transfer_screens_handle_comparison_prompt_action` | agrees |
| 0x8f5080 | Listed as a link site only (clickable-names inventory) | `Transfers::build_transfers_screen` (Haiku) | `transfer_screens_build_transfers_screen` | no claim |
| 0x8f6a10 | Listed as a link site only (clickable-names inventory) | `transfer_screens_set_staff_transfer_status_dialog` (Haiku) | `transfer_screens_build_transfer_status_screen` | no claim |
| 0x8f7640 | Input of Set Staff Transfer Status (slow-screens) | `transfer_screens_handle_action` (Haiku) | `transfer_screens_handle_transfer_status_screen` | agrees |
| 0x8f9a20 | Listed as a link site only (clickable-names inventory) | `transfer_screens_loan_bid_screen` (Haiku) | `transfer_screens_build_loan_bid_screen` | no claim |
| 0x8fbcc0 | Listed as a link site only (clickable-names inventory) | `transfer_screens_loan_recall_guard` (Haiku) | `transfer_screens_open_end_loan_dialog` | no claim |

#### Wave w11 (tier 4, 2026-10-16)

Run `sweep-w11`: 8 Opus agents named 215 functions of the Belgian competitions (`bel_*.cpp`, `belgium_awards.cpp`, with background picture gap functions) and the first Brazilian files (`bra_champ_cup.cpp` to `bra_reg_minas_gerais.cpp`). Each agent took 2 to 4 minutes and 123,000 to 160,000 tokens. Confidence: 192 high, 23 medium, 0 low. The merge needed no fixes. Several agents report that the Setup function 0x832980, named `create_norwegian_comps`, makes the Brazilian competitions. Its error string says Norwegian, so the source has a copy slip. The Setup.cpp sweep in tier 5 decides its name.

5 functions of the wave are cited. 4 agree and 1 citation only lists the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x41c9d0 | Picture lookup by club +0x53, the nation (team-views inventory) | `background_lookup_entry_value` (Haiku) | `background_find_nation_picture` | agrees |
| 0x41cad0 | Club picture lookup (team-views inventory) | `background_lookup_entry_pointer` (Haiku) | `background_find_club_picture` | agrees |
| 0x41cbd0 | Picture lookup by club +0x57, the division (team-views inventory) | `background_pick_entry_picture` (Haiku) | `background_find_comp_picture` | agrees |
| 0x41cdd0 | Picture lookup by the stadium (team-views inventory) | `background_lookup_entry_value` (Haiku) | `background_find_stadium_picture` | agrees |
| 0x42b130 | Listed by a constant scan only (widescreen inventory) | `bra_nat_first_assign_national_squad_players` (Haiku) | `bra_nat_first_exchange_state_league_clubs` | no claim |

### Hub sweep

The hubs are the game and library functions with 20 or more callers: 228 functions (224 game, 4 library) that take 71% of all calls. The league and cup sweeps already named 27 of them. Eight Opus agents on the subscription named the other 201 on 2026-10-16 (run `sweep-hubs`), with about 25 functions each. The dossiers showed the Haiku names and the league and cup sweep names. Each agent took 5 to 8 minutes and 155,000 to 194,000 tokens, about 1.4 million tokens in all. Confidence: 116 high, 81 medium, 4 low.

The agents changed almost every name. Examples: 0x417c10 is `Date::operator=` (Haiku: a shortlist copy), 0x77bbe0 is `NewsItem::NewsItem` with 211 callers (Haiku: a transfer rule constructor), 0x66f4e0 is `langlib_sprintf` with about 770 callers, and 0x68d020 to 0x68d1f0 are one linked list class. One hub is a proven function: 0x556940 (stores a ban) is `Discipline::add_ban` (high), and it agrees.

### Second Haiku run

The second Haiku run (`haiku-2`) named all 6,863 functions again on 2026-10-16. Its dossiers showed the hub, league and cup sweep names and the slot copies over the first Haiku names.

- All 6,863 requests succeeded, none refused.
- Cost $1.55 at batch prices. Wall time 13 minutes. Tokens: 35.3 million input (14.3 million cache reads), 1.6 million output.
- Confidence: 2,021 high, 4,410 medium, 432 low. The first run had 743 high, 4,254 medium, 1,866 low.
- 4,745 names (69%) are the same as in the first run.
- Check against the 14 proven functions: 8 agree, 1 in part, 5 wrong, the same result as the first run. 0x5554a0 moves into the right area (`discipline_cd_check_and_init`, high) but is still not a constructor. The other 13 names keep their result, and 11 of them keep the same name.

Better callee names did not fix the wrong names of the 14 checks. The confidence rose much more than the check result, so a high Haiku name is still only a hint. In the docs, the hub sweep name wins for 0x556940, so the docs have 9 of the 14 right.

### Slot copy

`tools/research/function_map/slot_copy.py` names the overrides of the per-nation competition classes (run `slot-copy`). A vtable derives from League (0x96d020) or Cup (0x9693c8) when it shares at least 5 of the base's own league.cpp or Cup.cpp slots. Each slot that holds another function gets `<file stem>::<base method>`, for example `eng_prm::create_schedule`, with model "slot copy" and confidence medium. It gave 875 names in under a second. It left out 6 functions that sit in slots of two different methods and 1 function that a sweep names. The most common methods are the scalar deleting destructor (179), `create_schedule` (115), `start_new_season` (112) and `set_up_rules` (109).

The docs use the runs in this order, a later run wins: haiku-2026-10-16, haiku-2, slot-copy, sweep-league, sweep-cup, sweep-hubs. Models in `names.tsv`: 314 Opus, 875 slot copy, 5,674 Haiku. Confidence: 1,777 high, 4,698 medium, 388 low.

Max chose the Haiku route on 2026-10-16: Haiku names every function once, and sweeps with Opus on the subscription improve one file at a time later. The docs are in `function-map/`.

- 6,863 game and library functions, one batch of 6,863 requests. All succeeded, none refused.
- Cost $1.56 at batch prices. Wall time 12 minutes. Tokens: 34.6 million input (13.5 million cache reads), 1.6 million output.
- Confidence: 743 high, 4,254 medium, 1,866 low.
- Check against the 14 functions that earlier research proves: 8 agree, 1 agrees in part, 5 are wrong.

| Address | Earlier research | Haiku name (confidence) | Result |
|---|---|---|---|
| 0x5554a0 | Discipline object constructor | `copy_protection_check_and_init` (high) | wrong |
| 0x556240 | Daily update of the rule sets | `discipline_array_call_vtable_slot_1` (medium) | in part: the mechanism, not the purpose |
| 0x556940 | Stores a ban | `discipline_record_ban_entry` (medium) | agrees |
| 0x556d00 | Releases pending bans | `discipline_slot_release_entry` (medium) | agrees |
| 0x55d270 | Player banned for a competition | `discipline_check_player_suspended_for_match` (low) | agrees |
| 0x55e300 | Long ban after a referee assault | `discipline_check_player_suspension` (low) | wrong |
| 0x4542f0 | Opens the team screen | `club_screens_populate_fixture_dialog` (medium) | wrong |
| 0x454900 | Club screen build | `club_screens_dispatch_main_view` (medium) | agrees |
| 0x456900 | Squad list builder | `club_view_squad_list` (medium) | agrees |
| 0x45cbe0 | Player available for the next match | `can_player_transfer_or_loan_check` (medium) | wrong |
| 0x45ccf0 | Next Match tab | `club_screens_fixture_detail_view` (medium) | agrees |
| 0x469da0 | Leave function, frees slot buffers | `club_screens_release_screen_handles` (medium) | agrees |
| 0x481ef0 | Team selector (Main, B, Reserve) | `club_squad_list_menu_build` (medium) | agrees |
| 0x4849f0 | Player status code | `validate_transfer_or_loan_offer` (medium) | wrong |

Read a first-pass name as a hint of the area, not as a fact. Three of the five wrong names have medium confidence and one has high. A sweep with Opus fixes a file when a feature needs it.

## Recommendation

Run the full map on the fast route. It costs about $120 and takes an afternoon. Every later feature and research doc then starts from a name for each function.

1. Commit the step 1 file table and the dossier and batch scripts under `tools/research/` (addresses, file names and model output only, no code of the original).
2. Run wave 1 as one batch, then wave 2 on the low names with the names of the whole game.
3. Write one doc per source file under `docs/research/function-map/`, and check a sample of 20 names against earlier research before use.
