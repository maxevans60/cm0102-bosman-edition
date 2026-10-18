/* Rule kind "cup_breaks" of the season change (season_change.h): the domestic cups of the original play an
 * edition only in the seasons that the rules table holds them (0.6.0 slice 21b, docs/research/
 * 2026-10-18-cup-breaks.md). Examples: the Coupe de la Ligue is not held from 2020/21, the Welsh Premier Cup from
 * 2008/09, the Norwegian Cup in 2020, the Leinster Cup from 2002/03 to 2008/09.
 *
 * The edition of a cup is its season year (+0x40). Every class below adds 1 to +0x40 in start_new_season before
 * it builds the rules, the round table and the dates of the new edition (FraLgeCup 0x5bd4be, WelPrmCup 0x928261,
 * NorCup 0x78fc2e, and the others in the table), and none of them calls start_new_season outside the season pass
 * (no call of vtable +0x8 in their files; the pass call is at 0x5d5d1c). season_cups.h maps an edition to its row.
 *
 * In a game (changing rules on, or the start season): in the before hook of start_new_season of season S, a cup
 * whose edition S the table does not hold calls season_change_skip(). The cup keeps its finished edition, makes
 * no fixtures and plays no matches (docs/research/2026-10-18-club-world-idle.md, section 3.2: all_stages_complete
 * stays true, the season-end work runs once). The pass asks again on the next update day, and the kind skips
 * again while the table does not hold the cup. A cup that the table holds again resumes: its year is the
 * edition before the gap, so the hook sets +0x40 to S - 1, and start_new_season builds edition S with dates in
 * the future. Why a skip and not a wait (+0x40 at the next edition, as season_kind_club_world.c does): six of
 * these classes make the fixtures of the new edition inside start_new_season (League::create_fixtures 0x6835c0
 * called at 0x9282f8 by WelPrmCup, and by KorLeagueCup, IreLgeCup, ItaCCup, NirLgeCup and WelLgeCup), and every
 * class picks its clubs there (FraLgeCup 0x5bd4d0). A wait would put the fixtures of the next edition into the
 * clubs' fixture lists a season or more early, with clubs of the season before.
 *
 * At a new game of a season game (start year not 2001; a 2001 game keeps the original's 2001/02): the
 * constructor builds the first edition from its year argument (thiscall: year, record; ret 8, every constructor
 * below). When the table does not hold that edition, the wrap gives the constructor the year of the next edition
 * that it holds, so the cup waits (its dates lie in the future; the club world doc, section 1). A cup that the
 * table never holds again gets SEASON_CUPS_NEVER, and the League::create_fixtures call of its constructor makes
 * no fixtures, so no fixture of the year 2999 reaches a club's list. Such a cup never completes, so the pass never
 * starts it again.
 *
 * The base game (2001/02, changing rules off) and RECOMP_SEASON_CODE=off get none of it (Q152, Q342): no hook
 * runs there, and the wraps test season_game(). RECOMP_CUPS=off turns this kind off too (a test switch of
 * slice 21). */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_cups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YEAR_AT 0x40u                   /* s16 season year of a competition object */
#define NAME_AT 4u                      /* the long name of the competition record (object +4) */

/* The classes with a season that the table does not hold (rules table rows with not_held, comp_file of the
 * original, tier cup or super cup) and their constructors. The note says the base class and whether the
 * constructor makes fixtures itself ("with fixtures": a call of League::create_fixtures 0x6835c0 inside the
 * constructor, for example 0x927970 in WelPrmCup), and the editions that the table does not hold. */
typedef struct {
    const char *file;
    uint32_t ctor;
} BreakClass;

