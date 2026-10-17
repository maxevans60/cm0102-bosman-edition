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
#define MAX_WIDGETS 1200
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
#define BACK_ID 0x7d47u             /* Back: the screen goes back to Setup Game */
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

/* ---- Layout (Max's choice of 2026-10-17, layout A: tiles by era) ----
 *
 * Every number of the layout is in this block, as screen pixels of each display mode, so a change of the
 * look is a change of these tables. The widget create of the game scales classic numbers (800 x 600) to the
 * screen (display.c), so px() and py() turn a pixel of the mode into a classic number first. */

/* A group of tiles under one heading bar: an era (a run of seasons) or the Custom database tile. */
typedef struct {
    int era;                        /* SeasonsEra, or -1 for the Custom database */
    int first, count;               /* the seasons of the group: start years first .. first + count - 1 */
    int heading;                    /* 0: a second row of an era, with an empty bar */
} Group;

typedef struct {
    int w, h;                       /* the screen of the mode */
    int left, right;                /* the content between the side bar and the right edge */
    int top, row_gap, group_gap;    /* the first heading bar; the room between rows and between groups */
    int bar_h, bar_gap;             /* a heading bar and the room under it */
    int tile_w, tile_h, tile_gap;
    uint32_t year_font;             /* the font of the season on a tile */
    int strip_top, strip_h;         /* the strip of the chosen season */
    int strip_pad, strip_left_w;    /* its inner margin and the width of the column of the year */
    int version_h, version_w;       /* the version buttons */
    int link_top, link_h;
    int bottom_top, bottom_h, start_w;  /* Back and Start */
    Group rows[4][3];               /* up to 4 rows of up to 3 groups; count 0 ends a row */
} Layout;

static const Layout layout_classic = {
    800, 600, 100, 790, 76, 6, 10, 18, 3, 80, 44, 6, 2,
    366, 132, 12, 140, 22, 132,
    504, 26,
    555, 36, 230,
    {
        {{SEASONS_ERA_ORIGINAL, 2001, 2, 1}, {SEASONS_ERA_2000S, 2003, 6, 1}},
        {{SEASONS_ERA_2010S, 2009, 8, 1}},
        {{SEASONS_ERA_2010S, 2017, 3, 0}, {SEASONS_ERA_EARLY_2020S, 2020, 4, 1}},
        {{SEASONS_ERA_LEAGUE_PHASE, 2024, 2, 1}, {-1, 0, 2, 1}},
    },
};

static const Layout layout_wide = {
    1280, 800, 160, 1264, 100, 12, 16, 24, 4, 94, 74, 6, 3,
    440, 196, 20, 220, 30, 220,
    650, 34,
    740, 47, 330,
    {
        {{SEASONS_ERA_ORIGINAL, 2001, 2, 1}, {SEASONS_ERA_2000S, 2003, 6, 1}},
        {{SEASONS_ERA_2010S, 2009, 11, 1}},
        {{SEASONS_ERA_EARLY_2020S, 2020, 4, 1}, {SEASONS_ERA_LEAGUE_PHASE, 2024, 2, 1}, {-1, 0, 2, 1}},
    },
};

static const Layout *lay(void) { return rt_screen_w >= 1280 ? &layout_wide : &layout_classic; }

/* A pixel of the mode as a classic number, for the widget create. */
static int px(int x) { return x * 800 / (int)lay()->w; }
static int py(int y) { return y * 600 / (int)lay()->h; }

/* The look of each era (Q166, Q240): the bar colour, a lighter colour for text on the dark strip and the run of
 * seasons. The era names come from seasons_db_era_name (seasons_db.c, Q343): one table for every screen. */
typedef struct { uint8_t r, g, b; } Rgb;
typedef struct { Rgb bar, text; const char *range; } EraLook;
static const EraLook era_look[5] = {
    {{224, 26, 50}, {255, 122, 138}, "2001/02 to 2002/03"},
    {{224, 138, 0}, {255, 184, 74}, "2003/04 to 2008/09"},
    {{44, 154, 68}, {111, 216, 138}, "2009/10 to 2019/20"},
    {{47, 98, 224}, {142, 170, 255}, "2020/21 to 2023/24"},
    {{138, 63, 208}, {199, 154, 242}, "From 2024/25"},
};
static const Rgb custom_bar = {90, 90, 114};
static const char *const custom_heading = "Custom database", *const custom_range = "your own folder";

