# France: second-source check for 0.6.0 (part `fra`)

Date: 2026-10-16. This file checks `fra.tsv`, `fra-clubs.tsv` and `fra.md`. I did not change those files. Result: `fra.check.tsv`, 1,469 rows, one per filled fact field.

## Method

- Every filled fact field of every row got a verdict. I skipped fields with `n/a`. For a `not held` row, only the `format` field carries the check.
- A second source must come from another publisher than the first source of that fact. Most first sources are Wikipedia (English and French). For the National and the CFA of 2001/02 to 2005/06, the first source is RSSSF. There I used Transfermarkt, or I wrote `no second source`.
- League tables: I read the RSSSF France season pages `fran02.html` to `fran09.html` and `fran2010.html` to `fran2026.html` (Ligue 1, Ligue 2, National, cups, and the CFA and CFA 2 up to 2008/09). A script counted the clubs and the promotion and relegation marks. I read each mismatch by eye.
- Tiers 4 and 5 from 2008/09: I read the Transfermarkt group tables (`FR4A` to `FR4D` and `FR5A` to `FR5L`) for every season and counted the clubs. Transfermarkt shows the current name for old tier-5 seasons, so I did not use it for those names.
- Tiers 1 to 3 in 2026/27: Transfermarkt tables (18, 18 and 18 clubs, names "Ligue 1", "Ligue 2" and "Ligue 3").
- Cups: the RSSSF lists of Coupe de France finals and Trophée des Champions matches, and the cup sections of the RSSSF season pages. Two match reports for the extra-time rule (macommune.info for the 2016/17 Coupe de la Ligue, ogcnice.com for the 2021/22 Coupe de France).
- Rules: the LFP Statuts et Règlements 2020/21 and the Charte du football professionnel 2019/20 (both PDFs hosted by newstank.fr), LyonMag (1 August 2020), the English Wikipedia substitute article (a second publisher only where the first source is not Wikipedia), goal.com, an Assemblée nationale answer of 1 October 2001, Maxifoot (spring 2001), a FIFA COVID-19 regulatory paper of April 2020 (two annual registration periods), dailymercato.com and ligue1.com for transfer dates.
- Where RSSSF shows a licence refusal, an exclusion or a reprieve, the rule places count. A `down` value that matches the relegation zone is `agree`, and the note names the clubs that stayed up.
- A fact that the first agent marked `unknown` gets `found` when I found it. Some `found` values come from RSSSF where the first agent also cited RSSSF. The note says so.
- I did not use worldfootball.net (it returned HTTP 403) or fff.fr (HTTP 403).

## Counts

| Verdict | Rows |
|---|---|
| agree | 754 |
| no second source | 675 |
| disagree | 13 |
| found | 27 |

Most `no second source` rows are fields marked `unknown` that I also could not find. These are substitutes and bench for tiers 3 to 5 and the Trophée des Champions, squad registration, most CFA and CFA 2 relegation counts, and the non-EU limits of most seasons. The others are:

- all 2027/28 rows
- the places of 2026/27
- the National and CFA of 2001/02 to 2005/06, where RSSSF is the first source
- the CFA and CFA 2 promotion counts from 2009/10
- the Ligue 1 bench before 2019/20 and the Ligue 2 bench before 2020/21
- Ligue 1 and Ligue 2 substitutes from 2021/22
- cup substitutes and benches (the first source is Wikipedia, so the Wikipedia substitute article is not a second publisher)
- the Coupe de France extra-time rule from 2022/23, and the Coupe de la Ligue rule in 2017/18 and 2018/19 (see below)
- the non-EU limits of 2014/15 to 2016/17, 2024/25 and 2026/27

## Disagreements

