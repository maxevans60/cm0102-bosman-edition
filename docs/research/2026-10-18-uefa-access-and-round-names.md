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

## Slice 25: 2024/25 to 2027/28

Research for 0.6.0 slice 25. Date of all page reads: 2026-10-19. The rows are in `docs/research/rules-0.6.0/uefa-access.tsv`. This slice adds 145 rows (532 data rows in total). From 2024/25 the group stage of all three competitions is a league phase of 36 clubs. The Europa Conference League is named UEFA Conference League from 2024/25 (title of the 2024/25 regulations). The comp values stay `ecl` and `ecl-holder`.

Rows added:

| season | cl | el | ecl | holder rows |
|---|---|---|---|---|
| 2024/25 | 13 | 9 | 10 | cl-holder, el-holder, ecl-holder |
| 2025/26 | 12 | 10 | 11 | cl-holder, el-holder, ecl-holder |
| 2026/27 | 13 | 10 | 11 | cl-holder, el-holder, ecl-holder |
| 2027/28 | 13 | 10 | 11 | cl-holder, el-holder, ecl-holder |

### Task A method

1. Source: the kassiesa access list of each season. 2024/25 is `history/AccessList2024.html`, 2025/26 is `history/AccessList2025.html`, 2026/27 is `AccessList2026.html`, 2027/28 is the PDF `files/2027-28-uefa-access-list.pdf` (dated 8 September 2026). A script read the HTML cells, the colour of each text segment and the strike marks. This is the method of slice 24. A green segment is a place that rebalancing created. A red struck segment is the default place that it replaced. The default list is every place without a green mark.
2. 2027/28: the PDF has no colours. A script read the text with its column positions. The rounds of each place come from the column positions. This is an inference, but the result is the same list as 2024/25 to 2026/27, rank by rank.
3. The default list is the same in all four seasons. Only the nation at each rank changes. The rows give the default places of each rank, before the removal of Russia.
4. Order of places: CL by league position (CH, N2, N3, N4). EL and ECL: cup winner first, then league places by position. A nation with places in EL and ECL has the cup winner and the EL league places in the EL row and the rest in the ECL row. This is the order of slice 24.
5. Second source: the Wikipedia CL, EL and Conference League season pages, table "Qualified teams", read as wikitext. A script counted the clubs per association and entry round, only clubs with a league position or cup winner label. It compared them with the final kassiesa list (every item that is not struck out). Result: 2024/25 235 clubs and 2026/27 236 clubs, no difference. 2025/26: one difference, Republic of Ireland (see the note of ECL ranks 34 to 38). The difference is a script artefact: kassiesa marks the Irish N2 place both green and red. Both sources describe the same change. The note of each row ends with `agree`.
6. 2027/28 second source: the Wikipedia CL 2027/28 page, table "Distribution", gives the same CL brackets. The EL and Conference League 2027/28 pages do not exist (the titles redirect). The EL and ECL rows of 2027/28 have one source. Their notes say so.
7. Third source for the rules: the regulations of the season on kassiesa, `https://kassiesa.net/uefa/files/<YYYY>-<YY>-uefa-<cl|el|co>-rules.pdf`, 2024/25 to 2026/27. No 2027/28 regulations were found. The 2027/28 holder rows cite the 2026/27 regulations.

### Place tokens

No new token is needed. The tool reads every row (checked with `read_access` and `LAST` set to 2027 in a scratch copy). `make_uefa_table.py --check` still passes, because `LAST` is 2023.

- `G` is a league phase place from 2024/25. The tool keeps working because the token has the same role as the group stage place.
- CL: the champions path is `Q1`, `Q2-CP`, `PO-CP` (no direct `Q3-CP` place). The league path is `Q2-LP`, `Q3-LP` (no direct `PO-LP` place).
- EL: the regulations name a main path (Q1, Q2, Q3) and a champions path (Q3 only, CL losers). Art. 13.02 of 2024/25 and 2026/27. Direct places are only in the main path, so `Q1`, `Q2`, `Q3` and `PO` cover them. A Europa League "Q3 LP" token is not needed. The EL Q3 main path is `Q3`, as in slice 24.
- ECL: the champions path (Q2, Q3, PO) holds only CL and EL losers. Direct places are `Q1`, `Q2` and `PO` (no direct `Q3` place).

### Changes from the cycle 2021/22 to 2023/24

