# Rules table part aus-jap-kor: Australia, Japan, South Korea

Slice 8 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Research of 2026-10-16. Files:

- `aus-jap-kor.tsv`: 459 rows, 17 competitions by 27 seasons (2001/02 to 2027/28), with one `nation rules` row per nation per season.
- `aus-jap-kor-clubs.tsv`: 1,656 club rows for the divisions whose real size differs from the season database, or that the databases do not hold.

## Method

1. I read each competition's main Wikipedia article and the season page of every season. I fetched the pages as raw HTML with `curl` and read the table rows directly. The Wikipedia API refused the requests (HTTP 429), so I used the normal page URLs. The URLs in the table are the pages that I read.
2. I counted the clubs of each season from the league table on its season page. Where a page timeline and the table disagree, the table wins (J3 2018 and 2019, see below).
3. For rules that Wikipedia does not give (substitutes, bench, foreign players, squad registration), I read league press releases and news pages. Those URLs are in the `sources` column.
4. A script matched the real clubs to the clubs of each season database by name, with a list of old and new spellings (for example Chunnam Dragons for Jeonnam Dragons, Pusan I.cons for Busan IPark, Vent Foret Kofu for Ventforet Kofu). I checked every match that is not exact by hand and removed the wrong ones.

## Season mapping

- Australia plays October to May, so the row label is the real season.
- Japan and South Korea play calendar years. Following the brief, the 2001/02 row holds the year 2002, up to the 2024/25 row with 2025.
- Japan moves to autumn-spring. The 2025/26 row holds the one-off half-season of February to June 2026 (the J1 100 Year Vision League and the J2/J3 100 Year Vision League). The 2026/27 row holds the first autumn-spring season, August 2026 to May 2027.
- South Korea stays on calendar years: the 2025/26 row holds 2026, the 2026/27 row holds 2027 and the 2027/28 row holds 2028.
- The Australia Cup and the Australian Championship are calendar-year competitions. I put the edition of year Y in the Y/Y+1 row, because their proper rounds run from July to December, at the start of that A-League season. This is my decision against the letter of the brief's calendar rule, which Max can reverse.
- The Japanese cups follow the edition numbers. The 105th Emperor's Cup was 2025 and the 106th runs August 2026 to 1 January 2027, so the 2025/26 row has no Emperor's Cup. The J.League Cup has the same gap (33rd edition 2025, 34th edition 2026/27). Both "not held" rows are inferences from the numbering.

## Change points

### Australia

- NSL: 13 clubs in 2001/02, 2002/03 and 2003/04. Top six to the finals in all three seasons. In 2002/03 the top six played a home-and-away championship series, and the top two met in the grand final. The NSL folded after 2003/04. No national league in 2004/05.
- A-League from 2005/06, renamed A-League Men from 2021/22. Clubs: 8 (2005/06 to 2008/09), 10, 11 (2010/11), 10 (2011/12 to 2018/19), 11, 12 (2020/21 to 2023/24), 13 (2024/25), 12 (2025/26 and 2026/27). No promotion or relegation.
- Finals: top four with a page play-off to 2008/09. Top six from 2009/10, with three formats (double chance to 2011/12, three weeks without double chance to 2020/21, and two-legged semi-finals from 2021/22).
- Australia Cup (FFA Cup to 2021) from 2014. 32-club proper, single matches, extra time and penalties. The 2020 edition was cancelled.
- Australian Championship: a national second tier of 16 NPL clubs from October 2025, with groups of four and a knockout. It has no promotion to the A-League. The original has no code for it.

### Japan

- J1 clubs: 16 (2002 to 2004), 18 (2005 to 2020), 20 (2021), 18 (2022, 2023), 20 (2024 on).
- J1 stages: two stages to 2004, single table 2005 to 2014, two stages with a championship stage in 2015 and 2016, then a single table again.
- J1 relegation: 2, then 0 in 2004 (16th to a play-off), 2 plus a play-off (2005 to 2008), 3 (2009 to 2017), 2 plus the J1 entry play-off (2018, 2019, 2022), 0 (2020), 4 (2021), 1 (2023), 3 (2024 on).
- J2 clubs: 12 (to 2005), 13, 15, 18, 19, 20, 22 (2012 to 2023), 20 (2024 on). J2 promotion play-offs for 3rd to 6th from 2012.
- J3 from 2014. Clubs: 12, 13, 16, 17, 17, 18, 18 in 2020 (FC Tokyo U-23 withdrew before the first match), 15, 18, 20 (2023 on). Relegation to the JFL from 2023, only against JFL clubs with a J3 licence.
- Foreign players: 3 to 2008, 3 plus 1 AFC from 2009, partner nations exempt from 2017, and from 2019 no limit on registration with 5 per J1 matchday squad (4 in J2 and J3).
- Substitutes: 3 to 2019, 5 from 2020. Bench: 7 in 2024, 9 from 2025.
- Squad registration: no upper limit on professional players from the 2026 special season. The limit was 27 Pro A players in principle in 2025, and 25 A-contract players in 2008.

