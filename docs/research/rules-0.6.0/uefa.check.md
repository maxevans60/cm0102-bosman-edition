# UEFA rules table, second-source check

Second-source check of `uefa.tsv` (part `uefa`, slice 9). All pages were read on 2026-10-16. The first agent's files are unchanged. The verdicts are in `uefa.check.tsv`: 1,638 lines covering all 216 rows.

## Method

1. Each row is split into checkable facts. The access list row gives one line per nation (`format/<nation>`), plus `notes/ranking year` and `notes/seeding`. Competition rows give `name`, `clubs`, `format/rounds`, `format/tie rules`, `subs_used`, `bench`, `registration`, `windows` and `notes`. A `not held` row gives one `format` line.
2. The second source always comes from a different publisher than the first source of that fact:
   - Entry places per nation (first source kassiesa.net): the Wikipedia "Teams" tables of the CL, UEFA Cup/EL and Conference League season pages, parsed from raw wikitext. Club count and entry round per competition were compared for each nation. Title-holder entries, transfers between competitions and Intertoto winners were set apart. Ranks were compared with the Wikipedia association ranking tables. All 564 ranks agree.
   - Intertoto places and the Intertoto rows (first source Wikipedia): RSSSF `tablesu/uicYY.html`. Club totals were computed from the ties in each round.
   - Club counts and rounds (first source Wikipedia and kassiesa): the RSSSF season pages `ec/ecYYYYyy.html` up to 2024/25. Totals were computed as new entrants per round. For 2025/26 and 2026/27, the source is Transfermarkt fixture lists: distinct clubs per round, and distinct clubs over qualifying plus the main stage.
   - Path split 2009/10 to 2016/17 (first source Wikipedia): the kassiesa season format pages, with the RSSSF totals.
   - Subs and bench (first source the UEFA regulations): the "Match rules" lists on the Wikipedia final pages, and The42 (July 2018).
   - Tie rules: the Wikipedia "Golden goal" and "Away goals rule" articles, the final pages and the RSSSF results.
   - Seeding (first source kassiesa calc page): rbkweb.no (2004 reprint of the 2004/05 rule), a 2007 Wikipedia copy, and the Wikipedia season and qualifying pages.
   - Squad registration (first source the UEFA regulations): the European Commission homegrown-player study (2013), Smokvina 2025 (University of Rijeka), STV (2017) and beIN Sports (2025).
   - Super Cup (first source Wikipedia): RSSSF for venue, date and results. The UEFA Super Cup regulations 2026 on documents.uefa.com. Sportsnaut for 2026. Flashscore for 2027.
3. When a second source covers only part of a field, the verdict is `agree` and the note says which part is not covered.
4. Not checked: the claims about the original game ("The original game plays this format"). Also not checked: the path labels, which RSSSF calls "champions section" and "non-champions section".

## Counts

| Verdict | Lines |
|---|---|
| agree | 1,337 |
| disagree | 26 |
| no second source | 265 |
| found | 10 |

| Field group | agree | disagree | no second source | found |
|---|---|---|---|---|
| entry places per nation | 552 | 15 | 20 | 0 |
| other format lines | 254 | 7 | 24 | 0 |
| clubs | 127 | 1 | 5 | 1 |
| name | 95 | 0 | 39 | 0 |
| notes | 151 | 1 | 24 | 0 |
| subs_used | 48 | 2 | 42 | 4 |
| bench | 49 | 0 | 43 | 4 |
| registration | 56 | 0 | 12 | 1 |
| windows | 5 | 0 | 56 | 0 |

Most `no second source` lines fall into these groups:

- Squad windows: only UEFA itself publishes them.
- Seeding 2001/02 to 2003/04: the 50% figure.
- Super Cup subs and bench before 2024: only Wikipedia gives them.
- Final pages without a match-rules list: 2006 and 2008 to 2011.
- The 2002/03 rows: the agent's first source there was already Wikipedia.
- Everything for 2027/28 outside the CL access table.

## Disagreements

The first URL is the first agent's source. The second URL is the check source. In both URLs, `UCL` stands for `https://en.wikipedia.org/wiki/<season>_UEFA_Champions_League` and `UC`/`EL` for the matching UEFA Cup or Europa League page.

### Entry places (access list rows)

