# Rules table: Wales, Northern Ireland, Republic of Ireland (slice 4)

Part `wal-nir-irl` of the 0.6.0 rules table. Research of 2026-10-16. Seasons 2001/02 to 2027/28.

Files:

- `wal-nir-irl.tsv`: 675 rows. One row per competition per season, and one `nation rules` row per nation per season.
- `wal-nir-irl-clubs.tsv`: 1913 rows. Real clubs of each resized or new division at each season start that has a database.
- This file: method, sources, missing clubs, unknowns and the decisions for Max.

## Scope

The original's competitions, with the `comp` value used in the table:

| comp_file | comp | Real competition |
|---|---|---|
| wel_first | Welsh Premier Division | League of Wales (2001/02), Welsh Premier League (2002/03 to 2018/19), Cymru Premier (2019/20 on) |
| wel_cup | Welsh Cup | Welsh Cup |
| wel_lge_cup | Welsh League Cup | Welsh League Cup |
| wel_prm_cup | Welsh Premier Cup | FAW Premier Cup, last held 2007/08 |
| nir_prm | Northern Irish League Premier Division | Irish League Premier Division, Irish Premier League (2003/04), IFA Premiership (2008/09), NIFL Premiership (2013/14) |
| nir_first | Northern Irish League First Division | Irish League First Division, Irish First Division (2003/04), IFA Championship (2008/09), IFA Championship 1 (2009/10), NIFL Championship 1 (2013/14), NIFL Championship (2016/17) |
| nir_cup | Northern Irish Cup | Irish Cup |
| nir_lge_cup | Northern Irish League Cup | Irish League Cup, NIFL Cup from 2013/14 |
| nir_charity | Northern Irish Charity Shield | NIFL Charity Shield, held 2014 to 2017 and 2022 to 2026 only |
| ire_prm | Irish Premier Division | League of Ireland Premier Division |
| ire_first | Irish First Division | League of Ireland First Division |
| ire_chal_cup | Irish Senior Challenge Cup | FAI Cup |
| ire_lge_cup | Irish League Cup | League of Ireland Cup, not held since 2020 |
| ire_leinster_cup | Irish Leinster Senior Cup | Leinster Senior Cup |
| ire_munster_cup | Irish Munster Senior Cup | Munster Senior Cup |
| ire_pres_cup | Irish Presidents Cup | LFA President's Cup to 2002/03, President of Ireland's Cup from 2014 (see decisions) |
| ire_super_cup | Irish Super Cup | FAI Super Cup, last held 2001 |

New rows (`comp_file` = `new`) cover real competitions of the same tiers that the original lacks:

- Welsh second tier: Cymru Alliance and Welsh Football League Division One to 2018/19, then Cymru North and Cymru South. The original runs only one Welsh division.
- Northern Irish third tier: Irish League Second Division, Irish Second Division, IFA Interim Intermediate League, Championship 2, NIFL Premier Intermediate League, then NCL Premier from 2026/27.
- Irish third tier: A Championship (2008 to 2011) and the FAI National League (from autumn 2026).
- All-Ireland cup: Setanta Sports Cup (2005 to 2014). Its nation value is "Northern Ireland and Republic of Ireland".

## Method

1. I read each Wikipedia season page as raw wikitext through the MediaWiki API on 2026-10-16, about 500 pages. A script read the league table module of each page: the club names, the result codes and the legend text. No summary model was used.
2. `clubs` is the number of clubs in the season table. `up`, `down` and `playoffs` come from the result legend of the final table, with the table positions in brackets. They give the actual outcome of that season, so licence decisions and expulsions are part of them. Rows with such an outcome have a note. From 2024/25 some pages split the season into phase tables, and the script joined them.
3. Facts that the script could not read (splits, formats, names, future formats) I took from the season page or the competition article by hand.
4. Subs and bench come from competition rules where I found them. Otherwise they come from the match details of cup finals, marked "inference from line-ups".
5. Every filled fact has a URL. `all=URL` means that one page gives the row.

### Calendar seasons (League of Ireland)

The League of Ireland moved to a summer season in 2003. Per the brief, the 2003/04 row holds 2004 and the 2027/28 row holds 2028. Two rows differ:

- 2001/02 holds the winter season 2001-02.
- 2002/03 holds two seasons: the shortened 2002-03 season (August 2002 to January 2003, 10 clubs, 27 matches, 9th into a four-club play-off) and the first summer season of 2003. Its table data are of 2003. See decision 1.

