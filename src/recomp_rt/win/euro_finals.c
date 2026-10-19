/* The team list of the European Championship finals with fewer teams than places.
 * Research: docs/research/2026-10-18-staff-reserve-boxes.md.
 *
 * EuroChamp::set_up_team_list 0x57bd80 makes 16 slots of 6 bytes (+0 the national team club, +4 a flag) at
 * +0x14, with the count at +0x36 and the size at +0x38 (0x57bd93). It puts the hosts of HostCountry first
 * (0x57be34, flag 1) and a default list of 14 key nations in slots 2 to 15 (0x57bf2f..0x57c104, flag 0).
 * EuroChamp::add_qualified_team 0x57c120 then writes each qualified team at the count (flag 6), over the
 * defaults. EuroChamp::seed_teams 0x57c710 (called from EuroChamp::update_for_date at 0x57c698, on the seeding
 * date +0x13e) copies the count of teams into the 16 seeds at +0xee (0x57c9ac..0x57c9d8), and
 * EuroChamp::run_group_draw 0x57c1d0 (0x57c6ee, a later date) draws all 16 seeds (0x57c3b7..0x57c46b). When the
 * hosts and the qualifiers together are fewer than 16, the last seeds hold no club, and a group gets a row with
 * no club. The group draw news
 * (League::send_group_draw_news 0x684230, through 0x7875e0 and 0x600a50) then reads that row as a club, and
 * 0x53b3d0 stops at the box "Database..cpp 13126". In a game from October 2023 the finals of 2024 get one host
 * and 14 qualifiers, and the host is a qualifier too.
 *
 * The wrap below runs before the seeding, only when the count is below the size. It takes out a team that is in
 * the list twice or has no nation (club +0x53); the first of two stays, so a host keeps its slot. It fills the
 * free slots with the national teams of the default key nations of 0x57bf2f that are not in the list and have a
 * nation, in the order of that list, flag 0 as the defaults have. A full list seeds and draws as in the
 * original. The test switch RECOMP_BOX_FIXES=off turns the wrap off (tests/seasons/season_boxes_run.py). */
#include "euro_finals.h"
#include "win.h"
#include "../rt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLOT_SIZE 6u
#define TEAMS 0x14u       /* the slots (0x57bda1) */
#define COUNT 0x36u       /* s16, the teams in the list (0x57be7d, 0x57c1bf) */
#define SIZE 0x38u        /* s16, 16 (0x57bd93) */
#define MAX_SLOTS 16
#define NATIONS 0xae23a8u /* the nation table, 0x122 bytes a nation */
#define NATION_ROW 0x122u
#define NATION_COUNT 0xae2350u
#define CLUBS 0xae23b4u   /* the club table, 0x245 bytes a club */
#define CLUB_ROW 0x245u
#define CLUB_COUNT 0xae235cu
#define CLUB_NATION 0x53u /* the nation of a club, a pointer after the load */

/* The key ids of the default list, in the order of 0x57bf2f..0x57c0e3 (slots 2 to 15). */
static const uint32_t default_keys[] = {0x9cf360, 0x9cf2fc, 0x9cf488, 0x9cf45c, 0x9cf444, 0x9cf314, 0x9cf300,
                                        0x9cf510, 0x9cf2e4, 0x9cf2c0, 0x9cf228, 0x9cf2ac, 0x9cf268, 0x9cf408};

/* The national team club of the key nation in key, as set_up_team_list gets it (nation = 0xae23a8 + key * 0x122
 * at 0x57bf34, then 0x53b3d0: club row id + clubs - 2 * nations at 0x53b4c2), or 0 when it is not in the tables. */
static uint32_t key_national_team(uint32_t key)
{
    int32_t k = (int32_t)rd32(g_mem, key), nations = (int32_t)rd32(g_mem, NATION_COUNT),
            clubs = (int32_t)rd32(g_mem, CLUB_COUNT);
    uint32_t nation_table = rd32(g_mem, NATIONS), club_table = rd32(g_mem, CLUBS);
    if (k < 0 || k >= nations || !nation_table || !club_table) return 0;
    int32_t row = (int32_t)rd32(g_mem, nation_table + (uint32_t)k * NATION_ROW) + clubs - 2 * nations;
    return row >= 0 && row < clubs ? club_table + (uint32_t)row * CLUB_ROW : 0;
}

/* 1 for a club with a nation (club +0x53), the test of a real national team. */
static int has_nation(uint32_t club) { return club && rd32(g_mem, club + CLUB_NATION); }

/* The fill of a short list of the EuroChamp at ec, for the seeding (euro_finals.h). A slice that wraps 0x57c710
 * itself calls this first and leaves out the wrap below. */
void euro_finals_fill_short_list(uint32_t ec)
{
    uint32_t t = rd32(g_mem, ec + TEAMS);
    int count = (int16_t)rd16(g_mem, ec + COUNT), size = (int16_t)rd16(g_mem, ec + SIZE);
    const char *off = getenv("RECOMP_BOX_FIXES");
    if (t && count >= 0 && count < size && size <= MAX_SLOTS && !(off && !strcmp(off, "off"))) {
        uint32_t club[MAX_SLOTS];
        uint8_t flag[MAX_SLOTS];
        int n = 0;
        for (int i = 0; i < count; i++) {
            uint32_t c = rd32(g_mem, t + SLOT_SIZE * (uint32_t)i), dup = !has_nation(c);
            for (int k = 0; k < n; k++) dup |= club[k] == c;
            if (!dup) { club[n] = c; flag[n++] = g_mem[t + SLOT_SIZE * (uint32_t)i + 4]; }
        }
        for (size_t d = 0; d < sizeof default_keys / sizeof *default_keys && n < size; d++) {
            uint32_t c = key_national_team(default_keys[d]), dup = !has_nation(c);
            for (int k = 0; k < n; k++) dup |= club[k] == c;
            if (!dup) { club[n] = c; flag[n++] = 0; }
        }
        if (getenv("RECOMP_SCREEN_LOG"))
            fprintf(stderr, "recomp: European Championship finals with %d of %d teams, now %d\n", count, size, n);
        for (int i = 0; i < n; i++) {
            wr32(g_mem, t + SLOT_SIZE * (uint32_t)i, club[i]);
            g_mem[t + SLOT_SIZE * (uint32_t)i + 4] = flag[i];
        }
        wr16(g_mem, ec + COUNT, (uint16_t)n);
    }
}

/* EuroChamp::seed_teams 0x57c710 is wrapped in season_intl_euro.c (0.6.0 slice 27, euro_seeding), which calls
 * euro_finals_fill_short_list on the 16-team path before the original. */
