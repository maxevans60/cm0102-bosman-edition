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

## Pass 2

Date: 2026-10-17. Decision agent. Row ids and row order stay the same. Each note that this pass adds starts with "Pass 2". `tools/research/rules_lint.py` gives 0 problems for `fra.tsv`, the same as before.

### Counts

| Item | Count |
|---|---|
| Rows deleted from `fra.tsv` | 0 |
| Club rows deleted from `fra-clubs.tsv` | 0 |
| Conflicts and open points settled | 7 |
| Club rows added (tier 5, 23 seasons) | 3,136 |
| Club-match corrections (tier 4) | 7 |
| Reserve teams that the databases lack (tiers 4 and 5) | 547 rows (188 in tier 4, 359 in tier 5) |

### 1. Lowest tier (Q331, Q288)

The lowest tier of the French files in the original is tier 5. `FraFirst::FraFirst` (0x5bb160) builds both `FraCfa` (0x5b91c0) and `FraLower` (0x5bd600) at a new game. Every season database holds the division "French Lower Division" (division 15, 138 to 194 clubs) below the CFA. The research list for Max (decision 3.2, Q280) also named tiers 2 to 5 for France. So the CFA 2, National 3 and National 2 (from 2026/27) rows stay. No French competition lies below tier 5, and no French competition needs a nation that a game cannot select. No row is deleted. Inference: `FraLower` mixes National clubs with clubs of the lower division and its real role in the original is not proven. The build slices 18 and 20 decide how tier 5 plays.

### 2. Conflicts and open points

All settled by what happened in real life, read with Q272 to Q278.

