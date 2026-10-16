/* The Bosman Edition Options box (spec 2026-10-16-features-0.5.0, slice 6; #56).
 *
 * A box the size of the original's Game Settings box (build 0x823950), with every option of the settings
 * file (options.h) on two tabs, Screen and Game, nine rows each. Labels and notes come from the mockup
 * docs/research/mockups/2026-10-16-options-screen.html, variant B.
 *
 * The box is a screen of the runtime on the game's own screen stack, as Team views (views.c). The push of
 * the original 0x7fe600 gets flag 1, as Game Settings does (0x8238bb): the game draws it as a box over the
 * screen below (docs/research/2026-10-16-options-screen.md, 1a). Its build and input functions are host
 * functions (rt_host_fn). The build copies the frame of 0x823950: a dark area over the whole screen
 * (0x823a92), the green box (145, 50)-(744, 549) (0x823af4), the heading (0x803fd0 at 0x823b41) and the
 * version line (0x823bb9). A row uses the original's choice row style: the label in font 3 (0x8247d9),
 * the current value as a yellow framed label, style 0x221 (0x804100 at 0x82477b), and each other value
 * as a green button (0x8040a0 at 0x8247a4). One Close button closes the box.
 *
 * Two places open it: a new item "Bosman Edition Options" after "Game Settings" in the side bar drop-down
 * "Game Options" (item create 0x760602, options_screen_widget_before/after), and an item of the start
 * screen menu (database.c).
 *
 * A click on a value calls options_set at once: the running game has the new value, and settings.txt
 * gets it through its temporary file. Rows whose value the game reads only at start say so in a note.
 * When each value takes effect (docs/research/2026-10-16-options-screen.md, 3b): display, sound and
 * save_folder at the next start; database at the next new game; match_columns and attribute_groups when
 * their screen is built next; full_player_search on the next game day (the search keeps its list for the
 * day, 0x7af160); sharp_picture at once (win_sharp_picture_changed); every other switch at the next key,
 * draw or save. save_folder is read only: the box has no text entry.
 *
 * Same game (Q152): the box allocates its scratch block at its first open, a click of the player. The text
 * of the side bar item is allocated at the first side bar build of every game. Switching an option in the
 * box leaves the game as clicking its tabs as often does (smoke_run.py --same-save --options).
 *
 * Node slot 0: the tab, 1 Screen or 2 Game. Only a number, so the box needs no leave function. */
#include "win.h"
#include "options.h"
#include "dbfolders.h"
#include "../host.h"

#include <stdio.h>
#include <string.h>

#ifndef CM0102_VERSION
#define CM0102_VERSION "dev"
#endif

extern const uint32_t rt_site_options_item;

#define SCREENS 0xb74450u           /* the screen stack S */
#define PUSH 0x7fe600u              /* thiscall on S (build, input, flag, leave, exit), ret 0x14 */
#define SET_SLOT 0x7ff270u          /* thiscall on S (slot, value, copy) */
#define NODE_SLOT 0x7ff010u         /* thiscall on S (slot) */
#define BACKGROUND_OFF 0x41c0d0u    /* cdecl (0): as the Game Settings build (0x823a57) */
#define COLOUR 0x5e4800u            /* cdecl (0, green, 0, 0): an RGB565 colour (0x823ac0) */
#define BOX_TEXT 0x803fd0u          /* thiscall on S, 11 args: a text widget with its own rect */
#define AREA_CREATE 0x55fba0u       /* thiscall on a Display, 11 args */
#define WIDGET_CREATE 0x55f990u     /* thiscall on a Display, 18 args */
#define CELL_BUTTON 0x8040a0u       /* thiscall on S, 11 args: a button in a cell of an area */
#define CELL_LABEL 0x804100u        /* thiscall on S, 9 args: a label in a cell of an area */

#define DISPLAYS 0xb74c4cu          /* the Display of a player slot, 0x300 bytes apart */
#define PLAYER_SLOT 0xb77c7au       /* s16: the player slot that has the turn */
#define WIDGETS 0xba95eu            /* Display + this: widget 0; 0x18c bytes each */
#define WIDGET_SIZE 0x18cu

