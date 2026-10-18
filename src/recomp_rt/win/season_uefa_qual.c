/* The qualifying rounds of the Champions League and the Europa League in a season of the access list (0.6.0
 * slice 23, Q369, Q296; season_uefa.h). Evidence: docs/research/2026-10-18-uefa-2009-2020.md.
 *
 * The seeding (season_uefa_seeding.c) gives each club an entry code of the access list (SEASON_UEFA_CL_*,
 * SEASON_UEFA_EL_*). This file builds both cups from those codes, in place of the original's fixed counts:
 *
 * Champions League (EuropeanCup, vtable 0x96a1d0). build_entrants 0x584fa0 wants exactly 71 clubs with codes 1
 * to 4 (0x584faa, caps 0x585095..0x585135); the entry table 0x584990 makes 55 slots (0x5849a5) of the Q1, Q2
 * and Q3 clubs; create_round_table 0x583c50 with stage -1 makes 3 rounds (0x583c9b). Here the list +0x14 holds
 * every entrant: the group clubs first, then the qualifying clubs; the entry table holds the qualifying clubs
 * by round from the lowest (PR, Q1, Q2 CP, Q2 LP, Q3 CP, Q3 LP, PO CP, PO LP), then the group clubs after the
 * first 16; the round table has one round for each round the row plays (codes 240 PR, 250 Q1, 260 Q2, 270 Q3,
 * 220 PO: the round names table gives their real names, season_round_names.h). The group draw
 * draw_phase1_groups 0x585290 takes the 16 clubs of list slots 0 to 15 and the clubs with entry byte +5 = 1
 * (0x585358..0x5853ba, 32 in all): the play-off winners get 1 from the last round (0x51af97), and the group
 * clubs after the first 16 get 1 here, after map_fixture_tree. Their slots come after every slot that a round
 * uses, so the completion test 0x51b039..0x51b06d does not see them.
 *
 * The paths. map_fixture_tree 0x518790 (vtable +0x94) makes the ties of every round at the season start: two
 * sides of entry slots (+0, +2; -1 for a winner to come), the source tie and round of each side (+0xa/+0xe
 * home, +0xc/+0x10 away) and where the winner goes (+6 tie, +8 1 home 0 away, +0x12 round; words, 0x518ef1..
 * 0x518fa7). Its draws put winners and new clubs in any tie, so a champion could meet a non-champion. Here the
 * wrap of 0x518790 writes the ties again after the original: a Champions Path tie has only champions (new
 * champions and the winners of the Champions Path ties below), a League Path tie only the other clubs, each
 * winner of the round below meets a new club when it can (as the seeded draw 0x51d2d0 does), and the rest
 * meet each other at random (0x90cfc0). Every round has flag 0x200 of +0xd, so the draw 0x51c410 keeps the
 * ties (0x51c5d4..0x51c5f1) and only applies the home and away rules (0x51e870, 0x522610).
 *
 * Europa League (Uefa_Cup, vtable 0x970790). build_entrants 0x902b90 (wrapped in season_uefa_cup.c) gives way
 * to uc_access_entrants: the qualifying clubs by round from the lowest (each round: its new clubs, then slots for
 * the Champions League losers that the drops of the row send to it), the group clubs, 16 slots for the
 * Champions League play-off losers (the original's hand-over at count - 0x18, 0x585a02, in draw_phase1_groups),
 * and 8 slots for the Champions League group thirds (count - 8, 0x586282). The rounds are drawn by the
 * original (seeded, 0x400): the new clubs of a round have a higher tier byte +4 than the winners. A round with
 * an empty slot waits (0x51c46d, 0x51c9ab), so the losers of each Champions League round are put into their
 * slots on the days after that round ends (the wrap of Cup::update_for_date 0x51f890 for the Europa League),
 * and the dates of each Europa League round come after the Champions League round that feeds it. The group
 * stage of season_uefa_cup.c takes the play-off winners and every club of the slots after the rounds. */
#include "win.h"
#include "season.h"
#include "season_uefa.h"

#include <stdio.h>
#include <string.h>

#define KEY_CL 0x9cf6e8u
#define KEY_UC 0x9cf6f0u
#define COMP_TABLE 0xadadfcu
#define CL_VTABLE 0x0096a1d0u
#define UC_VTABLE 0x00970790u
#define CLUBS 0xae23b4u
#define CLUB_COUNT 0xae235cu

#define F_MALLOC 0x00944e46u
#define F_FREE 0x009452cau
#define F_RANDOM 0x0090cfc0u
#define F_CL_ROUNDS 0x00583c50u         /* EuropeanCup::create_round_table, thiscall (stage, &rounds, &name, &league) */
#define F_DRAW_DATE 0x00521e60u
#define F_MATCH_DATE 0x00521eb0u

#define ROUND_SIZE 0x68u
#define TIE_SIZE 0x35u
#define ENTRY 6u
#define MAX_ROUNDS 6
#define MAX_ENTRANTS 260
#define DIRECT_IN_LIST 16               /* the group clubs that draw_phase1_groups reads from the list (0x585358) */
#define HANDOVER 0x18                   /* the hand-over slots of the play-off losers: count - 0x18 (0x585a02) */
#define THIRDS 8

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

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

static int random_below(int n)
{
    if (n <= 1) return 0;
    uint32_t a[1] = {(uint32_t)n};
    int k = (int)call_cdecl(F_RANDOM, 1, a);
    return k < 0 || k >= n ? 0 : k;
}

static uint32_t comp_object(uint32_t key, uint32_t vtable)
{
    int32_t id = (int32_t)rd32(g_mem, key);
    uint32_t table = rd32(g_mem, COMP_TABLE);
    if (id < 0 || !readable(table)) return 0;
    uint32_t obj = rd32(g_mem, table + 4u * (uint32_t)id);
    return readable(obj) && rd32(g_mem, obj) == vtable ? obj : 0;
}

/* The row of a cup's season (+0x40) when it has an access list, else NULL. */
static const SeasonUefa *access_row_of(uint32_t cup)
{
    const SeasonUefa *r = season_uefa_row((int16_t)rd16(g_mem, cup + 0x40));
    return r && season_uefa_access_places(r->season, 'c', 1) ? r : NULL;
}

