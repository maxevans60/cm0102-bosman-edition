# int-continental: second-source check

Check of `int-continental.tsv` (297 rows) for part `int-continental` of slice 10. The verdicts are in `int-continental.check.tsv`. The first agent's files are not changed.

## Method

- Every page was read on 2026-10-16, with `curl` and a text dump, or with WebFetch and a request for the exact sentence.
- The main second source is RSSSF (rsssf.org). It has one page per edition with match dates, venues, group tables and extra-time marks. RSSSF marks a golden goal as `[asdet]` and a silver goal as `[a1het]` or `[a2het]`.
- RSSSF does not cover every fact, so other publishers were used for those: uefa.com releases (through the Malta FA site), concacaf.com, oceaniafootball.com, Al Jazeera, FAW, England Football, Dublin.ie, Irish Times, Buenos Aires Herald, Goal.com (Spanish), Bolavip, beIN Sports, Legit.ng, Daily Sabah, VOV and the-shot.com.
- No Wikipedia edition was used. Where the first row cites RSSSF for a fact (AFCON 2004 golden goal), the check says that it has no independent second source.
- One TSV line covers one filled field of one row: name, clubs, up, down, playoffs, format, subs_used, bench or notes. Each `not held` row gets one `format` line, which says whether an edition was played in that season.
- A row counts as `agree` when every fact that the second source states matches. Facts that the second source does not state are named in the note.
- An omitted stage counts as `disagree`. Example: the first row lists "semi-finals, final", but the second source shows a third place match.

## Counts

625 checked fields (201 of them on `not held` rows).

| Verdict | Count |
|---|---|
| agree | 572 |
| disagree | 17 |
| found | 9 |
| no second source | 27 |

All 201 `not held` rows agree except int-continental-289 (see below).

## Disagreements

| id | field | First row | Second source | First URL | Second URL |
|---|---|---|---|---|---|
| int-continental-5 | format | Gold Cup 2002: groups, QF, SF, final | Also a third place match, 2 Feb 2002 (Canada 2-1 South Korea) | https://en.wikipedia.org/wiki/2002_CONCACAF_Gold_Cup | https://www.rsssf.org/tables/02gc.html |
| int-continental-27 | format | Gold Cup 2003: groups, QF, SF, final | Also a third place match, 26 Jul 2003 (USA 3-2 Costa Rica) | https://en.wikipedia.org/wiki/2003_CONCACAF_Gold_Cup | https://www.rsssf.org/tables/03gc.html |
| int-continental-159 | format | Gold Cup 2015: groups, QF, SF, final | Also a third place match, 25 Jul 2015 (USA 1-1 Panama, 2-3 pen) | https://en.wikipedia.org/wiki/2015_CONCACAF_Gold_Cup | https://www.rsssf.org/tables/2015gc.html |
| int-continental-258 | format | Gold Cup 2025: "extra time and penalties" | No extra time in the quarter-finals and semi-finals (straight to penalties). Extra time in the final only (regulation Art. 12.12). RSSSF shows the drawn quarter-finals with penalties and no `[aet]`. | https://en.wikipedia.org/wiki/2025_CONCACAF_Gold_Cup | https://bolavip.com/en/amp/soccer/gold-cup-2025-overtime-rules-when-do-matches-go-to-extra-time and https://www.rsssf.org/tables/2025gc.html |
| int-continental-21 | format | OFC 2002: groups, SF, final | Also a third place match, 14 Jul 2002 (Tahiti 1-0 Vanuatu) | https://en.wikipedia.org/wiki/2002_OFC_Nations_Cup | https://www.rsssf.org/tables/02oc.html |
| int-continental-120 | format | OFC 2012: groups, SF, final | Also a third place match, 10 Jun 2012 (Solomon Islands 3-4 New Zealand) | https://en.wikipedia.org/wiki/2012_OFC_Nations_Cup | https://www.rsssf.org/tables/2012oc.html |
| int-continental-42 | format | Asian Cup 2004: groups, QF, SF, final | Also a third place match, 6 Aug 2004 (Iran 4-2 Bahrain) | https://en.wikipedia.org/wiki/2004_AFC_Asian_Cup | https://www.rsssf.org/tables/04asch.html |
| int-continental-75 | format | Asian Cup 2007: groups, QF, SF, final | Also a third place match, 28 Jul 2007 (South Korea 0-0 Japan, 6-5 pen) | https://en.wikipedia.org/wiki/2007_AFC_Asian_Cup | https://www.rsssf.org/tables/07asch.html |
| int-continental-108 | format | Asian Cup 2011: groups, QF, SF, final | Also a third place match, 28 Jan 2011 (Uzbekistan 2-3 South Korea) | https://en.wikipedia.org/wiki/2011_AFC_Asian_Cup | https://www.rsssf.org/tables/11asch.html |
| int-continental-152 | format | Asian Cup 2015: QF, SF, final; "final extra time" | Extra time also in the quarter-finals (South Korea 2-0 Uzbekistan aet, Iran 3-3 Iraq aet). There was also a third place match, 30 Jan 2015 (Iraq 2-3 UAE). | https://en.wikipedia.org/wiki/2015_AFC_Asian_Cup | https://www.rsssf.org/tables/2015asch.html |
| int-continental-101 | format | Euro 2012 qualifying: "August 2010 to November 2011" | First qualifying match 3 Sep 2010 | https://en.wikipedia.org/wiki/UEFA_Euro_2012_qualifying | https://www.rsssf.org/tables/2012e.html |
| int-continental-112 | format | As int-continental-101 | As int-continental-101 | https://en.wikipedia.org/wiki/UEFA_Euro_2012_qualifying | https://www.rsssf.org/tables/2012e.html |
| int-continental-277 | playoffs | Euro 2028 play-offs: single-leg semi-finals and final | Single-leg paths only if one or both reserved host slots are used. If no host slot is used: "Eight teams contest four home-and-away play-off ties". | https://en.wikipedia.org/wiki/UEFA_Euro_2028_qualifying | https://www.maltafootball.com/2025/05/22/uefa-euro-2028-qualification-system-approved/ |
| int-continental-288 | playoffs | As int-continental-277 | As int-continental-277 | https://en.wikipedia.org/wiki/UEFA_Euro_2028_qualifying | https://www.maltafootball.com/2025/05/22/uefa-euro-2028-qualification-system-approved/ |
| int-continental-289 | format | UEFA Nations League 2027/28: not held | The League C/D play-offs are set for 23 to 28 March 2028. Neither page describes their format. All League D teams are promoted, so the window may not be used. | https://en.wikipedia.org/wiki/UEFA_Nations_League | https://www.aljazeera.com/sports/2026/9/23/uefa-nations-league-2026-27-format-teams-players-schedule and https://faw.cymru/news/uefa-nations-league-2026-27-draw-all-you-need-to-know/ |
| int-continental-237 | down | CONCACAF Nations League 2022-23: "none (no relegation from 2022-23)" | Relegated: the third-placed League A teams (Suriname, Martinique, Curacao, Grenada) and the fourth-placed League B teams (Barbados, Montserrat, St. Vincent, Belize) | https://en.wikipedia.org/wiki/2023%E2%80%9324_CONCACAF_Nations_League | https://www.rsssf.org/tables/2023concanl.html |
| int-continental-281 | format | CONCACAF Nations League 2026-27: "Finals 22 to 30 March 2027" | League A Finals 25 to 28 March 2027 at SoFi Stadium, Los Angeles. 22 to 30 March is the window for the League B and C Championships. | https://en.wikipedia.org/wiki/2026%E2%80%9327_CONCACAF_Nations_League | https://www.concacaf.com/competitions/nations-league/news/fifth-edition-of-nations-league-to-begin-in-september |

