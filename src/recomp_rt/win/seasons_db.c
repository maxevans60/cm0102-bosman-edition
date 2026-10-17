/* Season databases in the download (seasons_db.h). */
#include "seasons_db.h"
#include "seasons_zip.h"
#include "../host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* From win.h (files.c). Declared here so that the tests can link this file with two stand-ins. */
extern char win_game_dir[1024];
const char *win_databases_root(void);

/* The 20 data files of dbfolders.c. dbfolders_valid checks the same list after an unpack. */
static const char *const data_files[] = {
    "city.dat", "club.dat", "club_comp.dat", "club_comp_history.dat", "colour.dat", "common_names.dat",
    "continent.dat", "first_names.dat", "index.dat", "nat_club.dat", "nation.dat", "nation_comp.dat",
    "nation_comp_history.dat", "officials.dat", "second_names.dat", "stadium.dat", "staff.dat",
    "staff_comp.dat", "staff_comp_history.dat", "staff_history.dat"};
enum { FILE_COUNT = sizeof data_files / sizeof *data_files };

/* ---- Names and the table ---- */

SeasonsEra seasons_db_era(int y)
{
    return y <= 2002 ? SEASONS_ERA_ORIGINAL : y <= 2008 ? SEASONS_ERA_2000S : y <= 2019 ? SEASONS_ERA_2010S
         : y <= 2023 ? SEASONS_ERA_EARLY_2020S : SEASONS_ERA_LEAGUE_PHASE;
}

const char *seasons_db_era_name(SeasonsEra era)
{
    static const char *const names[] = {"Original", "2000s", "2010s", "Early 2020s", "League phase"};
    return (unsigned)era < 5 ? names[era] : "";
}

const SeasonKnownDb *seasons_db_known(const char *name)
{
    for (int i = 0; i < seasons_known_count; i++)
        if (!strcmp(seasons_known_dbs[i].name, name)) return &seasons_known_dbs[i];
    return NULL;
}

const SeasonKnownDb *seasons_db_for_season(int y)
{
    for (int i = 0; i < seasons_known_count; i++)
        if (seasons_known_dbs[i].role == SEASONS_ROLE_SEASON && seasons_known_dbs[i].season == y)
            return &seasons_known_dbs[i];
    return NULL;
}

/* ---- Recognition (Q264, Q271) ---- */

/* The path of the file with this name in dir, whatever its case. 0 when it is not there. */
static int find_file(const char *dir, const char *name, char *out, size_t room)
{
    HostDir *d = host_opendir(dir);
    if (!d) return 0;
    const char *e;
    int ok = 0;
    while ((e = host_readdir(d))) {
        if (!strcasecmp(e, name)) {
            ok = snprintf(out, room, "%s/%s", dir, e) < (int)room;
            break;
        }
    }
    host_closedir(d);
    return ok;
}

typedef struct {
    char path[1100];
    uint64_t club_size, staff_size;
    int64_t club_time, staff_time;
    const SeasonKnownDb *db;
} IdentCache;

enum { CACHE_ROWS = 64 };
static IdentCache cache[CACHE_ROWS];
static int cache_used, cache_next;

const SeasonKnownDb *seasons_db_identify(const char *folder)
{
    char club[1100], staff[1100];
    HostStat cs, ss;
    if (!find_file(folder, "club.dat", club, sizeof club) || !find_file(folder, "staff.dat", staff, sizeof staff) ||
        host_stat(club, &cs) || host_stat(staff, &ss) || cs.is_dir || ss.is_dir)
        return NULL;
    char key[1100];
    snprintf(key, sizeof key, "%s", folder);
    for (int i = 0; i < cache_used; i++) {
        IdentCache *c = &cache[i];
        if (!strcmp(c->path, key) && c->club_size == cs.size && c->staff_size == ss.size &&
            c->club_time == cs.mtime && c->staff_time == ss.mtime)
            return c->db;
    }
    const SeasonKnownDb *found = NULL;
    char club_hex[65] = "", staff_hex[65] = "";
    int hashed = 0;
    for (int i = 0; i < seasons_known_count && !found; i++) {
        const SeasonKnownDb *k = &seasons_known_dbs[i];
        if (k->club_size != cs.size || k->staff_size != ss.size) continue;       /* step 1: sizes only */
        if (!hashed) {                                                           /* step 2: contents */
            uint64_t n;
            if (!seasons_sha_file(club, &n, club_hex) || !seasons_sha_file(staff, &n, staff_hex)) return NULL;
            hashed = 1;
        }
        if (!strcmp(k->club_sha256, club_hex) && !strcmp(k->staff_sha256, staff_hex)) found = k;
    }
    IdentCache *c = &cache[cache_next];
    snprintf(c->path, sizeof c->path, "%s", key);
    c->club_size = cs.size; c->staff_size = ss.size;
    c->club_time = cs.mtime; c->staff_time = ss.mtime;
    c->db = found;
    cache_next = (cache_next + 1) % CACHE_ROWS;
    if (cache_used < CACHE_ROWS) cache_used++;
    return found;
}

