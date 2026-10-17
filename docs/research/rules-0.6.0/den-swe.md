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

## Pass 2

Date 2026-10-17. Decision agent. Input: Max's decisions Q268 to Q331 in the spec. Row ids and row order of `den-swe.tsv` are unchanged. `rules_lint.py` gives 0 problems before and after.

### Counts

| Item | Count |
|---|---|
| Rows deleted from `den-swe.tsv` | 0 |
| Club rows deleted | 0 |
| Conflicts and open points settled | 3 (den-swe-89, den-swe-274, den-swe-58) |
| Club rows added | 334 (Superettan 320, Allsvenskan 2006 14) |
| Club rows given a database match | 12 |
| Reserve teams listed (Q289) | 23 entries, 8 teams |
| Real clubs still without a database record | 37, plus the 23 reserve entries |

### No new tiers (Q331) and no new nations (Q288)

The original runs three tiers in each nation (`den_prm`, `den_first`, `den_second`; `swe_prm`, `swe_first`, `swe_second`). This part already had no row below tier 3, and no competition needs another nation. Nothing was deleted.

- The Danish 3rd Division (tier 4 from 2021/22) and the Swedish Division 2 from 2006 (tier 4) stay out.
- The Danish Super Cup and the Svenska Supercupen stay in. They are not tiers, and their clubs come from tier 1.
- No real tier of this part ends. The Swedish third tier changes from Division 2 to Division 1 in 2006, and `swe_second` follows the real third tier.

### Conflicts settled (Q293, Q297 to Q327)