1. fra-175 (National 2020/21) `down`: 1 becomes 4 (15th to 18th). The regulation at the season start relegates the 15th to 18th (https://fr.wikipedia.org/wiki/Championnat_de_France_de_football_de_troisième_division_2020-2021). Only SC Lyon went down, because National 2 was declared void. This is in the notes (Q278).
2. CFA 2 promotion 2002/03 to 2016/17 (Reconcile point 1): keep the rule places (12, or 8 in 2011/12 and 2012/13). The coloured rows of 12 to 15 clubs include reprieves, which go in notes by Q272. Source: the French Wikipedia season pages, for example https://fr.wikipedia.org/wiki/Championnat_de_France_de_football_de_cinquième_division_2009-2010. A second publisher is the task of the second-source agent.
3. Prose against table counts (Reconcile point 2): keep the table counts. The tables list every club that started the season.
4. Withdrawals during a season (Reconcile point 3): keep the season-start counts (Q278).
5. fra-176 and fra-177 (2020/21, stopped): keep the rule places (Q273). The notes say that no club went up or down.
6. CFA 2 `down` counts (Reconcile point 5): already written in the loose ends. No change.
7. Late editions (Q277): fra-169 and fra-170 (finals of July 2020) and fra-180, fra-207, fra-216 and fra-225 (Trophée des Champions played in January) now have a note. The game plays each in its normal slot.

Also filled: the tier-5 club count of fra-60 (130), fra-69 (130), fra-78 (128), fra-87 (130) and fra-96 (128), from the group tables of the French Wikipedia season pages.

### 3. Club moves (Q279, Q280, Q290)

`fra-clubs.tsv` now lists tier 5 (CFA 2, then National 3) for each of the 23 seasons with a database, 2001/02 to 2025/26. The real count differs from the database count of "French Lower Division" in every season. Source: the group tables of https://fr.wikipedia.org/wiki/Championnat_de_France_de_football_de_cinquième_division_YYYY-YYYY, read on 2026-10-17. A third team is written "(C)".

Method: a script matched each real club against the French clubs of the season database that tiers 1 to 4 do not use. It scores shared place-name tokens, with stems for adjectival forms (for example "dieppois" and "Dieppe"), a hand list of 14 aliases and a hand list of 43 rejected pairs. A reserve team matches only a database reserve team. Each database club takes at most one real club per season. I read every match below a full score and every full-score match with a different name by eye. Result: 2,159 of 3,136 rows have a database club (1,817 from French Lower Division, 183 from French CFA, 147 from A Lower Division, 8 with no division, 1 from French National).

Tier 4 corrections (the pool now includes every division, Q279): 2005/06 Racing CF to 9266 Racing Club de Paris. 2007/08 and 2008/09 SO Châtellerault to 2431. 2008/09 to 2011/12 RCO Agde to 274. No other database club is used twice in a season.

Weak point for Max: the Besançon clubs. The October 2018 database calls record 1541 "Racing Besançon", and later databases call it "Besançon Football". The 2018/19 and 2019/20 rows match "Besançon Football" to 1541 and leave the real "Racing Besançon" empty.

### 4. Reserve and B teams (Q289)

Each row below is a real reserve team that the tier needs and the season database lacks. The build adds them as new club records. Full names are in `fra-clubs.tsv` (rows with an empty `db_club_id`).

| Season | Count | Reserve teams [division] |
|---|---|---|
| 2001/02 | 47 | Lille OSC (B) [CFA], FC Metz (B) [CFA], RC Lens (B) [CFA], RC Strasbourg (B) [CFA], AS Nancy-Lorraine (B) [CFA], ES Troyes AC (B) [CFA], Olympique Marseille (B) [CFA], SC Bastia (B) [CFA], Montpellier HSC (B) [CFA], AS Saint-Etienne (B) [CFA], AS Monaco FC (B) [CFA], OGC Nice (B) [CFA], Olympique Lyonnais (B) [CFA], FC Nantes-Atlantique (B) [CFA], Toulouse FC (B) [CFA], AJ Auxerre (B) [CFA], Stade Rennais (B) [CFA], Paris Saint-Germain FC (B) [CFA], Le Havre AC (B) [CFA], EA Guingamp (B) [CFA], Amiens SC (B) [CFA 2], SM Caen (B) [CFA 2], Paris Saint-Germain (C) [CFA 2], ES Wasquehal (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], Stade de Reims (B) [CFA 2], US Créteil (B) [CFA 2], AS Red Star (B) [CFA 2], FC Sochaux-Montbéliard (B) [CFA 2], AJ Auxerroise (C) [CFA 2], Louhans-Cuiseaux FC (B) [CFA 2], Dijon FCO (B) [CFA 2], ASOA Valence (B) [CFA 2], Grenoble Foot (B) [CFA 2], FC Gueugnon (B) [CFA 2], Olympique d'Alès (B) [CFA 2], AS Cannes (B) [CFA 2], Nîmes Olympique (B) [CFA 2], Girondins de Bordeaux (B) [CFA 2], FC Lorient (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], Angers SCO (B) [CFA 2], La Roche VF (B) [CFA 2], Stade rennais FC (C) [CFA 2], Le Mans UC (B) [CFA 2], LB Châteauroux (B) [CFA 2], Stade lavallois (B) [CFA 2] |
| 2002/03 | 48 | Paris Saint-Germain FC (B) [CFA], AJ Auxerre (B) [CFA], RC Lens (B) [CFA], EA Guingamp (B) [CFA], Lille OSC (B) [CFA], Le Havre AC (B) [CFA], ES Troyes AC (B) [CFA], FC Sochaux-Montbéliard(B) [CFA], AS Nancy Lorraine (B) [CFA], RC Strasbourg (B) [CFA], FC Metz (B) [CFA], CS Sedan Ardennes (B) [CFA], Olympique Lyonnais (B) [CFA], Montpellier HSC (B) [CFA], AS Monaco (B) [CFA], SC Bastia (B) [CFA], Olympique Marseille (B) [CFA], AS Saint-Etienne (B) [CFA], OGC Nice (B) [CFA], FC Girondins Bordeaux (B) [CFA], Stade Rennes (B) [CFA], FC Nantes (B) [CFA], Toulouse FC (B) [CFA], Amiens SC (B) [CFA 2], ES Wasquehal (B) [CFA 2], AJ Auxerroise (C) [CFA 2], AS Beauvais (B) [CFA 2], Stade de Reims (B) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], Dijon FCO (B) [CFA 2], Louhans-Cuiseaux FC (B) [CFA 2], Racing Besançon (B) [CFA 2], FC Mulhouse (B) [CFA 2], Grenoble Foot (B) [CFA 2], ASOA Valence (B) [CFA 2], FC Gueugnon (B) [CFA 2], Clermont Foot (B) [CFA 2], AS Cannes (B) [CFA 2], Olympique d'Alès (B) [CFA 2], AS Angoulême (B) [CFA 2], La Roche VF (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], Angers SCO (B) [CFA 2], LB Châteauroux (B) [CFA 2], Le Mans UC (B) [CFA 2], FC Lorient (B) [CFA 2], SM Caen (B) [CFA 2], Stade lavallois (B) [CFA 2] |
| 2003/04 | 48 | Auxerre (2) [CFA], Paris Saint-Germain (2) [CFA], Lens (2) [CFA], Lille (2) [CFA], Olympique Lyonnais (2) [CFA], Metz (2) [CFA], Sochaux (2) [CFA], Saint-Etienne (2) [CFA], Strasbourg (2) [CFA], Nancy (2) [CFA], Troyes (2) [CFA], Monaco (2) [CFA], Montpellier (2) [CFA], Olmpique de Marseille (2) [CFA], Nice (2) [CFA], Bastia (2) [CFA], Toulouse (2) [CFA], Rennes (2) [CFA], Nantes (2) [CFA], Girondis de Bordeaux (2) [CFA], Guingamp (2) [CFA], Amiens SC (B) [CFA 2], Le Havre AC (B) [CFA 2], AS Beauvais (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], AJ Auxerroise (C) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], Stade de Reims (B) [CFA 2], Dijon FCO (B) [CFA 2], Racing Besançon (B) [CFA 2], ASOA Valence (B) [CFA 2], Clermont Foot (B) [CFA 2], FC Gueugnon (B) [CFA 2], Grenoble Foot (B) [CFA 2], Louhans-Cuiseaux FC (B) [CFA 2], Nîmes Olympique (B) [CFA 2], AS Cannes (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], LB Châteauroux (B) [CFA 2], FC Libourne-Saint-Seurin (B) [CFA 2], AS Angoulême (B) [CFA 2], Stade lavallois (B) [CFA 2], Le Mans UC (B) [CFA 2], SO Châtelleraudais (B) [CFA 2], Angers SCO (B) [CFA 2], La Roche VF (B) [CFA 2], SM Caen (B) [CFA 2], FC Lorient (B) [CFA 2] |
| 2005/06 | 50 | Auxerre 2 (reserves) [CFA], Lens 2 (reserves) [CFA], Lille 2 (reserves) [CFA], Metz 2 (reserves) [CFA], Strasbourg 2 (reserves) [CFA], Nancy 2 (reserves) [CFA], Lyon 2 (reserves) [CFA], Nice 2 (reserves) [CFA], Sochaux 2 (reserves) [CFA], Montpellier 2 (reserves) [CFA], Monaco 2 (reserves) [CFA], Toulouse 2 (reserves) [CFA], Saint-Etienne 2 (reserves) [CFA], Le Mans 2 (reserves) [CFA], Nantes 2 (reserves) [CFA], Bordeaux 2 (reserves) [CFA], Rennes 2 (reserves) [CFA], Guingamp 2 (reserves) [CFA], Paris-S.G. 2 (reserves) [CFA], Le Havre AC (B) [CFA 2], Entente Sannois Saint-Gratien (B) [CFA 2], Valenciennes FC (B) [CFA 2], Amiens SC (B) [CFA 2], FC Rouen (B) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], Stade de Reims (B) [CFA 2], AJ Auxerroise (C) [CFA 2], ES Troyes AC (B) [CFA 2], Dijon FCO (B) [CFA 2], Racing Besançon (B) [CFA 2], Nîmes Olympique (B) [CFA 2], FC Gueugnon (B) [CFA 2], Grenoble Foot (B) [CFA 2], AC Ajaccien (B) [CFA 2], Clermont Foot (B) [CFA 2], SC Bastia (B) [CFA 2], GFCO Ajaccio (B) [CFA 2], Olympique de Marseille (B) [CFA 2], FC Libourne-Saint-Seurin (B) [CFA 2], ÉSA Briviste (B) [CFA 2], LB Châteauroux (B) [CFA 2], Stade lavallois (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], Angers SCO (B) [CFA 2], SM Caen (B) [CFA 2], FC Lorient (B) [CFA 2], Stade brestois (B) [CFA 2], Vannes OC (B) [CFA 2], USON Mondeville (B) [CFA 2] |
| 2007/08 | 46 | Lille OSC (reserves) [CFA], SM Caen (reserves) [CFA], En Avant Guingamp (reserves) [CFA], RC Lens (reserves) [CFA], Stade Rennais FC (reserves) [CFA], Le Havre AC (reserves) [CFA], Valenciennes FC (reserves) [CFA], FC Sochaux-Montbéliard (reserves) [CFA], Olympique Lyonnais (reserves) [CFA], AJ Auxerre (reserves) [CFA], RC Strasbourg (reserves) [CFA], FC Metz (reserves) [CFA], AS Nancy (reserves) [CFA], AS Monaco FC (reserves) [CFA], AS Saint-Étienne (reserves) [CFA], Montpellier HSC (reserves) [CFA], OGC Nice (reserves) [CFA], Toulouse FC (reserves) [CFA], FC Girondins de Bordeaux (reserves) [CFA], Le Mans Union Club (reserves) [CFA], Paris Saint-Germain FC (reserves) [CFA], FC Nantes (reserves) [CFA], CS Sedan Ardennes (B) [CFA 2], Amiens SC (B) [CFA 2], Stade de Reims (B) [CFA 2], ES Troyes AC (B) [CFA 2], Racing Besançon (B) [CFA 2], Olympique de Marseille (B) [CFA 2], FC Gueugnon (B) [CFA 2], Clermont Foot (B) [CFA 2], Dijon FCO (B) [CFA 2], Grenoble Foot (B) [CFA 2], Nîmes Olympique (B) [CFA 2], FC Sète (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], FC Libourne-Saint-Seurin (B) [CFA 2], Angers SCO (B) [CFA 2], AJ Auxerroise (C) [CFA 2], LB Châteauroux (B) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], SO Châtelleraudais (B) [CFA 2], Stade brestois (B) [CFA 2], FC Lorient (B) [CFA 2], Stade lavallois (B) [CFA 2], SC Bastia (B) [CFA 2], Entente Sannois Saint-Gratien (B) [CFA 2] |
| 2008/09 | 53 | FC Sochaux-Montbéliard (reserves) [CFA], AJ Auxerre (reserves) [CFA], RC Lens (reserves) [CFA], RC Strasbourg (reserves) [CFA], Lille OSC (reserves) [CFA], AS Nancy (reserves) [CFA], FC Metz (reserves) [CFA], Olympique Lyonnais Reserves and Academy [CFA], Montpellier HSC (reserves) [CFA], AS Saint-Étienne (reserves) [CFA], AS Monaco FC (reserves) [CFA], FC Girondins de Bordeaux (reserves) [CFA], Le Mans Union Club (reserves) [CFA], Stade Rennais FC (reserves) [CFA], Paris Saint-Germain FC (reserves) [CFA], SM Caen (reserves) [CFA], Le Havre AC (reserves) [CFA], En Avant Guingamp (reserves) [CFA], US Créteil-Lusitanos (B) [CFA 2], Entente Sannois Saint-Gratien (B) [CFA 2], Amiens SC (B) [CFA 2], Pacy VEF (B) [CFA 2], Stade de Reims (B) [CFA 2], ES Troyes AC (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], Valenciennes FC (B) [CFA 2], Racing Besançon (B) [CFA 2], Vesoul HSC (B) [CFA 2], Grenoble Foot (B) [CFA 2], Clermont Foot (B) [CFA 2], ASF Andrézieux (B) [CFA 2], AC Ajaccien (B) [CFA 2], Dijon FCO (B) [CFA 2], FC Gueugnon (B) [CFA 2], FC Montceau Bourgogne (B) [CFA 2], Olympique de Marseille (B) [CFA 2], OGC Nice (B) [CFA 2], SC Bastia (B) [CFA 2], Nîmes Olympique (B) [CFA 2], Toulouse FC (B) [CFA 2], FC Sète (B) [CFA 2], Aviron bayonnais (B) [CFA 2], FC Libourne-Saint-Seurin (B) [CFA 2], Aurillac FCA (B) [CFA 2], Tours FC (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], LB Châteauroux (B) [CFA 2], AJ Auxerroise (C) [CFA 2], FC Nantes (B) [CFA 2], Stade brestois (B) [CFA 2], Stade lavallois (B) [CFA 2], FC Lorient (B) [CFA 2], Angers SCO (B) [CFA 2] |
| 2009/10 | 49 | AS Nancy (reserves) [CFA], Lille OSC (reserves) [CFA], RC Lens (reserves) [CFA], RC Strasbourg (reserves) [CFA], Olympique Lyonnais Reserves and Academy [CFA], FC Sochaux-Montbéliard (reserves) [CFA], Montpellier HSC (reserves) [CFA], Grenoble Foot (reserves) [CFA], Camp des Loges (reserves) [CFA], Le Mans Union Club (reserves) [CFA], FC Girondins de Bordeaux (reserves) [CFA], Le Havre AC (reserves) [CFA], AJ Auxerre (reserves) [CFA], Stade Rennais F.C. (reserves) [CFA], SM Caen (reserves) [CFA], Pacy VEF (B) [CFA 2], SC Bastia (B) [CFA 2], Amiens SC (B) [CFA 2], Entente Sannois Saint-Gratien (B) [CFA 2], Valenciennes FC (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], Stade de Reims (B) [CFA 2], AS Beauvais (B) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], FC Metz (B) [CFA 2], ES Troyes AC (B) [CFA 2], AJ Auxerroise (C) [CFA 2], Racing Besançon (B) [CFA 2], AS Saint-Étienne (B) [CFA 2], Clermont Foot (B) [CFA 2], Dijon FCO (B) [CFA 2], ASF Andrézieux (B) [CFA 2], AS Monaco (B) [CFA 2], ÉFC Fréjus Saint-Raphaël (B) [CFA 2], OGC Nice (B) [CFA 2], Nîmes Olympique (B) [CFA 2], Olympique de Marseille (B) [CFA 2], AC Ajaccien (B) [CFA 2], Toulouse FC (B) [CFA 2], Aviron bayonnais (B) [CFA 2], Angers SCO (B) [CFA 2], FC Nantes (B) [CFA 2], LB Châteauroux (B) [CFA 2], Tours FC (B) [CFA 2], Chamois Niortais FC (B) [CFA 2], FC Lorient (B) [CFA 2], EA Guingamp (B) [CFA 2], Stade brestois (B) [CFA 2], Stade lavallois (B) [CFA 2] |
| 2010/11 | 44 | FC Metz (reserves) [CFA], Havre Athletic Club (reserves) [CFA], RC Lens (reserves) [CFA], Lille OSC (reserves) [CFA], FC Sochaux-Montbéliard (reserves) [CFA], Paris Saint-Germain Youth Academy (reserves) [CFA], AJ Auxerre (reserves) [CFA], AS Nancy (reserves) [CFA], Olympique Lyonnais Reserves and Academy [CFA], AS Monaco FC (reserves) [CFA], AS Saint-Étienne (reserves) [CFA], Le Mans Union Club (reserves) [CFA], FC Lorient (reserves) [CFA], SM Caen (reserves) [CFA], Stade Rennais F.C. (reserves) [CFA], SC Bastia (B) [CFA 2], AS Beauvais (B) [CFA 2], Paris FC (B) [CFA 2], Pacy VEF (B) [CFA 2], Valenciennes FC (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], Stade de Reims (B) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], RC Strasbourg (B) [CFA 2], Dijon FCO (B) [CFA 2], ES Troyes AC (B) [CFA 2], AJ Auxerroise (C) [CFA 2], Grenoble Foot (B) [CFA 2], Clermont Foot (B) [CFA 2], FC Gueugnon (B) [CFA 2], Nîmes Olympique (B) [CFA 2], Montpellier HSC (B) [CFA 2], OGC Nice (B) [CFA 2], ÉFC Fréjus Saint-Raphaël (B) [CFA 2], Girondins de Bordeaux (B) [CFA 2], Toulouse FC (B) [CFA 2], Angers SCO (B) [CFA 2], FC Nantes (B) [CFA 2], Tours FC (B) [CFA 2], LB Châteauroux (B) [CFA 2], EA Guingamp (B) [CFA 2], FC Rouen (B) [CFA 2], Stade brestois (B) [CFA 2], Stade lavallois (B) [CFA 2] |
| 2011/12 | 49 | AJ Auxerre (reserves) [CFA], RC Lens (reserves) [CFA], AC Amiens (reserves) [CFA], Lille OSC (reserves) [CFA], Valenciennes FC (reserves) [CFA], Paris Saint-Germain Youth Academy (reserves) [CFA], Olympique Lyonnais Reserves and Academy [CFA], FC Metz (reserves) [CFA], AS Nancy (reserves) [CFA], FC Sochaux-Montbéliard (reserves) [CFA], AS Monaco FC (reserves) [CFA], AS Saint-Étienne (reserves) [CFA], FC Girondins de Bordeaux (reserves) [CFA], Havre Athletic Club (reserves) [CFA], Le Mans Union Club (reserves) [CFA], SM Caen (reserves) [CFA], FC Lorient (reserves) [CFA], US Boulogne CO (B) [CFA 2], Amiens SC (B) [CFA 2], FC Rouen (B) [CFA 2], AS Beauvais (B) [CFA 2], CS Sedan Ardennes (B) [CFA 2], ES Troyes AC (B) [CFA 2], Paris FC (B) [CFA 2], Stade de Reims (B) [CFA 2], US Créteil-Lusitanos (B) [CFA 2], AJ Auxerroise (C) [CFA 2], Dijon FCO (B) [CFA 2], Clermont Foot (B) [CFA 2], SC Bastia (B) [CFA 2], AC Ajaccien (B) [CFA 2], LB Châteauroux (B) [CFA 2], ÉFC Fréjus Saint-Raphaël (B) [CFA 2], Évian TG FC (B) [CFA 2], Olympique de Marseille (B) [CFA 2], OGC Nice (B) [CFA 2], Toulouse FC (B) [CFA 2], Nîmes Olympique (B) [CFA 2], Montpellier HSC (B) [CFA 2], Angers SCO (B) [CFA 2], FC Nantes (B) [CFA 2], Le Poiré-sur-Vie VF (B) [CFA 2], Tours FC (B) [CFA 2], Les Herbiers VF (B) [CFA 2], EA Guingamp (B) [CFA 2], Stade rennais FC (B) [CFA 2], Stade brestois (B) [CFA 2], Stade lavallois (B) [CFA 2], Vannes OC (B) [CFA 2] |
| 2012/13 | 42 | Lille OSC (reserves) [CFA], RC Lens (reserves) [CFA], FC Metz (reserves) [CFA], Valenciennes FC (reserves) [CFA], Paris Saint-Germain Youth Academy (reserves) [CFA], FC Sochaux-Montbéliard (reserves) [CFA], AJ Auxerre (reserves) [CFA], AS Nancy (reserves) [CFA], Olympique Lyonnais Reserves and Academy [CFA], AS Monaco FC (reserves) [CFA], AS Saint-Étienne (reserves) [CFA], FC Girondins de Bordeaux (reserves) [CFA], Le Mans Union Club (reserves) [CFA], SM Caen (reserves) [CFA], FC Lorient (reserves) [CFA], Havre Athletic Club (reserves) [CFA], CS Sedan Ardennes (B) [CFA 2], Amiens SC (B) [CFA 2], US Boulogne CO (B) [CFA 2], SC Bastia (B) [CFA 2], Paris FC (B) [CFA 2], FC Rouen (B) [CFA 2], AS Beauvais (B) [CFA 2], Dijon FCO (B) [CFA 2], SR Colmar (B) [CFA 2], ES Troyes AC (B) [CFA 2], Évian TG FC (B) [CFA 2], Clermont Foot (B) [CFA 2], OGC Nice (B) [CFA 2], Olympique de Marseille (B) [CFA 2], AC Ajaccien (B) [CFA 2], ÉFC Fréjus Saint-Raphaël (B) [CFA 2], Toulouse FC (B) [CFA 2], Montpellier HSC (B) [CFA 2], Nîmes Olympique (B) [CFA 2], FC Nantes (B) [CFA 2], Angers SCO (B) [CFA 2], LB Châteauroux (B) [CFA 2], Le Poiré-sur-Vie VF (B) [CFA 2], Stade rennais FC (B) [CFA 2], EA Guingamp (B) [CFA 2], Stade brestois (B) [CFA 2] |
| 2013/14 | 7 | Olympique Lyonnais Reserves and Academy [CFA], US Quevillaise (B) [CFA 2], Paris FC (B) [CFA 2], SR Colmar (B) [CFA 2], AS Moulins (B) [CFA 2], Le Poiré-sur-Vie VF (B) [CFA 2], EA Guingamp (B) [CFA 2] |
| 2014/15 | 5 | EA Guingamp (B) [CFA 2], Le Poiré-sur-Vie VF (B) [CFA 2], US Quevillaise (B) [CFA 2], Paris FC (B) [CFA 2], SR Colmar (B) [CFA 2] |
| 2015/16 | 1 | Chamois niortais (B) [CFA 2] |
| 2016/17 | 6 | US Quevilly-Rouen Métropole (B) [CFA 2], US Avranches (B) [CFA 2], SC Toulon (B) [CFA 2], Chamois niortais (B) [CFA 2], Pau FC (B) [CFA 2], Les Herbiers VF (B) [CFA 2] |
| 2017/18 | 11 | Bourg-en-Bresse 01 (B) [National 3], US Concarnoise (B) [National 3], US Orléans (B) [National 3], USL Dunkerque (B) [National 3], FC Chambly (B) [National 3], US Quevilly-Rouen Métropole (B) [National 3], US Avranches (B) [National 3], Chamois niortais (B) [National 3], Pau FC (B) [National 3], Les Herbiers VF (B) [National 3], FC Bastia-Borgo (B) [National 3] |
| 2018/19 | 14 | FC Girondins de Bordeaux (reserves) [National 2], Lyon Duchère AS (B) [National 3], US Orléans (B) [National 3], C' Chartres Football (B) [National 3], FC Chambly (B) [National 3], USL Dunkerque (B) [National 3], US Créteil (B) [National 3], US Avranches (B) [National 3], US Quevilly-Rouen Métropole (B) [National 3], Chamois niortais (B) [National 3], Pau FC (B) [National 3], Rodez AF (B) [National 3], Le Mans FC (B) [National 3], Les Herbiers VF (B) [National 3] |
| 2019/20 | 14 | Lyon Duchère AS (B) [National 3], Le Puy Foot (B) [National 3], US Orléans (B) [National 3], C' Chartres Football (B) [National 3], FC Chambly (B) [National 3], US Avranches (B) [National 3], US Quevilly-Rouen Métropole (B) [National 3], Girondins de Bordeaux (B) [National 3], Chamois niortais (B) [National 3], Pau FC (B) [National 3], Rodez AF (B) [National 3], AS Béziers (B) [National 3], US Créteil (B) [National 3], Les Herbiers VF (B) [National 3] |
| 2020/21 | 5 | Stade briochin (B) [National 3], US Avranches (B) [National 3], US Quevilly-Rouen Métropole (B) [National 3], Rodez AF (B) [National 3], US Créteil (B) [National 3] |
| 2021/22 | 7 | Stade briochin (B) [National 3], Bourges Foot 18 (B) [National 3], US Quevilly-Rouen Métropole (B) [National 3], US Avranches (B) [National 3], Rodez AF (B) [National 3], Nîmes Olympique (B) [National 3], US Créteil-Lusitanos (B) [National 3] |
| 2025/26 | 1 | FC Versailles 78 (B) [National 3] |

