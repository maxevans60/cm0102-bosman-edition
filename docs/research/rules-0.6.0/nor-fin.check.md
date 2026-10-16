# Second-source check: nor-fin (Norway and Finland)

Check of `nor-fin.tsv` (270 rows) against sources from other publishers than the first agent's Wikipedia pages. All pages were read on 2026-10-16. The verdicts per fact are in `nor-fin.check.tsv`. I did not change the first agent's files.

## Method

1. I checked every filled fact field of every row (`n/a` fields skipped). That gives 1,499 checks.
2. Main second source: the RSSSF season pages, read raw with curl. Norway: `rsssf.org/tablesn/noo02.html` to `noo09.html`, then `noo2010.html` to `noo2025.html`. Finland: `rsssf.org/tablesf/fin02.html` to `fin09.html`, then `fin2010.html` to `fin2025.html`. I took club counts, matches played, promotion and relegation marks, play-off sections, cup rounds and cup final line-ups from these pages.
3. Other second sources:
   - names: `snl.no/Eliteserien` (Store norske leksikon) and `lokalhistoriewiki.no/wiki/OBOS`
   - Norwegian super cup: `rsssf.org/tablesn/noosupcuphist.html`
   - Norwegian cup from 2025-26: Dagbladet (NTB), `dagbladet.no/sport/kjempedumt/83948867`
   - Finnish League Cup 2017 to 2026 and the 2026 Veikkausliiga format: veikkausliiga.com news and the `sarjajarjestelma` page
   - substitutes and bench: Transfermarkt match sheets. I read 288 sheets of the top two tiers of both nations, 2002 to 2026, and counted the named substitutes and the substitutions made per team. The highest count in a season sample is the value that I compare. A sample can only show a lower bound, so a lower count than the first value gives `no second source`, not `disagree`.
4. Nation rules (foreign players, work permits, registration, windows): web searches found no non-Wikipedia page. All 216 checks are `no second source`.
5. Rows for 2027 and 2028 carry the 2026 format forward. No source can confirm a future season, so they are `no second source`.

## Counts

| Verdict | Checks |
|---|---|
| agree | 887 |
| disagree | 19 |
| found | 35 |
| no second source | 558 |
| total | 1,499 |

Most `no second source` checks are the nation rules (216), the 2027 and 2028 rows (90), cup and super cup line-ups (about 115) and seasons before 2010 where Transfermarkt has no bench lists.

## Disagreements

