# Brazil rules, 2001/02 to 2027/28 (slice 8, part bra)

Files: `bra.tsv` (756 rows), `bra-clubs.tsv` (2,689 rows) and this note.

## Method

- Brazil plays in calendar years. Each row follows the brief: the 2001/02 row holds the year 2002, and the 2027/28 row holds 2028. `real_season` gives the year.
- League and cup facts come from the English Wikipedia season pages, read raw on 2026-10-16. Club counts of the cups and the club lists come from the Portuguese Wikipedia season pages, which list every participant in one format.
- The 2026 to 2028 changes come from the CBF calendar announcement (cbf.com.br, "CBF anuncia novo calendário do futebol profissional masculino") and from ge.globo of 1 October 2025 on the Série C.
- Substitutes: goal.com (3 until 2019, 5 from 2020 in three stoppages). Bench: Bahia Notícias (7 named substitutes, 12 from 1 January 2013). Both sources speak of CBF competitions in general. Their use for Séries B to D and the cups is an inference.
- Foreign players: Lance and Times Brasil, two publishers. 3 per match squad before 2014, 5 from 2014, 7 in 2023, 9 from 2024, then 8 in 2027 and 7 in 2028 (approved by the clubs in September 2026). Lance alone gives the value before 2013.
- Transfer windows: found for 2024, 2025 and 2026 only. Work permits and squad registration: unknown.
- State championships: club count, relegation count and the rules section of each pt.wikipedia edition page. The format column holds a keyword summary of the Portuguese rules text. It is marked as an inference. Substitutes and bench of the state championships are unknown, because no state regulation was read.

## Original files and the real competitions

The names come from the original's `club_comp.dat` (3.9.68). The club field at +0x60 of `club.dat` holds the state championship of each Brazilian club. The team buffer sizes in the function map agree with the club counts (for example 0x3b0 / 0x3b = 16 for the Northeast).

| File | Record | Original name | Real competition | Clubs in 3.9.68 |
|---|---|---|---|---|
| bra_nat_first | 65 | Brazilian National First Division | Série A | 28 |
| bra_nat_second | 79 | Brazilian National Second Division | Série B | 28 |
| bra_nat_third | 80 | Brazilian National Third Division | Série C | 56 |
| bra_cup | 68 | Brazilian Cup | Copa do Brasil | |
| bra_champ_cup | 270 | Brazilian Champions Cup | Copa dos Campeões (2000 to 2002) | |
| bra_reg_sp | 67 | São Paulo State Championship | Campeonato Paulista Série A1 | 16 |
| bra_reg_rio | 66 | Rio de Janeiro State Championship | Campeonato Carioca | 12 |
| bra_reg_minas_gerais | 262 | Minas Gerais State Championship | Campeonato Mineiro Módulo I | 12 |
| bra_reg_gaucho | 254 | Gaúcho State Championship | Campeonato Gaúcho | 17 |
| bra_reg_parana | 266 | Paraná State Championship | Campeonato Paranaense | 12 |
| bra_reg_santa | 268 | Santa Catarina State Championship | Campeonato Catarinense | 10 |
| bra_reg_goias | 260 | Goiás State Championship | Campeonato Goiano | 14 |
| bra_reg_pern | 264 | Pernambuco State Championship | Campeonato Pernambucano | 10 |
| bra_reg_bahia | 258 | Bahia State Championship | Campeonato Baiano | 10 |
| bra_reg_north | 73 | North State Championship | none: clubs of Maranhão, Amazonas and Pará | 10 |
| bra_reg_northeast | 256 | Northeast State Championship | none: clubs of Ceará, Rio Grande do Norte, Paraíba, Alagoas and Sergipe | 16 |
| bra_reg_central | 74 | Central State Championship | none: clubs of the Federal District, Espírito Santo, Mato Grosso, Mato Grosso do Sul and Tocantins | 15 |

- bra_reg_northeast is a state championship of five small states. It is not the Copa do Nordeste. The data has a separate "Northeast Regional Cup" record (272), and the Copa do Nordeste includes Bahia and Pernambuco clubs. This mapping is an inference from the data. The code was not read.
- The data also holds records with no source file: North, Northeast and Central Regional Cups (271 to 273), South-Minas Regional Cup (274), Rio-São Paulo Tournament (275) and Taça Brasil (276). The table gives the real cups as `new` rows.
- Added as `new`: Série D (2009 on), Supercopa do Brasil (2020 on), Copa do Nordeste, Copa Verde (2014 on), Copa Norte and Copa Centro-Oeste (2002, then 2026), Copa Sul-Sudeste (2026), Torneio Rio-São Paulo and Copa Sul-Minas (2002 only) and the Primeira Liga (2016 and 2017).

## Main change points

- Série A: 2002 26 clubs, single round robin and an 8-club knockout. 2003 24 clubs, double round robin, 2 down. 2004 24 clubs, 4 down. 2005 22 clubs, 4 down. 2006 on 20 clubs, 4 down. 2026 runs from January to December.
- Série B: 26 (2002), 24 (2003, 2004), 22 (2005) with group play-offs and 2 up. 20 clubs, 4 up and 4 down from 2006.
- Série C: 65, 95, 64, 64, 63, 64, 64 clubs in 2002 to 2008, no relegation. 20 clubs from 2009 with 4 up and 4 down (21 in 2013). 2 down in 2026 and 2027, 24 clubs in 2027, 28 clubs in two groups of 14 with 6 down from 2028.
- Série D: 40 (2009 to 2015), 68 (2016 to 2021), 64 (2022 to 2025), 96 with 6 up (2026 on).
- Copa do Brasil: 64 clubs to 2012, 86 to 92 clubs 2013 to 2025, 126 in 2026, 128 in 2027. Single-match first rounds from 2017. No away goals from 2018. Single-match final from 2026.
- Substitutes 3 to 5 in 2020. Bench 7 to 12 in 2013.

## Disagreements between sources (for Max)

- Série C clubs: 2002 65 (en) or 61 (pt), 2003 95 or 93, 2004 64 or 60, 2008 64 or 63. The table uses the English count. The club lists use the Portuguese pages.
- Copa do Brasil 2003: 65 clubs with a preliminary match (en) or 64 (pt).
- Copa do Nordeste 2015: the English page gives 16 in its infobox and 20 in its text.
- State pages with odd counts: Catarinense 2005 (32) and 2017 to 2018 (25) may count several divisions. Pernambucano 2023 (13) and 2026 (8) and Goiano 2026 (0 relegated) need a check.

## Unknowns

- State championship editions with no page found on pt.wikipedia: Carioca 2004, Mineiro 2002, Mineiro 2003, Mineiro 2004, Mineiro 2007, Mineiro 2008, Mineiro 2010, Mineiro 2011, Mineiro 2012, Mineiro 2014, Paranaense 2002, Paranaense 2003, Paranaense 2004, Paranaense 2005, Paranaense 2006, Paranaense 2008, Paranaense 2015, Paranaense 2016, Paranaense 2017, Catarinense 2007, Goiano 2002, Goiano 2003, Goiano 2004, Goiano 2005, Goiano 2006, Goiano 2008, Goiano 2011, Goiano 2012, Goiano 2013, Goiano 2014, Goiano 2015, Goiano 2016, Goiano 2017, Goiano 2018, Pernambucano 2002, Pernambucano 2003, Pernambucano 2004, Pernambucano 2005, Pernambucano 2006, Pernambucano 2007, Pernambucano 2008, Pernambucano 2009, Pernambucano 2010, Baiano 2003, Baiano 2004, Baiano 2005, Baiano 2006, Baiano 2007.
- Copa do Nordeste 2002 and 2003, Copa Norte and Copa Centro-Oeste 2002: held, club count unknown.
- Transfer windows before 2024, work permits and squad registration for every year.
- 2027 and 2028: Série A and B are an inference (no change announced). Copa do Brasil 2028, Supercopa, Copa do Nordeste and the state championships are unknown.