| Row | Field | First value | First URL | Second value | Second URL |
|---|---|---|---|---|---|
| fra-6 (2001/02 CFA 2) | up | 8 (group winners) | https://en.wikipedia.org/wiki/Championnat_National_2 | RSSSF: 2 promoted in each of the 8 groups, 16 in all (reserve teams included). | https://www.rsssf.org/tablesf/fran02.html |
| fra-33 (2004/05 CFA 2) | up | 8 (group winners) | https://en.wikipedia.org/wiki/Championnat_National_2 | RSSSF: 8 group winners (best eligible) plus 4 best runners-up, plus Sainte-Geneviève, Andrézieux and Blois promoted additionally (15). | https://www.rsssf.org/tablesf/fran05.html |
| fra-42 (2005/06 CFA 2) | up | 8 (group winners) | https://en.wikipedia.org/wiki/Championnat_National_2 | RSSSF: 8 group winners plus the 4 best runners-up after a play-off (12). | https://www.rsssf.org/tablesf/fran06.html |
| fra-42 (2005/06 CFA 2) | playoffs | none | https://en.wikipedia.org/wiki/Championnat_National_2 | RSSSF: 13th places (and the best 14th places) play a relegation play-off, and runners-up play a promotion play-off. | https://www.rsssf.org/tablesf/fran06.html |
| fra-51 (2006/07 CFA 2) | up | 8 (group winners) | https://en.wikipedia.org/wiki/Championnat_National_2 | RSSSF: 8 group winners plus the 4 best runners-up after a play-off (12). | https://www.rsssf.org/tablesf/fran07.html |
| fra-51 (2006/07 CFA 2) | playoffs | none | https://en.wikipedia.org/wiki/Championnat_National_2 | RSSSF: 13th places (and the best 14th places) play a relegation play-off, and runners-up play a promotion play-off. | https://www.rsssf.org/tablesf/fran07.html |
| fra-150 (2017/18 CFA 2) | clubs | 168 (12 groups of 14) | https://en.wikipedia.org/wiki/Championnat_National_2 | Transfermarkt group tables total 171 (12 groups: 10 of 14, one of 15, one of 16). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2017 (and FR5B to FR5L) |
| fra-159 (2018/19 CFA 2) | clubs | 168 (12 groups of 14) | https://en.wikipedia.org/wiki/Championnat_National_2 | Transfermarkt group tables total 167 (12 groups: 11 of 14, one of 13). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2018 (and FR5B to FR5L) |
| fra-186 (2021/22 CFA 2) | clubs | 168 (12 groups of 14) | https://en.wikipedia.org/wiki/Championnat_National_2 | Transfermarkt group tables total 169 (12 groups: 11 of 14, one of 15). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2021 (and FR5B to FR5L) |
| fra-204 (2023/24 CFA 2) | clubs | 154 (11 groups of 14) | https://en.wikipedia.org/wiki/Championnat_National_2 | Transfermarkt group tables total 153 (11 groups: 10 of 14, one of 13). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2023 (and FR5B to FR5L) |
| fra-208 (2024/25 nation rules) | windows | 11 June to 2 September; 3 to 31 January | https://en.wikipedia.org/wiki/Transfer_window | LFP dates: summer 10 June to 30 August 2024, 23:00 (dailymercato.com); winter 1 January to 3 February 2025, 23:00 (https://www.goal.com/fr/news/quelles-sont-les-dates-du-mercato-d-hiver-en-france-cette-annee/blt780d56851e0dfd14). | https://www.dailymercato.com/news/ligue-1-quand-souvre-officiellement-le-mercato-estival-2024 |
| fra-230 (2026/27 CFA) | clubs | 48 (3 groups of 16) | https://en.wikipedia.org/wiki/Championnat_National_1 | Transfermarkt group tables total 49. | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR4A/saison_id/2026 (and FR4B, FR4C, FR4D) |
| fra-231 (2026/27 CFA 2) | clubs | 112 (8 groups of 14) | https://en.wikipedia.org/wiki/Championnat_National_2 | Transfermarkt group tables total 111 (8 groups: 7 of 14, one of 13). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2026 (and FR5B to FR5L) |

Notes on the disagreements:

- CFA 2 promotion (fra-6, fra-33, fra-42, fra-51): RSSSF shows more than the 8 group winners going up in every season that it covers. The value "8 (group winners)" is probably wrong for other seasons too. I could not check 2002/03, 2003/04 or 2007/08 to 2016/17.
- CFA 2 play-offs (fra-42, fra-51): RSSSF shows promotion and relegation play-offs. For 2004/05 (fra-33) the first agent's own RSSSF source shows the same play-offs, but the row says `none`.
- Club counts of tier 5 (fra-150, fra-159, fra-186, fra-204, fra-231) and tier 4 in 2026/27 (fra-230): Transfermarkt differs by one to three clubs. Transfermarkt can keep a club that withdrew. I did not find a third source. In 2026/27, Transfermarkt shows group C of the National 1 with 17 clubs.
- Transfer windows 2024/25 (fra-208): the first row gives 11 June to 2 September and 3 to 31 January. The LFP dates are 10 June to 30 August 2024 and 1 January to 3 February 2025.

