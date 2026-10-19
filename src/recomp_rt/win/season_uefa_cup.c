/* The UEFA Cup with a group stage (0.6.0 slice 22, change point U2: 2004/05 to 2008/09), season_uefa.h.
 * Evidence: docs/research/2026-10-17-uefa-formats.md. Only when season_uefa_row gives a row with UEFA Cup groups:
 * never in the base game or with RECOMP_SEASON_CODE=off.
 *
 * The original UEFA Cup (Uefa_Cup, vtable 0x970790) is a Cup of one stage: the constructor 0x9022e0 sets the
 * stage count +0x2c to 0 and the current stage +0x30 to -1, and its round table (create_round_table 0x9025c0)
 * holds a qualifying round, rounds 1 to 4, the quarter-finals, the semi-finals and the final. Its entry table
 * +0xa7 (6 bytes an entry, club at +0) holds +0x3e = 145 slots: the seeded clubs, sorted with the qualifying
 * round first (build_entrants 0x902b90), then 3 slots for the Intertoto Cup winners (count - 0x1b, 0x633deb), 16
 * for the Champions League third qualifying round losers (count - 0x18, 0x585a02) and 8 for the Champions League
 * group thirds (count - 8 to count - 1, 0x586282). A round takes +0x1c new clubs from slot +0x1e.
 *
 * The format with groups keeps the entry table. Its round table has three rounds: Q1, Q2 and R1, which take the
 * seeded clubs, the Intertoto winners and the Champions League losers in slot order; the slots of the group
 * thirds stay for the knockout stage. The sizes: R1 and Q2 from the row, Q1 so that the three rounds take every
 * slot before the thirds (Q1/2 + Q2/2 + R1 = count - 8 - b2/2, b2 = +0xb2, the clubs of the original's extra
 * round, 0x902c85; with b2 > 0 the original's format stays). Then, as the Champions League does (its stages,
 * season_uefa.c), the object holds 9 stages: 8 groups of 5 clubs that meet once (League_Stage 0x68b100) in
 * stages 0 to 7, and a knockout stage of 32 clubs (Cup_Stage 0x522e00) in stage 8: the top three of each group
 * and the 8 Champions League group thirds. The Cup base plays the stages: Cup::update_for_date 0x51f890 calls
 * slot 10 when the rounds of the Cup end and when a stage ends while +0x30 < +0x2c - 1. Slot 10 of the UEFA
 * Cup is the empty function 0x5a8f60 (a "ret" that many vtables share), so its wrap acts only for a UEFA Cup
 * object with 9 stages. The save keeps +0x2c, +0x30 and the stages (see the wrap of Cup::load_from_disk). */
#include "win.h"
#include "season.h"
#include "season_uefa.h"
#include "season_cups.h"

#include <stdio.h>
#include <string.h>

#define COMP_TABLE 0xadadfcu
#define KEY_UC 0x9cf6f0u
#define UC_VTABLE 0x00970790u
#define RULING_BODIES 0xae28f0u

#define F_LEAGUE_ROW 0x006826d0u        /* League::get_table_row, thiscall (index) */
#define F_RANDOM 0x0090cfc0u            /* utils_random, cdecl (range) */
#define F_CALLOC 0x00945501u            /* cdecl (count, size) */
#define F_MALLOC 0x00944e46u            /* cdecl (size) */
#define F_FREE 0x009452cau              /* cdecl (block) */
#define F_NEW 0x00944cf1u               /* operator new, cdecl (size) */
#define F_QSORT 0x00945333u             /* qsort, cdecl (base, count, size, compare), as 0x585435 */
#define F_SEED_ORDER 0x00586f60u        /* european_cup_compare_clubs_by_seed: European seed, then reputation */
#define F_BOOKINGS 0x007e9180u          /* RulingBody::reset_bookings_for_club, thiscall (club) */
#define F_LEAGUE_STAGE 0x0068b100u      /* League_Stage::League_Stage, thiscall, 22 arguments */
#define F_GROUP_NEWS 0x00684230u        /* League::send_group_draw_news, thiscall () */
#define F_CUP_STAGE 0x00522e00u         /* Cup_Stage::Cup_Stage, thiscall, 15 arguments */
#define F_SET_UP_ROUND 0x0051c800u      /* Cup::set_up_round, thiscall (round) */
#define F_DRAW_DATE 0x00521e60u         /* cup_set_round_draw_date, cdecl (table, round, day, month, year+, weekday, year) */
#define F_MATCH_DATE 0x00521eb0u        /* cup_set_round_match_date, cdecl (..., weekday, time, year, venue) */
#define F_GROUP_DATE 0x0068a160u        /* league_round_set_date, cdecl (table, round, day, month, year+, weekday, time, year, 0) */
#define F_GROUP_SLOT 0x0068a1c0u        /* league_round_set_match_slot, cdecl (table, round, slot, weekday, kind, option, value) */

#define ROUND_SIZE 0x68u                /* a round of a Cup round table */
#define GROUP_ROUND_SIZE 0x41u          /* a match day of a League round table (0x68a160) */
#define STAGES 9
#define KO_SLOT 8
#define GROUPS 8
#define GROUP_CLUBS 5
#define GROUP_DAYS 5                    /* 5 clubs meet once: 5 match days, one club rests on each */
#define KO_CLUBS 32
#define KO_ROUNDS 5
#define WEEKDAY_UC 3                    /* the weekday of the UEFA Cup rounds of the original (0x9026a4) */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }
static int uc_inner;                    /* 1: the wrap of the round table passes to the original */

