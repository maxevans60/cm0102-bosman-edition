/* A loan club takes a staff job from a staff member on loan (O358, box contract_manager..cpp 5941).
 *
 * Each staff has a row of 79 bytes at [0xae2d0c] + id * 79: +0 the index of his main contract, +4 the index of his
 * loan contract, -1 for none. A contract is 80 bytes in the table at [CONTRACT_MANAGER] ([0xae1050]): +0x4 the club
 * id, +0x2d and +0x2f the end day and year, +0x35 the type. Clubs are 0x245 bytes from [0xae23b4].
 *
 * The cause. MANAGER_MANAGER::remove_staff_from_club 0x690890 (staff, club) takes a staff member out of a job at the
 * club. It always removes his main contract first (CONTRACT_MANAGER::remove_contract 0x4d0b50, called at 0x69089e),
 * whichever club holds it. Then, when staff +0x39 is the club (0x6908ac), it clears the job by +0x3d (jump table at
 * 0x690af8): a player/manager, player/coach or player/physio (12, 13, 15) becomes a player (+0x3d 11) and keeps +0x39
 * (0x6908f6, 0x69092d, 0x69094a); a plain staff job clears its slot and +0x39; a player (11) and the jobs 3, 4, 7 and
 * 14 take the default at 0x6909c7, which keeps +0x39 unless the low byte of the staff pointer is 0 (0x6909c7 compares
 * that byte with 0). For a staff on loan at the club, the main contract is the one with his parent club, so it goes
 * and the loan contract stays. On the loan's last day CONTRACT_MANAGER::daily_update 0x4c7ac0 calls end_loan 0x4d07e0
 * (0x4c8114), or at a transfer TRANSFER_MANAGER::move_player_to_club 0x8c9720 calls it (0x8c9d5d); end_loan needs the
 * main contract (0x4d0857..0x4d088b) and shows the box at 0x4d08e3.
 *
 * The call that was seen: Shortlist_Manager::assess_club_squad 0x83cb40 lists the club's staff slots (club +0xd3, then
 * the slots from +0x19f, 0x83ec1f..0x83ec64), takes the first coach or player/coach (+0x3d 6 or 13, 0x83edac) and,
 * when his rating (0x69d1b0) is below the slot's need and he has no contract offer from the club (0x4cc9e0), calls
 * remove_staff_from_club at 0x83ee72; the other staff jobs do the same at 0x83ef82 (7, 14), 0x83f092 (8, 15),
 * 0x83f19b and 0x83f2b1. Cases: soak-2021 leg 1, Brown de Adrogue and Alfredo Grelak, a player/coach on loan from
 * Platense (0x83ee72); soak-tip leg 0, Francisco Beltrao and Juliano Cesar de Moraes Tobias, on loan from Joinville
 * (0x83f092); gate-2015 leg 1, Larne and Matthew Kerr. The other callers of remove_staff_from_club
 * (MANAGER_MANAGER::daily_update 0x68f9c0, remove_club_manager 0x69b790, appoint_manager_to_club 0x69ebc0) pass a
 * staff with non-player data (+0x69) and a club of his, so they reach the same state for a loanee in a staff job.
 *
 * The fix, in a season game (season_game_changes, Q152), for every caller: when the club is the club of the staff's
 * loan contract and not the club of his main contract, remove_staff_from_club runs without the main contract's
 * removal (the wrap of 0x4d0b50 below passes over the call from 0x69089e). The job is cleared as in the original, so
 * a player/coach stays a player at the loan club, on loan, with his parent club's contract. Logged once a staff. The
 * base game keeps the original. Test switch RECOMP_TEST_LOAN_RELEASE=original: a season game takes the original too,
 * so that the net below acts.
 *
 * The net, a box fix in all games, the base game too, where the original would show the box (the test switch
 * RECOMP_BOX_FIXES=off turns it off): end_loan of a staff with no main contract does not call the original. It calls
 * remove_staff_from_club (staff, loan club), then CONTRACT_MANAGER::remove_loan_contract 0x4d0bd0. A released
 * player/coach is a player (+0x3d 11) by then, so remove_staff_from_club takes its default case: he stays at the loan
 * club and in its squad (+0x39 kept) with no contract, the state the original gives a released player/coach who is not
 * on loan. Logged. */
