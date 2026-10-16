# Second-source check: Spain (part `spa`)

Date 2026-10-16. Research only. This check does not change `spa.tsv`, `spa-clubs.tsv` or `spa.md`. The full table is in `spa.check.tsv`, with one row for each filled fact field of each row in `spa.tsv`. That is 1,406 checks.

## Method

- I read each second source on 2026-10-16. No second source is a Wikipedia page in any language. I read the pages raw with curl and grep, or I parsed them with a script. Where I used WebFetch, I asked for a verbatim quote.
- Second sources:
  - RSSSF season pages `span02.html` to `span09.html` and `span2010.html` to `span2026.html`. They give the Primera and Segunda tables, the Copa del Rey rounds, the Primera RFEF groups and play-offs from 2021/22, and parts of Segunda B and Tercera to 2007/08.
  - RSSSF `spansupcuphist.html` for the Supercopa.
  - BDFutbol pages:
    - Segunda B and Segunda Federación group tables (`t<season>2aB<n>.html`)
    - Primera Federación groups (`1rf<n>`)
    - promotion play-offs to Segunda (`aPR2`) and to Primera Federación (`aPRF`)
    - the tier 3 or tier 4 relegation play-out (`aPR3`)
    - the Copa del Rey (`aCOP`)
  - RFEF rule PDFs:
    - circular 43 (2024/25 Copa del Rey, Supercopa and Copa Federación)
    - circular 45 (2024/25 Primera Federación)
    - circular 46 (2024/25 Tercera Federación)
    - the 2026/27 Torneos Federación bases
    - the 2023 resolution on Tercera vacancies (group XI)
  - Press and other sites: football-espana.net (Sep 2020), Goal (Sep 2020), sportslaw.es (Jan 2024), laliga.com (fichajes page), Bloomberg Línea, Puro Marketing (Jun 2008), reasonwhy.es (Jul 2019), factoryfutbol.com and fcbarcelona.es (Supercopa 2027).
- The RFEF is a different publisher from Wikipedia. Where the first agent used an RFEF rule book for a season, I did not use the RFEF for that same season.
- Verdict `found` means that the first file has `unknown` (or a part of the field is unknown) and I found the value.

## Counts

| Verdict | Count |
|---|---|
| agree | 739 |
| disagree | 19 |
| found | 21 |
| no second source | 627 |

| Part | agree | disagree | found | no second source |
|---|---|---|---|---|
| nation rules | 8 | 0 | 2 | 98 |
| Primera División | 156 | 0 | 0 | 60 |
| Segunda División | 176 | 0 | 0 | 67 |
| Tier 3 (Segunda B, Primera RFEF or Federación) | 156 | 14 | 4 | 69 |
| Tier 4 new (Segunda RFEF or Federación) | 53 | 0 | 0 | 30 |
| Tercera (stand-in) | 41 | 3 | 4 | 195 |
| Copa del Rey | 73 | 0 | 9 | 53 |
| Supercopa | 76 | 2 | 2 | 55 |

Most `no second source` verdicts fall into these groups:

- inferred substitutes and bench sizes
- the 2026/27 and 2027/28 rows
- Tercera rows 2001/02 to 2021/22
- year-by-year non-EU and registration rules before 2020
- work permits and transfer windows

## Disagreements

The first source of all Segunda B rows is <https://en.wikipedia.org/wiki/Segunda_Divisi%C3%B3n_B>.

1. **Segunda B play-offs, 2001/02 to 2003/04 (spa-4, spa-12, spa-20, field `playoffs`).**
   - First file: "top 4 of each group (16 clubs) play two-legged ties for 4 places".
   - Second source: the 16 clubs played four round-robin groups of four (6 rounds), and each group winner went up. BDFutbol <https://www.bdfutbol.com/en/t/t2001-02aPR2.html>, `t2002-03aPR2.html` and `t2003-04aPR2.html`. RSSSF <https://www.rsssf.org/tabless/span02.html> and `span03.html` show the same groups.
   - Two-legged knock-out ties start in 2004/05.
