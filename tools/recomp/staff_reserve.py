"""The reserve of blank staff rows becomes a run-time value, for a custom data set with every league (0.6.0).

The database load reserves 23,000 blank staff rows, each with its own non-player row, at the end of the staff
table (sizing in 0x528540: staff + 0x5f05 = 23,000 + 1,325 at 0x529b39, non-players + 0x59d8 at 0x529bd1; the
blank rows at 0x52a1c2). When a new game is made, database_generate_extra_non_playing_staff 0x527240 fills
these rows from the top down with the coaches, scouts, physios and managers that clubs of the selected nations
lack. Each of its four loops stops the game at an error box when it needs row 23,001 (0x52747b, 0x52777c,
0x527a77, 0x527d43; the box of the fourth loop is "Database.cpp 1844", 0x527e4c). Some Update Team databases
need more rows when every league is selected.

rt_staff_reserve (rt.h, staff_reserve.c) replaces the constant at all seven sites. It is 23,000 for the
official data sets and for a custom data set whose estimated Select All need fits (staff_reserve.c), so the
base game allocates and does the same as before. The evidence is in
docs/research/2026-10-17-select-all-databases.md.
"""

R = "rt_staff_reserve"

# address: (constant in the original, C expression)
PATCHES = {
    # Sizing: the reserve of the staff table (with the 1,325 spare rows) and of the non-player table. Both set
    # the size for this load (rt_staff_reserve_size, staff_reserve.c), as the index file gives their order.
    0x529B39: (0x5F05, "(rt_staff_reserve_size() + 0x52du)"),
    0x529BD1: (0x59D8, "rt_staff_reserve_size()"),
    # The blank reserve rows: the loop runs for i = 1 to the reserve.
    0x52A1C2: (0x59D8, R),
    # database_generate_extra_non_playing_staff 0x527240: the end of the reserve in each of its four loops.
    0x52747B: (0x59D8, R),
    0x52777C: (0x59D8, R),
    0x527A77: (0x59D8, R),
    0x527D43: (0x59D8, R),
}
