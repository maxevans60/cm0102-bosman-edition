/* Right ages and dates in every season (0.6.0 slice 11b, Q344): the year shifts at the database load
 * (season_dates.h). Evidence and measures: docs/research/2026-10-17-season-date-shifts.md.
 *
 * Memory after the load 0x528540 (the loader's globals; the record layouts of include/database.h):
 *   index entries  0xae1a80, 22 rows of 0x43 bytes: +0x33 table id, +0x37 row count of the file
 *   staff          rows at [0xae23bc], 0x6e bytes, count [0xae2364]: the file rows first, then the reserve rows
 *   officials      rows at [0xae23e8], 0x2b bytes, count [0xae2390]
 *   staff history  rows at [0xae23cc], 0x11 bytes, count [0xae2374]; +0x04 the staff row (a pointer after the
 *                  id pass 0x5315a0), +0x08 u16 year
 *   comp history   rows at [0xae23d4], 0x1a bytes, count [0xae237c]: the club competition rows, then the nation
 *                  competition rows ([0xae23dc] points at the first of those); +0x04 the competition (a pointer
 *                  into the competitions at [0xae23d0], 0x6b bytes, club then nation competitions), +0x08 year
 * A date is 8 bytes: s16 day of the year, u16 year, u32 leap flag (Date.cpp, include/date.h). The year 1900
 * marks an empty date in the data; it stays. Only the rows of the files change, never the reserve rows. */
#include "win.h"
#include "season.h"
#include "season_dates.h"
#include "seasons_db.h"
#include "../host.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDEX 0xae1a80u
#define INDEX_ROWS 22u
#define INDEX_SIZE 0x43u
#define STAFF_PTR 0xae23bcu
#define STAFF_COUNT 0xae2364u
#define STAFF_SIZE 0x6eu
#define OFFICIALS_PTR 0xae23e8u
#define OFFICIALS_COUNT 0xae2390u
#define OFFICIAL_SIZE 0x2bu
#define HISTORY_PTR 0xae23ccu
#define HISTORY_COUNT 0xae2374u
#define HISTORY_SIZE 0x11u
#define COMP_HISTORY_PTR 0xae23d4u
#define COMP_HISTORY_COUNT 0xae237cu
#define NATION_HISTORY_PTR 0xae23dcu
#define COMP_HISTORY_SIZE 0x1au
#define COMPS_PTR 0xae23d0u
#define COMP_SIZE 0x6bu
#define CLUBS_PTR 0xae23b4u
#define CLUB_COUNT 0xae235cu
#define CLUB_SIZE 0x245u

#define T_STAFF 6
#define T_OFFICIALS 7
#define T_FIRST_NAMES 13
#define T_SECOND_NAMES 14
#define T_HISTORY 17
#define T_COMP_HISTORY 19
#define T_NATION_HISTORY 20

#define EMPTY_YEAR 1900u
#define LAST_ROW_YEAR 2001u        /* the history year where the Update Team databases keep the last champions */

static const SeasonDateShifts *applied;
static void log_load(void);
static SeasonDateShifts custom;     /* the shifts of a Custom database, measured at its load */

const SeasonDateShifts *season_dates_for(const char *name)
{
    for (int i = 0; name && i < season_date_shifts_count; i++)
        if (!strcmp(season_date_shifts[i].name, name)) return &season_date_shifts[i];
    return NULL;
}

const SeasonDateShifts *season_dates_applied(void) { return applied; }

/* The row count of a table in index.dat (the rows that came from the file). */
static uint32_t file_rows(int table)
{
    uint32_t n = 0;
    for (uint32_t i = 0; i < INDEX_ROWS; i++)
        if ((int32_t)rd32(g_mem, INDEX + i * INDEX_SIZE + 0x33) == table) n += rd32(g_mem, INDEX + i * INDEX_SIZE + 0x37);
    return n;
}

