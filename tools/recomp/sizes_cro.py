"""The Croatian second tier at 18 clubs (0.7.2 league sizes, slice 11, D22).

Instruction rows (address: (the instruction in the original, C code)) for league_sizes.INSNS. The runtime is
src/recomp_rt/win/season_sizes_cro.c. CroA2a::create_tiebreak_playoff_stage 0x5160f0 puts the tied clubs in a
stack array of 16 pointers at esp+0x1c; a table of 18 can tie 9 pairs. With a table of more than 16 rows in a
season game the stores and the list that Cup_Stage::Cup_Stage 0x522e00 reads go to a list of the runtime; else the
stack array of the original (Q152).

The regroup of the second tier, 0x515190 (called by CroA1::start_new_season at 0x5149c2), sorts the clubs of the
North and South groups in a stack array of 32 at esp+0x14 and gives the first 16 the North, for 32 slots. In a season
game it uses a list of the runtime, walks the clubs it found, and gives the North the count of its row when the South
is not held (2006/07 on).
"""

def _cmp(width_mask, size, left, right):
    return (f"t0 = {left}; t1 = ({right}) & {width_mask}; t2 = ((t0 - t1 - 0) & {width_mask}); "
            f"SETF(K_SUB | {size}, t0, t1, t2, 0);")


INSNS = {
    # The first club of a tied pair (index in edi, from bp at 0x516164).
    0x51616A: ("mov dword ptr [esp + edi*4 + 0x1c], ebp", "W32(rt_cro_tiebreak_slot(esi, esp + 0x1cu, edi), ebp);"),
    # The second club (index in edi, from di at 0x516181).
    0x516190: ("mov dword ptr [esp + edi*4 + 0x1c], ecx", "W32(rt_cro_tiebreak_slot(esi, esp + 0x1cu, edi), ecx);"),
    # The list for Cup_Stage::Cup_Stage: esp+0x48 here is the array (11 pushes after 0x516286).
    0x51629A: ("lea ecx, [esp + 0x48]", "ecx = rt_cro_tiebreak_list(esi, esp + 0x48u);"),
    # The regroup 0x515190: the bound of the clubs it takes (dl counts them, je at 0x515203).
    0x5151FC: ("cmp dl, 0x20", _cmp("0xffu", "SZ1", "(edx & 0xffu)", "rt_cro_regroup_max(0x20u)")),
    # Each club it takes (index in esi, from dl at 0x5151f7).
    0x5151FF: ("mov dword ptr [esp + esi*4 + 0x14], eax", "W32(rt_cro_regroup_slot(esp + 0x14u, esi), eax);"),
    # The array of the sort 0x945333 (esp+0x14 two pushes later), with the count it found in eax (0x51520f).
    0x515219: ("lea ecx, [esp + 0x1c]", "ecx = rt_cro_regroup_list(esp + 0x1cu, eax);"),
    # The array of the walk.
    0x51522D: ("lea esi, [esp + 0x14]", "esi = rt_cro_regroup_list(esp + 0x14u, 0xffffffffu);"),
    # The North's count: the first 16 go North (jl 0x51523a), the others South.
    0x515231: ("cmp eax, 0x10", _cmp("0xffffffffu", "SZ4", "eax", "rt_cro_regroup_north(0x10u)")),
    # The division of each club of the walk: the South keeps its 16 rows, the clubs after North + 16 go to A Lower
    # Division (season_sizes_cro.c). The place is [esp+0x10] (0x515229, 0x5152c1).
    0x51523A: ("mov eax, dword ptr [esi]", "ebx = rt_cro_regroup_target(ebx, R32(esp + 0x10u)); eax = R32(esi);"),
    # The end of the walk (jl 0x515231).
    0x5152BB: ("cmp eax, 0x20", _cmp("0xffffffffu", "SZ4", "eax", "rt_cro_regroup_end(0x20u)")),
}
