# UEFA access list by rank and round names, 2003/04 to 2020/21

Research for 0.6.0 slice 23. Date of all page reads: 2026-10-18. Table: `docs/research/rules-0.6.0/uefa-access.tsv`, 285 rows.

## Task B. Round names by season (spec Q370)

Each row gives the first season of a name. A name stays until a later row for the same round replaces it. Source: the UEFA regulations of that season (the copies on kassiesa.net), article "Number of rounds" or "Competition stages".

| season_from | comp | round | name | source URL |
|---|---|---|---|---|
| 2003/04 | cl | Q1 | first qualifying round | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | Q2 | second qualifying round | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | Q3 | third qualifying round | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | G | group stage | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | R16 | first knock-out round | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | QF | quarter-finals | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | SF | semi-finals | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2003/04 | cl | F | final | https://kassiesa.net/uefa/files/2003-uefa-cl-rules.pdf |
| 2007/08 | cl | R16 | first knockout round (spelling change only) | https://kassiesa.net/uefa/files/2007-uefa-cl-rules.pdf |
| 2009/10 | cl | PO | play-offs | https://kassiesa.net/uefa/files/2009-10-uefa-cl-rules.pdf |
| 2009/10 | cl | R16 | round of 16 | https://kassiesa.net/uefa/files/2009-10-uefa-cl-rules.pdf |
| 2015/16 | cl | paths | champions path, league path | https://kassiesa.net/uefa/files/2015-16-uefa-cl-rules.pdf |
| 2018/19 | cl | PR | preliminary round | https://kassiesa.net/uefa/files/2018-19-uefa-cl-rules.pdf |
| 2003/04 | uc | QR | qualifying round | https://kassiesa.net/uefa/files/2003-uefa-uc-rules.pdf |
| 2003/04 | uc | R1 | first round | https://kassiesa.net/uefa/files/2003-uefa-uc-rules.pdf |
| 2003/04 | uc | R2 | second round | https://kassiesa.net/uefa/files/2003-uefa-uc-rules.pdf |
| 2003/04 | uc | R3 | third round | https://kassiesa.net/uefa/files/2003-uefa-uc-rules.pdf |
| 2003/04 | uc | R4 | fourth round | https://kassiesa.net/uefa/files/2003-uefa-uc-rules.pdf |
| 2003/04 | uc | QF, SF, F | quarter-finals, semi-finals, final | https://kassiesa.net/uefa/files/2003-uefa-uc-rules.pdf |
| 2004/05 | uc | Q1 | first qualifying round | https://kassiesa.net/uefa/files/2004-uefa-uc-rules.pdf |
| 2004/05 | uc | Q2 | second qualifying round | https://kassiesa.net/uefa/files/2004-uefa-uc-rules.pdf |
| 2004/05 | uc | R1 | first round (now the round before the groups) | https://kassiesa.net/uefa/files/2004-uefa-uc-rules.pdf |
| 2004/05 | uc | G | group stage (five matchdays) | https://kassiesa.net/uefa/files/2004-uefa-uc-rules.pdf |
| 2004/05 | uc | R32 | round of 32 | https://kassiesa.net/uefa/files/2004-uefa-uc-rules.pdf |
| 2004/05 | uc | R16 | round of 16 | https://kassiesa.net/uefa/files/2004-uefa-uc-rules.pdf |
| 2009/10 | el | Q1, Q2 | first qualifying round, second qualifying round | https://kassiesa.net/uefa/files/2009-10-uefa-el-rules.pdf |
| 2009/10 | el | Q3 | third qualifying round | https://kassiesa.net/uefa/files/2009-10-uefa-el-rules.pdf |
| 2009/10 | el | PO | play-offs | https://kassiesa.net/uefa/files/2009-10-uefa-el-rules.pdf |
| 2009/10 | el | G | group stage (six matchdays) | https://kassiesa.net/uefa/files/2009-10-uefa-el-rules.pdf |
| 2009/10 | el | R32, R16, QF, SF, F | round of 32, round of 16, quarter-finals, semi-finals, final | https://kassiesa.net/uefa/files/2009-10-uefa-el-rules.pdf |
| 2018/19 | el | PR | preliminary round | https://kassiesa.net/uefa/files/2018-19-uefa-el-rules.pdf |
| 2018/19 | el | paths | main path, champions path | https://kassiesa.net/uefa/files/2018-19-uefa-el-rules.pdf |