- CL: ranks 1 to 4 G G G G. Rank 5 G G G Q3-LP. Rank 6 G G Q3-LP. Ranks 7 to 9 G Q3-LP. Rank 10 G Q2-LP. Ranks 11 to 14 PO-CP Q2-LP. Rank 15 Q2-CP Q2-LP. Ranks 16 to 22 Q2-CP. Ranks 23 to 55 Q1. The preliminary round ends. Rank 5 gets a fourth place. Ranks 13 to 15 lose the Q3 places.
- EL: cup winners of ranks 1 to 7 and N5 of ranks 1 to 5 to the league phase. Ranks 6 and 7 add a Q2 place. Ranks 8 to 12: PO Q2. Ranks 13 to 15: Q3. Ranks 16 to 33: Q1. EL Q1 and Q2 return.
- ECL: ranks 1 to 5 PO. Ranks 6 to 12 Q2. Ranks 13 to 29 Q2 Q2. Ranks 30 to 33 Q2 Q1. Ranks 34 to 38 Q2 Q1 Q1. Ranks 39 to 50 Q1 Q1 Q1. Ranks 51 to 55 Q1 Q1.
- Holder vacancies now go to clubs by club coefficient, not by association rank (CL Art. 3.04, EL Art. 3.09). The EL holder rule of slice 24 (N3 of association 5) ends.

### Task B. European Performance Spots

Rule (CL regulations Art. 3.07 of 2024/25): the two associations with the best season association coefficient of the season before each get one more league phase place. The place goes to the club with the highest league position of those that do not qualify for the league phase through the league, after the holder vacancies are filled. Art. 3.08: if that club also had an EL or ECL place, the other clubs of the nation each move up one place, and the best club with no place gets the last ECL place. The CL and EL title holders cannot take an EPS (Wikipedia CL 2024/25, Association team allocation).

| season | EPS nations (rank) | coefficient season | club (Wikipedia team list) | sources |
|---|---|---|---|---|
| 2024/25 | Germany (3), Italy (4) | 2023/24 | Borussia Dortmund 5th, Bologna 5th | kassiesa AccessList2024 footnote "Italy and Germany secured an EPS spot", Wikipedia CL 2024/25 |
| 2025/26 | England (1), Spain (3) | 2024/25 | Newcastle United 5th, Villarreal 5th | kassiesa AccessList2025 footnote, Wikipedia CL 2025/26 |
| 2026/27 | England (1), Spain (3) | 2025/26 | Liverpool 5th, Real Betis 5th | kassiesa AccessList2026 footnote, Wikipedia CL 2026/27 |
| 2027/28 | unknown | 2026/27 | unknown | the season 2026/27 is not finished. The PDF has a column "2x EPS" with no nation |

- No row includes an EPS place. The rows of ranks 1 to 4 hold the base list (four CL places). kassiesa marks the EPS green in 2024/25 and 2025/26 (N5 in the CL, and the moved EL and ECL places). In 2026/27 kassiesa shows England and Spain with CH to N5 and no green mark. I removed N5 from the CL, and N6 and N7 move back to N5 and N6, by hand.
- The EPS is in the note of the `cl` row of ranks 1 to 4 of each season. The tool has no comp for it. The game must add the EPS by the rule above.

### League cup places

- England: the last ECL place goes to the league cup winner, as in slice 24. Wikipedia ECL 2024/25 and 2026/27, Distribution: five sixth-placed clubs of associations 1 to 5, with the EFL Cup winners for England. The Conference League regulations Art. 3.03 allow "the winner of another official domestic competition" in place of the lowest league place. The note of the ECL row of ranks 1 to 5 holds "England: last place to the league cup winner".
- The English ECL clubs: Chelsea (6th, 2024/25), Crystal Palace (FA Cup winner, 2025/26), Brighton & Hove Albion (8th, 2026/27). Crystal Palace qualified for the EL and moved to the ECL by a UEFA decision of 11 July 2025 (Wikipedia ECL 2025/26, Note ENG). Nottingham Forest moved to the EL. The access list does not change.
- France: no league cup place. The French ECL club has a league position label in all three seasons (7th).

### Liechtenstein

Rank 34 (2024/25), 40 (2025/26), 44 (2026/27), 42 (2027/28). As before: `-` in CL and EL, one ECL place for the cup winner. It enters at the first ECL round of its rank: Q2 in 2024/25, Q1 from 2025/26.

