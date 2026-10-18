/* International competitions in the original (0.6.0 slice 27): the host table and Australia's move to Asia
 * (season_intl.h). Research and evidence: docs/research/2026-10-18-international.md.
 *
 * Hosts. HostCountry::init_default_table 0x5f9970 fills the host table (global 0xb5e134: +0 the rows, +4 the
 * count as a word) with 57 rows of 0x22 bytes at a new game; the constructor 0x5f97c0 then sorts them by year
 * and date (0x5f995a, comparator 0x5fb3f0). A load reads the rows of the save instead (0x5fb160). Row: +0 the
 * key id of the competition (the value of its key global, as 0x9cf79c), +4 the year (word, 0 = every year),
 * +6/+0xa, +0xe/+0x12, +0x16/+0x1a three host pairs (nation ids, -1 for none), +0x1e the choice (0 to 2 the
 * chosen pair, 0xfd fixed, 0xfe not decided), +0x1f the decision day, +0x21 the years that the row moves on
 * 1 January once it lies in the past (HostCountry::daily_update 0x5fa850, 0x5faafa). find_hosts 0x5fa730 takes
 * the first row of the competition with the year, or with year 0, and returns pair 1 of a fixed row. In a
 * season game the wrap of 0x5f9970 writes the hosts of the table into the row of each edition from the start
 * year (a new row where the original has none): pair 1, choice fixed, no move. The other rows stay.
 *
 * Australia. The confederation of a nation is its continent pointer at nation +0x71 (sweep section 8).
 * Australia joined the AFC on 1 January 2006 (int-world row int-world-49). A season game from 2006/07 starts
 * with Australia in Asia; a game with changing rules on from an earlier season moves it on the first update
 * day from 1 January 2006. The fixed Oceania lists of the original then get New Zealand or another Oceania
 * nation in its place. */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_intl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOSTS 0xb5e134u                 /* HostCountry: +0 rows, +4 count (word) */
#define ROW_SIZE 0x22u
#define ROW_COMP 0x0u
#define ROW_YEAR 0x4u
#define ROW_PAIRS 0x6u                  /* six dwords */
#define ROW_CHOICE 0x1eu
#define ROW_DAY 0x1fu
#define ROW_CYCLE 0x21u
#define CHOICE_FIXED 0xfdu
#define DEFAULT_DAY 200                 /* 20 July, the day of most rows of init (0x5f9970) */

#define NATIONS 0xae23a8u               /* nation records, 0x122 bytes */
#define NATION_COUNT 0xae2350u
#define NATION_SIZE 0x122u
#define NATION_NAME 0x4u
#define NATION_CONTINENT 0x71u          /* continent record; its +0 is the continent id */
#define CONTINENT_ASIA 0x9cfa0cu        /* key continent ids (sweep section 8) */
#define CONTINENT_OCEANIA 0x9cfa18u
#define KEY_AUSTRALIA 0x9cf224u         /* key nation ids: OceaniaNations 0x799f9c, 0x5fab70 */
#define KEY_NEW_ZEALAND 0x9cf3f4u
#define KEY_NEW_CALEDONIA 0x9cf530u     /* left out of the Oceania section at 0x91fc89 */

#define GAME_DATE 0xae2c90u             /* s16 day of the year, s16 year */
#define AUSTRALIA_TO_ASIA 2006          /* 1 January 2006 (int-world-49) */

#define GAME_MALLOC 0x00944e46u         /* malloc, cdecl (size): the rows of init (0x5f98c3) */
#define GAME_FREE 0x009452cau           /* free, cdecl (block) */
#define GAME_QSORT 0x00945333u          /* qsort, cdecl (base, count, size, compare): 0x5f995a */
#define ROW_COMPARE 0x005fb3f0u
#define NATIONAL_TEAM 0x0053b3d0u       /* the national team club of a nation record, cdecl (nation) */

/* The key global of each class with host rows (the comp column of init, read by find_hosts). The Oceania
 * Nations Cup has no host row (it calls no find_hosts). */
static const struct { const char *file; uint32_t key; } host_comps[] = {
    {"world_cup", 0x9cf79cu}, {"euro_champ", 0x9cf7a4u}, {"african_nations", 0x9cf7acu},
    {"olympics", 0x9cf7a0u}, {"goldcup", 0x9cf7a8u}, {"asia_nations", 0x9cf790u}, {"fifa_confed", 0x9cf964u},
};
#define HOST_COMPS ((int)(sizeof host_comps / sizeof host_comps[0]))
#define KEY_CONFED 0x9cf964u