| id | Nation | Agent | Second source | First URL | Second URL |
|---|---|---|---|---|---|
| uefa-25 (2004/05) | England | UEFA Cup 2 (UC2@R1, LC@R1) | 3 in R1: 5th, league cup, cup runner-up Millwall | kassiesa.net/uefa/history/qual2004.html | en.wikipedia.org/wiki/2004–05_UEFA_Cup |
| uefa-25 (2004/05) | France | UEFA Cup 2 (UC1@R1, LC@R1) | 3 in R1: 4th, league cup, cup runner-up Châteauroux | kassiesa.net/uefa/history/qual2004.html | en.wikipedia.org/wiki/2004–05_UEFA_Cup |
| uefa-41 (2006/07) | France | UEFA Cup 2 (CW@R1, UC2@R1) | 3 in R1: cup winner, 4th, league cup winner Nancy | kassiesa.net/uefa/history/qual2006.html | en.wikipedia.org/wiki/2006–07_UEFA_Cup |
| uefa-49 (2007/08) | Sweden, Norway, Finland | no fair play place | 1 fair play place each, UEFA Cup Q1 | kassiesa.net/uefa/history/access2007.html | en.wikipedia.org/wiki/2007–08_UEFA_Cup |
| uefa-57 (2008/09) | England, Germany, Denmark | no fair play place | 1 fair play place each, UEFA Cup Q1 | kassiesa.net/uefa/history/access2008.html | en.wikipedia.org/wiki/2008–09_UEFA_Cup |
| uefa-89 (2012/13) | Spain | EL 2 (N5@Q4, N6@Q3) | 3: 5th@group stage, 6th@play-off, cup runner-up@Q3 | kassiesa.net/uefa/history/access2012.html | en.wikipedia.org/wiki/2012–13_UEFA_Europa_League |
| uefa-105 (2014/15) | Spain | EL 2 (N5@Q4, N6@Q3) | 3: 5th@group stage, 6th@play-off, 7th@Q3 | kassiesa.net/uefa/history/access2014.html | en.wikipedia.org/wiki/2014–15_UEFA_Europa_League |
| uefa-113 (2015/16) | Spain | "none" | CL 4 (3 group, 1 play-off), EL 2 (group, Q3) | kassiesa.net/uefa/history/access2015.html | UCL and EL 2015–16 |
| uefa-121 (2016/17) | Spain | "none" | CL 4 (3 group, 1 play-off), EL 2 (both group) | kassiesa.net/uefa/history/access2016.html | UCL and EL 2016–17 |
| uefa-129 (2017/18) | Spain | "none" | CL 4 (3 group, 1 play-off), EL 3 (2 group, Q3) | kassiesa.net/uefa/history/access2017.html | UCL and EL 2017–18 |
| uefa-209 (2027/28) | Russia | CL 1, EL 1, ECL 2 | no Russian club in the 2027/28 CL table (Russia suspended) | kassiesa.net/uefa/files/2027-28-uefa-access-list.pdf | en.wikipedia.org/wiki/2027–28_UEFA_Champions_League |

Spain is missing from three seasons. These are the largest errors. The agent probably failed to parse the kassiesa pages for those years. In 2012/13 and 2014/15, Spain's EL title holders finished 5th. The agent probably dropped that slot (inference).

### Other fields

