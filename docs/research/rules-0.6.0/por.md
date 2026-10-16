# Rules table: Portugal (part `por`, slice 5)

Date 2026-10-16. Research only. Files: `por.tsv` (243 rows), `por-clubs.tsv` (2,765 rows), this note.

## Method

- Scope: the original's `por_prm`, `por_second`, `por_second_b`, `por_cup` and `por_super`, plus one nation rules row per season. New competitions (`comp_file` = `new`): Liga 3 (tier 3 from 2021/22), Terceira Divisão (tier 4 to 2012/13) and Taça da Liga (from 2007/08).
- Nine rows per season, 2001/02 to 2027/28: Primeira Liga, second tier, the `por_second_b` line, Liga 3, Terceira Divisão, Taça de Portugal, Supertaça, Taça da Liga, nation rules. A season in which a competition does not run has `format` = `not held`.
- The `por_second_b` line follows one lineage: Segunda Divisão B (to 2004/05), Segunda Divisão (2005/06 to 2012/13), Campeonato Nacional de Seniores (2013/14, 2014/15), Campeonato de Portugal (2015/16 on). From 2021/22 this line is the fourth tier, because Liga 3 takes the third. The build must decide whether `por_second_b` becomes the fourth tier or Liga 3 takes its code.
- Facts come from raw Wikipedia wikitext (`action=raw`), read on 2026-10-16, with the league tables parsed by script (club count, colour legend per position). The `sources` column names the page for each field.
- 2026/27 uses the season pages that exist. 2027/28 has no season pages. Its rows copy 2026/27 where no change is announced and say so (inference), or say `unknown`.
- Substitutes and bench: Liga Portugal press for the Liga competitions (Primeira Liga, second tier, Taça da Liga). For the FPF competitions (lower leagues, Taça de Portugal, Supertaça) no FPF rule text was found. Those rows give 3 to 2019/20 from the Laws of the Game and 5 from 2020/21 as an inference.

## Main sources

- Season pages on en.wikipedia: `YYYY–YY Primeira Liga`; `Segunda Liga`, `Liga de Honra`, `LigaPro` or `Liga Portugal 2`; `Segunda Divisão B` or `Segunda Divisão`; `Campeonato Nacional de Seniores` or `Campeonato de Portugal`; `Liga 3 (Portugal)`; `Terceira Divisão`; `Taça de Portugal`; `Taça da Liga`; `YYYY Supertaça Cândido de Oliveira`. Templates `YYYY–YY ... table` where a page transcludes its table.
- General pages: Primeira Liga (sizes, sponsor names), Liga Portugal 2 (names), Liga 3, Campeonato de Portugal, Taça da Liga (sponsor names), Taça de Portugal (replays abolished after 2001/02), Supertaça, Transfer window, Substitute (association football).
- Press: noticiasaominuto.com, 8 June 2020 (5 substitutes and bench 7 to 9 from round 26 of 2019/20); observador.pt, 15 May 2023 (bench 9 and 5 substitutes made permanent); abola.pt, 20 May 2026, and dnoticias.pt, 12 August 2026 (bench 12 from 2026/27, 22 under-23 players in the squad); record.pt, 4 December 2014 (8 locally trained players, 27 senior contracts); record.pt, 2 January 2019 (Taça da Liga: 2 locally trained starters); dnoticias.pt, 9 July 2024 (end of the "manifestação de interesse" route, windows of 12 and 4 weeks).
- A comparative study of foreign player quotas, updated 30 May 2008 (rdes.it): "No quota for non EU-players" for Portugal. A sports law guide, 2026 (chambers.com): no nationality quotas in Portugal, non-EU players need a residence visa.

## Change points found