#define TEXT_YELLOW 0xaebdecu       /* u16 colours of the Game Settings box */
#define TEXT_TITLE 0xaebdeau
#define TEXT_LIGHT 0xae3154u
#define BUTTON_COLOUR 0xae317eu
#define BUTTON_TEXT 0xae3184u

#define VALUE_ID 0x7d3au            /* data: row index * 16 + value index. Ids that the original does not use. */
#define TAB_ID 0x7d3bu              /* data: the tab, 1 or 2 */
#define CLOSE_ID 0x7d3cu
#define ITEM_ID 0x4b8u              /* the side bar item; the side bar input 0x7615d0 uses no such id */
#define REBUILD 0xfffffff5u         /* -0xb: build the box again */
#define CLOSE 0xfffffff7u           /* -9: close the box, as Ok and Cancel of Game Settings (0x8259a0) */

#define ROWS 9
#define TEXT_SIZE 0x200u

typedef enum { ROW_SWITCH, ROW_DISPLAY, ROW_DATABASE, ROW_PATH } RowKind;

typedef struct {
    int tab;                        /* 1 Screen, 2 Game */
    OptionId id;
    RowKind kind;
    const char *label;
    const char *note;
} Row;

static const Row rows[] = {
    {1, OPT_DISPLAY, ROW_DISPLAY, "Display mode", "Applies at the next start"},
    {1, OPT_SHARP_PICTURE, ROW_SWITCH, "Sharp picture", ""},
    {1, OPT_REMEMBER_WINDOW, ROW_SWITCH, "Remember window", ""},
    {1, OPT_SOUND, ROW_SWITCH, "All sound", "Applies at the next start"},
    {1, OPT_ATTRIBUTE_COLOURS, ROW_SWITCH, "Attribute colours", ""},
    {1, OPT_CONDITION_COLOURS, ROW_SWITCH, "Condition colours", ""},
    {1, OPT_FOREIGN_COLOURS, ROW_SWITCH, "Foreign colours", ""},
    {1, OPT_MATCH_COLUMNS, ROW_SWITCH, "Match columns", "When the screen opens next"},
    {1, OPT_ATTRIBUTE_GROUPS, ROW_SWITCH, "Attribute groups", "When a profile opens next"},
    {2, OPT_SPACE_TO_CONTINUE, ROW_SWITCH, "Space to continue", ""},
    {2, OPT_SHORTCUTS, ROW_SWITCH, "Shortcut keys", ""},
    {2, OPT_CLICKABLE_NAMES, ROW_SWITCH, "Clickable names", ""},
    {2, OPT_SQUAD_VIEWS, ROW_SWITCH, "Team views", ""},
    {2, OPT_FULL_PLAYER_SEARCH, ROW_SWITCH, "Full player search", "From the next game day"},
    {2, OPT_FASTER_DAYS, ROW_SWITCH, "Faster holidays", ""},
    {2, OPT_SAVE_BACKUPS, ROW_SWITCH, "Save backups", ""},
    {2, OPT_DATABASE, ROW_DATABASE, "Database", "For the next new game"},
    {2, OPT_SAVE_FOLDER, ROW_PATH, "Save folder", "Applies at the next start\nChange it in settings.txt"},
};
#define ROW_COUNT ((int)(sizeof rows / sizeof rows[0]))

static const char *const switch_text[] = {"On", "Off"}, *const switch_word[] = {"on", "off"};
static const char *const display_text[] = {"Widescreen", "Classic", "HD"};
static const char *const display_word[] = {"widescreen", "classic", "hd"};
static const char *const database_word[] = {"3.9.68", "3.9.60", "3.9.65"};

static uint32_t scratch;            /* game memory for texts and column weights; the creates copy texts */

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

static uint32_t text(const char *s)
{
    win_put_str(scratch, s, TEXT_SIZE);
    return scratch;
}

static uint32_t colour16(uint32_t at) { return rd16(g_mem, at); }

/* An area with columns of the given weights (bytes in game memory), one row. */
static int area(int x1, int y1, int x2, int y2, int columns, const uint8_t *weights, uint32_t flags,
                uint32_t colour)
{
    uint32_t w = 0;
    if (weights) {
        w = scratch + TEXT_SIZE;
        memcpy(g_mem + w, weights, (size_t)columns);
    }
    uint32_t a[11] = {(uint32_t)x1, (uint32_t)y1, (uint32_t)x2, (uint32_t)y2, (uint32_t)columns, w, 1, 0, flags,
                      colour, 0xffffffffu};
    return (int16_t)call_this(AREA_CREATE, display(), 11, a);
}

