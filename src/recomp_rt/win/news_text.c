/* News items of a match that write no text (issues 161 and 162 of 0.7.3).
 *
 * NewsItem::write_text 0x77bc80, thiscall (manager, String *text, full, flag), ret 0x10, clears the text (0x77bd30) and
 * writes it by the item's type. Type 16, the result of a match in a competition (0x77e2e2), reads the fixture from
 * slot 5 on (comp_util_read_fixture_from_news_slots 0x4b0010) and calls slot +0x48 of the competition,
 * build_fate_news_text (0x77e370). Each class writes only the stages that the original gives it. For another stage
 * it returns with no text: WcOceaniaLeague 0x91e990 needs the round code 0xa0 above stage 1 (0x91eb2b), and the
 * tournament of season_wcq26.c plays stages 2 and 3 with code 0.
 *
 * The text stays a null pointer. The news list then shows "(null)", and the News filter passes the null to
 * search_text_contains 0x806260 (0x7887f6, 0x788826, 0x78897e, 0x7889a3), which stops with the box "search_eng.cpp
 * 881" (0x8063da, line 0x371). Reported save: two items of 31 March 2222, competition 437, stage 3, code 0.
 *
 * In a season game such an item takes the shared match text of Comp::build_fate_news_text 0x48c6d0, which writes the
 * result of any fixture (0x48c6d9: argument 5 set, 0x4b0b80 for the full text and 0x4b4420 for the headline). The
 * item in the save does not change, so the items of an older save read correctly too. The base game keeps the
 * original (season_intl_on, Q152). */
#include "win.h"
#include "news_text.h"
#include "season_intl.h"
#include "../rt.h"

#include <stdio.h>

#define TYPE_MATCH 16u
#define COMPS 0x00adadfcu               /* the competition objects, by id */
#define READ_FIXTURE 0x004b0010u        /* cdecl (item, fixture, signed char *slot) */
#define COMP_FATE_TEXT 0x0048c6d0u      /* thiscall, 10 arguments, ret 0x28 */
#define FIRST_SLOT 5u                   /* 0x77e314 */
#define FIXTURE_SIZE 0x100u             /* the reader writes fields up to +0x4c */
#define LOG_LIMIT 20
#define OCEANIA_VTABLE 0x00970d78u      /* WcOceaniaLeague */
#define OCEANIA_FINALS 3                /* the finals of the tournament of season_wcq26.c */
#define NAME_SEMI_FINAL 0x82u           /* round name codes of comp_util 0x4b4850 */
#define NAME_FINAL 0x96u

#define SLOT(item, k) ((item) + 5u + 4u * (uint32_t)(k))   /* NewsItem::get_slot 0x785845 */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* The arguments of the call at 0x77e370, in the order of the callee. */
static void write_match_text(uint32_t item, uint32_t comp, uint32_t text, uint32_t full)
{
    Cpu saved = cpu;
    cpu.esp -= FIXTURE_SIZE + 0x10u;
    uint32_t fixture = cpu.esp + 0x10u, slot_at = fixture + FIXTURE_SIZE - 4u;
    wr32(g_mem, slot_at, FIRST_SLOT);
    uint32_t read[3] = {item, fixture, slot_at};
    rt_callback(READ_FIXTURE, 3, read);
    if (rt_unwinding) return;
    int next = (int8_t)rd8(g_mem, slot_at);
    /* Both rounds of the tournament take the name code of round 0 (season_nl_path_round), so the finals take their own
     * name here. A final with a group code of 0 at +0x32 means that the team won the competition (0x4b2808, 0x4b2815).
     * A final of the tournament wins one place, so the full text takes the code at +0x32 too. The headline 0x4b4420
     * names both codes, so it keeps +0x32 at 0. Only this copy of the fixture changes. */
    if (rd32(g_mem, comp) == OCEANIA_VTABLE && (int8_t)rd8(g_mem, fixture + 0x42u) == OCEANIA_FINALS &&
        rd16(g_mem, fixture + 0x30u) == NAME_SEMI_FINAL) {
        wr16(g_mem, fixture + 0x30u, NAME_FINAL);
        if (full) wr16(g_mem, fixture + 0x32u, NAME_FINAL);
    }
    uint32_t args[10] = {rd32(g_mem, SLOT(item, 0)), (uint32_t)(int8_t)rd8(g_mem, SLOT(item, 2)),
                         (uint32_t)(int8_t)rd8(g_mem, fixture + 0x42u), rd16(g_mem, fixture + 0x32u), fixture,
                         rd16(g_mem, SLOT(item, next)), rd32(g_mem, SLOT(item, 4)),
                         (uint32_t)(int8_t)rd8(g_mem, SLOT(item, 3)), full, text};
    cpu.ecx = comp;
    rt_callback(COMP_FATE_TEXT, 10, args);
    if (!rt_unwinding) cpu = saved;
}

void news_text_written(uint32_t item, uint32_t text, uint32_t full)
{
    static int logged;
    if (!season_intl_on() || !readable(text) || rd32(g_mem, text)) return;
    uint32_t type = rd32(g_mem, item), comps = rd32(g_mem, COMPS);
    uint32_t id = rd32(g_mem, SLOT(item, 1));
    uint32_t comp = type == TYPE_MATCH && readable(comps) ? rd32(g_mem, comps + 4u * id) : 0;
    if (readable(comp)) {
        write_match_text(item, comp, text, full);
        if (rt_unwinding) return;
    }
    if (logged < LOG_LIMIT) {
        uint32_t written = rd32(g_mem, text);
        logged++;
        fprintf(stderr, "recomp: news: item of type %u, competition %u wrote no text: %s%.120s\n", type, id,
                written ? "shared match text: " : "still empty", written ? (const char *)g_mem + written : "");
    }
}