| Season | Change |
|---|---|
| 2002/03 | Taça de Portugal replays abolished; FIFA windows compulsory; Primeira Liga named SuperLiga Galp Energia |
| 2005/06 | Primeira Liga 4 down, second tier 6 down, to cut both to 16; Segunda Divisão B becomes Segunda Divisão (4 series, play-off for 2 places) |
| 2006/07 | Primeira Liga and Liga de Honra 16 clubs |
| 2007/08 | Taça da Liga starts (32 clubs) |
| 2008/09 | Taça de Portugal semi-finals two legs; 8 rounds |
| 2009/10 | Terceira Divisão adds a Madeira series |
| 2012/13 | Second tier 22 clubs with B teams, which cannot be promoted |
| 2013/14 | Segunda Divisão and Terceira Divisão merge into the Campeonato Nacional de Seniores (80 clubs); Primeira Liga 15th in a play-off |
| 2014/15 | Primeira Liga back to 18; second tier 24 |
| 2015/16 | Third tier named Campeonato de Portugal; Taça da Liga first rounds single legs |
| 2016/17 | Second tier 22; Taça da Liga final four |
| 2017/18 | Second tier 20; Campeonato de Portugal 5 series of 16 |
| 2018/19 | Second tier 18; Primeira Liga 3 down (Gil Vicente reinstated); Taça de Portugal 4th substitute in extra time |
| 2019/20 | Primeira Liga 5 substitutes and bench 9 from June 2020; second tier abandoned; Campeonato de Portugal cancelled; Taça da Liga no extra time |
| 2020/21 | Primeira Liga 16th v second tier 3rd play-off; Campeonato de Portugal 96 clubs (transition); Taça da Liga 8 clubs |
| 2021/22 | Liga 3 starts (24 clubs); Campeonato de Portugal becomes tier 4; second tier 16th v Liga 3 play-off |
| 2022/23 | Taça da Liga 8 groups during the World Cup |
| 2023/24 | Liga 3 20 clubs; 5 substitutes and bench 9 made permanent |
| 2024/25 | Taça da Liga 8 clubs again; immigration route for non-EU players changed (June 2024) |
| 2026/27 | Bench 12 in Liga competitions; Campeonato de Portugal new relegation format |

## Facts with two values

- 2012/13 Taça de Portugal: infobox 165 entrants, text 162. The row uses 162.
- 2018/19 and 2019/20 Taça da Liga entrants: no number on the page. The row gives the sum of entrants per round (inference).
- 2019/20 Supertaça date: the page gives "August 2019" only.

## Club moves

`por-clubs.tsv` lists every real club of each division whose real size or structure differs from the season database. The database holds 6 Portuguese divisions in every season: Premier League 18, Second League 18, Second Division B North, Central and South (20 each), Third Division (94 to 2018, then 12 to 38), plus a pool "A Lower Division" and, from about 2021, a "Reserve" division.

- Listed: Primeira Liga 2007/08 to 2013/14 (16 clubs); second tier 2007/08 to 2017/18 (16, 22, 24, 22, 20); third tier where a zone is not 20 (2002/03 Centro, 2003/04 Norte) and every season from 2005/06 (4 series, then 3 zones of 16, then 8 series); Terceira Divisão every database season to 2012/13; Liga 3 and Campeonato de Portugal from 2021/22.
- Not listed: seasons with no database (2004/05, 2006/07, 2026/27, 2027/28). Seasons where the real size equals the database size (Primeira Liga and second tier outside the years above, third tier 2001/02).
- Matching is by name, by script, against that season's database. `db_club_id` is empty when no name match was clear. Club ids differ between databases (Sporting is 10915 in November 2007 and 2656 in October 2021).
- From 2013/14 to 2020/21 no national fourth tier exists. The database still has a Third Division of 94 clubs (to October 2018) for those seasons. Max decides what happens to those clubs.

## Missing clubs

Count of real clubs with no confident match in the database, after the reconcile of 2026-10-16. Names of up to 8 shown. The reconcile filled the 55 spelling misses that the check found and fixed three wrong matches (see "Reconcile"). Divisions with no missing club are left out.