### Missing clubs that are not reserve teams (for Max)

Real clubs with no record in that season's database, after the club moves. Names are in `fra-clubs.tsv`.

| Season | Division | Clubs |
|---|---|---|
| 2001/02 | CFA 2 | 5 |
| 2002/03 | CFA | 1 |
| 2002/03 | CFA 2 | 16 |
| 2003/04 | CFA | 1 |
| 2003/04 | CFA 2 | 33 |
| 2005/06 | CFA | 8 |
| 2005/06 | CFA 2 | 36 |
| 2007/08 | CFA | 3 |
| 2007/08 | CFA 2 | 47 |
| 2008/09 | CFA | 2 |
| 2008/09 | CFA 2 | 39 |
| 2009/10 | CFA | 4 |
| 2009/10 | CFA 2 | 41 |
| 2010/11 | CFA | 5 |
| 2010/11 | CFA 2 | 37 |
| 2011/12 | CFA | 2 |
| 2011/12 | CFA 2 | 38 |
| 2011/12 | Championnat National | 1 |
| 2012/13 | CFA 2 | 20 |
| 2013/14 | CFA 2 | 4 |
| 2014/15 | CFA 2 | 18 |
| 2015/16 | CFA 2 | 2 |
| 2016/17 | CFA 2 | 13 |
| 2017/18 | National 3 | 49 |
| 2018/19 | National 3 | 55 |
| 2019/20 | National 3 | 54 |
| 2020/21 | National 3 | 34 |
| 2021/22 | National 3 | 35 |
| 2023/24 | National 3 | 27 |
| 2024/25 | National 3 | 10 |
| 2025/26 | National 3 | 5 |

