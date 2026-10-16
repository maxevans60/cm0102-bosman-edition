# Brazil rules, second-source check (slice 8, part bra)

Files: `bra.check.tsv` (3,836 rows, one per filled fact field of `bra.tsv`) and this note. The first agent's files are unchanged.

## Method

- I read every page on 2026-10-16. Pages that I read raw with `curl` and grepped: RSSSF (`rsssf.org/tablesb/braz*.html`, `braz2-*`, `braz3-*`, `braz4-09`, `brazcup*`) and RSSSF Brasil (`rsssfbrasil.com`: national divisions `br<year>l2/l3/l4`, cups `cbr<year>`, regional cups, and 225 state championship pages for 2002 to 2026). Pages that I read with WebFetch and asked for a quote: Gazeta Esportiva, Netvasco, Netflu, Lance, CNN Brasil, A Tarde, Tribuna de Jundiaí, O Povo, TNH1, Portal de Prefeitura, Jornal Grande Bahia, and the CBF REC of the Copa do Brasil 2025 (PDF, read with pdftotext).
- Each second source is from another publisher than the first source of that fact. The first agent used en/pt Wikipedia, goal.com, Bahia Notícias, Lance, Times Brasil, Itatiaia, Placar, ge.globo and cbf.com.br. Where Lance was a first source (foreign players), I used Netvasco and Gazeta Esportiva.
- Club counts: a count from a participant list or from the group tables. Where clubs withdrew, I say whether the number counts entrants or clubs that played.
- State championship format: the first agent wrote a keyword summary. I checked only the stage keywords (quarter-finals, semi-finals, quadrangular, hexagonal, groups, Taça Guanabara) against the RSSSF Brasil page. A keyword that is not found gives "no second source", not "disagree". The round-robin type is not checked. `playoffs` = "see format" takes the format verdict.
- Verdict `found`: the first value was `unknown` (or "format unknown") and the second source gives it.
- I did not find a competition-specific second source for substitutions (any competition) or for the bench outside Série A. Those rows are `no second source`.

## Counts per verdict

| Verdict | Rows |
|---|---|
| agree | 2,145 |
| no second source | 1,541 |
| found | 111 |
| disagree | 39 |

Most `no second source` rows are fields that stay `unknown` (state substitutions and bench, work permits, registration, the 2027 and 2028 cups) and the substitution rule.

## Source conflicts listed in bra.md

- Série C 2002, 2003, 2004, 2008: RSSSF gives the number of clubs that played as 61, 93, 60 and 63. These are the pt.wikipedia counts. The en.wikipedia counts (65, 95, 64, 64) include clubs that withdrew (2003: 95 drawn, 2 withdrew). Série C 2005 is also off: RSSSF has 63 (group 1 had 3 clubs), the table has 64.
- Copa do Brasil 2003: RSSSF has 64 clubs in the national competition plus a Paraná qualifying match (Coritiba v Paraná). So 64 in the draw and 65 in all. The table note is correct.
- Copa do Nordeste 2015: RSSSF Brasil lists 20 participants. Use 20.
- Catarinense 2005 (32) and 2017 to 2018 (25): RSSSF Brasil lists 12, 10 and 10 clubs in the first division. The first counts include lower divisions.
- Pernambucano 2023 (13): RSSSF Brasil lists 13. Agree. Pernambucano 2026 (8): agree on 8 clubs, but RSSSF Brasil says "Jaguar foi rebaixado" (1 relegated, not 0).
- Goiano 2026 (0 relegated): the RSSSF Brasil page is not created. No second source.

## Other points for Max

- Copa do Nordeste: the 2018 row counts 20 clubs (with the preliminary phase), the 2019 to 2025 rows count 16 (without). RSSSF Brasil shows 8 preliminary clubs plus 12 direct entrants in those years. Choose one rule.
- Copa Verde 2026: Jornal Grande Bahia counts the 24 clubs of the Copa Norte and the Copa Centro-Oeste as the Copa Verde. The table counts only the 2 finalists.
- Série D 2026 to 2028 and Série C 2026 to 2028 agree with O Povo (1 Oct 2025). O Povo also confirms 4 promoted from Série C 2028 (2 per group). The table marks that value as an inference.
- Squad registration, found: Séries A to C 2020 at most 40 players and 8 replacements (CBF notice of 31 July 2020). From 2027 at most 50 players: Lista A 30 (over 23) and Lista B 20 (under 23), moving to 25/25 by 2030.
- Transfer windows 2027, found: 4 January to 2 March and 19 July to 10 September.


## Club matches (bra-clubs.tsv)

I reviewed the 165 token and id matches in `bra.md` against the club names and home towns on the RSSSF and RSSSF Brasil pages. The other matches are the same club (often a short name, an old name or a later name of one club).

Matches I reject (different clubs):

