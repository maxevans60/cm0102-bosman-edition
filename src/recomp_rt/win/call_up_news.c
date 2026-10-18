/* The news item of a national squad call-up (type 29) with a player who is in the squad more than once.
 * Research: docs/research/2026-10-18-staff-reserve-boxes.md.
 *
 * NationalTeams::select_squad 0x76c120 sends one item for each club run of the new squad (0x76e0b0, called
 * at 0x76d3dc) before national_teams_remove_duplicate_squad_entries 0x76c060 takes the duplicates out
 * (0x76c02f, 0x76c03d). The item holds the count of players in slot 4 and four slots for each player from
 * slot 10, the staff id first (0x76e2ce..0x76e324). The text writer 0x76e3b0 reads the groups from slot 10
 * and skips a group whose staff id it already has, without a step of its counter (0x76e4c5..0x76e4e8). So
 * with a player listed three times it reads past the groups that the item holds, the empty slots all hold
 * 0, and it reads on until NewsItem::get_slot 0x785830 gets slot 50 and shows the box "news..cpp 7797".
 *
 * The wrap below runs the reader's loop on the slots first. Only when that loop would reach slot 50 does
 * the reader get, for this call, the count of different players among the groups of the item. Every other
 * item reads as in the original. The item itself does not change, so the saved game holds the same bytes. */
#include "win.h"
#include "../rt.h"

#include <stdio.h>
#include <stdlib.h>

#define SLOT(item, k) ((item) + 5u + 4u * (uint32_t)(k))   /* NewsItem::get_slot 0x785845 */
#define SLOTS 50                                           /* 0x78583e */
#define COUNT_SLOT 4                                        /* 0x76e24a, read back at 0x76e400 */
#define FIRST_GROUP 10                                      /* 0x76e2c4, 0x76e422 */
#define GROUP 4                                             /* four slots for each player */
#define MAX_COUNT 3                                         /* the groups are read only for 1 to 3 (0x76e418) */

void o_0076e3b0(void);

/* 1 when the reader of 0x76e3b0 would read a slot from 50 on before it has count different players. */
static int reader_overrun(uint32_t item, int count)
{
    uint32_t seen[SLOTS];
    int got = 0;
    for (int at = FIRST_GROUP; got < count; at += GROUP) {
        if (at + GROUP > SLOTS) return 1;
        uint32_t id = rd32(g_mem, SLOT(item, at));
        int dup = 0;
        for (int k = 0; k < got; k++) dup |= seen[k] == id;
        if (!dup) seen[got++] = id;
    }
    return 0;
}

RT_WRAP(0076e3b0, write_call_up_news_text)
{
    uint32_t item = rt_arg(0), count_at = SLOT(item, COUNT_SLOT), count_word = rd32(g_mem, count_at);
    int count = (int8_t)count_word;
    if (count < 1 || count > MAX_COUNT || !reader_overrun(item, count)) {
        o_0076e3b0();
        return;
    }
    uint32_t ids[MAX_COUNT];
    int distinct = 0;
    for (int g = 0; g < count; g++) {
        uint32_t id = rd32(g_mem, SLOT(item, FIRST_GROUP + GROUP * g)), dup = 0;
        for (int k = 0; k < distinct; k++) dup |= ids[k] == id;
        if (!dup) ids[distinct++] = id;
    }
    if (getenv("RECOMP_SCREEN_LOG"))
        fprintf(stderr, "recomp: call-up news lists a player more than once: %d groups, %d players\n", count,
                distinct);
    wr32(g_mem, count_at, (count_word & ~0xffu) | (uint32_t)distinct);
    o_0076e3b0();
    wr32(g_mem, count_at, count_word);
}
