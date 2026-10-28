/* A probe for the joiner's match that does not start (0.7.0 slice 9, research note section 16).
 *
 * RECOMP_STACK_PROBE=1: each call of 0x803d80 (thiscall on the screen manager) prints the current channel
 * ([this + 0x382a]), the caller, the channel's screen list and the result. The list starts at
 * [this + ch*0x300 + 0x7de] and goes on through +0x1f4 of each node (0x803d9c..0x803dfe); for each node the probe
 * prints its build function (+0), +4, the close callback (+0xc) and the flag (+0x10). 0x803d80 returns early when
 * a node has the flag and no close callback (0x803dc7..0x803dd3). Without the variable the wrap only calls the
 * original. */
#include "win.h"

#include <stdio.h>
#include <stdlib.h>

void o_00803d80(void);

RT_WRAP(00803d80, screen_pop_probe)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_STACK_PROBE");
        on = v && *v == '1';
    }
    if (!on) {
        o_00803d80();
        return;
    }
    uint32_t self = cpu.ecx, caller = rd32(g_mem, cpu.esp);
    int ch = (int16_t)rd16(g_mem, self + 0x382a);
    uint32_t rec = self + (uint32_t)ch * 0x300u;
    fprintf(stderr, "recomp: probe 0x803d80 from %#x, channel %d, list:", caller, ch);
    uint32_t n = rd32(g_mem, rec + 0x7de);
    for (int k = 0; n && k < 16; k++, n = rd32(g_mem, n + 0x1f4))
        fprintf(stderr, " [%#x build %#x +4 %#x close %#x flag %#x]", n, rd32(g_mem, n), rd32(g_mem, n + 4),
                rd32(g_mem, n + 0xc), rd32(g_mem, n + 0x10));
    fprintf(stderr, "\n");
    o_00803d80();
    fprintf(stderr, "recomp: probe 0x803d80 channel %d gives %#x\n", ch, cpu.eax);
}
