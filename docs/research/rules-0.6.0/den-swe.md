# Rules table: Denmark and Sweden (slice 6, part den-swe)

Date 2026-10-16. Research only. Files of this part:

- `den-swe.tsv`: 324 rows. 27 seasons (2001/02 to 2027/28), 6 rows per nation per season.
- `den-swe-clubs.tsv`: 1,602 rows. Real clubs of each resized or new division at each season start that has a season database.
- `den-swe.md`: this file.

## Scope

| Nation | Rows per season | Original file |
|---|---|---|
| Denmark | nation rules, Superliga, 1st Division, 2nd Division, Danish Cup | `den_prm`, `den_first`, `den_second`, `den_cup` |
| Denmark | Danish Super Cup | `new` (held 2001, 2002, 2004) |
| Sweden | nation rules, Allsvenskan, Superettan, third tier, Svenska Cupen | `swe_prm`, `swe_first`, `swe_second`, `swe_cup` |
| Sweden | Svenska Supercupen | `new` (held 2007 to 2015) |

Sweden plays in calendar years. The 2001/02 row holds the 2002 season, and so on to the 2027/28 row, which holds 2028. The Svenska Cupen rows follow the same rule: the row of 2012/13 holds the 2012-13 cup, with its final in 2013. The 2011/12 row (year 2012) has no cup final, so its format is `not held`.

## Decisions in this part

1. **Swedish third tier.** The original's `swe_second` has 6 groups of 12 with the names of the real Division 2 groups. From 2002 to 2005 Division 2 was the third tier, so the size matches. In 2006 a new Division 1 (2 x 14, Norra and Södra) became the third tier, and it has had 2 x 16 since 2018. The `swe_second` rows follow the real third tier: Division 2 to 2005, Division 1 from 2006. Division 2 from 2006 is tier 4 and the original runs no fourth tier, so it has no rows.
2. **Ettan name.** The pages use "Division 1" up to 2019 and "Ettan" from 2020. I did not find the year of the name change. The `name` column follows the page titles. This is an inference.
3. **Danish 3rd Division.** The 3rd Division (12 clubs, tier 4) started in 2021/22. The original runs three Danish tiers, so the 3rd Division has no rows. The 2nd Division rows note it.
4. **Danish 2nd Division groups.** In the clubs file, "group 1, 2, 3" follow the table order of the season page. The pages give no group names in the tables.
5. **Seasons after the last page.** 2027/28 (Denmark) and the 2027 and 2028 rows (Sweden) carry the last known values. No announced change was found. Each such row says so in `notes`.

## Findings that differ from the brief or the change-points doc

- **Svenska Cupen group stage.** The group stage started in the 2012-13 cup, not in 2015/16. The 2012-13 page says the season "reintroduced a group stage". The format has been the same since: 96 clubs, two knockout rounds, 8 groups of 4, then knockout.
- **Superliga 2019/20.** The main Superliga page dates the 12-club format to 2019/20. The 2019-20 season page shows 14 clubs, with the 4th of each relegation group relegated and a play-off between the 3rd-placed clubs. The table uses the season page. The 12-club format starts in 2020/21. A second source must decide.
- **Allsvenskan.** 14 clubs to 2007, 16 from 2008 (change-point L37 agrees). In 2007 only one club went down.
- **Superliga.** 12 to 2015/16, 14 from 2016/17 to 2019/20, 12 with a split from 2020/21 (L39 agrees on the years of 14).

## Sources

Every fact field carries a URL in `sources`. The main sources:

- Wikipedia season pages for every league and cup season (English; Swedish for Ettan 2021, 2022, 2023 and 2026). I read the rendered league tables and counted the clubs and the zones of each table with a script.
- Wikipedia main pages: Danish Superliga, Danish 2nd Division, Danish Cup, Danish Super Cup, Allsvenskan, Ettan Fotboll, Svenska Supercupen, Transfer window, Laws of the Game.
- Swedish Wikipedia "Allsvenskan (fotboll)" for the transfer windows (introduced 15 November 2004) and the play-off rule.
- fck.dk (15 July 2022): Superliga bench of 9 from 2022/23, before that 7, and 5 substitutes.
- fotbollskanalen.se (25 November 2022): Allsvenskan bench of 9 from 2023, before that 7; Superettan stays at 7; max 9 foreign or non-homegrown players in the matchday squad.
- fotbolldirekt.se (2020, 2021, 2022): five substitutes in Allsvenskan, Superettan and Ettan for 2020, 2021 and 2022; IFAB made the rule permanent on 1 July 2022.