static uint32_t rows_of(int table, uint32_t count_at)
{
    uint32_t n = file_rows(table), have = rd32(g_mem, count_at);
    return n < have ? n : have;
}

/* Adds s to the year of the date at a, when the date is set. */
static void shift_date(uint32_t a, int s)
{
    uint32_t y = rd16(g_mem, a + 2);
    if (s && y > EMPTY_YEAR) wr16(g_mem, a + 2, y + (uint32_t)s);
}

static void shift_year(uint32_t a, int s)
{
    uint32_t y = rd16(g_mem, a);
    if (s && y > EMPTY_YEAR) wr16(g_mem, a, y + (uint32_t)s);
}

static int hexval(char c)
{
    return c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : 0;
}

/* 1 when the career of staff row i keeps the years of the data. */
static int career_kept(const SeasonDateShifts *s, uint32_t i)
{
    if (!s->kept_hex || i >= s->kept_rows) return 0;
    const char *p = s->kept_hex + (i >> 3) * 2;
    int byte = hexval(p[0]) << 4 | hexval(p[1]);
    return byte >> (i & 7) & 1;
}

static int last_row_comp(const SeasonDateShifts *s, uint32_t id)
{
    for (uint32_t i = 0; i < s->last_count; i++)
        if (s->last_ids[i] == id) return 1;
    return 0;
}

static void apply(const SeasonDateShifts *s)
{
    uint32_t staff = rd32(g_mem, STAFF_PTR), n_staff = rows_of(T_STAFF, STAFF_COUNT);
    for (uint32_t i = 0; staff && i < n_staff; i++) {
        uint32_t r = staff + i * STAFF_SIZE;
        shift_date(r + 0x10, s->birth);
        shift_date(r + 0x29, s->contract);
        shift_date(r + 0x31, s->contract);
        shift_date(r + 0x3e, s->joined);
        shift_date(r + 0x46, s->contract);
    }
    uint32_t off = rd32(g_mem, OFFICIALS_PTR), n_off = rows_of(T_OFFICIALS, OFFICIALS_COUNT);
    for (uint32_t i = 0; off && i < n_off; i++) shift_date(off + i * OFFICIAL_SIZE + 0x0c, s->officials);

    uint32_t hist = rd32(g_mem, HISTORY_PTR), n_hist = rows_of(T_HISTORY, HISTORY_COUNT);
    uint32_t staff_all = rd32(g_mem, STAFF_COUNT);
    for (uint32_t i = 0; hist && s->staff_history && i < n_hist; i++) {
        uint32_t r = hist + i * HISTORY_SIZE, who = rd32(g_mem, r + 4);
        uint32_t row = who >= staff && (who - staff) % STAFF_SIZE == 0 && (who - staff) / STAFF_SIZE < staff_all
                           ? (who - staff) / STAFF_SIZE : 0xffffffffu;
        if (!career_kept(s, row)) shift_year(r + 8, s->staff_history);
    }

    /* The club rows and the nation rows of one array. */
    uint32_t ch = rd32(g_mem, COMP_HISTORY_PTR);
    uint32_t n_ch = file_rows(T_COMP_HISTORY) + file_rows(T_NATION_HISTORY), have = rd32(g_mem, COMP_HISTORY_COUNT);
    if (n_ch > have) n_ch = have;
    uint32_t nation_at = rd32(g_mem, NATION_HISTORY_PTR);
    if (nation_at && nation_at != ch + file_rows(T_COMP_HISTORY) * COMP_HISTORY_SIZE)
        fprintf(stderr, "recomp: season dates: the nation competition history is not after the club rows "
                        "(%08x, %08x): only the club rows move\n", ch, nation_at), n_ch = file_rows(T_COMP_HISTORY);
    uint32_t comps = rd32(g_mem, COMPS_PTR);
    for (uint32_t i = 0; ch && i < n_ch; i++) {
        uint32_t r = ch + i * COMP_HISTORY_SIZE;
        if (s->comp_history) {
            shift_year(r + 8, s->comp_history);
        } else if (s->last_rows && rd16(g_mem, r + 8) == LAST_ROW_YEAR) {
            uint32_t c = rd32(g_mem, r + 4);
            if (c >= comps && (c - comps) % COMP_SIZE == 0 && last_row_comp(s, (c - comps) / COMP_SIZE))
                shift_year(r + 8, s->last_rows);
        }
    }
    fprintf(stderr, "recomp: season dates: %s: birth %+d, joined %+d, contract %+d, officials %+d, careers %+d "
                    "(%s kept), competition history %+d (last rows %+d of %u competitions); %u staff, %u careers, "
                    "%u history rows\n", s->name, s->birth, s->joined, s->contract, s->officials, s->staff_history,
            s->kept_hex ? "some" : "none", s->comp_history, s->last_rows, s->last_count, n_staff, n_hist, n_ch);
}