static uint32_t call_this(uint32_t fn, uint32_t self, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static uint32_t call_cdecl(uint32_t fn, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

static int is_uc(uint32_t obj) { return readable(obj) && rd32(g_mem, obj) == UC_VTABLE; }
static int groups_layout(uint32_t uc) { return is_uc(uc) && (int32_t)rd32(g_mem, uc + 0x2c) == STAGES; }
static uint32_t stage_at(uint32_t uc, int i) { return rd32(g_mem, rd32(g_mem, uc + 0xc) + 4u * (uint32_t)i); }

static int random_below(int n)
{
    uint32_t a[1] = {(uint32_t)n};
    int k = (int)call_cdecl(F_RANDOM, 1, a);
    return k < 0 || k >= n ? 0 : k;
}

static uint32_t row_club(uint32_t stage, int pos)
{
    uint32_t a[1] = {(uint32_t)pos};
    uint32_t row = call_this(F_LEAGUE_ROW, stage, 1, a);
    return rt_unwinding || !readable(row) ? 0 : rd32(g_mem, row);
}

static uint32_t game_malloc(uint32_t size)
{
    uint32_t a[1] = {size};
    uint32_t p = call_cdecl(F_MALLOC, 1, a);
    if (!rt_unwinding && readable(p)) memset(g_mem + p, 0, size);
    return rt_unwinding ? 0 : p;
}

static void game_free(uint32_t p)
{
    uint32_t a[1] = {p};
    if (readable(p)) call_cdecl(F_FREE, 1, a);
}

/* The stage array (13 slots, as the Champions League's, 0x583800) */
static int ensure_stage_array(uint32_t uc)
{
    if (readable(rd32(g_mem, uc + 0xc))) return 1;
    uint32_t arr = game_malloc(13 * 4);
    if (!arr) return 0;
    wr32(g_mem, uc + 0xc, arr);
    return 1;
}

/* Deletes the stages of the season that ends (as EuropeanCup::start_new_season 0x584e04..0x584e28). */
static void free_stages(uint32_t uc)
{
    uint32_t arr = rd32(g_mem, uc + 0xc);
    int32_t now = (int32_t)rd32(g_mem, uc + 0x30);
    for (int i = 0; readable(arr) && i <= now && i < 13 && !rt_unwinding; i++) {
        uint32_t st = rd32(g_mem, arr + 4u * (uint32_t)i);
        if (readable(st)) {
            uint32_t a[1] = {1};
            call_this(rd32(g_mem, rd32(g_mem, st)), st, 1, a);     /* the scalar deleting destructor */
        }
        if (!rt_unwinding) wr32(g_mem, arr + 4u * (uint32_t)i, 0);
    }
    if (!rt_unwinding) wr32(g_mem, uc + 0x30, 0xffffffffu);
}

static void set_date(uint32_t t, int r, int draw_day, int draw_month, int draw_year, int day, int month, int year_off,
                     int venue, uint32_t year)
{
    uint32_t d[7] = {t, (uint32_t)r, (uint32_t)draw_day, (uint32_t)draw_month, (uint32_t)draw_year, 4, year};
    call_cdecl(F_DRAW_DATE, 7, d);
    uint32_t m[9] = {t, (uint32_t)r, (uint32_t)day, (uint32_t)month, (uint32_t)year_off, WEEKDAY_UC, 2, year,
                     (uint32_t)venue};
    if (!rt_unwinding) call_cdecl(F_MATCH_DATE, 9, m);
}

/* The record of the original's table with round code code, or 0. */
static uint32_t record_of(uint32_t t, int rounds, int code)
{
    for (int r = 0; r < rounds; r++)
        if ((int16_t)rd16(g_mem, t + (uint32_t)r * ROUND_SIZE + 7) == code) return t + (uint32_t)r * ROUND_SIZE;
    return 0;
}

static void set_round(uint32_t rec, int index, int code, int clubs, int fresh, int first, int flags, int gap)
{
    wr16(g_mem, rec + 7, (uint16_t)code);
    wr16(g_mem, rec + 9, (uint16_t)index);
    wr16(g_mem, rec + 0xd, (uint16_t)flags);
    wr16(g_mem, rec + 0x18, (uint16_t)clubs);
    wr16(g_mem, rec + 0x1a, (uint16_t)(clubs / 2));
    wr16(g_mem, rec + 0x1c, (uint16_t)fresh);
    wr16(g_mem, rec + 0x1e, (uint16_t)first);
    if (gap) g_mem[rec + 0x22] = (uint8_t)gap;
}

/* The original's round table of stage -1 (8 or 9 rounds). */
static uint32_t original_table(uint32_t uc, int *rounds)
{
    uint32_t saved_esp = cpu.esp;
    cpu.esp -= 16;
    uint32_t out = cpu.esp;
    for (int i = 0; i < 16; i += 4) wr32(g_mem, out + (uint32_t)i, 0);
    uint32_t a[4] = {0xffffffffu, out, out + 4, out + 8};
    uc_inner = 1;
    uint32_t t = call_this(rd32(g_mem, rd32(g_mem, uc) + 0x3c), uc, 4, a);
    uc_inner = 0;
    *rounds = (int16_t)rd16(g_mem, out);
    if (rt_unwinding) return 0;
    cpu.esp = saved_esp;
    return rt_unwinding || !readable(t) ? 0 : t;
}

/* ---- Stage -1: Q1, Q2 and R1 ---- */

void o_009025c0(void);

/* The sizes of Q1, Q2 and R1 of row r for a UEFA Cup with count slots (see the head of the file), or 0. */
static int qual_sizes(uint32_t uc, const SeasonUefa *r, int *q1, int *q2, int *r1)
{
    int count = (int16_t)rd16(g_mem, uc + 0x3e), b2 = (int8_t)g_mem[uc + 0xb2];
    *r1 = r ? season_uefa_uc_round(r, "R1") : 0, *q2 = r ? season_uefa_uc_round(r, "Q2") : 0;
    *q1 = 2 * (count - 8 - *r1) - *q2;
    return r && r->uc_groups == GROUPS && r->uc_group_clubs == GROUP_CLUBS && r->uc_meetings == 1 &&
           season_uefa_uc_round(r, "KO") == KO_CLUBS && season_uefa_cl_round_of_16(r) && b2 == 0 && *r1 == 80 &&
           *q2 > 0 && *q2 % 2 == 0 && *q1 >= 2 && *q1 % 2 == 0 && count > 100;
}

/* The round table of Q1, Q2 and R1 (game memory, as the original's), or 0. It changes nothing else. */
static uint32_t qual_table(uint32_t uc, int q1, int q2, int r1)
{
    int rounds = 0;
    uint32_t orig = original_table(uc, &rounds);
    uint32_t qr = orig ? record_of(orig, rounds, 0xdc) : 0, first = orig ? record_of(orig, rounds, 10) : 0;
    uint32_t t = qr && first ? game_malloc(3 * ROUND_SIZE) : 0;
    if (t) {
        memcpy(g_mem + t, g_mem + qr, ROUND_SIZE);
        memcpy(g_mem + t + ROUND_SIZE, g_mem + qr, ROUND_SIZE);
        memcpy(g_mem + t + 2 * ROUND_SIZE, g_mem + first, ROUND_SIZE);
    }
    game_free(orig);
    if (!t || rt_unwinding) return 0;
    /* Codes 250 and 260 are the "First" and "Second Qualifying Phase" of the round names (0x4b4850), as the
     * Champions League's (0x583d13, 0x583d88); 10 is "First Round". */
    set_round(t, 0, 250, q1, q1, 0, 0, 0);
    set_round(t + ROUND_SIZE, 1, 260, q2, q2 - q1 / 2, q1, 0, 0);
    set_round(t + 2 * ROUND_SIZE, 2, 10, r1, r1 - q2 / 2, q1 + q2 - q1 / 2, 0x400, 0);
    /* Q1 on 13 July, Q2 on the original's qualifying round dates (draw 28 July, 10 August, 0x902720..0x902740),
     * R1 on its first round dates (14 September); each second leg 14 days later (+0x22). */
    set_date(t, 0, 1, 6, 0, 13, 6, 0, 0, (uint32_t)(int16_t)rd16(g_mem, uc + 0x40));
    return rt_unwinding ? 0 : t;
}

static uint32_t group_table(uint32_t year);
static uint32_t ko_table(uint32_t uc);

/* The return addresses of the two calls that set up a season's rounds: the constructor (call at 0x9023cd) and
 * set_up_rules 0x9030b0 (call at 0x9030ca). Any other caller, such as Comp::build_match_date_list 0x48d500 (the
 * Schedule tab, call at 0x48d5c4), only reads the tables. */
#define UC_SETUP_CONSTRUCTOR 0x009023d0u
#define UC_SETUP_RULES 0x009030cdu

/* Uefa_Cup::create_round_table, thiscall (stage, &rounds, &name, &league) ret 0x10. At the set-up of a season
 * (stage -1, after build_entrants) the format of the season is chosen here. A UEFA Cup with groups gives every
 * caller the tables of its stages: Comp::build_match_date_list 0x48d500 asks for each stage from -1 to
 * +0x2c - 1 (0x48d5b0..0x48d5cb), and the original builds only stage -1 (0x9025ca), so stages 0 to 8 come from
 * here: the groups (5 match days, a league table: league 1) and the knockout stage. */
RT_WRAP(009025c0, uc_round_table)
{
    uint32_t uc = cpu.ecx;
    int stage = (int8_t)rt_arg(0);
    if (uc_inner || !is_uc(uc)) {
        o_009025c0();
        return;
    }
    uint32_t out_rounds = rt_arg(1), out_name = rt_arg(2), out_league = rt_arg(3), ret = rd32(g_mem, cpu.esp);
    uint32_t year = (uint32_t)(int16_t)rd16(g_mem, uc + 0x40);
    int q1, q2, r1;
    if (stage == -1 && (ret == UC_SETUP_CONSTRUCTOR || ret == UC_SETUP_RULES)) {
        if (readable(rd32(g_mem, uc + 0xc)) && (int32_t)rd32(g_mem, uc + 0x30) >= 0) free_stages(uc);
        if (rt_unwinding) return;
        const SeasonUefa *r = season_uefa_row((int)year);
        if (!qual_sizes(uc, r, &q1, &q2, &r1)) {
            if ((int32_t)rd32(g_mem, uc + 0x2c) != 0) wr32(g_mem, uc + 0x2c, 0);
            if (r && r->uc_groups)
                fprintf(stderr, "recomp: uefa: UEFA Cup %u groups not possible here (b2 %d, %d slots)\n", year,
                        (int8_t)g_mem[uc + 0xb2], (int16_t)rd16(g_mem, uc + 0x3e));
            o_009025c0();
            return;
        }
        uint32_t t = qual_table(uc, q1, q2, r1);
        if (rt_unwinding) return;
        if (!t) {
            o_009025c0();
            return;
        }
        wr32(g_mem, uc + 0x2c, STAGES);
        wr32(g_mem, uc + 0x30, 0xffffffffu);
        if (!ensure_stage_array(uc)) {
            wr32(g_mem, uc + 0x2c, 0);
            game_free(t);
            if (!rt_unwinding) o_009025c0();
            return;
        }
        if (readable(out_rounds)) wr16(g_mem, out_rounds, 3);
        if (readable(out_name)) wr16(g_mem, out_name, 0);       /* as the original (0x902601) */
        if (readable(out_league)) wr32(g_mem, out_league, 0);
        fprintf(stderr, "recomp: uefa: UEFA Cup %u/%02u with groups: Q1 %d, Q2 %d, R1 %d\n", year, (year + 1) % 100,
                q1, q2, r1);
        rt_ret_stdcall(t, 4);
        return;
    }
    if (!groups_layout(uc) || stage < -1 || stage > KO_SLOT) {
        o_009025c0();
        return;
    }
    /* A reader: the tables of this season's format, with no change to the object. */
    uint32_t t = 0;
    int rounds = 0, name = 0, league = 0;
    if (stage == -1) {
        if (qual_sizes(uc, season_uefa_row((int)year), &q1, &q2, &r1)) t = qual_table(uc, q1, q2, r1), rounds = 3;
    } else if (stage < GROUPS) {
        t = group_table(year), rounds = GROUP_DAYS, name = 0x43d + stage, league = 1;
    } else {
        t = ko_table(uc), rounds = KO_ROUNDS;
    }
    if (rt_unwinding) return;
    if (!t) {
        o_009025c0();
        return;
    }
    if (readable(out_rounds)) wr16(g_mem, out_rounds, (uint16_t)rounds);
    if (readable(out_name)) wr16(g_mem, out_name, (uint16_t)name);
    if (readable(out_league)) wr32(g_mem, out_league, (uint32_t)league);
    rt_ret_stdcall(t, 4);
}

/* ---- The group stage ---- */

/* The match days of the groups: 21 October, 4 and 25 November, 2 and 16 December (Thursdays of 2004). */
static const int GROUP_DATES[GROUP_DAYS][2] = {{21, 9}, {4, 10}, {25, 10}, {2, 11}, {16, 11}};

static uint32_t group_table(uint32_t year)
{
    uint32_t t = game_malloc(GROUP_DAYS * GROUP_ROUND_SIZE);
    for (int d = 0; t && d < GROUP_DAYS && !rt_unwinding; d++) {
        uint32_t a[9] = {t, (uint32_t)d, (uint32_t)GROUP_DATES[d][0], (uint32_t)GROUP_DATES[d][1], 0, WEEKDAY_UC, 2,
                         year, 0};
        call_cdecl(F_GROUP_DATE, 9, a);
        uint32_t s[7] = {t, (uint32_t)d, 0, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0};
        if (!rt_unwinding) call_cdecl(F_GROUP_SLOT, 7, s);
    }
    return rt_unwinding ? 0 : t;
}

/* Puts the 40 clubs (in pots of 8 by seed) into 8 groups of 5, one of each pot a group, two clubs of one nation
 * (club +0x53) in a group only when no other draw is found. */
static int draw_groups(const uint32_t *clubs, uint32_t groups[GROUPS][GROUP_CLUBS])
{
    for (int relax = 0; relax < 2; relax++)
        for (int attempt = 0; attempt < 50; attempt++) {
            int ok = 1;
            for (int pot = 0; pot < GROUP_CLUBS && ok; pot++) {
                int used[GROUPS] = {0};
                for (int k = 0; k < GROUPS && ok; k++) {
                    uint32_t club = clubs[pot * GROUPS + k];
                    int cand[GROUPS], n = 0;
                    for (int g = 0; g < GROUPS; g++) {
                        if (used[g]) continue;
                        int same = 0;
                        for (int p = 0; p < pot && !relax; p++)
                            same |= rd32(g_mem, groups[g][p] + 0x53) == rd32(g_mem, club + 0x53);
                        if (!same) cand[n++] = g;
                    }
                    if (!n) {
                        ok = 0;
                        break;
                    }
                    int g = cand[random_below(n)];
                    if (rt_unwinding) return 0;
                    used[g] = 1;
                    groups[g][pot] = club;
                }
            }
            if (ok) return 1;
        }
    return 0;
}

static void draw_group_stage(uint32_t uc)
{
    uint32_t clubs[GROUPS * GROUP_CLUBS];
    int n = 0, count = (int16_t)rd16(g_mem, uc + 0x3e);
    uint32_t entries = rd32(g_mem, uc + 0xa7);
    /* The clubs that won their last round: entry byte +5 = 1, as the Champions League's qualifiers
     * (draw_phase1_groups 0x585383..0x5853a6). */
    for (int i = 0; readable(entries) && i < count; i++)
        if (g_mem[entries + 6u * (uint32_t)i + 5] == 1 && readable(rd32(g_mem, entries + 6u * (uint32_t)i))) {
            if (n < GROUPS * GROUP_CLUBS) clubs[n] = rd32(g_mem, entries + 6u * (uint32_t)i);
            n++;
        }
    if (n != GROUPS * GROUP_CLUBS) {
        fprintf(stderr, "recomp: uefa: UEFA Cup groups: %d clubs, not 40; no group stage\n", n);
        return;
    }
    uint32_t list = game_malloc(4 * GROUPS * GROUP_CLUBS);
    if (!list) return;
    for (int i = 0; i < n; i++) wr32(g_mem, list + 4u * (uint32_t)i, clubs[i]);
    uint32_t q[4] = {list, (uint32_t)n, 4, F_SEED_ORDER};
    call_cdecl(F_QSORT, 4, q);
    if (rt_unwinding) return;
    for (int i = 0; i < n; i++) clubs[i] = rd32(g_mem, list + 4u * (uint32_t)i);
    game_free(list);
    uint32_t groups[GROUPS][GROUP_CLUBS];
    if (rt_unwinding || !draw_groups(clubs, groups)) return;
    uint32_t year = (uint32_t)(int16_t)rd16(g_mem, uc + 0x40);
    for (int g = 0; g < GROUPS && !rt_unwinding; g++) {
        uint32_t members = game_malloc(4 * GROUP_CLUBS);
        uint32_t table = group_table(year);
        uint32_t sz[1] = {0xee};                                      /* as 0x5860b3 */
        uint32_t obj = members && table ? call_cdecl(F_NEW, 1, sz) : 0;
        if (rt_unwinding || !readable(obj)) return;
        for (int k = 0; k < GROUP_CLUBS; k++) wr32(g_mem, members + 4u * (uint32_t)k, groups[g][k]);
        /* Two small byte tables on the game stack: the order of the table (4, 1, 2, 0, as 0x585ad9) and the
         * places that go through (+0xbe: 3, the top three). */
        uint32_t saved_esp = cpu.esp;
        cpu.esp -= 16;
        uint32_t order = cpu.esp, places = cpu.esp + 8;
        g_mem[order] = 4, g_mem[order + 1] = 1, g_mem[order + 2] = 2, g_mem[order + 3] = 0;
        g_mem[places] = 3, g_mem[places + 1] = 0, g_mem[places + 2] = 0, g_mem[places + 3] = 0;
        /* The arguments as draw_phase2_groups gives them (0x5860dd..0x58611d): parent, clubs, list, meetings,
         * record, round table, match days, 3 and 1 (+0xc2, +0xc3: points of a win and a draw, inference), 8,
         * order, places, year, stage, name (0x43d + stage: the group names of phase 1, 0x583ed8), 0xf, 2, 0, 8,
         * -1, 0, 2. */
        uint32_t a[22] = {uc, GROUP_CLUBS, members, 1, rd32(g_mem, uc + 4), table, GROUP_DAYS, 3, 1, 8, order, places,
                          year, (uint32_t)g, 0x43du + (uint32_t)g, 0xf, 2, 0, 8, 0xffffffffu, 0, 2};
        call_this(F_LEAGUE_STAGE, obj, 22, a);
        if (rt_unwinding) return;
        cpu.esp = saved_esp;
        wr32(g_mem, rd32(g_mem, uc + 0xc) + 4u * (uint32_t)g, obj);
        game_free(members);
        game_free(table);
        if (!rt_unwinding) call_this(F_GROUP_NEWS, obj, 0, NULL);
    }
    if (rt_unwinding) return;
    wr32(g_mem, uc + 0x30, GROUPS - 1);          /* the last group, as draw_phase2_groups leaves it (0x586182) */
    fprintf(stderr, "recomp: uefa: UEFA Cup %u/%02u groups drawn\n", year, (year + 1) % 100);
}

/* ---- The knockout stage of 32 ---- */

/* The rounds of 32 and 16 and the quarter-finals, semi-finals and final, from the original's records. Dates of
 * 2004/05: 17 and 24 February, 10 and 17 March, 7 and 14 April, 28 April and 5 May, final 18 May (venue 4, as
 * the original's final 0x902b07). */
static uint32_t ko_table(uint32_t uc)
{
    int rounds = 0;
    uint32_t orig = original_table(uc, &rounds);
    if (!orig) return 0;
    static const int codes[KO_ROUNDS] = {30, 40, 0x78, 0x82, 0x96};
    uint32_t t = game_malloc(KO_ROUNDS * ROUND_SIZE);
    for (int r = 0; t && r < KO_ROUNDS; r++) {
        uint32_t rec = record_of(orig, rounds, codes[r]);
        if (!rec) {
            game_free(t);
            t = 0;
            break;
        }
        memcpy(g_mem + t + (uint32_t)r * ROUND_SIZE, g_mem + rec, ROUND_SIZE);
    }
    game_free(orig);
    if (!t || rt_unwinding) return 0;
    uint32_t year = (uint32_t)(int16_t)rd16(g_mem, uc + 0x40);
    int clubs = KO_CLUBS;
    for (int r = 0; r < KO_ROUNDS; r++, clubs /= 2)
        set_round(t + (uint32_t)r * ROUND_SIZE, r, codes[r], clubs, r == 0 ? KO_CLUBS : 0, 0, r == 0 ? 0x200 : 0,
                  r < KO_ROUNDS - 1 ? 7 : 0);
    wr16(g_mem, t + (KO_ROUNDS - 1) * ROUND_SIZE + 0xd, 0x83);       /* the final's flags, as 0x902b38 */
    static const int dates[KO_ROUNDS][6] = {
        {17, 11, 0, 17, 1, 1}, {25, 1, 1, 10, 2, 1}, {18, 2, 1, 7, 3, 1}, {15, 3, 1, 28, 3, 1}, {6, 4, 1, 18, 4, 1}};
    for (int r = 0; r < KO_ROUNDS && !rt_unwinding; r++)
        set_date(t, r, dates[r][0], dates[r][1], dates[r][2], dates[r][3], dates[r][4], dates[r][5],
                 r == KO_ROUNDS - 1 ? 4 : 0, year);
    return rt_unwinding ? 0 : t;
}

/* Pairs each unseeded club (a group third or a Champions League third) with a seeded one (a group winner or
 * runner-up): not of its own group, of another nation when a draw is found. */
static int pair_ko(const uint32_t *seed, const int *seed_group, const uint32_t *unseed, const int *unseed_group,
                   int *out)
{
    for (int relax = 0; relax < 3; relax++)
        for (int attempt = 0; attempt < 50; attempt++) {
            int used[16] = {0}, ok = 1;
            for (int i = 0; i < 16 && ok; i++) {
                int cand[16], n = 0;
                for (int j = 0; j < 16; j++) {
                    if (used[j]) continue;
                    if (relax < 2 && unseed_group[i] >= 0 && unseed_group[i] == seed_group[j]) continue;
                    if (relax < 1 && rd32(g_mem, unseed[i] + 0x53) == rd32(g_mem, seed[j] + 0x53)) continue;
                    cand[n++] = j;
                }
                if (!n) {
                    ok = 0;
                    break;
                }
                out[i] = cand[random_below(n)];
                if (rt_unwinding) return 0;
                used[out[i]] = 1;
            }
            if (ok) return 1;
        }
    return 0;
}

static void draw_knockout(uint32_t uc)
{
    uint32_t seed[16], unseed[16];
    int seed_group[16], unseed_group[16];
    for (int g = 0; g < GROUPS; g++) {
        uint32_t st = stage_at(uc, g);
        if (!readable(st)) return;
        seed[g] = row_club(st, 0), seed_group[g] = g;
        seed[8 + g] = row_club(st, 1), seed_group[8 + g] = g;
        unseed[g] = row_club(st, 2), unseed_group[g] = g;
        for (int p = 3; p < GROUP_CLUBS && !rt_unwinding; p++) {
            uint32_t out = row_club(st, p);                           /* out, as the fourths of 0x5862fe */
            if (readable(out)) wr32(g_mem, out + 0x1db, 0xffffffffu);
        }
        if (rt_unwinding || !readable(seed[g]) || !readable(seed[8 + g]) || !readable(unseed[g])) return;
    }
    int count = (int16_t)rd16(g_mem, uc + 0x3e);
    uint32_t entries = rd32(g_mem, uc + 0xa7);
    for (int k = 0; k < 8; k++) {                                     /* the slots of the Champions League thirds */
        unseed[8 + k] = readable(entries) ? rd32(g_mem, entries + 6u * (uint32_t)(count - 8 + k)) : 0;
        unseed_group[8 + k] = -1;
        if (!readable(unseed[8 + k])) {
            fprintf(stderr, "recomp: uefa: UEFA Cup knockout: no Champions League third in slot %d\n", count - 8 + k);
            return;
        }
    }
    int pair[16];
    if (!pair_ko(seed, seed_group, unseed, unseed_group, pair)) return;
    uint32_t ca[2] = {KO_CLUBS, 4};
    uint32_t list = call_cdecl(F_CALLOC, 2, ca);
    if (rt_unwinding || !readable(list)) return;
    for (int i = 0; i < 16; i++) {                                    /* the seeded club plays the second leg at home */
        wr32(g_mem, list + 8u * (uint32_t)i, unseed[i]);
        wr32(g_mem, list + 8u * (uint32_t)i + 4, seed[pair[i]]);
    }
    uint32_t bodies = rd32(g_mem, RULING_BODIES);
    uint32_t body = readable(bodies) ? rd32(g_mem, rd32(g_mem, bodies) + 4u * (uint32_t)(int8_t)g_mem[uc + 0x50]) : 0;
    for (int i = 0; i < KO_CLUBS && readable(body) && !rt_unwinding; i++) {
        uint32_t a[1] = {rd32(g_mem, list + 4u * (uint32_t)i)};
        call_this(F_BOOKINGS, body, 1, a);
    }
    uint32_t table = rt_unwinding ? 0 : ko_table(uc);
    uint32_t sz[1] = {0xb2};
    uint32_t obj = table ? call_cdecl(F_NEW, 1, sz) : 0;
    if (rt_unwinding || !readable(obj)) return;
    uint32_t args[15] = {uc, KO_CLUBS, list, KO_ROUNDS, rd32(g_mem, uc + 4), table,
                         (uint32_t)(int16_t)rd16(g_mem, uc + 0x40), KO_SLOT, 2, 0, 0x14, 1, 0, 0, 0};
    call_this(F_CUP_STAGE, obj, 15, args);
    if (rt_unwinding) return;
    wr32(g_mem, rd32(g_mem, uc + 0xc) + 4u * KO_SLOT, obj);
    game_free(list);
    game_free(table);
    uint32_t r0[1] = {0};
    if (!rt_unwinding) call_this(F_SET_UP_ROUND, obj, 1, r0);
    if (rt_unwinding) return;
    wr32(g_mem, uc + 0x30, KO_SLOT);
    fprintf(stderr, "recomp: uefa: UEFA Cup round of 32 drawn\n");
}

/* ---- Wraps ---- */

void o_005a8f60(void);
void o_0051fc00(void);
void o_00519a90(void);
void o_009030e0(void);

/* The empty function 0x5a8f60: slot 10 (advance_stage) of the UEFA Cup's vtable and of many others. */
RT_WRAP(005a8f60, empty_or_uc_advance)
{
    uint32_t uc = cpu.ecx;
    if (!groups_layout(uc)) {
        if (season_cup_groups_advance(uc)) {            /* slice 21c: a domestic cup with groups */
            if (!rt_unwinding) rt_ret_cdecl(0);
            return;
        }
        o_005a8f60();
        return;
    }
    int32_t now = (int32_t)rd32(g_mem, uc + 0x30);
    if (now == -1) draw_group_stage(uc);
    else if (now == GROUPS - 1) draw_knockout(uc);
    else if (now >= 0 && now < KO_SLOT - 1) wr32(g_mem, uc + 0x30, (uint32_t)(now + 1));
    if (!rt_unwinding) rt_ret_cdecl(cpu.eax);
}

/* A season whose UEFA Cup has groups: the year of the object (+0x40, the game year that the constructor gives
 * it at 0x902317, before the load) or the year before (a load from January to June). */
static int groups_near(uint32_t uc)
{
    int year = (int16_t)rd16(g_mem, uc + 0x40);
    const SeasonUefa *a = season_uefa_row(year), *b = season_uefa_row(year - 1);
    return (a && a->uc_groups) || (b && b->uc_groups);
}

/* Cup::load_from_disk, thiscall (by_comp_id) ret 4: the stage array of a UEFA Cup before its stages load. The
 * constructor calls it at a new game too (0x90235f), so the array is made only for a season with groups: a game
 * of the original's format (a 2001 start with changing rules on) allocates nothing (Q152). */
RT_WRAP(0051fc00, cup_load)
{
    uint32_t obj = cpu.ecx;
    if (is_uc(obj) && groups_near(obj) && !readable(rd32(g_mem, obj + 0xc))) {
        uint32_t saved_ecx = cpu.ecx;
        ensure_stage_array(obj);
        if (rt_unwinding) return;
        cpu.ecx = saved_ecx;
    }
    if (!is_uc(obj)) {                                  /* slice 21c: a domestic cup with groups */
        uint32_t saved_ecx = cpu.ecx;
        season_cup_groups_load(obj);
        if (rt_unwinding) return;
        cpu.ecx = saved_ecx;
    }
    o_0051fc00();
}

/* Cup::record_season_history 0x519a90 (slot 12): a UEFA Cup with groups records its season from the knockout
 * stage, as EuropeanCup::record_season_honours 0x584ad0 does (a jump to slot +0x30 of the stage). */
RT_WRAP(00519a90, cup_honours)
{
    uint32_t uc = cpu.ecx;
    uint32_t ko = groups_layout(uc) && (int32_t)rd32(g_mem, uc + 0x30) == KO_SLOT ? stage_at(uc, KO_SLOT) : 0;
    if (!readable(ko)) ko = season_cup_groups_ko(uc);  /* slice 21c: a domestic cup with groups */
    if (!readable(ko)) {
        o_00519a90();
        return;
    }
    cpu.ecx = ko;
    rt_jump(rd32(g_mem, rd32(g_mem, ko) + 0x30));
}

/* Uefa_Cup slot 17 0x9030e0: the base 0x519890 does nothing for a cup with stages (+0x2c, 0x5198a3); here it
 * runs as for the original's UEFA Cup (the title holder of 0x519974, the staff records). A round index past the
 * round table is not read (0x5199e0..0x5199e6). */
RT_WRAP(009030e0, uc_result)
{
    uint32_t uc = cpu.ecx;
    if (!groups_layout(uc)) {
        o_009030e0();
        return;
    }
    wr32(g_mem, uc + 0x2c, 0);
    o_009030e0();
    wr32(g_mem, uc + 0x2c, STAGES);
}

/* ---- The Intertoto Cup of 3 rounds, its 11 winners in the UEFA Cup Q2 (change point U3: 2006 to 2008) ----
 *
 * The original Intertoto Cup (Intertoto_Cup, vtable 0x96bb8c) plays 5 rounds (create_round_table 0x633300:
 * codes 10, 20, 30, the semi-finals and the final; 40, 32 and 24 clubs in the first three rounds, which take
 * 40, 12 and 8 new clubs of its 60 entrants from slots 0, 40 and 52). Its update_for_date 0x633d40 copies the
 * clubs that won the last round (entry byte +5 = 1) into the UEFA Cup's entry table from slot count - 0x1b
 * (0x633deb), 3 of them (0x633df3). In this format its first three rounds are the whole cup: 40, 36 and 22
 * clubs with 40, 16 and 4 new clubs (the seeding still gives 60, 0x905003), and the 11 winners of the third
 * round go to the end of the UEFA Cup's Q2 slots. The UEFA Cup's entry table grows by 8 slots for them
 * (161 are allocated, 0x902b9d): build_entrants 0x902b90 is wrapped to move the seeded clubs after the Q2
 * slots up by 11 and to make the list +0x14 (count slots, 0x902dff) as long. */

#define UIC_VTABLE 0x0096bb8cu
#define UIC_WINNERS 11
#define UC_SLOTS_ORIGINAL 145           /* 0x902ba7 */
#define UC_SLOTS_UIC 153                /* 145 and 8 more Intertoto winners; the table holds 161 (0x902b9d) */

static int uic_to_q2(const SeasonUefa *r)
{
    return r && r->uic_rounds == 3 && r->uic_winners == UIC_WINNERS && r->uic_entry && !strcmp(r->uic_entry, "Q2") &&
           r->uc_groups == GROUPS && season_uefa_cl_round_of_16(r);
}

/* The UEFA Cup takes the 11 winners: it has 9 stages, 153 slots, and the Intertoto Cup has 3 rounds. */
static int uic_handover(uint32_t uc, uint32_t uic)
{
    return groups_layout(uc) && (int16_t)rd16(g_mem, uc + 0x3e) == UC_SLOTS_UIC && readable(uic) &&
           (int16_t)rd16(g_mem, uic + 0x3c) == 3 && readable(rd32(g_mem, uc + 0xa3));
}

/* 0x633deb: the slot of the first winner, counted back from the UEFA Cup's slot count (0x1b in the original):
 * the last 11 new slots of its Q2 (round 1 of its table: +0x1e first slot, +0x1c new clubs). */
uint32_t rt_uefa_uic_back(uint32_t uc, uint32_t uic)
{
    if (!uic_handover(uc, uic)) return 0x1bu;
    uint32_t q2 = rd32(g_mem, uc + 0xa3) + ROUND_SIZE;
    int start = (int16_t)rd16(g_mem, q2 + 0x1e) + (int16_t)rd16(g_mem, q2 + 0x1c) - UIC_WINNERS;
    return (uint32_t)((int16_t)rd16(g_mem, uc + 0x3e) - start);
}

/* 0x633df3: the number of winners (3 in the original). */
uint32_t rt_uefa_uic_winners(uint32_t uc, uint32_t uic) { return uic_handover(uc, uic) ? UIC_WINNERS : 3u; }

void o_00633300(void);
void o_00902b90(void);

/* Intertoto_Cup::create_round_table, thiscall (stage, &rounds, &name, &league) ret 0x10: stage -1 at the
 * constructor and at each new season. The table of 5 rounds stays allocated; the cup plays its first 3. */
RT_WRAP(00633300, uic_round_table)
{
    uint32_t uic = cpu.ecx, out_rounds = rt_arg(1);
    int8_t stage = (int8_t)rt_arg(0);
    o_00633300();
    if (rt_unwinding || stage != -1 || !readable(uic) || rd32(g_mem, uic) != UIC_VTABLE) return;
    uint32_t t = cpu.eax;
    if (!readable(t) || !uic_to_q2(season_uefa_row((int16_t)rd16(g_mem, uic + 0x40)))) return;
    if ((int16_t)rd16(g_mem, t + 7) != 10 || (int16_t)rd16(g_mem, t + ROUND_SIZE + 7) != 20 ||
        (int16_t)rd16(g_mem, t + 2 * ROUND_SIZE + 7) != 30)
        return;
    set_round(t, 0, 10, 40, 40, 0, rd16(g_mem, t + 0xd), 0);
    set_round(t + ROUND_SIZE, 1, 20, 36, 16, 40, rd16(g_mem, t + ROUND_SIZE + 0xd), 0);
    set_round(t + 2 * ROUND_SIZE, 2, 30, 22, 4, 56, rd16(g_mem, t + 2 * ROUND_SIZE + 0xd), 0);
    if (readable(out_rounds)) wr16(g_mem, out_rounds, 3);
    fprintf(stderr, "recomp: uefa: Intertoto Cup %d with 3 rounds\n", (int16_t)rd16(g_mem, uic + 0x40));
}

/* Uefa_Cup::build_entrants, thiscall () returning 1: the gap of the Intertoto winners in the Q2 slots. */
RT_WRAP(00902b90, uc_build_entrants)
{
    uint32_t uc = cpu.ecx;
    o_00902b90();
    if (rt_unwinding || cpu.eax != 1 || !is_uc(uc)) return;
    const SeasonUefa *r = season_uefa_row((int16_t)rd16(g_mem, uc + 0x40));
    if (!uic_to_q2(r) || g_mem[uc + 0xb2] != 0 || (int16_t)rd16(g_mem, uc + 0x3e) != UC_SLOTS_ORIGINAL) return;
    /* The sizes of uc_round_table with 153 slots: Q1 and the seeded clubs of Q2 come before the winners. */
    int r1 = season_uefa_uc_round(r, "R1"), q2 = season_uefa_uc_round(r, "Q2");
    int q1 = 2 * (UC_SLOTS_UIC - 8 - r1) - q2;
    int before = q1 + q2 - q1 / 2 - UIC_WINNERS, seeded = UC_SLOTS_ORIGINAL - 0x1b;     /* 118 (0x902cb0) */
    if (r1 != 80 || q2 <= 0 || q2 % 2 || q1 < 2 || before < q1 || before > seeded) return;
    Cpu saved = cpu;
    int16_t listed = (int16_t)rd16(g_mem, uc + 0x36);
    uint32_t list = game_malloc(6u * UC_SLOTS_UIC);
    if (rt_unwinding || !list) return;
    uint32_t old = rd32(g_mem, uc + 0x14);
    if (readable(old) && listed > 0) memcpy(g_mem + list, g_mem + old, 6u * (uint32_t)listed);
    game_free(old);
    if (rt_unwinding) return;
    wr32(g_mem, uc + 0x14, list);
    wr16(g_mem, uc + 0x38, UC_SLOTS_UIC);
    uint32_t e = rd32(g_mem, uc + 0xa7);
    memmove(g_mem + e + 6u * (uint32_t)(before + UIC_WINNERS), g_mem + e + 6u * (uint32_t)before,
            6u * (uint32_t)(seeded - before));
    memset(g_mem + e + 6u * (uint32_t)before, 0, 6u * UIC_WINNERS);
    memset(g_mem + e + 6u * (uint32_t)(seeded + UIC_WINNERS), 0, 6u * (uint32_t)(UC_SLOTS_UIC - seeded - UIC_WINNERS));
    wr16(g_mem, uc + 0x3e, UC_SLOTS_UIC);
    cpu = saved;
}
