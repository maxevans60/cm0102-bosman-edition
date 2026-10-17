/* The Choose Season screen (0.6.0 slice 13; spec docs/specs/2026-10-16-seasons-0.6.0-spec.md, "Seasons and
 * databases", "Eras", "Changing rules", "Where it shows").
 *
 * The layout is "A3 + both" of docs/research/mockups/2026-10-15-season-picker.html with five eras (Q240,
 * Q261): one coloured heading per era, the tiles of its seasons under it in the era colour with a coloured
 * top edge, a status tag on each tile and a star on each season that has a saved game. A Custom database
 * tile follows the seasons (Q254). Below the tiles the strip shows the chosen season, its database, its
 * credit and the start date, and the version choice when more than one database of the season is there
 * (Q252, Q257). A line links to the Update Team's Databases forum at champman0102.co.uk.
 *
 * The screen is a screen of the runtime on the game's own screen stack, as Team views (views.c): the
 * Choose Season item of Setup Game (database.c) pushes it with the push of the original 0x7fe600, flag 0,
 * as the start screen's own items push their screens (0x81bd70, inventory 6c). Its build and input are host
 * functions (rt_host_fn). The frame copies the start screen 0x81bab0: the picture of the start screen (its
 * node title, through background_set_picture 0x41c0d0), the heading 0x803f30, the side bar 0x75db00 and the
 * Back bar 0x5edcc0. Tiles and labels are widgets of the original (Display::add_item 0x55f990,
 * ScreenManager::add_text_item 0x803fd0). The top edges, the frame of the chosen tile, the stars and the
 * era headings are drawn by the runtime after the game draws their widgets (draw.c, from the widget draw
 * wrap in names.c through views_widget_drawn), as the pitch of Team views.
 *
 * A click on a tile chooses it for the strip. "Choose" starts it: the season database comes from
 * seasons_db_prepare (slice 14), which unpacks a zipped database once into Databases. Before the unpack the
 * screen is built once more with a line that says so, and the unpack runs at the next input call. Then the
 * options database and season get the choice, season_set_new_game gets the start year and the screen goes
 * back to Setup Game, which shows the season, its database and the changing rules switch.
 *
 * Same game (Q152): the screen keeps its texts in runtime memory (win_static_alloc), not in the game's heap.
 * It reads files only: settings.txt changes through options_set. The push of the node is the original's.
 *
 * Node slots: 0 the chosen tile (1 to 25 for the seasons, 26 for the Custom database), 1 the version, 2 the
 * Custom folder, 3 the season of the Custom folder (start year), 4 the state (0, 1 unpack next, 2 error),
 * 5 the page of versions. Only numbers, so the screen needs no leave function. */
#include "win.h"
#include "draw.h"
#include "options.h"
#include "season.h"
#include "seasons_db.h"
#include "season_screen.h"
#include "../host.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREENS 0xb74450u           /* the screen stack S */
#define TOP_NODE (SCREENS + 0x7d4u + 0x0eu)
#define PUSH 0x7fe600u              /* thiscall on S (build, input, flag, leave, exit), ret 0x14 */
#define SET_SLOT 0x7ff270u          /* thiscall on S (slot, value, copy) */
#define NODE_SLOT 0x7ff010u         /* thiscall on S (slot) */
#define CANNOT_GO_BACK 0x7fec60u    /* thiscall on S (any): 0 when there is a screen to go back to */
#define SET_PICTURE 0x41c0d0u       /* cdecl (path): background_set_picture */
#define HEADING 0x803f30u           /* thiscall on S (bar colour, text colour, text) */
#define SIDE_BAR 0x75db00u          /* cdecl (mode, arg) */
#define BACK_BAR 0x5edcc0u          /* cdecl (back, next) */
#define AREA_CREATE 0x55fba0u       /* thiscall on a Display, 11 args */
#define WIDGET_CREATE 0x55f990u     /* thiscall on a Display, 18 args */
#define BOX_TEXT 0x803fd0u          /* thiscall on S, 11 args: a text widget with its own rect */
#define SIDE_BAR_INPUT 0x7615d0u    /* cdecl (widget): 0 when it does not handle the click */
#define GAME_INPUT 0x7643d0u        /* cdecl (widget): the game's own actions, Back among them */

#define DISPLAYS 0xb74c4cu          /* the Display of a player slot, 0x300 bytes apart */
#define PLAYER_SLOT 0xb77c7au       /* s16: the player slot that has the turn */
#define WIDGETS 0xba95eu            /* Display + this: widget 0; 0x18c bytes each */
#define WIDGET_SIZE 0x18cu
#define NO_DRAW 0xaebe0cu           /* not 0: the draw functions of the original draw nothing */
#define DRAW_SURFACE 0xaebde4u      /* the surface that the draw functions of the original draw on */
#define NODE_PREV 0x1f4u            /* screen node: the node before it */
#define NODE_TITLE 0x1fcu           /* screen node: the title, the picture of the screen (260 bytes) */

#define TEXT_YELLOW 0xaebdecu       /* u16 colours of the original (include/gui_utils.h) */
#define TEXT_LIGHT 0xae3184u
#define HEADING_RED 0xae31a8u
#define BUTTON_GREY 0xae317eu

