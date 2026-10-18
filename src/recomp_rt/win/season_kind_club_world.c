/* Rule kind "club_world" of the season change (0.6.0 slice 26, part a; season_change.h): the club competitions
 * of the original outside UEFA play an edition only in the seasons that the rules table holds them
 * (docs/research/rules-0.6.0/club-world.tsv). Examples: the Intercontinental Cup to 2004/05 only, the FIFA Club
 * World Championship every year from 2005/06 and not after 2024/25, the Copa Mercosur and the Merconorte Cup
 * in 2001/02 only, the Inter-American Cup never, the Oceania Champions Cup from 2004/05 except 2020/21.
 *
 * Evidence: docs/research/2026-10-18-club-world-idle.md. The method is the state that the original itself uses
 * between two editions of the Club World Championship: the object stays (other code reads it with no null
 * test: the Libertadores writes its winner into the Intercontinental Cup at 0x4c41a0..0x4c41d0, the FA Cup
 * walks the entry 2 array at 0x56fb08), and its season year +0x40 is the year of its next edition. The dates
 * of that edition lie in the future, so the object makes no fixtures (no constructor makes fixtures; a League
 * class draws on its start date, a Cup class on its draw date) and is not complete, so the season pass does
 * not start it again (all_stages_complete 0x48ce10, the pass at 0x5d5ccb). An edition of year Y plays in game
 * season Y for every class here (doc section 6).
 *
 * The two paths (build brief, "Common to slices 15b to 29"):
 *   - a new game of a season game (start year not 2001, or changing rules on): the wraps of the constructors
 *     below give the constructor the year of the first edition from the start season on (thiscall, ret 8:
 *     the year, then the record). The constructor builds every date from that year (doc section 1, point 2).
 *   - with changing rules on, at each season change: the before hook of start_new_season sets +0x40 to the
 *     year of the next edition minus the class's year step, so start_new_season makes that edition.
 * The base game (2001/02, changing rules off) and RECOMP_SEASON_CODE=off get none of it (Q342).
 *
 * A competition that the table never holds again waits for the year NEVER. Its menus still list it (the menus
 * walk the entry arrays, 0x49d156); what its screens show of that year is unknown (doc section 3.2).
 *
 * The year steps (doc section 5): the Club World Championship (+2 at 0x929d9e) and the Oceania Champions Cup
 * (+2 at 0x797dbe) step by 1 in a season game (tools/recomp/seasons.py, rt_club_world_step), and the Club
 * World Championship then draws on 1 July, after the update day of 20 June (rt_club_world_month and
 * rt_club_world_day at 0x929c39 and 0x929c3b): with the original's 5 February the draw date of the next
 * edition is past when start_new_season runs, and the class then never draws again (0x92a73d..0x92a74f). The
 * first edition loop of its constructor (0x92919c..0x9291bd) keeps an even year in a season game
 * (rt_club_world_annual, 0x9291b0). */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_rules.h"

#include <stdio.h>
#include <string.h>

#define YEAR_AT 0x40u                   /* s16 season year of a competition object */
#define NAME_AT 4u                      /* the long name of the competition record (object +4) */
#define NEVER 2999                      /* the year of a competition that the table never holds again */

typedef struct {
    const char *file;                   /* the source file of the class (season_comp_classes) */
    int late;                           /* 1: the pass call does nothing, and the class starts itself later */
} ClubWorldClass;

/* The classes with a season that the table does not hold. con_champ and conmebol_liber play every season.
 * Asian Super Cup: AsiaSuperCup::start_new_season 0x410c50 works only in June (0x410c64), so the call of the
 * pass on 28 August does nothing; the class starts its next edition itself on 25 June (update_for_date
 * 0x410e20, 0x410e34..0x410e72, outside the pass). Its before hook of season S therefore sets the edition of
 * S + 1. */
static const ClubWorldClass classes[] = {
    {"world_club_champ", 0},     /* FIFA Club World Championship, entry 2 */
    {"world_club_cup", 0},       /* Intercontinental Cup, entry 2 */
    {"inter_amer_cup", 0},       /* Inter-American Cup, entry 2 */
    {"con_merc_cup", 0},         /* Merconorte Cup, entry 4 */
    {"conmebol_merc", 0},        /* Copa Mercosur, entry 5 */
    {"asia_club_champ", 0},      /* Asian Club Championship, entry 6 (none in 2022/23) */
    {"asia_cup_winner", 0},      /* Asian Cup Winners' Cup, entry 6 */
    {"asia_super_cup", 1},       /* Asian Super Cup, entry 6 */
    {"oceania_club_champ", 0},   /* Oceania Champions Cup, entry 7 */
};
#define CLASS_COUNT ((int)(sizeof classes / sizeof classes[0]))

