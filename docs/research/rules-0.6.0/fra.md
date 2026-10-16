# Rules table: France (slice 5, part `fra`)

Files:

- `fra.tsv`: 243 rows, 9 rows per season from 2001/02 to 2027/28 (nation rules and 8 competitions).
- `fra-clubs.tsv`: 2,790 club rows for the club moves.
- This file: method, findings, inferences, unknowns and missing clubs.

All pages were read on 2026-10-16.

## Competitions and original files

| comp_file | comp (row label) | Tier | Real names by season |
|---|---|---|---|
| fra_first | Ligue 1 | 1 | Division 1 (2001/02), Ligue 1 (2002/03 on) |
| fra_second | Ligue 2 | 2 | Division 2 (2001/02), Ligue 2 (2002/03 on) |
| fra_third | Championnat National | 3 | Championnat National (2001/02 to 2025/26), Ligue 3 (2026/27 on) |
| fra_cfa | CFA | 4 | CFA (to 2016/17), National 2 (2017/18 to 2025/26), National 1 (2026/27 on) |
| fra_lower | CFA 2 | 5 | CFA 2 (to 2016/17), National 3 (2017/18 to 2025/26), National 2 (2026/27 on) |
| fra_cup | Coupe de France | cup | Coupe de France |
| fra_lge_cup | Coupe de la Ligue | cup | Coupe de la Ligue (last season 2019/20) |
| fra_super | Trophée des Champions | cup | Trophée des Champions |

The tier-5 mapping of `fra_lower` is an inference. The function map says that `fra_lower` selects candidate clubs and builds fixtures for two leagues. The 3.9.68 data puts 138 clubs in "French Lower Division". No real league has that size. Slice 18 or 20 must decide whether `fra_lower` becomes the real 5th tier.

The real competitions of tiers 1 to 5 all have an original file, so no row has `comp_file` = `new`. The 2026 renames (Ligue 3, National 1, National 2) are name changes of existing tiers.

## Method

1. I read the raw wikitext of the English Wikipedia season pages (Ligue 1, Ligue 2, Championnat National from 2007/08, CFA and National 2) and the French Wikipedia season pages (Coupe de France, Coupe de la Ligue, Trophée des champions). I read RSSSF season pages `fran02` to `fran06` for the National and CFA tables of 2001/02 to 2005/06. I read the LFP site, press pages and Wikipedia main pages for substitutes, bench and foreign player rules.
2. A script took the club lists from the league tables (the first `sports table` or `sports results` block of each page) and the play-off and relegation zones. I checked each zone count against the page text.
3. Club matching: a script matched each real club against the French clubs of the season database. It scores the share of distinctive name tokens found, accepts 0.8 or more, and uses a hand list of regular expressions for 90 hard names (renames, mergers, short forms). I checked a random sample of 45 rows and every alias pick by eye. A club in the CFA or National 2 that is also a club of tiers 1 to 3 in the same season is a reserve team, and the script then matches only database reserve teams (names ending in B, II or 2).

## Findings for the build

