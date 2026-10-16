# Second-source check of the Denmark and Sweden rules table

Check of part `den-swe` (`den-swe.tsv`, 324 rows, and a spot check of `den-swe-clubs.tsv`). Check date 2026-10-16. The first agent's files are unchanged.

## Method

- I checked every filled fact field of every row. A field with `n/a` gets no check row. The result is 1,780 rows in `den-swe.check.tsv`.
- Leagues and cups: RSSSF season pages, read raw with `curl` and parsed by script. Denmark: `rsssf.org/tablesd/denNN.html` (2001/02 to 2008/09) and `denYYYY.html` (2009/10 to 2025/26). Sweden: `rsssf.org/tablesz/zwedNN.html` and `zwedYYYY.html` (2002 to 2026). I compared club counts, matches played, the promoted, relegated and play-off marks, the play-off ties and the cup rounds. For cup entrant counts I added the round 1 clubs to the new clubs of each later round, from the tie counts.
- Gaps on RSSSF: no Danish 1st Division table for 2001/02 and 2002/03; no Danish 2nd Division table for 2001/02 to 2005/06, 2010/11 to 2019/20 and from 2024/25; no Swedish third tier for 2002 and 2011 to 2017. For some of these I derived the up count from the club lists of the next season (marked as inference in the note).
- Super Cups: the RSSSF lists of Danish and Swedish Super Cup finals.
- Swedish rules: fotbolldirekt.se (9 Dec 2007, 31 Mar 2017, 28 May 2020, 17 Sep 2024, 28 Nov 2025) and SVT (26 Mar 2020). The first agent used fotbollskanalen.se and Wikipedia for these facts. Where the first source of a field is fotbolldirekt.se, I did not use fotbolldirekt.se as the second source.
- Danish rules: Danish Immigration Service (nyidanmark.dk) for work permits. DBU circulars, Divisionsforeningen pages, the FIFA association page and Danish press gave no men's foreign-player rule or window dates.
- worldfootball.net and svenskfotboll.se return 403 to scripts. Soccerway loads its tables by script. I did not use them.
- Temporary files are in a folder made with `mktemp -d`. I deleted nothing.

## Counts per verdict

| Verdict | Rows |
|---|---|
| agree | 898 |
| no second source | 840 |
| found | 27 |
| disagree | 15 |

Most `no second source` rows are substitutes and bench sizes (no other publisher per league), the Danish and Swedish nation rules, the Danish 2nd Division seasons that RSSSF does not cover, and the seasons after the last real season (2026/27 and 2027/28, 2027 and 2028).

Some `agree` rows on cup formats agree on the rounds only. Their note says when the entrant count was not checked (Danish Cup 2001/02 to 2003/04 and from 2020/21, Svenska Cupen 2005 to 2008, 2010 and 2011).

## The two open questions

- **Superliga 2019/20 had 14 clubs.** RSSSF (`den2020.html`) shows 14 clubs, 26 regular rounds and lower groups of 4. Its note says the 4th of each lower group went down directly because the top level was cut to 12. Silkeborg and Esbjerg went down directly, and Hobro went down after the 11th-place tie with Lyngby. The 12-club format starts in 2020/21. The first agent's table is right.
- **The Svenska Cupen group stage started in the 2012/13 cup.** RSSSF `zwed2012.html` holds "Svenska Cupen 2012/13" with "Round 3 (Group Stage)", 8 groups of 4, and the final on 26 May 2013. The 2011 cup ended on 5 Nov 2011 with no group stage. The first agent's finding is right, and the change-points doc is wrong.

## Disagreements

