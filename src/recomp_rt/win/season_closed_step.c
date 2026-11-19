/* The season change of a closed league in the running game (season_closed_step.h). */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_closed.h"
#include "season_closed_step.h"
#include "season_clubs.h"
#include "season_records.h"
#include "season_rules.h"
#include "season_sizes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUBS 0xae23b4u             /* the club table (0x245 bytes a club) */
#define CLUB_COUNT 0xae235cu
#define CLUB_SIZE 0x245u
#define NATION_COUNT 0xae2350u      /* the last 2 x nations rows of the club table are national teams (0x53b900) */
#define CLUB_NAME 4u
#define CLUB_SHORT 0x38u
#define CLUB_NATION 0x53u           /* nation pointer */
#define CLUB_DIVISION 0x57u         /* club_comp pointer */
#define CLUB_LAST_DIVISION 0x5bu    /* the division of the last season */
#define CLUB_REPUTATION 0x80u       /* s16 */
#define REC_NATION 0x5du            /* club_comp: nation pointer */
#define COMP_RECORD 4u
#define COMP_YEAR 0x40u
#define COMP_TABLE 0xb1u
#define A_LEAGUE_MAX 13             /* AusNsl::create_table 0x411b82 */

int season_closed_step(uint32_t rec, const char *league, const char *tag, int season, int edition, int want)
{
    uint32_t table = rd32(g_mem, CLUBS), nation = rd32(g_mem, rec + REC_NATION);
    int count = (int)rd32(g_mem, CLUB_COUNT) - 2 * (int)rd32(g_mem, NATION_COUNT);
    if (count <= 0) return 0;
    ClosedClub *club = calloc((size_t)count, sizeof *club);
    uint32_t *at = calloc((size_t)count, sizeof *at);
    int *action = calloc((size_t)count, sizeof *action);
    unsigned *from = calloc((size_t)count, sizeof *from), *to = calloc((size_t)count, sizeof *to);
    if (!club || !at || !action || !from || !to) rt_fatal("closed league: no memory for the season change");
    int n = 0;
    for (int i = 0; i < count; i++) {
        uint32_t c = table + (uint32_t)i * CLUB_SIZE;
        int in_league = rd32(g_mem, c + CLUB_DIVISION) == rec;
        const char *name = win_str(c + CLUB_NAME), *short_name = win_str(c + CLUB_SHORT);
        if (!in_league && rd32(g_mem, c + CLUB_NATION) != nation && !season_closed_abroad(league, name, short_name))
            continue;
        at[n] = c;
        club[n].name = name;
        club[n].short_name = short_name;
        club[n].in_league = in_league;
        club[n].human = season_sizes_is_human(c);
        club[n].reserve = season_clubs_reserve_name(name);
        club[n].reputation = (int16_t)rd16(g_mem, c + CLUB_REPUTATION);
        n++;
    }
    ClosedResult r;
    season_closed_plan(league, season, want, club, n, action, &r);
    /* s073-16: a club that leaves the league takes the place of a club that joins (season_closed_places). */
    for (int i = 0; i < n; i++) from[i] = action[i] == CLOSED_JOIN ? rd32(g_mem, at[i] + CLUB_DIVISION) : 0;
    season_closed_places(club, action, from, n, to);
    for (int i = 0; i < n && !rt_unwinding; i++) {
        if (action[i] == CLOSED_FOLD || action[i] == CLOSED_CUT) {
            wr32(g_mem, at[i] + CLUB_LAST_DIVISION, rec);
            wr32(g_mem, at[i] + CLUB_DIVISION, to[i]);
            fprintf(stderr, "recomp: %s: %d: %.50s %s%s\n", tag, edition, club[i].name,
                    action[i] == CLOSED_FOLD ? "folds: it goes to no division" : "leaves for a club of the season",
                    to[i] ? ", to the place of a club that joins" : "");
        } else if (action[i] == CLOSED_JOIN) {
            uint32_t from = rd32(g_mem, at[i] + CLUB_DIVISION);
            if (from) wr32(g_mem, at[i] + CLUB_LAST_DIVISION, from);
            wr32(g_mem, at[i] + CLUB_DIVISION, rec);
            int players = season_records_fill_club(at[i], edition);
            fprintf(stderr, "recomp: %s: %d: %.50s joins with %d players, %d of them new with a contract\n", tag, edition,
                    club[i].name, players, season_records_joined_contracts());
        } else if (club[i].in_league && club[i].human &&
                   season_closed_member(league, club[i].name, club[i].short_name, season) == 0) {
            fprintf(stderr, "recomp: %s: %d: %.50s stays: a human manages it\n", tag, edition, club[i].name);
        }
    }
    if (r.missing)
        fprintf(stderr, "recomp: %s: %d: %d clubs of the season are not in this database\n", tag, edition, r.missing);
    free(club);
    free(at);
    free(action);
    free(from);
    free(to);
    return r.count;
}

/* AusNsl::start_new_season 0x411de0 frees the last table (League::free_table 0x687970 with 0 at 0x411df0 clears +0xb1
 * at 0x687a10), adds 1 to the edition (0x411e1e) and calls set_up_rules (slot +0x8c at 0x411e24). set_up_rules asks for
 * the dates of stage -1 (create_schedule 0x4111a0, slot +0x3c, at 0x411d74), and the size code counts the clubs of the
 * division for them, so the clubs move here, first. create_table 0x411e62 then takes the clubs of the division
 * (0x411c1a..0x411c2a). A game that loads keeps its table, so an empty table marks the season change. */
void season_closed_rules(uint32_t obj)
{
    if (!season_changing_rules() || season_change_is_creating()) return;
    if (strcmp(season_change_file(obj), "aus_nsl") || rd32(g_mem, obj + COMP_TABLE)) return;
    int edition = (int16_t)rd16(g_mem, obj + COMP_YEAR);
    if (!season_closed_active("aus_nsl", edition)) return;
    int s = season_rules_last_season(season_sizes_edition("aus_nsl", edition), "aus_nsl");
    const SeasonRule *row = s ? season_rules_find(s, "aus_nsl", 0) : NULL;
    int want = row && row->clubs >= 4 && row->clubs <= A_LEAGUE_MAX ? row->clubs : 0;
    int clubs = season_closed_step(rd32(g_mem, obj + COMP_RECORD), "aus_nsl", "a-league", edition, edition, want);
    if (rt_unwinding) return;
    fprintf(stderr, "recomp: a-league: %d: %d clubs, the row %d\n", edition, clubs, want);
}