static void set_dates(uint32_t t, int r, int draw_day, int draw_month, int day, int month, int weekday, uint32_t year)
{
    uint32_t d[7] = {t, (uint32_t)r, (uint32_t)draw_day, (uint32_t)draw_month, 0, 4, year};
    call_cdecl(F_DRAW_DATE, 7, d);
    uint32_t m[9] = {t, (uint32_t)r, (uint32_t)day, (uint32_t)month, 0, (uint32_t)weekday, 2, year, 0};
    if (!rt_unwinding) call_cdecl(F_MATCH_DATE, 9, m);
}

/* ---- The layout of a cup's entrants ---- */

/* A qualifying round: its code (the round code of the game), the entry codes of its paths, its legs and the
 * dates. */
typedef struct {
    int round_code;
    int cp, lp;                         /* entry codes of the Champions Path and League Path (lp 0: one path) */
    int legs;
    int draw[2], match[2];              /* draw and first leg: day, month 0 to 11 */
} Level;

/* The dates. A game in a moved season starts on 11 July (the first day of the rules report of a 2010/11
 * game), so every round is played from 12 July on, as the original's first qualifying round (12 July, 0x583c50).
 * The second leg comes 7 days after the first (+0x22). A round starts after the round below ends, and a Europa
 * League round after the Champions League round whose losers it takes (CL Q1 ends 21 July: EL Q2 from 26 July;
 * CL Q2 4 August: EL Q3 9 August; CL Q3 18 August: EL PO 23 August; CL PO 28 August, before the Europa League's
 * group draw after 30 August). The preliminary round is one match (the real one is a tournament of single
 * matches). The real calendar starts in late June (rows uefa-66 to uefa-160): these dates keep its order. */
static const Level CL_LEVELS[] = {
    {240, SEASON_UEFA_CL_PR, 0, 1, {12, 6}, {13, 6}},
    {250, SEASON_UEFA_CL_Q1, 0, 2, {12, 6}, {14, 6}},
    {260, SEASON_UEFA_CL_Q2_CP, SEASON_UEFA_CL_Q2_LP, 2, {22, 6}, {28, 6}},
    {270, SEASON_UEFA_CL_Q3_CP, SEASON_UEFA_CL_Q3_LP, 2, {5, 7}, {11, 7}},
    {220, SEASON_UEFA_CL_PO_CP, SEASON_UEFA_CL_PO_LP, 2, {19, 7}, {21, 7}},
};
static const Level EL_LEVELS[] = {
    {240, SEASON_UEFA_EL_PR, 0, 1, {12, 6}, {13, 6}},
    {250, SEASON_UEFA_EL_Q1, 0, 2, {14, 6}, {16, 6}},
    {260, SEASON_UEFA_EL_Q2, 0, 2, {22, 6}, {26, 6}},
    {270, SEASON_UEFA_EL_Q3, 0, 2, {5, 7}, {9, 7}},
    {220, SEASON_UEFA_EL_PO, 0, 2, {19, 7}, {23, 7}},
};
#define N_LEVELS 5

typedef struct {
    int n;                              /* entrants of the list */
    uint32_t clubs[MAX_ENTRANTS];       /* by code: group clubs, then the qualifying codes from the lowest round */
    int count[SEASON_UEFA_CODE_SLOTS];                      /* clubs of each code */
    int drops[SEASON_UEFA_CODE_SLOTS];                      /* Europa League: slots for Champions League losers at each code */
    int rounds;                         /* qualifying rounds played */
    int level[MAX_ROUNDS];              /* the level of each round (index into the LEVELS table) */
    int clubs_in[MAX_ROUNDS], fresh[MAX_ROUNDS], first[MAX_ROUNDS];
    int cp_fresh[MAX_ROUNDS], lp_fresh[MAX_ROUNDS];
    int direct_first, slots;            /* the first slot after the rounds, the slot count */
} Layout;

static int club_code(uint32_t club) { return g_mem[club + 0x1df]; }

/* The layout of comp ('c' or 'e'). The Champions League's from the list +0x14 of cl when cl is given (after
 * build_entrants: a loser that goes to the Europa League gets that cup's id at +0x1db, 0x585a6a, but stays in the
 * list), else from the clubs with its id at +0x1db; the Europa League's from the clubs with its id and a code of
 * 11 to 16 (the Champions League losers there keep their codes of 1 to 9). Returns 0 when the counts cannot make
 * rounds (an odd number of clubs in a path). */
