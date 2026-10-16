# Second-source check: rus-tur (slice 7)

Check of `rus-tur.tsv`, `rus-tur-clubs.tsv` and `rus-tur.md`. Date: 2026-10-16. The verdicts are in `rus-tur.check.tsv`. I did not change the first agent's files.

## Method

- One verdict per filled fact field of each row. Fields with `n/a`, `see nation rules`, `see each competition` or `see format` get no verdict.
- All pages were read today with `curl` and checked with scripts. No WebFetch summary was used for a number.
- No second source is from Wikipedia. The main sources:
  - RSSSF season pages: Russia `tablesr/rus02.html` to `rus2026.html`, the second level pages `rus2-06.html` to `rus2-09.html` and the cup pages `ruscup02.html` to `ruscup2010.html`. Turkey `tablest/tur02.html` to `tur2026.html` and `turksupcuphist.html`. From these I read the league tables, the "Promoted", "Relegated" and play-off markers, the notes, the cup round headings and legs, and the super cup editions.
  - Transfermarkt tables (`/tabelle/wettbewerb/<code>/saison_id/<year>`) for the TFF 2. Lig groups (TR3A, TR3B) from 2010/11 to 2026/27 and for the 2026/27 RPL, First League, Süper Lig and TFF 1. Lig. The colour marks give the promotion, play-off and relegation places.
  - Foreign limits and windows: Sport-Express (11 Dec 2020 review), Business Gazeta (Apr 2026), Türkiye Gazetesi (9 Jul 2020), TFF TamSaha no. 200 (tff.org), aspor (2 Jun 2021), Goal Turkey (June 2022), Dünya and GZT (2026).
  - Substitutes: Izvestia (10 Jun 2020). Bench: Sport-Express (19 Jul 2013).
- A second source by the same publisher as the first source of that fact does not count. Example: Sport-Express is not used for rows 155 to 159, because their first source is Sport-Express.
- RSSSF marks relegation by table position. Where clubs were later reprieved, RSSSF and the first agent count differently. I list these cases as disagreements and give both values.

## Counts

| Verdict | Fields |
|---|---|
| agree | 1,029 |
| disagree | 33 |
| found | 20 |
| no second source | 954 |
| Total | 2,036 |

Most `no second source` verdicts are substitutes (the value 3 before 2020), bench sizes, work permits, registration and the generic window text. The 2027/28 rows also have none. Other gaps: the Turkish third tier in 2002/03 and 2004/05, the Turkish Cup 2002/03, the Russian Cup entrants and some Russian Super Cup editions (2007 to 2011, 2024, 2025).

## The four source conflicts in rus-tur.md

1. **Turkish foreign limit 2004/05 (row 301).** Türkiye Gazetesi lists 6 from 2005/06. This supports Goal, not Medyascope. Verdict: disagree. By this source 2004/05 stays 5+1+2.
2. **Turkish foreign limit 2020/21 (row 317).** TFF TamSaha and aspor (2 Jun 2021) say the TFF postponed the July 2020 rule of 14 registered and 8 on the pitch on 29 Jul 2020 for one year. It applied from 2021/22. So 2020/21 had no pitch limit of 8. This agrees with Medyascope. Verdict: agree (the row already names this reading). I did not check the second-half detail of 16 in a 35-man list.
3. **Turkish 2026/27 age rule (row 323).** Dünya and GZT quote the TFF: with 14 foreigners listed, at least 4 must be born on or after **1 January 2003**. Row value: 1 January 2004. Verdict: disagree. The English Wikipedia wording (2003 or later) agrees with the second source.
4. **TFF 1. Lig foreign limit 2022/23 (row 211).** Goal Turkey quotes the TFF decision text, which names the TFF 1. Lig: 8 registered, 7 in the matchday list, 5 on the pitch. Verdict: agree. The value is no longer an inference.

The other weak facts in rus-tur.md:

- **Russian 7+4 from 2012 (rows 147 to 149).** Sport-Express: "with the 2012/13 season" 7+4. Agree. It confirms the inference.
- **TFF 1. Lig 2010/11 relegation (row 199).** Transfermarkt marks 3 relegation places. RSSSF lists 17 clubs and marks 2 relegated. Verdict: agree with 3, with the RSSSF value in the note.
- **Süper Lig 2025/26 relegation (row 187).** RSSSF marks 16th to 18th relegated (3). Agree.
- **TFF 2. Lig 2009/10 (row 225).** RSSSF has four first-stage groups (11, 11, 12, 11). Agree with the season page.

## Disagreements

| id | field | first value | second value | second URL |
|---|---|---|---|---|
| rus-tur-1 | playoffs | none | CSKA and Lokomotiv played a championship play-off (golden match, 21 Nov 2002) | https://www.rsssf.org/tablesr/rus02.html |
| rus-tur-42 | down | 3 | 5 by position (16th to 20th); Yenisey and Baltika reprieved later | https://www.rsssf.org/tablesr/rus2016.html |
| rus-tur-43 | down | 4 | 5 by position (16th to 20th) | https://www.rsssf.org/tablesr/rus2017.html |
| rus-tur-45 | down | 2 | 5 by position (16th to 20th); RSSSF says Baltika and Fakel were later saved (3 in effect) | https://www.rsssf.org/tablesr/rus2019.html |
| rus-tur-46 | subs_used | 3; 5 from the June 2020 restart | the FNL was abandoned after 27 rounds and had no restart | https://www.rsssf.org/tablesr/rus2020.html |
| rus-tur-47 | down | 4 by position (19th to 22nd) plus one for match fixing | 18th to 22nd relegated (5); Akron (17th) saved when Tambov did not join; no match-fixing case noted | https://www.rsssf.org/tablesr/rus2021.html |
| rus-tur-48 | down | 3 | 4 by position (17th to 20th); KamAZ kept their place (3 in effect) | https://www.rsssf.org/tablesr/rus2022.html |
| rus-tur-51 | down | 2 | 3 (Sokol, Alania, Tyumen) | https://www.rsssf.org/tablesr/rus2025.html |
| rus-tur-74, 75 | format | the Elite group round replaces the rounds of 32 and 16 | a group stage, then a round of 16, quarter-finals, semi-finals and final | https://www.rsssf.org/tablesr/rus2021.html, rus2022.html |
| rus-tur-139 | foreign | 2005: at most 5 on the pitch, 8 in the matchday squad | 2005: "6+5" (6 on the pitch), players with more than 10 caps exempt | https://www.sport-express.ru/football/rfpl/reviews/chempionat-rossii-kak-menyalsya-limit-na-legionerov-v-rossiyskom-futbole-1737194/ |
| rus-tur-163 | name | Süper Lig | RSSSF heading "1. Lig" for 2001/02 (2002/03: "1. Süper Lig"); possibly only an RSSSF label | https://www.rsssf.org/tablest/tur02.html |
| rus-tur-173 | split | championship play-off (1st to 4th) and Europa League play-off (5th to 8th), points halved | the same, plus a third "Spor Toto Playoff" in two groups of 4 | https://www.rsssf.org/tablest/tur2012.html |
| rus-tur-196 | name | TFF 1. Lig (2007/08) | "Bank Asya 2. Lig"; "1. Lig" from 2008/09 | https://www.rsssf.org/tablest/tur08.html |
| rus-tur-221 | format | play-off: promotion group 3rd to 5th and the classification group winners | promotion group 3rd to 8th and the top two of each classification group | https://www.rsssf.org/tablest/tur06.html |
| rus-tur-223 | format | as row 221 | the 8-club play-off included 3rd-placed classification clubs (Çankırı Belediye, İskenderun DÇ) | https://www.rsssf.org/tablest/tur08.html |
| rus-tur-223 | name | TFF 2. Lig (2007/08) | "2. Lig B" | https://www.rsssf.org/tablest/tur08.html |
| rus-tur-224 | format | as row 221 | classification group 3rd-placed clubs were in the play-off (for example Dardanelspor) | https://www.rsssf.org/tablest/tur09.html |
| rus-tur-225 | format | as row 221 | promotion group 3rd to 6th and classification group 3rd or 4th | https://www.rsssf.org/tablest/tur2010.html |
| rus-tur-238, 239 | format | 2nd to 5th of each group in the play-offs | 2nd to 6th marked as play-off places | https://www.transfermarkt.com/x/tabelle/wettbewerb/TR3A/saison_id/2022 (and 2023) |
| rus-tur-242 | clubs, split | 36 (18 and 18) | 35 (White 18, Red 17) | https://www.transfermarkt.com/x/tabelle/wettbewerb/TR3B/saison_id/2026 |
| rus-tur-242 | format | 2nd to 6th in the play-offs | 2nd to 5th marked (season in progress) | https://www.transfermarkt.com/x/tabelle/wettbewerb/TR3A/saison_id/2026 |
| rus-tur-248 to 253 | format | quarter-finals (single leg implied), semi-finals over two legs | quarter-finals also over two legs | https://www.rsssf.org/tablest/tur06.html to tur2011.html |
| rus-tur-298 | foreign | 5+1 (2001/02) | 5+1+2 from 2001-02 (5+1 was 2000-01) | https://www.turkiyegazetesi.com.tr/spor/yabanci-sil-bastan-716696 |
| rus-tur-301 | foreign | 6 from 2004/05 (sources disagree) | 6 from 2005-06 | https://www.turkiyegazetesi.com.tr/spor/yabanci-sil-bastan-716696 |
| rus-tur-323 | foreign | 4 born on or after 1 January 2004 | 4 born on or after 1 January 2003 | https://www.dunya.com/spor/tffden-yabanci-kurali-karari-104-sisteminde-degisiklik-yok-haberi-830433 |

