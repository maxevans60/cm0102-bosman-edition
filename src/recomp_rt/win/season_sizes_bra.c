/* Série A and Série B in two meetings with no play-off (0.7.2 league sizes, slice 17, D11; spec
 * docs/specs/2026-11-13-league-sizes-0.7.2-spec.md).
 *
 * The classes. BraNatFirst and BraNatSecond play one meeting (+0x3c 1 at 0x42a5af and 0x42cb83) on 27 dates
 * (create_schedule 0x429b50 and 0x42c140, D 0x1b at 0x429b8d and 0x42c17d), then a play-off of the top 4 as stage 0
 * (advance_stage 0x42a950 and 0x42cdd0, create_play_off_stage 0x42a9e0 and 0x42ce60). The constructors set the stage
 * count +0x2c to 1 and +0x30 to -1 (0x4298be, 0x4298c1) before set_up_rules (0x4298d5). start_new_season 0x42a640
 * sets +0x30 to -1 (0x42a6f2) and calls set_up_rules (vtable +0x8c, 0x42a6f9), and never writes +0x2c.
 *
 * The real leagues. Série A plays a double round robin with no play-off from 2003 (row 2002: rows are keyed one year
 * before the real season), Série B from 2006 (row 2005). The grown rows of league_sizes.py give the meetings of the
 * row (+0x3c 2) and the dates of the rounds: the wrap of create_schedule runs inside set_up_rules (vtable +0x3c,
 * 0x42a629 and 0x42cbfd). When the template has no room the class's meetings come back (season_sizes.c, D4).
 *
 * So after set_up_rules: with 2 meetings the league has no stage. The stage count goes to 0, as the 3. Liga in the
 * German Regionalliga class (season_divisions.c, ger_regional_south): with a count of 1 League::update_for_date
 * 0x6847c0 calls advance_stage when the table is complete and the play-off is played; with 0 the season ends as a
 * league's with no stages (the history, the honours and the complete flag). The destructor 0x429a90 and
 * start_new_season read the stage slots (+0xc) only while +0x30 >= 0 (0x429ae0..0x429aff, 0x42a6c8..0x42a6e8). The
 * save keeps +0x2c 0, and League::load 0x687b10 then makes no stage array (0x688e49); nothing reads +0xc then.
 * Série A also loses its 4 play-off places (+0xbe 4 at 0x42a5db): with no division above, the top 4 rows got the
 * play-off status (League::update_positions_and_status 0x684cd0 reads +0xbe at 0x684db0) and the news that they
 * qualify for the play-offs (BraNatFirst::build_fate_news_text 0x42ac70, result 6). A top league with no play-off
 * writes 0 there (EngPrm 0x574bb8, SpaFirst 0x84fe12). Série B keeps its places: they move its clubs up.
 *
 * The champion. update_positions_and_status gives the top row status 5 and the result code 5 only with bit 1 of the
 * rule flags +0xd9 (0x684dc0, 0x6850f2..0x685159); League::League sets 3 (0x6822a7), SpaFirst 0x43. The classes set
 * 8 and 2 (0x42a618, 0x42cbd8): their champion is the play-off winner (code 1 in stage 0, 0x42ac1c, row +0x37 5).
 * With no play-off the flags get bit 1, and Série A loses bit 8 (status 6 for the +0xbe rows, 0x6851dd..0x6851ff),
 * so the club on top of the table is the champion (D2). The result handlers (vtable +0x44, BraNatFirst 0x42abc0,
 * BraNatSecond 0x42d040) drop code 5 in the league stage (stage -1: codes 6 and 3 at 0x42abcf, 0x42abf6; codes 0
 * and 3 at 0x42d04f, 0x42d06f), so the wraps below hand code 5 to the base League handler 0x686940, which writes the
 * manager's title record (0x868c50 at 0x686957), as the classes do for their play-off winner (0x42ac3d).
 *
 * With 1 meeting (the base game, the rows of 2001 and the Série B rows 2002 to 2004, or no room for the dates) the
 * class plays its play-off as the original. The base game and RECOMP_LEAGUE_SIZES=off never change a league here
 * (season_sizes_rules calls season_sizes_bra_rules only with the size engine on, and the wraps below act only on a
 * Série A or B object whose stage count that function set to 0). */