| id | Season | Competition | Field | First value and URL | Second value and URL |
|---|---|---|---|---|---|
| nor-fin-2 | 2002 | Norwegian Premier Division | bench | 7, inferred from the cup final. https://en.wikipedia.org/wiki/2005_Norwegian_Football_Cup_final | The 2002 cup final shows 8 named substitutes for Vålerenga (3 used, 5 unused), so the basis of the inference is 8. https://www.rsssf.org/tablesn/noo02.html |
| nor-fin-4 | 2002 | Norwegian Cup | bench | 7 (final line-ups). https://en.wikipedia.org/wiki/2002_Norwegian_Football_Cup_final | 8 named substitutes for Vålerenga: Døhli, Holm, Viikmäe used, Bakke, Ohr, Heiaas, Belsvik, Krogstad unused. https://www.rsssf.org/tablesn/noo02.html |
| nor-fin-5 | 2002 | Norwegian Super Cup | format | not held. https://en.wikipedia.org/wiki/Mesterfinalen | A 2002 edition was played without official status: Rosenborg 2-0 Viking, 7 April 2002, Stavanger. https://www.rsssf.org/tablesn/noosupcuphist.html |
| nor-fin-7 | 2002 | Finnish Premier Division | down | 12th of the relegation stage (1). https://en.wikipedia.org/wiki/2002_Veikkausliiga | The bottom 2 of the 8-club promotion/relegation group go to Division One. VPS (Premier 11th) finished 7th and was the only Premier club relegated. The count of 1 agrees. The rule does not. https://www.rsssf.org/tablesf/fin02.html |
| nor-fin-30 | 2004 | Finnish League Cup | clubs | Veikkausliiga clubs. https://en.wikipedia.org/wiki/Finnish_League_Cup | All first-level clubs and the top 2 second-level clubs of the previous season. 16 clubs in 4 groups of 4. https://www.rsssf.org/tablesf/fin04.html |
| nor-fin-40 | 2005 | Finnish League Cup | clubs | Veikkausliiga clubs. https://en.wikipedia.org/wiki/Finnish_League_Cup | The same entry rule as 2004, 16 clubs in 4 groups of 4. https://www.rsssf.org/tablesf/fin05.html |
| nor-fin-72 | 2009 | Norwegian Premier Division | playoffs | 14th in a play-off with 1. divisjon 3rd to 5th, two-legged ties. https://en.wikipedia.org/wiki/2009_Tippeligaen | The semi-finals were single matches (Fredrikstad v Sarpsborg 08, Kongsvinger v Sogndal). Only the final had two legs. https://www.rsssf.org/tablesn/noo09.html |
| nor-fin-82 | 2010 | Norwegian Premier Division | playoffs | The same text as 2009. https://en.wikipedia.org/wiki/2010_Tippeligaen | The semi-finals were single matches (Fredrikstad v Løv-Ham, Hønefoss v Ranheim). Only the final had two legs. https://www.rsssf.org/tablesn/noo2010.html |
| nor-fin-169 | 2017-18 | Finnish Cup | format | Ran from July to September of the next year. https://en.wikipedia.org/wiki/2017–18_Finnish_Cup | The final was on 12 May 2018, after groups A to E, an intermediate round on 17 March and semi-finals on 1 and 2 April. https://www.rsssf.org/tablesf/fin2018.html |
| nor-fin-177 | 2019 | Finnish Premier Division | split | The Challenger winner joins 4th to 6th in a European play-off. https://en.wikipedia.org/wiki/2019_Veikkausliiga | The top 2 of the lower group (7th HIFK, 8th Lahti) joined 3rd, 5th and 6th. 4th Ilves were cup winners. Each group played 5 more matches, 27 in all. https://www.rsssf.org/tablesf/fin2019.html |
| nor-fin-188 | 2020 | Finnish First Division | playoffs | 3rd v 4th, winner v 2nd, then v Veikkausliiga 11th. https://en.wikipedia.org/wiki/2020_Ykkönen | 2nd KTP went straight to the two-legged play-off against TPS. There was no internal play-off. https://www.rsssf.org/tablesf/fin2020.html |
| nor-fin-188 | 2020 | Finnish First Division | split | Promotion series (top 6) and relegation series (bottom 6). https://en.wikipedia.org/wiki/2020_Ykkönen | The final table is after 22 matches. No series was played. This answers an open question in `nor-fin.md`. https://www.rsssf.org/tablesf/fin2020.html |
| nor-fin-188 | 2020 | Finnish First Division | format | round robin plus split. https://en.wikipedia.org/wiki/2020_Ykkönen | 12 clubs, a 22-match double round robin. https://www.rsssf.org/tablesf/fin2020.html |
| nor-fin-210 | 2022 | Finnish League Cup | format | Winter knockout (detail unknown). https://en.wikipedia.org/wiki/Finnish_League_Cup | 3 groups of 4 (single round robin, 15 Jan to 23 Feb). The top 2 and the 2 best 3rd-placed clubs go to the quarter-finals. Then semi-finals and the final on 19 March 2022. https://www.veikkausliiga.com/uutiset/2021/12/03/liigacupin-2022-lohkot-ja-runkoaikataulu-selvilla |
| nor-fin-218 | 2023 | Finnish First Division | down | bottom 2 (2). https://en.wikipedia.org/wiki/2023_Ykkönen | The relegation line is above 10th (3 places). 11th KPV and 12th JJK went down. 10th KäPa stayed only because HIFK were dissolved. https://www.rsssf.org/tablesf/fin2023.html |
| nor-fin-220 | 2023 | Finnish League Cup | format | Winter knockout (detail unknown). https://en.wikipedia.org/wiki/Finnish_League_Cup | 2 groups of 6, then semi-finals and the final. The 2024 article says the 2024 format was the same as the previous season's. https://www.veikkausliiga.com/uutiset/2023/12/01/liigacupin-2024-lohkojako-selvilla |
| nor-fin-230 | 2024 | Finnish League Cup | format | Winter knockout (detail unknown). https://en.wikipedia.org/wiki/Finnish_League_Cup | 2 geographic groups of 6 (single round robin), the top 2 of each to the semi-finals, then the final. https://www.veikkausliiga.com/uutiset/2023/12/01/liigacupin-2024-lohkojako-selvilla |
| nor-fin-240 | 2025 | Finnish League Cup | format | Winter knockout (detail unknown). https://en.wikipedia.org/wiki/Finnish_League_Cup | 2 groups of 6, the top 2 of each to the semi-finals, then the final in March. https://www.veikkausliiga.com/uutiset/2024/11/26/liigacupin-2025-lohkojako-selvilla-kilpailun-muoto-aiemmilta-kausilta-tuttu |
| nor-fin-250 | 2026 | Finnish League Cup | format | Winter knockout (detail unknown). https://en.wikipedia.org/wiki/Finnish_League_Cup | 2 geographic groups of 6, the top 2 of each to the semi-finals, then the final. https://www.veikkausliiga.com/uutiset/2025/12/16/liigacupin-2026-lohkovaiheen-alustava-ohjelma-valmis-tasan-kuukausi-alkuun |

## Found values

These fields were `unknown` or an `unknown (inference ...)` in the first file.

