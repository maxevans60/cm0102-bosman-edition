/* Host copies of hot loops of the original that game creation and the game days spend their time in
 * (docs/research/2026-10-29-new-game-speed.md, 0.6.2 slice 14). Each copy gives the same memory, stack bytes and
 * registers as the recompiled code, so the game stays the same (rule Q152). RECOMP_HOT=original runs the
 * recompiled code; RECOMP_HOT=check runs both on each call and stops at the first difference. The
 * self-test (RECOMP_QSORT_TEST, crt.c) calls hot_loops_self_test.
 *
 * 0x7ae350, thiscall (int *id), ret 4: is the id in the list of this? The list holds [this+8] rows of
 * 0x1a bytes from [this+4], with the id at +0 of a row. With all nations, the second "Initialising game
 * data" of a 2025/26 start spent most of its 50 s in this loop. It pushes esi and edi, so it leaves them
 * below esp. al is 1 when found, else 0; the upper bytes of eax are those of the found row, of the row
 * after the last, or of the argument; edx is the index of the row, or the count, when the loop runs.
 *
 * 0x69fc60, thiscall (int *id), ret 4: is the id in the linked list at [this+8]? (0.6.2 slice 14.) The
 * shortlist search fill_shortlist_slot 0x83f3a0 calls it for each candidate (0x83fa8e), and with its calls
 * of the list step 0x68d1b0 it took 13 % of the samples of July to September 2025 in the profile of
 * 30 October 2026. The other callers are 0x4ceca2 and add_job_vacancy 0x69ba50 (0x69bd8f, 0x69bfb3,
 * 0x69c20c, 0x69c5c2); each tests al next, so none reads the flags. The list holds a
 * 16-bit count at +0, the first node at +2 and a cursor at +0xa; a node holds its data at +0 and the next
 * node at +6, and the id is at +6 of the data. The code (0x69fc60..0x69fd4a) saves the cursor in ebx, sets
 * it to the first node, steps it with 0x68d1b0 up to count times and puts the saved cursor back, so the
 * list ends as it was. It pushes ebx, ebp, esi and edi below 0x200 bytes of locals, and each step pushes
 * its return address 0x69fc8e. al is 1 when found, else 0; the upper bytes of eax are those of the list
 * when found or when the count is 0 or less, else those of the count (movsx); edx is the id of the last
 * data read. A node or data of 0 before the count ends gives the original's error box: the host copy finds
 * that before it writes, and then runs the recompiled code. */
#include "win.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_HAS 0x7ae350u
#define LLIST_HAS 0x69fc60u

void o_007ae350(void);
void o_0069fc60(void);

static int hot_mode = -1;          /* 0 host, 1 check, 2 original, 3 the self-test */

static void list_has_host(void)
{
    uint32_t esp = cpu.esp, arg = rd32(g_mem, esp + 4), eax = arg, edx = cpu.edx;
    wr32(g_mem, esp - 4, cpu.esi);
    wr32(g_mem, esp - 8, cpu.edi);
    int found = 0;
    uint32_t n;
    if (arg && (n = rd32(g_mem, cpu.ecx + 8)) != 0) {
        edx = 0;
        if ((int32_t)n > 0) {
            uint32_t id = rd32(g_mem, arg), row = rd32(g_mem, cpu.ecx + 4);
            for (;;) {
                if (rd32(g_mem, row) == id) { found = 1; break; }
                edx++;
                row += 0x1a;
                if ((int32_t)edx >= (int32_t)n) break;
            }
            eax = row;
        }
    }
    cpu.eax = (eax & 0xffffff00u) | (uint32_t)found;
    cpu.edx = edx;
    cpu.esp = esp + 8;
}

/* Both from the same state; the original's result stays. 0 when all is equal. */
static int list_has_check(void)
{
    uint32_t low = cpu.esp - 8;
    uint8_t before[12], host[12];
    memcpy(before, g_mem + low, 12);
    Cpu in = cpu;
    list_has_host();
    Cpu want = cpu;
    memcpy(host, g_mem + low, 12);
    memcpy(g_mem + low, before, 12);
    cpu = in;
    o_007ae350();
    int bad = want.eax != cpu.eax || want.edx != cpu.edx || want.ecx != cpu.ecx || want.esi != cpu.esi
              || want.edi != cpu.edi || want.esp != cpu.esp || memcmp(host, g_mem + low, 12);
    if (bad)
        fprintf(stderr, "recomp: hot check: 0x7ae350 differs (eax 0x%x host 0x%x, edx 0x%x host 0x%x)\n", cpu.eax,
                want.eax, cpu.edx, want.edx);
    return bad;
}

/* 0 when the walk would reach the error box, else 1 with the result in the registers and the stack. */
static int llist_has_host(void)
{
    uint32_t esp = cpu.esp, list = rd32(g_mem, cpu.ecx + 8), arg = rd32(g_mem, esp + 4);
    int32_t n = (int16_t)rd16(g_mem, list);
    uint32_t eax = list, edx = cpu.edx, node = rd32(g_mem, list + 2);
    int found = 0;
    if (n > 0) {
        for (int32_t i = 0; i < n; i++) {
            if (!node) return 0;
            uint32_t data = rd32(g_mem, node);
            if (!data) return 0;
            node = rd32(g_mem, node + 6);
            edx = rd32(g_mem, data + 6);
            if (edx == rd32(g_mem, arg)) { found = 1; break; }
        }
        if (!found) eax = (uint32_t)n;
        wr32(g_mem, esp - 0x214, 0x69fc8eu);
    }
    wr32(g_mem, esp - 0x204, cpu.ebx);
    wr32(g_mem, esp - 0x208, cpu.ebp);
    wr32(g_mem, esp - 0x20c, cpu.esi);
    wr32(g_mem, esp - 0x210, cpu.edi);
    cpu.eax = (eax & 0xffffff00u) | (uint32_t)found;
    cpu.ecx = list;
    cpu.edx = edx;
    cpu.esp = esp + 8;
    return 1;
}

