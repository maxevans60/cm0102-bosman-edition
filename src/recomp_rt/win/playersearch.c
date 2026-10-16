/* The option full_player_search (0.5.0, #57).
 *
 * The player search list builder 0x7afa70 adds a player only when the human manager knows him: the score of
 * 0x5407e0 is 4000 or more (cmp ax, 0xfa0 at 0x7afc47), or a scout report exists, or he is from the same
 * region (docs/research/2026-10-16-crash-fixes.md, 1c). The jge at 0x7afc4b is the test of the score.
 * tools/recomp/widescreen.py replaces it: with the option on, every player passes. With the option off, the original's path runs. */
#include "options.h"
#include "../rt.h"

uint32_t rt_full_player_search(void)
{
    return options_bool(OPT_FULL_PLAYER_SEARCH) ? 1u : 0u;
}
