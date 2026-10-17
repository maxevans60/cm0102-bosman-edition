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

#### Wave w12 (tier 4, 2026-10-16)

Run `sweep-w12`: 8 Opus agents named 201 functions: the rest of the Brazilian state leagues, `brazil_awards.cpp`, `con_merc_cup.cpp`, the Croatian files with the credits and web sites screens from the `contract_screens.cpp` gap, and the Danish files. Each agent took 1 to 5 minutes and 99,000 to 171,000 tokens. Confidence: 189 high, 12 medium, 0 low. The merge needed no fixes. `denmark_awards.cpp` holds the `DENMARK_RULES` constructor from the gap before `discipline.cpp`.

9 functions of the wave are cited. 6 agree, 1 agrees in area and 2 citations only list the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x4c16f0 | Merconorte Cup constructor (club-world) | `Merc_cup::Merc_cup` (Haiku) | `ConMercCup::ConMercCup` | agrees |
| 0x4c1960 | Merconorte Cup, 3 groups, 16 clubs (sweep-europe) | `con_merc_cup::create_schedule` (slot copy) | `ConMercCup::create_schedule` | agrees in area. Opus finds four groups of four, the research says three groups. Not checked |
| 0x4c2070 | Not a function start (sweep-europe) | `con_merc_cup_assert_check_0x4c2070` (Haiku) | `con_merc_cup_start_new_season_error_tail` | agrees |
| 0x4c2560 | Takes 16 clubs from five nations (sweep-europe) | `con_merc_cup_fill_team_slots` (Haiku) | `ConMercCup::select_invited_clubs` | agrees |
| 0x4e5920 | Opens the credits (screen-stack inventory) | `contract_screens_open_credits_screen` (Haiku) | `credits_screen_open` | agrees |
| 0x5133c0 | Listed as a user of an injected action id only | `contract_screen_select_or_dispatch` (Haiku) | `credits_screen_handle_action` | no claim |
| 0x5134c0 | Opens the web sites screen (screen-stack inventory) | `credits_screen_open` (Haiku) | `web_sites_screen_open` | agrees |
| 0x517fa0 | Listed by a constant scan only (widescreen inventory) | `Croatia_awards::init_awards` (Haiku) | `CroatiaAwards::init_awards` | no claim |
| 0x555460 | Denmark rules constructor with transfer windows (sweep-rulebooks) | `transfer_rules_ctor_gap_0x555460` (Haiku) | `DENMARK_RULES::DENMARK_RULES` | agrees |

#### Wave w13 (tier 4, 2026-10-16)

Run `sweep-w13`: 8 Opus agents named 204 functions: the English competitions (`eng_*.cpp`, `england_awards.cpp`) with the `DISPUTE` gap functions, `european_awards.cpp`, `fin_cup.cpp`, `fin_first.cpp` and `fin_prm.cpp` with the finance code from the gap before `find_screens.cpp`. Each agent took 2 to 6 minutes and 107,000 to 180,000 tokens. Confidence: 172 high, 32 medium, 0 low.

Merge fixes: the finance gap holds two classes from a lost `finance.cpp`. `Finance` is the club finance record (0x167 bytes), as the hub sweep named it. `FinanceTable` is the manager at global 0xae2a38. One agent had called the manager `Finance`, so 20 of its rows and the hub row 0x5a0590 changed to `FinanceTable`. The empty body 0x583470 sits in about 20 vtable slots and is now `shared_noop_ret4`.

25 functions of the wave are cited. 21 agree and 4 agree in area.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x56aa30 | Competition constructor in the game creation table (game-creation inventory) | `cup_eng_auto_cup_init` (Haiku) | `EngAutoCup::EngAutoCup` | agrees |
| 0x56c2a0 | Competition constructor in the game creation table (game-creation inventory) | `Cup::Cup_from_disk_init` (Haiku) | `EngCcCup::EngCcCup` | agrees |
| 0x56d380 | Competition constructor in the game creation table (game-creation inventory) | `Charity_event::ctor` (Haiku) | `EngCharity::EngCharity` | agrees |
| 0x56db60 | Competition constructor in the game creation table (game-creation inventory) | `eng_conf_engine_init` (Haiku) | `EngConf::EngConf` | agrees |
| 0x56f040 | Competition constructor in the game creation table (game-creation inventory) | `FA_Cup::FA_Cup` (Haiku) | `EngFaCup::EngFaCup` | agrees |
| 0x56fab0 | FA Cup replay rules (season-change-points) | `fa_cup_build_round_list` (Haiku) | `EngFaCup::collect_entrants` | agrees in area |
| 0x570c00 | Competition constructor in the game creation table (game-creation inventory) | `Trophy::Trophy` (Haiku) | `EngFaTrophy::EngFaTrophy` | agrees |
| 0x571680 | Competition constructor in the game creation table (game-creation inventory) | `Engine_Init::construct_engine_first` (Haiku) | `EngFirst::EngFirst` | agrees |
| 0x573380 | Competition constructor in the game creation table (game-creation inventory) | `eng_prm_init_object` (Haiku) | `EngPrm::EngPrm` | agrees |
| 0x574b70 | English Premier Division rules (sweep-rulebooks) | `eng_prm::set_up_rules` (slot copy) | `EngPrm::set_up_rules` | agrees |
| 0x574c10 | start_new_season of a top league (sweep-season-end) | `eng_prm::start_new_season` (slot copy) | `EngPrm::start_new_season` | agrees |
| 0x574e60 | Named in the start_new_season step (sweep-season-end) | `eng_prm_assign_players_to_team_slot` (Haiku) | `eng_prm_exchange_conference_clubs_unplayed` | agrees in area |
| 0x5750a0 | Named in the start_new_season step (sweep-season-end) | `eng_prm_assign_top_teams` (Haiku) | `eng_prm_exchange_conference_feeder_clubs` | agrees in area |
| 0x5752e0 | Promotion and relegation chain of five leagues (sweep-season-end) | `eng_prm::process_promotion_relegation_chain` (slot copy) | `EngPrm::process_promotion_relegation_chain` | agrees |
| 0x5754a0 | Competition constructor in the game creation table (game-creation inventory) | `cm_object_init` (Haiku) | `EngSecond::EngSecond` | agrees |
| 0x576dd0 | Competition constructor in the game creation table (game-creation inventory) | `eng_third_tournament_ctor` (Haiku) | `EngThird::EngThird` | agrees |
| 0x591b70 | Build of the confirm screen (start-screen helpers) | `show_yes_no_dialog_screen` (Haiku) | `confirm_build` | agrees |
| 0x591dc0 | Input of the confirm screen (start-screen helpers) | `fin_cup_key_dispatch` (Haiku) | `confirm_input` | agrees |
| 0x59a370 | Setup step 31, global 0xae2a38 (game-creation inventory) | `fin_prm_load_from_cd` (Haiku) | `FinanceTable::FinanceTable` | agrees |
| 0x59b9c0 | Daily call (game-creation inventory) | `fin_prm_apply_club_finance_events` (Haiku) | `FinanceTable::daily_update` | agrees |
| 0x59cdd0 | Calls the season-end stadium routine (stadium research) | `fin_prm_copy_club_records_by_group` (Haiku) | `FinanceTable::end_season_for_nation` | agrees |
| 0x59cf50 | Setup step 33 (game-creation inventory) | `fin_prm_refresh_records_loop` (Haiku) | `finance_refresh_all_wage_bills` | agrees in area |
| 0x59cfa0 | Season-end stadium routine (stadium research) | `fin_prm_club_weekly_finance_tick` (Haiku) | `FinanceTable::season_end_club_update` | agrees |
| 0x59fae0 | Expansion plans shelved news 0x1788 (stadium research) | `news_post_club_event` (Haiku) | `finance_post_works_cancelled_news` | agrees |
| 0x5a0050 | Text of news 0x177d, stadium works (stadium research) | `fin_prm_news_headline_text` (Haiku) | `FinanceTable::write_stadium_works_news_text` | agrees |

#### Wave w14 (tier 4, 2026-10-16)

Run `sweep-w14`: 8 Opus agents named 216 functions: `finland_awards.cpp` with the `Fine` gap functions, `fog_of_war.cpp` with the T2K font code (`FONT`), the French, German and Greek competitions, and `game_config.cpp` font settings. Each agent took 2 to 6 minutes and 130,000 to 166,000 tokens. Confidence: 193 high, 23 medium, 0 low.

Merge fixes: awards slot 2 is now a fixed name, `record_match_ratings`. The return-false body 0x5dce00 that Comp slots 30 and 34 share is now `shared_return_false`. `fra_first.cpp` holds two small placeholder Comp classes for the CFA and the lower division, each with its own vtable.

10 functions of the wave are cited. 5 agree, 2 agree in area and 3 citations make no claim about the purpose.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x5a83e0 | Build of Discipline Player (slow-screens) | `show_discipline_dialog` (Haiku) | `draw_discipline_player_screen` | agrees |
| 0x5a8990 | Input of Discipline Player (slow-screens) | `fine_dialog_handler` (Haiku) | `discipline_player_screen_input` | agrees |
| 0x5a90b0 | Listed by a constant scan only (widescreen inventory) | `finland_awards::construct_awards` (Haiku) | `FinlandAwards::init_awards` | no claim |
| 0x5afd70 | Mentioned in a screen action table only (game-creation inventory) | `fog_of_war_slot_alloc_thiscall_wrapper` (Haiku) | `FOG_OF_WAR::add_manager_map` | no claim |
| 0x5b0b60 | Loads the T2K font files (hd-text) | `font_load_by_size_index` (Haiku) | `FONT::load_font_file` | agrees |
| 0x5b0d80 | T2K draw, called from the line draw (hd-text) | `blit_glyph_string_alpha` (Haiku) | `FONT::draw_text` | agrees |
| 0x5b18a0 | Receives the sizes from the text width (hd-text) | `formation_sum_score` (Haiku) | `FONT::get_text_width` | agrees |
| 0x5b1a00 | Mentioned in a memory layout note only (hd-text) | `fog_of_war_update_overlay` (Haiku) | `FONT::set_style_and_size` | no claim |
| 0x5b2170 | Fonts row of the Game Settings box (hd-text) | `fog_of_war_load_fog_dat_entries` (Haiku) | `font_scan_folder` | agrees in area |
| 0x5d8b50 | Sets a drop-down value on the options screen (options-screen) | `config_set_name_field` (Haiku) | `Config::set_font` | agrees in area |

#### Wave w15 (tier 4, 2026-10-16)

Run `sweep-w15`: 8 Opus agents named 196 functions: the Dutch competitions with the `Guio` widget and written history gap functions, the Irish competitions, `international_awards.cpp`, and the Italian cups `ita_c1_super.cpp`, `ita_c_cup.cpp` and `ita_cup.cpp`. Each agent took 2 to 4 minutes and 122,000 to 165,000 tokens. Confidence: 185 high, 11 medium, 0 low. The merge needed no fixes.

10 functions of the wave are cited. 5 agree, 3 agree in area, 1 citation makes no claim and 1 differs. The 0.6.0 research `hol-bel.md` says the original builds two play-off groups of eight. It took that from the Haiku name of 0x5f6720. The code builds two groups of four. That doc needs a fix.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x5ed300 | Menu builder (screen-stack inventory) | `gui_utils_build_button_grid_or_assert` (Haiku) | `gui_add_menu_buttons` | agrees |
| 0x5ed780 | Menu with headers, buttons with flags 0x4002 (start-screen helpers) | `gui_utils_paginated_list_layout` (Haiku) | `gui_add_tab_bar` | agrees in area |
| 0x5ee150 | Widget init, called by widget create (display inventory) | `gui_item_init` (Haiku) | `Guio::init` | agrees |
| 0x5ee4a0 | Called by the widget serialiser (display inventory) | `gui_object_unpack_fields` (Haiku) | `Guio::get_fields` | agrees in area |
| 0x5ee590 | Called by the screen draw (display inventory) | `button_draw` (Haiku) | `Guio::draw` | agrees in area |
| 0x5eec20 | Hover highlight in the hit test (display inventory) | `gui_widget_refresh_state` (Haiku) | `Guio::highlight_on` | agrees |
| 0x5eecb0 | Hover highlight in the hit test (display inventory) | `gui_element_reset_and_redraw` (Haiku) | `Guio::highlight_off` | agrees |
| 0x5f6720 | Two play-off groups of eight (hol-bel), from the Haiku name | `hol_first_build_cup_stage_fixtures` (Haiku) | `HolFirst::create_play_off_groups` | differs. The code builds two groups of four, the real format to 2004/05. The research took its group size from the wrong Haiku name |
| 0x5f73d0 | Crash guard site only (crash-fixes) | `hol_set_team_form_bands` (Haiku) | `HolFirst::set_club_progress` | no claim |
| 0x63a740 | Sets the table size to ten or twelve (wal-nir-irl) | `ire_prm_init_league_table` (Haiku) | `IrePrm::create_table` | agrees |

#### Wave w16 (tier 4, 2026-10-16)

Run `sweep-w16`: 8 Opus agents named 218 functions: the Italian leagues (`ita_ser_*.cpp`, `ita_super.cpp`, `italy_awards.cpp`), the Japanese and Korean competitions, `korea_awards.cpp`, `nation_awards.cpp` with month ratings and news screen gap functions, and `nir_charity.cpp`. Each agent took 2 to 4 minutes and 128,000 to 158,000 tokens. Confidence: 192 high, 26 medium, 0 low. The merge needed no fixes. One more Setup function has another country's name: 0x837200 (`create_spanish_comps`) builds the Korean competitions.

12 functions of the wave are cited. All 12 agree.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x641700 | start_new_season order, promotion then year + 1 (sweep-season-end) | `ita_ser_a::start_new_season` (slot copy) | `ItaSerA::start_new_season` | agrees |
| 0x642a70 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_a_build_fixture_table` (Haiku) | `ItaSerA::create_fixed_fixtures` | agrees |
| 0x647230 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `Table::build_serie_b_fixture_schedule` (Haiku) | `ItaSerB::set_up_fixed_fixtures` | agrees |
| 0x64be90 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_c1a_build_fixture_table` (Haiku) | `ItaSerC1a::set_up_fixed_fixtures` | agrees |
| 0x650100 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_c1b_build_league_fixtures` (Haiku) | `ItaSerC1b::set_up_fixed_fixtures` | agrees |
| 0x6545c0 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_c2a_build_fixture_table` (Haiku) | `ItaSerC2a::set_up_fixed_fixtures` | agrees |
| 0x656570 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_c2a_seed_league_teams` (Haiku) | `ita_ser_c2a_set_up_fixed_fixtures_tail` | agrees |
| 0x658750 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_c2b_load_fixtures` (Haiku) | `ItaSerC2b::load_fixed_fixtures` | agrees |
| 0x65c8e0 | Fixed 2001 fixtures of the Italian leagues (sweep-season-end) | `ita_ser_c2c_build_fixture_table` (Haiku) | `ItaSerC2c::load_fixed_fixtures` | agrees |
| 0x7881e0 | Build of the news screen (clickable-names inventory) | `news_screens_show_news_list` (Haiku) | `news_screens_build_news_screen` | agrees |
| 0x7892b0 | Input of the news screen (clickable-names inventory) | `news_screens_dispatch_selected_message` (Haiku) | `news_screens_news_input` | agrees |
| 0x7897a0 | Leave function of the news screen (clickable-names inventory) | `scrman_close_screen_slot_2_if_open` (Haiku) | `news_screens_news_exit` | agrees |

#### Wave w17 (tier 4, 2026-10-16)

Run `sweep-w17`: 8 Opus agents named 216 functions: the Northern Irish, Norwegian, Polish and Portuguese competitions with their awards, the four `old_*_awards.cpp` classes, the `OfficialsManager` stats gap functions, `Plot`, and `rus_cup.cpp`. Each agent took 2 to 4 minutes and 127,000 to 165,000 tokens. Confidence: 201 high, 15 medium, 0 low. The merge needed no fixes.

2 functions of the wave are cited. 1 agrees in area and 1 citation only lists the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x78ac90 | Fills up to 30 top clubs (wal-nir-irl) | `nir_cup_build_order_table` (Haiku) | `NirCup::collect_entrants` | agrees in area. Opus finds 20 top clubs and 12 lower clubs, 32 entries. Not checked |
| 0x79d680 | Listed by a constant scan only (widescreen inventory) | `old_finland_awards_init_award_objects` (Haiku) | `OldFinlandAwards::init_awards` | no claim |

#### Wave w18 (tier 4, 2026-10-16)

Run `sweep-w18`: 8 Opus agents named 195 functions: the Russian, Scottish, Spanish and Swedish competitions with their awards, and the printer and `SoundPlayer` gap functions in `simulated_stats.cpp`. Each agent took 2 to 4 minutes and 102,000 to 158,000 tokens. Confidence: 179 high, 16 medium, 0 low. One agent wrote the evidence and confidence values in each other's fields. The merge check caught it, and the 31 rows were swapped back.

12 functions of the wave are cited. 8 agree and 4 agree in area. For 0x84cbc0 the code takes the arguments as pan, path and loop. The import inventory gives the path first.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x84c7f0 | Printer check before the print function (import inventory) | `printer_installed_check` (Haiku) | `printer_is_installed` | agrees |
| 0x84c830 | Prints text lines in Courier, all GDI32 imports (import inventory) | `print_text_lines` (Haiku) | `print_text_lines` | agrees |
| 0x84ca80 | Sound object constructor, DirectSoundCreate (columns inventory) | `init_directsound_device_wrapper` (Haiku) | `SoundPlayer::SoundPlayer` | agrees |
| 0x84caf0 | Releases the sound object (import inventory) | `release_owned_object_field_0x28` (Haiku) | `SoundPlayer::~SoundPlayer` | agrees |
| 0x84cb10 | Sound call from scrman (display inventory, import inventory) | `sound_slots_release_all` (Haiku) | `SoundPlayer::stop_all` | agrees in area |
| 0x84cb70 | Sound call from scrman (display inventory, import inventory) | `slot_release_thiscall` (Haiku) | `SoundPlayer::stop_slot` | agrees in area |
| 0x84cbc0 | Play a sound file, path first (import inventory) | `sound_play_slot` (Haiku) | `SoundPlayer::play_file` | agrees. The argument order differs: the code takes pan, path, loop, and the import inventory gives path first |
| 0x84cc80 | Sound query from scrman (display inventory) | `array_slot_call_vfunc_0x24` (Haiku) | `SoundPlayer::is_slot_playing` | agrees in area |
| 0x84ccc0 | Sound volume (display inventory, import inventory) | `set_slot_volume_percent` (Haiku) | `SoundPlayer::set_slot_volume` | agrees |
| 0x84cd10 | One sound buffer for each play (columns inventory) | `load_wave_sound_buffer` (Haiku) | `SoundPlayer::load_wav` | agrees |
| 0x84cf00 | Sound slot search (display inventory) | `find_ready_slot_handler` (Haiku) | `SoundPlayer::find_free_slot` | agrees in area |
| 0x858e70 | Shared set_up_rounds when no prize fund is set (sweep-europe) | `spa_super::set_up_rounds` (slot copy) | `SpaSuper::set_up_rounds` | agrees |

#### Wave w19 (tier 4 done, tier 5 begins, 2026-10-16)

Run `sweep-w19`: 8 Opus agents named 223 functions: the rest of tier 4 (Swedish, Turkish, US and Welsh competitions with their awards, `SWEDEN_RULES`, the weather and ultimatum gap functions) and the first tier 5 files, `Area.cpp` and part of `Database.cpp`. Each agent took 2 to 4 minutes and 131,000 to 181,000 tokens. Confidence: 191 high, 32 medium, 0 low. Tier 4 is done. Several agents report that the Setup function 0x8383b0 (`create_spanish_comps`) also makes the Turkish, US and Welsh competitions.

Fixes from the research name check (`2026-10-16-research-name-check.md`): `goldcup.cpp` runs the Copa America. The constructor gets the competition record of 0x9cf7a8 from `create_other_int_comps` (0x830ac0), and 0x60efd0 sets that global from the name "Copa America". The 16 purposes now say Copa America, and the class keeps the file name `GoldCup`. 0x5fab70 is now `HostCountry::redraw_world_club_and_confed_hosts`. The same check confirmed the sweep formats of the Club World Championship (12 clubs, 3 groups) and the Merconorte Cup (4 groups of 4) and corrected the 0.6.0 research.