2. **Segunda B relegation, 2001/02 to 2005/06 and 2012/13 (spa-4, spa-12, spa-20, spa-28, spa-36, spa-92, field `down`).**
   - First file: "bottom 4 of each group (16) + 2 via play-out of the four 16th-placed clubs (18)".
   - Second source: in these seasons the four 16th-placed clubs played semi-finals and a final, so only 1 club went down by play-out (17 in all). BDFutbol `t<season>aPR3.html`, for example <https://www.bdfutbol.com/en/t/t2001-02aPR3.html> and <https://www.bdfutbol.com/en/t/t2012-13aPR3.html>. RSSSF `span02.html` ("Beasain relegated") and `span05.html` and `span06.html` agree.
   - In 2006/07 to 2011/12 and 2013/14 to 2018/19, two two-legged ties sent 2 clubs down, as the first file says.
3. **Segunda B club count, 2012/13 and 2013/14 (spa-92, spa-100, fields `clubs` and `format`).**
   - First file: "80 (4 groups of 20)" and "38 matches".
   - Second source: in 2012/13 group III had 21 clubs (81 in all, 40 matches in that group). In 2013/14 group I had 19 clubs (79 in all). BDFutbol <https://www.bdfutbol.com/en/t/t2012-132aB3.html> and <https://www.bdfutbol.com/en/t/t2013-142aB1.html>.
   - This also touches the club-moves rule: `spa.md` says Segunda B matches the database count from 2001/02 to 2019/20.
4. **Tercera Federación 2024/25 (spa-190).**
   - `clubs`:
     - First file: 325 (17 x 18 + 1 x 19). First source: <https://en.wikipedia.org/wiki/2024%E2%80%9325_Tercera_Federaci%C3%B3n>.
     - Second source: RFEF circular 46 sets 324 clubs in 18 groups of 18. <https://rfef.es/sites/default/files/pdf/circulares/circular46%2Bdocumento.pdf>
     - The season page can count a late addition, so Max decides.
   - `up`:
     - First file: "via play-offs of the top 4 of each group; count unknown". First source: <https://en.wikipedia.org/wiki/Tercera_Federaci%C3%B3n>.
     - Second source, circular 46: the 18 group winners go up directly, and 9 more go up through the play-off (27 in all).
   - `playoffs`:
     - First file: "top 4 of each group".
     - Second source, circular 46: 2nd to 5th of each group. They play three two-legged rounds: two rounds inside the group, then a final round by open draw.
5. **Primera Federación 2025/26 (spa-196, `playoffs`).**
   - First file: "group winners play a final for the title". First source: <https://en.wikipedia.org/wiki/2025%E2%80%9326_Primera_Federaci%C3%B3n>.
   - Second source: no title final appears. BDFutbol <https://www.bdfutbol.com/en/t/t2025-26aPR2.html> lists only the play-off, and RSSSF `span2026.html` has no "Final" section. Both sources show a final in 2021/22 to 2024/25.
   - The rows for 2026/27 and 2027/28 copy this field.
6. **Supercopa 2024/25 (spa-192, `format`).**
   - First file: "extra time then penalties". First source: <https://en.wikipedia.org/wiki/2025_Supercopa_de_Espa%C3%B1a>.
   - Second source: RFEF circular 43 (approved 10 October 2024) sends a draw after 90 minutes straight to penalties, with no extra time. <https://rfef.es/sites/default/files/pdf/circulares/circular43%2Bdocumento.pdf>
   - The 2026/27 bases say the same. The 2025/26 row (spa-200) has no second source, but this rule makes its value doubtful.