- **Ligue 1 size:** 18 in 2001/02, 20 from 2002/03 to 2022/23, 18 from 2023/24. 2022/23 relegates 4 and promotes 2. The original's 2001 branch (18 in the first season, then 20) matches the real 2001/02 to 2002/03 change only.
- **Every season database holds 18 clubs in the French First Division**, also for seasons in which the real Ligue 1 has 20. The club moves for 2002/03 to 2022/23 must move 2 clubs up from the database Ligue 2, and the effect runs down the tiers. For this reason `fra-clubs.tsv` lists tiers 1 to 3 for every season with a database, also where a tier's count matches.
- **Relegation play-off:** returns in 2016/17 (Ligue 1 18th v Ligue 2 3rd). From 2017/18 Ligue 2 has a play-off of 3rd, 4th and 5th, and the winner meets the Ligue 1 18th. From 2023/24 the Ligue 1 16th plays. No play-offs in 2019/20 (season stopped) or 2022/23.
- **Ligue 2:** 20 clubs to 2023/24, 18 from 2024/25. 2001/02 promotes 4. 2022/23 and 2023/24 relegate 4. The Ligue 2 18th (16th from 2024/25) plays the National 3rd from 2016/17, except in 2019/20, 2022/23 and 2023/24.
- **National:** 20 clubs to 2012/13 (21 in 2010/11), 18 from 2013/14 (17 in 2017/18 and 2024/25). 2022/23 and 2023/24 relegate 6. It becomes the professional Ligue 3 in 2026/27, with 18 clubs.
- **CFA and National 2:** 4 groups of 18 to 2012/13, 4 of 16 from 2013/14 to 2022/23, 4 of 14 in 2023/24, 3 of 16 from 2024/25. 2020/21 was declared void.
- **CFA 2 and National 3 promotion:** 2 per group (16) in 2001/02. 8 group winners and the 4 best runners-up (12) from 2002/03 to 2010/11 and from 2013/14 to 2016/17. Group winners only (8) in 2011/12 and 2012/13. Group winners only from 2017/18 (12 groups to 2022/23, then 11, 10 and 8). Reserve teams of Ligue 2 or National clubs cannot go up, and the next eligible club takes the place. 2020/21: no promotion (season stopped).
- **Coupe de la Ligue:** last held in 2019/20. Rows from 2020/21 say `not held`. From 2016/17 a drawn tie goes direct to penalties, except the final.
- **Coupe de France:** no extra time before the final in 2020/21 and 2021/22. A 4th substitution in extra time from 2018/19. The finals name 5 substitutes to 2006/07, 7 from 2007/08, 9 from 2019/20 and 12 in 2025/26. The 2023/24 final was at Stade Pierre-Mauroy.
- **Trophée des Champions:** venues for all 26 editions from 2001 to 2026 are in the `format` field. The 2020, 2023, 2024 and 2025 editions were played in the January of the season.
- **Substitutes:** 5 in Ligue 1 and Ligue 2 from 2020/21. Bench in Ligue 1: 9 from 2020/21 (20 on the sheet). Bench in Ligue 2 in 2020/21: 7 (18 on the sheet).
- **Non-EU players:** 5 per professional team from 2001 (up from 3), 5 in the professional squad (January 2003), 4 per Ligue 1 club in 2017/18, 4 in Ligue 1 and 2 in Ligue 2 in 2019/20, 4 per club in 2014/15 to 2016/17, 4 in 2024/25. In 2026/27: 4 in Ligue 1 and 2 in Ligue 2, under contract and on the match sheet. ACP nationals do not count (Cotonou, then Samoa from 2023). British players count as EU only on contracts signed before 1 January 2022.

## Inferences (marked in the rows)

- Ligue 1 bench of 5 (2001/02 to 2005/06) and 7 (2006/07 to 2018/19): taken from the Coupe de la Ligue finals of the same season. The LFP runs both competitions.
- Ligue 1 and Ligue 2 use 3 substitutes before 2020/21: the sources give 5 "since 2020/21" only.
- Ligue 2 bench of 5 before 2020/21: the source says 16 on the sheet "before", with no start year.
- Ligue 2 substitutes of 5 after 2020/21: carried from 2020/21.
- Coupe de France and Coupe de la Ligue substitutes of 3 where the final page states no limit.
- 2026/27 relegation play-offs: read from the 2026/27 Ligue 2 table zones.
- 2027/28: no pages yet. Every value is carried from 2026/27 under Q248.

## Unknowns

- Non-EU limits for 2003/04 to 2013/14, 2018/19, 2020/21 to 2023/24 and 2025/26.
- Squad registration rules for all seasons.
- Transfer window dates, except 2024/25 (10 June to 30 August, 1 January to 3 February) and 2025/26 (16 June to 1 September, 1 January to 2 February).
- Relegation counts of the CFA and National 2 from 2009/10 for most seasons, and of the CFA 2 and National 3 for most seasons.
- Substitutes and bench for tiers 3 to 5 and the Trophée des Champions.
- Coupe de la Ligue entrants for 2001/02 to 2007/08.
- Ligue 2 bench from 2021/22.

## Club moves

`fra-clubs.tsv` lists every real club of Ligue 1, Ligue 2 and the National for each season with a database from 2002/03. It also lists the CFA and National 2 for each season with a database from 2001/02. 2001/02 tiers 1 to 3 are not listed: the 3.9.68 data holds the real 2001/02 Division 1 (18), Division 2 (20) and National (20). Tier 5 is not listed (see the mapping note above).

