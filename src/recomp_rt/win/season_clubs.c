/* Club moves at a new game (season_clubs.h). The table lives in host memory. The moves change only the
 * division pointer (+0x57) of club records in the game's memory, and only in a season game. */
#include "win.h"
#include "season.h"
#include "season_clubs.h"
#include "season_names.h"
#include "season_rules.h"
#include "../host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT 1
#define MAX_ROWS 200000
#define MAX_BYTES (32u << 20)

#define CLUBS 0xae23b4u             /* the club table (0x245 bytes a club), clubs then national teams */
#define CLUB_COUNT 0xae235cu
#define NATION_COUNT 0xae2350u      /* the last 2 x nations rows are national teams (0x53b900) */
#define CLUB_SIZE 0x245u
#define CLUB_NAME 4u                /* long name, 52 bytes */
#define CLUB_NATION 0x53u           /* nation pointer */
#define CLUB_DIVISION 0x57u         /* club_comp pointer */
#define CLUB_LAST_DIVISION 0x5bu    /* club_comp pointer: the division of the last season */
#define COMPS 0xae23d0u             /* the club_comp table (0x6b bytes a record) */
#define COMP_COUNT 0xae2378u
#define COMP_SIZE 0x6bu
#define COMP_NAME 4u
#define COMP_NATION 0x5du           /* nation pointer */
#define COMP_RANK 0x69u             /* s16: a higher division has a higher value (English Premier 18, Third 4) */
#define KEY_LOWER 0x9cf764u         /* "A Lower Division" (0x60efd0 at 0x610141) */
#define KEY_PREMIER 0x9cf6b8u       /* "A Premier Division" (0x60efd0 at 0x60fb7f) */

typedef struct {
    int season, club_id;
    const char *division, *club, *real;
} ClubRow;

static char *text;
static ClubRow *rows;
static int nrows;

static int fail(const char *path, int line, const char *why)
{
    fprintf(stderr, "recomp: club moves table %s, line %d: %s; no club moves\n", path, line, why);
    free(text);
    free(rows);
    text = NULL;
    rows = NULL;
    nrows = 0;
    return 0;
}

static char *end_line(char *line)
{
    char *nl = strchr(line, '\n');
    if (!nl) return NULL;
    *nl = 0;
    if (nl > line && nl[-1] == '\r') nl[-1] = 0;
    return nl;
}

static int split(char *line, char **out, int n)
{
    int k = 0;
    out[k++] = line;
    for (char *p = line; *p; p++)
        if (*p == '\t') {
            if (k == n) return 0;
            *p = 0;
            out[k++] = p + 1;
        }
    return k == n;
}

static int number(const char *s, int lo, int hi)
{
    if (!*s) return -1;
    long v = 0;
    for (const char *p = s; *p; p++) {
        if (*p < '0' || *p > '9') return -1;
        v = v * 10 + (*p - '0');
        if (v > hi) return -1;
    }
    return v < lo ? -1 : (int)v;
}

int season_clubs_load(const char *path)
{
    free(text);
    free(rows);
    text = NULL;
    rows = NULL;
    nrows = 0;
    FILE *f = host_fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0 || size > (long)MAX_BYTES) { fclose(f); return fail(path, 0, "the file is too large"); }
    text = malloc((size_t)size + 1);
    if (!text) { fclose(f); return fail(path, 0, "no memory"); }
    size_t got = fread(text, 1, (size_t)size, f);
    fclose(f);
    if (got != (size_t)size) return fail(path, 0, "a read failed");
    text[size] = 0;
    char *line = text, *next = end_line(line);
    int want = -1, version = 0, lineno = 1;
    if (!next || sscanf(line, "#bosman-clubs\t%d\t%d", &version, &want) != 2 || version != FORMAT || want < 0 ||
        want > MAX_ROWS)
        return fail(path, 1, "not a club moves table of format 1");
    line = next + 1;
    lineno = 2;
    next = end_line(line);
    if (!next || strcmp(line, "season\tdivision\tclub_id\tclub\treal"))
        return fail(path, 2, "the columns are not those of format 1");
    rows = calloc((size_t)want + 1, sizeof *rows);
    if (!rows) return fail(path, 0, "no memory");
    for (line = next + 1; *line; line = next + 1) {
        lineno++;
        next = end_line(line);
        if (!next) return fail(path, lineno, "the last line has no end");
        if (nrows == want) return fail(path, lineno, "more rows than line 1 says");
        char *c[5];
        if (!split(line, c, 5)) return fail(path, lineno, "a wrong number of columns");
        ClubRow *r = &rows[nrows];
        r->season = number(c[0], SEASON_RULE_FIRST, SEASON_RULE_LAST);
        r->division = c[1];
        r->club_id = number(c[2], 0, 10000000);
        r->club = c[3];
        r->real = c[4];
        if (r->season < 0 || r->club_id < 0 || !*r->division || !*r->club || strlen(r->division) > 51 ||
            strlen(r->club) > 51)
            return fail(path, lineno, "a bad season, club id or name");
        nrows++;
    }
    if (nrows != want) return fail(path, lineno, "fewer rows than line 1 says");
    return nrows;
}