## Club moves

Every season database keeps the original's sizes: 28, 28 and 56 clubs in the three national divisions, plus "A Lower Division" from November 2007 on. Every real division differs in size, so `bra-clubs.tsv` lists every club of Séries A to D for each season with a database: 2002, 2003, 2004, 2006 and 2008 to 2026 (no database for 2005 and 2007 starts, 2004/05 and 2006/07). The years 2027 and 2028 have no database.

Matching: exact name first, then the same club id found by name in another season database, then a token match of at least 0.5. 41 wrong token matches were removed by hand. The remaining token and id matches are listed below for the checker. `db_club_id` is empty when no club matched.

Real clubs per division and their division in the database:

| Year | Real division | Clubs | Database division of those clubs |
|---|---|---|---|
| 2002 | Série A | 26 | First 24; Second 2 |
| 2002 | Série B | 26 | Second 19; First 4; Third 2; none 1 |
| 2002 | Série C | 61 | Third 27; none 22; not found 8; Second 4 |
| 2003 | Série A | 24 | First 20; Second 4 |
| 2003 | Série B | 24 | Second 13; First 7; Third 3; none 1 |
| 2003 | Série C | 93 | none 36; Third 31; not found 16; Second 9; First 1 |
| 2004 | Série A | 24 | First 21; Second 3 |
| 2004 | Série B | 24 | Second 13; First 5; Third 5; none 1 |
| 2004 | Série C | 60 | none 21; Third 18; not found 13; Second 7; First 1 |
| 2006 | Série A | 20 | First 18; Second 2 |
| 2006 | Série B | 20 | Second 9; First 6; Third 4; none 1 |
| 2006 | Série C | 63 | Third 19; not found 18; none 17; Second 6; First 3 |
| 2008 | Série A | 20 | First 17; Second 3 |
| 2008 | Série B | 20 | First 10; Second 9; Third 1 |
| 2008 | Série C | 63 | none 17; Third 13; A Lower 13; Second 10; not found 9; First 1 |
| 2009 | Série A | 20 | First 16; Second 4 |
| 2009 | Série B | 20 | Second 8; First 8; Third 3; A Lower 1 |
| 2009 | Série C | 20 | Third 12; First 4; Second 4 |
| 2009 | Série D | 39 | none 13; A Lower 10; Third 9; Second 4; not found 3 |
| 2010 | Série A | 20 | First 17; Second 3 |
| 2010 | Série B | 20 | Second 10; First 10 |
| 2010 | Série C | 20 | Second 12; Third 6; A Lower 1; First 1 |
| 2010 | Série D | 40 | Third 21; A Lower 8; not found 4; none 4; Second 3 |
| 2011 | Série A | 20 | First 18; Second 2 |
| 2011 | Série B | 20 | Second 9; First 8; Third 2; not found 1 |
| 2011 | Série C | 20 | Second 12; Third 5; A Lower 2; First 1 |
| 2011 | Série D | 40 | none 11; not found 10; Third 9; A Lower 7; Second 2; First 1 |
| 2012 | Série A | 20 | First 18; Second 2 |
| 2012 | Série B | 20 | First 10; Second 9; Third 1 |
| 2012 | Série C | 20 | Second 13; Third 6; not found 1 |
| 2012 | Série D | 40 | A Lower 21; Third 10; not found 6; Second 3 |
| 2013 | Série A | 20 | First 19; Second 1 |
| 2013 | Série B | 20 | Second 11; First 8; Third 1 |
| 2013 | Série C | 21 | Second 12; A Lower 4; Third 4; First 1 |
| 2013 | Série D | 40 | A Lower 17; Third 15; not found 6; Second 2 |
| 2014 | Série A | 20 | First 19; Second 1 |
| 2014 | Série B | 20 | Second 9; First 9; Third 2 |
| 2014 | Série C | 20 | Second 12; A Lower 4; Third 4 |
| 2014 | Série D | 41 | A Lower 18; Third 11; not found 8; Second 4 |
| 2015 | Série A | 20 | First 20 |
| 2015 | Série B | 20 | Second 12; First 8 |
| 2015 | Série C | 20 | Second 12; Third 6; A Lower 2 |
| 2015 | Série D | 40 | A Lower 21; Third 13; Second 3; not found 3 |
| 2016 | Série A | 20 | First 19; Second 1 |
| 2016 | Série B | 20 | Second 10; First 8; Third 2 |
| 2016 | Série C | 20 | Second 13; Third 6; First 1 |
| 2016 | Série D | 68 | A Lower 29; Third 21; not found 14; Second 4 |
| 2017 | Série A | 20 | First 18; Second 2 |
| 2017 | Série B | 20 | Second 10; First 9; none 1 |
| 2017 | Série C | 20 | Second 10; Third 7; A Lower 2; First 1 |
| 2017 | Série D | 68 | A Lower 30; Third 20; not found 16; Second 2 |
| 2018 | Série A | 20 | First 17; Second 3 |
| 2018 | Série B | 20 | Second 10; First 8; Third 2 |
| 2018 | Série C | 20 | Second 7; Third 7; A Lower 3; First 2; not found 1 |
| 2018 | Série D | 68 | A Lower 33; not found 15; Third 15; Second 4; First 1 |
| 2019 | Série A | 20 | First 16; Second 4 |
| 2019 | Série B | 20 | Second 10; First 8; Third 2 |
| 2019 | Série C | 20 | Third 8; Second 7; First 2; A Lower 2; not found 1 |
| 2019 | Série D | 68 | A Lower 33; Third 16; not found 13; Second 4; First 2 |
| 2020 | Série A | 20 | First 16; Second 4 |
| 2020 | Série B | 20 | Second 11; First 6; Third 3 |
| 2020 | Série C | 20 | Third 10; Second 5; First 3; A Lower 2 |
| 2020 | Série D | 68 | A Lower 34; Third 19; not found 11; First 2; Second 2 |
| 2021 | Série A | 20 | First 16; Second 4 |
| 2021 | Série B | 20 | Second 10; First 7; Third 3 |
| 2021 | Série C | 20 | Third 8; Second 7; A Lower 4; First 1 |
| 2021 | Série D | 68 | A Lower 32; Third 23; not found 7; First 3; Second 3 |
| 2022 | Série A | 20 | First 16; Second 4 |
| 2022 | Série B | 20 | Second 11; First 6; Third 3 |
| 2022 | Série C | 20 | Third 10; Second 5; First 3; A Lower 2 |
| 2022 | Série D | 64 | A Lower 30; Third 18; not found 12; Second 3; First 1 |
| 2023 | Série A | 20 | First 16; Second 4 |
| 2023 | Série B | 20 | Second 10; First 7; Third 3 |
| 2023 | Série C | 20 | Third 9; Second 6; A Lower 3; First 2 |
| 2023 | Série D | 64 | A Lower 32; Third 21; not found 7; Second 3; First 1 |
| 2024 | Série A | 20 | First 19; Second 1 |
| 2024 | Série B | 20 | Second 13; First 6; Third 1 |
| 2024 | Série C | 20 | Third 10; Second 7; First 2; A Lower 1 |
| 2024 | Série D | 64 | A Lower 27; Third 25; not found 10; Second 1; First 1 |
| 2025 | Série A | 20 | First 17; Second 3 |
| 2025 | Série B | 20 | Second 8; First 8; Third 4 |
| 2025 | Série C | 20 | Second 9; Third 7; First 2; A Lower 2 |
| 2025 | Série D | 64 | A Lower 27; Third 21; not found 10; Second 5; First 1 |
| 2026 | Série A | 20 | First 16; Second 4 |
| 2026 | Série B | 20 | First 9; Second 8; Third 3 |
| 2026 | Série C | 20 | Second 8; Third 8; A Lower 3; First 1 |
| 2026 | Série D | 96 | Third 31; A Lower 31; not found 25; Second 6; First 2; none 1 |

