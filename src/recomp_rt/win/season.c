/* The season of a game: start year, changing rules and the season part of a save (season.h).
 *
 * The save container (zip.cpp of the original, tools/save/savfile.py): three words (version, flags, count),
 * then 1000 rows of 0x10c bytes (offset, size, name of 260 bytes), then the parts. Version 2 and 4 store each
 * part with the byte run code of zwrite 0x932e10, version 1 and 3 store it as it is (zopen 0x9326e0 maps 1
 * to 3 and 2 to 4).
 *
 * The part bosman_season.dat, version 1, 16 bytes, little endian:
 *   +0  u32 format version (1)
 *   +4  u32 length of the part in bytes (16)
 *   +8  u32 start year
 *   +12 u32 changing rules (0 or 1)
 * A later version only adds fields after these and makes the length larger. A reader takes the fields that
 * it knows and that lie inside the length, and gives the others their old meaning.
 *
 * The original opens a part by its exact name (zopen 0x9326e0, the name scan at 0x93293b) and extracts only
 * the parts whose name ends in .tmp (load_game_data 0x82a9c0 calls 0x933320 with "*.tmp" at 0x82acbc), so it
 * never reads this part. The runtime adds the part after the save of the game 0x82e430 has written and
 * renamed the file, and reads it before the load 0x82a9c0 runs. Neither touches the game's memory. The
 * proof: docs/research/2026-10-17-season-save-part.md. */
#include "win.h"
#include "season.h"
#include "options.h"
#include "../host.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* rt_season_year (rt.h, defined in rt.c with 2001): read by the year sites of tools/recomp/seasons.py. */
static int rules_now;                          /* changing rules of the game that runs */
static int new_year = SEASON_FIRST_YEAR;        /* the season of the next new game */
static int new_rules = -1;                      /* -1: the option changing_rules */
static int game_on;                             /* 1 from a made or loaded game to free_game_data */

#define GAME_DATE 0xae2c90u        /* the game date: s16 day of the year, s16 year, u32 leap year */
#define DATE_SET 0x0054a130u       /* Date::set, thiscall (day 1 to 31, month 0 to 11, year, weekday or -1) */

#define PART_NAME "bosman_season.dat"
#define PART_VERSION 1u
#define PART_SIZE 16u
#define HEADER_ROWS 1000u
#define ROW_SIZE 0x10cu
#define ROW_NAME 8u
#define NAME_SIZE 260u
#define MAX_PARTS 999u             /* zopen 0x9326e0 refuses a new part when the count is 999 (0x932a3e) */

void season_init(void)
{
    const char *v = getenv("RECOMP_START_SEASON");     /* test only, until the Choose Season screen */
    if (!v || !*v) return;
    int y = atoi(v);
    if (y < SEASON_FIRST_YEAR || y > SEASON_LAST_YEAR) {
        fprintf(stderr, "recomp: RECOMP_START_SEASON=%s is not a year from %d to %d, so 2001 is used\n", v,
                SEASON_FIRST_YEAR, SEASON_LAST_YEAR);
        return;
    }
    new_year = y;
    rt_season_year = (uint32_t)y;
    fprintf(stderr, "recomp: a new game starts in %d/%02d\n", y, (y + 1) % 100);
}

int season_start_year(void) { return (int)rt_season_year; }
int season_changing_rules(void) { return rules_now; }

int season_set_new_game(int start_year, int changing_rules)
{
    if (start_year < SEASON_FIRST_YEAR || start_year > SEASON_LAST_YEAR) return 0;
    new_year = start_year;
    new_rules = changing_rules < 0 ? -1 : !!changing_rules;
    if (game_on) return 1;          /* a game runs: the values apply at its end, to the next new game */
    rt_season_year = (uint32_t)start_year;
    /* The start screen set the game date to 1 January of the old start year (0x81b938). A new game takes its
     * season from the year of that date (key_nation_build_table 0x667150, called at 0x81cf37). */
    if ((int16_t)rd16(g_mem, GAME_DATE + 2) != start_year) {
        Cpu saved = cpu;
        cpu.ecx = GAME_DATE;
        uint32_t args[4] = {1, 0, (uint32_t)start_year, 0xffffffffu};
        rt_callback(DATE_SET, 4, args);
        cpu = saved;
    }
    return 1;
}

/* The part of a game: written only when the game is not 2001 with changing rules off. */
static int part_needed(void) { return rt_season_year != SEASON_FIRST_YEAR || rules_now; }

static int read_at(int fd, int64_t at, void *buf, uint32_t n)
{
    return host_seek(fd, at, SEEK_SET) == at && host_read(fd, buf, n) == (long)n;
}

static int write_at(int fd, int64_t at, const void *buf, uint32_t n)
{
    return host_seek(fd, at, SEEK_SET) == at && host_write(fd, buf, n) == (long)n;
}