int season_clubs_init(const char *game_dir)
{
    char path[1200];
    season_data_path(game_dir, "clubs.tsv", path, sizeof path);
    int n = season_clubs_load(path);
    if (n) fprintf(stderr, "recomp: club moves table: %d clubs from %s\n", n, path);
    else fprintf(stderr, "recomp: club moves table: none (%s)\n", path);
    return n;
}

/* One club of the game. */
typedef struct {
    uint32_t at, nation, db_div, div;
    int id, national;
    int kept;                       /* 1 when a list keeps it in its division */
} Club;

/* One division of the game that a list or a fill touches. */
typedef struct {
    uint32_t at, nation;
    int rank, n0, target, spare, sized;
    int listed, found, kept, filled;
    const char *name;
} Division;

static int same_name(uint32_t at, const char *name)
{
    return !strncmp(win_str(at), name, 52);
}

static int cmp_rank(const void *a, const void *b)
{
    const Division *x = a, *y = b;
    if (x->rank != y->rank) return y->rank - x->rank;
    return x->at < y->at ? -1 : x->at > y->at;
}

/* Divisions whose class takes a count in a later season that differs from its count in 2001/02, which the
 * season databases keep (the size branches of the classes on +0x40, the season year):
 *   FraFirst::create_table 0x5bc2b0: 18 when +0x40 is 2001 (0x5bc2bd..0x5bc2c5), else 20 (0x5bc2cd). With 18
 *     clubs a later season stops at the assert fra_first.cpp 108 (FraFirst::FraFirst 0x5bb3ad).
 *   PolSecond::create_table 0x7cbdf0: 20 when +0x40 is 2001 (0x7cbdfd..0x7cbe03), else 18 (0x7cbe14).
 * The other classes with a year branch on the count take one count in every start year from 2001:
 * GerRegional 0x5dd7f0 (19 only in 2000, else 18) and ScoPrm 0x7f2820 (10 only in 1999, else 12). NorFirst
 * 0x7909a0 takes 14 when the data gives 14, else 16. The real count of each season is slice 18. */
static const struct { const char *name; int size; } LATER_SIZE[] = {
    {"French First Division", 20},
    {"Polish Second Division", 18},
};