/* ---- The list for the Choose Season screen ---- */

static int zip_path(const char *game_dir, const SeasonKnownDb *db, char *out, size_t room)
{
    return snprintf(out, room, "%s/Seasons/%s.zip", game_dir, db->name) < (int)room;
}

void seasons_db_scan_in(const char *game_dir, const char *save_dir, SeasonsList *out)
{
    memset(out, 0, sizeof *out);
    for (int i = 0; i < SEASONS_COUNT; i++) {
        SeasonsEntry *s = &out->season[i];
        s->start_year = SEASONS_FIRST + i;
        snprintf(s->label, sizeof s->label, "%04d/%02d", s->start_year, (s->start_year + 1) % 100);
        s->era = seasons_db_era(s->start_year);
        if (s->start_year <= 2002) {
            s->database = "3.9.68";
            s->status = SEASONS_OFFICIAL;
        } else {
            const SeasonKnownDb *db = seasons_db_for_season(s->start_year);
            s->database = db ? db->name : NULL;
            s->status = db ? SEASONS_MISSING : SEASONS_NO_DATABASE;
        }
    }
    static char names[DBFOLDER_MAX][DBFOLDER_NAME];     /* static: 16 KB, and the game runs this on one thread */
    int n = dbfolders_list(save_dir, names, DBFOLDER_MAX);
    for (int i = 0; i < n; i++) {
        char path[1100];
        if (!dbfolders_path(save_dir, names[i], path, sizeof path)) continue;
        const SeasonKnownDb *k = seasons_db_identify(path);
        if (!k) {
            if (out->custom_count < DBFOLDER_MAX) snprintf(out->custom[out->custom_count++], DBFOLDER_NAME, "%s", names[i]);
            continue;
        }
        if (k->role == SEASONS_ROLE_NOT_SEASON || k->role == SEASONS_ROLE_OFFICIAL_BASE || k->season < SEASONS_FIRST ||
            k->season >= SEASONS_FIRST + SEASONS_COUNT)
            continue;
        SeasonsEntry *s = &out->season[k->season - SEASONS_FIRST];
        if (k->role == SEASONS_ROLE_SEASON) {
            /* A folder with the neutral name wins over a copy under another name. */
            if (s->status != SEASONS_UNPACKED || !strcmp(names[i], k->name)) {
                s->status = SEASONS_UNPACKED;
                snprintf(s->folder, sizeof s->folder, "%s", names[i]);
            }
        } else if (s->extra_count < SEASONS_EXTRA_MAX) {
            s->extra[s->extra_count].db = k;
            snprintf(s->extra[s->extra_count].folder, DBFOLDER_NAME, "%s", names[i]);
            s->extra_count++;
        }
    }
    for (int i = 0; i < SEASONS_COUNT; i++) {
        SeasonsEntry *s = &out->season[i];
        char zp[1100];
        HostStat st;
        const SeasonKnownDb *db = s->database ? seasons_db_known(s->database) : NULL;
        if (s->status == SEASONS_MISSING && db && zip_path(game_dir, db, zp, sizeof zp) && !host_stat(zp, &st) && !st.is_dir)
            s->status = SEASONS_IN_DOWNLOAD;
    }
}

void seasons_db_scan(SeasonsList *out) { seasons_db_scan_in(win_game_dir, win_databases_root(), out); }