| Real club | Database club | Reason |
|---|---|---|
| Guarany Futebol Clube | Esporte Clube São Bento (SP) | no common name or town |
| Ivinhema Futebol Clube | EC Santo André | Ivinhema (MS) and Santo André (SP) are different clubs |
| Paragominas Futebol Clube | Esporte Clube Paraguaçuense | Paragominas (PA), Paraguaçu Paulista (SP) |
| Petrolina Social Futebol Clube | Rio Preto Esporte Clube | Petrolina (PE), São José do Rio Preto (SP) |
| Serra Futebol Clube | Serra Talhada Futebol Clube | Serra (ES), Serra Talhada (PE) |
| Sociedade Desportiva Juazeirense | Juazeiro Social Clube | two clubs of Juazeiro (BA); both play the 2012 to 2014 Baiano on RSSSF Brasil |
| Sport Club Penedense | Santa Helena Esporte Clube | Penedo (AL), Santa Helena (GO) |
| Ypiranga Futebol Clube (Erechim) | SE Ypiranga FC | "Sociedade Esportiva Ypiranga Futebol Clube" is a separate club in the 2006 Série C and 2012 Série D lists, so the database name most likely belongs to that club (inference) |
| Comercial Futebol Clube (Ribeirão Preto) | Comercial Esporte Clube | Comercial EC is the Campo Grande (MS) club in RSSSF |
| Rio Branco Futebol Clube | Rio Branco Atlético Clube (ES) | RSSSF Brasil Série D 2021 lists both: Rio Branco FC (Venda Nova do Imigrante) and Rio Branco AC (Vitória). Both rows of 2021 carry db id 9568 |

Matches I cannot confirm (check by hand):

- Ypiranga Clube = Ypiranga (SC): Ypiranga Clube is a Macapá (AP) name.
- Duque de Caxias Futebol Clube = Duquecaxiense Futebol Clube.
- Operário Futebol Clube = Novoperário Futebol Clube, and Operário Futebol Clube Ltda = Operário Futebol Clube (MS or MT club).
- Náutico Futebol Clube = Náutico Esporte Clube.
- Capital Futebol Clube = Capital Clube de Futebol (DF).
- Toledo Esporte Clube = Toledo Colonia Work (PR).
- Vitória Futebol Clube (Brasil) = Vitória FC, and America FC (RN) = América FC, Associação Atlética Portuguesa (Rio de Janeiro) = AA Portuguesa, Esporte Clube XV de Novembro (Piracicaba) = EC XV de Novembro: the database names are ambiguous (several clubs share them).
- Lineage matches that are the same club under a later name: Ipatinga = Betim, Ituiutaba = Boa Esporte, Guaratinguetá = Americana, Grêmio Barueri = Grêmio Prudente, Malutrom = J. Malucelli = Corinthians Paranaense, Desportiva Ferroviária = Desportiva Capixaba. Accept them only if the build wants one record per lineage.

## Spot checks of club lists

I compared, with a name match, every club list for 2016 (Séries A, B, C and D), Série C 2002, 2003, 2004, 2006 and 2008, and Série D 2014 and 2021 with RSSSF and RSSSF Brasil.

- 2016 Série A and Série C: all 20 clubs agree.
- 2016 Série B: 19 agree. The list has "Osasco Sporting" where RSSSF Brasil has Oeste Futebol Clube. "Red Bull Bragantino" is the later name of Clube Atlético Bragantino.
- 2016 Série D: "Associação Atlética Araguaia" and "AA Araguaia" are the same club (listed twice). Paraná Soccer Technical Center (PSTC, Londrina) is missing.
- 2014 Série D: "Ipatinga Futebol Clube" and "Betim" are one club (both db id 5412). Itaporã Futebol Clube (MS) is missing. RSSSF Brasil has 41 clubs.
- 2006 Série C: "Boa Esporte Clube" duplicates Ituiutaba (both db id 5479), "Fast Clube" duplicates "Nacional Fast Clube", and "Vitória Futebol Clube (Brasil)" appears to duplicate Vitória FC (Espírito Santo). EC Ipitanga, EC Novo Hamburgo and Tuna Luso are missing.
- 2003 Série C: the list has SE Guaxupé and Parnahyba, who withdrew (RSSSF). Tuna Luso and Flamengo (PI), who played, are missing.
- 2008 Série C: the list has Ceilândia, who withdrew (RSSSF: "Replacing Ceilândia that withdrew"). Bacabal (MA) and Toledo Colônia Work (PR), who played, are missing.
- Série C 2002 and 2004: no error found.

The pt.wikipedia lists seem to repeat a club under two link titles. A check of every season with a duplicate-name pass is advised before the build.

## Disagreements

Columns: first value, second-source note, first URL, second URL.