## Inferences

These values are marked in `notes` as inferences:

- 3 substitutes before 2020 for every competition, from the Laws of the Game. No Danish or Swedish page states it per league.
- 5 substitutes in Denmark in 2020/21, and in the Danish lower divisions, from the IFAB rule.
- 5 substitutes in Sweden from 2023, from the permanent IFAB rule.
- Bench sizes after the season of the source, where no later change was found.
- Up count of the Danish 2nd Division 2001/02 to 2004/05, from the 1st Division relegations.
- Sweden had no transfer windows before 15 November 2004.
- Season of the Danish Super Cup match (the summer match counts for the season that follows it).

## Unknowns

- Denmark: foreign player limits, work permits, squad registration and transfer window dates. I found no source. The original uses 3 non-EU for Denmark.
- Sweden: foreign player limits before 2022, work permits, squad registration, window dates other than 2013.
- Bench sizes: Denmark before 2021/22 and below the Superliga; Sweden before 2022, and Ettan in all years.
- Danish 2nd Division 2001/02 to 2005/06: relegation counts, play-offs and the 2005/06 club list (no season page in English or Danish, and the RSSSF season pages hold no 2nd Division tables).
- Danish Cup club counts for 2001/02 to 2010/11 and 2012/13 to 2014/15.
- Danish Cup 2026/27 and 2027/28, and Svenska Cupen 2026-27 and 2027-28: no page yet.

## Club moves

The databases keep the 2001/02 layout in every season: Superliga 12, 1st Division 16, 2nd Division 16, Allsvenskan 14, Superettan 16, Swedish Second Division 6 x 12. From November 2020 the databases also hold a "Danish Kvalifikationsrækken" pool (46 to 62 clubs).

The clubs file lists every real club where the real count differs from the database:

| Division | Seasons listed | Real count |
|---|---|---|
| Superliga | 2016/17 to 2019/20 | 14 |
| 1st Division | 2011/12 to 2025/26 | 14, then 12 |
| 2nd Division | 2007/08 to 2025/26 | 32, 24, 28, then 12 |
| Allsvenskan | 2007/08 to 2025/26 (years 2008 to 2026) | 16 |
| Division 1 / Ettan | 2005/06, 2007/08 to 2025/26 (years 2006, 2008 to 2026) | 28, then 32 |

Superettan and the Danish divisions of 2001/02 to 2003/04 match the database count, so they have no list. 2004/05 and 2006/07 have no season database.

Method of the match: a script compares the club name and the page link name with the database names of the same nation, first exactly after a normalisation, then by a hand-checked list of known name pairs (for example AaB to Aalborg BK, GAIS to Göteborgs AIS, Copenhagen to FC København). For a short list of renamed clubs it uses the club id of an earlier database (for example Elite 3000, which an older database calls Helsingør Idrætsforening). I rejected every match that joined two different clubs, so a club with no safe match has an empty `db_club_id`. Reserve teams carry "(reserve team)" and have no database record.

124 matched clubs sit in "A Lower Division" in the database, that is, with no division. They can fill a short division under Q244.

### Missing clubs for Max

72 real clubs have no database record. 23 of them are Superliga reserve teams, which the 2nd Division allowed from 2005/06 to 2009/10.