static int make_layout(const SeasonUefa *r, char comp, Layout *L, uint32_t cl)
{
    memset(L, 0, sizeof *L);
    uint32_t key = rd32(g_mem, comp == 'c' ? KEY_CL : KEY_UC), table = rd32(g_mem, CLUBS);
    int n = (int32_t)rd32(g_mem, CLUB_COUNT);
    int lo = comp == 'c' ? 1 : SEASON_UEFA_EL_G, codes = comp == 'c' ? SEASON_UEFA_CL_CODES : SEASON_UEFA_EL_CODES;
    uint32_t list = cl ? rd32(g_mem, cl + 0x14) : 0;
    int listed = cl ? (int16_t)rd16(g_mem, cl + 0x36) : 0;
    for (int code = lo; code < codes; code++) {
        if (cl) {
            for (int i = 0; readable(list) && i < listed && L->n < MAX_ENTRANTS; i++) {
                uint32_t club = rd32(g_mem, list + ENTRY * (uint32_t)i);
                if (readable(club) && club_code(club) == code) L->clubs[L->n++] = club, L->count[code]++;
            }
            continue;
        }
        for (int i = 0; i < n && readable(table); i++) {
            uint32_t club = table + 0x245u * (uint32_t)i;
            if (rd32(g_mem, club + 0x1db) != key || club_code(club) != code || L->n >= MAX_ENTRANTS) continue;
            L->clubs[L->n++] = club;
            L->count[code]++;
        }
    }
    if (comp == 'e')
        for (int code = lo; code < codes; code++) L->drops[code] = season_uefa_dropped(r, code);
    const Level *lv = comp == 'c' ? CL_LEVELS : EL_LEVELS;
    int slot = 0, cp_win = 0, lp_win = 0;
    for (int k = 0; k < N_LEVELS; k++) {
        int cp = L->count[lv[k].cp] + L->drops[lv[k].cp], lp = lv[k].lp ? L->count[lv[k].lp] : 0;
        if (!cp && !lp && !cp_win && !lp_win) continue;
        if (!cp && !lp) {                              /* a round with no new clubs: not in the access list */
            fprintf(stderr, "recomp: uefa: %s round %d has no new clubs\n", comp == 'c' ? "CL" : "EL", lv[k].round_code);
            return 0;
        }
        int i = L->rounds++;
        if (i >= MAX_ROUNDS) return 0;
        L->level[i] = k;
        /* Champions League: the winners stay in their path; a round of one path (Q2 to 2017/18) takes both. */
        if (comp == 'c' && !lp) cp_win += lp_win, lp_win = 0;
        if (comp == 'e') cp_win += lp_win, lp_win = 0;
        L->cp_fresh[i] = cp, L->lp_fresh[i] = lp;
        L->fresh[i] = cp + lp, L->first[i] = slot, L->clubs_in[i] = cp + lp + cp_win + lp_win;
        if ((cp + cp_win) % 2 || (lp + lp_win) % 2) {
            fprintf(stderr, "recomp: uefa: %s round %d: %d + %d and %d + %d clubs, not even\n", comp == 'c' ? "CL" : "EL",
                    lv[k].round_code, cp, cp_win, lp, lp_win);
            return 0;
        }
        slot += cp + lp;
        cp_win = (cp + cp_win) / 2, lp_win = (lp + lp_win) / 2;
    }
    L->direct_first = slot;
    int direct = L->count[comp == 'c' ? SEASON_UEFA_CL_G : SEASON_UEFA_EL_G];
    if (comp == 'c') {
        if (direct < DIRECT_IN_LIST || direct + cp_win + lp_win != 32) {
            fprintf(stderr, "recomp: uefa: CL groups: %d direct and %d winners, not 32\n", direct, cp_win + lp_win);
            return 0;
        }
        L->slots = slot + direct - DIRECT_IN_LIST;
    } else {
        L->drops[SEASON_UEFA_EL_G] = season_uefa_dropped(r, SEASON_UEFA_EL_G);
        L->slots = slot + direct + HANDOVER + THIRDS;
    }
    return L->rounds > 0;
}

/* The slot of the first new club of code in the entry table (the clubs of the list in code order). */
static int list_index_of_code(const Layout *L, int code)
{
    int at = 0;
    for (int i = 0; i < L->n; i++) {
        if (club_code(L->clubs[i]) == code) return at;
        at++;
    }
    return at;
}

/* ---- Champions League ---- */

void o_00584fa0(void);
void o_00584990(void);

/* The list +0x14 of the access list: the group clubs, then the qualifying clubs from the lowest round; list
 * byte +4 as the original's tiers (7 group, 8 Q3, 13 Q2, 0 others: 0x584fa0). */
static int cl_entrants(uint32_t cl, const Layout *L)
{
    uint32_t list = game_malloc(ENTRY * (uint32_t)L->n);
    if (!list) return 0;
    game_free(rd32(g_mem, cl + 0x14));
    if (rt_unwinding) return 0;
    int at = 0;
    for (int i = 0; i < L->n; i++) {                                  /* the group clubs first */
        if (club_code(L->clubs[i]) != SEASON_UEFA_CL_G) continue;
        wr32(g_mem, list + ENTRY * (uint32_t)at, L->clubs[i]);
        g_mem[list + ENTRY * (uint32_t)at + 4] = 7;
        at++;
    }
    for (int k = 0; k < N_LEVELS; k++)
        for (int path = 0; path < 2; path++) {
            int code = path ? CL_LEVELS[k].lp : CL_LEVELS[k].cp;
            for (int i = 0; code && i < L->n; i++) {
                if (club_code(L->clubs[i]) != code) continue;
                wr32(g_mem, list + ENTRY * (uint32_t)at, L->clubs[i]);
                g_mem[list + ENTRY * (uint32_t)at + 4] = k == 3 ? 8 : k == 2 ? 13 : 0;
                at++;
            }
        }
    wr32(g_mem, cl + 0x14, list);
    wr16(g_mem, cl + 0x36, (uint16_t)at);
    wr16(g_mem, cl + 0x38, (uint16_t)at);
    return 1;
}

/* EuropeanCup::build_entrants, thiscall () returning 1 or 0. */
RT_WRAP(00584fa0, cl_build_entrants)
{
    uint32_t cl = cpu.ecx;
    const SeasonUefa *r = rd32(g_mem, cl) == CL_VTABLE ? access_row_of(cl) : NULL;
    Layout L;
    if (!r || !make_layout(r, 'c', &L, 0)) {
        if (r) fprintf(stderr, "recomp: uefa: Champions League %d: the access list gives no layout\n", r->season);
        o_00584fa0();
        return;
    }
    int ok = cl_entrants(cl, &L);
    if (rt_unwinding) return;
    fprintf(stderr, "recomp: uefa: Champions League %d/%02d: %d entrants, %d direct, %d qualifying rounds\n",
            (int16_t)rd16(g_mem, cl + 0x40), ((int16_t)rd16(g_mem, cl + 0x40) + 1) % 100, L.n,
            L.count[SEASON_UEFA_CL_G], L.rounds);
    rt_ret_cdecl(ok ? 1u : 0u);
}

/* The entry table of the list: the qualifying clubs (list slots after the group clubs, in order), then the
 * group clubs after the first 16. Entry byte +4 is the round of entry (0 the lowest), +5 0. */
