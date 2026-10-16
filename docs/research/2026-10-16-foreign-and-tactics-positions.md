# Research for #47 and #48 (0.5.0)

Original: `clean-3968/cm0102.exe` 3.9.68. Memory = file offset + 0x400000 for .text, .rdata and .data (checked with the section table: .text 0x401000 at file 0x1000). Disassembly: capstone in `/Users/max/.venvs/cm0102`. Helper scripts and runs: `scratchpad/r4748/`. Screenshots (800 px wide): `scratchpad/r4748/shots/`. "Inference" marks a conclusion that I did not prove.

---

# #48 Show each player's position on the tactics screen

## 1. What the original does

- Tactics screen builder 0x899ea0. The View drop-down is node slot 10: 1 Position(s), 2 Condition, 3 Goals, 4 Form, 5 Average Rating, 6 Match Rating (in a match only), 7 and up the instructions. Each view shows one value column. Source: columns-and-colours inventory, section 2.
- Outside a match the screen opens with **Condition** ticked (screenshot `tactics-condition-*.png`, from start.sav, after Assistant > Pick best team). Position(s) is a separate view (`tactics-positions-*.png`). No view shows position and condition together.
- Position(s) view: 0x89d004 `cmp al, 1`, then `0x53ee50(player+0x61)` writes the positions text, colour yellow `[0xaebdec]` on the pitch, grey `[0xae317e]` off it (0x8aa9d0), column 6 (0x89d05a).
- Column weights 0x899ff4..0x89a01c: 14, 1, 5, 1, 60, 15, 16, 7, 7.

## 3. What our recompile already has

- Option `match_columns`, default `on` (`src/recomp_rt/win/options.c` line 39). Comment: "shows position, condition and match rating together on the match screens (widescreen only)".
- `columns_wanted()` in `src/recomp_rt/win/columns.c` line 128: on only when `rt_screen_w > 800`, so widescreen and HD. Classic only with the test variable `RECOMP_COLUMNS_CLASSIC=1`.
- Tactics wrap `RT_WRAP(00899ea0)` (columns.c line 257): only when slot 10 = 6, the **Match Rating view, which exists only in a match**. It adds position (0x53ee50) and condition (0x8aafd0) labels, weights 14, 1, 5, 1, 52, 10, 14, 26, 13, 8, 7.
- Player Ratings view of the match screen: condition beside the rating (0x722ed0, 0x723350).
- Known cut: with the test variable in classic, "D/DM RLC" shows as "D/DM" (inventory, build notes of slices 9 and 11). The position column is 26/151 of 330 px, about 57 px.

**Verdict: #48 is partly done.** Done: in a match, Match Rating view, widescreen and HD. Not done: outside a match (the screen a player uses most to pick a team), the Condition view (the default view), and classic. Screenshots confirm that from start.sav no view shows the position next to the condition in any mode.

## 4. How to do it in this port

Extend the existing tactics wrap in columns.c. Same mechanism, no new hook point:
- In `tactics_build`, also accept slot 10 = 2 (Condition), in and out of a match. Add one positions column (TACTICS_POSITIONS) and keep the condition the original draws. Outside a match the condition getter is 0x62c590, but the original already draws it, so only the positions label is new.
- The row capture uses the status call site `rt_site_tactics_status` (return 0x89e01f), which views 2 and 6 share. So the row list already works for view 2.
- Classic fit: narrower weights for the name column give the position column about 55 px. Long texts may still cut. Inference: font 1 "D/DM RC" is 42 px in the classic screenshot, so "D/DM RLC" is about 48 px and fits in 55 px if the cell has no padding. Our 57 px cut in a match suggests padding of about 10 px. Check on a classic screenshot with Lauren (D/DM R) and Kolo Abib Touré (D/DM RC).
- Cost: about half a session. Same code, a second view, a classic weight set, screenshots in three modes, one same-save run.

## 5. Can it change game decisions?

No by design: it reads the positions string (0x53ee50 into runtime scratch, kept and put back) and adds labels. Widgets come from the screen manager's fixed array (inference: no heap use, as for the shipped match columns, which pass `--match`). Test: `tests/fixtures/fixtures.py play` from start.sav: open Tactics, choose Condition, Back, then `--days 6` with `match_columns` on and off; the two saves must be equal. Also run `smoke_run.py --match`.

## 6. Open decisions for Max (#48)

