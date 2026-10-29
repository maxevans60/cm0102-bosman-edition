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

void o_007fe600(void);

/* RECOMP_STACK_PROBE=1: each screen push (0x7fe600, thiscall on the screen manager: build function, input
 * function, ...) prints the channel it goes to ([this + 0x382a], 0x7fe60c), the build and input functions and the
 * caller. The joiner's screens are pushed on its channel, so the log shows which screen a joiner's click opens. */
RT_WRAP(007fe600, screen_push_probe)
{
    const char *v = getenv("RECOMP_STACK_PROBE");
    if (v && *v == '1')
        fprintf(stderr, "recomp: probe push channel %d build %#x input %#x from %#x\n",
                (int16_t)rd16(g_mem, cpu.ecx + 0x382a), rt_arg(0), rt_arg(1), rd32(g_mem, cpu.esp));
    o_007fe600();
}

/* RECOMP_STACK_PROBE=1: the steps from the joiner's Continue Game to its match (research note section 16, second
 * code reading). Each wrap prints its first stack argument, the result and the caller. 0x725ca0 also prints the
 * channel of the screen manager at 0xb74450 (a guess from docs/research/2026-10-15-holiday-next-match-inventory.md)
 * and the match day words [0xb77c84], [0xdd6be0], [0xdd6be4] that it reads. 0x4671f0 and 0x7615d0 have wraps of
 * their own (assistant.c, holiday_next.c); the caller of 0x725ca0 shows which of them ran. */
static int probe_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_STACK_PROBE");
        on = v && *v == '1';
    }
    return on;
}

/* Calls the original orig with the probe's lines around it. tools/recomp/recomp.py finds a wrap only by a line that
 * starts with RT_WRAP(<address>, so each wrap below is written out. */
static void probe_call(void (*orig)(void), const char *label)
{
    if (!probe_on()) {
        orig();
        return;
    }
    uint32_t a0 = rt_arg(0), caller = rd32(g_mem, cpu.esp);
    orig();
    fprintf(stderr, "recomp: probe %s arg %d gives %d from %#x\n", label, (int)a0, (int)cpu.eax, caller);
}

void o_00727c30(void);
void o_00725ca0(void);
void o_00725f70(void);

RT_WRAP(00727c30, probe_confirm_input) { probe_call(o_00727c30, "0x727c30 confirm input"); }

RT_WRAP(00725ca0, probe_match_input)
{
    if (probe_on())
        fprintf(stderr, "recomp: probe 0x725ca0 channel %d b77c84 %#x dd6be0 %#x dd6be4 %#x\n",
                (int16_t)rd16(g_mem, 0xb74450u + 0x382a), rd32(g_mem, 0xb77c84u), rd32(g_mem, 0xdd6be0u),
                rd32(g_mem, 0xdd6be4u));
    probe_call(o_00725ca0, "0x725ca0 match input");
}

RT_WRAP(00725f70, probe_confirm_open) { probe_call(o_00725f70, "0x725f70 confirm open"); }