static const ClubWorldClass *class_of(const char *file)
{
    for (int i = 0; file && i < CLASS_COUNT; i++)
        if (!strcmp(classes[i].file, file)) return &classes[i];
    return NULL;
}

/* 1 when a game of the season code runs or is made: a start year that is not 2001, or changing rules on. */
static int season_game(void)
{
    return season_code_on() && (season_start_year() != SEASON_FIRST_YEAR || season_changing_rules());
}

/* The row of a class in season s. After the last row of the table the rule stays (Q248): the last row. */
static const SeasonRule *row_of(const char *file, int s)
{
    const SeasonRule *r = NULL;
    for (int y = s; y >= SEASON_RULE_FIRST && !r; y--) r = season_rules_find(y, file, 0);
    return r;
}

/* -1: no row (the kind changes nothing), 1: the table holds the competition in season s, 0: not held. */
static int held(const char *file, int s)
{
    const SeasonRule *r = row_of(file, s);
    if (!r) return -1;
    return r->clubs != SEASON_RULE_NOT_HELD && strcmp(r->format, "not_held") != 0;
}

/* The last season with a row of the file. The table is read once at the start (season_rules_init). */
static int last_row_season(const char *file)
{
    int last = 0;
    for (int y = SEASON_RULE_FIRST; y <= SEASON_RULE_LAST; y++)
        if (season_rules_find(y, file, 0)) last = y;
    return last;
}

/* The year of the next edition from season s on: s when the table holds it, else the next season that holds
 * it, else NEVER. 0 when the table has no row of the class (nothing changes). */
static int next_edition(const char *file, int s)
{
    int h = held(file, s);
    if (h < 0) return 0;
    if (h) return s;
    int last = last_row_season(file);
    for (int y = s + 1; y <= last; y++)
        if (held(file, y) == 1) return y;
    return NEVER;
}

/* ---- The year steps of tools/recomp/seasons.py (rt.h) ---- */

uint32_t rt_club_world_annual(void) { return (uint32_t)season_game(); }
uint32_t rt_club_world_step(uint32_t original) { return season_game() ? 1u : original; }
uint32_t rt_club_world_month(uint32_t original) { return season_game() ? 6u : original; }   /* 0-based: July */
uint32_t rt_club_world_day(uint32_t original) { return season_game() ? 1u : original; }

/* The class's step after the patches above. */
static int step_of(const char *file)
{
    if (!strcmp(file, "world_club_champ") || !strcmp(file, "oceania_club_champ"))
        return (int)rt_club_world_step(2u);
    return 1;
}

/* ---- New game: the constructors ---- */

/* The year argument of a constructor at a new game of a season game: the first edition from the start season
 * on. At a load the creators run too, and the object takes its year from its saved state (League::load
 * 0x687b10 reads +0x40, doc section 2; Cup::save_cup_to_disk 0x5210f0 writes +0x40 at 0x521315, and
 * Cup::load_from_disk 0x51fc00 reads the same fields: inference from the symmetry, not read field by field),
 * so the wraps act only while a new game is made. */
static void ctor(const char *file, void (*original)(void))
{
    if (season_game() && season_change_is_creating()) {
        int start = season_start_year();
        int year = next_edition(file, start);
        uint32_t slot = cpu.esp + 4;                        /* thiscall (year, record), ret 8 */
        uint32_t arg = rd32(g_mem, slot);
        if (year && year != (int16_t)(arg & 0xffffu)) {
            wr32(g_mem, slot, (arg & 0xffff0000u) | ((uint32_t)year & 0xffffu));
            fprintf(stderr, "recomp: %s waits for %d (start season %d)\n", file, year, start);
        }
    }
    original();
}

void o_00929140(void);
void o_0092b4b0(void);
void o_00632080(void);
void o_004c16f0(void);
void o_004c46c0(void);
void o_0040aae0(void);
void o_0040cbe0(void);
void o_00410650(void);
void o_00796fd0(void);