static int llist_has_check(void)
{
    uint32_t low = cpu.esp - 0x214;
    uint8_t before[0x14], host[0x14];
    memcpy(before, g_mem + low, 0x14);
    Cpu in = cpu;
    if (!llist_has_host()) { o_0069fc60(); return 0; }
    Cpu want = cpu;
    memcpy(host, g_mem + low, 0x14);
    memcpy(g_mem + low, before, 0x14);
    cpu = in;
    o_0069fc60();
    int bad = want.eax != cpu.eax || want.ecx != cpu.ecx || want.edx != cpu.edx || want.ebx != cpu.ebx
              || want.ebp != cpu.ebp || want.esi != cpu.esi || want.edi != cpu.edi || want.esp != cpu.esp
              || memcmp(host, g_mem + low, 0x14);
    if (bad)
        fprintf(stderr, "recomp: hot check: 0x69fc60 differs (eax 0x%x host 0x%x, edx 0x%x host 0x%x)\n", cpu.eax,
                want.eax, cpu.edx, want.edx);
    return bad;
}

static unsigned long long hot_checked;
static int hot_test_fails;

static void print_hot_checked(void) { fprintf(stderr, "recomp: hot check: %llu calls equal\n", hot_checked); }

static void hot_mode_read(void)
{
    if (hot_mode < 0) {
        const char *v = getenv("RECOMP_HOT");
        hot_mode = !v ? 0 : !strcmp(v, "check") ? 1 : !strcmp(v, "original") ? 2 : 0;
    }
}

RT_WRAP(007ae350, list_has_id)
{
    hot_mode_read();
    if (hot_mode == 2) { o_007ae350(); return; }
    if (hot_mode == 1) {
        if (list_has_check()) abort();
        if (!hot_checked++) atexit(print_hot_checked);
        return;
    }
    if (hot_mode == 3) { hot_test_fails += list_has_check(); return; }
    list_has_host();
}

RT_WRAP(0069fc60, llist_has_id)
{
    hot_mode_read();
    if (hot_mode == 2) { o_0069fc60(); return; }
    if (hot_mode == 1) {
        if (llist_has_check()) abort();
        if (!hot_checked++) atexit(print_hot_checked);
        return;
    }
    if (hot_mode == 3) { hot_test_fails += llist_has_check(); return; }
    if (!llist_has_host()) o_0069fc60();
}

/* Lists of 0 to 40 rows with ids from a small range, found and not found, a null argument and a count
 * below 0. Gives the count of calls that differ. */
int hot_loops_self_test(void)
{
    uint32_t list = rt_alloc(16), rows = rt_alloc(41 * 0x1a), idp = rt_alloc(4), seed = 777;
    int calls = 0;
    hot_mode = 3;
    for (int n = -2; n <= 40; n++)
        for (int k = 0; k < 30; k++) {
            for (int i = 0; i < 41 * 0x1a; i++) {
                seed = seed * 1103515245u + 12345u;
                g_mem[rows + (uint32_t)i] = (uint8_t)(seed >> 16);
            }
            for (int i = 0; i < 41; i++) wr32(g_mem, rows + (uint32_t)i * 0x1a, (seed >> 8) % 7 + (uint32_t)i % 5);
            wr32(g_mem, list + 4, rows);
            wr32(g_mem, list + 8, (uint32_t)n);
            wr32(g_mem, idp, (uint32_t)k % 12);
            cpu.ecx = list;
            cpu.edx = 0x1234u + (uint32_t)k;
            cpu.eax = 0xabcdef01u;
            uint32_t args[1] = {k == 29 ? 0 : idp};
            rt_callback(LIST_HAS, 1, args);
            calls++;
        }
    /* Linked lists of 0 to 30 nodes, a count from -2 to the length (a count below the length stops early),
     * ids from a small range, found and not found. */
    uint32_t head = rt_alloc(16), nodes = rt_alloc(31 * 10), datas = rt_alloc(31 * 10), this_ = rt_alloc(12);
    wr32(g_mem, this_ + 8, head);
    for (int len = 0; len <= 30; len++)
        for (int n = -2; n <= len; n++)
            for (int k = 0; k < 8; k++) {
                for (int i = 0; i < len; i++) {
                    seed = seed * 1103515245u + 12345u;
                    uint32_t node = nodes + (uint32_t)i * 10, data = datas + (uint32_t)i * 10;
                    wr32(g_mem, node, data);
                    wr32(g_mem, node + 6, i + 1 < len ? node + 10 : 0);
                    wr32(g_mem, data + 6, (seed >> 16) % 9);
                }
                wr16(g_mem, head, (uint32_t)n);
                wr32(g_mem, head + 2, len ? nodes : 0);
                wr32(g_mem, head + 0xa, 0x5150u + (uint32_t)k);
                wr32(g_mem, idp, (uint32_t)k);
                cpu.ecx = this_;
                cpu.edx = 0x4321u + (uint32_t)k;
                cpu.eax = 0xfedcba98u;
                cpu.ebx = 0x1111u, cpu.ebp = 0x2222u, cpu.esi = 0x3333u, cpu.edi = 0x4444u;
                uint32_t args[1] = {idp};
                rt_callback(LLIST_HAS, 1, args);
                if (rd32(g_mem, head + 0xa) != 0x5150u + (uint32_t)k) hot_test_fails++;
                calls++;
            }
    hot_mode = -1;
    fprintf(stderr, "hot loops test: %d calls, %d differ\n", calls, hot_test_fails);
    return hot_test_fails;
}