The FAI Cup, the League of Ireland Cup and the President of Ireland's Cup of year Y are in the row that ends in Y. The Leinster and Munster Senior Cups keep their own season labels.

## Main changes

Wales:

- 2001/02 to 2009/10: one national league of 17 or 18 clubs, 1 relegated, no split.
- 2009/10: 6 clubs relegated to cut the league to 12.
- 2010/11 to 2025/26: 12 clubs. After 22 matches the top six and the bottom six split and play each other twice more (32 matches). A European play-off follows for clubs from about 3rd or 4th to 7th or 8th. Relegation needs a second-tier club with a licence, so the bottom clubs were often spared.
- 2019/20: the league becomes the Cymru Premier. The second tier becomes Cymru North and Cymru South.
- 2026/27 on: 16 clubs. After 30 matches the league splits into groups of 6, 4 and 6, and each club plays its group once more. 2 clubs go down and the 14th goes into a relegation play-off. Three clubs came up from each of Cymru North and South in 2026.
- Subs: the Cymru Premier allowed 5 subs in 2022/23 with youth substitute conditions. From 2023/24 it allows 5 subs from 5 named, or from 7 named if two are youth substitutes.
- Welsh Cup: no replays from 2004/05 (round lists). Cancelled in 2019/20 and 2020/21. 5 subs from 2022/23. The FAW Premier Cup ended in 2008.

Northern Ireland:

- 2001/02: Premier Division of 10, First Division of 10, 2 promoted.
- 2002/03: Premier Division of 12 with a split after 33 matches. 4 promoted from the First Division of 8.
- 2003/04 to 2007/08: Irish Premier League of 16, no split, 1 relegated and 1 play-off.
- 2008/09 on: 12 clubs chosen by licence and points criteria, split after 33 matches, 38 matches. 1 relegated, and the 11th plays off with the Championship runner-up.
- Second tier: 17 clubs in 2008/09, 14 as Championship 1 from 2009/10, 12 with a split from 2016/17, 16 from 2026/27 with a split after 30 matches.
- 2020/21: the Championship clubs cancelled their season, and the Premiership had no relegation.
- Irish Cup: replays appear in the round lists up to 2013/14. From 2021/22 there are four qualifying rounds and a main draw of 32. The 2023/24 rules allow 5 subs from 7 named, and a sixth in extra time.

Republic of Ireland:

- 2003: summer season. The Premier Division has 10 clubs (2003, 2004, 2009 to 2011, 2018 on) or 12 (2005 to 2008, 2012 to 2017).
- First Division sizes: 10, 12, 11, 8 (2012 to 2017), 9 (2022) or 10.
- 2008 to 2011: the A Championship, mostly reserve sides.
- 2026: the FAI National League starts as a third tier (15 clubs, truncated autumn season, no promotion). From 2027 its champion plays the bottom First Division club.
- FAI Cup: replays until 2018, none from 2019 (round lists).
- League of Ireland Cup: abandoned in 2020 and not held since.

## Main sources

- https://en.wikipedia.org/wiki/Cymru_Premier and each season page, for example https://en.wikipedia.org/wiki/2010%E2%80%9311_Welsh_Premier_League
- https://en.wikipedia.org/wiki/NIFL_Premiership, https://en.wikipedia.org/wiki/NIFL_Championship, https://en.wikipedia.org/wiki/NCL_Premier_(Northern_Ireland)
- https://en.wikipedia.org/wiki/League_of_Ireland, https://en.wikipedia.org/wiki/League_of_Ireland_Premier_Division, https://en.wikipedia.org/wiki/FAI_National_League
- Cups: https://en.wikipedia.org/wiki/Welsh_League_Cup, https://en.wikipedia.org/wiki/List_of_Welsh_Cup_finals, https://en.wikipedia.org/wiki/FAW_Premier_Cup, https://en.wikipedia.org/wiki/Northern_Ireland_Football_League_Cup, https://en.wikipedia.org/wiki/NIFL_Charity_Shield, https://en.wikipedia.org/wiki/FAI_Cup, https://en.wikipedia.org/wiki/League_of_Ireland_Cup, https://en.wikipedia.org/wiki/President_of_Ireland's_Cup, https://en.wikipedia.org/wiki/LFA_President's_Cup, https://en.wikipedia.org/wiki/FAI_Super_Cup, https://en.wikipedia.org/wiki/Setanta_Sports_Cup
- Rules: https://cambrian-news.co.uk/sport/football/airbus-uk-deducted-points-for-youth-substitutes-rule-breach-603260 (Cymru Premier Rule 13, 2022/23), https://faw.cymru/cymru-leagues/news/jd-cymru-premier-rule-changes-for-2023-24-season/ (2023/24), https://www.irishfa.com/media/48574/challenge-cup-rules-23-24.pdf (Irish Cup 2023/24), https://www.thefa.com/news/2020/dec/01/gbe-points-based-system-launched-011220 (end of free movement)

