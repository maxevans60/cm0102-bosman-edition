/* Season databases in the download (0.6.0 slice 14; spec docs/specs/2026-10-16-seasons-0.6.0-spec.md,
 * "Seasons and databases").
 *
 * The download holds the 21 season databases of docs/research/2026-10-16-season-database-table.md as zips,
 * Seasons/<name>.zip in the game folder (win_game_dir). A season database is "unpacked" when the save folder
 * holds it in Databases/<name> (dbfolders.h). The first pick of a season unpacks its zip once.
 *
 * Names: <name> is the neutral name of docs/research/rules-0.6.0/databases-signatures.tsv, for example
 * "October 2003". The same table is in the code (seasons_table.c, made by tools/seasons/make_db_table.py),
 * so the game recognises a database by its content (Q264, Q271) and never by its folder name.
 *
 * Seasons are numbered by their start year: 2001 is the season 2001/02, 2025 is 2025/26.
 *
 * For the Choose Season screen (slice 13) the calls are:
 *   seasons_db_scan(&list)           everything the screen shows: 25 seasons, their status, their versions
 *   seasons_db_prepare(year, &r)     starts a season: unpacks its zip once and returns the folder name
 *   seasons_db_identify(folder)      the known database of one folder, or NULL for a Custom database
 * The folder name that prepare returns goes in the option as database = folder:<name>. Every call runs on
 * the caller's thread. An unpack takes a few seconds (up to 67 MB are written). */
#ifndef RECOMP_SEASONS_DB_H
#define RECOMP_SEASONS_DB_H

#include <stddef.h>
#include <stdint.h>

#include "dbfolders.h"

#define SEASONS_FIRST 2001          /* the first season, 2001/02 */
#define SEASONS_COUNT 25            /* 2001/02 to 2025/26 */
#define SEASONS_EXTRA_MAX 8         /* the most known extra versions listed for one season */

/* What a known database is. */
typedef enum {
    SEASONS_ROLE_OFFICIAL_VERSION,  /* 3.9.60 and 3.9.65: other versions of the 2001/02 data */
    SEASONS_ROLE_OFFICIAL_BASE,     /* 3.9.68: the data of 2001/02 and 2002/03 */
    SEASONS_ROLE_SEASON,            /* the database that a season uses */
    SEASONS_ROLE_EXTRA,             /* another version of a season: shown as a version choice (Q252, Q257) */
    SEASONS_ROLE_NOT_SEASON         /* the Empty database: no clubs, no game */
} SeasonsRole;

/* One row of the signatures table. */
typedef struct {
    const char *name;               /* the neutral name, e.g. "October 2003" */
    const char *style;              /* "n/a", "ODB" or "SIM" */
    int season;                     /* start year of the data season (2001 to 2025), 0 for none */
    SeasonsRole role;
    uint64_t club_size, staff_size; /* sizes of club.dat and staff.dat */
    const char *club_sha256, *staff_sha256;   /* lower case hex */
} SeasonKnownDb;

extern const SeasonKnownDb seasons_known_dbs[];     /* seasons_table.c */
extern const int seasons_known_count;               /* 49 */

/* The five eras (Q166, Q240, Q261): the colours and the headings belong to the screen. */
typedef enum {
    SEASONS_ERA_ORIGINAL,           /* 2001/02 to 2002/03 */
    SEASONS_ERA_2000S,              /* 2003/04 to 2008/09 */
    SEASONS_ERA_2010S,              /* 2009/10 to 2019/20 */
    SEASONS_ERA_EARLY_2020S,        /* 2020/21 to 2023/24 */
    SEASONS_ERA_LEAGUE_PHASE        /* 2024/25 on */
} SeasonsEra;

SeasonsEra seasons_db_era(int start_year);
const char *seasons_db_era_name(SeasonsEra era);    /* "Original", "2000s", "2010s", "Early 2020s", "League phase" */

/* The status of a season's tile. */
typedef enum {
    SEASONS_OFFICIAL,               /* 2001/02 and 2002/03: the official data sets, always there */
    SEASONS_UNPACKED,               /* the database is in Databases: a start needs no unpack */
    SEASONS_IN_DOWNLOAD,            /* Seasons/<name>.zip is there: the first start unpacks it */
    SEASONS_NO_DATABASE,            /* no database exists for the season (2004/05, 2006/07) */
    SEASONS_MISSING                 /* a database exists, but this game folder has neither its zip nor an unpacked copy */
} SeasonsStatus;