1. Views: Condition view only, or also Position(s)/Goals/Form? Recommended: Condition and Match Rating. The Condition view is the default outside a match, and Match Rating already ships.
2. Classic: on in classic if the screenshot shows no cut text? Recommended: yes, with narrower weights. The issue says "the tactics screen", and classic is a mode players use.
3. Option: keep `match_columns` and widen its meaning, or a new line? Recommended: keep the one line, rename the comment to "on the tactics screen and the match screens". One option for one feature.
4. Out of position: colour the position when the player's slot (GK, DR, ...) is not one of his positions? Recommended: not in 0.5.0. The issue asks to show the position. Marking needs a slot-to-position test that I did not research.
5. Position colour: the original uses yellow. Recommended: keep yellow, as the Position(s) view.

---

# #47 Mark players with a foreign player restriction

## 1. What the original does

### The restriction record (42 bytes, 0xff = no rule)

A rules object per nation fills it by its virtual slot +0x1c. The manager's lookup is **0x8cf6b0** (thiscall on the rules manager `[0xde1f54]`, args fixture, record, collapse flag). It resolves the fixture's competition with 0x8d2fa0 to (nation, continent, kind), finds one of 28 rules objects at manager +0x8ac (nation at +0xa, continent at +0xe, continental type 2 at +0x12) and calls its filler. With the flag set it keeps one rule in this order: +5, +3, +2, +0, then +0x16/+0x17.

Byte meanings from the readers (0x8cf7e0 summary, 0x4849f0 status, text at 0x45dec3..0x45e062):
- +0, +1, +2: **foreign** limits ("Max N foreign players in the match squad", 0x98f550).
- +3, +4, +5: **non-EU** limits ("Max N non-EU players in the match squad", 0x98f51c).
- +0x16 age, +0x17 limit: "Max N players over AGE yrs in the match squad" (0x98f4dc).
- +6, +8 (Belgium 0x424f80, Italy 0x6619e0: 21 and 2 or 5): not read by the match-squad code. Meaning unknown.
- Why three slots per kind: unknown. Inference: different counts (squad, match squad, pitch); only the highest set slot is used.

**Correction to the season-change-points doc:** 0x579c63 `mov byte [edx+5], 3` is not in England's constructor. It is in England's filler **0x579c50** (vtable 0x969f4c slot +0x1c). The same filler is in vtables 0x969860 and 0x970368 (inference: Denmark and Sweden; not proved which is which).

### Fillers in 3.9.68 (address, rule)

| Rules | Filler | Rule written |
|---|---|---|
| Argentina | 0x40aa10 | +2 = 4 foreign |
| Australia | 0x412db0 | +2 = 6 foreign |
| Belgium | 0x424f80 | +6 = 21, +8 = 2 (no match limit) |
| Brazil | 0x43f2a0 | +2 = 2 foreign, none for competition `[0x9cf684]` |
| Croatia | 0x518620 | +0 = 4 foreign |
| England, Denmark, Sweden | 0x579c50 | +5 = 3 non-EU, every competition |
| France | 0x5c1a30 | +5 = 5 non-EU for competition `[0x9cf5cc]`, else 3 |
| Germany | 0x5e02d0 | +5 = 3 non-EU for competition `[0x9cf714]` (inference: German Cup), else 5 |
| Greece | 0x5ed020 | +5 = 5 non-EU for `[0x9cf86c]`, else 4 |
| Italy | 0x6619e0 | +6 = 21, +8 = 5 (no match limit) |
| Japan | 0x667090 | +2 = 3 foreign |
| Norway | 0x794740 | +5 = 2 non-EU |
| Poland | 0x7cd960 | +2 = 5 foreign |
| Portugal | 0x7d43c0 | +3 = 3 non-EU for `[0x9cf7c0]`, else 4 |
| Russia | 0x7ec950 | +2 = 3 foreign for `[0x9cf92c]` |
| Scotland | 0x7f64c0 | +0x16 = 21, +0x17 = 3 (an age rule) when competition `[0x9cf684]` and the opponent's nation match |
| Spain | 0x859570 | +5 = 2, 3 or 4 non-EU by competition and season (0x7d0 = 2000) |
| Turkey | 0x9021d0 | +2 = 6 foreign, 0 for some competitions, an age rule for `[0x9cf684]` |
| Finland, Holland, Ireland, N. Ireland, Wales | 0x63c330 | none |
| Base and one more (vtable 0x96a188, near euro_champ_qual) | 0x5832c0 | none |

Korea and USA did not match my byte pattern. Not checked. The competition ids such as `[0x9cf684]` are globals filled at load; their names are inferences.