### South Korea

- K League: one division to 2012 with no relegation to 2011. Clubs: 10 (2002), 12, 13, 13, 14, 14, 14, 15, 15, 16, 16 (2012).
- Formats: single table (2002, 2003), two stages and a four-club championship (2004 to 2006), six-club championship play-off (2007 to 2011), split into top 8 and bottom 8 in 2012.
- From 2013: K League Classic (K League 1 from 2018) and K League Challenge (K League 2 from 2018). K League 1 has 14 clubs in 2013, 12 from 2014, and 14 from 2027.
- Promotion-relegation play-off from 2013. From 2022, two series: 10th against the K League 2 play-off winner, and 11th against the K League 2 runner-up.
- The League Cup ran in 2002, was not held in 2003, ran 2004 to 2011, and ended in 2012.
- The Super Cup ran 2002 to 2006, stopped in 2007 and came back in 2026.
- The Korean FA Cup became the Korea Cup in 2024. Its finals were two-legged from 2016 to 2022.
- Foreign players: from 3 on the pitch with 7 registered in 2002, down to 3 and 3 (2006 to 2008), then 3 plus 1 AFC (2009), plus 1 ASEAN (2020). No limit on registration from 2026, with a matchday limit of 5 in K League 1 and 4 in K League 2.
- Substitutes: 3 to 2020. From 2021, 5 if U-22 players are used. In K League 1 from 2026, 5 without that condition. Bench: 7 in 2023, 9 from 2024 in K League 1, and 9 from 2026 in K League 2.

## Inferences (marked in the table)

- NSL: no relegation in 2001/02 to 2003/04.
- A-League: 3 substitutes before the July 2020 change. A bench of 5 between 2013/14 and 2025/26 (the source dates are 2013 and 2025). 5 visa players between 2008 and 2025 (the sources date from 2008 to 2013 and from 2025).
- Australian Championship 2026/27 and 2027/28: same format as 2025.
- Japan: 3 foreign players before 2008. 5 substitutes in 2021 to 2023 (the sources give 2020 and 2024).
- Korea: the 2013 split of top 7 and bottom 7 (from 14 clubs and 38 matches). A K League 2 bench of 7 before 2025. The Super Cup in 2027 and 2028.
- Club name matches across a rename or a move: Gimcheon Sangmu to Sangju Sangmu, Sangju Sangmu Phoenix to Gwangju Sangmu Bulsajo, Jeju United to Bucheon SK (2005/06 database), FC Seoul to Anyang LG Cheetahs, Suwon FC to Suwon City, Reilac Shiga to MIO Biwako Shiga, Paju Frontier to Paju Citizen, Korean Police to National Police Agency.

## Unknowns

- Transfer windows and work permits in all three nations. No page that I read gave them by season.
- Substitutes and bench in the NSL and in the A-League after 2019/20 (substitutes) and before 2013/14 (bench source from 2013).
- Japan and Korea bench sizes before 2023 or 2024. The sources state only the size before each change.
- Substitutes and bench in the Emperor's Cup and the Korean FA Cup. These are FA competitions, so the league rules may not apply.
- Entrant counts of the Emperor's Cup (except 2005 to 2007 and 2026/27) and of the Korean FA Cup.
- The 2027/28 season in all three nations (2028 in Korea), the K League 2 size in 2027 ("15 or 16") and the 2027 Korean promotion and relegation rules. The KFA plans one combined system from 2027.
- The Japanese Super Cup for the autumn-spring seasons.
- The rules of foreign players in the NSL and in the first three A-League seasons.

## Club moves

The databases hold J1 with 16 clubs, J2 with 12, the NSL division with 13 and the Korean League with 10 in every season. No database holds J3, K League 2 or the Australian Championship. So every season start lists the real clubs of these divisions in the clubs file:

- J1 and J2: from 2005/06. They are equal to the database counts in 2001/02 to 2003/04.
- J3: from 2013/14.
- K League: from 2002/03.
- K League 2: from 2012/13.
- A-League: 2005/06 to 2023/24 and 2025/26. In 2024/25 the count is 13 and equal to the database.
- Australian Championship: 2025/26.

The 2001/02 to 2003/04 rows use the databases of those seasons (3.9.68 and October 2003). The 2004/05 and 2006/07 seasons have no database.

### Real clubs that no database club matches

| Season | Division | Clubs |
|---|---|---|
| 2002/03 | K League | Daegu FC |
| 2003/04 | K League | Daegu FC, Incheon United |
| 2005/06 | K League | Incheon United, Daegu FC, Gyeongnam FC |
| 2005/06 | A-League | Central Coast Mariners, Queensland Roar |
| 2005/06 | J2 League | Tokushima Vortis |
| 2007/08 | J2 League | Roasso Kumamoto |
| 2008/09 | K League | Gangwon FC |
| 2009/10 | K League | Gangwon FC |
| 2010/11 | K League | Gwangju FC |
| 2011/12 | J2 League | Matsumoto Yamaga |
| 2012/13 | K League 2 | FC Anyang, Goyang Hi FC |
| 2013/14 | J3 League | J.League U-22 Selection |
| 2013/14 | K League 2 | Ansan Police |
| 2014/15 | J3 League | J.League U-22 Selection |
| 2014/15 | K League 2 | Seoul E-Land, Ansan Police |
| 2015/16 | J3 League | Kagoshima United, Gamba Osaka U-23, FC Tokyo U-23, Cerezo Osaka U-23 |
| 2016/17 | J3 League | Kagoshima United, FC Tokyo U-23, Cerezo Osaka U-23, Gamba Osaka U-23 |
| 2016/17 | K League 2 | Asan Mugunghwa, Ansan Greeners |
| 2018/19 | J3 League | Cerezo Osaka U-23, FC Tokyo U-23, Gamba Osaka U-23 |
| 2019/20 | J3 League | Gamba Osaka U-23, Cerezo Osaka U-23 |
| 2019/20 | K League 2 | Chungnam Asan |
| 2025/26 | J2 League | Iwaki FC |
| 2025/26 | K League 2 | Yongin FC |

Max decides these cases. No club records were added.

### Question for Max on Q244

The rule takes clubs for a short division only from clubs with no division. In these databases, most real J2, J3, K League and K League 2 clubs sit in other divisions, not in "none":

- "A Premier Division" and "A Lower Division": spare divisions in which the Update Team parks real league clubs beyond the 16, 12 or 10 places.
- The Japanese Football League, the Japanese Administrative Division and the Korean Amateur League.
- The Australian state leagues, for the Australian Championship clubs.

The clubs file names the database division of each club in `db_division`. For example, 20 of the 20 J3 clubs of 2025/26 and 15 of the 17 K League 2 clubs sit in such divisions. Max decides whether a club move may take clubs from these divisions. If the answer is no, these divisions lack clubs in nearly every season.

## Code notes

- The original's foreign limits (Japan 3, South Korea 3, Australia 6) come from `2026-10-16-season-change-points.md`. The real values are in the `nation rules` rows.
- In 2002, J1 league matches still had golden-goal extra time with 3, 2 and 1 points (J2 dropped it in 2002 and J1 in 2003).
- The Japanese 2026 half-season uses regional groups and same-rank play-offs. Penalty shoot-outs settle draws, and no club goes up or down. The original has no code for this format.

## Reconcile