| id | Year | Competition | Field | First value | Second source | First URL | Second URL |
|---|---|---|---|---|---|---|---|
| bra-25 | 2026 | nation rules | windows | 5 January to 3 March; 20 July to 11 September | A Tarde: 5 Jan to 3 Mar; complementary domestic window 4 to 27 Mar; 20 Jul to 11 Sep. The first source omits the March domestic window | https://www.itatiaia.com.br/esportes/futebol/futebol-nacional/veja-todas-as-datas-do-novo-calendario-do-futebol-brasileiro-em-2026/ | https://atarde.com.br/esportes/cbf-amplia-janela-de-transferencias-nas-series-a-e-b-do-brasileirao-1380627 |
| bra-82 | 2002 | Série C | clubs | 65 | RSSSF: 66 names listed, 5 withdrew (one replaced), 61 clubs played (as pt.wikipedia); first source 65 | https://en.wikipedia.org/wiki/2002_Campeonato_Brasileiro_S%C3%A9rie_C | https://www.rsssf.org/tablesb/braz3-02.html |
| bra-83 | 2003 | Série C | clubs | 95 | RSSSF: 95 clubs drawn in the groups, 2 withdrew, so 93 played (as pt.wikipedia). 95 counts entrants, 93 counts clubs that played | https://en.wikipedia.org/wiki/2003_Campeonato_Brasileiro_S%C3%A9rie_C | https://www.rsssf.org/tablesb/braz3-03.html |
| bra-84 | 2004 | Série C | clubs | 64 | RSSSF: 60 clubs played (2 withdrew in groups 1 and 3, as pt.wikipedia); first source 64 | https://en.wikipedia.org/wiki/2004_Campeonato_Brasileiro_S%C3%A9rie_C | https://www.rsssf.org/tablesb/braz3-04.html |
| bra-85 | 2005 | Série C | clubs | 64 | RSSSF: 63 clubs played (group 1 had 3 clubs); first source 64 | https://en.wikipedia.org/wiki/2005_Campeonato_Brasileiro_S%C3%A9rie_C | https://www.rsssf.org/tablesb/braz3-05.html |
| bra-85 | 2005 | Série C | format | 16 groups of 4 (home and away), top 2 to two-legged knockout rounds down to 4, final group | RSSSF: group 1 had 3 clubs, so 15 groups of 4 and 1 group of 3; the later stages agree | https://en.wikipedia.org/wiki/2005_Campeonato_Brasileiro_S%C3%A9rie_C | https://www.rsssf.org/tablesb/braz3-05.html |
| bra-88 | 2008 | Série C | clubs | 64 | RSSSF: 63 clubs played (all Rondônia clubs withdrew; group 1 had 3 clubs, as pt.wikipedia); first source 64 | https://en.wikipedia.org/wiki/2008_Campeonato_Brasileiro_S%C3%A9rie_C | https://www.rsssf.org/tablesb/braz3-08.html |
| bra-116 | 2009 | Série D | clubs | 40 | RSSSF: 39 clubs played (Acre had no representative; all its clubs withdrew); first source 40 | https://en.wikipedia.org/wiki/2009_Campeonato_Brasileiro_S%C3%A9rie_D | https://www.rsssf.org/tablesb/braz4-09.html |
| bra-121 | 2014 | Série D | clubs | 40 | RSSSF Brasil lists 41 participants (group A1 had 6 clubs, Rio Branco-AC included); first source 40 | https://en.wikipedia.org/wiki/2014_Campeonato_Brasileiro_S%C3%A9rie_D | http://www.rsssfbrasil.com/tablesae/br2014l4.htm |
| bra-121 | 2014 | Série D | format | 8 groups of 5 (home and away); top 2 to two-legged knockout; quarter-final winners promote | group A1 had 6 clubs, so 7 groups of 5 and 1 group of 6 | https://en.wikipedia.org/wiki/2014_Campeonato_Brasileiro_S%C3%A9rie_D | http://www.rsssfbrasil.com/tablesae/br2014l4.htm |
| bra-150 | 2016 | Copa do Brasil | format | knockout, two-legged ties; away goals except in the final; first two rounds end after the  | RSSSF 2016: "Away goal rule applied on all phases"; no exception for the final (first source: away goals except in the final) | https://en.wikipedia.org/wiki/2016_Copa_do_Brasil | https://www.rsssf.org/tablesb/braz2016.html |
| bra-190 | 2002 | Copa dos Campeões | format | 4 groups of 4; top 2 to two-legged quarter-finals; single-match semi-finals; two-legged fi | RSSSF Brasil: quarter-finals were single matches (played in Fortaleza, Natal, Belém, Teresina); semi-finals single; final two legs; winner to the Libertadores 2003 | https://en.wikipedia.org/wiki/Copa_dos_Campe%C3%B5es | http://www.rsssfbrasil.com/tablesae/camp2002.htm |
| bra-230 | 2015 | Copa do Nordeste | clubs | 16 or 20 (the English page gives both) | RSSSF Brasil lists 20 participants; replace "16 or 20" with 20 | https://en.wikipedia.org/wiki/2015_Copa_do_Nordeste | http://www.rsssfbrasil.com/tablesfq/ne2015.htm |
| bra-295 | 2026 | Copa Norte | format | 2 groups of 6, each club plays the other group once; top 2 to single-match semi-finals; tw | Portal de Prefeitura (Centro-Oeste): top 4 of each group to single-match quarter-finals, then two-legged semi-finals and final. Jornal Grande Bahia (both cups) also has quarter-finals. First source: top 2 to single-match semi-finals | https://en.wikipedia.org/wiki/Copa_Norte | https://jornalgrandebahia.com.br/?p=1329801 |
| bra-322 | 2026 | Copa Centro-Oeste | format | 2 groups of 6, each club plays the other group once; top 2 to single-match semi-finals; tw | Portal de Prefeitura (Centro-Oeste): top 4 of each group to single-match quarter-finals, then two-legged semi-finals and final. Jornal Grande Bahia (both cups) also has quarter-finals. First source: top 2 to single-match semi-finals | https://en.wikipedia.org/wiki/Copa_Centro-Oeste | https://portaldeprefeitura.com.br/esportes/apos-24-anos-cbf-ressurge-com-a-copa-centro-oeste-e-revoluciona/604870/ |
| bra-453 | 2022 | Baiano state championship | down | 2 | RSSSF Brasil: bottom club is relegated (1), first source 2 | https://pt.wikipedia.org/wiki/Campeonato_Baiano_de_Futebol_de_2022 | http://www.rsssfbrasil.com/tablesae/ba2022.htm |
| bra-478 | 2020 | Carioca state championship | down | 1 | RSSSF Brasil: last of Grupo X relegated, then last of Grupo Z relegated, so 2 go down; first source 1 | https://pt.wikipedia.org/wiki/Campeonato_Carioca_de_Futebol_de_2020 | http://www.rsssfbrasil.com/tablesr/rj2020.htm |
| bra-487 | 2002 | Catarinense state championship | clubs | 11 | RSSSF Brasil lists 10 clubs (6 in the first-phase hexagonal plus 4 byes); first source 11 | https://pt.wikipedia.org/wiki/Campeonato_Catarinense_de_Futebol_de_2002 | http://www.rsssfbrasil.com/tablessz/sc2002.htm |
| bra-490 | 2005 | Catarinense state championship | clubs | 32 | 12 participants listed in the first division; 32 counts lower divisions too | https://pt.wikipedia.org/wiki/Campeonato_Catarinense_de_Futebol_de_2005 | http://www.rsssfbrasil.com/tablessz/sc2005.htm |
| bra-497 | 2012 | Catarinense state championship | down | 0 | RSSSF Brasil: bottom 2 relegated (Brusque, Marcílio Dias); first source 0 | https://pt.wikipedia.org/wiki/Campeonato_Catarinense_de_Futebol_de_2012 | http://www.rsssfbrasil.com/tablessz/sc2012.htm |
| bra-498 | 2013 | Catarinense state championship | down | 0 | RSSSF Brasil: bottom 2 relegated (Guarani, Camboriú); first source 0 | https://pt.wikipedia.org/wiki/Campeonato_Catarinense_de_Futebol_de_2013 | http://www.rsssfbrasil.com/tablessz/sc2013.htm |
| bra-502 | 2017 | Catarinense state championship | clubs | 25 | RSSSF Brasil lists 10 clubs in the first division; 25 counts lower divisions too | https://pt.wikipedia.org/wiki/Campeonato_Catarinense_de_Futebol_de_2017 | http://www.rsssfbrasil.com/tablessz/sc2017.htm |
| bra-503 | 2018 | Catarinense state championship | clubs | 25 | RSSSF Brasil lists 10 clubs in the first division; 25 counts lower divisions too | https://pt.wikipedia.org/wiki/Campeonato_Catarinense_de_Futebol_de_2018 | http://www.rsssfbrasil.com/tablessz/sc2018.htm |
| bra-551 | 2012 | Gaúcho state championship | down | 1 | RSSSF Brasil: bottom 2 overall relegated (Avenida, Ypiranga); first source 1 | https://pt.wikipedia.org/wiki/Campeonato_Ga%C3%BAcho_de_Futebol_de_2012_-_S%C3%A9rie_A | http://www.rsssfbrasil.com/tablesr/rs2012.htm |
| bra-573 | 2007 | Goiano state championship | down | 4 | RSSSF Brasil: bottom 2 overall relegated (Rioverdense, Goiânia); first source 4 | https://pt.wikipedia.org/wiki/Campeonato_Goiano_de_Futebol_de_2007 | http://www.rsssfbrasil.com/tablesfq/go2007.htm |
| bra-575 | 2009 | Goiano state championship | down | 6 | RSSSF Brasil: bottom 4 overall relegated; first source 6 | https://pt.wikipedia.org/wiki/Campeonato_Goiano_de_Futebol_de_2009 | http://www.rsssfbrasil.com/tablesfq/go2009.htm |
| bra-576 | 2010 | Goiano state championship | down | 4 | RSSSF Brasil: bottom 2 relegated (Itumbiara, Canedense); first source 4 | https://pt.wikipedia.org/wiki/Campeonato_Goiano_de_Futebol_de_2010 | http://www.rsssfbrasil.com/tablesfq/go2010.htm |
| bra-589 | 2023 | Goiano state championship | down | 1 | RSSSF Brasil: bottom 2 relegated (Grêmio Anápolis and the 12th); first source 1 | https://pt.wikipedia.org/wiki/Campeonato_Goiano_de_Futebol_de_2023 | http://www.rsssfbrasil.com/tablesfq/go2023.htm |
| bra-599 | 2006 | Mineiro state championship | down | 1 | RSSSF Brasil: 2 relegated (Uberlândia, URT); first source 1 | https://pt.wikipedia.org/wiki/Campeonato_Mineiro_de_Futebol_de_2006_-_M%C3%B3dulo_I | http://www.rsssfbrasil.com/tablesfq/mg2006.htm |
| bra-613 | 2020 | Mineiro state championship | down | 4 | RSSSF Brasil: 2 relegated (Villa Nova, Tupynambás); first source 4 | https://pt.wikipedia.org/wiki/Campeonato_Mineiro_de_Futebol_de_2020_-_M%C3%B3dulo_I | http://www.rsssfbrasil.com/tablesfq/mg2020.htm |
| bra-684 | 2010 | Paranaense state championship | down | 3 | RSSSF Brasil: the last 4 are relegated; first source 3 | https://pt.wikipedia.org/wiki/Campeonato_Paranaense_de_Futebol_de_2010 | http://www.rsssfbrasil.com/tablesfq/pr2010.htm |
| bra-700 | 2026 | Paranaense state championship | down | 1 | RSSSF Brasil: Galo Maringá and Andraus relegated (2, one after a relegation play-off); first source 1 | https://pt.wikipedia.org/wiki/Campeonato_Paranaense_de_Futebol_de_2026 | http://www.rsssfbrasil.com/tablesfq/pr2026.htm |
| bra-703 | 2002 | Paulista state championship | clubs | 16 | RSSSF Brasil: 12 clubs in the 2002 first level (the big clubs played the Rio-São Paulo tournament; a superchampionship of 4 followed); first source 16 | https://pt.wikipedia.org/wiki/Campeonato_Paulista_de_Futebol_de_2002 | http://www.rsssfbrasil.com/tablessz/sp2002.htm |
| bra-707 | 2006 | Paulista state championship | down | 6 | RSSSF Brasil: 4 relegated (Guarani, Portuguesa, Portuguesa Santista, Mogi Mirim); first source 6 | https://pt.wikipedia.org/wiki/Campeonato_Paulista_de_Futebol_de_2006 | http://www.rsssfbrasil.com/tablessz/sp2006.htm |
| bra-711 | 2010 | Paulista state championship | down | 5 | RSSSF Brasil: bottom 4 relegated; first source 5 | https://pt.wikipedia.org/wiki/Campeonato_Paulista_de_Futebol_de_2010_-_S%C3%A9rie_A1 | http://www.rsssfbrasil.com/tablessz/sp2010.htm |
| bra-712 | 2011 | Paulista state championship | down | 5 | RSSSF Brasil: bottom 4 relegated; first source 5 | https://pt.wikipedia.org/wiki/Campeonato_Paulista_de_Futebol_de_2011_-_S%C3%A9rie_A1 | http://www.rsssfbrasil.com/tablessz/sp2011.htm |
| bra-713 | 2012 | Paulista state championship | down | 5 | RSSSF Brasil: bottom 4 relegated; first source 5 | https://pt.wikipedia.org/wiki/Campeonato_Paulista_de_Futebol_de_2012_-_S%C3%A9rie_A1 | http://www.rsssfbrasil.com/tablessz/sp2012.htm |
| bra-714 | 2013 | Paulista state championship | down | 5 | RSSSF Brasil: bottom 4 relegated; first source 5 | https://pt.wikipedia.org/wiki/Campeonato_Paulista_de_Futebol_de_2013_-_S%C3%A9rie_A1 | http://www.rsssfbrasil.com/tablessz/sp2013.htm |
| bra-754 | 2026 | Pernambucano state championship | down | 0 | RSSSF Brasil: "Jaguar foi rebaixado" (1 relegated); first source 0 | https://pt.wikipedia.org/wiki/Campeonato_Pernambucano_de_Futebol_de_2026 | http://www.rsssfbrasil.com/tablesfq/pe2026.htm |

