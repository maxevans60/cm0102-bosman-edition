# Rules table part rus-tur: Russia and Turkey (slice 7)

Research for release 0.6.0. Date: 2026-10-16. Files:

- `rus-tur.tsv`: 324 rows, 12 competitions x 27 seasons (2001/02 to 2027/28).
- `rus-tur-clubs.tsv`: 1,595 club rows for the divisions whose real size differs from the season database.
- This file: method, decisions, missing clubs, matches to check and unknowns.

## Scope

| comp_file | Competition | Notes |
|---|---|---|
| rus_prm | Russian Premier League (tier 1) | 16 clubs in every season |
| rus_first | First Division, then FNL, then First League (tier 2) | size changes often |
| rus_cup | Russian Cup | |
| new | Russian Super Cup | from 2003 |
| new | Russian Premier League Cup | held once, 2003 |
| n/a | Russia nation rules | foreign limits, registration, windows, UEFA ban |
| tur_first | Süper Lig (tier 1) | 18, 21, 20, 19, 20, 19, 18 |
| tur_second | 2. Lig Category A, Lig A, TFF 1. Lig (tier 2) | the original's "Category A" |
| tur_second_b | 2. Lig Category B, Lig B, TFF 2. Lig (tier 3) | the original's "Category B" groups |
| tur_cup | Turkish Cup | |
| new | Turkish Super Cup | revived 2006, four clubs from 2025 |
| n/a | Turkey nation rules | foreign limits, registration, windows |

Out of scope: the Russian third tier (Second Division zones, later PFL and Second League) and the Turkish fourth tier (3. Lig). The original runs no league code for them, so they are not "same tier" additions. The databases still hold their clubs. Max can add them if he wants.

## Method

- Season pages of the English Wikipedia, read raw (`action=raw`) and as rendered HTML (REST API) on 2026-10-16. Turkish Wikipedia for the third tier in 2001/02 to 2004/05, 2006/07, 2008/09, 2017/18 to 2021/22 and 2024/25. Russian Wikipedia for the Russian foreign limits.
- Club counts come from the league table of each season page (rendered HTML), checked against the match count.
- Foreign limits come from news pages (sport-express.ru, TASS via news.mail.ru, aspor.com.tr, hurriyet.com.tr, medyascope.tv, goal.com Turkey).
- No WebFetch summary was used for a number. Every number was read from the page text with a script.
- `up` is direct places. A "2 + 1" in `up` means two direct places and one play-off place.
- For the Russian third tier `down` counts come from the table markers of the season page. The regulation count can differ where clubs withdrew or failed licensing. The notes say so.

## Decisions taken in the table

1. **Calendar years.** Russia played March to November until 2010. Per the brief, row 2001/02 holds 2002, up to row 2009/10 with 2010.
2. **The long season.** The 2011-12 season ran from 12 March 2011 to 22 May 2012 (44 rounds in the Premier League). Row 2010/11 holds its first stage (2011, 30 rounds, no promotion or relegation at the end of 2011). Row 2011/12 holds the second stage (spring 2012: groups of 8, two down, play-offs). The 2012/13 row is the first autumn-spring season, from July 2012. The FNL follows the same pattern (38 rounds, then a promotion group of 8 and a relegation group).
3. **Russian Super Cup.** Editions 2003 to 2011 were in March, before the calendar season, so edition N sits in row N-1/N. From 2012 the edition is in July, so edition N sits in row N/N+1. Row 2011/12 has no edition. Row 2001/02 has none (first held 2003).
4. **Russian Premier League Cup.** Held once, in 2003. By the calendar rule it sits in row 2002/03, although the final was in September 2003.
5. **Russian club lists.** For a calendar-year season the clubs of the row's real season are listed. Example: row 2010/11 lists the 2011-12 FNL, while the September 2010 database holds the clubs of 2010.
6. **UEFA ban.** UEFA suspended all Russian clubs on 28 February 2022. The nation rules rows say so from 2021/22. From 2022/23 the league and cup give no European places.
7. **Turkish Super Cup.** Edition N sits in row N/N+1 (curtain raiser). The 2011 edition was cancelled. The 2023 edition was played on 7 April 2024.

