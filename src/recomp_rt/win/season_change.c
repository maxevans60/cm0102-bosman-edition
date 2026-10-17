/* The season change and its news item (season_change.h). Evidence: docs/research/2026-10-17-season-change.md.
 *
 * Game memory that this file reads:
 * - g_comp_object_table [0xadadfc] with [0xadadf8] slots, and the league table entries at 0xb63d60 (35 of
 *   0x48 bytes): +0xc competition count, +0x10 competition array, +0x15 update day (day of the year, as the
 *   game date +0), +0x35 season year (s16). game_season_update_pass 0x5d5ae0 compares +0x15 with the day after
 *   the game date at 0x5d5e37 and the game date at 0x5d5bc8, and moves +0x35 at 0x5d5c0f.
 * - a competition object: +0 vtable, +4 record, +0x40 season year (s16; start_new_season adds 1, for
 *   example 0x574c10).
 *
 * The news item: NewsItem::NewsItem 0x77bbe0 (thiscall: type, important flag; 0xde bytes: +0 type, +4 flag,
 * +5 fifty slots of 4 bytes, +0xcd date) and NewsItem::set_slot 0x785790, then News::send_to_all_managers
 * 0x786780 on the news store [0xb7431c], which copies the item into each human manager's inbox
 * (News::deliver_to_manager 0x785d40). HostCountry::post_host_news 0x5faec0 makes its news in the same way
 * (0x5faf66, 0x5faf81, 0x5fb049). The type NEWS_TYPE is new: NewsItem::get_category 0x785670 puts types 1
 * to 999 in category 2, the Competitions filter of the news screen (news_screens_build_news_screen 0x7881e0,
 * the label at 0x788614 and the value 2 at 0x788630). The original makes types up to 0x2f in that range. The
 * runtime answers the four functions that read the type for this type: NewsItem::write_text 0x77bc80,
 * NewsItem::get_action_button 0x782b70, NewsItem::is_important_for 0x784eb0 and NewsItem::auto_respond
 * 0x784100. The news screen build calls only write_text and get_action_button with an item. */
#include "win.h"
#include "season.h"
#include "season_change.h"
#include "season_rules.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The rule kinds, in the order they run. Each later slice adds its kinds here (season_change.h). */
extern const SeasonRuleKind season_kind_names;
static const SeasonRuleKind *const season_rule_kinds[] = {
    &season_kind_names,                 /* slice 16: competition names (slice 15's names table) */
};
#define KIND_COUNT ((int)(sizeof season_rule_kinds / sizeof season_rule_kinds[0]))

#define GAME_DATE 0xae2c90u             /* s16 day of the year (0 = 1 January), s16 year */
#define COMP_TABLE 0xadadfcu            /* Comp ** */
#define COMP_SLOTS 0xadadf8u            /* s32 */
#define ENTRIES 0xb63d60u               /* league table entries */
#define ENTRY_SIZE 0x48u
#define ENTRY_COUNT 35u
#define ENTRY_SEASON 0x35u              /* s16 */
#define COMP_YEAR 0x40u                 /* s16 */
#define MAX_COMPS 4096
#define LATE_DAY 273                    /* 1 October (day 0 is 1 January; 274 in a leap year, no update day near) */
#define NAME_AT 4u                      /* the long name of a competition record (0x6b bytes) */
#define NAME_ROOM 51                    /* 50 characters and the 0 */

#define NEWS_STORE 0xb7431cu            /* News * */
#define NEWS_ITEM_NEW 0x0077bbe0u       /* NewsItem::NewsItem, thiscall (type, flag) */
#define NEWS_SET_SLOT 0x00785790u       /* NewsItem::set_slot, thiscall (slot, value) */
#define NEWS_SEND_ALL 0x00786780u       /* News::send_to_all_managers, thiscall (item) */
#define TEXT_CLEAR 0x004afa40u          /* Text::clear, thiscall () */
#define TEXT_APPEND 0x004ae770u         /* Text::append_string, thiscall (text) */
#define NEWS_ITEM_SIZE 0xdeu
#define NEWS_TYPE 0x300u                /* new: category 2 (Competitions), not used by the original */
#define NEWS_MAGIC 0x4e534f42u          /* "BOSN" in slot 0: an item of this file */

static int log_on = -1;                 /* RECOMP_SEASON_CHANGE_LOG=1: each change and each news text */

static int readable(uint32_t p) { return p >= 0x10000u && p < 0xfff00000u; }

static int logging(void)
{
    if (log_on < 0) log_on = getenv("RECOMP_SEASON_CHANGE_LOG") && *getenv("RECOMP_SEASON_CHANGE_LOG");
    return log_on;
}

