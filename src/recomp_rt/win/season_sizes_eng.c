/* England: League Two and the Conference exchange the clubs of the places of their rows (0.7.2 league sizes, slice 8,
 * D15; spec docs/specs/2026-11-13-league-sizes-0.7.2-spec.md).
 *
 * The original. EngPrm::start_new_season 0x574c10 runs the chain (vtable +0xb0, 0x574c23) and then one of two
 * Conference exchanges (0x574c3a: the Conference object of [0x9cf69c] in the object table [0xadadfc]):
 * - A Conference that the game plays: the chain body 0x5752f0 calls league_promote_and_relegate 0x689c80 for League
 *   Two and the Conference (0x57548d, pairs 1) when the Conference champion may go up (0x599e00 at 0x57541a). Else the
 *   last row of League Two gets status 0xfe (0x575438) and stays, and no club moves (0x57545f, 0x575468). In pairs
 *   the call moves as many clubs as the places of both divisions give, so this path takes the places of the rows once
 *   both divisions have them (season_sizes.c sizes_rules). exchange_conference_feeder_clubs 0x5750a0 then swaps 3
 *   clubs of three feeder leagues (push 3 at 0x5751f8, cmp bl, 3 at 0x575289) with the Conference rows of status 3.
 * - A Conference that the game does not play (the default: the setup screen has it as an extra division):
 *   exchange_conference_clubs_unplayed 0x574e60 lists the clubs of the Conference record (0x574ed9..0x574f01), sorts
 *   them and shuffles the top 1 (comp_util_sort_and_shuffle_top_clubs 0x4afbd0, push 1 at 0x574f7c), moves the first
 *   to League Two when it may go up (0x599e00 at 0x574fb7; League::move_club_to_division 0x6830b0 at 0x57502e), and
 *   then sends every League Two row of status 3 to the Conference record (0x6831a0 at 0x575070). An ineligible first
 *   club leaves the last League Two row at 0xfe and moves nothing (0x574fcd, 0x575002). So it is 1 up and every row of
 *   status 3 down: League Two relegates 1 in the class (EngThird::set_up_rules 0x578111), and with the 2 of the rows
 *   from 2002/03 it fell to 23 clubs (slice 4).
 *
 * The fix. League Two takes the relegation places of its row in a season game whether or not the Conference object
 * exists (season_sizes_eng_below): the object of a played Conference can be made after League Two at a new game, and
 * an unplayed one has none. The wrap of 0x574e60 runs the original and then moves up as many more clubs of the
 * Conference record as went down beyond the one it moved, the best by reputation that may go up (0x599e00 as
 * 0x574f85..0x574fb7). The base game and a League Two of one relegation place keep the original alone.
 *
 * Lead decision (sz08, 14 November 2026, Max away): the feeder exchange 0x5750a0 stays the class's 3 for 3. Its three
 * slots take one club from each of three feeder leagues (0x57520f..0x575245); the leagues below the Conference are
 * no divisions of the rules table, and the size step (D13) brings the Conference to its row's count. */
#include "win.h"
#include "../rt_host.h"
#include "season_change.h"
#include "season_sizes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUBS 0xae23b4u             /* the club table */
#define CLUB_COUNT 0xae235cu
#define CLUB_SIZE 0x245u
#define CLUB_DIVISION 0x57u         /* club_comp pointer; the record's first dword is its id (0x574ee0) */
#define CLUB_REPUTATION 0x80u       /* s16 */
#define COMP_OBJECTS 0xadadfcu      /* the competition objects by record id */
#define COMP_SLOTS 0xadadf8u
#define COMP_RECORD 4u              /* object +4: the competition record */
#define COMP_CLUBS 0x3eu
#define COMP_TABLE 0xb1u
#define ROW_SIZE 0x3bu
#define ROW_STATUS 0x37u
#define KEY_THIRD 0x9cf5c8u         /* League Two's record id (0x574e74) */
#define KEY_CONF 0x9cf69cu          /* the Conference's record id (0x574ee2) */
#define RECORDS 0xae23d0u           /* the competition records, 0x6b bytes each (0x575012..0x575028) */
#define RECORD_SIZE 0x6bu
#define CHECKER 0xae2a38u           /* thiscall object of 0x5a0590 (0x574fa9) */
#define F_CHECKER_OF 0x005a0590u    /* thiscall (club): the object whose 0x599e00 says whether the club may go up */
#define F_MAY_GO_UP 0x00599e00u     /* thiscall (+0xe4, +0xe2 of League Two, its record id, 1): al 1 when it may */
#define F_MOVE_CLUB 0x006830b0u     /* League::move_club_to_division, thiscall (club, record, 1) (0x57502e) */
#define STATUS_DOWN 3

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int logging(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_SIZE_LOG") && !strcmp(getenv("RECOMP_SIZE_LOG"), "1");
    return on;
}

int season_sizes_eng_below(const char *file)
{
    return file && !strcmp(file, "eng_third");
}

