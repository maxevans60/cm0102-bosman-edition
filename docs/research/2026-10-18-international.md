# International competitions in the original (0.6.0 slice 27)

Date 2026-10-18. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Spec: `docs/specs/2026-10-16-seasons-0.6.0-spec.md`, slice 27, Q248, Q276, Q277, Q288, Q366, Q367. Build brief: `docs/research/build-brief-0.6.0.md`, slice 27. Sweep: `docs/research/2026-10-16-sweep-international.md` (named "sweep" below).

Code: `src/recomp_rt/win/season_intl.h`, `season_intl_rules.c` (editions and rules, no game memory), `season_intl.c` (host table, Australia, the rule kind), `season_intl_table.c` (made by `tools/seasons/make_intl.py`), the sites in `tools/recomp/seasons.py`. Tests: `tests/seasons/test_season_intl.c` (ctest `season_intl`), ctest `season_intl_table`, `tests/seasons/season_intl_run.py`.

## The edition table

`tools/seasons/make_intl.py` reads the rows of `int-world.tsv` and `int-continental.tsv` whose `comp_file` is a finals class of the original (`world_cup`, `euro_champ`, `fifa_confed`, `olympics`, `goldcup`, `african_nations`, `asia_nations`, `oceania_nations`) and whose format is not "not held". Each row is one edition: the season of the row, the edition year that the class holds at +0x40, the team count, the golden goal, the first day and the hosts. Slices 28 and 29 add their competitions to the same table.

- The edition year is the year of the last date of the format. So the 2025 Africa Cup of Nations (21 December 2025 to 18 January 2026) is the edition 2026 of the class, and the 2022 World Cup is 2022.
- Rows with no edition that a class can play are in `SKIP` of the tool with the reason: the 2004 OFC final (part of row 32), the second row of the 2008 OFC round robin, the cancelled 2020 OFC Nations Cup, and the 2028 Africa Cup of Nations, OFC Nations Cup and Copa América (no host, dates or format yet, Q248).
- Q366 moves the Tokyo Olympics (row int-world-251) to 2020/21 on their planned first day, 22 July 2020.
- Q367: the format of Euro 2004 says silver goal, so the edition has no golden goal: full extra time, then penalties.
- Hosts with no automatic place get no host row: Euro 2020 (12 host nations, each had to qualify) and Euro 2028 (no automatic place; two places are kept for hosts that miss out in the qualifying, slice 28).
- Nation names of the table that nation.dat writes another way: Netherlands is Holland, China is China PR (China in later databases), the United Kingdom of the 2012 Olympics is England (inference: the game has no Great Britain team).