#define TILE_ID 0x7d40u             /* data: tile index 0 to 25. Ids that the original does not use. */
#define VERSION_ID 0x7d41u          /* data: version index */
#define CHOOSE_ID 0x7d42u
#define FOLDER_ID 0x7d43u           /* the next Custom folder */
#define CUSTOM_YEAR_ID 0x7d44u      /* data: 0 an earlier season, 1 a later one */
#define LINK_ID 0x7d45u
#define PAGE_ID 0x7d46u             /* the next page of versions */
#define REBUILD 0xfffffff5u         /* -0xb */
#define GO_BACK 0xfffffffeu         /* -2: the result of Back (0x762c97) */

#define CUSTOM_TILE SEASONS_COUNT   /* tile index of the Custom database */
#define TILES (SEASONS_COUNT + 1)
#define TEXT_ROOM 0x200u
#define TEXTS 8                     /* text buffers in a row: a create copies its text, but a few are kept */

#define LINK_URL "https://champman0102.co.uk/viewforum.php?f=73"

/* The credit of each database (Q147, Q269, Q340): the official data sets credit Sports Interactive, and every
 * Update Team database, October 2003 and September 2005 included, credits the Update Team. Never a person's
 * name in the code. One table, so a later change is one line. */
typedef struct { const char *name; const char *credit; } Credit;
static const Credit credits[] = {
    {"3.9.68", "Data by Sports Interactive"},
    {"3.9.60", "Data by Sports Interactive"},
    {"3.9.65", "Data by Sports Interactive"},
};
#define CREDIT_UPDATE_TEAM "Database by the CM 01/02 Update Team"
#define CREDIT_CUSTOM "Your own database"

const char *season_screen_credit(const char *name)
{
    for (size_t i = 0; i < sizeof credits / sizeof credits[0]; i++)
        if (!strcmp(credits[i].name, name)) return credits[i].credit;
    return CREDIT_UPDATE_TEAM;
}

/* The era colours (Q166, Q240): red, amber, green, blue, purple; the Custom database is grey. */
typedef struct { uint8_t r, g, b; } Rgb;
static const Rgb era_rgb[5] = {{200, 16, 46}, {217, 140, 0}, {42, 138, 58}, {42, 90, 220}, {130, 60, 200}};
static const Rgb custom_rgb = {120, 120, 130};
static const char *const era_heading[5] = {"Original", "2000s", "2010s", "Early 2020s", "League phase"};
static const char *const era_note[5] = {
    "2001/02 and 2002/03, the official data",
    "2003/04 to 2008/09, Update Team databases",
    "2009/10 to 2019/20, Update Team databases",
    "2020/21 to 2023/24, Update Team databases",
    "From 2024/25, Update Team databases",
};

