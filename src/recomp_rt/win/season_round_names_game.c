/* The real round names in the game (season_round_names.h, Q370). Evidence: docs/research/2026-10-17-uefa-formats.md,
 * section "Round names".
 *
 * The round name function 0x4b4850, cdecl (buffer, size, id, flag), writes the name of a round code (or of a
 * stage id) into the caller's buffer and returns it: flag 0 the long set ("Quarter Final", jump tables
 * 0x4b954c to 0x4b9744), flag 1 the short set ("Quarter Finals", 0x4b6947 on). A code + 1 to + 4 names a
 * replay or a leg. Its 23 calls are in 12 functions, and each one only puts the text on a screen, into a news
 * text or into a printout (none compares it or keeps it in a save): the competition screens 0x491380
 * (0x491570, 0x491a71), 0x492980 (0x492b3a), 0x495050 (0x495202), the league progress screen 0x49b990
 * (0x49babb), the cup tie news 0x4b0b80 (0x4b0e68), the group news 0x4b4590 (0x4b45d0, 0x4b46fd), the labels
 * 0x4bfa30 (6 calls) and 0x4bffc0 (4 calls), the live league table 0x7280e0 (0x7284ce, 0x7285e4),
 * NewsItem::write_text 0x77bc80 (0x77fad9), the printouts 0x7d8120 (0x7d82e9) and SpaSecondB news 0x857e20
 * (0x857f0a). The name function knows no competition, so the functions that take one set it for the call:
 * the label 0x4bfa30 (a competition record, arg 0) and the competition screens 0x491380, 0x492980, 0x495050
 * and the printouts 0x7d8120 (arg 0). A call with no competition known gets the original's text.
 *
 * The text goes into the caller's buffer with its size, as the original's (no buffer of the runtime). The base
 * game (start 2001, changing rules off) and RECOMP_SEASON_CODE=off get the original's text. */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_round_names.h"

#include <stdio.h>
#include <string.h>

#define COMP_TABLE 0xadadfcu
#define COMP_SLOTS 0xadadf8u

static uint32_t context;                /* the competition object of the call that runs, or 0 */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* The competition object of a competition object, stage object or record (club_comp: +0 the id, the slot of
 * g_comp_object_table [0xadadfc]; an object has its vtable at +0 and its record at +4). */
static uint32_t comp_of(uint32_t p)
{
    if (!readable(p)) return 0;
    uint32_t rec = rd32(g_mem, p) >= 0x400000u ? rd32(g_mem, p + 4) : p;
    if (!readable(rec)) return 0;
    int32_t id = (int32_t)rd32(g_mem, rec), slots = (int32_t)rd32(g_mem, COMP_SLOTS);
    uint32_t table = rd32(g_mem, COMP_TABLE);
    if (id < 0 || id >= slots || !readable(table)) return 0;
    uint32_t obj = rd32(g_mem, table + 4u * (uint32_t)id);
    return readable(obj) && rd32(g_mem, obj + 4) == rec ? obj : 0;
}

static int names_on(void)
{
    return season_code_on() && !(season_start_year() == SEASON_FIRST_YEAR && !season_changing_rules());
}

void o_004b4850(void);
void o_004bfa30(void);
void o_00491380(void);
void o_00492980(void);
void o_00495050(void);
void o_007d8120(void);

/* The round name function. */
RT_WRAP(004b4850, round_name)
{
    uint32_t buf = rt_arg(0), size = rt_arg(1);
    int code = (int16_t)rt_arg(2), shrt = (int8_t)rt_arg(3) != 0;
    const char *name = NULL;
    if (context && names_on() && code > 0 && code < 1000 && readable(buf) && size >= 2 && size <= 0x1000)
        name = season_round_name(season_change_file(context), code - code % 10, (int16_t)rd16(g_mem, context + 0x40),
                                 shrt ? SEASON_ROUND_NAME_SHORT : SEASON_ROUND_NAME_LONG);
    if (!name) {
        o_004b4850();
        return;
    }
    /* The words of a replay or a leg, as the original's names write them ("Quarter Final First Leg",
     * "Quarter Final First Legs"). */
    static const char *const long_words[5] = {"", " Replay", " Second Replay", " First Leg", " Second Leg"};
    static const char *const short_words[5] = {"", " Replays", " Second Replays", " First Legs", " Second Legs"};
    int part = code % 10;
    snprintf(win_str(buf), size, "%s%s", name, part <= 4 ? (shrt ? short_words : long_words)[part] : "");
    rt_ret_cdecl(buf);
}

/* The functions that know their competition (arg 0) set the context for their calls of the name function.
 * The recompiler reads each RT_WRAP line as it is, so they are written out in full. */
/* comp_util_format_comp_stage_round_label (record, stage, round, ...) */
RT_WRAP(004bfa30, label_round_name)
{
    uint32_t saved = context;
    context = comp_of(rt_arg(0));
    o_004bfa30();
    context = saved;
}

/* comp_screens_draw_league_table_tab (comp, ...) */
RT_WRAP(00491380, table_tab_round_name)
{
    uint32_t saved = context;
    context = comp_of(rt_arg(0));
    o_00491380();
    context = saved;
}

/* comp_screens_draw_cup_rounds_tab (comp, ...) */
RT_WRAP(00492980, rounds_tab_round_name)
{
    uint32_t saved = context;
    context = comp_of(rt_arg(0));
    o_00492980();
    context = saved;
}

/* comp_screens_draw_average_points_tab (comp, ...) */
RT_WRAP(00495050, points_tab_round_name)
{
    uint32_t saved = context;
    context = comp_of(rt_arg(0));
    o_00495050();
    context = saved;
}

/* printouts_write_league_tables (record, file) */
RT_WRAP(007d8120, printout_round_name)
{
    uint32_t saved = context;
    context = comp_of(rt_arg(0));
    o_007d8120();
    context = saved;
}

/* The rules report (season_uefa.c): the name of a round of a competition as the game's formatter
 * comp_util_format_stage_round_name 0x4bffc0 writes it for a fixture (stage 0: the round alone), with the
 * competition as the context. Test runs only. Writes at most size bytes to out. */
void season_round_name_label(uint32_t obj, int code, char *out, int size)
{
    snprintf(out, (size_t)size, "-");
    if (!readable(obj)) return;
    uint32_t saved_esp = cpu.esp, saved_context = context;
    cpu.esp -= 0x400;
    uint32_t buf = cpu.esp + 0x10;                  /* 0x4bffc0 wants 200 bytes or more (0x4bffdc) */
    g_mem[buf] = 0;
    uint32_t a[5] = {0, (uint32_t)code, buf, 0x100, 0};
    Cpu saved = cpu;
    context = obj;
    rt_callback(0x004bffc0u, 5, a);
    context = saved_context;
    if (rt_unwinding) return;
    cpu = saved;
    snprintf(out, (size_t)size, "%.120s", win_str(buf));
    cpu.esp = saved_esp;
}
