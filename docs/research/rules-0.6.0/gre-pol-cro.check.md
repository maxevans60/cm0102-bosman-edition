# Second-source check of the Greece, Poland and Croatia rules table

Check of part `gre-pol-cro` (`gre-pol-cro.tsv`, 486 rows). Check date 2026-10-16. The first agent's files are unchanged.

## Method

- I checked every filled fact field of every row. A field with `n/a`, a `see nation rules` pointer and the nation label of a `nation rules` row get no check row. The result is 2653 rows in `gre-pol-cro.check.tsv`.
- The first source of nearly every league and cup fact is English Wikipedia. For these I used RSSSF season pages, read raw with `curl` and parsed by script: `tablesg/grkNN.html` and `grkcupNN.html` (Greece), `tablesp/polNN.html` (Poland), `tablesk/kroaNN.html` (Croatia), plus the RSSSF super cup lists for each nation and `grk2hist.html` for the Greek second level of 2001/02. I compared club counts, matches played, table marks (promoted, relegated, play-off), split stages, notes on licences and withdrawals, cup round headings and leg dates.
- The first source of the Polish second level 2002/03 to 2006/07 is RSSSF. For these rows I used 90minut.pl league pages and Polish Wikipedia season pages (a different publisher from RSSSF). The 90minut.pl season archive pages also gave the league names and the seasons with no league cup. The 90minut.pl league cup pages gave the 2001/02 and 2006/07 to 2008/09 cup formats.
- Greek league names: footballhistory.org (rename to Super League in 2006; Beta Ethniki to Football League in 2010) and the RSSSF headings.
- Substitutes: IFAB circular 18 (May 2020) quotes Law 3 before the change: three substitutes at most for top-division club competitions. For second tiers, Law 3 lets the association allow up to five, so I give `agree` only where a national rule says three (HNS competition rules of 2020, article 35, for the Croatian second level). Every value of 5 got `no second source`: the IFAB news of 8 May 2020 gives an option to organisers and does not show that each league used it.
- Nation rules: FIFA RSTP (August 2021 and March 2023 editions) for the registration periods; insideworldfootball (27 Nov 2013), meczyki.pl (2019), polsatsport.pl (5 Feb 2025) and pzpn.pl (20 May 2025) for Polish foreign and youth rules; dalmacijanews.hr (12 Mar 2026) for the Croatian non-EU limit; sportmonks.com (17 Dec 2025) for the Greek limit.
- worldfootball.net, fbref.com and slgr.gr block scripted reads (HTTP 403). I did not use them. RSSSF does not list every early cup round, so many cup entrant counts stay unconfirmed.
- Rows of 2026/27 and 2027/28 got `no second source` for every fact that depends on matches not yet played. Where a page already fixes a fact (for example the Croatian second level grows to 16 in 2026/27), I checked it.

## Counts per verdict

| Verdict | Rows |
|---|---|
| agree | 1783 |
| no second source | 823 |
| found | 19 |
| disagree | 28 |

Most `no second source` rows are rule facts with one publisher only: bench sizes (all unknown but one), work permits and squad registration (all unknown), most foreign-player values, every substitute value of 5, the second-tier substitute value of 3 in Greece and Poland, cup entrant counts that RSSSF does not show, and the 2026/27 and 2027/28 values.

## Disagreements

First value and first URL are from `gre-pol-cro.tsv`. Second value and URL are from my check.

