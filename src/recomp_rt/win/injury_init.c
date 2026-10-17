/* The injury records at a new game (0.6.0 slice 15): a fix of a bug of the original that a season game can
 * reach.
 *
 * Injury::init_records 0x62cf70 (called once at a new game, from Injury::Injury 0x62a680 at 0x62a873, from
 * create_game at 0x829313) allocates one 31-byte record per staff (count [0xae2364], malloc at 0x62cf91) and
 * fills them in the order of the staff table: at 0x62d036..0x62d056 record i gets its values (+0xe, +0x14 and
 * +0x18 are -1 at 0x62d049..0x62d04f). In the same loop about one player in a hundred gets an injury or illness
 * (Injury::add_injury 0x62d250, called at 0x62d0df). An illness can spread to other players
 * (Injury::add_illness_outbreak 0x62e820, called at 0x62dbfd; it calls add_injury at 0x62eb6e), and a player of a
 * later staff row then has a record that the loop has not filled yet: fresh memory, all zero. add_injury reads
 * the current injury at +0xe (0x62d3f1); 0 is not -1, so it writes that injury to the injury history file
 * through FILE_LLIST::add_node 0x58e340 with the history offsets +0x14 and +0x18, both 0. With offsets that are
 * not -1 add_node opens the file "r+b" (0x58e5c1); the file does not exist yet, so the game stops at the box
 * "Disk Space Error" (file_llist.cpp 161, 0x58e601). A start in 2009/10 with an older club moves table showed it:
 * the illness of staff row 19736 reached row 19744; with this fix the same start reaches the game.
 *
 * The fix: while init_records runs, an add_injury call for a staff row after the row of the loop first gets the
 * history fields that the loop gives (+0xe, +0x14 and +0x18 = -1). The loop gives the row all its values later,
 * as before. Only a season game has the fix: the base game keeps the code of the original. */
#include "win.h"
#include "season_names.h"

#include <stdio.h>

#define STAFF_TABLE 0xae23bcu       /* the staff table, 0x6e bytes a staff */
#define STAFF_COUNT 0xae2364u
#define STAFF_SIZE 0x6eu
#define RECORD_SIZE 31u             /* 0x62cf83..0x62cf86: count * 31 */

static int in_init;                 /* init_records runs */
static int loop_row = -1;           /* the staff row of the loop: the row of the outer add_injury call */
static int depth;                   /* add_injury calls in progress */

void o_0062cf70(void);
void o_0062d250(void);

/* Injury::init_records 0x62cf70, thiscall, no arguments. */
RT_WRAP(0062cf70, injury_init_records)
{
    in_init = season_game_changes();
    loop_row = -1;
    depth = 0;
    o_0062cf70();
    in_init = 0;
    depth = 0;
}

/* Injury::add_injury 0x62d250, thiscall, 8 arguments (ret 0x20); the staff is the third (0x62d25a). The
 * record of a staff is at [this] + staff->id * 31 (0x62d29f, 0x62d3e3..0x62d3ee). */
RT_WRAP(0062d250, injury_add_injury)
{
    if (!in_init) {
        o_0062d250();
        return;
    }
    uint32_t staff = rt_arg(2), table = rd32(g_mem, STAFF_TABLE), records = rd32(g_mem, cpu.ecx);
    int count = (int)rd32(g_mem, STAFF_COUNT);
    if (staff && table && records && staff >= table) {
        int row = (int)((staff - table) / STAFF_SIZE), id = (int)rd32(g_mem, staff);
        if (depth == 0) loop_row = row;
        else if (row > loop_row && id >= 0 && id < count) {
            uint32_t r = records + (uint32_t)id * RECORD_SIZE;
            wr32(g_mem, r + 0xe, 0xffffffffu);
            wr32(g_mem, r + 0x14, 0xffffffffu);
            wr32(g_mem, r + 0x18, 0xffffffffu);
            fprintf(stderr, "recomp: injury records: staff row %d reached from row %d before its turn\n", row,
                    loop_row);
        }
    }
    depth++;
    o_0062d250();
    depth--;
}