### Russia

Russia is suspended in all four seasons (rule Q365 for 2027/28). Rank 18 (2024/25), 22 (2025/26), 26 (2026/27), 28 (2027/28).

- 2024/25 to 2026/27: kassiesa strikes the whole Russian row. Footnote: "Until further notice Russian clubs are suspended from participation in UEFA competitions". Wikipedia lists the changes "due to the suspension of Russia". Finding: the published list removes the Russian places and rebalances, as in slice 24.
- CL cascade: champions of 23 and 24 to Q2 (2024/25 Sweden and Poland, 2025/26 Cyprus and Hungary), champion of 23 to Q2 (2026/27 Ukraine, 2027/28 Ukraine).
- EL cascade: cup winner of 16 to Q2 (Norway, Denmark, Denmark).
- ECL cascade: cup winners of 39 to 44 to Q2 in each season.
- 2027/28: the PDF still lists Russia (CL Q1, EL Q1, ECL Q2 Q2). Wikipedia CL 2027/28 says Russia is suspended and moves Ukraine (23) to Q2. The EL and ECL cascade of 2027/28 is unknown. Inference from the three seasons before: EL cup winner of 16 to Q2, ECL cup winners of the next ranks to Q2.
- The rows keep the Russian default places. The Russia rank of each comp and season has its own row with the note.

### Title holders

| season | CL holder | EL holder | ECL holder |
|---|---|---|---|
| 2024/25 | Real Madrid qualified through the league (1st). Shakhtar Donetsk (14) to the league phase, Dinamo Zagreb (19) to PO, Ferencvaros (25) and Qarabag (29) to Q2 | Atalanta qualified through the league (4th). Benfica (Portugal N2) to the league phase, Slavia Prague and Red Bull Salzburg (N2 of 15 and 10) to Q3-LP | Olympiacos took the EL league phase place. Greece lost its ECL Q2 place. Cup winners of 45 and 46 to ECL Q2 |
| 2025/26 | Paris Saint-Germain qualified through the league. Olympiacos (15) to the league phase, Slovan Bratislava (29) and Qarabag (28) to Q2 | Tottenham Hotspur (17th) took the CL place. No vacancy | Chelsea qualified for the CL. Dinamo Zagreb (Croatia cup place) to the EL league phase. Cup winner of 34 (Finland) to EL Q1. Cup winners of 45 and 46 to ECL Q2 |
| 2026/27 | Paris Saint-Germain qualified through the league. Shakhtar Donetsk (23) to the league phase, Slovan Bratislava (27) and Celje (28) to Q2 | Aston Villa qualified through the league. Sporting CP (Portugal N2) to the league phase, Bodo/Glimt and Olympiacos (N2 of 11 and 12) to Q3-LP | Crystal Palace took the EL league phase place. No vacancy listed |
| 2027/28 | unknown | unknown | unknown |

Sources: Wikipedia CL, EL and ECL pages of the season (Distribution), kassiesa green marks and footnotes, and the regulations (CL Art. 3.03 to 3.08, EL Art. 3.08 and 3.09). The 2026/27 finals: CL 5 June 2027, EL 26 May 2027, ECL 2 June 2027 (Wikipedia schedules).

### Sum checks

Method of slice 24. Default list, plus the clubs from the round before and from other competitions, gives the round size. Inputs from other rounds come from the kassiesa legend rows. "Real" is the round size in the kassiesa "total" row and in `uefa.tsv`. A second check used the final kassiesa list. It agrees with the real size in every round of 2024/25 to 2026/27.

CL (default: Q1 32, Q2-CP 8, PO-CP 4, Q2-LP 6, Q3-LP 5, league phase 25 direct):

| round | default | real 2024/25, 2025/26, 2026/27 | result |
|---|---|---|---|
| Q1 | 32 | 28, 28, 28 | Russia and the CL holder cascade (see above) |
| Q2-CP | 8 + 16 = 24 | 24 | agree |
| Q3-CP | 12 winners | 12 | agree |
| PO-CP | 4 + 6 = 10 | 10 | agree |
| Q2-LP | 6 | 4, 6, 4 | EL holder cascade in 2024/25 and 2026/27 (two N2 to Q3-LP) |
| Q3-LP | 5 + 3 = 8 | 8 | agree |
| PO-LP | 4 winners | 4 | agree |
| league phase | 25 + 2 holders + 2 EPS + 5 + 2 = 36 | 36 | agree |

