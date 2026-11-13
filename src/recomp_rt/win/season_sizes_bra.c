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
 * With 1 meeting (the base game, the rows of 2001 and the Série B rows 2002 to 2004, or no room for the dates) the
 * class plays its play-off as the original. The base game and RECOMP_LEAGUE_SIZES=off never reach this file
 * (season_sizes_rules calls it only with the size engine on). */
#include "win.h"
#include "season_sizes.h"

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