/* --- A Custom database: the birth shift measured against the game's own data (3.9.68, real dates). --- */

typedef struct { uint64_t key; int16_t year; } Birth;

static uint64_t fnv(uint64_t h, const uint8_t *p, size_t n)
{
    for (size_t i = 0; i < n && p[i]; i++) h = (h ^ p[i]) * 0x100000001b3ull;
    return h * 0x100000001b3ull;
}

static uint64_t birth_key(const uint8_t *first, const uint8_t *second, int16_t day)
{
    uint64_t h = fnv(0xcbf29ce484222325ull, first, 51);
    h = fnv(h, second, 51);
    h = (h ^ (uint16_t)day) * 0x100000001b3ull;
    return h | 1;                   /* 0 marks a free slot */
}

static uint8_t *read_file(const char *path, long *size)
{
    int fd = host_open(path, O_RDONLY, 0);
    if (fd < 0) return NULL;
    long n = (long)host_seek(fd, 0, SEEK_END);
    uint8_t *buf = n > 0 ? malloc((size_t)n) : NULL;
    if (buf && (host_seek(fd, 0, SEEK_SET) != 0 || host_read(fd, buf, (uint32_t)n) != n)) {
        free(buf);
        buf = NULL;
    }
    host_close(fd);
    *size = n;
    return buf;
}

#define BIRTH_SLOTS (1u << 19)
#define MEASURE_MIN 2000           /* matched people that a measure needs */

/* The most common year difference of the people (first name, second name, day of birth) that the loaded data
 * and the game's Data share. Returns 0 and leaves *shift when it cannot be measured. */