static int logging(void)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_SEASON_INTL_LOG");
        on = v && *v == '1';
    }
    return on;
}

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

/* ---- Nations ---- */

static int nation_count(void) { return (int)rd32(g_mem, NATION_COUNT); }
static uint32_t nation_at(int id) { return rd32(g_mem, NATIONS) + (uint32_t)id * NATION_SIZE; }

static int nation_valid(int id) { return id >= 0 && id < nation_count() && readable(rd32(g_mem, NATIONS)); }

/* The id of the first nation with one of the names ("China PR/China"), or -1. */
static int nation_by_names(const char *names)
{
    char one[64];
    const char *p = names;
    while (*p) {
        size_t n = strcspn(p, "/");
        snprintf(one, sizeof one, "%.*s", (int)n, p);
        for (int id = 0; id < nation_count(); id++)
            if (!strcmp(win_str(nation_at(id) + NATION_NAME), one)) return id;
        p += n;
        if (*p == '/') p++;
    }
    return -1;
}

static uint32_t continent_id(int id)
{
    uint32_t c = rd32(g_mem, nation_at(id) + NATION_CONTINENT);
    return readable(c) ? rd32(g_mem, c) : 0xffffffffu;
}

/* The key nation id in a key global, or -1. */
static int key_nation(uint32_t global)
{
    int id = (int)rd32(g_mem, global);
    return nation_valid(id) ? id : -1;
}

/* ---- Australia ---- */

/* 1 when Australia's continent is Asia in the game's data. */
static int australia_in_asia(void)
{
    if (!season_intl_on()) return 0;
    int aus = key_nation(KEY_AUSTRALIA);
    return aus >= 0 && continent_id(aus) == rd32(g_mem, CONTINENT_ASIA);
}

/* Moves Australia from Oceania to Asia: its continent pointer becomes that of an Asian nation. Returns 1 when
 * it moved. The pointer is the data's own, so a save keeps it. */
static int australia_to_asia(void)
{
    int aus = key_nation(KEY_AUSTRALIA);
    if (aus < 0 || continent_id(aus) != rd32(g_mem, CONTINENT_OCEANIA)) return 0;
    uint32_t asia = rd32(g_mem, CONTINENT_ASIA);
    for (int id = 0; id < nation_count(); id++) {
        uint32_t c = rd32(g_mem, nation_at(id) + NATION_CONTINENT);
        if (id != aus && readable(c) && rd32(g_mem, c) == asia) {
            wr32(g_mem, nation_at(aus) + NATION_CONTINENT, c);
            if (logging()) fprintf(stderr, "recomp: season intl: Australia moves to Asia\n");
            return 1;
        }
    }
    return 0;
}

/* The Oceania section leaves New Caledonia out (0x91fc89) and needs 10 entrants (0x91fbda). With Australia in
 * Asia, New Caledonia (a FIFA member from 2004, in the 2010 qualifying) takes its place, so the count stays. */
uint32_t rt_intl_oceania_left_out(uint32_t id)
{
    return australia_in_asia() ? 0xffffffffu : id;
}

/* ---- The host table ---- */

