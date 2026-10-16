/* Test of the custom data set folders (src/recomp_rt/win/dbfolders.c).
 * Usage: dbfolders_test   (it makes and removes its own temporary folder) */
#include "dbfolders.h"
#include "../../src/recomp_rt/host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int failures;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static const char *const files[] = {
    "city.dat", "club.dat", "club_comp.dat", "club_comp_history.dat", "colour.dat", "common_names.dat",
    "continent.dat", "first_names.dat", "index.dat", "nat_club.dat", "nation.dat", "nation_comp.dat",
    "nation_comp_history.dat", "officials.dat", "second_names.dat", "stadium.dat", "staff.dat",
    "staff_comp.dat", "staff_comp_history.dat", "staff_history.dat"};

static char root[1024];

static void touch(const char *dir, const char *name)
{
    char p[2048];
    snprintf(p, sizeof p, "%s/%s", dir, name);
    FILE *f = fopen(p, "w");
    if (f) { fputs("x", f); fclose(f); }
}

/* A folder under root with every data file except the one named skip (or none). */
static void make_set(const char *name, const char *skip, int upper)
{
    char d[2048];
    snprintf(d, sizeof d, "%s/%s", root, name);
    mkdir(d, 0755);
    for (size_t i = 0; i < sizeof files / sizeof *files; i++) {
        if (skip && !strcmp(skip, files[i])) continue;
        char n[64];
        snprintf(n, sizeof n, "%s", files[i]);
        if (upper) for (char *c = n; *c; c++) if (*c >= 'a' && *c <= 'z') *c -= 32;
        touch(d, n);
    }
}

static int has(char names[][DBFOLDER_NAME], int n, const char *want)
{
    for (int i = 0; i < n; i++) if (!strcmp(names[i], want)) return 1;
    return 0;
}

