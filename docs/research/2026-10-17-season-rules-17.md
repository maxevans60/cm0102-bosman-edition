# Substitutes, foreign player rules, work permits and transfer windows by season (0.6.0 slice 17)

Date 2026-10-17. Original: `clean-3968/cm0102.exe` 3.9.68, virtual addresses. Code: `src/recomp_rt/win/season_kind_subs.c`, `season_subs_rule.c`, `season_kind_nation.c`, `season_nation.c` and `season_nation_table.c` (made by `tools/seasons/encode_rules.py`). Tests: `tests/seasons/test_season_subs.c`, `test_season_nation.c`, `test_encode_rules.py`, `season_rules17_run.py`. Rule kinds `subs` and `nation` in the list of `season_change.c`.

## 1. Substitutes and bench

The rules table gives `subs_used` and `bench` for 2,852 and 1,461 rows. The kind `subs` writes them into the competition object before the original packs them into a fixture.

- A competition object keeps the named substitutes at +0x49 (`Comp::Comp` 0x48c5c9 sets 5) and the substitutes used at +0x4a (0x48c5cd sets 3). `Comp::set_fixture_substitutes` 0x48f2d0 (vtable +0x7c) packs them into the word at fixture +0x4d: named in bits 0 to 3, used in bits 4 to 7 (0x48f2fa..0x48f32d).
- Four classes override that slot: `AusNsl` 0x4124e0, `FraCup` and `FraLgeCup` 0x5bb100, `GreCup` 0x5ea3d0. All four read +0x49 and +0x4a. French and Greek cups set fixed values for some rounds (0x5bb10c, 0x5ea3db). The kind wraps all four functions and sets +0x49 and +0x4a first, so the fixed rounds stay.
- The match engine and the team screens read the nibbles as `(word & 0xf) + 11` for the squad size (for example 0x459aa8, 0x474e49, 0x478d49, 0x47b0ff, 0x6d0f8c, 0x6db9ad, 0x721e84, 0x7261cf, 0x8cf8cf) and `init_match` 0x6b7fe6..0x6b8015 sets a nibble above 9 to 9. The Greek cup already names 7. So a bench of up to 9 works where the original reads it. **Proven:** the clamp at 0x6b7feb and 0x6b8006. **Not proven:** that the human team sheet screen shows 9 free slots (no match was played with a bench of 9 in these tests).
- A count above 9 gives 9. The table has 12 for the World Cup and many 2016+ competitions, and 15 for the Serie A and Serie C rows from 2022/23. These are the rows the game cannot hold.
- A row with `unknown`, `n/a`, `as the Champions League`, `see each` or a description gives no count and the competition keeps what it has. The first row of a file with a count wins. No competition file has two rows of one season with different counts (checked on the 4,914 file and season pairs).
- The season is the start year with changing rules off (Q232), the season of the pass that starts the competition (before hook), else the season year +0x40 of the object.
- The base game (start 2001, changing rules off) and `RECOMP_SEASON_CODE=off` keep the original's values.

Result (`season_rules17_run.py`, September 2010 ODB, England): with the start in 2010/11 the Premier Division and the three divisions, the League Cup, the FA Cup and the Charity Shield take 7 named substitutes (6 used for the Shield); the report equals the table for every competition with a count that the engine can hold; with changing rules on the three Football League divisions go to 5 named in 2011/12 as the table says, with changing rules off they keep 7.

## 2. Nation rules: the encoder

`tools/seasons/encode_rules.py` reads the 27 nation rows of every season (729 rows) and writes the committed outputs:

- `docs/research/2026-10-17-nation-rules-encoded.tsv`: for each nation and season the windows, four foreign player limits and the work permit rule.
- `docs/research/2026-10-17-nation-rules-gaps.tsv`: **the rows the encoder cannot encode, for Max** (400 texts), each with its reason. A text of `unknown` or `n/a` is not a gap: the game keeps its values.
- `src/recomp_rt/win/season_nation_table.c`: the C table. A row that changes nothing is left out. The runtime folds the rows of a nation in order, so a row of `unknown` keeps the earlier rule and the rules stay after the last row (Q248).

The tool stops on a gap reason it does not know. The windows texts are read by pattern (two or three date ranges, "end of season to 31 Aug" for England with 1 June as the start, an Italian "four weeks"). The foreign player texts are read by one rule for each nation and form. `test_encode_rules.py` holds the forms.

