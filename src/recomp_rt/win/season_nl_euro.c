/* European Championship qualifying with Nations League paths (0.6.0 slice 28, season_nl.h). Research and
 * evidence: docs/research/2026-10-18-nations-league.md, section "Euro qualifying".
 *
 * Euro 2020 and Euro 2024 qualifying played their groups from March to November of the year after the
 * campaign year (int-continental rows 189 and 233), because the Nations League took the autumn before. The
 * class euro_champ_qual plays its groups from September of the campaign year to October of the year after
 * (template 0x57d190, class doc section 4). In a season game the wrap of the template gives those two
 * campaigns the real matchdays (season_nl_qualifying). Slice 27 gives them their ten groups and the top two
 * of each group.
 *
 * The play-off paths. The class's play-off (create_playoff_stage 0x57ef50) pairs group runners-up in two-leg
 * ties in November. For the two campaigns the wrap builds instead the Nations League paths: four (Euro 2020) or
 * three (Euro 2024) paths of four nations that did not qualify from the groups, picked by the ranking of the
 * Nations League edition two years before the finals (season_nl_paths), as one Cup_Stage of two rounds:
 * semi-finals 1 v 4 and 2 v 3, then the path final, single matches in March of the Euro year. Link mode 1 pairs
 * the list two by two (0x524a20) and bit 0x200 of the final's rule flags sends ties 2k and 2k + 1 to final tie
 * k (0x5245c3, class doc 12.1). Only a final winner gets status 1 (0x51af95), so only it goes into the Euro
 * through the class's slot 17 (add_qualified_team, 0x57d6a0). The Euro's seeding and draw then move to after
 * the path finals (+0x13e and +0xb6 of the Euro object, saved at 0x57c5e9 and 0x689411, class doc 12.2). */
#include "win.h"
#include "season.h"
#include "season_comps.h"
#include "season_nl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUAL_VTABLE 0x0096a0d4u         /* the original Euro qualifying object (the Nations League has a copy) */
#define GAME_MALLOC 0x00944e46u         /* malloc, cdecl (size) */
#define F_YEAR 0x40u                    /* the campaign year: the year of the finals - 2 (0x57f6ac) */
#define F_LAST_GROUP 0xffu              /* byte: the index of the last group stage (0x57d1a2) */
#define LEAGUE_ROUND 0x41u              /* a league round record (0x57d1c8: 10 rounds of 0x41 bytes) */
#define ROUNDS 10
#define ROUND_SIZE 0x68u                /* a cup round record */
#define F_RECORD 0x4u
#define F_STAGES 0xcu
#define F_STAGE 0x30u
#define F_PLAYOFF 0x101u                /* byte: the play-off stage index (0x57ceb0) */
#define F_TABLE 0xb1u                   /* League table, rows of 0x3b bytes */
#define F_TEAMS 0x3eu
#define ROW_SIZE 0x3bu
#define ROW_POINTS 0x19u
#define GAME_NEW 0x00944cf1u            /* operator new, cdecl (size) */
#define GAME_FREE 0x009452cau
#define TABLE_SORT 0x006827d0u          /* League table sort, thiscall (0) */
#define CUP_STAGE_NEW 0x00522e00u       /* Cup_Stage ctor, thiscall, 15 arguments (0x57f46b..0x57f506) */
#define CUP_START 0x0051c800u           /* Cup::set_up_round, thiscall (0) (0x57f5cf) */
#define CUP_DRAW_DATE 0x00521e60u       /* cup_set_round_draw_date, cdecl (rec, round, day, month0, yoff, weekday,
                                           year) (0x57d4a3) */
#define CUP_MATCH_DATE 0x00521eb0u      /* cup_set_round_match_date, cdecl (rec, round, day, month0, yoff,
                                           weekday, time, year, venue) (0x57d4ba) */
