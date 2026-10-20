"""Club counts of the original that become run-time values for the new divisions (0.6.0 slice 20).

INSNS replaces a whole instruction, as in league_sizes.py. Evidence: docs/research/2026-10-20-new-divisions.md.

GerRegional::create_table 0x5dd7f0 stores 19 at 0x5dd801 (kept when +0x40 is 2000, je at 0x5dd807) and else
18 at 0x5dd809, then sizes its table from +0x3e (0x5dd80f..0x5dd832) and takes the clubs of German Regional
with the North tag (0x5dd8d5..0x5dd8f1). rt_ger_regional_clubs (src/recomp_rt/win/season_divisions.c) gives
the original's count outside a season of the 3. Liga, and the clubs of the division in one (20 from 2008/09).
"""

# PorSecondB::create_table 0x7d2400 stores 20 at 0x7d2412 (before its first-season test) and sizes its table from
# +0x3e; the zone builder 0x7d2870 wants exactly 20 (cmp bp, 0x14 at 0x7d2965 and 0x7d29ff) and makes a
# League_Stage of 20 (push 0x14 at 0x7d2ab2). Liga 3 from 2021/22 has 2 series of 12, then of 10 (Q291).
PATCHES = {
    0x7D2965: (0x14, "rt_por_stage_clubs(esi, 0x14u)"),
    0x7D29FF: (0x14, "rt_por_stage_clubs(esi, 0x14u)"),
    0x7D2AB2: (0x14, "rt_por_stage_clubs(esi, 0x14u)"),
}

INSNS = {
    0x7D2412: ("mov word ptr [edi + 0x3e], 0x14", "W16(edi + 0x3eu, rt_por_group_clubs(edi, 0x14u));"),
    0x5DD801: ("mov word ptr [edi + 0x3e], 0x13", "W16(edi + 0x3eu, rt_ger_regional_clubs(edi, 0x13u));"),
    0x5DD809: ("mov word ptr [edi + 0x3e], 0x12", "W16(edi + 0x3eu, rt_ger_regional_clubs(edi, 0x12u));"),
}