| id | Season | Competition | Field | Value | URL |
|---|---|---|---|---|---|
| nor-fin-39 | 2005 | Finnish Cup | format | Knockout. First, second and third level clubs enter in round 4. Final 29 Oct. | https://www.rsssf.org/tablesf/fin05.html |
| nor-fin-59 | 2007 | Finnish Cup | format | Single-match knockout: round 5, round 6, last 16, quarter-finals, semi-finals, final 11 Nov. | https://www.rsssf.org/tablesf/fin07.html |
| nor-fin-178 | 2019 | Finnish First Division | format | 10 clubs, 27 matches each (triple round robin). | https://www.rsssf.org/tablesf/fin2019.html |
| nor-fin-182 | 2020 | Norwegian Premier Division | subs_used | 5 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3329954 |
| nor-fin-182 | 2020 | Norwegian Premier Division | bench | 9 from August. Matches to early July show 7. | https://www.transfermarkt.com/spielbericht/index/spielbericht/3329833 |
| nor-fin-183 | 2020 | Norwegian First Division | subs_used | 5 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3330095 |
| nor-fin-183 | 2020 | Norwegian First Division | bench | 9 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3330095 |
| nor-fin-187 | 2020 | Finnish Premier Division | subs_used | 5 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3323241 |
| nor-fin-187 | 2020 | Finnish Premier Division | bench | 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3323185 |
| nor-fin-188 | 2020 | Finnish First Division | subs_used | 5 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3321730 |
| nor-fin-188 | 2020 | Finnish First Division | bench | 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3321730 |
| nor-fin-197 | 2021 | Finnish Premier Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3511635 |
| nor-fin-198 | 2021 | Finnish First Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3512669 |
| nor-fin-207 | 2022 | Finnish Premier Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3747710 |
| nor-fin-208 | 2022 | Finnish First Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/3788525 |
| nor-fin-217 | 2023 | Finnish Premier Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4014197 |
| nor-fin-218 | 2023 | Finnish First Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4045607 |
| nor-fin-227 | 2024 | Finnish Premier Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4273383 |
| nor-fin-228 | 2024 | Finnish First Division | subs_used / bench | 5 / 7 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4321457 |
| nor-fin-237 | 2025 | Finnish Premier Division | subs_used / bench | 5 / 9. The 19 April 2025 match shows 7, May to August show 8 or 9. | https://www.transfermarkt.com/spielbericht/index/spielbericht/4577445 |
| nor-fin-238 | 2025 | Finnish First Division | subs_used / bench | 5 / 9 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4577506 |
| nor-fin-247 | 2026 | Finnish Premier Division | subs_used / bench | 5 / 9 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4797025 |
| nor-fin-248 | 2026 | Finnish First Division | subs_used / bench | 5 / 9 | https://www.transfermarkt.com/spielbericht/index/spielbericht/4855988 |

The TSV has one line per field, with the date and count of every sampled match. The Transfermarkt values are the highest counts in a sample of 2 to 10 matches. They are evidence for the rule, not a statement of it.

## Notes on agreed facts

- Transfermarkt supports the first agent's inferred values: 3 substitutions in both nations to 2019, 7 on the bench where bench lists exist (Norway 2010 to 2019, Finland 2013 to 2019), and 5 and 9 in Norway from 2021.
- 2009 and 2010, Norwegian First Division play-off: the text "one place" agrees. The detail is single-match semi-finals and a two-legged final.
- 2010 Norwegian First Division: 16 clubs started. Lyn withdrew after 11 matches, so the final table has 15 clubs.
- 2014 Ykkönen: 1 promotion place, but KTP and Ilves also went up after MyPa and Honka were refused licences.
- 2019 Norwegian super cup: RSSSF gives the state of the pitch before Norway v Sweden as the reason for the cancellation. The first file says extreme weather.
- 2020 Veikkausliiga: the split was cancelled on 29 October 2020. The first file says so in `notes`, not in `split`.
- Soccerway lists a Norwegian "Super Cup" for 2025-26. I could not open it, so the 2020 to 2026 super cup rows stay `no second source`.

## Clubs file spot-check

I checked one season per division of `nor-fin-clubs.tsv` against the RSSSF final table of that year. All agree, club for club:

| Season | Division | RSSSF page | Result |
|---|---|---|---|
| 2001/02 | Ykkönen South and North zones (9 + 9) | https://www.rsssf.org/tablesf/fin02.html | agree |
| 2002/03 | Veikkausliiga (14) | https://www.rsssf.org/tablesf/fin03.html | agree |
| 2008/09 | Tippeligaen (16) | https://www.rsssf.org/tablesn/noo09.html | agree |
| 2012/13 | Tippeligaen (16) | https://www.rsssf.org/tablesn/noo2013.html | agree |
| 2019/20 | Ykkönen (12) | https://www.rsssf.org/tablesf/fin2020.html | agree |
| 2021/22 | Eliteserien (16) | https://www.rsssf.org/tablesn/noo2022.html | agree |
| 2024/25 | Ykkösliiga (10) | https://www.rsssf.org/tablesf/fin2025.html | agree |