/* The moves of a season. */
static void move_clubs(int season)
{
    int first = -1, last = -1;
    for (int i = 0; i < nrows; i++)
        if (rows[i].season == season) {
            if (first < 0) first = i;
            last = i;
        }
    if (first < 0) first = 0, last = -1;        /* no list: only the sizes of LATER_SIZE */
    int log = getenv("RECOMP_CLUB_LOG") && *getenv("RECOMP_CLUB_LOG");
    uint32_t table = rd32(g_mem, CLUBS), comps = rd32(g_mem, COMPS);
    int count = (int)rd32(g_mem, CLUB_COUNT), ncomps = (int)rd32(g_mem, COMP_COUNT);
    int national_from = count - 2 * (int)rd32(g_mem, NATION_COUNT);
    if (!table || !comps || count <= 0 || ncomps <= 0) return;
    int32_t key_lower = (int32_t)rd32(g_mem, KEY_LOWER), key_premier = (int32_t)rd32(g_mem, KEY_PREMIER);
    uint32_t spare1 = key_lower >= 0 ? comps + (uint32_t)key_lower * COMP_SIZE : 0;
    uint32_t spare2 = key_premier >= 0 ? comps + (uint32_t)key_premier * COMP_SIZE : 0;

    Club *club = calloc((size_t)count, sizeof *club);
    Division *div = calloc((size_t)ncomps, sizeof *div);
    int *by_list = calloc((size_t)(last - first + 2), sizeof *by_list);
    if (!club || !div || !by_list) {
        free(club);
        free(div);
        free(by_list);
        return;
    }
    for (int i = 0; i < count; i++) {
        Club *c = &club[i];
        c->at = table + (uint32_t)i * CLUB_SIZE;
        c->id = (int)rd32(g_mem, c->at);
        c->nation = rd32(g_mem, c->at + CLUB_NATION);
        c->db_div = c->div = rd32(g_mem, c->at + CLUB_DIVISION);
        c->national = c->id >= national_from;
    }
    /* The divisions: every club_comp record, with its club count in the database. */
    for (int i = 0; i < ncomps; i++) {
        Division *d = &div[i];
        d->at = comps + (uint32_t)i * COMP_SIZE;
        d->nation = rd32(g_mem, d->at + COMP_NATION);
        d->rank = (int16_t)rd16(g_mem, d->at + COMP_RANK);
        d->spare = d->at == spare1 || d->at == spare2;
        d->name = win_str(d->at + COMP_NAME);
    }
    for (int i = 0; i < count; i++)
        if (club[i].db_div >= comps && club[i].db_div < comps + (uint32_t)ncomps * COMP_SIZE)
            div[(club[i].db_div - comps) / COMP_SIZE].n0++;
    /* A division whose class takes another count after the first season of the original (LATER_SIZE) gets
     * that count when the start year is not 2001: the database keeps the 2001/02 count. */
    int resized = 0;
    for (int i = 0; i < ncomps && season != SEASON_FIRST_YEAR; i++)
        for (size_t k = 0; k < sizeof LATER_SIZE / sizeof LATER_SIZE[0]; k++)
            if (div[i].n0 > 0 && same_name(div[i].at + COMP_NAME, LATER_SIZE[k].name) && div[i].n0 != LATER_SIZE[k].size) {
                if (log) fprintf(stderr, "recomp: club moves: %s takes %d clubs, the database gives it %d\n",
                                 div[i].name, LATER_SIZE[k].size, div[i].n0);
                div[i].n0 = LATER_SIZE[k].size;
                div[i].sized = 1;
                resized++;
            }

    /* The lists: find each club and mark the target divisions. by_list holds the club index or -1. */
    int unknown_division = 0, unmatched = 0;
    for (int r = first; r <= last; r++) {
        by_list[r - first] = -1;
        if (rows[r].season != season) continue;
        int t;
        for (t = 0; t < ncomps && !(div[t].n0 > 0 && !div[t].spare && same_name(div[t].at + COMP_NAME, rows[r].division)); t++) {}
        if (t == ncomps) {
            unknown_division++;
            continue;
        }
        div[t].target = 1;
        div[t].listed++;
        int k = -1;
        for (int i = 0; i < count && k < 0; i++)
            if (club[i].id == rows[r].club_id && !club[i].national && club[i].nation == div[t].nation &&
                same_name(club[i].at + CLUB_NAME, rows[r].club))
                k = i;
        for (int i = 0; i < count && k < 0; i++)
            if (!club[i].national && club[i].nation == div[t].nation && same_name(club[i].at + CLUB_NAME, rows[r].club))
                k = i;
        if (k < 0) {
            unmatched++;
            if (log) fprintf(stderr, "recomp: club moves: %s (%s) of %s is not in this database\n", rows[r].club,
                             rows[r].real, rows[r].division);
            continue;
        }
        div[t].found++;
        by_list[r - first] = k;
    }
    /* Keep: in each target, the listed clubs that the database has there first, then the others, up to the
     * database count. A club in two lists keeps the first. */
    for (int pass = 0; pass < 2; pass++)
        for (int r = first; r <= last; r++) {
            int k = by_list[r - first];
            if (k < 0 || club[k].kept) continue;
            int t;
            for (t = 0; t < ncomps && !(div[t].target && same_name(div[t].at + COMP_NAME, rows[r].division)); t++) {}
            if (t == ncomps) continue;
            if ((pass == 0) != (club[k].db_div == div[t].at)) continue;
            if (div[t].kept >= div[t].n0) continue;
            club[k].kept = 1;
            club[k].div = div[t].at;
            div[t].kept++;
        }
    /* Leave: a club in a target that no list keeps there. */
    int left = 0;
    for (int i = 0; i < count; i++) {
        Club *c = &club[i];
        if (c->kept || c->national || !c->db_div) continue;
        int t = (int)((c->db_div - comps) / COMP_SIZE);
        if (c->db_div >= comps && t < ncomps && div[t].target) {
            c->div = 0;
            left++;
        }
    }
    /* Fill the divisions of the nations of the lists, from the highest rank down. */
    Division **order = calloc((size_t)ncomps, sizeof *order);
    int norder = 0;
    if (order) {
        for (int i = 0; i < ncomps; i++) {
            if (div[i].n0 <= 0 || div[i].spare) continue;
            int nation_listed = 0;
            for (int j = 0; j < ncomps && !nation_listed; j++)
                nation_listed = (div[j].target || div[j].sized) && div[j].nation == div[i].nation;
            if (nation_listed) order[norder++] = &div[i];
        }
        /* Sort a copy of the pointers by rank: qsort on the Division structs would move them. */
        Division *sorted = calloc((size_t)(norder ? norder : 1), sizeof *sorted);
        if (sorted) {
            for (int i = 0; i < norder; i++) sorted[i] = *order[i];
            qsort(sorted, (size_t)norder, sizeof *sorted, cmp_rank);
            for (int i = 0; i < norder; i++) order[i] = &div[(sorted[i].at - comps) / COMP_SIZE];
            free(sorted);
        }
    }
    int short_after = 0;
    for (int o = 0; o < norder; o++) {
        Division *d = order[o];
        int now = 0;
        for (int i = 0; i < count; i++) now += club[i].div == d->at;
        for (int group = 0; group < 4 && now < d->n0; group++)
            for (int i = 0; i < count && now < d->n0; i++) {
                Club *c = &club[i];
                if (c->kept || c->national || c->nation != d->nation || c->div == d->at) continue;
                int take = 0;
                if (group == 0) take = !c->div && c->db_div == d->at;
                else if (group == 1) take = !c->div;
                else if (group == 2) take = c->div == spare1 || c->div == spare2;
                else if (c->div >= comps && c->div < comps + (uint32_t)ncomps * COMP_SIZE) {
                    Division *from = &div[(c->div - comps) / COMP_SIZE];
                    take = !from->spare && from->rank < d->rank;
                }
                if (!take) continue;
                c->div = d->at;         /* a lower division that gives a club fills in its turn, later */
                now++;
                d->filled++;
            }
        if (now < d->n0) {
            short_after++;
            fprintf(stderr, "recomp: club moves: %s holds %d clubs, the database gave it %d\n", d->name, now, d->n0);
        }
    }
    /* Write the moves. */
    int moved = 0;
    for (int i = 0; i < count; i++)
        if (club[i].div != club[i].db_div) {
            wr32(g_mem, club[i].at + CLUB_DIVISION, club[i].div);
            /* The division of the last season (+0x5b, resolve_database_indexes 0x5315a0 at 0x531657) becomes the
             * database division: the club comes from there. FraLgeCup::collect_entrants 0x5bd160 takes 14 French
             * National clubs whose last division is set and is not CFA (0x5bd222..0x5bd249) and stops at
             * fra_lge_cup.cpp 73 (0x5bca3a) without 54 entrants. A club that had no division keeps its value. */
            if (club[i].db_div) wr32(g_mem, club[i].at + CLUB_LAST_DIVISION, club[i].db_div);
            moved++;
        }
    int lists = 0, whole = 0;
    for (int t = 0; t < ncomps; t++) {
        if (!div[t].target) continue;
        lists++;
        whole += div[t].found == div[t].listed && div[t].kept == div[t].listed && div[t].listed == div[t].n0;
        if (log)
            fprintf(stderr, "recomp: club moves: %s: %d real clubs, %d in this database, %d kept, %d filled, the "
                            "division holds %d\n", div[t].name, div[t].listed, div[t].found, div[t].kept,
                    div[t].filled, div[t].n0);
    }
    fprintf(stderr, "recomp: club moves of %d/%02d: %d divisions with a list (%d whole), %d clubs moved, %d left "
                    "their division, %d listed clubs not in this database, %d divisions not in it, %d divisions "
                    "short, %d divisions resized\n", season, (season + 1) % 100, lists, whole, moved, left, unmatched,
            unknown_division, short_after, resized);
    free(order);
    free(club);
    free(div);
    free(by_list);
}

void o_00524d70(void);

/* database_trim_staff_tables 0x524d70, cdecl (char database_size): called once, at 0x828389 in create_game,
 * after the load (0x8282a7) and before the trim of the club table (0x5267c0 at 0x828395). The moves run
 * before it, on the whole data of every nation: the trims then keep the clubs and staff of the moved data, as
 * they would for a database that held the real divisions. (After the trims the clubs of the nations that the
 * player did not select are cut down to a few per nation, so a later point would see broken divisions.) */
RT_WRAP(00524d70, trim_staff_tables)
{
    const char *off = getenv("RECOMP_CLUB_MOVES");     /* test switch: off moves no club */
    if (season_game_changes() && nrows && !(off && !strcmp(off, "off"))) move_clubs(season_start_year());
    o_00524d70();
}