30 functions of the wave are cited. 20 agree, 8 agree in area, 1 citation makes no claim and 1 differs. The game-creation inventory says 0x539750 reads the tactics files. The code builds the name index for each nation.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x402e70 | Called by the screen draw (display inventory) | `Area::draw` (Haiku) | `Area::draw` | agrees in area |
| 0x4031a0 | Called by the display reset (display inventory) | `Area::release_children` (Haiku) | `Area::release` | agrees in area |
| 0x403200 | Adds a widget to an area (area-widget-draw spec) | `Area::insert_sorted_index` (Haiku) | `Area::add_widget` | agrees |
| 0x4032e0 | Saves the scroll position of a list area (start-new-game inventory) | `area_get_selected_entry_field` (Haiku) | `Area::get_shown_row` | agrees |
| 0x403350 | Called by the screen draw (display inventory) | `area_layout_cells` (Haiku) | `Area::layout` | agrees in area |
| 0x403600 | Scrollbar setup (input inventory) | `area_layout_scrollbar_geometry` (Haiku) | `Area::setup_scrollbar` | agrees |
| 0x403760 | Computes the popup position (input inventory) | `area_compute_link_position` (Haiku) | `Area::get_popup_position` | agrees |
| 0x403a70 | Moves the popup (input inventory) | `area_move_clamped` (Haiku) | `Area::move_to` | agrees |
| 0x403c80 | Scrollbar drag code (display inventory) | `Area::draw_area_buttons` (Haiku) | `Area::draw_scrollbar` | agrees in area |
| 0x524d70 | Database trim (game-creation inventory) | `Database::rebuild_staff_and_player_tables` (Haiku) | `database_trim_staff_tables` | agrees |
| 0x524f50 | Mentioned in a key nation note only (sweep-international) | `Database::load_players_and_staff` (Haiku) | `database_trim_staff_tables_fragment` | no claim |
| 0x5267c0 | Club table step (game-creation inventory) | `compact_club_table` (Haiku) | `database_trim_club_table` | agrees |
| 0x528080 | free_tables (game-creation inventory) | `database_free_language_tables` (Haiku) | `database_free_all_tables` | agrees |
| 0x528540 | Database loader with the 0x43-byte entries (database inventory) | `load_database` (Haiku) | `load_database` | agrees in area |
| 0x52dad0 | Save writer of index.dat (attribute-groups) | `save_database` (Haiku) | `save_database` | agrees |
| 0x5315a0 | Index resolution after the load (game-creation inventory) | `resolve_database_indexes` (Haiku) | `resolve_database_indexes` | agrees in area |
| 0x532e00 | Runs when Use Real Players is No (game-creation inventory) | `database_refresh_club_staff_links_and_wage_tiers` (Haiku) | `database_apply_fake_players` | agrees |
| 0x5339a0 | Language names load (start-new-game inventory) | `Database::load_language_database` (Haiku) | `database_load_language_names` | agrees in area |
| 0x535930 | Language file check (game-creation inventory) | `load_language_file_path` (Haiku) | `database_language_file_exists` | agrees |
| 0x535a80 | Player Initialisation progress step (game-creation inventory) | `Database::player_initialisation_pass` (Haiku) | `database_player_initialisation` | agrees |
| 0x539750 | Reads the tactics files *.pct (game-creation inventory, emulator trace) | `Database::build_player_index_tables` (Haiku) | `database_build_nation_name_index` | differs. The code builds the name index for each nation. The emulator trace probably gave the tactics file read to the wrong function |
| 0x539ae0 | Comp history: keeps rows of selected nations (game-creation inventory) | `compact_active_records` (Haiku) | `database_prune_comp_history_table` | agrees |
| 0x893d80 | Sweden rules constructor with transfer windows (sweep-rulebooks) | `award_record_ctor` (Haiku) | `SWEDEN_RULES::SWEDEN_RULES` | agrees |
| 0x8fcdb0 | Slot 2 is start_new_season (research-name-check) | `tur_cup::release_data` (slot copy) | `TurCup::start_new_season` | agrees |
| 0x8fddc0 | Turkish Super Lig club count (season-change-points) | `tur_first_setup_cup_entrants` (Haiku) | `TurFirst::create_table` | agrees |
| 0x90a760 | Slot 2 is start_new_season (research-name-check) | `usa_mls_all_stars::release_data` (slot copy) | `UsaMlsAllStars::start_new_season` | agrees |
| 0x90b790 | Slot 2 is start_new_season (research-name-check) | `usa_open_cup::release_data` (slot copy) | `UsaOpenCup::start_new_season` | agrees |
| 0x922ee0 | Called at game creation (game-creation inventory) | `weather_fill_daily_conditions` (Haiku) | `Weather::update_city_weather` | agrees in area |
| 0x924c50 | Slot 2 is start_new_season (research-name-check) | `wel_cup::release_data` (slot copy) | `WelCup::start_new_season` | agrees |
| 0x925b30 | Welsh league table (research-name-check) | `wel_first_alloc_and_fill_league_slots` (Haiku) | `WelFirst::create_table` | agrees |

#### Wave w20 (tier 5, 2026-10-16)

Run `sweep-w20`: 8 Opus agents named 219 functions: the rest of `Database.cpp`, `Db_files.cpp`, `Fine.cpp` with the friendly and search screens from the gap, `Guio.cpp`, `History.cpp`, `Langlib.cpp`, `Menubar.cpp` and `Setup.cpp`. Each agent took 2 to 6 minutes and 106,000 to 181,000 tokens. Confidence: 192 high, 25 medium, 2 low. The merge needed no fixes. 0x53d740 is `database_is_staff_eu_national`, as the research name check found. `Setup.cpp` uses two naming patterns for its screen pairs (`setup_build_*_screen` with `setup_handle_*_input`, and `*_build` with `*_handle_input`). Both are free functions, and the review pass can align them.

91 functions of the wave are cited. 71 agree, 18 agree in area, 1 citation makes no claim and 1 differs. The import inventory calls the object that 0x5ef0b0 builds the screen manager. The code loads and saves `hall_of_fame.bin`.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x53ab90 | Named in 2026-10-14-game-creation-inventory.md | `database_compare_record_key` (Haiku) | `database_get_staff_age` | agrees |
| 0x53ad00 | Named in 2026-10-16-comparison-screen-inventory.md | `format_staff_job_title` (Haiku) | `database_get_job_name` | agrees |
| 0x53c190 | Named in 2026-10-14-game-creation-inventory.md | `clear_player_club_slot` (Haiku) | `database_remove_staff_from_club` | agrees |
| 0x53d740 | Named in 2026-10-16-foreign-and-tactics-positions.md | `transfer_offer_is_blocked` (Haiku) | `database_is_staff_eu_national` | agrees |
| 0x53d900 | Named in 2026-10-16-sweep-international.md | `is_oceania_confederation_id` (Haiku) | `database_is_melanesian_nation` | agrees |
| 0x53d940 | Named in 2026-10-16-sweep-international.md | `is_oceania_or_regional_team_id` (Haiku) | `database_is_polynesian_nation` | agrees |
| 0x53db40 | Named in 2026-10-14-game-creation-inventory.md | `database_sort_players_by_field_copy` (Haiku) | `database_sort_clubs_by_reputation` | agrees |
| 0x53ee50 | Named in 2026-10-14-columns-and-colours-inventory.md | `build_player_position_label` (Haiku) | `database_format_player_positions` | agrees |
| 0x540020 | Named in 2026-10-14-game-creation-inventory.md | `compare_player_short_field_0x80` (Haiku) | `database_compare_club_indexes_by_reputation` | agrees |
| 0x5402f0 | Named in 2026-10-14-game-creation-inventory.md | `compare_players_by_short_field` (Haiku) | `database_compare_staff_by_player_reputation` | agrees in area |
| 0x540780 | Named in 2026-10-14-game-creation-inventory.md | `compare_list_entries` (Haiku) | `database_compare_comp_history_rows` | agrees |
| 0x541e60 | Named in 2026-10-14-game-creation-inventory.md | `Database::select_squad_players_per_team` (Haiku) | `database_trim_senior_and_b_team_squads` | agrees |
| 0x542740 | Named in 2026-10-14-game-creation-inventory.md | `database_entry_is_assignable` (Haiku) | `database_club_has_reserve_squad` | agrees |
| 0x5428a0 | Named in 2026-10-15-attribute-groups.md | `get_player_attribute_name` (Haiku) | `database_get_player_attribute_name` | agrees |
| 0x5431e0 | Named in 2026-10-14-columns-and-colours-inventory.md | `get_player_attribute_rating_checked` (Haiku) | `staff_get_player_attribute` | agrees |
| 0x543280 | Named in 2026-10-14-columns-and-colours-inventory.md | `is_fixture_date_blocked` (Haiku) | `staff_is_player_attribute_visible` | agrees |
| 0x543440 | Named in 2026-10-15-attribute-groups.md | `database_set_player_field_checked` (Haiku) | `staff_get_player_attribute_for_viewer` | agrees in area |
| 0x543540 | Named in 2026-10-15-attribute-groups.md | `get_player_attribute_rating` (Haiku) | `staff_compute_player_attribute` | agrees in area |
| 0x544590 | Named in 2026-10-16-crash-fixes.md | `is_player_flagged_in_contract_or_lookup` (Haiku) | `staff_contract_has_flag_02` | no claim |
| 0x544970 | Named in 2026-10-14-game-creation-inventory.md | `club_squad_fill_from_reserves` (Haiku) | `database_merge_b_team_staff` | agrees |
| 0x54d5e0 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_continents` | agrees |
| 0x54d740 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_nations` | agrees |
| 0x54d8a0 | Named in 2026-10-13-database-inventory.md | `db_read_record_checked` (Haiku) | `db_files_read_cities` | agrees |
| 0x54da00 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_stadiums` | agrees |
| 0x54db50 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_or_assert` (Haiku) | `db_files_read_clubs` | agrees |
| 0x54dc90 | Named in 2026-10-13-database-inventory.md | `db_record_serialise_array` (Haiku) | `db_files_read_clubs_v1` | agrees |
| 0x54e630 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_staff` | agrees |
| 0x54e780 | Named in 2026-10-13-database-inventory.md | `db_read_record_checked` (Haiku) | `db_files_read_staff_v1` | agrees |
| 0x54e880 | Named in 2026-10-13-database-inventory.md | `db_files_dispatch_by_kind` (Haiku) | `db_files_read_players` | agrees |
| 0x54e9a0 | Named in 2026-10-13-database-inventory.md | `db_read_record_array` (Haiku) | `db_files_read_players_v1` | agrees |
| 0x54f030 | Named in 2026-10-13-database-inventory.md | `db_files_dispatch_by_type` (Haiku) | `db_files_read_non_players` | agrees |
| 0x54f150 | Named in 2026-10-13-database-inventory.md | `db_read_records_0x46` (Haiku) | `db_files_read_non_players_v1` | agrees |
| 0x54fc10 | Named in 2026-10-14-start-new-game-inventory.md | `db_read_record_checked` (Haiku) | `db_files_read_staff_extra` | agrees in area |
| 0x54fd60 | Named in 2026-10-13-database-inventory.md | `db_read_record_checked` (Haiku) | `db_files_read_staff_history` | agrees |
| 0x54feb0 | Named in 2026-10-13-database-inventory.md | `db_files_dispatch_by_kind` (Haiku) | `db_files_read_comps` | agrees |
| 0x54ffd0 | Named in 2026-10-13-database-inventory.md | `db_files_serialise_records` (Haiku) | `db_files_read_comps_v1` | agrees |
| 0x550230 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_comp_history` | agrees |
| 0x550380 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_staff_comps` | agrees |
| 0x5504d0 | Named in 2026-10-13-database-inventory.md | `db_files_read_field_check` (Haiku) | `db_files_read_staff_comp_history` | agrees |
| 0x5505c0 | Named in 2026-10-13-database-inventory.md | `db_files_read_record_checked` (Haiku) | `db_files_read_officials` | agrees |
| 0x550710 | Named in 2026-10-13-database-inventory.md | `db_files_check_record_version` (Haiku) | `db_files_read_colours` | agrees |
| 0x550860 | Named in 2026-10-13-database-inventory.md | `db_read_checked_record` (Haiku) | `db_files_read_names` | agrees |
| 0x5a4fc0 | Named in 2026-10-14-game-creation-inventory.md | `find_screens_fine_pair_init` (Haiku) | `Fine::Fine` | agrees |
| 0x5eed50 | Named in 2026-10-13-display-and-scrman-inventory.md | `Guio::set_text_and_refresh` (Haiku) | `Guio::set_text_and_refresh` | agrees in area |
| 0x5eef00 | Named in 2026-10-13-display-and-scrman-inventory.md | `hall_of_fame_release_cup_slot` (Haiku) | `Guio::free_images` | agrees in area |
| 0x5eef80 | Named in 2026-10-13-display-and-scrman-inventory.md | `gui_set_colour_channels` (Haiku) | `Guio::convert_colours_pixel_format` | agrees in area |
| 0x5ef0b0 | Object built at start, called the screen manager (import inventory) | `hall_of_fame_table_ctor` (Haiku) | `HallOfFame::HallOfFame` | differs. The code loads and saves hall_of_fame.bin, so this is the hall of fame table, not the screen manager |
| 0x66eb40 | Named in 2026-10-14-start-new-game-inventory.md | `langlib_change_language` (Haiku) | `langlib_change_language` | agrees |
| 0x66fbb0 | Named in 2026-10-14-start-screen-helpers.md | `league_database_load_rows` (Haiku) | `langlib_lookup_translation` | agrees |
| 0x81b890 | Named in 2026-10-13-screen-stack-inventory.md | `cm_main_entry_setup` (Haiku) | `setup_run_front_end` | agrees |
| 0x81bab0 | Named in 2026-10-13-screen-stack-inventory.md | `setup_game_menu_build` (Haiku) | `setup_build_setup_game_screen` | agrees in area |
| 0x81ce00 | Named in 2026-10-13-database-inventory.md | `setup_load_cm3_data_and_start` (Haiku) | `setup_start_new_game` | agrees in area |
| 0x81d010 | Named in 2026-10-14-start-new-game-inventory.md | `Setup::select_leagues_screen` (Haiku) | `setup_build_select_leagues_screen` | agrees |
| 0x81dc30 | Named in 2026-10-14-game-creation-inventory.md | `select_league_or_club_by_key` (Haiku) | `setup_handle_select_leagues_input` | agrees |
| 0x81e8d0 | Named in 2026-10-14-start-new-game-inventory.md | `select_start_season_screen` (Haiku) | `setup_build_select_start_season_screen` | agrees |
| 0x81eb00 | Named in 2026-10-14-start-new-game-inventory.md | `setup_handle_league_selection_command` (Haiku) | `setup_handle_select_start_season_input` | agrees |
| 0x81edd0 | Named in 2026-10-13-screen-stack-inventory.md | `setup_network_game_menu` (Haiku) | `setup_build_network_play_screen` | agrees |
| 0x81ef70 | Named in 2026-10-13-screen-stack-inventory.md | `start_network_game_by_mode` (Haiku) | `setup_handle_network_play_input` | agrees |
| 0x81fea0 | Named in 2026-10-14-game-creation-inventory.md | `start_network_manager_and_clear_slots` (Haiku) | `setup_open_manager_status_screen` | agrees |
| 0x81ff10 | Named in 2026-10-13-screen-stack-inventory.md | `manager_status_screen_refresh` (Haiku) | `setup_build_manager_status_screen` | agrees in area |
| 0x820950 | Named in 2026-10-13-screen-stack-inventory.md | `validate_saved_game_checksums` (Haiku) | `setup_handle_manager_status_input` | agrees in area |
| 0x820d40 | Named in 2026-10-14-game-creation-inventory.md | `setup_begin_manager_slot_and_open_menu` (Haiku) | `setup_add_human_manager` | agrees |
| 0x820f50 | Named in 2026-10-14-game-creation-inventory.md | `Setup::name_entry_dialog_build` (Haiku) | `setup_build_enter_name_screen` | agrees |
| 0x821520 | Named in 2026-10-14-game-creation-inventory.md | `validate_manager_name_and_password` (Haiku) | `setup_handle_enter_name_input` | agrees |
| 0x821900 | Named in 2026-10-14-game-creation-inventory.md | `select_nationality_screen` (Haiku) | `setup_build_select_nationality_screen` | agrees in area |
| 0x822040 | Named in 2026-10-14-game-creation-inventory.md | `select_team_or_club_result` (Haiku) | `setup_handle_select_nationality_input` | agrees in area |
| 0x822260 | Named in 2026-10-14-game-creation-inventory.md | `select_team_screen` (Haiku) | `setup_build_select_team_screen` | agrees |
| 0x8228f0 | Named in 2026-10-14-game-creation-inventory.md | `select_team_or_club_slot_check` (Haiku) | `select_team_screen_handle_input` | agrees |
| 0x822a60 | Named in 2026-10-14-game-creation-inventory.md | `setup_open_take_control_screen_wrapper` (Haiku) | `open_take_control_confirm` | agrees |
| 0x822aa0 | Named in 2026-10-14-game-creation-inventory.md | `take_control_confirm_dialog` (Haiku) | `take_control_confirm_build` | agrees |
| 0x822c50 | Named in 2026-10-14-game-creation-inventory.md | `take_control_of_club_check` (Haiku) | `take_control_confirm_handle_input` | agrees |
| 0x822ec0 | Named in 2026-10-14-game-creation-inventory.md | `show_enter_password_dialog` (Haiku) | `enter_password_box_build` | agrees |
| 0x8230b0 | Named in 2026-10-14-game-creation-inventory.md | `setup_resume_control_password_check` (Haiku) | `enter_password_box_handle_input` | agrees |
| 0x823420 | Named in 2026-10-14-game-creation-inventory.md | `confirm_resume_control_dialog` (Haiku) | `resume_control_confirm_build` | agrees |
| 0x823660 | Named in 2026-10-14-game-creation-inventory.md | `network_resume_control_request` (Haiku) | `resume_control_confirm_handle_input` | agrees |
| 0x823890 | Named in 2026-10-13-screen-stack-inventory.md | `show_game_config_screen` (Haiku) | `open_game_settings_box` | agrees |
| 0x823930 | Named in 2026-10-13-screen-stack-inventory.md | `setup_select_control_mode_screen` (Haiku) | `open_game_settings_from_side_bar` | agrees |
| 0x8259a0 | Named in 2026-10-16-options-screen.md | `game_setup_dispatch_option_handler` (Haiku) | `game_settings_box_handle_input` | agrees |
| 0x825d10 | Named in 2026-10-13-screen-stack-inventory.md | `hall_of_fame_view_dialog` (Haiku) | `hall_of_fame_screen_build` | agrees |
| 0x826690 | Named in 2026-10-13-screen-stack-inventory.md | `hall_of_fame_menu_action` (Haiku) | `hall_of_fame_screen_handle_input` | agrees |
| 0x826aa0 | Named in 2026-10-14-game-creation-inventory.md | `show_disk_full_dialog` (Haiku) | `disk_full_box_build` | agrees |
| 0x826c20 | Named in 2026-10-14-game-creation-inventory.md | `select_team_status_code` (Haiku) | `disk_full_box_handle_input` | agrees |
| 0x826d20 | Named in 2026-10-13-screen-stack-inventory.md | `show_out_of_memory_dialog` (Haiku) | `no_memory_box_build` | agrees |
| 0x826e60 | Named in 2026-10-13-screen-stack-inventory.md | `get_team_nation_status` (Haiku) | `no_memory_box_handle_input` | agrees |
| 0x826f20 | Named in 2026-10-14-game-creation-inventory.md | `assert_and_setup_manager_club_record` (Haiku) | `appoint_human_manager_to_club` | agrees in area |
| 0x827110 | Named in 2026-10-13-database-inventory.md | `load_setup_game_config` (Haiku) | `create_game_from_setup_file` | agrees in area |
| 0x827d50 | Named in 2026-10-14-game-creation-inventory.md | `setup_select_start_league` (Haiku) | `select_default_league_by_language` | agrees in area |
| 0x828110 | Named in 2026-10-13-database-inventory.md | `setup_game_data_and_cd_check` (Haiku) | `create_game` | agrees in area |
| 0x82a9c0 | Named in 2026-10-13-database-inventory.md | `load_game_data` (Haiku) | `load_game_data` | agrees in area |
| 0x82e430 | Named in 2026-10-15-contract-clauses.md | `save_game_to_disk` (Haiku) | `save_game_data` | agrees |
| 0x82ff40 | Named in 2026-10-16-sweep-season-end.md | `cleanup_world_state_and_delete_tmp` (Haiku) | `free_game_data` | agrees |

#### Wave w21 (tier 5, 2026-10-16)

Run `sweep-w21`: 8 Opus agents named 229 functions: the rest of `Setup.cpp`, `award_manager.cpp`, `award_screens.cpp`, `award_shortlist.cpp`, `awol.cpp`, `background.cpp`, `cash.cpp`, `club_history.cpp`, `club_records.cpp` and `club_screens.cpp`. Each agent took 2 to 6 minutes and 115,000 to 182,000 tokens. Confidence: 156 high, 71 medium, 2 low.

The Setup copy slips are settled. The competition builders now carry the nation that their constructors build: 0x832980 `create_brazilian_comps`, 0x8347c0 `create_greek_comps`, 0x831eb0 `create_oceania_club_comps`, 0x837200 `create_south_korean_comps`, 0x837d70 `create_turkish_comps` and 0x8383b0 `create_welsh_comps`. The key nation table in the port (`key_nation.c`) agrees with each one. Merge fixes: four club finance accessors were renamed from `ClubFinances::` to `Finance::`, the class of that record since the hub sweep, and one row got its missing model field.

66 functions of the wave are cited. 49 agree, 13 agree in area and 4 citations make no claim about the purpose.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x418440 | Named in 2026-10-14-game-creation-inventory.md | `award_shortlist_load_or_init` (Haiku) | `AWOL::AWOL` | agrees |
| 0x41b3d0 | Named in 2026-10-14-start-screen-helpers.md | `setup_background_system` (Haiku) | `setup_background_system` | agrees |
| 0x41b8c0 | Named in 2026-10-14-start-screen-helpers.md | `setup_background_system_add_picture` (Haiku) | `background_add_picture` | agrees |
| 0x442ab0 | Named in 2026-10-14-game-creation-inventory.md | `Club_History::open_history_files` (Haiku) | `Club_History::Club_History` | agrees |
| 0x442e80 | Named in 2026-10-16-research-name-check.md | `club_history_apply_season_records` (Haiku) | `Club_History::record_comp_season` | agrees in area |
| 0x444190 | Named in 2026-10-15-team-views-frame-inventory.md | `club_history_find_player_record` (Haiku) | `club_history_get_current_league_record` | agrees |
| 0x444520 | Named in 2026-10-14-game-creation-inventory.md | `Club_Records::load_from_cd_or_temp` (Haiku) | `Club_Records::Club_Records` | agrees |
| 0x447430 | Named in 2026-10-16-research-name-check.md | `club_records_update_season_records` (Haiku) | `Club_Records::update_league_position_records` | agrees in area |
| 0x447880 | Named in 2026-10-16-research-name-check.md | `Club_records::update_club_records_from_season` (Haiku) | `Club_Records::end_season_for_nation` | agrees in area |
| 0x448e30 | Named in 2026-10-14-game-creation-inventory.md | `club_records_flush_pending_entries` (Haiku) | `Club_Records::send_record_news` | agrees |
| 0x44c650 | Named in 2026-10-14-clickable-names-inventory.md | `club_records_read_news_record` (Haiku) | `club_records_write_record_news_text` | agrees |
| 0x453d60 | Named in 2026-10-14-clickable-names-inventory.md | `show_club_record_screen_fill` (Haiku) | `open_club_squad_screen` | agrees |
| 0x4542f0 | Named in 2026-10-14-assistant-and-days-inventory.md | `club_screens_populate_fixture_dialog` (Haiku) | `open_manager_club_screen` | agrees |
| 0x454900 | Named in 2026-10-15-assistant-pick-inventory.md | `club_screens_dispatch_main_view` (Haiku) | `club_screen_build` | agrees |
| 0x4568f0 | Named in 2026-10-13-screen-stack-inventory.md | `club_screens_reset_pair_ffff_zero` (Haiku) | `MenuItem::MenuItem` | agrees |
| 0x456900 | Named in 2026-10-14-columns-and-colours-inventory.md | `club_view_squad_list` (Haiku) | `club_screen_draw_squad_tab` | agrees |
| 0x45cbe0 | Named in picker.c | `can_player_transfer_or_loan_check` (Haiku) | `is_player_available_for_match` | agrees |
| 0x45ccf0 | Named in 2026-10-15-holiday-next-match-inventory.md | `club_screens_fixture_detail_view` (Haiku) | `club_screen_draw_next_match_tab` | agrees |
| 0x460590 | Named in 2026-10-14-clickable-names-inventory.md | `club_info_view_build_finance_stats_panel` (Haiku) | `club_screen_draw_info_tab` | no claim |
| 0x830520 | Named in 2026-10-14-game-creation-inventory.md | `setup_check_team_control_slots` (Haiku) | `setup_set_game_start_date` | agrees |
| 0x830620 | Named in 2026-10-14-game-creation-inventory.md | `create_world_cup_comps` (Haiku) | `create_world_cup_qualifying_comps` | agrees in area |
| 0x830ac0 | Named in 2026-10-14-start-new-game-inventory.md | `create_other_int_comps` (Haiku) | `create_other_int_comps` | agrees in area |
| 0x830fe0 | Named in 2026-10-16-research-name-check.md | `create_world_club_comps` (Haiku) | `create_world_club_comps` | agrees in area |
| 0x8312e0 | Named in 2026-10-16-research-name-check.md | `create_european_club_comps` (Haiku) | `create_european_club_comps` | agrees in area |
| 0x831650 | Named in 2026-10-16-research-name-check.md | `create_north_american_club_comps` (Haiku) | `create_north_american_club_comps` | agrees in area |
| 0x8318f0 | Named in 2026-10-16-research-name-check.md | `create_south_american_club_comps` (Haiku) | `create_south_american_club_comps` | agrees in area |
| 0x831ba0 | Named in 2026-10-16-research-name-check.md | `create_asian_comps` (Haiku) | `create_asian_club_comps` | agrees in area |
| 0x831eb0 | Named in 2026-10-16-research-name-check.md | `create_asian_club_comps` (Haiku) | `create_oceania_club_comps` | agrees in area |
| 0x832100 | Named in key_nation.c | `create_argentine_comps` (Haiku) | `create_argentine_comps` | agrees |
| 0x832380 | Named in key_nation.c | `create_australian_comps` (Haiku) | `create_australian_comps` | agrees |
| 0x8325d0 | Named in key_nation.c | `create_belgian_comps` (Haiku) | `create_belgian_comps` | agrees |
| 0x832980 | Key nation table entry for Brazil (key_nation.c) | `create_norwegian_comps` (Haiku) | `create_brazilian_comps` | agrees |
| 0x8330b0 | Named in key_nation.c | `create_croatian_comps` (Haiku) | `create_croatian_comps` | agrees |
| 0x833400 | Named in key_nation.c | `create_danish_comps` (Haiku) | `create_danish_comps` | agrees |
| 0x833750 | Named in 2026-10-14-game-creation-inventory.md | `create_english_comps` (Haiku) | `create_english_comps` | agrees in area |
| 0x833cf0 | Named in key_nation.c | `create_finnish_comps` (Haiku) | `create_finnish_comps` | agrees |
| 0x833ff0 | Named in key_nation.c | `create_french_comps` (Haiku) | `create_french_comps` | agrees |
| 0x8343f0 | Named in key_nation.c | `create_german_comps` (Haiku) | `create_german_comps` | agrees |
| 0x8347c0 | Key nation table entry for Greece (key_nation.c) | `create_german_comps` (Haiku) | `create_greek_comps` | agrees |
| 0x834b10 | Named in key_nation.c | `create_dutch_comps` (Haiku) | `create_dutch_comps` | agrees |
| 0x834e60 | Named in key_nation.c | `create_irish_comps` (Haiku) | `create_irish_comps` | agrees |
| 0x835320 | Named in key_nation.c | `create_italian_comps` (Haiku) | `create_italian_comps` | agrees |
| 0x835900 | Named in key_nation.c | `create_japanese_comps` (Haiku) | `create_japanese_comps` | agrees |
| 0x835c90 | Named in key_nation.c | `create_northern_irish_comps` (Haiku) | `create_northern_irish_comps` | agrees |
| 0x836050 | Named in key_nation.c | `create_norwegian_comps` (Haiku) | `create_norwegian_comps` | agrees |
| 0x836350 | Named in key_nation.c | `create_polish_comps` (Haiku) | `create_polish_comps` | agrees |
| 0x8366e0 | Named in key_nation.c | `create_portuguese_comps` (Haiku) | `create_portuguese_comps` | agrees |
| 0x836ac0 | Named in key_nation.c | `create_russian_comps` (Haiku) | `create_russian_comps` | agrees |
| 0x836dc0 | Named in key_nation.c | `create_scottish_comps` (Haiku) | `create_scottish_comps` | agrees |
| 0x837200 | Key nation table entry for South Korea (key_nation.c) | `create_spanish_comps` (Haiku) | `create_south_korean_comps` | agrees |
| 0x837560 | Named in key_nation.c | `create_spanish_comps` (Haiku) | `create_spanish_comps` | agrees |
| 0x8379f0 | Named in key_nation.c | `create_swedish_comps` (Haiku) | `create_swedish_comps` | agrees |
| 0x837d70 | Key nation table entry for Turkey (key_nation.c) | `create_spanish_comps` (Haiku) | `create_turkish_comps` | agrees |
| 0x8380c0 | Named in key_nation.c | `create_united_states_comps` (Haiku) | `create_united_states_comps` | agrees |
| 0x8383b0 | Named in 2026-10-14-start-new-game-inventory.md | `create_spanish_comps` (Haiku) | `create_welsh_comps` | agrees in area |
| 0x8386f0 | Named in 2026-10-14-game-creation-inventory.md | `setup_player_index_table` (Haiku) | `build_comp_object_table` | agrees |
| 0x8388e0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_game_connect_or_create` (Haiku) | `setup_create_or_join_network_game` | no claim |
| 0x838c60 | Named in 2026-10-13-screen-stack-inventory.md | `find_free_slot_or_report_error` (Haiku) | `find_free_human_manager_row` | agrees |
| 0x838d10 | Named in 2026-10-16-tactics-inventory.md | `count_selected_leagues_checked` (Haiku) | `count_human_managers` | no claim |
| 0x838de0 | Named in 2026-10-13-screen-stack-inventory.md | `is_current_manager_flag_5f_set` (Haiku) | `is_current_manager_being_created` | agrees |
| 0x838e10 | Named in savename.c | `valid_saved_game_version` (Haiku) | `check_saved_game_file` | agrees |
| 0x839340 | Named in 2026-10-16-research-name-check.md | `league_region_matches_filter` (Haiku) | `nation_matches_region_filter` | no claim |
| 0x839570 | Named in 2026-10-14-start-screen-helpers.md | `get_game_title_string_ptr` (Haiku) | `get_game_title` | agrees |
| 0x839590 | Named in 2026-10-14-start-screen-helpers.md | `get_selected_value_by_flag` (Haiku) | `get_heading_colour` | agrees |
| 0x839760 | Named in 2026-10-16-sweep-season-end.md | `load_fifa_transfer_rules` (Haiku) | `load_fifa_transfer_rules` | agrees |
| 0x839920 | Named in 2026-10-14-game-creation-inventory.md | `select_team_for_nation_squad` (Haiku) | `fix_division_club_count` | agrees |

