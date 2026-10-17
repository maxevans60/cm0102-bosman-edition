# int-continental: continental championships and Nations Leagues, 2001/02 to 2027/28

Part `int-continental` of slice 10 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. The table is `int-continental.tsv` (297 rows: 11 competitions, 27 seasons). No clubs file: club moves do not apply.

## Scope

| comp | comp_file | Notes |
|---|---|---|
| European Championship | `euro_champ` | Finals |
| European Championship qualifying | `euro_champ_qual` | Groups and play-offs, with Nations League paths from 2018/19 |
| UEFA Nations League | new | From 2018/19 |
| Finalissima | new | 2022, and 2026 (cancelled) |
| CONCACAF Gold Cup | new | No file of the original runs it |
| CONCACAF Nations League | new | Qualifying 2018/19, first edition 2019/20 |
| Africa Cup of Nations | `african_nations` | |
| African Nations League | new | First edition 2029, so every row is `not held` |
| AFC Asian Cup | `asia_nations` | |
| OFC Nations Cup | `oceania_nations` | |
| Copa América | `goldcup` | The class in goldcup.cpp runs the Copa América |

The Copa América runs in `goldcup.cpp`, despite the file name. At load the record named "Copa America" sets global 0x9cf7a8 (compare at 0x61035b). Setup passes that record to the goldcup constructor 0x5e0660 (call in 0x830ac0). The original has no CONCACAF Gold Cup: no name in the competition matcher 0x60efd0 is a Gold Cup. Proof: `2026-10-16-sweep-international.md` section 2 and the name check of 2026-10-16. The function map still calls the class `GoldCup`.

## Method

- **Season of a tournament:** the July to June season that holds its first match. A June final tournament (Euro 2004) goes to 2003/04. A July start (Gold Cup 2003, Asian Cup 2004, Copa América 2004) goes to the next season. An edition that spans seasons (Nations Leagues, Euro qualifying, OFC 2004 and 2008) gets one row in each season, with what is played in that season.
- **real_season:** the edition's own label, for example "2021 (played 2022)".
- **Columns:** `clubs` holds the number of teams. `format` holds the host, the dates with the months, the group and knockout format and the golden or silver goal rule. `notes` holds the qualifying format (finals rows) or the entry list. `up`, `down` and `playoffs` hold Nations League promotion, relegation and play-offs, and the Euro qualifying play-offs. Columns for club rules (`foreign`, `work_permit`, `registration`, `windows`, `split`) are `n/a`.
- **Sources:** each English Wikipedia page was read on 2026-10-16 as raw wikitext through the MediaWiki API. Facts came from the infobox fields and from sentences that a script found by keyword. Other pages that were read: RSSSF match details for the 2002 and 2004 African Nations Cups, ESPN and BBC (December 2025) for the AFCON cycle, and the OFC release on 2027 hosts.
- **Golden goal:** the Wikipedia page "Golden goal" says that IFAB removed the golden and silver goal after Euro 2004. Rows from 2004/05 cite that page for "no golden goal".

## Main findings for the build

- **Golden or silver goal:**
  - Gold Cup 2002 and 2003: golden goal (match notation).
  - OFC Nations Cup 2002: golden goal (match notation).
  - Euro 2004: silver goal.
  - AFCON 2004: no golden goal, because the Morocco v Algeria quarter-final had goals at 113 and 120+1.
  - AFCON 2002: no golden goal. Senegal scored in the 97th minute of the semi-final and play went on to 120+2 (set in the reconcile).
  - Asian Cup 2004: unknown.
- **Copa América extra time:** 2001, 2004 and 2007 went straight to penalties. The 2011 page says that 2011 added extra time where "previously" ties went straight to penalties (inference for the earlier editions). From 2015, only the final has extra time, except in 2019, where the semi-finals and the third place match also had extra time.
- **AFCON cycle:**
  - 16 teams to 2017, played in January and February.
  - 24 teams from 2019. The 2019 edition was played in June and July.
  - The 2021 and 2023 editions were played in January and February 2022 and 2024.
  - The 2025 edition was played from December 2025 to January 2026.
  - The 2027 edition is in June and July 2027.
  - A 2028 edition follows, then every four years. The African Nations League starts in 2029.
- **Asian Cup:** 24 teams from 2019. The 2023 edition was moved from China to Qatar and played in January and February 2024.
- **Nations Leagues:**
  - UEFA editions run 2018-19, 2020-21, 2022-23, 2024-25 and 2026-27.
  - From 2024-25, the UEFA Nations League has League A quarter-finals and promotion and relegation play-offs.
  - CONCACAF editions run 2019-20, 2022-23, 2023-24, 2024-25 and 2026-27. The CONCACAF Nations League also qualifies teams for the Gold Cup, and the 2023-24 edition qualified them for the 2024 Copa América.