static int measure_birth(int *shift)
{
    char path[1300];
    long n_idx, n_staff, n_first, n_second;
    snprintf(path, sizeof path, "%s/Data/index.dat", win_game_dir);
    uint8_t *idx = read_file(path, &n_idx);
    snprintf(path, sizeof path, "%s/Data/staff.dat", win_game_dir);
    uint8_t *st = read_file(path, &n_staff);
    snprintf(path, sizeof path, "%s/Data/first_names.dat", win_game_dir);
    uint8_t *fn = read_file(path, &n_first);
    snprintf(path, sizeof path, "%s/Data/second_names.dat", win_game_dir);
    uint8_t *sn = read_file(path, &n_second);
    Birth *tab = calloc(BIRTH_SLOTS, sizeof *tab);
    int ok = 0;
    if (!idx || !st || !fn || !sn || !tab || n_idx < 8 + 22 * 0x43) goto done;
    uint32_t s_off = 0, s_cnt = 0, s_size = 0;
    for (int e = 0; e < 22; e++) {
        const uint8_t *p = idx + 8 + e * 0x43;
        int32_t id, cnt, at, fmt;
        memcpy(&id, p + 0x33, 4); memcpy(&cnt, p + 0x37, 4); memcpy(&at, p + 0x3b, 4); memcpy(&fmt, p + 0x3f, 4);
        if (id == T_STAFF) { s_off = (uint32_t)at; s_cnt = (uint32_t)cnt; s_size = fmt == 2 ? 0x6e : 0x9d; }
    }
    if (!s_cnt || (uint64_t)s_off + (uint64_t)s_cnt * s_size > (uint64_t)n_staff) goto done;
    uint32_t n_fn = (uint32_t)(n_first / 0x3c), n_sn = (uint32_t)(n_second / 0x3c);
    for (uint32_t i = 0; i < s_cnt; i++) {
        const uint8_t *r = st + s_off + (size_t)i * s_size;
        int32_t f, s; int16_t day; uint16_t year;
        memcpy(&f, r + 4, 4); memcpy(&s, r + 8, 4); memcpy(&day, r + 0x10, 2); memcpy(&year, r + 0x12, 2);
        if (year <= EMPTY_YEAR || f < 0 || s < 0 || (uint32_t)f >= n_fn || (uint32_t)s >= n_sn) continue;
        uint64_t k = birth_key(fn + (size_t)f * 0x3c, sn + (size_t)s * 0x3c, day);
        for (uint32_t h = (uint32_t)(k >> 20) & (BIRTH_SLOTS - 1);; h = (h + 1) & (BIRTH_SLOTS - 1)) {
            if (!tab[h].key) { tab[h].key = k; tab[h].year = (int16_t)year; break; }
            if (tab[h].key == k) { tab[h].year = -1; break; }      /* a name seen twice does not count */
        }
    }
    uint32_t hist[81] = {0}, total = 0;       /* differences -20 to +60 */
    uint32_t staff = rd32(g_mem, STAFF_PTR), n = rows_of(T_STAFF, STAFF_COUNT);
    for (uint32_t i = 0; staff && i < n; i++) {
        uint32_t r = staff + i * STAFF_SIZE, f = rd32(g_mem, r + 4), s = rd32(g_mem, r + 8);
        uint32_t year = rd16(g_mem, r + 0x12);
        if (!f || !s || year <= EMPTY_YEAR) continue;
        uint64_t k = birth_key(g_mem + f, g_mem + s, (int16_t)rd16(g_mem, r + 0x10));
        for (uint32_t h = (uint32_t)(k >> 20) & (BIRTH_SLOTS - 1); tab[h].key; h = (h + 1) & (BIRTH_SLOTS - 1)) {
            if (tab[h].key != k) continue;
            int d = tab[h].year - (int)year;
            if (tab[h].year > 0 && d >= -20 && d <= 60) { hist[d + 20]++; total++; }
            break;
        }
    }
    int best = 0;
    for (int d = 1; d < 81; d++) if (hist[d] > hist[best]) best = d;
    fprintf(stderr, "recomp: season dates: Custom database: %u people also in the game's data, %u of them %+d "
                    "years\n", total, hist[best], best - 20);
    if (total >= MEASURE_MIN && hist[best] * 10 >= total * 8) {
        *shift = best - 20;
        ok = 1;
    }
done:
    free(idx); free(st); free(fn); free(sn); free(tab);
    return ok;
}

/* The shifts of the database that the load read, or NULL. */
static const SeasonDateShifts *shifts_of_load(void)
{
    const char *dir = win_data_set_dir();
    if (!dir || !*dir) return NULL;                  /* the game's own Data: 3.9.68, shift 0 */
    const SeasonKnownDb *k = seasons_db_identify(dir);
    if (k) return season_dates_for(k->name);        /* a known database: the measured table */
    /* A Custom database (the rule in the research doc): the birth shift measured against the game's own data
     * applies to every field but the officials and the award history, as in the Update Team databases from
     * 2010/11 on. When it cannot be measured, nothing moves. */
    int d = 0;
    if (!measure_birth(&d) || !d) return NULL;
    memset(&custom, 0, sizeof custom);
    custom.name = "Custom database";
    custom.birth = custom.joined = custom.contract = custom.staff_history = custom.comp_history = (int8_t)d;
    return &custom;
}

static int season_code_off(void)
{
    const char *v = getenv("RECOMP_SEASON_CODE");
    return v && !strcmp(v, "off");
}