#include "win.h"
#include "../rt_host.h"
#include "season_change.h"
#include "season_sizes.h"
#include "season_template.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_STAGES 0x0cu           /* the stage array */
#define COMP_STAGE_COUNT 0x2cu      /* s32: the stages after the league stage */
#define COMP_STAGE_NOW 0x30u        /* s32: the stage being played, -1 for the league stage */
#define COMP_MEETINGS 0x3cu         /* s16: meetings of each pair */
#define COMP_YEAR 0x40u             /* s16: the edition */
#define UP 0xbeu                    /* s8: the top places (the play-off places of Série A) */
#define SERIE_A_PLAYOFF_PLACES 4    /* 0x42a5ad, 0x42a5db */
#define RULE_FLAGS 0xd9u            /* u16: bit 1 a champion row, 2 promotion, 8 play-off rows (0x684dc0, 0x685191) */
#define FLAG_CHAMPION 0x01u
#define FLAG_PLAYOFF_ROWS 0x08u
#define F_BASE_RESULT 0x00686940u   /* League +0x44: code 5 the title record, 0 promotion, 3 relegation */
#define F_LEAGUE_HONOURS 0x00684640u /* League::record_season_honours (League vtable 0x96d020 +0x30), thiscall () */
#define STATUS_CHAMPION 5

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

void season_sizes_bra_rules(uint32_t obj, const char *file)
{
    if (!readable(obj) || !file) return;
    int first = !strcmp(file, "bra_nat_first");
    if (!first && strcmp(file, "bra_nat_second")) return;
    if ((int32_t)rd32(g_mem, obj + COMP_STAGE_NOW) >= 0) return;    /* a stage is live: set_up_rules mid-season */
    int meetings = (int16_t)rd16(g_mem, obj + COMP_MEETINGS);
    int32_t count = (int32_t)rd32(g_mem, obj + COMP_STAGE_COUNT);
    if (meetings >= 2) {
        wr32(g_mem, obj + COMP_STAGE_COUNT, 0);
        if (first && (int8_t)g_mem[obj + UP] == SERIE_A_PLAYOFF_PLACES) g_mem[obj + UP] = 0;
        uint16_t flags = rd16(g_mem, obj + RULE_FLAGS) | FLAG_CHAMPION;
        if (first) flags &= (uint16_t)~FLAG_PLAYOFF_ROWS;
        wr16(g_mem, obj + RULE_FLAGS, flags);
        if (count != 0 && getenv("RECOMP_SIZE_LOG"))
            fprintf(stderr, "recomp: league sizes: %s %d: %d meetings, no play-off\n", file,
                    (int16_t)rd16(g_mem, obj + COMP_YEAR), meetings);
        return;
    }
    /* One meeting after a season of two (no room for the dates): the play-off again, when the stage array is there
     * (a loaded game of a two-meeting season has none, League::load 0x688e49, and keeps the table's end). The
     * original's set_up_rules writes +0xbe 4 again. */
    if (count == 0 && readable(rd32(g_mem, obj + COMP_STAGES))) wr32(g_mem, obj + COMP_STAGE_COUNT, 1);
}

/* The dates. The classes play Sundays and Wednesdays from the first Sunday of August to early December (27 dates,
 * 0x429b50 and 0x42c140), with no gap over 7 days, so its 8 gaps of a week take the first new dates
 * (season_template.c). The real seasons begin in late March to May (Série A 2003 on 29 March, Série B 2006 in
 * April), so the earlier starts go back up to 13 weeks, to early May: Série A of 24 clubs (46 rounds) takes 11,
 * Série A and B of 20 (38 rounds) 3 (lead decision, slice 17). */
#define BRA_EARLIER 13

int season_sizes_bra_earlier(const char *file)
{
    return file && (!strcmp(file, "bra_nat_first") || !strcmp(file, "bra_nat_second")) ? BRA_EARLIER
                                                                                        : SEASON_TEMPLATE_EARLIER;
}

/* 1 for a Série A or B object that plays no play-off this season: its stage count is 0 (season_sizes_bra_rules). The
 * meetings are not read: a game loaded in a season of two meetings has no stage array (League::load 0x688e49), and
 * when the next season has no room for the dates the class's 1 meeting comes back with the count still 0
 * (season_sizes_bra_rules). The original 0x408eb0 would then jump through the absent stage 0. */
static int no_playoff(uint32_t obj)
{
    if (!readable(obj) || rd32(g_mem, obj + COMP_STAGE_COUNT) != 0) return 0;
    const char *file = season_change_file(obj);
    return file && (!strcmp(file, "bra_nat_first") || !strcmp(file, "bra_nat_second"));
}

/* The result handler (thiscall: club, code, stage, a4, a5, a6; ret 0x18): code 5 of the league stage goes to the base
 * League handler, the rest to the class's. */