Q244 note: the national divisions of the databases hold more clubs than the real ones (28 against 20), so the build must also take clubs out of them. Série D has no database division.

### Missing clubs (no club in the database matched), for Max

- 2002 Série C (8): CFZ de Brasília Sociedade Esportiva, União Bandeirante Futebol Clube, Sport Club Ulbra, Esporte Clube São Gabriel, Sociedade Esportiva Santa Inês, Tocantins Esporte Clube (Maranhão), Palmeiras Nordeste Futebol Ltda., Grêmio Esportivo Anápolis
- 2003 Série C (16): Centro de Futebol Amazônia, Esporte Clube Viana, Chapadinha Futebol Clube, Sociedade Esportiva Santa Inês, Parnahyba Sport Club, Esporte Clube Taveirópolis, Cuiabá Esporte Clube, Sociedade Esportiva e Recreativa Chapadão, Grêmio Esportivo Jaciara, CFZ de Brasília Sociedade Esportiva, Rio Branco de Andradas Futebol Clube, Clube Esportivo Rio Branco, Sertãozinho Futebol Clube, União Bandeirante Futebol Clube, Canoas Sport Club, RS Futebol
- 2004 Série C (13): Grêmio Atlético Coariense, Trem Desportivo Clube, Cuiabá Esporte Clube, CFZ de Brasília Sociedade Esportiva, Associação Desportiva Limoeiro Futebol Clube, Parnahyba Sport Club, Associação Atlética Coruripe, Sociedade Esportiva e Recreativa Chapadão, Atlético de Ibirama, Cianorte Futebol Clube, Sport Club Ulbra, Clube Atlético Lages, Itacuruba Sport Club
- 2006 Série C (18): Associação Desportiva Senador Guiomard, Fast Clube, Clube Municipal Ananindeua, Amapá Clube, Araguaína Futebol e Regatas, Sport Club Ulbra Ji-Paraná, Sociedade Esportiva Ypiranga Futebol Clube, Olímpico Pirambu Futebol Clube, Associação Atlética Luziânia, Coxim Atlético Clube, Sociedade Esportiva e Recreativa Chapadão, Grêmio Barueri, Vitória Futebol Clube (Espírito Santo), Sport Club Ulbra, Associação Atlética Coruripe, Associação Desportiva Recreativa Cultural Icasa, Vitória Futebol Clube (Brasil), Associação Desportiva Atlética do Paraná
- 2008 Série C (9): Luverdense Esporte Clube, Holanda Esporte Clube, Cristal Atlético Clube, Horizonte Futebol Clube, Salgueiro Atlético Clube, Sport Club Santa Cruz, Legião Futebol Clube, Linhares Futebol Clube, Clube Atlético Linense
- 2009 Série D (3): Sport Club Genus de Porto Velho, Tocantins Futebol Clube, Araguaia Atlético Clube
- 2010 Série D (4): Cametá Sport Club, Náuas Esporte Clube, Sociedade Esportiva River Plate, Botafogo de Futebol e Regatas do Distrito Federal
- 2011 Série B (1): Americana Futebol Ltda.
- 2011 Série D (10): Cuiabá Esporte Clube, Plácido de Castro Futebol Club, Independente Atlético Clube, Comercial Atlético Clube, Associação Desportiva Bahia de Feira, Sociedade Esportiva River Plate, Esporte Clube Primeiro Passo Vitória da Conquista, Audax Rio de Janeiro Esporte Clube, Bosque Formosa Esporte Clube, Centro Educativo Recreativo Associação Atlética São Mateus
- 2012 Série C (1): Cuiabá Esporte Clube
- 2012 Série D (6): Comercial Atlético Clube, Sociedade Esportiva Ypiranga Futebol Clube, Esporte Clube Primeiro Passo Vitória da Conquista, Esporte Clube Aracruz, Nacional Esporte Clube (Nova Serrana), Concórdia Atlético Clube
- 2013 Série D (6): Plácido de Castro Futebol Club, Sport Club Genus de Porto Velho, Sociedade Esportiva Ypiranga Futebol Clube, Esporte Clube Primeiro Passo Vitória da Conquista, Esporte Clube Aracruz, Clube Esportivo Lajeadense
- 2014 Série D (8): Sport Club Genus de Porto Velho, Princesa do Solimões Esporte Clube, Esporte Clube Jacuipense, Globo Futebol Clube, Esporte Clube Primeiro Passo Vitória da Conquista, Clube Esportivo Operário Várzea-Grandense, Sociedade Esportiva, Recreativa e Cultural Guarani, Maringá Futebol Clube (2010)
- 2015 Série D (3): Clube Esportivo Operário Várzea-Grandense, Red Bull Bragantino II, Foz do Iguaçu Futebol Clube
- 2016 Série D (14): Atlético Acreano, Sport Club Genus de Porto Velho, Trem Desportivo Clube, Baré Esporte Clube, Rondoniense Social Clube, Associação Atlética de Altos, Parnahyba Sport Club, Uniclinic Atlético Clube, Associação Atlética Araguaia, Clube Desportivo Sete de Setembro, Espírito Santo Futebol Clube, Esporte Clube Internacional de Lages, Sport Club São Paulo, AA Araguaia
- 2017 Série D (16): Atlético Acreano, Real Desportivo Ariquemes Futebol Clube, Trem Desportivo Clube, Baré Esporte Clube, Gurupi Esporte Clube, Sport Club Genus de Porto Velho, Associação Atlética de Altos, Cordino Esporte Clube, Tocantins Esporte Clube, Parnahyba Sport Club, Clube Atlético Pernambucano, Jacobina Esporte Clube, Sport Club São Paulo, Foz do Iguaçu Futebol Clube, Esporte Clube Internacional de Lages, Audax-SP
- 2018 Série C (1): Atlético Acreano
- 2018 Série D (15): 4 de Julho Esporte Clube, Associação Atlética de Altos, Barcelona Futebol Clube (Rondônia), Baré Esporte Clube, Clube Esportivo Dom Bosco, Gálatas Esporte Clube, Flamengo Sport Club de Arcoverde, Interporto Futebol Clube, Esporte Clube Jacuipense, Esporte Clube Macapá, Novo Futebol Clube, Prudentópolis Futebol Clube, Real Ariquemes Esporte Clube, Sport Club Santa Rita, Sociedade Desportiva Sparta
- 2019 Série C (1): Atlético Acreano
- 2019 Série D (13): Barcelona Futebol Clube (Rondônia), Real Ariquemes Esporte Clube, Galvez Esporte Clube, Bragantino Clube do Pará, Floresta Esporte Clube, Futebol Clube Atlético Cearense, Associação Atlética de Altos, Grêmio Recreativo Serrano, Esporte Clube Jacuipense, Interporto Futebol Clube, Associação Desportiva Itaboraí, Sport Clube Gaúcho, Foz do Iguaçu Futebol Clube
- 2020 Série D (11): Bragantino Clube do Pará, Galvez Esporte Clube, Ji-Paraná Futebol Clube, Vilhenense Esportivo Clube, Sociedade Esportiva Juventude, Baré Esporte Clube, Floresta Esporte Clube, Associação Desportiva Freipaulistano, Jacyobá Atlético Clube, Real Noroeste Capixaba Futebol Clube, Clube Esportivo Operário Várzea-Grandense
- 2021 Série D (7): 4 de Julho Esporte Clube, Jaraguá Esporte Clube, Nova Mutum Esporte Clube, Sociedade Esportiva de Picos, Porto Velho Esporte Clube, Real Ariquemes Esporte Clube, Santana Esporte Clube
- 2022 Série D (12): Amazonas Futebol Clube, Azuriz Futebol Clube, Costa Rica Esporte Clube, Crato Esporte Clube, Fluminense Esporte Clube, Sport Clube Humaitá, Lagarto Futebol Clube, Nova Venécia Futebol Clube, Academia de Futebol Pérolas Negras, Porto Velho Esporte Clube, Pouso Alegre Futebol Clube, Trem Desportivo Clube
- 2023 Série D (7): Fluminense Esporte Clube, Sport Clube Humaitá, Interporto Futebol Clube, Parnahyba Sport Club, Real Ariquemes Esporte Clube, Trem Desportivo Clube, Esporte Clube Cruzeiro Arapiraca
- 2024 Série D (10): Cametá Sport Club, Fluminense Esporte Clube, Sport Clube Humaitá, Itabuna Esporte Clube, Manauara Esporte Clube, Maracanã Esporte Clube, Porto Velho Esporte Clube, Real Brasília Futebol Clube, Serra Futebol Clube, Trem Desportivo Clube
- 2025 Série D (10): Sport Clube Humaitá, Independência Futebol Clube, Manauara Esporte Clube, Maracanã Esporte Clube, Maricá Futebol Clube, Parnahyba Sport Club, Porto Velho Esporte Clube, Porto Vitória Futebol Clube, Trem Desportivo Clube, União Atlético Clube (Tocantins)
- 2026 Série D (25): Associação Beneficente e Esportiva Catalana e Ouvidorense, Araguaína Futebol e Regatas, Blumenau Esporte Clube (2003), Fluminense Esporte Clube, Guaporé Futebol Clube, Sport Clube Humaitá, Instituto de Administração de Projetos Educacionais Futebol Clube, Independência Futebol Clube, Inhumas Esporte Clube, Clube Laguna SAF, Associação Atlética Maguary, Manauara Esporte Clube, Maricá Futebol Clube, Monte Roraima Futebol Clube, Clube Esportivo Operário Várzea-Grandense, Oratório Recreativo Clube, Parnahyba Sport Club, Piauí Esporte Clube, Porto Sport Club, Primavera Atlético Clube, Santa Catarina Clube, Centro de Formação de Atletas do Tirol, Trem Desportivo Clube, Decisão Goiana Futebol Clube, Maracanã Esporte Clube