## Season databases used for club moves

Russia: Premier Division 16 in every database, real 16 in every season, so no list. First Division 18 in every database. Lists for 2002/03, 2003/04, 2005/06, 2007/08 to 2013/14 and 2015/16 to 2021/22 (real 17, 19, 20 or 22). No list for 2001/02, 2014/15 and 2022/23 to 2025/26 (real 18).

Turkey: Premier Division 18 in every database. Lists for 2020/21 to 2024/25. Category A 20 in every database. Lists for every database season with a real size of 17, 18 or 19 (2002/03 to 2023/24). No list for 2001/02, 2024/25 and 2025/26 (real 20). Category B is 5 groups of 10 in every database. No list for 2001/02 and 2007/08 (real 5 x 10). Lists for 2002/03, 2003/04, 2008/09 and 2009/10 (first-stage groups) and 2010/11 to 2025/26 (White and Red groups).

Seasons without a database (2004/05, 2006/07, 2026/27, 2027/28) have no list.

## Missing clubs (for Max)

No database record was found for these clubs. Each division below lacks this club after the moves.

| Season | Division | Club |
|---|---|---|
| 2005/06 | Russian First Division | Spartak Nizhny Novgorod |
| 2008/09 | Russian First Division | Krasnodar (founded 2008), Nizhny Novgorod |
| 2009/10 | Category A (TFF 1. Lig) | Hacettepe |
| 2010/11 | Category A (TFF 1. Lig) | TKİ Tavşanlı Linyitspor |
| 2023/24 | Category A (TFF 1. Lig) | Yeni Malatyaspor (October 2023 holds only its U19 side) |
| 2002/03 | Category B, Group C | Siirtspor |
| 2003/04 | Category B | Sidespor, Yeni Kırşehirspor, Osmaniyespor, Siirtspor |
| 2008/09 | Category B | Altınordu, Karsspor |
| 2009/10 | Category B | TKİ Tavşanlı Linyitspor, Pursaklarspor, Karsspor, İskenderun Demir Çelikspor |
| 2010/11 | Category B | Yeni Malatyaspor, Körfez İskenderun, Hacettepe, Pursaklarspor, 1461 Trabzon |
| 2011/12 | Category B | 1461 Trabzon |
| 2012/13 | Category B | Yeni Malatyaspor |
| 2015/16, 2016/17 | Category B | Tepecikspor |
| 2017/18 | Category B | Eyüpspor |
| 2018/19 | Category B | Başkent Akademi |
| 2021/22 | Category B | 1461 Trabzon |
| 2023/24 | Category B | Kastamonuspor 1966 |
| 2005/06 | Category B | all clubs: the reconcile found the 51 clubs on RSSSF tur06.html; they are not yet in rus-tur-clubs.tsv |

Some of these may exist under an old name. Possible matches that the script did not accept: Siirtspor = "Siirt JETPAspor", Konya Anadolu Selçukspor = "Anadolu Selçukluspor", Eyüpspor = "Eyüp Sultan Spor", İskenderun Demir Çelikspor or Körfez İskenderun = "Iskenderunspor" (probably a different club), TKİ Tavşanlı Linyitspor = "Tavsanli Linyitspor" (accepted in other seasons). The rule of Q244 (clubs with no division) was not applied here. Max decides.

## Matches to check

The script matched names by tokens and string likeness, then I checked the low scores by hand. Some matches rest on a club rename or on the stable club id across databases. The second agent should check these:

- Russia, by id or rename: SOYUZ-Gazprom Izhevsk = Gazovik-Gazprom Izhevsk (4321); Oryol = FC Orel (8326); Sibir Novosibirsk = Chkalovets-1936 Novosibirsk (2512, same id as Sibir later); Dynamo St. Petersburg 2010 = Petrotrest St. Petersburg (3132, the same id is "FC Dynamo Saint Petersburg" in September 2010); Yenisey Krasnoyarsk 2011 = Metallurg Krasnoyarsk (7299; September 2011 adds a new record 14063); Lisma-Mordovia = Biokhimik-Mordovia; Metallurg-Kuzbass = Metallurg-Zapsib; Ural Sverdlovsk Oblast = Uralmash or Ural Yekaterinburg; Salyut Belgorod = Salyut-Energia; Mashuk-KMV = Mashuk Pyatigorsk.
- Turkey: Van BB and Belediye Vanspor were matched to "Vanspor A.S." (2008/09 to 2010/11), probably a different club. Şekerspor 2012/13 = Çamlıdere Şekerspor; Çorum 2019/20 = Yeni Çorumspor; Fatih Karagümrük 2017/18 = Ottocool Karagümrük; Kastamonuspor 1966 2022/23 = GMG Kastamonuspor; Diyarbakır BB Diskispor = DISKIspor; Kayseri Erciyesspor = Erciyesspor; Gümüşhanespor = Gümüşhane Doğanspor; Kırşehirspor = Yeni Kırşehirspor; Afjet Afyonspor = Afyonspor.
- Sponsor names were accepted (for example Medical Park Antalyaspor, Vestel Manisaspor, Torku Konyaspor).

## Disagreements and weak facts

- **Turkish foreign limit 2004/05.** Medyascope gives "6" from 2004/05. The Goal Turkey gallery gives 2005/06 as the first season with 6.
- **Turkish foreign limit 2020/21.** A July 2020 TFF plan (aspor) set 14 registered and 8 on the pitch. Medyascope says the limit of 8 on the pitch began in 2021/22. Goal says the second half of 2020/21 allowed 16 in a 35-man list with no pitch limit.
- **Turkish 2026/27 age rule.** aspor (TFF statement) says at least 4 of 14 born on or after 1 January 2004 (the text is cut after "01."). The English 2026-27 Süper Lig page says U23, born 2003 or later.
- **TFF 1. Lig foreign limit 2022/23.** The Hürriyet text with "8 registered, 7 in the list, 5 on the pitch" follows the words "TFF 1."; that it applies to the 1. Lig is an inference.
- **Russian 7+4 from 2012.** Russian Wikipedia says the league returned to 7+4 "in 2012". Row 2012/13 as the first season is an inference.
- **TFF 2. Lig 2009/10.** The TFF 2. Lig page describes five groups of ten, but the season page has four groups (11, 11, 12, 11). The table uses the season page.
- **Süper Lig 2025/26 relegation.** The infobox lists three relegated clubs. The table template marks four places. The table uses three.
- **TFF 1. Lig 2010/11.** The infobox lists three relegated, the table marks two. The table uses three.

## Unknowns

- Bench size for Russia in every season. Bench size for Turkey before 2015/16 and after 2020/21.
- Substitutes for Russia in 2020/21 and from 2022/23. Substitutes for Turkey from 2020/21. The IFAB made five substitutes permanent as an option in 2022, so five is likely. No league text was found.
- FNL foreign limit between 2002 and 2019 (the 1999 rule of 5 is the last one found before 2020).
- Work permits for both nations.
- Exact transfer window dates (only the summer 2020 Russian window was found).
- Turkish foreign pitch limit for 2023/24 to 2025/26.
- Third-tier relegation counts in many Turkish seasons.
- Russian Cup entrants before 2022/23.
- All 2027/28 facts. No 2027-28 season page exists yet. Inference: the 2026/27 rules stay (Q248).

## Sources read