/* ---- First pick ---- */

static void say(SeasonsPrepare *r, const char *fmt, const char *a, const char *b)
{
    snprintf(r->error, sizeof r->error, fmt, a, b);
}

/* Removes a folder of plain files and the folder. A missing folder is fine. */
static void remove_flat_folder(const char *dir)
{
    char names[64][260];
    int n = 0;
    HostDir *d = host_opendir(dir);
    if (!d) return;
    const char *e;
    while ((e = host_readdir(d)) && n < 64) {
        if (!strcmp(e, ".") || !strcmp(e, "..")) continue;
        snprintf(names[n++], sizeof names[0], "%s", e);
    }
    host_closedir(d);
    for (int i = 0; i < n; i++) {
        char p[1400];
        snprintf(p, sizeof p, "%s/%s", dir, names[i]);
        host_unlink(p);
    }
    host_rmdir(dir);
}

static int exists_dir(const char *path)
{
    HostStat st;
    return !host_stat(path, &st) && st.is_dir;
}

/* An entry name that is safe to take the file name from: no folder climbing, no drive, no backslash. */
static int safe_entry_name(const char *name, const char **base)
{
    if (name[0] == '/' || strchr(name, '\\') || strchr(name, ':')) return 0;
    for (const char *p = name; *p;) {
        const char *q = strchr(p, '/');
        size_t len = q ? (size_t)(q - p) : strlen(p);
        if (len == 2 && p[0] == '.' && p[1] == '.') return 0;
        if (!q) break;
        p = q + 1;
    }
    const char *slash = strrchr(name, '/');
    *base = slash ? slash + 1 : name;
    return **base != 0;
}

/* Unpacks the zip into tmp (a new folder) and checks club.dat and staff.dat against the row. 0 on success,
 * else r->error is set. The caller removes tmp. */
static int unpack_zip(const char *zip, const char *tmp, const SeasonKnownDb *row, SeasonsPrepare *r)
{
    SeasonsZip z;
    char err[160] = "";
    if (seasons_zip_open(&z, zip, err, sizeof err)) {
        say(r, "Could not read %s: %s.", row->name, err);
        return -1;
    }
    int seen[FILE_COUNT] = {0}, done = 0;
    char club_hex[65] = "", staff_hex[65] = "";
    uint64_t club_size = 0, staff_size = 0;
    for (int i = 0; i < z.count; i++) {
        const SeasonsZipEntry *e = &z.e[i];
        const char *base;
        if (e->is_dir) continue;
        if (!safe_entry_name(e->name, &base)) {
            say(r, "The file for %s holds a name that is not allowed.", row->name, "");
            seasons_zip_close(&z);
            return -1;
        }
        int k;
        for (k = 0; k < FILE_COUNT; k++) if (!strcasecmp(base, data_files[k])) break;
        if (k == FILE_COUNT) continue;              /* a readme or a picture: not part of a database */
        if (seen[k]) {
            say(r, "The file for %s holds %s twice.", row->name, data_files[k]);
            seasons_zip_close(&z);
            return -1;
        }
        seen[k] = 1;
        char out[1400], hex[65];
        snprintf(out, sizeof out, "%s/%s", tmp, data_files[k]);
        if (seasons_zip_extract(&z, i, out, hex, err, sizeof err)) {
            say(r, "Could not unpack %s: %s.", row->name, err);
            seasons_zip_close(&z);
            return -1;
        }
        if (!strcmp(data_files[k], "club.dat")) { memcpy(club_hex, hex, 65); club_size = e->size; }
        if (!strcmp(data_files[k], "staff.dat")) { memcpy(staff_hex, hex, 65); staff_size = e->size; }
        done++;
        if (r->progress) r->progress(r->progress_ctx, done, FILE_COUNT);
    }
    seasons_zip_close(&z);
    for (int k = 0; k < FILE_COUNT; k++) {
        if (!seen[k]) {
            say(r, "The file for %s lacks %s.", row->name, data_files[k]);
            return -1;
        }
    }
    if (club_size != row->club_size || staff_size != row->staff_size || strcmp(club_hex, row->club_sha256) ||
        strcmp(staff_hex, row->staff_sha256)) {
        say(r, "The data in the file for %s is not the expected season database.", row->name, "");
        return -1;
    }
    return 0;
}