## Club moves

For each season start with a database, the script compared the real club count of each division with the database count. Where they differ, or the division is new, it listed every real club. It matched each club to the database clubs of the same nation by name. An alias list covers sponsor names and renamings, for example GAP Connah's Quay, UWIC Inter Cardiff, Newry Town and Cork City FORAS Co-op. `db_division` shows where the club is in the database.

- 144 division-seasons are listed.
- The Welsh Premier Division matches the database (18) until 2009/10, except 2003/04 (17). It differs from 2010/11 (12).
- The Northern Irish Premier Division (database 10) differs from 2002/03 on.
- The Irish Premier Division (database 12) differs in 2002/03, 2003/04, 2008/09 to 2010/11 and from 2017/18.
- Many matched clubs sit in a playable division of the database, for example a First Division club that moves up. Q244 applies only to the clubs that fill a short division. Check that rule against the `db_division` column.
- Reserve sides ("Bohemians A", "Shamrock Rovers B") get no database club.
- The name matcher is a script. Treat each match as a proposal, and check the odd ones.

### Missing clubs

These clubs of a listed division have no database club. Max decides them. No club records were added.

| Season | Division | Real clubs | Missing | Clubs |
|---|---|---|---|---|
| 2001/02 | Cymru Alliance | 18 | 3 | Llangefni Glantraeth, Llanfairpwll, Guilsfield |
| 2002/03 | Cymru Alliance | 17 | 4 | Llangefni Glantraeth, Amlwch Town, Llanfairpwll, Guilsfield |
| 2003/04 | Cymru Alliance | 17 | 3 | Guilsfield, Llanfairpwll, Amlwch Town |
| 2005/06 | Cymru Alliance | 18 | 6 | Guilsfield, Bala Town, Bodedern, Queens Park, Llanfairpwll, Llandyrnog United |
| 2007/08 | Cymru Alliance | 17 | 1 | Llanfairpwll |
| 2007/08 | A Championship | 16 | 14 | Bohemians A, Finn Harps A, Drogheda United A, Shamrock Rovers A, Sligo Rovers A, Derry City A, UCD A, Cork City A, Galway United A, Bray Wanderers A, St Patrick's Athletic A, Cobh Ramblers A, Limerick 37 A, Tullamore Town |
| 2008/09 | Cymru Alliance | 17 | 1 | Llanfairpwll |
| 2008/09 | Northern Irish League First Division | 17 | 3 | Glebe Rangers, Dergview, Killymoon Rangers |
| 2008/09 | A Championship | 18 | 14 | Drogheda United A, Finn Harps A, Derry City A, Tullamore Town, Dundalk A, Sligo Rovers A, Galway United A, Shamrock Rovers A, Bohemians A, Bray Wanderers A, Sporting Fingal A, Cork City A, St Patrick's Athletic A, F.C. Carlow |
| 2009/10 | Cymru Alliance | 17 | 4 | Bethesda Athletic, Llangollen Town, Berriew, Llanfairpwll |
| 2009/10 | Welsh Football League Division One | 18 | 4 | Aberaman Athletic, West End, Cardiff Corinthians, Bettws |
| 2009/10 | Northern Irish League First Division | 14 | 1 | Glebe Rangers |
| 2009/10 | Championship 2 | 15 | 6 | Dergview, PSNI, Queen's University, Sport & Leisure Swifts, Annagh United, Killymoon Rangers |
| 2009/10 | A Championship | 18 | 15 | Bohemians A, Sporting Fingal A, Dundalk A, Sligo Rovers A, Finn Harps A, Shelbourne A, Tullamore Town, Drogheda United A, UCD A, St Patrick's Athletic A, Shamrock Rovers A, F.C. Carlow, Limerick A, Bray Wanderers A, Galway United A |
| 2010/11 | Cymru Alliance | 16 | 1 | Rhos Aelwyd |
| 2010/11 | Welsh Football League Division One | 16 | 3 | West End, Cardiff Corinthians, Aberaman Athletic |
| 2010/11 | Northern Irish League First Division | 14 | 2 | Dergview, Glebe Rangers |
| 2010/11 | Championship 2 | 16 | 6 | Warrenpoint Town, Queen's University, Sport & Leisure Swifts, PSNI, Annagh United, Killymoon Rangers |
| 2010/11 | A Championship | 16 | 13 | Derry City A, UCD A, Sligo Rovers A, Bohemians A, Drogheda United A, Finn Harps A, Dundalk A, Bray Wanderers A, Shamrock Rovers A, St Patrick's Athletic A, F.C. Carlow, Limerick A, Galway United A |
| 2011/12 | Cymru Alliance | 16 | 3 | Penycae, Llanrhaeadr, Rhos Aelwyd |
| 2011/12 | Welsh Football League Division One | 16 | 5 | AFC Porth, West End, Aberaman Athletic, Cwmaman Institute, Cardiff Corinthians |
| 2011/12 | Northern Irish League First Division | 14 | 3 | Dergview, Warrenpoint Town, Glebe Rangers |
| 2011/12 | Championship 2 | 16 | 5 | Queen's University, Annagh United, Sport & Leisure Swifts, PSNI, Killymoon Rangers |
| 2011/12 | Irish First Division | 8 | 1 | SD Galway |
| 2012/13 | Cymru Alliance | 16 | 3 | Conwy Borough F.C., Penycae F.C., Llanrhaeadr F.C. |
| 2012/13 | Welsh Football League Division One | 15 | 5 | West End, Aberdare Town, AFC Porth, Monmouth Town, Tata Steel |
| 2012/13 | Championship 2 | 16 | 4 | Queen's University, PSNI, Annagh United, Sport & Leisure Swifts |
| 2013/14 | Cymru Alliance | 16 | 2 | Penycae, Llanrhaeadr-ym-Mochnant |
| 2013/14 | Welsh Football League Division One | 16 | 4 | Monmouth Town, AFC Porth, Tata Steel, Aberbargoed Buds |
| 2013/14 | Championship 2 | 16 | 1 | PSNI |
| 2013/14 | Irish First Division | 8 | 2 | Galway, Shamrock Rovers B |
| 2014/15 | Cymru Alliance | 16 | 2 | Penycae, Llandrindod Wells |
| 2014/15 | Welsh Football League Division One | 16 | 3 | Briton Ferry Llansawel, Garden Village, AFC Porth |
| 2014/15 | Northern Irish League First Division | 14 | 1 | PSNI |
| 2014/15 | Championship 2 | 15 | 1 | Dollingstown |
| 2014/15 | Irish First Division | 8 | 1 | Cabinteely |
| 2015/16 | Championship 2 | 14 | 1 | PSNI |
| 2016/17 | Cymru Alliance | 16 | 2 | Gresford Athletic, Llanfair United |
| 2016/17 | Welsh Football League Division One | 16 | 2 | Undy Athletic, Caldicot Town |
| 2016/17 | Northern Irish League First Division | 12 | 1 | PSNI |
| 2017/18 | Cymru Alliance | 15 | 3 | Gresford Athletic, Queens Park, Llandudno Junction |
| 2017/18 | Welsh Football League Division One | 16 | 3 | Llanelli Town, Undy Athletic, Briton Ferry Llansawel |
| 2017/18 | Northern Irish League First Division | 12 | 1 | PSNI |
| 2018/19 | Welsh Premier Division | 12 | 1 | Llanelli Town |
| 2018/19 | Cymru Alliance | 16 | 2 | Gresford Athletic, Llanrhaeadr |
| 2018/19 | Welsh Football League Division One | 16 | 3 | Llantwit Major, Briton Ferry Llansawel, Undy Athletic |
| 2019/20 | Cymru North | 17 | 5 | Llanrhaeadr-ym-Mochnant, Gresford Athletic, Llangefni Town, Corwen, Llanfair United |
| 2019/20 | Cymru South | 16 | 6 | Swansea University, Briton Ferry Llansawel, Llanelli Town, STM Sports, Llantwit Major, Undy Athletic |
| 2020/21 | Cymru North | 16 | 3 | Gresford Athletic, Llangefni Town, Llanrhaeadr |
| 2020/21 | Cymru South | 16 | 5 | Llanelli Town, Llantwit Major, Swansea University, Trefelin BGC, Undy Athletic |
| 2021/22 | Cymru South | 16 | 5 | Llantwit Major, Swansea University, Llanelli Town, Trefelin BGC, Undy Athletic |
| 2022/23 | Cymru South | 16 | 1 | Swansea University |