## Second source, pass 2

Date: 2026-10-17. Second-source agent. File: `fra.check2.tsv` (265 lines). Row ids and row order stay the same. `tools/research/rules_lint.py` gives 0 problems for `fra.tsv`, the same as before.

### Scope

`fra.check.tsv` has 675 lines with `no second source`. I checked the 265 lines whose field in `fra.tsv` still holds a fact. I did not check these lines:

- 301 lines whose field is `unknown`.
- 109 lines whose value is an inference: every 2027/28 row (carried under Q248), the values marked "(inference)", the Ligue 1 bench before 2019/20 and in 2021/22 to 2023/24 and from 2025/26, and the Ligue 2 bench before 2019/20.

### Counts

| Verdict | Lines |
|---|---|
| agree | 115 |
| disagree, settled | 1 |
| disagree, still split (conflict) | 3 |
| no second source | 146 |

| Competition | agree | disagree | no second source |
|---|---|---|---|
| Ligue 1 | 8 | 0 | 1 |
| Ligue 2 | 3 | 1 | 1 |
| Championnat National and Ligue 3 | 18 | 0 | 0 |
| CFA (tier 4) | 33 | 0 | 28 |
| CFA 2 (tier 5) | 48 | 3 | 41 |
| Coupe de France | 0 | 0 | 41 |
| Coupe de la Ligue | 5 | 0 | 29 |
| nation rules | 0 | 0 | 5 |

