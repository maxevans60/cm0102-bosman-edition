# Sweep of the rule book files (slice 1, sweep-rulebooks)

Date 2026-10-16. Research for release 0.6.0, slice 1 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Docs only, no code changes.

Scope: every `rb_*.cpp` file except `rb_europe`, `rb_south_america`, `rb_asia`, `rb_oceania` and `rb_international`, and every `*_rules.cpp` file. The function map places 313 functions in these files, with the gap functions that the map shares with a neighbour file. Proven names: `/Users/max/Projects/cm0102/research-0.6.0/sweeps/sweep-rulebooks.jsonl` (one line per function, fields address, name, file, convention, purpose, evidence, confidence, model).

Method: the clean 3.9.68 `cm0102.exe`, opened with REA (Ghidra) for callers, and a capstone disassembly of every function for the bulk reads. Competition and nation ids at `0x9cf...` are resolved to names with the key id chains in `src/cm3/key_ids_table.c`. "Inference" marks a conclusion that I did not prove. A virtual address equals the file offset + 0x400000 for `.text`.

Note: two other sweep runs (`sweep-t1a`, `sweep-t1b`) wrote names for the same addresses while this sweep ran. Their names agree with this sweep where I read the code. The JSON lines say for each function whether this sweep read the body.

## 1. Answers first

1. **The `rb_*` files are not rule books for competitions.** `rb` is the RulingBody class: the disciplinary body that counts bookings and red cards and gives bans. A RulingBody does not create competitions and holds no club counts, promotion, relegation, play-off, bench or substitute values. A competition points at its ruling body by the byte `comp+0x50`, which the competition's own constructor sets (for example `0x5733d6` eng_prm = 9 England, `0x640721` ita_ser_a = 0x11 Italian league, `0x902355` uefa_cup = 1 Europe). Appendix A lists every `+0x50` store.
2. **The `*_rules` files are the per-nation transfer rules** (class TRANSFER_RULES, base constructor `0x8e84a0`). They hold the transfer windows, the foreign and non-EU signing caps, the loan rules and the match squad restriction filler (rules byte +5 and its neighbours).
3. **Club counts, promotion, relegation, play-off places, bench and substitutes are set in the competition files**, mostly in each competition's `set_up_rules` override and its constructor. This sweep found the field offsets and lists every store in section 5, so that slices 18 to 21 have the sites. The field meanings are proven by the League readers `0x684790` (`+0xbe` with flag `+0xd9` bit 2 = promotion) and `0x6847b0` (`+0xc1` = relegation) and by the 2001/02 values (section 4).
4. **Foreign players:** two separate limits exist. (a) The match squad limit, in the 42-byte restriction record that each nation's filler (rules vtable slot +0x1c) writes; byte +5 is the non-EU limit. (b) The signing cap at TRANSFER_RULES +0x13 (foreign) and +0x14 (non-EU), checked when a club signs a player (`0x8e8560`). The base code also hard-codes Spanish and Turkish caps (section 3.3).
5. **Work permits:** no rule in these files. The game shows "Work Permit" and "does not have a work permit" texts (`0x455a44` club_screens, `0x7272a3` match_screens), so a work permit state exists elsewhere. Where the game decides it: unknown (not traced in this sweep).
6. **Year constants:** only three kinds in these files. Spain's non-EU match squad limit tests the fixture's season year against 2000, 2001 and 2002 (`0x85959f` to `0x8595c1`). England builds two day-of-year anchors with the year 1998 (`0x579c8d`, `0x57a028`), but compares the day of year only. The English ruling body builds its booking cut-off dates from the season year + 1 (`0x7dd06b`, `0x7dd0cf`), with no constant year. No other year test is in these files.
7. **Transfer windows are per nation, in the rules constructors**, as 12-byte records (section 3.1). Denmark (`0x555460`) and Sweden (`0x893d80`) have no window records, so their clubs can sign at any time. The European rules object (`0x583280`) has none either.

## 2. The RulingBody (disciplinary body)

### 2.1 How the ruling bodies are made