#### Wave w22 (tier 5, 2026-10-16)

Run `sweep-w22`: 7 Opus agents named 188 functions: the rest of `club_screens.cpp` (club history, past meetings, friendlies, tournaments, squad numbers, part exchange and the squad list comparators), `coach.cpp` and `comp_screens.cpp` with the `Comp_Stats` record code. A batch of 2 functions joined another batch, so the wave ran 7 agents. Each agent took 3 to 6 minutes and 145,000 to 202,000 tokens. Confidence: 149 high, 35 medium, 4 low. The merge needed no fixes. Several rows are false entries inside a larger function, named with a `_fragment`, `_tail` or `_false_entry` suffix.

16 functions of the wave are cited. 11 agree, 3 agree in area and 2 citations only list the address.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x468b40 | Named in 2026-10-14-start-screen-helpers.md | `club_screens_get_club_index_or_default` (Haiku) | `ScreenManager::get_widget_action_id` | agrees |
| 0x468b90 | Named in 2026-10-15-squad-dropdown-inventory.md | `get_club_field_by_index` (Haiku) | `ScreenManager::get_widget_action_data` | agrees |
| 0x468c10 | Named in 2026-10-15-squad-dropdown-inventory.md | `club_screens_handle_match_action` (Haiku) | `club_screen_next_match_tab_input` | agrees |
| 0x469da0 | Named in 2026-10-15-squad-dropdown-inventory.md | `club_screens_release_screen_handles` (Haiku) | `club_screen_leave` | agrees |
| 0x4739a0 | Named in 2026-10-14-clickable-names-inventory.md | `screen_array_lookup_by_index` (Haiku) | `Display::get_widget_data` | agrees in area |
| 0x475520 | Named in 2026-10-14-columns-and-colours-inventory.md | `club_squad_list_view_build` (Haiku) | `squad_numbers_screen_build` | no claim |
| 0x47c180 | Named in 2026-10-14-columns-and-colours-inventory.md | `Club::squad_view_menu_build` (Haiku) | `part_exchange_screen_build` | no claim |
| 0x481ef0 | Named in 2026-10-15-team-views-frame-inventory.md | `club_squad_list_menu_build` (Haiku) | `build_club_squad_navigation_menu` | agrees |
| 0x482a40 | Named in 2026-10-15-team-views-frame-inventory.md | `club_screens_add_club_action_button` (Haiku) | `add_club_screen_action_button` | agrees |
| 0x482d10 | Named in 2026-10-15-team-views-frame-inventory.md | `Club::friendly_and_tournament_action_dispatch` (Haiku) | `handle_club_screen_action` | agrees |
| 0x4849f0 | Named in 2026-10-16-foreign-and-tactics-positions.md | `validate_transfer_or_loan_offer` (Haiku) | `get_player_status_code` | agrees |
| 0x48fcd0 | Named in 2026-10-14-clickable-names-inventory.md | `comp_screens_dispatch` (Haiku) | `comp_screens_draw_comp_screen` | agrees |
| 0x4965d0 | Named in 2026-10-14-clickable-names-inventory.md | `comp_screens_handle_save_printout_action` (Haiku) | `comp_screens_handle_comp_screen_click` | agrees |
| 0x4985e0 | Named in 2026-10-14-clickable-names-inventory.md | `comp_screens_release_slots` (Haiku) | `comp_screens_close_comp_screen` | agrees |
| 0x49d930 | Named in 2026-10-13-display-and-scrman-inventory.md | `comp_screens_league_name_panel_show` (Haiku) | `comp_screens_add_stage_menu_item` | agrees in area |
| 0x4a19f0 | Named in 2026-10-16-research-name-check.md | `comp_screens_update_round_records` (Haiku) | `Comp_Stats::update_season_end_records` | agrees in area |

#### Wave w23 (tier 5, 2026-10-16)

Run `sweep-w23`: 8 Opus agents named 240 functions: the `Comp_Stats` load, save and record news code in `comp_screens.cpp`, `contract_screens.cpp`, `discipline.cpp` with the `Display` constructor from the gap, `display.cpp`, `dispute.cpp`, `file_screens.cpp`, `find_screens.cpp`, `formation.cpp` and `friendly.cpp`. Each agent took 2 to 5 minutes and 118,000 to 190,000 tokens. Confidence: 163 high, 76 medium, 1 low. One merge fix: `SCREEN_MANAGER::set_area_selection` now uses the map's class name `ScreenManager`.

51 functions of the wave are cited. 26 agree and 25 agree in area. The display, discipline and formation research docs describe most of these functions, and the sweep names fit them.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x4a7f50 | Named in 2026-10-14-clickable-names-inventory.md | `comp_stats_record_check_and_announce` (Haiku) | `comp_stats_write_record_news_text` | agrees in area |
| 0x4e2280 | Named in user32.c | `contract_screens_apply_clause_switch` (Haiku) | `contract_screens_handle_basic_terms_action` | agrees |
| 0x5554a0 | Named in 2026-10-14-game-creation-inventory.md | `discipline_cd_check_and_init` (Haiku) | `Discipline::Discipline` | agrees |
| 0x556210 | Named in 2026-10-16-research-name-check.md | `discipline_dispatch_by_id_slot` (Haiku) | `Discipline::start_new_season_for_comp` | agrees in area |
| 0x556240 | Named in 2026-10-14-game-creation-inventory.md | `discipline_array_call_vtable_slot_1_for_each` (Haiku) | `Discipline::daily_update` | agrees |
| 0x556d00 | Named in 2026-10-14-violent-conduct-inventory.md | `discipline_slot_release_entry` (Haiku) | `Discipline::release_ban` | agrees |
| 0x55d270 | Named in 2026-10-14-assistant-and-days-inventory.md | `discipline_check_player_suspended_for_match` (Haiku) | `Discipline::is_player_banned` | agrees in area |
| 0x55d420 | Named in 2026-10-16-sweep-rulebooks.md | `discipline_rules_init` (Haiku) | `Discipline::create_rule_sets` | agrees |
| 0x55e300 | Named in 2026-10-14-violent-conduct-inventory.md | `discipline_check_player_suspension` (Haiku) | `Discipline::handle_referee_assault` | agrees |
| 0x55ef50 | Named in 2026-10-13-display-and-scrman-inventory.md | `discipline_reset_state` (Haiku) | `Display::Display` | agrees |
| 0x55efe0 | Named in 2026-10-13-display-and-scrman-inventory.md | `discipline_state_free_all` (Haiku) | `Display::~Display` | agrees |
| 0x55f1c0 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_assert_fail_on_init` (Haiku) | `display_alloc_guio_snapshot_pool` | agrees |
| 0x55f270 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_release_pic_region` (Haiku) | `display_free_guio_snapshot_pool` | agrees |
| 0x55f290 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_clear_slot_flag` (Haiku) | `display_clear_guio_snapshot` | agrees |
| 0x55f2c0 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_init_once_and_clear_screen` (Haiku) | `Display::clear_screen_once` | agrees |
| 0x55f300 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_redraw_screen` (Haiku) | `Display::draw_screen` | agrees |
| 0x55f620 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_dispatch_pic_mode` (Haiku) | `Display::poll_input` | agrees |
| 0x5603e0 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_push_pending_index` (Haiku) | `Display::push_area_order` | agrees in area |
| 0x560490 | Named in 2026-10-13-display-and-scrman-inventory.md | `order_select_target` (Haiku) | `Display::hit_test_widgets` | agrees in area |
| 0x560c30 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_update_player_queue_pass` (Haiku) | `Display::track_popup_hover` | agrees in area |
| 0x561050 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_scroll_area_step` (Haiku) | `Display::drag_scroll_thumb` | agrees in area |
| 0x561210 | Named in 2026-10-13-display-and-scrman-inventory.md | `Display::scroll_and_hit_test_pitch_view` (Haiku) | `Display::drag_widget` | agrees in area |
| 0x5617c0 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_edit_field_handle_key` (Haiku) | `Display::handle_text_entry_key` | agrees in area |
| 0x561f10 | Named in 2026-10-13-display-and-scrman-inventory.md | `display_drag_cursor_update` (Haiku) | `Display::hit_test_area_scrollbar` | agrees in area |
| 0x5621f0 | Named in 2026-10-13-input-inventory.md | `display_set_scroll_target` (Haiku) | `display_press_scrollbar_key` | agrees in area |
| 0x5622a0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_log_screen_sent` (Haiku) | `Display::log_screen_sent` | agrees |
| 0x562370 | Named in 2026-10-13-display-and-scrman-inventory.md | `serialise_match_state_packet` (Haiku) | `Display::send_screen_section` | agrees in area |
| 0x567ac0 | Named in 2026-10-13-display-and-scrman-inventory.md | `rect_set_bounds` (Haiku) | `guio_set_fixed_rect` | agrees in area |
| 0x567ae0 | Named in 2026-10-13-display-and-scrman-inventory.md | `rect_set_bounds` (Haiku) | `guio_set_rect` | agrees in area |
| 0x567b00 | Named in 2026-10-13-display-and-scrman-inventory.md | `copy_string_to_buffer_if_flagged` (Haiku) | `guio_copy_text_to_caller` | agrees in area |
| 0x5680b0 | Named in 2026-10-13-display-and-scrman-inventory.md | `Network::set_slot_value` (Haiku) | `ScreenManager::set_area_selection` | agrees in area |
| 0x5680f0 | Named in 2026-10-13-display-and-scrman-inventory.md | `resolve_media_path_for_type` (Haiku) | `display_localise_media_path` | agrees in area |
| 0x568290 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_send_buffered_byte` (Haiku) | `display_send_status_packet` | agrees in area |
| 0x568470 | Named in 2026-10-13-display-and-scrman-inventory.md | `Network::send_data_or_report_error` (Haiku) | `Display::send_keepalive` | agrees in area |
| 0x568560 | Named in 2026-10-13-display-and-scrman-inventory.md | `Network_session::pack_and_send_player_record` (Haiku) | `Display::send_widget_click` | agrees in area |
| 0x569610 | Named in 2026-10-13-display-and-scrman-inventory.md | `dispute_tick_players` (Haiku) | `Display::animate_widgets` | agrees in area |
| 0x5697e0 | Named in 2026-10-14-game-creation-inventory.md | `dispute_file_load_or_reset` (Haiku) | `DISPUTE::DISPUTE` | agrees |
| 0x58f500 | Named in 2026-10-13-screen-stack-inventory.md | `file_browser_open_dialog` (Haiku) | `open_file_dialog` | agrees |
| 0x591840 | Named in 2026-10-14-game-creation-inventory.md | `assert_screen_args_and_open_screen` (Haiku) | `confirm_push` | agrees |
| 0x5919b0 | Named in 2026-10-14-start-screen-spec.md | `create_screen_assert_or_open` (Haiku) | `confirm_push_text_object` | agrees |
| 0x5b2ae0 | Named in 2026-10-14-assistant-and-days-inventory.md | `formation_get_position_slot` (Haiku) | `Formation::get_slot_position` | agrees in area |
| 0x5b3560 | Named in 2026-10-16-tactics-inventory.md | `Formation::choose_formation_players` (Haiku) | `Formation::set_slot_coordinate` | agrees |
| 0x5b3c70 | Named in 2026-10-16-tactics-inventory.md | `Formation::get_player_slot` (Haiku) | `Formation::get_slot_coordinate` | agrees |
| 0x5b50d0 | Named in 2026-10-14-assistant-and-days-inventory.md | `formation_set_role_flags` (Haiku) | `Formation::set_team_instruction` | agrees in area |
| 0x5b5270 | Named in 2026-10-15-assistant-pick-inventory.md | `Formation::get_position_pair` (Haiku) | `Formation::get_player_instructions` | agrees |
| 0x5b57c0 | Named in 2026-10-15-assistant-pick-inventory.md | `formation::get_position_slot_flag` (Haiku) | `Formation::get_marking_type` | agrees |
| 0x5b59a0 | Named in 2026-10-16-tactics-inventory.md | `formation_recompute_slot_mask` (Haiku) | `Formation::update_shape_code` | agrees in area |
| 0x5b5d10 | Named in 2026-10-16-tactics-inventory.md | `Formation::load_from_disk` (Haiku) | `Formation::load_from_disk` | agrees |
| 0x5c1f80 | Named in 2026-10-14-game-creation-inventory.md | `friendly_cup_load_cd_and_init` (Haiku) | `Friendly::Friendly` | agrees |
| 0x5ca030 | Named in 2026-10-14-game-creation-inventory.md | `friendly_store_squad_ids` (Haiku) | `Friendly::register_comp_objects` | agrees |
| 0x5ca4f0 | Named in 2026-10-14-widescreen-inventory.md | `friendly_cup_table_init` (Haiku) | `Friendly::build_international_date_table` | agrees in area |

