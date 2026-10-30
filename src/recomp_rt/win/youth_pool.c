/* The youth pool of every nation (0.6.2 slice 20, open item 52). This fixes a fault of the original that every
 * game reaches, the base 2001 game included.
 *
 * A player who retires gives his player data and his nation to the pool of PLAYER_REGEN (process_retirements
 * 0x7a59e0: add_player_data_to_pool 0x7a8350 at 0x7a6463, or queue_staff_for_regen 0x7a87d0, which pools the
 * player data with the nation **(staff +0x1a) and queues the staff slot). process_regen_queue 0x7a65d0 makes one
 * new player for each due slot of the queue with regenerate_as_youth_player 0x7abac0. That function takes the
 * nation of the first pool entry (this +0x3c, entry +4, read at 0x7abb13), so each nation gets back about as many
 * new players as it loses.
 *
 * The age of the new player comes from his nation (0x7abe87..0x7abeee). When the nation is not loaded (league
 * flags +0x11c are 0) and it had 349 players or fewer at the start (+0x88 below 0x15e), the age is 23 to 27 when
 * rand(+0x88) < rand(1000), else 17 to 23. Nation +0x88 is the number of staff with player data of that nation,
 * counted once by database_trim_staff_tables 0x524d70 and never again. Any other nation gets 15 to 20 in nearly
 * all cases (0x7ac3dd). The birth year is the game year less the age (0x7ac7f4 in the Ghidra listing: staff +0x12).
 * So nearly every new player of a small nation that is not loaded is 23 or older. Its young players of the data
 * grow up and none follow: in a 2001 game with England only, Croatia, Nigeria and Norway have no player under 21
 * by July 2004, and 13 nations with 23 players or more have none by 2007 (0.6.0 long game, seed 5). The forum
 * save of January 2041 (Greece only) has 24 such nations, Belgium among them. Their under-21 and Olympic squads
 * stay empty.
 *
 * The fix has two parts. Both keep the code of the original while every nation has its youth pool.
 *
 * 1. Young ages. When the pool nation of a new player has the older ages, 23 players or more, and fewer than 1 in
 *    20 of them under 21 or fewer than 1 in 10 under 23 (youth_pool_short), the wrap of 0x7abac0 sets that
 *    nation's +0x88 to 0x15e for the call and puts the old value back after it. +0x88 is read only in this
 *    function (0x7abcd7, 0x7abd15, 0x7abea0), and no save runs in the call. With 0x15e the nation keeps the new
 *    player (the change of nation at 0x7abcd7 needs fewer than 100) and he gets the young ages of 0x7ac3dd.
 * 2. A new player for a nation that runs out. A nation gets new players only when its own players retire, and a
 *    young new player plays longer, so a nation of 23 to 30 players can still reach none under 21 for a while
 *    (a model of 40 such nations over 300 years: 2 % of the checks each decade). So when a nation that is not
 *    short makes a new player, and a nation with a continent and 23 players or more will have none under 21 or
 *    none under 23 in a year (none under 20 or none under 22 today, youth_pool_runs_out), the new player goes to
 *    that nation with the young ages: the wrap writes that nation into the pool entry (+4) for the call, with no
 *    second nation (+8), as the original's own change of nation does at 0x7abd1d. The original removes the entry
 *    in the call (0x7abb1a); when it returns before that, the wrap puts the entry back. With both parts the
 *    model has no empty nation in any decade, and a nation of under 30 players grows by at most 35 in 300 years
 *    (tests/seasons/test_youth_pool.c).
 *
 * A game changes from the first new player of a nation that is short or runs out. At the start of a 2001 game
 * no nation is either (the start, prematch and all-leagues fixtures; the lowest shares are 6 % under 21 and 13 %
 * under 23). In an England-only 2001 game the first is about 2003. A 2025 start has nations that are short in
 * its first year (Angola, Algeria and Belarus in July 2027).
 *
 * The counts come from one pass over the staff table at the first new player of each call of 0x7a65d0. Then each
 * new player changes the counts of his slot. A queued slot keeps its old player data until the new player takes
 * it, so the pass counts it under its old nation and age. */
#include "win.h"
#include "youth_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STAFF_TABLE 0xae23bcu       /* the staff table, 0x6e bytes a staff */
#define STAFF_COUNT 0xae2364u
#define STAFF_SIZE 0x6eu
#define NATION_TABLE 0xae23a8u      /* the nation table, 0x122 bytes a nation; the nation id is its row */
#define NATION_COUNT 0xae2350u
#define NATION_SIZE 0x122u
#define GAME_DATE 0xae2c90u         /* s16 day of the year from 0, then the u16 year */
#define MAX_NATIONS 1024

static int counted;                 /* the counts hold for this call of 0x7a65d0 */
static int players[MAX_NATIONS], under_20[MAX_NATIONS], under_21[MAX_NATIONS], under_22[MAX_NATIONS],
    under_23[MAX_NATIONS];

void o_007a65d0(void);
void o_007abac0(void);

static int nation_count(void)
{
    int n = (int)rd32(g_mem, NATION_COUNT);
    return n < MAX_NATIONS ? n : MAX_NATIONS;
}