`db_division` "A Lower Division" (division 357) is a shared catch-all division of many nations in the later databases. A club there plays in no French competition.

Sources of the club lists: English Wikipedia season pages, and RSSSF for the National of 2002/03, 2003/04 and 2005/06 and the CFA of 2001/02, 2002/03, 2003/04 and 2005/06. 2009/10 CFA: 73 clubs. The English table gave 72; the reconcile added La Vitréenne FC from the French page.

## Missing clubs (for Max)

Most missing clubs are reserve teams of professional clubs. The older databases hold no reserve teams.

- 2001/02 CFA: 20 missing, all reserve teams.
- 2002/03 CFA: 24 missing, 23 reserve teams and Racing Club de France.
- 2003/04 CFA: 22 missing, 21 reserve teams and Choisy-le-Roi.
- 2005/06 CFA: 28 missing, 19 reserve teams and Compiègne, Lesquin, Feignies, Le Pontet, Cassis-Carnoux, Racing CF, Genêts d'Anglet, Villemomble and Sainte-Geneviève.
- 2007/08 CFA: 26 missing, 22 reserve teams and ASM Belfort, RC Épernay Champagne, SO Châtellerault and TVEC Les Sables d'Olonne.
- 2008/09 CFA: 22 missing, 18 reserve teams and RCO Agde, US Colomiers, SO Châtellerault and UJA Alfortville.
- 2009/10 CFA: 20 missing, 15 reserve teams and UJA Alfortville, JA Drancy, AS Marck, RCO Agde and US Colomiers.
- 2010/11 CFA: 21 missing, 15 reserve teams and Monts d'Or Azergues Foot, US Colomiers, ES Uzès Pont du Gard, RCO Agde, SJA Le Poiré-sur-Vie and Saint-Pryvé Saint-Hilaire.
- 2011/12 National: SJA Le Poiré-sur-Vie.
- 2011/12 CFA: 20 missing, 17 reserve teams and Monts d'Or Azergues Foot, RCO Agde and GS Consolat.
- 2012/13 CFA: 16 missing, all reserve teams.
- 2013/14 CFA: Olympique Lyonnais reserves.
- 2018/19 National 2: Girondins de Bordeaux reserves.

Rows with an empty `db_club_id` in `fra-clubs.tsv` give the full names.

## Main sources

- Ligue 1: https://en.wikipedia.org/wiki/Ligue_1 and the season pages.
- Ligue 2: https://en.wikipedia.org/wiki/Ligue_2 and the season pages.
- Ligue 3 and National: https://en.wikipedia.org/wiki/Ligue_3, the season pages and https://www.rsssf.org/tablesf/fran02.html to fran06.
- National 1 (former CFA and National 2): https://en.wikipedia.org/wiki/Championnat_National_1
- National 2 (former CFA 2 and National 3): https://en.wikipedia.org/wiki/Championnat_National_2
- Cups: https://en.wikipedia.org/wiki/Coupe_de_la_Ligue, https://en.wikipedia.org/wiki/Trophée_des_Champions, the French Wikipedia season pages and the English final pages.
- Substitutes: https://ligue1.com/fr/articles/l1_article_3236- , https://ligue1.com/fr/articles/l1_article_1388- , https://www.maxifoot.fr/info-330057_200731/football.php
- Foreign players: https://www.cahiersdufootball.net/article/fc-babylone-1332 , https://kohenavocats.com/joueur-extra-communautaire-quota-ligue-1-titre-sejour-homologation-refusee-recours-2026/

## Reconcile

Date: 2026-10-16. Third agent. The decisions are in `fra.reconcile.tsv`. Changed rows keep their ids and order. Each changed field has its new URL in `sources`, and each note added by this pass starts with "Reconcile:".

### Counts

| Decision | Rows |
|---|---|
| second | 12 of the 13 disagreements, plus the 2009/10 CFA club count |
| first | 1 (fra-159 clubs) |
| conflict | 0 |
| found accepted | 26 |
| found rejected | 1 (fra-132 clubs: 111 is the count after a withdrawal, 112 written) |
| lead request (CFA 2 promotion in every season, play-offs, club counts) | 27 |
| club-file correction | 1 (AS Vitré, 7 rows) |

