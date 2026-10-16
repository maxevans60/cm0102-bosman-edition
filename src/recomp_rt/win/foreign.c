/* Foreign player colour (#47). Research: docs/research/2026-10-16-foreign-and-tactics-positions.md (#47).
 *
 * On the squad screen (0x456900) and the tactics screen (0x899ea0), the name of a player who counts
 * against the foreign player limit of his club's next competitive fixture gets light violet (RGB565
 * 0xCC7F) in place of the normal name colour. On the tactics screen the grey names change too: its list
 * has 20 rows (0x89e3f7), the eleven in light grey and the other rows (the named substitutes and, when
 * fewer are named, reserves) in grey. The rule counts the eleven and the named substitutes (0x8cf8cf), so
 * a violet reserve counts only when he is picked. A name with another colour of the original (on loan, a
 * player of another club and so on) keeps it. Option foreign_colours, on by default.
 *
 * The next competitive fixture: the first unplayed first-team fixture of the club from today on, in the
 * order of the game's fixture search 0x5acbe0 (day, then the three slots of a day, then the club's
 * categories), whose competition is not a friendly. The runtime reads the fixture store as 0x5acbe0 and
 * 0x5abf10 do, but it never calls them: the search puts each fixture in a new list node on the game's
 * heap, and a heap block more changes later days (Q152). A friendly is a fixture for which the game's
 * competition function 0x8d2fa0 gives kind 0xff (0x5c6710 at 0x8d2ff2). So in July the colour uses the
 * first league or cup match, after the friendlies.
 *
 * The rule: the game's own lookup 0x8cf6b0 (fixture, record, 1) keeps one rule in the record of 42
 * bytes: +5 a non-EU limit or +2 a foreign limit (or +0x16 an age limit, which the colour ignores).
 * A player counts for a non-EU rule when 0x53d980 (foreign) gives 1 and 0x53d740 (EU) gives 0, and
 * for a foreign rule when 0x53d980 gives 1, each with the nation of the club (+0x53), as the player
 * status 0x4849f0 does. The original's "Fgn" status stays.
 *
 * Every call only reads the game's data. The record and the out values go into the end of the game's
 * text buffer, and the runtime puts those bytes back. Only the text colour argument of a name changes.
 *
 * RECOMP_FOREIGN_LOG=1 writes a line for each rule that the runtime finds and for each name that gets
 * the colour (tests/smoke/foreign_run.py). */
#include "win.h"
#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const uint32_t rt_site_squad_name, rt_site_tactics_name;

#define VIOLET 0xCC7Fu           /* light violet (206, 142, 255) */
#define NAME_COLOUR 0xae3184u    /* u16: the normal name colour, light grey */
#define BENCH_COLOUR 0xae317eu   /* u16: grey, the substitutes on the tactics screen (0x89e2ec) */
#define CLUBS 0xae23b4u          /* pointer to the club records */
#define CLUB_SIZE 0x245u
#define GAME_DAY 0xae2c90u       /* s16 day of the year from 0, s16 year */
#define FIXTURES 0xae2a58u       /* the fixture store: year blocks, +0x112c next block, +0x1130 s16 year */
#define RULES 0xde1f54u          /* the rules manager: this of 0x8cf6b0 */
#define PROFILE_ID 0x7d7u        /* name buttons: a staff profile, data the staff record */
#define PROFILE_ID_2 0x7dcu      /* the same with another page (0x45b8f1, 0x89e1ba) */
#define SEARCH_DAYS 250

#define NATIONAL 0x53b900u       /* cdecl (club): nonzero for a national side */
#define CATEGORY_A 0x48dee0u     /* cdecl (club): a fixture category byte */
#define CATEGORY_B 0x5af5b0u     /* cdecl (club): a fixture category byte, or 0xff */
#define COMPETITION 0x8d2fa0u    /* thiscall RULES (fixture, kind out, nation out, continent out) */
#define RULE_LOOKUP 0x8cf6b0u    /* thiscall RULES (fixture, record, 1) */
#define IS_FOREIGN 0x53d980u     /* cdecl (person, nation, 1) */
#define IS_EU 0x53d740u          /* cdecl (person, nation) */

/* The end of the game's text buffer 0xde1f64 (2000 bytes): a rule record and three out values. The
 * match columns use 1600..1855 (columns.c). */
#define SCRATCH (0xde1f64u + 1900u)
#define SCRATCH_SIZE 56u
#define S_RECORD SCRATCH         /* 42 bytes */
#define S_KIND (SCRATCH + 44u)
#define S_NATION (SCRATCH + 48u)
#define S_CONTINENT (SCRATCH + 52u)

static uint32_t call_this(uint32_t fn, uint32_t self, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, n, args);
    cpu = saved;
    return r;
}

static uint32_t call_cdecl(uint32_t fn, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, n, args);
    cpu = saved;
    return r;
}

static int logging(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RECOMP_FOREIGN_LOG") != NULL;
    return on;
}