### Second publishers

- Stat Football Club France (statfootballclubfrance.fr), one page per season for the National, the CFA and the CFA 2, with group tables and the clubs that went up and down. It confirms names, club counts and groups. It gives the real outcome and no rules. I marked `up` and `down` `agree` only where the outcome count equals the rule count. Where reprieves, administrative relegations or withdrawals change the count, the line stays `no second source` and the note gives the outcome. This page names no play-offs, not even the known CFA 2 relegation play-offs of 2002 to 2007, so every tier 4 and 5 `playoffs` line stays `no second source`.
- French Wikipedia for the National and the CFA of 2001/02 to 2005/06 and the CFA 2 of 2001/02 and 2004/05, where RSSSF is the first source.
- RSSSF for the CFA 2 `down` lines whose first source is Wikipedia (2001/02, 2004/05, 2005/06, 2006/07). RSSSF has no CFA 2 tables for 2002/03, 2003/04, 2007/08 and 2008/09.
- footballdatabase.eu for the venue of the Coupe de la Ligue finals of 2001/02 to 2004/05 and 2015/16 (Stade de France).
- ligue1.com (LFP) and the Maxifoot table keys for the 2026/27 places of Ligue 1, Ligue 2 and Ligue 3.
- abargenteuil.fr (December 2025) for five substitutes in Ligue 1 from 2021/22 to 2025/26 and 20 players on the Ligue 1 sheet in 2024/25. For these fields the first source is ligue1.com.