Some of these may exist under another name. The state championship clubs (field +0x60) were not compared with the real editions.

### Token and id matches to check

Afogados da Ingazeira Futebol Clube = Afogados da Ingazeira
Alagoinhas Atlético Clube = Alagoinhas AC
Amazonas Futebol Clube = Amazonas FC
Americano Futebol Clube = Americano (RJ)
América Futebol Clube (Rio Grande do Norte) = América FC
Anápolis Futebol Clube = Anápolis FC
Associação Acadêmica e Desportiva Vitória das Tabocas = AAD Vitória das Tabocas
Associação Atlética Aparecidense = AA Aparecidense
Associação Atlética Aparecidense = AA Aparecidense (GO)
Associação Atlética Aparecidense = Assossiação Atlética Aparecidense
Associação Atlética Internacional (Limeira) = AA Internacional Limeira
Associação Atlética Internacional (Limeira) = AA Internacional de Limeira
Associação Atlética Internacional (Limeira) = AA Internacional de Limeira (SP)
Associação Atlética Portuguesa (Rio de Janeiro) = AA Portuguesa
Associação Atlética Portuguesa (Rio de Janeiro) = AA Portuguesa (RJ)
Associação Atlética de Altos = AA Altos
Associação Cultural Esporte Clube Baraúnas = Associação Esportiva Clube Baraúnas
Associação Cultural e Desportiva Potiguar = ACD Potiguar (RN)
Associação Desportiva Bahia de Feira = AD Bahia de Feira (BA)
Associação Desportiva Confiança = AD Confiança
Associação Desportiva Ferroviária Vale do Rio Doce = Desportiva Capixaba
Associação Desportiva Iguatu = AD Iguatu
Associação Desportiva Jequié = AD Jequié
Associação Desportiva Recreativa Cultural Icasa = AD Recreativa Cultural Icasa (CE)
Associação Desportiva Recreativa Cultural Icasa = Icasa Futebol Clube
Associação Esportiva Recreativa Engenheiro Beltrão = Engenheiro Beltrão
Associação Esportiva Velo Clube Rioclarense = AE Velo Clube Rioclarense (SP)
Associação Ferroviária de Esportes = Ferroviária
Atlético Acreano = Club Atlético Acreano
Atlético Clube Goianiense = AC Goianiense (GO)
Audax Rio de Janeiro Esporte Clube = Audax Rio de Janeiro EC (RJ)
Auritânia Foz do Iguaçu Futebol Clube = AER Auritânia Foz do Iguaçu
Azuriz Futebol Clube = Azuriz FC
Barcelona Futebol Clube = Barcelona FC
Barra Futebol Clube (Santa Catarina) = Barra FC
Betim Esporte Clube = Ipatinga Futebol Clube
Boa Esporte Clube = Ituiutaba Esporte Clube
Boavista Sport Club = Boavista SC
Boavista Sport Club = Sport Club Boavista
Botafogo Futebol e Regatas = Botafogo de Futebol e Regatas
Brasiliense Futebol Clube = Brasiliense FC
Brasiliense Futebol Clube = Brasiliense FC (DF)
Capital Clube de Futebol = Capital CF
Capital Futebol Clube = Capital Clube de Futebol (DF)
Catuense Futebol S/A = Associação Esportiva Catuense
Ceilândia Esporte Clube = Ceilândia EC
Central Sport Club = Central SC
Club Sportivo Sergipe = CS Sergipe
Club de Regatas Brasil = Clube de Regatas Brasil
Club de Regatas Vasco da Gama = Clube de Regatas Vasco da Gama
Clube Atlético Patrocinense = CA Patrocinense
Clube Atlético Patrocinense = CA Patrocinense (MG)
Clube Atlético Tubarão = Clube AtléticoTubarão (SC)
Clube Esportivo Bento Gonçalves = Clube Esportivo de Bento Gonçalves (RS)
Clube Esportivo Nova Esperança = Centro Esportivo Nova Esperança
Clube Esportivo Operário Várzea-Grandense = CE Operário Várzea-Grandense (MT)
Clube J. Malucelli = J. Malucelli
Comercial Futebol Clube (Ribeirão Preto) = Comercial Esporte Clube
Concórdia Atlético Clube = Concórdia AC
Corumbaense Futebol Clube = Corumbaense
Cuiabá Esporte Clube = Cuiabá
Duque de Caxias Futebol Clube = Duquecaxiense Futebol Clube
Esporte Clube Bahia = EC Bahia
Esporte Clube Cruzeiro (Porto Alegre) = Cruzeiro (RS)
Esporte Clube Democrata = EC Democrata
Esporte Clube Democrata = EC Democrata (MG)
Esporte Clube Novo Hamburgo = EC Novo Hamburgo
Esporte Clube Pelotas = EC Pelotas (RS)
Esporte Clube Primeiro Passo Vitória da Conquista = EC Primeiro Passo Vitória da Conquista
Esporte Clube Primeiro Passo Vitória da Conquista = Vitória da Conquista
Esporte Clube Santo André = EC Santo André
Esporte Clube São Luiz = EC São Luiz
Esporte Clube XV de Novembro (Piracicaba) = EC XV de Novembro
Esporte Clube Água Santa = EC Água Santa
Esporte Clube Água Santa = EC Água Santa (SP)
Feirense Futebol Clube = Feirense Esporte Clube
Ferroviário Atlético Clube (Ceará) = Ferroviário AC
Fluminense Football Club = Fluminense
Fluminense Football Club = Fluminense FC
Futebol Clube Atlético Cearense = FC Atlético Cearense
Globo Futebol Clube = Globo Futebol Clube RN
Goiatuba Esporte Clube = Goiatuba EC
Goiânia Esporte Clube = Goiânia EC
Grêmio Atlético Sampaio = GA Sampaio (RR)
Grêmio Barueri = Grêmio Barueri Futebol Ltda.
Grêmio Barueri = Grêmio Recreativo Barueri
Grêmio Barueri Futebol Ltda. = Grêmio Prudente Futebol
Grêmio Maringá S/S Ltda = Grêmio de Esportes Maringá
Guarany Futebol Clube = Esporte Clube São Bento (SP)
Guarany Sporting Club = Guarany Sporting Clube
Guaratinguetá Futebol Ltda = Americana Futebol
Guaratinguetá Futebol Ltda = Guaratinguetá Futebol
Hercílio Luz Futebol Clube = Hercílio Luz FC
Horizonte Futebol Clube = Horizonte FC
Ipatinga Futebol Clube = Betim EC
Ipatinga Futebol Clube = Ipatinga FC
Iporá Esporte Clube = Iporá EC
Itabirito Futebol Clube = Itabirito FC
Itabuna Esporte Clube = Itabuna
Ivinhema Futebol Clube = EC Santo André
J.Malucelli Futebol S/A = J. Malucelli Futebol
J.Malucelli Futebol S/A = Sport Club Corinthians Paranaense
Joinville Esporte Clube = Joinville EC (SC)
Lagarto Futebol Clube = Lagarto FC
Luverdense Esporte Clube = Luverdense EC
Luverdense Esporte Clube = Luverdense EC (MT)
Macaé Esporte Futebol Clube = Macaé Sports
Madureira Esporte Clube = Madureira EC
Maranhão Atlético Clube = Maranhão AC
Maringá Futebol Clube (2010) = Maringá FC
Mixto Esporte Clube = Mixto EC
Mixto Esporte Clube = Mixto EC (MT)
Moto Club = Moto Club de São Luís
Moto Club = Moto Club de São Luís (MA)
Moto Club de São Luís = Moto Clube de São Luís
Nacional Fast Clube = Nacional Fast Club
Nacional Futebol Clube (Amazonas) = Nacional FC
Nova Iguaçu Futebol Clube = Nova Iguaçu FC
Nova Iguaçu Futebol Clube = Nova Iguaçu FC (RJ)
Náutico Futebol Clube = Náutico Esporte Clube
Operário Ferroviário Esporte Clube = Operário Ferroviário EC
Operário Futebol Clube = Novoperário Futebol Clube
Operário Futebol Clube Ltda = Operário Futebol Clube
Paragominas Futebol Clube = Esporte Clube Paraguaçuense
Paulista Futebol Clube = Paulista FC
Paysandu Sport Club = Paysandu SC
Penarol Atlético Clube = Penharol Atlético Clube
Petrolina Social Futebol Clube = Rio Preto Esporte Clube
Porto Velho Esporte Clube = Porto Velho EC
Pouso Alegre Futebol Clube = Pouso Alegre FC
Real Noroeste Capixaba Futebol Clube = Real Noroeste CFC
Retrô Futebol Clube Brasil = Retrô FC Brasil
Rio Branco Football Club = Rio Branco (AC)
Rio Branco Futebol Clube = Rio Branco Atlético Clube (ES)
River Atlético Clube = River AC
Sampaio Corrêa Futebol Clube = Sampaio Corrêa FC (MA)
Serra Branca Esporte Clube = Serra Branca EC
Serra Futebol Clube = Serra Talhada Futebol Clube
Sociedade Desportiva Juazeirense = Juazeiro Social Clube
Sociedade Desportiva Juazeirense = SD Juazeirense
Sociedade Desportiva Juazeirense = SD Juazeirense (BA)
Sociedade Desportiva Serra Futebol Clube = Serra Futebol Clube
Sociedade Esportiva e Recreativa Caxias do Sul = S.E.R. Caxias do Sul (RS)
Sociedade Esportiva e Recreativa Caxias do Sul = SER Caxias do Sul
Sociedade Esportiva e Recreativa Caxias do Sul = SER Caxias do Sul (RS)
Sociedade Esportiva e Recreativa Caxias do Sul = Sociedade Esportiva Caxias do Sul
Sousa Esporte Clube = Sousa EC
Sousa Esporte Clube = Souza Esporte Clube
Sport Club Corinthians Paranaense = Clube Malutrom
Sport Club Penedense = Santa Helena Esporte Clube
São Bernardo Futebol Clube = São Bernardo FC
São Caetano Futebol = AD São Caetano (SP)
São Caetano Futebol = Associação Desportiva São Caetano
Tocantinópolis Esporte Clube = Tocantinópolis EC
Toledo Esporte Clube = Toledo Colonia Work (PR)
Treze Futebol Clube = Treze FC
Tupi Football Club = Tupi
UNRESOLVED:Betim = Betim EC
UNRESOLVED:Águia de Marabá = Águia de Marabá FC
Uberlândia Esporte Clube = Uberlândia EC
União Esporte Clube = União Rondonópolis
União Esporte Clube = União Rondonópolis EC (MT)
Vilhena Esporte Club = Vilhena Esporte Clube
Vitória Futebol Clube (Brasil) = Vitória FC
Ypiranga Clube = Ypiranga (SC)
Ypiranga Futebol Clube (Erechim) = SE Ypiranga FC