#### Wave w24 (tier 5, 2026-10-17)

Run `sweep-w24`: 7 Opus agents named 202 functions: `game_config.cpp`, `hall_of_fame.cpp`, `human_manager.cpp`, `index.cpp`, `injury.cpp`, `main.cpp` and `manager_manager.cpp`. A batch of 4 functions joined another batch. Each agent took 3 to 5 minutes and 123,000 to 197,000 tokens. Confidence: 136 high, 65 medium, 1 low. The merge needed no fixes. Twelve rows in `index.cpp` are decode fragments inside three large functions.

48 functions of the wave are cited. 33 agree, 10 agree in area and 5 citations make no claim about the purpose.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x5d72c0 | Named in 2026-10-14-import-inventory.md | `game_config_load` (Haiku) | `Config::Config` | agrees |
| 0x5d7ad0 | Named in 2026-10-16-options-screen.md | `game_config_copy_record` (Haiku) | `Config::Config` | agrees |
| 0x5d7bc0 | Named in 2026-10-14-import-inventory.md | `game_config_apply_mode_flags` (Haiku) | `Config::set_defaults` | agrees in area |
| 0x5d7c60 | Named in 2026-10-14-columns-and-colours-inventory.md | `game_config_set_flag` (Haiku) | `Config::set_option` | agrees |
| 0x5d8020 | Named in 2026-10-14-start-new-game-inventory.md | `game_config_select_team` (Haiku) | `Config::set_language` | agrees in area |
| 0x5d8270 | Named in 2026-10-16-options-screen.md | `Config::set_index_checked` (Haiku) | `Config::set_currency` | agrees |
| 0x5d83e0 | Named in 2026-10-16-options-screen.md | `Config::set_season_length_checked` (Haiku) | `Config::set_auto_save_interval` | agrees |
| 0x5d8480 | Named in 2026-10-14-game-creation-inventory.md | `Config::load_preferences` (Haiku) | `Config::apply_and_save` | agrees in area |
| 0x5ef5e0 | Named in 2026-10-14-widescreen-inventory.md | `hall_of_fame_try_insert_entry` (Haiku) | `HallOfFame::try_add_entry` | no claim |
| 0x5fb430 | Named in 2026-10-14-game-creation-inventory.md | `human_manager_load_from_cd` (Haiku) | `HUMAN_MANAGER::HUMAN_MANAGER` | agrees |
| 0x5fb810 | Named in 2026-10-14-game-creation-inventory.md | `Human_manager::load_human_record` (Haiku) | `HUMAN_MANAGER::reset_human_record` | agrees |
| 0x5fbd00 | Named in 2026-10-14-assistant-and-days-inventory.md | `human_manager_get_record` (Haiku) | `HUMAN_MANAGER::get_active_tactic` | agrees in area |
| 0x5fbe20 | Named in 2026-10-14-assistant-and-days-inventory.md | `human_manager_set_player_state` (Haiku) | `HUMAN_MANAGER::select_tactic_slot` | agrees |
| 0x5fc760 | Named in 2026-10-16-tactics-inventory.md | `human_manager_get_name_string` (Haiku) | `HUMAN_MANAGER::get_recent_file` | agrees |
| 0x5fcff0 | Named in 2026-10-15-commentary-speed-inventory.md | `human_manager_lookup_record` (Haiku) | `HUMAN_MANAGER::get_commentary_speed` | agrees |
| 0x5fd100 | Named in 2026-10-15-commentary-speed-inventory.md | `human_manager_set_record_field` (Haiku) | `HUMAN_MANAGER::set_commentary_speed` | agrees |
| 0x5fd2f0 | Named in 2026-10-15-holiday-next-match-inventory.md | `human_manager_set_flag` (Haiku) | `HUMAN_MANAGER::set_on_holiday` | agrees |
| 0x5fd480 | Named in 2026-10-14-assistant-and-days-inventory.md | `human_manager_is_player_flag` (Haiku) | `HUMAN_MANAGER::controls_reserve_team` | no claim |
| 0x5fdc30 | Named in 2026-10-14-assistant-and-days-inventory.md | `human_manager_get_record` (Haiku) | `HUMAN_MANAGER::get_holiday_stamp` | agrees |
| 0x5fdd30 | Named in 2026-10-14-assistant-and-days-inventory.md | `human_manager_set_field` (Haiku) | `HUMAN_MANAGER::set_holiday_stamp` | agrees |
| 0x5fde40 | Named in 2026-10-14-game-creation-inventory.md | `human_manager_record_ptr` (Haiku) | `HUMAN_MANAGER::get_password` | agrees in area |
| 0x5fdf40 | Named in 2026-10-14-game-creation-inventory.md | `human_manager_record_address` (Haiku) | `HUMAN_MANAGER::get_password_entry` | agrees in area |
| 0x5fe040 | Named in 2026-10-14-assistant-and-days-inventory.md | `human_manager_get_record` (Haiku) | `HUMAN_MANAGER::is_computer_controlled` | agrees in area |
| 0x5fe290 | Named in 2026-10-15-holiday-next-match-inventory.md | `human_manager_match_entry_is_valid` (Haiku) | `HUMAN_MANAGER::is_watching_fixture` | agrees |
| 0x5fe8d0 | Named in 2026-10-15-holiday-next-match-inventory.md | `human_manager_clear_unmatched_slots` (Haiku) | `HUMAN_MANAGER::clear_invalid_watched_fixtures` | agrees in area |
| 0x5feb60 | Named in 2026-10-15-holiday-next-match-inventory.md | `human_manager_set_lineup_slot` (Haiku) | `HUMAN_MANAGER::set_holiday_option` | agrees |
| 0x5fed50 | Named in 2026-10-15-holiday-next-match-inventory.md | `human_manager_get_record` (Haiku) | `HUMAN_MANAGER::get_saved_lineup` | agrees in area |
| 0x5fee50 | Named in 2026-10-15-commentary-speed-inventory.md | `human_manager_load` (Haiku) | `HUMAN_MANAGER::load` | agrees |
| 0x5ff9e0 | Named in 2026-10-15-commentary-speed-inventory.md | `Human_manager::load_human_manager_dat` (Haiku) | `HUMAN_MANAGER::save` | agrees |
| 0x601560 | Named in 2026-10-15-squad-dropdown-inventory.md | `human_manager_clear_slot_value` (Haiku) | `HUMAN_MANAGER::clear_squad_number` | agrees |
| 0x601a40 | Named in 2026-10-14-game-creation-inventory.md | `index_build_colour_and_nation_tables` (Haiku) | `set_key_ids` | agrees |
| 0x60d780 | Named in 2026-10-16-sweep-international.md | `index_report_missing_index_fatal` (Haiku) | `report_missing_key_id` | agrees in area |
| 0x60d810 | Named in 2026-10-14-game-creation-inventory.md | `nation_name_to_id_store` (Haiku) | `match_nation_key_id` | agrees |
| 0x60efd0 | Named in 2026-10-14-game-creation-inventory.md | `set_competition_id_by_name` (Haiku) | `match_club_comp_key_id` | agrees |
| 0x6115e0 | Named in 2026-10-14-game-creation-inventory.md | `award_name_to_slot` (Haiku) | `match_staff_comp_key_id` | agrees |
| 0x6146b0 | Named in 2026-10-14-game-creation-inventory.md | `index_set_club_id_by_name` (Haiku) | `match_club_key_id` | agrees |
| 0x62a680 | Named in 2026-10-14-game-creation-inventory.md | `injury_history_open_cd` (Haiku) | `Injury::Injury` | agrees |
| 0x62a920 | Named in 2026-10-14-game-creation-inventory.md | `injury_update_recovery_rates` (Haiku) | `Injury::init_squad_fitness` | agrees |
| 0x62b7b0 | Named in 2026-10-14-game-creation-inventory.md | `injury_daily_tick` (Haiku) | `Injury::daily_update` | agrees |
| 0x62c590 | Named in 2026-10-14-columns-and-colours-inventory.md | `injury_recovery_percent_or_days` (Haiku) | `Injury::get_condition` | agrees |
| 0x62c680 | Named in 2026-10-15-assistant-pick-inventory.md | `injury_update_player_recovery` (Haiku) | `Injury::update_player_fitness` | no claim |
| 0x68d370 | Named in 2026-10-13-screen-stack-inventory.md | `winmain` (Haiku) | `win_main` | agrees |
| 0x68d520 | Named in 2026-10-13-database-inventory.md | `main_pump_messages` (Haiku) | `pump_messages` | no claim |
| 0x68d630 | Named in 2026-10-13-input-inventory.md | `main_window_proc` (Haiku) | `main_window_proc` | agrees |
| 0x68dbc0 | Named in 2026-10-14-import-inventory.md | `restart_game_process` (Haiku) | `restart_game` | agrees |
| 0x68dbf0 | Named in 2026-10-14-game-creation-inventory.md | `manager_manager_load_cd_and_init` (Haiku) | `MANAGER_MANAGER::MANAGER_MANAGER` | agrees |
| 0x68f9c0 | Named in 2026-10-14-game-creation-inventory.md | `manager_manager_process_transfer_offers` (Haiku) | `MANAGER_MANAGER::daily_update` | agrees |
| 0x696b10 | Named in 2026-10-14-assistant-and-days-inventory.md | `manager_manager_evaluate_transfer_offer` (Haiku) | `MANAGER_MANAGER::check_board_transfer_veto` | no claim |

#### Wave w25 (tier 5, 2026-10-17)

Run `sweep-w25`: 8 Opus agents named 234 functions: the rest of `manager_manager.cpp` (board and FA news), `manager_screens.cpp`, `match_day.cpp` and the first five batches of `match_eng.cpp` (`CM3_MATCH_ENGINE`). Each agent took 2 to 7 minutes and 125,000 to 228,000 tokens. Confidence: 114 high, 109 medium, 11 low. The merge needed no fixes. The match engine names have lower confidence: its code works with event codes and grid cells and has few strings. From this wave on, the brief allows agents to disassemble a function when the pseudocode hides the convention or the ECX flow.

