/* The state of a Brazilian club from its name and its city (0.6.0 slice 20e, Q600). No game memory: the runtime
 * (season_bra_states.c) and the ctest call it. */
#include "season_bra_states.h"

#include <string.h>

static const char *const STATES[] = {"AC", "AL", "AM", "AP", "BA", "CE", "DF", "ES", "GO", "MA", "MG", "MS", "MT", "PA",
                                     "PB", "PE", "PI", "PR", "RJ", "RN", "RO", "RR", "RS", "SC", "SE", "SP", "TO"};

static int is_state(const char *s)
{
    for (unsigned i = 0; i < sizeof STATES / sizeof STATES[0]; i++)
        if (s[0] == STATES[i][0] && s[1] == STATES[i][1]) return 1;
    return 0;
}

/* The length of a name without its trailing spaces. */
static size_t trimmed(const char *s)
{
    size_t n = strlen(s);
    while (n && s[n - 1] == ' ') n--;
    return n;
}

static const char *in_table(int club, const char *name, size_t n)
{
    for (int i = 0; n && i < season_bra_name_count; i++) {
        const SeasonBraName *e = &season_bra_names[i];
        if (e->club == club && strlen(e->name) == n && !memcmp(e->name, name, n)) return e->state;
    }
    return NULL;
}

int season_bra_state_from(const char *club, const char *city, char out[3])
{
    const char *state = NULL;
    size_t n = city ? trimmed(city) : 0;
    /* "Fortaleza (CE)": the suffix, else the name before it. */
    if (n >= 4 && city[n - 1] == ')' && city[n - 4] == '(') {
        if (!is_state(city + n - 3)) return 0;
        out[0] = city[n - 3], out[1] = city[n - 2], out[2] = 0;
        return 1;
    }
    /* From November 2020 the databases write the code at the end of the club's name: "Sociedade Esportiva do Gama
     * (DF)" (review of 2026-10-20). */
    size_t m = club ? trimmed(club) : 0;
    if (m >= 4 && club[m - 1] == ')' && club[m - 4] == '(' && is_state(club + m - 3)) {
        out[0] = club[m - 3], out[1] = club[m - 2], out[2] = 0;
        return 1;
    }
    if (city) state = in_table(0, city, n);
    if (!state && club) state = in_table(1, club, m);
    if (!state) return 0;
    out[0] = state[0], out[1] = state[1], out[2] = 0;
    return 1;
}