static uint32_t call_this(uint32_t fn, uint32_t self, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t object_of(uint32_t key)
{
    int32_t id = (int32_t)rd32(g_mem, key), slots = (int32_t)rd32(g_mem, COMP_SLOTS);
    uint32_t table = rd32(g_mem, COMP_OBJECTS);
    if (id < 0 || id >= slots || !readable(table)) return 0;
    uint32_t obj = rd32(g_mem, table + 4u * (uint32_t)id);
    return readable(obj) ? obj : 0;
}

/* 1 when the club's division is the record of the id at key. */
static int in_division(uint32_t club, uint32_t key)
{
    uint32_t d = rd32(g_mem, club + CLUB_DIVISION);
    return readable(d) && rd32(g_mem, d) == rd32(g_mem, key);
}

static const char *name_of(uint32_t club) { return win_str(club + 4); }

static int by_reputation(const void *a, const void *b)
{
    int ra = (int16_t)rd16(g_mem, *(const uint32_t *)a + CLUB_REPUTATION);
    int rb = (int16_t)rd16(g_mem, *(const uint32_t *)b + CLUB_REPUTATION);
    if (ra != rb) return rb - ra;
    return *(const uint32_t *)a < *(const uint32_t *)b ? -1 : *(const uint32_t *)a > *(const uint32_t *)b;
}

/* As 0x574f85..0x574fbc: the club may go up to League Two. */
static int may_go_up(uint32_t third, uint32_t club)
{
    uint32_t a1[1] = {club};
    uint32_t checker = call_this(F_CHECKER_OF, rd32(g_mem, CHECKER), 1, a1);
    if (rt_unwinding || !readable(checker)) return 0;
    uint32_t rec = rd32(g_mem, third + COMP_RECORD);
    uint32_t a2[4] = {(uint32_t)(int32_t)(int16_t)rd16(g_mem, third + 0xe4),
                      (uint32_t)(int32_t)(int16_t)rd16(g_mem, third + 0xe2), readable(rec) ? rd32(g_mem, rec) : 0, 1};
    return (call_this(F_MAY_GO_UP, checker, 4, a2) & 0xff) != 0;
}

void o_00574e60(void);

/* EngPrm::exchange_conference_clubs_unplayed 0x574e60, thiscall (EngPrm), no arguments. */
RT_WRAP(00574e60, eng_conf_unplayed)
{
    uint32_t self = cpu.ecx, third = object_of(KEY_THIRD);
    uint32_t clubs = rd32(g_mem, CLUBS);
    int count = (int32_t)rd32(g_mem, CLUB_COUNT), rows = third ? (int16_t)rd16(g_mem, third + COMP_CLUBS) : 0;
    uint32_t table = third ? rd32(g_mem, third + COMP_TABLE) : 0;
    int down = 0;
    for (int i = 0; readable(table) && i < rows && i < 64; i++)
        down += g_mem[table + (uint32_t)i * ROW_SIZE + ROW_STATUS] == STATUS_DOWN;
    uint32_t *conf = NULL, *went = NULL;
    if (season_sizes_on() && down > 1 && readable(clubs) && count > 0 && count < 100000) {
        conf = malloc(sizeof *conf * (size_t)count);
        went = malloc(sizeof *went * (size_t)down);
    }
    if (!conf || !went) {
        free(conf);
        free(went);
        o_00574e60();
        return;
    }
    /* Before the original: the clubs of the Conference record and the League Two rows of status 3. */
    int nconf = 0, nwent = 0;
    for (int i = 0; i < count; i++) {
        uint32_t club = clubs + (uint32_t)i * CLUB_SIZE;
        if (in_division(club, KEY_CONF)) conf[nconf++] = club;
    }
    for (int i = 0; i < rows && i < 64 && nwent < down; i++) {
        uint32_t row = table + (uint32_t)i * ROW_SIZE;
        if (g_mem[row + ROW_STATUS] == STATUS_DOWN) went[nwent++] = rd32(g_mem, row);
    }
    o_00574e60();
    if (rt_unwinding) {
        free(conf);
        free(went);
        return;
    }
    int moved_down = 0, moved_up = 0;
    for (int k = 0; k < nwent; k++) moved_down += readable(went[k]) && in_division(went[k], KEY_CONF);
    for (int k = 0; k < nconf; k++) moved_up += in_division(conf[k], KEY_THIRD);
    int need = moved_down - moved_up;
    if (logging())
        fprintf(stderr, "recomp: league sizes: eng: the unplayed Conference exchange moved %d down and %d up (%d rows "
                        "of status 3)\n", moved_down, moved_up, down);
    if (need > 0) {
        qsort(conf, (size_t)nconf, sizeof *conf, by_reputation);
        uint32_t rec = rd32(g_mem, KEY_THIRD);
        for (int k = 0; k < nconf && need > 0 && !rt_unwinding; k++) {
            uint32_t club = conf[k];
            if (!in_division(club, KEY_CONF)) continue;
            if (!may_go_up(third, club)) {
                if (logging()) fprintf(stderr, "recomp: league sizes: eng: %.51s may not go up\n", name_of(club));
                continue;
            }
            uint32_t a[3] = {club, rd32(g_mem, RECORDS) + rec * RECORD_SIZE, 1};
            call_this(F_MOVE_CLUB, self, 3, a);
            need--;
            if (logging()) fprintf(stderr, "recomp: league sizes: eng: %.51s goes up to League Two\n", name_of(club));
        }
        if (need > 0 && !rt_unwinding)
            fprintf(stderr, "recomp: league sizes: eng: %d places of League Two left with no Conference club that "
                            "may go up\n", need);
    }
    free(conf);
    free(went);
}