/* The tags of the tiles (the mockup): text and box colours. */
typedef struct { const char *text; Rgb box, ink; int boxed; } Tag;
static const Tag tag_official = {"Official", {217, 168, 0}, {26, 18, 0}, 1};
static const Tag tag_installed = {"Installed", {42, 154, 68}, {255, 255, 255}, 1};
static const Tag tag_included = {"Included", {58, 62, 104}, {230, 233, 255}, 1};
static const Tag tag_missing = {"Missing", {128, 32, 32}, {255, 255, 255}, 1};
static const Tag tag_none = {"No database", {40, 40, 52}, {154, 154, 171}, 0};

static const Rgb yellow_rgb = {245, 224, 0}, navy_rgb = {14, 18, 66}, white_rgb = {255, 255, 255};
static const Rgb pale_rgb = {214, 220, 255}, grey_text = {138, 138, 156}, greyed_box = {40, 40, 52};
static const Rgb start_ink = {26, 20, 0}, error_rgb = {255, 120, 100};

static uint16_t rgb565(Rgb c) { return (uint16_t)(((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3)); }

/* c over the navy of the tiles with weight w of 256: the tint of a tile. */
static Rgb mix(Rgb c, int w)
{
    Rgb n = navy_rgb;
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
    char path[2400];
    *exists = dbfolders_path(win_databases_root(), name, path, sizeof path);
    return *exists ? seasons_db_identify(path) : NULL;
}

int season_screen_year(void)
{
    if (!season_code_on()) return SEASONS_FIRST;
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
    if (!season_code_on()) {                            /* RECOMP_SEASON_CODE=off: no folder is read */
        dbfolders_label(db + 7, out, room);
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

typedef enum { MARK_TILE, MARK_BAND } MarkKind;
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
    draw_polygon(t, xy, 10, rgb565(yellow_rgb), 256);
}

void season_screen_widget_drawn(uint32_t w)
{
    if (!mark_count || !on_top() || rd32(g_mem, NO_DRAW)) return;
    if (w < display() + WIDGETS || (w - display() - WIDGETS) % WIDGET_SIZE) return;
    int index = (int)((w - display() - WIDGETS) / WIDGET_SIZE);
    const Mark *m = NULL;
    for (int i = 0; i < mark_count; i++)
        for (int k = 0; k < 3; k++)
            if (marks[i].widgets[k] == index) m = &marks[i];
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
        /* The heading bar: the era colour, fading out towards 85 % of the width. */
        int w85 = (x2 - x1) * 85 / 100;
        for (int x = x1; x <= x2; x++) {
            int a = x - x1 >= w85 ? 0 : 240 - 240 * (x - x1) / (w85 ? w85 : 1);
            if (a > 0) fill(&t, x, y1, x, y2, c, a);
        }
        return;
    }
    int edge = rt_screen_w >= 1280 ? 3 : 3;
    uint16_t yellow = rgb565(yellow_rgb);
    fill(&t, x1, y1, x2, y1 + edge - 1, m->chosen ? yellow : c, 256);     /* the top edge */
    if (m->chosen) {                                                       /* the yellow frame */
        fill(&t, x1, y2 - 1, x2, y2, yellow, 256);
        fill(&t, x1, y1, x1 + 1, y2, yellow, 256);
        fill(&t, x2 - 1, y1, x2, y2, yellow, 256);
    }
    if (m->star) {                                                         /* top right */
        int r = rt_screen_w >= 1280 ? 8 : 6;
        star(&t, x2 - r - 3, y1 + edge + r, r);
    }
}

/* ---- The tiles ---- */

static Tag status_tag(const SeasonsEntry *s)
{
    switch (s->status) {
    case SEASONS_OFFICIAL: return tag_official;
    case SEASONS_UNPACKED: return tag_installed;
    case SEASONS_IN_DOWNLOAD: return tag_included;
    case SEASONS_NO_DATABASE: return tag_none;
    default: return tag_missing;
    }
}

/* One tile, in pixels of the mode: a button in the tint of its era, the season and the status tag. The
 * season and the tag are buttons with the same action, so a click anywhere on the tile chooses it. A tile
 * with no database is greyed and takes no click. */
static void tile(int x1, int y1, int x2, int y2, const char *title, Tag tag, Rgb era, int index, int enabled,
                 int is_star, int chosen)
{
    const Layout *L = lay();
    uint32_t flags = enabled ? 2 : 1, id = enabled ? TILE_ID : 0, data = enabled ? (uint32_t)index : 0;
    uint16_t fill_c = rgb565(enabled ? mix(era, 80) : greyed_box);
    uint16_t year_c = rgb565(!enabled ? grey_text : chosen ? yellow_rgb : white_rgb);
    int w = widget(flags, px(x1), py(y1), px(x2), py(y2), 0x30, fill_c, 0xc, 3, year_c, "", id, data);
    int mid = y1 + (y2 - y1) * 3 / 5;
    int t = widget(flags, px(x1 + 3), py(y1 + 6), px(x2 - 3), py(mid), 0x30, fill_c, 0xc, L->year_font, year_c, title,
                   id, data);
    int th = (y2 - y1) / 4, tw = tag.boxed ? (x2 - x1) * 3 / 4 : x2 - x1 - 6;
    int tx = x1 + (x2 - x1 - tw) / 2, ty = mid + (y2 - mid - th) / 2;
    int g = widget(flags, px(tx), py(ty), px(tx + tw), py(ty + th), 0x30, tag.boxed ? rgb565(tag.box) : fill_c, 0xc, 1,
                   rgb565(tag.ink), tag.text, id, data);
    mark(w, t, g, MARK_TILE, enabled ? era : (Rgb){85, 85, 106}, is_star, chosen);
}

/* A heading bar over a group: the name and the run of seasons, on the era colour that fades out. */
static void bar(int x1, int y1, int x2, int y2, Rgb colour, const char *name, const char *range)
{
    int w = widget(1, px(x1), py(y1), px(x2), py(y2), 1, 0, 1, 1, 0, "", 0, 0);
    mark(w, -1, -1, MARK_BAND, colour, 0, 0);
    if (!name) return;
    /* The widths of the game's fonts 2 and 1, about 8 and 6 pixels for each letter: the range shows only when it
     * fits after the name, so a narrow group (two tiles in classic) shows the name alone. */
    int name_w = (int)strlen(name) * 8 + 12, range_w = (int)strlen(range) * 6;
    label(px(x1 + 6), py(y1), px(x1 + 6 + name_w), py(y2), 1, 2, rgb565(white_rgb), name);
    if (x1 + 6 + name_w + range_w <= x2)
        label(px(x1 + 6 + name_w), py(y1), px(x2), py(y2), 1, 1, rgb565(pale_rgb), range);
}

/* ---- The strip ---- */

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

static int folder_name_ok(const char *name);
static int chosen_version(int sel, int nv);

/* A database name for the player: "Official data 3.9.68" for the data sets. */
static void db_title(const char *name, char *out, size_t room)
{
    if (!strncmp(name, "3.9.", 4)) snprintf(out, room, "Official data %s", name);
    else snprintf(out, room, "%s", name);
}

/* A grey button in pixels of the mode. */
static void pbutton(int x1, int y1, int x2, int y2, uint32_t font, const char *s, uint32_t id, uint32_t data, int on)
{
    button(px(x1), py(y1), px(x2), py(y2), font, s, id, data, on);
}

static void plabel(int x1, int y1, int x2, int y2, uint32_t flags, uint32_t font, Rgb colour, const char *s)
{
    label(px(x1), py(y1), px(x2), py(y2), flags, font, rgb565(colour), s);
}

/* The strip shows the chosen tile. Returns 1 when Start can start it. */
static int strip(int sel)
{
    const Layout *L = lay();
    int x1 = L->left, x2 = L->right, y1 = L->strip_top, y2 = L->strip_top + L->strip_h;
    int state = (int)node_slot(4);
    int custom = sel == CUSTOM_TILE;
    Rgb era_bar = custom ? custom_bar : sel >= 0 ? era_look[list.season[sel].era].bar : custom_bar;
    panel(px(x1), py(y1), px(x2), py(y2));
    widget(1, px(x1), py(y1), px(x2), py(y1 + 3), 0x30, rgb565(era_bar), 0xc, 1, 0, "", 0, 0);   /* the top line */
    if (sel < 0) {
        plabel(x1 + L->strip_pad, y1 + L->strip_pad, x2 - L->strip_pad, y1 + L->strip_pad + 30, 1, 3, pale_rgb,
               "Click a season to see its database.");
        return 0;
    }
    int lx = x1 + L->strip_pad, mx = lx + L->strip_left_w, rx = x2 - L->strip_pad;
    int line = (L->strip_h - 2 * L->strip_pad) / 5;          /* the rows of the strip */
    int ty = y1 + L->strip_pad;
    char a[200], b[200];
    int ok = 1, year;
    const char *note = NULL;

    if (custom) {
        year = (int)node_slot(3);
        if (year < SEASONS_FIRST || year >= SEASONS_FIRST + SEASONS_COUNT) year = 2025;
        plabel(lx, ty, mx, ty + 2 * line, 1, 6, yellow_rgb, "Custom");
        snprintf(a, sizeof a, "Season: %d/%02d", year, (year + 1) % 100);
        plabel(lx, ty + 2 * line, mx, ty + 3 * line, 1, 2, pale_rgb, a);
        int folder = (int)(node_slot(2) % (uint32_t)(list.custom_count ? list.custom_count : 1));
        if (list.custom_count) {
            dbfolders_label(list.custom[folder], b, 48);
            plabel(mx, ty, rx, ty + line + line / 2, 1, 3, white_rgb, b);
            if (!folder_name_ok(list.custom[folder])) {
                ok = 0;
                note = "Rename this folder: its name starts or ends with a space.";
            } else {
                note = "A folder in Databases. You name its season.";
            }
        } else {
            ok = 0;
            plabel(mx, ty, rx, ty + line + line / 2, 1, 3, white_rgb, "No folder found");
            note = "Copy a database folder into Databases in the save folder, then open this screen again.";
        }
    } else {
        const SeasonsEntry *s = &list.season[sel];
        year = s->start_year;
        Version v[SEASONS_EXTRA_MAX + 1];
        int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1), ver = chosen_version(sel, nv);
        plabel(lx, ty, mx, ty + 2 * line, 1, 6, yellow_rgb, s->label);
        snprintf(a, sizeof a, "%s era", seasons_db_era_name(s->era));
        plabel(lx, ty + 2 * line, mx, ty + 3 * line, 1, 2, era_look[s->era].text, a);
        db_title(nv ? v[ver].name : "No database", b, sizeof b);
        plabel(mx, ty, rx, ty + line + line / 2, 1, 3, white_rgb, b);
        if (nv) note = season_screen_credit(v[ver].name);
        if (!nv || s->status == SEASONS_NO_DATABASE || (s->status == SEASONS_MISSING && v[ver].own)) ok = 0;
    }
    snprintf(a, sizeof a, "Starts July %d", year);
    plabel(lx, ty + 3 * line, mx, ty + 4 * line, 1, 1, pale_rgb, a);
    if (note) plabel(mx, ty + line + line / 2, rx, ty + 2 * line + line / 2, 1, 1, pale_rgb, note);

    /* The row of choices: the unpack, an error, the versions or the Custom choices. */
    int cy = ty + 3 * line - L->version_h / 4, ch = L->version_h;
    if (state == 1) {
        snprintf(a, sizeof a, "Unpacking %d/%02d into Databases. This takes a few seconds.", year, (year + 1) % 100);
        plabel(mx, cy, rx, cy + ch, 1, 2, yellow_rgb, a);
        return 0;
    }
    if (state == 2) {
        plabel(mx, cy, rx, cy + ch, 1, 2, error_rgb, error_text);
        return ok;
    }
    int bx = mx + (L->w >= 1280 ? 80 : 60), bw = L->version_w, gap = L->tile_gap;
    if (custom) {
        plabel(mx, cy, bx, cy + ch, 1, 1, pale_rgb, "Season:");
        pbutton(bx, cy, bx + ch, cy + ch, 2, "<", CUSTOM_YEAR_ID, 0, year > SEASONS_FIRST);
        snprintf(a, sizeof a, "%d/%02d", year, (year + 1) % 100);
        plabel(bx + ch, cy, bx + ch + bw / 2, cy + ch, 0xc, 2, yellow_rgb, a);
        pbutton(bx + ch + bw / 2, cy, bx + 2 * ch + bw / 2, cy + ch, 2, ">", CUSTOM_YEAR_ID, 1,
                year < SEASONS_FIRST + SEASONS_COUNT - 1);
        if (list.custom_count > 1)
            pbutton(bx + 2 * ch + bw / 2 + 2 * gap, cy, bx + 2 * ch + bw / 2 + 2 * gap + bw, cy + ch, 2, "Next folder",
                    FOLDER_ID, 0, 1);
        return ok;
    }
    const SeasonsEntry *s = &list.season[sel];
    Version v[SEASONS_EXTRA_MAX + 1];
    int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1), ver = chosen_version(sel, nv);
    if (nv < 2) {
        const char *t = s->status == SEASONS_IN_DOWNLOAD ? "The first start unpacks this database into Databases."
                        : s->status == SEASONS_MISSING ? "This database is not in this game folder."
                                                       : "Other versions of this season show here once you add them "
                                                         "to Databases.";
        plabel(mx, cy, rx, cy + ch, 1, 1, pale_rgb, t);
        return ok;
    }
    plabel(mx, cy, bx, cy + ch, 1, 1, pale_rgb, "Version:");
    int fit = (rx - bx + gap) / (bw + gap), page = (int)node_slot(5);
    int per = nv > fit ? fit - 1 : nv;
    if (per < 1) per = 1;
    if (page < 0 || page * per >= nv) page = 0;
    int x = bx;
    for (int i = page * per, k = 0; i < nv && k < per; i++, k++, x += bw + gap) {
        char t[80];
        db_title(v[i].name, t, sizeof t);
        if (i == ver) widget(1, px(x), py(cy), px(x + bw), py(cy + ch), 0x221, rgb565(yellow_rgb), 0xc, 1,
                             rgb565(yellow_rgb), t, 0, 0);
        else pbutton(x, cy, x + bw, cy + ch, 1, t, VERSION_ID, (uint32_t)i, 1);
    }
    if (per < nv) pbutton(x, cy, x + bw, cy + ch, 1, "More versions", PAGE_ID, 0, 1);
    return ok;
}