#include "win.h"
#include "season_names.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STAFF_COUNT 0xae2364u
#define STAFF_ROWS 0xae2d0cu            /* 79-byte rows: +0 main contract, +4 loan contract */
#define CONTRACTS 0xae1050u             /* CONTRACT_MANAGER: [this] is the contract table */
#define CLUBS 0xae23b4u
#define CLUB_SIZE 0x245u
#define MANAGER_MANAGER 0xb67a44u       /* ecx of remove_staff_from_club (0x83ee6c) */
#define CLUB_OF 0x39u
#define MAIN_FROM_REMOVE 0x006908a3u    /* the return address of the remove_contract call at 0x69089e */
#define REMOVE_STAFF 0x00690890u
#define REMOVE_LOAN 0x004d0bd0u

void o_00690890(void);
void o_004d0b50(void);
void o_004d07e0(void);

static int contract_index(uint32_t staff, uint32_t k)
{
    uint32_t id = rd32(g_mem, staff);
    if (id >= rd32(g_mem, STAFF_COUNT) || !rd32(g_mem, STAFF_ROWS)) return -1;
    return (int32_t)rd32(g_mem, rd32(g_mem, STAFF_ROWS) + id * 79u + 4u * k);
}

/* The club of contract k (0 main, 1 loan) of the staff, as a club pointer; 0 when none. */
static uint32_t contract_club(uint32_t staff, uint32_t k)
{
    int c = contract_index(staff, k);
    uint32_t table = rd32(g_mem, rd32(g_mem, CONTRACTS)), clubs = rd32(g_mem, CLUBS);
    if (c < 0 || !table || !clubs) return 0;
    int32_t id = (int32_t)rd32(g_mem, table + (uint32_t)c * 80u + 4u);
    return id >= 0 ? clubs + (uint32_t)id * CLUB_SIZE : 0;
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

/* The staff whose main contract remove_staff_from_club keeps in this call, else 0. */
static uint32_t keep_main;

/* 1 the first time a staff id is logged (ids below 1 << 20). */
static int first_log(uint32_t id)
{
    static uint8_t seen[1u << 17];
    if (id >= (1u << 20)) return 1;
    int first = !(seen[id >> 3] & (1u << (id & 7)));
    seen[id >> 3] |= (uint8_t)(1u << (id & 7));
    return first;
}

/* remove_staff_from_club 0x690890, thiscall (staff, club) ret 8. */
RT_WRAP(00690890, remove_staff_from_club)
{
    uint32_t staff = rt_arg(0), club = rt_arg(1);
    if (!staff || !club || !season_game_changes() || test_original() || contract_club(staff, 1) != club ||
        contract_index(staff, 0) < 0 || contract_club(staff, 0) == club) {
        o_00690890();
        return;
    }
    if (getenv("RECOMP_SCREEN_LOG") && first_log(rd32(g_mem, staff))) {
        char name[120];
        fprintf(stderr, "recomp: loans: %s leaves his job at %s, his loan club, and keeps his contract with %s\n",
                name_of(staff, name, sizeof name), club_name(club), club_name(contract_club(staff, 0)));
    }
    uint32_t outer = keep_main;
    keep_main = staff;
    o_00690890();
    keep_main = outer;
}

/* CONTRACT_MANAGER::remove_contract 0x4d0b50, thiscall (staff) ret 4. */
RT_WRAP(004d0b50, remove_contract)
{
    if (keep_main && rt_arg(0) == keep_main && rd32(g_mem, cpu.esp) == MAIN_FROM_REMOVE) {
        rt_ret_stdcall(0, 1);
        return;
    }
    o_004d0b50();
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
    fprintf(stderr, "recomp: loans: net: %s has no main contract at the end of his loan at %s: the loan is freed\n",
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
