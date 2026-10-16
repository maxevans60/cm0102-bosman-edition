# Second-source check: Portugal (part `por`)

Date 2026-10-16. Files: `por.check.tsv` (1,419 rows, one per filled fact field of `por.tsv`) and this note. The first agent's files are unchanged.

## Method

- I checked every filled fact field (clubs, up, down, playoffs, split, format, subs_used, bench, foreign, work_permit, registration, windows, name) of all 243 rows. Fields with `n/a` or an empty value have no check row.
- The first source of nearly every fact is en.wikipedia. No second source is a Wikipedia edition. I read every second-source page on 2026-10-16 with `curl` and grep, or through a search tool with the quoted text checked.
- League tables, promotion, relegation and play-offs: RSSSF season pages `port02.html` to `port09.html` and `port2010.html` to `port2026.html`. RSSSF has the lower divisions for 2001/02 to 2005/06 and 2007/08, Liga 3 from 2021/22, and the Campeonato de Portugal promotion lists from 2020/21. It has no lower-division tables for 2006/07 and 2008/09 to 2019/20.
- Club counts: Transfermarkt tables for the top two tiers (2002/03 to 2026/27), Liga 3 (2021/22 to 2026/27) and the Campeonato de Portugal series A to D (2013/14 to 2026/27). Transfermarkt has no series E to H, so the 8-series seasons are only partly covered.
- Cups: RSSSF season pages (rounds, replays, legs, final venues) and the RSSSF Super Cup list `portsupcuphist.html`. The RSSSF Taça de Portugal match lists are incomplete after 2001/02, so most entrant counts have no second source. I counted Taça da Liga entrants from the RSSSF round lists where they are complete.
- Substitutes and bench: Renascença (rr.pt, 9 May 2020 and 8 June 2020), Correio da Manhã (June 2020), the Lusa text of 15 May 2023 on pt.soccerway.com, Maisfutebol (May 2026, bench of 12) and Record (February 2026, Taça de Portugal extra-time substitution).
- Lower-tier formats: Bola na Rede and PortuGOAL (2017/18), Maisfutebol (April 2020 and 2020/21 format), Record (2020/21 format).
- Nation rules: Cuatrecasas on FIFA RSTP article 6 (windows), A Bola (July 2024) and Record (June 2024) on Decree-Law 37-A/2024. FPF and zerozero.pt refuse automated reads (HTTP 403), so no FPF regulation text was read.
- Rows for 2027/28, and the 2026/27 rules that are not yet played out, have no second source.

## Counts

| Verdict | Rows |
|---|---|
| agree | 904 |
| disagree | 15 |
| found | 2 |
| no second source | 498 |
| total | 1,419 |

| Field | agree | disagree | found | no second source |
|---|---|---|---|---|
| clubs | 115 | 0 | 0 | 60 |
| up | 54 | 1 | 0 | 18 |
| down | 61 | 8 | 0 | 31 |
| playoffs | 27 | 2 | 0 | 24 |
| split | 69 | 0 | 0 | 31 |
| format | 181 | 3 | 0 | 32 |
| subs_used | 127 | 0 | 0 | 48 |
| bench | 22 | 0 | 2 | 151 |
| foreign | 0 | 0 | 0 | 27 |
| work_permit | 2 | 0 | 0 | 25 |
| registration | 1 | 0 | 0 | 27 |
| windows | 25 | 0 | 0 | 2 |
| name | 220 | 1 | 0 | 22 |

Most "no second source" rows are bench sizes before 2019/20, FPF substitute rules from 2020/21 (an inference in the first file), nation rules, Taça de Portugal entrant counts, and lower-tier details for 2006/07 and 2008/09 to 2016/17.

## Disagreements