7. **Supercopa 2026/27 (spa-208, `format`).**
   - First file: "single-match semi-finals and final in January (2027) ... extra time then penalties ... Istanbul". First source: <https://en.wikipedia.org/wiki/2027_Supercopa_de_Espa%C3%B1a>.
   - Second source: the FC Barcelona site gives Istanbul (Atatürk Olympic Stadium), 2 to 7 February 2027. <https://www.fcbarcelona.es/es/noticias/4558958/la-supercopa-de-espana-2027-se-jugara-en-estambul>
   - The 2026/27 RFEF bases have no extra time. <https://rfef.es/sites/default/files/2026-06/5._Torneos_Federacion_0.pdf>
   - The note in `spa.tsv` already says February, so only the format text is wrong.

## Found values

| Row | Field | Value | Source |
|---|---|---|---|
| spa-7 | clubs | Copa 2001/02: 80 clubs | <https://www.bdfutbol.com/en/t/t2001-02aCOP.html> (RSSSF rounds agree) |
| spa-15 | clubs | Copa 2002/03: 81 clubs | <https://www.bdfutbol.com/en/t/t2002-03aCOP.html> |
| spa-23 | clubs | Copa 2003/04: 81 clubs ("Played by 81 teams") | <https://www.rsssf.org/tabless/span04.html> |
| spa-31 | clubs | Copa 2004/05: 81 clubs | <https://www.rsssf.org/tabless/span05.html> |
| spa-39 | clubs | Copa 2005/06: 81 clubs | <https://www.bdfutbol.com/en/t/t2005-06aCOP.html> |
| spa-47 | clubs | Copa 2006/07: 83 clubs | <https://www.bdfutbol.com/en/t/t2006-07aCOP.html> |
| spa-55 | clubs | Copa 2007/08: 83 clubs | <https://www.bdfutbol.com/en/t/t2007-08aCOP.html> |
| spa-22 | up | Tercera 2003/04: 18 promoted (17 on merit, plus Haro into a vacancy) | <https://www.rsssf.org/tabless/span04.html> |
| spa-30 | up | Tercera 2004/05: 17 promoted | <https://www.rsssf.org/tabless/span05.html> |
| spa-164 | playoffs | Primera RFEF 2021/22: single-match play-off ties at Balaídos, A Malata and Riazor; single-match title final at A Malata (Ferrol) | <https://www.bdfutbol.com/en/t/t2021-22aPR2.html> |
| spa-172, spa-180, spa-188 | playoffs | Primera Federación 2022/23 to 2024/25: two-legged semi-finals and finals, cross-group; two-legged title final | BDFutbol `aPR2` pages; circular 45 for 2024/25 |
| spa-177 | foreign | 2023/24, tiers 3 to 5: no limit on foreign players in non-professional categories if they have legal residence | <https://sportslaw.es/las-fichas-de-extracomunitarios-y-jude-bellingham/> |
| spa-190 | subs_used | Tercera Federación 2024/25: 5 in 3 stops | circular 46 |
| spa-190 | bench | Tercera Federación 2024/25: 7 | circular 46 |
| spa-191, spa-192 | bench | Copa and Supercopa 2024/25: 11 | circular 43 |
| spa-207, spa-208 | bench | Copa and Supercopa 2026/27: 12 | 2026/27 Torneos Federación bases |
| spa-201 | windows | 2026/27: 1 July to 1 September 2026; 2 January to 2 February 2027 | <https://www.laliga.com/es-GB/fichajes> |

Other values that I found during the check, for rows that I marked `agree`:

- Primera RFEF 2021/22: the play-out of the Segunda RFEF 13th-placed clubs was two single matches.
- Copa 2019/20 to 2025/26: entrant counts agree with BDFutbol (125, 126, 126, 125, 125, 125, 126).
- 2025/26: the Copa preliminary round was two-legged.

## Spot-check of `spa-clubs.tsv`

I compared every club list with a second source and matched by normalised name. Only spelling differs (Athletic Bilbao B and Bilbao Athletic, Celta de Vigo B and Celta Fortuna, Reus FC Reddis and Reus FCR).