void season_change_latin1(char *s)
{
    unsigned char *in = (unsigned char *)s, *out = (unsigned char *)s;
    while (*in) {
        if ((in[0] == 0xc2 || in[0] == 0xc3) && in[1] >= 0x80 && in[1] <= 0xbf) {
            *out++ = (unsigned char)(((in[0] & 0x1f) << 6) | (in[1] & 0x3f));
            in += 2;
        } else {
            *out++ = *in++;
        }
    }
    *out = 0;
}

/* ---- The competition objects ---- */

static const SeasonCompClass *class_of(uint32_t vtable)
{
    int lo = 0, hi = season_comp_class_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (season_comp_classes[mid].vtable == vtable) return &season_comp_classes[mid];
        if (season_comp_classes[mid].vtable < vtable) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

const char *season_change_file(uint32_t obj)
{
    const SeasonCompClass *c = readable(obj) ? class_of(rd32(g_mem, obj)) : NULL;
    return c ? c->file : "?";
}

static int add_unique(uint32_t *out, int n, int max, uint32_t obj)
{
    if (!readable(obj) || n >= max) return n;
    for (int k = 0; k < n; k++)
        if (out[k] == obj) return n;
    out[n] = obj;
    return n + 1;
}

int season_change_comps(uint32_t *out, int max)
{
    int n = 0;
    uint32_t table = rd32(g_mem, COMP_TABLE);
    int32_t slots = (int32_t)rd32(g_mem, COMP_SLOTS);
    for (int32_t i = 0; readable(table) && i < slots && i < MAX_COMPS; i++)
        n = add_unique(out, n, max, rd32(g_mem, table + 4u * (uint32_t)i));
    for (uint32_t e = 0; e < ENTRY_COUNT; e++) {
        uint32_t entry = ENTRIES + e * ENTRY_SIZE, arr = rd32(g_mem, entry + 0x10);
        int32_t count = (int32_t)rd32(g_mem, entry + 0xc);
        for (int32_t k = 0; readable(arr) && k < count && k < 256; k++)
            n = add_unique(out, n, max, rd32(g_mem, arr + 4u * (uint32_t)k));
    }
    return n;
}

/* ---- The news texts of the game: one per season, kept in the save part ---- */

typedef struct {
    int season;
    char *text;                         /* sentences, one a line */
    uint32_t len;
    int fresh;                          /* 1: made in this pass, its news item is not sent yet */
} SeasonNews;

static SeasonNews *news;
static int news_count, news_room;
static int news_season = -1;            /* the season of the change that runs now; -1: no change runs */
static int last_season = -1;            /* the last season whose season functions ran */

static SeasonNews *news_of(int season)
{
    for (int i = 0; i < news_count; i++)
        if (news[i].season == season) return &news[i];
    return NULL;
}

static SeasonNews *news_new(int season)
{
    if (news_count == news_room) {
        int room = news_room ? 2 * news_room : 8;
        SeasonNews *p = realloc(news, (size_t)room * sizeof *p);
        if (!p) return NULL;
        news = p;
        news_room = room;
    }
    SeasonNews *s = &news[news_count++];
    s->season = season;
    s->text = NULL;
    s->len = 0;
    s->fresh = 0;
    return s;
}

static void news_append(SeasonNews *s, const char *line)
{
    uint32_t n = (uint32_t)strlen(line), extra = s->len ? 1u : 0u;
    if (s->len + extra + n > 0x8000u) return;                     /* a text the screen can hold */
    char *p = realloc(s->text, s->len + extra + n + 1);
    if (!p) return;
    if (extra) p[s->len++] = '\n';
    memcpy(p + s->len, line, n + 1);
    s->text = p;
    s->len += n;
}

void season_change_news(const char *fmt, ...)
{
    if (news_season < 0) return;
    char line[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    for (char *p = line; *p; p++)
        if (*p == '<' || *p == '\n' || *p == '\r' || *p == '\t') *p = ' ';
    SeasonNews *s = news_of(news_season);
    if (!s && (s = news_new(news_season))) s->fresh = 1;
    if (!s) return;
    news_append(s, line);
    if (logging()) fprintf(stderr, "recomp: season change: news %d/%02d: %s\n", news_season, (news_season + 1) % 100,
                           line);
}

/* ---- The names that the game has changed ---- */

/* Each record whose long name the season code changed: its name in the data (the original) and its name now.
 * set_key_ids 0x601a40 finds the key competitions by name at every load of the data (match_club_comp_key_id
 * 0x60efd0, _stricmp with fixed strings; the club comps and then the nation comps, loop at 0x60425d). A save
 * holds the names of now, so a load would miss "UEFA Cup" and stop at the box "index.cpp 5809". The wrap of
 * 0x60efd0 below gives it the original name instead. */
typedef struct {
    char original[NAME_ROOM], now[NAME_ROOM];
} SeasonRename;

static SeasonRename *renames;
static int rename_count, rename_room;

void season_change_rename(uint32_t rec, const char *name)
{
    char old[NAME_ROOM], now[NAME_ROOM];
    snprintf(old, sizeof old, "%.*s", NAME_ROOM - 1, win_str(rec + NAME_AT));
    snprintf(now, sizeof now, "%s", name);
    memset(g_mem + rec + NAME_AT, 0, NAME_ROOM);
    memcpy(g_mem + rec + NAME_AT, now, strlen(now));
    for (int i = 0; i < rename_count; i++)
        if (!strcmp(renames[i].now, old)) {
            memcpy(renames[i].now, now, NAME_ROOM);
            return;
        }
    if (rename_count == rename_room) {
        int room = rename_room ? 2 * rename_room : 16;
        SeasonRename *p = realloc(renames, (size_t)room * sizeof *p);
        if (!p) return;
        renames = p;
        rename_room = room;
    }
    memcpy(renames[rename_count].original, old, NAME_ROOM);
    memcpy(renames[rename_count].now, now, NAME_ROOM);
    rename_count++;
}

void o_0060efd0(void);

/* match_club_comp_key_id, cdecl (name, index): a renamed record is matched by its original name. */
RT_WRAP(0060efd0, match_comp_key_id)
{
    uint32_t name = rt_arg(0);
    const SeasonRename *r = NULL;
    for (int i = 0; i < rename_count && !r; i++)
        if (readable(name) && !strncmp(win_str(name), renames[i].now, NAME_ROOM)) r = &renames[i];
    if (!r) {
        o_0060efd0();
        return;
    }
    static uint32_t buf;
    if (!buf) buf = win_static_alloc(NAME_ROOM);
    win_put_str(buf, r->original, NAME_ROOM);
    uint32_t slot = cpu.esp + 4;
    wr32(g_mem, slot, buf);
    o_0060efd0();
    wr32(g_mem, slot, name);        /* cdecl: the caller's argument slot gets its value back */
}

void season_change_reset(void)
{
    for (int i = 0; i < news_count; i++) free(news[i].text);
    news_count = 0;
    rename_count = 0;
    last_season = -1;
}

uint32_t season_change_part_size(void)
{
    if (!news_count && !rename_count) return 0;
    uint32_t n = 8 + (uint32_t)rename_count * 2 * NAME_ROOM;
    for (int i = 0; i < news_count; i++) n += 8 + news[i].len;
    return n;
}

static void put32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }
static uint32_t get32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }

void season_change_part_write(uint8_t *out)
{
    put32(out, (uint32_t)news_count);
    uint32_t at = 4;
    for (int i = 0; i < news_count; i++) {
        put32(out + at, (uint32_t)news[i].season);
        put32(out + at + 4, news[i].len);
        if (news[i].len) memcpy(out + at + 8, news[i].text, news[i].len);
        at += 8 + news[i].len;
    }
    put32(out + at, (uint32_t)rename_count);
    at += 4;
    for (int i = 0; i < rename_count; i++) {
        memcpy(out + at, renames[i].original, NAME_ROOM);
        memcpy(out + at + NAME_ROOM, renames[i].now, NAME_ROOM);
        at += 2 * NAME_ROOM;
    }
}

int season_change_part_read(const uint8_t *in, uint32_t n)
{
    season_change_reset();
    if (n < 4) return 0;
    uint32_t count = get32(in), at = 4;
    for (uint32_t i = 0; i < count; i++) {
        if (n - at < 8) return 0;
        uint32_t season = get32(in + at), len = get32(in + at + 4);
        if (season < SEASON_FIRST_YEAR || season > SEASON_LAST_YEAR || len > 0x8000u || n - at - 8 < len) return 0;
        SeasonNews *s = news_of((int)season);
        if (!s) s = news_new((int)season);
        if (!s) return 0;
        char *text = malloc(len + 1);
        if (!text) return 0;
        memcpy(text, in + at + 8, len);
        text[len] = 0;
        for (uint32_t k = 0; k < len; k++)
            if (!text[k] || text[k] == '<') text[k] = ' ';
        free(s->text);
        s->text = text;
        s->len = len;
        at += 8 + len;
    }
    if (n - at < 4) return 0;
    uint32_t names = get32(in + at);
    at += 4;
    if (names > 100000u || (n - at) / (2 * NAME_ROOM) < names) return 0;
    if (names > (uint32_t)rename_room) {
        SeasonRename *p = realloc(renames, (size_t)names * sizeof *p);
        if (!p) return 0;
        renames = p;
        rename_room = (int)names;
    }
    for (uint32_t i = 0; i < names; i++) {
        memcpy(renames[i].original, in + at, NAME_ROOM);
        memcpy(renames[i].now, in + at + NAME_ROOM, NAME_ROOM);
        renames[i].original[NAME_ROOM - 1] = renames[i].now[NAME_ROOM - 1] = 0;
        at += 2 * NAME_ROOM;
    }
    rename_count = (int)names;
    return 1;
}