Notes:
- The UEFA Cup was renamed UEFA Europa League in 2009/10. The first round of the UEFA Cup ended after 2008/09.
- The regulations call the round before the groups "play-offs" in every season from 2009/10 to 2020/21. The same regulations use "play-off round" once, in the registration deadlines. Wikipedia uses "Play-off round".
- 2009/10 to 2014/15: the regulations text gives no name to the two paths. The 2009/10 access list (Annex Ia) heads the columns "Champions" and "Non Champions". The 2015/16 regulations name them "champions path" and "league path" (Art. 3.07).

Second source:
- Wikipedia season pages (section headings, read as wikitext) agree with every name above with two exceptions. For CL 2003/04 to 2008/09 Wikipedia heads the round "Round of 16", where UEFA wrote "first knock-out round". For the play-offs Wikipedia writes "Play-off round".
- RSSSF, https://www.rsssf.org/ec/ec200304det.html, agrees for 2003/04: "FIRST KNOCK-OUT ROUND" in the CL, and "QUALIFYING ROUND", "FIRST ROUND" to "FOURTH ROUND" in the UEFA Cup. The RSSSF detail pages for 2009/10 and later return 404, so they were not read.

## Task A. Access list by association rank, 2009/10 to 2020/21

### Method

1. Source: the kassiesa.net access list page of each season. The page shows the final list. A green mark is a place that rebalancing created, a red mark (or strike-through from 2018) is the default place that it replaced. The default list is every place without a green mark. A script read the HTML cells and colours.
2. Order of places: CL by league position (CH, N2, N3, N4). EL cup winner first, then league places. In every season the cup winner holds the best EL round of its association (checked on all rows).
3. Second source: the Wikipedia CL and EL season pages, table "Teams entering in this round", read as wikitext. Wikipedia gives the final list and the title-holder changes. The script compared Wikipedia's final list with kassiesa's final list place by place. They agree in all 24 competition seasons. The only differences are rows where the title holder took a place and one publisher names the slot and the other names the club, plus the fair play places, which Wikipedia does not list by rank.
4. Third source, one season only: the 2009/10 regulations, Annex Ia, read line by line. It matches the default list of 2009/10 exactly.
5. Fair play nations: kassiesa marks and the Wikipedia team lists ("FP") agree for all seven seasons.

### Cycles and changes

- 2009/10 to 2011/12. CL: ranks 1 to 3 four places (G G G PO-LP), 4 and 5 three (G G PO-LP), 6 three (G G Q3-LP), 7 to 12 two (G Q3-LP), 13 to 15 two (Q3-CP Q3-LP), 16 to 47 one (Q2), 48 to 53 one (Q1). EL: no direct group places. 2010/11 changes the bottom of the EL list: ranks 33 and 34 lose a Q2 place and ranks 52 and 53 gain a second place.
- 2012/13 to 2014/15. CL unchanged. EL: the cup winners of ranks 1 to 6 go to the group stage. More clubs enter in Q1. 2014/15: Gibraltar joins (rank 54), rank 47 champion moves to Q1, rank 27 loses a Q2 place.
- 2015/16 to 2017/18. CL unchanged except Q1 (8 champions, 10 from 2016/17). The EL holder enters the CL. EL: cup winners of ranks 1 to 12 and N5 of ranks 1 to 3 and N4 of rank 4 to the group stage. Fair play places end after 2015/16. 2016/17: Kosovo joins (rank 55, clubs not licensed that season).
- 2018/19 to 2020/21. CL: ranks 1 to 4 get four places, all to the group stage. Ranks 5 and 6: G G Q3-LP. A preliminary round of 4 champions. Q2 splits into champions path and league path. Both title holders get a CL group place. EL: preliminary round, and a champions path in Q2, Q3 and PO for clubs that drop from the CL. Cup winners of ranks 1 to 12 to the group stage. 2020/21: rank 51 has two EL places (three in 2019/20) and rank 55 has two (one in 2019/20).
- League cup places: Wikipedia names the last EL place of England (all seasons) and France (all seasons) as the league cup winner's place. The UEFA access list itself has no league cup slot. The EL regulations allow "the winner of another official domestic competition" (Art. 2.03 to 2014/15, Art. 3.03 from 2015/16). The table marks these in `notes`.

### Sum checks that failed

