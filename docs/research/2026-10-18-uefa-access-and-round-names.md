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