static void box_text(int x1, int y1, int x2, int y2, uint32_t style, uint32_t colour, uint32_t flags, uint32_t font,
                     uint32_t text_colour, const char *s)
{
    uint32_t a[11] = {(uint32_t)x1, (uint32_t)y1, (uint32_t)x2, (uint32_t)y2, style, colour, flags, font,
                      text_colour, text(s), 1};
    call_this(BOX_TEXT, SCREENS, 11, a);
}

/* The current value: a yellow label with a frame, as Game Settings (0x82477b). */
static void current(int a, int column, const char *s)
{
    uint32_t y = colour16(TEXT_YELLOW);
    uint32_t l[9] = {(uint32_t)a, (uint32_t)column, 0, 0x221, y, 0xc, 2, y, text(s)};
    call_this(CELL_LABEL, SCREENS, 9, l);
}

/* Another value: a green button, as Game Settings (0x8247a4). */
static void choice(int a, int column, const char *s, uint32_t id, uint32_t data, uint32_t green)
{
    uint32_t b[11] = {(uint32_t)a, (uint32_t)column, 0, 1, 0, 0xc, 2, green, text(s), id, data};
    call_this(CELL_BUTTON, SCREENS, 11, b);
}

/* A plain text in a cell: flags 1 left, font 1 small or 3 as the row labels (0x8247d9). */
static void plain(int a, int column, const char *s, uint32_t font, uint32_t colour)
{
    uint32_t l[9] = {(uint32_t)a, (uint32_t)column, 0, 1, 0, 1, font, colour, text(s)};
    call_this(CELL_LABEL, SCREENS, 9, l);
}

/* The folder of the saved games, with the home folder as "~", cut at the front to room - 1 characters. */
static void folder_text(char *out, size_t room)
{
    char path[1100], home[1024];
    const char *dir = win_save_folder();
    size_t hl = host_home_dir(home, sizeof home) ? strlen(home) : 0;
    if (hl && !strncmp(dir, home, hl) && (dir[hl] == '/' || dir[hl] == '\\' || !dir[hl]))
        snprintf(path, sizeof path, "~%s", dir + hl);
    else
        snprintf(path, sizeof path, "%s", dir);
    char label[1100];
    dbfolders_label(path, label, sizeof label);
    size_t n = strlen(label);
    if (n < room) {
        snprintf(out, room, "%s", label);
        return;
    }
    snprintf(out, room, "...%s", label + n - (room - 4));
}

/* The index of the current value of a row: 0 or 1 for a switch, 0 to 2 for a choice, 3 for a custom
 * data set (folder:<name>). */
static int current_index(const Row *r)
{
    if (r->kind == ROW_SWITCH) return options_bool(r->id) ? 0 : 1;
    if (r->kind == ROW_DISPLAY) return options_enum(r->id);
    if (r->kind == ROW_DATABASE) {
        int e = options_enum(r->id);
        if (e == DATABASE_CUSTOM) return 3;
        for (int i = 0; i < 3; i++)
            if (!strcmp(options_string(r->id), database_word[i])) return i;
    }
    return 0;
}

/* Column weights: the label, three values and the note, as wide as the original's columns allow (Game
 * Settings: 3, 1, 1, 1, 1 at 0x8239ad). A switch row has two values, so its note starts in the third value
 * column. */
static const uint8_t weights_three[] = {36, 17, 17, 17, 33};
static const uint8_t weights_two[] = {36, 17, 17, 50};
static const uint8_t weights_custom[] = {36, 17, 17, 17, 17, 16};    /* database with a custom data set */
static const uint8_t weights_path[] = {36, 51, 33};
#define ROW_X1 160
#define ROW_X2 729

/* The left edge of the last column of a row area. */
static int last_column_x(const uint8_t *weights, int columns)
{
    int total = 0, before = 0;
    for (int i = 0; i < columns; i++) total += weights[i];
    for (int i = 0; i < columns - 1; i++) before += weights[i];
    return ROW_X1 + (ROW_X2 - ROW_X1 + 1) * before / total;
}

