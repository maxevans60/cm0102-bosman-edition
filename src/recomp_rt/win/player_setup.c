/* The dates of a season database's player_setup.cfg in a season game (open item 229).
 *
 * The Update Team databases keep their loans, injuries, bans, retirements and future transfers in player_setup.cfg,
 * one line each. The game reads it at the creation of a new game (player_setup_cfg_load 0x7a30c0, which calls
 * player_setup_apply_line 0x7a3a10 for each line). The dates of the lines are written for a 2001/02 start, as the
 * other dates of these databases: October 2026 has "LOAN" ... 1 6 2001 15 6 2002 for a loan of 2026/27. The parser
 * takes only years 1980 to 2010 (the checks against 0x7bc and 0x7db, for example 0x7a4f5c and 0x7a4f97 for a loan),
 * so the file cannot hold the real years.
 *
 * The cause. In a game that starts after 2002 every such date lies in the past:
 * - LOAN: TRANSFER_MANAGER::add_config_loan 0x8bf400 (called at 0x7a500c) signs the loan only when its start is on or
 *   before the game date [0xae2c90] and its end on or after it. So a 2026/27 game started with no loans.
 * - FUTURE_TRANSFER (0x8bf060, called at 0x7a4cfc) and RETIREMENT (0x7a8510, called at 0x7a4901) keep their dates
 *   for later; a date in the past is a move or a retirement that the season never reaches as the data meant.
 *
 * The fix, in a season game only (start year after 2001, season code on), when the data set folder holds its own
 * player_setup.cfg (win_data_set_has): the four Dates that apply_line makes from the line, Date::Date 0x549f70 at
 * 0x7a48b8 (retirement), 0x7a4cd0 (future transfer), 0x7a4fbf and 0x7a4fe6 (loan start and end), get the year shift
 * of the database's contract ends (season_dates_contract_shift: D - 2001 + N - D for a 2001-based year, N - D for a
 * real one). The shift comes after the parser's year checks, so they pass as in a 2001 game. A 29 February that lands
 * in a year that is not a leap year becomes 28 February.
 *
 * Not moved: the three ban lines (MISCONDUCT, DRUGS, FALSE_PASSPORT; Dates at 0x7a3dba, 0x7a4025, 0x7a427c). Their
 * branch compares the year with the game year ([0xae2c92], for example at 0x7a3d6e) before it makes the Date, so a
 * 2001-based ban never reaches the Date in a later game, as before. INJURY and INT_RETIREMENT lines have no date.
 *
 * Without its own player_setup.cfg the data set reads the game's own Data/player_setup.cfg (the 3.9.68 lines, files.c
 * data_dir_swap). Those dates do not move: the base game and our own databases (Summer 2002, 2004, 2006) play as
 * before. The official data sets 3.9.60 and 3.9.65 have their own file; they start only in 2001/02, where nothing
 * moves, and in a later test start (RECOMP_START_SEASON) they take N - 2001, the rule of their contract ends.
 * Test switch RECOMP_PLAYER_SETUP_DATES=original: no date moves. */
#include "win.h"
#include "season.h"
#include "season_dates.h"
#include "player_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The return addresses of the four Date::Date calls of apply_line whose dates move. */
static const uint32_t moved_calls[] = {
    0x007a48bdu,    /* 0x7a48b8: RETIREMENT */
    0x007a4cd5u,    /* 0x7a4cd0: FUTURE_TRANSFER */
    0x007a4fc4u,    /* 0x7a4fbf: LOAN start */
    0x007a4febu,    /* 0x7a4fe6: LOAN end */
};

static int original(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("RECOMP_PLAYER_SETUP_DATES"); v = e && !strcmp(e, "original"); }
    return v;
}

static int is_leap(int y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }

int player_setup_moved_date(int day, int month0, int year, const SeasonShiftPlan *p, int *out_day)
{
    int ny = year + season_dates_contract_shift(p, year);
    *out_day = day;
    if (month0 == 1 && day == 29 && !is_leap(ny)) *out_day = 28;
    return ny;
}

/* The plan of the game that is made, cached by start year and folder. 0 when nothing moves. */
static int plan_now(SeasonShiftPlan *out)
{
    static int cached_year;
    static char cached_dir[1100];
    static int cached_ok;
    static SeasonShiftPlan cached;
    int N = season_start_year();
    if (N <= SEASON_FIRST_YEAR || !season_code_on() || original() || !win_data_set_has("player_setup.cfg")) return 0;
    const char *dir = win_data_set_dir();
    if (N != cached_year || strcmp(dir, cached_dir)) {
        cached_year = N;
        snprintf(cached_dir, sizeof cached_dir, "%s", dir);
        cached_ok = season_dates_plan(dir, N, &cached);
        const char *slash = strrchr(dir, '/');
        if (cached_ok)
            fprintf(stderr, "recomp: player setup: the dates of %s/player_setup.cfg move to %d/%02d (2001-based years "
                    "+%d, real years +%d)\n", slash ? slash + 1 : dir, N, (N + 1) % 100, cached.contract_old,
                    cached.contract_real);
    }
    *out = cached;
    return cached_ok;
}

void player_setup_date_args(uint32_t ret, uint32_t args)
{
    int hit = 0;
    for (size_t i = 0; i < sizeof moved_calls / sizeof *moved_calls; i++) hit |= ret == moved_calls[i];
    if (!hit) return;
    SeasonShiftPlan p;
    if (!plan_now(&p)) return;
    int day = (int16_t)rd16(g_mem, args), month0 = (int16_t)rd16(g_mem, args + 4u);
    int year = (int16_t)rd16(g_mem, args + 8u);
    if (day < 1 || day > 31 || month0 < 0 || month0 > 11 || year <= 1900) return;
    int nd, ny = player_setup_moved_date(day, month0, year, &p, &nd);
    wr32(g_mem, args, (uint32_t)nd);
    wr32(g_mem, args + 8u, (uint32_t)ny);
}