- English Wikipedia: Russian Premier League; Russian Premier League seasons 2002 to 2026-27; Russian First Division 2002 to 2010; Russian Football National League and Russian First League 2011-12 to 2026-27; Russian Cup (football) and its seasons 2001-02 to 2026-27; Russian Super Cup and its 2003, 2011 and 2012 editions; Russian Premier League Cup; Süper Lig and its seasons 2001-02 to 2026-27; TFF 1. Lig and its seasons; TFF 2. Lig and its seasons 2007-08 to 2016-17, 2022-23, 2023-24, 2025-26 and 2026-27; Turkish Cup and its seasons; Turkish Super Cup; Substitute (association football); Transfer window.
- Turkish Wikipedia: 2. Futbol Ligi B Kategorisi 2001-02 to 2004-05; 2006-07 Lig B; 2. Lig 2008-09, 2017-18 to 2021-22 and 2024-25.
- Russian Wikipedia: Чемпионат России по футболу (foreign limit section); Лимит на легионеров в футболе.
- News: sport-express.ru (23 March 2022, 24 May 2022, 4 December 2021, 2020 restart review); news.mail.ru (TASS, 2026 limit order); aspor.com.tr (8 July 2020, 22 July 2024, 21 May 2026); hurriyet.com.tr (18 June 2022); medyascope.tv (11 January 2024); goal.com Turkey foreign limit gallery; tgrthaber.com (2020 five substitutes).

## Reconcile

Reconcile date 2026-10-16. The full list is in `rus-tur.reconcile.tsv` (60 rows). I changed 65 fields in 46 rows of `rus-tur.tsv` and added the URL of each new value to `sources`. I changed 16 rows of `rus-tur-clubs.tsv`. Row order and ids are unchanged.

| Decision | Rows |
|---|---|
| second | 26 (plus 3 club-match rows) |
| first | 4 (plus 1 club-match row) |
| third (primary record gives a third value) | 1 |
| conflict | 2 |
| found value accepted | 20 (19 from the check file, 1 from the check notes: rus-tur-210) |
| found value rejected | 1 (rus-tur-242 down) |
| consequential change | 1 (rus-tur-242 up) |

How I decided:

- **Russian First Division `down` (rows 42, 43, 45, 47, 48, 51).** The brief counts places by the rules of the season. I read the FNL regulation of each season on 1fnl.ru. They give the last 5 places in 2015/16, 2016/17 and 2018/19, the last 6 places in 2020/21, the last 4 in 2021/22 and the last 3 in 2024/25. The first source counted the clubs that went down in effect. Those counts are now in `notes`.
- **Row 47 (2020/21).** The regulation (art. 3.3) relegates the last 6 of 22 clubs, 17th to 22nd. This agrees with neither source. RSSSF marks 18th to 22nd because Akron (17th) kept their place later. No source except the first names a relegation for match fixing.
- **Rows 38 and 40.** I accepted the found values with the withdrawals moved to `notes`: 4 places in 2011/12, 2 on the table in 2013/14. I did not find the 2013/14 regulation.
- **Row 1.** Lenta.ru confirms the 2002 golden match. The row now gives the tie rule and the match.
- **Row 46.** Transfermarkt shows 27 matches for every FNL club in 2019/20. There was no restart.
- **Rows 74 and 75.** Transfermarkt and RSSSF both have a round of 16 after the group stage. The group stage replaced the round of 32 only.
- **Row 139.** Sportbox and Sovetsky Sport give at most 5 foreigners on the pitch in 2005. I kept the first value and put the Sport-Express reading in `notes`.
- **Row 163.** The TFF history page says the league took the name Süper Lig in 2002/03. The 2001/02 name is now 1. Lig.
- **Row 173.** Hürriyet confirms the Spor Toto Cup of 2012. It had no effect on places. I added it to `split` with that remark.
- **Rows 221 to 225 (third tier, 2005/06 to 2009/10).** RSSSF numbers each classification table from 3, because places 1 and 2 of each first-stage group went to the promotion group. A "3rd-placed" classification club is the classification group winner. With this reading the first value is right for 2007/08 and 2008/09. For 2005/06 the play-off had promotion group 3rd to 8th and two clubs from each classification group. For 2009/10 it had promotion group 3rd to 6th. The TFF archive pages show the same play-off clubs.
- **Row 223 name.** The TFF 2. Lig archive lists "2. Lig B Kategorisi" champions up to 2006/07 and "TFF 2. Lig" champions from 2007/08.
- **Rows 238 and 239.** The TFF archive shows play-off round 1 with the 3rd to 6th of each group. Runners-up entered in round 3.
- **Row 242 (2026/27).** Sporbursa (12 July 2026) reports the TFF statute after Altınordu withdrew: 35 clubs (18 and 17), 2nd to 5th in the play-offs, one final per group, White 3 and Red 2 relegated. `up` is now "2 + 2" to match the two finals.
- **Rows 248 to 253.** The check cited the RSSSF pages of 2002/03 to 2007/08 for these rows. The rows are 2005/06 to 2010/11. I read the right RSSSF pages and the TFF cup archive for 2005/06, 2007/08 and 2010/11. All give two-legged quarter-finals.
- **Rows 298 and 323.** Habertürk gives 5+1+2 for 2001/02. Medyascope (21 May 2026) gives 1 January 2003 for the 2026/27 age rule.