RT_WRAP(00929140, world_club_champ_ctor) { ctor("world_club_champ", o_00929140); }
RT_WRAP(0092b4b0, world_club_cup_ctor) { ctor("world_club_cup", o_0092b4b0); }
RT_WRAP(00632080, inter_amer_cup_ctor) { ctor("inter_amer_cup", o_00632080); }
RT_WRAP(004c16f0, con_merc_cup_ctor) { ctor("con_merc_cup", o_004c16f0); }
RT_WRAP(004c46c0, conmebol_merc_ctor) { ctor("conmebol_merc", o_004c46c0); }
RT_WRAP(0040aae0, asia_club_champ_ctor) { ctor("asia_club_champ", o_0040aae0); }
RT_WRAP(0040cbe0, asia_cup_winner_ctor) { ctor("asia_cup_winner", o_0040cbe0); }
RT_WRAP(00410650, asia_super_cup_ctor) { ctor("asia_super_cup", o_00410650); }
RT_WRAP(00796fd0, oceania_club_champ_ctor) { ctor("oceania_club_champ", o_00796fd0); }

/* ---- The Copa Mercosur's own start ---- */

/* ConmebolMerc::update_for_date 0x4c5c90 calls start_new_season (0x4c5ce4) on 1 June when +0x40 is not the
 * game year (compare 0x4c5cdc, jump 0x4c5cde). A waiting object has a later year, so each 1 June would add 1
 * again. tools/recomp/seasons.py makes the jump at 0x4c5cde ask this function too: 1 skips the call. */
uint32_t rt_club_world_waits(uint32_t obj)
{
    if (!season_game()) return 0;
    int year = (int16_t)rd16(g_mem, obj + YEAR_AT);
    int now = (int16_t)rd16(g_mem, 0xae2c92u);              /* the game year */
    return year > now;
}

/* ---- The season change ---- */

static const char *name_of(uint32_t comp, char *buf, size_t n)
{
    uint32_t rec = rd32(g_mem, comp + NAME_AT);
    snprintf(buf, n, "%.50s", rec >= 0x10000u && rec < 0xfff00000u ? win_str(rec + NAME_AT) : "");
    return buf;
}

static void club_world_before(const SeasonChange *c)
{
    if (!season_game()) return;                             /* never in the base game (Q152, Q342) */
    const ClubWorldClass *k = class_of(c->file);
    if (!k || !c->started) return;
    /* The start season hooks run with changing rules off too (season_change.h, "The start season"). A late
     * class sets the edition of the next season there, which is a change only with changing rules on. */
    if (k->late && c->start && !season_changing_rules()) return;
    int s = c->season + k->late;                            /* the season of the edition that starts next */
    int year = next_edition(k->file, s);
    if (!year) return;
    int want = year - step_of(k->file);
    int now = (int16_t)rd16(g_mem, c->comp + YEAR_AT);
    if (want == now) return;
    wr16(g_mem, c->comp + YEAR_AT, (uint16_t)want);
    fprintf(stderr, "recomp: %s in season %d waits for %d (+0x40 %d to %d)\n", k->file, c->season, year, now, want);
}

/* One sentence when the table starts or stops holding the competition in the season of the change (Q258). It
 * runs for a competition that starts and for one that waits (started 0), so a competition that comes back
 * after a gap gets its sentence too. */
static void club_world_after(const SeasonChange *c)
{
    if (!season_game()) return;                             /* never in the base game (Q152, Q342) */
    const ClubWorldClass *k = class_of(c->file);
    if (!k) return;
    int s = c->season;
    int now = held(k->file, s), before = held(k->file, s - 1);
    if (now < 0 || before < 0 || now == before) return;
    char name[64];
    name_of(c->comp, name, sizeof name);
    if (!*name) return;
    if (now)
        season_change_news("The %s is played again from the %d/%02d season.", name, s, (s + 1) % 100);
    else if (next_edition(k->file, s) == NEVER)
        season_change_news("The %s is no longer played.", name);
    else
        season_change_news("The %s is not played in the %d/%02d season.", name, s, (s + 1) % 100);
}

const SeasonRuleKind season_kind_club_world = {"club_world", NULL, NULL, NULL, NULL, club_world_before,
                                               club_world_after};

