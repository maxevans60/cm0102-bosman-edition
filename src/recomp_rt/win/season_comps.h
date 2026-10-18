/* New competitions (0.6.0, shared by slices 20, 21, 24, 26, 28 and 29; owner slice 26). Design and evidence:
 * docs/research/2026-10-18-new-comps.md.
 *
 * A new competition is one that the original has no object for (the Copa Sudamericana, the Conference League,
 * the 3. Liga, the Nations League). This module makes its object, puts it into its league table entry and keeps
 * it across a save and a load. A slice adds one SeasonCompDef line per competition to season_comp_defs in
 * season_comps.c, in its own small block.
 *
 * The shared rule for a season with no edition (lead, 2026-10-18), for the original's competitions and the new
 * ones alike. Never remove an object: other code reads it with no null test (0x4c41a0..0x4c41d0, 0x56fb08).
 *   - A competition that comes back after a gap waits for its next edition: in the before hook of
 *     start_new_season set its season year +0x40 to (next edition year - the class's year step), and at a new
 *     game give the constructor that year. Its dates then lie in that year, it is not complete, the pass does not
 *     start it again (0x5d5ccb), and it draws on its start date. Example: season_kind_club_world.c.
 *   - Only a competition that never comes back uses season_change_skip() in a before hook.
 * Why: a skipped competition keeps the start date of its old year. A League class draws only when today equals
 * that start date (WorldClubChamp::update_for_date 0x92a73d..0x92a74f, Copa Mercosur 0x4c5d2c..0x4c5d3d,
 * Merconorte 0x4c2ce5..0x4c2cf6), so after a skip and a later start_new_season it never draws again; a Cup class
 * draws its past rounds at once (0x51fa45..0x51fa7d). Evidence: docs/research/2026-10-18-club-world-idle.md,
 * section 1.
 *
 * When an object is made (the module, not the slice, decides it): once per game, in the entry hook of its
 * entry (season_change.h) in the first season S from the start season on that the table holds it, with
 * season_change_add_comp. The registry runs that hook
 *   - for the start season with changing rules on or off: in the run to the start date of a new game for an
 *     entry with its update day before the start date, on the entry's update day in the game for one after
 *     it, at the end of the new game for one from 1 October (season_change.h, "The start season");
 *   - for a later season only with changing rules on. So with changing rules off a competition that starts
 *     after the start season never comes (Q232);
 *   - never in the base game (start 2001, changing rules off) or with RECOMP_SEASON_CODE=off (Q152, Q342).
 * The entry must exist in the game: a national entry needs its nation selected (+0x11c, the registry does not
 * test it for this hook); a continental entry needs at least one object of the original in its array (the
 * creators make none when no nation of the continent is selected, 0x6699d0).
 * A season that the table does not hold after the first edition is the slice's matter: its before hook keeps
 * the object waiting or calls season_change_skip().
 *
 * The record. Each new competition needs a competition record (0x6b bytes in the array at [0xae23d0]) for its
 * name, history and [0xadadfc] slot. The def names a record that the data holds and no object of the original
 * uses (for example "South American CONMEBOL Cup", record 60 in every Update Team database), and the module
 * gives it the competition's name through season_change_rename. A def with record NULL needs a new record;
 * the module does not make one yet (design doc, "Open").
 *
 * Save and load (docs/research/2026-10-18-new-comps-load.md). The object saves itself like every object of an
 * entry array (key_nation_save_all_comps 0x6692f0 calls vtable +0x1c; the save packs every *.tmp file of
 * CM3_TEMP, 0x933610), to CM3_TEMP\<record name>_<record id>.tmp. A load makes only the original's objects
 * (the creators, key_nation_create_all_comps 0x669180 at 0x82c5aa). So the module wraps build_comp_object_table
 * 0x8386f0: at a load (call at 0x82c5f6), before the original fills [0xadadfc], it makes each def's object again
 * whose file is in CM3_TEMP and adds it to its entry. The constructor reads its state from the file itself
 * (Conmebol_Merc 0x4c471f, World_Club_Cup 0x92b52f), year and record included. The save part holds nothing for
 * this module: the file is the record that the object exists.
 *
 * Classes. A def's make function takes an original class (operator new with the class size, then the
 * constructor, thiscall (year, record), ret 8) and, where the class reads key id globals or fixed lists for its
 * clubs, a vtable copy with the club picker slot changed (season_comps_vtable). Only Conmebol_Liber takes its
 * clubs by its own record (club +0x1db, 0x4c3fb7); every other class picks the same clubs as the original's
 * object (doc section 5). */
#ifndef RECOMP_SEASON_COMPS_H
#define RECOMP_SEASON_COMPS_H

#include <stdint.h>

#include "season_change.h"

typedef struct SeasonCompDef SeasonCompDef;

struct SeasonCompDef {
    const char *comp;          /* the rules table `comp` of its rows (comp_file "new"): "sudamericana" */
    const char *file;          /* its name in the rules report and the logs: "new_sudamericana" */
    const char *record;        /* the long name in the data of the record it takes, or NULL (see above) */
    int entry;                 /* the league table entry whose array holds it, 0 to 34 */
    /* Makes the object in the game's heap for season year `year` with the record at `rec` (the address in the
     * array at [0xae23d0]) and returns it, or 0. `loading` is 1 at a load: the object then reads its state
     * from its saved file, as the original's constructors do. Called only by the module. */
    uint32_t (*make)(const SeasonCompDef *d, uint32_t rec, int year, int loading);
};

/* The competitions. Each slice adds its lines in season_comps.c. */
extern const SeasonCompDef *const season_comp_defs[];
extern const int season_comp_def_count;

/* 1 in a season game (see above). */
int season_comps_on(void);

/* The rules of a competition of the table by its `comp` (any comp_file): 1 when the table holds it in season s,
 * 0 when it is not held, -1 with no row. After the last row the rule stays (Q248). */
int season_comps_held(const char *comp, int s);

/* The first season from s on that the table holds the competition, 2999 when none, 0 with no row. */
int season_comps_next_edition(const char *comp, int s);

/* The object of a def in the game that runs, or 0 (not made, or the game has none). */
uint32_t season_comps_object(const SeasonCompDef *d);

/* The address of a competition record by its long name in the data (the name before any season_change_rename),
 * or 0. */
uint32_t season_comps_record(const char *name);

/* A copy of a class's vtable in runtime memory, so a new competition can take an original class and change
 * some slots. slots: the count to copy (Cup classes 40, League classes 45, sweep-europe 2.2). Returns the
 * address of the copy, or 0. */
uint32_t season_comps_vtable(uint32_t vtable, int slots);

/* Slice 28: the class of a vtable copy (the original class and kind, the def's file), or NULL for another
 * vtable. The rules report and season_change_file use it, so a new competition shows its def's file. */
#include "season_rules.h"
const SeasonCompClass *season_comps_class(uint32_t vtable);

/* Points slot `slot` of a vtable copy at a runtime function (rt_host_fn gives it a game address). */
void season_comps_set_slot(uint32_t vtable, int slot, const char *name, void (*fn)(void));

/* The rule kind "comps" (entry and loaded hooks). season_change.c lists it first in season_rule_kinds, so a
 * slice's own kind meets the object in the same season. */
extern const SeasonRuleKind season_kind_comps;

/* season_change_reset calls it: before a new game, a load and the end of a game. */
void season_comps_reset(void);

#endif