static const BreakClass classes[] = {
    {"bel_super", 0x420ef0u},           /* Cup, not held 2020/21 */
    {"bra_champ_cup", 0x4259a0u},       /* League, from 2003 */
    {"fra_lge_cup", 0x5bc980u},         /* Cup, from 2020/21 */
    {"ger_lge_cup", 0x5dc0e0u},         /* Cup, from 2008/09 */
    {"gre_super", 0x5ec040u},           /* Cup, held 2007/08 and from 2025/26 */
    {"hol_super", 0x5f8820u},           /* Cup, not held 2020/21 */
    {"ire_leinster_cup", 0x6364a0u},    /* Cup, 2002/03 to 2008/09 and 2019/20 to 2021/22 */
    {"ire_lge_cup", 0x636dd0u},         /* League with fixtures, 2002/03 and from 2020/21 */
    {"ire_munster_cup", 0x6387a0u},     /* Cup, 2020/21 */
    {"ire_pres_cup", 0x639280u},        /* Cup, 2003/04 to 2012/13 and 2019/20 */
    {"ire_super_cup", 0x63acd0u},       /* League, from 2002/03 */
    {"ita_c1_super", 0x63c5e0u},        /* Cup, 2019/20 */
    {"ita_c_cup", 0x63cc40u},           /* League with fixtures, 2020/21 */
    {"jap_emp_cup", 0x661d30u},         /* Cup, 2026 */
    {"jap_j_cup", 0x665380u},           /* Cup, 2026 */
    {"jap_super", 0x665d70u},           /* Cup, 2026 */
    {"kor_league_cup", 0x66bf00u},      /* League with fixtures, 2003 and from 2012 */
    {"kor_super_cup", 0x66d820u},       /* Cup, 2003 and 2007 to 2025 */
    {"nir_charity", 0x789df0u},         /* Cup, to 2013/14 and 2018/19 to 2021/22 */
    {"nir_lge_cup", 0x78c780u},         /* League with fixtures, 2020/21 */
    {"nor_cup", 0x78f020u},             /* Cup, 2020 */
    {"pol_lge_cup", 0x7ca4e0u},         /* Cup, 2002/03 to 2005/06 and from 2009/10 */
    {"pol_super", 0x7cc920u},           /* Cup, 2002/03, 2003/04, 2005/06, 2011/12, 2013/14 */
    {"sco_chal_cup", 0x7ec980u},        /* Cup, 2020/21 */
    {"swe_cup", 0x88cbb0u},             /* Cup, 2012 */
    {"usa_open_cup", 0x90aeb0u},        /* Cup, 2020 and 2021 */
    {"wel_cup", 0x924420u},             /* Cup, 2020/21 */
    {"wel_lge_cup", 0x925e90u},         /* League with fixtures, 2020/21 */
    {"wel_prm_cup", 0x9277f0u},         /* League with fixtures, from 2008/09 */
};
#define CLASS_COUNT ((int)(sizeof classes / sizeof classes[0]))

static const BreakClass *class_of(const char *file)
{
    for (int i = 0; file && i < CLASS_COUNT; i++)
        if (!strcmp(classes[i].file, file)) return &classes[i];
    return NULL;
}

static int logging(void)
{
    const char *v = getenv("RECOMP_SEASON_CHANGE_LOG");
    return v && *v;
}

/* RECOMP_CUPS=off: the test switch of the cup kinds (season_kind_cups.c). */
static int cups_off(void)
{
    const char *v = getenv("RECOMP_CUPS");
    return v && !strcmp(v, "off");
}

/* 1 when a game of the season code runs or is made: a start year that is not 2001, or changing rules on. */
static int season_game(void)
{
    return season_code_on() && !cups_off() && (season_start_year() != SEASON_FIRST_YEAR || season_changing_rules());
}

/* ---- New game: the constructors ---- */

static uint32_t never_obj;              /* the object whose constructor runs with SEASON_CUPS_NEVER */

static void ctor(const char *file, void (*original)(void))
{
    uint32_t self = cpu.ecx;
    if (season_game() && season_change_is_creating() && season_start_year() != SEASON_FIRST_YEAR) {
        uint32_t slot = cpu.esp + 4;                        /* thiscall (year, record), ret 8 */
        uint32_t arg = rd32(g_mem, slot);
        int year = (int16_t)(arg & 0xffffu);
        int next = season_cups_next(file, year);
        if (next && next != year) {
            wr32(g_mem, slot, (arg & 0xffff0000u) | ((uint32_t)next & 0xffffu));
            if (next == SEASON_CUPS_NEVER) never_obj = self;
            fprintf(stderr, "recomp: cup_breaks: %s waits for %d (edition %d not held, start season %d)\n", file,
                    next, year, season_start_year());
        }
    }
    original();
    never_obj = 0;
}

