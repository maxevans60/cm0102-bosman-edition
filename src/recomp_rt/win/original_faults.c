/* Faults of the original that long games reach (0.6.2 slice 11, tools/recomp/original_faults.py). The fixes act in
 * every game, the base game too (Q23, Q24, Q27). The test switch RECOMP_ORIGINAL_FAULTS=keep turns them off. */
#include "win.h"
#include "../rt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUB_COUNT 0xae235cu     /* clubs, national teams and under-21 teams */
#define NATION_COUNT 0xae2350u

static int fixes_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *v = getenv("RECOMP_ORIGINAL_FAULTS");
        on = !(v && !strcmp(v, "keep"));
    }
    return on;
}

/* A name of the game for a log line, with each byte above 0x7e as '?': the test scripts read the log as UTF-8, and
 * the game's names are Latin-1 ("S\xe3o Tom\xe9"). */
static const char *log_name(uint32_t at)
{
    static char out[64];
    size_t i = 0;
    for (const char *p = win_str(at); *p && i < sizeof out - 1; p++)
        out[i++] = (unsigned char)*p > 0x7e ? '?' : *p;
    out[i] = 0;
    return out;
}

/* `game_run_to_start_date` 0x5cc850 (called from create_game at 0x829fd9) plays the days of a new game up to its
 * start date, across a change of month. The fix stays off there, so a new game starts as the original's does (about
 * 95 of 213 national teams of the 3.9.68 data have no manager then) and changes only at the first month change in
 * play (Q152: the same-save set ends before it). */
static int creating;

void o_005cc850(void);

RT_WRAP(005cc850, run_to_start_date)
{
    creating = 1;
    o_005cc850();
    creating = 0;
}

/* 1 when a vacancy opens for this club although `club_can_be_managed` 0x544820 refuses it: a senior national team,
 * by the club id range of 0x544835..0x544857, with a nation (+0x53, 0x54482e). The original asks for more than 150
 * players of the nation (0x544859), or 100 in `add_job_vacancy` (0x69bafb). */
uint32_t rt_national_vacancy(uint32_t club, uint32_t site)
{
    static const char *const why[] = {"the vacancy", "a retirement", "a move to another national team", "the month check",
                                       "a departure"};
    if (!fixes_on() || creating || !club) return 0;
    uint32_t nation = rd32(g_mem, club + 0x53u);
    if (!nation) return 0;
    int32_t id = (int32_t)rd32(g_mem, club), clubs = (int32_t)rd32(g_mem, CLUB_COUNT);
    int32_t nations = (int32_t)rd32(g_mem, NATION_COUNT);
    if (id < clubs - 2 * nations || id >= clubs - nations) return 0;
    if (site)    /* site 0, the bound in add_job_vacancy, always follows one of the others */
        fprintf(stderr, "recomp: national manager: a vacancy for %s after %s (%d players)\n", log_name(club + 4u),
                why[site < 5u ? site : 0u], (int32_t)rd32(g_mem, nation + 0x88u));
    return 1;
}

/* `database_get_staff_age` 0x53ab90 gives the age in ax; the original stores it as the sign extension of al. */
uint32_t rt_staff_age_store(uint32_t eax)
{
    int16_t age = (int16_t)(eax & 0xffffu);
    if (fixes_on() && age > 127) return 127u;
    return (uint32_t)(int32_t)(int8_t)(eax & 0xffu);
}



/* Money (Q24). `Finance::get_transfer_budget` 0x598800 works on a 64-bit budget, but cuts three values to 32 bits.
 * The first two (0x598f4f, 0x598f8b, 0x598f92) re-extend the low word with cdq: here the high word is used. */
uint32_t rt_budget_hi(uint32_t lo, uint32_t hi)
{
    return fixes_on() ? hi : (uint32_t)((int32_t)lo >> 31);
}

/* The third is the floor R = balance - 10m (or 15m) (0x599135, 0x599185), a 32-bit value in [esp+0x5c] that the
 * budget falls back to (0x5991d0..0x599201: above R only a tenth counts). Above 2,147,483,647 the store keeps
 * 2,147,483,647 and the full value waits here for the three reads of R. A new call resets it (the wrap below). */
static int64_t floor_wide;
static int floor_on, floor_used;

uint32_t rt_budget_floor(uint32_t finance, uint32_t less)
{
    int64_t r = (int64_t)rd64(g_mem, finance) - (int64_t)less;
    floor_on = fixes_on() && r > INT32_MAX;
    if (!floor_on) return rd32(g_mem, finance);
    floor_wide = r;
    return (uint32_t)INT32_MAX + less;      /* the sub that follows leaves 2,147,483,647 */
}

uint32_t rt_budget_floor_low(uint32_t r)
{
    floor_used = floor_on && r == (uint32_t)INT32_MAX;
    return floor_used ? (uint32_t)floor_wide : r;
}

