/* A club releases a staff member who is away on loan (O358, box contract_manager..cpp 5941).
 *
 * Each staff has a row of 79 bytes at [0xae2d0c] + id * 79: +0 the index of his main contract, +4 the index of his
 * loan contract, -1 for none. A contract is 80 bytes in the table at [CONTRACT_MANAGER] ([0xae1050]): +0x4 the club,
 * +0x2d and +0x2f the end day and year, +0x35 the type.
 *
 * The cause. Shortlist_Manager::assess_club_squad 0x83cb40 lists the club's staff who are away on loan: a loan
 * contract (get_loan_contract 0x4cf6d0, 0x83d41b) and staff +0x39 not the club (0x83d424), into [esp+0xe4]
 * (0x83d432). For a staff slot of the club it takes the first of them who is a coach or a player/coach (+0x3d 6 or
 * 13, 0x83edac); when his rating for the slot (0x69d1b0, 0x83ee23) is below the slot's need and he has no contract
 * offer from the club (find_contract_offer 0x4cc9e0, 0x83ee4b), it releases him: MANAGER_MANAGER::
 * remove_staff_from_club 0x690890 (staff, club) at 0x83ee72. That function removes his main contract (0x4d0b50,
 * called at 0x69089e) and clears his club slot only when +0x39 is the club (0x6908ac), so the loan contract stays.
 * On the loan's last day CONTRACT_MANAGER::daily_update 0x4c7ac0 calls end_loan 0x4d07e0 (0x4c8114), which needs the
 * main contract (0x4d0857..0x4d088b) and shows the box at 0x4d08e3. Seen in soak-2021 leg 1 (Select All, 2021):
 * Platense released the player/coach Alfredo Grelak, on loan at Brown de Adrogue to day 262 of 2022.
 *
 * The fix, in a season game (season_game_changes, Q152): the release at 0x83ee72 of a staff who is away on loan does
 * not happen; he stays under contract until his loan ends, and the club's daily assessment can release him then.
 * The slot fields that the original clears before the call (0x83ee62, 0x83ee68) stay cleared. The base game keeps
 * the original's call. Test switch RECOMP_TEST_LOAN_RELEASE=original: a season game takes the original's call too,
 * so that the net below acts.
 *
 * The net (all games; the test switch RECOMP_BOX_FIXES=off turns it off): end_loan of a staff with no main contract
 * ends the loan as a release would have: remove_staff_from_club (staff, loan club) takes him out of the loan club
 * (+0x39 to 0), then CONTRACT_MANAGER::remove_loan_contract 0x4d0bd0 frees the loan contract. Logged. */
#include "win.h"
#include "season_names.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STAFF_COUNT 0xae2364u
#define STAFF_ROWS 0xae2d0cu            /* 79-byte rows: +0 main contract, +4 loan contract */
#define MANAGER_MANAGER 0xb67a44u       /* ecx of remove_staff_from_club (0x83ee6c) */
#define CLUB_OF 0x39u
#define RELEASE_IN_ASSESS 0x0083ee77u   /* the return address of the call at 0x83ee72 */
#define REMOVE_STAFF 0x00690890u
#define REMOVE_LOAN 0x004d0bd0u

void o_00690890(void);
void o_004d07e0(void);

static int contract_index(uint32_t staff, uint32_t k)
{
    uint32_t id = rd32(g_mem, staff);
    if (id >= rd32(g_mem, STAFF_COUNT) || !rd32(g_mem, STAFF_ROWS)) return -1;
    return (int32_t)rd32(g_mem, rd32(g_mem, STAFF_ROWS) + id * 79u + 4u * k);
}

static const char *name_of(uint32_t staff, char *out, size_t size)
{
    uint32_t first = rd32(g_mem, staff + 0x4), second = rd32(g_mem, staff + 0x8);
    snprintf(out, size, "%s %s", first ? win_str(first) : "", second ? win_str(second) : "");
    return out;
}

static const char *club_name(uint32_t club)
{
    return club ? win_str(club + 4) : "no club";
}

static int test_original(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("RECOMP_TEST_LOAN_RELEASE"); v = e && !strcmp(e, "original"); }
    return v;
}

/* remove_staff_from_club 0x690890, thiscall (staff, club) ret 8. */
RT_WRAP(00690890, remove_staff_from_club)
{
    uint32_t staff = rt_arg(0), club = rt_arg(1);
    if (rd32(g_mem, cpu.esp) == RELEASE_IN_ASSESS && staff && season_game_changes() && !test_original() &&
        contract_index(staff, 1) >= 0 && rd32(g_mem, staff + CLUB_OF) != club) {
        if (getenv("RECOMP_SCREEN_LOG")) {
            char name[120];
            fprintf(stderr, "recomp: loans: %s stays at %s while on loan at %s: the release waits for the loan's end\n",
                    name_of(staff, name, sizeof name), club_name(club), club_name(rd32(g_mem, staff + CLUB_OF)));
        }
        rt_ret_stdcall(0, 2);
        return;
    }
    o_00690890();
}

/* CONTRACT_MANAGER::end_loan 0x4d07e0, thiscall (staff) ret 4. */
RT_WRAP(004d07e0, end_loan)
{
    uint32_t staff = rt_arg(0), self = cpu.ecx, loan_club = staff ? rd32(g_mem, staff + CLUB_OF) : 0;
    const char *off = getenv("RECOMP_BOX_FIXES");
    if (!staff || !loan_club || contract_index(staff, 0) >= 0 || contract_index(staff, 1) < 0 ||
        (off && !strcmp(off, "off"))) {
        o_004d07e0();
        return;
    }
    char name[120];
    fprintf(stderr, "recomp: loans: net: %s has no main contract at the end of his loan at %s: he leaves the club\n",
            name_of(staff, name, sizeof name), club_name(loan_club));
    Cpu saved = cpu;
    uint32_t a[2] = {staff, loan_club};
    cpu.ecx = rd32(g_mem, MANAGER_MANAGER);
    rt_callback(REMOVE_STAFF, 2, a);
    if (rt_unwinding) return;
    cpu.ecx = self;
    rt_callback(REMOVE_LOAN, 1, a);
    if (rt_unwinding) return;
    cpu = saved;
    rt_ret_stdcall(0, 1);
}