- 2005/06 Division 1 Norra: Syrianska FC, Falu FK
- 2005/06 Danish 2nd Division: club list unknown (28 real clubs against 16)
- 2007/08 Danish 2nd Division group 1: Brøndby 2, KB (FCK 2) (reserve teams)
- 2007/08 Danish 2nd Division group 2: OB 2, AGF 2, AaB 2, Esbjerg 2, Ikast FS (FCM 2) (reserve teams)
- 2007/08 Division 1 Norra: Syrianska FC, Ersboda SK, Falu FK
- 2008/09 Danish 2nd Division group 1: Brøndby 2, Copenhagen 2 (reserve teams)
- 2008/09 Danish 2nd Division group 2: Blokhus FC, Aarhus Fremad; reserve teams OB 2, Esbjerg 2, Randers Freja (RFC 2), AaB 2, Ikast FS (FCM 2), AGF 2
- 2008/09 Division 1 Norra: Skellefteå FF
- 2008/09 Division 1 Södra: FC Rosengård
- 2009/10 Danish 2nd Division group 1: Boldklubberne Glostrup Albertslund, Allerød FK; reserve teams Brøndby 2, Copenhagen 2
- 2009/10 Danish 2nd Division group 2: reserve teams Randers Freja (RFC 2), OB 2, Esbjerg 2, Ikast FS (FCM 2), AGF 2, AaB 2
- 2009/10 Division 1 Norra: Dalkurd FF
- 2009/10 Division 1 Södra: FC Rosengård
- 2010/11, 2011/12, 2012/13 Allsvenskan: Syrianska FC
- 2010/11 Danish 2nd Division group 1: Boldklubberne Glostrup Albertslund, Allerød FK, Ballerup-Skovlunde Fodbold
- 2010/11 Danish 2nd Division group 2: Rishøj Boldklub, FC Djursland
- 2011/12 Danish 2nd Division group 1: Rishøj Boldklub, Svebølle B&I, SC Egedal, Boldklubberne Glostrup Albertslund
- 2011/12 Danish 2nd Division group 2: Kjellerup IF, Aarup BK, Viby IF
- 2013/14 Danish 2nd Division group 1: Gentofte-Vangede Idrætsforening
- 2013/14 Danish 2nd Division group 2: Odder IGF
- 2014/15 Division 1 Norra: Södertälje FK
- 2015/16 Danish 2nd Division group 2: Dalum IF
- 2015/16 and 2016/17 Division 1 Norra: Team TG FF
- 2016/17 Danish 2nd Division group 3: VSK Aarhus
- 2016/17 Division 1 Södra: FK Karlskrona
- 2018/19 Division 1 Södra: Assyriska IK
- 2018/19 to 2022/23 Danish 2nd Division: AB Gladsaxe
- 2019/20 Ettan Norra: IF Karlstad Fotboll
- 2020/21 Ettan Norra: Hammarby TFF
- 2021/22 Ettan Norra: FC Stockholm Internazionale
- 2022/23 Ettan Norra: Nordic United FC, FC Stockholm Internazionale
- 2023/24 Ettan Norra: FBK Karlstad
- 2025/26 Ettan Södra: AFC Malmö

Some of these clubs may exist under an older name: Syrianska FC (the older databases hold "Syrianska Föreningen", but that did not match in the 2010 to 2012 databases), Falu FK, Kristianstad FC and FC Höllviken (matched to their predecessors Kristianstads FF and Höllvikens GIF, an inference). Max decides.

### Name pairs that are inferences

Matched to a predecessor or merged club: IF Limhamn Bunkeflo to Bunkeflo IF, Kristianstad FC to Kristianstads FF, FC Höllviken to Höllvikens GIF, FC Vestsjælland to the database record of Slagelse B&I (same club id), AFC Eskilstuna and AFC United to the record of Väsby United (same club id), Vasalunds IF to Vasalund Essinge.

## Reconcile

Date 2026-10-16. Third agent. Input: the 15 `disagree` rows and the 27 `found` rows of `den-swe.check.tsv`. Output: `den-swe.reconcile.tsv` (43 rows), changes to 24 rows of `den-swe.tsv`, and one added row in `den-swe-clubs.tsv`. Row order and ids are unchanged.

### Counts

| Input | Decision | Rows |
|---|---|---|
| disagree | second | 13 |
| disagree | conflict | 2 |
| found | accepted | 12 |
| found | rejected | 15 |

### Settled