void o_00420ef0(void);
void o_004259a0(void);
void o_005bc980(void);
void o_005dc0e0(void);
void o_005ec040(void);
void o_005f8820(void);
void o_006364a0(void);
void o_00636dd0(void);
void o_006387a0(void);
void o_00639280(void);
void o_0063acd0(void);
void o_0063c5e0(void);
void o_0063cc40(void);
void o_00661d30(void);
void o_00665380(void);
void o_00665d70(void);
void o_0066bf00(void);
void o_0066d820(void);
void o_00789df0(void);
void o_0078c780(void);
void o_0078f020(void);
void o_007ca4e0(void);
void o_007cc920(void);
void o_007ec980(void);
void o_0088cbb0(void);
void o_0090aeb0(void);
void o_00924420(void);
void o_00925e90(void);
void o_009277f0(void);

/* The recompiler reads the RT_WRAP lines as they are, so each is written out. */
RT_WRAP(00420ef0, bel_super_ctor) { ctor("bel_super", o_00420ef0); }
RT_WRAP(004259a0, bra_champ_cup_ctor) { ctor("bra_champ_cup", o_004259a0); }
RT_WRAP(005bc980, fra_lge_cup_ctor) { ctor("fra_lge_cup", o_005bc980); }
RT_WRAP(005dc0e0, ger_lge_cup_ctor) { ctor("ger_lge_cup", o_005dc0e0); }
RT_WRAP(005ec040, gre_super_ctor) { ctor("gre_super", o_005ec040); }
RT_WRAP(005f8820, hol_super_ctor) { ctor("hol_super", o_005f8820); }
RT_WRAP(006364a0, ire_leinster_cup_ctor) { ctor("ire_leinster_cup", o_006364a0); }
RT_WRAP(00636dd0, ire_lge_cup_ctor) { ctor("ire_lge_cup", o_00636dd0); }
RT_WRAP(006387a0, ire_munster_cup_ctor) { ctor("ire_munster_cup", o_006387a0); }
RT_WRAP(00639280, ire_pres_cup_ctor) { ctor("ire_pres_cup", o_00639280); }
RT_WRAP(0063acd0, ire_super_cup_ctor) { ctor("ire_super_cup", o_0063acd0); }
RT_WRAP(0063c5e0, ita_c1_super_ctor) { ctor("ita_c1_super", o_0063c5e0); }
RT_WRAP(0063cc40, ita_c_cup_ctor) { ctor("ita_c_cup", o_0063cc40); }
RT_WRAP(00661d30, jap_emp_cup_ctor) { ctor("jap_emp_cup", o_00661d30); }
RT_WRAP(00665380, jap_j_cup_ctor) { ctor("jap_j_cup", o_00665380); }
RT_WRAP(00665d70, jap_super_ctor) { ctor("jap_super", o_00665d70); }
RT_WRAP(0066bf00, kor_league_cup_ctor) { ctor("kor_league_cup", o_0066bf00); }
RT_WRAP(0066d820, kor_super_cup_ctor) { ctor("kor_super_cup", o_0066d820); }
RT_WRAP(00789df0, nir_charity_ctor) { ctor("nir_charity", o_00789df0); }
RT_WRAP(0078c780, nir_lge_cup_ctor) { ctor("nir_lge_cup", o_0078c780); }
RT_WRAP(0078f020, nor_cup_ctor) { ctor("nor_cup", o_0078f020); }
RT_WRAP(007ca4e0, pol_lge_cup_ctor) { ctor("pol_lge_cup", o_007ca4e0); }
RT_WRAP(007cc920, pol_super_ctor) { ctor("pol_super", o_007cc920); }
RT_WRAP(007ec980, sco_chal_cup_ctor) { ctor("sco_chal_cup", o_007ec980); }
RT_WRAP(0088cbb0, swe_cup_ctor) { ctor("swe_cup", o_0088cbb0); }
RT_WRAP(0090aeb0, usa_open_cup_ctor) { ctor("usa_open_cup", o_0090aeb0); }
RT_WRAP(00924420, wel_cup_ctor) { ctor("wel_cup", o_00924420); }
RT_WRAP(00925e90, wel_lge_cup_ctor) { ctor("wel_lge_cup", o_00925e90); }
RT_WRAP(009277f0, wel_prm_cup_ctor) { ctor("wel_prm_cup", o_009277f0); }