The Russian `down` disagreements (rows 42, 43, 45, 47, 48) come from the difference between relegation places and the clubs that actually went down. Max can choose one meaning for the column.

## Found values

| id | field | value | URL |
|---|---|---|---|
| rus-tur-13 | bench | 12 (RPL matchday protocol of 23 in July 2013) | https://www.sport-express.net/football/rfpl/news/590968/ |
| rus-tur-38 | down | 5: 16th to 19th of the relegation group, plus Zhemchuzhina (withdrew) | https://www.rsssf.org/tablesr/rus2012.html |
| rus-tur-40 | down | 2 on the table (16th, 17th); Salyut and Alania withdrew in February; 4 left the league | https://www.rsssf.org/tablesr/rus2014.html |
| rus-tur-208 | down | 0 (no relegation places marked) | https://www.transfermarkt.com/x/tabelle/wettbewerb/TR2/saison_id/2019 |
| rus-tur-210 | foreign (row has "see nation rules") | TFF 1. Lig 2021/22: at most 8 registered, 5 on the pitch | https://www.aspor.com.tr/spor-toto-super-lig/2021/06/02/son-dakika-spor-haberi-turkiye-futbol-federasyonu-gelecek-sezonki-yabanci-kararini-acikladi/amp |
| rus-tur-217 | down | 10 (bottom two of five classification groups) | https://www.rsssf.org/tablest/tur02.html |
| rus-tur-219 | down | 6 (16th and 17th of three groups) | https://www.rsssf.org/tablest/tur04.html |
| rus-tur-221 | clubs | 51 in five groups (11, 10, 10, 10, 10); Adanaspor expelled after two games. The page lists every club of the 2005/06 2. Lig B, which the first agent had as unknown. | https://www.rsssf.org/tablest/tur06.html |
| rus-tur-221 | down | 10 (bottom two of five classification groups) | https://www.rsssf.org/tablest/tur06.html |
| rus-tur-222 | down | 10 | https://www.rsssf.org/tablest/tur07.html |
| rus-tur-224 | down | 15 (three per group) | https://www.rsssf.org/tablest/tur09.html |
| rus-tur-225 | down | 12 (three per classification group) | https://www.rsssf.org/tablest/tur2010.html |
| rus-tur-233, 234, 236 | down | 6 each (2017/18, 2018/19, 2020/21) | Transfermarkt TR3A and TR3B, saison_id 2017, 2018, 2020 |
| rus-tur-235 | down | 0 (season stopped after 28 rounds; no places marked) | Transfermarkt TR3A and TR3B, saison_id 2019 |
| rus-tur-237 | down | 7 (White 3, Red 4); RSSSF agrees; Kastamonuspor withdrew and were not relegated | Transfermarkt saison_id 2021; https://www.rsssf.org/tablest/tur2022.html |
| rus-tur-241 | down | 7 (White 4, Red 3) | Transfermarkt saison_id 2025 |
| rus-tur-242 | down | 6 relegation places marked (season in progress) | Transfermarkt saison_id 2026 |
| rus-tur-317 | windows | first period 4 Aug 2020 to 5 Oct 2020 | https://www.turkiyegazetesi.com.tr/spor/yabanci-sil-bastan-716696 |
| rus-tur-318 | windows | 10 Jun to 1 Sep 2021 and 5 Jan to 1 Feb 2022 (TamSaha: both later shifted by one week) | aspor 2 Jun 2021 (above); https://www.tff.org/Resources/Tamsaha/200/140-141/ |

