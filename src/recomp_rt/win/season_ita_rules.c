/* The Italian rules of the league sizes that depend only on numbers (season_ita_rules.h). */
#include "season_ita_rules.h"

int ita_serie_b_zone(int year, int clubs, int up, int down, int row)
{
    if (year > ITA_SPAREGGIO_LAST || row < 0 || row >= clubs) return 0;
    if (row == 0) return 1;
    if (row < up) return 2;
    if (down > 0 && row >= clubs - down) return 3;
    return 0;
}

int ita_cup_pot(const ItaCupClub *c, int b_count)
{
    int last_b_low = b_count > 2 ? b_count - 2 : 18;
    if (!c->has_last) return 1;                                                 /* 0x6405ab */
    if (c->last_is_b && c->place >= 3 && c->place <= 6) return 4;               /* 0x6405af..0x6405be */
    if (c->last_is_a && c->cur_is_b) return 4;                                  /* 0x6405c0..0x6405cd */
    if (c->last_is_b && c->place >= 7 && c->place <= 14) return 3;              /* 0x6405d7..0x6405e6 */
    if (c->last_is_b && c->place >= 15 && c->place <= last_b_low) return 2;     /* 0x6405f2..0x640601 */
    if (!c->cur_is_b || c->last_is_cur || c->last_is_a) return 1;               /* 0x640603..0x640614 */
    return 2;
}

int ita_cup_b_count(int scanned, int season, int start)
{
    if (scanned <= 0) return 0;
    if (season <= start && scanned < 20) return 0;
    return scanned;
}

int ita_c1_playout_row(int clubs, int row18)
{
    int row = row18 + clubs - 18;
    if (clubs < 6 || row < 0 || row >= clubs) return row18;
    return row;
}