/* ---- The news item ---- */

static uint32_t call_this(uint32_t fn, uint32_t self, int nargs, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, nargs, args);
    if (!rt_unwinding) cpu = saved;
    return r;
}

/* One news item for season to every human manager. The text stays in the list: write_text reads it. */
static void news_send(int season)
{
    static uint32_t item;
    if (!item) item = win_static_alloc(0x100);
    uint32_t store = rd32(g_mem, NEWS_STORE);
    if (!readable(store)) return;
    uint32_t a[2] = {NEWS_TYPE, 0};
    call_this(NEWS_ITEM_NEW, item, 2, a);
    if (rt_unwinding) return;
    uint32_t s0[2] = {0, NEWS_MAGIC}, s1[2] = {1, (uint32_t)season};
    call_this(NEWS_SET_SLOT, item, 2, s0);
    if (rt_unwinding) return;
    call_this(NEWS_SET_SLOT, item, 2, s1);
    if (rt_unwinding) return;
    uint32_t b[1] = {item};
    call_this(NEWS_SEND_ALL, store, 1, b);
    if (logging() && !rt_unwinding)
        fprintf(stderr, "recomp: season change: news item for %d/%02d sent\n", season, (season + 1) % 100);
}

static int our_item(uint32_t item)
{
    return readable(item) && rd32(g_mem, item) == NEWS_TYPE && rd32(g_mem, item + 5) == NEWS_MAGIC;
}

static void text_put(uint32_t text, const char *s)
{
    static uint32_t buf;
    if (!buf) buf = win_static_alloc(0x8100);
    win_put_str(buf, s, 0x8100);
    uint32_t a[1] = {buf};
    call_this(TEXT_APPEND, text, 1, a);
}

void o_0077bc80(void);
void o_00782b70(void);
void o_00784eb0(void);
void o_00784100(void);

/* NewsItem::write_text, thiscall (manager, text, full_text, flag): the headline (full_text 0) or the body. */
RT_WRAP(0077bc80, news_write_text)
{
    uint32_t item = cpu.ecx, text = rt_arg(1);
    if (!our_item(item) || !readable(text)) {
        o_0077bc80();
        return;
    }
    int season = (int)rd32(g_mem, item + 9);
    char line[160];
    call_this(TEXT_CLEAR, text, 0, NULL);
    if (rt_unwinding) return;
    if (!rt_arg(2)) {
        snprintf(line, sizeof line, "New rules for the %d/%02d season", season, (season + 1) % 100);
        text_put(text, line);
    } else {
        SeasonNews *s = news_of(season);
        snprintf(line, sizeof line, "These changes come in with the %d/%02d season:\n\n", season, (season + 1) % 100);
        text_put(text, line);
        if (!rt_unwinding) text_put(text, s && s->len ? s->text : "The rules of the new season are in force.");
    }
    if (!rt_unwinding) rt_ret_stdcall(1, 4);
}

/* NewsItem::get_action_button, thiscall (manager, label, label size, handler): no button. */
RT_WRAP(00782b70, news_action_button)
{
    if (!our_item(cpu.ecx)) {
        o_00782b70();
        return;
    }
    uint32_t label = rt_arg(1), handler = rt_arg(3);
    if (readable(label)) g_mem[label] = 0;
    if (readable(handler)) wr32(g_mem, handler, 0);
    rt_ret_stdcall(0, 4);
}

/* NewsItem::is_important_for, thiscall (manager): not important. */
RT_WRAP(00784eb0, news_important)
{
    if (!our_item(cpu.ecx)) {
        o_00784eb0();
        return;
    }
    rt_ret_stdcall(0, 1);
}