### Conflicts

1. **rus-tur-196 `name` (TFF 1. Lig 2007/08).** The season page gives TFF 1. Lig, with the sponsor name Bank Asya 1. Lig from 16 January 2008. RSSSF heads the season "Bank Asya 2. Lig". The TFF archive files the season under its present 1. Lig name, so it does not settle the question. I found no other publisher. The first value stays. Max decides.
2. **rus-tur-301 `foreign` (Süper Lig 2004/05).** Medyascope and Habertürk give 6 foreigners from 2004/05. Goal and Türkiye Gazetesi give 6 from 2005/06, so 2004/05 stays 5+1+2. The first value stays, with both readings in `notes`. Max decides.

### Club matches

- **Gölbaşıspor, 2011/12 to 2013/14.** The TFF tables name the club Kızılcahamamspor A.Ş. in these seasons and Gölbaşıspor A.Ş. from 2014/15. The databases hold it as id 14267 (September 2011) and id 3847 (September 2012, Autumn 2013, and October 2014 as "Gölbasispor"). I matched the three rows.
- **Kocaeli Birlik Spor, 2010/11 to 2013/14.** The rows were matched to Kocaelispor (6061). The TFF tables name the club Körfez Spor or Körfez Futbol Kulübü. It took the name Kocaeli Birlik Spor in 2014/15 (id 5933). Kocaelispor is a different club and played in the same division in 2010/11 and 2011/12. I matched the four rows to 5933.
- **Körfez İskenderun Spor, 2010/11 to 2013/14.** The TFF name of these seasons is İskenderun Demir Çelikspor. I changed the club name only.
- **Konya Anadolu Selçukspor and Amed, 2012/13 to 2014/15.** The TFF tables name them Anadolu Selçukluspor and Diyarbakır Büyükşehir Belediyespor. The databases hold them as 4984 and 2883. I matched the five rows and took them off the missing-clubs table.
- **Dynamo St. Petersburg, 2009/10 (real 2010).** The real 2010 Dynamo and Petrotrest are different clubs. RSSSF shows that Petrotrest played in no professional zone in 2009. In September 2009 the database has id 3132 as its only St Petersburg record that fits, and September 2010 calls the same id "FC Dynamo Saint Petersburg". I kept the match. The September 2009 name "Petrotrest" is a database label.

### For Max (not changed)