RT_WRAP(00584990, cl_entry_table)
{
    uint32_t cl = cpu.ecx;
    const SeasonUefa *r = rd32(g_mem, cl) == CL_VTABLE ? access_row_of(cl) : NULL;
    Layout L;
    if (!r || !make_layout(r, 'c', &L, cl) || (int16_t)rd16(g_mem, cl + 0x36) != L.n) {
        o_00584990();
        return;
    }
    uint32_t e = game_malloc(ENTRY * (uint32_t)(L.slots > 0 ? L.slots : 1));
    if (!e) {
        if (!rt_unwinding) rt_ret_cdecl(0);
        return;
    }
    uint32_t list = rd32(g_mem, cl + 0x14);
    int direct = L.count[SEASON_UEFA_CL_G], q = L.n - direct;
    for (int i = 0; i < q; i++) {
        uint32_t club = rd32(g_mem, list + ENTRY * (uint32_t)(direct + i));
        wr32(g_mem, e + ENTRY * (uint32_t)i, club);
        int tier = 0;
        for (int k = 0; k < L.rounds; k++)
            if (i >= L.first[k] && i < L.first[k] + L.fresh[k]) tier = k;
        g_mem[e + ENTRY * (uint32_t)i + 4] = (uint8_t)tier;
    }
    for (int i = DIRECT_IN_LIST; i < direct; i++)
        wr32(g_mem, e + ENTRY * (uint32_t)(q + i - DIRECT_IN_LIST), rd32(g_mem, list + ENTRY * (uint32_t)i));
    wr32(g_mem, cl + 0xa7, e);
    wr16(g_mem, cl + 0x3e, (uint16_t)L.slots);
    rt_ret_cdecl(1);
}

static int cl_inner;

/* The round table of stage -1 for the access list (from season_uefa.c's wrap of 0x583c50), or 0: one round
 * for each round of the layout, made from the original's Q2 record (code 260, 0x583d.., as slice 22's tables),
 * with flag 0x200 (the ties of map_fixture_tree stay) and the nation rule flag 4 of +0xb. */
uint32_t season_uefa_cl_qual_table(uint32_t cl, int *rounds_out, uint32_t *name_out, uint32_t *league_out)
{
    const SeasonUefa *r = access_row_of(cl);
    Layout L;
    if (cl_inner || !r || !make_layout(r, 'c', &L, cl)) return 0;
    uint32_t saved_esp = cpu.esp;
    cpu.esp -= 16;
    uint32_t out = cpu.esp;
    for (int i = 0; i < 16; i += 4) wr32(g_mem, out + (uint32_t)i, 0);
    uint32_t a[4] = {0xffffffffu, out, out + 4, out + 8};
    cl_inner = 1;
    uint32_t orig = call_this(F_CL_ROUNDS, cl, 4, a);
    cl_inner = 0;
    int orig_rounds = (int16_t)rd16(g_mem, out);
    *name_out = rd32(g_mem, out + 4);
    *league_out = rd32(g_mem, out + 8);
    cpu.esp = saved_esp;
    if (rt_unwinding || !readable(orig) || orig_rounds < 2) return 0;
    uint32_t t = game_malloc(ROUND_SIZE * (uint32_t)L.rounds);
    uint32_t year = (uint32_t)(int16_t)rd16(g_mem, cl + 0x40);
    for (int i = 0; t && i < L.rounds && !rt_unwinding; i++) {
        uint32_t rec = t + ROUND_SIZE * (uint32_t)i;
        memcpy(g_mem + rec, g_mem + orig + ROUND_SIZE, ROUND_SIZE);
        const Level *lv = &CL_LEVELS[L.level[i]];
        wr16(g_mem, rec + 7, (uint16_t)lv->round_code);
        wr16(g_mem, rec + 9, (uint16_t)i);
        wr16(g_mem, rec + 0xd, 0x200);
        g_mem[rec + 0xb] = 4;
        wr16(g_mem, rec + 0x18, (uint16_t)L.clubs_in[i]);
        wr16(g_mem, rec + 0x1a, (uint16_t)(L.clubs_in[i] / 2));
        wr16(g_mem, rec + 0x1c, (uint16_t)L.fresh[i]);
        wr16(g_mem, rec + 0x1e, (uint16_t)L.first[i]);
        g_mem[rec + 0x21] = (uint8_t)lv->legs;
        g_mem[rec + 0x22] = 7;
        set_dates(t, i, lv->draw[0], lv->draw[1], lv->match[0], lv->match[1], 2, year);
    }
    game_free(orig);
    if (rt_unwinding || !t) return 0;
    *rounds_out = L.rounds;
    fprintf(stderr, "recomp: uefa: Champions League %u/%02u: %d qualifying rounds, %d slots\n", year, (year + 1) % 100,
            L.rounds, L.slots);
    return t;
}

/* ---- The ties of the paths (wrap of map_fixture_tree 0x518790) ---- */

static uint32_t tie_at(uint32_t cup, int round, int t)
{
    uint32_t arr = rd32(g_mem, cup + 0xad);
    return rd32(g_mem, arr + 4u * (uint32_t)round) + TIE_SIZE * (uint32_t)t;
}

/* A side to place: an entry slot (src -1) or the winner of tie src of the round before. */
typedef struct {
    int slot, src;
} Side;

static void shuffle(Side *s, int n)
{
    for (int i = n - 1; i > 0 && !rt_unwinding; i--) {
        int j = random_below(i + 1);
        Side x = s[i];
        s[i] = s[j], s[j] = x;
    }
}

/* Writes side s into tie t of round r (home h). */
static void put_side(uint32_t cup, int r, int t, int h, Side s)
{
    uint32_t tie = tie_at(cup, r, t);
    wr16(g_mem, tie + (h ? 0 : 2), (uint16_t)(int16_t)(s.src < 0 ? s.slot : -1));
    if (s.src < 0) return;
    wr16(g_mem, tie + (h ? 0xa : 0xc), (uint16_t)s.src);
    wr16(g_mem, tie + (h ? 0xe : 0x10), (uint16_t)(r - 1));
    uint32_t from = tie_at(cup, r - 1, s.src);
    wr16(g_mem, from + 6, (uint16_t)t);
    wr16(g_mem, from + 8, (uint16_t)(h ? 1 : 0));
    wr16(g_mem, from + 0x12, (uint16_t)r);
}

/* Pairs the winners w (nw) with the new clubs f (nf) into ties first .. of round r: each winner meets a new
 * club when it can, the rest meet each other. Returns the ties used. */