/* One staff: its nation row and its age, or -1 for a staff with no player data or no nation. */
static int staff_nation(uint32_t s, int *age)
{
    uint32_t nations = rd32(g_mem, NATION_TABLE), n = rd32(g_mem, s + 0x1a);
    if (!rd32(g_mem, s + 0x61) || !n || n < nations || (n - nations) % NATION_SIZE) return -1;
    int row = (int)((n - nations) / NATION_SIZE);
    if (row >= nation_count()) return -1;
    int day = (int16_t)rd16(g_mem, GAME_DATE), year = (int)rd16(g_mem, GAME_DATE + 2);
    *age = youth_pool_age(year, day, (int16_t)rd16(g_mem, s + 0x12), (int16_t)rd16(g_mem, s + 0x10));
    return row;
}

static void add(uint32_t s, int step)
{
    int age, row = staff_nation(s, &age);
    if (row < 0) return;
    players[row] += step;
    under_20[row] += step * (age < 20);
    under_21[row] += step * (age < 21);
    under_22[row] += step * (age < 22);
    under_23[row] += step * (age < 23);
}

static void count_all(void)
{
    memset(players, 0, sizeof players);
    memset(under_20, 0, sizeof under_20);
    memset(under_21, 0, sizeof under_21);
    memset(under_22, 0, sizeof under_22);
    memset(under_23, 0, sizeof under_23);
    uint32_t staff = rd32(g_mem, STAFF_TABLE);
    int n = (int)rd32(g_mem, STAFF_COUNT);
    for (int i = 0; staff && i < n; i++) add(staff + (uint32_t)i * STAFF_SIZE, 1);
    counted = 1;
}

/* The nation with a continent that runs out first: the most players, then the lowest row. -1 for none. */
static int runs_out(int except)
{
    uint32_t nations = rd32(g_mem, NATION_TABLE);
    int best = -1;
    for (int n = 0; n < nation_count(); n++) {
        if (n == except || !rd32(g_mem, nations + (uint32_t)n * NATION_SIZE + 0x71)) continue;
        if (youth_pool_runs_out(players[n], under_20[n], under_22[n]) && (best < 0 || players[n] > players[best]))
            best = n;
    }
    return best;
}

static void log_act(const char *what, int nation)
{
    if (!getenv("RECOMP_YOUTH_LOG")) return;
    char name[52];
    memcpy(name, g_mem + rd32(g_mem, NATION_TABLE) + (uint32_t)nation * NATION_SIZE + 4, 51);
    name[51] = 0;
    fprintf(stderr, "recomp: youth pool: %d/%d %s %s: %d players, %d under 21, %d under 23, %d under 20, "
            "%d under 22\n", (int16_t)rd16(g_mem, GAME_DATE), (int)rd16(g_mem, GAME_DATE + 2), what, name,
            players[nation], under_21[nation], under_23[nation], under_20[nation], under_22[nation]);
}

/* PLAYER_REGEN::process_regen_queue 0x7a65d0, thiscall, no arguments. */
RT_WRAP(007a65d0, youth_pool_regen_queue)
{
    counted = 0;
    o_007a65d0();
    counted = 0;
}

/* PLAYER_REGEN::regenerate_as_youth_player 0x7abac0, thiscall (ret 4), the staff slot on the stack. */
RT_WRAP(007abac0, youth_pool_new_player)
{
    uint32_t self = cpu.ecx, slot = rt_arg(0), nations = rd32(g_mem, NATION_TABLE);
    uint32_t pool = rd32(g_mem, self + 0x3c), pool_count = rd32(g_mem, self + 0x40);
    int nation = pool && pool_count && nations ? (int)rd32(g_mem, pool + 4) : -1;
    if (nation < 0 || nation >= nation_count()) {
        o_007abac0();
        return;
    }
    if (!counted) count_all();
    uint32_t rec = nations + (uint32_t)nation * NATION_SIZE;
    int target = nation, young = youth_pool_older_rule(g_mem[rec + 0x11c], (int)rd32(g_mem, rec + 0x88)) &&
                                 youth_pool_short(players[nation], under_21[nation], under_23[nation]);
    if (young) log_act("young", nation);
    else if ((target = runs_out(nation)) >= 0) {
        young = 1;
        log_act("new player for", target);
    } else target = nation;
    uint32_t id = rd32(g_mem, pool), old_nation = rd32(g_mem, pool + 4), old_second = rd32(g_mem, pool + 8);
    uint32_t trec = nations + (uint32_t)target * NATION_SIZE, start_players = rd32(g_mem, trec + 0x88);
    if (target != nation) {
        wr32(g_mem, pool + 4, (uint32_t)target);
        wr32(g_mem, pool + 8, 0);
    }
    if (young && (int)start_players <= YOUTH_POOL_OLDER_MAX) wr32(g_mem, trec + 0x88, YOUTH_POOL_OLDER_MAX + 1);
    add(slot, -1);
    o_007abac0();
    add(slot, 1);
    wr32(g_mem, trec + 0x88, start_players);
    if (target != nation && rd32(g_mem, self + 0x3c) == pool && rd32(g_mem, self + 0x40) == pool_count &&
        rd32(g_mem, pool) == id) {                  /* it returned before it took the entry (0x7abae6, 0x7abaf1) */
        wr32(g_mem, pool + 4, old_nation);
        wr32(g_mem, pool + 8, old_second);
    }
}