## Found values

| Row | Field | Value and note | URL |
|---|---|---|---|
| fra-1 (2001/02 nation rules) | foreign | 5 non-EU players per team in Division 1 (Assemblée nationale answer, 1 October 2001). Maxifoot (spring 2001, https://www.maxifoot.fr/newsplus225.php) reports the planned change from 3 to 5 in D1 and 1 to 3 in D2. | https://questions.assemblee-nationale.fr/dyn/11/questions/QANR5L11QE61641.pdf |
| fra-5 (2001/02 CFA) | down | 3 per group (16th to 18th) in groups A to C; group D has no marks on the page. Same publisher as the first source. | https://www.rsssf.org/tablesf/fran02.html |
| fra-14 (2002/03 CFA) | down | 3 per group (16th to 18th), 12, plus Limoges (14th, D) for financial reasons. | https://www.rsssf.org/tablesf/fran03.html |
| fra-23 (2003/04 CFA) | down | 3 per group (16th to 18th), 12. | https://www.rsssf.org/tablesf/fran04.html |
| fra-32 (2004/05 CFA) | up | RSSSF: group winners Boulogne, Toulon, Vannes and group B (line unclear on the page), plus Châtellerault after the forced relegation of Racing. Same publisher as the first source. | https://www.rsssf.org/tablesf/fran05.html |
| fra-32 (2004/05 CFA) | down | 3 per group (16th to 18th), plus administrative relegations (Angoulême, Trélissac, Fontenay). Same publisher as the first source. | https://www.rsssf.org/tablesf/fran05.html |
| fra-41 (2005/06 CFA) | down | RSSSF: "Last 3 of each group are relegated to CFA 2." 12. | https://www.rsssf.org/tablesf/fran06.html |
| fra-42 (2005/06 CFA 2) | clubs | 128 (8 groups of 16). | https://www.rsssf.org/tablesf/fran06.html |
| fra-49 (2006/07 Championnat National) | clubs | 20 clubs (RSSSF National 2006/07 final table). | https://www.rsssf.org/tablesf/fran07.html |
| fra-49 (2006/07 Championnat National) | up | 3 (1st to 3rd promoted). | https://www.rsssf.org/tablesf/fran07.html |
| fra-49 (2006/07 Championnat National) | down | 4 (17th to 20th relegated). | https://www.rsssf.org/tablesf/fran07.html |
| fra-50 (2006/07 CFA) | clubs | 72 (4 groups of 18, 34 rounds). | https://www.rsssf.org/tablesf/fran07.html |
| fra-50 (2006/07 CFA) | up | 4 (group winners Calais, Arles, Rodez, Villemomble). | https://www.rsssf.org/tablesf/fran07.html |
| fra-50 (2006/07 CFA) | down | 12 (16th to 18th of each group). | https://www.rsssf.org/tablesf/fran07.html |
| fra-51 (2006/07 CFA 2) | clubs | 128 (8 groups of 16). | https://www.rsssf.org/tablesf/fran07.html |
| fra-59 (2007/08 CFA) | down | 12 (16th to 18th of each group). | https://www.rsssf.org/tablesf/fran08.html |
| fra-68 (2008/09 CFA) | down | 3 per group (16th to 18th) in the groups shown. | https://www.rsssf.org/tablesf/fran09.html |
| fra-105 (2012/13 CFA 2) | clubs | 112 (8 groups of 14). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2012 (and FR5B to FR5L) |
| fra-112 (2013/14 Championnat National) | down | Zone of 4 (15th to 18th). Only Vannes and Uzès went down: Colomiers and Strasbourg were reprieved after Luzenac and Carquefou left. | https://www.rsssf.org/tablesf/fran2014.html |
| fra-114 (2013/14 CFA 2) | clubs | 112 (8 groups of 14). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2013 (and FR5B to FR5L) |
| fra-121 (2014/15 Championnat National) | down | Zone of 4 (15th to 18th). Colomiers relegated, Istres demoted; CA Bastia and Epinal reprieved. | https://www.rsssf.org/tablesf/fran2015.html |
| fra-123 (2014/15 CFA 2) | clubs | 112 (8 groups of 14). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2014 (and FR5B to FR5L) |
| fra-132 (2015/16 CFA 2) | clubs | 111 (8 groups of 14, one of 13). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2015 (and FR5B to FR5L) |
| fra-141 (2016/17 CFA 2) | clubs | 112 (8 groups of 14). | https://www.transfermarkt.fr/x/tabelle/wettbewerb/FR5A/saison_id/2016 (and FR5B to FR5L) |
| fra-145 (2017/18 nation rules) | foreign | 4 non-EU players per Ligue 1 club: "jusqu'à présent limité à quatre" (goal.com, 17 March 2018). | https://www.goal.com/fr/news/la-lfp-ouverte-a-un-cinquieme-extracommunautaire-en-ligue-1/1rt0svpwbl2ur12mmw8ho97h5o |
| fra-163 (2019/20 nation rules) | foreign | Charte du football professionnel 2019/20, art. 553 and 556: Ligue 1 at most 4 non-EU/EEA players under contract and on the match sheet, Ligue 2 at most 2; Cotonou (ACP) nationals do not count (art. 552). | https://sport.newstank.fr/file/public/57c0dfa770f28ef415c584c8e4bc281f/charte-football-professionnel-edition-2019-20.pdf |
| fra-217 (2025/26 nation rules) | windows | Summer 16 June to 1 September 2025, 20:00; winter 1 January to 2 February 2026, 20:00. | https://ligue1.com/fr/articles/l1_article_2392- |

## Other observations

- The LFP Statuts et Règlements 2020/21 (art. 529) gives at most 18 players on the match sheet in Ligue 1 and Ligue 2 and 3 substitutes. LyonMag (1 August 2020) gives 20 for Ligue 1 and 5 substitutes, decided after the IFAB change of May 2020. I used LyonMag for Ligue 1 and the rule book for the Ligue 2 bench of 7.
- RSSSF marks drawn cup ties "aet, pen" in the Coupe de la Ligue from 2016/17 and in the Coupe de France in 2021/22. Match reports show penalties straight after 90 minutes in both. Thus the RSSSF "aet" marks do not settle the extra-time rule for those seasons or later ones. The Coupe de France rows from 2022/23 still say "extra time rule unknown".
- Trophée des Champions: all 26 venues from 2001 to 2026 agree with the RSSSF list. The 2010 venue "Stade 7 November, Tunis" is in Radès, as RSSSF writes it.
- Non-EU rule: the Charte 2019/20 counts Cotonou nationals as non-limited (art. 552) and sets 4 for Ligue 1 and 2 for Ligue 2, under contract and on the match sheet (art. 553 and 556). This matches the first agent's 2026/27 wording. For 2001/02 the limit rose from 3 to 5 in Division 1.

## Club file spot-checks

I compared `fra-clubs.tsv` with a second source for one season per division and read the database matches by eye.

| Season | Division | Second source | Result |
|---|---|---|---|
| 2010/11 | Ligue 1 | RSSSF `fran2011.html` | 20 of 20 clubs agree. All 20 database matches are correct. |
| 2015/16 | Ligue 2 | RSSSF `fran2016.html` | 20 of 20 clubs agree. All 20 database matches are correct (Valenciennes has another club id in this database, 4151; the id belongs to an Indian club in the April 2010 and April 2011 databases, so ids do not carry across databases). |
| 2018/19 | National | RSSSF `fran2019.html` | 18 of 18 clubs agree. All 18 database matches are correct. |
| 2009/10 | CFA | Transfermarkt `FR4A` to `FR4D` | Group A: 19 of 19 clubs agree, matches correct. Transfermarkt has 73 clubs in all; the file lists 72 (the first agent notes this). |
| 2015/16 | CFA | Transfermarkt group C | 15 Transfermarkt clubs all in the file; the file adds AC Arles-Avignon, which left during the season. Matches correct. |
| 2016/17 | CFA | Transfermarkt group C | 15 Transfermarkt clubs all in the file; the file adds Evian Thonon Gaillard, which left during the season. Matches correct. |

I found no wrong name-pattern match in these 109 rows.
