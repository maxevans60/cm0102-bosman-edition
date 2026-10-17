/* The season lines of the Setup Game screen (#39, then 0.6.0 slice 13).
 *
 * The Setup Game screen (build 0x81bab0, input 0x81bd70) fills 9 of the 25 menu items on its stack and calls
 * the menu builder 0x5ed300 (0x81bcc5) with (110, 145, 780, 535, 2 columns, 6 rows, font 3, count 9, items,
 * 0, 0). The runtime fills items 9 to 11 and passes count 12, so the menu is a full grid of 2 x 6:
 * - item 9, "Choose Season" with the season and the database of the next new game: a click opens the
 *   Choose Season screen (season_screen.c). It takes the cell of the Database button of 0.4.0 to 0.5.0: the
 *   tile chooses the database now (Q236), so Setup Game has no Database button any more;
 * - item 10, "Bosman Edition Options": a click opens the options box (options_screen.c, since 0.5.0);
 * - item 11, "Changing rules: On" or "Off": the changing rules switch of the next new game (Q232, Q259). A
 *   click turns it over for this run of the game; the option changing_rules keeps the default.
 * Item layout: +0x00 s16 font (-1 = default), +0x02 s16 text colour (0 = default), +0x04 text,
 * +0xcc id, +0xd0 data (docs/research/2026-10-13-screen-stack-inventory.md section 6a).
 *
 * At each build the season of the next new game comes again from the options database and season
 * (season_screen_year), so an old settings file with database = 3.9.60, 3.9.65 or folder:<name> starts that
 * data set, in the season of that data. The test variable RECOMP_START_SEASON (season.c) wins over them.
 *
 * The lines change no game state: a saved game loads with its own season and data set. */
#include "win.h"
#include "options.h"

#include "dbfolders.h"
#include "season.h"
#include "season_screen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const uint32_t rt_site_setup_menu;

#define WIDGETS 0xb77c94u + 0xba95eu   /* the widgets of screen manager 0 (names.c) */
#define WIDGET_SIZE 0x18cu
#define MAX_WIDGETS 1200
#define ITEM_SIZE 0xd4u
#define ITEM_TEXT_SIZE 0xc8u
#define SEASON_ID 0x7d39         /* the action id of the item; the original uses no such id */
#define OPTIONS_ID 0x7d3d        /* the item that opens the Bosman Edition Options box (options_screen.c) */
#define RULES_ID 0x7d3e          /* the changing rules switch */

static void fill_item(uint32_t item, const char *text, uint32_t id)
{
    wr16(g_mem, item, 0xffff);
    wr16(g_mem, item + 2, 0);
    win_put_str(item + 4, text, ITEM_TEXT_SIZE);
    wr32(g_mem, item + 0xcc, id);
    wr32(g_mem, item + 0xd0, 0);
}

void o_005ed300(void);
void o_0081bd70(void);

/* The menu builder, cdecl (x1, y1, x2, y2, columns, rows, font, count, items, highlight bits, sort). */
RT_WRAP(005ed300, menu_build)
{
    if (rd32(g_mem, cpu.esp) == rt_site_setup_menu && rt_arg(7) == 9) {
        if (season_code_on() && (!getenv("RECOMP_START_SEASON") || !*getenv("RECOMP_START_SEASON")))
            season_set_new_game(season_screen_year(), season_screen_rules_choice());
        int year = season_start_year();
        char db[DBFOLDER_NAME], text[ITEM_TEXT_SIZE];
        season_screen_database_text(db, dbfolders_label_room(rt_screen_w));
        snprintf(text, sizeof text, "Choose Season: %d/%02d\n%s", year, (year + 1) % 100, db);
        uint32_t item = rt_arg(8) + 9 * ITEM_SIZE;
        const char *old = getenv("RECOMP_SETUP_TEXT");
        if (old && !strcmp(old, "0.5.0")) {
            /* Test only (Q342): the Setup Game text of 0.5.0, the Database button and the options item, so a
             * run shows that only this text moves memory against the 0.5.0 build. */
            const char *d = options_string(OPT_DATABASE);
            snprintf(text, sizeof text, "Database: %s", !strcmp(d, "3.9.60") ? "2001/02 (3.9.60)"
                                                        : !strcmp(d, "3.9.65") ? "2001/02 Dec (3.9.65)"
                                                                               : "2002/03 (3.9.68)");
            fill_item(item, text, SEASON_ID);
            fill_item(item + ITEM_SIZE, "Bosman Edition Options", OPTIONS_ID);
            wr32(g_mem, cpu.esp + 4 + 4 * 7, 11);
            o_005ed300();
            return;
        }
        fill_item(item, text, SEASON_ID);
        fill_item(item + ITEM_SIZE, "Bosman Edition Options", OPTIONS_ID);
        fill_item(item + 2 * ITEM_SIZE, season_screen_rules_on() ? "Changing rules: On" : "Changing rules: Off",
                  RULES_ID);
        wr32(g_mem, cpu.esp + 4 + 4 * 7, 12);
    }
    o_005ed300();
}

/* The Setup Game input, cdecl (clicked widget or -1). */
RT_WRAP(0081bd70, setup_input)
{
    int clicked = (int16_t)rt_arg(0);
    int id = clicked >= 0 && clicked < MAX_WIDGETS ? (int16_t)rd16(g_mem, WIDGETS + WIDGET_SIZE * (uint32_t)clicked + 8)
                                                   : 0;
    if (id == OPTIONS_ID) {
        options_screen_open(1);
        rt_ret_cdecl((uint32_t)-4);
        return;
    }
    if (id == SEASON_ID) {
        if (season_code_on()) season_screen_open();     /* RECOMP_SEASON_CODE=off: the item does nothing */
        rt_ret_cdecl((uint32_t)-4);
        return;
    }
    if (id == RULES_ID) {
        season_screen_toggle_rules();
        season_set_new_game(season_start_year(), season_screen_rules_choice());
        fprintf(stderr, "recomp: changing rules of the next new game: %s\n", season_screen_rules_on() ? "on" : "off");
        rt_ret_cdecl((uint32_t)-4);
        return;
    }
    o_0081bd70();
}