/* ---- Part b: the knockout of the FIFA Club World Championship (Form K) ----
 *
 * Evidence: docs/research/2026-10-18-club-world-formats.md. The original plays 3 groups of 4 and a knockout of 4
 * (12 clubs). The table gives a knockout of 6 clubs (2005/06, 2006/07, 2020/21) or 7 clubs (2007/08 to 2023/24,
 * not 2020/21): a play-off of the host champion against the OFC winner, quarter-finals, the UEFA and CONMEBOL
 * winners in the semi-finals, the final and the third place match. In a season game the start date branch of
 * WorldClubChamp::update_for_date 0x92a6b0 makes, in place of the groups, one Cup_Stage (0x522e00) in stage
 * slot 0 with stage index 0: League::add_result gives a result to the stage slot of the fixture's stage index
 * (0x685dd0..0x685dfa), and the parent updates the stages from slot 0 to +0x30 with no null test (0x6848e5).
 * The three slots that test the stage index for the knockout (2) get 2 for this stage: staff records 0x929c70,
 * fate news 0x9298a0, progress codes 0x92ad50. Clubs that enter in a later round are byes of the round table (+0x1c new entrants
 * from club list index +0x1e, map_fixture_tree 0x519265..0x519504). The third place match is the last round
 * (round id 140: Cup::process_fixture puts the semi-final losers there, 0x51b129..0x51b158). No fifth place
 * match: no class of the original has one (doc 3.4).
 *
 * The parent League holds no club: its club count +0x3e is 0 and its table +0xb1 one zeroed row, because the
 * parent ends its season only with a table (0x68495e, League::check_complete 0x689add) and League::update_for_date
 * reads row 0 (0x68484e). The history thunk 0x40f590 forwards to stage slot 2; for this form it forwards to
 * slot 0. The matches keep the original's weeks (late July and August), so the fixture checks are as before. */

#define CWC_VTABLE 0x97119cu
#define CWC_STAGES 0xcu                 /* stage pointer array (3 slots) */
#define CWC_STAGE_COUNT 0x2cu           /* s32 */
#define CWC_STAGE_NOW 0x30u             /* s32, -1 at a new edition */
#define CWC_CLUBS 0x3eu                 /* s16 */
#define CWC_DRAWN 0x45u                 /* s32 */
#define CWC_TABLE 0xb1u                 /* the parent's own table, rows of 0x3b bytes */
#define CWC_TABLE_B5 0xb5u              /* 0 for check_complete (0x689ae9) */
#define CWC_START_DAY 0xb6u             /* s16 day of the year */
#define CWC_START_OFFSET 0xb8u          /* s16 years after +0x40 */
#define CWC_PICKS 0xeeu                 /* Club *[12] of pick_entrants 0x92a8b0 */
#define CWC_PICK_COUNT 0xf2u
#define ROUND_SIZE 0x68u
#define TABLE_ROW 0x3bu

/* The places of pick_entrants 0x92a8b0 (calls 0x92a9f2..0x92ab7c): Africa 0, 1; Asia 2, 3; Europe 4 (the
 * Champions League), 5 (the UEFA Cup); North America 6, 7; Oceania 8; South America 9, 10; host 11. */
enum { PICK_CAF = 0, PICK_AFC = 2, PICK_UEFA = 4, PICK_CONCACAF = 6, PICK_OFC = 8, PICK_CONMEBOL = 9, PICK_HOST = 11 };

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

/* The club count of the knockout in the edition of year `year`: 6 or 7, or 0 for the original form. With
 * changing rules off the form of the first edition of the game stays (Q232). */
static int knockout_clubs(int year, int *host_in_place_of_ofc)
{
    *host_in_place_of_ofc = 0;
    if (!season_game()) return 0;
    int s = season_changing_rules() ? year : next_edition("world_club_champ", season_start_year());
    const SeasonRule *r = s > 0 ? row_of("world_club_champ", s) : NULL;
    if (!r || (r->clubs != 6 && r->clubs != 7)) return 0;
    /* 2020/21: the OFC winner withdrew, so the host champion plays in the quarter-finals (club-world.tsv,
     * row club-world-363). */
    if (r->clubs == 6 && r->season == 2020) *host_in_place_of_ofc = 1;
    return r->clubs;
}

/* The parent: no club, one zeroed table row (see above). Returns 0 when it cannot. */
static int parent_table(uint32_t obj)
{
    uint32_t sz[1] = {TABLE_ROW};
    uint32_t row = call_cdecl(0x944cf1u, 1, sz);
    if (rt_unwinding || !row) return 0;
    memset(g_mem + row, 0, TABLE_ROW);
    wr16(g_mem, obj + CWC_CLUBS, 0);
    wr32(g_mem, obj + CWC_TABLE, row);
    g_mem[obj + CWC_TABLE_B5] = 0;
    return 1;
}