/* The club at address a, or 0 when a is not a club record. */
static uint32_t club_at(uint32_t a)
{
    uint32_t base = rd32(g_mem, CLUBS);
    if (!base || a < base || (a - base) % CLUB_SIZE) return 0;
    return rd32(g_mem, a) == (a - base) / CLUB_SIZE ? a : 0;
}

#define DAY_SLOTS 366   /* every year block has 366 days, leap year or not (0x5acd2b) */

static uint32_t year_block(int year)
{
    uint32_t b = rd32(g_mem, FIXTURES);
    for (int n = 0; b && n < 100; b = rd32(g_mem, b + 0x112c), n++)
        if ((int16_t)rd16(g_mem, b + 0x1130) == year) return b;
    return 0;
}

/* The fixture categories of a club, as 0x5acbe0 makes them (0x5acc22..0x5accde): of two equal
 * categories the earlier one goes (0x5accd4), so the order is the search's. */
static int categories(uint32_t club, uint8_t *out)
{
    uint32_t a[1] = {club};
    if (call_cdecl(NATIONAL, 1, a)) {
        out[0] = 0;
        out[1] = 1;
        return 2;
    }
    uint8_t c[8] = {2, 3, 4, 5, 6, 7, (uint8_t)call_cdecl(CATEGORY_A, 1, a), (uint8_t)call_cdecl(CATEGORY_B, 1, a)};
    for (int i = 0; i < 7; i++)
        for (int k = i + 1; k < 8; k++)
            if (c[i] == c[k]) { c[i] = 0xff; break; }
    int n = 0;
    for (int i = 0; i < 8; i++)
        if (c[i] != 0xff) out[n++] = c[i];
    return n;
}

/* 1 when the fixture is an unplayed first-team fixture of the club (0x5acd9e..0x5acdd3). Unplayed: the
 * result byte +0x43 is -1 or less (0x454c0d). */
static int club_fixture(uint32_t f, uint32_t id)
{
    if (!f || (int8_t)rd8(g_mem, f + 0x43) > -1) return 0;
    if (rd32(g_mem, f + 0xc) == id) return rd8(g_mem, f + 0x40) == 1;
    if (rd32(g_mem, f + 0x10) == id) return rd8(g_mem, f + 0x41) == 1;
    return 0;
}

/* The competition kind of a fixture (0x8d2fa0): 0xff for a friendly. */
static int fixture_kind(uint32_t f)
{
    wr8(g_mem, S_KIND, 0xff);
    uint32_t a[4] = {f, S_KIND, S_NATION, S_CONTINENT};
    call_this(COMPETITION, rd32(g_mem, RULES), 4, a);
    return rd8(g_mem, S_KIND);
}

/* The club's next competitive fixture, or 0. The search reads only the year blocks that the store has. */
static uint32_t next_competitive(uint32_t club)
{
    uint8_t cats[8];
    int count = categories(club, cats);
    uint32_t id = rd32(g_mem, club);
    int day = (int16_t)rd16(g_mem, GAME_DAY), year = (int16_t)rd16(g_mem, GAME_DAY + 2);
    uint32_t block = year_block(year);
    for (int n = 0; n < SEARCH_DAYS && block; n++) {
        for (uint32_t slot = 0; slot < 3; slot++) {
            uint32_t lists = rd32(g_mem, block + ((uint32_t)day * 3 + slot) * 4);
            for (int c = 0; lists && c < count; c++) {
                uint32_t list = rd32(g_mem, lists + 4u * cats[c]);
                uint32_t node = list ? rd32(g_mem, list + 2) : 0;
                for (int k = 0; node && k < 2000; node = rd32(g_mem, node + 6), k++) {
                    uint32_t f = rd32(g_mem, node);
                    if (club_fixture(f, id) && fixture_kind(f) != 0xff) return f;
                }
            }
        }
        if (++day >= DAY_SLOTS) {
            day = 0;
            block = year_block(++year);
        }
    }
    return 0;
}

enum { RULE_NONE, RULE_NON_EU, RULE_FOREIGN };

/* The rule of a club's next competitive fixture, kept per club for the day until the next screen build:
 * each build starts with the display reset 0x55f7c0 (names.c), and a load, a new game or a new fixture
 * (a cup draw) all show a new screen before a name is drawn again. */
typedef struct { uint32_t club, fixture; int day, year, rule; } RuleCache;
static RuleCache cache[16];
static int cache_next;