- **Euro qualifying:** the Nations League play-off paths started with Euro 2020. The Euro 2028 qualifying has 12 groups, two reserved host places and play-offs in March 2028.

## Unknowns (rows say `unknown`)

- Substitutes and bench size for most editions. The table fills only those that a page states, for example Euro 2020 (five), Gold Cup 2019 (fourth in extra time) and Finalissima 2022.
- AFCON 2028: host (bids from Ethiopia, Morocco, and Botswana with South Africa), dates, and so its season. The team count of 28 comes from the Wikipedia infobox only.
- OFC Nations Cup 2028: the host and dates of the finals. Qualifying is in the Cook Islands in 2027.
- Copa América 2028: the host and the team count. Wikipedia gives June to July 2028.
- Gold Cup 2027: the host and the format. Dates 19 June to 11 July 2027.
- Host of the 2027 UEFA Nations League Finals.
- Qualifying detail for Gold Cup 2003 to 2015 and for AFCON 2002.
- Relegation from Leagues B and C of the CONCACAF Nations League 2026-27.
- Finalissima after 2026: the UEFA and CONMEBOL agreement runs to 2028, but no edition is set within 2027/28.

## Notes for Max

- The 2001 Copa América runs 11 to 29 July 2001, inside the game's first season. A 2001/02 start in July would meet it in progress.
- Euro 2020 and the 2019-20 CONCACAF Nations League Finals moved from 2019/20 to 2020/21 because of COVID-19. The rows follow the real dates.

## Reconcile

Reconcile date 2026-10-16. The full list is in `int-continental.reconcile.tsv`. I changed 19 rows of `int-continental.tsv` and added each new URL to `sources`. Row order and ids are the same. No clubs file exists for this part.

| Decision | Fields |
|---|---|
| first | 4 |
| second | 13 |
| conflict | 0 |
| found value accepted | 9 |
| found value rejected | 0 |

How I decided:

- Third place matches (Gold Cup 2002, 2003 and 2015, OFC 2002 and 2012, Asian Cup 2004, 2007, 2011 and 2015): each match is in a third publisher (Concacaf, US Soccer, OFC, Soccerpunter, Al Jazeera, The Peninsula). I added "third place match" to each format field.
- Asian Cup 2015: Gulf News and The Peninsula report extra time in two quarter-finals. The format now says extra time in the knockout rounds.
- Gold Cup 2025: an AP report says that Concacaf removed extra time except in the final. The format now says that drawn quarter-finals and semi-finals go straight to penalties.
- Euro 2012 qualifying (101, 112): first. UEFA says that Estonia v Faroe Islands opened qualifying on 11 August 2010. The main matchdays began on 3 September.
- Euro 2028 play-offs (277, 288): second. The UEFA release gives three cases. The playoffs field now lists all three.
- UEFA Nations League 2027/28 (289): first. The UEFA promotion and relegation document of 15 September 2026 has only A/B and B/C play-offs, in March 2027. All League D teams go up. The March 2028 C/D date is from the old plan.
- CONCACAF Nations League 2022-23 down (237): first. The Concacaf release of 28 February 2023 says that there is promotion but no relegation into 2023/24. RSSSF shows the marks of the old format.
- CONCACAF Nations League 2026-27 Finals (281): second. The Concacaf schedule gives League A Finals 25 to 28 March 2027 at SoFi Stadium. The 22 to 30 March window is for Leagues B and C.
- Found values: I read each page and it states the value. I accepted all nine.

There are no conflicts.

Items for Max that the reconcile did not change:

- int-continental-281 down lists League A only. The Concacaf page also says that the League B fourth-placed teams go down to League C.
- The golden goal rule at the 2004 Asian Cup is still unknown.

## Loose ends

- int-continental-281 `down`: the CONCACAF page of the fifth edition says that the fourth-placed club of each League B group goes down to League C. The field now lists the 4 League B teams as well as the League A 5th and 6th. Source: https://www.concacaf.com/competitions/nations-league/news/fifth-edition-of-nations-league-to-begin-in-september

## Source fill

Method: I ran the rules lint and read each page on 2026-10-16 with curl. I added sources only.

