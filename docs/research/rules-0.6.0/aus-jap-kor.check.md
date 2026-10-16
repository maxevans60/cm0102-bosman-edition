# Second-source check of the Australia, Japan and South Korea rules table

Check of part `aus-jap-kor` (`aus-jap-kor.tsv`, 459 rows, and `aus-jap-kor-clubs.tsv`). Check date 2026-10-16. The first agent's files are unchanged.

## Method

- I checked every filled fact field of every row. A field with `n/a` gets no check row. The result is 2,077 rows in `aus-jap-kor.check.tsv`.
- Leagues and most cups: RSSSF season pages, read raw with `curl` and parsed by script. The pages are `tablesj/jap02.html` to `jap04.html` and `jpn05.html` to `jpn2025.html` (Japan), `tabless/skor02.html` to `skor2025.html` (South Korea), and `tablesa/aus02.html` to `aus2026.html` (Australia). I compared club counts, the promotion, relegation and play-off marks, stage and split tables, cup group sizes, knockout rounds and legs.
- Emperor's Cup entrants: a script counted the distinct clubs in the RSSSF match lists and the round in which each club first plays. The count is exact where the page lists every round (2002 to 2004, 2007, 2013, 2015 to 2019, 2022, 2024). For other years the script count is between 87 and 92 because of spelling variants, so I give no count.
- Rules: Goal Japan, Soccer King, Sportiva, J.League and JFA pages for Japan. Kyunghyang, Kyeonggi Ilbo, OSEN, Yonhap (via Daum), Korea JoongAng Daily and K League United for South Korea. Melbourne Victory, Western United and Sydney FC club pages, ESPN, australiacup.com.au and Football Australia for Australia. I used a page only where its publisher differs from the first source of that fact.
- `aus-jap-kor-clubs.tsv`: spot check of one season per division against the RSSSF tables (see below).
- WebFetch passes pages through a summary model. I asked it to quote the sentence. Where I relied on a search-result summary only, the note says so.

## Counts per verdict

| Verdict | Rows |
|---|---|
| agree | 1,181 |
| no second source | 880 |
| disagree | 16 |
| found | 0 |

Most `no second source` rows are `unknown` fields (571 of 2,077 fields), substitutes and bench sizes before the dated rule changes, the transfer windows and work permits, inferences for 2026/27 and 2027/28, and the entrant totals of the Australia Cup.

## Disagreements