/* Builds the knockout on the start date. Returns 1 when the object now holds Form K. */
static int build_knockout(uint32_t obj, int clubs, int host_for_ofc)
{
    call_this(0x92a8b0u, obj, 0, NULL);                         /* pick_entrants: 12 clubs into +0xee */
    if (rt_unwinding) return 0;
    uint32_t picks = rd32(g_mem, obj + CWC_PICKS);
    if (!picks || (int32_t)rd32(g_mem, obj + CWC_PICK_COUNT) < 12) return 0;
    /* The club list in round order: the clubs of round 0, then each later round's new entrants. */
    int order7[7] = {PICK_HOST, PICK_OFC, PICK_AFC, PICK_CAF, PICK_CONCACAF, PICK_UEFA, PICK_CONMEBOL};
    int order6[6] = {PICK_AFC, PICK_CAF, PICK_CONCACAF, PICK_OFC, PICK_UEFA, PICK_CONMEBOL};
    int *order = clubs == 7 ? order7 : order6;
    if (clubs == 6 && host_for_ofc) order6[3] = PICK_HOST;
    static uint32_t list;
    if (!list) list = win_static_alloc(4u * 8u);
    for (int i = 0; i < clubs; i++) {
        uint32_t club = rd32(g_mem, picks + 4u * (uint32_t)order[i]);
        if (!club) return 0;                                    /* the original form then plays */
        wr32(g_mem, list + 4u * (uint32_t)i, club);
    }
    uint32_t b[1] = {picks};
    call_cdecl(0x9452cau, 1, b);                                /* as the original frees it, 0x92a869 */
    wr32(g_mem, obj + CWC_PICKS, 0);
    wr32(g_mem, obj + CWC_PICK_COUNT, 0);

    /* The rounds, from the original's knockout table (create_schedule 0x929390, stage 2: semi-final, final,
     * third place, 0x929705..0x929884) as templates. Days move from the semi-final (draw 7 August, match
     * 9 August): the play-off 8 days earlier, the quarter-finals 4 days earlier. */
    static uint32_t out;                                        /* rounds, code, is_league */
    if (!out) out = win_static_alloc(12);
    uint32_t a[4] = {2, out, out + 4, out + 8};
    uint32_t orig = call_this(0x929390u, obj, 4, a);
    if (rt_unwinding || !orig || (int16_t)rd16(g_mem, out) != 3) return 0;
    static uint32_t table;
    if (!table) table = win_static_alloc(5u * ROUND_SIZE);
    int n = 0;
    struct { int tmpl, id, teams, ties, fresh, first, days; } rounds[5];
    if (clubs == 7) {
        rounds[n++] = (typeof(rounds[0])){0, 0x14, 2, 1, 2, 0, -8};    /* play-off */
        rounds[n++] = (typeof(rounds[0])){0, 0x78, 4, 2, 3, 2, -4};    /* quarter-finals */
        rounds[n++] = (typeof(rounds[0])){0, 0x82, 4, 2, 2, 5, 0};     /* semi-finals */
    } else {
        rounds[n++] = (typeof(rounds[0])){0, 0x78, 4, 2, 4, 0, -4};
        rounds[n++] = (typeof(rounds[0])){0, 0x82, 4, 2, 2, 4, 0};
    }
    rounds[n++] = (typeof(rounds[0])){1, 0x96, 2, 1, 0, 0, 0};        /* final */
    rounds[n++] = (typeof(rounds[0])){2, 0x8c, 2, 1, 0, 0, 0};        /* third place */
    for (int i = 0; i < n; i++) {
        uint32_t r = table + (uint32_t)i * ROUND_SIZE;
        memcpy(g_mem + r, g_mem + orig + (uint32_t)rounds[i].tmpl * ROUND_SIZE, ROUND_SIZE);
        wr16(g_mem, r + 0x00, rd16(g_mem, r + 0x00) + (uint32_t)rounds[i].days);   /* draw day of the year */
        wr16(g_mem, r + 0x03, rd16(g_mem, r + 0x03) + (uint32_t)rounds[i].days);   /* match day of the year */
        wr16(g_mem, r + 0x07, (uint32_t)rounds[i].id);
        wr16(g_mem, r + 0x09, (uint32_t)i);
        g_mem[r + 0x0d] = 0x03;                                 /* extra time and penalties, no golden goal */
        wr16(g_mem, r + 0x18, (uint32_t)rounds[i].teams);
        wr16(g_mem, r + 0x1a, (uint32_t)rounds[i].ties);
        wr16(g_mem, r + 0x1c, (uint32_t)rounds[i].fresh);
        wr16(g_mem, r + 0x1e, (uint32_t)rounds[i].first);
    }
    uint32_t code = rd32(g_mem, out + 4);
    uint32_t f[1] = {orig};
    call_cdecl(0x9452cau, 1, f);                                /* as the original frees it, 0x92a38b */
    if (rt_unwinding) return 0;

    if (!parent_table(obj)) return 0;

    /* The stage, as 0x92a326..0x92a370 makes the original's knockout. */
    uint32_t sz2[1] = {0xb2};
    uint32_t stage = call_cdecl(0x944cf1u, 1, sz2);
    if (rt_unwinding || !stage) return 0;
    uint32_t args[15] = {obj, (uint32_t)clubs, list, (uint32_t)n, rd32(g_mem, obj + 4), table,
                         (uint32_t)(int16_t)rd16(g_mem, obj + YEAR_AT), 0, 2, code, 0x14, 0, 0, 0, 0};
    call_this(0x522e00u, stage, 15, args);
    if (rt_unwinding) return 0;
    wr32(g_mem, rd32(g_mem, obj + CWC_STAGES), stage);          /* slot 0 */
    wr32(g_mem, obj + CWC_STAGE_COUNT, 1);
    uint32_t r0[1] = {0};
    call_this(0x51c800u, stage, 1, r0);                         /* set_up_round(0), as 0x92a414 */
    if (rt_unwinding) return 0;
    wr32(g_mem, obj + CWC_STAGE_NOW, 0);
    wr32(g_mem, obj + CWC_DRAWN, 1);                            /* the original's start branch now skips */
    fprintf(stderr, "recomp: club world knockout of %d clubs for %d\n", clubs,
            (int16_t)rd16(g_mem, obj + YEAR_AT));
    return 1;
}