static uint16_t rgb565(Rgb c) { return (uint16_t)(((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3)); }

/* c over the navy of the original's buttons (30, 30, 120) with weight w of 256. */
static Rgb mix(Rgb c, int w)
{
    Rgb n = {30, 30, 120};
    return (Rgb){(uint8_t)((c.r * w + n.r * (256 - w)) / 256), (uint8_t)((c.g * w + n.g * (256 - w)) / 256),
                 (uint8_t)((c.b * w + n.b * (256 - w)) / 256)};
}

/* ---- What the screen shows: filled when the screen opens and after an unpack ---- */
static SeasonsList list;
static int starred[SEASONS_COUNT];  /* 1 when a saved game of the save folder starts in that season */
static char error_text[240];

static void scan_saves(void)
{
    memset(starred, 0, sizeof starred);
    const char *dir = win_save_folder();
    HostDir *d = host_opendir(dir);
    if (!d) return;
    for (const char *n; (n = host_readdir(d));) {
        size_t len = strlen(n);
        if (len < 5 || strcasecmp(n + len - 4, ".sav")) continue;
        char path[2048];
        snprintf(path, sizeof path, "%s/%s", dir, n);
        int year;
        if (season_of_save(path, &year) >= 0 && year >= SEASONS_FIRST && year < SEASONS_FIRST + SEASONS_COUNT)
            starred[year - SEASONS_FIRST] = 1;
    }
    host_closedir(d);
}

/* ---- The season of the next new game, from the options database and season ---- */

static int option_season(void) { return SEASONS_FIRST + options_enum(OPT_SEASON); }

/* The known database of a folder of Databases, or NULL (a Custom database or no such folder). *exists gets 1
 * when the folder is there and usable. */
static const SeasonKnownDb *folder_db(const char *name, int *exists)
{
    char path[1300];
    *exists = dbfolders_path(win_databases_root(), name, path, sizeof path);
    return *exists ? seasons_db_identify(path) : NULL;
}

int season_screen_year(void)
{
    const char *db = options_string(OPT_DATABASE);
    int opt = option_season();
    int official = opt == 2002 ? 2002 : 2001;           /* 3.9.68 is the data of both official tiles */
    if (strncmp(db, "folder:", 7)) return strcmp(db, "3.9.68") ? 2001 : official;
    int exists;
    const SeasonKnownDb *k = folder_db(db + 7, &exists);
    if (!exists) return 2001;                            /* files.c then loads 3.9.68 */
    if (!k) return opt;                                  /* a Custom database: the season the player named */
    if (k->role == SEASONS_ROLE_OFFICIAL_BASE) return official;
    if (k->role == SEASONS_ROLE_SEASON || k->role == SEASONS_ROLE_EXTRA) return k->season;
    return 2001;
}

void season_screen_database_text(char *out, size_t room)
{
    const char *db = options_string(OPT_DATABASE);
    if (strncmp(db, "folder:", 7)) {
        snprintf(out, room, "%s data", db);
        return;
    }
    int exists;
    const SeasonKnownDb *k = folder_db(db + 7, &exists);
    if (!exists) snprintf(out, room, "3.9.68 data");
    else if (k && k->role == SEASONS_ROLE_OFFICIAL_BASE) snprintf(out, room, "3.9.68 data");
    else if (k) snprintf(out, room, "%s", k->name);
    else dbfolders_label(db + 7, out, room);
}

/* ---- Calls into the game ---- */

static uint32_t call_this(uint32_t fn, uint32_t self, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    cpu.ecx = self;
    uint32_t r = rt_callback(fn, n, args);
    cpu = saved;
    return r;
}

static uint32_t call(uint32_t fn, int n, const uint32_t *args)
{
    Cpu saved = cpu;
    uint32_t r = rt_callback(fn, n, args);
    cpu = saved;
    return r;
}

static uint32_t node_slot(int n)
{
    uint32_t a[1] = {(uint32_t)n};
    return call_this(NODE_SLOT, SCREENS, 1, a);
}

static void set_slot(int n, uint32_t value)
{
    uint32_t a[3] = {(uint32_t)n, value, 0};
    call_this(SET_SLOT, SCREENS, 3, a);
}

static uint32_t display(void)
{
    return rd32(g_mem, DISPLAYS + (uint32_t)(int32_t)(int16_t)rd16(g_mem, PLAYER_SLOT) * 0x300u);
}

static uint32_t scratch;            /* runtime memory: TEXTS buffers of TEXT_ROOM bytes */
static int next_text;

static uint32_t text(const char *s)
{
    uint32_t at = scratch + (uint32_t)(next_text++ % TEXTS) * TEXT_ROOM;
    win_put_str(at, s, TEXT_ROOM);
    return at;
}

static uint32_t colour16(uint32_t at) { return rd16(g_mem, at); }

/* A widget with its own rect (classic numbers), no area. flags 2 is a button, 1 a label. */
static int widget(uint32_t flags, int x1, int y1, int x2, int y2, uint32_t style, uint32_t colour, uint32_t text_flags,
                  uint32_t font, uint32_t text_colour, const char *s, uint32_t id, uint32_t data)
{
    uint32_t w[18] = {flags, (uint32_t)x1, (uint32_t)y1, (uint32_t)x2, (uint32_t)y2, 0, 0, style, colour,
                      flags == 2 ? colour : 0, text_flags, font, text_colour, text(s), 0, id, data, 0xffffffffu};
    return (int16_t)call_this(WIDGET_CREATE, display(), 18, w);
}

/* A text: flags 1 left, 0xc centred; font 1 small, 2 medium, 3 as the menu, 4 large. */
static void label(int x1, int y1, int x2, int y2, uint32_t text_flags, uint32_t font, uint32_t colour, const char *s)
{
    uint32_t a[11] = {(uint32_t)x1, (uint32_t)y1, (uint32_t)x2, (uint32_t)y2, 1, 0, text_flags, font, colour, text(s), 1};
    call_this(BOX_TEXT, SCREENS, 11, a);
}

/* A grey button as Back of the original (style 0x30, 0x5edd10); greyed text and no id when off. */
static void button(int x1, int y1, int x2, int y2, uint32_t font, const char *s, uint32_t id, uint32_t data, int on)
{
    widget(on ? 2 : 1, x1, y1, x2, y2, 0x30, colour16(BUTTON_GREY), on ? 0xc : 0x2c, font, colour16(TEXT_LIGHT), s,
           on ? id : 0, on ? data : 0);
}

static void panel(int x1, int y1, int x2, int y2)
{
    uint32_t a[11] = {(uint32_t)x1, (uint32_t)y1, (uint32_t)x2, (uint32_t)y2, 1, 0, 1, 0, 2, 0, 0xffffffffu};
    call_this(AREA_CREATE, display(), 11, a);
}

/* ---- What the runtime draws over the widgets ---- */

typedef enum { MARK_NONE, MARK_TILE, MARK_BAND } MarkKind;
/* The rect widget gives the place. A draw of any of the widgets of the mark draws it again, so it stays
 * over the texts of a tile when the game draws one of them alone. */
typedef struct {
    int widgets[3];
    int rect;
    MarkKind kind;
    Rgb colour;
    int star, chosen;
} Mark;
#define MARKS 64
static Mark marks[MARKS];
static int mark_count;

static void mark(int rect, int w1, int w2, MarkKind kind, Rgb colour, int star, int chosen)
{
    if (rect < 0 || mark_count >= MARKS) return;
    marks[mark_count++] = (Mark){{rect, w1, w2}, rect, kind, colour, star, chosen};
}

static uint32_t build_fn(void);

static int on_top(void)
{
    uint32_t node = rd32(g_mem, TOP_NODE);
    return node && rd32(g_mem, node) == build_fn();
}

static void fill(const DrawTarget *t, int x1, int y1, int x2, int y2, uint16_t c, int alpha)
{
    int xy[8] = {x1, y1, x2 + 1, y1, x2 + 1, y2 + 1, x1, y2 + 1};
    draw_polygon(t, xy, 4, c, alpha);
}

/* A five-pointed star of radius r around (cx, cy), in yellow with a dark edge. */
static void star(const DrawTarget *t, int cx, int cy, int r)
{
    static const int unit[20] = {0, -100, 22, -31, 95, -31, 36, 12, 59, 81, 0, 38, -59, 81, -36, 12, -95, -31, -22, -31};
    int xy[20], edge[20];
    for (int i = 0; i < 20; i += 2) {
        xy[i] = cx + unit[i] * r / 100;
        xy[i + 1] = cy + unit[i + 1] * r / 100;
        edge[i] = cx + unit[i] * (r + 1) / 100;
        edge[i + 1] = cy + unit[i + 1] * (r + 1) / 100 + 1;
    }
    draw_polygon(t, edge, 10, 0x0000, 256);
    draw_polygon(t, xy, 10, rgb565((Rgb){240, 224, 0}), 256);
}

void season_screen_widget_drawn(uint32_t w)
{
    if (!mark_count || !on_top() || rd32(g_mem, NO_DRAW)) return;
    int index = (int)((w - display() - WIDGETS) / WIDGET_SIZE);
    if (w < display() + WIDGETS || (w - display() - WIDGETS) % WIDGET_SIZE) return;
    const Mark *m = NULL;
    for (int i = 0; i < mark_count; i++)
        for (int k = 0; k < 3; k++)
            if (marks[i].widgets[k] == index && index >= 0) m = &marks[i];
    if (!m) return;
    w = display() + WIDGETS + (uint32_t)m->rect * WIDGET_SIZE;
    uint32_t surface = rd32(g_mem, DRAW_SURFACE);
    if (!surface) return;
    DrawTarget t = {g_mem + rd32(g_mem, surface + 0x14), rd32(g_mem, surface + 0x10),
                    (int)rd32(g_mem, surface + 8), (int)rd32(g_mem, surface + 0xc)};
    int x1 = (int32_t)rd32(g_mem, w + 0x10), y1 = (int32_t)rd32(g_mem, w + 0x14);
    int x2 = (int32_t)rd32(g_mem, w + 0x18), y2 = (int32_t)rd32(g_mem, w + 0x1c);
    uint16_t c = rgb565(m->colour);
    if (m->kind == MARK_BAND) {
        /* The era heading: the era colour, fading out towards 70 % of the width (the mockup's gradient). */
        int w70 = (x2 - x1) * 7 / 10;
        for (int x = x1; x <= x2; x++) {
            int a = x - x1 >= w70 ? 0 : 230 - 230 * (x - x1) / (w70 ? w70 : 1);
            if (a > 0) fill(&t, x, y1, x, y2, c, a);
        }
        return;
    }
    int edge = (y2 - y1) / 9 > 3 ? (y2 - y1) / 9 : 3;
    fill(&t, x1, y1, x2, y1 + edge - 1, c, 256);            /* the top edge in the era colour */
    if (m->chosen) {
        uint16_t yellow = rgb565((Rgb){240, 224, 0});
        fill(&t, x1, y1, x2, y1 + 1, yellow, 256);
        fill(&t, x1, y2 - 1, x2, y2, yellow, 256);
        fill(&t, x1, y1, x1 + 1, y2, yellow, 256);
        fill(&t, x2 - 1, y1, x2, y2, yellow, 256);
    }
    if (m->star) {
        /* At the right of the tag row (tile(): the tag runs from 55 % of the height to the bottom). */
        int mid = y1 + (y2 - y1) * 11 / 20, cy = (mid + y2 + 1) / 2;
        int r = (y2 - mid) / 2 - 1, room = (int)(8 * rt_screen_w / 800) - 2;
        if (r > room) r = room;
        if (r < 3) r = 3;
        star(&t, x2 - (int)(9 * rt_screen_w / 800), cy, r);
    }
}

/* ---- Layout, in classic numbers (800 x 600); the widget create scales them to the screen ---- */
#define LEFT 100
#define RIGHT 790
#define COLUMNS 8
#define TOP 76
#define BAND_H 17
#define TILE_H 38

static int col_x1(int c) { return LEFT + c * (RIGHT - LEFT + 5) / COLUMNS; }
static int col_x2(int c) { return LEFT + (c + 1) * (RIGHT - LEFT + 5) / COLUMNS - 5; }

static const char *status_tag(const SeasonsEntry *s)
{
    switch (s->status) {
    case SEASONS_OFFICIAL: return "Official";
    case SEASONS_UNPACKED: return "Ready";
    case SEASONS_IN_DOWNLOAD: return "Zipped";
    case SEASONS_NO_DATABASE: return "No database yet";
    default: return "Missing";
    }
}

static Rgb tag_rgb(const SeasonsEntry *s)
{
    switch (s->status) {
    case SEASONS_OFFICIAL: return (Rgb){160, 120, 0};
    case SEASONS_UNPACKED: return (Rgb){26, 122, 26};
    case SEASONS_IN_DOWNLOAD: return (Rgb){85, 85, 85};
    default: return (Rgb){128, 32, 32};
    }
}

/* One tile: a button in the tint of its era with the season, and its status tag in a small box below the
 * season. The tag box is a button with the same action, so a click on it chooses the tile too. */
static void tile(int x1, int y1, int x2, int y2, const char *title, const char *tag, Rgb era, Rgb tag_colour,
                 int index, int enabled, int is_star, int chosen)
{
    uint32_t yellow = colour16(TEXT_YELLOW), light = colour16(TEXT_LIGHT);
    Rgb fill_rgb = enabled ? mix(era, 90) : (Rgb){40, 40, 60};
    uint32_t text_colour = !enabled ? rgb565((Rgb){150, 150, 150}) : chosen ? yellow : light;
    int w = widget(enabled ? 2 : 1, x1, y1, x2, y2, 0x30, rgb565(fill_rgb), 0xc, 3, text_colour, "",
                   enabled ? TILE_ID : 0, (uint32_t)index);
    int mid = y1 + (y2 - y1) * 11 / 20;
    if (!enabled) {
        /* A greyed tile (no database): the season in a smaller font and the tag on two lines, so that
         * "No database yet" fits a tile of classic mode. */
        int split = y1 + (y2 - y1) * 7 / 20;
        int t = widget(1, x1 + 2, y1 + 3, x2 - 2, split, 0x30, rgb565(fill_rgb), 0xc, 1, text_colour, title, 0, 0);
        char two[64];
        const char *space = strrchr(tag, ' ');
        if (space) snprintf(two, sizeof two, "%.*s\n%s", (int)(space - tag), tag, space + 1);
        else snprintf(two, sizeof two, "%s", tag);
        int g = widget(1, x1 + 2, split, x2 - 2, y2 - 1, 0x30, rgb565(fill_rgb), 0xc, 1, text_colour, two, 0, 0);
        mark(w, t, g, MARK_TILE, (Rgb){90, 90, 100}, 0, 0);
        return;
    }
    int t = widget(2, x1 + 2, y1 + 4, x2 - 2, mid, 0x30, rgb565(fill_rgb), 0xc, 3, text_colour, title, TILE_ID,
                   (uint32_t)index);
    int tw = (x2 - x1) * 4 / 5;
    int tx = x1 + (x2 - x1 - tw) / 2;
    if (is_star) {                  /* the tag moves left: the star goes at its right (season_screen_widget_drawn) */
        tx = x1 + 3;
        tw = x2 - x1 - 20;
    }
    int g = widget(2, tx, mid + 2, tx + tw, y2 - 4, 0x30, rgb565(tag_colour), 0xc, 1, rgb565((Rgb){255, 255, 255}), tag,
                   TILE_ID, (uint32_t)index);
    mark(w, t, g, MARK_TILE, enabled ? era : (Rgb){90, 90, 100}, is_star, chosen);
}

static void band(int x1, int y1, int x2, int y2, Rgb colour, const char *heading, const char *note)
{
    int w = widget(1, x1, y1, x2, y2, 1, 0, 1, 1, 0, "", 0, 0);
    mark(w, -1, -1, MARK_BAND, colour, 0, 0);
    char s[160];
    snprintf(s, sizeof s, "%s", heading);
    label(x1 + 6, y1, x1 + 150, y2, 1, 2, rgb565((Rgb){255, 255, 255}), s);
    if (note) label(x1 + 150, y1, x2, y2, 1, 1, colour16(TEXT_LIGHT), note);
}

/* The versions of a season: its own database first, then the versions of seasons_db_scan. */
typedef struct { const char *name; const char *value; int own; } Version;

static int versions_of(int index, Version *v, int max)
{
    const SeasonsEntry *s = &list.season[index];
    int n = 0;
    if (s->database && n < max) v[n++] = (Version){s->database, NULL, 1};
    for (int i = 0; i < s->extra_count && n < max; i++) v[n++] = (Version){s->extra[i].db->name, s->extra[i].value, 0};
    return n;
}

static const char *start_month(void) { return "July"; }

/* The strip: what the chosen tile starts. */
static void strip(int sel, int y1, int y2)
{
    uint32_t yellow = colour16(TEXT_YELLOW), light = colour16(TEXT_LIGHT), white = rgb565((Rgb){255, 255, 255});
    panel(LEFT, y1, RIGHT, y2);
    int state = (int)node_slot(4);
    int line2 = y1 + 48;
    char a[200], b[200];
    if (sel < 0) {
        label(LEFT + 10, y1 + 4, RIGHT - 10, y1 + 44, 1, 3, light, "Click a season to see its database.");
        return;
    }
    int enabled = 1, custom = sel == CUSTOM_TILE;
    int year;
    if (custom) {
        year = (int)node_slot(3);
        if (year < SEASONS_FIRST || year >= SEASONS_FIRST + SEASONS_COUNT) year = 2025;
        label(LEFT + 10, y1 + 2, LEFT + 165, y1 + 30, 1, 4, yellow, "Custom");
        snprintf(a, sizeof a, "Plays as %d/%02d", year, (year + 1) % 100);
        label(LEFT + 10, y1 + 28, LEFT + 165, y1 + 44, 1, 1, light, a);
        int folder = (int)node_slot(2);
        if (list.custom_count) {
            dbfolders_label(list.custom[folder % list.custom_count], b, 40);
            label(LEFT + 168, y1 + 2, LEFT + 370, y1 + 26, 1, 3, white, b);
            label(LEFT + 168, y1 + 26, LEFT + 525, y1 + 44, 1, 1, light, CREDIT_CUSTOM);
        } else {
            enabled = 0;
            label(LEFT + 168, y1 + 2, LEFT + 370, y1 + 26, 1, 3, white, "No folder found");
            label(LEFT + 168, y1 + 26, LEFT + 525, y1 + 44, 1, 1, light, "No other folder in Databases");
        }
    } else {
        const SeasonsEntry *s = &list.season[sel];
        year = s->start_year;
        Version v[SEASONS_EXTRA_MAX + 1];
        int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1);
        int ver = (int)node_slot(1);
        if (ver >= nv) ver = 0;
        label(LEFT + 10, y1 + 2, LEFT + 165, y1 + 30, 1, 4, yellow, s->label);
        snprintf(a, sizeof a, "%s era", era_heading[s->era]);
        label(LEFT + 10, y1 + 28, LEFT + 165, y1 + 44, 1, 1, light, a);
        const char *name = nv ? v[ver].name : "No database yet";
        if (nv && !strncmp(name, "3.9.", 4)) snprintf(b, sizeof b, "%s data", name);
        else snprintf(b, sizeof b, "%s", name);
        label(LEFT + 168, y1 + 2, LEFT + 370, y1 + 26, 1, 3, white, b);
        if (nv) label(LEFT + 168, y1 + 26, LEFT + 525, y1 + 44, 1, 1, light, season_screen_credit(name));
        if (s->status == SEASONS_MISSING && v[ver].own) enabled = 0;
        if (s->status == SEASONS_NO_DATABASE) enabled = 0;
    }
    snprintf(a, sizeof a, "Starts %s %d", start_month(), year);
    label(LEFT + 375, y1 + 2, LEFT + 525, y1 + 26, 1, 3, white, a);
    const char *note = "Ready to play";
    if (!custom) {
        const SeasonsEntry *s = &list.season[sel];
        Version v[SEASONS_EXTRA_MAX + 1];
        int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1);
        int ver = (int)node_slot(1);
        if (ver >= nv) ver = 0;
        if (nv && v[ver].own && s->status == SEASONS_IN_DOWNLOAD) note = "Unpacks at the first start";
        if (nv && v[ver].own && s->status == SEASONS_MISSING) note = "Not in this game folder";
    } else if (!list.custom_count) {
        note = "";
    }
    label(RIGHT - 160, y1 + 30, RIGHT - 8, y1 + 44, 0xc, 1, light, note);
    snprintf(a, sizeof a, "Choose %d/%02d", year, (year + 1) % 100);
    button(RIGHT - 160, y1 + 4, RIGHT - 8, y1 + 29, 3, state == 1 ? "Please wait" : a, CHOOSE_ID, 0,
           enabled && state != 1);

    /* Line 2: the unpack, an error, the version choice or the Custom choices. */
    if (state == 1) {
        label(LEFT + 10, line2, RIGHT - 10, y2 - 4, 1, 2, yellow,
              "Unpacking the season database into Databases. This takes a few seconds.");
        return;
    }
    if (state == 2) {
        label(LEFT + 10, line2, RIGHT - 10, y2 - 4, 1, 2, rgb565((Rgb){255, 120, 100}), error_text);
        return;
    }
    if (custom) {
        label(LEFT + 10, line2, LEFT + 165, y2 - 4, 1, 2, light, "Its season:");
        button(LEFT + 168, line2 + 2, LEFT + 205, y2 - 6, 2, "<", CUSTOM_YEAR_ID, 0, year > SEASONS_FIRST);
        snprintf(a, sizeof a, "%d/%02d", year, (year + 1) % 100);
        label(LEFT + 208, line2, LEFT + 290, y2 - 4, 0xc, 3, yellow, a);
        button(LEFT + 293, line2 + 2, LEFT + 328, y2 - 6, 2, ">", CUSTOM_YEAR_ID, 1,
               year < SEASONS_FIRST + SEASONS_COUNT - 1);
        if (list.custom_count > 1)
            button(LEFT + 375, line2 + 2, RIGHT - 8, y2 - 6, 2, "Next folder", FOLDER_ID, 0, 1);
        else if (!list.custom_count)
            label(LEFT + 340, line2, RIGHT - 10, y2 - 4, 1, 1, light,
                  "Copy a database folder into Databases in the save folder, then open this screen again.");
        return;
    }
    Version v[SEASONS_EXTRA_MAX + 1];
    int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1);
    if (nv < 2) {
        label(LEFT + 10, line2, RIGHT - 10, y2 - 4, 1, 1, light,
              nv ? "Other versions of this season show here once you add them to Databases."
                 : "The CM 01/02 Update Team has made no database for this season yet.");
        return;
    }
    int ver = (int)node_slot(1), page = (int)node_slot(5);
    if (ver >= nv) ver = 0;
    label(LEFT + 10, line2, LEFT + 100, y2 - 4, 1, 2, light, "Version:");
    int per = nv > 4 ? 3 : 4;
    if (page * per >= nv) page = 0;
    int x = LEFT + 100, step = (RIGHT - 8 - x) / 4;
    for (int i = page * per, k = 0; i < nv && k < per; i++, k++, x += step) {
        char t[80];
        if (!strncmp(v[i].name, "3.9.", 4)) snprintf(t, sizeof t, "%s data", v[i].name);
        else snprintf(t, sizeof t, "%s", v[i].name);
        if (i == ver) {
            widget(1, x, line2 + 2, x + step - 6, y2 - 6, 0x221, colour16(TEXT_YELLOW), 0xc, 2, yellow, t, 0, 0);
        } else {
            button(x, line2 + 2, x + step - 6, y2 - 6, 2, t, VERSION_ID, (uint32_t)i, 1);
        }
    }
    if (per == 3) button(x, line2 + 2, x + step - 6, y2 - 6, 2, "More versions", PAGE_ID, 0, 1);
}