fff.fr and its league sites refuse automated reads (HTTP 403), so no FFF rule text was read. Transfermarkt cup pages no longer give match links to a plain read.

### Change to `fra.tsv`

1. fra-228 (Ligue 2 2026/27) `playoffs`: "16th v Ligue 3 3rd" becomes "16th v the winner of the Ligue 3 play-offs (3rd to 6th), two legs". The LFP page https://ligue1.com/fr/articles/l1_article_5089- says that the 3rd to 6th of Ligue 3 play off and that the winner meets the Ligue 2 16th home and away. Third publisher: https://www.getfootballnewsfrance.com/2026/french-fa-president-philippe-diallo-confirms-ligue-3-launch-from-2026/ quotes the FFF president on two clubs up direct and four clubs in the play-offs. Decision: `second`. The Maxifoot Ligue 3 table key (https://www.maxifoot.fr/resultat-national.htm) still says "Place 3: barrage montée", which agrees with the old value. I take it as a key not yet updated (inference).
2. fra-229 (Ligue 3 2026/27) `playoffs`: the inference "3rd v Ligue 2 16th" becomes "3rd to 6th play off; the winner plays the Ligue 2 16th over two legs", with the LFP page as source. This follows from change 1. The 2027/28 rows (fra-237, fra-238) say "as 2026/27", so they now carry the new format.