| id | Season, competition, field | First value | First URL | Second value | Second URL |
|---|---|---|---|---|---|
| por-3 | 2001/02 Segunda Divisão B, playoffs | none | https://en.wikipedia.org/wiki/2001%E2%80%9302_Segunda_Divis%C3%A3o_B | 16th of each zone to a relegation play-off (RSSSF NB) | https://www.rsssf.org/tablesp/port02.html |
| por-6 | 2001/02 Taça de Portugal, format | draws replayed in rounds 1 to 5 | https://en.wikipedia.org/wiki/2001%E2%80%9302_Ta%C3%A7a_de_Portugal | replays in rounds 1 to 5, plus a 1/8-final replay (9 Jan) and a quarter-final replay (30 Jan) | https://www.rsssf.org/tablesp/port02.html |
| por-12 | 2002/03 Segunda Divisão B, down | 12 (Norte 4, Centro 3, Sul 5) | https://en.wikipedia.org/wiki/2002%E2%80%9303_Segunda_Divis%C3%A3o_B | 11 (North 4, Central 3, South 4) | https://www.rsssf.org/tablesp/port03.html |
| por-21 | 2003/04 Segunda Divisão B, down | 11 (Norte 3, Centro 4, Sul 4) | https://en.wikipedia.org/wiki/2003%E2%80%9304_Segunda_Divis%C3%A3o_B | 12 (North 4: Caçador Taipas, Bragança, Ermesinde, Leça; none of them is in the 2004/05 North table) | https://www.rsssf.org/tablesp/port04.html |
| por-23 | 2003/04 Terceira Divisão, down | about 4 per mainland series | https://en.wikipedia.org/wiki/2003%E2%80%9304_Terceira_Divis%C3%A3o | 3 per mainland series (16th to 18th) | https://www.rsssf.org/tablesp/port04.html |
| por-30 | 2004/05 Segunda Divisão B, playoffs | none | https://en.wikipedia.org/wiki/2004%E2%80%9305_Segunda_Divis%C3%A3o_B | South 12th Lusitânia played the Açores champion of the fourth level and was relegated | https://www.rsssf.org/tablesp/port05.html |
| por-39 | 2005/06 Segunda Divisão, down | 13 (3, 3, 3, 4) | https://en.wikipedia.org/wiki/2005%E2%80%9306_Segunda_Divis%C3%A3o | 15 (3, 3, 3, 6: Série D 11th to 16th) | https://www.rsssf.org/tablesp/port06.html |
| por-41 | 2005/06 Terceira Divisão, down | about 4 per mainland series | https://en.wikipedia.org/wiki/2005%E2%80%9306_Terceira_Divis%C3%A3o | A 6, B 5, C 5, D 4, E 6 (RSSSF has no Série F) | https://www.rsssf.org/tablesp/port06.html |
| por-57 | 2007/08 Segunda Divisão, down | 15 | https://en.wikipedia.org/wiki/2007%E2%80%9308_Segunda_Divis%C3%A3o | 19 (A 5, B 4, C 5, D 5: bottom 2 of each relegation group plus the worst runners-up) | https://www.rsssf.org/tablesp/port08.html |
| por-70 | 2008/09 Supertaça, format (date) | 16 August 2008 | https://en.wikipedia.org/wiki/2008_Superta%C3%A7a_C%C3%A2ndido_de_Oliveira | 15 August 2008 | https://www.rsssf.org/tablesp/portsupcuphist.html |
| por-92 | 2011/12 Liga de Honra, down | 2 | https://en.wikipedia.org/wiki/2011%E2%80%9312_Liga_de_Honra | 0 (RSSSF NB: União de Leiria demoted and Varzim refused promotion, so no club was relegated) | https://www.rsssf.org/tablesp/port2012.html |
| por-102 | 2012/13 Segunda Divisão, up | 2 (play-off of zone winners) | https://en.wikipedia.org/wiki/2012%E2%80%9313_Segunda_Divis%C3%A3o | 3 promoted: Chaves, Farense, Académico de Viseu | https://www.rsssf.org/tablesp/port2013.html |
| por-119 | 2014/15 Segunda Liga, down | 3 (23rd, 24th, and Marítimo B to district level) | https://en.wikipedia.org/wiki/2014%E2%80%9315_Segunda_Liga | 3, but they are 23rd Marítimo B, 24th Trofense and 10th Beira-Mar (demoted for finances); 22nd Atlético stayed | https://www.rsssf.org/tablesp/port2015.html |
| por-173 | 2020/21 second tier, name | Liga Portugal 2 (Liga Portugal SABSEG) | https://en.wikipedia.org/wiki/Liga_Portugal_2 | LigaPro (the name Liga Portugal 2 first appears for 2021/22) | https://www.rsssf.org/tablesp/port2021.html |
| por-223 | 2025/26 Supertaça, format (date and venue) | Estádio Algarve, August 2025 | https://en.wikipedia.org/wiki/2025_Superta%C3%A7a_C%C3%A2ndido_de_Oliveira | Faro/Loulé (Estádio Algarve), 31 July 2025 | https://www.rsssf.org/tablesp/portsupcuphist.html |

Notes on the table:

- por-6, por-12, por-21, por-39, por-57 and por-92 change a count that the build uses. The RSSSF value comes from the final tables and notes.
- por-70 is a one-day difference. A search result also gives 16 August, so I don't know which date is right.
- Two RSSSF tables do not match the next season. In 2003/04 and 2004/05 RSSSF marks the 16th to 18th of the second tier as relegated. The next season's tables agree with the first file (Salgueiros out in 2004, Felgueiras and Alverca out in 2005), so I marked both agree.
- por-164 (2019/20 second tier, down 2): Correio da Manhã (June 2020) confirms the decision to relegate Cova da Piedade and Casa Pia. RSSSF adds that both later stayed up when Setúbal and Aves were demoted. Marked agree with that note.

## Found values

| id | Field | Value | URL |
|---|---|---|---|
| por-164 | bench (2019/20 LigaPro) | 7. The Liga assembly of 8 June 2020 raised the bench from 7 to 9 for the rest of the I Liga, after this competition had stopped. | https://rr.pt/bola-branca/noticia/futebol-nacional/2020/06/08/clubes-aprovam-cinco-substituicoes-e-nove-suplentes-ate-final-da-epoca/195917/ |
| por-170 | bench (2019/20 Taça da Liga) | 7. Same source, and the cup ended in January 2020. | same |