Counts (`encode_rules.py --summary`): 96 rows with windows, 400 gap texts: windows 22 (20 partial, 2 fixed index), foreign 100, work permit 137 (the permit criteria are not coded in the game), registration 143 (squad caps and home-grown rules need new code).

## 3. Transfer windows

Evidence in `2026-10-16-sweep-rulebooks.md` section 3.1 and `sweep-season-end.md` section 4.2. The kind rewrites the window records of the nation's rules object (+4 array, +8 count byte; 12 bytes a record: id, index, weekday 0xff, day, month 0 to 11, start flag) with a new array from the game's allocator 0x944e46, and makes the manager's sorted table of bounds (+0x8a4, count +0x8a8) again as `setup_transfer_rules` does (0x8d2e27..0x8d2f78, qsort 0x945333, comparator 0x8e5590). Byte 0 of a bound is the index of the rules object in the manager (`update_transfer_window_news` 0x8c6d17, 0x8c6d1c).

- The rules objects are made again at every load, so the `loaded` hook applies the rules of the last season point again. A new game applies the start year; the season hook applies each July.
- Nations with rows that give dates: England (from 2002/03), Germany (all seasons), Spain (2020/21 and 2022/23 on), Italy (2007/08 on, with some seasons missing), Russia, Turkey 2021, the United States 2019 to 2025, Brazil 2023 to 2026, the Republic of Ireland 2024, Holland 2025, Sweden 2012 and 2019.
- Argentina and France are left alone: the original gives a meaning to the window index (0x40a884, 0x40a995 for Argentina; 0x5c1830, 0x5c1930 for France). Italy keeps its window 2 (free transfers only, 0x661520, 0x661630) and its regular windows take indexes 0 and 1 (further ones 3 and up).
- A window with no day for "end of season" is an inference: 1 June.
- The weekday rule of a bound ("move to the next Thursday") is not used: the research gives dates only. "1 Sep when 31 Aug is a weekend" is not coded; the date in the table is the plain one.
- Inference, not tested: nothing else reads the window array in a way that a new array breaks (the readers found are `find_open_window` 0x8e8e50, `transfer_window_contains_date` 0x8e8c40, `get_days_until_window_closes` 0x8e8ed0 and the news function above). A transfer in a changed window was not played.

## 4. Foreign player limits

Two mechanisms, both data in the rules objects or the restriction record (`2026-10-16-foreign-and-tactics-positions.md`):

- **Signing caps** `+0x13` (foreign) and `+0x14` (non-EU) of the nation's rules object: `can_sign_player` 0x8e8560 refuses a signing at the cap; 0xff turns the check off (0x8e8614). The kind writes the number of the table ("a club may hold at most N").
- **Match squad limit**: the kind wraps the lookup 0x8cf6b0 and, for a fixture of a first division (the table row of the competition has tier 1), gives the slots of the record that are already set (+0..+2 foreign, +3..+5 non-EU) the number of the table, or 0xff for no limit. It adds no limit that the game does not have.

Not applied, with reasons in the gaps file:

- `hardcoded` (27, Spain): the First and Second Division caps of Spain are numbers in code (0x8e85d3, 0x8e85e8, 0x8e85ef, sweep-rulebooks 3.3), not in the rules object. A checked code patch is needed.
- `new_code` (27, Italy): the quota of signings from abroad per season has no field in the game.
- `pitch` (26): a limit on the pitch at one time (Russia, Greece, Poland, Sweden). The game limits the match squad only.
- `kind` (11): a non-EU limit where the game's record holds a foreign slot (Croatia from 2025, Sweden): applying it would limit EU players.
- `mid_season` (3): a rule that changes during a season (Argentina 2017, Croatia 2016, Turkey 2007).

England: the table says there is no nationality quota. The original has a limit of 3 non-EU players in the match squad of every English competition (0x579c63, in the filler 0x579c50). With changing rules on, the kind takes that limit away for English first division fixtures. **Question for Max (1), recommended: yes**, because the table is the rule of the season and the work permit still limits non-EU players.

## 5. Work permits and Brexit

`player_needs_work_permit` 0x8c7560 (thiscall: player, club, flag) asks 0x53d740 whether the player is of the EU and stops when he is. With the flag set it returns 1 for the others at once (0x8c760d); with the flag clear it looks for a permit in the manager's list (0x8c7650). The 30 callers push 0 or 1 for the flag (`callers.py` scan of the .text).