/* Back to the original form (3 stage slots). After a load of a Form K game the stage array has 1 slot
 * (League::load 0x687b10 makes it with the saved +0x2c, 0x688e3a..0x688e5b, and the constructor skips its own
 * array after a load, 0x9291d1), so the object gets a new array of 3 slots from the operator new of the
 * constructor (0x944e46, as 0x9291e9) and the old one goes back with free (as the destructor, 0x929349). */
static void original_form(uint32_t obj)
{
    uint32_t sz[1] = {12};
    uint32_t arr = call_cdecl(0x944e46u, 1, sz);
    if (rt_unwinding || !arr) return;
    memset(g_mem + arr, 0, 12);
    uint32_t old[1] = {rd32(g_mem, obj + CWC_STAGES)};
    wr32(g_mem, obj + CWC_STAGES, arr);
    wr32(g_mem, obj + CWC_STAGE_COUNT, 3);
    if (old[0]) call_cdecl(0x9452cau, 1, old);
}

void o_0092a6b0(void);

/* WorldClubChamp::update_for_date, thiscall (Date *today, int arg2), ret 8. Date: s16 day of the year, s16 year.
 * The start date test is that of 0x92a73d..0x92a765. */
RT_WRAP(0092a6b0, world_club_champ_update)
{
    uint32_t obj = cpu.ecx, today = rt_arg(0);
    int host_for_ofc;
    int year = (int16_t)rd16(g_mem, obj + YEAR_AT);
    int clubs = knockout_clubs(year, &host_for_ofc);
    if (clubs && rd32(g_mem, obj) == CWC_VTABLE && !rd32(g_mem, obj + CWC_DRAWN) && rt_arg(1) == 0) {
        int day = (int16_t)rd16(g_mem, obj + CWC_START_DAY);
        int start_year = year + (int16_t)rd16(g_mem, obj + CWC_START_OFFSET);
        if ((int16_t)rd16(g_mem, today) == day && (int16_t)rd16(g_mem, today + 2) == start_year) {
            Cpu saved = cpu;
            int ok = build_knockout(obj, clubs, host_for_ofc);
            if (rt_unwinding) return;
            cpu = saved;
            if (!ok && rd32(g_mem, obj + CWC_STAGE_COUNT) == 1) original_form(obj);
        }
    } else if (rd32(g_mem, obj) == CWC_VTABLE && rd32(g_mem, obj + CWC_STAGE_COUNT) == 1 &&
               rd32(g_mem, obj + CWC_DRAWN) && !rd32(g_mem, obj + CWC_TABLE)) {
        /* A loaded Form K game: the load makes no table for a parent of 0 clubs (inference from this run: the
         * loaded knockout did not end), and without a table the parent never ends (0x68495e). */
        Cpu saved = cpu;
        int ok = parent_table(obj);
        if (rt_unwinding) return;
        cpu = saved;
        fprintf(stderr, "recomp: club world knockout %d: the parent table again after a load (%s)\n", year,
                ok ? "made" : "failed");
    } else if (clubs == 0 && rd32(g_mem, obj) == CWC_VTABLE && rd32(g_mem, obj + CWC_STAGE_COUNT) == 1 &&
               !rd32(g_mem, obj + CWC_DRAWN)) {
        original_form(obj);                                     /* a later edition of the original form */
    }
    o_0092a6b0();
}

