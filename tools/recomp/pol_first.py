"""Club slot counts of PolFirst that become run-time values for the Ekstraklasa in one table (GitHub issue 92).
Runtime: src/recomp_rt/win/season_pol_first.c.

The constructor PolFirst::PolFirst 0x7c8a80 sorts the club slots +0xee (qsort count 16, push 0x10 at 0x7c8ce5, the
object in esi) and sets a predicted position for each (cmp di, 0x10 at 0x7c8d09). set_predicted_positions 0x7ca1e0
(vtable +0x5c) does the same with the object in edi (push 0x10 at 0x7ca264, cmp si, 0x10 at 0x7ca288). In one table
the slots hold every club of the division, 14 to 18; the original's form keeps 16.
"""

PATCHES = {
    0x7C8CE5: (0x10, "rt_pol_first_slots(esi, 0x10u)"),
    0x7C8D09: (0x10, "rt_pol_first_slots(esi, 0x10u)"),
    0x7CA264: (0x10, "rt_pol_first_slots(edi, 0x10u)"),
    0x7CA288: (0x10, "rt_pol_first_slots(edi, 0x10u)"),
}