/* ---- Build and input ---- */

static void season_build(void)
{
    if (!scratch) scratch = win_static_alloc(TEXTS * TEXT_ROOM);
    mark_count = 0;
    /* The picture of the screen below: the start screen's own (its node title). */
    uint32_t node = rd32(g_mem, TOP_NODE), prev = node ? rd32(g_mem, node + NODE_PREV) : 0;
    char title[260] = "";
    if (prev) snprintf(title, sizeof title, "%.259s", win_str(prev + NODE_TITLE));
    uint32_t p[1] = {text(title)};
    call(SET_PICTURE, 1, p);
    uint32_t h[3] = {colour16(HEADING_RED), colour16(TEXT_LIGHT), text("Choose Season")};
    call_this(HEADING, SCREENS, 3, h);
    uint32_t sb[2] = {4, 0};
    call(SIDE_BAR, 2, sb);

    int sel = (int)node_slot(0) - 1;
    if (sel < -1 || sel >= TILES) sel = -1;
    int y = TOP, row_y = 0;
    for (int era = 0; era < 5; era++) {
        int first = -1, count = 0;
        for (int i = 0; i < SEASONS_COUNT; i++)
            if ((int)list.season[i].era == era) {
                if (first < 0) first = i;
                count++;
            }
        int band_x2 = era == 4 ? col_x2(COLUMNS - 3) : RIGHT;
        band(LEFT, y, band_x2, y + BAND_H, era_rgb[era], era_heading[era], era_note[era]);
        if (era == 4) band(col_x1(COLUMNS - 2), y, RIGHT, y + BAND_H, custom_rgb, "Custom database", NULL);
        row_y = y + BAND_H + 2;
        for (int k = 0; k < count; k++) {
            int i = first + k, c = k % COLUMNS;
            if (k && !c) row_y += TILE_H + 3;
            const SeasonsEntry *s = &list.season[i];
            int enabled = s->status != SEASONS_NO_DATABASE;
            tile(col_x1(c), row_y, col_x2(c), row_y + TILE_H, s->label, status_tag(s), era_rgb[era], tag_rgb(s), i,
                 enabled, starred[i], sel == i);
        }
        if (era == 4) {
            char tag[40];
            snprintf(tag, sizeof tag, list.custom_count == 1 ? "1 folder" : "%d folders", list.custom_count);
            tile(col_x1(COLUMNS - 2), row_y, col_x2(COLUMNS - 1), row_y + TILE_H, "Custom", tag, custom_rgb,
                 list.custom_count ? (Rgb){26, 122, 26} : (Rgb){85, 85, 85}, CUSTOM_TILE, 1, 0, sel == CUSTOM_TILE);
        }
        y = row_y + TILE_H + 5;
    }

    int strip_y = y + 2;
    strip(sel, strip_y, strip_y + 86);
    int link_y = strip_y + 90;
    label(LEFT + 4, link_y, RIGHT - 210, link_y + 22, 1, 1, colour16(TEXT_LIGHT),
          "More versions of each season are on the Update Team's Databases forum.");
    button(RIGHT - 200, link_y, RIGHT, link_y + 22, 2, "Open champman0102.co.uk", LINK_ID, 0, 1);

    uint32_t back_arg[1] = {0};
    uint32_t back = call_this(CANNOT_GO_BACK, SCREENS, 1, back_arg) == 0;
    uint32_t bar[2] = {back, 0};
    call(BACK_BAR, 2, bar);
    fprintf(stderr, "recomp: choose season built, tile %d\n", sel + 1);
    rt_ret_cdecl(0);
}