**European cups:** I found no filler for a continental rules object that writes a limit. Inference: European cups have no foreign limit in the match squad. The European registration list (25 players, 0x85b05a) is a different rule. Check on the Next Match tab before a Champions League fixture.

### Who counts

- Foreign: **0x53d980**(person, club nation `[club+0x53]`, 1) returns 1 when neither nationality (+0x1a) nor second nationality (+0x1e) is the club's nation, with an exception list for the British Isles nations (0x53d9cf..). 
- EU: **0x53d740**(person, club nation) returns 1 when either nationality has the EU flag (nation +0x7f == 2), plus special cases per club nation (Spain and others, 0x53d770..0x53d7c1).
- Non-EU = foreign and not EU.
- Age rule: person byte +0x18 above the age byte (inference: +0x18 is the age).
- Only the **match squad** counts: 11 plus the named subs (`(fixture+0x4d & 0xf) + 11`, 0x8cf8cf).

### Where the game checks and blocks

- **0x8cf880** (team sheet check, args fixture, club, list, out text, ...): counts the match squad, and on too many writes "You have selected N non-EU players in the squad. Only M ..." (0xa8f07c..0xa8f208, foreign 0xa8ee68..0xa8effc). Returns 1.
- **0x725f70** at 0x7272cd: for the human's team at match start; on 1 it shows a "Team Selection" box (0xa17b64, box 0x803f80). This is the block.
- **0x6dbc50** (match_man.cpp) at 0x6dd47b and 0x6dd615: loops on the check (inference: fixes the team sheet of computer clubs).
- Next Match tab 0x45ccf0 "Match Rules" shows the rule. Start.sav: the friendly at Derby shows "No player restrictions"; the Premier Division match with Blackburn on 25.8.01 shows "Max 3 non-EU players in the match squad" (`next-match-premier-classic.png`).
- Club info 0x460590 shows "Number Of Non-EU Players" and "Number Of Foreign Players" (0x98fb70, 0x98fb8c).

## 3. What the original and our recompile already have

- **The original already marks foreign players, in part.** The player status 0x4849f0 (club_screens.cpp, args player, club, fixture) returns 7 for foreign (record +0..+2 set and 0x53d980) and 9 for non-EU (+3..+5 set, 0x53d980 and not 0x53d740). The squad list 0x456900 calls it at 0x45b0fc and shows **"Fgn"** (0x98e984) in the small status box (0x45b2f4, 0x45b3d5). Limits: one box shows only the first status (Wpm, Cup, Ine, Sus, Inj, Abs come first), and only when the squad screen has a fixture. The fixture is node slot 3 of the squad screen (0x456b61..0x456b6d). Inference: from the manager menu slot 3 is empty, so no "Fgn" shows; not proved.
- Our runtime colours numbers only: attribute bands and condition and morale bands (columns.c). Clickable names add an underline in the text's own colour (names.c). No name colour exists.

## 4. Proposal and hook points

**Rule:** mark a player when the rule of his club's next competitive fixture counts him: non-EU rule and he is non-EU, or foreign rule and he is foreign. Ignore the age rule. Use the game's own functions: 0x8cf6b0(fixture, record, 1), then 0x53d980 and 0x53d740 with the club's nation. These only read (0x8d2fa0 and the fillers write only the record). Do not call 0x4849f0: it does more work (0x62c590, 0x842180, 0x8c7560) that I did not check for writes.

**Where:**
- Squad screen: names are buttons with id 0x7d7 and the staff record as data (clickable-names inventory). After the build of 0x456900, set the text colour (widget +0x78, as names.c reads it) of each 0x7d7 widget whose player counts. The same wrap point works for the other squad lists 0x475520 and 0x47c180.
- Tactics screen 0x899ea0: name labels at the 0x7d7 sites 0x89dff6, 0x89e1ed, 0x89e279, 0x89e2ec. Same rule. This is the team sheet, where the block happens.
- Unknown: the function that gives the next fixture of a club. 0x45ccf0 finds it for the Next Match tab. Research this first in the build slice.

**Cost:** about one session, plus half a session to find the next-fixture function.

## 5. Can it change game decisions?

No: it reads rules and nationalities and changes a text colour. Test from start.sav: open Squad and Tactics, `--days 6`, option on and off, saves equal. A screen check needs a club with a rule and non-EU players: start.sav Arsenal (Lauren, Edu, Kanu, Gilberto Silva, Stepanovs; nationalities in the data not checked), with the next competitive fixture the Premier Division (3 non-EU). Check also a club with a foreign rule (for example a Turkish club, 6 foreign).

## 6. Open decisions for Max (#47)