void o_00524d50(void);

/* The load of the database files, cdecl (shared memory flag, a second flag): 0x528540 with the file source.
 * Called by the creation of a new game (0x828110) and two other new game paths (0x81ce00, 0x827110); a saved
 * game loads through 0x524d10 (the stream source) and is not touched. Nonzero when the data is loaded. */
RT_WRAP(00524d50, database_load_files)
{
    o_00524d50();
    if (rt_unwinding) return;
    applied = NULL;
    if (!cpu.eax) return;
    uint32_t eax = cpu.eax;
    const SeasonDateShifts *s = season_start_year() == SEASON_FIRST_YEAR || season_code_off() ? NULL : shifts_of_load();
    if (s) {
        apply(s);
        applied = s;
    }
    log_load();
    cpu.eax = eax;
}

/* --- Test log: RECOMP_DATES_LOG=<first> <second>;...;comp:<competition name>;... ---
 * At the end of each database load: for each person of that name (file rows only), the first and last career
 * years. At the first human turn: the date of birth, the age that the game keeps (+0x18, set each day of the
 * run-up by 0x5cc9a0 from 0x53ab90), the years joined and contract end and the club; for each competition, its
 * last three history rows. It only reads memory. */

static const int16_t month_start[2][12] = {
    {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333},     /* 0x9b8448 */
    {-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},     /* 0x9b8460 */
};

static void day_month(uint32_t a, int *day, int *month)
{
    int16_t d = (int16_t)rd16(g_mem, a);
    int leap = rd32(g_mem, a + 4) != 0;
    int m = 11;
    while (m > 0 && d <= month_start[leap][m]) m--;
    *month = m + 1;
    *day = d - month_start[leap][m];
}

static void text_at(uint32_t a, char *out, size_t room)
{
    size_t i = 0;
    /* The log is plain ASCII: a letter of code page 1252 above 0x7f becomes '?'. */
    if (a) for (; i + 1 < room && g_mem[a + i]; i++) out[i] = g_mem[a + i] < 0x80 ? (char)g_mem[a + i] : '?';
    out[i] = 0;
}

/* The first and last career years of each staff row with this name. They are read at the load: at the first
 * turn the staff history rows no longer point at these staff rows (seen in the test; inference: the new game
 * builds its own career records from the table). */
static void log_careers(const char *want)
{
    uint32_t staff = rd32(g_mem, STAFF_PTR), n = rows_of(T_STAFF, STAFF_COUNT);
    uint32_t hist = rd32(g_mem, HISTORY_PTR), n_hist = rows_of(T_HISTORY, HISTORY_COUNT);
    for (uint32_t i = 0; staff && i < n; i++) {
        uint32_t r = staff + i * STAFF_SIZE;
        char first[64], second[64], full[140];
        text_at(rd32(g_mem, r + 4), first, sizeof first);
        text_at(rd32(g_mem, r + 8), second, sizeof second);
        snprintf(full, sizeof full, "%s %s", first, second);
        if (strcmp(full, want)) continue;
        int day, month, lo = 9999, hi = 0;
        day_month(r + 0x10, &day, &month);
        for (uint32_t h = 0; hist && h < n_hist; h++) {
            uint32_t hr = hist + h * HISTORY_SIZE;
            if (rd32(g_mem, hr + 4) != r) continue;
            int y = (int)rd16(g_mem, hr + 8);
            if (y < lo) lo = y;
            if (y > hi) hi = y;
        }
        fprintf(stderr, "recomp: dates log: career %s born %d.%d: %d to %d\n", full, day, month,
                lo == 9999 ? 0 : lo, hi);
    }
}