/* League::create_fixtures 0x6835c0 (thiscall, no arguments, ret at 0x683edf; returns non-zero on success, which
 * the callers test, for example 0x927975). For the object of a constructor that runs with SEASON_CUPS_NEVER it
 * makes no fixture and returns 1. Every other call runs the original. */
void o_006835c0(void);
RT_WRAP(006835c0, cup_breaks_create_fixtures)
{
    if (never_obj && cpu.ecx == never_obj) {
        if (logging()) fprintf(stderr, "recomp: cup_breaks: no fixtures for the cup that waits for ever (0x%x)\n",
                               cpu.ecx);
        rt_ret_stdcall(1, 0);
        return;
    }
    o_006835c0();
}

/* ---- The season change ---- */

static const char *name_of(uint32_t comp, char *buf, size_t n)
{
    uint32_t rec = rd32(g_mem, comp + NAME_AT);
    snprintf(buf, n, "%.50s", rec >= 0x10000u && rec < 0xfff00000u ? win_str(rec + NAME_AT) : "");
    return buf;
}

/* The edition that starts in the season of the change: the season S of the hook (season_change.h). For every
 * class here S is the year that start_new_season gives the cup (+0x40 + 1) when the cup did not skip a season
 * (log line "starts" of RECOMP_SEASON_CHANGE_LOG, docs/research/2026-10-18-cup-breaks.md). */
static void breaks_before(const SeasonChange *c)
{
    if (!season_game() || !c->started) return;
    const BreakClass *k = class_of(c->file);
    if (!k) return;
    int edition = c->season;
    int held = season_cups_held(k->file, edition);
    int year = (int16_t)rd16(g_mem, c->comp + YEAR_AT);
    if (logging())
        fprintf(stderr, "recomp: cup_breaks: %s season %d, year %d, held %d\n", k->file, edition, year, held);
    if (held < 0) return;
    if (!held) {
        season_change_skip();
        fprintf(stderr, "recomp: cup_breaks: %s is not held in %d (year stays %d)\n", k->file, edition, year);
        return;
    }
    if (year + 1 < edition) {                               /* it skipped a season before: resume in step */
        wr16(g_mem, c->comp + YEAR_AT, (uint16_t)(edition - 1));
        fprintf(stderr, "recomp: cup_breaks: %s resumes in %d (year %d to %d)\n", k->file, edition, year,
                edition - 1);
    }
}

/* One sentence when the table starts or stops holding the cup in the season of the change (Q258). It runs for a
 * cup that starts and for one that skips or waits (started 0), so a cup that comes back gets its sentence too. */
static void breaks_after(const SeasonChange *c)
{
    if (!season_game()) return;
    const BreakClass *k = class_of(c->file);
    if (!k) return;
    int s = c->season;
    int now = season_cups_held(k->file, s), before = season_cups_held(k->file, s - 1);
    if (now < 0 || before < 0 || now == before) return;
    char name[64], when[32];
    name_of(c->comp, name, sizeof name);
    if (!*name) return;
    if (season_cups_calendar(k->file)) snprintf(when, sizeof when, "in %d", s);
    else snprintf(when, sizeof when, "in the %d/%02d season", s, (s + 1) % 100);
    if (now)
        season_change_news("The %s is played again %s.", name, when);
    else if (season_cups_next(k->file, s) == SEASON_CUPS_NEVER)
        season_change_news("The %s is no longer played.", name);
    else
        season_change_news("The %s is not played %s.", name, when);
}

const SeasonRuleKind season_kind_cup_breaks = {"cup_breaks", NULL, NULL, NULL, NULL, breaks_before, breaks_after};