static int day_of(int month, int day)
{
    static const int first[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    return month >= 1 && month <= 12 && day >= 1 ? first[month - 1] + day - 1 : DEFAULT_DAY;
}

/* Writes the hosts of an edition into a row: pair 1 the first two hosts that the data holds, the other pairs
 * none, the choice fixed and no move. */
static void row_write(uint32_t row, uint32_t key, const SeasonIntlEdition *e)
{
    int ids[2] = {-1, -1}, n = 0;
    char host[128];
    const char *p = e->hosts;
    while (*p && n < 2) {
        size_t len = strcspn(p, ";");
        snprintf(host, sizeof host, "%.*s", (int)len, p);
        int id = nation_by_names(host);
        if (id >= 0) ids[n++] = id;
        else if (logging()) fprintf(stderr, "recomp: season intl: host %s of %s %d is not in the data\n", host,
                                    e->file, e->year);
        p += len;
        if (*p == ';') p++;
    }
    wr32(g_mem, row + ROW_COMP, key);
    wr16(g_mem, row + ROW_YEAR, (uint16_t)e->year);
    for (int k = 0; k < 6; k++) wr32(g_mem, row + ROW_PAIRS + 4u * (uint32_t)k, 0xffffffffu);
    wr32(g_mem, row + ROW_PAIRS, (uint32_t)ids[0]);
    wr32(g_mem, row + ROW_PAIRS + 4u, (uint32_t)ids[1]);
    wr8(g_mem, row + ROW_CHOICE, CHOICE_FIXED);
    wr8(g_mem, row + ROW_CYCLE, 0);
    if (logging()) {
        fprintf(stderr, "recomp: season intl: host %s %d: ", e->file, e->year);
        for (int k = 0; k < n; k++) fprintf(stderr, "%s%s", k ? ";" : "", win_str(nation_at(ids[k]) + NATION_NAME));
        fprintf(stderr, "%s\n", n ? "" : "-");
    }
}

static uint32_t row_find(uint32_t rows, int count, uint32_t key, int year)
{
    for (int i = 0; i < count; i++) {
        uint32_t row = rows + (uint32_t)i * ROW_SIZE;
        if (rd32(g_mem, row + ROW_COMP) == key && (int16_t)rd16(g_mem, row + ROW_YEAR) == year) return row;
    }
    return 0;
}

/* The rows of the editions from the start year. The table grows by the editions that have no row. */
static void hosts_apply(uint32_t self)
{
    uint32_t rows = rd32(g_mem, self);
    int count = (int16_t)rd16(g_mem, self + 4);
    if (!readable(rows) || count <= 0 || count > 1000) return;
    int start = season_start_year(), extra = 0;
    for (int c = 0; c < HOST_COMPS; c++) {
        uint32_t key = rd32(g_mem, host_comps[c].key);
        if (key == 0xffffffffu) continue;
        for (int i = 0; i < season_intl_edition_count; i++) {
            const SeasonIntlEdition *e = &season_intl_editions[i];
            if (!strcmp(e->file, host_comps[c].file) && e->year >= start && !row_find(rows, count, key, e->year))
                extra++;
        }
    }
    if (extra) {
        uint32_t size = (uint32_t)(count + extra) * ROW_SIZE;
        uint32_t args[1] = {size};
        uint32_t now = call_cdecl(GAME_MALLOC, 1, args);
        if (rt_unwinding || !readable(now)) return;
        memcpy(g_mem + now, g_mem + rows, (size_t)count * ROW_SIZE);
        memset(g_mem + now + (uint32_t)count * ROW_SIZE, 0, (size_t)extra * ROW_SIZE);
        uint32_t old[1] = {rows};
        call_cdecl(GAME_FREE, 1, old);
        if (rt_unwinding) return;
        rows = now;
        wr32(g_mem, self, rows);
    }
    int used = count;
    for (int c = 0; c < HOST_COMPS; c++) {
        uint32_t key = rd32(g_mem, host_comps[c].key);
        if (key == 0xffffffffu) continue;
        if (key == rd32(g_mem, KEY_CONFED)) {
            /* The Confederations Cup row of every year (year 0, host redrawn by 0x5fab70) would come first in
             * find_hosts: it leaves for the year that no game reaches. */
            uint32_t any = row_find(rows, used, key, 0);
            if (any) wr16(g_mem, any + ROW_YEAR, 2200);
        }
        for (int i = 0; i < season_intl_edition_count; i++) {
            const SeasonIntlEdition *e = &season_intl_editions[i];
            if (strcmp(e->file, host_comps[c].file) || e->year < start) continue;
            uint32_t row = row_find(rows, used, key, e->year);
            if (!row) {
                row = rows + (uint32_t)used++ * ROW_SIZE;
                wr16(g_mem, row + ROW_DAY, (uint16_t)day_of(e->month, e->day));
            }
            row_write(row, key, e);
        }
    }
    /* A row of the original that lies in the past moves on by its cycle on 1 January (0x5faafa) and takes a
     * random host. A row whose move would land on an edition of the table (the 1999 Copa America row on 2011,
     * the 2010 African Cup of Nations row on 2026) stops where it is, so the row of the table stays the only
     * one of its year. */
    for (int i = 0; i < used; i++) {
        uint32_t row = rows + (uint32_t)i * ROW_SIZE;
        int year = (int16_t)rd16(g_mem, row + ROW_YEAR), cycle = rd8(g_mem, row + ROW_CYCLE);
        const char *file = NULL;
        for (int c = 0; c < HOST_COMPS; c++)
            if (rd32(g_mem, host_comps[c].key) == rd32(g_mem, row + ROW_COMP)) file = host_comps[c].file;
        if (!file || year <= 0 || cycle <= 0) continue;
        for (int y = year + cycle; y <= SEASON_LAST_YEAR + 4; y += cycle)
            if (season_intl_edition(file, y)) {
                wr8(g_mem, row + ROW_CYCLE, 0);
                break;
            }
    }
    wr16(g_mem, self + 4, (uint16_t)used);
}

void o_005f9970(void);

/* HostCountry::init_default_table, thiscall (): the rows of a new game. The constructor sorts them after the
 * return (0x5f994a..0x5f995a), with the count of +4. The original's 57 random draws (0x5fa640..0x5fa70b) stay
 * as they are. */
RT_WRAP(005f9970, host_init)
{
    uint32_t self = cpu.ecx;
    o_005f9970();
    if (rt_unwinding || !season_intl_on()) return;
    hosts_apply(self);
}

void o_005fab70(void);

/* HostCountry::redraw_world_club_and_confed_hosts, thiscall: it writes a random nation into pair 1 of every
 * Confederations Cup row and swaps the host of the World Cup Oceania section between Australia and New
 * Zealand. In a season game the Confederations Cup rows of the table keep their hosts, and with Australia in
 * Asia the section's host is New Zealand. */
RT_WRAP(005fab70, host_redraw)
{
    uint32_t self = cpu.ecx;
    if (!season_intl_on()) {
        o_005fab70();
        return;
    }
    uint32_t rows = rd32(g_mem, HOSTS);
    int count = (int16_t)rd16(g_mem, HOSTS + 4);
    uint32_t key = rd32(g_mem, KEY_CONFED);
    enum { MAX_KEEP = 64 };
    uint32_t keep_at[MAX_KEEP], keep[MAX_KEEP];
    int kept = 0;
    for (int i = 0; readable(rows) && i < count && kept < MAX_KEEP; i++) {
        uint32_t row = rows + (uint32_t)i * ROW_SIZE;
        if (rd32(g_mem, row + ROW_COMP) == key && season_intl_edition("fifa_confed", (int16_t)rd16(g_mem, row + ROW_YEAR))) {
            keep_at[kept] = row + ROW_PAIRS;
            keep[kept++] = rd32(g_mem, row + ROW_PAIRS);
        }
    }
    cpu.ecx = self;
    o_005fab70();
    if (rt_unwinding) return;
    for (int k = 0; k < kept; k++) wr32(g_mem, keep_at[k], keep[k]);
    if (australia_in_asia()) {
        uint32_t section = rd32(g_mem, 0x9cf76cu);          /* World Cup Oceania Qualifying Section */
        int nz = key_nation(KEY_NEW_ZEALAND);
        for (int i = 0; nz >= 0 && i < count; i++) {
            uint32_t row = rows + (uint32_t)i * ROW_SIZE;
            if (rd32(g_mem, row + ROW_COMP) == section) wr32(g_mem, row + ROW_PAIRS, (uint32_t)nz);
        }
    }
}

void o_00799e30(void);

/* OceaniaNations::pick_entrants, thiscall (): six teams at +0xee: Australia and New Zealand by key id
 * (0x799f9c, 0x799fc1), then the two best Melanesian and the two best Polynesian nations (0x79a03c,
 * 0x79a04d). With Australia in Asia its place goes to the Oceania nation with the highest reputation (nation
 * +0x8e, inference from docs/research/2026-10-17-season-date-shifts.md) that is not in the six. */
RT_WRAP(00799e30, ofc_entrants)
{
    uint32_t self = cpu.ecx;
    o_00799e30();
    if (rt_unwinding || !cpu.eax || !australia_in_asia()) return;
    uint32_t pool = rd32(g_mem, self + 0xee);
    int aus = key_nation(KEY_AUSTRALIA);
    if (!readable(pool) || aus < 0) return;
    uint32_t args[1] = {nation_at(aus)};
    uint32_t team = call_cdecl(NATIONAL_TEAM, 1, args);
    if (rt_unwinding) return;
    int slot = -1;
    for (int k = 0; k < 6; k++)
        if (rd32(g_mem, pool + 4u * (uint32_t)k) == team) slot = k;
    if (slot < 0) return;
    uint32_t oceania = rd32(g_mem, CONTINENT_OCEANIA), best = 0;
    int best_rep = -1;
    for (int id = 0; id < nation_count(); id++) {
        if (id == aus || continent_id(id) != oceania) continue;
        args[0] = nation_at(id);
        uint32_t t = call_cdecl(NATIONAL_TEAM, 1, args);
        if (rt_unwinding) return;
        int in = 0;
        for (int k = 0; k < 6; k++) in |= rd32(g_mem, pool + 4u * (uint32_t)k) == t;
        int rep = rd16(g_mem, nation_at(id) + 0x8eu);
        if (!in && readable(t) && rep > best_rep) {
            best_rep = rep;
            best = t;
        }
    }
    if (best) wr32(g_mem, pool + 4u * (uint32_t)slot, best);
}

/* ---- Finals that start before their qualifying can end ---- */

#define SLOTS 0x14u                     /* team slots: 6 bytes each, +0 the team, +4 a flag */
#define SLOT_COUNT 0x36u                /* word: the teams in the slots */
#define SLOT_ROOM 0x38u                 /* word: 32 for the World Cup (0x92bffa), 16 for the Euro (0x57bd93) */
#define SLOT_SIZE 6u
#define NATION_REPUTATION 0x8eu         /* s16, 500 to 9500 in the 3.9.68 nation.dat (Brazil 9500) */
#define CONTINENT_EUROPE 0x9cfa10u

/* A game that starts in a later season meets finals whose qualifying began before the start: the sections of
 * a 2005 start play the 2004 campaign from July 2005 and cannot end before the World Cup draw of December 2005,
 * so WorldCup::seed_teams 0x92f230 finds fewer than 32 teams and stops at the box "world_cup.cpp 1438"
 * (0x92f24c, 0x92f2b1). The main build does the same (a run of the main build with September 2005 stops at that
 * box). EuroChamp::run_group_draw 0x57c1d0 of a 2003 start meets empty slots and stops at the box
 * "comp_stats.CPP 1664" (Comp_Stats::set_club_final_position 0x4a2540 with no club, from
 * EuroChamp::set_predicted_positions 0x57c9f0). In a season game the slots that no qualifier filled get the
 * class's default nations still in them (written by the team slot builders 0x92dbd5..0x92dfb3, 0x57bf2f),
 * then the nations with the highest reputation (European nations for the Euro) that are not in yet. */
static int team_in(const uint32_t *teams, int n, uint32_t t)
{
    for (int k = 0; k < n; k++)
        if (teams[k] == t) return 1;
    return 0;
}

static void fill_slots(uint32_t comp, uint32_t continent, const char *name)
{
    uint32_t slots = rd32(g_mem, comp + SLOTS);
    int count = (int16_t)rd16(g_mem, comp + SLOT_COUNT), room = (int16_t)rd16(g_mem, comp + SLOT_ROOM);
    if (!readable(slots) || count < 0 || room <= 0 || room > 64) return;
    if (count > room) count = room;
    uint32_t teams[64], spare[64];
    int open[64], n_open = 0, n_spare = 0, empty = 0;
    for (int i = 0; i < room; i++) {
        uint32_t t = rd32(g_mem, slots + (uint32_t)i * SLOT_SIZE);
        teams[i] = 0;
        if (i < count && readable(t) && !team_in(teams, i, t)) {
            teams[i] = t;
        } else {
            open[n_open++] = i;
            empty += i < count;
            if (i >= count && readable(t)) spare[n_spare++] = t;   /* a default nation still in the slot */
        }
    }
    if (!n_open) return;
    int filled = 0, s = 0;
    for (int k = 0; k < n_open; k++) {
        uint32_t t = 0;
        while (s < n_spare && !t) {
            uint32_t c = spare[s++];
            if (!team_in(teams, room, c)) t = c;
        }
        if (!t) {                                        /* the nation with the highest reputation */
            int best_rep = -1;
            for (int id = 0; id < nation_count(); id++) {
                int rep = (int16_t)rd16(g_mem, nation_at(id) + NATION_REPUTATION);
                if (rep <= best_rep || (continent && continent_id(id) != rd32(g_mem, continent))) continue;
                uint32_t args[1] = {nation_at(id)};
                uint32_t c = call_cdecl(NATIONAL_TEAM, 1, args);
                if (rt_unwinding) return;
                if (readable(c) && !team_in(teams, room, c)) {
                    best_rep = rep;
                    t = c;
                }
            }
        }
        if (!t) break;
        teams[open[k]] = t;
        wr32(g_mem, slots + (uint32_t)open[k] * SLOT_SIZE, t);
        wr8(g_mem, slots + (uint32_t)open[k] * SLOT_SIZE + 4u, 0);
        filled++;
    }
    if (filled == n_open) wr16(g_mem, comp + SLOT_COUNT, (uint16_t)room);
    if (logging()) fprintf(stderr, "recomp: season intl: %s %d: %d qualified teams of %d (%d empty), %d filled\n", name,
                           (int16_t)rd16(g_mem, comp + 0x40u), count, room, empty, filled);
}

void o_0092f230(void);

/* WorldCup::seed_teams, thiscall: the draw of the finals. The 2002 World Cup has its own draw (0x92f240). */
RT_WRAP(0092f230, wc_seed_teams)
{
    uint32_t self = cpu.ecx;
    if (season_intl_on() && (int16_t)rd16(g_mem, self + 0x40u) != 2002) fill_slots(self, 0, "world_cup");
    if (rt_unwinding) return;
    cpu.ecx = self;
    o_0092f230();
}

void o_0057c1d0(void);

/* EuroChamp::run_group_draw, thiscall: the draw of the finals. */
RT_WRAP(0057c1d0, euro_group_draw)
{
    uint32_t self = cpu.ecx;
    if (season_intl_on()) fill_slots(self, CONTINENT_EUROPE, "euro_champ");
    if (rt_unwinding) return;
    cpu.ecx = self;
    o_0057c1d0();
}

void o_0092e740(void);
void o_0057c120(void);

/* A qualifier that ends after the fill finds no free slot: WorldCup::add_qualified_team 0x92e740 and
 * EuroChamp::add_qualified_team 0x57c120 (thiscall (team), ret 4) stop at a box when the slots are full
 * (0x92e74a, 0x57c12a). In a season game the team does not come in. */
static int slots_full(uint32_t comp)
{
    return season_intl_on() && (int16_t)rd16(g_mem, comp + SLOT_COUNT) >= (int16_t)rd16(g_mem, comp + SLOT_ROOM);
}

RT_WRAP(0092e740, wc_add_qualified)
{
    if (slots_full(cpu.ecx)) {
        if (logging()) fprintf(stderr, "recomp: season intl: world_cup is full, a qualifier stays out\n");
        rt_ret_stdcall(0, 1);
        return;
    }
    o_0092e740();
}

RT_WRAP(0057c120, euro_add_qualified)
{
    if (slots_full(cpu.ecx)) {
        if (logging()) fprintf(stderr, "recomp: season intl: euro_champ is full, a qualifier stays out\n");
        rt_ret_stdcall(0, 1);
        return;
    }
    o_0057c120();
}

void o_0076d880(void);

/* NationalTeams::release_overage_u21_players, thiscall (nation), ret 4: called for both teams of an
 * international fixture (0x76bc37, 0x76bc45, with the team's nation +0x53; the fixture in esi). It reads the
 * nation (0x76d88d) and asks for its under-21 team (database_get_national_u21_team 0x53b4f0), which stops at
 * the box "Database.cpp 13160" for no nation (0x53b4fc). A 2003 start met a team with no nation on 23 May 2004.
 * In a season game such a call does nothing, and the log names the fixture's teams. */
RT_WRAP(0076d880, release_overage_u21)
{
    if (!season_intl_on() || rt_arg(0)) {
        o_0076d880();
        return;
    }
    if (logging()) {
        uint32_t fx = cpu.esi, home = rd32(g_mem, fx + 0x1cu), away = rd32(g_mem, fx + 0x20u);
        fprintf(stderr, "recomp: season intl: a fixture team has no nation: %s v %s\n",
                readable(home) ? win_str(home + 4u) : "?", readable(away) ? win_str(away + 4u) : "?");
    }
    rt_ret_stdcall(0, 1);
}

/* ---- The rule kind ---- */

/* A new game (season.c, before the original makes it): a season game from 2006/07 starts with Australia in
 * Asia, before the competitions take their entrants. */
void season_intl_at_create(void)
{
    if (season_intl_on() && season_start_year() >= AUSTRALIA_TO_ASIA) australia_to_asia();
}

/* Each league table entry on its update day, with changing rules on: Australia moves on the first one from
 * 1 January 2006. */
static void intl_entry(const SeasonChange *c)
{
    (void)c;
    if ((int16_t)rd16(g_mem, GAME_DATE + 2) >= AUSTRALIA_TO_ASIA && australia_to_asia())
        season_change_news("Australia now plays in the Asian Football Confederation.");
}

const SeasonRuleKind season_kind_intl = {"international", NULL, NULL, NULL, intl_entry, NULL, NULL};
