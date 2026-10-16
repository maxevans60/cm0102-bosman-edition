# Second-source check of the Scotland rules table

Check of part `sco` (`sco.tsv`, 216 rows). Check date 2026-10-16. The first agent's files are unchanged.

## Method

- I checked every filled fact field of every row. A field with `n/a` gets no check row. The result is 1,468 rows in `sco.check.tsv`.
- Leagues (`sco_prm`, `sco_first`, `sco_second`, `sco_third`), 2001/02 to 2025/26: RSSSF season pages (`rsssf.org/tabless/scotNN.html`), read raw with `curl` and parsed by script. I compared club count, matches played, the promoted, relegated and play-off marks, the split stages and the play-off ties. Every season agrees.
- League names: RSSSF headings, and a Stenhousemuir FC page of 24 July 2013 that names the SPL, the old divisions and the four SPFL names.
- Cups to 2019/20: tie counts per round on the RSSSF season pages give the entrant counts (League Cup 42, then 40 in groups plus 4 byes, Challenge Cup 30, 32, 54, 56, 58), the replay rounds and the extra time.
- Cups from 2020/21: Scottish FA news pages and the Scottish Cup rules and composition PDFs for 2018/19, 2019/20 and 2022/23 to 2026/27. For the League Cup and Challenge Cup I used SPFL news pages, the SPFL Trust site and a Livingston FC page. The first source of these facts is Wikipedia, so the publisher differs.
- Substitutes: an SPFL statement of September 2020 as published by Clyde FC, the Inverness Courier, the Press and Journal (14 and 26 January 2022), a Not The Old Firm page of 26 January 2022 and STV (5 September 2016).
- Nation rules: Scottish Legal News (June 2021) and the Scottish Daily Express (6 December 2021) for work permits. Premierleague.com for the start of the window system in 2002/03.
- `sco-clubs.tsv` has no rows. I spot-checked the 12, 10, 10 and 10 clubs in the Autumn 2013, April 2021 and December 2025 division files. They match the RSSSF tables.
- worldfootball.net blocks scripted reads (Cloudflare). I did not use it.

## Counts per verdict

| Verdict | Rows |
|---|---|
| agree | 942 |
| no second source | 508 |
| found | 11 |
| disagree | 7 |

Most `no second source` rows are rulebook facts with no other publisher: substitutes and bench before 2020/21, SFL bench "5 (inference)", foreign player and squad rules (SPFL rule C43), and every 2026/27 and 2027/28 league value that the first agent carried over.

## Disagreements

1. **sco-158 format (Scottish Cup 2020/21).** First: "R1 (League Two, Highland and Lowland clubs)", from https://en.wikipedia.org/wiki/2020–21_Scottish_Cup and https://livingstonfc.co.uk/scottish-cup-2020-21-format-confirmed/. Second: 15 Highland and 15 Lowland clubs entered in preliminary round two. Only 2 Highland and 2 Lowland clubs entered in round one, with the 10 League Two clubs. Source: https://www.scottishfa.co.uk/news/scottish-cup-202021-format-confirmed/. The "no replays" part agrees.
2. **sco-159 subs_used (League Cup 2020/21).** First: "3 (plus a fourth in extra time)", from the SPFL rulebook of 30 July 2020 (web.archive.org copy). Second: up to five substitutes in Betfred Cup matches, with one more in extra time, in an SPFL statement of September 2020. Source: https://www.clydefc.co.uk/news/2020/09/spfl-statement-use-of-five-subs/. The first agent's note already says that it does not know of later changes in 2020/21.
3. **sco-174 bench (Scottish Cup 2022/23).** First: "up to 7 (Wikipedia, rule date not stated)", from https://en.wikipedia.org/wiki/Scottish_Cup. Second: up to nine nominated substitutes (rule 13.1), from http://web.archive.org/web/20230213155210/https://www.scottishfa.co.uk/media/9973/scottish-cup-rules-2022-23.pdf
4. **sco-182 bench (2023/24).** First: up to 7, from the same Wikipedia page. Second: up to nine, from https://www.scottishfa.co.uk/media/10830/2023-24-scottish-gas-scottish-cup-rules.pdf
5. **sco-190 bench (2024/25).** First: up to 7, from the same Wikipedia page. Second: up to eight, from https://www.scottishfa.co.uk/media/12006/scottish-cup-rules-2024-25-fv.pdf
6. **sco-198 bench (2025/26).** First: up to 7, from the same Wikipedia page. Second: up to nine, from https://www.scottishfa.co.uk/media/13230/mens-scottish-cup-rules-2025-26.pdf
7. **sco-206 bench (2026/27).** First: up to 7, from the same Wikipedia page. Second: up to nine, from https://assets.ctfassets.net/qu0slcxvbwp6/4TZlz8yWaTibtBRSUk8Ai0/48f87db2a4679b5ad04a0039fe8683a5/Men-s_Scottish_Cup_Rules_2026-27.pdf