27 functions of the wave are cited. 19 agree, 7 agree in area and 1 differs. The stadium research lists 0x6adb70 as a second text of board reply news 0x1786. The sweep names it the text of the manager's ultimatum response.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x6a8770 | Named in 2026-10-16-stadium-research.md | `add_board_request_option` (Haiku) | `get_board_request_option_text` | agrees |
| 0x6a8810 | Named in 2026-10-16-stadium-research.md | `manager_manager_job_offer_decision` (Haiku) | `MANAGER_MANAGER::handle_board_request` | agrees |
| 0x6a9470 | Named in 2026-10-16-stadium-research.md | `MANAGER_MANAGER::compose_board_reaction_news` (Haiku) | `MANAGER_MANAGER::news_board_request_reply_text` | agrees |
| 0x6adb70 | Named in 2026-10-16-stadium-research.md | `manager_news_board_demand_response` (Haiku) | `news_ultimatum_response_text` | differs. The research lists it as a second text of board reply news 0x1786. The sweep names it the text of the manager's ultimatum response. Not checked |
| 0x6b2080 | Named in 2026-10-16-stadium-research.md | `manager_screens_show_error_or_open_board` (Haiku) | `open_board_confidence_screen` | agrees |
| 0x6b2130 | Named in 2026-10-16-stadium-research.md | `manager_screens_board_request_panel_show` (Haiku) | `board_confidence_screen_build` | agrees |
| 0x6b2370 | Named in 2026-10-16-stadium-research.md | `competition_selector_dispatch` (Haiku) | `board_confidence_screen_handle_action` | agrees |
| 0x6b2d60 | Named in 2026-10-15-holiday-next-match-inventory.md | `manager_screens_open_holiday_screen` (Haiku) | `open_holiday_screen` | agrees |
| 0x6b2d80 | Named in 2026-10-14-assistant-and-days-inventory.md | `show_holiday_confirm_dialog` (Haiku) | `holiday_screen_build` | agrees |
| 0x6b3440 | Named in 2026-10-15-holiday-next-match-inventory.md | `manager_screens_dispatch_holiday_action` (Haiku) | `holiday_screen_handle_action` | agrees |
| 0x6b3c10 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_day_schedule_build` (Haiku) | `MATCH_DAY::MATCH_DAY` | agrees |
| 0x6b4190 | Named in 2026-10-14-game-creation-inventory.md | `match_day_release_resources` (Haiku) | `MATCH_DAY::~MATCH_DAY` | agrees |
| 0x6b42a0 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_day_clear_and_select_fixture` (Haiku) | `MATCH_DAY::show_match_day_screen` | agrees |
| 0x6b4360 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_day_run_round` (Haiku) | `MATCH_DAY::play_matches` | agrees in area |
| 0x6b5040 | Named in 2026-10-15-holiday-next-match-inventory.md | `match_day_refresh_flags` (Haiku) | `MATCH_DAY::refresh_human_interest_flags` | agrees |
| 0x6b55c0 | Named in 2026-10-15-commentary-speed-inventory.md | `match_day_select_next_event` (Haiku) | `MATCH_DAY::step_live_match` | agrees |
| 0x6b59a0 | Named in 2026-10-14-game-creation-inventory.md | `match_day_has_fixture_for_team` (Haiku) | `MATCH_DAY::is_club_playing` | agrees in area |
| 0x6b5a70 | Named in 2026-10-15-commentary-speed-inventory.md | `match_day_schedule_next_fixture` (Haiku) | `MATCH_DAY::start_next_background_match` | agrees in area |
| 0x6b5ce0 | Named in 2026-10-15-commentary-speed-inventory.md | `match_day_process_pending_match` (Haiku) | `MATCH_DAY::step_background_match` | agrees |
| 0x6b7f20 | Named in 2026-10-14-assistant-and-days-inventory.md | `CM3_MATCH_ENGINE::init_match` (Haiku) | `CM3_MATCH_ENGINE::init_match` | agrees in area |
| 0x6b98c0 | Named in 2026-10-14-assistant-and-days-inventory.md | `CM3_MATCH_ENGINE::simulate_minute_loop` (Haiku) | `CM3_MATCH_ENGINE::run_match_steps` | agrees |
| 0x6cfc60 | Named in 2026-10-14-violent-conduct-inventory.md | `match_eng_forward_seven_args` (Haiku) | `match_eng_referee_judge_tackle` | agrees in area |
| 0x6cfd40 | Named in 2026-10-14-violent-conduct-inventory.md | `match_engine_forward_four_args` (Haiku) | `match_eng_referee_decide_misconduct_card` | agrees |
| 0x6d1210 | Named in 2026-10-14-violent-conduct-inventory.md | `CM3_MATCH_ENGINE::process_match_state` (Haiku) | `CM3_MATCH_ENGINE::process_queued_fouls` | agrees in area |
| 0x6d4250 | Named in 2026-10-16-sweep-international.md | `match_engine_is_player_available` (Haiku) | `CM3_MATCH_ENGINE::is_golden_goal_active` | agrees |
| 0x6d5bf0 | Named in 2026-10-14-columns-and-colours-inventory.md | `crowd_sound_init` (Haiku) | `Match_Events::Match_Events` | agrees |
| 0x6d78b0 | Named in 2026-10-15-commentary-speed-inventory.md | `match_events_next_event` (Haiku) | `Match_Events::get_next_event` | agrees in area |

#### Wave w26 (tier 5, 2026-10-17)

Run `sweep-w26`: 7 Opus agents named 196 functions: `match_events.cpp` (`Match_Events`), `match_man.cpp` (`MATCH_MAN`), `match_official.cpp` (`MATCH_OFFICIAL`), `match_pl.cpp` (`MATCH_PLAYER`) and `match_screens.cpp` with its match stats functions (`Match_Stats`). The first launch stopped at the usage limit and wrote nothing, so the wave ran again. Each agent took 3 to 9 minutes and 150,000 to 232,000 tokens. Confidence: 116 high, 74 medium, 6 low.

The merge needed two fixes. Nine `match_pl.cpp` getters and setters take the match engine or the event queue as ECX. One agent named them `CM3_MATCH_ENGINE::` and `Match_Events::` members. They now use the free style of the other engine getters in the file, for example `match_engine_get_ball_col`. Twelve false entries (addresses inside other functions) had three name styles. They now use the `<parent>_fragment_<addr>` style of the earlier waves. 0x6ea760 is `MATCH_OFFICIAL::find_offside_player`, as the known issue said.

51 functions of the wave are cited. 38 agree and 13 agree in area. None differs.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x6d7b00 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_events_format_text_token` (Haiku) | `Match_Events::show_event_text` | agrees |
| 0x6d8c70 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_events_check_order_ready` (Haiku) | `Match_Events::is_line_finished` | agrees |
| 0x6d9e50 | Named in 2026-10-14-violent-conduct-inventory.md | `match_event_handler_read_config` (Haiku) | `match_events_read_config` | agrees in area |
| 0x6db500 | Named in 2026-10-14-assistant-and-days-inventory.md | `CM3_MATCH_ENGINE::init_match_player` (Haiku) | `MATCH_MAN::init` | agrees |
| 0x6dbb80 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_man_count_eligible_players` (Haiku) | `MATCH_MAN::place_selected_players` | agrees |
| 0x6dbc50 | Named in 2026-10-14-assistant-and-days-inventory.md | `CM3_MATCH_ENGINE::build_match_lineups` (Haiku) | `MATCH_MAN::pick_team` | agrees |
| 0x6ddab0 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_man_tactical_step` (Haiku) | `MATCH_MAN::set_computer_instructions` | agrees |
| 0x6df460 | Named in 2026-10-15-assistant-pick-inventory.md | `match_man_score_next_action` (Haiku) | `MATCH_MAN::choose_mentality` | agrees in area |
| 0x6e0230 | Named in 2026-10-14-assistant-and-days-inventory.md | `CM3_MATCH_ENGINE::select_team_tactic_score_tables` (Haiku) | `MATCH_MAN::choose_computer_tactic` | agrees |
| 0x6e0d00 | Named in 2026-10-14-assistant-and-days-inventory.md | `CM3_MATCH_ENGINE::place_player_in_team` (Haiku) | `MATCH_MAN::place_player_in_team` | agrees |
| 0x6e1090 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_man_assign_players_to_slots` (Haiku) | `MATCH_MAN::make_substitutions` | agrees |
| 0x6e1940 | Named in 2026-10-15-assistant-pick-inventory.md | `match_man_best_candidate_player` (Haiku) | `MATCH_MAN::choose_captain` | agrees |
| 0x6e1aa0 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_man_assign_substitute_positions` (Haiku) | `MATCH_MAN::reassign_positions` | agrees |
| 0x6e1cf0 | Named in 2026-10-15-assistant-pick-inventory.md | `match_man_player_rating_check` (Haiku) | `match_man_get_captain_score` | agrees in area |
| 0x6e2000 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_man_assign_players_to_positions` (Haiku) | `match_man_assign_players_to_positions` | agrees |
| 0x6e2f20 | Named in 2026-10-15-assistant-pick-inventory.md | `match_man_score_player_for_role` (Haiku) | `match_man_score_player_for_role` | agrees |
| 0x6e8910 | Named in 2026-10-15-assistant-pick-inventory.md | `match_man_assign_substitutes` (Haiku) | `match_man_apply_squad_restriction` | agrees |
| 0x6eab60 | Named in 2026-10-14-violent-conduct-inventory.md | `match_official_shot_resolve` (Haiku) | `MATCH_OFFICIAL::decide_card` | agrees |
| 0x6eaea0 | Named in 2026-10-15-assistant-pick-inventory.md | `CM3_MATCH_ENGINE::setup_non_playing_staff` (Haiku) | `MATCH_PLAYER::setup` | agrees in area |
| 0x6ec010 | Named in 2026-10-14-game-creation-inventory.md | `match_official_compute_player_ratings` (Haiku) | `MATCH_PLAYER::calc_match_attributes` | agrees in area |
| 0x6eecb0 | Named in 2026-10-16-crash-fixes.md | `match_official_compute_player_attributes` (Haiku) | `MATCH_PLAYER::calc_ratings` | agrees |
| 0x7164b0 | Named in 2026-10-14-violent-conduct-inventory.md | `match_player_check_threshold` (Haiku) | `MATCH_PLAYER::passes_anger_check` | agrees |
| 0x716630 | Named in 2026-10-14-violent-conduct-inventory.md | `MATCH_PLAYER::try_shot_or_pass_action` (Haiku) | `MATCH_PLAYER::start_fight` | agrees |
| 0x716e10 | Named in 2026-10-14-assistant-and-days-inventory.md | `MATCH_PLAYER::resolve_shot_outcome` (Haiku) | `MATCH_PLAYER::react_to_foul_decision` | agrees in area |
| 0x717450 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_player_forward_if_active` (Haiku) | `MATCH_PLAYER::get_secondary_position` | agrees in area |
| 0x717470 | Named in 2026-10-14-widescreen-inventory.md | `MATCH_PLAYER::check_action_trigger` (Haiku) | `MATCH_PLAYER::is_story_event_applicable` | agrees in area |
| 0x717cc0 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_screens_show_fixtures_or_assert` (Haiku) | `open_day_fixtures_screen` | agrees |
| 0x718930 | Named in 2026-10-14-assistant-and-days-inventory.md | `match_screens_dispatch_fixture_selection` (Haiku) | `handle_day_fixtures_input` | agrees |
| 0x718aa0 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_init_window_or_assert` (Haiku) | `open_match_screen` | agrees |
| 0x718e90 | Named in 2026-10-15-commentary-speed-inventory.md | `match_screens_draw_match_overview` (Haiku) | `build_match_screen` | agrees |
| 0x71a240 | Named in 2026-10-15-commentary-speed-inventory.md | `match_screens_handle_matchday_input` (Haiku) | `handle_match_screen_input` | agrees |
| 0x71b4a0 | Named in 2026-10-14-clickable-names-inventory.md | `match_screens_open_screen_with_config` (Haiku) | `open_match_details_screen` | agrees |
| 0x71c270 | Named in 2026-10-14-clickable-names-inventory.md | `match_screens_draw_match_report` (Haiku) | `draw_match_overview_panel` | agrees |
| 0x71d9e0 | Named in display.c | `match_screens_draw_possession_bars` (Haiku) | `refresh_possession_bar` | agrees |
| 0x71e040 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_build_stats_grid` (Haiku) | `draw_match_stats_panel` | agrees |
| 0x71f500 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_draw_shot_zone_panel` (Haiku) | `draw_action_zones_panel` | agrees |
| 0x720320 | Named in 2026-10-14-widescreen-inventory.md | `match_screens_update_live_panels` (Haiku) | `refresh_action_zones_values` | agrees in area |
| 0x721000 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_show_match_report` (Haiku) | `draw_match_report_panel` | agrees |
| 0x7217a0 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_draw_team_stats_table` (Haiku) | `draw_team_stats_panel` | agrees |
| 0x722ed0 | Named in 2026-10-14-clickable-names-inventory.md | `match_screens_build_team_selection_rows` (Haiku) | `draw_player_ratings_panel` | agrees |
| 0x723350 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_fill_event_rows` (Haiku) | `refresh_player_ratings_rows` | agrees |
| 0x723d20 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_draw_latest_scores_body` (Haiku) | `draw_latest_scores_panel` | agrees |
| 0x724520 | Named in 2026-10-14-widescreen-inventory.md | `match_screens_draw_fixture_list_rows` (Haiku) | `refresh_latest_scores_rows` | agrees in area |
| 0x725ca0 | Named in 2026-10-14-start-screen-helpers.md | `match_screens_wait_for_other_players_or_advance` (Haiku) | `match_screens_handle_continue_command` | agrees |
| 0x725f70 | Named in 2026-10-14-clickable-names-inventory.md | `match_screens_open_team_select_view` (Haiku) | `match_screens_open_team_selection` | agrees |
| 0x7260f0 | Named in build-brief-0.3.0.md | `match_screens_team_selection_check_squad` (Haiku) | `match_screens_draw_team_selection` | agrees in area |
| 0x727f00 | Named in 2026-10-15-holiday-next-match-inventory.md | `match_screens_assert_or_start_worker` (Haiku) | `match_screens_open_waiting_screen_for_slot` | agrees in area |
| 0x7280e0 | Named in 2026-10-14-columns-and-colours-inventory.md | `match_screens_draw_league_table` (Haiku) | `match_screens_draw_live_league_table` | agrees in area |
| 0x729000 | Named in 2026-10-14-game-creation-inventory.md | `match_stats_load_from_cd` (Haiku) | `Match_Stats::Match_Stats` | agrees |
| 0x729e30 | Named in 2026-10-14-clickable-names-inventory.md | `match_stats_find_fixture_by_index` (Haiku) | `Match_Stats::find_record_fixture` | agrees |
| 0x72b1d0 | Named in 2026-10-14-game-creation-inventory.md | `media_block_reset_and_release` (Haiku) | `Media::Media` | agrees |

#### Wave w27 (tier 5, 2026-10-17)

Run `sweep-w27`: 8 Opus agents named 203 functions: `media.cpp` (`Media`), `month_award.cpp`, `month_ratings.cpp`, `network.h`, `news.cpp`, `news_screens.cpp`, `notes.cpp`, `officials_manager.cpp`, `os.cpp`, `physio.cpp` and the `scrman.cpp` gap functions. Each agent took 2 to 7 minutes and 115,000 to 229,000 tokens. Confidence: 150 high, 53 medium, none low.

The merge needed two fixes. One agent named four media news text writers as free stdcall functions. Two agents named the same kind of writer `Media::`, because every caller sets ECX to the Media object at 0xb6b7c8. The four rows now use `Media::`. Seventeen false entries had four name styles. They now use the `<parent>_fragment_<addr>` style. Three files keep more than one class, because their code serves more than one object: `network.h` (`Display`, `NetworkBuffer`, `ScreenManager`, a header with inline methods), `news.cpp` (`NewsItem` and `News`) and `officials_manager.cpp` (`OfficialsManager` and one `Official::operator=`).

46 functions of the wave are cited. 20 agree, 24 agree in area and 2 differ. The display and scrman research calls the scrman record key a screen key. The sweep reads it as a manager key, with away and done flags. The two differences are 0x758de0 and 0x800000.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x55f7c0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_session_reset` (Haiku) | `Display::reset` | agrees |
| 0x560140 | Named in 2026-10-13-display-and-scrman-inventory.md | `byte_buffer_push_byte` (Haiku) | `NetworkBuffer::put_char` | agrees |
| 0x5601e0 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_append_word` (Haiku) | `NetworkBuffer::put_short` | agrees |
| 0x560290 | Named in 2026-10-13-display-and-scrman-inventory.md | `Network_buffer::put_word` (Haiku) | `NetworkBuffer::put_ushort` | agrees |
| 0x560340 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_write_u32` (Haiku) | `NetworkBuffer::put_ulong` | agrees |
| 0x567b40 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_write_u32` (Haiku) | `NetworkBuffer::put_int` | agrees in area |
| 0x567be0 | Named in 2026-10-13-display-and-scrman-inventory.md | `net_buffer_read_byte` (Haiku) | `NetworkBuffer::get_char` | agrees in area |
| 0x567c80 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_read_u16` (Haiku) | `NetworkBuffer::get_short` | agrees in area |
| 0x567d30 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_buffer_read_u32` (Haiku) | `NetworkBuffer::get_long` | agrees in area |
| 0x567de0 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_read_u32` (Haiku) | `NetworkBuffer::get_int` | agrees in area |
| 0x567e90 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_read_u16` (Haiku) | `NetworkBuffer::get_ushort` | agrees in area |
| 0x567f40 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_read_u32` (Haiku) | `NetworkBuffer::get_ulong` | agrees in area |
| 0x567ff0 | Named in 2026-10-13-display-and-scrman-inventory.md | `net_buffer_read_string` (Haiku) | `NetworkBuffer::get_sized_block` | agrees in area |
| 0x5694a0 | Named in 2026-10-13-display-and-scrman-inventory.md | `buffer_write_u32` (Haiku) | `NetworkBuffer::put_long` | agrees in area |
| 0x569540 | Named in 2026-10-13-display-and-scrman-inventory.md | `Packet::write_sized_payload` (Haiku) | `NetworkBuffer::put_sized_block` | agrees |
| 0x72c1f0 | Named in 2026-10-14-columns-and-colours-inventory.md | `media_generate_player_interview` (Haiku) | `Media::create_player_media_story` | agrees in area |
| 0x758de0 | Named in 2026-10-14-violent-conduct-inventory.md | `media_queue_response_screen` (Haiku) | `Media::queue_manager_fa_complaint_story` | differs. The violent conduct research guesses a club reaction (transfer list, fine or release). The sweep names it the queue function of news 0x178e, the manager's complaint to the FA. Not checked |
| 0x77bc80 | Named in 2026-10-14-clickable-names-inventory.md | `news_queue_item_text` (Haiku) | `NewsItem::write_text` | agrees |
| 0x784100 | Named in 2026-10-15-holiday-next-match-inventory.md | `news_respond_dispatch` (Haiku) | `NewsItem::auto_respond` | agrees in area |
| 0x785670 | Named in 2026-10-14-clickable-names-inventory.md | `news_item_category_from_id` (Haiku) | `NewsItem::get_category` | agrees |
| 0x785b20 | Named in 2026-10-14-game-creation-inventory.md | `news_cd_check_loop` (Haiku) | `News::News` | agrees |
| 0x7874b0 | Named in 2026-10-15-holiday-next-match-inventory.md | `news_item_ring_walk_dispatch` (Haiku) | `News::auto_respond_all` | agrees in area |
| 0x788010 | Named in 2026-10-14-assistant-and-days-inventory.md | `news_screens_build_manager_list_screen` (Haiku) | `news_screens_open_news_screen` | agrees |
| 0x79bf40 | Named in 2026-10-14-game-creation-inventory.md | `officials_manager_clear_flags` (Haiku) | `OfficialsManager::clear_busy_flags` | agrees |
| 0x7a1820 | Named in 2026-10-14-game-creation-inventory.md | `os_get_free_disk_space_mb` (Haiku) | `os_get_free_disk_space_mb` | agrees |
| 0x7fc380 | Named in 2026-10-13-display-and-scrman-inventory.md | `scout_manager_reset_state` (Haiku) | `ScreenManager::ScreenManager` | agrees |
| 0x7fc690 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_session_poll_and_flush` (Haiku) | `ScreenManager::disconnect_network` | agrees in area |
| 0x7fc7e0 | Named in 2026-10-13-display-and-scrman-inventory.md | `SCREEN_MANAGER::monitor` (Haiku) | `ScreenManager::monitor` | agrees |
| 0x7fdc00 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_session_loop` (Haiku) | `ScreenManager::run_network_client` | agrees |
| 0x7fe4a0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_iterate_slots_notify` (Haiku) | `ScreenManager::open_screen_for_each_record` | agrees |
| 0x7ff500 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_session_create_screen_manager` (Haiku) | `ScreenManager::start_network_session` | agrees |
| 0x7ff5f0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_session_destroy` (Haiku) | `ScreenManager::end_network_session` | agrees |
| 0x7ff630 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_player_slot_register` (Haiku) | `scrman_add_player_record` | agrees |
| 0x7ff7b0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_flush_pending_for_slot` (Haiku) | `scrman_remove_player_records` | agrees |
| 0x7ff810 | Named in 2026-10-13-display-and-scrman-inventory.md | `squad_swap_players_by_id` (Haiku) | `ScreenManager::swap_manager_keys` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7ff8f0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_set_slot_and_flag` (Haiku) | `ScreenManager::add_manager_record` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7ff990 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_remove_player_slot` (Haiku) | `ScreenManager::remove_manager_record` | agrees in area |
| 0x7ffcd0 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_remove_player_slot` (Haiku) | `ScreenManager::switch_to_manager` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7ffd80 | Named in 2026-10-13-display-and-scrman-inventory.md | `get_screen_player_list` (Haiku) | `ScreenManager::get_channel_manager_keys` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7ffdd0 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_find_player_slot_value` (Haiku) | `ScreenManager::is_manager_away` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7ffe40 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_find_slot_value_by_key` (Haiku) | `ScreenManager::is_manager_done` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7ffeb0 | Named in 2026-10-13-display-and-scrman-inventory.md | `network_set_slot_value_by_id` (Haiku) | `ScreenManager::set_manager_done` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7fff20 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_slot_free_by_id` (Haiku) | `ScreenManager::is_player_done` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7fff70 | Named in 2026-10-13-display-and-scrman-inventory.md | `count_inactive_or_unset_slots` (Haiku) | `ScreenManager::count_idle_channels` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x7fffa0 | Named in 2026-10-13-display-and-scrman-inventory.md | `find_slot_by_id` (Haiku) | `ScreenManager::find_manager_channel` | agrees in area. The display and scrman research calls the record key a screen key. The sweep reads it as a manager key |
| 0x800000 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_fill_buffer_or_fail` (Haiku) | `ScreenManager::send_clear_click` | differs. The research says opcode 0x11 is a redraw request. The sweep names it a clear click. Not checked |

#### Wave w28 (tier 5, 2026-10-17)

Run `sweep-w28`: 8 Opus agents named 223 functions: `player_search.cpp` (`PLAYER_SEARCH`), `player_stats.cpp` (`PlayerStats`), `plot.cpp` (`Plot`), `printouts.cpp`, `record_utils.cpp`, `scout_manager.cpp` (`SCOUT_MANAGER`), `scrman.cpp` (`ScreenManager`), `search_edit_session.cpp` (`SearchEditSession`), `search_eng.cpp` (`SearchEngine`) and three `search_filters.cpp` gap functions (`SearchFilters`). Each agent took 2 to 6 minutes and 140,000 to 187,000 tokens. Confidence: 132 high, 90 medium, 1 low.

The merge needed one fix. Three false entries had a `_tail` suffix. They now use the `<parent>_fragment_<addr>` style. Two agents named the `search_edit_session.cpp` functions and both chose `SearchEditSession`. The scrman agents name most network sends after the opcode that the client applies, for example `set_widget_rect` for sub-operation 8 and `play_sound` for opcode 0x0a.