int main(void)
{
    char tmpl[] = "/tmp/dbfolders_test_XXXXXX";
    if (!mkdtemp(tmpl)) return 2;
    snprintf(root, sizeof root, "%s", tmpl);
    char dbs[1100];
    snprintf(dbs, sizeof dbs, "%s/Databases", root);
    mkdir(dbs, 0755);
    snprintf(root, sizeof root, "%s", dbs);

    make_set("May 2026", NULL, 0);                 /* spaces */
    make_set("Zo\xc3\xab Update", NULL, 0);        /* a non-ASCII letter (UTF-8) */
    make_set("Upper", NULL, 1);                    /* file names in capitals */
    make_set("No staff", "staff.dat", 0);          /* a data file is missing */
    char d[2048];
    snprintf(d, sizeof d, "%s/Empty", dbs);
    mkdir(d, 0755);
    snprintf(d, sizeof d, "%s/Notes", dbs);
    mkdir(d, 0755);
    touch(d, "readme.txt");
    touch(dbs, "stray.dat");                       /* a file, not a folder */

    char names[DBFOLDER_MAX][DBFOLDER_NAME];
    int n = dbfolders_list(tmpl, names, DBFOLDER_MAX);
    CHECK(n == 3);
    CHECK(has(names, n, "May 2026"));
    CHECK(has(names, n, "Zo\xc3\xab Update"));
    CHECK(has(names, n, "Upper"));
    CHECK(!has(names, n, "No staff"));
    CHECK(!has(names, n, "Empty"));
    CHECK(!has(names, n, "Notes"));
    CHECK(!has(names, n, "stray.dat"));
    /* The dated folder comes first, then the others by name, ignoring case. */
    CHECK(n == 3 && !strcmp(names[0], "May 2026") && !strcmp(names[1], "Upper"));

    CHECK(dbfolders_valid(dbs) == 0);
    snprintf(d, sizeof d, "%s/May 2026", dbs);
    CHECK(dbfolders_valid(d) == 1);
    CHECK(dbfolders_path(tmpl, "May 2026", d, sizeof d) == 1);
    CHECK(strstr(d, "Databases/May 2026") != NULL);
    CHECK(dbfolders_path(tmpl, "No staff", d, sizeof d) == 0);
    CHECK(dbfolders_path(tmpl, "../Databases/May 2026", d, sizeof d) == 0);   /* no path parts in a name */
    CHECK(dbfolders_path(tmpl, "gone", d, sizeof d) == 0);

    /* Dated folders in the list come in date order, before the names with no date (Q174). */
    {
        char od[] = "/tmp/dbfolders_order_XXXXXX";
        if (!mkdtemp(od)) return 2;
        char odb[1100];
        snprintf(odb, sizeof odb, "%s/Databases", od);
        mkdir(odb, 0755);
        char saved[1024];
        snprintf(saved, sizeof saved, "%s", root);
        snprintf(root, sizeof root, "%s", odb);
        static const char *const made[] = {"May 2026", "Original 3.9.68", "October 2014", "Autumn 2013"};
        for (int i = 0; i < 4; i++) make_set(made[i], NULL, 0);
        static char got[DBFOLDER_MAX][DBFOLDER_NAME];
        int c = dbfolders_list(od, got, DBFOLDER_MAX);
        CHECK(c == 4);
        CHECK(c == 4 && !strcmp(got[0], "Autumn 2013") && !strcmp(got[1], "October 2014") &&
              !strcmp(got[2], "May 2026") && !strcmp(got[3], "Original 3.9.68"));
        static char two[2][DBFOLDER_NAME];
        CHECK(dbfolders_list(od, two, 2) == 2);
        CHECK(!strcmp(two[0], "Autumn 2013") && !strcmp(two[1], "October 2014"));
        for (int i = 0; i < 4; i++) {
            char dir[1300];
            snprintf(dir, sizeof dir, "%s/%s", odb, made[i]);
            for (size_t f = 0; f < sizeof files / sizeof *files; f++) {
                char fp[1500];
                snprintf(fp, sizeof fp, "%s/%s", dir, files[f]);
                unlink(fp);
            }
            rmdir(dir);
        }
        rmdir(odb);
        rmdir(od);
        snprintf(root, sizeof root, "%s", saved);
    }

    /* No Databases folder: an empty list. */
    char none[] = "/tmp/dbfolders_none_XXXXXX";
    if (!mkdtemp(none)) return 2;
    CHECK(dbfolders_list(none, names, DBFOLDER_MAX) == 0);
    rmdir(none);

    /* 49 folders (the Update Team zips): all are listed, in order, and a smaller list keeps the first by name. */
    {
        char big[] = "/tmp/dbfolders_big_XXXXXX";
        if (!mkdtemp(big)) return 2;
        char bdb[1100];
        snprintf(bdb, sizeof bdb, "%s/Databases", big);
        mkdir(bdb, 0755);
        char saved[1024];
        snprintf(saved, sizeof saved, "%s", root);
        snprintf(root, sizeof root, "%s", bdb);
        for (int i = 49; i >= 1; i--) {         /* made in reverse, so the disk order is not the name order */
            char nm[32];
            snprintf(nm, sizeof nm, "Update %02d", i);
            make_set(nm, NULL, 0);
        }
        static char all[DBFOLDER_MAX][DBFOLDER_NAME];
        int count = dbfolders_list(big, all, DBFOLDER_MAX);
        CHECK(count == 49);
        for (int i = 0; i < count; i++) {
            char nm[32];
            snprintf(nm, sizeof nm, "Update %02d", i + 1);
            CHECK(!strcmp(all[i], nm));
        }
        static char few[10][DBFOLDER_NAME];
        CHECK(dbfolders_list(big, few, 10) == 10);
        for (int i = 0; i < 10; i++) {
            char nm[32];
            snprintf(nm, sizeof nm, "Update %02d", i + 1);
            CHECK(!strcmp(few[i], nm));
        }
        for (int i = 1; i <= 49; i++) {
            char dir[1300];
            snprintf(dir, sizeof dir, "%s/Update %02d", bdb, i);
            for (size_t f = 0; f < sizeof files / sizeof *files; f++) {
                char fp[1500];
                snprintf(fp, sizeof fp, "%s/%s", dir, files[f]);
                unlink(fp);
            }
            rmdir(dir);
        }
        rmdir(bdb);
        rmdir(big);
        snprintf(root, sizeof root, "%s", saved);
    }

    /* The button text: code page 1252, "e with diaeresis" is the byte 0xeb. */
    char text[64];
    dbfolders_label("Zo\xc3\xab Update", text, sizeof text);
    CHECK(!strcmp(text, "Zo\xeb Update"));
    dbfolders_label("A very long folder name that goes on and on", text, 20);
    CHECK(strlen(text) == 19);

    /* The room for the name grows with the button (0.4.0 slice 10): 23 letters in classic, as before. */
    CHECK(dbfolders_label_room(800) == 24);
    CHECK(dbfolders_label_room(1280) == 38);
    CHECK(dbfolders_label_room(1920) == 57);
    dbfolders_label("Update Team May 2026 Championship Edition", text, dbfolders_label_room(1920));
    CHECK(!strcmp(text, "Update Team May 2026 Championship Edition"));

    /* The date in a folder name (Q174): a month (or Spring or Autumn) and a four-digit year. */
    {
        int y = 0, m = 0;
        CHECK(dbfolders_date("May 2026", &y, &m) == 1 && y == 2026 && m == 5);
        CHECK(dbfolders_date("October 2003 Extra", &y, &m) == 1 && y == 2003 && m == 10);
        CHECK(dbfolders_date("march 2012 odb", &y, &m) == 1 && y == 2012 && m == 3);
        CHECK(dbfolders_date("Autumn 2013", &y, &m) == 1 && y == 2013 && m == 10);
        CHECK(dbfolders_date("Sep 2008", &y, &m) == 1 && y == 2008 && m == 9);
        CHECK(dbfolders_date("Original 3.9.68", &y, &m) == 0);
        CHECK(dbfolders_date("EmptyDatabase", &y, &m) == 0);
        CHECK(dbfolders_date("May", &y, &m) == 0);
        CHECK(dbfolders_date("2026", &y, &m) == 0);
        CHECK(dbfolders_date("Mayday 2026", &y, &m) == 0);
        CHECK(dbfolders_date("May 20260", &y, &m) == 0);
    }

    /* The season: July to December starts it, January to June ends it. */
    {
        char s[16];
        CHECK(dbfolders_season("May 2026", s, sizeof s) == 1 && !strcmp(s, "2025/26"));
        CHECK(dbfolders_season("October 2014", s, sizeof s) == 1 && !strcmp(s, "2014/15"));
        CHECK(dbfolders_season("Autumn 2013", s, sizeof s) == 1 && !strcmp(s, "2013/14"));
        CHECK(dbfolders_season("June 2010", s, sizeof s) == 1 && !strcmp(s, "2009/10"));
        CHECK(dbfolders_season("July 2010", s, sizeof s) == 1 && !strcmp(s, "2010/11"));
        CHECK(dbfolders_season("December 1999", s, sizeof s) == 1 && !strcmp(s, "1999/00"));
        CHECK(dbfolders_season("Original 3.9.68", s, sizeof s) == 0);
    }

    /* The order: by date, oldest first (February before April in one season), a tie by name, and names
     * with no date last, by name. */
    {
        static char v[8][DBFOLDER_NAME] = {"May 2026", "Original 3.9.68", "April 2010 SIM", "December 2025",
                                           "EmptyDatabase", "February 2010 ODB", "March 2012 SIM", "March 2012 ODB"};
        dbfolders_sort(v, 8);
        static const char *const want[] = {"February 2010 ODB", "April 2010 SIM", "March 2012 ODB", "March 2012 SIM",
                                           "December 2025", "May 2026", "EmptyDatabase", "Original 3.9.68"};
        for (int i = 0; i < 8; i++) CHECK(!strcmp(v[i], want[i]));
    }

    /* The button text: the season and the folder name, or the season alone when both do not fit. */
    {
        char t[DBFOLDER_NAME];
        dbfolders_button_text("May 2026", t, dbfolders_label_room(800));
        CHECK(!strcmp(t, "2025/26 (May 2026)"));
        dbfolders_button_text("February 2010 ODB", t, dbfolders_label_room(800));
        CHECK(!strcmp(t, "2009/10"));
        dbfolders_button_text("February 2010 ODB", t, dbfolders_label_room(1280));
        CHECK(!strcmp(t, "2009/10 (February 2010 ODB)"));
        dbfolders_button_text("Original 3.9.68", t, dbfolders_label_room(800));
        CHECK(!strcmp(t, "Original 3.9.68"));
        dbfolders_button_text("Original 3.9.60 Swedish_fixed", t, dbfolders_label_room(800));
        CHECK(strlen(t) == 23);
    }

    /* Remove the test folders: only the names made above. */
    static const char *const sets[] = {"May 2026", "Zo\xc3\xab Update", "Upper", "No staff"};
    for (size_t s = 0; s < 4; s++) {
        snprintf(d, sizeof d, "%s/%s", dbs, sets[s]);
        for (size_t i = 0; i < sizeof files / sizeof *files; i++) {
            char f[2200], n2[64];
            snprintf(n2, sizeof n2, "%s", files[i]);
            if (s == 2) for (char *c = n2; *c; c++) if (*c >= 'a' && *c <= 'z') *c -= 32;
            snprintf(f, sizeof f, "%s/%s", d, n2);
            unlink(f);
        }
        rmdir(d);
    }
    snprintf(d, sizeof d, "%s/Notes/readme.txt", dbs);
    unlink(d);
    snprintf(d, sizeof d, "%s/Notes", dbs);
    rmdir(d);
    snprintf(d, sizeof d, "%s/Empty", dbs);
    rmdir(d);
    snprintf(d, sizeof d, "%s/stray.dat", dbs);
    unlink(d);
    rmdir(dbs);
    rmdir(tmpl);
    if (failures) { printf("%d failures\n", failures); return 1; }
    printf("dbfolders: ok\n");
    return 0;
}