uint32_t rt_budget_floor_high(uint32_t lo)
{
    return floor_used ? (uint32_t)((uint64_t)floor_wide >> 32) : (uint32_t)((int32_t)lo >> 31);
}

int64_t rt_budget_floor_wide(uint32_t r)
{
    return floor_on && r == (uint32_t)INT32_MAX ? floor_wide : (int64_t)(int32_t)r;
}

/* The 37 callers of 0x598800 (by the return address): these 14 keep the 64-bit result (the docs file), three of
 * them only to show it (0x463b20, 0x808ee9, 0x8ea2b4, with rt_cash_wide). The other 23 keep eax: the clamp gives
 * them 2,147,483,647 for a larger budget, where eax alone would be 0 or a wrong amount. */
static const uint32_t budget_wide_callers[] = {
    0x463afdu, 0x463b20u, 0x4d7541u, 0x6970a3u, 0x6a3b8cu, 0x7b6d5fu, 0x7b6e25u, 0x808ee9u, 0x8c0eb6u, 0x8e74a7u,
    0x8e83a1u, 0x8ea2b4u, 0x8f4b28u, 0x8f4bd5u,
};

void o_00598800(void);

RT_WRAP(00598800, get_transfer_budget)
{
    uint32_t from = rd32(g_mem, cpu.esp), finance = cpu.ecx;
    floor_on = floor_used = 0;
    o_00598800();
    if (rt_unwinding) return;
    int64_t v = (int64_t)(((uint64_t)cpu.edx << 32) | cpu.eax);
    int wide = 0;
    for (size_t i = 0; i < sizeof budget_wide_callers / sizeof budget_wide_callers[0]; i++)
        wide |= from == budget_wide_callers[i];
    if (fixes_on() && !wide) {
        if (v > INT32_MAX) cpu.eax = (uint32_t)INT32_MAX, cpu.edx = 0;
        else if (v < INT32_MIN) cpu.eax = 0x80000000u, cpu.edx = 0xffffffffu;
    }
    /* test switch (tests/seasons/original_faults_run.py money): the budgets of a club above 2,147,483,647 */
    if (getenv("RECOMP_MONEY_LOG") && (int64_t)rd64(g_mem, finance) > INT32_MAX) {
        uint32_t club = rd32(g_mem, 0xae23b4u) + rd32(g_mem, finance + 8u) * 0x245u;
        fprintf(stderr, "recomp: money budget: %s from 0x%x: %lld, the caller sees %lld\n", log_name(club + 4u), from,
                (long long)v, wide ? (long long)v : (long long)(int32_t)cpu.eax);
    }
}

/* The money text. A Cash holds 32 bits, and `Cash::get_display_amount` 0x43f460 starts with `fild dword [ecx]`;
 * after that the original works on 64 bits (the rounding step 0x4426e0 and the text `cash_format_money`
 * 0x441c10 take an int64). A screen that puts a 64-bit amount into a Cash registers it here; the next two reads of
 * that Cash (0x463994..0x4639e6 formats the balance twice) take the full amount while its low word is unchanged. */
static uint32_t wide_at, wide_reads;
static int64_t wide_amount;

void rt_cash_wide(uint32_t cash, uint32_t lo, uint32_t hi)
{
    int64_t v = (int64_t)(((uint64_t)hi << 32) | lo);
    wide_reads = fixes_on() && v != (int64_t)(int32_t)lo ? 2u : 0u;
    wide_at = cash;
    wide_amount = v;
}

int64_t rt_cash_amount(uint32_t cash)
{
    uint32_t lo = rd32(g_mem, cash);
    if (wide_reads && cash == wide_at && lo == (uint32_t)wide_amount) {
        wide_reads--;
        if (getenv("RECOMP_MONEY_LOG"))     /* test switch: tests/seasons/original_faults_run.py money */
            fprintf(stderr, "recomp: money text: %lld\n", (long long)wide_amount);
        return wide_amount;
    }
    return (int64_t)(int32_t)lo;
}

/* `Finance::get_max_wage` 0x5968d0: the wage term plus ftol(1.3 x balance) (0x5968db..0x5968ee keeps eax), in 32
 * bits at 0x597019. Here the high word of that product comes back, and the sum is clamped to the 32-bit range. */
uint32_t rt_max_wage_sum(uint32_t term, uint32_t lo, uint32_t finance)
{
    if (!fixes_on()) return term + lo;
    int64_t product = (int64_t)((double)(int64_t)rd64(g_mem, finance) * rdf64(g_mem, 0x968de8u));
    int64_t sum = (int64_t)(int32_t)term + (int64_t)(((uint64_t)product & 0xffffffff00000000u) | lo);
    if (sum > INT32_MAX) return (uint32_t)INT32_MAX;
    if (sum < INT32_MIN) return 0x80000000u;
    return (uint32_t)sum;
}