I found no source for the bench size before 2019/20, for foreign-player rules before 2024, or for the 2026/27 Campeonato de Portugal promotion places.

## Clubs file

Method: I checked every row of `por-clubs.tsv` against the season database in `research-0.6.0/db-divisions/` (the database of each season from the spec table). For every filled `db_club_id` I confirmed that the id, name, division and nation match. I compared the real club lists of one season per division with a second source:

| Season and division | Second source | Result |
|---|---|---|
| 2002/03 Segunda Divisão B Centro (19) | RSSSF | same clubs |
| 2002/03 Terceira Divisão Série A (18) | RSSSF | same clubs |
| 2005/06 Segunda Divisão Série A (14) | RSSSF | same clubs |
| 2005/06 Terceira Divisão Série A (18) | RSSSF | same clubs |
| 2007/08 Primeira Liga (16), Liga de Honra (16) | Transfermarkt | same clubs |
| 2012/13 Segunda Liga (22) | Transfermarkt | same clubs |
| 2014/15 Campeonato Nacional de Seniores Série B (10) | Transfermarkt | same clubs |
| 2018/19 Campeonato de Portugal Série A (18) | Transfermarkt | same clubs (Mirandês is listed there as GDM 1968) |
| 2022/23 Liga 3 Séries A and B (12 each) | Transfermarkt | same clubs |
| 2025/26 Campeonato de Portugal Série D (14) | Transfermarkt | same clubs |

Wrong or doubtful database matches:

- Atlético CP in Liga 3 Série B, 2023/24, 2024/25 and 2025/26, is matched to id 2656 "Sporting Clube de Portugal". That is wrong. The October 2023 database holds no Atlético club, so the id should be empty.
- Lusitano FCV in 2014/15 CNS Série D is matched to 6845 "Lusitano Futebol Clube VRSA". The club is Lusitano de Vildemoinhos (Viseu), id 1911 "Lusitano Futebol Clube Viseu" in October 2014.
- Académica – SF in 2015/16 (Série E) and 2016/17 (Série D) is matched to 218 "Associação Académica de Coimbra", the professional club, which plays in the top two tiers in those databases. Académica – SF is a separate amateur club. The id should be empty.
- The other low-overlap matches (for example Atlético CP to Atlético Clube de Portugal, AA Avanca, O Elvas) are correct.

Spelling misses: these "missing" clubs (empty `db_club_id`) exist in the season database under another name.

| Real name in the file | Database club (id) | Seasons |
|---|---|---|
| Lusitânia Lourosa | Lusitânia Futebol Clube (6765) | 2001/02, 2002/03, 2003/04, 2005/06, 2007/08 to 2010/11 |
| Estrela Portalegre | Sport Clube Estrela (3676) | 2001/02, 2002/03, 2007/08 |
| Juventude Évora, Juventude de Évora | Juventude Sport Clube (5676) | 2001/02, 2002/03, 2003/04, 2005/06, 2008/09 to 2010/11, 2012/13 |
| Lusitano Évora, Lusitano GC Évora | Lusitano Ginásio Clube (6840) | 2001/02, 2002/03, 2005/06, 2007/08 to 2009/10 |
| Académico Viseu | Clube Académico de Futebol (215) | 2002/03, 2007/08 to 2009/10, 2011/12 |
| SC São João de Ver, S. João de Ver, São João Ver | Sporting Clube S.J. Ver (9797) | 2002/03, 2003/04, 2005/06, 2007/08 to 2014/15 |
| União Funchal | Clube Futebol União Madeira (11914) | 2005/06, 2008/09 |
| Operário Açores, Operário dos Açores | Clube Operário Desportivo (8307) | 2005/06, 2008/09 to 2010/11 |
| Lusitano de Vildemoinhos | Lusitano Futebol Clube Viseu (1911) | 2015/16 |
| Armacenenses | Clube de Futebol Os Armacenences (11305) | 2016/17 to 2018/19 |
| União Almeirim | UFC Almeirim (6382) | 2020/21 |
| Sp. Ideal | SC Ideal (3382) | 2021/22 |
| Sp. Pombal | SC Pombal (10809) | 2024/25 |
| Oriental de Lisboa | Clube Oriental Lissabon (8329) | 2025/26 |

These names in the `por.md` table are spelling misses: Lusitânia Lourosa, Estrela Portalegre, Académico Viseu, SC São João de Ver (all spellings), União Funchal, Operário Açores, Operário dos Açores, Juventude de Évora, Lusitano de Vildemoinhos, Armacenenses, Sp. Ideal, Sp. Pombal and Oriental de Lisboa. In all, 55 of the 410 empty-id rows are spelling misses. The other names in that table were not found in the season database. The B teams (Chaves B, Varzim B, Rio Ave B, Alverca B, Portimonense B, Belenenses SAD B, and Vitória de Guimarães B to 2016/17) have no B record in their season database, so they are true misses. I don't know if GRAP (2020/21) is the database club FC Pedras Rubras (8650), so I did not count it.
