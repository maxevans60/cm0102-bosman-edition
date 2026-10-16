/* Custom data sets (Q143): folders in Databases, in the save folder.
 *
 * The player copies the files of a database (for example an Update Team zip) into a folder such as
 * Databases/May 2026. The Database choice on Setup Game lists each such folder, in season order (Q174). A folder
 * counts when it holds all 20 database files that the game loads (the .dat files of the clean Data folder). The
 * check ignores the case of file names. That all 20 are needed is an inference: the Update Team
 * databases that were read replace the 20 files of the plain 3.9.68 data. */
#ifndef RECOMP_DBFOLDERS_H
#define RECOMP_DBFOLDERS_H

#include <stddef.h>

#define DBFOLDER_MAX 128     /* the most folders that are listed: the Update Team zips hold 49 */
#define DBFOLDER_NAME 128    /* the longest folder name in bytes, with the end */

/* 1 when the folder holds every database file. */
int dbfolders_valid(const char *dir);

/* Lists the usable folders of <save_dir>/Databases in the order of dbfolders_sort. Returns the count, at
 * most max. When more folders exist, the first max in that order stay. A missing Databases folder gives 0. */
int dbfolders_list(const char *save_dir, char names[][DBFOLDER_NAME], int max);

/* The host path of the folder name in <save_dir>/Databases. Returns 0 when the name has a path part,
 * the folder is missing or it is not usable. */
int dbfolders_path(const char *save_dir, const char *name, char *out, size_t room);

/* The name as button text in code page 1252: a letter that the code page lacks becomes '?'. At most
 * room - 1 bytes. */
void dbfolders_label(const char *utf8, char *out, size_t room);

/* The room for dbfolders_label on the Setup Game button, for a screen screen_w pixels wide. The button
 * holds about 32 characters in classic mode (room 24, after "Database: "). The button grows with the
 * screen and the text keeps its size, so the room grows by the same factor (0.4.0 slice 10). */
size_t dbfolders_label_room(unsigned screen_w);

/* The date in a folder name (Q174): 1 when the name holds a month and a four-digit year as words, for
 * example "May 2026" or "October 2003 Sawczuk". A month is its English name or its first three letters
 * ("Sept" too), without regard to case. "Spring" counts as April and "Autumn" as October. The first month
 * and the first year in the name count. */
int dbfolders_date(const char *name, int *year, int *month);

/* The season of a dated folder name, as "2025/26": July to December starts a season, January to June
 * ends it. Returns 0 and writes nothing when the name has no date. */
int dbfolders_season(const char *name, char *out, size_t room);

/* Sorts the names: the dated names first, oldest first, two names of one month by name; then the names
 * with no date, by name without regard to case. */
void dbfolders_sort(char names[][DBFOLDER_NAME], int n);

/* The button text of a folder, at most room - 1 bytes in code page 1252: "2025/26 (May 2026)", or the
 * season alone when that does not fit, or the name (shortened) when it has no date. */
void dbfolders_button_text(const char *name, char *out, size_t room);

#endif
