/* New competitions (0.6.0; season_comps.h, design docs/research/2026-10-18-new-comps.md). */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_comps.h"
#include "season_rules.h"

#include <stdio.h>
#include <string.h>

#define COMP_RECORDS 0xae23d0u          /* the club comp records, then the nation comp records */
#define COMP_RECORD_COUNT 0xae2378u     /* s32: the club comp records ([0xadadfc] has one slot each, 0x8386fb) */
#define COMP_TABLE 0xadadfcu            /* Comp **, indexed by record id (record +0) */
#define COMP_SLOTS 0xadadf8u            /* s32 */
#define RECORD_SIZE 0x6bu
#define NAME_AT 4u
#define ENTRIES 0xb63d60u               /* league table entries, 0x48 bytes */
#define ENTRY_SIZE 0x48u
#define ENTRY_COUNT 35
#define GAME_YEAR 0xae2c92u             /* s16 */
#define NEVER 2999

/* ---- The competitions: each slice adds its lines here, in its own block ---- */

/* Slice 28 (season_nl.c): the UEFA Nations League. */
extern const SeasonCompDef season_nl_comp;

const SeasonCompDef *const season_comp_defs[] = {
    NULL,                               /* keeps the array non-empty; the module skips NULL */
    &season_nl_comp,                    /* slice 28 */
};
const int season_comp_def_count = (int)(sizeof season_comp_defs / sizeof season_comp_defs[0]);

/* ---- State of the game that runs ---- */

#define MAX_DEFS 64
static uint32_t objects[MAX_DEFS];      /* the object of each def, 0 when not made */

static void reset(void) { memset(objects, 0, sizeof objects); }

int season_comps_on(void)
{
    return season_code_on() && (season_start_year() != SEASON_FIRST_YEAR || season_changing_rules());
}

/* ---- The rules table ---- */

static const SeasonRule *row_of(const char *comp, int s)
{
    const SeasonRule *best = NULL;
    for (int i = 0; i < season_rules_count(); i++) {
        const SeasonRule *r = season_rules_row(i);
        if (strcmp(r->comp, comp) || r->season > s) continue;
        if (!best || r->season > best->season) best = r;
    }
    return best;
}

static int is_held(const SeasonRule *r)
{
    return r->clubs != SEASON_RULE_NOT_HELD && strcmp(r->format, "not_held") != 0;
}

int season_comps_held(const char *comp, int s)
{
    const SeasonRule *r = comp ? row_of(comp, s) : NULL;
    return r ? is_held(r) : -1;
}

int season_comps_next_edition(const char *comp, int s)
{
    int h = season_comps_held(comp, s);
    if (h < 0) return 0;
    if (h) return s;
    int last = 0;
    for (int i = 0; i < season_rules_count(); i++) {
        const SeasonRule *r = season_rules_row(i);
        if (!strcmp(r->comp, comp) && r->season > last) last = r->season;
    }
    for (int y = s + 1; y <= last; y++)
        if (season_comps_held(comp, y) == 1) return y;
    return NEVER;
}

/* ---- Records, objects and vtables ---- */

static int def_index(const SeasonCompDef *d)
{
    for (int i = 0; i < season_comp_def_count && i < MAX_DEFS; i++)
        if (season_comp_defs[i] == d) return i;
    return -1;
}

uint32_t season_comps_object(const SeasonCompDef *d)
{
    int i = def_index(d);
    return i < 0 ? 0 : objects[i];
}

uint32_t season_comps_record(const char *name)
{
    uint32_t base = rd32(g_mem, COMP_RECORDS);
    int32_t count = (int32_t)rd32(g_mem, COMP_RECORD_COUNT);
    if (!name || !base || count <= 0 || count > 100000) return 0;
    for (int32_t i = 0; i < count; i++) {
        uint32_t rec = base + (uint32_t)i * RECORD_SIZE;
        const char *n = season_change_data_name(rec);
        if (n && !strcmp(n, name)) return rec;
    }
    return 0;
}

uint32_t season_comps_vtable(uint32_t vtable, int slots)
{
    if (slots <= 0 || slots > 128) return 0;
    uint32_t copy = win_static_alloc(4u * (uint32_t)slots);
    if (!copy) return 0;
    memcpy(g_mem + copy, g_mem + vtable, 4u * (size_t)slots);
    return copy;
}

void season_comps_set_slot(uint32_t vtable, int slot, const char *name, void (*fn)(void))
{
    if (vtable && slot >= 0) wr32(g_mem, vtable + 4u * (uint32_t)slot, rt_host_fn(name, fn));
}

/* The entry exists in the game (season_comps.h): a national entry with its nation selected, a continental or
 * world entry with at least one object of the original. */
static int entry_in_game(int entry)
{
    if (entry < 0 || entry >= ENTRY_COUNT) return 0;
    uint32_t e = ENTRIES + (uint32_t)entry * ENTRY_SIZE;
    uint32_t nation = rd32(g_mem, e);
    if (nation) return g_mem[nation + 0x11c] != 0;     /* the selected flag, as the pass reads it (0x5d5be5) */
    return (int32_t)rd32(g_mem, e + 0xc) > 0;
}

/* [0xadadfc][record id] = obj. The daily update walks the table, not the entry arrays (0x5ce060), so an object
 * added after build_comp_object_table 0x8386f0 needs its slot (new-comps-load doc, section 1, point 5). */