1. **gre-pol-cro-6 format** (Greek Football Cup, 2006/07). First: "first round, second round, additional round and round of 32 single matches; round of 16 single match with replay; quarter-finals and semi-finals two legs; final single match", from https://en.wikipedia.org/wiki/2006–07_Greek_Football_Cup. Second: RSSSF 2006/07 shows the 1/8 finals played over two legs (First Legs Dec 19, Second Legs), not a single match with replay; round 3, quarter-finals, semi-finals and final agree. Source: https://www.rsssf.org/tablesg/grkcup07.html
2. **gre-pol-cro-66 down** (Football League, 2012/13). First: "3 (bottom three)", from https://en.wikipedia.org/wiki/2012–13_Football_League_(Greece). Second: RSSSF marks only 20th and 21st as relegated (19th not); 8th and 13th demoted. First value: 3 (bottom three). Source: https://www.rsssf.org/tablesg/grk2013.html
3. **gre-pol-cro-160 foreign** (Greece, 2025/26). First: "Super League 1: max 5 non-EU players on the field at once; squad cap of non-EU players over 23 was 6 before July 2026", from https://www.insideworldfootball.com/2026/07/06/greek-super-league-increases-squad-cap-on-foreign-players-to-7/. Second: Sportmonks (17 Dec 2025): limit of seven non-EU players per squad in 2025/26; the first value says the over-23 cap was 6 before July 2026. The page does not give the age split, so the difference may be in scope. Source: https://www.sportmonks.com/glossary/greek-super-league-1-greece/
4. **gre-pol-cro-166 format** (Polish Cup, 2004/05). First: "single-match rounds to the round of 16; quarter-finals and semi-finals two legs; final two legs", from https://en.wikipedia.org/wiki/2004–05_Polish_Cup. Second: RSSSF 2004/05: preliminary round, round 1, then a group stage (rounds 1-6 in groups of 4), 1/8 finals over two legs, quarter-finals, semi-finals and final over two legs. First value: single-match rounds to the round of 16. Source: https://www.rsssf.org/tablesp/pol05.html
5. **gre-pol-cro-167 format** (Polish Cup, 2005/06). First: "single-match rounds to the round of 16; quarter-finals and semi-finals two legs; final two legs", from https://en.wikipedia.org/wiki/2005–06_Polish_Cup. Second: RSSSF 2005/06: qualifying round and first round single, second round and 1/8 finals over two legs (First Legs / Second Legs), then quarter-finals, semi-finals and final over two legs. First value: single-match rounds to the round of 16. Source: https://www.rsssf.org/tablesp/pol06.html
6. **gre-pol-cro-197 playoffs** (Ekstraklasa, 2008/09). First: "none", from https://en.wikipedia.org/wiki/2008–09_Ekstraklasa. Second: RSSSF 2008/09: 14th Cracovia was set for a promotion/relegation play-off against I liga 3rd Korona; it was cancelled after a protest. First value: none. Source: https://www.rsssf.org/tablesp/pol09.html
7. **gre-pol-cro-249 up** (II liga, 2006/07). First: "4 (1st-4th)", from https://www.rsssf.org/tablesp/pol07.html. Second: pl.wikipedia: planned 2 promoted (1st-2nd) with 3rd to a play-off; 3rd and 4th went up directly only after corruption demotions. First value: 4 (1st-4th). Source: https://pl.wikipedia.org/wiki/II_liga_polska_w_pi%C5%82ce_no%C5%BCnej_(2006/2007)
8. **gre-pol-cro-249 playoffs** (II liga, 2006/07). First: "14th-17th: relegation play-offs", from https://www.rsssf.org/tablesp/pol07.html. Second: pl.wikipedia: planned 3rd v 14th of the top level and 11th to 14th relegation play-offs; RSSSF shows 14th to 17th in the play-offs after the changes. First value: 14th-17th relegation play-offs. Source: https://pl.wikipedia.org/wiki/II_liga_polska_w_pi%C5%82ce_no%C5%BCnej_(2006/2007)
9. **gre-pol-cro-254 down** (I liga, 2011/12). First: "4 (15th-18th)", from https://en.wikipedia.org/wiki/I_liga. Second: RSSSF 2011/12: line after 15th; 16th to 18th relegated. First value: 4 (15th-18th). Source: https://www.rsssf.org/tablesp/pol2012.html
10. **gre-pol-cro-361 down** (Druga HNL, 2010/11). First: "4 (13th-16th)", from https://en.wikipedia.org/wiki/2010–11_Croatian_Second_Football_League. Second: RSSSF 2010/11 note: only 15th or better may remain, so 1 planned; real: 15th expelled, 16th dissolved. First value: 4 (13th-16th). Source: https://www.rsssf.org/tablesk/kroa2011.html
11. **gre-pol-cro-366 down** (Druga HNL, 2015/16). First: "3 (10th-12th)", from https://en.wikipedia.org/wiki/2015–16_Croatian_Second_Football_League. Second: RSSSF note: only 11th or better may remain; 11th and 12th relegated. First value: 3 (10th-12th). Source: https://www.rsssf.org/tablesk/kroa2016.html
12. **gre-pol-cro-367 down** (Druga HNL, 2016/17). First: "3 (10th-12th)", from https://en.wikipedia.org/wiki/2016–17_Croatian_Second_Football_League. Second: RSSSF note: only 11th or better may remain; 11th and 12th relegated. First value: 3 (10th-12th). Source: https://www.rsssf.org/tablesk/kroa2017.html
13. **gre-pol-cro-368 playoffs** (Druga HNL, 2017/18). First: "2nd: promotion play-off v HNL 9th; 11th: relegation play-off", from https://en.wikipedia.org/wiki/2017–18_Croatian_Second_Football_League. Second: RSSSF 2017/18: 3rd Varazdin went to the promotion play-off (Dinamo B, 2nd, cannot go up); note says 11th or better remain, so no relegation play-off for 11th. First value: 2nd promotion play-off, 11th relegation play-off. Source: https://www.rsssf.org/tablesk/kroa2018.html
14. **windows, 2023/24 to 2027/28** (rows gre-pol-cro-158, 159, 160, 161, 162, 320, 321, 322, 323, 324, 482, 483, 484, 485, 486). First: "two registration periods: one between seasons (max 12 weeks) and one mid-season (max 4 weeks)", from https://en.wikipedia.org/wiki/Transfer_window. Second: the FIFA RSTP edition of March 2023 came into force on 1 April 2023. It sets the first period at 8 to 12 weeks and the second at 4 to 8 weeks, with 16 weeks at most for both. Source: https://digitalhub.fifa.com/m/153157b40ca1dfd/original/Regulations-on-the-Status-and-Transfer-of-Players-March-2023-edition.pdf. The August 2021 edition still has 12 and 4 weeks, so the rows to 2022/23 agree.

