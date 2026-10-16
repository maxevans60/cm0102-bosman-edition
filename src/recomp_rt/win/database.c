/* The data set choice on the Setup Game screen (#39).
 *
 * The Setup Game screen (build 0x81bab0, input 0x81bd70) gets a tenth menu item, "Database". The build
 * fills 9 of the 25 menu items on its stack and calls the menu builder 0x5ed300 (0x81bcc5) with
 * (110, 145, 780, 535, 2 columns, 6 rows, font 3, count 9, items, 0, 0). The runtime fills item 9 and
 * passes count 10, so the button takes the free cell after "Web Sites" and the menu stays a full grid.
 * Since 0.5.0 it also fills item 10, "Bosman Edition Options", with count 11: a click opens the options box
 * (options_screen.c). The last cell stays free for the Choose Season button of 0.6.0.
 * Item layout: +0x00 s16 font (-1 = default), +0x02 s16 text colour (0 = default), +0x04 text,
 * +0xcc id, +0xd0 data (docs/research/2026-10-13-screen-stack-inventory.md section 6a).
 *
 * A click on it moves the option database to the next data set and writes it to the settings file. The
 * three data sets of the game folder come first, the default 3.9.68 at the start (Q174). After them come
 * the folders of Databases in the save folder (Q143, dbfolders.h), oldest season first, each shown by its
 * season and its folder name (Q174); the option then reads folder:<name>. The
 * input function then returns -4, so the screen builds again with the new text. The next Start New Game
 * loads the database from the chosen folder (files.c).
 *
 * The button only changes which files the next database load reads. It changes no game state, and a
 * saved game loads as before, because a save holds its own database. */
#include "win.h"
#include "options.h"

#include "dbfolders.h"

#include <stdio.h>
#include <string.h>

extern const uint32_t rt_site_setup_menu;

#define WIDGETS 0xb77c94u + 0xba95eu   /* the widgets of screen manager 0 (names.c) */
#define WIDGET_SIZE 0x18cu
#define MAX_WIDGETS 1200
#define ITEM_SIZE 0xd4u
#define ITEM_TEXT_SIZE 0xc8u
#define DATABASE_ID 0x7d39       /* the action id of the item; the original uses no such id */
#define OPTIONS_ID 0x7d3d        /* the item that opens the Bosman Edition Options box (options_screen.c) */

/* The choices: the three data sets of the game folder (3.9.68, 3.9.60, 3.9.65), then each usable folder of Databases (dbfolders.h),
 * listed again each time the screen builds. */
typedef struct { char value[DBFOLDER_NAME + 8]; char text[ITEM_TEXT_SIZE]; } Choice;
static Choice sets[3 + DBFOLDER_MAX];
static int set_count;

static void choice_set(Choice *c, const char *value, const char *text)
{
    snprintf(c->value, sizeof c->value, "%s", value);
    snprintf(c->text, sizeof c->text, "%s", text);
}

static void sets_fill(void)
{
    choice_set(&sets[0], "3.9.68", "Database: 2002/03 (3.9.68)");
    choice_set(&sets[1], "3.9.60", "Database: 2001/02 (3.9.60)");
    choice_set(&sets[2], "3.9.65", "Database: 2001/02 Dec (3.9.65)");
    set_count = 3;
    char names[DBFOLDER_MAX][DBFOLDER_NAME];
    int n = dbfolders_list(win_databases_root(), names, DBFOLDER_MAX);
    for (int i = 0; i < n; i++) {
        char value[DBFOLDER_NAME + 8], label[DBFOLDER_NAME];
        snprintf(value, sizeof value, "folder:%s", names[i]);
        dbfolders_button_text(names[i], label, dbfolders_label_room(rt_screen_w));
        char text[ITEM_TEXT_SIZE];
        snprintf(text, sizeof text, "Database: %s", label);
        choice_set(&sets[set_count++], value, text);
    }
}

/* The index of the chosen set. A custom folder that is gone counts as the default, 3.9.68, which is what
 * the game reads then (files.c). */
static int current_set(void)
{
    for (int i = 0; i < set_count; i++)
        if (!strcmp(options_string(OPT_DATABASE), sets[i].value)) return i;
    return 0;
}

/* The options box changed the data set in a game (options_screen.c). The open game keeps its data set
 * folder. The change reaches files.c at the next build of the start screen, before any new game. */
static int changed_in_game;
void database_changed_in_game(void) { changed_in_game = 1; }

void o_005ed300(void);
void o_0081bd70(void);

/* The menu builder, cdecl (x1, y1, x2, y2, columns, rows, font, count, items, highlight bits, sort). */
RT_WRAP(005ed300, menu_build)
{
    if (rd32(g_mem, cpu.esp) == rt_site_setup_menu && rt_arg(7) == 9) {
        if (changed_in_game) {
            changed_in_game = 0;
            win_data_set_changed();
        }
        sets_fill();
        uint32_t item = rt_arg(8) + 9 * ITEM_SIZE;
        wr16(g_mem, item, 0xffff);
        wr16(g_mem, item + 2, 0);
        win_put_str(item + 4, sets[current_set()].text, ITEM_TEXT_SIZE);
        wr32(g_mem, item + 0xcc, DATABASE_ID);
        wr32(g_mem, item + 0xd0, 0);
        /* Item 10: Bosman Edition Options (0.5.0 slice 6). One cell of the 2 x 6 grid stays free. */
        item += ITEM_SIZE;
        wr16(g_mem, item, 0xffff);
        wr16(g_mem, item + 2, 0);
        win_put_str(item + 4, "Bosman Edition Options", ITEM_TEXT_SIZE);
        wr32(g_mem, item + 0xcc, OPTIONS_ID);
        wr32(g_mem, item + 0xd0, 0);
        wr32(g_mem, cpu.esp + 4 + 4 * 7, 11);
    }
    o_005ed300();
}

/* The Setup Game input, cdecl (clicked widget or -1). */
RT_WRAP(0081bd70, setup_input)
{
    int clicked = (int16_t)rt_arg(0);
    if (clicked >= 0 && clicked < MAX_WIDGETS &&
        (int16_t)rd16(g_mem, WIDGETS + WIDGET_SIZE * (uint32_t)clicked + 8) == OPTIONS_ID) {
        options_screen_open(1);
        rt_ret_cdecl((uint32_t)-4);
        return;
    }
    if (clicked >= 0 && clicked < MAX_WIDGETS &&
        (int16_t)rd16(g_mem, WIDGETS + WIDGET_SIZE * (uint32_t)clicked + 8) == DATABASE_ID) {
        sets_fill();
        options_set_enum(OPT_DATABASE, sets[(current_set() + 1) % set_count].value);
        win_data_set_changed();
        rt_ret_cdecl((uint32_t)-4);
        return;
    }
    o_0081bd70();
}
