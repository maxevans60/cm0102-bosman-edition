# Rules table part nor-fin: Norway and Finland

Slice 6 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Files:

- `nor-fin.tsv`: 270 rows, 10 per season for 2001/02 to 2027/28.
- `nor-fin-clubs.tsv`: 644 club rows for the divisions whose real club count differs from the season database.
- This note.

## Method

1. Both nations play in calendar years. Per the brief, the 2001/02 row holds the year 2002, and so on to 2027/28 (year 2028). `real_season` holds the year.
2. Cups that do not follow the calendar year use the edition that starts in the row's year or carries the row's split label. The 2021 and 2022 Norwegian cups ended in May of the next year. The Norwegian cup changed to autumn to spring in 2025-26, so the 2024/25 row holds the 2025 cup and the 2025/26 row holds the 2025-26 cup. These two editions overlap from August to December 2025. The Finnish cups of 2016-17 and 2017-18 ran from July to September of the next year, and they sit in the 2016/17 and 2017/18 rows.
3. I read every season page of English Wikipedia as raw wikitext on 2026-10-16 and took the club counts, places and play-offs from the result keys of the league tables. Main pages gave the names, sponsor names and cup history.
4. Scope: the six competitions of the original (`nor_prm`, `nor_first`, `nor_cup`, `fin_prm`, `fin_first`, `fin_cup`). I added two competitions that the original lacks, with `comp_file` = `new`:
   - the Norwegian super cup (Superfinalen 2009 and 2010, Mesterfinalen 2017 and 2018)
   - the Finnish League Cup (Liigacup).
5. `fin_first` follows tier 2. That is the Ykkönen to 2023 and the Ykkösliiga from 2024. From 2024 the Ykkönen is tier 3 with 12 clubs. Tier 3 is out of scope, so it has no rows.
6. Rows for 2027 and 2028 carry the 2026 format forward. No page read today announces a change. Each such row says so in `notes`.

## Main findings

### Norway
- Top flight: 14 clubs to 2008, 16 from 2009. Name Tippeligaen to 2016, Eliteserien from 2017.
- 2008 was the transition season: 1 relegated, 13th in the play-off, 3 promoted from the 1. divisjon.
- Relegation play-off: 12th v 3rd (2002 to 2007), 13th v 4th (2008), a play-off with the 1. divisjon 3rd to 5th (2009, 2010), none in 2011, 14th v the winner of the 1. divisjon 3rd to 6th play-off from 2012. It was one match on neutral ground in 2020 and 2021 and two legs in the other seasons.
- 1. divisjon: 16 clubs in every season. Sponsor names Adeccoligaen (2005 to 2013) and OBOS-ligaen (from 2015). Four relegated until 2016, then 2 relegated plus a 14th-place play-off against the 2. divisjon from 2017.
- Cup: 128 clubs in the competition proper, single matches with extra time and penalties. All top-flight clubs enter in round 1. There was no men's cup in 2020 (COVID-19). In 2025-26 the Eliteserien clubs entered in round 3.
- Substitutes: the cup final pages give 7 named and 3 used to 2019, and 9 named and 5 used from 2021. The league rows use the same values as an inference. 2020 is unknown.

### Finland
- Veikkausliiga: 12 clubs in 2002, with a first stage and then an 8-club championship group and an 8-club promotion/relegation group with the Ykkönen top 4. 14 clubs from 2003 to 2010, but 13 in 2006 (licence refusal). 12 clubs from 2011.
- Matches: double round robin to 2010, triple round robin (33 matches) from 2011 to 2018.
- Split: 2019 and 2020 used a Championship Series and a Challenger Series. In 2020 the split was cancelled after 22 rounds. From 2021 to 2024 the top 6 and bottom 6 each played 5 more matches. From 2025 the top 6 play a double round (32 matches) and the bottom 6 a single round (27 matches).
- Relegation: 1 direct place in every season. The relegation play-off ran from 2003 to 2010 (13th) and from 2015 on (11th). There was none from 2011 to 2014.
- Tier 2 sizes: two zones of 9 (2002), then 14 (2003 to 2010), 13 (2011), 10 (2012 to 2019), 12 with a split (2020 to 2023). The Ykkösliiga has had 10 clubs and a triple round robin since 2024.
- Finnish Cup: a knockout to 2016. Group stages for the top clubs ran from 2016-17 to 2021. Knockout only again from 2022.
- League Cup: not played 2002 and 2003. Played 2004 to 2016, abolished after 2016, revived from 2022.