Divisions with no team list at all:

- Welsh Football League Division One, 2001/02 to 2008/09 and 2015/16. Cymru Alliance 2015/16. No season table was found.
- Northern Irish third tier, 2001/02 to 2008/09. No season pages were found.
- FAI National League 2026 (15 clubs). The December 2025 database predates the league, and the script could not read the page's group tables.
- Cymru North and South 2026/27 and later. League of Ireland 2027 and 2028.

## Unknowns

- Subs and bench are unknown for most league seasons of all three nations. Known values:
  - Cymru Premier 2022/23 to 2025/26.
  - Welsh Cup finals 2010 and 2011 (3 from 5), Welsh Cup 2022/23 and 2026/27 (5).
  - Irish Cup 2023/24 (5 from 7).
  - FAI Cup finals 2007 and 2008 (3 from 7, match rules).
  - Line-up inferences for FAI Cup, League of Ireland Cup, Setanta Cup and President's Cup finals.
- The IFAB made 5 subs permanent in 2022 and raised the named limit to 15. Each competition sets its own numbers, so this does not fill the rows.
- Squad registration rules are unknown for all three nations.
- Work permits: the League of Ireland rule for non-EEA players is known only for the current years. For Welsh and Northern Irish clubs after Brexit, the GBE text covers the Premier League and EFL only.
- Transfer windows: FIFA made registration periods compulsory in 2002/03. Exact dates are known only for the current League of Ireland windows (1 December to 22 February, 1 to 28 July).
- Formats: Welsh League Cup 2001/02 to 2003/04, League of Ireland Cup 2001-02 and 2003, Munster Senior Cup rounds, and the Leinster Senior Cup before its current format.
- 2027/28 (2028 in Ireland): no change is announced, except the Cymru Premier format and the National League play-off.