#define COMP_TABLE 0xadadfcu            /* Comp **, by record id */
#define KEY_EURO 0x9cf7a4u              /* the European Championship (sweep-international 3) */
#define EURO_SLOTS 0x14u                /* team slots, 6 bytes each (season_intl.c) */
#define EURO_SLOT_COUNT 0x36u
#define EURO_SEED_DAY 0x13eu            /* words: day of the year and year offset (0x57b6ca, 0x57b6dd) */
#define EURO_DRAW_DAY 0xb6u             /* (0x57b6f0, 0x57b709) */
#define MAX_GROUPS 16

static int logging(void)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_SEASON_NL_LOG");
        on = v && *v == '1';
    }
    return on;
}

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

/* The matchdays of the campaign that the object holds, or NULL: a season game, the original class (not the
 * Nations League copy), a campaign of the table. */
static const SeasonNlQualifying *moved(uint32_t self)
{
    if (!season_comps_on() || !readable(self) || rd32(g_mem, self) != QUAL_VTABLE) return NULL;
    int campaign = (int16_t)rd16(g_mem, self + F_YEAR);
    for (int i = 0; i < season_nl_qualifying_count; i++)
        if (!season_nl_qualifying[i].world_cup && season_nl_qualifying[i].euro == campaign + 2)
            return &season_nl_qualifying[i];
    return NULL;
}

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t call_this(uint32_t fn, uint32_t self, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t game_malloc(uint32_t size)
{
    uint32_t a[1] = {size};
    uint32_t p = call_cdecl(GAME_MALLOC, 1, a);
    if (readable(p)) memset(g_mem + p, 0, size);
    return readable(p) ? p : 0;
}

static void game_free(uint32_t p)
{
    uint32_t a[1] = {p};
    if (p) call_cdecl(GAME_FREE, 1, a);
}

/* One round of the path play-off: its draw on the first day of the month of the matches, its match on the day
 * (weekday -1: no move, class doc 12.3), one leg with extra time and penalties (+0x0d 3, +0x21 1; the final
 * with bit 0x200 for the fixed links), the fields of the Euro's semi-final and final (0x57b0a2..0x57b194). */
void season_nl_path_round(uint32_t rec, int round, SeasonNlDate d, int campaign, int teams)
{
    /* The record keeps a year offset from the stage's year (the campaign year, Cup_Stage argument 7), as the
     * class's play-off (+0x40 and offset 1 at 0x57d494..0x57d4b6). */
    uint32_t a[7] = {rec, (uint32_t)round, 1, (uint32_t)(d.month - 1), (uint32_t)d.year, 0xffffffffu, (uint32_t)campaign};
    call_cdecl(CUP_DRAW_DATE, 7, a);
    if (rt_unwinding) return;
    uint32_t b[9] = {rec, (uint32_t)round, (uint32_t)d.day, (uint32_t)(d.month - 1), (uint32_t)d.year, 0xffffffffu, 1,
                     (uint32_t)campaign, 0};
    call_cdecl(CUP_MATCH_DATE, 9, b);
    if (rt_unwinding) return;
    uint32_t r = rec + (uint32_t)round * ROUND_SIZE;
    wr16(g_mem, r + 7u, round ? 0x96u : 0x82u);
    wr16(g_mem, r + 9u, (uint32_t)round);
    wr16(g_mem, r + 0xdu, round ? 0x203u : 3u);
    wr16(g_mem, r + 0xfu, 0);
    wr8(g_mem, r + 0x17u, 0xa);
    wr16(g_mem, r + 0x18u, (uint32_t)teams);
    wr16(g_mem, r + 0x1au, (uint32_t)teams / 2u);
    wr16(g_mem, r + 0x1cu, round ? 0u : (uint32_t)teams);
    wr16(g_mem, r + 0x1eu, 0);
    wr8(g_mem, r + 0x20u, 0);
    wr8(g_mem, r + 0x21u, 1);
    wr8(g_mem, r + 0x22u, 0);
}

void o_0057d190(void);

/* EuroChampQual::create_schedule, thiscall (stage as a byte, short *rounds, short *code, int *is_league), ret
 * 0x10 (class doc 4). A group stage (stage up to +0xff, 0x57d1a2) of a moved campaign: ten rounds as the
 * original (count 10 at 0x57d1de, code 0x3e9 + stage at 0x57d1d8, is_league 1 at 0x57d1bb) on the real days.
 * The play-off and every other object: the original. */
RT_WRAP(0057d190, nl_euro_qual_template)
{
    uint32_t self = cpu.ecx, sp = cpu.esp;
    const SeasonNlQualifying *q = moved(self);
    int stage = (int8_t)rd8(g_mem, sp + 4u);
    if (q && stage == (int8_t)rd8(g_mem, self + F_PLAYOFF)) {
        /* The path play-off: two cup rounds (0x68 bytes each, class doc 12.1). */
        int teams = 4 * q->paths;
        uint32_t out_rounds = rd32(g_mem, sp + 8u), out_code = rd32(g_mem, sp + 0xcu), out_league = rd32(g_mem, sp + 0x10u);
        if (readable(out_league)) wr32(g_mem, out_league, 0);
        if (readable(out_rounds)) wr16(g_mem, out_rounds, 2);
        if (readable(out_code)) wr16(g_mem, out_code, 0);
        uint32_t rec = game_malloc(2u * ROUND_SIZE);
        if (rt_unwinding) return;
        int campaign = (int16_t)rd16(g_mem, self + F_YEAR);
        if (rec) season_nl_path_round(rec, 0, q->po_sf, campaign, teams);
        if (rec && !rt_unwinding) season_nl_path_round(rec, 1, q->po_final, campaign, teams / 2);
        if (rt_unwinding) return;
        rt_ret_stdcall(rec, 4);
        return;
    }
    if (!q || stage > (int8_t)rd8(g_mem, self + F_LAST_GROUP)) {
        o_0057d190();
        return;
    }
    uint32_t out_rounds = rd32(g_mem, sp + 8u), out_code = rd32(g_mem, sp + 0xcu), out_league = rd32(g_mem, sp + 0x10u);
    if (readable(out_league)) wr32(g_mem, out_league, 1);
    if (readable(out_rounds)) wr16(g_mem, out_rounds, ROUNDS);
    if (readable(out_code)) wr16(g_mem, out_code, (uint32_t)(0x3e9 + stage));
    uint32_t a[1] = {ROUNDS * LEAGUE_ROUND};
    Cpu saved = cpu;
    uint32_t rec = rt_callback(GAME_MALLOC, 1, a);
    if (rt_unwinding) return;
    cpu = saved;
    if (readable(rec)) {
        memset(g_mem + rec, 0, ROUNDS * LEAGUE_ROUND);
        int year = (int16_t)rd16(g_mem, self + F_YEAR);
        for (int i = 0; i < ROUNDS && !rt_unwinding; i++) season_nl_league_round(rec, i, q->md[i], year);
        if (rt_unwinding) return;
        if (logging() && stage == -1)
            fprintf(stderr, "recomp: season nl: euro %d qualifying groups from %d March %d\n", q->euro, q->md[0].day,
                    year + q->md[0].year);
    } else {
        rec = 0;
    }
    rt_ret_stdcall(rec, 4);
}

/* ---- The path play-off (create_playoff_stage 0x57ef50) ---- */

static uint32_t euro_object(void)
{
    int32_t id = (int32_t)rd32(g_mem, KEY_EURO);
    uint32_t table = rd32(g_mem, COMP_TABLE);
    return id >= 0 && readable(table) ? rd32(g_mem, table + 4u * (uint32_t)id) : 0;
}

/* The nations that have a place in the finals: the top two of each qualifying group (slice 27: Euro 2020 and
 * 2024 take the top two) and the teams already in the Euro's slots (the host of 2024). */
static int qualified(uint32_t self, uint32_t euro, char *mark, int nations)
{
    int groups = (int8_t)rd8(g_mem, self + F_LAST_GROUP) + 2, count = 0;
    for (int g = 0; g < groups && g < MAX_GROUPS; g++) {
        uint32_t st = g == 0 ? self : rd32(g_mem, rd32(g_mem, self + F_STAGES) + 4u * (uint32_t)(g - 1));
        if (!readable(st)) continue;
        uint32_t z[1] = {0};
        call_this(TABLE_SORT, st, 1, z);
        if (rt_unwinding) return count;
        uint32_t table = rd32(g_mem, st + F_TABLE);
        int teams = (int16_t)rd16(g_mem, st + F_TEAMS);
        for (int r = 0; readable(table) && r < 2 && r < teams; r++) {
            int n = season_nl_nation_of_club(rd32(g_mem, table + (uint32_t)r * ROW_SIZE));
            if (n >= 0 && n < nations && !mark[n]) mark[n] = 1, count++;
        }
    }
    if (readable(euro)) {
        uint32_t slots = rd32(g_mem, euro + EURO_SLOTS);
        int k = (int16_t)rd16(g_mem, euro + EURO_SLOT_COUNT);
        for (int i = 0; readable(slots) && i < k && i < 64; i++) {
            int n = season_nl_nation_of_club(rd32(g_mem, slots + 6u * (uint32_t)i));
            if (n >= 0 && n < nations && !mark[n]) mark[n] = 1, count++;
        }
    }
    return count;
}

/* Without the Nations League tables (a game that started after its edition): the nations of the qualifying
 * groups that did not qualify, ranked by group place and points, all as league A, so the paths take the best
 * of them in order. */
static int group_rows(uint32_t self, const char *mark, int nations, SeasonNlRow *rows)
{
    int groups = (int8_t)rd8(g_mem, self + F_LAST_GROUP) + 2, n = 0;
    for (int g = 0; g < groups && g < MAX_GROUPS; g++) {
        uint32_t st = g == 0 ? self : rd32(g_mem, rd32(g_mem, self + F_STAGES) + 4u * (uint32_t)(g - 1));
        uint32_t table = readable(st) ? rd32(g_mem, st + F_TABLE) : 0;
        int teams = readable(st) ? (int16_t)rd16(g_mem, st + F_TEAMS) : 0;
        for (int r = 0; readable(table) && r < teams && n < NL_MAX_NATIONS; r++) {
            uint32_t row = table + (uint32_t)r * ROW_SIZE;
            int id = season_nl_nation_of_club(rd32(g_mem, row));
            if (id < 0 || id >= nations || mark[id]) continue;
            rows[n++] = (SeasonNlRow){id, 0, g + 1, r + 2, 0, (int16_t)rd16(g_mem, row + ROW_POINTS), 0, 0, 0};
        }
    }
    season_nl_rank(rows, n);
    return n;
}

/* The Euro's seeding and draw two and four days after the path finals (class doc 12.2: both tested by the
 * exact day in EuroChamp::update_for_date 0x57c620). */
static void move_euro_draw(uint32_t euro, const SeasonNlQualifying *q, int campaign)
{
    if (!readable(euro) || (int16_t)rd16(g_mem, euro + F_YEAR) != q->euro) return;
    int y = campaign + q->po_final.year;
    int last = season_nl_day_of_year(q->po_final.day, q->po_final.month, y);
    if (rt_unwinding) return;
    wr16(g_mem, euro + EURO_SEED_DAY, (uint32_t)(last + 2));
    wr16(g_mem, euro + EURO_SEED_DAY + 2u, (uint32_t)(y - q->euro));
    wr16(g_mem, euro + EURO_DRAW_DAY, (uint32_t)(last + 4));
    wr16(g_mem, euro + EURO_DRAW_DAY + 2u, (uint32_t)(y - q->euro));
    if (logging()) fprintf(stderr, "recomp: season nl: euro %d seeding on day %d and draw on day %d of %d\n", q->euro,
                           last + 2, last + 4, y);
}

static void build_paths(uint32_t self, const SeasonNlQualifying *q)
{
    int campaign = (int16_t)rd16(g_mem, self + F_YEAR), nations = (int)rd32(g_mem, 0xae2350u);
    if (nations <= 0 || nations > 4096) return;
    char *mark = calloc((size_t)nations, 1);
    if (!mark) return;
    uint32_t euro = euro_object();
    qualified(self, euro, mark, nations);
    if (rt_unwinding) { free(mark); return; }
    SeasonNlRow rows[NL_MAX_NATIONS];
    int n = season_nl_held_results(q->euro - 2, rows), from_nl = n >= 4 * q->paths;
    if (rt_unwinding) { free(mark); return; }
    if (from_nl) {
        for (int i = 0; i < n; i++) rows[i].qualified = rows[i].nation >= 0 && rows[i].nation < nations && mark[rows[i].nation];
    } else {
        n = group_rows(self, mark, nations, rows);
    }
    free(mark);
    if (rt_unwinding) return;
    int picked[16], teams = 4 * q->paths;
    season_nl_paths(rows, n, q->paths, 4, picked);
    uint32_t list = game_malloc(4u * (uint32_t)teams), out = game_malloc(8);
    if (rt_unwinding || !list || !out) return;
    int ok = 1;
    for (int p = 0; p < q->paths; p++) {
        int pairs[4];
        season_nl_path_pairs(&picked[4 * p], pairs);
        for (int k = 0; k < 4; k++) {
            uint32_t team = pairs[k] >= 0 ? season_nl_nation_team(pairs[k]) : 0;
            if (rt_unwinding) return;
            if (!team) ok = 0;
            wr32(g_mem, list + 4u * (uint32_t)(4 * p + k), team);
        }
        if (logging()) {
            fprintf(stderr, "recomp: season nl: euro %d path %c (%s):", q->euro, 'A' + p, from_nl ? "Nations League" : "groups");
            for (int k = 0; k < 4; k++)
                fprintf(stderr, " %s;", pairs[k] >= 0 ? win_str(rd32(g_mem, 0xae23a8u) + (uint32_t)pairs[k] * 0x122u + 4u) : "none");
            fputc('\n', stderr);
        }
    }
    int po = (int8_t)rd8(g_mem, self + F_PLAYOFF);
    uint32_t cup = 0, tmpl = 0;
    if (ok) {
        uint32_t xa[4] = {(uint32_t)po, out, out + 4u, 0};
        tmpl = call_this(rd32(g_mem, rd32(g_mem, self) + 0x3cu), self, 4, xa);
        if (rt_unwinding) return;
        uint32_t na[1] = {0xb2};
        cup = tmpl ? call_cdecl(GAME_NEW, 1, na) : 0;
        if (rt_unwinding) return;
        if (readable(cup)) {
            uint32_t ca[15] = {self, (uint32_t)teams, list, (uint32_t)(int16_t)rd16(g_mem, out), rd32(g_mem, self + F_RECORD),
                               tmpl, (uint32_t)campaign, (uint32_t)po, 4, (uint32_t)(int16_t)rd16(g_mem, out + 4u), 0x14, 1,
                               0, 0, 0};
            cup = call_this(CUP_STAGE_NEW, cup, 15, ca);
            if (rt_unwinding) return;
        } else {
            cup = 0;
        }
    }
    game_free(list);
    game_free(tmpl);
    game_free(out);
    if (rt_unwinding) return;
    if (!cup) {
        fprintf(stderr, "recomp: season nl: euro %d: no path play-off (a path without four nations)\n", q->euro);
        return;
    }
    wr32(g_mem, rd32(g_mem, self + F_STAGES) + 4u * (uint32_t)po, cup);
    uint32_t z[1] = {0};
    call_this(CUP_START, cup, 1, z);
    if (rt_unwinding) return;
    wr32(g_mem, self + F_STAGE, (uint32_t)po);
    move_euro_draw(euro, q, campaign);
}

void o_0057ef50(void);

/* EuroChampQual::create_playoff_stage, thiscall (), from advance_stage 0x57ecf5 when the groups are complete. */
RT_WRAP(0057ef50, nl_euro_paths)
{
    uint32_t self = cpu.ecx;
    const SeasonNlQualifying *q = moved(self);
    if (!q) {
        o_0057ef50();
        return;
    }
    build_paths(self, q);
    if (rt_unwinding) return;
    rt_ret_stdcall(0, 0);
}