The default list plus the title-holder places gives valid round sizes in every season (even counts, 32 or 48 in the group stage), except 2015/16 to 2017/18 (see below). The real round sizes in `uefa.tsv` are final sizes, after rebalancing. These differ from the default:

| season | comp | round | default | real | reason |
|---|---|---|---|---|---|
| 2009/10, 2010/11, 2011/12, 2013/14 | cl | Q1 | 6 | 4 | CL holder qualified through its league, two champions to Q2 |
| 2014/15 | cl | Q1 | 8 | 6 | same |
| 2016/17 | cl | Q1 | 10 | 8 | Kosovo champion not licensed, Latvia champion to Q2 |
| 2012/13 | cl | Q3-LP | 10 | 8 | Chelsea rule, England N4 to EL, two runners-up to PO-LP |
| 2015/16 to 2017/18 | cl | PO-LP | 11 | 10 | both holders counted gives an odd 11. The list is built for the EL holder to fill the CL holder's group place (CL Art. 3.07) |
| 2018/19, 2019/20 | cl | Q1 | 34 | 32 | CL holder rule |
| 2018/19, 2019/20 | cl | Q2-LP | 6 | 4 | EL holder rule |
| 2020/21 | cl | Q3-CP, Q3-LP | 12, 8 | 10, 6 | both holder rules |
| 2009/10, 2011/12 | el | Q1, PO | 50/54, 74 | 46/50, 76 | holder in the CL, play-off round gained a tie |
| 2010/11, 2012/13 to 2017/18 | el | Q1 | 2 to 4 more | | cup winners moved up a round by the holder rule (2016/17 also Kosovo) |
| 2018/19, 2019/20 | el | PR, Q2-CP | 16, 20 | 14, 18 | one CL loser fewer in each path |
| 2020/21 | el | Q2-MP, Q3-CP | 74, 20 | 72, 18 | Wikipedia: two CL Q2-CP losers went straight to EL PO-CP |

Each case is explained by that season's rebalancing as kassiesa and Wikipedia describe it. The `cl-holder` and `el-holder` rows give the numbers per season.

### Points not settled

- The cascade after an unused CL holder place in 2009/10 to 2014/15 (champions of 13, 16 and two Q1 champions move up) is practice taken from kassiesa and Wikipedia. The regulations say only that domestic champions have priority (Annex Ia, N.B.). The exact rule is unknown.
- The same applies to the EL holder cascade to 2017/18 (cup winners move up, best-ranked first). The regulations give no rank numbers. Unknown.
- The 2012/13 EL default for ranks 1 and 2 (cup winner to the group stage) comes from Wikipedia and from 2013/14 and 2014/15. kassiesa shows only the final list there, and the 2012/13 Annex Ia has a garbled text layer. This is an inference from two sources.
- Regulations Annex Ia was compared line by line for 2009/10 only. For the other seasons the default rests on kassiesa plus Wikipedia.
- France 2020/21: Wikipedia still names a league cup place. Whether France had one after the last Coupe de la Ligue (2019/20) is unknown.
- 2019/20 and 2020/21 single-match rounds (COVID) do not change the access list. They are in `uefa.tsv`.

## Sources

- Access lists: `https://kassiesa.net/uefa/history/access<YYYY>.html` (2009 to 2017), `https://kassiesa.net/uefa/history/AccessList<YYYY>.html` (2018 to 2020).
- Regulations: `https://kassiesa.net/uefa/files/<YYYY>-uefa-cl-rules.pdf` and `-uc-rules.pdf` (2003 to 2008), `https://kassiesa.net/uefa/files/<YYYY>-<YY>-uefa-cl-rules.pdf` and `-el-rules.pdf` (2009/10 to 2020/21).
- Wikipedia: `https://en.wikipedia.org/wiki/<YYYY>%E2%80%93<YY>_UEFA_Champions_League`, `_UEFA_Cup`, `_UEFA_Europa_League`.
- RSSSF: `https://www.rsssf.org/ec/ec200304det.html`.

## Slice 24: 2021/22 to 2023/24

Research for 0.6.0 slice 24. Date of all page reads: 2026-10-18. The rows are in `docs/research/rules-0.6.0/uefa-access.tsv`. This slice adds 102 rows (387 data rows in total). The Europa Conference League (ECL) starts in 2021/22. The table gets the comp values `ecl` and `ecl-holder`.

Rows added:

| season | cl | el | ecl | holder rows |
|---|---|---|---|---|
| 2021/22 | 12 | 7 | 9 | cl-holder, el-holder |
| 2022/23 | 14 | 9 | 11 | cl-holder, el-holder, ecl-holder |
| 2023/24 | 12 | 9 | 11 | cl-holder, el-holder, ecl-holder |

### Task A method

1. Source: the kassiesa access list page of each season. 2021/22 is `AccessList2021.html`, 2022/23 is `AccessList2022.html`, 2023/24 is `AccessList2023.html`. A script read the HTML cells and the cell colours.
2. A green cell (colour `#00B050`) is a place that rebalancing created. A struck-out item is the default place that it replaced. The default list is every place without a green mark. Struck items stay in the default list. This is the method of slice 23.
3. The default list includes the struck Russian row in 2022/23 and 2023/24. The rows show the places before any removal. The notes say what the published list did.
4. Order of places: CL by league position (CH, N2, N3, N4). EL and ECL: cup winner first, then league places by position. A nation with places in both EL and ECL has the cup winner and the EL league places in the EL row and the rest in the ECL row.
5. An EL or ECL rank with no places has the value `-`. Liechtenstein has `-` for CL and EL and has its cup winner place in ECL (Q1 in the default list).
6. Second source: the Wikipedia CL, EL and ECL season pages, table "Qualified teams" (the teams table that sits under "Teams entering in this round"). A script counted the clubs per association and entry round, only clubs with a league position or cup winner label. It compared them with the final kassiesa list (every item that is not struck out). Result: no difference in all 9 competition seasons. The note of each row ends with `agree`.
7. Third source for the rules: the regulations of the season on kassiesa, `https://kassiesa.net/uefa/files/<YYYY>-<YY>-uefa-<cl|el|ecl>-rules.pdf`. They give the title holder rules (CL Art. 3.03 to 3.07, EL Art. 3.08 to 3.12) and the league cup rule (ECL Art. 3.03).

### Changes in the cycle 2021/22 to 2023/24

- CL: ranks 1 to 4 get four places (G G G G). Ranks 5 and 6 get G G Q3-LP. Ranks 7 to 9 get G Q3-LP. Same as 2020/21 except that the CL losers of PR and Q1 now go to the ECL, not to the EL.
- EL: cup winners of ranks 1 to 6 and the league places N5 (ranks 1 to 4) and N4 (rank 5) go to the group stage. The cup winners of ranks 7 to 12 go to PO. Ranks 13 to 15 go to Q3. EL has no Q1 and no Q2. Associations from rank 16 have no EL place in the default list.
- ECL: N6 of ranks 1 to 4 and N5 of rank 5 go to PO. Ranks 6 to 12 get Q3 and Q2 (N4 and N5 for rank 6). Ranks 13 to 15 get Q2 Q2. Ranks 16 to 28 get Q2 Q2 Q2. From rank 29 the places drop to Q1.
- ECL Q2 has two paths from 2021/22: champions path (CL losers) and main path.

### League cup places

- England: the last ECL place goes to the league cup winner. Wikipedia ECL 2021/22: "3 domestic league sixth-placed teams from associations 1-4 (EFL Cup winners for England)". 2022/23: "4 ... sixth-placed teams from associations 1-4 (EFL Cup winners for England)". 2023/24: "1 league cup winner from England" and "3 ... sixth-placed teams from associations 2-4". The ECL regulations allow "the winner of another official domestic competition" in place of the lowest league place (Art. 3.03, 2021/22 and 2023/24). The text "England: last place to the league cup winner" is in the note of the ECL row that holds England's rank.
- In the three seasons the English ECL club has the label "7th" in the Wikipedia team list (Tottenham Hotspur, West Ham United, Aston Villa). So the place went to the 7th-placed club. I did not check who won the league cup in each year.
- France: no league cup place. The Coupe de la Ligue ended after 2019/20, the last final was in 2020 (Wikipedia, Coupe de la Ligue). The French ECL club has a league position label (6th in 2021/22, 5th in 2022/23 and 2023/24). The row notes say so. This settles the point left open in the slice 23 section (France 2020/21). For 2020/21 itself the answer is still unknown.
- Other last places: Wikipedia marks some ECL clubs "PW" (winner of the end-of-season ECL play-offs of the league). This applies to Belgium, Netherlands, Austria, Denmark, Bulgaria, Romania, Slovakia, Wales, Northern Ireland and, in 2023/24, San Marino. The access list does not change. The table does not mark them.