1. **den-swe-34, 40, 46, 52 `up` (Danish 2nd Division 2006/07 to 2009/10).** First: 3. Second: 2 direct places plus 1 play-off winner. 2006/07 Skive and Lolland-Falster up, Hvidovre won the play-off. 2007/08 Thisted and FC Roskilde, Fremad Amager won the play-off. 2008/09 FC Fyn and FC Vestsjælland, Brabrand won the play-off. 2009/10 FC Hjørring and Brønshøj, Hobro won the play-off. First URLs: the season pages, for example https://en.wikipedia.org/wiki/2006%E2%80%9307_Danish_2nd_Divisions. Second URLs: https://www.rsssf.org/tablesd/den07.html, den08.html, den09.html, den2010.html.
2. **den-swe-52 `clubs` (Danish 2nd Division 2009/10).** First: 31. Second: 32 (2 x 16, every club played 30 matches). The clubs file lists 15 clubs in group 2 and leaves out Aarhus Fremad, 3rd in the Vest group. First URL: https://en.wikipedia.org/wiki/2009%E2%80%9310_Danish_2nd_Divisions. Second: https://www.rsssf.org/tablesd/den2010.html
3. **den-swe-118 `down` (Danish 2nd Division 2020/21).** First: 8. Second: 16, 8 per group: 7th to 10th went to the new 3rd Division and 11th to 14th went to the fifth level. First URL: https://en.wikipedia.org/wiki/2020%E2%80%9321_Danish_2nd_Divisions. Second: https://www.rsssf.org/tablesd/den2021.html
4. **den-swe-92 and 98 `playoffs` (Superliga 2016/17 and 2017/18).** First: the two 3rd-placed clubs of the qualifying groups play each other, and the two 4th-placed clubs play each other. Second: a cross round first (4th of one group v 3rd of the other). The winners then met for 11th place (loser to a promotion/relegation final) and the losers met for 13th place (loser relegated). Example 2016/17: Viborg v Horsens and Esbjerg v AGF, then Esbjerg v Horsens and Viborg v AGF. The 2018/19 row agrees with the first agent's description. First URL: https://en.wikipedia.org/wiki/Danish_Superliga. Second: https://www.rsssf.org/tablesd/den2017.html and den2018.html
5. **den-swe-104 `split` (Superliga 2018/19).** First: fixed groups (7th, 10th, 11th, 14th and 8th, 9th, 12th, 13th). Second: the 11th and 14th went to group A and the 12th and 13th to group B, then the 7th and the 8th chose a group. The groups were 9th, 10th, 11th, 14th and 7th, 8th, 12th, 13th. First URL: https://en.wikipedia.org/wiki/Danish_Superliga. Second: https://www.rsssf.org/tablesd/den2019.html
6. **den-swe-89 and 95 `format` (Danish Cup 2015/16 and 2016/17).** First: 108 clubs. Second: RSSSF lists 47 first-round ties (94 clubs), 9 new clubs in round 2 and 4 in round 3, which makes 107. RSSSF may miss a bye. The rounds agree. First URL: the Wikipedia season pages. Second: https://www.rsssf.org/tablesd/den2016.html and den2017.html
7. **den-swe-309, 315, 321 `bench` (Superettan 2026, 2027, 2028).** First: 7. Second: 9 from 2026. Superettan had 7 in 2025, and the federation meeting of November 2025 allowed nine substitutes and nine officials from the next season, the same as Allsvenskan. First URL: https://www.fotbollskanalen.se/allsvenskan/klubbat-nio-avbytare-tillats-i-allsvenskan/. Second: https://fotbolldirekt.se/superettan/superettan-andrar-reglerna-foljer-efter-allsvenskan/
8. **den-swe-274 `name` (Swedish third tier 2020).** First: Ettan. Second: RSSSF uses "Division 1" ("Div. 1 Norra", "Div. 1 Södra") for 2020 and "Ettan" from the 2021 page. This is weak evidence, because RSSSF can keep an old name. The first agent's notes already call the year of the name change an inference. First URL: https://en.wikipedia.org/wiki/Ettan_Fotboll. Second: https://www.rsssf.org/tablesz/zwed2020.html

## Found values (first value `unknown`)