/* The history thunk of slot 12 (vtable +0x30), shared by 4 vtables: it forwards to stage slot 2 (0x40f593). A
 * Club World Championship of Form K holds its knockout in slot 0. Slice 27 wraps the same function for the Copa
 * America of 16 (season_intl_copa.c); its wrap calls this first (merge of slices 26 and 27 in slice 28). Returns
 * 1 when it forwarded the call. */
int season_club_world_history_thunk(void)
{
    uint32_t obj = cpu.ecx;
    if (rd32(g_mem, obj) == CWC_VTABLE && rd32(g_mem, obj + CWC_STAGE_COUNT) == 1) {   /* Form K */
        uint32_t stage = rd32(g_mem, rd32(g_mem, obj + CWC_STAGES));
        if (stage) {
            /* Test log: the clubs with fates 1 to 3 of the club list +0xa7 (6 bytes a club: pointer, seed, fate;
             * Cup slot 12 0x519aca..0x519aeb reads them), count +0x3e. The club's name is at +0x04 (51 bytes). */
            uint32_t list = rd32(g_mem, stage + 0xa7);
            int n = (int16_t)rd16(g_mem, stage + 0x3e);
            for (int i = 0; list && i < n && i < 8; i++) {
                uint32_t e = list + 6u * (uint32_t)i, club = rd32(g_mem, e);
                int fate = g_mem[e + 5];
                if (club && fate >= 1 && fate <= 3)
                    fprintf(stderr, "recomp: club world knockout %d: place %d %.50s\n",
                            (int16_t)rd16(g_mem, obj + YEAR_AT), fate, win_str(club + 4));
            }
            cpu.ecx = stage;
            rt_jump(rd32(g_mem, rd32(g_mem, stage) + 0x30));
            return 1;
        }
    }
    return 0;
}

/* Form K: 1 for a Club World Championship whose knockout is its only stage. */
static int is_knockout(uint32_t obj)
{
    return rd32(g_mem, obj) == CWC_VTABLE && rd32(g_mem, obj + CWC_STAGE_COUNT) == 1;
}

/* The stage index argument (a byte) at esp + 4 + 4 * arg: 0 becomes 2 for Form K. thiscall, so the callee pops
 * the argument and its slot needs no restore. */
static void stage_as_knockout(int arg)
{
    uint32_t slot = cpu.esp + 4u + 4u * (uint32_t)arg;
    if (is_knockout(cpu.ecx) && g_mem[slot] == 0) g_mem[slot] = 2;
}

void o_00929c70(void);
void o_009298a0(void);
void o_0092ad50(void);

/* WorldClubChamp::update_staff_records_for_status (club, fate, stage, ...), ret 0x18: stage < 2 at 0x929c74. */
RT_WRAP(00929c70, world_club_champ_staff_records) { stage_as_knockout(2); o_00929c70(); }
/* WorldClubChamp::build_fate_news_text (club index, fate, stage, ...), ret 0x28: stage < 2 at 0x9298a6. */
RT_WRAP(009298a0, world_club_champ_fate_news) { stage_as_knockout(2); o_009298a0(); }
/* WorldClubChamp::set_club_progress (club, stage, ...), ret 0x14: stage < 2 at 0x92ad5e. */
RT_WRAP(0092ad50, world_club_champ_progress) { stage_as_knockout(1); o_0092ad50(); }