/* NewsItem::auto_respond, thiscall (manager, club): the item needs no reply. */
RT_WRAP(00784100, news_auto_respond)
{
    if (!our_item(cpu.ecx)) {
        o_00784100();
        return;
    }
    rt_ret_stdcall(0, 2);
}

/* ---- The July step ---- */

void o_005d5ae0(void);

static uint32_t objs[MAX_COMPS];
static int16_t years[MAX_COMPS];
static int16_t entry_years[ENTRY_COUNT];

/* 1 when obj is in the competition array of an entry whose season year moved in the pass. */
static int in_moved_entry(uint32_t obj)
{
    for (uint32_t e = 0; e < ENTRY_COUNT; e++) {
        uint32_t entry = ENTRIES + e * ENTRY_SIZE, arr = rd32(g_mem, entry + 0x10);
        if ((int16_t)rd16(g_mem, entry + ENTRY_SEASON) == entry_years[e]) continue;
        int32_t count = (int32_t)rd32(g_mem, entry + 0xc);
        for (int32_t k = 0; readable(arr) && k < count && k < 256; k++)
            if (rd32(g_mem, arr + 4u * (uint32_t)k) == obj) return 1;
    }
    return 0;
}

/* game_season_update_pass, cdecl (): at the end of every day. */
RT_WRAP(005d5ae0, season_update_pass)
{
    if (!season_code_on() || !season_changing_rules()) {
        o_005d5ae0();
        return;
    }
    static int days_logged;
    if (logging() && !days_logged++)
        for (uint32_t e = 0; e < ENTRY_COUNT; e++)
            fprintf(stderr, "recomp: season change: entry %u update day %d season year %d competitions %d\n", e,
                    (int16_t)rd16(g_mem, ENTRIES + e * ENTRY_SIZE + 0x15),
                    (int16_t)rd16(g_mem, ENTRIES + e * ENTRY_SIZE + ENTRY_SEASON),
                    (int32_t)rd32(g_mem, ENTRIES + e * ENTRY_SIZE + 0xc));
    int n = season_change_comps(objs, MAX_COMPS);
    for (int i = 0; i < n; i++) years[i] = (int16_t)rd16(g_mem, objs[i] + COMP_YEAR);
    for (uint32_t e = 0; e < ENTRY_COUNT; e++)
        entry_years[e] = (int16_t)rd16(g_mem, ENTRIES + e * ENTRY_SIZE + ENTRY_SEASON);
    o_005d5ae0();
    if (rt_unwinding) return;

    /* The season that starts: the year of the game date, or the next year from 1 October (season_change.h). */
    int season = (int16_t)rd16(g_mem, GAME_DATE + 2) + ((int16_t)rd16(g_mem, GAME_DATE) >= LATE_DAY ? 1 : 0);
    for (uint32_t e = 0; e < ENTRY_COUNT && logging(); e++) {
        int16_t y = (int16_t)rd16(g_mem, ENTRIES + e * ENTRY_SIZE + ENTRY_SEASON);
        if (y != entry_years[e])
            fprintf(stderr, "recomp: season change: entry %u season year %d -> %d on day %d of %d\n", e, entry_years[e],
                    y, (int16_t)rd16(g_mem, GAME_DATE), season);
    }
    for (int i = 0; i < n; i++) {
        uint32_t obj = objs[i];
        int16_t y = (int16_t)rd16(g_mem, obj + COMP_YEAR);
        if (y == years[i] && !in_moved_entry(obj)) continue;
        const char *file = season_change_file(obj);
        if (logging()) {
            uint32_t rec = rd32(g_mem, obj + 4);
            fprintf(stderr, "recomp: season change: %s (%.50s) year %d -> %d, season %d\n", file,
                    readable(rec) ? win_str(rec + 4) : "?", years[i], y, season);
        }
        if (season <= season_start_year() || season > SEASON_LAST_YEAR) continue;
        news_season = season;
        if (season > last_season) {
            last_season = season;
            for (int k = 0; k < KIND_COUNT && !rt_unwinding; k++)
                if (season_rule_kinds[k]->season) season_rule_kinds[k]->season(season);
        }
        for (int k = 0; k < KIND_COUNT && !rt_unwinding; k++)
            if (season_rule_kinds[k]->comp) season_rule_kinds[k]->comp(obj, file, season);
        news_season = -1;
        if (rt_unwinding) return;
    }
    /* One news item per season, sent on the first day with a sentence for that season. A later day of the
     * same season adds its sentences to the text of that item. */
    for (int i = 0; i < news_count; i++) {
        if (!news[i].fresh) continue;
        news[i].fresh = 0;
        news_send(news[i].season);
        if (rt_unwinding) return;
    }
}