### Russia

Russian clubs were suspended from 28 February 2022 (Wikipedia, CL 2022/23, note RUS). The suspension runs through 2022/23 and 2023/24.

- 2022/23, rank 8. kassiesa strikes the whole Russian row in CL, EL and ECL. Footnote 1 of the page: "Russian clubs are suspended from participation in UEFA competitions". Wikipedia writes "except Russia" in the access list tables and lists the changes "due to the suspension of Russia". Finding: the published list removed the Russian places and rebalanced. It did not keep the places and vacate them.
- 2023/24, rank 10. Same treatment. kassiesa strikes the row. Wikipedia writes "except Russia".
- Cascade 2022/23, CL: champion of 11 to G, of 13 to PO, of 15 to Q3, of 18 and 19 to Q2, runners-up of 10 and 11 to Q3-LP. EL: cup winners of 13 and 14 to PO, of 16 to Q3. ECL: cup winners of 18 and 19 to Q3, of 30 to 39 to Q2.
- Cascade 2023/24, CL: champion of 11 to G, of 13 to PO, of 15 to Q3, of 18 and 19 to Q2, runner-up of 11 to Q3-LP. EL: cup winners of 13 and 14 to PO, of 16 to Q3. ECL: cup winners of 17 to 19 to Q3, of 30 to 39 to Q2.
- In 2023/24 the Russian runner-up place has no replacement in CL Q2-LP, so that round has 4 clubs.
- The default rows keep the Russian places. The Russia row of each comp and season has its own row with the note. Source: kassiesa AccessList2022 and AccessList2023, Wikipedia CL, EL and ECL 2022/23 and 2023/24, "Distribution". Wikipedia cites the UEFA decision of 2 May 2022 (`https://www.uefa.com/news-media/news/0275-150c9887cacb-882c686f407f-1000/`). I did not read that page.

### Title holders

| season | CL holder | EL holder | ECL holder |
|---|---|---|---|
| 2021/22 | Chelsea qualified through the league. Turkey (11) to G, Denmark (13) to PO, Czech Republic (15) to Q3, Greece (18) and Serbia (19) to Q2 | Villarreal took a CL group place. Spain lost its N6 ECL place, not passed down | none, first season. The vacant EL group place moved the EL cup winners of 7, 13 and 16 up |
| 2022/23 | Real Madrid qualified through the league (1st). Ukraine (12) to G, Denmark (14) to PO, Serbia (16) to Q3, Greece (20) and Israel (21) to Q2 | Eintracht Frankfurt took a CL group place | Roma qualified for the EL through the league (6th). EL cup winners of 7, 15 and 17 moved up. ECL cup winners of 20, 40 and 41 moved up |
| 2023/24 | Manchester City qualified through the league (1st). Ukraine (12) to G, Switzerland (14) to PO, Czech Republic (16) to Q3, Turkey (20) and Cyprus (21) to Q2 | Sevilla took a CL group place | West Ham United took the EL group place. No vacancy |

Sources: Wikipedia CL, EL and ECL pages of the season ("Distribution"), kassiesa green marks, and the regulations (CL Art. 3.03 to 3.07, EL Art. 3.08 to 3.12). The league positions of Chelsea, Real Madrid, Manchester City and Roma are from the Wikipedia team lists. For Villarreal, Frankfurt, Sevilla and West Ham the team list shows only the label EL or ECL. I give no league position for them.

### Sum checks

Method: for each round, the rows (default list) plus the clubs that come from the round before and from other competitions give the round size. The default list counts both title holder places in the CL group stage and the ECL holder place in the EL group stage. Inputs from other rounds come from the kassiesa legend rows and the Wikipedia "Distribution" tables. "Real" is the round size in the kassiesa "total" row. A second check used the final list (green items in, struck items out). The final list agrees with the real size in every round of every season.

CL default rows (champions: PR 4 entrants, Q1 33, Q2-CP 3, Q3-CP 2, PO-CP 2, G 24 direct. League path: Q2-LP 6, Q3-LP 5):

