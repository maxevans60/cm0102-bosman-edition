/* National team stats in the club records (0.6.0 slice 28 fix: club_records.cpp 794). Evidence below; research
 * doc docs/research/2026-10-18-nations-league.md, section "National team stats in the club records". */
#include "win.h"
#include "season_comps.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int logging(void)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_SEASON_NL_LOG");
        on = v ? atoi(v) : 0;
    }
    return on;
}

void o_007c3920(void);

/* PlayerStats::get_category_record, thiscall (Player *player, char category), ret 8, returns the player's 32-byte
 * stats record of a category or 0 (0x7c3920..0x7c39b5). Club_Records::update_records_for_match 0x444930 asks it
 * for each player of a national team with category 5 (0x445f7b..0x445f96) and stops at the box "club_records.cpp
 * 794" (0x44602d) when the player has none. A player has a category 5 record only when his national team called
 * him up. The 2022 Finalissima (Argentina, with no other match in that window) and other matches of slice 28's
 * competitions can field players with none: box in a 2021 start with Greece (October 2021), text "No player stats
 * for Argentina playing in 2022 Finalissima". The record is only read there (player_stats_get_record_value
 * 0x7c2150 at 0x44653f, 0x446690, 0x4467f7), so in a season game this one call gets an empty record of its own
 * (no matches, so the default rating) in place of 0. Every other call and the base game keep the original's
 * answer (Q152). */
RT_WRAP(007c3920, club_records_national_stats)
{
    uint32_t ret = rd32(g_mem, cpu.esp);
    int category = (int8_t)rd8(g_mem, cpu.esp + 8u);
    o_007c3920();
    if (rt_unwinding || cpu.eax || ret != 0x00445f9bu || category != 5 || !season_comps_on()) return;
    static uint32_t empty;
    if (!empty) empty = win_static_alloc(0x20u);
    if (!empty) return;
    memset(g_mem + empty, 0, 0x20u);
    cpu.eax = empty;
    if (logging())
        fprintf(stderr, "recomp: season nl: a national team player with no stats record gets an empty one\n");
}
