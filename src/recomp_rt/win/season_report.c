/* The rules report and the season jump (0.6.0 slice 12, Q265). Test runs only: without the variables below
 * the human turn calls season_report_turn, which returns at once and changes nothing.
 *
 * RECOMP_RULES_REPORT=<file>: at the first human turn of the game, and at the first human turn on or after
 * 1 August of each later year, the runtime appends to the file the competitions that the game holds. It only
 * reads the game's memory.
 * RECOMP_SEASON_JUMP=<year>: the game plays on with no match engine, and the program ends with code 0 after
 * the report of the season that starts in that year. The test steps put the manager on holiday
 * (tests/seasons/rules_report_check.py), so the days pass with no input.
 *
 * No match engine: the original has a switch for it, ENABLE_MATCH_ENGINE=NO in its setup file
 * (create_game_from_setup_file 0x827110 compares with "YES" 0xa812c0 and "NO" 0xa812bc and writes the byte
 * 0xa5a390 at 0x8277a6 and 0x8277cf; it is 1 in the data of the exe). With the byte 0, MATCH_DAY::play_matches
 * 0x6b4360 plays the human club's match with the others (the test at 0x6b4a29 skips the deferred path at
 * 0x6b4df5), and CM3_MATCH_ENGINE::init_match 0x6b7f20 sets its quick flag (0x6b85fb..0x6b8604), so every
 * match is played by the quick result engine CM3_MATCH_ENGINE::quick_simulate_match 0x6beeb0. The results go
 * through match_day_apply_match_result 0x6b5f00 into each competition as in a normal day, so the tables,
 * prize money, history, records and the end-of-season step run (docs/research/2026-10-16-sweep-season-end.md,
 * section 2.4). The jump writes the byte at the first human turn, after the game is made.
 *
 * What the report reads, for each competition object:
 * - The objects: the table g_comp_object_table [0xadadfc] with [0xadadf8] slots, one per club competition
 *   record (built by 0x8386f0 from the count 0xae2378), and the competition arrays of the 35 league table
 *   entries at 0xb63d60 (0x48 bytes each; +0xc count, +0x10 array; game_season_update_pass 0x5d5cab..0x5d5cbb).
 *   An object in both is written once.
 * - +0 the vtable: the class and its source file (season_comp_classes, tools/seasons/make_comp_classes.py).
 * - +4 the competition record (club_comp.dat or nation_comp.dat, 0x6b bytes): +0 its id, +4 its long name
 *   (docs/research/2026-10-13-database-inventory.md; the record is read at 0x689b66 and 0x48f2df).
 * - +0x40 the season year (s16; League start_new_season adds 1, for example 0x574c10).
 * - +0x3e the club count (s16): a league compares each table row with it (League::check_complete 0x689b3d);
 *   a cup sets it in its constructor (EngCcCup 0x56c32d: 92).
 * - +0x3c (s16): for a league, the times that each pair of clubs meets ((clubs - 1) * +0x3c = matches per
 *   club, 0x689b27..0x689b36); for a cup, the round count (Cup::map_fixture_tree 0x51879c).
 * - +0xa7 (s16, leagues only): matches per club (0x689b1d).
 * - +0x4a substitutes used and +0x49 substitutes named (bytes; Comp::Comp 0x48c5c9 sets 5 and 3;
 *   Comp::set_fixture_substitutes 0x48f2fa..0x48f32d packs them into the fixture). */
#include "win.h"
#include "season.h"
#include "season_rules.h"
#include "../host.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MATCH_ENGINE_ON 0xa5a390u   /* u8, 1 in the exe data */
#define COMP_TABLE 0xadadfcu        /* Comp ** */
#define COMP_SLOTS 0xadadf8u        /* s32 */
#define ENTRIES 0xb63d60u           /* league table entries */
#define ENTRY_SIZE 0x48u
#define ENTRY_COUNT 35u
#define MAX_COMPS 4096

static int jump_to = -2;            /* -2: not read yet, -1: off */
static FILE *report;
static int reported = -1;           /* the last start year in the report */