47 functions of the wave are cited. 35 agree and 12 agree in area. None differs.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x7af160 | Named in build-brief-0.5.0.md | `Search_Engine::refresh_results` (Haiku) | `PLAYER_SEARCH::get_result_list` | agrees in area |
| 0x7afa70 | Named in 2026-10-16-crash-fixes.md | `Search_Engine::collect_candidates` (Haiku) | `PLAYER_SEARCH::collect_known_people` | agrees |
| 0x7bfd50 | Named in 2026-10-14-game-creation-inventory.md | `player_stats_load_from_cd` (Haiku) | `PlayerStats::PlayerStats` | agrees |
| 0x7c1350 | Named in 2026-10-16-research-name-check.md | `player_stats_refresh_nation_players` (Haiku) | `PlayerStats::end_season_for_key_nation` | agrees in area |
| 0x7c1660 | Named in 2026-10-14-columns-and-colours-inventory.md | `PLAYER_STATS::format_ability_string` (Haiku) | `PlayerStats::format_recent_ratings` | agrees |
| 0x7c25b0 | Named in 2026-10-16-comparison-screen-inventory.md | `player_stats_format_name_or_error` (Haiku) | `PlayerStats::format_player_stat` | agrees |
| 0x7c6a30 | Named in 2026-10-14-widescreen-inventory.md | `player_stats_list::append_record` (Haiku) | `PlayerStats::add_language` | agrees in area |
| 0x7d7c80 | Named in print_pdf.c | `printouts_write_report` (Haiku) | `printouts_print_comp_report` | agrees |
| 0x7f6760 | Named in 2026-10-14-game-creation-inventory.md | `scout_manager_init_and_load_cd_check` (Haiku) | `SCOUT_MANAGER::SCOUT_MANAGER` | agrees |
| 0x7f7780 | Named in 2026-10-16-comparison-screen-inventory.md | `scout_manager_set_slot` (Haiku) | `SCOUT_MANAGER::set_compare_player` | agrees |
| 0x7f7920 | Named in 2026-10-16-comparison-screen-inventory.md | `scout_manager_get_rating_slot` (Haiku) | `SCOUT_MANAGER::get_compare_player` | agrees |
| 0x7f8610 | Named in 2026-10-16-comparison-screen-inventory.md | `scout_manager_find_scout_slot_for_player` (Haiku) | `SCOUT_MANAGER::find_club_search_entry` | agrees in area |
| 0x7f8d70 | Named in 2026-10-16-crash-fixes.md | `scout_manager_check_scout_report_visible` (Haiku) | `SCOUT_MANAGER::has_scout_request` | agrees |
| 0x7feb20 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_set_search_criteria` (Haiku) | `ScreenManager::set_default_screen` | agrees |
| 0x7feb70 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_count_empty_slots` (Haiku) | `ScreenManager::count_active_records` | agrees in area |
| 0x7fed30 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_get_slot_ptr_or_fatal` (Haiku) | `ScreenManager::get_current_screen_flag` | agrees in area |
| 0x7fedd0 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_check_active_slot_or_abort` (Haiku) | `ScreenManager::current_screen_has_exit_fn` | agrees |
| 0x7fee80 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_get_active_slot_ptr` (Haiku) | `ScreenManager::get_screen_title` | agrees |
| 0x7feeb0 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_copy_name_into_slot` (Haiku) | `ScreenManager::set_screen_title` | agrees |
| 0x7fef10 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_free_list_clear` (Haiku) | `ScreenManager::free_all_screens` | agrees |
| 0x7ff130 | Named in 2026-10-13-display-and-scrman-inventory.md | `ScreenManager::iterate_order_list_filtered` (Haiku) | `ScreenManager::set_screen_param_for_each_record` | agrees |
| 0x800160 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_build_message_packet` (Haiku) | `ScreenManager::set_pending_click_widget` | agrees in area |
| 0x800330 | Named in 2026-10-13-display-and-scrman-inventory.md | `Net_Buffer::write_packet_fields` (Haiku) | `ScreenManager::send_area_scroll_row` | agrees in area |
| 0x800580 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::queue_message_packet` (Haiku) | `ScreenManager::set_widget_flags` | agrees |
| 0x800890 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::send_substitution_packet` (Haiku) | `ScreenManager::set_widget_rect` | agrees |
| 0x800d30 | Named in 2026-10-13-display-and-scrman-inventory.md | `scr_write_net_message_packet` (Haiku) | `ScreenManager::set_widget_style` | agrees |
| 0x801040 | Named in 2026-10-13-display-and-scrman-inventory.md | `Screen_Session::queue_message_packet` (Haiku) | `ScreenManager::set_widget_colour` | agrees |
| 0x801350 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_queue_session_message` (Haiku) | `ScreenManager::set_widget_text_flags` | agrees |
| 0x801660 | Named in 2026-10-13-display-and-scrman-inventory.md | `Net_message_write_header_and_send_match_packet` (Haiku) | `ScreenManager::set_widget_font` | agrees |
| 0x801960 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::queue_net_message_bytes` (Haiku) | `ScreenManager::set_widget_text_colour` | agrees |
| 0x801c70 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_build_and_send_packet` (Haiku) | `ScreenManager::set_widget_text` | agrees |
| 0x801fc0 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::queue_packet_and_send` (Haiku) | `ScreenManager::set_widget_id` | agrees |
| 0x8022d0 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::queue_net_message_bytes` (Haiku) | `ScreenManager::set_widget_data` | agrees |
| 0x8025e0 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_send_net_message` (Haiku) | `ScreenManager::play_sound` | agrees |
| 0x8029f0 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::get_player_value_or_fallback` (Haiku) | `ScreenManager::is_sound_playing` | agrees |
| 0x802a50 | Named in 2026-10-13-display-and-scrman-inventory.md | `scrman_search_edit_char_input` (Haiku) | `ScreenManager::stop_sound` | agrees |
| 0x802cb0 | Named in 2026-10-13-display-and-scrman-inventory.md | `set_slot_flag_and_out_byte` (Haiku) | `ScreenManager::on_remote_sound_started` | agrees in area |
| 0x802ce0 | Named in 2026-10-13-display-and-scrman-inventory.md | `Scrman::clear_slot_entry` (Haiku) | `ScreenManager::on_remote_sound_finished` | agrees in area |
| 0x802d00 | Named in 2026-10-13-display-and-scrman-inventory.md | `search_edit_session_fill_slots` (Haiku) | `ScreenManager::redraw_screen` | agrees |
| 0x803390 | Named in 2026-10-13-input-inventory.md | `forward_to_display_scroll_target` (Haiku) | `ScreenManager::press_scrollbar_key` | agrees |
| 0x8033e0 | Named in print_pdf.c | `scr_write_search_key_and_player_record` (Haiku) | `ScreenManager::begin_print_job` | agrees |
| 0x803710 | Named in print_pdf.c | `Message_Packet::write_string_packet` (Haiku) | `ScreenManager::add_print_line` | agrees |
| 0x803930 | Named in print_pdf.c | `print_buffer_flush_or_assert` (Haiku) | `ScreenManager::add_print_page_break` | agrees |
| 0x803a90 | Named in print_pdf.c | `scrman_write_search_entry_or_assert` (Haiku) | `ScreenManager::print_job` | agrees |
| 0x803d50 | Named in 2026-10-13-screen-stack-inventory.md | `sum_16x16_word_grid_at_0x7e6` (Haiku) | `ScreenManager::count_flagged_screens` | agrees |
| 0x803d80 | Named in 2026-10-13-display-and-scrman-inventory.md | `search_edit_session_advance_or_close` (Haiku) | `ScreenManager::close_all_screens` | agrees in area |
| 0x804160 | Named in 2026-10-13-screen-stack-inventory.md | `display_add_item_ten_args_thunk` (Haiku) | `ScreenManager::add_area_text_by_ref` | agrees in area |

#### Wave w29 (tier 5, 2026-10-17)

Run `sweep-w29`: 8 Opus agents named 199 functions: `search_filters.cpp` (`SearchFilters`), `search_screens.cpp`, `shortlist_manager.cpp` (`Shortlist_Manager`), `simulated_stats.cpp` (`SimulatedStats`), `squad_manager.cpp` (`SQUAD_MANAGER`), `stadium.cpp` (`Stadium`) and `staff_contracts.cpp` (`STAFF_CONTRACT`). Each agent took 1 to 6 minutes and 110,000 to 173,000 tokens. Confidence: 120 high, 78 medium, 1 low. From this wave on, the brief gives the `<parent>_fragment_<addr>` style for false entries, and the agents used it.

The merge needed one fix. One agent gave the 26 column sort comparators of `search_screens.cpp` the prefix `search_`. They now use the file prefix `search_screens_`, as the other free functions of the file do.

19 functions of the wave are cited. 7 agree and 12 agree in area. None differs.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x807450 | Named in 2026-10-14-columns-and-colours-inventory.md | `Search_Filters::player_matches_filter` (Haiku) | `SearchFilters::player_matches_filter` | agrees in area |
| 0x808690 | Named in 2026-10-14-columns-and-colours-inventory.md | `search_screens_build_player_search_panel` (Haiku) | `search_screens_build_player_search_screen` | agrees in area |
| 0x83a1b0 | Named in 2026-10-14-game-creation-inventory.md | `shortlist_manager_build_shortlists` (Haiku) | `Shortlist_Manager::create_initial_shortlists` | agrees |
| 0x8420e0 | Named in 2026-10-15-holiday-next-match-inventory.md | `shortlist_manager_remove_entries_for_player` (Haiku) | `Shortlist_Manager::collect_club_shortlist_entries` | agrees in area |
| 0x842180 | Named in 2026-10-16-foreign-and-tactics-positions.md | `shortlist_add_eligible_scouted_players` (Haiku) | `Shortlist_Manager::collect_interested_clubs` | agrees in area |
| 0x8423f0 | Named in 2026-10-14-widescreen-inventory.md | `shortlist_add_entry` (Haiku) | `Shortlist_Manager::add_shortlist_entry` | agrees in area |
| 0x844660 | Named in 2026-10-15-holiday-next-match-inventory.md | `shortlist_manager_refresh_slot_status` (Haiku) | `shortlist_manager_release_club_slot_targets` | agrees in area |
| 0x844780 | Named in 2026-10-14-widescreen-inventory.md | `shortlist_manager_score_candidate_for_club` (Haiku) | `staff_interest_in_job_at_club` | agrees in area |
| 0x84abe0 | Named in 2026-10-15-holiday-next-match-inventory.md | `shortlist_manager_refresh_club_shortlist` (Haiku) | `Shortlist_Manager::rebuild_club_shortlist` | agrees |
| 0x84aeb0 | Named in 2026-10-16-crash-fixes.md | `shortlist_manager_clear_slot_entries_for_player` (Haiku) | `Shortlist_Manager::clear_bid_markers_for_player` | agrees in area |
| 0x84b940 | Named in 2026-10-14-game-creation-inventory.md | `simulated_stats_build_ranking` (Haiku) | `SimulatedStats::SimulatedStats` | agrees |
| 0x859dc0 | Named in 2026-10-14-game-creation-inventory.md | `squad_manager_reset_registered_flags` (Haiku) | `SQUAD_MANAGER::init_new_game` | agrees |
| 0x85a470 | Named in 2026-10-16-crash-fixes.md | `squad_manager_assign_players_to_squad_slots` (Haiku) | `SQUAD_MANAGER::allocate_squad_number` | agrees in area |
| 0x85a9b0 | Named in 2026-10-16-research-name-check.md | `squad_manager_reset_player_flags` (Haiku) | `SQUAD_MANAGER::process_euro_registration` | agrees in area |
| 0x85b670 | Named in 2026-10-14-game-creation-inventory.md | `find_cd_and_load_squad_data` (Haiku) | `Stadium::Stadium` | agrees |
| 0x85b820 | Named in 2026-10-14-game-creation-inventory.md | `squad_manager_release_transfer_slot` (Haiku) | `Stadium::daily_update` | agrees in area |
| 0x85d8a0 | Named in 2026-10-16-crash-fixes.md | `stadium_crowd_pull_score` (Haiku) | `Stadium::get_travel_distance_factor` | agrees in area |
| 0x85f7a0 | Named in 2026-10-15-contract-clauses.md | `STAFF_CONTRACT::set_release_clause_flag` (Haiku) | `STAFF_CONTRACT::add_release_clause` | agrees |
| 0x868020 | Named in 2026-10-15-contract-clauses.md | `STAFF_CONTRACT::format_release_clause_comment` (Haiku) | `STAFF_CONTRACT::get_release_clause_abbreviation` | agrees |

#### Wave w30 (tier 5, 2026-10-17)

Run `sweep-w30`: 8 Opus agents named 221 functions: `staff_records.cpp` (`Staff_Records`), `staff_screens.cpp`, `tactics.cpp` (`Tactics`), `tactics_screens.cpp` (`Tactics_Screen`), `tcpip.cpp` (`TcpIp`), `team_award.cpp` (`TEAM_AWARD`), `training_edit_session.cpp` (`TRAINING_EDIT_SESSION`), `training_manager.cpp` (`TRAINING_MANAGER`) and six `training_schedule.cpp` gap functions (`TRAINING_SCHEDULE`). Each agent took 2 to 8 minutes and 107,000 to 200,000 tokens. Confidence: 160 high, 60 medium, 1 low.

The merge needed no fixes. Two files keep a second class: `tactics_screens.cpp` has one earlier `Tactics_Team_Data` row (the team block inside `Tactics_Screen`), and `training_manager.cpp` holds five `TRAINING_SCHEDULE` methods beside `TRAINING_MANAGER`. The training agent notes that 0x8b1750 is now `TRAINING_MANAGER::update_player_training`, so the comparison screen inventory and `compare_screen.c` use an older name.

35 functions of the wave are cited. 20 agree and 15 agree in area. None differs.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x868390 | Named in 2026-10-14-game-creation-inventory.md | `staff_records_load_cd_and_init` (Haiku) | `Staff_Records::Staff_Records` | agrees |
| 0x86cd70 | Named in 2026-10-16-crash-fixes.md | `staff_records_award_shortlist_update` (Haiku) | `Staff_Records::add_transfer_records` | agrees in area |
| 0x86d6e0 | Named in 2026-10-16-research-name-check.md | `staff_records_remove_player_from_teams` (Haiku) | `Staff_Records::end_season_for_nation` | agrees in area |
| 0x86dda0 | Named in 2026-10-14-assistant-and-days-inventory.md | `staff_records_reset_slots_for_date` (Haiku) | `Staff_Records::daily_update` | agrees |
| 0x8702a0 | Named in 2026-10-14-clickable-names-inventory.md | `assert_staff_player_and_open_view` (Haiku) | `staff_screens_open_staff_profile` | agrees |
| 0x870650 | Named in 2026-10-16-comparison-screen-inventory.md | `staff_screens_check_build_and_quit_on_mismatch` (Haiku) | `staff_screens_open_compare_players_screen` | agrees |
| 0x870700 | Named in 2026-10-14-clickable-names-inventory.md | `Staff_Screens::build_player_context_menu` (Haiku) | `staff_screens_build_profile_screen` | agrees |
| 0x87a510 | Named in 2026-10-14-columns-and-colours-inventory.md | `draw_player_profile_panel` (Haiku) | `staff_screens_draw_profile_tab` | agrees |
| 0x87dd80 | Named in 2026-10-14-widescreen-inventory.md | `staff_screens_player_detail_report` (Haiku) | `staff_screens_draw_transfer_tab` | agrees in area |
| 0x888260 | Named in 2026-10-14-clickable-names-inventory.md | `staff_screens_handle_action` (Haiku) | `staff_screens_handle_profile_action` | agrees |
| 0x889c30 | Named in 2026-10-16-comparison-screen-inventory.md | `staff_screens_dispatch_action` (Haiku) | `staff_screens_handle_compare_action` | agrees |
| 0x88b2f0 | Named in 2026-10-16-comparison-screen-inventory.md | `staff_screens_check_squad_slot_limits` (Haiku) | `staff_screens_handle_profile_tab_action` | agrees in area |
| 0x88b8a0 | Named in 2026-10-14-clickable-names-inventory.md | `staff_screens_release_screen_slots` (Haiku) | `staff_screens_leave_profile_screen` | agrees |
| 0x893dc0 | Named in 2026-10-14-game-creation-inventory.md | `Tactics::load_tactics_files` (Haiku) | `Tactics::Tactics` | agrees |
| 0x896570 | Named in 2026-10-16-tactics-inventory.md | `tactics_get_record_ptr` (Haiku) | `Tactics::get_tactic_block` | agrees |
| 0x896660 | Named in 2026-10-15-holiday-next-match-inventory.md | `tactics_get_error_or_resolve_slot` (Haiku) | `Tactics::get_club_tactic` | agrees in area |
| 0x8968f0 | Named in 2026-10-14-assistant-and-days-inventory.md | `tactics_get_slot_player_record` (Haiku) | `Tactics::get_selected_player` | agrees in area |
| 0x896a90 | Named in 2026-10-14-assistant-and-days-inventory.md | `tactics_set_slot_value` (Haiku) | `Tactics::set_selected_player` | agrees in area |
| 0x896fe0 | Named in 2026-10-14-assistant-and-days-inventory.md | `Tactics::store_restore_club_slot` (Haiku) | `Tactics::clear_or_restore_selection` | agrees in area |
| 0x897250 | Named in 2026-10-15-squad-dropdown-inventory.md | `tactics_slot_matches` (Haiku) | `Tactics::has_selection_undo` | agrees in area |
| 0x897760 | Named in 2026-10-14-assistant-and-days-inventory.md | `Tactics::set_slot_tactic` (Haiku) | `Tactics::set_captain_from_slot` | agrees in area |
| 0x8983f0 | Named in 2026-10-15-assistant-pick-inventory.md | `tactics_copy_record_by_id` (Haiku) | `Tactics::copy_selection_record` | agrees |
| 0x899ea0 | Named in 2026-10-14-columns-and-colours-inventory.md | `tactics_screen_build_team_instructions` (Haiku) | `tactics_screen_build` | agrees |
| 0x89f710 | Named in 2026-10-14-widescreen-inventory.md | `tactics_formation_dialog_handler` (Haiku) | `tactics_screen_handle_event` | agrees in area |
| 0x8a2ca0 | Named in 2026-10-14-clickable-names-inventory.md | `tactics_screen_init_lists` (Haiku) | `Tactics_Screen::~Tactics_Screen` | agrees in area |
| 0x8a9950 | Named in 2026-10-16-tactics-inventory.md | `tactics_screens_set_formation_from_name` (Haiku) | `Tactics_Screen::set_preset_formation` | agrees |
| 0x8aa9d0 | Named in 2026-10-14-columns-and-colours-inventory.md | `tactics_lookup_slot_player` (Haiku) | `Tactics_Screen::get_slot_pitch_position` | agrees |
| 0x8aadb0 | Named in 2026-10-14-columns-and-colours-inventory.md | `substitution_minute_text` (Haiku) | `Tactics_Screen::get_slot_match_status_text` | agrees |
| 0x8aafd0 | Named in 2026-10-14-columns-and-colours-inventory.md | `tactics_screen_injury_or_player_slot_status` (Haiku) | `Tactics_Screen::get_slot_condition` | agrees in area |
| 0x8ab150 | Named in 2026-10-14-columns-and-colours-inventory.md | `tactics_get_player_slot_byte` (Haiku) | `Tactics_Screen::get_slot_match_rating` | agrees |
| 0x8ab260 | Named in 2026-10-14-columns-and-colours-inventory.md | `tactics_screens_get_player_slot` (Haiku) | `Tactics_Screen::get_slot_goals` | agrees |
| 0x8ab480 | Named in 2026-10-14-columns-and-colours-inventory.md | `tactics_get_slot_player_record` (Haiku) | `Tactics_Screen::get_slot_player` | agrees |
| 0x8b0ec0 | Named in 2026-10-14-game-creation-inventory.md | `training_manager::wait_for_game_cd` (Haiku) | `TRAINING_MANAGER::TRAINING_MANAGER` | agrees |
| 0x8b1750 | Named in 2026-10-15-features-0.4.0-spec.md | `Training_Manager::apply_daily_training` (Haiku) | `TRAINING_MANAGER::update_player_training` | agrees in area |
| 0x8b37c0 | Named in 2026-10-14-columns-and-colours-inventory.md | `training_manager_apply_stat_delta` (Haiku) | `TRAINING_MANAGER::get_training_attribute_change` | agrees in area |

#### Wave w31 (tier 5, the last game files, 2026-10-17)

Run `sweep-w31`: 3 Opus agents named the last 76 game functions: `training_schedule.cpp`, `training_screens.cpp`, `ultimatum.cpp`, `utils.cpp`, `virtual_staff.cpp` (`VIRTUAL_STAFF`), `weather.cpp` (`Weather`), `year_award.cpp` (`YEAR_AWARD`), `year_ratings.cpp` (`YEAR_RATINGS`) and `zipdir.cpp`. Each agent took 3 to 4 minutes and 142,000 to 150,000 tokens. Confidence: 57 high, 19 medium, none low. With this wave every game function of tiers 1 to 5 has an Opus name.

The merge needed no fixes. `training_screens.cpp` holds one destructor each of `TRAINING_EDIT_SESSION` and `TRAINING_SCHEDULE`.

12 functions of the wave are cited. 8 agree and 4 agree in area. None differs.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x90ba90 | Named in 2026-10-13-display-and-scrman-inventory.md | `assert_swap_bytes` (Haiku) | `utils_swap_bytes` | agrees in area |
| 0x90bc50 | Named in 2026-10-14-import-inventory.md | `get_user_folder_path` (Haiku) | `utils_get_folder_for_key` | agrees in area |
| 0x90bea0 | Named in 2026-10-16-tactics-inventory.md | `get_user_folder` (Haiku) | `utils_get_user_folder` | agrees |
| 0x90c120 | Named in 2026-10-14-start-screen-helpers.md | `utils_is_zero_result` (Haiku) | `utils_file_exists` | agrees |
| 0x90c140 | Named in 2026-10-16-tactics-inventory.md | `read_checked_value_from_stream` (Haiku) | `utils_read_file_format_marker` | agrees |
| 0x90c510 | Named in 2026-10-13-display-and-scrman-inventory.md | `crt_system_time_thunk` (Haiku) | `utils_get_time_ms` | agrees in area |
| 0x90cba0 | Named in 2026-10-14-start-screen-helpers.md | `read_config_line` (Haiku) | `utils_read_config_line` | agrees |
| 0x90cc50 | Named in 2026-10-14-start-new-game-inventory.md | `copy_file_contents` (Haiku) | `utils_copy_file` | agrees |
| 0x90cd50 | Named in 2026-10-14-game-creation-inventory.md | `copy_cm3_temp_and_scan_dir` (Haiku) | `utils_clear_temp_folder` | agrees |
| 0x90d0a0 | Named in 2026-10-14-assistant-and-days-inventory.md | `seed_random_generator` (Haiku) | `utils_seed_random` | agrees |
| 0x90d140 | Named in 2026-10-14-game-creation-inventory.md | `find_cd_and_load_config` (Haiku) | `VIRTUAL_STAFF::VIRTUAL_STAFF` | agrees |
| 0x90e760 | Named in 2026-10-14-widescreen-inventory.md | `virtual_staff_slot_coords` (Haiku) | `VIRTUAL_STAFF::get_id_block` | agrees in area |

#### Wave lib-a (tier 6, Sonnet, 2026-10-17)

Run `sweep-lib-a`: 5 Sonnet agents named the first 125 of the 250 library functions without asserts (0x933a00 to 0x939f50). Max chose Sonnet for tier 6 on 2026-10-17, so these rows carry `sonnet` in the model column. Each agent took 1 to 3 minutes and 98,000 to 112,000 tokens. Confidence: 63 high, 45 medium, 17 low.

The agents found that this part of the library is font code: a glyph hinter that works in 26.6 fixed point, and a TrueType bytecode interpreter. One agent decoded the opcode table that `tt_init_opcode_table` (0x9375d0) fills, and the handler names follow the TrueType instruction in each slot. The Haiku names (match engine, audio, zlib, PE relocation) were all wrong. 0x933a00 is the one zip helper, `zipdir_match_filename_pattern`. No string confirms that the font code is TrueType, so the agents give low or medium confidence where only the fixed-point formats and the table order support the name.

The merge needed one fix. The agents used five prefixes for two pieces of code. The rows now use `glyph_hint_` for the hinter context functions, `tt_` for the interpreter and `tt_op_<opcode>` for the opcode handlers.

No function of the run is cited.

#### Wave lib-b (tier 6, Sonnet, 2026-10-17)

Run `sweep-lib-b`: 5 Sonnet agents named the other 125 library functions (0x93a190 to 0x944c10). Each agent took 1 to 3 minutes and 112,000 to 150,000 tokens. Confidence: 63 high, 59 medium, 3 low. With this run every row of `names.tsv` has an Opus or Sonnet name: 6,613 opus and 250 sonnet.

This part of the library is the rest of the font code. It holds the remaining TrueType opcode handlers, the font file loader (`tt_font_*`, `tt_read_*`), the glyph outline object, the byte stream (`input_stream_*`), a memory context (`lib_ctx_*`) and the T2K scaler with its scan converter (`t2k_*`). One agent read the stream tag T2KG (0x54324b47) and the game's `FONT` class as the only caller, and the HD text research also names the T2K engine. 0x944c10 is the `EnumPrintersA` import thunk.

The merge needed one fix. One agent named five stream functions `t2k_stream_*`, while another named the same 0x1d-byte stream record `input_stream_*`. All now use `input_stream_`. One fragment now takes its parent's name `t2k_scaler_new`.

3 functions of the run are cited. 1 agrees and 2 agree in area. None differs.

| Address | Earlier research | Earlier name | Opus name | Result |
|---|---|---|---|---|
| 0x93de70 | Named in 2026-10-16-hd-text.md | `map_view_init_with_setjmp_guard` (Haiku) | `t2k_new_transformation` | agrees in area |
| 0x93fa90 | Named in 2026-10-16-hd-text.md | `lib_setjmp_guarded_cleanup` (Haiku) | `t2k_delete_t2k` | agrees in area |
| 0x944c10 | Named in 2026-10-14-import-inventory.md | `enum_printers_a_import_thunk` (Haiku) | `EnumPrintersA_import_thunk` | agrees |

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

### Review pass

Max chose the cheap review pass on 2026-10-17. A fresh Opus reviewer checks each row in scope against its dossier, and a fresh Opus third check decides each change of name, class or file. The brief is `sweep/BRIEF-review.md` and the plan is `sweep/REVIEW-PLAN.md` in the work folder. Waves r01 to r04 review the 430 hard rows and a sample of 200 medium rows.

#### Review r01 (tiers 1 to 3, 2026-10-17)

Run `review-r01`: 8 Opus reviewers checked 191 rows of the rule books, the international competitions, comp.cpp and the transfer files. Agree 107, fix 74, rename 10. The third check took the reviewer's name 9 times and wrote a new row once. 32 false entries got the name `<parent>_fragment_<addr>`. 114 rows changed in all.

Tokens: the 8 reviewers took 110,000 to 179,000 tokens each, about 140,000 on average and 1.12 million in all. Each took 3 to 5.5 minutes. The third check took 108,000 tokens. The wave took about 1.23 million tokens.

Most fixes raise a row to high confidence with a quoted vtable slot or update an old caller name in the evidence. The renames are mostly shared stubs that the sweep named after one slot. For example, 0x48ce70 and 0x48ce90 also have direct cdecl calls from `match_eng_choose_kit`, and 0x4011f0 is also slot 2 of the ruling body base vtable. The 20 "shared stub" rows from 0x585920 to 0x586180 are false entries: their two "pointers" are import table entries. transfer_offer.cpp keeps two classes, because `TRANSFER_OFFER` (no vtable) and `TRANSFER_RULES` (vtable 0x970458) are real classes.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x4011f0 | `League::set_fixed_draw_slot` | `shared_noop_ret8` | takes reviewer | `shared_noop_ret8` |
| 0x5839d0 | `comp_stub_return_zero` | `shared_return_zero_ret8` | takes reviewer | `shared_return_zero_ret8` |
| 0x776f70 | `national_teams_save_dat_error_tail` | `NationalTeams::save_dat_fragment_776f70` | takes reviewer | `NationalTeams::save_dat_fragment_776f70` |
| 0x48ce70 | `Comp::skip_season_reset` | `shared_return_zero` | takes reviewer | `shared_return_zero` |
| 0x48ce90 | `Comp::set_up_rules` | `shared_return_one` | takes reviewer | `shared_return_one` |
| 0x48cea0 | `Comp::save_extra_data` | `shared_noop_ret4_48cea0` | takes reviewer | `shared_noop_ret4_48cea0` |
| 0x48ceb0 | `Comp::load_extra_data` | `shared_return_zero_ret4` | takes reviewer | `shared_return_zero_ret4` |
| 0x48e360 | `Comp::allows_fixture_clash` | `Comp::counts_for_fixture_clash` | new name | `Comp::counts_for_fixture_clash` |
| 0x5af5b0 | `fix_man_get_partner_nation_type` | `fix_man_get_partner_key_nation_index` | takes reviewer | `fix_man_get_partner_key_nation_index` |
| 0x404480 | `ArgPrm::has_relegation` | `shared_return_true` | takes reviewer | `shared_return_true` |

#### Review r02 (tiers 4 and 5, 2026-10-17)

Run `review-r02`: 8 Opus reviewers checked 195 rows of the national leagues, cups and awards, Cup.cpp, Database.cpp and the first screen files. Agree 113, fix 70, rename 12. The third check took the reviewer's name 10 times and wrote a new row twice. 40 false entries got the name `<parent>_fragment_<addr>`. 120 rows changed in all. The reviewers took 134,000 to 179,000 tokens each, and the third check took 111,000.

The sweep's errors fall into three kinds. Four rows were thiscall members written as free functions (ItaSerA 0x641fc0 and 0x6421c0, NationAwards 0x7698f0 and 0x769af0). Two slot overrides took a name from their callees and not from the slot: 0x519890 is Cup slot 17 `update_staff_records_for_status` and does no finance work, and 0x522c50 is slot 33 `update_projected_positions`. The rest were generic names that the code makes exact, such as `Cash::Cash` (0x450a90). Two reviewers gave two different single `ret` bodies the same name, so the third check added the address to each.

Open point: the other Cup slot 17 overrides (about 10 rows, for example `UefaCup::apply_team_result_finance` 0x9030e0) still carry the finance name. They are outside the sample.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x641fc0 | `ita_ser_a_exchange_serie_c2_clubs_unplayed` | `ItaSerA::exchange_serie_c2_clubs_unplayed` | takes reviewer | `ItaSerA::exchange_serie_c2_clubs_unplayed` |
| 0x6421c0 | `ita_ser_a_exchange_serie_d_clubs` | `ItaSerA::exchange_serie_d_clubs` | takes reviewer | `ItaSerA::exchange_serie_d_clubs` |
| 0x7698f0 | `nation_awards_award_team_of_season` | `NationAwards::award_team_of_season` | takes reviewer | `NationAwards::award_team_of_season` |
| 0x769af0 | `nation_awards_award_tournament_team` | `NationAwards::award_tournament_team` | takes reviewer | `NationAwards::award_tournament_team` |
| 0x794220 | `NorwayAwards::init_awards_month_awards_tail` | `NorwayAwards::init_awards_fragment_794220` | takes reviewer | `NorwayAwards::init_awards_fragment_794220` |
| 0x519890 | `Cup::apply_team_result_finance` | `Cup::update_staff_records_for_status` | takes reviewer | `Cup::update_staff_records_for_status` |
| 0x522c50 | `Cup::team_position` | `Cup::update_projected_positions` | takes reviewer | `Cup::update_projected_positions` |
| 0x5a8f60 | `pre_exit_noop` | `shared_noop_ret0` | new name | `shared_noop_ret0_5a8f60` |
| 0x450a90 | `cash_set_value` | `Cash::Cash` | takes reviewer | `Cash::Cash` |
| 0x486e60 | `compare_players_by_contract_check_then_name` | `compare_players_by_fifa_protected_period_then_name` | takes reviewer | `compare_players_by_fifa_protected_period_then_name` |
| 0x49f2a0 | `date_pair_record_construct` | `comp_unbeaten_run_record_construct` | takes reviewer | `comp_unbeaten_run_record_construct` |
| 0x5a53c0 | `noop_destructor` | `shared_noop_ret0` | new name | `shared_noop_ret0_5a53c0` |

#### Review r03 (tier 5, 2026-10-17)

Run `review-r03`: 8 Opus reviewers checked 213 rows of league.cpp, the match engine, media, news, the screens and the staff files. Agree 156, fix 45, rename 12. The third check took the reviewer's name in all 12 rows. 6 false entries got the name `<parent>_fragment_<addr>`. 63 rows changed in all. The reviewers took 111,000 to 196,000 tokens each, and the third check took 118,000.

Eight of the 12 renames correct the convention. Ghidra drops ECX in short bodies and forwarders, and the sweep took that output as given. The news.cpp rows 0x7861e0, 0x7863f0 and 0x786780 are now `News::` members with ret 8, ret 8 and ret 4, as the w27 agent found. news.cpp keeps two classes: `NewsItem` is a 0xdf-byte value object without a vtable, and `News` is the inbox store at 0xb7431c. Two reviewers found this on their own.

Open point: the forwarders 0x6cfcd0, 0x6cfd10, 0x6cfd40 and 0x6cfd70 have the same shape as 0x6cfc60 and 0x6cfca0, but keep free `match_eng_referee_*` names. They are outside the sample.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x6831a0 | `League::move_club_to_unplayed_division` | `League::relegate_club_to_division` | takes reviewer | `League::relegate_club_to_division` |
| 0x68aa80 | `League::review_club_managers` | `League::set_club_season_expectations` | takes reviewer | `League::set_club_season_expectations` |
| 0x6cfc60 | `match_eng_referee_judge_tackle` | `CM3_MATCH_ENGINE::referee_judge_tackle` | takes reviewer | `CM3_MATCH_ENGINE::referee_judge_tackle` |
| 0x6cfca0 | `match_eng_referee_check_foul` | `CM3_MATCH_ENGINE::referee_check_foul` | takes reviewer | `CM3_MATCH_ENGINE::referee_check_foul` |
| 0x70a8c0 | `is_free_run_action_code` | `MATCH_PLAYER::is_free_run_action_code` | takes reviewer | `MATCH_PLAYER::is_free_run_action_code` |
| 0x7861e0 | `news_send_to_club_manager` | `News::send_to_club_manager` | takes reviewer | `News::send_to_club_manager` |
| 0x7863f0 | `news_send_to_nation_managers` | `News::send_to_nation_managers` | takes reviewer | `News::send_to_nation_managers` |
| 0x786780 | `news_send_to_all_managers` | `News::send_to_all_managers` | takes reviewer | `News::send_to_all_managers` |
| 0x7daf50 | `apply_match_incident_ban` | `apply_obscene_gesture_ban` | takes reviewer | `apply_obscene_gesture_ban` |
| 0x803f80 | `display_add_screen_subtitle` | `ScreenManager::add_screen_subtitle` | takes reviewer | `ScreenManager::add_screen_subtitle` |
| 0x819870 | `search_screens_compare_results_by_field_2b` | `search_screens_compare_results_by_status_code` | takes reviewer | `search_screens_compare_results_by_status_code` |
| 0x846920 | `staff_interest_in_joining_club` | `Shortlist_Manager::staff_interest_in_joining_club` | takes reviewer | `Shortlist_Manager::staff_interest_in_joining_club` |

#### Review r04 (tier 6, 2026-10-17)

Run `review-r04`: 1 Opus reviewer checked the 31 library rows in scope (the Sonnet font code). Agree 17, fix 12, rename 2. The third check took the reviewer's name in both rows. The 11 false entries are setjmp return points inside their parents, and they already had the right fragment names. 14 rows changed, and the changed Sonnet rows are now `opus`. The reviewer took 131,000 tokens, and the third check took 75,000.

The sweep missed two function pointers: `FONT::set_style_and_size` stores 0x93d870 and 0x93d820 as a bold style pair, so 0x93d820 widens the advance widths. The TrueType opcode table and the round state table raised six rows to high.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x936510 | `glyph_hint_row_and_get_advance` | `glyph_hint_outline_and_get_advance` | takes reviewer | `glyph_hint_outline_and_get_advance` |
| 0x93d820 | `glyph_outline_embolden_widths` | `font_embolden_advance_widths` | takes reviewer | `font_embolden_advance_widths` |

#### Gate after r01 to r04 (2026-10-17)

`review.py gate`: all 200 sample rows were reviewed. 7 of them (3.5%) got a new name from the third check, and 83 got a field fix. This is under the 10% line (20 rows), so the plan stops here and does not review the other 1,372 medium rows (r05 to r11). Max decides.

Over the four waves, 630 rows were reviewed and 311 rows changed: 36 new names from the third check, 78 false entries renamed to `<parent>_fragment_<addr>`, and the rest field fixes. The third check kept the sweep name in none of the 36 disagreements. It took the reviewer's row 33 times and wrote a new row 3 times. The hard rows hold most of the errors: shared stubs named after one slot, and thiscall members that Ghidra showed without ECX. Confidence in `names.tsv` is now 5,217 high, 1,618 medium and 28 low.

Two open points are outside the sample: the other Cup slot 17 overrides with the finance name (r02), and the four `match_eng_referee_*` forwarders (r03).

#### Review r05 (medium rows of tiers 1 to 3, 2026-10-17)

Max chose on 2026-10-17 to review the other 1,372 medium rows too, although the gate was under the line. Run `review-r05`: 8 Opus reviewers checked 184 medium rows of the rule books, ruling bodies, international competitions and national teams. Agree 72, fix 103, rename 9. The third check took the reviewer's name in all 9 rows. 3 false entries got the fragment name. 114 rows changed in all. The reviewers took 129,000 to 170,000 tokens each, and the third check took 102,000.

Most fixes raise a row to high with a vtable slot that the reviewer read from the exe, or replace an old caller name in the evidence. The renames are two thiscall members of `ConmebolSeeding`, two more slot 17 overrides with the finance name (EuropeanCup and UefaCup), and four constructors and destructors (`TcpIp`, `NetworkBuffer`, `UefaSeeding`). national_teams_screens.cpp now holds `Network`, `TcpIp` and `NetworkBuffer`, and its assert path names network.CPP.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x4c6730 | `find_nth_unqualified_club_of_nation` | `ConmebolSeeding::find_nth_unqualified_club_of_nation` | takes reviewer | `ConmebolSeeding::find_nth_unqualified_club_of_nation` |
| 0x4c6890 | `is_mercosur_invited_club` | `ConmebolSeeding::is_mercosur_invited_club` | takes reviewer | `ConmebolSeeding::is_mercosur_invited_club` |
| 0x584ae0 | `EuropeanCup::apply_team_result_finance` | `EuropeanCup::update_staff_records_for_status` | takes reviewer | `EuropeanCup::update_staff_records_for_status` |
| 0x778f60 | `national_teams_screens_open_squad_screen` | `national_teams_screens_open_squad_move_screen` | takes reviewer | `national_teams_screens_open_squad_move_screen` |
| 0x77a870 | `Network::release_socket_table` | `TcpIp::~TcpIp` | takes reviewer | `TcpIp::~TcpIp` |
| 0x77b630 | `net_buffer_init` | `NetworkBuffer::NetworkBuffer` | takes reviewer | `NetworkBuffer::NetworkBuffer` |
| 0x77b6e0 | `net_buffer_free` | `NetworkBuffer::~NetworkBuffer` | takes reviewer | `NetworkBuffer::~NetworkBuffer` |
| 0x9030e0 | `UefaCup::apply_team_result_finance` | `UefaCup::update_staff_records_for_status` | takes reviewer | `UefaCup::update_staff_records_for_status` |
| 0x903570 | `UefaSeeding::free_club_list` | `UefaSeeding::~UefaSeeding` | takes reviewer | `UefaSeeding::~UefaSeeding` |


#### Review r06 (medium rows of tier 3, 2026-10-17)

Run `review-r06`: 8 Opus reviewers checked 200 medium rows of the contract, fixture, game, player regen and transfer files and of some national leagues. Agree 105, fix 74, rename 21. The third check took the reviewer's name 19 times, kept the sweep name once and wrote a new row once. 94 rows changed in all. The reviewers took 133,000 to 203,000 tokens each, and the third check took 131,000.

Twelve renames make free functions in fix_man.cpp and transfer_manager.cpp into `FIXTURE_MANAGER::` and `TRANSFER_MANAGER::` members, because every caller sets ECX. Four `TRANSFER_MANAGER` rows said "contract offer" but handle loan offers: offer byte +0x1f set to 1 marks a loan. The third check kept two rows as members although one reviewer made them stdcall: a body that does not read ECX is still a member when the callers set ECX.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x4d44e0 | `contract_manager_get_renewal_job` | `contract_manager_get_offer_job` | takes reviewer | `contract_manager_get_offer_job` |
| 0x5ab3c0 | `fix_man_add_away_trip_injury_risk` | `FIXTURE_MANAGER::add_away_trip_illness_risk` | takes reviewer | `FIXTURE_MANAGER::add_away_trip_illness_risk` |
| 0x5ab510 | `fix_man_find_new_fixture_date` | `FIXTURE_MANAGER::find_new_fixture_date` | takes reviewer | `FIXTURE_MANAGER::find_new_fixture_date` |
| 0x5abb10 | `fix_man_reschedule_club_fixtures_near_date` | `FIXTURE_MANAGER::reschedule_club_fixtures_near_date` | takes reviewer | `FIXTURE_MANAGER::reschedule_club_fixtures_near_date` |
| 0x5ad780 | `fix_man_find_fixture_clashes` | `FIXTURE_MANAGER::find_fixture_clashes` | takes reviewer | `FIXTURE_MANAGER::find_fixture_clashes` |
| 0x5adba0 | `fix_man_collect_day_clashes` | `FIXTURE_MANAGER::collect_day_clashes` | takes reviewer | `FIXTURE_MANAGER::collect_day_clashes` |
| 0x5ae380 | `fix_man_resolve_same_day_clash` | `FIXTURE_MANAGER::resolve_same_day_clash` | takes reviewer | `FIXTURE_MANAGER::resolve_same_day_clash` |
| 0x5ae9d0 | `fix_man_move_clashing_club_fixtures` | `FIXTURE_MANAGER::move_nearby_club_fixtures` | takes reviewer | `FIXTURE_MANAGER::move_nearby_club_fixtures` |
| 0x5aee40 | `fix_man_get_club_first_fixture_year` | `FIXTURE_MANAGER::get_club_first_fixture_year` | takes reviewer | `FIXTURE_MANAGER::get_club_first_fixture_year` |
| 0x5aee60 | `fix_man_get_club_last_fixture_year` | `FIXTURE_MANAGER::get_club_last_fixture_year` | takes reviewer | `FIXTURE_MANAGER::get_club_last_fixture_year` |
| 0x5af2f0 | `fix_man_get_time_slice` | `FIXTURE_MANAGER::get_time_slice` | takes reviewer | `FIXTURE_MANAGER::get_time_slice` |
| 0x5cc830 | `Hall_of_Fame::~Hall_of_Fame` | `HallOfFame::~HallOfFame` | takes reviewer | `HallOfFame::~HallOfFame` |
| 0x5d6aa0 | `game_check_players_going_awol` | `game_check_players_missing_training` | takes reviewer | `game_check_players_missing_training` |
| 0x8bed60 | `TRANSFER_MANAGER::make_contract_offer` | `TRANSFER_MANAGER::make_loan_offer` | takes reviewer | `TRANSFER_MANAGER::make_loan_offer` |
| 0x8bee50 | `TRANSFER_MANAGER::add_contract_negotiation` | `TRANSFER_MANAGER::add_loan_negotiation` | takes reviewer | `TRANSFER_MANAGER::add_loan_negotiation` |
| 0x8bfa30 | `TRANSFER_MANAGER::reject_contract_offer` | `TRANSFER_MANAGER::reject_loan_offer` | takes reviewer | `TRANSFER_MANAGER::reject_loan_offer` |
| 0x8c0660 | `TRANSFER_MANAGER::accept_contract_offer` | `TRANSFER_MANAGER::accept_loan_offer` | takes reviewer | `TRANSFER_MANAGER::accept_loan_offer` |
| 0x8c6ae0 | `transfer_manager_detach_offer_players` | `TRANSFER_MANAGER::detach_offer_players` | takes reviewer | `TRANSFER_MANAGER::detach_offer_players` |
| 0x8c8fd0 | `TRANSFER_MANAGER::build_clause_offer_option` | `transfer_manager_build_clause_offer_option` | keeps first | `TRANSFER_MANAGER::build_clause_offer_option` |
| 0x8cdb80 | `transfer_manager_check_manager_veto` | `transfer_manager_check_board_veto` | new name | `TRANSFER_MANAGER::check_board_veto` |
| 0x8ce0e0 | `transfer_manager_set_player_squad_status` | `TRANSFER_MANAGER::set_player_squad_status` | takes reviewer | `TRANSFER_MANAGER::set_player_squad_status` |


#### Review r07 (medium rows of tier 4, 2026-10-17)

Run `review-r07`: 8 Opus reviewers checked 196 medium rows of the national leagues, cups and awards, Cup.cpp, Database.cpp and finance. Agree 97, fix 86, rename 13. The third check took the reviewer's name in all 13 rows. 99 rows changed in all. The reviewers took 127,000 to 195,000 tokens each, and the third check took 96,000.

Six renames make league and finance helpers into members, because the caller sets ECX to `this`. The other renames take their meaning from strings that the sweep did not read: the transfer status screen strings name the contract flag bits at +0x4e (loan listed, listed by request), and two langlib tables hold Norwegian and German player-type phrases.

Open point: the asserts of the Finance code name finance.CPP, but `names.tsv` files most Finance rows under fin_prm.cpp. Some rows say finance.cpp. The file of the Finance class needs one decision.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x553cb0 | `den_prm_refresh_second_division_clubs` | `DenPrm::refresh_second_division_clubs` | takes reviewer | `DenPrm::refresh_second_division_clubs` |
| 0x574e60 | `eng_prm_exchange_conference_clubs_unplayed` | `EngPrm::exchange_conference_clubs_unplayed` | takes reviewer | `EngPrm::exchange_conference_clubs_unplayed` |
| 0x5750a0 | `eng_prm_exchange_conference_feeder_clubs` | `EngPrm::exchange_conference_feeder_clubs` | takes reviewer | `EngPrm::exchange_conference_feeder_clubs` |
| 0x595f00 | `fin_prm_refresh_first_division_clubs` | `FinPrm::refresh_first_division_clubs` | takes reviewer | `FinPrm::refresh_first_division_clubs` |
| 0x596050 | `fin_prm_assign_first_division_groups` | `FinPrm::assign_first_division_groups` | takes reviewer | `FinPrm::assign_first_division_groups` |
| 0x59b7e0 | `FinanceTable::add_weekly_commercial_income` | `FinanceTable::add_weekly_merchandising_income` | takes reviewer | `FinanceTable::add_weekly_merchandising_income` |
| 0x59cf50 | `finance_refresh_all_wage_bills` | `FinanceTable::refresh_all_wage_bills` | takes reviewer | `FinanceTable::refresh_all_wage_bills` |
| 0x53b610 | `database_is_not_player_job` | `database_is_not_plain_player_job` | takes reviewer | `database_is_not_plain_player_job` |
| 0x544520 | `staff_is_listed_or_unsettled` | `staff_is_transfer_listed_or_requested` | takes reviewer | `staff_is_transfer_listed_or_requested` |
| 0x544590 | `staff_contract_has_flag_02` | `staff_is_loan_listed` | takes reviewer | `staff_is_loan_listed` |
| 0x544600 | `staff_contract_has_flag_08` | `staff_is_listed_by_request` | takes reviewer | `staff_is_listed_by_request` |
| 0x6820a0 | `langlib_inflect_noun_table_9eb328` | `langlib_inflect_norwegian_player_type` | takes reviewer | `langlib_inflect_norwegian_player_type` |
| 0x682150 | `langlib_inflect_noun_table_9eb7ac` | `langlib_inflect_german_player_type` | takes reviewer | `langlib_inflect_german_player_type` |


#### Review r08 (medium rows of tier 5, 2026-10-17)

Run `review-r08`: 8 Opus reviewers checked 208 medium rows of the award, club, comp, discipline, display, formation, friendly, human manager, injury and league files. Agree 152, fix 39, rename 17. The third check took the reviewer's name 16 times and kept the sweep row once, because both rows had the same name. 55 rows changed in all. The reviewers took 127,000 to 156,000 tokens each, and the third check took 111,000.

Eleven renames make free functions into members, because the body reads ECX or every caller loads it: six in friendly.cpp, two in human_manager.cpp, and rows of award_screens.cpp, cash and club_records.cpp. Two false entries were named as real functions. `League::pay_clubs` (0x683010) is `League::pay_tv_rights`, and 0x4a7f20 is `Date::operator==`.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x415600 | `award_screens_add_menu_area` | `ScreenManager::add_menu_area` | takes reviewer | `ScreenManager::add_menu_area` |
| 0x43f460 | `cash_round_to_display_unit` | `Cash::get_display_amount` | takes reviewer | `Cash::get_display_amount` |
| 0x4486c0 | `club_records_offer_breaks_fee_record` | `Club_Records::offer_breaks_fee_record` | takes reviewer | `Club_Records::offer_breaks_fee_record` |
| 0x4a7f20 | `id_pair_equals` | `Date::operator==` | takes reviewer | `Date::operator==` |
| 0x5563f0 | `Discipline::hold_bans_on_transfer` | `Discipline::hold_bans_for_loan` | takes reviewer | `Discipline::hold_bans_for_loan` |
| 0x5b3270 | `Formation::get_slots_in_position_order_loop` | `Formation::get_slots_in_position_order_fragment_5b3270` | takes reviewer | `Formation::get_slots_in_position_order_fragment_5b3270` |
| 0x5c2d40 | `friendly_set_preseason_windows` | `Friendly::set_preseason_windows` | takes reviewer | `Friendly::set_preseason_windows` |
| 0x5c45a0 | `friendly_candidate_has_nearby_date` | `Friendly::candidate_has_nearby_date` | takes reviewer | `Friendly::candidate_has_nearby_date` |
| 0x5c4680 | `friendly_pick_tour_nation` | `Friendly::pick_tour_nation` | takes reviewer | `Friendly::pick_tour_nation` |
| 0x5c57d0 | `friendly_schedule_assign_opponent` | `Friendly::schedule_assign_opponent` | takes reviewer | `Friendly::schedule_assign_opponent` |
| 0x5c6350 | `friendly_pick_replacement_invitee` | `Friendly::pick_replacement_invitee` | takes reviewer | `Friendly::pick_replacement_invitee` |
| 0x5c6710 | `Friendly::is_friendly_competition` | `Friendly::is_friendly_competition` | keeps first | `Friendly::is_friendly_competition` |
| 0x5cb9e0 | `friendly_remove_national_fixture` | `Friendly::remove_national_fixture` | takes reviewer | `Friendly::remove_national_fixture` |
| 0x5fe420 | `human_manager_is_fixture_watched` | `HUMAN_MANAGER::is_fixture_watched` | takes reviewer | `HUMAN_MANAGER::is_fixture_watched` |
| 0x5fe700 | `human_manager_can_watch_fixture` | `HUMAN_MANAGER::can_watch_fixture` | takes reviewer | `HUMAN_MANAGER::can_watch_fixture` |
| 0x683010 | `League::pay_clubs` | `League::pay_tv_rights` | takes reviewer | `League::pay_tv_rights` |
| 0x687470 | `League::init_table_row_after_assert` | `League::init_table_row_fragment_687470` | takes reviewer | `League::init_table_row_fragment_687470` |


#### Review r09 (medium rows of tier 5, 2026-10-17)

Run `review-r09`: 8 Opus reviewers checked 208 medium rows of the manager manager, the match day and match engine, the match player, the match officials, match stats, media and network.h. Agree 150, fix 48, rename 10. The third check took the reviewer's name in all 10 rows. 58 rows changed in all. The reviewers took 134,000 to 167,000 tokens each, and the third check took 99,000.

Seven renames correct the convention. Three are `MANAGER_MANAGER` members, and four are the referee forwarders 0x6cfcd0, 0x6cfd10, 0x6cfd40 and 0x6cfd70, which r03 left open. They are now `CM3_MATCH_ENGINE::` members. 0x6b6cc0 is `match_day_apply_post_match_player_changes`, because its main work copies morale and reputation back from the match record. Two Match_Stats rows now say match_stats.cpp, as the asserts do. The placement has no match_stats.cpp file, so the docs still show them under match_screens.cpp.

From wave r10 on, the brief prints the current row in run order. Its earlier grep hint sorted the runs by folder name and could print an old Haiku row. The merge check refuses a fix whose name differs from the current row, so no wrong row got in.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x69c540 | `best_manager_candidate_score` | `MANAGER_MANAGER::best_manager_candidate_score` | takes reviewer | `MANAGER_MANAGER::best_manager_candidate_score` |
| 0x69c7e0 | `post_board_interest_memo_news` | `MANAGER_MANAGER::post_board_interest_memo_news` | takes reviewer | `MANAGER_MANAGER::post_board_interest_memo_news` |
| 0x69f810 | `is_staff_suitable_for_job` | `MANAGER_MANAGER::is_staff_suitable_for_job` | takes reviewer | `MANAGER_MANAGER::is_staff_suitable_for_job` |
| 0x6b6cc0 | `match_day_generate_player_match_stories` | `match_day_apply_post_match_player_changes` | takes reviewer | `match_day_apply_post_match_player_changes` |
| 0x6cfcd0 | `match_eng_referee_decide_foul_punishment` | `CM3_MATCH_ENGINE::referee_decide_foul_punishment` | takes reviewer | `CM3_MATCH_ENGINE::referee_decide_foul_punishment` |
| 0x6cfd10 | `match_eng_referee_decide_booking` | `CM3_MATCH_ENGINE::referee_decide_booking` | takes reviewer | `CM3_MATCH_ENGINE::referee_decide_booking` |
| 0x6cfd40 | `match_eng_referee_decide_misconduct_card` | `CM3_MATCH_ENGINE::referee_decide_misconduct_card` | takes reviewer | `CM3_MATCH_ENGINE::referee_decide_misconduct_card` |
| 0x6cfd70 | `match_eng_referee_check_offside` | `CM3_MATCH_ENGINE::referee_check_offside` | takes reviewer | `CM3_MATCH_ENGINE::referee_check_offside` |
| 0x729bd0 | `match_stats_get_record_bucket` | `match_stats_get_record_bucket` | takes reviewer | `match_stats_get_record_bucket` |
| 0x72b160 | `match_stats_compare_events` | `match_stats_compare_events` | takes reviewer | `match_stats_compare_events` |


#### Review r10 (medium rows of tier 5, 2026-10-17)

Run `review-r10`: 8 Opus reviewers checked 207 medium rows of news, officials, player search, player stats, record utils, scouting, the screen manager, search, shortlists, squads and staff contracts. Agree 158, fix 33, rename 16. The third check took the reviewer's name in all 16 rows. 49 rows changed in all. The reviewers took 122,000 to 149,000 tokens each, and the third check took 117,000.

Twelve renames make free functions into members, because the callers load a global object into ECX: `PlayerStats` (0xb74350), `PunditList`, `ReserveComp` (0xb74434), `SCOUT_MANAGER` (0xb74448) and `ScreenManager` (0xb74450). The five `display_add_*` rows said that ECX was a Display, but the bodies read the channel at this+0x382a. 0x83b690 had its sort order reversed in the name: open slots sort first.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x7c3060 | `player_stats_format_totals_field` | `PlayerStats::format_totals_field` | takes reviewer | `PlayerStats::format_totals_field` |
| 0x7dab30 | `parse_quoted_token` | `PunditList::parse_quoted_token` | takes reviewer | `PunditList::parse_quoted_token` |
| 0x7e6380 | `schedule_reserve_fixtures` | `ReserveComp::schedule_reserve_fixtures` | takes reviewer | `ReserveComp::schedule_reserve_fixtures` |
| 0x7e6720 | `add_b_or_under21_fixture` | `ReserveComp::add_b_or_under21_fixture` | takes reviewer | `ReserveComp::add_b_or_under21_fixture` |
| 0x7e6a70 | `cancel_unplayable_reserve_fixtures` | `ReserveComp::cancel_unplayable_reserve_fixtures` | takes reviewer | `ReserveComp::cancel_unplayable_reserve_fixtures` |
| 0x7e6ba0 | `can_clubs_field_reserve_sides` | `ReserveComp::can_clubs_field_reserve_sides` | takes reviewer | `ReserveComp::can_clubs_field_reserve_sides` |
| 0x7fc310 | `scout_manager_clear_manager_lists` | `SCOUT_MANAGER::clear_manager_lists` | takes reviewer | `SCOUT_MANAGER::clear_manager_lists` |
| 0x803320 | `ScreenManager::get_area_selection` | `ScreenManager::get_area_scroll_row` | takes reviewer | `ScreenManager::get_area_scroll_row` |
| 0x803f30 | `display_add_screen_title` | `ScreenManager::add_screen_title` | takes reviewer | `ScreenManager::add_screen_title` |
| 0x803fd0 | `display_add_text_item` | `ScreenManager::add_text_item` | takes reviewer | `ScreenManager::add_text_item` |
| 0x804030 | `display_add_button_item` | `ScreenManager::add_button_item` | takes reviewer | `ScreenManager::add_button_item` |
| 0x8040a0 | `display_add_area_button` | `ScreenManager::add_area_button` | takes reviewer | `ScreenManager::add_area_button` |
| 0x804100 | `display_add_area_text` | `ScreenManager::add_area_text` | takes reviewer | `ScreenManager::add_area_text` |
| 0x8043c0 | `SearchEditSession::refresh_scout_player_slots` | `SearchEditSession::refresh_scout_assignments` | takes reviewer | `SearchEditSession::refresh_scout_assignments` |
| 0x83b360 | `shortlist_old_slot_record_ctor` | `ShortlistOldSlot::ShortlistOldSlot` | takes reviewer | `ShortlistOldSlot::ShortlistOldSlot` |
| 0x83b690 | `shortlist_compare_slots_filled_first` | `shortlist_compare_slots_open_first` | takes reviewer | `shortlist_compare_slots_open_first` |


#### Review r11 (medium rows of tiers 5 and 6, 2026-10-17)

Run `review-r11`: 6 Opus reviewers checked 169 medium rows of tactics, training, weather, the year ratings and the font library. Agree 107, fix 54, rename 8. The third check took the reviewer's name in all 8 rows. 62 rows changed in all. The reviewers took 120,000 to 158,000 tokens each, and the third check took 99,000.

The key player pair at Tactics record +0x56 is the playmaker, as the Team Instructions dialog says. Four weather.cpp rows are `Weather::` members, because every caller loads the Weather global 0xde2f74 into ECX. 12 library rows went to high on an opcode slot or a round state of the TrueType tables. Two hub rows used the file label `lib`. The reviewer changed it to `library_without_asserts` in a fix, which the merge refuses, so the part file keeps `lib`. The docs already show both rows under `library_without_asserts`.

| Address | First name | Reviewer name | Third check | Final name |
|---|---|---|---|---|
| 0x897dc0 | `Tactics::get_key_player` | `Tactics::get_playmaker` | takes reviewer | `Tactics::get_playmaker` |
| 0x897ee0 | `Tactics::set_key_player` | `Tactics::set_playmaker` | takes reviewer | `Tactics::set_playmaker` |
| 0x8bb6d0 | `training_screens_compare_by_ability_then_name` | `training_screens_compare_by_form_then_name` | takes reviewer | `training_screens_compare_by_form_then_name` |
| 0x9232c0 | `weather_get_temperature` | `Weather::get_temperature` | takes reviewer | `Weather::get_temperature` |
| 0x923970 | `weather_get_pitch_condition` | `Weather::get_pitch_condition` | takes reviewer | `Weather::get_pitch_condition` |
| 0x923cc0 | `weather_post_postponed_news` | `Weather::post_postponed_news` | takes reviewer | `Weather::post_postponed_news` |
| 0x924050 | `weather_post_abandoned_news` | `Weather::post_abandoned_news` | takes reviewer | `Weather::post_abandoned_news` |
| 0x934b10 | `glyph_hint_grid_fit_checked` | `glyph_hint_collect_stem_widths_checked` | takes reviewer | `glyph_hint_collect_stem_widths_checked` |

#### End result of the review pass (2026-10-17)

The pass reviewed all 2,002 rows in scope over 11 waves: 1,234 agree, 638 fix and 130 rename. The third check took the reviewer's row 124 times, wrote a new row 4 times and kept the sweep row twice. 81 false entries got the name `<parent>_fragment_<addr>`. 842 rows changed in all. Confidence in `names.tsv` is now 5,454 high, 1,381 medium and 28 low. 201 library rows are still `sonnet`, and the reviewed and changed ones are `opus`.

The medium rows (r05 to r11) had 105 new names from the third check in 1,372 rows, about 8%. The sample had 3.5%. Most fixes in all waves raise the confidence with a quoted vtable slot, string or caller, or replace an old caller name in the evidence.

The sweep made the same few errors throughout. It named shared stubs after one slot. It read Ghidra's argument list as the convention, so many members whose callers load a global object into ECX became free functions. It named some functions from their callees and not from their vtable slot. The reviewers made few errors. One reviewer took "the body does not read ECX" as proof of a free function, and the third check kept the member. Two reviewers gave two bodies the same name, because no reviewer sees the other batches.

A reviewer took 75,000 to 203,000 tokens, most of them 120,000 to 170,000, and 2 to 6 minutes. A third check took 75,000 to 131,000 tokens. A wave of 8 reviewers and one third check took about 1.2 to 1.4 million tokens.

Open points for a later pass, outside the rows in scope:
- Seven Cup slot 17 overrides still have the name `apply_team_result_finance`. Slot 17 is `update_staff_records_for_status`.
- 0x8e9080 `TRANSFER_RULES::post_deadline_news`: the third argument is the year as a short, not a date pointer.
- 0x8e7810 `TRANSFER_OFFER::init_contract_offer` sets the loan flag +0x1f, so it is probably `init_loan_offer`.
- 0x7feeb0 `ScreenManager::set_screen_title` probably sets the name of the background picture.
- 0x7c2750 `player_stats_format_record_field` and the `post_*_news` rows of manager_manager.cpp (for example 0x6ac700) are probably members, because their callers load ECX.
- Four key_nation.cpp menu rows (0x66a280, 0x66a350, 0x66a360, 0x66a430) are thiscall on a small stack struct, with free names.
- File questions: the Finance code asserts finance.CPP but sits under fin_prm.cpp, the Match_Stats block asserts match_stats.CPP but sits under match_screens.cpp, six `FILE_LLIST` rows sit under fifa_rankings.cpp, and some network.h rows lie in the display.cpp and scrman.cpp ranges. The placement decides these files, so they need one decision for all.

#### Review fixes (2026-10-17)

One Opus check of the open points above, against the dossiers and the disassembly. The run `review-fix` changes 32 rows.

| Address | Old name | New name | Evidence |
|---|---|---|---|
| 0x40c5b0, 0x40e260, 0x56bee0, 0x633c10, 0x88e2f0, 0x90fbf0, 0x915710 | `<Cup>::apply_team_result_finance` | `<Cup>::update_staff_records_for_status` | Each vtable pointer is slot 17 of its Cup vtable. The bodies call only Staff_Records and qualification functions. |
| 0x8e9080 | `TRANSFER_RULES::post_deadline_news` | same name, new convention | The third argument goes to `transfer_window_get_date` as the year. The caller `update_transfer_window_news` compares it with the year word of a date. |
| 0x8e7810 | `TRANSFER_OFFER::init_contract_offer` | `TRANSFER_OFFER::init_loan_offer` | The callers are `make_loan_offer` and `submit_loan_contract_offer`. It sets the loan flag +0x1f to 1. |
| 0x7feeb0, 0x7fee80 | `ScreenManager::set_screen_title`, `get_screen_title` | `ScreenManager::set_background_picture`, `get_background_picture` | `Display::draw_screen` loads node +0x1fc with `gui_load_rgn_picture`, else DEFAULT_PIC.RGN. The setters are the `background_set_*_picture` helpers. |
| 0x7c2750 | `player_stats_format_record_field` | `PlayerStats::format_record_field` | The callers load ECX with the PlayerStats global 0xb74350 or pass their own `this`. |
| 15 `post_*_news` rows and 0x69ebc0 | free names | `MANAGER_MANAGER::` members | Every caller passes its own `this` or the global 0xb67a44 in ECX. |
| 0x66a280, 0x66a350, 0x66a360, 0x66a430 | `key_nation_menu_split_*` | `NATION_MENU_SPLIT::` constructor, `part_count`, `format_header`, `in_second_part` | All six callers load ECX with the address of a 14-byte object on their stack. key_nation.cpp has two real classes: the 0x48-byte `KEY_NATION` entry and this object. |

The `news_*_text` rows of manager_manager.cpp mix free and member names. This check did not cover them. The file questions above are still open for Max.

### Data map

Spec: `docs/specs/2026-10-17-data-map-and-confidence-spec.md`.

#### Slice 1: dossiers, script names and docs (2026-10-17)

`globals.py` finds 4,428 globals that game and library functions touch, in 746 address groups. The spec counted 3,431 from the "Globals referenced" lines of the function dossiers. Those lines stop at 80 globals per function, so the long functions lost about 1,000 read-only constants and tables. The import table is left out. 465 globals have readers in many files and go to `shared.md`. The rest go to 320 owner file pages.

Script names: 2,289 rows, 2,148 high and 141 medium.

| Kind | High | Medium | Quote |
|---|---|---|---|
| Key ids | 1,169 | 115 | The string of the name chain that stores the row (from `src/cm3/key_ids_table.c`). The 115 medium ones are set to -1 and no chain names them. |
| Read-only float constants | 740 | 0 | The value in .rdata. Every access reads it. |
| Vtables | 239 | 3 | The constructor or destructor of the class that stores the address. Medium when two classes store it. |
| String pointers | 0 | 18 | The string at the address. Medium, because some are the first entry of a table. |
| Library data | 0 | 6 | Only library functions touch it. |

A check of 20 random high script names against the disassembly (7 key ids, 5 vtables, 8 constants) found 20 agree. Two vtables have no destructor slot: RbNorwayCup and BrazilAwards have no virtual destructor, and their slots hold their own members.

2,280 globals are left to name, in 31 batches of 47 to 80 globals and 4 waves. A batch input is 31 to 154 KB.

#### Wave data-w01 and the Sonnet gate (2026-10-17)

Eight Sonnet agents named the 600 globals of batches `data-w01-aa` to `-ah`: 435 high, 143 medium and 22 low. The merge found no problem, and the check passes.

| Batch | Globals | High | Medium | Low | Tokens | What |
|---|---|---|---|---|---|---|
| aa | 68 | 58 | 10 | 0 | 148,000 | Vtables, background picture sections, cash tables, Brazilian club names |
| ab | 80 | 74 | 5 | 1 | 174,000 | Key ids whose names have accents, history category names |
| ac | 79 | 79 | 0 | 0 | 90,000 | Spanish nationality adjectives |
| ad | 79 | 76 | 3 | 0 | 117,000 | Language library data and Spanish verb texts |
| ae | 80 | 66 | 13 | 1 | 144,000 | Staff and transfer news texts, stat id lists |
| af | 70 | 34 | 35 | 1 | 148,000 | Database table counts and pointers, static Dates |
| ag | 70 | 23 | 47 | 0 | 112,000 | Fields of the league table at 0xb63d60 |
| ah | 74 | 25 | 30 | 19 | 247,000 | Manager object pointers, commentary event table, screen records |

The wave took about 1.18 million tokens and 7 minutes. The agents took 1 to 7 minutes each.

Gate: one Opus agent checked 60 random rows (142,000 tokens). 59 are right and 1 is wrong (1.7%), under the 10% line, so the rest of the data map stays on Sonnet. The wrong row is 0xae23b4 `g_club_table`, high without a quote. The check kept the name and quoted 0x5267c0 `database_trim_club_table` and 0x53db40 `database_sort_clubs_by_reputation` (run `data-w01-check`). The brief now says that a row size and a reader count alone are medium, that a Date is 8 bytes, and that a namer names the base of its object. The league table base 0xb63d60 is in wave 2.

The wave showed two gaps in the script rules. The string rule of the function map takes only ASCII text of three or more characters, so short strings and strings with accents came into scope as globals, and the key id rule missed names with accents. `globals.py` now names text that every access passes by address, with Latin-1 letters and of any length, and reads key id names with accents. Script names rise from 2,289 to 3,341, and high script names from 2,148 to 3,317. 813 globals are left, in 12 batches over waves 2 and 3. One false global is known: 0xb71b00 is the number 12,000,000 in `EngPrm::create_table`, not an address.

## Recommendation

Run the full map on the fast route. It costs about $120 and takes an afternoon. Every later feature and research doc then starts from a name for each function.

1. Commit the step 1 file table and the dossier and batch scripts under `tools/research/` (addresses, file names and model output only, no code of the original).
2. Run wave 1 as one batch, then wave 2 on the low names with the names of the whole game.
3. Write one doc per source file under `docs/research/function-map/`, and check a sample of 20 names against earlier research before use.