- int-continental-67: `name` and `clubs` (16) from https://en.wikipedia.org/wiki/UEFA_Euro_2008. The row already had `teams=`, which the lint does not read as `clubs`.
- int-continental-223: `name`, `clubs`, `playoffs`, `format` and `subs_used` from https://en.wikipedia.org/wiki/2020%E2%80%9321_UEFA_Nations_League (Finals 6 to 10 October 2021, League C play-outs in March 2022, five substitutes and a sixth in extra time). `name`, `clubs` (55) and `format` (league phase from 1 June 2022) also from https://en.wikipedia.org/wiki/2022%E2%80%9323_UEFA_Nations_League.

No source found: none. Disagreements: none.

## Pass 2

Date 2026-10-17. Decision agent. Lint before and after: 0 problems. Row ids and order are the same. 16 rows changed. No clubs file: club moves (Q279, Q280, Q290) and reserve teams (Q289) do not apply.

### No new tiers, no new nations (Q331, Q288)

No row is deleted. Every competition of this part is a national-team competition. The game holds a national team for every nation, and the original already runs finals with nations that a player cannot select (Africa Cup of Nations, Asian Cup, Oceania Nations Cup). The Nations League leagues are not national tiers, so Q331 does not apply (inference from the wording of Q331 and Q288, which name club tiers and club competitions).

### Late editions (Q277)

Each edition that real life played after its planned slot moves to the season of the planned slot, with the planned dates and hosts. The row of the real season becomes `not held`, and its notes point to the new row. The rest of the format stays as played.

| Edition | From row (real) | To row (planned) | Planned slot | Source |
|---|---|---|---|---|
| Euro 2020 | 210 (June to July 2021) | 199 | 12 June to 12 July 2020, 12 hosts with Dublin | https://en.wikipedia.org/wiki/UEFA_Euro_2020 |
| Euro 2020 play-offs | 211 (October and November 2020) | 200 | 26 and 31 March 2020 | https://en.wikipedia.org/wiki/UEFA_Euro_2020_qualifying_play-offs |
| CONCACAF Nations League 2019-20 Finals | 215 (June 2021, Denver) | 204 | 4 to 7 June 2020, Houston and Arlington | https://en.wikipedia.org/wiki/2021_CONCACAF_Nations_League_Finals |
| Copa América 2020 (played as 2021) | 220 (June to July 2021, Brazil, 10 teams) | 209 | 12 June to 12 July 2020, Argentina and Colombia, 12 teams with guests Australia and Qatar | https://en.wikipedia.org/wiki/2021_Copa_Am%C3%A9rica |
| Africa Cup of Nations 2021 | 227 (January 2022) | 216 | 9 January to 6 February 2021 | https://en.wikipedia.org/wiki/2021_Africa_Cup_of_Nations |
| Africa Cup of Nations 2023 | 249 (January 2024) | 238 | June to July 2023 (days unknown) | https://en.wikipedia.org/wiki/2023_Africa_Cup_of_Nations |
| Asian Cup 2023 | 251 (Qatar, January 2024) | 240 | 16 June to 16 July 2023, China | https://en.wikipedia.org/wiki/2023_AFC_Asian_Cup |

- The Copa América zones of 6 are an inference: the plan had 12 teams in two zones.
- The 2025 Africa Cup of Nations (row 271) stays. It started on 21 December 2025, inside its own year, and the page gives no earlier set dates (inference: not a late edition).
- The 2020 OFC Nations Cup (row 208) stays cancelled. Real life cancelled it and did not play it later.
- The 2026 Finalissima (row 268) stays cancelled. Real life did not play it.

### Open points settled

- **Golden goal at the 2004 Asian Cup (row 42):** no golden goal. RSSSF marks the semi-final Bahrain 3-4 Japan `[aet]` (full extra time), not `[asdet]` (golden goal). Japan scored at 93 minutes. Source: https://www.rsssf.org/tables/04asch.html. IFAB had removed the golden goal after Euro 2004, which ended before this tournament (https://en.wikipedia.org/wiki/Golden_goal).
- **The 2001 Copa América at a 2001/02 start (row 11):** the row keeps the real dates, 11 to 29 July 2001, and the 12 real teams. The original already builds the 2001 edition with a fixed list of 12 (0x5e1d65, `2026-10-16-sweep-international.md`). Game creation runs the engine day by day from 1 January 2001 to the start date (14 July 2001 with England, `2026-10-14-game-creation-inventory.md`). So the matches before the start date play during game creation, and the player sees the rest live. With a later start date the whole tournament plays during creation. No special code for a tournament in progress is needed (inference from the creation run).
- **int-continental-281 `down`:** already settled in "Loose ends" (League A 5th and 6th, League B 4th).
- **Euro 2020 and the 2019-20 CONCACAF Finals (old note for Max):** replaced by Q277 above.

