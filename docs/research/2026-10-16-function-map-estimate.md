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
