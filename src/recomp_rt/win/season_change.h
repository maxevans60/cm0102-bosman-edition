/* The season change (0.6.0 slice 16; spec docs/specs/2026-10-16-seasons-0.6.0-spec.md, Q232, Q245, Q248, Q258).
 *
 * With changing rules on, each competition takes the rules of its new season at its season change, and the
 * player gets one news item for each season in which the rules change. With changing rules off, nothing
 * changes after the start (Q232). Research and evidence: docs/research/2026-10-17-season-change.md.
 *
 * When it runs: the original has no single "new season" step. game_season_update_pass 0x5d5ae0 runs at the
 * end of every day. On the update day of a league table entry it moves the entry's season year (+0x35) and
 * calls start_new_season (vtable +0x8) of each finished competition of the entry. A top league also starts
 * its lower leagues. start_new_season adds 1 (or the edition step) to the competition's season year +0x40.
 * The runtime wraps the pass: a competition object whose year +0x40 changed in the pass, or that is in the
 * competition array of an entry whose +0x35 moved, had its season change on that day. The second case holds
 * the international competitions that wait for their next edition (their year stays, but the season
 * moves). The new season comes from the game date on that day: a change from January to September of year Y
 * starts season Y (2009 for 2009/10; update days 10 January to 28 August), a change from 1 October starts
 * season Y + 1 (calendar-year leagues with update days 27 November and 27 December start their next edition,
 * which the rules table puts in the row of the year it is played). The entry's own year +0x35 is not used:
 * international entries move it by the edition step (2008 to 2010 in July 2009), and a December entry
 * moves it a year late. The change applies only to a season after the start
 * year: the run from 1 January to the start date at a new game (0x5cc850) is not a change.
 *
 * The registry. A rule kind is a table of two functions. Each later slice (17 to 29) adds its kinds to the
 * list season_rule_kinds in season_change.c, one line each. For each season change:
 *   1. season(S) of each kind runs once, at the first competition change of season S in this run of the
 *      program, before the comp calls of that day;
 *   2. comp(obj, file, S) of each kind runs once for each competition object whose season changed.
 * A kind applies the rule of season S from its own table (the rules table, season_rules.h, or another).
 * It writes the game's memory only for a real change, so a season with no rule change leaves the game as it
 * is. For each change that the player sees, it calls season_change_news with one sentence in plain words.
 * After the last row of a table the rule stays (Q248): a kind that finds no row changes nothing.
 *
 * The news item (Q258): the sentences of one season go into one news item of the Competitions category,
 * made with the game's own news code (NewsItem::NewsItem 0x77bbe0, News::send_to_all_managers 0x786780).
 * Later changes of the same season (a calendar-year league changes in another month) add their sentences to
 * the same item. The text is kept in the season part of the save (season.c, part version 2). */
#ifndef RECOMP_SEASON_CHANGE_H
#define RECOMP_SEASON_CHANGE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *name;                                         /* for the log: "names" */
    void (*season)(int season);                               /* NULL: no rule for the whole season */
    void (*comp)(uint32_t obj, const char *file, int season); /* NULL: no rule for each competition */
} SeasonRuleKind;

/* Adds one sentence to the news item of the season that changes now ("The UEFA Cup is now called ...").
 * Only a kind calls it, inside its season or comp function. The text is plain words, British English, no
 * exclamation marks, and no "<" (the game's text code reads it as the start of a token), in the game's code
 * (season_change_latin1). */
void season_change_news(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/* Turns UTF-8 text (the rules table is UTF-8) into the game's one-byte code (Windows-1252, as the data files):
 * each two-byte sequence of a character up to U+00FF becomes one byte. Other bytes stay. */
void season_change_latin1(char *s);

/* The competition objects that the game holds: the table g_comp_object_table [0xadadfc] and the
 * competition arrays of the 35 league table entries (as the rules report reads them, season_report.c).
 * Fills out with at most max distinct objects and returns their count. It only reads memory. */
int season_change_comps(uint32_t *out, int max);

/* The source file of a competition object's class (eng_first), or "?" for a class that is not known. */
const char *season_change_file(uint32_t obj);

/* Writes name (50 characters at most, in the game's code) as the long name (+4) of the competition record at
 * rec, and keeps the record's name in the data. The game finds its key competitions by name at every load
 * (match_club_comp_key_id 0x60efd0), so a load matches each changed record by its name in the data. Every
 * change of a competition's name in the season code goes through this function (slice 15 too). */
void season_change_rename(uint32_t rec, const char *name);

/* The season part of the save (season.c). The news texts and the changed names go after the 16 bytes of
 * version 1:
 *   +16 u32 count, then count times: u32 season, u32 length n, n bytes of text (no 0 at the end);
 *   then u32 count of names, then count times: 51 bytes name in the data, 51 bytes name now.
 * season_change_part_size gives 0 when the game has neither (the part stays version 1). */
uint32_t season_change_part_size(void);
void season_change_part_write(uint8_t *out);                  /* season_change_part_size bytes */
int season_change_part_read(const uint8_t *in, uint32_t n);   /* 0 for damaged bytes */
void season_change_reset(void);                               /* a new game, a load or the end of a game */

#endif
