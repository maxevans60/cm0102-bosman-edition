/* The state of a Brazilian club (0.6.0 slice 20e, Q600): season_bra_state_from with a city suffix, a city of the
 * table, a club of the table and no state. Names in code page 1252, as the game holds them. */
#include "season_bra_states.h"

#include <stdio.h>
#include <string.h>

static int fails;

static void expect(const char *club, const char *city, const char *want)
{
    char got[3] = "";
    int ok = season_bra_state_from(club, city, got);
    if ((want && (!ok || strcmp(got, want))) || (!want && ok)) {
        printf("FAIL  club '%s' city '%s': got %s, want %s\n", club, city, ok ? got : "none", want ? want : "none");
        fails++;
    }
}

int main(void)
{
    expect("Ceara", "Fortaleza (CE)", "CE");                    /* the suffix */
    expect("Paysandu", "Bel\351m (PA)", "PA");                   /* the suffix of a name with an accent */
    expect("Any Club", "Fortaleza", "CE");                       /* the city of the table */
    expect("Any Club", "Macei\363", "AL");
    expect("Any Club", "Natal  ", "RN");                         /* trailing spaces */
    expect("Any Club", "Fortaleza (XX)", NULL);                  /* not a state code: no state */
    expect("Any Club", "Nowhere", NULL);
    expect("", "", NULL);
    expect("Sociedade Esportiva do Gama (DF)", "Nowhere", "DF");   /* the club's suffix (databases from 2020) */
    expect("Some Club (XX)", "Nowhere", NULL);                     /* not a state code */
    expect("Some Club (DF)", "Fortaleza (CE)", "CE");             /* the city's code first */
    /* A club of the table with a city that the table lacks: the first club row of the table. */
    for (int i = 0; i < season_bra_name_count; i++)
        if (season_bra_names[i].club) {
            expect(season_bra_names[i].name, "Nowhere", season_bra_names[i].state);
            break;
        }
    if (season_bra_name_count < 300) printf("FAIL  the table holds %d names\n", season_bra_name_count), fails++;
    printf(fails ? "FAILED: %d\n" : "PASS\n", fails);
    return fails ? 1 : 0;
}