## Found values

| id | Year | Competition | Field | Value found | Second URL |
|---|---|---|---|---|---|
| bra-19 | 2020 | nation rules | registration | Séries A, B and C 2020: at most 40 registered players, at most 8 replaced until the final registration date (CBF notice of 31 Jul 2020) | https://www.netvasco.com.br/n/252682/cbf-anuncia-prorrogacao-de-prazos-de-inscricao-de-atletas-para-competicoes-nacionais |
| bra-26 | 2027 | nation rules | registration | at most 50 registered players: Lista A 30 (over 23), Lista B 20 (under 23); split moves to 25/25 by 2030 | https://www.gazetaesportiva.com/?p=3860733 |
| bra-26 | 2027 | nation rules | windows | 4 January to 2 March; 19 July to 10 September (CBF 2027 calendar) | https://tribunadejundiai.com.br/mais/esportes/cbf-divulga-calendario-do-brasil-em-2027/ |
| bra-27 | 2028 | nation rules | registration | at most 50 registered players; the A/B split changes gradually from 30/20 (2027) to 25/25 (2030); 2028 split not stated | https://www.gazetaesportiva.com/?p=3860733 |
| bra-188 | 2027 | Supercopa do Brasil | format | Supercopa Rei: 24 de janeiro (one date, so one match, inference) | https://tribunadejundiai.com.br/mais/esportes/cbf-divulga-calendario-do-brasil-em-2027/ |
| bra-217 | 2002 | Copa do Nordeste | clubs | 16 clubs | http://www.rsssfbrasil.com/tablesfq/ne2002.htm |
| bra-217 | 2002 | Copa do Nordeste | format | single round robin of 16; top 4 to semi-finals and finals; bottom club relegated | http://www.rsssfbrasil.com/tablesfq/ne2002.htm |
| bra-218 | 2003 | Copa do Nordeste | clubs | 13 listed, Confiança marked "withdrew?", so 12 played (8 in the first round plus 4 byes) | http://www.rsssfbrasil.com/tablesfq/ne2003.htm |
| bra-218 | 2003 | Copa do Nordeste | format | knockout: single-match first and second phases (4 byes), two-legged semi-finals and finals | http://www.rsssfbrasil.com/tablesfq/ne2003.htm |
| bra-271 | 2002 | Copa Norte | clubs | 16 clubs | http://www.rsssfbrasil.com/tablesfq/n2002.htm |
| bra-271 | 2002 | Copa Norte | format | 4 groups of 4 (home and away); top 2 to 2 groups of 4 (home and away); group winners play a two-legged final; winner to the Copa dos Campeões | http://www.rsssfbrasil.com/tablesfq/n2002.htm |
| bra-298 | 2002 | Copa Centro-Oeste | clubs | 8 clubs in the main competition (after a qualifying tournament) | http://www.rsssfbrasil.com/tablesfq/co2002.htm |
| bra-298 | 2002 | Copa Centro-Oeste | format | double round robin of 8; top 4 to two-legged semi-finals and final; bottom club relegated; winner to the Copa dos Campeões | http://www.rsssfbrasil.com/tablesfq/co2002.htm |
| bra-420 | 2016 | Primeira Liga | format | 3 groups of 4, single round robin; group winners and the best runner-up to semi-finals, then final | http://www.rsssfbrasil.com/tablesae/1liga2016.htm |
| bra-433 | 2002 | Baiano state championship | down | 1 relegated (Barreiras, 9th) | http://www.rsssfbrasil.com/tablesae/ba2002.htm |
| bra-434 | 2003 | Baiano state championship | clubs | 13 participants listed | http://www.rsssfbrasil.com/tablesae/ba2003.htm |
| bra-435 | 2004 | Baiano state championship | clubs | 13 participants listed | http://www.rsssfbrasil.com/tablesae/ba2004.htm |
| bra-436 | 2005 | Baiano state championship | clubs | 13 participants listed | http://www.rsssfbrasil.com/tablesae/ba2005.htm |
| bra-436 | 2005 | Baiano state championship | down | 1 relegated (Palmeiras, after a relegation play-off) | http://www.rsssfbrasil.com/tablesae/ba2005.htm |
| bra-437 | 2006 | Baiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesae/ba2006.htm |
| bra-437 | 2006 | Baiano state championship | down | 1 relegated (Camaçariense) | http://www.rsssfbrasil.com/tablesae/ba2006.htm |
| bra-438 | 2007 | Baiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesae/ba2007.htm |
| bra-438 | 2007 | Baiano state championship | down | 1 relegated (bottom team, Catuense) | http://www.rsssfbrasil.com/tablesae/ba2007.htm |
| bra-441 | 2010 | Baiano state championship | down | 2 relegated from the relegation group (Madre de Deus, Itabuna) | http://www.rsssfbrasil.com/tablesae/ba2010.htm |
| bra-443 | 2012 | Baiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesae/ba2012.htm |
| bra-461 | 2003 | Carioca state championship | down | 1 relegated (Volta Redonda) | http://www.rsssfbrasil.com/tablesr/rj2003.htm |
| bra-462 | 2004 | Carioca state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesr/rj2004.htm |
| bra-462 | 2004 | Carioca state championship | down | 1 relegated (Bangu, relegation table) | http://www.rsssfbrasil.com/tablesr/rj2004.htm |
| bra-463 | 2005 | Carioca state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesr/rj2005.htm |
| bra-463 | 2005 | Carioca state championship | down | 1 relegated (Olaria, relegation table) | http://www.rsssfbrasil.com/tablesr/rj2005.htm |
| bra-464 | 2006 | Carioca state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesr/rj2006.htm |
| bra-464 | 2006 | Carioca state championship | down | 1 relegated (Portuguesa) | http://www.rsssfbrasil.com/tablesr/rj2006.htm |
| bra-492 | 2007 | Catarinense state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablessz/sc2007.htm |
| bra-492 | 2007 | Catarinense state championship | down | 2 relegated (Joinville, Próspera) | http://www.rsssfbrasil.com/tablessz/sc2007.htm |
| bra-494 | 2009 | Catarinense state championship | down | bottom 2 relegated (Marcílio Dias, Atlético Tubarão) | http://www.rsssfbrasil.com/tablessz/sc2009.htm |
| bra-495 | 2010 | Catarinense state championship | down | bottom 2 relegated (Juventus and Hermann Aichinger, who quit; Chapecoense's relegation reversed) | http://www.rsssfbrasil.com/tablessz/sc2010.htm |
| bra-503 | 2018 | Catarinense state championship | down | bottom 2 relegated to Série B | http://www.rsssfbrasil.com/tablessz/sc2018.htm |
| bra-568 | 2002 | Goiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/go2002.htm |
| bra-568 | 2002 | Goiano state championship | down | 2 relegated (CRAC, Santa Helena) | http://www.rsssfbrasil.com/tablesfq/go2002.htm |
| bra-569 | 2003 | Goiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/go2003.htm |
| bra-569 | 2003 | Goiano state championship | down | 2 relegated (Goiânia, Atlético) | http://www.rsssfbrasil.com/tablesfq/go2003.htm |
| bra-570 | 2004 | Goiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/go2004.htm |
| bra-570 | 2004 | Goiano state championship | down | 2 relegated (Rioverdense, Real) | http://www.rsssfbrasil.com/tablesfq/go2004.htm |
| bra-571 | 2005 | Goiano state championship | clubs | 12 participants listed (Novo Horizonte quit and was replaced) | http://www.rsssfbrasil.com/tablesfq/go2005.htm |
| bra-571 | 2005 | Goiano state championship | down | 2 relegated (Anápolis, Goiânia) | http://www.rsssfbrasil.com/tablesfq/go2005.htm |
| bra-572 | 2006 | Goiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/go2006.htm |
| bra-572 | 2006 | Goiano state championship | down | bottom 2 overall relegated (Goiatuba, Grêmio Anápolis) | http://www.rsssfbrasil.com/tablesfq/go2006.htm |
| bra-574 | 2008 | Goiano state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/go2008.htm |
| bra-574 | 2008 | Goiano state championship | down | bottom 2 overall relegated (Novo Horizonte, Canedense) | http://www.rsssfbrasil.com/tablesfq/go2008.htm |
| bra-577 | 2011 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2011.htm |
| bra-577 | 2011 | Goiano state championship | down | bottom 2 relegated (Santa Helena, Trindade) | http://www.rsssfbrasil.com/tablesfq/go2011.htm |
| bra-578 | 2012 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2012.htm |
| bra-578 | 2012 | Goiano state championship | down | bottom 2 relegated (Morrinhos, Anapolina) | http://www.rsssfbrasil.com/tablesfq/go2012.htm |
| bra-579 | 2013 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2013.htm |
| bra-579 | 2013 | Goiano state championship | down | bottom 2 relegated (Rio Verde, Itumbiara) | http://www.rsssfbrasil.com/tablesfq/go2013.htm |
| bra-580 | 2014 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2014.htm |
| bra-580 | 2014 | Goiano state championship | down | bottom 2 overall relegated (Anápolis, Vila Nova) | http://www.rsssfbrasil.com/tablesfq/go2014.htm |
| bra-581 | 2015 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2015.htm |
| bra-581 | 2015 | Goiano state championship | down | bottom 2 overall relegated (Grêmio Anápolis, Caldas Novas) | http://www.rsssfbrasil.com/tablesfq/go2015.htm |
| bra-582 | 2016 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2016.htm |
| bra-582 | 2016 | Goiano state championship | down | bottom 2 overall relegated (Trindade, Anapolina) | http://www.rsssfbrasil.com/tablesfq/go2016.htm |
| bra-583 | 2017 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2017.htm |
| bra-584 | 2018 | Goiano state championship | clubs | 10 participants listed | http://www.rsssfbrasil.com/tablesfq/go2018.htm |
| bra-584 | 2018 | Goiano state championship | down | bottom 2 overall relegated | http://www.rsssfbrasil.com/tablesfq/go2018.htm |
| bra-595 | 2002 | Mineiro state championship | clubs | 8 participants (América, Atlético, Cruzeiro and Mamoré did not take part) | http://www.rsssfbrasil.com/tablesfq/mg2002.htm |
| bra-595 | 2002 | Mineiro state championship | down | 1 relegated (Uberlândia) | http://www.rsssfbrasil.com/tablesfq/mg2002.htm |
| bra-596 | 2003 | Mineiro state championship | clubs | 13 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2003.htm |
| bra-596 | 2003 | Mineiro state championship | down | 1 relegated (Nacional) | http://www.rsssfbrasil.com/tablesfq/mg2003.htm |
| bra-597 | 2004 | Mineiro state championship | clubs | 14 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2004.htm |
| bra-597 | 2004 | Mineiro state championship | down | 4 relegated (Social, Uberaba, Tupi, Rio Branco) | http://www.rsssfbrasil.com/tablesfq/mg2004.htm |
| bra-600 | 2007 | Mineiro state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2007.htm |
| bra-600 | 2007 | Mineiro state championship | down | bottom 2 relegated (Caldense, América) | http://www.rsssfbrasil.com/tablesfq/mg2007.htm |
| bra-601 | 2008 | Mineiro state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2008.htm |
| bra-601 | 2008 | Mineiro state championship | down | bottom 2 relegated (Ipatinga, Democrata) | http://www.rsssfbrasil.com/tablesfq/mg2008.htm |
| bra-603 | 2010 | Mineiro state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2010.htm |
| bra-603 | 2010 | Mineiro state championship | down | bottom 2 relegated (Uberlândia, Ituiutaba) | http://www.rsssfbrasil.com/tablesfq/mg2010.htm |
| bra-605 | 2012 | Mineiro state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2012.htm |
| bra-605 | 2012 | Mineiro state championship | down | bottom 2 relegated (Democrata, Uberaba) | http://www.rsssfbrasil.com/tablesfq/mg2012.htm |
| bra-607 | 2014 | Mineiro state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/mg2014.htm |
| bra-607 | 2014 | Mineiro state championship | down | bottom 2 relegated (Nacional, Minas) | http://www.rsssfbrasil.com/tablesfq/mg2014.htm |
| bra-677 | 2003 | Paranaense state championship | clubs | 16 participants listed (ADAP played in Ponta Grossa's place) | http://www.rsssfbrasil.com/tablesfq/pr2003.htm |
| bra-677 | 2003 | Paranaense state championship | down | 2 relegated (Cascavel, Portuguesa) | http://www.rsssfbrasil.com/tablesfq/pr2003.htm |
| bra-678 | 2004 | Paranaense state championship | clubs | 16 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2004.htm |
| bra-678 | 2004 | Paranaense state championship | down | 2 relegated from the relegation tournament (Grêmio Maringá, Prudentópolis) | http://www.rsssfbrasil.com/tablesfq/pr2004.htm |
| bra-679 | 2005 | Paranaense state championship | clubs | 16 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2005.htm |
| bra-679 | 2005 | Paranaense state championship | down | 2 relegated (Engenheiro Beltrão, Império) | http://www.rsssfbrasil.com/tablesfq/pr2005.htm |
| bra-680 | 2006 | Paranaense state championship | clubs | 16 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2006.htm |
| bra-680 | 2006 | Paranaense state championship | down | bottom 2 overall relegated (Francisco Beltrão, TCW) | http://www.rsssfbrasil.com/tablesfq/pr2006.htm |
| bra-682 | 2008 | Paranaense state championship | clubs | 16 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2008.htm |
| bra-682 | 2008 | Paranaense state championship | down | bottom 2 relegated (Iguaçu, Portuguesa/CAC) | http://www.rsssfbrasil.com/tablesfq/pr2008.htm |
| bra-683 | 2009 | Paranaense state championship | down | bottom 3 relegated | http://www.rsssfbrasil.com/tablesfq/pr2009.htm |
| bra-689 | 2015 | Paranaense state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2015.htm |
| bra-690 | 2016 | Paranaense state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2016.htm |
| bra-690 | 2016 | Paranaense state championship | down | bottom 2 relegated (Operário, Maringá) | http://www.rsssfbrasil.com/tablesfq/pr2016.htm |
| bra-691 | 2017 | Paranaense state championship | clubs | 12 participants listed | http://www.rsssfbrasil.com/tablesfq/pr2017.htm |
| bra-704 | 2003 | Paulista state championship | down | 2 relegated (Internacional, and Botafogo after the promotion/relegation play-off) | http://www.rsssfbrasil.com/tablessz/sp2003.htm |
| bra-731 | 2003 | Pernambucano state championship | clubs | 10 clubs in the first-phase table | http://www.rsssfbrasil.com/tablesfq/pe2003.htm |
| bra-731 | 2003 | Pernambucano state championship | down | 2 relegated (Intercontinental, 1º de Maio) | http://www.rsssfbrasil.com/tablesfq/pe2003.htm |
| bra-732 | 2004 | Pernambucano state championship | clubs | 10 clubs in the first-phase table | http://www.rsssfbrasil.com/tablesfq/pe2004.htm |
| bra-732 | 2004 | Pernambucano state championship | down | 2 relegated (Central, AGA) | http://www.rsssfbrasil.com/tablesfq/pe2004.htm |
| bra-733 | 2005 | Pernambucano state championship | clubs | 10 clubs in the first-phase table | http://www.rsssfbrasil.com/tablesfq/pe2005.htm |
| bra-733 | 2005 | Pernambucano state championship | down | 2 relegated (Petrolina, Manchete) | http://www.rsssfbrasil.com/tablesfq/pe2005.htm |
| bra-734 | 2006 | Pernambucano state championship | clubs | 10 clubs in the first-phase table | http://www.rsssfbrasil.com/tablesfq/pe2006.htm |
| bra-734 | 2006 | Pernambucano state championship | down | 2 relegated (Estudantes, Desportiva Vitória) | http://www.rsssfbrasil.com/tablesfq/pe2006.htm |
| bra-735 | 2007 | Pernambucano state championship | clubs | 10 clubs in the first-phase table | http://www.rsssfbrasil.com/tablesfq/pe2007.htm |
| bra-735 | 2007 | Pernambucano state championship | down | 2 relegated (Cabense, Belo Jardim) | http://www.rsssfbrasil.com/tablesfq/pe2007.htm |
| bra-736 | 2008 | Pernambucano state championship | clubs | 12 clubs (top 6 to the second stage, bottom 6 to the relegation hexagonal) | http://www.rsssfbrasil.com/tablesfq/pe2008.htm |
| bra-737 | 2009 | Pernambucano state championship | clubs | 12 clubs in the table | http://www.rsssfbrasil.com/tablesfq/pe2009.htm |
| bra-737 | 2009 | Pernambucano state championship | down | 2 relegated (Serrano, Petrolina) | http://www.rsssfbrasil.com/tablesfq/pe2009.htm |
| bra-738 | 2010 | Pernambucano state championship | clubs | 12 clubs in the table | http://www.rsssfbrasil.com/tablesfq/pe2010.htm |
| bra-740 | 2012 | Pernambucano state championship | down | bottom 2 relegated | http://www.rsssfbrasil.com/tablesfq/pe2012.htm |