2027/28: Wikipedia gives Q1 30 and Q2-LP 6. The holder cascade is unknown.

EL (default: Q1 18, Q2 7, Q3 3, PO 5, league phase 12 direct):

| round | default | real 2024/25, 2025/26, 2026/27 | result |
|---|---|---|---|
| Q1 | 18 | 12, 16, 12 | cup winners moved to Q2 (Russia, EL holder cascade), Croatia to the league phase and Finland in (2025/26) |
| Q2 | 7 + 9 = 16 | 18, 16, 18 | follows from Q1 |
| Q3 CP | 12 CL Q2-CP losers | 12 | agree |
| Q3 MP | 3 + 8 + 3 CL Q2-LP losers = 14 | 14 | agree (2 CL losers and 9 Q2 winners in 2024/25 and 2026/27) |
| PO | 5 + 7 + 6 + 6 CL Q3-CP losers = 24 | 24 | agree |
| league phase | 12 + 1 ECL holder + 12 + 5 + 2 + 4 = 36 | 36 | agree |

ECL (default: Q1 58, Q2 MP 50, PO MP 5):

| round | default | real 2024/25, 2025/26, 2026/27 | result |
|---|---|---|---|
| Q1 | 58 | 50, 48, 52 | cup winners of 39 to 46, 39 to 47 and 39 to 44 moved to Q2. 2025/26 also the Irish N2 |
| Q2 MP | 50 + 29 + 9 EL Q1 losers = 88 | 86, 88, 86 | follows from Q1 and from the EL Q1 size |
| Q2 CP | 12 CL Q1 losers | 12 | agree. The default CL Q1 gives 16 losers. The real CL Q1 is 28 in each season, so 14 losers: 12 to Q2 CP and 2 to Q3 CP |
| Q3 MP | 44 + 8 EL Q2 losers = 52 | 52 | agree |
| Q3 CP | 6 + 2 = 8 | 8 | agree |
| PO MP | 5 + 26 + 7 EL Q3 MP losers = 38 | 38 | agree |
| PO CP | 4 + 6 EL Q3 CP losers = 10 | 10 | agree |
| league phase | 19 + 5 + 12 EL PO losers = 36 | 36 | agree |

### Task C. Dates of the round tables

Source: the "Schedule" table of each Wikipedia season page. Second source for 2024/25 and 2025/26: Annex C (match calendar) of the CL regulations, read with column positions. It shows the same weeks for the league phase and the knockout rounds. For 2026/27 and 2027/28 I found no second source. The weekdays are calculated from the dates.

League phase match days:

| comp | MD | 2024/25 | 2025/26 | 2026/27 | 2027/28 |
|---|---|---|---|---|---|
| CL | 1 | Tue 17 to Thu 19 Sep 2024 | Tue 16 to Thu 18 Sep 2025 | Tue 8 to Thu 10 Sep 2026 | Tue 7 to Thu 9 Sep 2027 |
| CL | 2 | Tue 1, Wed 2 Oct | Tue 30 Sep, Wed 1 Oct | Tue 13, Wed 14 Oct | 12 to 14 Oct |
| CL | 3 | Tue 22, Wed 23 Oct | Tue 21, Wed 22 Oct | Tue 20, Wed 21 Oct | 19 to 21 Oct |
| CL | 4 | Tue 5, Wed 6 Nov | Tue 4, Wed 5 Nov | Tue 3, Wed 4 Nov | 2 to 4 Nov |
| CL | 5 | Tue 26, Wed 27 Nov | Tue 25, Wed 26 Nov | Tue 24, Wed 25 Nov | 23 to 25 Nov |
| CL | 6 | Tue 10, Wed 11 Dec | Tue 9, Wed 10 Dec | Tue 8, Wed 9 Dec | 7 to 9 Dec |
| CL | 7 | Tue 21, Wed 22 Jan 2025 | Tue 20, Wed 21 Jan 2026 | Tue 19, Wed 20 Jan 2027 | 18 to 20 Jan 2028 |
| CL | 8 | Wed 29 Jan 2025 | Wed 28 Jan 2026 | Wed 27 Jan 2027 | Wed 26 Jan 2028 |
| EL | 1 | Wed 25, Thu 26 Sep 2024 | Wed 24, Thu 25 Sep 2025 | Wed 16, Thu 17 Sep 2026 | unknown |
| EL | 2 to 8 | Thu 3 Oct, 24 Oct, 7 Nov, 28 Nov, 12 Dec, 23 Jan, 30 Jan | Thu 2 Oct, 23 Oct, 6 Nov, 27 Nov, 11 Dec, 22 Jan, 29 Jan | Thu 15 Oct, 22 Oct, 5 Nov, 26 Nov, 10 Dec, 21 Jan, 28 Jan | unknown |
| ECL | 1 to 6 | Thu 3 Oct, 24 Oct, 7 Nov, 28 Nov, 12 Dec, 19 Dec 2024 | Thu 2 Oct, 23 Oct, 6 Nov, 27 Nov, 11 Dec, 18 Dec 2025 | Thu 15 Oct, 22 Oct, 5 Nov, 26 Nov, 10 Dec, 17 Dec 2026 | unknown |