| Season | Division | Real clubs | No match | Clubs |
|---|---|---|---|---|
| 2001/02 | Terceira Divisão | 117 | 17 | Águias Graça, União Nogueirense, GD Milheiroense, AD Sátão, GD Portalegrense, CR Alqueidão da Serra, CD São Vicente, AD Carregado, ... |
| 2002/03 | Terceira Divisão | 117 | 21 | FC Marinhas, Águias Graça, Fiães SC, AD São Pedro da Cova, FC Pampilhosa, GD Milheiroense, AD Sátão, UD Tocha, ... |
| 2003/04 | Terceira Divisão | 118 | 34 | Santa Maria FC, CA Cabeceirense, AD Ponte da Barca, CD Rebordelo, Fiães SC, Aliados Lordelo, AD São Pedro da Cova, União Nogueirense, ... |
| 2005/06 | Segunda Divisão | 58 | 8 | União Torcatense, Fiães SC, Aliados Lordelo, GD Tourizense, FC Pampilhosa, SL Nelas, RSC Queluz, Vitória Setúbal B |
| 2005/06 | Terceira Divisão | 115 | 40 | Brito FC, Atlético Cabeceirense, Mondinense FC, FC Vinhais, ADC Correlhã, AC Vila Meã, AD São Pedro da Cova, CR Ataense, ... |
| 2007/08 | Segunda Divisão | 55 | 8 | Vila Meã, Fiães, Eléctrico, Pampilhosa, Nelas, Sátão, Carregado, Messinense |
| 2007/08 | Terceira Divisão | 94 | 34 | Vieira SC, Mondinense FC, FC Marinhas, Vidago FC, GD Prado, Brito SC, Morais FC, Padroense FC, ... |
| 2008/09 | Segunda Divisão | 47 | 8 | Aliados Lordelo, Santana, FC Pampilhosa, GDR Monsanto, Eléctrico FC, AD Carregado, Real SC, Mineiro Aljustrelense |
| 2008/09 | Terceira Divisão | 93 | 29 | Vieira SC, GD Prado, FC Marinhas, Mondinense FC, FC Mãe de Água, Padroense FC, SC Coimbrões, AC Vila Meã, ... |
| 2009/10 | Segunda Divisão | 47 | 10 | Aliados FC Lordelo, Padroense FC, Vieira SC, FC Pampilhosa, Eléctrico FC, GDR Monsanto, Atlético de Reguengos, Aljustrelense, ... |
| 2009/10 | Terceira Divisão | 94 | 29 | Santa Maria FC, FC Marinhas, Morais FC, AC Vila Meã, SC Coimbrões, Fiães SC, CD Candal, GD Milheiroense, ... |
| 2010/11 | Segunda Divisão | 48 | 8 | CF Andorinha, Padroense FC, SC Coimbrões, Aliados FC Lordelo, FC Pampilhosa, Eléctrico FC, Atlético SC Reguengos, Real SC |
| 2010/11 | Terceira Divisão | 94 | 41 | SC Melgacense, Vieira SC, Santa Maria FC, UD Sousense, CD Candal, AC Vila Meã, Mondinense FC, FC Alpendorada, ... |
| 2011/12 | Segunda Divisão | 48 | 2 | Marítimo B, Atlético SC |
| 2011/12 | Terceira Divisão | 93 | 28 | SC Melgacense, FC Marinhas, AD Grijó, SC Mêda, FC Alpendorada, UD Sampedrense, AD Nogueirense, UD Tocha, ... |
| 2012/13 | Segunda Liga | 22 | 1 | Vitória de Guimarães B |
| 2012/13 | Terceira Divisão | 82 | 3 | GD Parada, Atlético S.C., Aljustrelense |
| 2013/14 | Campeonato Nacional de Seniores | 80 | 6 | Vitória de Guimarães B, Lusitano Vildemoinhos, Águias do Moradal, Carapinheirense, Almodôvar, Ferreiras |
| 2014/15 | Campeonato Nacional de Seniores | 80 | 1 | Atl. Reguengos |
| 2014/15 | Segunda Liga | 24 | 1 | Vitória de Guimarães B |
| 2015/16 | Campeonato de Portugal | 80 | 2 | Varzim B, Atlético de Reguengos |
| 2015/16 | LigaPro | 24 | 1 | Vitória de Guimarães B |
| 2016/17 | LigaPro | 22 | 1 | Vitória de Guimarães B |
| 2018/19 | Campeonato de Portugal | 72 | 5 | Chaves B, Mirandês, Mação, Santa Iria, Vasco da Gama Vidigueira |
| 2019/20 | Campeonato de Portugal | 72 | 1 | Chaves B |
| 2020/21 | Campeonato de Portugal | 96 | 10 | Águia Vimioso, Vidago, Brito, Rio Ave B, Vila Cortez, GRAP, Belenenses SAD B, Estrela da Amadora, ... |
| 2021/22 | Liga 3 | 24 | 1 | União de Santarém |
| 2022/23 | Campeonato de Portugal | 56 | 1 | Atlético |
| 2023/24 | Campeonato de Portugal | 56 | 1 | Alverca B |
| 2025/26 | Campeonato de Portugal | 56 | 3 | Chaves B, Ribeira Brava, Portimonense B |

