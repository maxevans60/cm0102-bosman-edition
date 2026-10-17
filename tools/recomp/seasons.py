"""Year constants of the original that become run-time values, for a game that starts in another season.

The original starts every game in 2001/02 and writes 2001, 2002 and 2003 as constants in some instructions.
Each entry of PATCHES names the address of one instruction, the constant it holds and the C expression that
replaces it. rt_season_year is the start year of the game (src/recomp_rt/win/season.c): 2001 in the base game,
so every replaced instruction gives the original's result there. INSNS replaces a whole instruction, as in
widescreen.py. The evidence for each address is in docs/research/2026-10-17-first-season-branches.md
(0.6.0 slice 11).
"""

Y = "rt_season_year"
Y1 = "(rt_season_year + 1u)"
Y2 = "(rt_season_year + 2u)"

# address: (constant in the original, C expression)
PATCHES = {
    # The start date (doc table 1, sweep-season-end 3.1). The game date is 1 January of the start year: at the
    # start screen (setup_run_front_end 0x81b890, Date::set at 0x81b949) and after a game ends (free_game_data
    # 0x82ff40, Date::set at 0x8304fe). The league table takes the season of each entry from that year (0x667150).
    0x81B93C: (2001, Y),
    0x8304F1: (2001, Y),
    # The Europe clamp of setup_pick_start_date 0x830520: in the first season a European start date after
    # 1 September becomes 1 September (the compare and the two dates).
    0x83058A: (2001, Y),
    0x830597: (2001, Y),
    0x8305DB: (2001, Y),

    # First-season branches that mean "the first season of the game" (MOVE). Doc tables 2 and 3.
    # The database merges of group divisions into one division, in the first season of each new game.
    0x4224EC: (2001, Y),        # BelThird::fill_league_table: Third Division A and B
    0x5DB23E: (2001, Y),        # GerFirst::create_table: the four regional divisions
    0x7D240C: (2001, Y),        # PorSecondB::create_table: II Divisao B Norte, Centro, Sul
    0x856CDC: (2001, Y),        # SpaSecondB::load_group_clubs: Segunda B groups 1 to 4
    0x8FDDCE: (2001, Y),        # TurFirst::create_table: 2 Division Category B groups 1 to 5
    0x56FC5F: (2001, Y),        # EngFaCup::collect_entrants: the FA Trophy clubs from the database history
    # The run from 1 January of the start year to the start date plays no season: these skip that year.
    0x5836C4: (2001, Y),        # EuropeanAwards::daily_update
    0x5FA89B: (2001, Y),        # HostCountry::daily_update
    0x632E30: (2001, Y),        # InternationalAwards::daily_update
    0x79E8A0: (2001, Y),        # OldInternationalAwards::daily_update
    0x5C3065: (2001, Y),        # Friendly::plan_preseason_tournaments_and_tours
    0x931AE9: (2001, Y),        # YEAR_RATINGS::collect_best_players: half a year of ratings
    # The first calendar years of the game.
    0x59B56B: (2002, Y1),       # finance_get_days_to_season_start: split-season leagues end in Y + 1
    0x6EB28D: (2002, Y1),       # MATCH_PLAYER::setup: Japan (calendar year) has no full season before Y + 1
    0x5AEE52: (2001, Y),        # FIXTURE_MANAGER::get_club_first_fixture_year: Y + bit 0 of the club dates
    0x5AF218: (2001, Y),        # fix_man_record_club_fixture_years: a fixture in the first calendar year
    0x5AF243: (2001, Y),
    0x5AF288: (2001, Y),
    0x5AF2B9: (2001, Y),
    0x45582D: (2001, Y),        # club_screen_build_fragment_454950: the first season starts on 1 January
    0x45583C: (2001, Y),
    0x45F4E9: (2001, Y),        # club_screen_draw_fixtures_tab: no season before the first
    0x45F979: (2001, Y),
    0x45F980: (2001, Y),
    0x483322: (2001, Y),        # handle_club_screen_action: a friendly date before the game start
    # The first season year of a league table entry (+0x1b, doc table 4). The national create functions write
    # the game year there. Read by UefaSeeding::assign_uefa_cup_places at 0x904951.
    0x830FB0: (2001, Y),
    0x8312B0: (2001, Y),
    0x83161E: (2001, Y),
    0x8318C2: (2001, Y),
    0x831B69: (2001, Y),
    0x831E7C: (2001, Y),
    0x8320C9: (2001, Y),
    0x8371D1: (2001, Y),

    # International and club editions (doc table 5).
    0x58B385: (2001, Y),        # FifaConfed::FifaConfed: no Confederations Cup in the start year (the run-up)
    0x91F486: (2001, Y),        # WcOceaniaLeague::update_for_date: a campaign that began before the game
    0x92AF4E: (2001, Y),        # WorldClubChamp::pick_entrant: first edition, entrants from the history
    0x92B24B: (2001, Y),
    0x92B970: (2001, Y),        # WorldClubCup::pick_qualifiers: first edition, champions from the history
    0x92BA9C: (2001, Y),
    0x632540: (2001, Y),        # InterAmerCup::select_teams: first edition, champions from the history
    0x63266C: (2001, Y),
    # UEFA seeding in the first season of the game (doc table 6).
    0x9041EC: (2001, Y),        # UefaSeeding::collect_european_clubs: read euro.cfg of the database
    0x903B6F: (2001, Y),        # UefaSeeding::roll_coefficients: the coefficients of the database as they are
    0x9038AF: (2001, Y),        # UefaSeeding::pick_english_intertoto_club: no volunteer list yet
    0x9059B2: (2001, Y),        # uefa_check_registration_deadlines: no 1 February deadline in the run-up

    # The Select Start Season screen 0x81e8d0 writes a split season as "%s %02d/%02d" with year % 100 and
    # (year + 1) % 10 (0x81e9c9): 2010 shows "10/01". With % 100 it shows "10/11". For 2001 to 2008 both give
    # the same text, so the base game is unchanged.
    0x81E9C9: (10, "100u"),

    # Q358: the season formatters of key_nation.cpp write "%d/%d" (0x9eb024) with (year + 1) % 10, "2009/0".
    # rt_season_text_mod and rt_season_text_format (win/season.c) give % 100 and "%d/%02d", "2009/10"; with
    # RECOMP_SEASON_CODE=off the original's values.
    0x6696D3: (10, "rt_season_text_mod(10u)"),              # key_nation_format_club_season 0x669630
    0x6696E6: (0x9EB024, "rt_season_text_format(0x9eb024u)"),
    0x669869: (10, "rt_season_text_mod(10u)"),              # key_nation_format_comp_season 0x6697c0
    0x669876: (0x9EB024, "rt_season_text_format(0x9eb024u)"),

    # International finals (0.6.0 slice 27, docs/research/2026-10-18-international.md). The golden goal is bit
    # 0x80 of the rule flags of a knockout round (sweep-international section 7). The round template function
    # of each class holds the competition in edi (set at 0x92c299, 0x57ab99, 0x58b56f) and the round records in
    # esi. rt_intl_round_flags (win/season_intl_rules.c) clears the bit for an edition without golden goal; the
    # base game keeps the flags. The first argument is the class (season_intl.h): 0 World Cup, 1 European
    # Championship, 2 Confederations Cup.
    0x92C96B: (0x383, "rt_intl_round_flags(0u, R16(edi + 0x40u), 0x383u)"),    # World Cup last 16
    0x92CA46: (0x383, "rt_intl_round_flags(0u, R16(edi + 0x40u), 0x383u)"),    # quarter-final
    0x92CB15: (0x383, "rt_intl_round_flags(0u, R16(edi + 0x40u), 0x383u)"),    # semi-final
    0x92CBB9: (0x83, "rt_intl_round_flags(0u, R16(edi + 0x40u), 0x83u)"),      # third place
    0x57B002: (0x383, "rt_intl_round_flags(1u, R16(edi + 0x40u), 0x383u)"),    # Euro quarter-final
    0x57B0B2: (0x383, "rt_intl_round_flags(1u, R16(edi + 0x40u), 0x383u)"),    # semi-final
    0x57B150: (0x283, "rt_intl_round_flags(1u, R16(edi + 0x40u), 0x283u)"),    # final
    0x58B892: (0x383, "rt_intl_round_flags(2u, R16(edi + 0x40u), 0x383u)"),    # Confederations Cup semi-final
    0x58B90B: (0x83, "rt_intl_round_flags(2u, R16(edi + 0x40u), 0x83u)"),      # third place
    0x58B9A5: (0x83, "rt_intl_round_flags(2u, R16(edi + 0x40u), 0x83u)"),      # final
}