## Notes from the original (function map names, checked 2026-10-16)

- `IrePrm::create_table` at 0x63a740 counts the Premier Division clubs and accepts only 10 (4 meetings per pair) or 12 (3 meetings per pair). Any other count is an error. The Irish Premier sizes of 10 and 12 need no new code.
- `wel_first_alloc_and_fill_league_slots` at 0x925b30 (first-pass name, not yet swept by Opus) fills up to 18 rows. The 16-club Cymru Premier and its three-way split need new code.
- `NirCup::collect_entrants` at 0x78ac90 builds a 32-club entry list: 12 clubs drawn at random from up to 30 clubs of one division (key id 0x9cf8b8), then every club of two other divisions (key ids 0x9cf8b0 and 0x9cf8b4). The function map reads these as the lower, Premier and First divisions.
- `docs/research/2026-10-16-season-change-points.md` lists no foreign rule for Wales, Northern Ireland and Ireland in the original.

## Decisions for Max

1. The 2002/03 League of Ireland row holds the short 2002-03 season and the 2003 summer season. Play both, or start the summer calendar at once?
2. `ire_pres_cup`: use it for the President of Ireland's Cup (2014 on), or keep it for the LFA President's Cup only (ended 2002/03)?
3. New competitions: add the Welsh second tier, the Northern Irish third tier, the A Championship, the FAI National League and the Setanta Sports Cup? The A Championship is mostly reserve sides, which the database lacks.
4. Left out: the County Antrim Shield and the Gold Cup (regional; the data has their names but the original has no code file for them), and the all-Ireland Champions Cup (2019 on, one two-legged tie). Add any of them?
5. The missing clubs above.

## Reconcile

Done on 2026-10-16. The decision per field is in `wal-nir-irl.reconcile.tsv`. Third sources read today, all from publishers other than Wikipedia and RSSSF:

- claudionicoletti.eu (top-flight tables and cups of the three nations)
- Soccerway (Welsh second tier, Welsh League Cup, Irish Cup)
- BetExplorer (League of Ireland Cup, FAI Cup, NIFL Championship)
- RTE, The42, the News Letter
- the St Patrick's Athletic and Larne FC club sites

I applied the new default of the brief: `up` and `down` are the direct places by the rules of that season. Withdrawals, expulsions and licence refusals go in `notes`. Where that default moved an actual outcome out of `up` or `down`, I added a "Reconcile:" sentence to `notes`. I changed no other field.