static int club_rule(uint32_t club)
{
    int day = (int16_t)rd16(g_mem, GAME_DAY), year = (int16_t)rd16(g_mem, GAME_DAY + 2);
    for (int i = 0; i < 16; i++)
        if (cache[i].club == club && cache[i].day == day && cache[i].year == year &&
            (!cache[i].fixture || (int8_t)rd8(g_mem, cache[i].fixture + 0x43) <= -1))
            return cache[i].rule;
    uint32_t f = rd32(g_mem, RULES) ? next_competitive(club) : 0;
    int rule = RULE_NONE;
    if (f) {
        uint32_t a[3] = {f, S_RECORD, 1};
        call_this(RULE_LOOKUP, rd32(g_mem, RULES), 3, a);
        if (rd8(g_mem, S_RECORD + 5) != 0xff) rule = RULE_NON_EU;
        else if (rd8(g_mem, S_RECORD + 2) != 0xff) rule = RULE_FOREIGN;
        if (logging())
            fprintf(stderr, "recomp: foreign rule club %u fixture %d.%d competition %u kind %d non-eu %d foreign %d\n",
                    rd32(g_mem, club), (int16_t)rd16(g_mem, f + 0x2a), (int16_t)rd16(g_mem, f + 0x28), rd32(g_mem, f),
                    fixture_kind(f), rd8(g_mem, S_RECORD + 5) == 0xff ? -1 : rd8(g_mem, S_RECORD + 5),
                    rd8(g_mem, S_RECORD + 2) == 0xff ? -1 : rd8(g_mem, S_RECORD + 2));
    } else if (logging())
        fprintf(stderr, "recomp: foreign rule club %u no competitive fixture\n", rd32(g_mem, club));
    cache[cache_next] = (RuleCache){club, f, day, year, rule};
    cache_next = (cache_next + 1) % 16;
    return rule;
}

/* Called by the display reset wrap (names.c): the next screen looks up each rule again. */
void foreign_forget(void)
{
    memset(cache, 0, sizeof cache);
}

/* 1 when the player counts against the rule of the club's next competitive fixture. */
static int counts(uint32_t person, uint32_t club)
{
    uint8_t saved[SCRATCH_SIZE];
    memcpy(saved, g_mem + SCRATCH, SCRATCH_SIZE);
    int rule = club_rule(club), result = 0;
    uint32_t nation = rd32(g_mem, club + 0x53);
    if (rule != RULE_NONE) {
        uint32_t a[3] = {person, nation, 1};
        int foreign = call_cdecl(IS_FOREIGN, 3, a) != 0;
        result = rule == RULE_FOREIGN ? foreign : foreign && !call_cdecl(IS_EU, 2, a);
    }
    memcpy(g_mem + SCRATCH, saved, SCRATCH_SIZE);
    return result;
}

/* The colour of a name: violet when the player counts, else 0. */
static uint32_t name_colour(uint32_t id, uint32_t person, uint32_t club, uint32_t colour, uint32_t text,
                            int bench, const char *screen)
{
    colour &= 0xffff;
    if ((id != PROFILE_ID && id != PROFILE_ID_2) || !person ||
        (colour != rd16(g_mem, NAME_COLOUR) && !(bench && colour == rd16(g_mem, BENCH_COLOUR))))
        return 0;
    if (!club) club = club_at(rd32(g_mem, person + 0x39));
    if (!club || !counts(person, club)) return 0;
    if (logging()) {
        char name[48];
        snprintf(name, sizeof name, "%s", text ? win_str(text) : "");
        for (char *s = name; *s; s++)
            if ((unsigned char)*s >= 0x80) *s = '?';   /* the log stays ASCII: names are cp1252 */
        fprintf(stderr, "recomp: foreign mark %s club %u staff %u \"%s\"\n", screen, rd32(g_mem, club),
                rd32(g_mem, person), name);
    }
    return VIOLET;
}

/* Called by the cell button wrap (assistant.c) before the original: (area, column, row, style, colour,
 * text_flags, font, text_colour, text, id, data). The squad list's name (0x45ba84). The squad's club is
 * the caller's [esp + 0x14] (0x45ba24), 11 arguments above it. */
void foreign_cell_button(void)
{
    if (rd32(g_mem, cpu.esp) != rt_site_squad_name || !options_bool(OPT_FOREIGN_COLOURS)) return;
    uint32_t club = club_at(rd32(g_mem, cpu.esp + 4 + 11 * 4 + 0x14));
    uint32_t c = name_colour(rt_arg(9), rt_arg(10), club, rt_arg(7), rt_arg(8), 0, "squad");
    if (c) wr32(g_mem, cpu.esp + 4 + 4 * 7, c);
}

/* Called by the widget create wrap (display.c) before the original: (flags, x1, y1, x2, y2, column, row,
 * ..., text_colour 12, text 13, ..., id 15, data 16, area 17). The tactics screen's name (0x89e3eb). The
 * club is +0 of the tactics object, the caller's [esp + 0x1c] (0x89e219), 18 arguments above it. */
void foreign_widget_before(void)
{
    if (rd32(g_mem, cpu.esp) != rt_site_tactics_name || !options_bool(OPT_FOREIGN_COLOURS)) return;
    uint32_t tactics = rd32(g_mem, cpu.esp + 4 + 18 * 4 + 0x1c);
    uint32_t club = tactics ? club_at(rd32(g_mem, tactics)) : 0;
    uint32_t c = name_colour(rt_arg(15), rt_arg(16), club, rt_arg(12), rt_arg(13), 1, "tactics");
    if (c) wr32(g_mem, cpu.esp + 4 + 4 * 12, c);
}
