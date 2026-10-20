"""Italy's single Serie C of three groups, 2014/15 on (0.6.0 slice 20b): the patched instructions.

The groups A, B and C are the objects of Serie C1/A, C1/B and C2/A (src/recomp_rt/win/season_ita_serie_c.c, doc
docs/research/2026-10-20-ita-serie-c.md). Their classes hold 18 clubs: create_table allocates 18 rows of 0x3b
bytes (push 0x426 at 0x64b67b, 0x64f82b, 0x653a9b, the object in edi from 0x64b679, 0x64f829, 0x653a99). In the
Serie C era a group holds up to 20 (rt_ita_table_bytes). C1/A and C1/B store their count through slice 18
(league_sizes.py); C2/A is a group class there, so its store 0x653aa2 is here (rt_ita_c2a_clubs).

ItaSerA::start_new_season 0x641700 reads C2/B and C2/C whenever C2/A exists. Two loads of the C2/A object become
run-time values: 0x6418d5 (the tier block: 0 sends it to C1/A and C1/B, 0x641915) and 0x641986 (the start block:
0 skips the starts of C2/B and C2/C, 0x6419ba). In the Serie C era rt_ita_tier_c2 and rt_ita_start_c2 do the
C2/A part themselves and give 0; else they give the object.
"""

PATCHES = {
    0x64B67B: (0x426, "rt_ita_table_bytes(edi, 0x426u)"),
    0x64F82B: (0x426, "rt_ita_table_bytes(edi, 0x426u)"),
    0x653A9B: (0x426, "rt_ita_table_bytes(edi, 0x426u)"),
}

INSNS = {
    0x653AA2: ("mov word ptr [edi + 0x3e], 0x12", "W16(edi + 0x3eu, rt_ita_c2a_clubs(edi, 0x12u));"),
    0x6418D5: ("mov ecx, dword ptr [eax + edx*4]", "ecx = rt_ita_tier_c2(R32(eax + edx * 4u));"),
    0x641986: ("mov ecx, dword ptr [ecx + edx*4]", "ecx = rt_ita_start_c2(R32(ecx + edx * 4u));"),
}