static void result(void (*original)(void))
{
    uint32_t self = cpu.ecx;
    if ((rt_arg(1) & 0xff) != STATUS_CHAMPION || (rt_arg(2) & 0xff) != 0xff || !no_playoff(self)) {
        original();
        return;
    }
    uint32_t a[6];
    for (int i = 0; i < 6; i++) a[i] = rt_arg(i);
    Cpu saved = cpu;
    cpu.ecx = self;
    rt_callback(F_BASE_RESULT, 6, a);
    if (rt_unwinding) return;
    cpu = saved;
    if (getenv("RECOMP_SIZE_LOG"))
        fprintf(stderr, "recomp: league sizes: %s %d: champion club %.40s (the top of the table)\n",
                season_change_file(self), (int16_t)rd16(g_mem, self + COMP_YEAR),
                readable(a[0]) ? win_str(a[0] + 4) : "?");
    rt_ret_stdcall(0, 6);
}

void o_0042abc0(void);
RT_WRAP(0042abc0, bra_first_result) { result(o_0042abc0); }

/* The Série B play-off with fewer places than its 4 rows. In stage 0 the play-off winner (code 1) gets status 5
 * (0x42d098..0x42d0ba: the row of 0x68ab70, 0x868c50 the title record), and League::update_positions_and_status
 * 0x684cd0 gave the top +0xbe rows status 0. league_promote_and_relegate 0x689c80 moves up the rows of status 0 and 5
 * (0x689d47..0x689d4e). The class has 4 places, so its winner is always one of them. The rows 2002 to 2004 give 2
 * (Série B 2003 to 2005: the champion and the runner-up of the final group went up), so a winner from row 3 or 4
 * made 3 promotions against 2 relegations, and Série A one club over its count. So the winner takes the place of the
 * lowest row of status 0, which gets no status (0xff), and the promotions stay the places. */
#define ROW_TABLE 0xb1u
#define ROW_SIZE 0x3bu
#define ROW_STATUS 0x37u
#define COMP_CLUBS 0x3eu
#define STATUS_UP 0
#define STATUS_NONE 0xffu

static void playoff_places(uint32_t self)
{
    int places = (int8_t)g_mem[self + UP];
    uint32_t table = rd32(g_mem, self + ROW_TABLE);
    int n = (int16_t)rd16(g_mem, self + COMP_CLUBS);
    if (places <= 0 || !readable(table) || n <= 0 || n > 200) return;
    int moving = 0, last = -1;
    for (int i = 0; i < n; i++) {
        int st = g_mem[table + (uint32_t)i * ROW_SIZE + ROW_STATUS];
        if (st == STATUS_UP || st == STATUS_CHAMPION) moving++;
        if (st == STATUS_UP) last = i;
    }
    if (moving <= places || last < 0) return;
    uint32_t row = table + (uint32_t)last * ROW_SIZE;
    g_mem[row + ROW_STATUS] = STATUS_NONE;
    if (getenv("RECOMP_SIZE_LOG"))
        fprintf(stderr, "recomp: league sizes: bra_nat_second %d: the play-off winner goes up in place of row %d "
                        "(%.40s), %d places\n", (int16_t)rd16(g_mem, self + COMP_YEAR), last + 1,
                readable(rd32(g_mem, row)) ? win_str(rd32(g_mem, row) + 4) : "?", places);
}

void o_0042d040(void);
RT_WRAP(0042d040, bra_second_result)
{
    uint32_t self = cpu.ecx;
    int won = (rt_arg(1) & 0xff) == 1 && (rt_arg(2) & 0xff) == 0;
    result(o_0042d040);
    if (rt_unwinding || !won || !season_sizes_on() || !readable(self)) return;
    const char *file = season_change_file(self);
    if (file && !strcmp(file, "bra_nat_second")) playoff_places(self);
}

/* The honours. League::update_for_date 0x6847c0 calls vtable +0x30 when the season is complete (0x684a8f). The
 * Brazilian classes hold 0x408eb0 there, which jumps to +0x30 of stage 0 (0x408eb0..0x408eb7: [this+0xc][0]), the
 * play-off; the base League holds record_season_honours 0x684640, which takes rows 0 and 1 of the table +0xb1
 * (0x68465b..0x68465d). With no play-off stage 0 is not there (a jump to 0), so a Série A or B season with no
 * play-off takes the League's, as the OFC Nations Cup with no stage (season_intl_ofc.c). Other classes that hold
 * 0x408eb0 keep it. */
void o_00408eb0(void);
RT_WRAP(00408eb0, staged_honours)
{
    if (no_playoff(cpu.ecx)) {
        rt_jump(F_LEAGUE_HONOURS);
        return;
    }
    o_00408eb0();
}