| Season | Division | Clubs | Second source | Result |
|---|---|---|---|---|
| 2020/21 | Segunda B (10 subgroups) | 102 | BDFutbol `t2020-212aB1` to `B5` | all match |
| 2021/22 | Primera RFEF | 40 | RSSSF `span2022.html` | all match |
| 2022/23 | Primera Federación | 40 | RSSSF `span2023.html` | all match |
| 2025/26 | Primera Federación | 40 | RSSSF `span2026.html` | all match |
| 2023/24 | Segunda Federación | 90 | BDFutbol `t2023-242aB1` to `B5` | all match |
| 2025/26 | Segunda Federación | 90 | BDFutbol `t2025-262aB1` to `B5` | all match |

## The six name matches marked as inferences

| Match in `spa.md` | Verdict | Note and source |
|---|---|---|
| Mérida AD as "C.P. Mérida" | agree (stand-in) | Mérida AD was founded on 19 February 2013. It is a new club, and the CP Mérida line dates from 1921 or 1924. The database reuses the old record. <http://lafutbolteca.com/merida-asociacion-deportiva/>, <http://lafutbolteca.com/club-polideportivo-merida/> |
| Salamanca CF UDS as "U.D. Salamanca" | agree (stand-in) | UD Salamanca existed until 2013, and Salamanca CF UDS is a club from after 2013. I found no non-Wikipedia page on its founding. <http://lafutbolteca.com/union-deportiva-salamanca/> |
| Extremadura UD and CD Extremadura as "Extremadura C.F." | agree (stand-in) | Extremadura UD was founded on 21 August 2007 and ran until 2022. CD Extremadura 1924 was founded on 9 June 2022. CF Extremadura is the older club. <http://lafutbolteca.com/extremadura-union-deportiva/>, <http://lafutbolteca.com/club-deportivo-extremadura/>, <http://lafutbolteca.com/club-de-futbol-extremadura/> |
| Ourense CF as "C.D. Ourense" | disagree | CD Ourense was liquidated on 15 July 2014. Its fan successor is UD Ourense (founded 10 July 2014). Ourense CF is the former Puente or Ponte Ourense (founded 1977) and was renamed in July 2014. The "C.D. Ourense" record fits UD Ourense better, and `spa.md` lists UD Ourense as a missing club in 2025/26. <https://spherasports.com/ourense-sus-clubes-y-la-lucha-por-la-identidad-de-una-ciudad/> |
| CD El Ejido (2022/23) as "Club Polideportivo El Ejido" | agree | CD El Ejido 2012 now uses the name Club Polideportivo El Ejido 1969, so the record name is the name of the same club. A separate Poli Ejido CF (2020) also claims the old club's heritage. <https://www.huelva24.com/albiazules/03032023-el-ejido-un-club-en-disputa-por-ser-heredero-del-historico-poli/> |
| Rayo Cantabria as "Racing Club de Santander B" | agree | Rayo Cantabria is the filial of Racing de Santander. <https://www.elgoldigital.com/futbol/racing-de-santander/alboniga-menor-rayo-cantabria-racing-filial-athletic-2026/> |

## Notes for Max

- The RSSSF headings call Segunda "Liga Adelante" up to 2020/21. That is out of date on RSSSF, so I did not count it as a disagreement with the LaLiga 1|2|3 and SmartBank names.
- The RSSSF 2004/05 Copa note says "354, in 18 groups" for the Tercera of the season whose champions qualified. If that season is 2003/04, the first file's 17 groups for 2003/04 is wrong. The wording is not clear, so spa-22 `clubs` stays `no second source`.
- The first file's Supercopa format for 2019/20 does not say that the league third place (Real Madrid) took the place of the Copa runner-up, which was also the league champion (RSSSF). I marked it `agree` with a note.