static int pair_path(uint32_t cup, int r, int first, Side *w, int nw, Side *f, int nf)
{
    shuffle(w, nw);
    shuffle(f, nf);
    int t = first, iw = 0, iff = 0;
    while (iw < nw && iff < nf) {
        int h = random_below(2);
        put_side(cup, r, t, h, w[iw++]);
        put_side(cup, r, t, !h, f[iff++]);
        t++;
    }
    while (iw + 1 < nw) put_side(cup, r, t, 1, w[iw++]), put_side(cup, r, t++, 0, w[iw++]);
    while (iff + 1 < nf) put_side(cup, r, t, 1, f[iff++]), put_side(cup, r, t++, 0, f[iff++]);
    return t - first;
}

/* The Champions Path ties of round r (cl_paths puts them first): its new champions and the winners of the
 * Champions Path ties of the round before (of every tie when round r has one path). */
static int cp_ties_of(const Layout *L, int r)
{
    if (r < 0) return 0;
    int win = r == 0 ? 0 : L->lp_fresh[r] == 0 ? L->clubs_in[r - 1] / 2 : cp_ties_of(L, r - 1);
    return (win + L->cp_fresh[r]) / 2;
}

static void cl_paths(uint32_t cl, const Layout *L)
{
    /* The ties of round r: Champions Path ties 0 .. cp_ties[r] - 1, League Path ties after them. */
    int cp_ties[MAX_ROUNDS] = {0};
    for (int r = 0; r < L->rounds && !rt_unwinding; r++) {
        int ties = L->clubs_in[r] / 2;
        for (int t = 0; t < ties; t++) {
            uint32_t tie = tie_at(cl, r, t);
            for (int off = 0; off <= 0x12; off += 2) wr16(g_mem, tie + (uint32_t)off, 0xffffu);
            wr16(g_mem, tie + 8, 0);
        }
        Side w[2][64], f[2][64];
        int nw[2] = {0, 0}, nf[2] = {0, 0};
        int one_path = L->lp_fresh[r] == 0;
        if (r > 0) {
            int prev = L->clubs_in[r - 1] / 2;
            for (int t = 0; t < prev && t < 64; t++) {
                int p = one_path || t < cp_ties[r - 1] ? 0 : 1;
                w[p][nw[p]++] = (Side){-1, t};
            }
        }
        for (int i = 0; i < L->cp_fresh[r] && i < 64; i++) f[0][nf[0]++] = (Side){L->first[r] + i, -1};
        for (int i = 0; i < L->lp_fresh[r] && i < 64; i++) f[1][nf[1]++] = (Side){L->first[r] + L->cp_fresh[r] + i, -1};
        cp_ties[r] = pair_path(cl, r, 0, w[0], nw[0], f[0], nf[0]);
        pair_path(cl, r, cp_ties[r], w[1], nw[1], f[1], nf[1]);
    }
}

void o_00518790(void);

/* Cup::map_fixture_tree, thiscall () (vtable +0x94; 0x5838df, 0x584f14). */
RT_WRAP(00518790, cup_map_fixture_tree)
{
    uint32_t cup = cpu.ecx;
    o_00518790();
    if (rt_unwinding || !readable(cup) || rd32(g_mem, cup) != CL_VTABLE) return;
    const SeasonUefa *r = access_row_of(cup);
    Layout L;
    uint32_t t = rd32(g_mem, cup + 0xa3);
    if (!r) return;
    if (!make_layout(r, 'c', &L, cup) || !readable(t) || (int16_t)rd16(g_mem, cup + 0x3c) != L.rounds ||
        (int16_t)rd16(g_mem, cup + 0x3e) != L.slots || (int16_t)rd16(g_mem, t + 7) != CL_LEVELS[L.level[0]].round_code) {
        fprintf(stderr, "recomp: uefa: Champions League ties: the cup does not hold the access list layout (%d rounds, "
                "%d slots)\n", (int16_t)rd16(g_mem, cup + 0x3c), (int16_t)rd16(g_mem, cup + 0x3e));
        return;
    }
    Cpu saved = cpu;
    cl_paths(cup, &L);
    if (rt_unwinding) return;
    /* The group clubs after the first 16: +5 = 1, as a winner of the last round (0x51af97). */
    uint32_t e = rd32(g_mem, cup + 0xa7);
    for (int i = L.direct_first; i < L.slots; i++) g_mem[e + ENTRY * (uint32_t)i + 5] = 1;
    cpu = saved;
    fprintf(stderr, "recomp: uefa: Champions League ties of %d rounds in two paths\n", L.rounds);
}

/* ---- Europa League ---- */

/* The entry table and list of the Europa League (from season_uefa_cup.c's wrap of build_entrants 0x902b90).
 * Returns 1 when it made them. */
int season_uefa_uc_entrants(uint32_t uc)
{
    const SeasonUefa *r = access_row_of(uc);
    Layout L;
    if (!r || !make_layout(r, 'e', &L, 0)) return 0;
    uint32_t e = game_malloc(ENTRY * (uint32_t)L.slots), list = e ? game_malloc(ENTRY * (uint32_t)L.slots) : 0;
    if (!list) return 0;
    game_free(rd32(g_mem, uc + 0xa7));
    game_free(rd32(g_mem, uc + 0x14));
    if (rt_unwinding) return 0;
    int slot = 0;
    for (int i = 0; i < L.rounds; i++) {
        int code = EL_LEVELS[L.level[i]].cp, k = list_index_of_code(&L, code);
        for (int j = 0; j < L.count[code]; j++, slot++) {
            wr32(g_mem, e + ENTRY * (uint32_t)slot, L.clubs[k + j]);
            g_mem[e + ENTRY * (uint32_t)slot + 4] = (uint8_t)(i + 1);
        }
        for (int j = 0; j < L.drops[code]; j++, slot++) g_mem[e + ENTRY * (uint32_t)slot + 4] = (uint8_t)(i + 2);
    }
    int n = 0, k = list_index_of_code(&L, SEASON_UEFA_EL_G);
    for (int j = 0; j < L.count[SEASON_UEFA_EL_G]; j++, slot++) {
        wr32(g_mem, e + ENTRY * (uint32_t)slot, L.clubs[k + j]);
        wr32(g_mem, list + ENTRY * (uint32_t)n, L.clubs[k + j]);
        g_mem[list + ENTRY * (uint32_t)n + 4] = 3;                      /* as the first round clubs of 0x902deb */
        n++;
    }
    wr32(g_mem, uc + 0xa7, e);
    wr32(g_mem, uc + 0x14, list);
    wr16(g_mem, uc + 0x36, (uint16_t)n);
    wr16(g_mem, uc + 0x38, (uint16_t)L.slots);
    wr16(g_mem, uc + 0x3e, (uint16_t)L.slots);
    g_mem[uc + 0xb2] = 0;
    fprintf(stderr, "recomp: uefa: Europa League %d/%02d: %d entrants, %d direct, %d qualifying rounds, %d slots\n",
            (int16_t)rd16(g_mem, uc + 0x40), ((int16_t)rd16(g_mem, uc + 0x40) + 1) % 100, L.n,
            L.count[SEASON_UEFA_EL_G], L.rounds, L.slots);
    return 1;
}

