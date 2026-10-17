/* Rule kind "cups" of the season change (season_change.h): the round formats of the domestic cups by season
 * (0.6.0 slice 21, docs/research/2026-10-17-cup-round-record.md).
 *
 * The cup classes of the original make their round table (0x68 bytes a round, cup+0xa3, cup+0x3c rounds) in
 * set_up_rounds at every season start and at a new game. This kind writes the five format fields of each round
 * (season_cups.h) after the table is made:
 * - new_game: for the cups of the game, to the format of the start year (a game that starts in 2001 gets none:
 *   the table has no row before 2002, so the base game keeps the original's rounds);
 * - after the start_new_season of a cup (changing rules on): to the format of the new season, and one news
 *   sentence when a row starts in that season and the rounds changed.
 * The fixtures of a round are made when its draw comes, after the start_new_season of the cup, so no fixture
 * holds the old flags. A saved game keeps the table in its cup file (Cup::save_cup_to_disk 0x5210f0 writes the
 * rounds, Cup::load_from_disk 0x51fc00 reads them without a rebuild), so the loaded hook does nothing. */
#include "win.h"
#include "season_change.h"
#include "season_cups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUP_ROUND_COUNT 0x3cu       /* s16 */
#define CUP_ROUND_TABLE 0xa3u       /* pointer to the rounds */
#define ROUND_SIZE 0x68u
#define ROUND_FIRST 0x0du           /* u16 */
#define ROUND_DECIDING 0x0fu        /* u16 */
#define ROUND_REPLAYS 0x20u
#define ROUND_LEGS 0x21u
#define ROUND_DAYS 0x22u

static int logging(void)
{
    const char *v = getenv("RECOMP_SEASON_CHANGE_LOG");
    return v && *v;
}

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* Writes the format of the row to the cup. Returns the number of rounds that changed, or -1 when the cup does not
 * hold the row's round count (a wrong class, nothing is written). */
static int apply_row(uint32_t cup, const SeasonCupRow *row)
{
    int n = (int16_t)rd16(g_mem, cup + CUP_ROUND_COUNT);
    uint32_t table = rd32(g_mem, cup + CUP_ROUND_TABLE);
    if (n != row->rounds || !readable(table) || !readable(table + (uint32_t)n * ROUND_SIZE)) return -1;
    int changed = 0;
    for (int i = 0; i < n; i++) {
        uint32_t at = table + (uint32_t)i * ROUND_SIZE;
        SeasonCupRound r = {(uint16_t)rd16(g_mem, at + ROUND_FIRST), (uint16_t)rd16(g_mem, at + ROUND_DECIDING),
                            g_mem[at + ROUND_REPLAYS], g_mem[at + ROUND_LEGS], g_mem[at + ROUND_DAYS]};
        if (!season_cups_apply(row->types[i], &r)) continue;
        wr16(g_mem, at + ROUND_FIRST, r.first);
        wr16(g_mem, at + ROUND_DECIDING, r.deciding);
        g_mem[at + ROUND_REPLAYS] = r.replays;
        g_mem[at + ROUND_LEGS] = r.legs;
        g_mem[at + ROUND_DAYS] = r.days;
        changed++;
    }
    return changed;
}

static int apply_cup(uint32_t cup, const char *file, int season, const char *why)
{
    const SeasonCupRow *row = season_cups_row(file, season);
    if (!row) return 0;
    int changed = apply_row(cup, row);
    if (logging())
        fprintf(stderr, "recomp: cups: %s %s season %d: row %d, %s\n", why, file, season, row->season,
                changed < 0 ? "not applied (the cup holds another round count)" : "rounds changed");
    if (logging() && changed >= 0) fprintf(stderr, "recomp: cups: %s changed %d\n", file, changed);
    return changed;
}

static void cups_new_game(int start_year)
{
    uint32_t comps[512];
    int n = season_change_comps(comps, 512);
    for (int i = 0; i < n; i++) apply_cup(comps[i], season_change_file(comps[i]), start_year, "new game");
}

static void cups_after(const SeasonChange *c)
{
    if (!c->started) return;
    const SeasonCupRow *row = season_cups_row(c->file, c->season);
    if (!row) return;
    int changed = apply_cup(c->comp, c->file, c->season, "season change");
    if (changed > 0 && row->season == c->season) season_change_news("%s", row->sentence);
}

const SeasonRuleKind season_kind_cups = {"cups", cups_new_game, NULL, NULL, NULL, NULL, cups_after};