| season | round | default | real | result |
|---|---|---|---|---|
| 2021/22 | PR 4, Q2-CP 20, Q2-LP 6, Q3-CP 12, Q3-LP 8, PO-CP 8, PO-LP 4, G 32 | same | same | agree |
| 2021/22 | Q1 | 34 | 32 | champions of 18 and 19 moved up to Q2 (holder rule) |
| 2022/23 | PR 4, Q2-CP 20, Q3-CP 12, Q3-LP 8, PO-CP 8, PO-LP 4, G 32 | same | same | agree |
| 2022/23 | Q1 | 34 | 30 | champions of 18, 19 (Russia) and 20, 21 (holder) moved up to Q2 |
| 2022/23 | Q2-LP | 6 | 4 | runners-up of 10 and 11 moved up to Q3-LP (Russia) |
| 2023/24 | PR 4, Q2-CP 20, Q3-CP 12, Q3-LP 8, PO-CP 8, PO-LP 4, G 32 | same | same | agree |
| 2023/24 | Q1 | 34 | 30 | champions of 18, 19 (Russia) and 20, 21 (holder) moved up to Q2 |
| 2023/24 | Q2-LP | 6 | 4 | Russian runner-up place removed, runner-up of 11 moved up to Q3-LP |

The G sum is 24 direct places, 2 title holder places and 6 play-off winners (4 CP and 2 LP).

EL (Q3 CP 10 are all CL Q2-CP losers):

| season | round | default | real | result |
|---|---|---|---|---|
| 2021/22 | Q3 CP 10, Q3 MP 6, PO 20, G 32 | same | same | agree |
| 2022/23 | Q3 CP 10, PO 20, G 32 | same | same | agree |
| 2022/23 | Q3 MP | 6 | 4 | cup winners of 13, 14, 15 moved up to PO, cup winners of 16 and 17 came in from ECL Q2, and the CL Q2-LP gave 2 losers, not 3 |
| 2023/24 | Q3 CP 10, PO 20, G 32 | same | same | agree |
| 2023/24 | Q3 MP | 6 | 4 | cup winners of 13 and 14 moved up to PO, the cup winner of 16 came in from ECL Q2, and the CL Q2-LP gave 2 losers, not 3 |

The EL G sum is 11 direct places, 1 ECL holder place, 10 play-off winners and 10 CL losers.

ECL:

| season | round | default | real | result |
|---|---|---|---|---|
| 2021/22 | Q1 | 72 | 66 | cup winners of 30 to 35 moved up to Q2 |
| 2021/22 | Q2 CP | 20 | 18 | CL Q1 losers: 17 in the default, 15 real |
| 2021/22 | Q3 MP | 52 | 54 | cup winners of 17 and 18 moved up from Q2 |
| 2021/22 | Q2 MP 90, Q3 CP 10, PO CP 10, PO MP 34, G 32 | same | same | agree |
| 2022/23 | Q1 | 72 | 60 | cup winners of 30 to 41 moved up to Q2 |
| 2022/23 | Q2 CP | 20 | 16 | CL Q1 losers: 17 in the default, 13 real |
| 2022/23 | Q3 MP | 52 | 54 | cup winners of 18, 19, 20 moved up, the Russian N3 place was removed |
| 2022/23 | Q2 MP 90, Q3 CP 10, PO CP 10, PO MP 34, G 32 | same | same | agree |
| 2023/24 | Q1 | 72 | 62 | cup winners of 30 to 39 moved up to Q2 |
| 2023/24 | Q2 CP | 20 | 16 | CL Q1 losers: 17 in the default, 13 real |
| 2023/24 | Q3 MP | 52 | 54 | cup winners of 17 to 19 moved up, the Russian N3 place was removed |
| 2023/24 | Q2 MP 90, Q3 CP 10, PO CP 10, PO MP 34, G 32 | same | same | agree |

Notes on the ECL sums:
- The ECL default PO MP contains 5 places in 2021/22. The Spanish N6 place is struck out there because Villarreal took a CL place. In the final list PO MP has 4 entrants and 34 clubs in total, because 3 EL Q3 losers join.
- CL Q1 losers go to the ECL champions path. The real count is 15 + 1 (2021/22) and 13 + 2 (2022/23 and 2023/24). The second figure goes to ECL Q3 CP (Wikipedia ECL, "Distribution").

### Task B. Round names from 2021/22 (spec Q370)

Source: the regulations of each season on kassiesa (`https://kassiesa.net/uefa/files/<YYYY>-<YY>-uefa-ecl-rules.pdf` and `-el-rules.pdf`), Article 13.02 "The competition stages are as follows". The text is the same in 2021/22, 2022/23 and 2023/24. Second source: the section headings of the Wikipedia season pages (read as wikitext).