/* The byte run code of zwrite 0x932e10 (the same as tools/save/savfile.py encode). */
static uint32_t encode(const uint8_t *in, uint32_t n, uint8_t *out)
{
    uint32_t o = 0;
    for (uint32_t i = 0; i < n;) {
        uint32_t j = i + 1;
        while (j < n && in[j] == in[i] && j - i < 0x7f) j++;
        uint32_t count = j - i;
        if (count < 2 && in[i] < 0x81) {
            out[o++] = in[i];
        } else {
            out[o++] = (uint8_t)(count + 0x80);
            out[o++] = in[i];
        }
        i = j;
    }
    return o;
}

/* The decoder of zread 0x932c30: n bytes out of at most room stored bytes. 0 when the bytes end early. */
static int decode(const uint8_t *in, uint32_t room, uint8_t *out, uint32_t n)
{
    uint32_t o = 0, i = 0;
    while (o < n && i < room) {
        uint8_t b = in[i++];
        if (b < 0x81) { out[o++] = b; continue; }
        if (i >= room) return 0;
        for (uint32_t k = 0; k < (uint32_t)(b - 0x80) && o < n; k++) out[o++] = in[i];
        i++;
    }
    return o == n;
}

static void put32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }
static uint32_t get32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }

/* Adds the part to the save file at host path. 0 on success. */
static int add_part(const char *host)
{
    int fd = host_open(host, O_RDWR, 0);
    if (fd < 0) return 1;
    int bad = 1;
    uint8_t head[12], row[ROW_SIZE];
    if (!read_at(fd, 0, head, 12)) goto done;
    uint32_t version = get32(head), count = get32(head + 8);
    if (version < 1 || version > 4 || count >= MAX_PARTS) goto done;
    for (uint32_t k = 0; k < count; k++) {
        if (!read_at(fd, 12 + (int64_t)k * ROW_SIZE, row, ROW_SIZE)) goto done;
        if (!strncmp((const char *)row + ROW_NAME, PART_NAME, NAME_SIZE)) goto done;   /* there already */
    }
    uint8_t data[PART_SIZE], stored[2 * PART_SIZE];
    put32(data, PART_VERSION);
    put32(data + 4, PART_SIZE);
    put32(data + 8, rt_season_year);
    put32(data + 12, (uint32_t)rules_now);
    uint32_t n = PART_SIZE;
    if (version == 2 || version == 4) n = encode(data, PART_SIZE, stored);
    else memcpy(stored, data, PART_SIZE);
    int64_t end = host_seek(fd, 0, SEEK_END);
    if (end < 12 + (int64_t)HEADER_ROWS * ROW_SIZE || end > 0x7fffffff) goto done;
    /* The bytes first, then the row, then the count: a write that stops half way leaves the old file whole. */
    if (!write_at(fd, end, stored, n)) goto done;
    memset(row, 0, sizeof row);
    put32(row, (uint32_t)end);
    put32(row + 4, PART_SIZE);
    memcpy(row + ROW_NAME, PART_NAME, sizeof PART_NAME);
    if (!write_at(fd, 12 + (int64_t)count * ROW_SIZE, row, ROW_SIZE)) goto done;
    put32(head + 8, count + 1);
    if (!write_at(fd, 8, head + 8, 4)) goto done;
    bad = 0;
done:
    host_close(fd);
    return bad;
}

/* Reads the part of the save file at host path into year and rules. 1 when the file has the part, 0 when
 * it has none, -1 when the file or the part cannot be read. */
static int read_part(const char *host, int *year, int *rules)
{
    int fd = host_open(host, O_RDONLY, 0);
    if (fd < 0) return -1;
    int result = -1;
    uint8_t head[12], row[ROW_SIZE];
    if (!read_at(fd, 0, head, 12)) goto done;
    uint32_t version = get32(head), count = get32(head + 8);
    if (version < 1 || version > 4 || count > HEADER_ROWS) goto done;
    result = 0;
    for (uint32_t k = 0; k < count; k++) {
        if (!read_at(fd, 12 + (int64_t)k * ROW_SIZE, row, ROW_SIZE)) { result = -1; break; }
        if (strncmp((const char *)row + ROW_NAME, PART_NAME, NAME_SIZE)) continue;
        result = -1;
        uint32_t offset = get32(row), size = get32(row + 4);
        if (size < 16 || size > 4096) break;
        uint8_t stored[8192], data[4096];
        int64_t end = host_seek(fd, 0, SEEK_END);
        if (end <= offset) break;
        uint32_t room = (uint32_t)(end - offset) < sizeof stored ? (uint32_t)(end - offset) : sizeof stored;
        if (!read_at(fd, offset, stored, room)) break;
        if (version == 2 || version == 4) {
            if (!decode(stored, room, data, size)) break;
        } else {
            if (room < size) break;
            memcpy(data, stored, size);
        }
        uint32_t length = get32(data + 4);
        if (get32(data) < 1 || length < 16 || length > size) break;
        uint32_t y = get32(data + 8), r = get32(data + 12);
        if (y < SEASON_FIRST_YEAR || y > SEASON_LAST_YEAR || r > 1) break;
        *year = (int)y;
        *rules = (int)r;
        result = 1;
        break;
    }
done:
    host_close(fd);
    return result;
}