static void choose_select(int tile_index)
{
    set_slot(0, (uint32_t)tile_index + 1);
    set_slot(1, 0);
    set_slot(4, 0);
    set_slot(5, 0);
}

/* Starts the choice: the options database and season, and the season of the next new game. Returns 1 when
 * the screen goes back to Setup Game, 0 when it shows an error. */
static int choose_apply(void)
{
    int sel = (int)node_slot(0) - 1;
    char value[DBFOLDER_NAME + 8];
    int year;
    if (sel == CUSTOM_TILE) {
        if (!list.custom_count) return 0;
        year = (int)node_slot(3);
        if (year < SEASONS_FIRST || year >= SEASONS_FIRST + SEASONS_COUNT) year = 2025;
        snprintf(value, sizeof value, "folder:%s", list.custom[node_slot(2) % (uint32_t)list.custom_count]);
    } else if (sel >= 0 && sel < SEASONS_COUNT) {
        const SeasonsEntry *s = &list.season[sel];
        year = s->start_year;
        Version v[SEASONS_EXTRA_MAX + 1];
        int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1);
        int ver = (int)node_slot(1);
        if (ver >= nv) ver = 0;
        if (!nv) return 0;
        if (!v[ver].own) {
            snprintf(value, sizeof value, "%s", v[ver].value);
        } else if (year <= 2002) {
            snprintf(value, sizeof value, "3.9.68");
        } else {
            SeasonsPrepare r;
            memset(&r, 0, sizeof r);
            SeasonsPrepareStatus st = seasons_db_prepare(year, &r);
            fprintf(stderr, "recomp: choose season %d: prepare status %d, folder %s, unpacked %d\n", year, (int)st,
                    r.name, r.unpacked);
            if (st != SEASONS_PREPARE_OK) {
                snprintf(error_text, sizeof error_text, "%s",
                         r.error[0] ? r.error : "The database of this season cannot be started.");
                seasons_db_scan(&list);
                return 0;
            }
            snprintf(value, sizeof value, "folder:%s", r.name);
        }
    } else {
        return 0;
    }
    char season_word[16];
    snprintf(season_word, sizeof season_word, "%d/%02d", year, (year + 1) % 100);
    int a = options_set(OPT_DATABASE, value), b = options_set(OPT_SEASON, season_word);
    if (a < 0 || b < 0)
        fprintf(stderr, "cm0102_recomp: the season choice applies to this run only: settings.txt cannot be written\n");
    win_data_set_changed();
    season_set_new_game(year, season_screen_rules_choice());
    fprintf(stderr, "recomp: choose season: %s with database = %s\n", season_word, value);
    return 1;
}