Counts:

| Decision | Fields |
|---|---|
| second | 29 |
| first | 7 |
| conflict | 4 |
| found, accepted | 91 |
| found, rejected | 1 |

Notes on the decisions:

- League of Ireland Cup (11 rows): extra time comes before penalties. For 2005 and 2007 I found no third publisher. These two rest on the RSSSF tie lines (the primary record).
- NIFL Championship split, 2016/17 to 2023/24 (6 rows): there is a split. It came after 22 matches (32 in all) up to 2018/19, and after 33 matches (38 in all) from 2022/23. For 2021/22 the match count is not confirmed.
- 2005/06 Irish First Division: the first value (pos 11,12) stays. Neither club is in the 2006/07 Division One table. The RSSSF note on Omagh Town belongs to 2004/05.
- 2007/08 Irish Premier League and 2008/09 IFA Premiership: the first values stay under the new default. The licence exits are now in `notes`.
- The found value for 2002/03 FAI Cup entrants ("about 41") is rejected because it is an estimate. The field stays `unknown`.
- Found `up` values for the Welsh second tier were set to the direct place by the rules. Where no club went up, the value is "1 (pos 1; needs a Welsh Premier licence; inference)" and the actual outcome is in `notes`.

### Conflicts for Max

1. **wal-nir-irl-161, 2007/08 Irish First Division, `up`.** The first value is "1 (pos 4)". RSSSF has Bangor 3rd on 37 points and Ballyclare Comrades 4th on 36. The third source (claudionicoletti.eu) confirms that Bangor went up as the only second-level applicant, but it gives no position. Promotion was by application in the restructure, so the direct place by the rules is also unclear. Decide the position and whether the value should name a place at all.
2. **wal-nir-irl-406, 2017/18 Welsh Football League Division One, `down`.** The first value is "2 (pos 15,16)". RSSSF says 14th Monmouth Town and 15th Caerau (Ely) went down for no Tier 2 certificate and 16th Ton Pentre stayed. Soccerway marks three relegation places (pos 14,15,16) and still lists Ton Pentre in 2018/19. No source states the rule. The choices are 2 (pos 15,16) and 3 (pos 14,15,16).
3. **wal-nir-irl-456, 2019/20 Cymru South, `down`.** The first value is "3 (pos 6,15,16)". The season stopped early (COVID-19) and the order was set by points per game. RSSSF marks pos 14,15,16. Soccerway marks STM Sports and Cwmamman United. The 2021/22 table holds Taffs Well and Llanelli Town but not STM Sports, Cwmamman United or Caerau (Ely). No source states the relegation places by the rules.
4. **wal-nir-irl-488, 2020/21 Irish Cup, `clubs`.** This is a question of reading. 32 clubs were in the first-round draw. Six ties were walkovers, so 26 clubs played. The first value is 26 and RSSSF gives 32. Soccerway agrees with both counts.

Rows outside the check that still say "up and down read from the final table (actual outcome, licence decisions included)" were not re-read under the new default.

## Loose ends

Date: 2026-10-16. I re-read every league row under the brief default: `up` and `down` give the direct places by the rules of that season. The primary record is the RSSSF season page of each nation (`tablesw/walYY`, `tablesn/nilYY`, `tablesi/ierYY`), read today. Where a reprieve or a licence change needed more detail, I also read the Wikipedia season page today. Each changed value has its URL in `sources`. The four conflict rows (161, 406, 456, 488) are unchanged. Row order and ids are unchanged.

Changed values (47 rows). The actual outcome is now in `notes`, after the sentence "Actual outcome:".