| id | Field | Agent | Second source | First URL | Second URL |
|---|---|---|---|---|---|
| uefa-41 | notes/ranking year | ranking 2005 | page text: 2006 coefficients (2001–02 to 2005–06). Its own rank table equals the agent's ranks, so the page text is probably wrong | kassiesa.net/uefa/history/qual2006.html | en.wikipedia.org/wiki/2006–07_UEFA_Champions_League |
| uefa-45 (UEFA Cup 2006/07) | clubs | 131 in qualifying; 80 in R1 | 155 clubs in all (Q1 70, Q2 64 with 11 Intertoto winners, R1 80 with 16 CL losers, 8 CL thirds). 131 leaves out the 24 CL clubs | en.wikipedia.org/wiki/2006–07_UEFA_Cup | rsssf.org/ec/ec200607.html |
| uefa-5 (UEFA Cup 2001/02) | format/tie rules | final: golden goal extra time | final page: "30 minutes of extra time", no golden goal (the CL final page of the same season says golden goal). Weak | kassiesa.net/uefa/files/2001-uefa-uc-rules.pdf | en.wikipedia.org/wiki/2002_UEFA_Cup_final |
| uefa-146 (CL 2019/20) | subs_used | 3, plus a 4th in extra time | August 2020 final: max 5, plus a 6th in extra time | kassiesa.net/uefa/files/2019-20-uefa-cl-rules.pdf | en.wikipedia.org/wiki/2020_UEFA_Champions_League_final |
| uefa-149 (EL 2019/20) | subs_used | 3, plus a 4th in extra time | August 2020 final: max 5, plus a 6th in extra time | kassiesa.net/uefa/files/2019-20-uefa-el-rules.pdf | en.wikipedia.org/wiki/2020_UEFA_Europa_League_final |
| uefa-7, 15, 31, 47, 55, 63 (Intertoto 2001, 2002, 2004, 2006, 2007, 2008) | format/tie rules | away goals and penalties settle level ties | RSSSF shows ties decided "aet" in each of these years. The 2007 regulations: away goals, then 2x15 extra time (away goals count double), then penalties | en.wikipedia.org/wiki/<year>_UEFA_Intertoto_Cup | rsssf.org/tablesu/uicYY.html; web.archive.org copy of uefa.com/newsfiles/19069.pdf |

Notes on smaller differences marked `agree`:

- 2002/03 Spain and 2003/04 Italy: the agent marks a "TH" slot. The CL count is right.
- 2005/06 England: CL 5, with Liverpool entering Q1 as title holder.
- 2007/08 Italy: the agent has the 4th-place slot in Q3, but Milan used it in the group stage as title holder.
- 2001/02 CL Q1: RSSSF shows 10 ties (20 clubs). This supports the Wikipedia figure over kassiesa's 22 (point 3 of the agent's list for Max).
- EL 2024/25 Q3: RSSSF lacks one champions-path tie. Transfermarkt confirms the agent's 26 clubs.
- UEFA Cup 2005/06 and 2008/09: the totals agree, but "144 in qualifying" and "157 in qualifying" are the totals of all entrants, not of qualifying.
- 2017/18 windows: STV also names one "wildcard" player in qualifying, which the agent leaves out.

## Found values (fields the agent marked unknown)

| id | Field | Value | URL |
|---|---|---|---|
| uefa-53 | clubs (UEFA Cup 2007/08) | 157 in all: Q1 74, Q2 64, R1 80, plus 8 CL thirds | https://www.rsssf.org/ec/ec200708.html |
| uefa-55 | subs_used (Intertoto 2007) | 3 substitutions (Art. 14.02) | http://web.archive.org/web/20260925121003/https://www.uefa.com/newsfiles/19069.pdf |
| uefa-55 | bench | 7 substitutes (Art. 13.03) | same |
| uefa-55 | registration | List A max 25; places 20 to 25 only for locally trained players, max 3 association-trained (Art. 17.09) | same |
| uefa-192 | subs_used (Super Cup 2024) | max 5 (3 stoppages) | https://en.wikipedia.org/wiki/2024_UEFA_Super_Cup |
| uefa-192 | bench | 12 named substitutes | same |
| uefa-200 | subs_used (Super Cup 2025) | max 5 | https://en.wikipedia.org/wiki/2025_UEFA_Super_Cup |
| uefa-200 | bench | 12 | same |
| uefa-208 | subs_used (Super Cup 2026) | up to 5 (Art. 41.02) | https://documents.uefa.com/r/Regulations-of-the-UEFA-Super-Cup-2026/Article-41-Player-replacements-and-substitutions-Online |
| uefa-208 | bench | 23 players on the match sheet, so 12 substitutes (Art. 40.01) | https://documents.uefa.com/r/Regulations-of-the-UEFA-Super-Cup-2026/Article-40-Match-sheet-Online |

The 2024 and 2025 Super Cup values come from the match-details template on the Wikipedia match page. That page is the same publisher as the agent's first source, which reported no values there. The 2027 Super Cup date, 11 August 2027 (Flashscore), is a fact the agent left out. It was not marked unknown.

Values still unknown after the check:

- 2002/03 squad list and windows.
- Super Cup 2006 rules.
- Intertoto rules outside 2007.
- All 2027/28 rules, and the 2027/28 Europa League and Conference League places.