/* The note of a row in the last column, in the small font. A "\n" splits it into two lines, for the narrow
 * note columns of classic mode: a label shows one line only. */
static void note(const char *s, const uint8_t *weights, int columns, int y1, int y2, uint32_t colour)
{
    if (!s[0]) return;
    int x = last_column_x(weights, columns) + 4;
    const char *nl = strchr(s, '\n');
    if (!nl) {
        box_text(x, y1, ROW_X2, y2, 1, 0, 1, 1, colour, s);
        return;
    }
    char first[96];
    snprintf(first, sizeof first, "%.*s", (int)(nl - s), s);
    int mid = (y1 + y2) / 2;
    box_text(x, y1 + 1, ROW_X2, mid, 1, 0, 1, 1, colour, first);
    box_text(x, mid + 1, ROW_X2, y2 - 1, 1, 0, 1, 1, colour, nl + 1);
}

static void build_row(const Row *r, int index, int y1, int y2, uint32_t green, uint32_t light)
{
    if (r->kind == ROW_PATH) {
        int a = area(ROW_X1, y1, ROW_X2, y2, 3, weights_path, 1, 0);
        plain(a, 0, r->label, 3, light);
        /* A label with a text wider than its cell shows nothing, so the path is cut to what fits. */
        char path[128];
        size_t room = rt_screen_w >= 1920 ? 88 : rt_screen_w >= 1280 ? 58 : 36;
        folder_text(path, room);
        plain(a, 1, path, 2, colour16(TEXT_YELLOW));
        note(r->note, weights_path, 3, y1, y2, light);
        return;
    }
    int cur = current_index(r);
    int custom = r->kind == ROW_DATABASE && cur == 3;
    int count = r->kind == ROW_SWITCH ? 2 : 3;
    int columns = custom ? 6 : count + 2;
    const uint8_t *weights = custom ? weights_custom : count == 2 ? weights_two : weights_three;
    int a = area(ROW_X1, y1, ROW_X2, y2, columns, weights, 1, 0);
    plain(a, 0, r->label, 3, light);
    int col = 1;
    if (custom) {
        char name[64];
        const char *v = options_string(r->id) + 7;      /* after "folder:" */
        dbfolders_label(v, name, rt_screen_w >= 1280 ? 20 : 12);
        current(a, col++, name);
    }
    for (int i = 0; i < count; i++, col++) {
        const char *s = r->kind == ROW_SWITCH ? switch_text[i] : r->kind == ROW_DISPLAY ? display_text[i]
                                                                                       : database_word[i];
        if (i == cur) current(a, col, s);
        else choice(a, col, s, VALUE_ID, (uint32_t)index * 16u + (uint32_t)i, green);
    }
    note(custom ? "For the next\nnew game" : r->note, weights, columns, y1, y2, light);
}

/* The build of the box: cdecl, no arguments. The frame follows 0x823950. */
static void options_build(void)
{
    if (!scratch) scratch = rt_alloc_zero(TEXT_SIZE + 16);
    int tab = (int)node_slot(0);
    if (tab != 1 && tab != 2) {
        tab = 1;
        set_slot(0, 1);
    }
    area(0, 0, 0x31f, 0x257, 1, NULL, 2, 0);                    /* the dark cover, 0x823a92 */
    uint32_t off[1] = {0};
    call(BACKGROUND_OFF, 1, off);
    uint32_t c[4] = {0, 0x38, 0, 0};
    uint32_t box_colour = call(COLOUR, 4, c) & 0xffff;
    uint32_t g[4] = {0, 0xc0, 0, 0};
    uint32_t green = call(COLOUR, 4, g) & 0xffff;              /* the value buttons, 0x823979 */
    uint32_t light = colour16(TEXT_LIGHT);
    area(145, 50, 744, 549, 1, NULL, 0x30, box_colour);
    box_text(155, 60, 734, 105, 0x30, colour16(TEXT_TITLE), 0xc, 4, colour16(BUTTON_TEXT), "Bosman Edition Options");
    box_text(165, 115, 420, 135, 1, 0, 1, 1, light, "Bosman Edition " CM0102_VERSION);
    box_text(490, 115, 724, 135, 1, 0, 1, 1, light, "Changes save at once to settings.txt");

    /* The tabs: the open tab as a framed label, the other as a button. */
    int t = area(165, 142, 724, 168, 2, NULL, 1, 0);
    static const char *const tab_names[2] = {"Screen", "Game"};
    for (int i = 1; i <= 2; i++) {
        if (i == tab) current(t, i - 1, tab_names[i - 1]);
        else choice(t, i - 1, tab_names[i - 1], TAB_ID, (uint32_t)i, green);
    }

    int y = 176;
    for (int i = 0; i < ROW_COUNT; i++) {
        if (rows[i].tab != tab) continue;
        build_row(&rows[i], i, y, y + 32, green, light);
        y += 35;
    }

    int close = area(155, 509, 734, 539, 1, NULL, 1, 0);
    uint32_t b[11] = {(uint32_t)close, 0, 0, 0x30, colour16(BUTTON_COLOUR), 0xc, 3, colour16(BUTTON_TEXT),
                      text("Close"), CLOSE_ID, 0};
    call_this(CELL_BUTTON, SCREENS, 11, b);
    fprintf(stderr, "recomp: options box built, tab %d\n", tab);
    rt_ret_cdecl(0);
}