# address: (the instruction in the original, C code)
INSNS = {
    # The 2002 World Cup qualifying data of Europe (load_2002_campaign 0x91b760) and South America (0x9216d0).
    # The constructors call it when the start year is not before the campaign (cmp at 0x91846f, 0x9204b4). In a
    # 2001 game the campaign is 2004, so the original never calls it (doc table 7). For a later start year the
    # data would go into a later campaign, and its asserts (0x91dbc4, 0x9223ab) want dates in 2000 and 2001.
    0x918473: ("jl 0x91847c", "if (cc_l(F) || rt_season_year != 2001u) goto L_91847c;"),
    0x9204B8: ("jl 0x9204c1", "if (cc_l(F) || rt_season_year != 2001u) goto L_9204c1;"),

    # International finals (0.6.0 slice 27, docs/research/2026-10-18-international.md). Each class keeps its
    # edition year in the word +0x40 of the object (this in esi). The constructor rounds the game year up to the
    # class's cycle; the first instruction after that loop also gets the year of rt_intl_first_year (the first
    # edition of the table from the start season, win/season_intl_rules.c). start_new_season adds the step;
    # rt_intl_next_year gives the next edition of the table. The base game keeps the original's values. The
    # first argument is the class (season_intl.h): 0 World Cup, 1 European Championship, 2 Confederations Cup,
    # 3 Copa America, 4 African Cup of Nations, 5 Asian Cup, 6 Oceania Nations Cup, 7 Olympics. No instruction
    # after a step reads its flags before another instruction sets them.
    0x92BFEE: ("push 6", "W16(esi + 0x40u, rt_intl_first_year(0u, R16(esi + 0x40u))); esp -= 4; W32(esp, 6u);"),  # world_cup ctor 0x92bf50
    0x57A92B: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(1u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # euro_champ ctor 0x57a890
    0x58B392: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(2u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # fifa_confed ctor 0x58b310
    0x5E06FB: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(3u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # copa_america ctor 0x5e0660
    0x401097: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(4u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # african_nations ctor 0x401000
    0x40E8F5: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(5u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # asia_nations ctor 0x40e860
    0x798952: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(6u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # oceania_nations ctor 0x7988b0
    0x79F5C5: ("push 1", "W16(esi + 0x40u, rt_intl_first_year(7u, R16(esi + 0x40u))); esp -= 4; W32(esp, 1u);"),  # olympics ctor 0x79f530
    0x92D409: ("add word ptr [esi + 0x40], 4", "W16(esi + 0x40u, rt_intl_next_year(0u, R16(esi + 0x40u), 4u));"),  # world_cup::start_new_season 0x92d330
    0x57B7CA: ("add word ptr [esi + 0x40], 4", "W16(esi + 0x40u, rt_intl_next_year(1u, R16(esi + 0x40u), 4u));"),  # euro_champ 0x57b750
    0x58C050: ("add word ptr [esi + 0x40], 2", "W16(esi + 0x40u, rt_intl_next_year(2u, R16(esi + 0x40u), 2u));"),  # fifa_confed 0x58bff0
    0x5E1590: ("add word ptr [esi + 0x40], 2", "W16(esi + 0x40u, rt_intl_next_year(3u, R16(esi + 0x40u), 2u));"),  # copa_america 0x5e1530
    0x401D80: ("add word ptr [esi + 0x40], 2", "W16(esi + 0x40u, rt_intl_next_year(4u, R16(esi + 0x40u), 2u));"),  # african_nations 0x401d20
    0x40F780: ("add word ptr [esi + 0x40], 4", "W16(esi + 0x40u, rt_intl_next_year(5u, R16(esi + 0x40u), 4u));"),  # asia_nations 0x40f720
    0x799940: ("add word ptr [esi + 0x40], 2", "W16(esi + 0x40u, rt_intl_next_year(6u, R16(esi + 0x40u), 2u));"),  # oceania_nations 0x7998e0
    0x7A04B0: ("add word ptr [esi + 0x40], 4", "W16(esi + 0x40u, rt_intl_next_year(7u, R16(esi + 0x40u), 4u));"),  # olympics 0x7a0450

    # The World Cup holder (0.6.0 slice 27): from the 2006 World Cup the holder qualifies like the others
    # (int-world). Each qualifying section asks for the holder (comp_util_get_latest_placed_club 0x4afd80, place
    # 0 of the World Cup history) and leaves it out of its pool; rt_intl_wc_holder gives no holder in a season
    # game, at the instruction after each call. The sections of a 2001 game play the 2004 campaign (Europe,
    # South America, Asia) or the 2000 campaign, whose holder France is in none of their pools.
    0x919F47: ("add esp, 8", "eax = rt_intl_wc_holder(eax); esp += 8u;"),  # wc_europe_league
    0x920E38: ("add esp, 8", "eax = rt_intl_wc_holder(eax); esp += 8u;"),  # wc_south_american_league
    0x920F18: ("mov dword ptr [esp + 0x20], eax", "eax = rt_intl_wc_holder(eax); W32(esp + 0x20u, eax);"),  # wc_south_american_league, its pool
    0x9107A6: ("add esp, 8", "eax = rt_intl_wc_holder(eax); esp += 8u;"),  # wc_african_cup
    0x914077: ("add esp, 8", "eax = rt_intl_wc_holder(eax); esp += 8u;"),  # wc_asia_league
    0x916394: ("mov edx, eax", "eax = rt_intl_wc_holder(eax); edx = eax;"),  # wc_concacaf_cup
    0x91FB98: ("add esp, 8", "eax = rt_intl_wc_holder(eax); esp += 8u;"),  # wc_oceania_league
    # world_cup build_team_slots 0x92da40 puts the holder in slot 0 (0x92da86) and counts it (0x92da89). From
    # 2006 the slot is not counted, so the first host takes slot 0; slots 1 and 2 are cleared, because the
    # checks at 0x92db02 and 0x92db57 compare slot 0 with them.
    0x92DA89: ("inc word ptr [esi + 0x36]", "if (rt_intl_wc_holder_slot(R16(esi + 0x40u))) W16(esi + 0x36u, R16(esi + 0x36u) + 1u); else { W32(R32(esi + 0x14u) + 6u, 0u); W32(R32(esi + 0x14u) + 0xcu, 0u); }"),
    # The World Cup Oceania section (wc_oceania_league, pool at 0x91fc66..0x91fcde) takes exactly 10 entrants
    # (0x91fbda) and leaves out New Caledonia (key nation 0x9cf530) and Papua New Guinea (0x9cf41c). With
    # Australia in Asia (win/season_intl.c) New Caledonia comes in, so the count of 10 stays.
    0x91FC89: ("mov ecx, dword ptr [0x9cf530]", "ecx = rt_intl_oceania_left_out(R32(0x9cf530u));"),
}
