/* The option full_player_search (0.5.0, #57).
 *
 * The player search list builder 0x7afa70 adds a player only when the human manager knows him: the score of
 * 0x5407e0 is 4000 or more (jge at 0x7afc4b), or a scout report exists, or he is from the same region
 * and one of 0x544520, 0x544590 and 0x76d9b0 is true (docs/research/2026-10-16-crash-fixes.md, 1c).
 * Changing the jge to a jmp lists every player. Here every original call still runs, so that the
 * game state is the same with the option on and off. tools/recomp/widescreen.py makes the three places that
 * reject a player ask rt_full_player_search instead. With the option off, the original's path runs. */
#include "options.h"
#include "../rt.h"

#define LIST_COUNT 0x1f6        /* [search screen + 0x1f6]: the players in the list */
#define LIST_MAX 32767          /* the screen keeps its count in 16 bits (+0x361e, inc word at 0x68d191) */

/* 1 when a player that the original rejects joins the list: the option is on and the list has room. */
uint32_t rt_full_player_search(uint32_t screen)
{
    if (!options_bool(OPT_FULL_PLAYER_SEARCH)) return 0u;
    return rd32(g_mem, screen + LIST_COUNT) < LIST_MAX ? 1u : 0u;
}