1. **den-swe-89, Danish Cup 2015/16: 107 clubs.** The `format` field now says 107. RSSSF lists 47 first-round ties (one awarded), 28 second-round ties and 16 third-round ties. That is 94 clubs, then 9 new clubs, then 4 new clubs. The Wikipedia round text gives the same figures. The infobox figure 108 matches no round. Sources: https://www.rsssf.org/tablesd/den2016.html and https://en.wikipedia.org/wiki/2015%E2%80%9316_Danish_Cup
2. **den-swe-274, Swedish third tier name in 2020: Ettan.** The row keeps "Ettan". In 2020 the league body and the press call the tier Ettan Norra and Ettan Södra. The league body uses the name Ettan for the tier from 2006 (https://ettanfotboll.se/om-oss/). Division 1 remains the formal series name of the tier in the league system. I found no source with the year that the official name changed. The rows 2006 to 2019 keep "Division 1", which follows the season page titles.
3. **den-swe-58, Danish 2nd Division 2010/11 `down`: 8.** The final tables mark 13th to 16th of each group as relegated. None of these 8 clubs is in the 2011-12 2nd Division (https://en.wikipedia.org/wiki/2011%E2%80%9312_Danish_2nd_Divisions). The plan of March 2010 for 3 relegations did not happen. The row keeps 8.

The two Superettan and Allsvenskan play-off points of 2026 were settled in "Loose ends" above. The Superliga 2019/20 point was settled by the reconcile (14 clubs, 12 from 2020/21).

### Club moves (Q279, Q280, Q290)

I went down the three tiers of each nation in each season with a database. Clubs that a listed tier takes from an unlisted tier leave a gap there.

- **Superettan, 2005/06 and 2007/08 to 2025/26 (years 2006 and 2008 to 2026).** Each season the Allsvenskan list and the third-tier list take 3 to 7 clubs from the database's Superettan. The real Superettan has 16 clubs, so the list holds all 16. Source: the Wikipedia season page of each year (`https://en.wikipedia.org/wiki/<year>_Superettan`).
- **Allsvenskan 2005/06 (year 2006).** The Superettan list of 2006 takes Landskrona BoIS, GIF Sundsvall and Assyriska FF from the database's Allsvenskan. The list holds the 14 real clubs. Source: https://en.wikipedia.org/wiki/2006_Allsvenskan
- **Denmark.** No listed tier takes a club from an unlisted Danish tier. No list was added.

After these lists, no unlisted tier of this part loses or gains a club.

New database matches (pool: clubs with no division and clubs of the lower and spare divisions):

- Syrianska FC = "Syrianska Föreningen" (2005/06, 2007/08, 2010/11 to 2012/13, and the Superettan lists). The club was founded in 1977 in Södertälje as Syrianska Föreningen (https://sv.wikipedia.org/wiki/Syrianska_FC). Each database places the record in the real division of that year or one tier away.
- AB Gladsaxe = "Akademisk Boldklub" (2018/19 to 2022/23). The full name of the club is Akademisk Boldklub Gladsaxe (https://en.wikipedia.org/wiki/Akademisk_Boldklub). The October 2017 database calls the same record "AB Gladsaxe".
- 2005/06 Division 1 Norra: the club was Falu BS, not Falu FK (https://sv.wikipedia.org/wiki/Division_1_i_fotboll_f%C3%B6r_herrar_2006). Falu FK was formed in autumn 2006 as a joint club of Falu BS, Korsnäs IF and Slätta SK (https://sv.wikipedia.org/wiki/Falu_FK). The 2007/08 row of Falu FK takes the Falu BS record. This is an inference.
- Ljungskile SK = "Panos Ljungskile SK" (Superettan 2006, September 2005 database).

The Danish 2nd Division 2005/06 (28 real clubs against 16) is still not listed. No page that I could read gives the 2005-06 tables. The English season page now redirects to the main page, and the soccerway pages hold no table text.

### Missing clubs for Max

These real clubs have no record in their season's database after the pass-2 matches:

- Danish 2nd Division: 2008/09 Blokhus FC, Aarhus Fremad; 2009/10 Boldklubberne Glostrup Albertslund, Allerød FK; 2010/11 Boldklubberne Glostrup Albertslund, Allerød FK, Ballerup-Skovlunde Fodbold, Rishøj Boldklub, FC Djursland; 2011/12 Rishøj Boldklub, Svebølle B&I, SC Egedal, Boldklubberne Glostrup Albertslund, Kjellerup IF, Aarup BK, Viby IF; 2013/14 Gentofte-Vangede IF, Odder IGF; 2015/16 Dalum IF; 2016/17 VSK Aarhus.
- Swedish third tier: 2007/08 Ersboda SK; 2008/09 Skellefteå FF, FC Rosengård; 2009/10 Dalkurd FF, FC Rosengård; 2014/15 Södertälje FK; 2015/16 and 2016/17 Team TG FF; 2016/17 FK Karlskrona; 2018/19 Assyriska IK; 2019/20 IF Karlstad Fotboll; 2020/21 Hammarby TFF; 2021/22 FC Stockholm Internazionale; 2022/23 Nordic United FC, FC Stockholm Internazionale; 2023/24 FBK Karlstad; 2025/26 AFC Malmö.
- Possible predecessor records (inference, not matched): Boldklubberne Glostrup Albertslund (Glostrup FK - IF 32 and Albertslund IF), Ballerup-Skovlunde Fodbold (Skovlunde IF).

### Reserve and B teams (Q289)

The Danish 2nd Division admitted Superliga reserve teams from 2005/06 to 2009/10. No season database holds them. The build adds them as new club records:

| Season | Group | Reserve teams |
|---|---|---|
| 2007/08 | 1 | Brøndby 2, KB (FC København 2) |
| 2007/08 | 2 | OB 2, AGF 2, AaB 2, Esbjerg 2, Ikast FS (FC Midtjylland 2) |
| 2008/09 | 1 | Brøndby 2, FC København 2 |
| 2008/09 | 2 | OB 2, Esbjerg 2, Randers Freja (Randers FC 2), AaB 2, Ikast FS (FC Midtjylland 2), AGF 2 |
| 2009/10 | 1 | Brøndby 2, FC København 2 |
| 2009/10 | 2 | Randers Freja (Randers FC 2), OB 2, Esbjerg 2, Ikast FS (FC Midtjylland 2), AGF 2, AaB 2 |

The 8 teams are Brøndby 2, FC København 2 (KB in 2007/08), OB 2, AGF 2, AaB 2, Esbjerg 2, FC Midtjylland 2 (Ikast FS) and Randers FC 2 (Randers Freja). The sources are the season pages in `den-swe-clubs.tsv`. The 2005/06 list is unknown (see above), and 2006/07 has no database. Sweden needs no reserve or B team in tiers 1 to 3.

### Part rules (Q277, Q283, Q284, Q294, Q296)

None applies to this part. The Svenska Cupen and Danish Cup rows already put each edition in its normal slot.

## Second source, pass 2

Date 2026-10-17. Second-source agent. Input: the 840 lines of `den-swe.check.tsv` with verdict `no second source`. Output: `den-swe.check2.tsv`, 189 lines. I read every second page on 2026-10-17 with curl, except two pages that I read with WebFetch and quoted.

### Scope

| Item | Lines |
|---|---|
| `no second source` lines in the check file | 840 |
| Field holds `unknown` (out of scope) | 377 |
| Field holds an inference or a carried value (out of scope) | 274 |
| Lines checked in `den-swe.check2.tsv` | 189 |

The inferences out of scope are the rows that `notes` marks as inferences. These are the 3 substitutes before 2020 from the Laws of the Game, the 5 substitutes from the IFAB rule, bench sizes after the source season, the Danish 2nd Division `up` of 2001/02 and 2002/03, the Swedish "no transfer windows" of 2002 to 2004, and the 2027/28 Danish rows and 2027 and 2028 Swedish rows that carry the last known values.

### Counts

| Verdict | Lines |
|---|---|
| agree | 160 |
| disagree, settled | 0 |
| disagree, still split | 0 |
| no second source | 29 |

No line disagrees, so `den-swe.tsv` and `den-swe-clubs.tsv` are unchanged. `rules_lint.py` gives 0 problems for this part before and after.

### Second sources

- Denmark, leagues: the final tables of each season on danskfodbold.com, the site of the DBU's official statisticians (`https://www.danskfodbold.com/tabel.php?seasonid=<end year>`). They give the clubs, groups, promotion, relegation and play-off places of the Superliga, the 1st Division and the 2nd Division.
- Denmark, bench and substitutes: bold.dk (12 July 2022), with the Divisionsforeningen decision of a bench of nine from 2022/23 and five substitutes. A danskfodbold.com match report of July 2021 shows 5 substitutions per team.
- Denmark, cups: TV 2 Sport (DBU Pokalen 2026/27) and the RSSSF Super Cup list.
- Sweden, Division 2 of 2002 and Division 1 of 2011 to 2017: svenskafotbollsklubbar.se series pages (clubs per series) and club pages (division of the next year). ettanfotboll.se states 14 clubs in each Division 1 series from 2006 to 2017.
- Sweden, substitutes: the SvFF notices of 2020, 2021 and 2022 and the SvFF rules of Svenska Cupen 2021/2022.
- Sweden, 2026: the SvFF play-off page for 2026, the Svensk Elitfotboll Allsvenskan page and ettanfotboll.se.

### Derived values

The `down` lines of the Swedish Division 2 of 2002 and Division 1 of 2011 to 2017, and the `up` and `playoffs` lines of the Swedish Division 2 of 2002, are marked `agree`, but the second publisher does not state them. I counted them from its club pages: a club that played in a lower division in the next year counts as relegated. Each such line says "derived, not stated" in its note.

### Still no second source

- 18 Swedish `windows` lines ("two windows since 15 November 2004"), 5 Swedish `foreign` lines and 4 Swedish `bench` lines (den-swe-284, 285, 290, 291). The web search limit of this session was reached before I found a second publisher.
- den-swe-156 `format` (Danish Super Cup 2026/27, `not held`). The RSSSF list ends with the 2004 final, but its last update is 30 May 2025.
- den-swe-310 `down` (Ettan 2026, 6). No page of another publisher states the 2026 relegation places.

### Observations outside the checked fields (no change made)

These fields are inferences or `unknown`, so pass 2 does not change them. The values are for the maintainer and Max.

- den-swe-4, Danish 2nd Division 2001/02 `up` 2 (inference): the DBU statistics page lists 3 promoted clubs (BK Skjold, Ølstykke FC, HIK). Lyngby FC went bankrupt and was moved down two levels. The page does not say whether the third place was a rule place.
- Danish 2nd Division `down` (`unknown` in den-swe-4, 10, 16, 22, 28): the DBU statistics pages list 2 relegated in 2001/02, 3 in 2002/03, 3 in 2003/04, none in 2004/05, and 5 in 2005/06 (3 from the west group, 2 from the east group).
- Danish 2nd Division 2005/06: the DBU statistics page (`https://www.danskfodbold.com/tabel.php?seasonid=2006`) gives both group tables of 14. It lists the reserve teams Esbjerg fB 2 and Silkeborg IF 2 (west) and Brøndby IF 2 and KB (FC København 2) (east). The pass-2 decision section says this list was unknown.
- The bold.dk article says the bench of nine from 2022/23 also applies to the Danish 1st Division.
- The SvFF special rules for men 2026 (`https://www.svenskfotboll.se/48e766/globalassets/svff/dokumentdokumentblock/tavling/tavlingsforeskrifter/2026/stb-herr-2026-v1-251222.pdf`, 5 kap. 4 §) allow at most nine substitutes on the team sheet in Allsvenskan and Superettan.
- The SvFF play-off page for 2026 does not give the away-goals rule. The playoffs text of den-swe-308 and the rows that copy it includes away goals.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after. Source: the DBU statisticians' final tables, https://www.danskfodbold.com/tabel.php?seasonid=2002 to seasonid=2006, read on 2026-10-17.

| Row | Field | Old | New |
|---|---|---|---|
| den-swe-4 (2001/02) | down, playoffs | unknown | 2 (15th, 16th), none |
| den-swe-4 | up | 2 | 2 (kept; the page lists 3 promoted, see below) |
| den-swe-10 (2002/03) | down, playoffs | unknown | 3 (14th to 16th), none |
| den-swe-16 (2003/04) | down, playoffs | unknown | 3 (14th to 16th), none |
| den-swe-22 (2004/05) | down, playoffs | unknown | 0, none (the level grew to 2 groups of 14) |
| den-swe-28 (2005/06) | up | 3 | 2 (the best eligible club of each group) |
| den-swe-28 | down | unknown | 4 (13th and 14th of each group) |
| den-swe-28 | playoffs | unknown | the best eligible runner-up of each group play two legs for one 1st Division place |

- den-swe-4 `up`: BK Skjold, Ølstykke FC and HIK (places 1 to 3) went up. The 1st Division had 2 relegation places. Lyngby BK went bankrupt and was sent from the Superliga to the Danmarksserien, so the third promotion filled its place. This reading is an inference from the same page, and it is in the row notes.
- den-swe-28 outcome: Esbjerg fB 2 won the vest group but could not go up, so Aarhus Fremad went up. Næstved BK won the øst group. Thisted FC and Holbæk B&IF played off. Five clubs went down: Dalum IF (12th) was also sent down after the FC Fyn merger of B 1909, B 1913 and Dalum IF.
- Club moves: the 2005/06 database (September 2005) has 16 clubs in the Danish Second Division, and the real level has 28. I added the 28 clubs to `den-swe-clubs.tsv` as "Danish 2nd Division group 1" (vest) and "group 2" (øst), in the table order of the DBU page.
- Matches to note: Aarhus Fremad is club 184 ("Aarhus Fremad", A Lower Division). Club 3761 ("FC Aarhus", Danish First Division) is the same club under its 2002 to 2004 name (the DBU page records the change of name). FC Roskilde is club 9671 ("Roskilde Boldklub Af 1906"), which is an inference: the database name is the older club. B 1913 and B 93 are in the database's First Division.
- Missing clubs for Max, 2005/06 Danish 2nd Division: Hobro IK, BSV, AB 1970, Stenløse BK, Værløse BK. Reserve teams for the build (Q289): Esbjerg fB 2, Silkeborg IF 2, Brøndby IF 2, KB (FC København 2). The line "2005/06 Danish 2nd Division: club list unknown" in "Missing clubs" no longer applies.

## Second source, retry

Date 2026-10-17. Second-source retry agent. Input: the 27 Swedish lines of `den-swe.check2.tsv` with verdict `no second source` (18 `windows`, 5 `foreign`, 4 `bench`). I updated these lines in place. I did not touch den-swe-156 and den-swe-310. I read every page on 2026-10-17 with curl, or through the FotbollDirekt site API.

### Counts

| Verdict | Lines |
|---|---|
| agree | 17 |
| disagree, settled | 0 |
| disagree, still split | 0 |
| still no second source | 10 |

| Field | Agree | Still no second source |
|---|---|---|
| windows | 8 (2011, 2015, 2016, 2017, 2021, 2022, 2025, 2026) | 10 (2005 to 2010, 2012, 2014, 2018, 2019) |
| foreign | 5 (2022 to 2026) | 0 |
| bench | 4 (den-swe-284, 285, 290, 291) | 0 |

No line disagrees, so `den-swe.tsv` and `den-swe-clubs.tsv` are unchanged. `rules_lint.py` gives 0 problems for this part before and after.

### Second sources

- SvFF Tävlingsbestämmelser 2022 to 2026, 4 kap. 4 § (homegrown players) and 5 § (substitutes on the team sheet). I read copies on svenskfotboll.se and its district pages.
- SvFF Representationsbestämmelser 2022, 2025 and 2026, which give the two registration periods and their dates.
- FotbollDirekt articles of 2015, 2016, 2017 and 2021, which give the closing date of the winter window and the opening date of the summer window.
- A Djurgården club article of 2011 with the two Swedish windows.

### Notes on the checked lines

- den-swe-283 `foreign` (2022) is marked `agree`, but the value is derived. The 2022 rules say that at least half of the team sheet must be homegrown, with at most seven substitutes. That makes at most 9 of 18 non-homegrown. The explicit cap of nine first appears in the 2023 text.
- The 10 `windows` lines still without a second source need a dated page of that year. The oldest SvFF representation rules that I found online are those of 2022. FotbollDirekt has few pages before 2012. For 2018 and 2019 I found only the summer window. FIFA RSTP art. 6.1 requires two registration periods in every association, but it does not name Sweden, so I did not count it.

### Observations outside the checked lines (no change made)

- The SvFF representation rules give dates that the table holds as `unknown`. 2022: 8 January to 31 March and 15 July to 11 August (men). 2023 (in force 25 November 2022): 1 February to 28 March and 20 July to 16 August (men). The summer window was later extended to 7 July to 31 August. 2024: 1 February to 27 March and 2 July to 26 August (men). 2025: 29 January to 25 March and 8 July to 29 August. 2026: 29 January to 25 March and 7 July to 31 August.
- The 2011 club article gives 1 January to 31 March and 1 August to 31 August for Sweden.
- SvFF Tävlingsbestämmelser 2025 still allow at most seven substitutes in Superettan. The 2026 text allows nine in Allsvenskan and Superettan.

## Pass 3 (Q337)

Q337 (2026-10-17): a real play-off against a tier that is not in the game is played against a club with no division, as for the Scottish pyramid play-off (Q284). The rows below keep the real format in `playoffs`. Each row gets this note: "Q337: the lower-tier side is a club with no division in the game." No other field changed.

Rows: 166, 172, 178 (Swedish Division 2, 10th against Division 3 clubs), 262, 268, 274, 280, 286, 292, 298, 304, 310, 316, 322 (Swedish Division 1, 13th against Division 2 clubs).

Not changed, because the row does not say who the opponent is: 70 and 82 (Danish 2nd Divisions), 250 (Swedish Division 1, 2015/16).