## Reconcile

Third pass of 2026-10-16 over the 39 `disagree` verdicts and the 111 `found` values of `bra.check.tsv`, and over the club matches that the check rejected or could not confirm. Every decision and its URL is in `bra.reconcile.tsv`. Row order and ids in `bra.tsv` are unchanged. Club counts are clubs that played. Withdrawals are in `notes`. `down` holds direct places by the rules of the season. A relegation play-off goes in `playoffs`.

### Counts

| Decision | Fields |
|---|---|
| first | 6 |
| second | 33 |
| conflict | 0 |
| found: accepted | 107 |
| found: accepted (adjusted) | 2 |
| found: rejected | 2 |
| consequential change | 2 |

| Club list decision | Entries |
|---|---|
| match rejected (database fields cleared) | 10 |
| match replaced by the right record | 7 |
| match kept after a hand check | 10 |
| conflict | 1 |
| row removed (duplicate or did not play) | 9 |
| row added (missing club) | 9 |
| name corrected | 1 |

### Disagreements

1. Série C clubs 2002, 2003, 2004 and 2008 (bra-82, -83, -84, -88): the facts agree. en.wikipedia counts entrants. RSSSF and pt.wikipedia count clubs that played. The rows now hold 61, 93, 60 and 63, and the notes name the withdrawals.
2. Série C 2005 (bra-85): 63 clubs, 15 groups of 4 and 1 group of 3. pt.wikipedia and the RSSSF match list of group 1 agree. en.wikipedia (64) is wrong.
3. Série D 2009 (bra-116): 39 clubs, because every Acre club declined. The format now says 9 groups of 4 and 1 group of 3. Série D 2014 (bra-121): 41 clubs, 7 groups of 5 and 1 group of 6 (pt.wikipedia).
4. Windows 2026 (bra-25): a complementary domestic window ran from 4 to 27 March (A Tarde, Diário de Pernambuco). Only players from the 2026 state championships could join in it.
5. Copa do Brasil 2016 (bra-150): first. A União (7 December 2016): no away-goals rule in the final.
6. Copa dos Campeões 2002 (bra-190): second. The quarter-finals were single matches at neutral grounds (Diário do Pará, RSSSF).
7. Copa do Nordeste 2015 (bra-230): 20 clubs (Trivela).
8. Copa Norte and Copa Centro-Oeste 2026 (bra-295, bra-322): first. The cups were played with groups, single-match semi-finals and a two-legged final (Diário do Pará, May 2026). The second sources describe a plan with quarter-finals from before the season.
9. State championships, first kept: Baiano 2022 (2 down; the FBF rule put the second place in force when the second division had 10 clubs, Bahia Notícias), Carioca 2020 (1 down; the RSSSF Brasil match list shows that the last club of Grupo X played on in Grupo Z) and Catarinense 2002 (11 clubs; Chapecoense came from the qualifying tournament).
10. State championships, second taken: most first values were count errors on the pt.wikipedia infobox. The page's own rule text gives the second value. Primary records: the FGF Normas Especiais of 2007 and 2010 (web archive). Values now: Catarinense 2005 12, 2012 and 2013 2 down, 2017 and 2018 10 clubs; Gaúcho 2012 2; Goiano 2007 2, 2009 4, 2010 2, 2023 2; Mineiro 2006 and 2020 2; Paranaense 2010 4; Paulista 2002 12 clubs; Paulista 2006 and 2010 to 2013 4 down; Pernambucano 2026 1 (Diário de Pernambuco quotes the regulation).
11. Paranaense 2026 (bra-700): two clubs went down, both through two-legged relegation ties ("Torneio da Morte"). `down` is now 0 and `playoffs` describes the ties.