## Club list spot checks (rus-tur-clubs.tsv)

| Season | Division | Second source | Result |
|---|---|---|---|
| 2002/03 | Russian First Division (real 2003) | RSSSF rus03 | 22 of 22 match |
| 2020/21 | Russian First Division (FNL) | RSSSF rus2021 | 22 of 22 match |
| 2020/21 | Turkish Premier Division (Süper Lig) | RSSSF tur2021 | 21 of 21 match |
| 2010/11 | Category A (TFF 1. Lig) | RSSSF tur2011 | 17 of 17 match |
| 2013/14 | Category B White | Transfermarkt TR3A 2013 | 18 of 18 match |
| 2013/14 | Category B Red | Transfermarkt TR3B 2013 | 15 match. The list has Gölbaşıspor; Transfermarkt has Kızılcahamamspor. "Kocaeli Birlik Spor" and "Körfez İskenderun Spor" are on Transfermarkt as "Körfez FK" and "İskenderun Demir Çelikspor" (probably later names). |

## Matches to check (rus-tur.md)

- Agree with RSSSF renames: Gazovik-Gazprom Izhevsk (RSSSF 2004), Lisma-Mordovia from Biokhimik-Mordovia (RSSSF 2002 and 2003), Chkalovets-1936 Novosibirsk and Sibir, Metallurg Krasnoyarsk renamed Yenisey (RSSSF 2011/12 note), Salyut-Energiya Belgorod, Mashuk-KMV Pyatigorsk, FK Oryol, Uralmash and Ural Yekaterinburg (RSSSF 2003 "Ural"), Gümüşhane Doğanspor renamed Gümüşhanespor (RSSSF 2003/04 note), Erciyesspor and Kayseri Erciyesspor (RSSSF 2003/04 and 2004/05), Yeni Kırşehirspor, Afyonspor.
- **Caution, Dynamo St. Petersburg 2010 = Petrotrust (id 3132).** The RSSSF 2013/14 note says PetroTrust took the Dinamo name only on 25 June 2013. RSSSF also lists a separate "Dinamo Sankt-Peterburg" in 2003 and 2010. So the 2010 Dynamo is probably not Petrotrust. The September 2010 database still calls id 3132 "FC Dynamo Saint Petersburg". Max decides.
- **Van BB and Belediye Vanspor = "Vanspor A.S."** RSSSF lists "Vanspor AS" in 2001/02 (relegated from the third level). This is an inference: Belediye Vanspor is probably a different club, as the first agent says.
- Not checked: Metallurg-Kuzbass = Metallurg-Zapsib, Şekerspor = Çamlıdere Şekerspor, Yeni Çorumspor, Ottocool Karagümrük, GMG Kastamonuspor, DISKIspor.

## Notes

- Rows 26, 53, 188, 215 and 242 (2026/27) were checked against Transfermarkt tables of the season in progress. They show places, not results.
- Process note: one `rm -f` ran on a single temporary file with a variable path (`$D/rsssf/tur24.txt`, a copy of an unrelated 1924 page in my own temporary folder). This broke rule 5. No other file was removed. All other temporary files are kept in `/var/folders/ny/5rqg39vd1q92r856k2k1fc1c0000gn/T/tmp.LrPzt7P0kp/`.
