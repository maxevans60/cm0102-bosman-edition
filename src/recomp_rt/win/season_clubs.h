/* Club moves at a new game (0.6.0 slice 15, Q225, Q237, Q244, Q279, Q290; spec
 * docs/specs/2026-10-16-seasons-0.6.0-spec.md).
 *
 * The club moves table: Seasons/clubs.tsv of the download (tools/seasons/make_clubs.py makes it from the
 * clubs files of the research, docs/research/rules-0.6.0/<part>-clubs.tsv). One row per real club of a
 * division at the start of a season: the database division (its long name), the club record id and name.
 *
 * At a new game in a season game (season_names.h: start year not 2001, or changing rules on) the runtime
 * puts the clubs of each list in their division, in memory: the division of a club is the pointer at club
 * +0x57 to its club_comp record (resolve_database_indexes 0x5315a0 at 0x531625; the leagues take their clubs
 * by it, for example EngConf::create_table 0x56ecb0 at 0x56ed54..0x56ed64). The database files stay
 * unchanged. It runs once, at the start of database_trim_staff_tables 0x524d70 (called at 0x828389 in
 * create_game 0x828110): after the load (0x8282a7, and after the date shift of slice 11b, season_dates.c, at
 * the end of that load) and before the trims of the staff and club tables (0x524d70, 0x5267c0) and before the
 * competitions are made. A loaded game needs nothing: its save holds the clubs as they were moved.
 *
 * A division keeps the club count that the database gives it (the count that the class of the division
 * takes; a different count is slice 18), except the divisions of LATER_SIZE (season_clubs.c): their class
 * takes another count after 2001/02 than the database keeps (Ligue 1: 20, not 18), so they get that count when
 * the start year is not 2001. So:
 *   - a list with more clubs than the division holds keeps the clubs that the database has there first, then
 *     the others in the order of the list;
 *   - a club that the database has in a listed division and that no list keeps there leaves the division
 *     (no division, as the original's clubs with no division; Q290);
 *   - a division with fewer clubs than before (a short list, or clubs that moved up into another division)
 *     takes clubs in this order (Q244, Q279): the clubs that the database had there and that no list keeps;
 *     the nation's clubs with no division; the clubs of the spare divisions "A Lower Division" and "A Premier
 *     Division" (keys 0x9cf764 and 0x9cf6b8); the clubs of lower divisions (club_comp +0x69, the rank) that no
 *     list keeps. The divisions are filled from the highest rank down, so a lower division that gives a club
 *     takes one in its turn. Inside each group the order is that of the club table.
 * A club of a list must have the record id and the name of the row and the nation of the division; else the
 * club of that name in the nation; else it is left out (an extra database of the season can hold other ids).
 *
 * The test switch RECOMP_SEASON_CODE=off and the base game (2001/02 with changing rules off) move no club.
 * RECOMP_CLUB_LOG=1 writes a line for each list to stderr. The test switch RECOMP_CLUB_MOVES=off moves no club
 * (the names stay). */
#ifndef RECOMP_SEASON_CLUBS_H
#define RECOMP_SEASON_CLUBS_H

/* Reads the table once at the start: clubs.tsv in the folder of RECOMP_RULES_TABLE (test runs) or
 * Seasons/clubs.tsv of the game folder. Host memory only. Returns the row count, 0 when the file is missing
 * or damaged (no club moves). */
int season_clubs_init(const char *game_dir);
int season_clubs_load(const char *path);

#endif