1. Which fixture: the next competitive fixture, the club's league, or the squad screen's own fixture (slot 3)? Recommended: next competitive fixture. Pre-season friendlies have "No player restrictions", so the next fixture would show nothing in July.
2. Screens: squad screen only (issue text) or also the tactics screen? Recommended: both. The tactics screen is where the "Team Selection" box blocks a pick.
3. Players of other clubs (Nations & Clubs > squad): mark them too? Recommended: yes, with that club's next fixture. Same code.
4. One colour for foreign and non-EU, or two? Recommended: one. Each competition has only one kind, so one colour is clear.
5. Colour: see the list below. Recommended: light violet, RGB565 0xCC7F (206, 142, 255). It differs from every colour in use.
6. Option: a new line `foreign_colours = on`? Recommended: yes, on by default, as the other colour options.
7. Keep the original's "Fgn" box as it is? Recommended: yes. It is the original's own cue.

## 7. Build findings (slice 4, 2026-10-16)

- **Next competitive fixture.** No game function returns it. The fixture search 0x5acbe0 puts each match in a new list node on the game's heap (0x68d0d0 at 0x5acdee), so the runtime reads the store the same way and calls nothing that allocates: year blocks from `[0xae2a58]` (+0x112c next, +0x1130 year); per day and slot the list array `[block + (day * 3 + slot) * 4]` (0x5abfd2); per category the list `[array + category * 4]`, nodes +2 first, +0 fixture, +6 next (0x68d1b0). The categories of a club: 0 and 1 for a national side (0x53b900), else 2 to 7 and the results of 0x48dee0 and 0x5af5b0 (0x5acc5a..0x5accde). A fixture of the club: home id +0xc or away id +0x10, the first-team flag +0x40 or +0x41 = 1, unplayed +0x43 <= -1 (0x454c0d). Each year block has 366 day slots (0x5acd2b). Of two equal categories the earlier one goes (0x5accd4).
- **Friendly.** 0x8d2fa0 (thiscall on `[0xde1f54]`, args fixture, kind, nation, continent; ret 0x10) gives kind 0xff when 0x5c6710 says the competition is a friendly (0x8d2ff2..0x8d3014). So "competitive" = kind other than 0xff.
- **Start.sav:** Arsenal's next competitive fixture is the Charity Shield with Liverpool on Sunday 12.8.01 (day 223), not the Premier Division. England's filler gives 3 non-EU for every competition, so the rule is the same. Marked: Gilberto Silva, Luzhny, Kolo Touré, Kanu, Stepanovs.
- **Foreign rule:** no saved test game had a nation with a foreign rule. `foreign.sav` (new game, England and Croatia, made by `tests/smoke/foreign_run.py --make`): Dinamo, Croatian First Division on 20.7.01, 4 foreign players. The squad screen's own "Fgn" boxes (0x4849f0) show for the same players that the runtime marks.
- **Name sites.** Squad list: one cell button call 0x45ba84 (0x8040a0), text colour argument 7, id 0x7d7 or 0x7dc, data the staff record, the club at the caller's `[esp + 0x14]`. Tactics: one widget create call 0x89e3eb (0x55f990), text colour argument 12, id argument 15, data argument 16, the club at +0 of the tactics object, the caller's `[esp + 0x1c]`. The tactics list has 20 rows (0x89e3f7): the eleven in light grey `[0xae3184]`, the named substitutes and other reserves in grey `[0xae317e]`. Both get violet.

## Colours already used for names and nearby text (RGB565)

- Names, normal: light grey 0xe71c (231, 227, 231), `[0xae3184]` (sampled from the squad and tactics shots).
- Names not in the team on the tactics screen: grey `[0xae317e]`.
- Positions and most values: yellow 0xffe0 `[0xaebdec]`.
- Original attribute numbers and match ratings: light cyan 0x87ff `[0xaebde8]`.
- Profile condition and morale: orange 0xfc00 `[0xaebdd4]`.
- Red `[0xae31a8]` (status texts).
- Our bands (columns.c): red 0xfa8a, amber 0xfd84, light green 0xa70c, bright green 0x37e6.
- Status boxes (squad list): Inj red box, Int blue box, Wnt and Fgn grey text on `[0xaebe04]`.
- Club colours on Team views (0x53b640, views.c): any colour of a kit.

## Screenshots

`scratchpad/r4748/shots/`: `tactics-condition-{classic,widescreen,hd}.png` (default view, no position), `tactics-positions-{classic,widescreen,hd}.png` (Position(s) view), `squad-classic.png`, `next-match-premier-classic.png`.