Date: 2026-10-16. File: `aus-jap-kor.reconcile.tsv` (39 rows). Third sources: the A-Leagues site, the J.League data site and press release, RSSSF (for the Emperor's Cup 2020 and the club renames), Dailian, Kyunghyang, The Fact, Sidae Ilbo, OSEN, Xportsnews and Kyeonggi Ilbo. For the Emperor's Cup 2002 and 2003 I found no third publisher. The JFA archive redirects and worldfootball.net refused the request. I read the full RSSSF match lists as the primary record.

| Decision | Rows |
|---|---|
| first | 1 (plus 5 club matches confirmed) |
| second | 17 |
| conflict | 1 |
| found accepted | 15 |

### Changes to aus-jap-kor.tsv

- All 16 disagreements go to the second value (rows 42, 43, 44, 136, 137, 154, 178, 181, 185, 321, 411, 423, 428, 429, 438, 439). The 2020/21 A-League bench (row 43) is an inference between the 2019 and 2022 rule texts.
- Emperor's Cup entrants: 80 in 2002 to 2004 and 88 in 2012 to 2019, 2022, 2023 and 2024. 2020 has 52 and 2021 has 90. The format text of rows 136 to 158 now gives the round in which each group enters. The check note on 2015 and 2016 was incomplete. In those years the J2 clubs and the two promoted J1 clubs played round 1, 12 J1 clubs entered in round 2 and the 4 ACL clubs in round 4. Rows 142 to 145 (2008 to 2011) still have no count.
- J.League Cup bench: 7 up to 2023 and 9 in 2024 (J.League press release). Row 184 is now 7. Rows 163 to 183 now say "unknown (7 in 2023)".
- Row 262 (J3 2020): clubs 18. The withdrawal of FC Tokyo U-23 on 5 June 2020, before any match, is in notes. The club is out of the 2019/20 J3 list in `aus-jap-kor-clubs.tsv`, which now has 18 rows for that season.
- Rule for `up`/`down`: row 335 keeps 2, now given as 15th and 16th, with Sangju Sangmu in notes. Row 343 changes from 2 to 1 (12th), with Sangju Sangmu in notes.

### Club matches

All five matches that had no second source are now confirmed: Jeju United and Bucheon SK, Suwon FC and Suwon City, Korean Police and National Police Agency, Paju Frontier and Paju Citizen, Reilac Shiga and MIO Biwako Shiga. No change to the clubs file was needed for them.

One finding for Max: database club 1962 is "Bucheon SK" in the 2002 to 2005 databases and "Bucheon FC 1995" in the later ones. Bucheon FC 1995 is a different club from Bucheon SK. The clubs file matches by the name in each database, so Jeju United uses 1962 only in 2005/06 and 9323 or 13040 later.

### Conflicts for Max

1. **aus-jap-kor-343, K League 1 2020, `down`.** First value 2 (12th and Sangju Sangmu), second check agreed. Under the brief default the value is 1, and Sangju Sangmu (4th) is in notes. RSSSF says Sangju "will be relegated regardless of its final position" because of its move, and the league fixed this before the season. If a drop fixed in advance counts as a rule of the season, the value is 2.

### Other findings

- Rows 45 to 47 (A-League 2022/23 to 2024/25) keep a bench of 5. The U-23 rule of 2019 to 2022 may still apply there. I did not check it.
- Row 430 (Korea Cup 2026) copies the 2024 text without the two-legged semi-finals. I did not check the 2026 format.

## Loose ends

- aus-jap-kor-45 to 47 (A-League Men 2022/23 to 2024/25): the competition regulations were not found. ESPN match rosters (about 40 sampled matches per season) show 7 substitutes named and up to 5 used per team, with two team-matches of 6 used (likely concussion substitutes). `bench` 7 and `subs_used` 5 are inferences from these data. The U-23 rule of October 2019 (6 or 7 substitutes when enough eligible U-23 players are named) is the rule source.
- aus-jap-kor-430 (Korea Cup, year 2026): the KFA changed the cup to a summer-to-spring calendar. The 2026–27 edition (63 clubs, K League 1 to K5, every tie a single match, 5 substitutes plus concussion substitutes) runs from 20 June 2026 to the final on 5 June 2027 (KFA, 21 May 2026). The row now holds it. Row 431 (year 2027) is labelled "2027" and probably holds the 2027–28 edition: Max decides the label.


## Source fill

Method: the lint of 2026-10-16 listed each filled fact field without a `field=URL` entry. For each one, I read the pages the row already cites today (curl, text search for the value). If no cited page states the value, I read the general page of the competition. I added `field=URL` to `sources` only where the page text holds the value. I changed no values.

Sources added: 49 fields (name 49).

- `name` = none on not-held rows cites the page that gives the dates of the competition: https://en.wikipedia.org/wiki/K_League_2 (founded 2013) for aus-jap-kor-352 to 362, https://en.wikipedia.org/wiki/Korean_League_Cup (abolished 2012, last edition 2011) for aus-jap-kor-443 to 459, and https://en.wikipedia.org/wiki/2026–27_J.League_Cup (previous edition 2025, next 2027-28) for aus-jap-kor-187.

Disagreements: none. Fields with no source: none.