The kind answers 1 for a player of the EU who is not a citizen of the UK or Ireland, when the flag is set, the club is of England, Scotland, Wales or Northern Ireland, the table row of the season has the Brexit rule (England 2020/21, the others the same season) and the game date is 1 January 2021 or later. **Inference, not proven:** the flag marks a new signing. The players already at a club are asked with the flag clear and stay, as the real transition did. If the flag means something else for some callers, EU players could be refused outside a signing. The test run counts the refusals; it does not show which screen asked.

The permit criteria themselves (FIFA ranking bands, the points system) are not coded in the game: the game's own permit decision stays. This is listed as `new_code` for 137 texts.

## 6. News

At the season point the kind adds one sentence for each nation that has a competition in the game when the transfer windows, a cap, a squad limit or the Brexit rule change (for example "England: the transfer windows are now 14 June to 1 September and 1 January to 1 February."). The text goes through `season_change_news`.

## 7. Not done

- Registration rules: squad size caps and home-grown quotas (143 texts: Premier League 25-man list, Serie A 25-player list, Russian, Turkish and Portuguese lists, the Belgian and Danish rules). The game has no squad registration list except the European one (squad_manager.cpp 0x85b05a).
- The rules of other competitions (UEFA, World Cup) for foreign players: no row of those parts has a foreign column. Their substitutes and bench come in through the `subs` kind (UEFA rows `european_cup`, `uefa_cup`, and the international files), as far as the engine holds the counts.
- The Brexit rule for the Welsh and Northern Irish clubs: the texts say the leagues outside the Premier League and EFL are unknown. The kind applies it to all four nations (**Question for Max (2)**).
- Out of scope by Q249: loan limits, financial rules.

## 8. Tests and results

Run on 2026-10-17 and 2026-10-18 (load 10 to 24 from other runs):

| Command | Result |
|---|---|
| `cd build && ctest` | 20 of 20 pass (`season_subs`, `season_nation`, `season_encode_rules` among them) |
| `python3 tests/seasons/season_rules17_run.py build/cm0102_recomp $G --only on2010` | PASS (310 s): 7 named substitutes from 2010/11, 5 for the Football League from 2011/12, England with windows 1.6-31.8 and 1.1-31.1, the squad record without the non-EU limit of 3 |
| same, `--only off2010` | PASS (298 s): the rules of 2010/11 stay in 2011/12 |
| same, `--only on2020` | PASS (460 s): 3,540,500 asks of the permit function with the flag set and an EU player at an English club after 1 January 2021 (the same count in two runs) |
| `python3 tests/smoke/smoke_run.py build/cm0102_recomp $G --same-save --save start.sav --ref-build <main build>` | PASS, also `prematch.sav` and `yeovil.sav --days 7`. `season-code-off` and the main build equal `normal` |
| the same without `--save` (a new game) | FAIL before the change to `smoke_run.py`: `rules-on` differs from `normal` in the season part and in four parts of the game (`general.dat` ...). This is by design (Q232): with changing rules on the 2001/02 rules of the table apply. `RULES_ON_PARTS` in `smoke_run.py` allows these parts for `rules-on` only. The new-game run was not repeated after that change (the lead runs the full set) |
| `rules_report_check.py --run ... --year 2010 --last 2011` (changing rules off) | 18 lines of `2010-england.tsv` changed: 13 bench and subs_used differences gone, 5 new (the Football League keeps 7 in 2011/12; the Club World Cup holds 9 where the table has 12) |

A start in 2019 (October 2019 or March 2020 database) stops at `comp_stats.CPP 1664` (`Comp_Stats::set_club_final_position` 0x4a2540 with a null club) on 7 February 2020 in the season jump. The main build stops at the same place, so it is not a finding of this slice. It blocks a news check for the Brexit sentence, which needs a start before 2020.

Tests that I mutated to see them fail: the clamp of the lookup (`season_subs`).

## 9. Questions for Max

1. England has no nationality quota in the table, and the original limits non-EU players in the match squad to 3. With changing rules on the kind takes that limit away. Recommended: yes.
2. The Brexit rule for Wales and Northern Ireland: the texts say the scope for their leagues is unknown. Applied to all four. Recommended: yes.
3. A bench above 9 is held at 9 (World Cup 12, Serie A and Serie C 15 from 2022/23). Recommended: accept; a larger bench needs a change to the match engine.
4. The rules of 2001/02 now apply to a 2001 game with changing rules on (new defaults: 7 named substitutes in some leagues, one English window change, foreign caps). Recommended: accept, it is what Q232 says.