- League phase draw: Thu 29 Aug 2024 (CL), Fri 30 Aug 2024 (EL, ECL). Thu 28 Aug 2025 (CL), Fri 29 Aug 2025 (EL, ECL). Thu 27 Aug 2026 (CL), Fri 28 Aug 2026 (EL, ECL). Thu 26 Aug 2027 (CL).
- 2027/28 CL: Wikipedia gives date ranges of three days (Tuesday to Thursday) for MD 1 to 7. The weekdays are Tuesday to Thursday.
- 2026/27: MD 2 and MD 3 of all three competitions are in two weeks in a row (13 to 22 October). There is no match day in the second half of September after MD 1. Single source.

Knockout phase (draw, first leg, second leg):

| comp | round | 2024/25 | 2025/26 | 2026/27 | 2027/28 |
|---|---|---|---|---|---|
| CL | KO play-offs | Fri 31 Jan 2025, Tue 11 to Wed 12 Feb, Tue 18 to Wed 19 Feb | Fri 30 Jan 2026, 17 to 18 Feb, 24 to 25 Feb | Fri 29 Jan 2027, 16 to 17 Feb, 23 to 24 Feb | Fri 28 Jan 2028, 15 to 16 Feb, 22 to 23 Feb |
| CL | R16 | Fri 21 Feb 2025, 4 to 5 Mar, 11 to 12 Mar | Fri 27 Feb 2026, 10 to 11 Mar, 17 to 18 Mar | Fri 26 Feb 2027, 9 to 10 Mar, 16 to 17 Mar | Fri 25 Feb 2028, 7 to 8 Mar, 14 to 15 Mar |
| CL | QF | 8 to 9 Apr, 15 to 16 Apr | 7 to 8 Apr, 14 to 15 Apr | 6 to 7 Apr, 13 to 14 Apr | 4 to 5 Apr, 11 to 12 Apr |
| CL | SF | 29 to 30 Apr, 6 to 7 May | 28 to 29 Apr, 5 to 6 May | 27 to 28 Apr, 4 to 5 May | 25 to 26 Apr, 2 to 3 May |
| CL | F | Sat 31 May 2025, Munich | Sat 30 May 2026, Budapest | Sat 5 Jun 2027, Madrid | Sat 27 May 2028 (Munich in `uefa.tsv`) |
| EL | KO play-offs | Fri 31 Jan 2025, Thu 13 Feb, Thu 20 Feb | Fri 30 Jan 2026, Thu 19 Feb, Thu 26 Feb | Fri 29 Jan 2027, Thu 18 Feb, Thu 25 Feb | unknown |
| EL | R16 | Fri 21 Feb 2025, Thu 6 Mar, Thu 13 Mar | Fri 27 Feb 2026, Thu 12 Mar, Thu 19 Mar | Fri 26 Feb 2027, Thu 11 Mar, Thu 18 Mar | unknown |
| EL | QF, SF | 10, 17 Apr. 1, 8 May | 9, 16 Apr. 30 Apr, 7 May | 8, 15 Apr. 29 Apr, 6 May | unknown |
| EL | F | Wed 21 May 2025, Bilbao | Wed 20 May 2026, Istanbul | Wed 26 May 2027, Frankfurt | unknown |
| ECL | KO play-offs | Fri 20 Dec 2024, Thu 13 Feb, Thu 20 Feb | Fri 16 Jan 2026, Thu 19 Feb, Thu 26 Feb | Fri 15 Jan 2027, Thu 18 Feb, Thu 25 Feb | unknown |
| ECL | R16, QF, SF | as the EL of the same season (same Thursdays, same R16 draw) | as the EL | as the EL | unknown |
| ECL | F | Wed 28 May 2025, Wroclaw | Wed 27 May 2026, Leipzig | Wed 2 Jun 2027, Istanbul | unknown |