/* The round table of stage -1 of the Europa League (from season_uefa_cup.c's wrap of 0x9025c0), made from the
 * record rec of the original's table (its qualifying round). Seeded draws (0x400; 0x51d2d0: the new clubs have
 * the higher tier) that avoid two clubs of one nation (+0xb 4). Returns the table or 0. */
uint32_t season_uefa_uc_qual_table(uint32_t uc, uint32_t rec, int *rounds_out)
{
    const SeasonUefa *r = access_row_of(uc);
    Layout L;
    if (!r || !readable(rec) || !make_layout(r, 'e', &L, 0) || (int16_t)rd16(g_mem, uc + 0x3e) != L.slots) return 0;
    uint32_t t = game_malloc(ROUND_SIZE * (uint32_t)L.rounds);
    uint32_t year = (uint32_t)(int16_t)rd16(g_mem, uc + 0x40);
    for (int i = 0; t && i < L.rounds && !rt_unwinding; i++) {
        uint32_t x = t + ROUND_SIZE * (uint32_t)i;
        memcpy(g_mem + x, g_mem + rec, ROUND_SIZE);
        const Level *lv = &EL_LEVELS[L.level[i]];
        wr16(g_mem, x + 7, (uint16_t)lv->round_code);
        wr16(g_mem, x + 9, (uint16_t)i);
        wr16(g_mem, x + 0xd, i ? 0x400 : 0);
        g_mem[x + 0xb] = 4;
        wr16(g_mem, x + 0x18, (uint16_t)L.clubs_in[i]);
        wr16(g_mem, x + 0x1a, (uint16_t)(L.clubs_in[i] / 2));
        wr16(g_mem, x + 0x1c, (uint16_t)L.fresh[i]);
        wr16(g_mem, x + 0x1e, (uint16_t)L.first[i]);
        g_mem[x + 0x21] = (uint8_t)lv->legs;
        g_mem[x + 0x22] = 7;
        set_dates(t, i, lv->draw[0], lv->draw[1], lv->match[0], lv->match[1], 4, year);
    }
    if (rt_unwinding || !t) return 0;
    *rounds_out = L.rounds;
    return t;
}

/* The layout of the Europa League from its round table +0xa3 (+0x3c rounds), once the season has started: its
 * clubs lose their id at +0x1db when they go out, so make_layout cannot count them any more. Round i takes
 * +0x1c new slots from +0x1e; the last of them are the drop slots of the row. Returns 0 when the table is not
 * one of the access list (a round code that is not of EL_LEVELS, slots that do not follow each other). */
static int uc_table_layout(uint32_t uc, const SeasonUefa *r, Layout *L)
{
    memset(L, 0, sizeof *L);
    uint32_t t = rd32(g_mem, uc + 0xa3);
    int rounds = (int16_t)rd16(g_mem, uc + 0x3c), slot = 0, k = 0;
    if (!readable(t) || rounds < 1 || rounds > MAX_ROUNDS || (int32_t)rd32(g_mem, uc + 0x30) != -1) return 0;
    for (int i = 0; i < rounds; i++) {
        uint32_t rec = t + ROUND_SIZE * (uint32_t)i;
        int code = (int16_t)rd16(g_mem, rec + 7);
        while (k < N_LEVELS && EL_LEVELS[k].round_code != code) k++;
        if (k == N_LEVELS || (int16_t)rd16(g_mem, rec + 0x1e) != slot) return 0;
        int lv = EL_LEVELS[k].cp, fresh = (int16_t)rd16(g_mem, rec + 0x1c);
        L->level[i] = k, L->first[i] = slot, L->fresh[i] = fresh, L->cp_fresh[i] = fresh;
        L->clubs_in[i] = (int16_t)rd16(g_mem, rec + 0x18);
        L->drops[lv] = season_uefa_dropped(r, lv);
        L->count[lv] = fresh - L->drops[lv];
        slot += fresh;
        k++;
    }
    L->rounds = rounds;
    L->direct_first = slot;
    L->slots = (int16_t)rd16(g_mem, uc + 0x3e);
    L->drops[SEASON_UEFA_EL_G] = season_uefa_dropped(r, SEASON_UEFA_EL_G);
    return L->slots > slot + HANDOVER + THIRDS - 1;
}

/* The first slot after the Europa League's rounds (the group clubs, the hand-over, the thirds), or -1. */
int season_uefa_uc_direct_first(uint32_t uc)
{
    const SeasonUefa *r = access_row_of(uc);
    Layout L;
    return r && uc_table_layout(uc, r, &L) ? L.direct_first : -1;
}

/* ---- The Champions League losers into the Europa League ---- */

/* The round of the Champions League layout at level k, or -1. */
static int cl_round_of_level(const Layout *L, int k)
{
    for (int i = 0; i < L->rounds; i++)
        if (L->level[i] == k) return i;
    return -1;
}

/* The losers of the decided ties of Champions League round r and path (0 Champions Path, 1 League Path, 2
 * both), in tie order, into out (max). A tie is decided when its winner is in the tie of the next round
 * (+6/+8/+0x12); the loser is the other side. Returns the count, or -1 when a tie is not decided yet. */