## Found values (first value `unknown`)

- **gre-pol-cro-65 down** (Football League, 2011/12): RSSSF marks 16th to 18th relegated: value 3 (16th-18th). Source: https://www.rsssf.org/tablesg/grk2012.html
- **gre-pol-cro-72 down** (Football League, 2018/19): RSSSF: 11th and 12th relegated, 13th to 16th dropped to the fourth level (new level structure): 6 in all. Source: https://www.rsssf.org/tablesg/grk2019.html
- **gre-pol-cro-73 down** (Super League 2, 2019/20): RSSSF: 11th and 12th relegated, 9th demoted to the fourth level. Source: https://www.rsssf.org/tablesg/grk2020.html
- **gre-pol-cro-74 down** (Super League 2, 2020/21): RSSSF: due to the restructuring no clubs were relegated; two clubs withdrew: value 0. Source: https://www.rsssf.org/tablesg/grk2021.html
- **gre-pol-cro-75 down** (Super League 2 (North group), 2021/22): RSSSF North group: 14th to 17th relegated: 4. Source: https://www.rsssf.org/tablesg/grk2022.html
- **gre-pol-cro-76 down** (Super League 2 (North group), 2022/23): RSSSF North group: 12th to 14th relegated, Apollon Larissas withdrew: 3 plus 1. Source: https://www.rsssf.org/tablesg/grk2023.html
- **gre-pol-cro-77 down** (Super League 2 (North group), 2023/24): RSSSF North group: 12th of the regular season and 9th to 11th of the play-out round relegated: 4. Source: https://www.rsssf.org/tablesg/grk2024.html
- **gre-pol-cro-129 down** (Super League 2 (South group), 2021/22): RSSSF South group: 13th, 15th, 16th and 17th relegated (Panathinaikos B in 14th cannot go down): 4. Source: https://www.rsssf.org/tablesg/grk2022.html
- **gre-pol-cro-130 down** (Super League 2 (South group), 2022/23): RSSSF South group: 12th to 14th relegated, PASA Irodotos expelled: 3 plus 1. Source: https://www.rsssf.org/tablesg/grk2023.html
- **gre-pol-cro-131 down** (Super League 2 (South group), 2023/24): RSSSF South group: 12th of the regular season and 9th to 11th of the play-out round relegated: 4. Source: https://www.rsssf.org/tablesg/grk2024.html
- **gre-pol-cro-195 down** (I liga (Ekstraklasa), 2006/07): RSSSF: 15th and 16th relegated; Arka Gdynia and Gornik Leczna also relegated for corruption: value 2 (15th-16th) plus punishments. Source: https://www.rsssf.org/tablesp/pol07.html
- **gre-pol-cro-196 down** (I liga (Ekstraklasa), 2007/08): RSSSF: 15th and 16th relegated to the third level; Zaglebie Lubin and Korona Kielce also relegated for corruption: value 2 (15th-16th) plus punishments. Source: https://www.rsssf.org/tablesp/pol08.html
- **gre-pol-cro-249 down** (II liga, 2006/07): pl.wikipedia: planned 4 last relegated; 90minut.pl: only 2 went down directly after the demotions. Source: https://pl.wikipedia.org/wiki/II_liga_polska_w_pi%C5%82ce_no%C5%BCnej_(2006/2007)
- **gre-pol-cro-250 up** (II liga, 2007/08): RSSSF 2007/08: no play-offs; after appeals the top 4 were promoted: value 4 (1st-4th). Source: https://www.rsssf.org/tablesp/pol08.html
- **gre-pol-cro-359 up** (Druga HNL, 2008/09): RSSSF 2008/09: 1st to 3rd and 5th promoted (4th demoted) as the top level grew to 16: 4. Source: https://www.rsssf.org/tablesk/kroa09.html
- **gre-pol-cro-360 up** (Druga HNL, 2009/10): RSSSF 2009/10: 1st and 3rd promoted: 2. Source: https://www.rsssf.org/tablesk/kroa2010.html
- **gre-pol-cro-362 down** (Druga HNL, 2011/12): RSSSF 2011/12: 12th, 13th and 15th relegated: 3. Source: https://www.rsssf.org/tablesk/kroa2012.html
- **gre-pol-cro-363 down** (Druga HNL, 2012/13): RSSSF 2012/13 note: only 15th or better may remain (1 planned); real: 12th to 16th relegated, 4th demoted, 5th withdrew. Source: https://www.rsssf.org/tablesk/kroa2013.html
- **gre-pol-cro-369 down** (Druga HNL, 2018/19): RSSSF 2018/19: 14th relegated, 8th Zadar relegated after licence revocation: 1 plus 1. Source: https://www.rsssf.org/tablesk/kroa2019.html