The 2027/28 row (sco-214) copies "up to 7". The rule of 2026/27 suggests nine. This is an inference.

## Found values (first value `unknown`)

| Row | Field | Value | Source |
|---|---|---|---|
| sco-58 | registration | SPL rule: three outfield players aged 21 or under in each matchday squad, in force on 3 January 2009. Start year unknown. | https://www.skysports.com/football/celtic-vs-dundee-u/11153 |
| sco-142 | bench | Scottish Cup 2018/19: up to 7 | http://web.archive.org/web/20220808112136/https://www.scottishfa.co.uk/media/4819/scottish-cup-rules-for-18-19-fv-docx.pdf |
| sco-150 | bench | Scottish Cup 2019/20: up to 7 | http://web.archive.org/web/20191014103133/https://www.scottishfa.co.uk/media/5364/scottish-cup-rules-for-2019-20.pdf |
| sco-158 | clubs | Scottish Cup 2020/21: 106 planned entrants (clubs could decline) | https://www.scottishfa.co.uk/news/scottish-cup-202021-format-confirmed/ |
| sco-166 | clubs | Scottish Cup 2021/22: 113 entrants | https://scottishfa.co.uk/news/202122-scottish-cup-fixture-dates-confirmed |
| sco-190 | clubs | Scottish Cup 2024/25: 130 entrants | https://www.scottishfa.co.uk/media/12024/2024-25-scottish-cup-format-composition.pdf |
| sco-174 | subs_used | Scottish Cup 2022/23: 5, plus a sixth in extra time | 2022/23 rules PDF (as in disagreement 3) |
| sco-182 | subs_used | 2023/24: 5 (three substitution opportunities), plus a sixth in extra time | 2023/24 rules PDF |
| sco-190 | subs_used | 2024/25: same as 2023/24 | 2024/25 rules PDF |
| sco-198 | subs_used | 2025/26: same as 2023/24 | 2025/26 rules PDF |
| sco-206 | subs_used | 2026/27: same as 2023/24 | 2026/27 rules PDF |

## Other notes for the lead

- The rulebooks show 3 substitutes from 7, plus a fourth in extra time, in the Scottish Cup of 2018/19 and 2019/20. This agrees with the first agent's subs value.
- RSSSF splits the Challenge Cup first round into North/East and South/West sections in 2006/07 and 2007/08. The first agent puts regional sections from 2011/12 on. RSSSF also shows the 2013/14 East v South preliminary tie over two legs.
- The 2021/22 Scottish Cup row (sco-166) says 5 substitutes from round 5. On 26 January 2022, before round 5, a news page says that the cup still allowed three. This does not contradict the row, and I marked it agree.
- The SPL under-21 rule is in force in January 2009 and June 2012. The first agent records it from 2010/11 only. Its start year is still unknown.