### Method

- Third sources: the RSSSF season pages for the primary tables (`fran02` to `fran09`, `fran2014`, `fran2015`), the French Wikipedia tier-4 and tier-5 season pages (raw wikitext, every group table counted by script), dailymercato.com and topmercato.com for the 2024/25 windows, ligue1.com for 2025/26, and the Assemblée nationale answer and the Charte 2019/20 for the non-EU rules.
- The English Wikipedia value "8 (group winners)" for the CFA 2 was checked in every season from 2001/02 to 2016/17. It is right only for 2011/12 and 2012/13. All other seasons now say 12 (8 winners and the 4 best runners-up), and 2001/02 says 16 (2 per group). The National 3 rows from 2017/18 now give the number of group winners.
- Club file: the 2009/10 CFA had 73 clubs (group A of 19). La Vitréenne FC was missing and is now listed with database record 12556. A duplicate-match scan found "AS Vitré" matched to the La Vitréenne record in 2007/08, 2008/09 and 2013/14 to 2017/18. These rows now point to "Amicale Sportive de Vitré" (143, French CFA). No other duplicate matches remain.

### Points for Max

There is no conflict where sources stay split. These points are weaker or need a decision:

1. CFA 2 promotion for 2002/03, 2003/04 and 2007/08 to 2016/17 (rows fra-15, fra-24, fra-60, fra-69, fra-78, fra-87, fra-114, fra-123, fra-132, fra-141) and the group-winners-only value of 2011/12 and 2012/13 (fra-96, fra-105) rest on one publisher only: the French Wikipedia season pages. Its prose is the same boilerplate on each page, and in 2001/02 it disagrees with its own table. The coloured rows in the tables give 12 to 15 promoted clubs in most seasons, which fits 12 places plus reprieves. No RSSSF table and no FFF rule text was found for these seasons.
2. The French Wikipedia prose and the tables disagree for 2023/24 tier 5 (154 v 153), 2026/27 tier 4 (48 v 49) and 2026/27 tier 5 (112 v 111). The tables and Transfermarkt agree, so the rows use the table counts. The reason for the 2026/27 tier-5 group of 13 is not known. The 2026/27 season is in progress.
3. Club counts are season-start counts. Clubs that withdrew during the season are in the notes (fra-132, fra-141, fra-159).
4. fra-177 (2020/21 National 3) keeps the rule value of 12 places with a note that no club went up. fra-176 (2020/21 National 2) writes 0 and 0 for the same stopped season. Max decides which form the build uses.
5. CFA 2 relegation counts stay `unknown` in most seasons. The French Wikipedia pages state 3 per group (2001/02 to 2007/08, 2013/14 to 2015/16) and 4 per group (2008/09 to 2012/13). This pass did not write them, because the check did not list them.

## Loose ends

- fra-176 (National 2 2020/21): `up` 4 (group winners) and `down` 12 (14th to 16th of each group), the rule places from French Wikipedia. The note says that the FFF stopped the season on 23 April 2021 with no club up or down. Same form as fra-177.
- CFA 2 `down` (fra-6 to fra-141, 2001/02 to 2016/17): from the French Wikipedia season pages (prose and table colours). 2001/02 to 2007/08: last 3 of each group (24), less the 14th-placed clubs that went to the play-off in 2004/05 (1), 2005/06 (2) and 2006/07 (1). 2008/09 to 2012/13: last 4 of each group (32). 2013/14 to 2015/16: last 3 of each group (24). 2016/17: set by place and by region after the territorial reform (count unknown). No second publisher found.

## Source fill

Method: I ran the rules lint and read each page on 2026-10-16 with curl. I added sources only.

- fra-240 (2027/28) `up` = 8 (group winners): https://en.wikipedia.org/wiki/Championnat_National_2 says the winner of each group goes up and gives 8 groups of 14 from 2025/26. Note: the lead of that page mixes the names National 2 and National 3 for the fifth tier.

No source found: none. Disagreements: none.