void o_0082e430(void);

/* The save of a game, cdecl (path, flags): 1 when the file is written. The file at path is whole when it
 * returns: with the option bit 0x20000000 the save writes temp_save_file.sav and renames it to path first
 * (0x82e5cb, 0x82f684). The test saves of testrun.c and the Auto Save (0x7617d8) come here too. */
RT_WRAP(0082e430, save_game)
{
    char path[1024];
    snprintf(path, sizeof path, "%s", win_str(rt_arg(0)));
    o_0082e430();
    if (rt_unwinding || cpu.eax != 1 || !part_needed()) return;
    char host[2048];
    if (!win_existing_file(path, host, sizeof host) || add_part(host))
        fprintf(stderr, "cm0102_recomp: the season of this game (%u, changing rules %s) could not be added to "
                        "the save %s. The save loads as a 2001/02 game.\n", rt_season_year,
                rules_now ? "on" : "off", path);
}

void o_0082a9c0(void);

/* The load of a saved game, cdecl (path): nonzero when the game is loaded. The season comes from the save
 * before any code of the load runs: the load builds the league table from the game year (0x82b845). */
RT_WRAP(0082a9c0, load_game)
{
    char host[2048];
    int year = SEASON_FIRST_YEAR, rules = 0;
    const char *path = win_str(rt_arg(0));
    if (win_existing_file(path, host, sizeof host) && read_part(host, &year, &rules) < 0) {
        fprintf(stderr, "cm0102_recomp: the season part of the save %s cannot be read, so it loads as a 2001/02 "
                        "game.\n", path);
        year = SEASON_FIRST_YEAR;
        rules = 0;
    }
    rt_season_year = (uint32_t)year;
    rules_now = rules;
    if (year != SEASON_FIRST_YEAR || rules)
        fprintf(stderr, "recomp: the save is a game of %d/%02d with changing rules %s\n", year, (year + 1) % 100,
                rules ? "on" : "off");
    o_0082a9c0();
    if (!rt_unwinding && cpu.eax) game_on = 1;
}

void o_00828110(void);

/* The creation of a new game, cdecl (flag): nonzero when the game is made. Its changing rules are those of
 * the last season_set_new_game, or the option. */
RT_WRAP(00828110, create_game)
{
    rt_season_year = (uint32_t)new_year;
    rules_now = new_rules < 0 ? options_bool(OPT_CHANGING_RULES) : new_rules;
    o_00828110();
    if (!rt_unwinding && cpu.eax) game_on = 1;
}

void o_0082ff40(void);

/* The end of a game, cdecl (flag): it frees the game and sets the game date to 1 January of the start
 * year (0x8304ec). The values go back to those of a new game first, so that date is the new game's. */
RT_WRAP(0082ff40, free_game)
{
    game_on = 0;
    rt_season_year = (uint32_t)new_year;
    rules_now = 0;
    o_0082ff40();
}

#define FIXTURES 0xae2a58u         /* the fixture store: one block per calendar year, +0x1130 s16 year,
                                    * +0x112c the next block (holiday_next.c); the last points to the first */

/* Test log (RECOMP_SEASON_LOG=1): at each human turn where it changes, the game date, the start year and
 * the calendar years of the fixture store. A game that starts in 2010 must hold no year before 2010. It
 * only reads memory. */
void season_log_turn(void)
{
    static int on = -1;
    static char last[512];
    if (on < 0) on = getenv("RECOMP_SEASON_LOG") && *getenv("RECOMP_SEASON_LOG");
    if (!on) return;
    char now[512];
    size_t n = (size_t)snprintf(now, sizeof now, "start year %u, changing rules %s, fixture years", rt_season_year,
                                rules_now ? "on" : "off");
    uint32_t store = rd32(g_mem, FIXTURES);
    for (uint32_t b = store, k = 0; b && k < 100 && n < sizeof now - 8; b = rd32(g_mem, b + 0x112c), k++) {
        if (k && b == store) break;            /* the blocks form a ring */
        n += (size_t)snprintf(now + n, sizeof now - n, " %d", (int16_t)rd16(g_mem, b + 0x1130));
    }
    if (!strcmp(now, last)) return;
    snprintf(last, sizeof last, "%s", now);
    fprintf(stderr, "recomp: season log: date %d/%d, %s\n", (int16_t)rd16(g_mem, GAME_DATE),
            (int16_t)rd16(g_mem, GAME_DATE + 2), now);
}
