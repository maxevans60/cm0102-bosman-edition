/* International caps above 255 in the game (0.6.2 slice 12, caps_wide.h, tools/recomp/caps_wide.py). */
#include "win.h"
#include "caps_wide.h"
#include "../rt.h"

#include <stdio.h>

#define STAFF_ID 0x00u          /* staff +0: the staff id */
#define STAFF_CAPS 0x22u        /* staff +0x22: the caps byte */

/* 0x76b8ce inc al, with the caps byte of the staff in esi in al (0x76b8c8): the new byte. */
uint32_t rt_caps_add(uint32_t staff, uint32_t byte)
{
    uint32_t id = rd32(g_mem, staff + STAFF_ID), next = caps_wide_add(id, byte);
    if ((byte & 0xffu) == CAPS_BYTE_MAX)
        fprintf(stderr, "recomp: caps: staff %u has %u caps\n", id, caps_wide_full(id, CAPS_BYTE_MAX));
    return next;
}

/* The full caps of the staff record at staff. */
uint32_t rt_caps_full(uint32_t staff)
{
    return caps_wide_full(rd32(g_mem, staff + STAFF_ID), rd8(g_mem, staff + STAFF_CAPS));
}