| season_from | comp | round | name in the regulations | Wikipedia heading | source URL |
|---|---|---|---|---|---|
| 2021/22 | ecl | Q1 | first qualifying round (main path only) | First qualifying round | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | Q2 | second qualifying round (main path and champions path) | Second qualifying round | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | Q3 | third qualifying round (main path and champions path) | Third qualifying round | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | PO | play-offs (main path and champions path) | Play-off round | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | G | group stage (six matchdays) | Group stage | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | KO play-offs | knockout round play-offs | Knockout round play-offs | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | R16 | round of 16 | Round of 16 | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | QF | quarter-finals | Quarter-finals | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | SF | semi-finals | Semi-finals | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | F | final | Final | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | ecl | paths | main path, champions path | Main Path, Champions Path (table labels MP, CH) | https://kassiesa.net/uefa/files/2021-22-uefa-ecl-rules.pdf |
| 2021/22 | el | KO play-offs | knockout round play-offs | Knockout round play-offs | https://kassiesa.net/uefa/files/2021-22-uefa-el-rules.pdf |
| 2021/22 | el | R16 | round of 16 (replaces the round of 32 of 2009/10 to 2020/21) | Round of 16 | https://kassiesa.net/uefa/files/2021-22-uefa-el-rules.pdf |

Notes:
- The ECL name list is identical in all three seasons. Q1, Q2 and Q3 have the same names as in the EL before 2021/22.
- The EL knockout round play-offs are new in 2021/22. The 8 group runners-up play the 8 clubs that finish third in the CL group stage (EL regulations Art. 3.07 of 2022/23: "the eight clubs that finish the UEFA Champions League group stage in third place are admitted to the UEFA Europa League knockout round play-offs"). The EL round of 32 no longer exists. The regulations name Article 17 "Match system - knockout round play-offs".
- The EL has only one qualifying round from 2021/22, the third qualifying round (main path and champions path). The EL Q1 and Q2 of earlier seasons are gone. The names `first qualifying round` and `second qualifying round` do not occur in the EL regulations of these seasons.
- The ECL group stage feeds the knockout round play-offs in the same way (group runners-up against the EL group third places). Source: Wikipedia ECL 2021/22, "Distribution", table row "Preliminary knockout round (16 teams)". Wikipedia uses this phrase in the table and the heading "Knockout round play-offs" in the text. The regulations use "knockout round play-offs".
- The regulations call the group of rounds after the group stage "knockout stage". Wikipedia heads it "Knockout phase". The regulations name the round before the group stage "play-offs". Wikipedia heads it "Play-off round". This matches the points found for 2009/10 to 2020/21.
- CL names are unchanged from 2020/21 (preliminary round to third qualifying round, play-offs, group stage, round of 16, quarter-finals, semi-finals, final. Regulations Art. 13.02 of 2021/22 to 2023/24). The paths are "champions path" and "league path".

### Points not settled

- The cascade of ECL holder and Russia vacancies beyond the first named association is practice taken from kassiesa and Wikipedia. The EL regulations name only the rank 7 cup winner for the group stage (Art. 3.10 a). The other moves come from the two sources. The exact rank rule is unknown.
- The UEFA decision of 2 May 2022 on the Russian suspension was not read. Wikipedia cites it.
- League positions of Villarreal, Eintracht Frankfurt, Sevilla and West Ham United are not sourced. The notes do not state them.
- Whether Spain's vacated N6 place of 2021/22 should stay in the default list is a choice. It is in the default list because kassiesa strikes it and does not mark it green.
- 2021/22 has no `ecl-holder` row, because no ECL holder existed. The vacancy is described in the `el-holder` note of 2021/22.

### Sources of this section

- Access lists: `https://kassiesa.net/uefa/history/AccessList2021.html`, `AccessList2022.html`, `AccessList2023.html`.
- Regulations: `https://kassiesa.net/uefa/files/2021-22-uefa-cl-rules.pdf`, `-el-rules.pdf`, `-ecl-rules.pdf`, and the same for 2022-23 and 2023-24.
- Wikipedia: `https://en.wikipedia.org/wiki/2021%E2%80%9322_UEFA_Champions_League`, `_UEFA_Europa_League`, `_UEFA_Europa_Conference_League`, and the same for 2022%E2%80%9323 and 2023%E2%80%9324. Also `https://en.wikipedia.org/wiki/Coupe_de_la_Ligue`.
