"""The knockout stage of the Champions League in a format with one group stage (0.6.0 slice 22).

The original keeps the knockout stage of the Champions League in slot 12 of the stage array (+0xc of the
EuropeanCup object; draw_quarter_finals 0x586330 writes it at 0x58656d). In the format of 2003/04 on, the
runtime keeps it in slot 8 and sets the stage count +0x2c to 9 (src/recomp_rt/win/season_uefa.c). Two
functions of the class read the slot with a fixed offset 0x30. rt_uefa_cl_ko_offset(cl) gives 0x30 for a
Champions League of 13 stages, as in every base game, and 0x20 for one of 9 stages. Evidence:
docs/research/2026-10-17-uefa-formats.md.
"""

# address: (constant in the original, C expression)
PATCHES = {
    # EuropeanCup::record_season_honours 0x584ad0: mov ecx, [eax + 0x30] with this in ecx; it then jumps to
    # slot +0x30 of the knockout stage.
    0x584AD3: (0x30, "rt_uefa_cl_ko_offset(ecx)"),
    # EuropeanCup::update_staff_records_for_status 0x584ae0: mov ecx, [edx + 0x30] with this in esi; it reads
    # the round code of the knockout round table (+0xa3, the round after the club's round, +0x6f).
    0x584CF2: (0x30, "rt_uefa_cl_ko_offset(esi)"),
    # Intertoto_Cup::update_for_date 0x633d40 (UEFA Cup in ecx, Intertoto Cup in edi) copies the winners of its
    # last round into the UEFA Cup: sub dx, 0x1b gives the first slot (count - 0x1b), lea eax, [edx + 3] the end.
    # With an Intertoto Cup of 3 rounds and a UEFA Cup of 153 slots (win/season_uefa_cup.c), the 11 winners go
    # to the last 11 new slots of the UEFA Cup's second qualifying round; else 0x1b and 3, as the original.
    0x633DEB: (0x1B, "rt_uefa_uic_back(ecx, edi)"),
    0x633DF3: (0x3, "rt_uefa_uic_winners(ecx, edi)"),
}