/* ---- Build and input ---- */

static void season_build(void)
{
    if (!scratch) scratch = win_static_alloc(TEXTS * TEXT_ROOM);
    const Layout *L = lay();
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
    int y = L->top;
    for (int r = 0; r < 4 && L->rows[r][0].count; r++) {
        int x = L->left;
        for (int g = 0; g < 3 && L->rows[r][g].count; g++) {
            const Group *G = &L->rows[r][g];
            int custom = G->era < 0;
            int tiles = custom ? 1 : G->count;
            int width = custom ? G->count * L->tile_w + (G->count - 1) * L->tile_gap
                               : tiles * L->tile_w + (tiles - 1) * L->tile_gap;
            Rgb colour = custom ? custom_bar : era_look[G->era].bar;
            bar(x, y, x + width - 1, y + L->bar_h, colour,
                !G->heading ? NULL : custom ? custom_heading : seasons_db_era_name((SeasonsEra)G->era),
                custom ? custom_range : era_look[G->era].range);
            int ty = y + L->bar_h + L->bar_gap;
            if (custom) {
                char tag[40];
                snprintf(tag, sizeof tag, list.custom_count == 1 ? "1 folder" : "%d folders", list.custom_count);
                Tag t = tag_included;
                t.text = tag;
                tile(x, ty, x + width - 1, ty + L->tile_h, "Custom", t, custom_bar, CUSTOM_TILE, 1, 0, sel == CUSTOM_TILE);
            } else {
                for (int k = 0; k < tiles; k++) {
                    int i = G->first - SEASONS_FIRST + k, tx = x + k * (L->tile_w + L->tile_gap);
                    if (i < 0 || i >= SEASONS_COUNT) continue;
                    const SeasonsEntry *s = &list.season[i];
                    tile(tx, ty, tx + L->tile_w - 1, ty + L->tile_h, s->label, status_tag(s), era_look[s->era].bar, i,
                         s->status != SEASONS_NO_DATABASE, starred[i], sel == i);
                }
            }
            x += width + L->group_gap;
        }
        y += L->bar_h + L->bar_gap + L->tile_h + L->row_gap;
    }

    int ok = strip(sel);
    int link_w = L->w >= 1280 ? 300 : 200;
    plabel(L->left + 4, L->link_top, L->right - link_w - 8, L->link_top + L->link_h, 1, 1, pale_rgb,
           "More versions of each season are on the Update Team's forum.");
    pbutton(L->right - link_w, L->link_top, L->right, L->link_top + L->link_h, 2, "Open champman0102.co.uk", LINK_ID,
            0, 1);

    /* Back, and the yellow Start button of the chosen season. */
    int by = L->bottom_top, bh = L->bottom_h, sx = L->right - L->start_w;
    pbutton(L->left, by, sx - 4, by + bh, 3, "Back", BACK_ID, 0, 1);
    char start[40];
    int state = (int)node_slot(4);
    int year = sel == CUSTOM_TILE ? (int)node_slot(3) : sel >= 0 ? SEASONS_FIRST + sel : 0;
    if (state == 1) snprintf(start, sizeof start, "Please wait");
    else if (year) snprintf(start, sizeof start, "Start %d/%02d", year, (year + 1) % 100);
    else snprintf(start, sizeof start, "Start");
    if (ok && state != 1)
        widget(2, px(sx), py(by), px(L->right), py(by + bh), 0x30, rgb565(yellow_rgb), 0xc, 3, rgb565(start_ink), start,
               CHOOSE_ID, 0);
    else
        widget(1, px(sx), py(by), px(L->right), py(by + bh), 0x30, colour16(BUTTON_GREY), 0x2c, 3,
               colour16(TEXT_LIGHT), start, 0, 0);
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
/* 1 when the option database can hold the folder name as it is: options.c trims spaces at the ends of a value
 * and a line cannot hold a control byte, so such a folder would start another data set. */
static int folder_name_ok(const char *name)
{
    size_t n = strlen(name);
    if (!n || name[0] == ' ' || name[n - 1] == ' ' || name[0] == '\t' || name[n - 1] == '\t') return 0;
    for (size_t i = 0; i < n; i++)
        if ((unsigned char)name[i] < 0x20 || name[i] == 0x7f) return 0;
    return 1;
}

/* The version index of the strip, checked against the versions of the tile. */
static int chosen_version(int sel, int nv)
{
    int ver = (int)node_slot(1);
    return sel >= 0 && sel < SEASONS_COUNT && ver >= 0 && ver < nv ? ver : 0;
}

static int choose_apply(void)
{
    int sel = (int)node_slot(0) - 1;
    char value[DBFOLDER_NAME + 8];
    int year;
    if (sel == CUSTOM_TILE) {
        if (!list.custom_count) return 0;
        year = (int)node_slot(3);
        if (year < SEASONS_FIRST || year >= SEASONS_FIRST + SEASONS_COUNT) year = 2025;
        const char *name = list.custom[node_slot(2) % (uint32_t)list.custom_count];
        if (!folder_name_ok(name)) {
            snprintf(error_text, sizeof error_text, "Rename this folder: its name starts or ends with a space.");
            return 0;
        }
        snprintf(value, sizeof value, "folder:%s", name);
    } else if (sel >= 0 && sel < SEASONS_COUNT) {
        const SeasonsEntry *s = &list.season[sel];
        year = s->start_year;
        Version v[SEASONS_EXTRA_MAX + 1];
        int nv = versions_of(sel, v, SEASONS_EXTRA_MAX + 1);
        int ver = chosen_version(sel, nv);
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
    if (!strncmp(value, "folder:", 7) && !folder_name_ok(value + 7)) {
        snprintf(error_text, sizeof error_text, "Rename this folder: its name starts or ends with a space.");
        return 0;
    }
    int a = options_set(OPT_DATABASE, value), b = options_set(OPT_SEASON, season_word);
    if (!a || !b || strcmp(options_string(OPT_DATABASE), value)) {
        /* options_set refused or changed the value: the next new game would start another data set. */
        snprintf(error_text, sizeof error_text, "This database cannot be chosen: its folder name cannot be stored.");
        fprintf(stderr, "cm0102_recomp: database = %s was not stored as it is\n", value);
        return 0;
    }
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
    if (w >= 0 && w < MAX_WIDGETS) {
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
            handled_press = win_press_count();  /* one press opens one page: a held button repeats */
            if (!getenv("RECOMP_SCRIPT") && SDL_OpenURL(LINK_URL))
                fprintf(stderr, "cm0102_recomp: the page %s could not be opened: %s\n", LINK_URL, SDL_GetError());
            r = 0;
        } else if (id == BACK_ID) {
            r = GO_BACK;
        } else if (id == CHOOSE_ID) {
            int sel = (int)node_slot(0) - 1;
            Version v[SEASONS_EXTRA_MAX + 1];
            int nv = sel >= 0 && sel < SEASONS_COUNT ? versions_of(sel, v, SEASONS_EXTRA_MAX + 1) : 0;
            int unpack = nv && list.season[sel].status == SEASONS_IN_DOWNLOAD && v[chosen_version(sel, nv)].own;
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