- rus-tur-241 (TFF 2. Lig 2025/26) still says 2nd to 6th and one final. The TFF archive (pageID 1769) shows 8 clubs in play-off round 1 and two finals, and Transfermarkt marks 2nd to 5th. So `up` may be "2 + 2" and the format may be wrong. The check agreed with the row, so I did not change it.
- rus-tur-47 and rus-tur-51 `subs_used` are still `unknown`. The FNL regulations of 2020/21 (art. 10.24) and 2024/25 (art. 10.18) allow 5 substitutes.
- rus-tur-211 (TFF 1. Lig 2022/23 foreign limit). The TFF decision of 2 June 2021 (aspor) planned 5 registered and 4 on the pitch for 2022/23. The row has 8, 7 and 5 from a later decision text. I did not check it further.
- The 51 clubs of the 2005/06 Category B are on RSSSF tur06.html. They are not yet in `rus-tur-clubs.tsv`.

## Loose ends

- rus-tur-241 (TFF 2. Lig 2025/26): each group has its own play-off and final, so `up` is now "2 + 2". The White final (10 May 2026) went to Muğlaspor on penalties against Elazığspor (beIN Sports). The Red final went to Mardin 1969 against Muşspor (season page bracket). The bracket shows semi-finals of 2nd to 5th only, so the place of the 6th-placed clubs is unknown.
- FNL regulations, rows 47 (2020/21) and 51 (2024/25): I read both regulation PDFs. Neither gives a number of substitutes, so `subs_used` stays unknown.
- Turkish third tier 2005/06 (2. Lig B): the September 2005 database has 50 clubs in 5 groups of 10, the real league 51 clubs in groups of 11, 10, 10, 10, 10. That is a resize, so the 51 clubs are now in rus-tur-clubs.tsv (source RSSSF tur06). 7 have no database club: Alanyaspor, Yeni Kırşehirspor, Ünyespor, Giresunspor, Pazarspor, İskenderun Demir Çelik, Osmaniyespor. Matches to check: Anadolu Üsküdar (RSSSF "1980") matched to 42 "Anadolu Üsküdar 1908 Spor" (11908 "Üsküdar Anadolu" has no division); Gençlerbirliği ASAŞ matched to 148 "ASASspor"; Siirtspor to 10515 "Siirt JETPAspor". Adanaspor was expelled after 2 matches.

## Source fill

Method: I ran the rules lint and took each filled fact field with no `field=URL` entry. I read each page on 2026-10-16 with curl and checked the value against the page text or league table. For `split` = none and `format` = double round robin, the season page must show one table for the division with every club on 2 x (clubs - 1) matches, and no championship or relegation round. For a stopped season (2019/20, 2020/21), a table with fewer matches counts if the page says the season was stopped. I added sources only. I changed no value, row order or id.

- Sources added: 421 field entries in 268 rows.
- Russian Super Cup `clubs` = 2: https://en.wikipedia.org/wiki/Russian_Super_Cup ("Number of clubs 2", the champions and the cup winners). `name`: the match page of the real year (for example https://en.wikipedia.org/wiki/2019_Russian_Super_Cup).
- Second League Category A and B 2001/02 to 2004/05 `name`: the English pages give "Second League Category A" and "Category B" (https://en.wikipedia.org/wiki/TFF_2._Lig). The Turkish pages give "2. Futbol Ligi A Kategorisi" and "2. Futbol Ligi B Kategorisi". The row writes "2. Lig A Kategorisi".
- Süper Lig `playoffs` = none: the season pages name play-offs only for the TFF First League, for UEFA rounds and as the last tie-breaker.
- 2026/27 rows (rus-tur-26, 53, 188, 215): the season page shows one table, so it covers `split`.

No source found (added to `notes`):

- rus-tur-324 (2027/28 nation rules): `foreign` reads "Süper Lig: unknown". No page gives the 2027/28 quota.

Disagreements: none. Notes on the pages:

- 2005 Premier League (rus-tur-4): the page table gives Dynamo Moscow 20 matches and every other club 30. This looks like an error on the page.
- 2011/12 Süper Lig (rus-tur-173): the page calls the post-season groups "European play-offs". The row puts them in `split` and keeps `playoffs` = none.
- 2011–12 first stage (rus-tur-10, rus-tur-37): the pages put the relegation and promotion play-offs after the second stage. That agrees with `playoffs` = none for the first stage.