| Rows | Change |
|---|---|
| 4, 29, 79, 104, 154, 179 | League of Wales, 18 clubs: `down` is 2 (pos 17,18). The 17th club stayed because no second-tier club was promoted, or because a club resigned. |
| 204 | 2009/10 Welsh Premier: `down` is 6, the cut from 18 to 12 clubs. The 12 places were set by licence and criteria, so no table places are given. |
| 229, 254, 354, 404, 479 | Welsh Premier, 12 clubs: `down` is 2 (pos 11,12). The actual exits were a spared 11th club (2010/11), Neath (3rd, 2011/12), Port Talbot Town (10th, 2015/16), Bangor City (2nd, 2017/18) and no relegation in 2020/21. |
| 279 | 2012/13 Welsh Premier: `down` is 1 (pos 12). Llanelli (11th) was liquidated and 12th Afan Lido stayed. |
| 80, 105, 205, 206, 230, 231, 256, 281, 306, 331, 455, 506, 555 | Welsh second tier: `up` is 1 (pos 1) with the licence condition. The champion lacked a licence or was not promoted. Row 206 is marked as an inference. |
| 431 | 2018/19 Welsh League Division One: `down` is 3 (pos 14,15,16). 13th Port Talbot Town went down for no Tier 2 certificate, so 14th Taff's Well stayed. |
| 605 | 2025/26 Cymru North: `down` is 1 (pos 16). 14th Flint Mountain went down voluntarily, so 16th Ruthin Town was reprieved. |
| 86 | 2004/05 Irish League First Division: `down` is 2 (pos 11,12). 11th Ballymoney United stayed after Omagh Town withdrew. |
| 210, 211 | 2009/10: Premiership `down` is 1 (pos 12) with the 11th in the play-off. Championship 1 `up` is 1 (pos 1). Loughgall declined promotion, so 12th Institute played off. |
| 235, 236, 310, 311 | 2010/11 and 2013/14: `playoffs` gives the Premiership 11th and the Championship 1 runner-up. No play-off was played because the runner-up had no licence. |
| 361, 362 | 2015/16 NIFL Championship 1 and 2: `down` is 3 (pos 12,13,14), and the play-off rule follows the season rule text. `up` of Championship 2 is 1 (pos 1). Bangor was demoted, and champion Limavady United had no licence. |
| 460, 485 | NIFL Premiership 2019/20 and 2020/21: the 11th plays off by the rules, and in 2020/21 `down` is 1 (pos 12). COVID-19 cancelled the play-off and the relegation. |
| 536 | 2022/23 NIFL Championship: `down` is 1 (pos 12) and `playoffs` is "pos 2,11". Warrenpoint Town (2nd) was expelled. |
| 116, 117 | League of Ireland 2006: Premier `down` is 1 (pos 12). First Division `up` is 1 (pos 1) with the 2nd in the play-off. The FAI set the 2007 places by licence. |
| 191, 216, 266 | League of Ireland Premier 2009, 2010 and 2012: `down` is 1 (pos 10), 1 (pos 10) and 1 (pos 12). Derry City and Cork City were demoted (2009). Sporting Fingal withdrew (2010 to 2011). Monaghan United withdrew (2012). |

Notes only: 505 and 530 (Cymru North withdrawals) and 286 (Newry City dissolved, 2012/13).

Wording: 74 rows had the note "up and down read from the final table (actual outcome, licence decisions included)". Their values agree with the rules places. The note now says that `up` and `down` are the rules places, and that RSSSF shows no licence decision, withdrawal or reprieve that changed them. Row 636 (2026/27) says instead that the season was not finished on 2026-10-16.

Open points for Max:

- 2009/10 Welsh Premier (204): the rules gave no table places for the cut to 12 clubs. The value is the count only.
- 2015/16 NIFL Championship 1 (361): the rule text is from the Wikipedia season page (section "Changes from 2014/15"). No second publisher was read.
- 2019/20 NIFL Championship (461): the play-offs were cancelled (COVID-19). `playoffs` still says "none", because I found no rule text for that season.

## Source fill

Method: I ran the rules lint and took each filled fact field with no `field=URL` entry. I read each page on 2026-10-16 with curl and checked the value against the page text or league table. For `split` = none and `format` = double round robin, the season page must show one table for the division with every club on 2 x (clubs - 1) matches, and no championship or relegation round. For a stopped season (2019/20, 2020/21), a table with fewer matches counts if the page says the season was stopped. I added sources only. I changed no value, row order or id.

- Sources added: 22 field entries in 22 rows.
- Irish third tier `name` = none for 2001/02 and 2002/03 (wal-nir-irl-18, 43): https://en.wikipedia.org/wiki/A_Championship (founded 2008 as the third level).
- 2026/27 and 2027/28 Premier Division `split` = none: https://en.wikipedia.org/wiki/League_of_Ireland_Premier_Division (each club plays every other club four times, no split). The row carries the current rule forward.

No source found (added to `notes`):

- wal-nir-irl-642 and wal-nir-irl-667 (First Division, 2027 and 2028): `split`. The league article gives no format. The 2026 season page gives the 2026 format only.

Disagreements: none.