The CL knockout legs are on Tuesday and Wednesday. The EL and ECL legs are on Thursday. The draws for QF and SF are with the R16 draw (2024/25: one draw on 21 February 2025 for R16, QF and SF).

### League phase draw and knockout bracket

Source: regulations of 2024/25, CL Art. 16, 17 and 19, EL Art. 16, 17 and 19, Conference League Art. 16 and 17. The text is the same in 2026/27.

- CL and EL: 36 clubs in four pots of nine by club coefficient. CL only: the CL title holder is the top seed in pot 1 (CL Art. 16.01). The EL text has no holder rule for pot 1. Each club meets two opponents from each pot, one at home and one away. Eight matches, four at home.
- ECL: six pots of six by club coefficient. Each club meets one opponent from each pot. Pots 1 and 2, 3 and 4, 5 and 6 are paired. Each club plays one opponent of each pair at home and the other away. Six matches, three at home.
- All three: "In principle, teams from the same association cannot be drawn against each other and each team may play against a maximum of two opponents from any other one association" (Art. 16.02). UEFA may change the conditions to avoid a deadlock (Art. 16.03).
- CL and EL Art. 17.02: in principle no club plays more than two home or two away matches in a row, and each club plays one home and one away match in the first two and in the last two match days.
- Table places: 1 to 8 to the round of 16, 9 to 24 to the knockout phase play-offs, 25 to 36 out (Art. 17.04). Tie rules of the table: Art. 18 (goal difference, goals, away goals, wins, away wins, opponents' points, opponents' goal difference, opponents' goals, disciplinary points, club coefficient).
- Knockout phase play-offs (Art. 19.02): seeded pairs 9 and 10, 11 and 12, 13 and 14, 15 and 16. Unseeded pairs 17 and 18, 19 and 20, 21 and 22, 23 and 24. Clubs 9 or 10 play clubs 23 or 24, 11 or 12 play 21 or 22, 13 or 14 play 19 or 20, 15 or 16 play 17 or 18. The seeded club plays the second leg at home.
- Round of 16 (Art. 19.03): seeded pairs 1 and 2, 3 and 4, 5 and 6, 7 and 8, each drawn against the winner of a fixed play-off tie. From the 2024/25 CL bracket (Wikipedia page of the 2024/25 CL knockout phase): 1 or 2 against the winner of 15/16 v 17/18, 3 or 4 against the winner of 13/14 v 19/20, 5 or 6 against the winner of 11/12 v 21/22, 7 or 8 against the winner of 9/10 v 23/24. The seeded club plays the second leg at home. The bracket picture is Annex B. Its text layer is not readable, so this mapping is from the Wikipedia bracket (inference from one season of results).
- Quarter-finals, semi-finals and final follow the bracket (Art. 19.04). In the 2024/25 bracket the 1/2 tie meets the 7/8 tie and the 3/4 tie meets the 5/6 tie in the quarter-finals. A draw decides only which club plays the first leg at home. No association protection in the knockout phase (Wikipedia, draw procedure).
- Two-legged ties: no away goals rule. Level on aggregate: 2 x 15 minutes extra time, then penalties (CL Art. 21.01).

### Task B (Q370). Round names from 2024/25

Source: Article 13.02 of the regulations of each competition, 2024/25 (first season of the names) and 2026/27 (same text). Second source: the section headings of the Wikipedia season pages.

| season_from | comp | round | name in the regulations | Wikipedia heading | source URL |
|---|---|---|---|---|---|
| 2024/25 | cl, el, ecl | G | league phase | League phase | https://kassiesa.net/uefa/files/2024-25-uefa-cl-rules.pdf |
| 2024/25 | cl, el, ecl | KO play-offs | knockout phase play-offs | Knockout phase play-offs | https://kassiesa.net/uefa/files/2024-25-uefa-cl-rules.pdf |
| 2024/25 | cl, el, ecl | R16, QF, SF, F | round of 16, quarter-finals, semi-finals, final | Round of 16, Quarter-finals, Semi-finals, Final | https://kassiesa.net/uefa/files/2024-25-uefa-cl-rules.pdf |
| 2024/25 | cl, el, ecl | stage after the league phase | knockout phase | Knockout phase | https://kassiesa.net/uefa/files/2024-25-uefa-cl-rules.pdf |
| 2024/25 | cl | qualifying | first, second and third qualifying round (champions path), second and third qualifying round (league path), play-offs (champions path and league path) | Play-off round | https://kassiesa.net/uefa/files/2024-25-uefa-cl-rules.pdf |
| 2024/25 | el | qualifying | first, second and third qualifying round (main path), third qualifying round (champions path), play-offs | Play-off round | https://kassiesa.net/uefa/files/2024-25-uefa-el-rules.pdf |
| 2024/25 | ecl | qualifying | first, second and third qualifying round (main path), second and third qualifying round (champions path), play-offs (main path and champions path) | Play-off round | https://kassiesa.net/uefa/files/2024-25-uefa-co-rules.pdf |
| 2024/25 | ecl | competition | UEFA Conference League | UEFA Conference League | https://kassiesa.net/uefa/files/2024-25-uefa-co-rules.pdf |

Notes:
- "League phase" replaces "group stage" in all three. The CL preliminary round ends. The EL "knockout round play-offs" of 2021/22 to 2023/24 become "knockout phase play-offs".
- kassiesa writes "League Stage" in its tables. The regulations write "league phase". The table uses the regulations.
- The Wikipedia EL schedule tables of 2025/26 and 2026/27 write "Knockout round play-offs". The section headings of the same pages and the regulations write "Knockout phase play-offs".
- The regulations name the round before the league phase "play-offs". Wikipedia heads it "Play-off round", as in earlier seasons.

### Conflicts for Max

1. Path names in `uefa.tsv`. The rounds lists of the EL write "Q3 LP" in 2024/25 and 2025/26 (uefa-189, uefa-197) and "Q3 MP" in 2026/27 (uefa-205). The Conference League rows write "Q2 LP", "Q3 LP", "PO LP" (uefa-190, uefa-198, uefa-206). The regulations name the EL and ECL path "main path" (Art. 13.02 of 2024/25 and 2026/27). Wikipedia writes "MP". kassiesa writes "Cup Winners / League Path". The tool accepts both. Decide whether these rows change to MP.
2. EPS. The access table has no place for the EPS. The rows give the base list. The game needs a rule (or a new comp) to add the two EPS places each season. The nations of 2027/28 are unknown until May 2027.
3. 2027/28 EL and ECL. The rows rest on one source (the PDF, read by column position). The Russia cascade and the holder moves of 2027/28 are unknown.

### Points not settled

- The EPS of 2026/27 is not marked on the kassiesa page. I removed it by hand, by the rule of Art. 3.08 and the Wikipedia team list.
- 2026/27: kassiesa strikes the Ukrainian champion in Q2 and in Q1. I took Q1 as the default place (rank 23 is a Q1 rank, and Wikipedia moves Ukraine to Q2 for Russia).
- Second source for the dates of 2026/27 and 2027/28: none found. The 2026/27 match days 2 and 3 in two weeks in a row are unusual.
- The round of 16 bracket mapping comes from the 2024/25 results bracket, not from the regulations text.
- The EL Q2 has only the main path from 2024/25. The tool reads EL "Q2" as a place token, so it needs no path.

### Sources of this section

- Access lists: `https://kassiesa.net/uefa/history/AccessList2024.html`, `https://kassiesa.net/uefa/history/AccessList2025.html`, `https://kassiesa.net/uefa/AccessList2026.html`, `https://kassiesa.net/uefa/files/2027-28-uefa-access-list.pdf`.
- Regulations: `https://kassiesa.net/uefa/files/2024-25-uefa-cl-rules.pdf`, `-el-rules.pdf`, `-co-rules.pdf`, and the same for 2025-26 and 2026-27.
- Wikipedia: `https://en.wikipedia.org/wiki/2024%E2%80%9325_UEFA_Champions_League`, `_UEFA_Europa_League`, `_UEFA_Conference_League`, and the same for 2025%E2%80%9326 and 2026%E2%80%9327. `https://en.wikipedia.org/wiki/2027%E2%80%9328_UEFA_Champions_League`. `https://en.wikipedia.org/wiki/2024%E2%80%9325_UEFA_Champions_League_knockout_phase`.