static void set_table_slot(uint32_t rec, uint32_t obj)
{
    uint32_t table = rd32(g_mem, COMP_TABLE);
    int32_t slots = (int32_t)rd32(g_mem, COMP_SLOTS);
    int32_t id = (int32_t)rd32(g_mem, rec);
    if (table && id >= 0 && id < slots) wr32(g_mem, table + 4u * (uint32_t)id, obj);
}

/* The name of the competition in season s from the table, in the game's code, or "" with none. */
static void table_name(const SeasonCompDef *d, int s, char *out, size_t room)
{
    const SeasonRule *r = row_of(d->comp, s);
    snprintf(out, room, "%s", r && r->name && strcmp(r->name, "n/a") ? r->name : "");
    season_change_latin1(out);
    out[room > 51 ? 50 : room - 1] = 0;
}

static uint32_t make(const SeasonCompDef *d, int i, int s, int loading)
{
    uint32_t rec = season_comps_record(d->record);
    if (!rec || !d->make) {
        fprintf(stderr, "recomp: new competition %s: no record \"%s\" in the data\n", d->file,
                d->record ? d->record : "(none)");
        return 0;
    }
    uint32_t obj = d->make(d, rec, s, loading);
    if (!obj || rt_unwinding) return 0;
    if (!season_change_add_comp(d->entry, obj)) {
        fprintf(stderr, "recomp: new competition %s: not added to entry %d\n", d->file, d->entry);
        return 0;
    }
    objects[i] = obj;
    if (!loading) set_table_slot(rec, obj);
    fprintf(stderr, "recomp: new competition %s made for %d (entry %d, record %d)%s\n", d->file, s, d->entry,
            (int)rd32(g_mem, rec), loading ? " from the save" : "");
    return obj;
}

/* ---- The rule kind ---- */

static void comps_entry(const SeasonChange *c)
{
    if (!season_comps_on() || !entry_in_game(c->entry)) return;
    for (int i = 0; i < season_comp_def_count && i < MAX_DEFS; i++) {
        const SeasonCompDef *d = season_comp_defs[i];
        if (!d || d->entry != c->entry || objects[i] || season_comps_held(d->comp, c->season) != 1) continue;
        uint32_t obj = make(d, i, c->season, 0);
        if (!obj) continue;
        char name[64];
        table_name(d, c->season, name, sizeof name);
        if (*name) season_change_rename(rd32(g_mem, obj + NAME_AT), name);
        if (*name) season_change_news("The %s is played for the first time.", name);
    }
}

/* A later name of the table (Q226, Q238). */
static void comps_after(const SeasonChange *c)
{
    if (!season_comps_on()) return;
    for (int i = 0; i < season_comp_def_count && i < MAX_DEFS; i++) {
        const SeasonCompDef *d = season_comp_defs[i];
        if (!d || !objects[i] || objects[i] != c->comp) continue;
        char name[64];
        table_name(d, c->season, name, sizeof name);
        uint32_t rec = rd32(g_mem, c->comp + NAME_AT);
        char old[52];
        snprintf(old, sizeof old, "%.50s", win_str(rec + NAME_AT));
        if (!*name || !strcmp(old, name)) continue;
        season_change_rename(rec, name);
        season_change_news("%s is now called %s.", old, name);
    }
}

const SeasonRuleKind season_kind_comps = {"comps", NULL, NULL, NULL, comps_entry, NULL, comps_after};

void season_comps_reset(void) { reset(); }

/* ---- The load: build_comp_object_table 0x8386f0 ---- */

/* The file name that the object's save writes (Cup 0x5210fe..0x521162, League 0x6891ce..0x68923a): the long name
 * of the record with "/" and "." made "_", then "_<record id>.tmp". */
static void save_file(uint32_t rec, char *out, size_t room)
{
    char name[64];
    snprintf(name, sizeof name, "%.50s", win_str(rec + NAME_AT));
    for (char *p = name; *p; p++)
        if (*p == '/' || *p == '.') *p = '_';
    snprintf(out, room, "%s_%d.tmp", name, (int)rd32(g_mem, rec));
}

void o_008386f0(void);

/* build_comp_object_table, cdecl, called by create_game (0x829640) and the load (0x82c5f6). At a load the
 * creators made only the original's objects (key_nation_create_all_comps 0x669180 at 0x82c5aa), so each def
 * whose object's file came out of the save is made again here, before the original fills [0xadadfc] from the
 * entry arrays. The constructor reads its state from that file. Inference: the temp files are in the user
 * folder, as the game's own *.tmp files of a run are. */
RT_WRAP(008386f0, build_comp_object_table)
{
    if (season_comps_on() && !season_change_is_creating()) {
        reset();
        int year = (int16_t)rd16(g_mem, GAME_YEAR);
        for (int i = 0; i < season_comp_def_count && i < MAX_DEFS; i++) {
            const SeasonCompDef *d = season_comp_defs[i];
            if (!d || !entry_in_game(d->entry)) continue;
            uint32_t rec = season_comps_record(d->record);
            if (!rec) continue;
            char file[128], host[2048];
            save_file(rec, file, sizeof file);
            if (win_existing_file(file, host, sizeof host)) make(d, i, year, 1);
            if (rt_unwinding) return;
        }
    }
    o_008386f0();
}