static int cl_losers(uint32_t cl, const Layout *L, int r, int path, uint32_t *out, int max)
{
    uint32_t e = rd32(g_mem, cl + 0xa7);
    int ties = L->clubs_in[r] / 2, n = 0;
    int cp_ties = cp_ties_of(L, r);
    for (int t = 0; t < ties; t++) {
        if (path == 0 && t >= cp_ties) continue;
        if (path == 1 && t < cp_ties) continue;
        uint32_t tie = tie_at(cl, r, t);
        int16_t home = (int16_t)rd16(g_mem, tie), away = (int16_t)rd16(g_mem, tie + 2);
        int16_t next_t = (int16_t)rd16(g_mem, tie + 6), side = (int16_t)rd16(g_mem, tie + 8),
                next_r = (int16_t)rd16(g_mem, tie + 0x12);
        if (home < 0 || away < 0) return -1;
        int16_t win;
        if (next_t >= 0 && next_r > r) {
            win = (int16_t)rd16(g_mem, tie_at(cl, next_r, next_t) + (side ? 0 : 2));
            if (win < 0) return -1;
        } else {                                                  /* the last round: +5 1 the winner (0x51af97) */
            int8_t s = (int8_t)g_mem[e + ENTRY * (uint32_t)home + 5];
            if (s != 1 && s != 2) return -1;
            win = s == 1 ? home : away;
        }
        int16_t lose = win == home ? away : home;
        if (n < max) out[n] = rd32(g_mem, e + ENTRY * (uint32_t)lose);
        n++;
    }
    return n;
}

/* The source of a drop ("Q3-CP"): its Champions League level (index of CL_LEVELS) and path (0 Champions Path,
 * 1 League Path, 2 both), or -1. */
static int drop_source(const char *tok, int n, int *path)
{
    static const char *const names[N_LEVELS] = {"PR", "Q1", "Q2", "Q3", "PO"};
    *path = n == 5 && !strncmp(tok + 2, "-CP", 3) ? 0 : n == 5 && !strncmp(tok + 2, "-LP", 3) ? 1 : 2;
    for (int k = 0; k < N_LEVELS; k++)
        if (n >= 2 && !strncmp(tok, names[k], 2)) return k;
    return -1;
}

/* The losers of each decided Champions League round go into their Europa League slots (the drops of the row).
 * The play-off losers that go to the groups come with the original's hand-over (0x585a02) into the slots from
 * count - 0x18; other losers for the groups (a League Path third qualifying round, 2018/19 on) take the slots
 * from count - 9 down, so the two never meet. A slot that holds a club is left as it is. */
static void uc_fill_drops(uint32_t uc)
{
    const SeasonUefa *r = access_row_of(uc);
    uint32_t cl = comp_object(KEY_CL, CL_VTABLE);
    Layout L, C;
    if (!r || !cl || !r->drops || !*r->drops || !access_row_of(cl) || !uc_table_layout(uc, r, &L) ||
        !make_layout(access_row_of(cl), 'c', &C, cl) ||
        (int16_t)rd16(g_mem, cl + 0x3e) != C.slots || (int16_t)rd16(g_mem, cl + 0x3c) != C.rounds)
        return;
    uint32_t e = rd32(g_mem, uc + 0xa7), list = rd32(g_mem, uc + 0x14);
    int used[SEASON_UEFA_CODE_SLOTS] = {0}, count = (int16_t)rd16(g_mem, uc + 0x3e);
    for (const char *p = r->drops; *p && !rt_unwinding;) {
        while (*p == ' ') p++;
        const char *gt = strchr(p, '>');
        if (!gt) break;
        const char *end = gt + 1;
        while (*end && *end != ' ') end++;
        int path, level = drop_source(p, (int)(gt - p), &path);
        int to = season_uefa_code('e', gt + 1, (int)(end - gt - 1)), cr = level >= 0 ? cl_round_of_level(&C, level) : -1;
        p = end;
        if (cr < 0 || to <= 0 || (to == SEASON_UEFA_EL_G && level == 4)) continue;   /* PO>G: the hand-over */
        uint32_t losers[64];
        int n = cl_losers(cl, &C, cr, path, losers, 64);
        int first;
        if (to == SEASON_UEFA_EL_G) {
            first = count - THIRDS - 1;                           /* from count - 9 down */
        } else {
            int i = 0;
            while (i < L.rounds && EL_LEVELS[L.level[i]].cp != to) i++;
            if (i == L.rounds) continue;
            first = L.first[i] + L.count[to];
        }
        int base = used[to];
        used[to] += n > 0 ? n : (path == 2 ? C.clubs_in[cr] / 2 : 0);
        int moved = 0;
        for (int k = 0; k < n && k < 64; k++) {
            int slot = to == SEASON_UEFA_EL_G ? first - base - k : first + base + k;
            if (slot < 0 || slot >= count || !readable(losers[k])) continue;
            uint32_t at = e + ENTRY * (uint32_t)slot;
            if (readable(rd32(g_mem, at))) continue;
            wr32(g_mem, at, losers[k]);                           /* as the hand-over 0x585a32..0x585a6a */
            g_mem[at + 5] = 0;
            int16_t m = (int16_t)rd16(g_mem, uc + 0x36);
            if (m < (int16_t)rd16(g_mem, uc + 0x38)) {
                wr32(g_mem, list + ENTRY * (uint32_t)m, losers[k]);
                g_mem[list + ENTRY * (uint32_t)m + 4] = 5;
                wr16(g_mem, uc + 0x36, (uint16_t)(m + 1));
            }
            wr32(g_mem, losers[k] + 0x1db, rd32(g_mem, KEY_UC));
            moved++;
        }
        if (moved)
            fprintf(stderr, "recomp: uefa: %d losers of Champions League round %d (path %d) into Europa League slots from %d\n",
                    moved, CL_LEVELS[level].round_code, path, first);
    }
}

/* For season_uefa_cup.c: the drops before the group draw. */
void season_uefa_uc_fill_drops(uint32_t uc) { uc_fill_drops(uc); }

void o_0051f890(void);

#include <stdlib.h>