static uint32_t handled_press = 0xffffffffu;

static void season_input(void)
{
    int16_t w = (int16_t)rt_arg(0);
    if ((int)node_slot(4) == 1) {
        /* The screen with the unpack line is on show: the unpack runs now. */
        if (choose_apply()) {
            set_slot(4, 0);
            rt_ret_cdecl(GO_BACK);
        } else {
            set_slot(4, 2);
            rt_ret_cdecl(REBUILD);
        }
        return;
    }
    /* One click for each press of the mouse button, as the options box (a held button repeats). */
    if (w >= 0 && win_press_count() == handled_press) w = -1;
    if (w >= 0) {
        uint32_t at = display() + WIDGETS + 8 + (uint32_t)w * WIDGET_SIZE;
        uint32_t id = (uint16_t)rd32(g_mem, at), data = rd32(g_mem, at + 0x40);
        uint32_t r = 0;
        if (id == TILE_ID && data < TILES) {
            choose_select((int)data);
            r = REBUILD;
        } else if (id == VERSION_ID) {
            set_slot(1, data);
            set_slot(4, 0);
            r = REBUILD;
        } else if (id == PAGE_ID) {
            set_slot(5, node_slot(5) + 1);
            r = REBUILD;
        } else if (id == FOLDER_ID) {
            set_slot(2, list.custom_count ? (node_slot(2) + 1) % (uint32_t)list.custom_count : 0);
            r = REBUILD;
        } else if (id == CUSTOM_YEAR_ID) {
            int year = (int)node_slot(3) + (data ? 1 : -1);
            if (year >= SEASONS_FIRST && year < SEASONS_FIRST + SEASONS_COUNT) set_slot(3, (uint32_t)year);
            r = REBUILD;
        } else if (id == LINK_ID) {
            fprintf(stderr, "recomp: choose season: open %s\n", LINK_URL);
            if (!getenv("RECOMP_SCRIPT") && SDL_OpenURL(LINK_URL))
                fprintf(stderr, "cm0102_recomp: the page %s could not be opened: %s\n", LINK_URL, SDL_GetError());
            r = 0;
        } else if (id == CHOOSE_ID) {
            int sel = (int)node_slot(0) - 1;
            Version v[SEASONS_EXTRA_MAX + 1];
            int unpack = sel >= 0 && sel < SEASONS_COUNT && list.season[sel].status == SEASONS_IN_DOWNLOAD &&
                         versions_of(sel, v, SEASONS_EXTRA_MAX + 1) && v[node_slot(1) < (uint32_t)SEASONS_EXTRA_MAX + 1
                                                                          ? node_slot(1) : 0].own;
            if (unpack) {
                set_slot(4, 1);             /* show the unpack line first; the unpack runs at the next call */
                r = REBUILD;
            } else if (choose_apply()) {
                r = GO_BACK;
            } else {
                set_slot(4, 2);
                r = REBUILD;
            }
        }
        if (r) {
            handled_press = win_press_count();
            rt_ret_cdecl(r);
            return;
        }
    }
    uint32_t a[1] = {rt_arg(0)};
    uint32_t r = call(SIDE_BAR_INPUT, 1, a);
    if (!r) r = call(GAME_INPUT, 1, a);
    rt_ret_cdecl(r);
}