static const SeasonCompClass *class_of(uint32_t vtable)
{
    int lo = 0, hi = season_comp_class_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (season_comp_classes[mid].vtable == vtable) return &season_comp_classes[mid];
        if (season_comp_classes[mid].vtable < vtable) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

/* A pointer of the game that can be read: inside the 32-bit space and not near 0. */
static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static void write_comp(FILE *f, int season, const char *slot, uint32_t obj)
{
    uint32_t vt = rd32(g_mem, obj), rec = rd32(g_mem, obj + 4);
    const SeasonCompClass *c = class_of(vt);
    char name[64] = "?", vts[16];
    int id = -1;
    if (readable(rec)) {
        id = (int)rd32(g_mem, rec);
        snprintf(name, sizeof name, "%.51s", win_str(rec + 4));
        for (char *p = name; *p; p++)
            if (*p == '\t' || *p == '\n' || *p == '\r') *p = ' ';
    }
    snprintf(vts, sizeof vts, "0x%x", vt);
    int league = c && c->kind == SEASON_KIND_LEAGUE;
    char matches[16] = "-";
    if (league) snprintf(matches, sizeof matches, "%d", (int16_t)rd16(g_mem, obj + 0xa7));
    fprintf(f, "comp\t%d\t%s\t%d\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s\n", season, slot, id,
            c ? c->file : "?", c ? c->cls : "?", c ? (league ? "league" : c->kind == SEASON_KIND_CUP ? "cup" : "comp") : "?",
            vts, (int16_t)rd16(g_mem, obj + 0x40), (int16_t)rd16(g_mem, obj + 0x3e), (int16_t)rd16(g_mem, obj + 0x3c),
            matches, g_mem[obj + 0x4a], g_mem[obj + 0x49], name);
}

static void write_season(int season)
{
    int y = 0, m = 0, d = 0;
    win_game_date(&y, &m, &d);
    fprintf(report, "season\t%d\t%04d-%02d-%02d\t%d\t%s\n", season, y, m, d, season_start_year(),
            season_changing_rules() ? "on" : "off");
    static uint32_t seen[MAX_COMPS];
    int nseen = 0;
    uint32_t table = rd32(g_mem, COMP_TABLE);
    int32_t slots = (int32_t)rd32(g_mem, COMP_SLOTS);
    char slot[32];
    for (int32_t i = 0; readable(table) && i < slots && i < MAX_COMPS; i++) {
        uint32_t obj = rd32(g_mem, table + 4u * (uint32_t)i);
        if (!readable(obj)) continue;
        if (nseen < MAX_COMPS) seen[nseen++] = obj;
        snprintf(slot, sizeof slot, "t%d", i);
        write_comp(report, season, slot, obj);
    }
    for (uint32_t e = 0; e < ENTRY_COUNT; e++) {
        uint32_t entry = ENTRIES + e * ENTRY_SIZE;
        int32_t n = (int32_t)rd32(g_mem, entry + 0xc);
        uint32_t arr = rd32(g_mem, entry + 0x10);
        for (int32_t k = 0; readable(arr) && k < n && k < 256; k++) {
            uint32_t obj = rd32(g_mem, arr + 4u * (uint32_t)k);
            if (!readable(obj)) continue;
            int dup = 0;
            for (int s = 0; s < nseen && !dup; s++) dup = seen[s] == obj;
            if (dup) continue;
            if (nseen < MAX_COMPS) seen[nseen++] = obj;
            snprintf(slot, sizeof slot, "e%u.%d", e, k);
            write_comp(report, season, slot, obj);
        }
    }
    fflush(report);
    fprintf(stderr, "recomp: rules report: season %d/%02d on %02d.%02d.%04d\n", season, (season + 1) % 100, d, m, y);
}

void season_report_turn(void)
{
    if (jump_to == -2) {
        const char *v = getenv("RECOMP_SEASON_JUMP");
        jump_to = v && *v ? atoi(v) : -1;
        const char *path = getenv("RECOMP_RULES_REPORT");
        if (path && *path) {
            report = host_fopen(path, "w");
            if (!report) fprintf(stderr, "recomp: rules report: cannot write %s\n", path);
            else fprintf(report, "#bosman-rules-report\t1\n"
                         "columns\tseason\tslot\trecord\tfile\tclass\tkind\tvtable\tyear\tclubs\trounds\tmatches"
                         "\tsubs_used\tbench\tname\n");
        }
        if (jump_to >= 0) {
            g_mem[MATCH_ENGINE_ON] = 0;
            fprintf(stderr, "recomp: season jump to %d/%02d with no match engine\n", jump_to, (jump_to + 1) % 100);
        }
    }
    if (jump_to < 0 && !report) return;
    int y = 0, m = 0, d = 0;
    if (!win_game_date(&y, &m, &d)) return;
    int season = reported < 0 ? season_start_year() : (m >= 8 ? y : -1);
    if (season > reported) {
        if (report) write_season(season);
        else fprintf(stderr, "recomp: season jump: season %d/%02d\n", season, (season + 1) % 100);
        reported = season;
    }
    if (jump_to >= 0 && reported >= jump_to) {
        if (report) fclose(report);
        report = NULL;
        fprintf(stderr, "recomp: season jump: done\n");
        if (rt_on_fatal) rt_on_fatal();
        SDL_Quit();
        exit(0);
    }
}