## Found values

| id | field | Value | URL |
|---|---|---|---|
| int-continental-7 | format | AFCON 2002 had no golden goal. Senegal scored at 97 min in the semi-final against Nigeria, and play went on to 120+2 min. During that time Nigeria missed a penalty (110 min) and had two players sent off. | http://www.the-shot.com/africa/african-nations-cup-2002/matches/semi-finals/nigeria-senegal.html |
| int-continental-57 | notes | Euro 2008 qualifying group sizes: group 1 (A) 8 teams, groups 2 to 7 7 teams each | https://www.rsssf.org/tables/08e.html |
| int-continental-288 | notes | Euro 2028 qualifying groups end in November 2027. Play-offs are in March 2028. | https://dublin.ie/live/stories/euro-2028-all-you-need-to-know/ |
| int-continental-172 | notes | AFCON 2017 qualifying: qualifying round, then 13 groups. The group winners and the 2 best runners-up (group I excluded) qualify. Gabon are hosts. | https://www.rsssf.org/tables/2017a.html |
| int-continental-194 | notes | AFCON 2019 qualifying: preliminary round, then 12 groups. Group winners and runners-up qualify. | https://www.rsssf.org/tables/2019a.html |
| int-continental-249 | notes | AFCON 2023 qualifying: preliminary round, then 12 groups from 9 June 2022. Hosts Ivory Coast also played the groups. | https://www.rsssf.org/tables/2023a.html |
| int-continental-259 | up | CONCACAF Nations League 2024-25: League B group winners and League C group winners are promoted, plus the best League C runner-up (Cayman Islands) | https://www.rsssf.org/tables/2025concanl.html |
| int-continental-259 | down | CONCACAF Nations League 2024-25: League A 5th and 6th of each group (to a play-in for the Gold Cup preliminary round) and League B 4th are relegated | https://www.rsssf.org/tables/2025concanl.html |
| int-continental-281 | up | CONCACAF Nations League 2026-27: League B group winners are promoted. League C group winners and the best League C runner-up are promoted. | https://www.concacaf.com/competitions/nations-league/news/fifth-edition-of-nations-league-to-begin-in-september |

## Unknowns that are still unknown

- Golden goal at the 2004 Asian Cup. The only extra-time winner (Japan 4-3 Bahrain, 93 min) fits both rules. No report found says which rule applied.
- AFCON 2028 host and dates. Legit.ng (9 Oct 2026) says that no host is confirmed. It also says CAF will discuss whether to cancel the four-year cycle: https://www.legit.ng/sports/football/1735221-caf-reverse-afcon-4-year-cycle-decision-2028-edition-report/
- Host of the 2027 UEFA Nations League Finals. Al Jazeera says that it is not confirmed.
- Host of the 2027 Gold Cup and of the 2028 Copa America. Neither is set (concacaf.com, beIN Sports).
- Substitutes and bench size. Only Euro 2020 has a second source: five substitutes (Irish Times). The search did not find the subs and bench rules for other editions.

## Notes for the lead

- Nine finals rows omit a third place match: Gold Cup 2002, 2003 and 2015, OFC 2002 and 2012, and Asian Cup 2004, 2007, 2011 and 2015. Check the other rows that list stages before the build uses them.
- The Copa America inference (2001, 2004 and 2007 went straight to penalties) agrees with RSSSF score lines: drawn knockout ties show penalties and no extra time.
- Two `no second source` lines are on int-continental-296 (OFC 2028). Only the OFC's own release names the Cook Islands qualifier.