typedef struct {
    const SeasonKnownDb *db;        /* a known extra version in Databases */
    char folder[DBFOLDER_NAME];     /* its folder in Databases, whatever the player named it */
} SeasonsExtra;

typedef struct {
    int start_year;                 /* 2001 to 2025 */
    char label[8];                  /* "2003/04" */
    SeasonsEra era;
    const char *database;           /* the name of the season's database: "October 2003", "3.9.68" for the
                                       two official seasons, NULL when no database exists */
    SeasonsStatus status;
    char folder[DBFOLDER_NAME];     /* with SEASONS_UNPACKED: the folder in Databases that holds it */
    int extra_count;                /* known other versions of this season in Databases (Update Team
                                       "extra version" rows; 3.9.60 and 3.9.65 too, when a folder holds them) */
    SeasonsExtra extra[SEASONS_EXTRA_MAX];
} SeasonsEntry;

typedef struct {
    SeasonsEntry season[SEASONS_COUNT];     /* index = start year - SEASONS_FIRST */
    int custom_count;                       /* folders of Databases that no known database matches (Q254) */
    char custom[DBFOLDER_MAX][DBFOLDER_NAME];
} SeasonsList;

/* The known database of a folder by the sizes and SHA-256 of its club.dat and staff.dat (Q271), or NULL when
 * the folder is a Custom database: unknown content, an edited database, or a missing file. The name of the
 * folder and index.dat play no part. The result is cached per folder by file sizes and modification times,
 * so the hash of a 45 MB database runs once. */
const SeasonKnownDb *seasons_db_identify(const char *folder);

/* The row of the table with this name, or NULL. */
const SeasonKnownDb *seasons_db_known(const char *name);

/* The season database row of a season (SEASONS_ROLE_SEASON), or NULL for 2001, 2002 and the seasons that
 * have none. */
const SeasonKnownDb *seasons_db_for_season(int start_year);

/* Fills the list from the game folder and the Databases folder. Folders of Databases that hold the Empty
 * database are in neither the versions nor the Custom list. */
void seasons_db_scan(SeasonsList *out);
void seasons_db_scan_in(const char *game_dir, const char *save_dir, SeasonsList *out);

typedef enum {
    SEASONS_PREPARE_OK,             /* r->name is a usable folder of Databases */
    SEASONS_PREPARE_OFFICIAL,       /* 2001 and 2002: no folder, the official data set applies */
    SEASONS_PREPARE_NO_DATABASE,    /* the season has no database, or the year is out of range */
    SEASONS_PREPARE_NO_ZIP,         /* the database is not in Databases and Seasons/<name>.zip is missing */
    SEASONS_PREPARE_FAILED          /* the unpack failed (r->error says why); no folder that looks whole is left */
} SeasonsPrepareStatus;

typedef struct {
    SeasonsPrepareStatus status;
    char name[DBFOLDER_NAME];       /* with OK: the folder name in Databases */
    char path[1100];                /* with OK: the host path of the folder */
    int unpacked;                   /* 1 when this call unpacked the zip, 0 when the folder was there already */
    char error[240];                /* plain words, for the player; with NO_ZIP and FAILED */
    /* Optional: called after each of the 20 files with (done, 20). Set before the call. */
    void (*progress)(void *ctx, int done, int total);
    void *progress_ctx;
} SeasonsPrepare;

/* The first pick of a season (Q235). If a folder of Databases holds the season's database (found by content,
 * so a folder the player renamed counts; the exact name wins), it is used. Otherwise Seasons/<name>.zip is
 * unpacked once into a temporary folder, checked against the signatures table, and renamed to
 * Databases/<name>. A folder with that name that holds other data is never replaced: the new folder gets
 * "<name> (2)". A failed unpack removes its temporary files. r->progress and r->progress_ctx
 * stay as the caller set them; the rest of r is overwritten. Returns r->status. */
SeasonsPrepareStatus seasons_db_prepare(int start_year, SeasonsPrepare *r);
SeasonsPrepareStatus seasons_db_prepare_in(const char *game_dir, const char *save_dir, int start_year, SeasonsPrepare *r);

#endif