static void log_person(const char *want)
{
    uint32_t staff = rd32(g_mem, STAFF_PTR), n = rows_of(T_STAFF, STAFF_COUNT);
    uint32_t clubs = rd32(g_mem, CLUBS_PTR), n_clubs = rd32(g_mem, CLUB_COUNT);
    int found = 0;
    for (uint32_t i = 0; staff && i < n; i++) {
        uint32_t r = staff + i * STAFF_SIZE;
        char first[64], second[64], full[140];
        text_at(rd32(g_mem, r + 4), first, sizeof first);
        text_at(rd32(g_mem, r + 8), second, sizeof second);
        snprintf(full, sizeof full, "%s %s", first, second);
        if (strcmp(full, want)) continue;
        found++;
        int day, month;
        day_month(r + 0x10, &day, &month);
        char club[64] = "";
        uint32_t c = rd32(g_mem, r + 0x39);
        if (clubs && c >= clubs && c < clubs + n_clubs * CLUB_SIZE) text_at(c + 4, club, sizeof club);
        fprintf(stderr, "recomp: dates log: person %s: born %d.%d.%d, age %d, joined %d, contract %d, club %s\n",
                full, day, month, (int)rd16(g_mem, r + 0x12), (int16_t)rd16(g_mem, r + 0x18),
                (int)rd16(g_mem, r + 0x40), (int)rd16(g_mem, r + 0x48), club[0] ? club : "none");
    }
    if (!found) fprintf(stderr, "recomp: dates log: person %s: not found\n", want);
}

/* At the end of a database load: the careers of the people of RECOMP_DATES_LOG. */
static void log_load(void)
{
    const char *v = getenv("RECOMP_DATES_LOG");
    if (!v || !*v) return;
    char buf[2048];
    snprintf(buf, sizeof buf, "%s", v);
    for (char *p = strtok(buf, ";"); p; p = strtok(NULL, ";"))
        if (strncmp(p, "comp:", 5)) log_careers(p);
}

static void log_comp(const char *want)
{
    uint32_t ch = rd32(g_mem, COMP_HISTORY_PTR), n = rd32(g_mem, COMP_HISTORY_COUNT);
    uint32_t clubs = rd32(g_mem, CLUBS_PTR), n_clubs = rd32(g_mem, CLUB_COUNT);
    int years[3] = {0, 0, 0};
    char winners[3][64] = {"", "", ""};
    for (uint32_t i = 0; ch && i < n; i++) {
        uint32_t r = ch + i * COMP_HISTORY_SIZE, comp = rd32(g_mem, r + 4);
        char name[64];
        text_at(comp ? comp + 4 : 0, name, sizeof name);
        if (strcmp(name, want)) continue;
        int y = (int)rd16(g_mem, r + 8);
        for (int k = 0; k < 3; k++) {
            if (y <= years[k]) continue;
            for (int j = 2; j > k; j--) { years[j] = years[j - 1]; memcpy(winners[j], winners[j - 1], 64); }
            years[k] = y;
            uint32_t c = rd32(g_mem, r + 0xa);
            winners[k][0] = 0;
            if (clubs && c >= clubs && c < clubs + n_clubs * CLUB_SIZE) text_at(c + 4, winners[k], 64);
            break;
        }
    }
    fprintf(stderr, "recomp: dates log: competition %s: %d %s; %d %s; %d %s\n", want, years[0], winners[0],
            years[1], winners[1], years[2], winners[2]);
}

void season_dates_log_turn(void)
{
    static int done;
    const char *v = getenv("RECOMP_DATES_LOG");
    if (done || !v || !*v) return;
    done = 1;
    const SeasonDateShifts *s = applied;
    fprintf(stderr, "recomp: dates log: date %d/%d, start year %d, shifts %s\n", (int16_t)rd16(g_mem, 0xae2c90u),
            (int16_t)rd16(g_mem, 0xae2c92u), season_start_year(), s ? s->name : "none");
    char buf[2048];
    snprintf(buf, sizeof buf, "%s", v);
    for (char *p = strtok(buf, ";"); p; p = strtok(NULL, ";")) {
        if (!strncmp(p, "comp:", 5)) log_comp(p + 5);
        else log_person(p);
    }
}
