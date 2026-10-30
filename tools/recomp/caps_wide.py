"""International caps above 255 (0.6.2 slice 12, fault 30, Q25; src/recomp_rt/win/caps_wide.h).

`NationalTeams::record_international_result` 0x76b600 gives each player of a national side his cap: 0x76b890 reads
the caps byte at staff +0x22 and, when it is 0, takes the first-cap path (0x76b897..0x76b8c3: the nation of the
match becomes his first nation, and call 0x86cc50 writes the "first cap" history row). Then 0x76b8c8 mov al,
[esi + 0x22] / 0x76b8ce inc al / 0x76b8d0 mov [esi + 0x22], al raise the byte with no check, so the 256th cap gave
0 and the next one a second first cap. The flags of the inc are not read: 0x76b8d6 add dl, cl sets them again.

Here the byte stops at 255 and the runtime counts on (rt_caps_add). Below 255 the result is the original's.

The screens that show caps take the full count (rt_caps_full, the byte below 255). Each site below reads the byte into
a register that it cleared first (xor) or masks with 0xff, then gives it as the number of a "%d" text (sprintf 0x944c9f,
langlib_sprintf 0x66f4e0) or a news slot. The inventory of all 171 caps sites (run-logs/0.6.2-s12/caps-sites.tsv,
outside the repo): the other readers compare, sort or copy the byte, and see 255, the most it holds.
- 0x45bcaf (selectable column) and 0x45c870 (caps column) of `club_screen_draw_squad_tab` 0x456900, staff in edi
- 0x462550 and 0x462638 of `club_screen_draw_info_tab` 0x460590: the highest and the lowest capped player, staff in eax
- 0x479af2 and 0x47a60a of `squad_numbers_screen_build` 0x475520, staff in edi
- 0x480401 and 0x480f07 of `part_exchange_screen_build` 0x47c180, staff in ecx
- 0x7a9422 of `player_regen_post_retirement_news` 0x7a92e0: news slot 9 (NewsItem::set_slot 0x785790), staff in esi
- 0x7d4c6c of the squad printout 0x7d4460 (caps not 0, read at 0x7d4c65), staff in ebx
- 0x80e5f1 of `search_screens_build_player_search_screen` 0x808690, staff in esi
- 0x874e7f, 0x874ea2 and 0x874ec3 of `staff_screens_build_profile_screen` 0x870700: the nationality line with more than
  one cap (read at 0x874e6a), for more than one goal, one goal and none, staff in ebx
"""

INSNS = {
    0x76B8CE: ("inc al", "eax = (eax & 0xffffff00u) | rt_caps_add(esi, eax & 0xffu);"),
    0x45BCAF: ("mov dl, byte ptr [edi + 0x22]", "edx = rt_caps_full(edi);"),
    0x45C870: ("mov dl, byte ptr [edi + 0x22]", "edx = rt_caps_full(edi);"),
    0x462550: ("mov cl, byte ptr [eax + 0x22]", "ecx = rt_caps_full(eax);"),
    0x462638: ("mov cl, byte ptr [eax + 0x22]", "ecx = rt_caps_full(eax);"),
    0x479AF2: ("mov dl, byte ptr [edi + 0x22]", "edx = rt_caps_full(edi);"),
    0x47A60A: ("mov al, byte ptr [edi + 0x22]", "eax = rt_caps_full(edi);"),
    0x480401: ("mov al, byte ptr [ecx + 0x22]", "eax = rt_caps_full(ecx);"),
    0x480F07: ("mov al, byte ptr [ecx + 0x22]", "eax = rt_caps_full(ecx);"),
    0x7A9422: ("mov cl, byte ptr [esi + 0x22]", "ecx = rt_caps_full(esi);"),
    0x7D4C6C: ("and eax, 0xff", "eax = rt_caps_full(ebx);"),
    0x80E5F1: ("mov cl, byte ptr [esi + 0x22]", "ecx = rt_caps_full(esi);"),
    0x874E7F: ("and eax, 0xff", "eax = rt_caps_full(ebx);"),
    0x874EA2: ("and eax, 0xff", "eax = rt_caps_full(ebx);"),
    0x874EC3: ("and eax, 0xff", "eax = rt_caps_full(ebx);"),
}
