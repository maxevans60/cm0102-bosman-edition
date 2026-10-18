/* Rule kind "cups" of the season change (season_change.h): the round formats of the domestic cups by season
 * (0.6.0 slice 21, docs/research/2026-10-17-cup-round-record.md).
 *
 * The cup classes of the original make their round table in create_round_table (a virtual of each class, called
 * with stage 0xff by set_up_rounds, vtable +0x8c): at a new game, in start_new_season, and again at times that
 * are not a season start (the FA Cup table was made anew by 1 June of its first season; a log of
 * every start_new_season showed none in between). So the kind does not write the table after a season
 * start. It wraps create_round_table of each cup class that has a row in the cups table and changes the table
 * that the function returns (0x68 bytes a round, five format fields, season_cups.h) before the caller stores it.
 * Whatever makes the table, it holds the format of the season:
 * - the season of a cup is its season year (cup+0x40, set before the table is made) with changing rules on,
 *   and the start year of the game with changing rules off (the rules stay, Q232);
 * - a season without a row, and a game that starts in 2001/02 with changing rules off, keep the original's
 *   rounds (the table has no row before 2002).
 * A saved game keeps its table in its cup file (Cup::save_cup_to_disk 0x5210f0 writes the rounds, Cup::
 * load_from_disk 0x51fc00 reads them without a call of create_round_table), so a load changes nothing.
 * The news sentence of a row comes in the after hook of the season change, once for the cup that starts that
 * season. */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_cups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUP_ROUND_COUNT 0x3cu       /* s16 */
#define CUP_YEAR 0x40u              /* s16 season year */
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

/* Test switch: RECOMP_CUPS=off leaves the cups as the original makes them (to find out whether a fault of a run
 * comes from this kind). */
static int cups_off(void)
{
    const char *v = getenv("RECOMP_CUPS");
    return v && !strcmp(v, "off");
}

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* Writes the format of the row to a table of n rounds. Returns the number of rounds that changed, or -1 when
 * the table has another round count than the row (nothing is written). */
static int apply_row(uint32_t table, int n, const SeasonCupRow *row)
{
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

/* The season whose format a cup takes (module comment). */
static int cup_season(uint32_t cup)
{
    if (!season_changing_rules()) return season_start_year();
    int y = (int16_t)rd16(g_mem, cup + CUP_YEAR);
    return y >= SEASON_FIRST_YEAR && y <= SEASON_LAST_YEAR ? y : season_start_year();
}

/* After create_round_table of a cup: self is the cup, stage and count_ptr are its first two arguments, the table
 * is the result (cpu.eax). */
static void table_made(uint32_t self, uint32_t stage, uint32_t count_ptr)
{
    if (stage != 0xffu || !season_code_on() || cups_off() || !readable(count_ptr)) return;
    const char *file = season_change_file(self);
    int season = cup_season(self);
    const SeasonCupRow *row = season_cups_row(file, season);
    if (!row) return;
    int changed = apply_row(cpu.eax, (int16_t)rd16(g_mem, count_ptr), row);
    if (logging())
        fprintf(stderr, "recomp: cups: %s (0x%x) season %d, row %d: %s %d\n", file, self, season, row->season,
                changed < 0 ? "not applied, another round count, rounds" : "rounds changed", changed < 0 ? 0 : changed);
}

/* The news of a row: with changing rules on, the cup that starts the season of a row says so. */
static void cups_after(const SeasonChange *c)
{
    if (!c->started || cups_off()) return;
    const SeasonCupRow *row = season_cups_row(c->file, c->season);
    if (!row || row->season != c->season) return;
    if ((int16_t)rd16(g_mem, c->comp + CUP_ROUND_COUNT) != row->rounds) return;
    season_change_news("%s", row->sentence);
}

const SeasonRuleKind season_kind_cups = {"cups", NULL, NULL, NULL, NULL, NULL, cups_after};

/* create_round_table of the cup classes that have a row in the cups table (thiscall: this, char stage, short
 * *round_count, short *first_id, int *out; ret 0x10; the function map and the read of each, docs/research/
 * 2026-10-17-cup-round-record.md). The recompiler reads the RT_WRAP lines as they are, so each is written out. */
void o_0056f320(void);
RT_WRAP(0056f320, cups_eng_fa_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_0056f320();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_0056c570(void);
RT_WRAP(0056c570, cups_eng_cc_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_0056c570();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_00570ec0(void);
RT_WRAP(00570ec0, cups_eng_fa_trophy)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_00570ec0();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_007ed5d0(void);
RT_WRAP(007ed5d0, cups_sco_fa_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_007ed5d0();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_0041d310(void);
RT_WRAP(0041d310, cups_bel_fa_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_0041d310();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_005ba3f0(void);
RT_WRAP(005ba3f0, cups_fra_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_005ba3f0();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_005bcc40(void);
RT_WRAP(005bcc40, cups_fra_lge_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_005bcc40();
    if (!rt_unwinding) table_made(self, stage, count);
}

void o_0078f2e0(void);
RT_WRAP(0078f2e0, cups_nor_cup)
{
    uint32_t self = cpu.ecx, stage = rt_arg(0) & 0xffu, count = rt_arg(1);
    o_0078f2e0();
    if (!rt_unwinding) table_made(self, stage, count);
}