No foreign-player value was found for the seasons that the first agent marked `unknown`. I searched Greek, Polish and Croatian sources (federation pages, national news) and found no dated rule for those seasons.

## Other points for the maintainer

- **gre-pol-cro-160 foreign.** The disagreement may be one of scope. Sportmonks gives seven non-EU players per squad and no age split. The first value gives six over-23 players. A federation text for 2025/26 would settle it.
- **gre-pol-cro-161 and 162 foreign.** Sportmonks agrees on seven non-EU players per squad. It does not confirm the five-on-the-field rule, the over-23 wording or the Greek under-23 rule.
- **gre-pol-cro-459 format (Croatian Super Cup 2027/28).** The row says "(real: not played this season)". The July 2027 match depends on the 2026/27 results, so this cannot be known yet. The 2026/27 row (458) agrees: RSSSF lists Dinamo as the 2026 double winner.
- **gre-pol-cro-452 (Croatian Super Cup 2020/21).** RSSSF agrees that no match was played, but the reason was no free date, not a double.
- **gre-pol-cro-106 (Greek Super Cup 2025/26).** RSSSF shows the match of 3 January 2026 (Olympiakos, double winners, against cup runner-up OFI). The HFF decision date of 21 January 2025 is not on that page.
- **Croatia 2026/27.** tportal.hr (2 July 2025) reports a new rule from 2026/27: at least one player under 21 on the pitch for the whole match in the top league. The 2026/27 and 2027/28 nation rows do not mention it.
- **Polish youth rule 2022/23 to 2024/25.** polsatsport.pl confirms the 3000-minute floor and the fines. It does not confirm the cap of 270 minutes counted per match.
- **Planned against real values.** Many rows hold the planned count and give the real outcome in the notes. I marked those `agree` where a second source shows the same plan, and I wrote the real outcome in the note (for example Greek second level 2016/17, Polish I liga 2012/13 and 2015/16, Croatian second level 2014/15 and 2019/20).

## Clubs file (`gre-pol-cro-clubs.tsv`)

Spot checks against RSSSF and 90minut.pl. All club lists match in count and names.

| Season | Division | Rows | Second source |
|---|---|---|---|
| 2013/14 | gre_prm | 18 | RSSSF grk2014 |
| 2021/22 | gre_second (North) | 17 | RSSSF grk2022 |
| 2014/15 | gre second, group 2 | 13 | RSSSF grk2015 |
| 2003/04 | pol_first | 14 | RSSSF pol04 |
| 2003/04 | pol_second | 18 (16 finished, KSZO and Blekitni withdrew) | 90minut.pl liga633 |
| 2002/03 | cro_a1 | 12 | RSSSF kroa03 |
| 2011/12 | cro_a2a | 15 | RSSSF kroa2012 |
| 2005/06 | cro_a2b | 12 | RSSSF kroa06 |

A script listed every pair where the real name and the database name share little text. I checked these by hand.

- **Doubtful: 2003/04 cro_a2b, "Pula 1856" matched to NK Istra (5463).** RSSSF 2002/03 shows Istra Pula relegated from the South group after the play-off. Pula 1856 is the club that the file later matches to 12006 ("NK Pula 1856" in 2005/06, then Istra 1961). The match to 5463 is probably wrong. Max decides.
- **Doubtful: 2014/15 gre_second, "Serres" matched to Panserraikos (10360).** RSSSF lists the club as "Serres FC". Panserraikos is not in that season's table. I could not confirm that the two are the same club.
- **Correct by name history:** AEL to Larisa, Egaleo to Aigaleo, Platanias to AO Platania Chanion, LKS to Lodzki Klub Sportowy, GKS Bogdanka to Gornik Leczna, Medimurje to NK Medjimurje Cakovec, Dospel Katowice to GKS Katowice, Sesvete to NK (Radnik) Sesvete.

## Temporary files

Downloaded pages and scripts are in a folder made with `mktemp -d` under `/var/folders/.../T/`. I left them in place.
