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
    uint32_t ctor;                      /* its constructor */
    int late;                           /* 1: the pass call does nothing, and the class starts itself later */
} ClubWorldClass;

/* The classes with a season that the table does not hold. con_champ and conmebol_liber play every season.
 * Asian Super Cup: AsiaSuperCup::start_new_season 0x410c50 works only in June (0x410c64), so the call of the
 * pass on 28 August does nothing; the class starts its next edition itself on 25 June (update_for_date
 * 0x410e20, 0x410e34..0x410e72, outside the pass). Its before hook of season S therefore sets the edition of
 * S + 1. */
static const ClubWorldClass classes[] = {
    {"world_club_champ", 0x929140u, 0},     /* FIFA Club World Championship, entry 2 */
    {"world_club_cup", 0x92b4b0u, 0},       /* Intercontinental Cup, entry 2 */
    {"inter_amer_cup", 0x632080u, 0},       /* Inter-American Cup, entry 2 */
    {"con_merc_cup", 0x4c16f0u, 0},         /* Merconorte Cup, entry 4 */
    {"conmebol_merc", 0x4c46c0u, 0},        /* Copa Mercosur, entry 5 */
    {"asia_club_champ", 0x40aae0u, 0},      /* Asian Club Championship, entry 6 (none in 2022/23) */
    {"asia_cup_winner", 0x40cbe0u, 0},      /* Asian Cup Winners' Cup, entry 6 */
    {"asia_super_cup", 0x410650u, 1},       /* Asian Super Cup, entry 6 */
    {"oceania_club_champ", 0x796fd0u, 0},   /* Oceania Champions Cup, entry 7 */
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
    if (season_game() && season_creating_game()) {
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
    const ClubWorldClass *k = class_of(c->file);
    if (!k || !c->started) return;
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