static uint32_t build_fn(void) { return rt_host_fn("season_build", season_build); }
uint32_t season_screen_build_fn(void) { return build_fn(); }

/* ---- Setup Game: the changing rules switch of the next new game ---- */
static int rules_choice = -1;       /* -1: the option changing_rules */

int season_screen_rules_choice(void) { return rules_choice; }
int season_screen_rules_on(void) { return rules_choice < 0 ? options_bool(OPT_CHANGING_RULES) : rules_choice; }
void season_screen_toggle_rules(void) { rules_choice = !season_screen_rules_on(); }

void season_screen_open(void)
{
    seasons_db_scan(&list);
    scan_saves();
    error_text[0] = 0;
    /* The tile of the current choice. */
    int year = season_screen_year(), tile_index = year - SEASONS_FIRST, ver = 0, folder = 0;
    const char *db = options_string(OPT_DATABASE);
    if (!strncmp(db, "folder:", 7)) {
        for (int i = 0; i < list.custom_count; i++)
            if (!strcmp(list.custom[i], db + 7)) {
                tile_index = CUSTOM_TILE;
                folder = i;
            }
    }
    if (tile_index >= 0 && tile_index < SEASONS_COUNT) {
        const SeasonsEntry *s = &list.season[tile_index];
        for (int i = 0; i < s->extra_count; i++)
            if (!strcmp(s->extra[i].value, db)) ver = i + 1;
    }
    uint32_t p[5] = {build_fn(), rt_host_fn("season_input", season_input), 0, 0, 0};
    call_this(PUSH, SCREENS, 5, p);
    set_slot(0, (uint32_t)tile_index + 1);
    set_slot(1, (uint32_t)ver);
    set_slot(2, (uint32_t)folder);
    set_slot(3, (uint32_t)(tile_index == CUSTOM_TILE ? year : 2025));
    set_slot(4, 0);
    set_slot(5, 0);
    handled_press = win_press_count();          /* the press that opened the screen does not click in it */
    fprintf(stderr, "recomp: choose season opened, %d custom folders\n", list.custom_count);
}