SeasonsPrepareStatus seasons_db_prepare_in(const char *game_dir, const char *save_dir, int year, SeasonsPrepare *r)
{
    void (*progress)(void *, int, int) = r->progress;
    void *progress_ctx = r->progress_ctx;
    memset(r, 0, sizeof *r);
    r->progress = progress;
    r->progress_ctx = progress_ctx;
    if (year < SEASONS_FIRST || year >= SEASONS_FIRST + SEASONS_COUNT) {
        r->status = SEASONS_PREPARE_NO_DATABASE;
        snprintf(r->error, sizeof r->error, "There is no such season.");
        return r->status;
    }
    if (year <= 2002) return r->status = SEASONS_PREPARE_OFFICIAL;
    const SeasonKnownDb *row = seasons_db_for_season(year);
    if (!row) {
        r->status = SEASONS_PREPARE_NO_DATABASE;
        snprintf(r->error, sizeof r->error, "No database exists for this season yet.");
        return r->status;
    }

    /* 1. A folder of Databases that holds this database. The exact name wins. */
    static char names[DBFOLDER_MAX][DBFOLDER_NAME];
    int n = dbfolders_list(save_dir, names, DBFOLDER_MAX), pick = -1;
    for (int i = 0; i < n; i++) {
        char path[1100];
        if (!dbfolders_path(save_dir, names[i], path, sizeof path) || seasons_db_identify(path) != row) continue;
        if (pick < 0 || !strcmp(names[i], row->name)) pick = i;
    }
    if (pick >= 0) {
        snprintf(r->name, sizeof r->name, "%s", names[pick]);
        dbfolders_path(save_dir, r->name, r->path, sizeof r->path);
        return r->status = SEASONS_PREPARE_OK;
    }

    /* 2. Unpack the zip of the download, once, into a temporary folder, then rename it. */
    char zip[1100];
    HostStat st;
    if (!zip_path(game_dir, row, zip, sizeof zip) || host_stat(zip, &st) || st.is_dir) {
        r->status = SEASONS_PREPARE_NO_ZIP;
        say(r, "The game folder has no file for the season database %s. Install the game again.", row->name, "");
        return r->status;
    }
    char base[1100], tmp[1200], dbs[1100];
    snprintf(base, sizeof base, "%s/unpack-tmp", save_dir);
    snprintf(tmp, sizeof tmp, "%s/%s", base, row->name);
    snprintf(dbs, sizeof dbs, "%s/Databases", save_dir);
    r->status = SEASONS_PREPARE_FAILED;
    if (!exists_dir(base)) host_mkdir(base);
    remove_flat_folder(tmp);                      /* what an earlier unpack left, if the game stopped in it */
    if (host_mkdir(tmp)) {
        say(r, "Could not make a folder for %s in the save folder.", row->name, "");
        return r->status;
    }
    if (unpack_zip(zip, tmp, row, r)) {
        remove_flat_folder(tmp);
        return r->status;
    }
    if (!exists_dir(dbs)) host_mkdir(dbs);
    char dest[1200];
    for (int k = 1; k < 10; k++) {                /* never replace a folder of the player */
        if (k == 1) snprintf(r->name, sizeof r->name, "%s", row->name);
        else snprintf(r->name, sizeof r->name, "%s (%d)", row->name, k);
        snprintf(dest, sizeof dest, "%s/%s", dbs, r->name);
        HostStat ds;
        if (host_stat(dest, &ds)) break;
        r->name[0] = 0;
    }
    if (!r->name[0] || host_rename(tmp, dest)) {
        remove_flat_folder(tmp);
        say(r, "Could not move the unpacked season database %s into the Databases folder.", row->name, "");
        return r->status;
    }
    snprintf(r->path, sizeof r->path, "%s", dest);
    r->unpacked = 1;
    return r->status = SEASONS_PREPARE_OK;
}

SeasonsPrepareStatus seasons_db_prepare(int start_year, SeasonsPrepare *r)
{
    return seasons_db_prepare_in(win_game_dir, win_databases_root(), start_year, r);
}
