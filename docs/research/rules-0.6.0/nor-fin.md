# Rules table part nor-fin: Norway and Finland

Slice 6 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Files:

- `nor-fin.tsv`: 270 rows, 10 per season for 2001/02 to 2027/28.
- `nor-fin-clubs.tsv`: 1112 club rows for the divisions whose real club count differs from the season database (644 from pass 1, 468 added in pass 2).
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


## Pass 2

Date 2026-10-17. Decisions Q268 to Q331 of the spec apply. Row ids and row order stay the same. `rules_lint.py` gives 0 problems before and after.

### No new tiers and no new nations (Q331, Q288)

The original runs two tiers for each nation (`nor_prm`, `nor_first`, `fin_prm`, `fin_first`). The part has no rows below tier 2, and no row needs a nation that a game cannot select. The Norwegian Super Cup and the Finnish League Cup are cups of the existing tiers, so they stay.

- Rows deleted: 0.
- Club rows deleted: 0.

### Conflicts and open points

1. nor-fin-2 and nor-fin-4, 2002 Norwegian bench: settled as 7. A third publisher, the TV 2 match report on altomfotball.no, lists 7 Vålerenga substitutes on the bench and no Jonas Krogstad. Its Odd list agrees with the 7 of Wikipedia. URL: https://web.archive.org/web/20230426040929/https://www.altomfotball.no/element.do?cmd=match&matchId=45940&tournamentId=110&seasonId=324&useFullUrl=false
2. nor-fin-5, 2002 Norwegian Super Cup: settled as `not held`. In real life no official edition took place. The Rosenborg v Viking match of 7 April 2002 was a friendly without official status, so the game does not play it. URL: https://www.rsssf.org/tablesn/noosupcuphist.html
3. League Cup entry in 2006, 2007, 2009 to 2011 and 2013 to 2015: already settled under "Loose ends" (only Veikkausliiga clubs). No change.
4. nor-fin-260 and nor-fin-270, League Cup 2027 and 2028: nothing has happened yet. The rows now carry the 2026 format and entry forward as an inference, as all other 2027 and 2028 rows do. URL: https://www.veikkausliiga.com/uutiset/2025/12/16/liigacupin-2026-lohkovaiheen-alustava-ohjelma-valmis-tasan-kuukausi-alkuun
5. Knock-on of club moves ("Missing clubs" above): settled by the club moves below (Q279, Q280).
6. The 2002 Ykkönen zones: real life had 9 and 9 clubs. The database has 10 and 10. The pass 1 list of 9 and 9 stays. The 2 extra database clubs become clubs with no division.
7. Clubs in low database divisions (AC Oulu in "none", Sandefjord in a Second Division group): Q279 allows the club moves to take them. No change.

Conflicts settled: 2 (nor-fin-2 and nor-fin-4 count as one conflict). Open points settled: 5.

### Club moves (Q279, Q280, Q290)

Pass 1 listed each division whose real count differs from the database. A listed division also takes clubs from the tier next to it. These clubs leave a gap in a division whose count is correct, so pass 2 lists that division too.

- Norwegian 1. divisjon, 2008/09 to 2025/26 (18 seasons, 16 clubs each, 288 rows). Each season the two extra Eliteserien clubs come from the database First Division. Source: the season page `https://en.wikipedia.org/wiki/<year>_Norwegian_First_Division`.
- Veikkausliiga, 2010/11 to 2015/16 and 2017/18 to 2025/26 (15 seasons, 12 clubs each, 180 rows). In these seasons the Ykkönen list takes 1 or 2 clubs from the database Premier Division. 2016/17 needs no list, because its Ykkönen list takes no Premier club. Source: `https://en.wikipedia.org/wiki/<year>_Veikkausliiga`.

Club rows added: 468. Every real club has a record in its season database. No club appears twice in a season. Matches by database name, checked by hand where a club changed its name:

- Sarpsborg 08 (2008/09, 2009/10): record 6496, named "Sparta Sarpsborg" in those databases. Later databases give the same record the name "Sarpsborg 08". In September 2008 it sits in a Second Division group.
- Øygarden (2019/20): record 7857, named "Nest-Sotra IL" in October 2019. Øygarden FK took over the elite licence of Nest-Sotra in 2020 (https://en.wikipedia.org/wiki/Øygarden_FK), so the Nest-Sotra record takes the place.
- Lyn (2009/10): record 6864, "FC Lyn Oslo". The real club withdrew on 7 July 2010 after bankruptcy (season page). By Q272 the club keeps its place in the list.
- Lyn (2023/24 to 2025/26): record 6864, "Lyn 1896 FK", not Gjøvik-Lyn.
- Some clubs have a new record id in later databases (for example Sandnes Ulf, Skeid, Sogndal, Start, Stabæk, SJK, TPS, VPS). Each row gives the id of its own season database.

Seasons with no database (2004/05, 2006/07) and the seasons after 2025/26 have no club rows. Norway from 2002/03 to 2007/08 and Finland in 2001/02 and 2016/17 need no new list: each count agrees and no listed division takes a club from the other tier.

### Reserve and B teams (Q289)

The tiers of this part need two reserve teams: Klubi 04 (HJK) and SJK Akatemia. Each season database that needs them has a record ("HJK Reserves", "Klubi-04 Helsinki", "SJK Seinäjoki II"). Norway allows no reserve team in its top two tiers. "Atlantis Akatemia" and "KPV juniorit" are older database names of the first teams Atlantis FC and KPV, not reserve teams.

Reserve teams that the build must add: 0.

## Second source, pass 2

Date 2026-10-17. The check file holds 558 lines with verdict `no second source`. 388 of them hold `unknown` or an inference, so I did not check them again. I took the other 170 lines and wrote the result in `nor-fin.check2.tsv`. `rules_lint.py` gives 0 problems before and after.

| Verdict | Lines |
|---|---|
| agree | 58 |
| disagree, settled | 1 |
| disagree, still split | 0 |
| no second source | 111 |

Agree:

- Finnish League Cup: 2002 and 2003 not held, and the name, entry and format of 2008 to 2016 (RSSSF and the Finnish Football Archive). The 2014 groups were 4 groups of 3, but the archive does not say that they were by geography. The 2023 name agrees with the organiser's page.
- Norwegian 1. divisjon names: Adeccoligaen from 2005 to 2013 (Kampanje, February 2014), and no sponsor name in 2014 (Lokalhistoriewiki, with the Aftenposten report of 15 January 2015).
- Norwegian Cup substitutes and bench in 2016, 2019, 2023, 2024 and 2025-26 (NFF cup rules of those years, read on the Wayback Machine). The 2019 rules also allowed a 4th substitution in extra time.
- Norwegian 1. divisjon 2026: every field (NFF OBOS-ligaen rules 2026).
- Ykkösliiga 2026: every field (the Finnish players' association JPY).
- Norwegian Cup 2026-27 format (NFF NM Menn 2027 rules). The rules do not give the dates.

Change to `nor-fin.tsv`:

- nor-fin-249, Finnish Cup 2026, `format`. The first source said that the clubs of the top three tiers enter in round 3, with a regional draw to round 5. Flashscore says the top four tiers enter in round 3 and the 4 clubs in European competition in round 5. The third source is the primary record, the Palloliitto cup rules for 2026. They agree with Flashscore and give geographic draw pots to round 4 and an open draw from round 5. Decision: second. I wrote the rule-text format, with extra time and penalties, and replaced the `format` sources. URL: https://www-assets.palloliitto.fi/62562/1768387037-miesten-suomen-cup-ja-regions-cup-roots-saannot-2026.pdf
- The 2027 and 2028 Finnish Cup rows hold `unknown`, so no row copies the old text. nor-fin-239 (2025) has the same wording as the old 2026 text. The check of pass 1 covered it, and I did not change it. Max may want it checked against the 2025 cup rules.

No second source (111 lines):

- Norwegian Cup bench and substitutes in the other years (26 lines). RSSSF gives no line-ups after 2005, and the Wayback Machine holds no NFF cup rules for those years.
- Finnish Cup bench, 2002 to 2019 (18 lines). No other publisher gives unused substitutes. Every year except 2013 carries the 2013 count forward, which is an inference in substance.
- Norwegian Super Cup `not held`, 2020 to 2026 (7 lines). The RSSSF list ends in 2019.
- Finnish League Cup 2023 entry (1 line).
- Rows for 2027 and 2028 (59 lines). Each carries the 2026 value forward as an inference, so I did not check them.

## Closing

Date: 2026-10-17. Closing agent. `rules_lint.py`: 0 problems before and after.

- nor-fin-239 (Finnish Cup 2025), `format`: the old text said that the clubs of the top three tiers enter in round 3. The round table of the 2025 season page (https://en.wikipedia.org/wiki/2025_Finnish_Cup, read on 2026-10-17) shows that the 8 Veikkausliiga clubs not in UEFA competition, the 10 Ykkösliiga clubs and the entered Ykkönen and Kakkonen clubs enter in round 3, and the 4 Veikkausliiga clubs in UEFA competition enter in round 5. Lower-tier clubs play at home to round 4, and the draw is in geographic groups until round 5. I wrote this. I found no Palloliitto cup rules for 2025 (two searches and the archive), so the row is not checked against a rule text.

## Pass 3 (Q337)

Q337 (2026-10-17): a real play-off against a tier that is not in the game is played against a club with no division, as for the Scottish pyramid play-off (Q284). The rows below keep the real format in `playoffs`. Each row gets this note: "Q337: the lower-tier side is a club with no division in the game." No other field changed.

Rows: 153, 163, 173, 183, 193, 203, 213, 223, 233, 243, 253, 263 (Norwegian First Division 14th against the 2. divisjon play-off winner), 8, 18, 28, 38 (Finnish First Division against Kakkonen clubs), 228, 238, 248, 258, 268 (Ykkösliiga 9th against the Ykkönen 2nd).