### Unknowns left

The unknowns of the first pass stay, except the 2004 Asian Cup golden goal. The planned days of the 2023 Africa Cup of Nations are unknown.

## Second source, pass 2

Date 2026-10-17. Second-source agent. The file is `int-continental.check2.tsv` (39 lines). Each page was read on this date with curl, or with a fetch that quotes the sentence. Lint before and after: 0 problems. Row ids and order are the same.

Scope: the 27 check lines with verdict `no second source`, and 12 new lines for the planned dates, hosts and sizes of the late editions that pass 2 moved (rows 199, 200, 204, 209, 216, 238 and 240).

### Counts

| Lines | agree | disagree, settled | disagree, still split | no second source |
|---|---|---|---|---|
| 27 first-pass lines | 20 | 1 | 0 | 6 |
| 12 moved-row lines | 12 | 0 | 0 | 0 |
| Total | 32 | 1 | 0 | 6 |

No second source stays for these lines:

- int-continental-224 `subs_used` and `bench` (2022 Finalissima). UEFA confirms no extra time, but no page of another publisher gives the substitution limit or the squad size.
- int-continental-277 `clubs` and int-continental-288 `clubs` (54 teams in Euro 2028 qualifying). UEFA and SI give 12 groups of four or five, with no team count.
- int-continental-277 `notes` (Nations League quarter-finalists go into groups of 4). Wikipedia only.
- int-continental-296 `notes` (season of the 2028 OFC finals). The field states an unknown, and no page gives the finals dates.

### Changes to int-continental.tsv

- int-continental-293 `clubs`: changed from "28 (Wikipedia infobox only: unknown)" to 24. The CAF executive committee rejected the 28-team plan in June 2026 (AllAfrica, reporting the Guardian, https://allafrica.com/stories/202607010070.html, and Inside World Football, https://www.insideworldfootball.com/2026/07/10/motsepe-plan-to-add-expand-afcon-crashes-and-burns-at-caf-exco/). The third source, NBS Sport (https://nbssport.co.ug/article/motsepes-plan-for-bigger-africa-cup-of-nations-voted-down), states that the 2028 tournament continues with the 24-team format. Decision: second. The unknown "team count of 28" in the Unknowns section above no longer applies.
- int-continental-209 `format`: the 2020 Copa America zones of 6 are no longer an inference. The Buenos Aires Times (https://www.batimes.com.ar/news/amp/sports/co-hosts-argentina-to-face-australia-at-2020-copa-america.phtml) gives two first-round groups of six, the top four of each to the quarter-finals. The words "(inference: 12 teams in two zones)" are removed, and the URL is added as `zones=`.

### Other findings

- The first check said that RSSSF does not list promotions for the 2020-21 and 2022-23 UEFA Nations League (rows 212 and 234). It does: each table marks Promoted and Relegated.
- Moved rows: every planned date, host and size agrees with a second publisher. Wembley Stadium gives the planned Euro 2020 final on 12 July 2020. Sky Sports gives the planned play-offs on 26 and 31 March 2020. CONCACAF gives the planned Nations League Finals on 4 and 7 June 2020 in Houston and Arlington. COSAFA gives 9 January to 6 February 2021 for the Africa Cup of Nations in Cameroon. Football Australia gives 16 June to 16 July 2023 for the Asian Cup in China.
- The planned days of the 2023 Africa Cup of Nations stay unknown. Every page read gives only "June and July 2023".
- int-continental-212: the UEFA decision of 24 September 2020 (PZPN reprint) also raised the match sheet to 23 players, which means 12 substitutes. The row's `bench` stays `unknown`, because it was not a check line.
- int-continental-278: the 2028-29 Nations League of three leagues of 18 was to go for final approval on 15 September 2026 (FIGC, May 2026). The result of that meeting was not found, so the note "proposal" stays.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after.

- int-continental-212 (UEFA Nations League 2020-21), `bench`: unknown changed to "12 (23 players on the match sheet, from October 2020)". The UEFA Executive Committee decision of 24 September 2020 raised the match sheet to 23 players for the Nations League group phase. 23 less 11 starters gives 12. Source: PZPN reprint, https://pzpn.pl/en/association/news/2020-09-24/five-substitutions-allowed-for-2020-21-club-and-national-team-competitions (read on 2026-10-17).