- **Danish 2nd Division 2006/07 `down`:** 2. Only the reserve teams Silkeborg IF 2 and Vejle BK 2 went down, because their first teams were relegated. No other club went down, because the level grew to 2 x 16. https://www.rsssf.org/tablesd/den07.html
- **Danish 2nd Division 2009/10 `down`:** 0 direct places. The two 16th-placed clubs played off, and Otterup B&IK lost and went down. https://www.rsssf.org/tablesd/den2010.html
- **Danish 2nd Division 2010/11 `up`:** 1, Blokhus FC. This is an inference from the 1st Division lists of 2010/11 and 2011/12. https://www.rsssf.org/tablesd/den2012.html
- **Danish 2nd Division 2011/12 `up`:** 1, FC Fyn. This is an inference from the 1st Division lists of 2011/12 and 2012/13. https://www.rsssf.org/tablesd/den2013.html
- **Danish Cup club counts (inference from RSSSF tie counts):** 2006/07 102, 2007/08 104, 2008/09 108, 2009/10 107, 2012/13 108, 2013/14 106, 2014/15 108. URLs: den07.html, den08.html, den09.html, den2010.html, den2013.html, den2014.html, den2015.html. 2010/11 stays unknown, because RSSSF starts at round 2.
- **Denmark `work_permit`, 2024/25 to 2027/28:** a footballer from outside the EU/EEA needs a residence and work permit on "special individual qualifications". The club must belong to a specialised association, and the application needs an advance statement from that association on pay and terms. The start date of the scheme is unknown. The seasons after 2025/26 are an inference. https://www.nyidanmark.dk/en-GB/You-want-to-apply/Work/athlete-special-qualifications
- **Sweden `foreign`, 2002 to 2007:** at most 3 players from non-EU/EEA countries on the pitch at once, in the "previous seasons" before the decision of December 2007. Applying it to 2002 is an inference. https://fotbolldirekt.se/allsvenskan/fler-utlandska-spelare-i-allsvenskan/
- **Sweden `foreign`, 2008:** of the 16 players on the matchday list, 9 can come from outside Europe. Same URL.
- **Sweden `foreign`, 2017:** half of an Allsvenskan matchday squad must be homegrown (registered 3 years with a Swedish club between ages 12 and 21), so 9 of 18. https://fotbolldirekt.se/allsvenskan/pa-fd-bajen-tvingas-satta-en-utlandsk-stjarna-pa-laktaren-varje-match/
- **Sweden `windows`, 2020:** 8 January to 31 March (12 weeks) and 15 July to 11 August (4 weeks). https://www.svt.se/sport/fotboll/de-svenska-klubbarna-vadjar-till-fifa
- **Sweden `windows`, 2023 and 2024:** two windows of 8 weeks each from 2023. The 2024 summer window closed on 26 August. https://fotbolldirekt.se/?p=140888
- **Svenska Cupen 2027 row (2026/27 cup) `format`:** RSSSF already lists the group stage of 8 groups. The entrants and later rounds are not shown yet. https://www.rsssf.org/tablesz/zwed2026.html

Still unknown after the check: Danish foreign-player limits, squad registration and window dates for men; Swedish work permits and squad registration; Swedish foreign rules for 2009 to 2016 and 2018 to 2021.

## Spot check of `den-swe-clubs.tsv`

| Season | Division | Result |
|---|---|---|
| 2016/17 | Danish Superliga (14) | agrees with RSSSF |
| 2011/12 | Danish 1st Division (14) | agrees |
| 2007/08 | Danish 2nd Division group 1 (Øst, 16) | agrees |
| 2009/10 | Danish 2nd Division group 2 (Vest) | **disagrees**: 15 listed, RSSSF has 16. Aarhus Fremad is missing. |
| 2007/08 | Allsvenskan 2008 (16) | agrees |
| 2007/08 | Division 1 Norra 2008 (14) | agrees |
| 2019/20 | Ettan Norra 2020 (16) | agrees |

Aarhus Fremad must be added to the 2009/10 Danish 2nd Division list, and the 2009/10 count corrected to 32.