- `discipline_rules_init` `0x55d420` (discipline.cpp) allocates a table of 43 pointers and constructs one RulingBody per slot: slot 0 International, 1 Europe, 2 South America, then one per nation (and per league and cup where a nation has two bodies). It builds a nation's body only when the nation byte `+0x11c` is set (the nation is loaded; inference: "playable"). Objects are 0x23 bytes, England 0x2b.
- Base class: `RulingBody::RulingBody` `0x7e7760`, vtable `0x96f104`. Base slots: +0x0 `RulingBody::start_new_season` `0x7e7ed0` (clears each player's booking counts at the start of a season, releases old bans and compacts the entry array), +0x4 `RulingBody::daily_update` `0x7e7d50` (applies each queued ban whose start date has come and posts the ban news), +0x8 process match cards (stub), +0xc stub, +0x10 get_name (assert stub), +0x14 is_one_booking_from_ban (stub), +0x18, +0x1c stubs.
- Object fields (from the 38 constructors and the readers `0x7e85b0`, `0x7e82e0`):
  - +0 vtable, +4 kind (0 nation or league body, 1 cup body, 2 continent, 3 international), +5 body id (the value that `comp+0x50` holds), +6 a small number from 1 to 17 (meaning unknown), +7 pending ban list, +0xb discipline entry array (14 bytes per player: +4 booking count, +8 points word, +0xa and +0xb packed season counts), +0xf and +0x13 counts.
  - +0x17 nation id, +0x1b continent id, +0x1f and +0x20 the league and cup body ids of the same nation (0xff = none), +0x21 parent continental body (1 Europe, 2 South America, 0x23 Oceania, 0x29 Asia, 0xff none), +0x22 flag, +0x23 a Date.
- Booking record that slot +0x8 reads: +0 player id, +0x2c yellow cards in the match, +0x2d red card flag, +0x2e and +0x2f offence codes (9 to 12 grade the red card), +0x48 a flag copied to the ban.

### 2.2 The 38 ruling bodies of this sweep

Every body has a constructor, `process_match_cards` (slot +0x8), `get_name` (slot +0x10) and `is_one_booking_from_ban` (slot +0x14). Northern Ireland and the Scottish league also override slot +0x0 (start of season reset), and the Scottish league slot +0xc (points ban check). `get_name` writes the nation adjective (`0x6181d0`) or a fixed string such as "Italian cup" (`0xa79a34`).

| Ctor | File | Vtable | Kind +4 | Body id +5 | +6 (meaning unknown) | Nation +0x17 | Continent +0x1b | League body +0x1f | Cup body +0x20 | Parent +0x21 | Slot +0x14 warning |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0x7db100 | rb_argentina.cpp | 0x96e9f4 | 0x0 | 0x3 | 10 | Argentina | South America | 0x3 | 0xff | 0x2 | 0x7e4710 |
| 0x7db5b0 | rb_australia.cpp | 0x96ea34 | 0x0 | 0x22 | 10 | Australia | Oceania | 0x22 | 0xff | 0x23 | 0x7db8a0 |
| 0x7db8f0 | rb_belgium_cup.cpp | 0x96ea54 | 0x1 | 0x5 | 5 | Belgium | Europe | 0x4 | 0x5 | 0x1 | 0x7ded50 |
| 0x7dbca0 | rb_belgium_league.cpp | 0x96ea74 | 0x0 | 0x4 | 5 | Belgium | Europe | 0x4 | 0x5 | 0x1 | 0x7ded50 |
| 0x7dc080 | rb_brazil_national.cpp | 0x96ea94 | 0x0 | 0x6 | 5 | Brazil | South America | 0x6 | 0x7 | 0x2 | 0x7ded50 |
| 0x7dc3c0 | rb_brazil_regional.cpp | 0x96eab4 | 0x1 | 0x7 | 5 | Brazil | South America | 0x6 | 0x7 | 0x2 | 0x7ded50 |
| 0x7dc700 | rb_croatia.cpp | 0x96ead4 | 0x0 | 0x21 | 10 | Croatia | Europe | 0x21 | 0xff | 0x1 | 0x7ded50 |
| 0x7dcae0 | rb_denmark.cpp | 0x96eaf4 | 0x0 | 0x8 | 1 | Denmark | Europe | 0x8 | 0xff | 0x1 | 0x7dcf30 |
| 0x7dcf90 | rb_england.cpp | 0x96eb14 | 0x0 | 0x9 | 5 | England | Europe | 0x9 | 0xff | 0x1 | 0x7dd3a0 |
| 0x7dd660 | rb_finland_cup.cpp | 0x96eb54 | 0x1 | 0xb | 7 | Finland | Europe | 0xa | 0xb | 0x1 | 0x7ded50 |
| 0x7dd990 | rb_finland_league.cpp | 0x96eb74 | 0x0 | 0xa | 7 | Finland | Europe | 0xa | 0xb | 0x1 | 0x7ded50 |
| 0x7ddcc0 | rb_france.cpp | 0x96eb94 | 0x0 | 0xc | 5 | France | Europe | 0xc | 0xff | 0x1 | 0x7de260 |
| 0x7de2e0 | rb_germany_cup.cpp | 0x96ebb4 | 0x1 | 0xe | 10 | Germany | Europe | 0xd | 0xe | 0x1 | 0x7e4710 |
| 0x7de620 | rb_germany_league.cpp | 0x96ebd4 | 0x0 | 0xd | 7 | Germany | Europe | 0xd | 0xe | 0x1 | 0x7e4710 |
| 0x7de960 | rb_greece.cpp | 0x96ebf4 | 0x0 | 0x20 | 10 | Greece | Europe | 0x20 | 0xff | 0x1 | 0x7ded50 |
| 0x7ded90 | rb_holland.cpp | 0x96ec14 | 0x0 | 0xf | 17 | Holland | Europe | 0xf | 0xff | 0x1 | 0x7df150 |
| 0x7df480 | rb_ireland.cpp | 0x96ec54 | 0x0 | 0x10 | 10 | Republic Of Ireland | Europe | 0x10 | 0xff | 0x1 | 0x7df910 |
| 0x7df960 | rb_italy_cup.cpp | 0x96ec74 | 0x1 | 0x12 | 8 | Italy | Europe | 0x11 | 0x12 | 0x1 | 0x7dfd30 |
| 0x7dfd70 | rb_italy_league.cpp | 0x96ec94 | 0x0 | 0x11 | 8 | Italy | Europe | 0x11 | 0x12 | 0x1 | 0x7e0160 |
| 0x7e01b0 | rb_japan_cup.cpp | 0x96ecb4 | 0x1 | 0x14 | 10 | Japan | Asia | 0x13 | 0x14 | 0xff | 0x7e0450 |
| 0x7e0490 | rb_japan_league.cpp | 0x96ecd4 | 0x0 | 0x13 | 10 | Japan | Asia | 0x13 | 0x14 | 0xff | 0x7ded50 |
| 0x7e0730 | rb_korea.cpp | 0x96ecf4 | 0x0 | 0x2a | 8 | South Korea | Asia | 0x2a | 0xff | 0x29 | 0x7e0aa0 |
| 0x7e0af0 | rb_northern_ireland.cpp | 0x96ed14 | 0x0 | 0x24 | 5 | Northern Ireland | Europe | 0x24 | 0xff | 0x1 | 0x7e0e20 |
| 0x7e0ef0 | rb_norway_cup.cpp | 0x96ed34 | 0x1 | 0x16 | 10 | Norway | Europe | 0x15 | 0x16 | 0x1 | 0x7e1120 |
| 0x7e1150 | rb_norway_league.cpp | 0x96ed54 | 0x0 | 0x15 | 10 | Norway | Europe | 0x15 | 0x16 | 0x1 | 0x7e1390 |
| 0x7e1610 | rb_poland.cpp | 0x96ed94 | 0x0 | 0x1f | 10 | Poland | Europe | 0x1f | 0xff | 0x1 | 0x7e1a10 |
| 0x7e1a60 | rb_portugal.cpp | 0x96edb4 | 0x0 | 0x17 | 8 | Portugal | Europe | 0x17 | 0xff | 0x1 | 0x7e1db0 |
| 0x7e1e00 | rb_russia.cpp | 0x96edd4 | 0x0 | 0x27 | 8 | Russia | Europe | 0x27 | 0xff | 0x1 | 0x7e2170 |
| 0x7e21a0 | rb_scotland_cup.cpp | 0x96edf4 | 0x1 | 0x19 | 5 | Scotland | Europe | 0x18 | 0x19 | 0x1 | 0x7e2440 |
| 0x7e2470 | rb_scotland_league.cpp | 0x96ee14 | 0x0 | 0x18 | 5 | Scotland | Europe | 0x18 | 0x19 | 0x1 | 0x7e2930 |
| 0x7e2c30 | rb_spain_cup.cpp | 0x96ee54 | 0x1 | 0x1b | 10 | Spain | Europe | 0x1a | 0x1b | 0x1 | 0x7dfd30 |
| 0x7e2f80 | rb_spain_league.cpp | 0x96ee74 | 0x0 | 0x1a | 10 | Spain | Europe | 0x1a | 0x1b | 0x1 | 0x7e4710 |
| 0x7e32d0 | rb_sweden_cup.cpp | 0x96ee94 | 0x1 | 0x1d | 12 | Sweden | Europe | 0x1c | 0x1d | 0x1 | 0x7e39c0 |
| 0x7e3650 | rb_sweden_league.cpp | 0x96eeb4 | 0x0 | 0x1c | 12 | Sweden | Europe | 0x1c | 0x1d | 0x1 | 0x7e39c0 |
| 0x7e3a10 | rb_turkey_cup.cpp | 0x96eed4 | 0x1 | 0x26 | 10 | Turkey | Europe | 0x25 | 0x26 | 0x1 | 0x7e40b0 |
| 0x7e3d60 | rb_turkey_league.cpp | 0x96eef4 | 0x0 | 0x25 | 7 | Turkey | Europe | 0x25 | 0x26 | 0x1 | 0x7e40b0 |
| 0x7e40f0 | rb_usa.cpp | 0x96ef14 | 0x0 | 0x1e | 10 | United States | North America | 0x1e | 0x1e | 0x1e | 0x7e4390 |
| 0x7e43c0 | rb_wales.cpp | 0x96ef34 | 0x0 | 0x28 | 8 | Wales | Europe | 0x28 | 0xff | 0x1 | 0x7e4710 |

Booking rules by body (from slot +0x14 and the compare constants of slot +0x8):

- Every 3rd booking (`0x7ded50`): Belgian cup and league, Brazilian national and regional, Croatia, Finnish cup and league, Greece, Japanese league.
- Every 5th booking (`0x7e4710`): Argentina, German cup and league, Spanish league, Wales.
- Every 2nd booking (`0x7dfd30`, `0x7e0450`): Italian cup, Spanish cup, Japanese cup.
- Every 4th booking (`0x7e40b0`): Turkish cup and league. Russia counts in cycles of four (`0x7e1ea7`).
- 3rd, then every 2nd (`0x7e39c0`): Swedish cup and league. Korea: 3rd and every odd booking (`0x7e08b8`).
- Mixed steps: Italian league 4, 7, then odd from 9 (`0x7dff97`); Poland 4, 7, then even from 10 (`0x7e16c9`); Norwegian league 3, then every booking from 5 (`0x7e11fe`); Norwegian cup every booking from the 2nd (`0x7e0f9e`); Portugal every 3rd in the Portuguese Second Division B, else every 5th (`0x7e1dd0`); Holland 3, then every booking from 5 (inference from `0x7dede0` constants 3 and 4); Ireland 5, 8, 11, 14 (inference from `0x7df4d0` constants).
- England: 5, 10, 15 bookings, with the 5-booking ban only before a cut-off date built from the season year + 1 (`0x7dd0fd` to `0x7dd18e`); ban starts 14 days later (`0x7dd212`). The warning slot ignores the 5th booking between day 100 and day 171 of the year (`0x7dd3d8`, `0x7dd3de`).
- Points systems: Denmark (`0x7dcb30`), Northern Ireland (4 points a booking, bans at 20, 32, 42 and 51 points, `0x7e0bd5` to `0x7e0c30`), Scottish league (2 to 32 points by offence code, a ban per 16 points, `0x7e256c` to `0x7e25af`), USA (bans past 20 points, `0x7e4140`).
- Red cards: a one-match ban, and for an offence code above 9 a points ban (`RulingBody::make_points_ban` `0x7e8e10`) queued from the next Wednesday (Italian bodies `0x7dfed4`, Poland `0x7e17bd`) or the next day.

These rules are not in the 0.6.0 spec. If a later slice changes them, every value above is an immediate in the named function.

## 3. The transfer rules

### 3.1 Objects and windows

- `TRANSFER_MANAGER::setup_transfer_rules` `0x8d26b0` (transfer_manager.cpp) builds the European rules object (`0x583280`, at manager +0x8ac) and one object per nation (manager +0x8b0 onwards): Argentina, Australia, Belgium, Brazil, Croatia, Denmark (`0x555460`), England, Finland, France, Germany, Greece, Holland, Ireland, Italy, Japan, Northern Ireland, Norway, Poland, Portugal, Russia, Scotland, Spain, Sweden (`0x893d80`), Turkey, USA, Wales, Korea. It passes a rules id (0xa Germany to 0x1b Korea) as an argument.
- TRANSFER_RULES fields (`0x8e84a0`): +0 vtable, +4 pointer to the window records, +8 window count, +9 an argument byte, +0xa nation record, +0xe continent record, +0x12 scope (1 nation, 2 continent, 4 none), +0x13 foreign signing cap (default 50), +0x14 non-EU signing cap (default 30), +0x15 dword.
- Vtable slots (13): +0x0 daily update, +0x4 record a transfer, +0x8 record a loan, +0xc can make offer, +0x10 can sign player, +0x14 slot 5 (always true), +0x18 match ineligibility code, +0x1c fill match squad restrictions, +0x20 can loan player, +0x24 can end loan, +0x28 loan period, +0x2c post deadline news, +0x30 write deadline news text.
- Window record: 12 bytes, a start bound and an end bound of 6 bytes each. Bound bytes: +0 rules id, +1 window index, +2 weekday (0 Sunday to 6 Saturday; 0xff none), +3 day, +4 month (0 to 11), +5 1 for the start bound, 0 for the end bound. `transfer_window_get_date` `0x40a7d0` builds `Date(day, month, year)` and moves it to the next given weekday on or after it (`0x54a2d0`). `TRANSFER_RULES::find_open_window` `0x8e8e50` tests each window with `0x8e8c40`; a start month after the end month means the window runs across the new year. With no windows (count 0) every date is open.
- Window index 1 of Argentina is a limited window: a club may make two signings (`0x40a884`, `0x40a995`). France's windows 1 and 2 allow one purchase and one free agent (`0x5c1830`, `0x5c1930`). Italy's window 2 is for free transfers only (`0x661520`, `0x661630`).

The 2001/02 windows (section 5 has every patch byte):

| Rules | Init | Windows (start to end; "next Thu" = moved to the next Thursday on or after) |
|---|---|---|
| Argentina | 0x40a620 | 0: 24 Jul (next Sun) to 6 Aug (next Wed); 1: 14 Dec (next Sun) to 4 Mar (next Wed) |
| England | 0x579b30 | 0: 2 Jun (next Mon) to 26 Mar (next Thu) |
| Australia | 0x412c80 | 0: 11 Jun (next Sat) to 17 Apr |
| Belgium | 0x424c00 | 0: 1 Jun to 30 Jun; 1: 1 Jun to 31 Dec; 2: 1 Jun to 31 Mar |
| Brazil | 0x43f0e0 | 0: 11 Dec (next Wed) to 8 Nov (next Fri) |
| Croatia | 0x518500 | 0: 1 Jun to 30 Mar |
| Finland | 0x5a9950 | 0: 16 Nov to 15 Aug |
| France | 0x5c15e0 | 0: 30 May (next Sat) to 31 Aug (next Wed); 1: 1 Sep (next Thu) to 20 Dec (next Thu); 2: 21 Dec (next Fri) to 10 Jan (next Thu) |
| Germany | 0x5dffe0 | 0: 1 Jul to 15 Aug; 1: 1 Jul to 15 Jan |
| Greece | 0x5ecf20 | 0: 22 May (next Sun) to 29 Aug (next Sat); 1: 1 Jan to 20 Jan |
| Holland | 0x5f9690 | 0: 8 Jun (next Sat) to 4 Apr (next Fri) |
| Ireland | 0x63c210 | 0: 10 May to 31 Jan |
| Italy | 0x6613f0 | 0: 1 Jul to 27 Oct (next Thu); 1: 2 Jan to 31 Jan; 2: 1 Jul to 30 Apr |
| Japan | 0x666c40 | 0: 12 Dec (next Sat) to 4 May; 1: 30 May (next Sat) to 29 Oct |
| Korea | 0x66e630 | 0: 25 Nov (next Sun) to 25 Jul |
| Northern Ireland | 0x7936d0 | 0: 6 May (next Sat) to 20 Mar |
| Norway | 0x7944f0 | 0: 1 Sep to 31 Aug |
| Poland | 0x7cd830 | 0: 1 Jul to 20 Feb |
| Portugal | 0x7d3df0 | 0: 1 Jun to 1 Sep; 1: 15 Dec to 15 Jan |
| Russia | 0x7ec7e0 | 0: 15 Dec to 24 Mar (next Thu); 1: 28 Jun (next Tue) to 4 Aug |
| Scotland | 0x7f6390 | 0: 30 May (next Sat) to 30 Mar (next Thu) |
| Spain | 0x859470 | 0: 17 Jun (next Wed) to 20 Aug (next Thu); 1: 15 Dec to 31 Jan |
| Turkey | 0x9020a0 | 0: 1 Jun to 31 Jan |
| USA | 0x90b940 | 0: 7 Jan (next Wed) to 15 Aug |
| Wales | 0x90f080 | 0: 7 May (next Sun) to 20 Mar |
| Denmark, Sweden, Europe | 0x555460, 0x893d80, 0x583280 | none: always open |

### 3.2 Signing caps (+0x13 foreign, +0x14 non-EU)

`TRANSFER_RULES::can_sign_player` `0x8e8560` (transfer_offer.cpp) counts the buying club's foreign players (`0x53d980`) and non-EU players (foreign and `0x53d740` false) and refuses the signing when the count reaches the cap: code 0x14 for the foreign cap, code 0x15 for the non-EU cap. The non-EU count runs only when the rules object's nation is in Europe. A cap of 0xff turns the check off (`0x8e8614`).

| Rules | Store | Field | Value | Patch byte | Size | Note |
|---|---|---|---|---|---|---|
| Argentina | 0x40a5d6 | +0x13 | 20 | 0x40a5d9 | 1 | |
| Australia | 0x412d89 | +0x13 | 6 | 0x412d56 | 4 | shared: the same `mov ecx, 6` is the window record step at 0x412d72. Patch the store instead. |
| Australia | 0x412d4f | +0x13 | 6 | 0x412d52 | 1 | error path only |
| Belgium | 0x424be8, 0x424beb | +0x13, +0x14 | 0xff (none) | 0x424be7 | 1 | one `or al, 0xff` feeds both |
| Brazil | 0x43f1af, 0x43f1e8 | +0x13 | 3 | 0x43f1b2, 0x43f1b6 | 1 | 0x43f1b6 also feeds window bytes |
| Greece | 0x5ecf06 | +0x14 | 6 | 0x5ecf09 | 1 | |
| Holland | 0x5f979b, 0x5f979e | +0x13, +0x14 | 50 | 0x5f979a | 1 | shared |
| Italy | 0x6613d6 | +0x14 | 40 | 0x6613d9 | 1 | |
| Korea | 0x66e738 | +0x13 | 7 | 0x66e73b | 1 | |
| Poland | 0x7cd93c, 0x7cd93f | +0x13, +0x14 | 5 | 0x7cd93b | 1 | shared |
| Spain | 0x859456 | +0x14 | 6 | 0x859459 | 1 | overridden by 3.3 for Spanish clubs |
| Turkey | 0x9021ab, 0x9021ae | +0x13, +0x14 | 8 | 0x9021aa | 1 | shared |
| USA | 0x90ba48 | +0x13 | 4 | 0x90ba4b | 1 | |
| All others | 0x8e84db.. | +0x13, +0x14 | 50, 30 | 0x8e84e2, 0x8e84e6 (and 0x8e8514, 0x8e8518, 0x8e8533, 0x8e8537) | 1 | base defaults, shared by all nations |

### 3.3 Hard-coded caps in the base check (transfer_offer.cpp)

| Store | Rule | Value | Patch byte | Size |
|---|---|---|---|---|
| 0x8e85b1 | Turkish club outside the Turkish Premier Division: foreign cap | 0 | 0x8e85b5 | 1 |
| 0x8e85d3 | Spanish First Division club: non-EU cap | 4 | 0x8e85d7 | 1 |
| 0x8e85e8 | Spanish Second Division club: non-EU cap | 3 | 0x8e85ec | 1 |
| 0x8e85ef | other Spanish club: non-EU cap | 0 | 0x8e85f3 | 1 |

Nation and division ids: `[0x9cf4dc]` Turkey, `[0x9cf8d8]` turkish premier division, `[0x9cf488]` Spain, `[0x9cf624]` Spanish First Division, `[0x9cf628]` Spanish Second Division.

### 3.4 Match squad restrictions (rules vtable slot +0x1c)

The manager's lookup `0x8cf6b0` calls the filler of the fixture's rules object; the team sheet check `0x8cf880` counts the 11 and the named substitutes. Record bytes: +0..+2 foreign limit, +3..+5 non-EU limit, +0x16 age and +0x17 limit of an age rule, +6 and +8 written by Belgium, Italy and Scotland but not read by the match squad check (meaning unknown). 0xff = no limit. Earlier research: `2026-10-16-foreign-and-tactics-positions.md`.

| Patch address | Filler | What | Value |
|---|---|---|---|
| 0x40aa26 | 0x40aa10 | Argentina match squad foreign limit (+2), all competitions | 4 |
| 0x412dc6 | 0x412db0 | Australia match squad foreign limit (+2), all | 6 |
| 0x424f96 | 0x424f80 | Belgium restriction byte +6 (meaning unknown), all | 21 |
| 0x424f9a | 0x424f80 | Belgium restriction byte +8 (meaning unknown), all | 2 |
| 0x43f2cb | 0x43f2a0 | Brazil match squad foreign limit (+2), all except Reserve | 2 |
| 0x518635 | 0x518620 | Croatia match squad foreign limit (+0), all | 4 |
| 0x579c66 | 0x579c50 | non-EU match squad limit (+5), all; shared by England, Denmark (vtable 0x969860) and Sweden (0x970368) | 3 |
| 0x5c1a56 | 0x5c1a30 | France non-EU limit (+5), French First Division | 5 |
| 0x5c1a5d | 0x5c1a30 | France non-EU limit (+5), other competitions | 3 |
| 0x5e02fb | 0x5e02d0 | Germany non-EU limit (+5), German Cup | 3 |
| 0x5e0302 | 0x5e02d0 | Germany non-EU limit (+5), other competitions | 5 |
| 0x5ed04b | 0x5ed020 | Greece non-EU limit (+5), Greek premier division | 5 |
| 0x5ed052 | 0x5ed020 | Greece non-EU limit (+5), other competitions | 4 |
| 0x661a12 | 0x6619e0 | Italy restriction byte +6, not Serie A or B | 21 |
| 0x661a16 | 0x6619e0 | Italy restriction byte +8, not Serie A or B | 5 |
| 0x661a1d | 0x6619e0 | Italy restriction byte +6, Serie A and B | 21 |
| 0x661a21 | 0x6619e0 | Italy restriction byte +8, Serie A and B | 4 |
| 0x6670a6 | 0x667090 | Japan match squad foreign limit (+2), all | 3 |
| 0x66e765 | 0x66e750 | Korea match squad foreign limit (+0), all | 3 |
| 0x794756 | 0x794740 | Norway non-EU limit (+5), all | 2 |
| 0x7cd976 | 0x7cd960 | Poland match squad foreign limit (+2), all | 5 |
| 0x7d43ed | 0x7d43c0 | Portugal non-EU limit (+3), Portuguese Second Division B | 3 |
| 0x7d43f4 | 0x7d43c0 | Portugal non-EU limit (+3), other competitions | 4 |
| 0x7ec97b | 0x7ec950 | Russia foreign limit (+2), russian first division only | 3 |
| 0x7f64fe | 0x7f64c0 | Scotland age rule age (+0x16), Reserve for a Premier Division club | 21 |
| 0x7f6502 | 0x7f64c0 | Scotland age rule limit (+0x17), same case | 3 |
| 0x7f6517 | 0x7f64c0 | Scotland restriction byte +6, Scottish Premier Division | 21 |
| 0x7f651b | 0x7f64c0 | Scotland restriction byte +8, Scottish Premier Division | 2 |
| 0x8595a8 | 0x859570 | Spain non-EU limit (+5), other competitions, season 2000 | 4 |
| 0x8595bc | 0x859570 | Spain non-EU limit (+5), other competitions, season 2003 on | 2 |
| 0x8595cc | 0x859570 | Spain non-EU limit (+5), First Division or Cup, season 2000 | 4 |
| 0x8595d4 | 0x859570 | Spain non-EU limit (+5), First Division or Cup any later season; other competitions 2001 and 2002 | 3 |
| 0x902206 | 0x9021d0 | Turkey foreign limit (+2), competition [0x9cf8d0] | 0 |
| 0x90221c | 0x9021d0 | Turkey age rule age (+0x16), Reserve | 21 |
| 0x902220 | 0x9021d0 | Turkey age rule limit (+0x17), Reserve | 0 |
| 0x902230 | 0x9021d0 | Turkey foreign limit (+2), Turkish Premier Division and FA Cup | 6 |
| 0x90ba8b | 0x90ba60 | USA foreign limit (+2), all except MLS All-Stars | 4 |

### 3.5 Loans and other per-nation rules (no patch site needed by slices 17 to 21)

England: no loan inside one division, at most five loans a season and two at a time, outside the window refused (`0x579d60`); the season loan count resets on 20 June (`0x57a020`). Ireland: five loans a season, none inside one division (`0x63c3d0`). Spain: no loan between clubs of one division after five league games (`0x859630`). Italy: at most four loan players for clubs of five listed divisions (`0x661770`). Scotland: no loan between two Scottish Premier Division clubs (`0x7f6650`). Cup-tied players: England (FA Cup, League Cup, `0x57a090`), Scotland (Scottish Cup, League Cup, `0x7f65d0`), Ireland and Northern Ireland (three cups each, `0x63c350`, `0x7937f0`). Loan periods (`+0x28`): England, Finland, Northern Ireland, Russia, Scotland, Wales override the base `0x8e8bd0`.

## 4. Competition fields (for slices 17 to 21)

These fields are in the competition objects, not in the rule book files. Field meanings:

| Offset | Size | Meaning | Proof |
|---|---|---|---|
| +0x3e | word | club count (entrants for a cup) | read by `League::update_positions_and_status` `0x684ce9`; eng_prm 20, eng_first 24, eng_conf 22, FA Cup 152 |
| +0x3c | word | times each pair of clubs meets (inference) | 2 in most leagues, 4 in sco_prm and ire_prm, 3 or 4 by branch in ire_first; multiplied with +0x3e at `0x684cfa` |
| +0xbe | byte | direct promotion places | `0x684790`; eng_first 2, eng_third 3, eng_conf 1, top divisions 0 |
| +0xbf | byte | promotion play-off places | eng_first, eng_second, eng_third 4 |
| +0xc0 | byte | relegation play-off places | swe_prm 1 (12th place play-off), ire_prm 1 |
| +0xc1 | byte | direct relegation places | `0x6847b0`; eng_prm 3, eng_second 4, eng_third 1, swe_prm 2 |
| +0x49 | byte | named substitutes | base default 5 at `0x48c5c9` (comp.cpp) |
| +0x4a | byte | substitutes used | base default 3 at `0x48c5cd` |
| +0x50 | byte | ruling body id | Appendix A |

- The English Premier Division sets no +0x49 or +0x4a in `eng_prm::set_up_rules` `0x574b70`. It uses the base defaults 5 named and 3 used from `0x48c5c9` and `0x48c5cd`. Patching those two bytes changes every competition that does not set its own values. A per-season change for the Premier League needs new code in `0x574b70`, not a byte patch.
- Many stores take their value from one register that feeds several fields (marked "shared" in section 5). A one-byte patch of that register load changes all of them. For example `0x572d98 mov cl, 3` sets eng_first relegation (+0xc1) and substitutes used (+0x4a).
- First-season branches change some of these values by year (season change points doc, section 2), for example `fra_first` sets +0x3e to 18 or 20 (`0x5bc2c5`, `0x5bc2cd`) and +0xc1 to 2 or 3 (`0x5bc472`, `0x5bc47a`).

## 5. Patch-site table for slices 17 to 21

One table with every site. "Patch address" is the address of the immediate operand, so a patch writes "Size" bytes there (file offset = address - 0x400000). Rows are in this order: transfer window bytes, signing caps, match squad restriction bytes, year constants, base defaults, then every competition field store of the whole exe (from a scan of all game functions; contract code and an award layout table are left out).

Read the note before a patch. "shared register" means that one register load feeds more than one field, so a byte patch there changes all of them; change the code instead. "outside set_up_rules" means a store in another function of a competition (a constructor, a cup stage, a first-season branch). A value "unknown" comes from a register that the scan could not follow.

Slice column: 17 substitutes, foreign rules and windows; 18 league sizes, promotion and relegation; 19 play-offs and splits; 20 new divisions (club counts of the reused objects); 21 cups (entrant counts). 11 is the season core (year constants).

| Patch address | File | Function | What it sets | Current value | Size | Slice | Note |
|---|---|---|---|---|---|---|---|
| 0x40a6b0 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 0 start, day | 24 | 1 | 17 |  |
| 0x40a6b4 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 0 start, month (0-11) | 6 | 1 | 17 |  |
| 0x40a635 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 0 start, weekday (0 Sun, 0xff none) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x40a6cb | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 0 end, day | 6 | 1 | 17 |  |
| 0x40a6cf | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x40a6bf | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 0 end, weekday (0 Sun, 0xff none) | 3 | 1 | 17 | shared register: patch changes other fields |
| 0x40a6e5 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 1 start, day | 14 | 1 | 17 |  |
| 0x40a6e9 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 1 start, month (0-11) | 11 | 1 | 17 |  |
| 0x40a635 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 1 start, weekday (0 Sun, 0xff none) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x40a702 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 1 end, day | 4 | 1 | 17 |  |
| 0x40a706 | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 1 end, month (0-11) | 2 | 1 | 17 |  |
| 0x40a6bf | argentina_rules.cpp | 0x40a620 init windows | Argentina transfer window 1 end, weekday (0 Sun, 0xff none) | 3 | 1 | 17 | shared register: patch changes other fields |
| 0x579c02 | england_rules.cpp | 0x579b30 init windows | England transfer window 0 start, day | 2 | 1 | 17 | shared register: patch changes other fields |
| 0x579c13 | england_rules.cpp | 0x579b30 init windows | England transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x579c0c | england_rules.cpp | 0x579b30 init windows | England transfer window 0 start, weekday (0 Sun, 0xff none) | 1 | 1 | 17 |  |
| 0x579c2b | england_rules.cpp | 0x579b30 init windows | England transfer window 0 end, day | 26 | 1 | 17 |  |
| 0x579c02 | england_rules.cpp | 0x579b30 init windows | England transfer window 0 end, month (0-11) | 2 | 1 | 17 | shared register: patch changes other fields |
| 0x579c27 | england_rules.cpp | 0x579b30 init windows | England transfer window 0 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 |  |
| 0x412d66 | australia_rules.cpp | 0x412c80 init windows | Australia transfer window 0 start, day | 11 | 1 | 17 |  |
| 0x412d6a | australia_rules.cpp | 0x412c80 init windows | Australia transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x412d56 | australia_rules.cpp | 0x412c80 init windows | Australia transfer window 0 start, weekday (0 Sun, 0xff none) | 6 | 4 | 17 | shared register: patch changes other fields |
| 0x412d81 | australia_rules.cpp | 0x412c80 init windows | Australia transfer window 0 end, day | 17 | 1 | 17 |  |
| 0x412d85 | australia_rules.cpp | 0x412c80 init windows | Australia transfer window 0 end, month (0-11) | 3 | 1 | 17 |  |
| 0x412d7d | australia_rules.cpp | 0x412c80 init windows | Australia transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x424c87 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 0 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x424c97 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x424c85 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x424cae | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 0 end, day | 30 | 1 | 17 |  |
| 0x424cb2 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 0 end, month (0-11) | 5 | 1 | 17 |  |
| 0x424c85 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x424c87 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 1 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x424ccb | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 1 start, month (0-11) | 5 | 1 | 17 |  |
| 0x424c85 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 1 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x424ce2 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 1 end, day | 31 | 1 | 17 |  |
| 0x424ce6 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 1 end, month (0-11) | 11 | 1 | 17 |  |
| 0x424c85 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x424c87 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 2 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x424d00 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 2 start, month (0-11) | 5 | 1 | 17 |  |
| 0x424c85 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 2 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x424d1c | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 2 end, day | 31 | 1 | 17 |  |
| 0x424d20 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 2 end, month (0-11) | 2 | 1 | 17 |  |
| 0x424c85 | belgium_rules.cpp | 0x424c00 init windows | Belgium transfer window 2 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x43f1b8 | brazil_rules.cpp | 0x43f0e0 init windows | Brazil transfer window 0 start, day | 11 | 1 | 17 | shared register: patch changes other fields |
| 0x43f1b8 | brazil_rules.cpp | 0x43f0e0 init windows | Brazil transfer window 0 start, month (0-11) | 11 | 1 | 17 | shared register: patch changes other fields |
| 0x43f1b6 | brazil_rules.cpp | 0x43f0e0 init windows | Brazil transfer window 0 start, weekday (0 Sun, 0xff none) | 3 | 1 | 17 | shared register: patch changes other fields |
| 0x43f1e0 | brazil_rules.cpp | 0x43f0e0 init windows | Brazil transfer window 0 end, day | 8 | 1 | 17 |  |
| 0x43f1e4 | brazil_rules.cpp | 0x43f0e0 init windows | Brazil transfer window 0 end, month (0-11) | 10 | 1 | 17 |  |
| 0x43f1dc | brazil_rules.cpp | 0x43f0e0 init windows | Brazil transfer window 0 end, weekday (0 Sun, 0xff none) | 5 | 1 | 17 |  |
| 0x5185e0 | croatia_rules.cpp | 0x518500 init windows | Croatia transfer window 0 start, day | 1 | 1 | 17 |  |
| 0x5185e4 | croatia_rules.cpp | 0x518500 init windows | Croatia transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x5185d3 | croatia_rules.cpp | 0x518500 init windows | Croatia transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5185fb | croatia_rules.cpp | 0x518500 init windows | Croatia transfer window 0 end, day | 30 | 1 | 17 |  |
| 0x5185ff | croatia_rules.cpp | 0x518500 init windows | Croatia transfer window 0 end, month (0-11) | 2 | 1 | 17 |  |
| 0x5185d3 | croatia_rules.cpp | 0x518500 init windows | Croatia transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5a9a30 | finland_rules.cpp | 0x5a9950 init windows | Finland transfer window 0 start, day | 16 | 1 | 17 |  |
| 0x5a9a34 | finland_rules.cpp | 0x5a9950 init windows | Finland transfer window 0 start, month (0-11) | 10 | 1 | 17 |  |
| 0x5a9a23 | finland_rules.cpp | 0x5a9950 init windows | Finland transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5a9a4b | finland_rules.cpp | 0x5a9950 init windows | Finland transfer window 0 end, day | 15 | 1 | 17 |  |
| 0x5a9a4f | finland_rules.cpp | 0x5a9950 init windows | Finland transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x5a9a23 | finland_rules.cpp | 0x5a9950 init windows | Finland transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5c1674 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 0 start, day | 30 | 1 | 17 |  |
| 0x5c1664 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 0 start, month (0-11) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x5c1670 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 0 start, weekday (0 Sun, 0xff none) | 6 | 1 | 17 |  |
| 0x5c168f | france_rules.cpp | 0x5c15e0 init windows | France transfer window 0 end, day | 31 | 1 | 17 |  |
| 0x5c1693 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x5c168b | france_rules.cpp | 0x5c15e0 init windows | France transfer window 0 end, weekday (0 Sun, 0xff none) | 3 | 1 | 17 |  |
| 0x5c1666 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 1 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x5c16ac | france_rules.cpp | 0x5c15e0 init windows | France transfer window 1 start, month (0-11) | 8 | 1 | 17 |  |
| 0x5c1664 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 1 start, weekday (0 Sun, 0xff none) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x5c16c3 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 1 end, day | 20 | 1 | 17 |  |
| 0x5c16c7 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 1 end, month (0-11) | 11 | 1 | 17 |  |
| 0x5c1664 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 1 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x5c16df | france_rules.cpp | 0x5c15e0 init windows | France transfer window 2 start, day | 21 | 1 | 17 |  |
| 0x5c16e3 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 2 start, month (0-11) | 11 | 1 | 17 |  |
| 0x5c16db | france_rules.cpp | 0x5c15e0 init windows | France transfer window 2 start, weekday (0 Sun, 0xff none) | 5 | 1 | 17 |  |
| 0x5c1701 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 2 end, day | 10 | 1 | 17 |  |
| 0x5c15f5 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 2 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x5c1664 | france_rules.cpp | 0x5c15e0 init windows | France transfer window 2 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x5e0067 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 0 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x5e0077 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 0 start, month (0-11) | 6 | 1 | 17 |  |
| 0x5e0065 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5e008e | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 0 end, day | 15 | 1 | 17 |  |
| 0x5e0092 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x5e0065 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5e0067 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 1 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x5e00ab | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 1 start, month (0-11) | 6 | 1 | 17 |  |
| 0x5e0065 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 1 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5e00c8 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 1 end, day | 15 | 1 | 17 |  |
| 0x5dfff5 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 1 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x5e0065 | germany_rules.cpp | 0x5dffe0 init windows | Germany transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5ecfb1 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 0 start, day | 22 | 1 | 17 |  |
| 0x5ecfb5 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 0 start, month (0-11) | 4 | 1 | 17 |  |
| 0x5ecf35 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 0 start, weekday (0 Sun, 0xff none) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x5ecfcf | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 0 end, day | 29 | 1 | 17 |  |
| 0x5ecfd3 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x5ecfcb | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 0 end, weekday (0 Sun, 0xff none) | 6 | 1 | 17 |  |
| 0x5ecfa4 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 1 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x5ecf35 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 1 start, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x5ecfc1 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 1 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5ed009 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 1 end, day | 20 | 1 | 17 |  |
| 0x5ecf35 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 1 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x5ecfc1 | greece_rules.cpp | 0x5ecf20 init windows | Greece transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x5f9770 | holland_rules.cpp | 0x5f9690 init windows | Holland transfer window 0 start, day | 8 | 1 | 17 |  |
| 0x5f9762 | holland_rules.cpp | 0x5f9690 init windows | Holland transfer window 0 start, month (0-11) | 5 | 1 | 17 | shared register: patch changes other fields |
| 0x5f976c | holland_rules.cpp | 0x5f9690 init windows | Holland transfer window 0 start, weekday (0 Sun, 0xff none) | 6 | 1 | 17 |  |
| 0x5f978a | holland_rules.cpp | 0x5f9690 init windows | Holland transfer window 0 end, day | 4 | 1 | 17 |  |
| 0x5f978e | holland_rules.cpp | 0x5f9690 init windows | Holland transfer window 0 end, month (0-11) | 3 | 1 | 17 |  |
| 0x5f9762 | holland_rules.cpp | 0x5f9690 init windows | Holland transfer window 0 end, weekday (0 Sun, 0xff none) | 5 | 1 | 17 | shared register: patch changes other fields |
| 0x63c2f0 | ireland_rules.cpp | 0x63c210 init windows | Ireland transfer window 0 start, day | 10 | 1 | 17 |  |
| 0x63c2f4 | ireland_rules.cpp | 0x63c210 init windows | Ireland transfer window 0 start, month (0-11) | 4 | 1 | 17 |  |
| 0x63c2e3 | ireland_rules.cpp | 0x63c210 init windows | Ireland transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x63c30b | ireland_rules.cpp | 0x63c210 init windows | Ireland transfer window 0 end, day | 31 | 1 | 17 |  |
| 0x63c262 | ireland_rules.cpp | 0x63c210 init windows | Ireland transfer window 0 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x63c2e3 | ireland_rules.cpp | 0x63c210 init windows | Ireland transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x661477 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 0 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x661487 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 0 start, month (0-11) | 6 | 1 | 17 |  |
| 0x661475 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x66149f | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 0 end, day | 27 | 1 | 17 |  |
| 0x6614a3 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 0 end, month (0-11) | 9 | 1 | 17 |  |
| 0x66149b | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 0 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 |  |
| 0x6614b9 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 1 start, day | 2 | 1 | 17 |  |
| 0x661405 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 1 start, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x661475 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 1 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x6614d3 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 1 end, day | 31 | 1 | 17 |  |
| 0x661405 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 1 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x661475 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x661477 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 2 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x6614f0 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 2 start, month (0-11) | 6 | 1 | 17 |  |
| 0x661475 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 2 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x66150c | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 2 end, day | 30 | 1 | 17 |  |
| 0x661510 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 2 end, month (0-11) | 3 | 1 | 17 |  |
| 0x661475 | italy_rules.cpp | 0x6613f0 init windows | Italy transfer window 2 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x666cd2 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 0 start, day | 12 | 1 | 17 |  |
| 0x666cd6 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 0 start, month (0-11) | 11 | 1 | 17 |  |
| 0x666cce | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 0 start, weekday (0 Sun, 0xff none) | 6 | 1 | 17 |  |
| 0x666ce1 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 0 end, day | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x666ce1 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 0 end, month (0-11) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x666ceb | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x666d09 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 1 start, day | 30 | 1 | 17 |  |
| 0x666ce1 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 1 start, month (0-11) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x666d05 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 1 start, weekday (0 Sun, 0xff none) | 6 | 1 | 17 |  |
| 0x666d27 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 1 end, day | 29 | 1 | 17 |  |
| 0x666d2b | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 1 end, month (0-11) | 9 | 1 | 17 |  |
| 0x666d23 | japan_rules.cpp | 0x666c40 init windows | Japan transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x66e702 | korea_rules.cpp | 0x66e630 init windows | Korea transfer window 0 start, day | 25 | 1 | 17 | shared register: patch changes other fields |
| 0x66e712 | korea_rules.cpp | 0x66e630 init windows | Korea transfer window 0 start, month (0-11) | 10 | 1 | 17 |  |
| 0x66e682 | korea_rules.cpp | 0x66e630 init windows | Korea transfer window 0 start, weekday (0 Sun, 0xff none) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x66e702 | korea_rules.cpp | 0x66e630 init windows | Korea transfer window 0 end, day | 25 | 1 | 17 | shared register: patch changes other fields |
| 0x66e72d | korea_rules.cpp | 0x66e630 init windows | Korea transfer window 0 end, month (0-11) | 6 | 1 | 17 |  |
| 0x66e726 | korea_rules.cpp | 0x66e630 init windows | Korea transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x7937a2 | northern_ireland_rules.cpp | 0x7936d0 init windows | Northern Ireland transfer window 0 start, day | 6 | 4 | 17 | shared register: patch changes other fields |
| 0x7937b5 | northern_ireland_rules.cpp | 0x7936d0 init windows | Northern Ireland transfer window 0 start, month (0-11) | 4 | 1 | 17 |  |
| 0x7937a2 | northern_ireland_rules.cpp | 0x7936d0 init windows | Northern Ireland transfer window 0 start, weekday (0 Sun, 0xff none) | 6 | 4 | 17 | shared register: patch changes other fields |
| 0x7937cc | northern_ireland_rules.cpp | 0x7936d0 init windows | Northern Ireland transfer window 0 end, day | 20 | 1 | 17 |  |
| 0x7937d0 | northern_ireland_rules.cpp | 0x7936d0 init windows | Northern Ireland transfer window 0 end, month (0-11) | 2 | 1 | 17 |  |
| 0x7937c8 | northern_ireland_rules.cpp | 0x7936d0 init windows | Northern Ireland transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x7945d0 | norway_rules.cpp | 0x7944f0 init windows | Norway transfer window 0 start, day | 1 | 1 | 17 |  |
| 0x7945d4 | norway_rules.cpp | 0x7944f0 init windows | Norway transfer window 0 start, month (0-11) | 8 | 1 | 17 |  |
| 0x7945c3 | norway_rules.cpp | 0x7944f0 init windows | Norway transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7945eb | norway_rules.cpp | 0x7944f0 init windows | Norway transfer window 0 end, day | 31 | 1 | 17 |  |
| 0x7945ef | norway_rules.cpp | 0x7944f0 init windows | Norway transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x7945c3 | norway_rules.cpp | 0x7944f0 init windows | Norway transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7cd910 | poland_rules.cpp | 0x7cd830 init windows | Poland transfer window 0 start, day | 1 | 1 | 17 |  |
| 0x7cd914 | poland_rules.cpp | 0x7cd830 init windows | Poland transfer window 0 start, month (0-11) | 6 | 1 | 17 |  |
| 0x7cd903 | poland_rules.cpp | 0x7cd830 init windows | Poland transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7cd92b | poland_rules.cpp | 0x7cd830 init windows | Poland transfer window 0 end, day | 20 | 1 | 17 |  |
| 0x7cd92f | poland_rules.cpp | 0x7cd830 init windows | Poland transfer window 0 end, month (0-11) | 1 | 1 | 17 |  |
| 0x7cd903 | poland_rules.cpp | 0x7cd830 init windows | Poland transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7d3e77 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 0 start, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x7d3e87 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x7d3e75 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7d3e77 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 0 end, day | 1 | 1 | 17 | shared register: patch changes other fields |
| 0x7d3ea1 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 0 end, month (0-11) | 8 | 1 | 17 |  |
| 0x7d3e75 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7d3eb7 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 1 start, day | 15 | 1 | 17 |  |
| 0x7d3ebb | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 1 start, month (0-11) | 11 | 1 | 17 |  |
| 0x7d3e75 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 1 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7d3ed8 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 1 end, day | 15 | 1 | 17 |  |
| 0x7d3e05 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 1 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x7d3e75 | portugal_rules.cpp | 0x7d3df0 init windows | Portugal transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x7ec872 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 0 start, day | 15 | 1 | 17 |  |
| 0x7ec876 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 0 start, month (0-11) | 11 | 1 | 17 |  |
| 0x7ec86e | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x7ec88e | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 0 end, day | 24 | 1 | 17 |  |
| 0x7ec892 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 0 end, month (0-11) | 2 | 1 | 17 |  |
| 0x7ec881 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 0 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x7ec8aa | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 1 start, day | 28 | 1 | 17 |  |
| 0x7ec8ae | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 1 start, month (0-11) | 5 | 1 | 17 |  |
| 0x7ec8a6 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 1 start, weekday (0 Sun, 0xff none) | 2 | 1 | 17 |  |
| 0x7ec881 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 1 end, day | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x7ec8cc | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 1 end, month (0-11) | 7 | 1 | 17 |  |
| 0x7ec8c5 | russia_rules.cpp | 0x7ec7e0 init windows | Russia transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x7f6462 | scotland_rules.cpp | 0x7f6390 init windows | Scotland transfer window 0 start, day | 30 | 1 | 17 | shared register: patch changes other fields |
| 0x7f6464 | scotland_rules.cpp | 0x7f6390 init windows | Scotland transfer window 0 start, month (0-11) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x7f646e | scotland_rules.cpp | 0x7f6390 init windows | Scotland transfer window 0 start, weekday (0 Sun, 0xff none) | 6 | 1 | 17 |  |
| 0x7f6462 | scotland_rules.cpp | 0x7f6390 init windows | Scotland transfer window 0 end, day | 30 | 1 | 17 | shared register: patch changes other fields |
| 0x7f648f | scotland_rules.cpp | 0x7f6390 init windows | Scotland transfer window 0 end, month (0-11) | 2 | 1 | 17 |  |
| 0x7f6464 | scotland_rules.cpp | 0x7f6390 init windows | Scotland transfer window 0 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 | shared register: patch changes other fields |
| 0x859502 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 0 start, day | 17 | 1 | 17 |  |
| 0x859506 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x8594fe | spain_rules.cpp | 0x859470 init windows | Spain transfer window 0 start, weekday (0 Sun, 0xff none) | 3 | 1 | 17 |  |
| 0x859520 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 0 end, day | 20 | 1 | 17 |  |
| 0x859524 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 0 end, month (0-11) | 7 | 1 | 17 |  |
| 0x85951c | spain_rules.cpp | 0x859470 init windows | Spain transfer window 0 end, weekday (0 Sun, 0xff none) | 4 | 1 | 17 |  |
| 0x85953b | spain_rules.cpp | 0x859470 init windows | Spain transfer window 1 start, day | 15 | 1 | 17 |  |
| 0x85953f | spain_rules.cpp | 0x859470 init windows | Spain transfer window 1 start, month (0-11) | 11 | 1 | 17 |  |
| 0x859512 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 1 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x85955c | spain_rules.cpp | 0x859470 init windows | Spain transfer window 1 end, day | 31 | 1 | 17 |  |
| 0x859485 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 1 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x859512 | spain_rules.cpp | 0x859470 init windows | Spain transfer window 1 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x902180 | turkey_rules.cpp | 0x9020a0 init windows | Turkey transfer window 0 start, day | 1 | 1 | 17 |  |
| 0x902184 | turkey_rules.cpp | 0x9020a0 init windows | Turkey transfer window 0 start, month (0-11) | 5 | 1 | 17 |  |
| 0x902173 | turkey_rules.cpp | 0x9020a0 init windows | Turkey transfer window 0 start, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x90219b | turkey_rules.cpp | 0x9020a0 init windows | Turkey transfer window 0 end, day | 31 | 1 | 17 |  |
| 0x9020f2 | turkey_rules.cpp | 0x9020a0 init windows | Turkey transfer window 0 end, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x902173 | turkey_rules.cpp | 0x9020a0 init windows | Turkey transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 | shared register: patch changes other fields |
| 0x90ba12 | usa_rules.cpp | 0x90b940 init windows | USA transfer window 0 start, day | 7 | 1 | 17 | shared register: patch changes other fields |
| 0x90b992 | usa_rules.cpp | 0x90b940 init windows | USA transfer window 0 start, month (0-11) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x90ba1c | usa_rules.cpp | 0x90b940 init windows | USA transfer window 0 start, weekday (0 Sun, 0xff none) | 3 | 1 | 17 |  |
| 0x90ba3a | usa_rules.cpp | 0x90b940 init windows | USA transfer window 0 end, day | 15 | 1 | 17 |  |
| 0x90ba12 | usa_rules.cpp | 0x90b940 init windows | USA transfer window 0 end, month (0-11) | 7 | 1 | 17 | shared register: patch changes other fields |
| 0x90ba36 | usa_rules.cpp | 0x90b940 init windows | USA transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x90f15d | wales_rules.cpp | 0x90f080 init windows | Wales transfer window 0 start, day | 7 | 1 | 17 |  |
| 0x90f161 | wales_rules.cpp | 0x90f080 init windows | Wales transfer window 0 start, month (0-11) | 4 | 1 | 17 |  |
| 0x90f0d2 | wales_rules.cpp | 0x90f080 init windows | Wales transfer window 0 start, weekday (0 Sun, 0xff none) | 0 | 2 | 17 | shared register: patch changes other fields |
| 0x90f17a | wales_rules.cpp | 0x90f080 init windows | Wales transfer window 0 end, day | 20 | 1 | 17 |  |
| 0x90f17e | wales_rules.cpp | 0x90f080 init windows | Wales transfer window 0 end, month (0-11) | 2 | 1 | 17 |  |
| 0x90f176 | wales_rules.cpp | 0x90f080 init windows | Wales transfer window 0 end, weekday (0 Sun, 0xff none) | 255 | 1 | 17 |  |
| 0x40a5d9 | argentina_rules.cpp | 0x40a580 | Argentina foreign signing cap (+0x13) | 20 | 1 | 17 |  |
| 0x412d52 | australia_rules.cpp | 0x412c80 | Australia foreign signing cap (+0x13), error path | 6 | 1 | 17 |  |
| 0x412d56 | australia_rules.cpp | 0x412c80 | Australia foreign signing cap (+0x13), via mov ecx,6 | 6 | 4 | 17 | shared: also the window record step at 0x412d72; patch the store 0x412d89 instead |
| 0x424be7 | belgium_rules.cpp | 0x424b90 | Belgium foreign and non-EU signing caps (+0x13, +0x14) | 0xff (none) | 1 | 17 | shared by both caps |
| 0x43f1b2 | brazil_rules.cpp | 0x43f0e0 | Brazil foreign signing cap (+0x13), error path | 3 | 1 | 17 |  |
| 0x43f1b6 | brazil_rules.cpp | 0x43f0e0 | Brazil foreign signing cap (+0x13) | 3 | 1 | 17 | shared: also window bytes |
| 0x5ecf09 | greece_rules.cpp | 0x5eceb0 | Greece non-EU signing cap (+0x14) | 6 | 1 | 17 |  |
| 0x5f979a | holland_rules.cpp | 0x5f9690 | Holland foreign and non-EU signing caps (+0x13, +0x14) | 50 | 1 | 17 | shared by both caps |
| 0x6613d9 | italy_rules.cpp | 0x661380 | Italy non-EU signing cap (+0x14) | 40 | 1 | 17 |  |
| 0x66e73b | korea_rules.cpp | 0x66e630 | Korea foreign signing cap (+0x13) | 7 | 1 | 17 |  |
| 0x7cd93b | poland_rules.cpp | 0x7cd830 | Poland foreign and non-EU signing caps (+0x13, +0x14) | 5 | 1 | 17 | shared by both caps |
| 0x859459 | spain_rules.cpp | 0x859400 | Spain non-EU signing cap (+0x14) | 6 | 1 | 17 | Spanish clubs use the hard-coded caps at 0x8e85d7..0x8e85f3 |
| 0x9021aa | turkey_rules.cpp | 0x9020a0 | Turkey foreign and non-EU signing caps (+0x13, +0x14) | 8 | 1 | 17 | shared by both caps |
| 0x90ba4b | usa_rules.cpp | 0x90b940 | USA foreign signing cap (+0x13) | 4 | 1 | 17 |  |
| 0x8e84e2 | transfer_offer.cpp | 0x8e84a0 | default foreign signing cap (+0x13), nation scope | 50 | 1 | 17 | base, all nations |
| 0x8e84e6 | transfer_offer.cpp | 0x8e84a0 | default non-EU signing cap (+0x14), nation scope | 30 | 1 | 17 | base, all nations |
| 0x8e8514 | transfer_offer.cpp | 0x8e84a0 | default foreign signing cap (+0x13), continent scope | 50 | 1 | 17 | base |
| 0x8e8518 | transfer_offer.cpp | 0x8e84a0 | default non-EU signing cap (+0x14), continent scope | 30 | 1 | 17 | base |
| 0x8e8533 | transfer_offer.cpp | 0x8e84a0 | default foreign signing cap (+0x13), no scope | 50 | 1 | 17 | base |
| 0x8e8537 | transfer_offer.cpp | 0x8e84a0 | default non-EU signing cap (+0x14), no scope | 30 | 1 | 17 | base |
| 0x8e85b5 | transfer_offer.cpp | 0x8e8560 | foreign cap for Turkish clubs outside the Turkish Premier Division | 0 | 1 | 17 | hard-coded |
| 0x8e85d7 | transfer_offer.cpp | 0x8e8560 | non-EU cap for Spanish First Division clubs | 4 | 1 | 17 | hard-coded |
| 0x8e85ec | transfer_offer.cpp | 0x8e8560 | non-EU cap for Spanish Second Division clubs | 3 | 1 | 17 | hard-coded |
| 0x8e85f3 | transfer_offer.cpp | 0x8e8560 | non-EU cap for other Spanish clubs | 0 | 1 | 17 | hard-coded |
| 0x40aa26 | argentina_rules.cpp | 0x40aa10 | Argentina match squad foreign limit (+2), all competitions | 4 | 1 | 17 |  |
| 0x412dc6 | australia_rules.cpp | 0x412db0 | Australia match squad foreign limit (+2), all | 6 | 1 | 17 |  |
| 0x424f96 | belgium_rules.cpp | 0x424f80 | Belgium restriction byte +6 (meaning unknown), all | 21 | 1 | 17 |  |
| 0x424f9a | belgium_rules.cpp | 0x424f80 | Belgium restriction byte +8 (meaning unknown), all | 2 | 1 | 17 |  |
| 0x43f2cb | brazil_rules.cpp | 0x43f2a0 | Brazil match squad foreign limit (+2), all except Reserve | 2 | 1 | 17 |  |
| 0x518635 | croatia_rules.cpp | 0x518620 | Croatia match squad foreign limit (+0), all | 4 | 1 | 17 |  |
| 0x579c66 | england_rules.cpp | 0x579c50 | non-EU match squad limit (+5), all; shared by England, Denmark (vtable 0x969860) and Sweden (0x970368) | 3 | 1 | 17 |  |
| 0x5c1a56 | france_rules.cpp | 0x5c1a30 | France non-EU limit (+5), French First Division | 5 | 1 | 17 |  |
| 0x5c1a5d | france_rules.cpp | 0x5c1a30 | France non-EU limit (+5), other competitions | 3 | 1 | 17 |  |
| 0x5e02fb | germany_rules.cpp | 0x5e02d0 | Germany non-EU limit (+5), German Cup | 3 | 1 | 17 |  |
| 0x5e0302 | germany_rules.cpp | 0x5e02d0 | Germany non-EU limit (+5), other competitions | 5 | 1 | 17 |  |
| 0x5ed04b | greece_rules.cpp | 0x5ed020 | Greece non-EU limit (+5), Greek premier division | 5 | 1 | 17 |  |
| 0x5ed052 | greece_rules.cpp | 0x5ed020 | Greece non-EU limit (+5), other competitions | 4 | 1 | 17 |  |
| 0x661a12 | italy_rules.cpp | 0x6619e0 | Italy restriction byte +6, not Serie A or B | 21 | 1 | 17 |  |
| 0x661a16 | italy_rules.cpp | 0x6619e0 | Italy restriction byte +8, not Serie A or B | 5 | 1 | 17 |  |
| 0x661a1d | italy_rules.cpp | 0x6619e0 | Italy restriction byte +6, Serie A and B | 21 | 1 | 17 |  |
| 0x661a21 | italy_rules.cpp | 0x6619e0 | Italy restriction byte +8, Serie A and B | 4 | 1 | 17 |  |
| 0x6670a6 | japan_rules.cpp | 0x667090 | Japan match squad foreign limit (+2), all | 3 | 1 | 17 |  |
| 0x66e765 | korea_rules.cpp | 0x66e750 | Korea match squad foreign limit (+0), all | 3 | 1 | 17 |  |
| 0x794756 | norway_rules.cpp | 0x794740 | Norway non-EU limit (+5), all | 2 | 1 | 17 |  |
| 0x7cd976 | poland_rules.cpp | 0x7cd960 | Poland match squad foreign limit (+2), all | 5 | 1 | 17 |  |
| 0x7d43ed | portugal_rules.cpp | 0x7d43c0 | Portugal non-EU limit (+3), Portuguese Second Division B | 3 | 1 | 17 |  |
| 0x7d43f4 | portugal_rules.cpp | 0x7d43c0 | Portugal non-EU limit (+3), other competitions | 4 | 1 | 17 |  |
| 0x7ec97b | russia_rules.cpp | 0x7ec950 | Russia foreign limit (+2), russian first division only | 3 | 1 | 17 |  |
| 0x7f64fe | scotland_rules.cpp | 0x7f64c0 | Scotland age rule age (+0x16), Reserve for a Premier Division club | 21 | 1 | 17 |  |
| 0x7f6502 | scotland_rules.cpp | 0x7f64c0 | Scotland age rule limit (+0x17), same case | 3 | 1 | 17 |  |
| 0x7f6517 | scotland_rules.cpp | 0x7f64c0 | Scotland restriction byte +6, Scottish Premier Division | 21 | 1 | 17 |  |
| 0x7f651b | scotland_rules.cpp | 0x7f64c0 | Scotland restriction byte +8, Scottish Premier Division | 2 | 1 | 17 |  |
| 0x8595a8 | spain_rules.cpp | 0x859570 | Spain non-EU limit (+5), other competitions, season 2000 | 4 | 1 | 17 |  |
| 0x8595bc | spain_rules.cpp | 0x859570 | Spain non-EU limit (+5), other competitions, season 2003 on | 2 | 1 | 17 |  |
| 0x8595cc | spain_rules.cpp | 0x859570 | Spain non-EU limit (+5), First Division or Cup, season 2000 | 4 | 1 | 17 |  |
| 0x8595d4 | spain_rules.cpp | 0x859570 | Spain non-EU limit (+5), First Division or Cup any later season; other competitions 2001 and 2002 | 3 | 1 | 17 |  |
| 0x902206 | turkey_rules.cpp | 0x9021d0 | Turkey foreign limit (+2), competition [0x9cf8d0] | 0 | 1 | 17 |  |
| 0x90221c | turkey_rules.cpp | 0x9021d0 | Turkey age rule age (+0x16), Reserve | 21 | 1 | 17 |  |
| 0x902220 | turkey_rules.cpp | 0x9021d0 | Turkey age rule limit (+0x17), Reserve | 0 | 1 | 17 |  |
| 0x902230 | turkey_rules.cpp | 0x9021d0 | Turkey foreign limit (+2), Turkish Premier Division and FA Cup | 6 | 1 | 17 |  |
| 0x90ba8b | usa_rules.cpp | 0x90ba60 | USA foreign limit (+2), all except MLS All-Stars | 4 | 1 | 17 |  |
| 0x8595a1 | spain_rules.cpp | 0x859570 | year test: season year == 2000 (other competitions) | 0x7d0 (2000) | 2 | 11, 17 |  |
| 0x8595af | spain_rules.cpp | 0x859570 | year test: season year == 2001 | 0x7d1 (2001) | 2 | 11, 17 |  |
| 0x8595b5 | spain_rules.cpp | 0x859570 | year test: season year == 2002 | 0x7d2 (2002) | 2 | 11, 17 |  |
| 0x8595c5 | spain_rules.cpp | 0x859570 | year test: season year == 2000 (First Division and Cup) | 0x7d0 (2000) | 2 | 11, 17 |  |
| 0x579c8e | england_rules.cpp | 0x579c70 | loan period anchor 19 June, year only builds the Date (day of year compared) | 0x7ce (1998) | 4 | 11, 17 |  |
| 0x57a029 | england_rules.cpp | 0x57a020 | loan count reset anchor 20 June, year only builds the Date | 0x7ce (1998) | 4 | 11, 17 |  |
| 0x48c5cc | comp.cpp | Comp::Comp | default named substitutes (+0x49) for every competition that sets none, the Premier Division among them | 5 | 1 | 17 | base default |
| 0x48c5d0 | comp.cpp | Comp::Comp | default substitutes used (+0x4a) | 3 | 1 | 17 | base default |
| 0x4019d2 | african_nations.cpp | AfricanNations::init_group_a_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x401acd | african_nations.cpp | AfricanNations::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x401ad1 | african_nations.cpp | AfricanNations::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x401b05 | african_nations.cpp | AfricanNations::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x401ace | african_nations.cpp | AfricanNations::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x401ace | african_nations.cpp | AfricanNations::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x401ace | african_nations.cpp | AfricanNations::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x401b31 | african_nations.cpp | AfricanNations::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x405556 | arg_prm.cpp | ArgPrm::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x405675 | arg_prm.cpp | ArgPrm::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 |  |
| 0x40567d | arg_prm.cpp | ArgPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x40567d | arg_prm.cpp | ArgPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x40567d | arg_prm.cpp | ArgPrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x40567d | arg_prm.cpp | ArgPrm::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x40567a | arg_prm.cpp | ArgPrm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x40570b | arg_prm.cpp | ArgPrm::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x407ce6 | arg_second.cpp | ArgSecond::fill_league_table | club count (+0x3e) | 25 | 2 | 18, 20 |  |
| 0x407e55 | arg_second.cpp | ArgSecond::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 | shared register |
| 0x407e55 | arg_second.cpp | ArgSecond::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x407e60 | arg_second.cpp | ArgSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x407e60 | arg_second.cpp | ArgSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x407e60 | arg_second.cpp | ArgSecond::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x407e5a | arg_second.cpp | ArgSecond::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 |  |
| 0x407ee8 | arg_second.cpp | ArgSecond::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x40ab6c | asia_club_champ.cpp | AsiaClubChamp::AsiaClubChamp | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x40ab70 | asia_club_champ.cpp | AsiaClubChamp::AsiaClubChamp | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x40b630 | asia_club_champ.cpp | AsiaClubChamp::build_team_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x40cc6c | asia_cup_winner.cpp | AsiaCupWinner::AsiaCupWinner | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x40cc70 | asia_cup_winner.cpp | AsiaCupWinner::AsiaCupWinner | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x40d4e0 | asia_cup_winner.cpp | AsiaCupWinner::create_team_entries | club count (+0x3e) | 16 | 2 | 21 |  |
| 0x40f3e2 | asia_nations.cpp | AsiaNations::create_group_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x40f4dd | asia_nations.cpp | AsiaNations::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x40f4df | asia_nations.cpp | AsiaNations::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x40f513 | asia_nations.cpp | AsiaNations::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x40f502 | asia_nations.cpp | AsiaNations::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x40f502 | asia_nations.cpp | AsiaNations::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x40f502 | asia_nations.cpp | AsiaNations::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x40f53f | asia_nations.cpp | AsiaNations::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x4106bc | asia_super_cup.cpp | AsiaSuperCup::AsiaSuperCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x4106c0 | asia_super_cup.cpp | AsiaSuperCup::AsiaSuperCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x410a60 | asia_super_cup.cpp | AsiaSuperCup::select_teams | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x411b86 | aus_nsl.cpp | AusNsl::create_table | club count (+0x3e) | 13 | 2 | 18, 20 |  |
| 0x411ce3 | aus_nsl.cpp | AusNsl::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x411ce3 | aus_nsl.cpp | AusNsl::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x411ce3 | aus_nsl.cpp | AusNsl::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x411d08 | aus_nsl.cpp | AusNsl::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x411d08 | aus_nsl.cpp | AusNsl::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x411d58 | aus_nsl.cpp | AusNsl::set_up_rules | direct promotion places (+0xbe) | 6 | 1 | 18 |  |
| 0x411d73 | aus_nsl.cpp | AusNsl::set_up_rules | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x41d091 | bel_fa_cup.cpp | BelFaCup::BelFaCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x41d0c1 | bel_fa_cup.cpp | BelFaCup::BelFaCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x41d8e5 | bel_fa_cup.cpp | BelFaCup::collect_entrants | club count (+0x3e) | 112 | 2 | 21 |  |
| 0x41dd94 | bel_first.cpp | BelFirst::BelFirst | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x41dd98 | bel_first.cpp | BelFirst::BelFirst | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x41e8c6 | bel_first.cpp | BelFirst::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x41e9e4 | bel_first.cpp | BelFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x41e9ea | bel_first.cpp | BelFirst::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x41e9ea | bel_first.cpp | BelFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x41e9ea | bel_first.cpp | BelFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x41e9e4 | bel_first.cpp | BelFirst::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x41f084 | bel_second.cpp | BelSecond::BelSecond | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x41f088 | bel_second.cpp | BelSecond::BelSecond | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x41fd86 | bel_second.cpp | BelSecond::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x41fea7 | bel_second.cpp | BelSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x41feac | bel_second.cpp | BelSecond::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x41febc | bel_second.cpp | BelSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 |  |
| 0x41feac | bel_second.cpp | BelSecond::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register |
| 0x41fea7 | bel_second.cpp | BelSecond::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x420f31 | bel_super.cpp | BelSuper::BelSuper | named substitutes (+0x49) | 7 | 1 | 17 | shared register |
| 0x420f31 | bel_super.cpp | BelSuper::BelSuper | substitutes used (+0x4a) | 7 | 1 | 17 | shared register |
| 0x421300 | bel_super.cpp | BelSuper::select_teams | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x4224f6 | bel_third.cpp | BelThird::fill_league_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x4226f7 | bel_third.cpp | BelThird::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x4226fc | bel_third.cpp | BelThird::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x422700 | bel_third.cpp | BelThird::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 |  |
| 0x4226fc | bel_third.cpp | BelThird::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register |
| 0x4226f7 | bel_third.cpp | BelThird::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x4226f5 | bel_third.cpp | BelThird::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x422797 | bel_third.cpp | BelThird::set_up_rules | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x4259f4 | bra_champ_cup.cpp | BraChampCup::BraChampCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x4259f8 | bra_champ_cup.cpp | BraChampCup::BraChampCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x4259d7 | bra_champ_cup.cpp | BraChampCup::BraChampCup | club count (+0x3e) | 0 | 2 | 21 |  |
| 0x426142 | bra_champ_cup.cpp | BraChampCup::create_preliminary_table | club count (+0x3e) | 3 | 2 | 21 |  |
| 0x42624b | bra_champ_cup.cpp | BraChampCup::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x42625a | bra_champ_cup.cpp | BraChampCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x42625a | bra_champ_cup.cpp | BraChampCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x42625a | bra_champ_cup.cpp | BraChampCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x42623a | bra_champ_cup.cpp | BraChampCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 |  |
| 0x4263a3 | bra_champ_cup.cpp | BraChampCup::start_new_season | club count (+0x3e) | unknown | ? | 21 | store 0x4263a3: value from a register, read the code |
| 0x427504 | bra_cup.cpp | BraCup::BraCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x427508 | bra_cup.cpp | BraCup::BraCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x427c32 | bra_cup.cpp | BraCup::build_team_table | club count (+0x3e) | 71 | 2 | 21 |  |
| 0x427ed6 | bra_cup.cpp | BraCup::set_up_rounds | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x427eda | bra_cup.cpp | BraCup::set_up_rounds | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x42a496 | bra_nat_first.cpp | BraNatFirst::create_table | club count (+0x3e) | 28 | 2 | 18, 20 |  |
| 0x42a5a5 | bra_nat_first.cpp | BraNatFirst::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 |  |
| 0x42a5ae | bra_nat_first.cpp | BraNatFirst::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 | shared register |
| 0x42a5ab | bra_nat_first.cpp | BraNatFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x42a5ab | bra_nat_first.cpp | BraNatFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x42a5ae | bra_nat_first.cpp | BraNatFirst::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 | shared register |
| 0x42a5aa | bra_nat_first.cpp | BraNatFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x42a628 | bra_nat_first.cpp | BraNatFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x42ca86 | bra_nat_second.cpp | BraNatSecond::create_table | club count (+0x3e) | 28 | 2 | 18, 20 |  |
| 0x42cb75 | bra_nat_second.cpp | BraNatSecond::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 |  |
| 0x42cbb4 | bra_nat_second.cpp | BraNatSecond::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 |  |
| 0x42cb80 | bra_nat_second.cpp | BraNatSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x42cb80 | bra_nat_second.cpp | BraNatSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x42cbc7 | bra_nat_second.cpp | BraNatSecond::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x42cb7a | bra_nat_second.cpp | BraNatSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x42cbfc | bra_nat_second.cpp | BraNatSecond::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x42dca6 | bra_nat_third.cpp | BraNatThird::create_table | club count (+0x3e) | 7 | 2 | 18, 20 |  |
| 0x42de54 | bra_nat_third.cpp | BraNatThird::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x42de54 | bra_nat_third.cpp | BraNatThird::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x42de5a | bra_nat_third.cpp | BraNatThird::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x42de5a | bra_nat_third.cpp | BraNatThird::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x42de54 | bra_nat_third.cpp | BraNatThird::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x42de59 | bra_nat_third.cpp | BraNatThird::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x42ded8 | bra_nat_third.cpp | BraNatThird::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x42fd26 | bra_reg_bahia.cpp | BraRegBahia::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x430f06 | bra_reg_central.cpp | BraRegCentral::load_region_clubs | club count (+0x3e) | 15 | 2 | 18, 20 |  |
| 0x430ff4 | bra_reg_central.cpp | BraRegCentral::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x430ff4 | bra_reg_central.cpp | BraRegCentral::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x430ff7 | bra_reg_central.cpp | BraRegCentral::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x430ff7 | bra_reg_central.cpp | BraRegCentral::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x431026 | bra_reg_central.cpp | BraRegCentral::set_up_rules | direct promotion places (+0xbe) | 8 | 4 | 18 |  |
| 0x43104d | bra_reg_central.cpp | BraRegCentral::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 |  |
| 0x431081 | bra_reg_central.cpp | BraRegCentral::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x4321e6 | bra_reg_gaucho.cpp | BraRegGaucho::load_region_clubs | club count (+0x3e) | 17 | 2 | 18, 20 |  |
| 0x4322d4 | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x4322d4 | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x4322d8 | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x4322dc | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | direct relegation places (+0xc1) | 2 | 1 | 18 |  |
| 0x432327 | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 |  |
| 0x432348 | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 |  |
| 0x432352 | bra_reg_gaucho.cpp | BraRegGaucho::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x433556 | bra_reg_goias.cpp | BraRegGoias::create_table | club count (+0x3e) | 14 | 2 | 18, 20 |  |
| 0x433644 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x433644 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x433647 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x433676 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | direct promotion places (+0xbe) | 8 | 4 | 18 |  |
| 0x433697 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 19, 20 |  |
| 0x4336c0 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x4336d2 | bra_reg_goias.cpp | BraRegGoias::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x434826 | bra_reg_minas_gerais.cpp | BraRegMinasGerais::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x435d96 | bra_reg_north.cpp | BraRegNorth::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x436da6 | bra_reg_northeast.cpp | BraRegNortheast::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x437ed6 | bra_reg_parana.cpp | BraRegParana::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x437fc4 | bra_reg_parana.cpp | BraRegParana::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x437fd5 | bra_reg_parana.cpp | BraRegParana::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x437fd5 | bra_reg_parana.cpp | BraRegParana::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x437fd5 | bra_reg_parana.cpp | BraRegParana::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x437fd8 | bra_reg_parana.cpp | BraRegParana::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x438038 | bra_reg_parana.cpp | BraRegParana::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x438042 | bra_reg_parana.cpp | BraRegParana::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x439346 | bra_reg_pern.cpp | BraRegPern::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x439433 | bra_reg_pern.cpp | BraRegPern::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x439433 | bra_reg_pern.cpp | BraRegPern::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x439439 | bra_reg_pern.cpp | BraRegPern::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x43943b | bra_reg_pern.cpp | BraRegPern::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x43943b | bra_reg_pern.cpp | BraRegPern::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x4394a8 | bra_reg_pern.cpp | BraRegPern::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 |  |
| 0x4394b2 | bra_reg_pern.cpp | BraRegPern::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x43a516 | bra_reg_rio.cpp | BraRegRio::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x43a603 | bra_reg_rio.cpp | BraRegRio::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x43a603 | bra_reg_rio.cpp | BraRegRio::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x43a603 | bra_reg_rio.cpp | BraRegRio::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x43a609 | bra_reg_rio.cpp | BraRegRio::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x43a60b | bra_reg_rio.cpp | BraRegRio::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x43a60b | bra_reg_rio.cpp | BraRegRio::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x43a681 | bra_reg_rio.cpp | BraRegRio::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x43bb86 | bra_reg_santa.cpp | BraRegSanta::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x43bc75 | bra_reg_santa.cpp | BraRegSanta::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x43bc88 | bra_reg_santa.cpp | BraRegSanta::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x43bc88 | bra_reg_santa.cpp | BraRegSanta::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x43bc7b | bra_reg_santa.cpp | BraRegSanta::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x43bce5 | bra_reg_santa.cpp | BraRegSanta::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 |  |
| 0x43bcc8 | bra_reg_santa.cpp | BraRegSanta::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 |  |
| 0x43bcf5 | bra_reg_santa.cpp | BraRegSanta::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x43cc56 | bra_reg_sp.cpp | BraRegSp::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x4c0cc8 | con_champ.cpp | ConChamp::ConChamp | club count (+0x3e) | 8 | 2 | 18, 20 |  |
| 0x4c11b3 | con_champ.cpp | ConChamp::select_qualified_clubs | club count (+0x3e) | 8 | 2 | 18, 20 |  |
| 0x4c1727 | con_merc_cup.cpp | ConMercCup::ConMercCup | club count (+0x3e) | 0 | 2 | 21 |  |
| 0x4c1df2 | con_merc_cup.cpp | ConMercCup::create_first_group_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x4c1eed | con_merc_cup.cpp | ConMercCup::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 21 |  |
| 0x4c1f38 | con_merc_cup.cpp | ConMercCup::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x4c1f13 | con_merc_cup.cpp | ConMercCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x4c1f13 | con_merc_cup.cpp | ConMercCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x4c1f13 | con_merc_cup.cpp | ConMercCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x4c2f87 | conmebol_liber.cpp | ConmebolLiber::ConmebolLiber | club count (+0x3e) | 0 | 2 | 18, 20 |  |
| 0x4c37b2 | conmebol_liber.cpp | ConmebolLiber::build_first_group_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x4c38ad | conmebol_liber.cpp | ConmebolLiber::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x4c38ad | conmebol_liber.cpp | ConmebolLiber::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x4c38b2 | conmebol_liber.cpp | ConmebolLiber::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x4c38b3 | conmebol_liber.cpp | ConmebolLiber::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x4c38b3 | conmebol_liber.cpp | ConmebolLiber::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x4c38b3 | conmebol_liber.cpp | ConmebolLiber::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x4c38d6 | conmebol_liber.cpp | ConmebolLiber::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x4c46f7 | conmebol_merc.cpp | ConmebolMerc::ConmebolMerc | club count (+0x3e) | 0 | 2 | 18, 20 |  |
| 0x4c4e72 | conmebol_merc.cpp | ConmebolMerc::create_first_group_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x4c4f6d | conmebol_merc.cpp | ConmebolMerc::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x4c4f6f | conmebol_merc.cpp | ConmebolMerc::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x4c4fbc | conmebol_merc.cpp | ConmebolMerc::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x4c4f95 | conmebol_merc.cpp | ConmebolMerc::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x4c4f95 | conmebol_merc.cpp | ConmebolMerc::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x4c4f95 | conmebol_merc.cpp | ConmebolMerc::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x4c4f8e | conmebol_merc.cpp | ConmebolMerc::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x4c564f | conmebol_merc.cpp | conmebol_merc_create_quarter_final_stage_mid | direct promotion places (+0xbe) | unknown | ? | 18 | store 0x4c564f: value from a register, read the code; outside set_up_rules |
| 0x5147c6 | cro_a1.cpp | CroA1::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x5148e7 | cro_a1.cpp | CroA1::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5148f0 | cro_a1.cpp | CroA1::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x5148f0 | cro_a1.cpp | CroA1::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5148f0 | cro_a1.cpp | CroA1::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5148e7 | cro_a1.cpp | CroA1::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x5148e5 | cro_a1.cpp | CroA1::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x51497d | cro_a1.cpp | CroA1::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x515cc6 | cro_a2a.cpp | CroA2a::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x515de7 | cro_a2a.cpp | CroA2a::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x515df0 | cro_a2a.cpp | CroA2a::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x515df0 | cro_a2a.cpp | CroA2a::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x515df0 | cro_a2a.cpp | CroA2a::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x515de7 | cro_a2a.cpp | CroA2a::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x515de5 | cro_a2a.cpp | CroA2a::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x515e7f | cro_a2a.cpp | CroA2a::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x516de6 | cro_a2b.cpp | CroA2b::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x516f07 | cro_a2b.cpp | CroA2b::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x516f10 | cro_a2b.cpp | CroA2b::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x516f10 | cro_a2b.cpp | CroA2b::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x516f10 | cro_a2b.cpp | CroA2b::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x516f07 | cro_a2b.cpp | CroA2b::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x516f05 | cro_a2b.cpp | CroA2b::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x516f9d | cro_a2b.cpp | CroA2b::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5174ec | cro_cup.cpp | CroCup::CroCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5174f0 | cro_cup.cpp | CroCup::CroCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x517bc9 | cro_cup.cpp | CroCup::collect_entrants | club count (+0x3e) | 48 | 2 | 21 |  |
| 0x522e9d | cup_stage.cpp | Cup_Stage::Cup_Stage | meetings per pair (+0x3c) | unknown | ? | 21 | store 0x522e9d: value from a register, read the code; outside set_up_rules |
| 0x522eb2 | cup_stage.cpp | Cup_Stage::Cup_Stage | named substitutes (+0x49) | unknown | ? | 17 | store 0x522eb2: value from a register, read the code |
| 0x522ebf | cup_stage.cpp | Cup_Stage::Cup_Stage | substitutes used (+0x4a) | unknown | ? | 17 | store 0x522ebf: value from a register, read the code |
| 0x522f38 | cup_stage.cpp | Cup_Stage::Cup_Stage | club count (+0x3e) | unknown | ? | 21 | store 0x522f38: value from a register, read the code |
| 0x550a8f | den_cup.cpp | DenCup::DenCup | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x550a93 | den_cup.cpp | DenCup::DenCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x551258 | den_cup.cpp | DenCup::collect_entrants | club count (+0x3e) | 92 | 2 | 21 |  |
| 0x5516d6 | den_cup.cpp|den_first.cpp | DenCup::set_up_rounds | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x5516da | den_cup.cpp|den_first.cpp | DenCup::set_up_rounds | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5522a6 | den_first.cpp | DenFirst::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x5523b4 | den_first.cpp | DenFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5523b4 | den_first.cpp | DenFirst::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x5523ba | den_first.cpp | DenFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5523ba | den_first.cpp | DenFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5523b9 | den_first.cpp | DenFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x5523b9 | den_first.cpp | DenFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x55243a | den_first.cpp | DenFirst::set_up_rules | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x5538e6 | den_prm.cpp | DenPrm::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x553a04 | den_prm.cpp | DenPrm::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x553a0a | den_prm.cpp | DenPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x553a0a | den_prm.cpp | DenPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x553a0a | den_prm.cpp | DenPrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x553a09 | den_prm.cpp | DenPrm::set_up_rules | direct relegation places (+0xc1) | 2 | 1 | 18 |  |
| 0x553a04 | den_prm.cpp | DenPrm::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x553a8c | den_prm.cpp | DenPrm::set_up_rules | named substitutes (+0x49) | 4 | 1 | 17 |  |
| 0x554a56 | den_second.cpp | DenSecond::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x554b64 | den_second.cpp | DenSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x554b69 | den_second.cpp | DenSecond::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x554b7a | den_second.cpp | DenSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x554b7a | den_second.cpp | DenSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x554b7d | den_second.cpp | DenSecond::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 | shared register |
| 0x554b69 | den_second.cpp | DenSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x554b7d | den_second.cpp | DenSecond::set_up_rules | named substitutes (+0x49) | 4 | 1 | 17 | shared register |
| 0x56aab8 | eng_auto_cup.cpp | EngAutoCup::EngAutoCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x56aabc | eng_auto_cup.cpp | EngAutoCup::EngAutoCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x56b3d3 | eng_auto_cup.cpp | EngAutoCup::collect_entrants | club count (+0x3e) | 28 | 2 | 21 |  |
| 0x56c331 | eng_cc_cup.cpp | EngCcCup::EngCcCup | club count (+0x3e) | 92 | 2 | 21 |  |
| 0x56ccd8 | eng_cc_cup.cpp | EngCcCup::collect_entrants | club count (+0x3e) | 92 | 2 | 21 |  |
| 0x56d3ec | eng_charity.cpp | EngCharity::EngCharity | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x56d3f0 | eng_charity.cpp | EngCharity::EngCharity | substitutes used (+0x4a) | 5 | 1 | 17 |  |
| 0x56d790 | eng_charity.cpp | EngCharity::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x56ecc6 | eng_conf.cpp | EngConf::create_table | club count (+0x3e) | 22 | 2 | 18, 20 |  |
| 0x56ede7 | eng_conf.cpp | EngConf::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x56edec | eng_conf.cpp | EngConf::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 |  |
| 0x56ee00 | eng_conf.cpp | EngConf::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x56ee00 | eng_conf.cpp | EngConf::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x56ede5 | eng_conf.cpp | EngConf::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x56ede5 | eng_conf.cpp | EngConf::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x56ee5d | eng_conf.cpp | EngConf::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x56f0e1 | eng_fa_cup.cpp | EngFaCup::EngFaCup | club count (+0x3e) | 152 | 2 | 21 |  |
| 0x56fbc5 | eng_fa_cup.cpp | EngFaCup::collect_entrants | club count (+0x3e) | 152 | 2 | 21 |  |
| 0x570c41 | eng_fa_trophy.cpp | EngFaTrophy::EngFaTrophy | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x570c41 | eng_fa_trophy.cpp | EngFaTrophy::EngFaTrophy | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5712c5 | eng_fa_trophy.cpp | EngFaTrophy::select_teams | club count (+0x3e) | 32 | 2 | 18, 20 |  |
| 0x572c05 | eng_first.cpp | EngFirst::create_table | club count (+0x3e) | 24 | 2 | 18, 20 |  |
| 0x572d94 | eng_first.cpp | EngFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x572d94 | eng_first.cpp | EngFirst::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x572dd4 | eng_first.cpp | EngFirst::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 |  |
| 0x572ddb | eng_first.cpp | EngFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x572d99 | eng_first.cpp | EngFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x572d99 | eng_first.cpp | EngFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x572e10 | eng_first.cpp | EngFirst::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x574a36 | eng_prm.cpp | EngPrm::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x574b74 | eng_prm.cpp | EngPrm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x574b7a | eng_prm.cpp | EngPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x574b7a | eng_prm.cpp | EngPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x574b7a | eng_prm.cpp | EngPrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x574b79 | eng_prm.cpp | EngPrm::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 |  |
| 0x576646 | eng_second.cpp | EngSecond::create_table | club count (+0x3e) | 24 | 2 | 18, 20 |  |
| 0x576784 | eng_second.cpp | EngSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x576784 | eng_second.cpp | EngSecond::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x57678b | eng_second.cpp | EngSecond::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x5767cc | eng_second.cpp | EngSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x57678b | eng_second.cpp | EngSecond::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 | shared register |
| 0x576789 | eng_second.cpp | EngSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x576801 | eng_second.cpp | EngSecond::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x577f86 | eng_third.cpp | EngThird::create_table | club count (+0x3e) | 24 | 2 | 18, 20 |  |
| 0x5780c7 | eng_third.cpp | EngThird::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x5780c5 | eng_third.cpp | EngThird::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x5780d1 | eng_third.cpp | EngThird::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 |  |
| 0x578110 | eng_third.cpp | EngThird::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x5780cc | eng_third.cpp | EngThird::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 |  |
| 0x5780c5 | eng_third.cpp | EngThird::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x57815c | eng_third.cpp | EngThird::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x57a171 | eur_super_cup.cpp | EurSuperCup::EurSuperCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x57a141 | eur_super_cup.cpp | EurSuperCup::EurSuperCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x57a510 | eur_super_cup.cpp | EurSuperCup::select_teams | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x57aa7d | euro_champ.cpp | EuroChamp::EuroChamp | club count (+0x3e) | unknown | ? | 18, 20 | store 0x57aa7d: value from a register, read the code |
| 0x57b543 | euro_champ.cpp | EuroChamp::create_group_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x57b64d | euro_champ.cpp | EuroChamp::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x57b650 | euro_champ.cpp | EuroChamp::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x57b680 | euro_champ.cpp | EuroChamp::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x57b664 | euro_champ.cpp | EuroChamp::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x57b664 | euro_champ.cpp | EuroChamp::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x57b664 | euro_champ.cpp | EuroChamp::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x57b6b8 | euro_champ.cpp | EuroChamp::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x57b75a | euro_champ.cpp | EuroChamp::start_new_season | club count (+0x3e) | 0 | 2 | 18, 20 |  |
| 0x57e7f0 | euro_champ_qual.cpp | EuroChampQual::init_group_one_table | club count (+0x3e) | unknown | ? | 18, 20 | store 0x57e7f0: value from a register, read the code |
| 0x57e912 | euro_champ_qual.cpp | EuroChampQual::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x57e90d | euro_champ_qual.cpp | EuroChampQual::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x57e962 | euro_champ_qual.cpp | EuroChampQual::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x57e969 | euro_champ_qual.cpp | EuroChampQual::set_up_rules | promotion play-off places (+0xbf) | 1 | 1 | 19 |  |
| 0x57e922 | euro_champ_qual.cpp | EuroChampQual::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x57e922 | euro_champ_qual.cpp | EuroChampQual::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x57e979 | euro_champ_qual.cpp | EuroChampQual::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x57f0d4 | euro_champ_qual.cpp | EuroChampQual::create_playoff_stage | direct promotion places (+0xbe) | 2 | 1 | 18 | outside set_up_rules |
| 0x57f0dc | euro_champ_qual.cpp | EuroChampQual::create_playoff_stage | promotion play-off places (+0xbf) | 0 | 1 | 19 | outside set_up_rules |
| 0x580f56 | euro_champ_qual.cpp | EuroChampQual::create_groups_and_results | club count (+0x3e) | unknown | ? | 18, 20 | store 0x580f56: value from a register, read the code |
| 0x583842 | european_cup.cpp | EuropeanCup::EuropeanCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5837ff | european_cup.cpp | EuropeanCup::EuropeanCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5849a3 | european_cup.cpp | EuropeanCup::build_qualifying_entry_table | club count (+0x3e) | 55 | 2 | 21 |  |
| 0x58bcc2 | fifa_confed.cpp | FifaConfed::create_group_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x58bdbc | fifa_confed.cpp | FifaConfed::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x58bdb4 | fifa_confed.cpp | FifaConfed::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x58bdeb | fifa_confed.cpp | FifaConfed::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x58bdb9 | fifa_confed.cpp | FifaConfed::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x58bdb9 | fifa_confed.cpp | FifaConfed::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x58bdb9 | fifa_confed.cpp | FifaConfed::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x58be1e | fifa_confed.cpp | FifaConfed::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5922bc | fin_cup.cpp | FinCup::FinCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5922c0 | fin_cup.cpp | FinCup::FinCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x592a38 | fin_cup.cpp | FinCup::collect_entrants | club count (+0x3e) | 76 | 2 | 21 |  |
| 0x593a86 | fin_first.cpp | FinFirst::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x593b8a | fin_first.cpp | FinFirst::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x593b8f | fin_first.cpp | FinFirst::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 | shared register |
| 0x593bc7 | fin_first.cpp | FinFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 1 | 19 |  |
| 0x593b85 | fin_first.cpp | FinFirst::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 |  |
| 0x593b8f | fin_first.cpp | FinFirst::set_up_rules | direct relegation places (+0xc1) | 2 | 1 | 18 | shared register |
| 0x593b8a | fin_first.cpp | FinFirst::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x593c09 | fin_first.cpp | FinFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x595b86 | fin_prm.cpp | FinPrm::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x595c95 | fin_prm.cpp | FinPrm::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x595ca0 | fin_prm.cpp | FinPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x595ca0 | fin_prm.cpp | FinPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x595c9a | fin_prm.cpp | FinPrm::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register |
| 0x595c9a | fin_prm.cpp | FinPrm::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x595cfd | fin_prm.cpp | FinPrm::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x595c95 | fin_prm.cpp | FinPrm::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x5b9fb6 | fra_cfa.cpp | FraCfa::create_group_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5ba0a4 | fra_cfa.cpp|fra_cup.cpp | FraCfa::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 21 |  |
| 0x5ba0b8 | fra_cfa.cpp|fra_cup.cpp | FraCfa::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x5ba0b8 | fra_cfa.cpp|fra_cup.cpp | FraCfa::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5ba0b8 | fra_cfa.cpp|fra_cup.cpp | FraCfa::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5ba0b8 | fra_cfa.cpp|fra_cup.cpp | FraCfa::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x5ba171 | fra_cup.cpp | FraCup::FraCup | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x5ba171 | fra_cup.cpp | FraCup::FraCup | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5baa68 | fra_cup.cpp | FraCup::collect_entrants | club count (+0x3e) | 222 | 2 | 21 |  |
| 0x5bc2c9 | fra_first.cpp | FraFirst::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5bc2d1 | fra_first.cpp | FraFirst::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x5bc414 | fra_first.cpp | FraFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5bc41a | fra_first.cpp | FraFirst::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x5bc41a | fra_first.cpp | FraFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5bc41a | fra_first.cpp | FraFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5bc414 | fra_first.cpp | FraFirst::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x5bc419 | fra_first.cpp | FraFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x5bc48c | fra_first.cpp | FraFirst::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x5bc419 | fra_first.cpp | FraFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5bc9c1 | fra_lge_cup.cpp | FraLgeCup::FraLgeCup | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x5bc9c1 | fra_lge_cup.cpp | FraLgeCup::FraLgeCup | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5bd178 | fra_lge_cup.cpp | FraLgeCup::collect_entrants | club count (+0x3e) | 54 | 2 | 21 |  |
| 0x5be246 | fra_lower.cpp | FraLower::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x5bef25 | fra_second.cpp | FraSecond::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x5bf0a4 | fra_second.cpp | FraSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5bf0e7 | fra_second.cpp | FraSecond::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 |  |
| 0x5bf0a4 | fra_second.cpp | FraSecond::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x5bf0a9 | fra_second.cpp | FraSecond::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x5bf0a9 | fra_second.cpp | FraSecond::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x5bf0fc | fra_second.cpp | FraSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5bf0fc | fra_second.cpp | FraSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5bf0a9 | fra_second.cpp | FraSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5bf137 | fra_second.cpp | FraSecond::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x5bf780 | fra_super.cpp | FraSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x5c05e6 | fra_third.cpp | FraThird::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x5c0704 | fra_third.cpp | FraThird::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x5c0741 | fra_third.cpp | FraThird::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 | shared register |
| 0x5c0741 | fra_third.cpp | FraThird::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 | shared register |
| 0x5c0709 | fra_third.cpp | FraThird::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x5c075c | fra_third.cpp | FraThird::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x5c075d | fra_third.cpp | FraThird::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5c075d | fra_third.cpp | FraThird::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5c0709 | fra_third.cpp | FraThird::set_up_rules | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x5c0709 | fra_third.cpp | FraThird::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5d8c8c | ger_cup.cpp | GerCup::GerCup | named substitutes (+0x49) | 6 | 1 | 17 |  |
| 0x5d8c90 | ger_cup.cpp | GerCup::GerCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5d9389 | ger_cup.cpp | GerCup::collect_entrants | club count (+0x3e) | 64 | 2 | 21 |  |
| 0x5db248 | ger_first.cpp | GerFirst::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5db404 | ger_first.cpp | GerFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x5db40a | ger_first.cpp | GerFirst::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x5db40a | ger_first.cpp | GerFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5db40a | ger_first.cpp | GerFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5db409 | ger_first.cpp | GerFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x5db409 | ger_first.cpp | GerFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5db48c | ger_first.cpp | GerFirst::set_up_rules | named substitutes (+0x49) | 6 | 1 | 17 |  |
| 0x5dc1ad | ger_lge_cup.cpp | GerLgeCup::GerLgeCup | named substitutes (+0x49) | 6 | 1 | 17 |  |
| 0x5dc1b1 | ger_lge_cup.cpp | GerLgeCup::GerLgeCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5dc714 | ger_lge_cup.cpp | GerLgeCup::collect_entrants | club count (+0x3e) | 6 | 2 | 21 |  |
| 0x5dcba6 | ger_lge_cup.cpp|ger_regional.cpp | GerLgeCup::set_up_rounds | named substitutes (+0x49) | 6 | 1 | 17 |  |
| 0x5dcbaa | ger_lge_cup.cpp|ger_regional.cpp | GerLgeCup::set_up_rounds | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5dd805 | ger_regional.cpp | GerRegional::create_table | club count (+0x3e) | 19 | 2 | 18, 20 |  |
| 0x5dd80d | ger_regional.cpp | GerRegional::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5dd9f4 | ger_regional.cpp | GerRegional::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5dd9f4 | ger_regional.cpp | GerRegional::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x5dd9f8 | ger_regional.cpp | GerRegional::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5dd9f8 | ger_regional.cpp | GerRegional::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5dda49 | ger_regional.cpp | GerRegional::set_up_rules | direct relegation places (+0xc1) | 5 | 1 | 18 |  |
| 0x5dda52 | ger_regional.cpp | GerRegional::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x5df2c6 | ger_second.cpp | GerSecond::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5df3e4 | ger_second.cpp | GerSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x5df3e9 | ger_second.cpp | GerSecond::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x5df427 | ger_second.cpp | GerSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 1 | 19 |  |
| 0x5df42e | ger_second.cpp | GerSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x5df3eb | ger_second.cpp | GerSecond::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x5df3e9 | ger_second.cpp | GerSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x5df47a | ger_second.cpp | GerSecond::set_up_rules | named substitutes (+0x49) | 6 | 1 | 17 |  |
| 0x5e11f2 | goldcup.cpp | GoldCup::create_group_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x5e12f2 | goldcup.cpp | GoldCup::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5e1316 | goldcup.cpp | GoldCup::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 21 |  |
| 0x5e12ed | goldcup.cpp | GoldCup::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 |  |
| 0x5e1309 | goldcup.cpp | GoldCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5e1309 | goldcup.cpp | GoldCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5e1309 | goldcup.cpp | GoldCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x5e1357 | goldcup.cpp | GoldCup::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5e1b33 | goldcup.cpp | GoldCup::create_knockout_stage | direct promotion places (+0xbe) | unknown | ? | 18 | store 0x5e1b33: value from a register, read the code; outside set_up_rules |
| 0x5e88f4 | gre_cup.cpp | GreCup::GreCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5e88f8 | gre_cup.cpp | GreCup::GreCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5e9272 | gre_cup.cpp | GreCup::create_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x5e9369 | gre_cup.cpp | GreCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5e9369 | gre_cup.cpp | GreCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5e9369 | gre_cup.cpp | GreCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x5e9357 | gre_cup.cpp | GreCup::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x5e9359 | gre_cup.cpp | GreCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 |  |
| 0x5ead76 | gre_prm.cpp | GrePrm::create_table | club count (+0x3e) | 14 | 2 | 18, 20 |  |
| 0x5eae97 | gre_prm.cpp | GrePrm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5eae9b | gre_prm.cpp | GrePrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x5eae9b | gre_prm.cpp | GrePrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5eae9b | gre_prm.cpp | GrePrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5eae97 | gre_prm.cpp | GrePrm::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x5eae95 | gre_prm.cpp | GrePrm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5eaf29 | gre_prm.cpp | GrePrm::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5ebcc6 | gre_second.cpp | GreSecond::create_table | club count (+0x3e) | 14 | 2 | 18, 20 |  |
| 0x5ebde4 | gre_second.cpp | GreSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5ebdea | gre_second.cpp | GreSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5ebdea | gre_second.cpp | GreSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5ebde4 | gre_second.cpp | GreSecond::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x5ebde4 | gre_second.cpp | GreSecond::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x5ebde9 | gre_second.cpp | GreSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5ebe77 | gre_second.cpp | GreSecond::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5ec0ac | gre_super.cpp | GreSuper::GreSuper | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5ec0b0 | gre_super.cpp | GreSuper::GreSuper | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5ec440 | gre_super.cpp | GreSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x5f34aa | hol_cup.cpp | HolCup::HolCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5f34ae | hol_cup.cpp | HolCup::HolCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5f3487 | hol_cup.cpp | HolCup::HolCup | club count (+0x3e) | 0 | 2 | 21 |  |
| 0x5f3d82 | hol_cup.cpp | HolCup::create_group_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x5f3e89 | hol_cup.cpp | HolCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5f3e89 | hol_cup.cpp | HolCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5f3e89 | hol_cup.cpp | HolCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x5f3e77 | hol_cup.cpp | HolCup::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x5f3e79 | hol_cup.cpp | HolCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 |  |
| 0x5f6256 | hol_first.cpp | HolFirst::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5f6345 | hol_first.cpp | HolFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x5f634c | hol_first.cpp | HolFirst::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 |  |
| 0x5f6360 | hol_first.cpp | HolFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5f6360 | hol_first.cpp | HolFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x5f6360 | hol_first.cpp | HolFirst::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x5f634a | hol_first.cpp | HolFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5f63d5 | hol_first.cpp | HolFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5f68da | hol_first.cpp | HolFirst::create_play_off_groups | direct relegation places (+0xc1) | 0 | 2 | 18 | outside set_up_rules |
| 0x5f8446 | hol_prm.cpp | HolPrm::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x5f8567 | hol_prm.cpp | HolPrm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x5f8570 | hol_prm.cpp | HolPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x5f8570 | hol_prm.cpp | HolPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x5f8567 | hol_prm.cpp | HolPrm::set_up_rules | relegation play-off places (+0xc0) | 2 | 4 | 19 | shared register |
| 0x5f856c | hol_prm.cpp | HolPrm::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 |  |
| 0x5f8565 | hol_prm.cpp | HolPrm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x5f85f5 | hol_prm.cpp | HolPrm::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x5f8c20 | hol_super.cpp | HolSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x5fbb43 | human_manager.cpp | Human_manager::load_human_record | named substitutes (+0x49) | unknown | ? | 17 | store 0x5fbb43: value from a register, read the code |
| 0x6320f0 | inter_amer_cup.cpp | InterAmerCup::InterAmerCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x6320f4 | inter_amer_cup.cpp | InterAmerCup::InterAmerCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x632496 | inter_amer_cup.cpp | InterAmerCup::select_teams | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x633021 | intertoto_cup.cpp | IntertotoCup::IntertotoCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x632ff1 | intertoto_cup.cpp | IntertotoCup::IntertotoCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x6336db | intertoto_cup.cpp | IntertotoCup::collect_entrants | club count (+0x3e) | 60 | 2 | 21 |  |
| 0x63410c | ire_chal_cup.cpp | IreChalCup::IreChalCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x634110 | ire_chal_cup.cpp | IreChalCup::IreChalCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x6347e9 | ire_chal_cup.cpp | IreChalCup::collect_entrants | club count (+0x3e) | 42 | 2 | 21 |  |
| 0x63591f | ire_first.cpp | IreFirst::create_table | club count (+0x3e) | unknown | ? | 18, 20 | store 0x63591f: value from a register, read the code |
| 0x635993 | ire_first.cpp | IreFirst::create_table | meetings per pair (+0x3c) | 3 | 2 | 19, 20 | outside set_up_rules |
| 0x63599b | ire_first.cpp | IreFirst::create_table | meetings per pair (+0x3c) | 4 | 2 | 19, 20 | outside set_up_rules |
| 0x635ab4 | ire_first.cpp | IreFirst::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x635aa7 | ire_first.cpp | IreFirst::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x635aa7 | ire_first.cpp | IreFirst::set_up_rules | promotion play-off places (+0xbf) | 1 | 4 | 19 | shared register |
| 0x635aad | ire_first.cpp | IreFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x635aad | ire_first.cpp | IreFirst::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x635aa5 | ire_first.cpp | IreFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x635b20 | ire_first.cpp | IreFirst::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x63650c | ire_leinster_cup.cpp | IreLeinsterCup::IreLeinsterCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x636510 | ire_leinster_cup.cpp | IreLeinsterCup::IreLeinsterCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x636a96 | ire_leinster_cup.cpp | IreLeinsterCup::select_clubs | club count (+0x3e) | 16 | 2 | 21 |  |
| 0x636e24 | ire_lge_cup.cpp | IreLgeCup::IreLgeCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x636e28 | ire_lge_cup.cpp | IreLgeCup::IreLgeCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x6375d2 | ire_lge_cup.cpp | IreLgeCup::create_table | club count (+0x3e) | 3 | 2 | 21 |  |
| 0x6376da | ire_lge_cup.cpp | IreLgeCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x6376da | ire_lge_cup.cpp | IreLgeCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x6376da | ire_lge_cup.cpp | IreLgeCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x6376ca | ire_lge_cup.cpp | IreLgeCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 | shared register |
| 0x6376ca | ire_lge_cup.cpp | IreLgeCup::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x63880c | ire_munster_cup.cpp | IreMunsterCup::IreMunsterCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x638810 | ire_munster_cup.cpp | IreMunsterCup::IreMunsterCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x638e49 | ire_munster_cup.cpp | IreMunsterCup::collect_entrants | club count (+0x3e) | 20 | 2 | 21 |  |
| 0x63968b | ire_pres_cup.cpp | IrePresCup::collect_entrants | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x63a74f | ire_prm.cpp | IrePrm::create_table | club count (+0x3e) | unknown | ? | 18, 20 | store 0x63a74f: value from a register, read the code |
| 0x63a7c2 | ire_prm.cpp | IrePrm::create_table | meetings per pair (+0x3c) | 4 | 2 | 19, 20 | outside set_up_rules |
| 0x63a7ca | ire_prm.cpp | IrePrm::create_table | meetings per pair (+0x3c) | 3 | 2 | 19, 20 | outside set_up_rules |
| 0x63a8b4 | ire_prm.cpp | IrePrm::create_table | relegation play-off places (+0xc0) | 1 | 1 | 19 | outside set_up_rules |
| 0x63a8bb | ire_prm.cpp | IrePrm::create_table | direct relegation places (+0xc1) | 3 | 1 | 18 | outside set_up_rules |
| 0x63a90a | ire_prm.cpp | IrePrm::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x63a910 | ire_prm.cpp | IrePrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x63a910 | ire_prm.cpp | IrePrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x63a905 | ire_prm.cpp | IrePrm::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register |
| 0x63a905 | ire_prm.cpp | IrePrm::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x63a90a | ire_prm.cpp | IrePrm::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x63a98e | ire_prm.cpp | IrePrm::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x63ad24 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x63ad28 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | substitutes used (+0x4a) | 5 | 1 | 17 |  |
| 0x63ad7a | ire_super_cup.cpp | IreSuperCup::IreSuperCup | meetings per pair (+0x3c) | 1 | 2 | 21 | outside set_up_rules |
| 0x63ad07 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register; outside set_up_rules |
| 0x63ad07 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register; outside set_up_rules |
| 0x63ad07 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register; outside set_up_rules |
| 0x63ad07 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register; outside set_up_rules |
| 0x63ad07 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | club count (+0x3e) | 0 | 2 | 21 | shared register |
| 0x63b0d8 | ire_super_cup.cpp | IreSuperCup::create_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x63b4d9 | ire_super_cup.cpp | IreSuperCup::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x63b4d9 | ire_super_cup.cpp | IreSuperCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x63b4d9 | ire_super_cup.cpp | IreSuperCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x63b4d9 | ire_super_cup.cpp | IreSuperCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x63b4c9 | ire_super_cup.cpp | IreSuperCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 |  |
| 0x63c64c | ita_c1_super.cpp | ItaC1Super::ItaC1Super | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x63c650 | ita_c1_super.cpp | ItaC1Super::ItaC1Super | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x63c9e0 | ita_c1_super.cpp | ItaC1Super::init_team_list | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x63cc94 | ita_c_cup.cpp | ItaCCup::ItaCCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x63cc98 | ita_c_cup.cpp | ItaCCup::ItaCCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x63d5f2 | ita_c_cup.cpp | ItaCCup::init_first_group | club count (+0x3e) | 5 | 2 | 21 |  |
| 0x63d6fa | ita_c_cup.cpp | ItaCCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x63d6fa | ita_c_cup.cpp | ItaCCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x63d6fa | ita_c_cup.cpp | ItaCCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x63d6ea | ita_c_cup.cpp | ItaCCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 | shared register |
| 0x63d6ea | ita_c_cup.cpp | ItaCCup::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x63de20 | ita_c_cup.cpp | ItaCCup::create_knockout_stage | direct promotion places (+0xbe) | 2 | 1 | 18 | outside set_up_rules |
| 0x63e954 | ita_cup.cpp | ItaCup::ItaCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x63e958 | ita_cup.cpp | ItaCup::ItaCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x63f262 | ita_cup.cpp | ItaCup::create_first_group_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x63f36a | ita_cup.cpp | ItaCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x63f36a | ita_cup.cpp | ItaCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x63f36a | ita_cup.cpp | ItaCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x63f35a | ita_cup.cpp | ItaCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 | shared register |
| 0x63f35a | ita_cup.cpp | ItaCup::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x641526 | ita_ser_a.cpp | ItaSerA::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x64164a | ita_ser_a.cpp | ItaSerA::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x64164e | ita_ser_a.cpp | ItaSerA::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x64164e | ita_ser_a.cpp | ItaSerA::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x64164e | ita_ser_a.cpp | ItaSerA::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x641651 | ita_ser_a.cpp | ItaSerA::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x6416d9 | ita_ser_a.cpp | ItaSerA::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x6416dd | ita_ser_a.cpp | ItaSerA::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x646796 | ita_ser_b.cpp | ItaSerB::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x6468aa | ita_ser_b.cpp | ItaSerB::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x6468b1 | ita_ser_b.cpp | ItaSerB::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 | shared register |
| 0x6468ae | ita_ser_b.cpp | ItaSerB::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x6468ae | ita_ser_b.cpp | ItaSerB::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x6468b1 | ita_ser_b.cpp | ItaSerB::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 | shared register |
| 0x64693b | ita_ser_b.cpp | ItaSerB::set_up_rules_fragment | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x64693f | ita_ser_b.cpp | ItaSerB::set_up_rules_fragment | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x64b686 | ita_ser_c1a.cpp | ItaSerC1a::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x64b777 | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x64b77c | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x64b793 | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x64b793 | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | relegation play-off places (+0xc0) | 4 | 1 | 19 | shared register |
| 0x64b77c | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x64b775 | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x64b806 | ita_ser_c1a.cpp | ItaSerC1a::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x64f836 | ita_ser_c1b.cpp | ItaSerC1b::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x64f925 | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x64f92c | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x64f943 | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x64f943 | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | relegation play-off places (+0xc0) | 4 | 1 | 19 | shared register |
| 0x64f92c | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x64f92a | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x64f9cd | ita_ser_c1b.cpp | ItaSerC1b::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x653aa6 | ita_ser_c2a.cpp | ItaSerC2a::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x653b97 | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x653b9c | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x653bb3 | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x653bb3 | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | relegation play-off places (+0xc0) | 4 | 1 | 19 | shared register |
| 0x653b9c | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x653b95 | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x653c26 | ita_ser_c2a.cpp | ItaSerC2a::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x657f66 | ita_ser_c2b.cpp | ItaSerC2b::init_team_list | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x658057 | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x65805c | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x658073 | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x658073 | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | relegation play-off places (+0xc0) | 4 | 1 | 19 | shared register |
| 0x65805c | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x658055 | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x6580e6 | ita_ser_c2b.cpp | ItaSerC2b::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x65c0f6 | ita_ser_c2c.cpp | ItaSerC2c::init_team_list | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x65c1e7 | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x65c1ec | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x65c203 | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x65c203 | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | relegation play-off places (+0xc0) | 4 | 1 | 19 | shared register |
| 0x65c1ec | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x65c1e5 | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x65c274 | ita_ser_c2c.cpp | ItaSerC2c::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x65f6dc | ita_super.cpp | ItaSuper::ItaSuper | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x65f6e0 | ita_super.cpp | ItaSuper::ItaSuper | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x65fa70 | ita_super.cpp | ItaSuper::init_team_list | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x661d9c | jap_emp_cup.cpp | JapEmpCup::JapEmpCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x661da0 | jap_emp_cup.cpp | JapEmpCup::JapEmpCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x662514 | jap_emp_cup.cpp | JapEmpCup::collect_entrants | club count (+0x3e) | 80 | 2 | 21 |  |
| 0x6634f6 | jap_j1.cpp | JapJ1::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x663624 | jap_j1.cpp | JapJ1::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x66362a | jap_j1.cpp | JapJ1::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x66362a | jap_j1.cpp | JapJ1::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x66362a | jap_j1.cpp | JapJ1::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x663624 | jap_j1.cpp | JapJ1::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x663629 | jap_j1.cpp | JapJ1::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x6636be | jap_j1.cpp | JapJ1::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x665046 | jap_j2.cpp | JapJ2::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x66514d | jap_j2.cpp | JapJ2::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x665146 | jap_j2.cpp | JapJ2::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x665147 | jap_j2.cpp | JapJ2::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x665147 | jap_j2.cpp | JapJ2::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x665147 | jap_j2.cpp | JapJ2::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x665144 | jap_j2.cpp | JapJ2::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x6651d0 | jap_j2.cpp | JapJ2::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x6653ec | jap_j_cup.cpp | JapJCup::JapJCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x6653f0 | jap_j_cup.cpp | JapJCup::JapJCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x665a18 | jap_j_cup.cpp | JapJCup::collect_entrants | club count (+0x3e) | 28 | 2 | 21 |  |
| 0x666170 | jap_super.cpp | JapSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x66ab0a | kor_fa_cup.cpp | KorFaCup::collect_entrants | club count (+0x3e) | 32 | 2 | 21 |  |
| 0x66bbb6 | kor_league.cpp | KorLeague::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x66bcc3 | kor_league.cpp | KorLeague::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x66bcc3 | kor_league.cpp | KorLeague::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x66bcc3 | kor_league.cpp | KorLeague::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x66bcc3 | kor_league.cpp | KorLeague::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x66bcc6 | kor_league.cpp | KorLeague::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x66bcc6 | kor_league.cpp | KorLeague::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x66bd49 | kor_league.cpp | KorLeague::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x66bf54 | kor_league_cup.cpp | KorLeagueCup::KorLeagueCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x66bf58 | kor_league_cup.cpp | KorLeagueCup::KorLeagueCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x66c782 | kor_league_cup.cpp | KorLeagueCup::create_group_a_table | club count (+0x3e) | 5 | 2 | 21 |  |
| 0x66c873 | kor_league_cup.cpp | KorLeagueCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x66c873 | kor_league_cup.cpp | KorLeagueCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x66c873 | kor_league_cup.cpp | KorLeagueCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x66c876 | kor_league_cup.cpp | KorLeagueCup::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 21 | shared register |
| 0x66c876 | kor_league_cup.cpp | KorLeagueCup::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x66d888 | kor_super_cup.cpp | KorSuperCup::KorSuperCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x66d88c | kor_super_cup.cpp | KorSuperCup::KorSuperCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x66dc20 | kor_super_cup.cpp | KorSuperCup::create_table | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x682474 | league.cpp | League::copy_for_display | meetings per pair (+0x3c) | unknown | ? | 19, 20 | store 0x682474: value from a register, read the code; outside set_up_rules |
| 0x68247c | league.cpp | League::copy_for_display | club count (+0x3e) | unknown | ? | 18, 20 | store 0x68247c: value from a register, read the code |
| 0x6824a3 | league.cpp | League::copy_for_display | named substitutes (+0x49) | unknown | ? | 17 | store 0x6824a3: value from a register, read the code |
| 0x6824a9 | league.cpp | League::copy_for_display | substitutes used (+0x4a) | unknown | ? | 17 | store 0x6824a9: value from a register, read the code |
| 0x687a05 | league.cpp | League::free_table | club count (+0x3e) | 0 | 2 | 18, 20 |  |
| 0x68b141 | league_stage.cpp | League_Stage::League_Stage | club count (+0x3e) | unknown | ? | 18, 20 | store 0x68b141: value from a register, read the code |
| 0x68b14d | league_stage.cpp | League_Stage::League_Stage | meetings per pair (+0x3c) | unknown | ? | 19, 20 | store 0x68b14d: value from a register, read the code; outside set_up_rules |
| 0x68b24d | league_stage.cpp | League_Stage::League_Stage | named substitutes (+0x49) | unknown | ? | 17 | store 0x68b24d: value from a register, read the code |
| 0x68b261 | league_stage.cpp | League_Stage::League_Stage | substitutes used (+0x4a) | unknown | ? | 17 | store 0x68b261: value from a register, read the code |
| 0x765ca9 | mini_cup.cpp | MiniCup::MiniCup | named substitutes (+0x49) | 9 | 1 | 17 | shared register |
| 0x765ca9 | mini_cup.cpp | MiniCup::MiniCup | substitutes used (+0x4a) | 9 | 1 | 17 | shared register |
| 0x7662a0 | mini_cup.cpp | MiniCup::set_up_clubs | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x76655d | mini_league.cpp | MiniLeague::MiniLeague | club count (+0x3e) | unknown | ? | 18, 20 | store 0x76655d: value from a register, read the code |
| 0x766636 | mini_league.cpp | MiniLeague::MiniLeague | named substitutes (+0x49) | 9 | 1 | 17 | shared register |
| 0x766636 | mini_league.cpp | MiniLeague::MiniLeague | substitutes used (+0x4a) | 9 | 1 | 17 | shared register |
| 0x76664e | mini_league.cpp | MiniLeague::MiniLeague | meetings per pair (+0x3c) | 1 | 2 | 19, 20 | outside set_up_rules |
| 0x766530 | mini_league.cpp | MiniLeague::MiniLeague | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register; outside set_up_rules |
| 0x766530 | mini_league.cpp | MiniLeague::MiniLeague | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register; outside set_up_rules |
| 0x766530 | mini_league.cpp | MiniLeague::MiniLeague | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register; outside set_up_rules |
| 0x766530 | mini_league.cpp | MiniLeague::MiniLeague | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register; outside set_up_rules |
| 0x76685a | mini_league.cpp | MiniLeague::MiniLeague | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x766db3 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x766db3 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x766db3 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x766db3 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x766db6 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | named substitutes (+0x49) | 9 | 1 | 17 | shared register |
| 0x766db6 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | substitutes used (+0x4a) | 9 | 1 | 17 | shared register |
| 0x766df2 | mini_league.cpp|month_award.cpp | MiniLeague::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x789e31 | nir_charity.cpp | NirCharity::NirCharity | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x789e31 | nir_charity.cpp | NirCharity::NirCharity | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x78a1f0 | nir_charity.cpp | NirCharity::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x78a5f1 | nir_cup.cpp | NirCup::NirCup | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x78a5f1 | nir_cup.cpp | NirCup::NirCup | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x78aca9 | nir_cup.cpp | NirCup::collect_entrants | club count (+0x3e) | 32 | 2 | 21 |  |
| 0x78bc66 | nir_first.cpp | NirFirst::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x78bd75 | nir_first.cpp | NirFirst::set_up_rules | meetings per pair (+0x3c) | 4 | 4 | 19, 20 |  |
| 0x78bd7c | nir_first.cpp | NirFirst::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 | shared register |
| 0x78bd7c | nir_first.cpp | NirFirst::set_up_rules | promotion play-off places (+0xbf) | 1 | 4 | 19 | shared register |
| 0x78bd8c | nir_first.cpp | NirFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x78bd8c | nir_first.cpp | NirFirst::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x78bd7a | nir_first.cpp | NirFirst::set_up_rules | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x78bd7a | nir_first.cpp | NirFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x78c7d4 | nir_lge_cup.cpp | NirLgeCup::NirLgeCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x78c7d8 | nir_lge_cup.cpp | NirLgeCup::NirLgeCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x78cfe2 | nir_lge_cup.cpp | NirLgeCup::create_table | club count (+0x3e) | 5 | 2 | 21 |  |
| 0x78d0e9 | nir_lge_cup.cpp | NirLgeCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x78d0e9 | nir_lge_cup.cpp | NirLgeCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x78d0e9 | nir_lge_cup.cpp | NirLgeCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x78d0d7 | nir_lge_cup.cpp | NirLgeCup::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x78d0d9 | nir_lge_cup.cpp | NirLgeCup::set_up_rules | meetings per pair (+0x3c) | 1 | 4 | 21 |  |
| 0x78ec76 | nir_prm.cpp | NirPrm::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x78ed96 | nir_prm.cpp | NirPrm::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x78ed90 | nir_prm.cpp | NirPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x78ed90 | nir_prm.cpp | NirPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x78ed85 | nir_prm.cpp | NirPrm::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register |
| 0x78ed85 | nir_prm.cpp | NirPrm::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 | shared register |
| 0x78ed8a | nir_prm.cpp | NirPrm::set_up_rules | named substitutes (+0x49) | 3 | 4 | 17 | shared register |
| 0x78ed8a | nir_prm.cpp | NirPrm::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x78f08c | nor_cup.cpp | NorCup::NorCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x78f090 | nor_cup.cpp | NorCup::NorCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x78f8ac | nor_cup.cpp | NorCup::collect_entrants | club count (+0x3e) | 128 | 2 | 21 |  |
| 0x7909b5 | nor_first.cpp | NorFirst::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x790a77 | nor_first.cpp | NorFirst::create_table | club count (+0x3e) | 14 | 2 | 18, 20 |  |
| 0x790a7f | nor_first.cpp | NorFirst::create_table | direct relegation places (+0xc1) | 2 | 1 | 18 | outside set_up_rules |
| 0x790b05 | nor_first.cpp | NorFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x790b05 | nor_first.cpp | NorFirst::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x790b0c | nor_first.cpp | NorFirst::set_up_rules | promotion play-off places (+0xbf) | 1 | 4 | 19 |  |
| 0x790b4d | nor_first.cpp | NorFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x790b54 | nor_first.cpp | NorFirst::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x790b0a | nor_first.cpp | NorFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x790b81 | nor_first.cpp | NorFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7925f6 | nor_prm.cpp | NorPrm::create_table | club count (+0x3e) | 14 | 2 | 18, 20 |  |
| 0x792705 | nor_prm.cpp | NorPrm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x792710 | nor_prm.cpp | NorPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x792710 | nor_prm.cpp | NorPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x79270c | nor_prm.cpp | NorPrm::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 |  |
| 0x792705 | nor_prm.cpp | NorPrm::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x79270a | nor_prm.cpp | NorPrm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x79278f | nor_prm.cpp | NorPrm::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7978d2 | oceania_club_champ.cpp | OceaniaClubChamp::create_group_a_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x7979cd | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x7979cf | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | named substitutes (+0x49) | 3 | 1 | 17 | shared register |
| 0x7979cf | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x797a00 | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x7979ec | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7979ec | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7979ec | oceania_club_champ.cpp | OceaniaClubChamp::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7995b2 | oceania_nations.cpp | OceaniaNations::create_group_table | club count (+0x3e) | 3 | 2 | 18, 20 |  |
| 0x7996b2 | oceania_nations.cpp | OceaniaNations::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7996d6 | oceania_nations.cpp | OceaniaNations::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x7996ad | oceania_nations.cpp | OceaniaNations::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 |  |
| 0x7996c9 | oceania_nations.cpp | OceaniaNations::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7996c9 | oceania_nations.cpp | OceaniaNations::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7996c9 | oceania_nations.cpp | OceaniaNations::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x799711 | oceania_nations.cpp | OceaniaNations::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7a0152 | olympics.cpp | Olympics::init_group_a_table | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x7a0252 | olympics.cpp | Olympics::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7a0276 | olympics.cpp | Olympics::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x7a024d | olympics.cpp | Olympics::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 |  |
| 0x7a0269 | olympics.cpp | Olympics::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7a0269 | olympics.cpp | Olympics::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7a0269 | olympics.cpp | Olympics::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7a02b1 | olympics.cpp | Olympics::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7ab40b | player_regen.cpp | player_regen_regenerate_as_non_player | not a competition field | n/a | n/a | none | not a competition: the store writes the game date word 0xae2c90 into a staff record (name check 2026-10-16) |
| 0x7abdbd | player_regen.cpp | PLAYER_REGEN::regenerate_as_youth_player | not a competition field | n/a | n/a | none | not a competition: the store writes the game date word 0xae2c90 into a staff record (name check 2026-10-16) |
| 0x7c7dec | pol_cup.cpp | PolCup::PolCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7c7df0 | pol_cup.cpp | PolCup::PolCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7c856d | pol_cup.cpp | PolCup::collect_entrants | club count (+0x3e) | 52 | 2 | 21 |  |
| 0x7c945b | pol_first.cpp | PolFirst::create_table | club count (+0x3e) | 8 | 4 | 18, 20 |  |
| 0x7c9694 | pol_first.cpp | PolFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x7c969b | pol_first.cpp | PolFirst::set_up_rules | direct promotion places (+0xbe) | 4 | 1 | 18 |  |
| 0x7c9698 | pol_first.cpp | PolFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7c9698 | pol_first.cpp | PolFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7c9698 | pol_first.cpp | PolFirst::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7c9719 | pol_first.cpp | PolFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7c971d | pol_first.cpp | PolFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7ca5bf | pol_lge_cup.cpp | PolLgeCup::PolLgeCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7ca5c3 | pol_lge_cup.cpp | PolLgeCup::PolLgeCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7cad4b | pol_lge_cup.cpp | PolLgeCup::collect_entrants | club count (+0x3e) | 36 | 2 | 21 |  |
| 0x7cad53 | pol_lge_cup.cpp | PolLgeCup::collect_entrants | club count (+0x3e) | 34 | 2 | 21 |  |
| 0x7cb1b6 | pol_lge_cup.cpp|pol_second.cpp | PolLgeCup::set_up_rounds | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7cb1ba | pol_lge_cup.cpp|pol_second.cpp | PolLgeCup::set_up_rounds | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7cbe07 | pol_second.cpp | PolSecond::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x7cbe18 | pol_second.cpp | PolSecond::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x7cbf44 | pol_second.cpp | PolSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x7cbf44 | pol_second.cpp | PolSecond::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x7cbf44 | pol_second.cpp | PolSecond::set_up_rules | promotion play-off places (+0xbf) | 2 | 4 | 19 | shared register |
| 0x7cbf4d | pol_second.cpp | PolSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 |  |
| 0x7cbfac | pol_second.cpp | PolSecond::set_up_rules | direct relegation places (+0xc1) | 6 | 1 | 18 |  |
| 0x7cbfb5 | pol_second.cpp | PolSecond::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x7cbf49 | pol_second.cpp | PolSecond::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 |  |
| 0x7cbfe7 | pol_second.cpp | PolSecond::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7cc988 | pol_super.cpp | PolSuper::PolSuper | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7cc98c | pol_super.cpp | PolSuper::PolSuper | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7ccd20 | pol_super.cpp | PolSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x7ce238 | por_cup.cpp | PorCup::collect_entrants | club count (+0x3e) | 164 | 2 | 21 |  |
| 0x7cf816 | por_prm.cpp | PorPrm::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x7cf934 | por_prm.cpp | PorPrm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x7cf93a | por_prm.cpp | PorPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x7cf93a | por_prm.cpp | PorPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7cf93a | por_prm.cpp | PorPrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7cf939 | por_prm.cpp | PorPrm::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x7cf939 | por_prm.cpp | PorPrm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x7cf9c3 | por_prm.cpp | PorPrm::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7d1536 | por_second.cpp | PorSecond::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x7d16a4 | por_second.cpp | PorSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x7d16a9 | por_second.cpp | PorSecond::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x7d16f7 | por_second.cpp | PorSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 1 | 19 |  |
| 0x7d16fe | por_second.cpp | PorSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x7d16a9 | por_second.cpp | PorSecond::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x7d16a9 | por_second.cpp | PorSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x7d1745 | por_second.cpp | PorSecond::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7d2416 | por_second_b.cpp | PorSecondB::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x7d2615 | por_second_b.cpp | PorSecondB::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x7d261a | por_second_b.cpp | PorSecondB::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 |  |
| 0x7d2620 | por_second_b.cpp | PorSecondB::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7d2620 | por_second_b.cpp | PorSecondB::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7d261f | por_second_b.cpp | PorSecondB::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x7d268e | por_second_b.cpp | PorSecondB::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7d2692 | por_second_b.cpp | PorSecondB::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7d3270 | por_super.cpp | PorSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x7e927c | rus_cup.cpp | RusCup::RusCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7e9280 | rus_cup.cpp | RusCup::RusCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7e9a5b | rus_cup.cpp | RusCup::select_entrants | club count (+0x3e) | 48 | 2 | 21 |  |
| 0x7ea6d7 | rus_first.cpp | RusFirst::create_table | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x7ea814 | rus_first.cpp | RusFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x7ea814 | rus_first.cpp | RusFirst::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x7ea81a | rus_first.cpp | RusFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7ea81a | rus_first.cpp | RusFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7ea819 | rus_first.cpp | RusFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x7ea819 | rus_first.cpp | RusFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x7ea8a1 | rus_first.cpp | RusFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7eb626 | rus_prm.cpp | RusPrm::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x7eb747 | rus_prm.cpp | RusPrm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x7eb750 | rus_prm.cpp | RusPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x7eb750 | rus_prm.cpp | RusPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7eb750 | rus_prm.cpp | RusPrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7eb747 | rus_prm.cpp | RusPrm::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x7eb745 | rus_prm.cpp | RusPrm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7eb7dd | rus_prm.cpp | RusPrm::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x7ec9ec | sco_chal_cup.cpp | ScoChalCup::ScoChalCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x7ec9f0 | sco_chal_cup.cpp | ScoChalCup::ScoChalCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7ed028 | sco_chal_cup.cpp | ScoChalCup::collect_entrants | club count (+0x3e) | 30 | 2 | 21 |  |
| 0x7ed37c | sco_fa_cup.cpp | ScoFaCup::ScoFaCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x7ed380 | sco_fa_cup.cpp | ScoFaCup::ScoFaCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7edb0c | sco_fa_cup.cpp | ScoFaCup::collect_entrants | club count (+0x3e) | 50 | 2 | 21 |  |
| 0x7eed46 | sco_first.cpp | ScoFirst::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x7eee67 | sco_first.cpp | ScoFirst::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x7eee57 | sco_first.cpp | ScoFirst::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 |  |
| 0x7eee5c | sco_first.cpp | ScoFirst::set_up_rules | promotion play-off places (+0xbf) | 2 | 1 | 19 | shared register |
| 0x7eee98 | sco_first.cpp | ScoFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7eee98 | sco_first.cpp | ScoFirst::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7eee5c | sco_first.cpp | ScoFirst::set_up_rules | direct relegation places (+0xc1) | 2 | 1 | 18 | shared register |
| 0x7eeeae | sco_first.cpp | ScoFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7eeeae | sco_first.cpp | ScoFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7eee55 | sco_first.cpp | ScoFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7eeeec | sco_first.cpp | ScoFirst::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x7efa4c | sco_lge_cup.cpp | ScoLgeCup::ScoLgeCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x7efa50 | sco_lge_cup.cpp | ScoLgeCup::ScoLgeCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7efa79 | sco_lge_cup.cpp | ScoLgeCup::ScoLgeCup | club count (+0x3e) | 40 | 2 | 21 |  |
| 0x7f041b | sco_lge_cup.cpp | ScoLgeCup::collect_entrants | club count (+0x3e) | 40 | 2 | 21 |  |
| 0x7f0423 | sco_lge_cup.cpp | ScoLgeCup::collect_entrants | club count (+0x3e) | 42 | 2 | 21 |  |
| 0x7f09e9 | sco_lge_cup.cpp | ScoLgeCup::start_new_season | club count (+0x3e) | 40 | 2 | 21 |  |
| 0x7f2839 | sco_prm.cpp | ScoPrm::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x7f2841 | sco_prm.cpp | ScoPrm::create_table | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x7f298d | sco_prm.cpp | ScoPrm::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x7f2982 | sco_prm.cpp | ScoPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x7f2982 | sco_prm.cpp | ScoPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7f2985 | sco_prm.cpp | ScoPrm::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 |  |
| 0x7f2982 | sco_prm.cpp | ScoPrm::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7f2976 | sco_prm.cpp | ScoPrm::set_up_rules | meetings per pair (+0x3c) | 3 | 4 | 19, 20 | shared register |
| 0x7f29b2 | sco_prm.cpp | ScoPrm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x7f29b2 | sco_prm.cpp | ScoPrm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7f29b2 | sco_prm.cpp | ScoPrm::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7f29b5 | sco_prm.cpp | ScoPrm::set_up_rules | direct relegation places (+0xc1) | 1 | 4 | 18 |  |
| 0x7f2976 | sco_prm.cpp | ScoPrm::set_up_rules | substitutes used (+0x4a) | 3 | 4 | 17 | shared register |
| 0x7f2a46 | sco_prm.cpp | ScoPrm::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x7f2fef | sco_prm.cpp | ScoPrm::create_split_stages | meetings per pair (+0x3c) | 4 | 2 | 19, 20 | outside set_up_rules |
| 0x7f3e96 | sco_second.cpp | ScoSecond::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x7f3f91 | sco_second.cpp | ScoSecond::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x7f3f86 | sco_second.cpp | ScoSecond::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 | shared register |
| 0x7f3fc5 | sco_second.cpp | ScoSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7f3fc5 | sco_second.cpp | ScoSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7f3fc5 | sco_second.cpp | ScoSecond::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7f3f86 | sco_second.cpp | ScoSecond::set_up_rules | direct relegation places (+0xc1) | 2 | 1 | 18 | shared register |
| 0x7f3fdb | sco_second.cpp | ScoSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7f3fdb | sco_second.cpp | ScoSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7f3f84 | sco_second.cpp | ScoSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7f4017 | sco_second.cpp | ScoSecond::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x7f4e36 | sco_third.cpp | ScoThird::create_table | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x7f4f24 | sco_third.cpp | ScoThird::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x7f4f45 | sco_third.cpp | ScoThird::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x7f4f39 | sco_third.cpp | ScoThird::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x7f4f39 | sco_third.cpp | ScoThird::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x7f4f39 | sco_third.cpp | ScoThird::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x7f4f26 | sco_third.cpp | ScoThird::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x7f4f9f | sco_third.cpp | ScoThird::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x84cfbc | spa_cup.cpp | SpaCup::SpaCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x84cfc0 | spa_cup.cpp | SpaCup::SpaCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x84cfe3 | spa_cup.cpp | SpaCup::SpaCup | club count (+0x3e) | 83 | 2 | 21 |  |
| 0x84da48 | spa_cup.cpp | SpaCup::collect_entrants | club count (+0x3e) | 83 | 2 | 21 |  |
| 0x84fc86 | spa_first.cpp | SpaFirst::create_table | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x84fdf5 | spa_first.cpp | SpaFirst::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x84fdf5 | spa_first.cpp | SpaFirst::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x84fdf5 | spa_first.cpp | SpaFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x84fdf8 | spa_first.cpp | SpaFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x84fe03 | spa_first.cpp | SpaFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x84fe03 | spa_first.cpp | SpaFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x84fe92 | spa_first.cpp | SpaFirst::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x8533e6 | spa_lower.cpp | SpaLower::create_table | club count (+0x3e) | 22 | 2 | 18, 20 |  |
| 0x853794 | spa_lower.cpp | SpaLower::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x853794 | spa_lower.cpp | SpaLower::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x853794 | spa_lower.cpp | SpaLower::set_up_rules | promotion play-off places (+0xbf) | 2 | 4 | 19 | shared register |
| 0x8537dc | spa_lower.cpp | SpaLower::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x85379b | spa_lower.cpp | SpaLower::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x853799 | spa_lower.cpp | SpaLower::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x853814 | spa_lower.cpp | SpaLower::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x855b56 | spa_second.cpp | SpaSecond::create_table | club count (+0x3e) | 22 | 2 | 18, 20 |  |
| 0x855cc4 | spa_second.cpp | SpaSecond::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x855cc9 | spa_second.cpp | SpaSecond::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x855d0d | spa_second.cpp | SpaSecond::set_up_rules | promotion play-off places (+0xbf) | 0 | 1 | 19 |  |
| 0x855d14 | spa_second.cpp | SpaSecond::set_up_rules | relegation play-off places (+0xc0) | 0 | 1 | 19 |  |
| 0x855ccb | spa_second.cpp | SpaSecond::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 |  |
| 0x855cc9 | spa_second.cpp | SpaSecond::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x855d69 | spa_second.cpp | SpaSecond::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x856ce6 | spa_second_b.cpp | SpaSecondB::load_group_clubs | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x856ef7 | spa_second_b.cpp | SpaSecondB::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x856f33 | spa_second_b.cpp | SpaSecondB::set_up_rules | direct promotion places (+0xbe) | 0 | 1 | 18 |  |
| 0x856f01 | spa_second_b.cpp | SpaSecondB::set_up_rules | promotion play-off places (+0xbf) | 4 | 1 | 19 | shared register |
| 0x856efc | spa_second_b.cpp | SpaSecondB::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 |  |
| 0x856f01 | spa_second_b.cpp | SpaSecondB::set_up_rules | direct relegation places (+0xc1) | 4 | 1 | 18 | shared register |
| 0x856ef5 | spa_second_b.cpp | SpaSecondB::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x856f77 | spa_second_b.cpp | SpaSecondB::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x85871c | spa_super.cpp | SpaSuper::SpaSuper | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x858720 | spa_super.cpp | SpaSuper::SpaSuper | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x858ab0 | spa_super.cpp | SpaSuper::select_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x88c758 | sub_league.cpp | SubLeague::SubLeague | meetings per pair (+0x3c) | unknown | ? | 19, 20 | store 0x88c758: value from a register, read the code; outside set_up_rules |
| 0x88c79a | sub_league.cpp | SubLeague::SubLeague | club count (+0x3e) | unknown | ? | 18, 20 | store 0x88c79a: value from a register, read the code |
| 0x88cc20 | swe_cup.cpp | SweCup::SweCup | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x88cc24 | swe_cup.cpp | SweCup::SweCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x88d5b3 | swe_cup.cpp | SweCup::collect_entrants | club count (+0x3e) | 30 | 2 | 21 |  |
| 0x88efa6 | swe_first.cpp | SweFirst::create_table | club count (+0x3e) | 16 | 2 | 18, 20 |  |
| 0x88f095 | swe_first.cpp | SweFirst::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x88f095 | swe_first.cpp | SweFirst::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x88f09c | swe_first.cpp | SweFirst::set_up_rules | promotion play-off places (+0xbf) | 1 | 4 | 19 |  |
| 0x88f0a0 | swe_first.cpp | SweFirst::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 |  |
| 0x88f09a | swe_first.cpp | SweFirst::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x88f09a | swe_first.cpp | SweFirst::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x88f139 | swe_first.cpp | SweFirst::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x890446 | swe_prm.cpp | swe_prm_alloc_and_load_players | club count (+0x3e) | 14 | 2 | 18, 20 |  |
| 0x890557 | swe_prm.cpp | swe_prm::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x890560 | swe_prm.cpp | swe_prm::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x890560 | swe_prm.cpp | swe_prm::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x89055c | swe_prm.cpp | swe_prm::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 |  |
| 0x890557 | swe_prm.cpp | swe_prm::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x890555 | swe_prm.cpp | swe_prm::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x8905df | swe_prm.cpp | swe_prm::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x891774 | swe_second.cpp | swe_second_ctor | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register; outside set_up_rules |
| 0x8917ab | swe_second.cpp | swe_second_ctor | direct promotion places (+0xbe) | 0 | 1 | 18 | outside set_up_rules |
| 0x891729 | swe_second.cpp | swe_second_ctor | promotion play-off places (+0xbf) | 1 | 4 | 19 | shared register; outside set_up_rules |
| 0x891729 | swe_second.cpp | swe_second_ctor | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register; outside set_up_rules |
| 0x891774 | swe_second.cpp | swe_second_ctor | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register; outside set_up_rules |
| 0x8921c6 | swe_second.cpp | swe_second_fill_team_slots | club count (+0x3e) | 12 | 2 | 18, 20 |  |
| 0x892385 | swe_second.cpp | swe_second::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x89238e | swe_second.cpp | swe_second::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 |  |
| 0x89238a | swe_second.cpp | swe_second::set_up_rules | promotion play-off places (+0xbf) | 1 | 4 | 19 | shared register |
| 0x89238a | swe_second.cpp | swe_second::set_up_rules | relegation play-off places (+0xc0) | 1 | 4 | 19 | shared register |
| 0x892385 | swe_second.cpp | swe_second::set_up_rules | direct relegation places (+0xc1) | 2 | 4 | 18 | shared register |
| 0x8bf3b4 | transfer_manager.cpp | TRANSFER_MANAGER::add_config_future_transfer | not a competition field | n/a | n/a | none | not a competition: a byte of a transfer record (name check 2026-10-16) |
| 0x8fc41c | tur_cup.cpp | Tur_cup::Tur_cup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x8fc420 | tur_cup.cpp | Tur_cup::Tur_cup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x8fcbb0 | tur_cup.cpp | tur_cup::build_cup_roster_list | club count (+0x3e) | 64 | 2 | 21 |  |
| 0x8fddd8 | tur_first.cpp | tur_first_setup_cup_entrants | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x8fdf94 | tur_first.cpp | tur_first::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x8fdf9a | tur_first.cpp | tur_first::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x8fdf9a | tur_first.cpp | tur_first::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x8fdf9a | tur_first.cpp | tur_first::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x8fdf99 | tur_first.cpp | tur_first::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x8fdf99 | tur_first.cpp | tur_first::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x8fe023 | tur_first.cpp | tur_first::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x8ff166 | tur_second.cpp | tur_second_alloc_and_fill_cup_slots | club count (+0x3e) | 20 | 2 | 18, 20 |  |
| 0x8ff284 | tur_second.cpp | tur_second::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x8ff289 | tur_second.cpp | tur_second::set_up_rules | direct promotion places (+0xbe) | 3 | 1 | 18 | shared register |
| 0x8ff28a | tur_second.cpp | tur_second::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x8ff28a | tur_second.cpp | tur_second::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x8ff289 | tur_second.cpp | tur_second::set_up_rules | direct relegation places (+0xc1) | 3 | 1 | 18 | shared register |
| 0x8ff289 | tur_second.cpp | tur_second::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 | shared register |
| 0x8ff315 | tur_second.cpp | tur_second::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x9002d6 | tur_second_b.cpp | tur_second_b_fill_qualifiers | club count (+0x3e) | 10 | 2 | 18, 20 |  |
| 0x900464 | tur_second_b.cpp | tur_second_b::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 | shared register |
| 0x90046a | tur_second_b.cpp | tur_second_b::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x900464 | tur_second_b.cpp | tur_second_b::set_up_rules | promotion play-off places (+0xbf) | 2 | 4 | 19 | shared register |
| 0x90046a | tur_second_b.cpp | tur_second_b::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x90046a | tur_second_b.cpp | tur_second_b::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x900469 | tur_second_b.cpp | tur_second_b::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x9004e2 | tur_second_b.cpp | tur_second_b::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x902351 | uefa_cup.cpp | UefaCup::UefaCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x902321 | uefa_cup.cpp | UefaCup::UefaCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x902bab | uefa_cup.cpp | UefaCup::build_entrants | club count (+0x3e) | 145 | 2 | 21 |  |
| 0x902ebf | uefa_cup.cpp | UefaCup::build_entrants | club count (+0x3e) | unknown | ? | 21 | store 0x902ebf: value from a register, read the code |
| 0x90704a | usa_mls.cpp | usa_mls_cup_ctor | direct promotion places (+0xbe) | 1 | 1 | 18 | outside set_up_rules |
| 0x906fb7 | usa_mls.cpp | usa_mls_cup_ctor | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register; outside set_up_rules |
| 0x906fb7 | usa_mls.cpp | usa_mls_cup_ctor | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register; outside set_up_rules |
| 0x906fb7 | usa_mls.cpp | usa_mls_cup_ctor | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register; outside set_up_rules |
| 0x907078 | usa_mls.cpp | usa_mls_cup_ctor | meetings per pair (+0x3c) | 4 | 2 | 19, 20 | outside set_up_rules |
| 0x908576 | usa_mls.cpp | usa_mls_fill_league_teams | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x908661 | usa_mls.cpp | usa_mls::set_up_rules | promotion play-off places (+0xbf) | 2 | 1 | 19 | shared register |
| 0x908661 | usa_mls.cpp | usa_mls::set_up_rules | relegation play-off places (+0xc0) | 2 | 1 | 19 | shared register |
| 0x908661 | usa_mls.cpp | usa_mls::set_up_rules | direct relegation places (+0xc1) | 2 | 1 | 18 | shared register |
| 0x908663 | usa_mls.cpp | usa_mls::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 |  |
| 0x9086cf | usa_mls.cpp | usa_mls::set_up_rules | meetings per pair (+0x3c) | 4 | 2 | 19, 20 |  |
| 0x90a550 | usa_mls_all_stars.cpp | usa_mls_all_stars_init_teams | club count (+0x3e) | 2 | 2 | 18, 20 |  |
| 0x90b548 | usa_open_cup.cpp | usa_open_cup::build_cup_entrants | club count (+0x3e) | 32 | 2 | 21 |  |
| 0x90f295 | wc_african_cup.cpp | WcAfricanCup::WcAfricanCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x90f299 | wc_african_cup.cpp | WcAfricanCup::WcAfricanCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x910137 | wc_african_cup.cpp | WcAfricanCup::build_first_round_entries | club count (+0x3e) | 50 | 2 | 21 |  |
| 0x910143 | wc_african_cup.cpp | WcAfricanCup::build_first_round_entries | club count (+0x3e) | 48 | 2 | 21 |  |
| 0x91014b | wc_african_cup.cpp | WcAfricanCup::build_first_round_entries | club count (+0x3e) | 46 | 2 | 21 |  |
| 0x911c64 | wc_asia_league.cpp | WcAsiaLeague::WcAsiaLeague | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x911c68 | wc_asia_league.cpp | WcAsiaLeague::WcAsiaLeague | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x912ae0 | wc_asia_league.cpp | WcAsiaLeague::init_first_group_table | club count (+0x3e) | unknown | ? | 18, 20 | store 0x912ae0: value from a register, read the code |
| 0x912ecd | wc_asia_league.cpp | WcAsiaLeague::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x912f17 | wc_asia_league.cpp | WcAsiaLeague::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x912ee5 | wc_asia_league.cpp | WcAsiaLeague::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x912ee5 | wc_asia_league.cpp | WcAsiaLeague::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x912ee5 | wc_asia_league.cpp | WcAsiaLeague::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x914a2b | wc_concacaf_cup.cpp | WcConcacafCup::WcConcacafCup | named substitutes (+0x49) | 7 | 4 | 17 |  |
| 0x914a75 | wc_concacaf_cup.cpp | WcConcacafCup::WcConcacafCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x915d59 | wc_concacaf_cup.cpp | WcConcacafCup::build_team_table | club count (+0x3e) | unknown | ? | 21 | store 0x915d59: value from a register, read the code |
| 0x9190e0 | wc_europe_league.cpp | WcEuropeLeague::init_own_group_table | club count (+0x3e) | unknown | ? | 18, 20 | store 0x9190e0: value from a register, read the code |
| 0x919202 | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x9191fd | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x919252 | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x919212 | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x919212 | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x919212 | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x919268 | wc_europe_league.cpp | WcEuropeLeague::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x91baf7 | wc_europe_league.cpp | WcEuropeLeague::load_2002_campaign | club count (+0x3e) | unknown | ? | 18, 20 | store 0x91baf7: value from a register, read the code |
| 0x91ec12 | wc_oceania_league.cpp | WcOceaniaLeague::init_group_table | club count (+0x3e) | 5 | 2 | 18, 20 |  |
| 0x91ed0d | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x91ed3c | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x91ed59 | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x91ed10 | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x91ed10 | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x91ed10 | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x91ed6f | wc_oceania_league.cpp | WcOceaniaLeague::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x920f23 | wc_south_american_league.cpp | WcSouthAmericanLeague::build_team_slots | club count (+0x3e) | unknown | ? | 18, 20 | store 0x920f23: value from a register, read the code |
| 0x921087 | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x9210ad | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | direct promotion places (+0xbe) | unknown | ? | 18 | store 0x9210ad: value from a register, read the code |
| 0x9210ef | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | promotion play-off places (+0xbf) | 1 | 1 | 19 |  |
| 0x92105e | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x92105e | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x9210ff | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x9210ac | wc_south_american_league.cpp | WcSouthAmericanLeague::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x924ac8 | wel_cup.cpp | wel_cup_select_candidates_alloc | club count (+0x3e) | 32 | 2 | 21 |  |
| 0x925b46 | wel_first.cpp | wel_first_alloc_and_fill_league_slots | club count (+0x3e) | 18 | 2 | 18, 20 |  |
| 0x925c53 | wel_first.cpp | wel_first::set_up_rules | direct promotion places (+0xbe) | 0 | 2 | 18 | shared register |
| 0x925c53 | wel_first.cpp | wel_first::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x925c53 | wel_first.cpp | wel_first::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x925c53 | wel_first.cpp | wel_first::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x925c59 | wel_first.cpp | wel_first::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x925c5b | wel_first.cpp | wel_first::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 19, 20 |  |
| 0x925cd4 | wel_first.cpp | wel_first::set_up_rules | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x925ee4 | wel_lge_cup.cpp | wel_lge_cup_ctor | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x925ee8 | wel_lge_cup.cpp | wel_lge_cup_ctor | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x926712 | wel_lge_cup.cpp | wel_lge_cup_init_teams | club count (+0x3e) | 3 | 2 | 21 |  |
| 0x92681c | wel_lge_cup.cpp | wel_lge_cup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x92681c | wel_lge_cup.cpp | wel_lge_cup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x92681c | wel_lge_cup.cpp | wel_lge_cup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x926807 | wel_lge_cup.cpp | wel_lge_cup::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 21 |  |
| 0x92680c | wel_lge_cup.cpp | wel_lge_cup::set_up_rules | direct promotion places (+0xbe) | 1 | 4 | 18 |  |
| 0x927844 | wel_prm_cup.cpp | wel_prm_cup_screen::construct | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x927848 | wel_prm_cup.cpp | wel_prm_cup_screen::construct | named substitutes (+0x49) | 5 | 1 | 17 |  |
| 0x928072 | wel_prm_cup.cpp | wel_prm_cup_alloc_teams | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x928176 | wel_prm_cup.cpp | wel_prm_cup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x928176 | wel_prm_cup.cpp | wel_prm_cup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x928176 | wel_prm_cup.cpp | wel_prm_cup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x928166 | wel_prm_cup.cpp | wel_prm_cup::set_up_rules | meetings per pair (+0x3c) | 2 | 4 | 21 | shared register |
| 0x928166 | wel_prm_cup.cpp | wel_prm_cup::set_up_rules | direct promotion places (+0xbe) | 2 | 4 | 18 | shared register |
| 0x929ab2 | world_club_champ.cpp | WorldClubChamp::draw_first_group | club count (+0x3e) | 4 | 2 | 18, 20 |  |
| 0x929bbc | world_club_champ.cpp | WorldClubChamp::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x929be5 | world_club_champ.cpp | WorldClubChamp::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 19, 20 |  |
| 0x929c09 | world_club_champ.cpp | WorldClubChamp::set_up_rules | direct promotion places (+0xbe) | 1 | 1 | 18 |  |
| 0x929bb9 | world_club_champ.cpp | WorldClubChamp::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x929bb9 | world_club_champ.cpp | WorldClubChamp::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x929bb9 | world_club_champ.cpp | WorldClubChamp::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x929c1f | world_club_champ.cpp | WorldClubChamp::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x92b521 | world_club_cup.cpp | WorldClubCup::WorldClubCup | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x92b4f1 | world_club_cup.cpp | WorldClubCup::WorldClubCup | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x92b8c6 | world_club_cup.cpp | WorldClubCup::pick_qualifiers | club count (+0x3e) | 2 | 2 | 21 |  |
| 0x92bf8a | world_cup.cpp | WorldCup::WorldCup | club count (+0x3e) | 0 | 2 | 21 |  |
| 0x92d102 | world_cup.cpp | WorldCup::create_group_table | club count (+0x3e) | 4 | 2 | 21 |  |
| 0x92d21d | world_cup.cpp | WorldCup::set_up_rules | direct promotion places (+0xbe) | 2 | 1 | 18 |  |
| 0x92d220 | world_cup.cpp | WorldCup::set_up_rules | substitutes used (+0x4a) | 3 | 1 | 17 |  |
| 0x92d251 | world_cup.cpp | WorldCup::set_up_rules | meetings per pair (+0x3c) | 1 | 2 | 21 |  |
| 0x92d240 | world_cup.cpp | WorldCup::set_up_rules | promotion play-off places (+0xbf) | 0 | 2 | 19 | shared register |
| 0x92d240 | world_cup.cpp | WorldCup::set_up_rules | relegation play-off places (+0xc0) | 0 | 2 | 19 | shared register |
| 0x92d240 | world_cup.cpp | WorldCup::set_up_rules | direct relegation places (+0xc1) | 0 | 2 | 18 | shared register |
| 0x92d28f | world_cup.cpp | WorldCup::set_up_rules | named substitutes (+0x49) | 7 | 1 | 17 |  |
| 0x92d33a | world_cup.cpp | WorldCup::start_new_season | club count (+0x3e) | 0 | 2 | 21 |  |

## 6. Unknowns

- Where the game decides that a player has no work permit. Not traced.
- The meaning of RulingBody +6 (values 1 to 17) and of restriction bytes +6 and +8.
- The competition at `[0x9cf8d0]` in the Turkish filler: no name in the key id chains.
- Holland, Ireland, Denmark, France and USA booking rules: taken from compare constants, not read line by line.
- +0x3c as "meetings per pair" is an inference.
- The 51 gap functions that belong to other files (award_manager, cash, gui_utils, Langlib, notes, printouts, key_nation, fix_man, scout_manager, squad_manager, the board request screens near belgium_rules) keep the names of the earlier sweep. The JSON lines mark them "not rules code".

## Appendix A. Ruling body id of each competition (`comp+0x50`)

A scan of all game functions for `mov byte [reg+0x50], value`. The Comp base constructor `0x48c580` sets 0xff first (`0x48c5e2`). A new competition that 0.6.0 adds (slice 20) must set this byte to the body of its nation, or bans in it will not count. Rows that are not competition objects may appear: check the function.

| Store | File | Function | Body id | Body | Value byte |
|---|---|---|---|---|---|
| 0x40106c | african_nations.cpp | AfricanNations::AfricanNations | bl | read the code | register |
| 0x4042a1 | arg_prm.cpp | ArgPrm::ArgPrm | 0x3 | Argentina | 0x4042a4 |
| 0x406c41 | arg_second.cpp | ArgSecond::ArgSecond | 0x3 | Argentina | 0x406c44 |
| 0x40ab65 | asia_club_champ.cpp | AsiaClubChamp::AsiaClubChamp | 0x29 | Asia | 0x40ab68 |
| 0x40cc65 | asia_cup_winner.cpp | AsiaCupWinner::AsiaCupWinner | 0x29 | Asia | 0x40cc68 |
| 0x40e8ce | asia_nations.cpp | AsiaNations::AsiaNations | bl | read the code | register |
| 0x4106c1 | asia_super_cup.cpp | AsiaSuperCup::AsiaSuperCup | 0x29 | Asia | 0x4106c4 |
| 0x410ee1 | aus_nsl.cpp | AusNsl::AusNsl | 0x22 | Australia | 0x410ee4 |
| 0x41d0c2 | bel_fa_cup.cpp | BelFaCup::BelFaCup | al | read the code | register |
| 0x41dd99 | bel_first.cpp | BelFirst::BelFirst | 0x4 | Belgian league | 0x41dd9c |
| 0x41f089 | bel_second.cpp | BelSecond::BelSecond | 0x4 | Belgian league | 0x41f08c |
| 0x420f61 | bel_super.cpp | BelSuper::BelSuper | 0x5 | Belgian cup | 0x420f64 |
| 0x421741 | bel_third.cpp | BelThird::BelThird | 0x4 | Belgian league | 0x421744 |
| 0x4259f9 | bra_champ_cup.cpp | BraChampCup::BraChampCup | 0x6 | Brazilian national | 0x4259fc |
| 0x427499 | bra_cup.cpp | BraCup::BraCup | 0x6 | Brazilian national | 0x42749c |
| 0x4298a1 | bra_nat_first.cpp | BraNatFirst::BraNatFirst | 0x6 | Brazilian national | 0x4298a4 |
| 0x42be91 | bra_nat_second.cpp | BraNatSecond::BraNatSecond | 0x6 | Brazilian national | 0x42be94 |
| 0x42d317 | bra_nat_third.cpp | BraNatThird::BraNatThird | 0x7 | Brazilian regional | 0x42d31a |
| 0x42f321 | bra_reg_bahia.cpp | BraRegBahia::BraRegBahia | 0x7 | Brazilian regional | 0x42f324 |
| 0x430501 | bra_reg_central.cpp | BraRegCentral::BraRegCentral | 0x7 | Brazilian regional | 0x430504 |
| 0x431811 | bra_reg_gaucho.cpp | BraRegGaucho::BraRegGaucho | 0x7 | Brazilian regional | 0x431814 |
| 0x432ba1 | bra_reg_goias.cpp | BraRegGoias::BraRegGoias | 0x7 | Brazilian regional | 0x432ba4 |
| 0x433da1 | bra_reg_minas_gerais.cpp | BraRegMinasGerais::BraRegMinasGerais | 0x7 | Brazilian regional | 0x433da4 |
| 0x435391 | bra_reg_north.cpp | BraRegNorth::BraRegNorth | 0x7 | Brazilian regional | 0x435394 |
| 0x436441 | bra_reg_northeast.cpp | BraRegNortheast::BraRegNortheast | 0x7 | Brazilian regional | 0x436444 |
| 0x437451 | bra_reg_parana.cpp | BraRegParana::BraRegParana | 0x7 | Brazilian regional | 0x437454 |
| 0x438941 | bra_reg_pern.cpp | BraRegPern::BraRegPern | 0x7 | Brazilian regional | 0x438944 |
| 0x439a91 | bra_reg_rio.cpp | BraRegRio::BraRegRio | 0x7 | Brazilian regional | 0x439a94 |
| 0x43b181 | bra_reg_santa.cpp | BraRegSanta::BraRegSanta | 0x7 | Brazilian regional | 0x43b184 |
| 0x43c2d1 | bra_reg_sp.cpp | BraRegSp::BraRegSp | 0x7 | Brazilian regional | 0x43c2d4 |
| 0x48c5e2 | comp.cpp | Comp::Comp | dl | read the code | register |
| 0x4c0ca9 | con_champ.cpp | ConChamp::ConChamp | 0xff | ? | 0x4c0cac |
| 0x4c1741 | con_merc_cup.cpp | ConMercCup::ConMercCup | 0xff | ? | 0x4c1744 |
| 0x4c2fa1 | conmebol_liber.cpp | ConmebolLiber::ConmebolLiber | 0x2 | South America | 0x4c2fa4 |
| 0x4c4711 | conmebol_merc.cpp | ConmebolMerc::ConmebolMerc | 0x2 | South America | 0x4c4714 |
| 0x4cc707 | contract_manager.cpp | CONTRACT_MANAGER::add_contract_offer | 0x2 | not a competition: a contract offer record (name check 2026-10-16) | 0x4cc70a |
| 0x4cc70d | contract_manager.cpp | CONTRACT_MANAGER::add_contract_offer | 0x0 | not a competition: a contract offer record (name check 2026-10-16) | 0x4cc710 |
| 0x4cecfd | contract_manager.cpp | CONTRACT_MANAGER::resolve_contract_offers | 0x2 | not a competition: a contract offer record (name check 2026-10-16) | 0x4ced00 |
| 0x4ced1b | contract_manager.cpp | CONTRACT_MANAGER::resolve_contract_offers | 0x1 | not a competition: a contract offer record (name check 2026-10-16) | 0x4ced1e |
| 0x513e61 | cro_a1.cpp | CroA1::CroA1 | 0x21 | Croatia | 0x513e64 |
| 0x515361 | cro_a2a.cpp | CroA2a::CroA2a | 0x21 | Croatia | 0x515364 |
| 0x516481 | cro_a2b.cpp | CroA2b::CroA2b | 0x21 | Croatia | 0x516484 |
| 0x5174f1 | cro_cup.cpp | CroCup::CroCup | 0x21 | Croatia | 0x5174f4 |
| 0x53a747 | Database.cpp | player_build_skill_vector | dl | read the code | register |
| 0x550a19 | den_cup.cpp | DenCup::DenCup | 0x8 | Denmark | 0x550a1c |
| 0x551741 | den_first.cpp | DenFirst::DenFirst | 0x8 | Denmark | 0x551744 |
| 0x552625 | den_prm.cpp | DenPrm::DenPrm | 0x8 | Denmark | 0x552628 |
| 0x553ed1 | den_second.cpp | DenSecond::DenSecond | 0x8 | Denmark | 0x553ed4 |
| 0x56aab1 | eng_auto_cup.cpp | EngAutoCup::EngAutoCup | 0x9 | England | 0x56aab4 |
| 0x56c309 | eng_cc_cup.cpp | EngCcCup::EngCcCup | 0x9 | England | 0x56c30c |
| 0x56d3f1 | eng_charity.cpp | EngCharity::EngCharity | 0x9 | England | 0x56d3f4 |
| 0x56dbb1 | eng_conf.cpp | EngConf::EngConf | 0x9 | England | 0x56dbb4 |
| 0x56f0a9 | eng_fa_cup.cpp | EngFaCup::EngFaCup | 0x9 | England | 0x56f0ac |
| 0x570c71 | eng_fa_trophy.cpp | EngFaTrophy::EngFaTrophy | 0x9 | England | 0x570c74 |
| 0x5716d1 | eng_first.cpp | EngFirst::EngFirst | 0x9 | England | 0x5716d4 |
| 0x5733d6 | eng_prm.cpp | EngPrm::EngPrm | 0x9 | England | 0x5733d9 |
| 0x5754f1 | eng_second.cpp | EngSecond::EngSecond | 0x9 | England | 0x5754f4 |
| 0x576e33 | eng_third.cpp | EngThird::EngThird | 0x9 | England | 0x576e36 |
| 0x57a175 | eur_super_cup.cpp | EurSuperCup::EurSuperCup | 0x1 | Europe | 0x57a178 |
| 0x57a8fd | euro_champ.cpp | EuroChamp::EuroChamp | 0x0 | International | 0x57a900 |
| 0x57ce9f | euro_champ_qual.cpp | EuroChampQual::EuroChampQual | bl | read the code | register |
| 0x58384a | european_cup.cpp | EuropeanCup::EuropeanCup | 0x1 | Europe | 0x58384d |
| 0x58b396 | fifa_confed.cpp | FifaConfed::FifaConfed | bl | read the code | register |
| 0x5922c1 | fin_cup.cpp | FinCup::FinCup | 0xb | Finnish cup | 0x5922c4 |
| 0x592f01 | fin_first.cpp | FinFirst::FinFirst | 0xa | Finnish league | 0x592f04 |
| 0x5951c1 | fin_prm.cpp | FinPrm::FinPrm | 0xa | Finnish league | 0x5951c4 |
| 0x5b922a | fra_cfa.cpp | FraCfa::FraCfa | al | read the code | register |
| 0x5ba1a1 | fra_cup.cpp | FraCup::FraCup | 0xc | France | 0x5ba1a4 |
| 0x5bb1b4 | fra_first.cpp | FraFirst::FraFirst | 0xc | France | 0x5bb1b7 |
| 0x5bb1fd | fra_first.cpp | FraFirst::FraFirst | 0xff | ? | 0x5bb200 |
| 0x5bb25c | fra_first.cpp | FraFirst::FraFirst | 0xff | ? | 0x5bb25f |
| 0x5bc9f1 | fra_lge_cup.cpp | FraLgeCup::FraLgeCup | 0xc | France | 0x5bc9f4 |
| 0x5bd669 | fra_lower.cpp | FraLower::FraLower | al | read the code | register |
| 0x5be381 | fra_second.cpp | FraSecond::FraSecond | 0xc | France | 0x5be384 |
| 0x5bf389 | fra_super.cpp | FraSuper::FraSuper | 0xc | France | 0x5bf38c |
| 0x5bfb91 | fra_third.cpp | FraThird::FraThird | 0xc | France | 0x5bfb94 |
| 0x5d8c91 | ger_cup.cpp | GerCup::GerCup | 0xe | German cup | 0x5d8c94 |
| 0x5d9a01 | ger_first.cpp | GerFirst::GerFirst | 0xd | German league | 0x5d9a04 |
| 0x5dc149 | ger_lge_cup.cpp | GerLgeCup::GerLgeCup | 0xd | German league | 0x5dc14c |
| 0x5dcc21 | ger_regional.cpp | GerRegional::GerRegional | 0xd | German league | 0x5dcc24 |
| 0x5de331 | ger_second.cpp | GerSecond::GerSecond | 0xd | German league | 0x5de334 |
| 0x5e06ce | goldcup.cpp | GoldCup::GoldCup | bl | read the code | register |
| 0x5e88f9 | gre_cup.cpp | GreCup::GreCup | 0x20 | Greece | 0x5e88fc |
| 0x5ea461 | gre_prm.cpp | GrePrm::GrePrm | 0x20 | Greece | 0x5ea464 |
| 0x5eb301 | gre_second.cpp | GreSecond::GreSecond | 0x20 | Greece | 0x5eb304 |
| 0x5ec0b1 | gre_super.cpp | GreSuper::GreSuper | 0x20 | Greece | 0x5ec0b4 |
| 0x5f34af | hol_cup.cpp | HolCup::HolCup | 0xf | Holland | 0x5f34b2 |
| 0x5f54e1 | hol_first.cpp | HolFirst::HolFirst | 0xf | Holland | 0x5f54e4 |
| 0x5f7661 | hol_prm.cpp | HolPrm::HolPrm | 0xf | Holland | 0x5f7664 |
| 0x5f8889 | hol_super.cpp | HolSuper::HolSuper | 0xf | Holland | 0x5f888c |
| 0x6320f5 | inter_amer_cup.cpp | InterAmerCup::InterAmerCup | 0xff | ? | 0x6320f8 |
| 0x63302b | intertoto_cup.cpp | IntertotoCup::IntertotoCup | 0x1 | Europe | 0x63302e |
| 0x634111 | ire_chal_cup.cpp | IreChalCup::IreChalCup | 0x10 | Ireland | 0x634114 |
| 0x634c91 | ire_first.cpp | IreFirst::IreFirst | 0x10 | Ireland | 0x634c94 |
| 0x636511 | ire_leinster_cup.cpp | IreLeinsterCup::IreLeinsterCup | 0x10 | Ireland | 0x636514 |
| 0x636e29 | ire_lge_cup.cpp | IreLgeCup::IreLgeCup | 0x10 | Ireland | 0x636e2c |
| 0x638811 | ire_munster_cup.cpp | IreMunsterCup::IreMunsterCup | 0x10 | Ireland | 0x638814 |
| 0x6392e9 | ire_pres_cup.cpp | IrePresCup::IrePresCup | 0x10 | Ireland | 0x6392ec |
| 0x639bfd | ire_prm.cpp | IrePrm::IrePrm | 0x10 | Ireland | 0x639c00 |
| 0x63ad29 | ire_super_cup.cpp | IreSuperCup::IreSuperCup | 0x10 | Ireland | 0x63ad2c |
| 0x63c651 | ita_c1_super.cpp | ItaC1Super::ItaC1Super | 0x11 | Italian league | 0x63c654 |
| 0x63cc99 | ita_c_cup.cpp | ItaCCup::ItaCCup | 0x12 | Italian cup | 0x63cc9c |
| 0x63e959 | ita_cup.cpp | ItaCup::ItaCup | 0x12 | Italian cup | 0x63e95c |
| 0x640721 | ita_ser_a.cpp | ItaSerA::ItaSerA | 0x11 | Italian league | 0x640724 |
| 0x645861 | ita_ser_b.cpp | ItaSerB::ItaSerB | 0x11 | Italian league | 0x645864 |
| 0x64aac1 | ita_ser_c1a.cpp | ItaSerC1a::ItaSerC1a | 0x11 | Italian league | 0x64aac4 |
| 0x64ec71 | ita_ser_c1b.cpp | ItaSerC1b::ItaSerC1b | 0x11 | Italian league | 0x64ec74 |
| 0x652ee1 | ita_ser_c2a.cpp | ItaSerC2a::ItaSerC2a | 0x11 | Italian league | 0x652ee4 |
| 0x6573a1 | ita_ser_c2b.cpp | ItaSerC2b::ItaSerC2b | 0x11 | Italian league | 0x6573a4 |
| 0x65b531 | ita_ser_c2c.cpp | ItaSerC2c::ItaSerC2c | 0x11 | Italian league | 0x65b534 |
| 0x65f6e1 | ita_super.cpp | ItaSuper::ItaSuper | 0x11 | Italian league | 0x65f6e4 |
| 0x661da1 | jap_emp_cup.cpp | JapEmpCup::JapEmpCup | 0x14 | Japanese cup | 0x661da4 |
| 0x662b81 | jap_j1.cpp | JapJ1::JapJ1 | 0x13 | Japanese league | 0x662b84 |
| 0x664511 | jap_j2.cpp | JapJ2::JapJ2 | 0x13 | Japanese league | 0x664514 |
| 0x6653f1 | jap_j_cup.cpp | JapJCup::JapJCup | 0x14 | Japanese cup | 0x6653f4 |
| 0x665dd9 | jap_super.cpp | JapSuper::JapSuper | 0x14 | Japanese cup | 0x665ddc |
| 0x66a4c9 | kor_fa_cup.cpp | KorFaCup::KorFaCup | 0x2a | Korea | 0x66a4cc |
| 0x66aec1 | kor_league.cpp | KorLeague::KorLeague | 0x2a | Korea | 0x66aec4 |
| 0x66bf59 | kor_league_cup.cpp | KorLeagueCup::KorLeagueCup | 0x2a | Korea | 0x66bf5c |
| 0x66d891 | kor_super_cup.cpp | KorSuperCup::KorSuperCup | 0x2a | Korea | 0x66d894 |
| 0x6824bb | league.cpp | League::copy_for_display | dl | read the code | register |
| 0x789e61 | nir_charity.cpp | NirCharity::NirCharity | 0x24 | Northern Ireland | 0x789e64 |
| 0x78a621 | nir_cup.cpp | NirCup::NirCup | 0x24 | Northern Ireland | 0x78a624 |
| 0x78b061 | nir_first.cpp | NirFirst::NirFirst | 0x24 | Northern Ireland | 0x78b064 |
| 0x78c7d9 | nir_lge_cup.cpp | NirLgeCup::NirLgeCup | 0x24 | Northern Ireland | 0x78c7dc |
| 0x78e0c1 | nir_prm.cpp | NirPrm::NirPrm | 0x24 | Northern Ireland | 0x78e0c4 |
| 0x78f091 | nor_cup.cpp | NorCup::NorCup | 0x16 | Norwegian cup | 0x78f094 |
| 0x78fd91 | nor_first.cpp | NorFirst::NorFirst | 0x15 | Norwegian league | 0x78fd94 |
| 0x791551 | nor_prm.cpp | NorPrm::NorPrm | 0x15 | Norwegian league | 0x791554 |
| 0x797062 | oceania_club_champ.cpp | OceaniaClubChamp::OceaniaClubChamp | 0x23 | Oceania | 0x797065 |
| 0x798913 | oceania_nations.cpp | OceaniaNations::OceaniaNations | bl | read the code | register |
| 0x79f59e | olympics.cpp | Olympics::Olympics | bl | read the code | register |
| 0x7c7df1 | pol_cup.cpp | PolCup::PolCup | 0x1f | Poland | 0x7c7df4 |
| 0x7c8ad7 | pol_first.cpp | PolFirst::PolFirst | 0x1f | Poland | 0x7c8ada |
| 0x7ca549 | pol_lge_cup.cpp | PolLgeCup::PolLgeCup | 0x1f | Poland | 0x7ca54c |
| 0x7cb221 | pol_second.cpp | PolSecond::PolSecond | 0x1f | Poland | 0x7cb224 |
| 0x7cc991 | pol_super.cpp | PolSuper::PolSuper | 0x1f | Poland | 0x7cc994 |
| 0x7cd9e9 | por_cup.cpp | PorCup::PorCup | 0x17 | Portugal | 0x7cd9ec |
| 0x7ce771 | por_prm.cpp | PorPrm::PorPrm | 0x17 | Portugal | 0x7ce774 |
| 0x7d09a1 | por_second.cpp | PorSecond::PorSecond | 0x17 | Portugal | 0x7d09a4 |
| 0x7d1971 | por_second_b.cpp | PorSecondB::PorSecondB | 0x17 | Portugal | 0x7d1974 |
| 0x7d2ea9 | por_super.cpp | PorSuper::PorSuper | 0x17 | Portugal | 0x7d2eac |
| 0x7e9281 | rus_cup.cpp | RusCup::RusCup | 0x27 | Russia | 0x7e9284 |
| 0x7e9f81 | rus_first.cpp | RusFirst::RusFirst | 0x27 | Russia | 0x7e9f84 |
| 0x7eaad1 | rus_prm.cpp | RusPrm::RusPrm | 0x27 | Russia | 0x7eaad4 |
| 0x7ec9f1 | sco_chal_cup.cpp | ScoChalCup::ScoChalCup | 0x19 | Scottish cup | 0x7ec9f4 |
| 0x7ed381 | sco_fa_cup.cpp | ScoFaCup::ScoFaCup | 0x19 | Scottish cup | 0x7ed384 |
| 0x7ee011 | sco_first.cpp | ScoFirst::ScoFirst | 0x18 | Scottish league | 0x7ee014 |
| 0x7efa51 | sco_lge_cup.cpp | ScoLgeCup::ScoLgeCup | 0x19 | Scottish cup | 0x7efa54 |
| 0x7f0be4 | sco_prm.cpp | ScoPrm::ScoPrm | 0x18 | Scottish league | 0x7f0be7 |
| 0x7f32a1 | sco_second.cpp | ScoSecond::ScoSecond | 0x18 | Scottish league | 0x7f32a4 |
| 0x7f4241 | sco_third.cpp | ScoThird::ScoThird | 0x18 | Scottish league | 0x7f4244 |
| 0x84cfc1 | spa_cup.cpp | SpaCup::SpaCup | 0x1b | Spanish cup | 0x84cfc4 |
| 0x84e181 | spa_first.cpp | SpaFirst::SpaFirst | 0x1a | Spanish league | 0x84e184 |
| 0x85107c | spa_lower.cpp | SpaLower::SpaLower | 0xff | ? | 0x85107f |
| 0x8539f1 | spa_second.cpp | SpaSecond::SpaSecond | 0x1a | Spanish league | 0x8539f4 |
| 0x855f91 | spa_second_b.cpp | SpaSecondB::SpaSecondB | 0x1a | Spanish league | 0x855f94 |
| 0x858721 | spa_super.cpp | SpaSuper::SpaSuper | 0x1a | Spanish league | 0x858724 |
| 0x88cc25 | swe_cup.cpp | SweCup::SweCup | 0x1d | Swedish cup | 0x88cc28 |
| 0x88e631 | swe_first.cpp | SweFirst::SweFirst | 0x1c | Swedish league | 0x88e634 |
| 0x88fb61 | swe_prm.cpp | swe_prm_load_game_object | 0x1c | Swedish league | 0x88fb64 |
| 0x891746 | swe_second.cpp | swe_second_ctor | 0x1c | Swedish league | 0x891749 |
| 0x8c6e10 | transfer_manager.cpp | transfer_manager_close_offer_contract_offers | 0x4 | not a competition: a contract offer from `CONTRACT_MANAGER::find_contract_offer` 0x4cc9e0 (name check 2026-10-16) | 0x8c6e13 |
| 0x8c6e42 | transfer_manager.cpp | transfer_manager_close_offer_contract_offers | 0x4 | not a competition: a contract offer from `CONTRACT_MANAGER::find_contract_offer` 0x4cc9e0 (name check 2026-10-16) | 0x8c6e45 |
| 0x8fc421 | tur_cup.cpp | Tur_cup::Tur_cup | 0x26 | Turkish cup | 0x8fc424 |
| 0x8fcfb1 | tur_first.cpp | tur_first_screen_ctor | 0x25 | Turkish league | 0x8fcfb4 |
| 0x8fe741 | tur_second.cpp | Tournament_Screen::ctor_with_checks | 0x25 | Turkish league | 0x8fe744 |
| 0x8ff511 | tur_second_b.cpp | tur_second_b_setup_cup | 0x25 | Turkish league | 0x8ff514 |
| 0x902355 | uefa_cup.cpp | UefaCup::UefaCup | 0x1 | Europe | 0x902358 |
| 0x906fd1 | usa_mls.cpp | usa_mls_cup_ctor | 0x1e | USA | 0x906fd4 |
| 0x90a1b9 | usa_mls_all_stars.cpp | USA_MLS_All_Stars::ctor | 0xff | ? | 0x90a1bc |
| 0x90af19 | usa_open_cup.cpp | USA_Open_Cup::USA_Open_Cup | 0x1e | USA | 0x90af1c |
| 0x90f29a | wc_african_cup.cpp | WcAfricanCup::WcAfricanCup | bl | read the code | register |
| 0x911c6f | wc_asia_league.cpp | WcAsiaLeague::WcAsiaLeague | bl | read the code | register |
| 0x914a76 | wc_concacaf_cup.cpp | WcConcacafCup::WcConcacafCup | bl | read the code | register |
| 0x91828f | wc_europe_league.cpp | WcEuropeLeague::WcEuropeLeague | bl | read the code | register |
| 0x91e001 | wc_oceania_league.cpp | WcOceaniaLeague::WcOceaniaLeague | bl | read the code | register |
| 0x920320 | wc_south_american_league.cpp | WcSouthAmericanLeague::WcSouthAmericanLeague | bl | read the code | register |
| 0x924489 | wel_cup.cpp | Cup::Cup | 0x28 | Wales | 0x92448c |
| 0x924e81 | wel_first.cpp | wel_first_screen_ctor | 0x28 | Wales | 0x924e84 |
| 0x925ee9 | wel_lge_cup.cpp | wel_lge_cup_ctor | 0x28 | Wales | 0x925eec |
| 0x927849 | wel_prm_cup.cpp | wel_prm_cup_screen::construct | 0xff | ? | 0x92784c |
| 0x9291c3 | world_club_champ.cpp | WorldClubChamp::WorldClubChamp | bl | read the code | register |
| 0x92b525 | world_club_cup.cpp | WorldClubCup::WorldClubCup | 0xff | ? | 0x92b528 |
| 0x92bfc1 | world_cup.cpp | WorldCup::WorldCup | bl | read the code | register |
