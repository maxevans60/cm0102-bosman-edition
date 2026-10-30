"""International caps above 255 (0.6.2 slice 12, fault 30, Q25; src/recomp_rt/win/caps_wide.h).

`NationalTeams::record_international_result` 0x76b600 gives each player of a national side his cap: 0x76b890 reads
the caps byte at staff +0x22 and, when it is 0, takes the first-cap path (0x76b897..0x76b8c3: the nation of the
match becomes his first nation, and call 0x86cc50 writes the "first cap" history row). Then 0x76b8c8 mov al,
[esi + 0x22] / 0x76b8ce inc al / 0x76b8d0 mov [esi + 0x22], al raise the byte with no check, so the 256th cap gave
0 and the next one a second first cap. The flags of the inc are not read: 0x76b8d6 add dl, cl sets them again.

Here the byte stops at 255 and the runtime counts on (rt_caps_add). Below 255 the result is the original's.
"""

INSNS = {
    0x76B8CE: ("inc al", "eax = (eax & 0xffffff00u) | rt_caps_add(esi, eax & 0xffu);"),
}