The Primeira Liga, Liga de Honra and LigaPro lists not in this table have a match for every club.

The database count per real division also matters. Example: the real third tier from 2013/14 has 80 clubs, the database 60 in three divisions. The rest must come from the Third Division or the pool.

## Unknowns

- Bench size in Liga competitions before 2019/20 (7 in 2019/20 before June 2020; start of 7 not found).
- Bench size and the start of 5 substitutes in FPF competitions (lower leagues, Taça de Portugal, Supertaça).
- Foreign player rules 2001/02 to 2006/07. The original uses 4/4/3 non-EU by division. No source for those seasons was found.
- Start season of the 8 locally trained players rule (found in force in 2014/15). Squad registration in other seasons.
- Work permit rules before 2024.
- Exact transfer window dates per season; transfer windows in 2001/02.
- 2026/27 Campeonato de Portugal promotion places to Liga 3, and the 2026/27 Taça de Portugal format change.
- 2027/28: no season pages.

## Inferences

- 5 substitutes and bench 9 in 2020/21 to 2022/23 (the 2023 assembly made a temporary rule permanent).
- Liga rules on substitutes and bench apply to the second tier and the Taça da Liga, not only the Primeira Liga.
- No non-EU quota from 2008/09 to 2025/26 (none in 2008, none in 2026).
- Lower-tier promotion and relegation counts read from table colours where the text gives none.
- 2027/28 rows copy 2026/27.

## Reconcile

Reconcile date 2026-10-16. The full list is in `por.reconcile.tsv` (20 rows: the 17 disputed fields and 3 follow-on fields). Third publishers come mostly from Arquivo.pt copies of Portuguese papers of the time. Two primary records helped: the FPF competition rules (RPO, April 2005 text) and FPF Comunicado Oficial 17 of 17 July 2007. I changed 18 fields in `por.tsv`, added each URL to `sources` and kept row order and ids.

| Decision | Fields |
|---|---|
| first | 4 (por-21 down, por-23 down, por-70 format, por-92 down) |
| second | 9 (por-3, por-6, por-12, por-30, por-57, por-102, por-119, por-173, por-223) |
| conflict | 2 (por-39 down, por-41 down) |
| found value accepted | 2 (por-164 bench 7, por-170 bench 7 as an inference) |
| found value rejected | 0 |

How I decided:

- por-3, por-12, por-21: from 2001/02 to 2003/04 the 16th of each Segunda Divisão B zone played a three-club play-off when the Açores champion went up (RPO 402.02). In 2001/02 one club stayed. In 2002/03 and 2003/04 one club went down. RSSSF counts the play-off loser in its relegation total, Wikipedia does so in 2002/03 only. The `down` fields now hold the direct places (12, 11, 11) and `playoffs` describes the play-off.
- por-23: Maisfutebol lists four clubs to district level in each series A to F, and RPO 502.02 says the same. I applied the same rule text to por-5, por-14 and por-32, which said "about 4".
- por-30: the Açores play-off (RPO 502.01) is a standing rule. A União says it is played "ano após ano".
- por-57: Comunicado Oficial 17 gives 20 places. 19 clubs went down because Série B had 13 clubs.
- por-92, por-119: the brief rule for `up` and `down` applies. Outcomes are in `notes`.
- por-173: the season started as LigaPro. The name Liga Portugal SABSEG dates from 16 October 2020.

Conflicts for Max:

- por-39 (2005/06 Segunda Divisão, down). First value 13 (3, 3, 3, 4). Second value 15 (3, 3, 3, 6). Maisfutebol (May 2006): an FPF rule change in mid-season sent one extra club down for each relegated B team. Vitória de Setúbal B went down, so União Micaelense also went down. That gives 5 in Série D and 14 in all. RSSSF also marks Benfica B, which stayed. Decide whether `down` holds 13 (rules at the start) or 14 (rules as changed). The value stays 13 for now.
- por-41 (2005/06 Terceira Divisão, down). First value "about 4 per mainland series". Second value A 6, B 5, C 5, D 4, E 6. I found no 2005/06 rule text. A Jornal Regional headline names three Série A clubs going to district level, RSSSF marks six. The value stays as first.

Club matches (`por-clubs.tsv`):

- 55 spelling misses filled from the season database, as the check listed.
- Three wrong matches fixed, and the check was wrong on two of them. Atlético CP in Liga 3 (2023/24 to 2025/26) is id 989 "Atlético CP" in October 2023, October 2024 and December 2025, not an empty id. Académica – SF (2015/16, 2016/17) is id 222 "Associação Académica de Coimbra/Secção de Futebol" in October 2015 and October 2016. Lusitano FCV (2014/15) is id 1911 "Lusitano Futebol Clube Viseu", as the check said.
- The missing-club table above is updated. 2013/14 Lusitano Vildemoinhos stays missing. Autumn 2013 has only "Lusitano Futebol Clube" (6845), and I don't know if that is the Viseu club.

Items for Max that the reconcile did not change:

- The Açores play-off of RPO 502.01 applies in every season to 2004/05. The `playoffs` fields of por-3, por-12 and por-21 do not name it.
- por-32 (2004/05 Terceira Divisão) `up` counts the Açores winner as promoted. Madalena went up only through the Açores play-off.

## Loose ends

- Açores play-off (por-3, por-12, por-21): the RPO (articles 401 and 502.01) caps the Açores clubs in the Segunda Divisão B at 3. Only when the cap stops the Açores series winner of the Terceira Divisão does it play the worst-placed Açores club of the Segunda Divisão B over two legs. The play-off is conditional, not yearly. The `playoffs` fields now state the rule. In 2001/02 to 2003/04 the 16th-place play-off took place, which RPO 402.02 holds only when the Açores winner goes up, so the Açores play-off was not needed (inference).
- por-32 (Terceira Divisão 2004/05) `up`: now 12 (top 2 of each mainland series). The Açores winner goes up direct only when the cap allows. In 2004/05 the cap applied and Madalena went up through the play-off against Lusitânia (por-30). The `playoffs` field now describes it.
- Database club 6845 "Lusitano Futebol Clube" is the Vila Real de Santo António club, not the Viseu club. Other databases name 6845 "Lusitano FC Vila Real de Santo António" or "Lusitano Futebol Clube VRSA". The Viseu club is 1911 ("Lusitano Futebol Clube Viseu", "Lusitano FCV"), and in Autumn 2013 id 1911 holds another club. Lusitano Vildemoinhos stays missing for 2013/14. No change to por-clubs.tsv.

## Source fill

Date: 2026-10-16. A source-fill pass added `field=URL` entries to `sources` for filled fact fields that had none. It changed no values and kept row order and ids. Each URL was read on this date, and the value was checked against the page text.

Method:

- For `name`, the pass took the pages that the row already cites, season pages first, and kept the first page that contains the name. For a name with a note in brackets, the base name had to match. For an Italian group (girone), the page also had to name that group.
- For a value of `none` in `split` or `playoffs`, the source is the season page with one league stage and no championship or relegation round.

Fields filled:

- `name`: 115 rows.
- `split` = `none`: 22 rows.
- `split` = second stage with promotion and relegation groups: 7 rows (por-57, 66, 111, 120, 129, 138, 183), each from its season page.
- `format` of Liga 3: 7 rows. 2021/22 to 2025/26 cite the season page ("home-and-away double round-robin", then promotion and relegation series). The rows with no season page cite the Liga 3 main page.
- `format` of por-235 (Primeira Liga 2027/28) and por-236 (Segunda Liga 2027/28): the main league pages, which say that each club plays the others twice, home and away.

Disagreements: none found.

Lint after the pass: 0 problems.