### Found values

- Accepted, each from a page read today: 2020 registration (40 players, 8 replacements), 2027 and 2028 registration (50 players in two lists), 2027 windows, the Copa do Nordeste 2002 and 2003 and Copa Norte 2002 counts and formats, the Primeira Liga 2016 format, and 95 club counts and relegation places of state championships. The Supercopa 2027 format is an inference from one date.
- Adjusted: Baiano 2005 (Palmeiras went down through a play-off, so `down` = 0) and Paulista 2003 (1 direct place; Botafogo went down through the promotion and relegation play-off).
- Rejected: Copa Centro-Oeste 2002 clubs and format (bra-298). The RSSSF Brasil page says the file "ainda não foi criado". Both fields stay as they were.

### Club lists (bra-clubs.tsv)

- Rejected matches, database fields cleared: Ivinhema, Paragominas (2013), Petrolina Social (2008), Serra FC (2019), SD Juazeirense (2013, where record 5653 is still Juazeiro Social Clube), Penedense, Ypiranga of Erechim (2009), Rio Branco FC (2021), Operário of Várzea Grande (2006) and Capital FC of Palmas (2024).
- Matches replaced: Guarany FC 2025 and 2026 is Guarany de Bagé (4658). Operário of Campo Grande 2019, 2023, 2025 and 2026 is record 2464 "Operário (MS)", not 8306 (Novoperário). XV de Piracicaba 2026 is record 228, not 759 (XV de Jaú).
- Kept after a hand check: Ypiranga Clube (AP), Duque de Caxias, Operário (MS) 2003 and 2008, Náutico (RR), Toledo, Vitória (ES), América (RN), Portuguesa (RJ), XV de Piracicaba (12940, 228) and the lineage matches. The proof is the names that each record carries in the other databases.
- Removed: duplicates Boa Esporte Clube, Fast Clube and Vitória FC (Brasil) in 2006, Betim in 2014 and AA Araguaia in 2016. Clubs that did not play: SE Guaxupé and Parnahyba in 2003, Ceilândia and Brazlândia in 2008.
- Added: 2003 Tuna Luso (11861) and Flamengo (PI) (4061). 2006 Ipitanga (no record), Novo Hamburgo (8060) and Tuna Luso (11861). 2008 Bacabal (1202) and Toledo Colônia Work (no record). 2014 Itaporã (no record). 2016 PSTC (no record).
- Name corrected: Série B 2016 "Osasco Sporting" is Oeste Futebol Clube. The match (8166) was right.

Changes to the missing-clubs list above: Parnahyba (2003), Fast Clube, Vitória Futebol Clube (Brasil) (2006) and AA Araguaia (2016) leave the list. Now missing: 2006 Ipitanga and Operário (MT); 2008 Petrolina Social and Toledo Colônia Work; 2009 Ypiranga (Erechim); 2013 Paragominas and SD Juazeirense; 2014 Itaporã; 2016 PSTC; 2019 Serra FC (ES); 2021 Rio Branco FC; 2024 Capital FC (TO); 2025 Penedense; 2026 Ivinhema.

### Conflicts

1. Comercial FC of Ribeirão Preto (2002, 2003). The check rejected the match with record 2676 "Comercial Esporte Clube". I did not apply that rejection. Record 2676 is "Comercial Futebol Clube (SP)" in 13 later databases, and the Campo Grande club is record 2674. Max decides: keep the match (recommended) or clear it.
Point for Max, not a conflict: Toledo Esporte Clube (2020) = record 516, the Toledo Colônia Work record. Later databases rename it "Toledo Esporte Clube (PR)". The match is kept as an inference. Max may prefer no match.

### Seen but not changed

- The infobox count error that caused the state `down` disagreements may also affect state rows with `no second source`. Rows to check: Paulista 2014 (5), Gaúcho 2005, 2006, 2017, 2025 and 2026, Baiano 2012, 2013 and 2017, and Pernambucano 2014, 2015, 2017, 2018 and 2021.
- Pernambucano 2026 (bra-754) format leaves out the quarter-finals for 3rd to 6th place.
- Catarinense 2017 `down` stays `unknown`. Its Série A page gives 2 (Almirante Barroso, Metropolitano).
- Clube Esportivo Operário Várzea-Grandense is on the missing list for 2014, 2015, 2020 and 2026. Record 2616 carries that name in later databases, so a lineage match may exist.

## Loose ends

Pass of 2026-10-16 over the state rows that the Reconcile section listed under "Seen but not changed". For each row I read the rule text of the season and the RSSSF Brasil page of that season and of the next season. The RSSSF pages show the clubs that left the first division. Row order and ids are unchanged.

Changed:

- Paulista 2014 (bra-715) `down`: 5 is now 4. The RSSSF Brasil page states "bottom 4 are relegated" (http://www.rsssfbrasil.com/tablessz/sp2014.htm). The pt.wikipedia rule text agrees ("Os quatro times que somarem menos pontos").
- Catarinense 2017 (bra-502) `down`: `unknown` is now 2. The pt.wikipedia rule text says the last two go down to Série B 2018. The RSSSF Brasil 2018 page shows that Metropolitano and Almirante Barroso left the division (http://www.rsssfbrasil.com/tablessz/sc2018.htm).

Checked, value kept (2 in each row):

- Gaúcho 2005 (bra-544) and 2006 (bra-545). No rule text found. RSSSF Brasil 2006 and 2007 show two clubs out each year (http://www.rsssfbrasil.com/tablesr/rs2006.htm, http://www.rsssfbrasil.com/tablesr/rs2007.htm).
- Gaúcho 2017 (bra-556). Rule text: the last two go down. RSSSF Brasil 2018 shows Passo Fundo and Ypiranga out (http://www.rsssfbrasil.com/tablesr/rs2018.htm).
- Gaúcho 2025 (bra-564). The bottom two of the relegation group go down (Soccerway guide: https://br.soccerway.com/noticias/futebol-gaucho-guia-do-campeonato-gaucho-2025-regulamento-favoritos-e-onde-assistir/tOuYrwIq).
- Gaúcho 2026 (bra-565). The relegation group sends two clubs down (Itatiaia: https://www.itatiaia.com.br/esportes/futebol/futebol-nacional/campeonato-gaucho/campeonato-gaucho-2026-regulamento-participantes-contratacoes-e-onde-assistir/).
- Baiano 2012 (bra-443). RSSSF Brasil marks the last two "Rebaixado" (http://www.rsssfbrasil.com/tablesae/ba2012.htm).
- Baiano 2013 (bra-444) and 2017 (bra-448). RSSSF Brasil 2014 and 2018 show two clubs out each year (http://www.rsssfbrasil.com/tablesae/ba2014.htm, http://www.rsssfbrasil.com/tablesae/ba2018.htm). The 2014 page also states "Os dois últimos da 1ª Fase caem".
- Pernambucano 2014 (bra-742) and 2017 (bra-745). Rule text: the last two of the relegation group go down. RSSSF Brasil 2015 and 2018 show two clubs out (http://www.rsssfbrasil.com/tablesfq/pe2015.htm, http://www.rsssfbrasil.com/tablesfq/pe2018.htm).
- Pernambucano 2015 (bra-743). RSSSF Brasil: "2 últimos caem para a 2ª Divisão" (http://www.rsssfbrasil.com/tablesfq/pe2015.htm).
- Pernambucano 2018 (bra-746). No rule text found. The pt.wikipedia table marks two places, and RSSSF Brasil 2019 shows Belo Jardim and Pesqueira out (http://www.rsssfbrasil.com/tablesfq/pe2019.htm). The pt.wikipedia infobox names only Pesqueira.
- Pernambucano 2021 (bra-749). RSSSF Brasil: the bottom 2 of the relegation group are relegated (http://www.rsssfbrasil.com/tablesfq/pe2021.htm).

The Gaúcho 2005, Gaúcho 2006 and Pernambucano 2018 values rest on the clubs that went down, not on a rule text. The `sources` of the kept rows are unchanged.

## Source fill

Pass of 2026-10-16. The lint listed 903 filled fact fields in `bra.tsv` with no `field=URL` entry. I read each page today with curl and checked that it states the value. I added `field=URL` entries to `sources` only. Values, row order and ids are unchanged. 166 fields remain without a source.

Sources added (506 rows):

- `name` in 506 rows. Most come from the season page that the row already cites. For 60 rows no season page names the competition: the RSSSF Brasil pages use English names and many pt.wikipedia season pages do not exist. These rows cite the pt.wikipedia page of the competition, which states its name but not the name of each season. This applies to state rows of single seasons, to the 2026/27 and 2027/28 rows, and to the Mineiro rows, where the general page states that the first division is known as "Módulo I".
- `clubs` = 0 in 218 not-held rows. Each cites the page that its `format` entry already cites, which states that the competition did not run that year. Copa Verde 2026/27 and 2027/28 (`clubs` = 2) cite the Copa Verde page, which states a final between the winners of the Copa Norte and the Copa Centro-Oeste from 2026.
- Série A 2026/27 and 2027/28 (bra-53, bra-54): `name`, `down`, `playoffs` and `format` cite the 2026 season page (https://en.wikipedia.org/wiki/2026_Campeonato_Brasileiro_S%C3%A9rie_A). The values stay marked as inference.
- Série B 2026/27 and 2027/28 (bra-80, bra-81): `name`, `down` and `format` cite https://en.wikipedia.org/wiki/Campeonato_Brasileiro_S%C3%A9rie_B.

Disagreements (value kept, no source added):

- Série B 2026/27 and 2027/28 (bra-80, bra-81) `up` = 4 and `playoffs` = none. From 2026 the top two go up directly, and 3rd to 6th play two-legged semi-finals (3rd v 6th, 4th v 5th) for two more places. Sources: https://en.wikipedia.org/wiki/Campeonato_Brasileiro_S%C3%A9rie_B and https://pt.wikipedia.org/wiki/Campeonato_Brasileiro_de_Futebol_de_2026_-_S%C3%A9rie_B. The same change applies to Série B 2025/26 (bra-79, `up` = 4, `playoffs` = none, `all=` the 2026 page). That field has a source, so this pass did not touch it.

No source found (note added to the row):

- `name` and `format` of the 81 rows of the three composite championships of the original (Central bra-514 to bra-540, North bra-622 to bra-648, Northeast bra-649 to bra-675). No real competition has these names or this format, so no page states them.

## Pass 2

Decision pass of 2026-10-17 (Q268 to Q331). Row ids are unchanged. New rows are at the end. `tools/research/rules_lint.py` gave 162 problems for `bra.tsv` before this pass and 0 after it.

### No new tiers (Q331, Q288)

- The lowest national tier of the original is `bra_nat_third` (Série C). Série D is a tier below it, so its 27 rows (bra-109 to bra-135) and its 1,040 club rows are deleted.
- `bra.tsv` now has 1,053 rows. `bra-clubs.tsv` now has 1,649 club rows.
- The relegation places of Série C stay as rule places. In the game, a club that goes down from Série C leaves the national divisions.
- No competition of the part needs a nation that a game cannot select. The cups (Copa do Brasil, Supercopa, Copa dos Campeões and the regional cups) stay.

### Composite championships (Q283)

The original runs three composite championships made of clubs of several small states. Each composite code now runs one real state championship. The other states of each composite get `new` rows. The state of each composite club comes from the 3.9.68 `club.dat` (field +0x60) and from the names of the same records in later databases. River (PI) and AD Vasco da Gama of Rio Branco (AC) are in the North composite, so Piauí and Acre come in too (https://pt.wikipedia.org/wiki/R%C3%ADver_Atl%C3%A9tico_Clube, https://pt.wikipedia.org/wiki/Associa%C3%A7%C3%A3o_Desportiva_Vasco_da_Gama).

| Code | Rows | Real competition | Other states, as `new` rows |
|---|---|---|---|
| bra_reg_northeast | bra-649 to bra-675 | Campeonato Cearense | Potiguar, Paraibano, Alagoano, Sergipano |
| bra_reg_north | bra-622 to bra-648 | Campeonato Paraense | Maranhense, Amazonense, Piauiense, Acriano |
| bra_reg_central | bra-514 to bra-540 | Campeonato Mato-Grossense | Candango (Federal District), Capixaba, Sul-Mato-Grossense, Tocantinense |

- Rule for the code: the state whose clubs held the most Série A places from 2002 to 2026 in `bra-clubs.tsv`. Ceará has 17 (Ceará 8, Fortaleza 9) against 1 for Alagoas. Pará has 4 (Paysandu 3, Remo 1). Mato Grosso has 4 (Cuiabá) against 1 for the Federal District (Gama).
- New rows: 12 championships of 27 seasons, bra-757 to bra-1080 (324 rows).
- The regional cups stay as their own `new` rows, as they ran: Copa do Nordeste, Copa Verde (2014 on), Copa Norte and Copa Centro-Oeste (2002, then 2026).
- The build gives each club of these 15 states the state championship of its state. A club of Rondônia, Roraima or Amapá has no state championship in the game, as in the original.

Sources and method of the state rows:

- RSSSF Brasil season pages give the participants, the rule lines and the stage headings. The site refused connections during the pass, so 285 pages were read as archived copies, and the `sources` give the web.archive.org address. The 25 pages read before the refusal give the direct address.
- pt.wikipedia season pages give the infobox club count, the relegated clubs and the rules section where RSSSF has no page.
- `clubs` is the RSSSF participants list. Where the RSSSF table has more places, the table count is used and the note says so. Where pt.wikipedia gives another count, the note says so.
- `down` is the rule text of RSSSF ("bottom 2 are relegated") where it exists. Otherwise it counts the clubs marked relegated in the RSSSF tables or in the pt.wikipedia infobox, and the note marks it as an inference. A count of more than half the clubs was rejected.
- `format` is a keyword summary and an inference, as in the other state rows. Substitutes and bench stay `unknown`, because no state regulation was read.
- 2027 and 2028 rows are `unknown` (future editions).
- Fields with no source: `clubs` 65, `down` 168 and `format` 71 of the 375 rows from 2002 to 2026 are `unknown`.

### Conflicts and open points, settled by real life

1. Comercial FC of Ribeirão Preto, Série C 2002 and 2003: keep the match with record 2676. That record is "Comercial Futebol Clube (SP)" in 13 later databases, and record 2674 is the club of Campo Grande, "Esporte Clube Comercial (MS)".
2. Toledo Esporte Clube 2020 = record 516: keep. The club was founded as Toledo Colônia Work in 2004, became Toledo Futebol in 2015 and Toledo Esporte Clube in 2016 (https://pt.wikipedia.org/wiki/Toledo_Esporte_Clube). The database record carries these three names in that order.
3. Série B 2026 to 2028 `up` and `playoffs`: settled before this pass by the lead edit of 2026-10-16. Two clubs go up directly, and 3rd to 6th play two-legged ties for two more places (CBF regulation, ge.globo of 5 February 2026). The 2027 and 2028 rows copy 2026 as an inference.
4. Pernambucano 2026 (bra-754): `format` now names the two-legged quarter-finals of 3rd to 6th (https://pt.wikipedia.org/wiki/Campeonato_Pernambucano_de_Futebol_de_2026).
5. Clube Esportivo Operário Várzea-Grandense: every missing entry was in Série D, so the point falls away with Série D.

### Club moves (Q279, Q280, Q290)

Every season with a database already lists every real club of Séries A, B and C (2002, 2003, 2004, 2006 and 2008 to 2026). Série C is the lowest tier, so no tier below it needs filling. The pool rule now also takes clubs of "A Lower Division" and clubs with no division. A second search of the season databases found four more matches:

- União Bandeirante FC, Série C 2002 and 2003 = record 12035 "União Bandeirantes Futebol Clube" (Third Division in 3.9.68).
- Americana Futebol, Série B 2011 = record 1861 "Guaratinguetá Futebol" (September 2010 ODB). The club moved from Guaratinguetá to Americana for 2011 (https://pt.wikipedia.org/wiki/Guaratinguet%C3%A1_Futebol_Ltda.), and the record is "Americana Futebol" in September 2011 ODB.
- Cuiabá EC, Série C 2012 = record 3835 "Cuiabá" (September 2011 ODB). The record has no nation in that database. The build must give it Brazil.

Club rows added: 0. The club lists of the national divisions were complete. The four matches fill empty `db_club_id` fields.

Clubs of Séries A to C that still have no record in the season database: 65 rows, for Max.

- Série C 2002 to 2008 (62 rows): the names in the missing list above, with the Reconcile changes, except União Bandeirante and Toledo Colônia Work. The Série D names of that list leave with Série D.
- Atlético Acreano, Série C 2018 and 2019. Record 992 is an Italian club in October 2017 and October 2018.
- Toledo Colônia Work, Série C 2008. Record 516 is a Sudanese club in November 2007.

### Reserve and B teams (Q289)

None. No reserve or B team played in Séries A, B or C from 2002 to 2026. Bragantino II played in Série D 2015, which is out of the game.

## Second source, pass 2

Pass of 2026-10-17. I took every check line with verdict `no second source` whose row still exists, whose id is bra-756 or lower, and whose field holds a value (not `unknown`, not `n/a`, not marked as inference). That gives 505 lines. The 41 lines of deleted Série D rows and the new rows bra-757 to bra-1080 were not checked. Each second source is from another publisher than the first source of that field, and I read each page today. Results are in `bra.check2.tsv`.

### Counts

| Verdict | Lines |
|---|---|
| agree | 427 |
| disagree, settled | 1 |
| disagree, still split | 0 |
| no second source | 77 |

Agree by field: substitutes 157, bench 121, name 91, down 22, clubs 19, format 10, playoffs 4, up 3.

### Substitutes and bench

The first sources (goal.com, Bahia Notícias) speak of CBF competitions in general. The second sources are the CBF rule books, which apply to every CBF competition:

- Bench of 7: CBF Regulamento Geral das Competições dated 15 December 2010 (copy on the Pernambuco federation site, https://www.fpf-pe.com.br/assets/uploads/138564787574.pdf). It states the 2011 rule. The other years up to 2012 rest on it and on netflu 2013, which gives seven as the earlier limit.
- Bench of 12 and 5 substitutes in 3 stoppages: CBF RGC 2024 (https://futeboldealagoas.net/assets/uploads/RGC_CBF_2024.pdf, art. 13) and the CBF Manual de Competições 2026 (https://stcbfsiteprdimgbrs.blob.core.windows.net/img-site/cdn/Manual_de_Competicoes_CBF_2026_f5b02a4e04.pdf). Each states the rule of its own year. The other years rest on netflu (12 from 2013, 5 from the 2020 restart).
- Substitutes 3 to 2019: Law 3 allowed three from 1995 to 2020 (https://en.wikipedia.org/wiki/Substitute_(association_football)). Jovem Pan of 14 May 2020 gives three as the Brazilian limit before the change. This is a general rule, not a page of each competition.
- No second source for the bench of the cups that the CBF did not run: Torneio Rio-São Paulo, Copa Sul-Minas, Copa Norte and Copa Centro-Oeste 2002, Copa do Nordeste 2002, 2003 and 2010, and the Primeira Liga.
- netflu of 6 July 2013 says the CBF "now" allows 12 on the bench in the Campeonato Brasileiro. The first source dates the change to 1 January 2013. The 2013 rows keep 12.

### Change to bra.tsv

- Goiano 2026 (bra-592) `down`: 0 is now 2. The first value came from the empty relegated list of the pt.wikipedia infobox. O Hoje of 26 November 2025 states that the 12th goes down directly and that the 10th and 11th play two legs for the second place (https://ohoje.com/2025/11/26/goiano-de-2026-tera-calendario-ampliado/). The third source, Tribuna do Planalto, states the same (https://tribunadoplanalto.com.br/campeonato-goiano-tera-14-datas-e-comeca-em-11-de-janeiro/). The rules section of the pt.wikipedia page also names the 11th and 12th as relegated. Decision: second. The `sources` entry for `down` now cites both pages, and `notes` describes the play-off. `rules_lint.py` gives 0 problems for `bra.tsv` before and after.

### No second source (77 lines)

- 45 `playoffs` lines hold `see format`. The value points to the format field, which is a keyword summary marked as inference, so there is nothing to check.
- 17 names and 2 Série A play-off fields of 2027 or 2028 rows that no page states yet: all 2028 state names, the 2028 Supercopa name, Mineiro and Mato-Grossense 2027, and Catarinense and Goiano 2027 (the pages found refused the request).
- 9 bench lines of cups that the CBF did not run (see above).
- Baiano 2005 `playoffs` (no other publisher), Catarinense 2017 `down` (RSSSF Brasil and pt.wikipedia are both first sources), Catarinense 2024 `down` (the RSSSF page has no rule text, and three clubs differ between the 2024 and 2025 lists), Série B 2014 `down` (the ESPN table has no relegation marks).

### Agreements that rest on club moves

Where RSSSF Brasil has no rule text, the `down` value agrees with the clubs that left the division by the next season's participants list. This is an inference, as in the first pass: Baiano 2013 and 2017, Gaúcho 2005 and 2006, Paranaense 2018, Paulista 2015, Pernambucano 2014, 2017 and 2018.

### Seen, not changed (for Max)

- Cearense 2027 (bra-674) has `clubs` = `unknown`. Diário do Nordeste of 16 October 2026 names 10 clubs (https://diariodonordeste.verdesmares.com.br/jogada/conheca-novo-formato-do-campeonato-cearense-para-2027-veja-1-rodada-1.3799201).
- Paraense 2027 (bra-647) and Paranaense 2027 (bra-701): 12 clubs each (Diário do Pará, Um Dois Esportes).
- Pernambucano 2027 (bra-755): the federation plans one competition without divisions, with a first stage of 24 clubs from October 2026 (Diário de Pernambuco, 6 May 2026).
- Série A 2027 and 2028: in February 2026 the CBF began a study of 18 clubs and 3 relegated places. No decision was found.
- The 2027 Supercopa is named "Supercopa Rei" in the CBF calendar of October 2026 (Gazeta Esportiva).
- Paranaense 2022 and 2023: RSSSF Brasil lists Toledo among the participants, but its tables have 12 clubs without Toledo. The rows keep 12.