/* Test only (RECOMP_UEFA_LOG=1): each week, the state of a cup's qualifying rounds: the current stage +0x30,
 * the next round +0xab, the rounds set up +0xac, +0xb1, and for each round its ties by state (+0x34: 0 not
 * drawn, 1 drawn, 2 ready, 3 fixtures made) and its sides still to come (-1); the entry bytes +5 of the slots. */
static void log_cup(const char *name, uint32_t cup)
{
    uint32_t t = rd32(g_mem, cup + 0xa3), e = rd32(g_mem, cup + 0xa7);
    int rounds = (int16_t)rd16(g_mem, cup + 0x3c), slots = (int16_t)rd16(g_mem, cup + 0x3e);
    fprintf(stderr, "recomp: uefa log %s day %d: stage %d next %d set %d done %d slots %d", name,
            (int16_t)rd16(g_mem, 0xae2c90u), (int32_t)rd32(g_mem, cup + 0x30), g_mem[cup + 0xab], g_mem[cup + 0xac],
            g_mem[cup + 0xb1], slots);
    for (int r = 0; r < rounds && r < MAX_ROUNDS && readable(t); r++) {
        uint32_t rec = t + ROUND_SIZE * (uint32_t)r;
        int ties = (int16_t)rd16(g_mem, rec + 0x1a), st[4] = {0}, open = 0, empty = 0;
        for (int k = 0; k < ties; k++) {
            uint32_t tie = tie_at(cup, r, k);
            if (g_mem[tie + 0x34] < 4) st[g_mem[tie + 0x34]]++;
            for (int h = 0; h < 2; h++) {
                int16_t side = (int16_t)rd16(g_mem, tie + (h ? 0 : 2));
                if (side < 0) open++;
                else if (readable(e) && !readable(rd32(g_mem, e + ENTRY * (uint32_t)side))) empty++;
            }
        }
        fprintf(stderr, " | r%d code %d draw %d/%d match %d/%d ties %d st %d/%d/%d/%d open %d empty %d", r,
                (int16_t)rd16(g_mem, rec + 7), (int16_t)rd16(g_mem, rec), g_mem[rec + 2], (int16_t)rd16(g_mem, rec + 3),
                g_mem[rec + 5], ties, st[0], st[1], st[2], st[3], open, empty);
    }
    int h[4] = {0};
    for (int i = 0; i < slots && readable(e); i++) {
        int8_t b5 = (int8_t)g_mem[e + ENTRY * (uint32_t)i + 5];
        h[b5 == -1 ? 0 : b5 == 0 ? 1 : b5 == 1 ? 2 : 3]++;
    }
    fprintf(stderr, " | +5: out %d in %d won %d other %d\n", h[0], h[1], h[2], h[3]);
}

static void log_week(void)
{
    static int last = -1;
    int day = (int16_t)rd16(g_mem, 0xae2c90u);
    if (!getenv("RECOMP_UEFA_LOG") || day == last || day % 7) return;
    last = day;
    uint32_t cl = comp_object(KEY_CL, CL_VTABLE), uc = comp_object(KEY_UC, UC_VTABLE);
    if (cl) log_cup("CL", cl);
    if (uc) log_cup("EL", uc);
}

/* Cup::update_for_date, thiscall (slot 5 of the UEFA Cup, 0x970794): the drops before the draws of the day. */
RT_WRAP(0051f890, cup_update_for_date)
{
    uint32_t cup = cpu.ecx;
    if (readable(cup) && rd32(g_mem, cup) == UC_VTABLE && access_row_of(cup)) {
        Cpu saved = cpu;
        log_week();
        uc_fill_drops(cup);
        if (rt_unwinding) return;
        cpu = saved;
    }
    o_0051f890();
}

/* ---- The Intertoto Cup that is not held ----
 *
 * The constructor of Intertoto_Cup (0x632fb0) makes a whole edition at a new game, and build_entrants 0x6336c0
 * shows a box with fewer than 60 clubs (0x6336d7, 0x6337a0). In a season with no Intertoto Cup (uic_clubs -1
 * in the row) the seeding gives none, so the wrap of 0x6336c0 returns 1 with no entrants, the round table
 * keeps its slots empty, and the wrap of its update_for_date 0x633d40 (slot 5, 0x96bba0) does nothing: no
 * draw, no match, no hand-over to the UEFA Cup (0x633deb). Its start_new_season does not run (the before hook
 * of the kind uefa, season_change_skip). */

#define UIC_VTABLE 0x0096bb8cu

static int uic_not_held(uint32_t uic)
{
    if (!readable(uic) || rd32(g_mem, uic) != UIC_VTABLE) return 0;
    const SeasonUefa *r = season_uefa_row((int16_t)rd16(g_mem, uic + 0x40));
    return r && r->uic_clubs < 0;
}

void o_006336c0(void);
void o_00633d40(void);

/* Intertoto_Cup::build_entrants, thiscall () returning 1 or 0. */
RT_WRAP(006336c0, uic_build_entrants)
{
    uint32_t uic = cpu.ecx;
    if (!uic_not_held(uic)) {
        o_006336c0();
        return;
    }
    wr16(g_mem, uic + 0x36, 0);
    rt_ret_cdecl(1);
}

/* Intertoto_Cup::update_for_date, thiscall (2 arguments) ret 8; it returns what Cup::update_for_date 0x51f890
 * returns (0x633e92). */
RT_WRAP(00633d40, uic_update_for_date)
{
    if (!uic_not_held(cpu.ecx)) {
        o_00633d40();
        return;
    }
    rt_ret_stdcall(0, 2);
}

void o_005223a0(void);

/* Cup::rank_teams 0x5223a0, thiscall () (slot 23 of the Intertoto Cup, 0x96bbe8; called by its constructor at
 * 0x633176): it gives every entry slot a band of positions, the first band with no test of an empty slot
 * (0x52254b..0x52256e, comp_stats.cpp 1664 at 0x4a25a8 for a club of 0). An Intertoto Cup that is not held has
 * no clubs, so it ranks none. */
RT_WRAP(005223a0, cup_rank_teams)
{
    if (!uic_not_held(cpu.ecx)) {
        o_005223a0();
        return;
    }
    rt_ret_cdecl(0);
}
