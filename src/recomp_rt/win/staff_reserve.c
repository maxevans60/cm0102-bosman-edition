/* The reserve of blank staff rows (0.6.0, tools/recomp/staff_reserve.py).
 *
 * A database load reserves 23,000 blank staff rows (0x528540). A new game fills them from the top down with
 * the extra coaches, scouts, physios and chairmen that database_generate_extra_non_playing_staff 0x527240
 * makes for the clubs of the selected nations. With every league selected, some Update Team databases need
 * more than 23,000, and the game stops at the error box "Database.cpp 1844" (0x527e4c). The research is in
 * docs/research/2026-10-17-select-all-databases.md.
 *
 * The official data sets keep 23,000, so the base game allocates and plays as before. A custom data set
 * (database = folder:<name>) gets RESERVE_CUSTOM rows. The test switch RECOMP_STAFF_RESERVE=<rows> sets any
 * size from 23,000 up. RECOMP_SCREEN_LOG=1 prints how many rows a new game used. */
#include "options.h"
#include "../rt.h"

#include <stdio.h>
#include <stdlib.h>

#define RESERVE_BASE 23000u
#define RESERVE_CUSTOM 46000u    /* twice the original: the season databases need up to 27,156 */
#define RESERVE_MAX 400000u

#define STAFF_COUNT 0xae2364u   /* g_staff_count */
#define STAFF_TABLE 0xae23bcu   /* g_staff_table, 0x6e bytes a row */
#define STAFF_ROW 0x6eu
#define MANAGER_ROWS 16u        /* the last rows of the table belong to the human managers (0x52a9d3) */
#define CLUB_JOB 0x3du         /* 8, 9 and 10 from loops 1 to 3 of 0x527240, 5 from loop 4 */
#define ROW_STATE 0x5fu         /* 6 in a blank reserve row (0x52a1c2 loop), 1 in a generated row */

uint32_t rt_staff_reserve_size(void)
{
    uint32_t r = options_enum(OPT_DATABASE) == DATABASE_CUSTOM ? RESERVE_CUSTOM : RESERVE_BASE;
    const char *v = getenv("RECOMP_STAFF_RESERVE");
    if (v && *v) {
        long n = strtol(v, NULL, 10);
        if (n >= (long)RESERVE_BASE && n <= (long)RESERVE_MAX) r = (uint32_t)n;
        else fprintf(stderr, "recomp: RECOMP_STAFF_RESERVE=%s is not %u to %u rows\n", v, RESERVE_BASE, RESERVE_MAX);
    }
    rt_staff_reserve = r;
    return r;
}

/* database_generate_extra_non_playing_staff: the test log counts the reserve rows that it used. The wrap
 * only reads memory after the call. */
void o_00527240(void);
RT_WRAP(00527240, generate_extra_staff)
{
    o_00527240();
    if (!getenv("RECOMP_SCREEN_LOG")) return;
    uint32_t count = rd32(g_mem, STAFF_COUNT), table = rd32(g_mem, STAFF_TABLE), used = 0, job[4] = {0};
    for (uint32_t i = 1; i <= rt_staff_reserve && i + MANAGER_ROWS <= count; i++) {
        uint32_t row = table + (count - MANAGER_ROWS - i) * STAFF_ROW;
        if (g_mem[row + ROW_STATE] == 6) continue;
        used++;
        switch (g_mem[row + CLUB_JOB]) {     /* the job that each of the four loops gives */
        case 8: job[0]++; break;
        case 9: job[1]++; break;
        case 10: job[2]++; break;
        default: job[3]++; break;
        }
    }
    fprintf(stderr, "recomp: extra non-playing staff: %u of %u reserve rows used (loops: %u job 8, %u job 9, "
            "%u job 10, %u chairmen), result %u\n", used, rt_staff_reserve, job[0], job[1], job[2], job[3], cpu.eax);
}
