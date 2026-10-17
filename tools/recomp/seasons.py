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
    # The start date (sweep-season-end 3.1). The game date is 1 January of the start year: at the start screen
    # (setup_run_front_end 0x81b890, Date::set at 0x81b949) and after a game ends (free_game_data 0x82ff40,
    # Date::set at 0x8304fe). The league table takes the season of each entry from that year (0x667150).
    0x81B93C: (2001, Y),
    0x8304F1: (2001, Y),
    # The Europe clamp of setup_pick_start_date 0x830520: in the first season a European start date after
    # 1 September becomes 1 September (the compare and the two dates).
    0x83058A: (2001, Y),
    0x830597: (2001, Y),
    0x8305DB: (2001, Y),

    # First-season branches that mean "the first season of the game" (MOVE). Doc table 1.
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
    # The first season year of a league table entry (+0x1b). The national create functions write the game
    # year there. Read by UefaSeeding::assign_uefa_cup_places at 0x904951.
    0x830FB0: (2001, Y),
    0x8312B0: (2001, Y),
    0x83161E: (2001, Y),
    0x8318C2: (2001, Y),
    0x831B69: (2001, Y),
    0x831E7C: (2001, Y),
    0x8320C9: (2001, Y),
    0x8371D1: (2001, Y),

    # International and club editions (doc table 3).
    0x58B385: (2001, Y),        # FifaConfed::FifaConfed: no Confederations Cup in the start year (the run-up)
    0x91F486: (2001, Y),        # WcOceaniaLeague::update_for_date: a campaign that began before the game
    0x92AF4E: (2001, Y),        # WorldClubChamp::pick_entrant: first edition, entrants from the history
    0x92B24B: (2001, Y),
    0x92B970: (2001, Y),        # WorldClubCup::pick_qualifiers: first edition, champions from the history
    0x92BA9C: (2001, Y),
    0x632540: (2001, Y),        # InterAmerCup::select_teams: first edition, champions from the history
    0x63266C: (2001, Y),
    # UEFA seeding in the first season of the game (doc table 4).
    0x9041EC: (2001, Y),        # UefaSeeding::collect_european_clubs: read euro.cfg of the database
    0x903B6F: (2001, Y),        # UefaSeeding::roll_coefficients: the coefficients of the database as they are
    0x9038AF: (2001, Y),        # UefaSeeding::pick_english_intertoto_club: no volunteer list yet
    0x9059B2: (2001, Y),        # uefa_check_registration_deadlines: no 1 February deadline in the run-up

    # The Select Start Season screen 0x81e8d0 writes a split season as "%s %02d/%02d" with year % 100 and
    # (year + 1) % 10 (0x81e9c9): 2010 shows "10/01". With % 100 it shows "10/11". For 2001 to 2008 both give
    # the same text, so the base game is unchanged.
    0x81E9C9: (10, "100u"),
}

# address: (the instruction in the original, C code)
INSNS = {
    # The 2002 World Cup qualifying data of Europe (load_2002_campaign 0x91b760) and South America (0x9216d0).
    # The constructors call it when the start year is not before the campaign (cmp at 0x91846f, 0x9204b4). In a
    # 2001 game the campaign is 2004, so the original never calls it (doc table 3). For a later start year the
    # data would go into a later campaign, and its asserts (0x91dbc4, 0x9223ab) want dates in 2000 and 2001.
    0x918473: ("jl 0x91847c", "if (cc_l(F) || rt_season_year != 2001u) goto L_91847c;"),
    0x9204B8: ("jl 0x9204c1", "if (cc_l(F) || rt_season_year != 2001u) goto L_9204c1;"),
}