1. **aus-jap-kor-42, -43, -44 bench (A-League 2019/20 to 2021/22).** First: 5 (inference), from https://en.wikipedia.org/wiki/A-League_Men. Second: from 2019/20 a club may name six substitutes with two U-23 players in the starting list, or seven with three. Sources: https://wufc.com.au/?p=273 (FFA release, 11 Oct 2019) and https://sydneyfc.com/?p=96960 (14 Apr 2022, rule still in force in 2021/22). Rows 45 to 47 are `no second source`, but the U-23 rule may still apply there.
2. **aus-jap-kor-136, -137 format (Emperor's Cup 2002, 2003).** First: "JFL and J2 clubs enter later, J1 clubs last", from https://en.wikipedia.org/wiki/Emperor%27s_Cup. Second: 80 clubs. 64 start in round 1, the J2 and JFL clubs among them (for example Oita Trinita, Mito HollyHock, Albirex Niigata). The 16 J1 clubs enter in round 3. Sources: https://www.rsssf.org/tablesj/jap02.html and https://www.rsssf.org/tablesj/jap03.html
3. **aus-jap-kor-154 format (Emperor's Cup 2020).** First: the normal format of 2008 to 2023, from the Wikipedia page above. Second: 48 amateur sides in regional rounds, the top J2 and J3 clubs from the quarter-finals and the top two J1 clubs from the semi-finals. Source: https://www.jfa.jp/eng/news/00025339/
4. **aus-jap-kor-178 format (J.League Cup 2017).** First: "ACL clubs bye to the quarter-finals; others in two groups of 7 or 8", from https://en.wikipedia.org/wiki/J.League_Cup. Second: two groups of 7, then a play-off first round before the quarter-finals. Source: https://www.rsssf.org/tablesj/jpn2017.html
5. **aus-jap-kor-181 format (J.League Cup 2020).** First: "group stage, play-off stage, knockout", from the Wikipedia page above. Second: "Group stage and knock-out stage are shortened to one-leg format". Four groups of 4, then the quarter-finals, with no play-off stage. Source: https://www.rsssf.org/tablesj/jpn2020.html
6. **aus-jap-kor-185 bench (J.League Cup 2024).** First: "7; the 18-player entry limit did not cover this cup", from https://www.soccer-king.jp/news/japan/jl/20241029/1949786.html. Second: in 2024 only the YBC Levain Cup already allowed 20 players, so the bench was 9. Source: https://www.goal.com/jp/%E3%83%8B%E3%83%A5%E3%83%BC%E3%82%B9/2024-1029-jleague-member/bltdbd75ba75c880dbd. The first agent's own note says that the 18-player limit did not cover this cup, so the value 7 looks like a slip.
7. **aus-jap-kor-321 foreign (South Korea 2025).** First: "K League 1: 6 in the matchday squad, 4 on the pitch; K League 2: 4 plus 1 ASEAN", from https://www.kleagueunited.com/2024/02/whats-changing-in-k-league-in-2024.html. Second: K League 1 up to 6 registered and 4 on the pitch. The K League 2 ASEAN quota was abolished for 2025, with up to 5 registered and 4 on the pitch. Source: https://osen.co.kr/article/G1112466302 (30 Nov 2024).
8. **aus-jap-kor-411 format (Korean FA Cup 2007).** First: one-match final, from https://en.wikipedia.org/wiki/Korea_Cup_(domestic). Second: two-legged final, Chunnam Dragons 3-2 Pohang Steelers (25 Nov) and Pohang 1-3 Chunnam (2 Dec). Source: https://www.rsssf.org/tabless/skor07.html
9. **aus-jap-kor-423 format (Korean FA Cup 2019).** First: single-elimination knockout with a two-legged final, from the Wikipedia page above. Second: the semi-finals were also two-legged. Source: https://www.rsssf.org/tabless/skor2019.html
10. **aus-jap-kor-428, -429 format (Korea Cup 2024, 2025).** First: single-elimination knockout with a one-match final, from the Wikipedia page above. Second: two-legged semi-finals, then a one-match final. Sources: https://www.rsssf.org/tabless/skor2024.html and https://www.rsssf.org/tabless/skor2025.html
11. **aus-jap-kor-438, -439 format (Korean League Cup 2007, 2008).** First: "two groups, then quarter-finals, semi-finals, final", from https://en.wikipedia.org/wiki/Korean_League_Cup. Second: two groups of 6. The top two of each group and two ACL clubs play a first round of two ties, then semi-finals and a final. There are no quarter-finals. Sources: https://www.rsssf.org/tabless/skor07.html and https://www.rsssf.org/tabless/skor08.html

## Found values

No `unknown` field was found. These values fill gaps that the first agent marked inside filled fields:

- Emperor's Cup entrants (RSSSF): 80 in 2002, 2003 and 2004, 80 in 2007, and 88 in 2013, 2015 to 2019, 2022 and 2024. From 2013, 48 clubs (47 prefectural winners and one amateur seed) play round 1 and the J1 and J2 clubs enter in round 2. In 2015 and 2016 the ACL clubs enter after round 3.
- K League 2013 split: RSSSF shows a championship group of 7 and a relegation group of 7 after 26 rounds. This confirms the first agent's inference.
- Korean foreign rule 2005: 4 registered and 3 on the pitch, confirmed by Kyeonggi Ilbo (8 Mar 2006), which says registration fell "from four to three" for 2006.

## Club matches after a rename or move

RSSSF uses the old names in the season tables, so the matches can be checked against them:

- FC Seoul and Anyang LG Cheetahs: RSSSF 2004 says Anyang LG Cheetahs "moved to Seoul and changed name to FC Seoul". Agree.
- Sangju Sangmu and Gwangju Sangmu: RSSSF 2011 says Gwangju Sangmu Phoenix were "relocated and renamed Sangju Sangmu Phoenix". Agree. RSSSF 2020 says Sangju Sangmu move to a new franchise city for 2021, which is Gimcheon Sangmu. Agree.
- Busan IPark and Pusan I.cons: RSSSF 2002 "Pusan Icons changed to Busan Icons", 2005 "Busan I'Park (new name for Busan Icons)". Agree.
- Jeonnam Dragons and Chunnam Dragons: RSSSF uses "Chunnam Dragons (Gwangyang)" to 2013 and "Jeonnam Dragons" from 2014. Agree.
- Jeju United and Bucheon SK, Suwon FC and Suwon City, Korean Police and National Police Agency, Paju Frontier and Paju Citizen, Reilac Shiga and MIO Biwako Shiga: no second source read. These matches remain inferences.

## Spot check of the clubs file

| Season | Division | Clubs file | RSSSF | Result |
|---|---|---|---|---|
| 2010/11 | J1 League | 18 | 18 (jpn2011) | same clubs |
| 2012/13 | J2 League | 22 | 22 (jpn2013) | same clubs |
| 2019/20 | J3 League | 19 | 18 (jpn2020) | **differs**: the file lists FC Tokyo U-23, which RSSSF does not show in 2020. The rules row says 18 played. |
| 2009/10 | K League | 15 | 15 (skor2010) | same clubs |
| 2016/17 | K League 2 | 10 | 10 (skor2017) | same clubs |
| 2010/11 | A-League | 11 | 11 (aus2011) | same clubs |
| 2025/26 | Australian Championship | 16 | 16 (aus2025) | same clubs |

Max should decide whether FC Tokyo U-23 stays in the 2019/20 J3 list.

## Other notes

- aus-jap-kor-107 (NSL 2003/04 play-offs): the first value omits the two-legged elimination finals and the preliminary final. I marked it `agree`, with the full RSSSF format in the note.
- Japanese partner-nation players were already exempt from the foreign limit in December 2016 (https://www.jleague.jp/news/article/7968/). The first agent starts this exemption in 2017. I found no start year.
- The 2019/20 A-League `subs_used` agreement rests on a search result for a page that returned HTTP 403. Treat it as weak.
- worldfootball.net and soccerway were not used.