- Danish 2nd Division `up` 2006/07 to 2009/10: 2. The two sources agree on the facts. The first value counted the play-off winner, and the brief counts direct places only. The play-off winners are in `notes`.
- Danish 2nd Division 2010/11 and 2011/12 `up`: 0. The season pages say the two group winners play off for one place. The found value 1 is the outcome. Blokhus FC and FC Fyn are in `notes` as inferences.
- Danish 2nd Division 2006/07 `down`: 0. The level grew to 2 x 16, so it had no relegation places. The reserve teams of Silkeborg and Vejle went down with their first teams (in `notes`).
- Danish 2nd Division 2009/10: 32 clubs, `down` 0 (relegation game between the 16th-placed clubs). Aarhus Fremad was added to the clubs file with database record 3761 (September 2009, Danish Second Division).
- Danish 2nd Division 2020/21 `down`: 16. Places 7 to 10 went to the new tier 4, and places 11 to 14 went to the Denmark Series.
- Superliga 2016/17 and 2017/18 `playoffs`: a cross round, then 11th-place and 13th-place ties. The match results show this.
- Superliga 2018/19 `split`: the 11th and 14th went to group A and the 12th and 13th to group B, then the 7th and 8th chose a group.
- Danish Cup 2016/17: 107 clubs. FC Vestsjælland did not enter.
- Superettan bench 2026 to 2028: 9. A third publisher (Västergötland FA annual report 2025) confirms it.
- Accepted found values: Danish work permit 2026/27 and 2027/28 (2027/28 is an inference), Swedish foreign rules 2006, 2007, 2008 and 2017, Swedish windows 2020, 2023 and 2024, Svenska Cupen 2026/27 group stage.
- Rejected found values: Danish Cup club counts that are only sums of tie counts, the Danish work permit for 2024/25 and 2025/26 (no start date), and the Swedish 3 non-EU rule for 2002 to 2005 (the page says only "previous seasons").

### Conflicts for Max

1. **den-swe-89, Danish Cup 2015/16 club count.** The first source has 108 in its infobox. The round figures on the same page (94, then 47 winners and 9 new clubs, then 28 winners and 4 new clubs) make 107, and RSSSF agrees with 107. A bye may explain the difference. No third publisher was found. The row keeps 108.
2. **den-swe-274, Swedish third tier 2020 name.** The first value is "Ettan" and the second is "Division 1". The league body (ettanfotboll.se) calls the tier Ettan since 2006 and renamed its own organisation in 2015. RSSSF uses Division 1 up to 2020. No source gives the year that the official competition name changed. The row keeps "Ettan". This is a question of reading.

### Observations outside the disputed fields

- den-swe-58 (2nd Division 2010/11) `down` is 8, from the table marks (13th to 16th). The intro of the same page says that only the bottom club of each group went down, plus the loser of a play-off between the 15th-placed clubs. This was not a disputed field, so the row is unchanged.
- A Västergötland FA report says the November 2025 federation meeting also set a new play-off format between Superettan and Allsvenskan. The Superettan and Allsvenskan 2026 `playoffs` fields may need a check.


## Loose ends

- den-swe-58 (Danish 2nd Divisions 2010/11) `down` 8: kept. The English season page gives a plan of March 2010 with 3 clubs down (the last of each group and the loser of a play-off of the two 15th-placed clubs). Its final tables mark 13th to 16th of each group as relegated. The Danish Wikipedia page of 2011-12 lists no 13th to 16th club of 2010-11 among its clubs. The note now gives both.
- Allsvenskan and Superettan play-off from 2026 (den-swe-308, 309, 314, 315, 320, 321): the SvFF representatives' meeting of 28 November 2025 adopted a Superettan play-off. The 3rd and 4th play over two legs (4th at home first) and the winner meets the Allsvenskan 14th over two legs. Sources: https://fotbolldirekt.se/?p=345425 and https://hurbra.se/nya-kvalet-allsvenskan-2026-fyrans-chans/. The 2027 and 2028 rows carry the rule (inference).

## Source fill

Method: I ran the rules lint and took each filled fact field with no `field=URL` entry. I read each page on 2026-10-16 with curl and checked the value against the page text or league table. For `split` = none and `format` = double round robin, the season page must show one table for the division with every club on 2 x (clubs - 1) matches, and no championship or relegation round. For a stopped season (2019/20, 2020/21), a table with fewer matches counts if the page says the season was stopped. I added sources only. I changed no value, row order or id.

- Sources added: 4 `split` entries (den-swe-4, 10, 16, 22, 2001/02 to 2004/05), from the Danish 1st Division season pages that the rows already cite.

No source found: none. Disagreements: none.