## Club moves

The database counts are the same in every season database: Norwegian Premier 14, Norwegian First 16, Finnish Premier 12, Finnish First Division North 10 and South 10. The clubs file lists every real club where the counts differ:

- Tippeligaen/Eliteserien: every season with a database from 2008/09 (year 2009) to 2025/26. Real 16, database 14.
- Veikkausliiga: 2002/03, 2003/04, 2005/06, 2007/08, 2008/09 and 2009/10. Real 14 (13 in 2006), database 12.
- Finnish tier 2: every season with a database. Two zones of 9 in 2002, then a single division that the database does not have.

Seasons with no database (2004/05, 2006/07, 2026/27, 2027/28) have no club rows. I matched clubs by name per season database, with an alias list. The database names vary between versions (for example "Tromsö IL", "Valerenga Oslo", "FC TPS", "HJK Reserves" for Klubi-04 in September 2005).

## Missing clubs

None. Every real club of a listed division has a record in its season database. These cases need Max's decision:

- Inference: a move into a larger division takes clubs out of the division below. For example, the two extra Eliteserien clubs come from the database's First Division, which then holds 14, not 16. The same knock-on applies to the Finnish Second Division groups. Max decides whether the build lists the lower division too.
- Some real clubs sit in a low or unexpected database division, for example AC Oulu in "none" (Autumn 2013) and Sandefjord in a Second Division group (September 2009). See `db_division`.
- The 2002 zones: the database has 10 and 10, the real zones 9 and 9.

## Unknowns

- All nation rules rows (foreign players, work permits, squad registration, transfer windows) for both nations. I found no page with these facts. The original sets 2 non-EU for Norway [code]. Transfer list pages on Wikipedia do not give the window dates.
- Finnish substitutes and bench from 2020. Values to 2019 are an inference from the 2006 and 2013 cup final line-ups (7 named).
- Norwegian substitutes in 2020. Super cup and League Cup substitutes in every season.
- Finnish Cup pages for 2005 and 2007 are missing. Their format is an inference from 2004 and 2006.
- League Cup format by year, except 2008, 2012 and 2016.
- Whether the 2020 Ykkönen promotion and relegation series were played.

## Sources

The table cites each page in `sources`. Main pages: https://en.wikipedia.org/wiki/Eliteserien, https://en.wikipedia.org/wiki/Norwegian_First_Division, https://en.wikipedia.org/wiki/Norwegian_Football_Cup, https://en.wikipedia.org/wiki/Mesterfinalen, https://en.wikipedia.org/wiki/Veikkausliiga, https://en.wikipedia.org/wiki/Finnish_Cup, https://en.wikipedia.org/wiki/Finnish_League_Cup. The second-source check should use RSSSF, fotball.no, palloliitto.fi or veikkausliiga.com.

## Reconcile

Reconcile date 2026-10-16. The full list is in `nor-fin.reconcile.tsv`. I changed 59 fields in `nor-fin.tsv` and added each URL to `sources`. `nor-fin-clubs.tsv` needed no change: the check found no club-match error.

| Decision | Fields |
|---|---|
| first | 0 |
| second | 16 |
| conflict | 3 |
| found value accepted | 35 |
| found value rejected | 0 |
| rows that copy a changed row | 8 |

How I decided:

- 2002 Finnish relegation (nor-fin-7): the full group table shows VPS 7th of 8 and relegated. The bottom 2 of the group go down, and only one of them was a Premier club.
- League Cup entry, 2004 and 2005 (nor-fin-30, 40): no third publisher. The group tables give 16 clubs, all Veikkausliiga clubs plus the top 2 of the Ykkönen.
- Norwegian play-off, 2009 and 2010 (nor-fin-72, 82): Dagbladet and FotMob confirm single-match semi-finals. The final had two legs.
- Finnish Cup 2017-18 (nor-fin-169): Yle gives the final on 12 May 2018. Method point 2 above is wrong for 2017-18. The 2016-17 cup did end in September.
- 2019 Veikkausliiga split (nor-fin-177): the HIFK club site says the 7th and the 8th both went to the play-off because cup winners Ilves finished 4th.
- 2020 Ykkönen (nor-fin-188): Kaleva says the season ended after the regular rounds. Yle reports KTP v TPS over two legs. The open question under "Unknowns" is now answered: no series were played.
- League Cup 2022 to 2026 (nor-fin-210 to 250): the organiser's announcements on veikkausliiga.com give the format. Yle confirms the 2022 final date. I found no third publisher for the groups.
- 2023 Ykkönen relegation (nor-fin-218): the Palloliitto rule text says three go down.
- Substitutes and bench, 2020 to 2026: I accepted the Transfermarkt counts only as "(observed)" values. They show a lower limit, not the written rule. The 2027 and 2028 rows carry the 2026 values forward, marked as an inference.

Conflicts:

1. nor-fin-2 and nor-fin-4, 2002 Norwegian bench. Wikipedia lists 7 Vålerenga substitutes in the 2002 cup final. RSSSF lists 8 and adds Jonas Krogstad. I found no third publisher. The value stays 7. If RSSSF is right, the rule allowed at least 8, and the league row inferred from it changes too.
2. nor-fin-5, 2002 Norwegian super cup. Both sources agree that no official edition took place. RSSSF lists an unofficial match, Rosenborg 2-0 Viking on 7 April 2002. The format stays "not held" and the match is in `notes`. Max decides whether the game plays it.

Items for Max that the reconcile did not change:

- League Cup rows for 2006, 2007, 2009 to 2011 and 2013 to 2015 still say "Veikkausliiga clubs". The 2004 and 2005 rules also admitted 2 Ykkönen clubs. Nobody checked the other years.
- The League Cup rows for 2027 and 2028 are still `unknown`.

## Loose ends

- Finnish League Cup rows nor-fin-50, 60, 80, 90, 100, 120, 130, 140 (2006, 2007, 2009 to 2011, 2013 to 2015): no Ykkönen club took part. RSSSF (2006, 2007, 2010) and Finnish Wikipedia (2009, 2011, 2013 to 2015) list only the Veikkausliiga clubs (13 in 2006, 14 in 2007 to 2011, 12 in 2013 to 2015). The `clubs` field now gives the count, and `format` gives the groups and the knockout rounds from the same pages.


## Source fill

Method: the lint of 2026-10-16 listed each filled fact field without a `field=URL` entry. For each one, I read the pages the row already cites today (curl, text search for the value). If no cited page states the value, I read the general page of the competition. I added `field=URL` to `sources` only where the page text holds the value. I changed no values.

Sources added: 82 fields (clubs 8, name 74).

- "Suomen Cup" and "Liigacup" cite the season page or https://en.wikipedia.org/wiki/Finnish_League_Cup, which give the Finnish name.
- "Norgesmesterskapet (Norwegian Football Cup)" counts as stated when the page holds both names.
- Liigacup `clubs` = "Veikkausliiga clubs" cites the season page or https://en.wikipedia.org/wiki/Finnish_League_Cup, which states that teams from the Veikkausliiga contest the cup.

Disagreements: none. Fields with no source: none.