### Conflicts (for Max)

1. fra-33 (CFA 2 2004/05) `down` = 23. RSSSF (https://www.rsssf.org/tablesf/fran05.html) marks two "best 14th place" clubs (Belfort, Montélimar) and the worst 12th (Vénissieux) for the relegation play-off, so 22 go down direct. French Wikipedia (the first source) says that Belfort and Montélimar joined the play-off only after administrative relegations. No third publisher found. The row keeps 23.
2. fra-33 `playoffs`. The row says "the 13th of each group, with the worst 12th and best 14th places". French Wikipedia says the 13th play the DH clubs and names the other places as fill-ins after administrative relegations. RSSSF names two best 14th. The row keeps its value.
3. fra-51 (CFA 2 2006/07) `down` = 23. RSSSF (https://www.rsssf.org/tablesf/fran07.html) marks two 14th-placed clubs (Pontarlier, Locminé) and the worst 12th (La Valette) for the play-off, so 22 go down direct. French Wikipedia (https://fr.wikipedia.org/wiki/Championnat_de_France_de_football_de_cinqui%C3%A8me_division_2006-2007) calls La Valette the worst 12th by rule and the two 14th fill-ins after administrative relegations. It also says that 11 CFA 2 clubs meet the winners of a first round of the 22 DH runners-up. The row keeps 23.

Inference for Max: from 2004/05 to 2006/07 the play-off had 11 CFA 2 places each season (8 times 13th, the worst 12th and two 14th), and 2005/06 (fra-42) already has 22. If the 11 places are the rule, `down` is 22 for fra-33 and fra-51.

### Other findings

- CFA 2001/02 to 2005/06: French Wikipedia and RSSSF also show a final phase of the best reserve teams for the reserve title. It has no effect on promotion. The rows say `playoffs` = none. The game does not need it unless the build plays a reserve title.
- fra-5 (CFA 2001/02) `up` = 6: French Wikipedia colours 7 clubs promoted and states no count.
- abargenteuil.fr (12 March 2026, https://abargenteuil.fr/blog/extra-communautaire-psg/) gives four non-EU players per Ligue 1 club and the Samoa exemption as the rule in force. The 2025/26 nation rules row (fra-217) has `foreign` = unknown. This page can fill it, but abargenteuil.fr is the first source of fra-208, so Max or the build can decide.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after.

- fra-217 (2025/26 nation rules), `foreign`: unknown changed to "4 non-EU players per Ligue 1 club". Source: abargenteuil.fr, 12 March 2026 (https://abargenteuil.fr/blog/extra-communautaire-psg/, read on 2026-10-17). It gives the LFP limit of 4 as the rule in force. This is one publisher only, and the same publisher is the source of fra-208. The row notes say so.
- fra-33 and fra-51 (CFA 2 2004/05 and 2006/07), `down`: I found no FFF rule text for these seasons (two searches, no archive copy). The value 23 stays, and the open point for Max in the second-source section stays open. A note in each row says so.