/* Sets the value of a row from a click, and applies what can apply at once. */
static void apply(const Row *r, int value)
{
    const char *word = NULL;
    if (r->kind == ROW_SWITCH && value < 2) word = switch_word[value];
    if (r->kind == ROW_DISPLAY && value < 3) word = display_word[value];
    if (r->kind == ROW_DATABASE && value < 3) word = database_word[value];
    if (!word) return;
    int ok = options_set(r->id, word);
    fprintf(stderr, "recomp: option %s = %s (%d)\n", options_name(r->id), word, ok);
    if (r->id == OPT_SHARP_PICTURE) win_sharp_picture_changed();
    if (r->id == OPT_DATABASE) win_data_set_changed();
}

/* The input of the box: cdecl (s16 widget). Only the box's own widgets: the box covers the screen. */
static void options_input(void)
{
    int16_t w = (int16_t)rt_arg(0);
    uint32_t r = 0;
    if (w >= 0) {
        uint32_t at = display() + WIDGETS + 8 + (uint32_t)w * WIDGET_SIZE;
        uint32_t id = (uint16_t)rd32(g_mem, at), data = rd32(g_mem, at + 0x40);
        if (id == CLOSE_ID) {
            r = CLOSE;
        } else if (id == TAB_ID && (data == 1 || data == 2)) {
            set_slot(0, data);
            r = REBUILD;
        } else if (id == VALUE_ID && data / 16 < (uint32_t)ROW_COUNT) {
            apply(&rows[data / 16], (int)(data % 16));
            r = REBUILD;
        }
    }
    rt_ret_cdecl(r);
}

void options_screen_open(void)
{
    uint32_t p[5] = {rt_host_fn("options_build", options_build), rt_host_fn("options_input", options_input), 1, 0, 0};
    call_this(PUSH, SCREENS, 5, p);
    set_slot(0, 1);
    fprintf(stderr, "recomp: options box opened\n");
}

/* The side bar item. */
static uint32_t item_args[18];
static uint32_t item_display;

int options_screen_widget_before(void)
{
    if (rd32(g_mem, cpu.esp) != rt_site_options_item) return 0;
    for (int i = 0; i < 18; i++) item_args[i] = rt_arg(i);
    item_display = cpu.ecx;
    return 1;
}

/* After Game Settings: the same item below it, with the new text and id. The text block is made at the
 * first side bar build of every game, so every run has it. */
void options_screen_widget_after(void)
{
    static uint32_t label;
    if (!label) label = rt_alloc_str("Bosman Edition Options");
    uint32_t args[18];
    memcpy(args, item_args, sizeof args);
    args[13] = label;
    args[15] = ITEM_ID;
    uint32_t eax = cpu.eax;
    call_this(WIDGET_CREATE, item_display, 18, args);
    cpu.eax = eax;
}

int options_screen_side_bar_click(int id)
{
    if (id != (int)ITEM_ID) return 0;
    options_screen_open();
    return 1;
}
