# Brazil: check pass

Check of `brazil.tsv` for slice 11c. All pages were read on 2026-10-17 with curl and grep. The lines are in `brazil.check.tsv`.

## Counts

There are 493 checked fields: `first`, `second` and `ended` for the 151 held rows (`ended` only where it is filled), and `held` for the 45 `not held` rows.

| Result | Count |
|---|---|
| agree | 465 |
| disagree | 5 |
| no second source | 23 |

## Second sources

The row's `first` and `second` sources are Wikipedia (English or Portuguese). Its `ended` sources are Wikipedia, RSSSF or RSSSF Brasil. I treat RSSSF and RSSSF Brasil as one publisher. Each field was checked against a page of a different publisher:

- State championships, `first` and `second`: the RSSSF Brasil season pages (live site, `www.rsssfbrasil.com`).
- State championships, `ended`: Bola na Área season pages when the row cites RSSSF Brasil, and RSSSF Brasil when the row cites Wikipedia.
- National First, Second and Third Division: Bola na Área final placings for the places, and the RSSSF season pages for the month.
- Brazilian Cup: the RSSSF list of finals and the RSSSF season pages.
- Interstate cups: the RSSSF 2002 and 2003 pages for the places.
- `not held` rows: the RSSSF Brasil lists of champions, and a Lance page for the Copa dos Campeões.

## Disagreements

The five `disagree` lines cover four points. Point 1 has two lines.

1. Brazilian Bahia State Championship 2002, `first` and `second` (row: Esporte Clube Vitória, Fluminense de Feira Futebol Clube). Bahia had two titles in 2002. Bola na Área and the RSSSF list of Bahia champions give the state FA championship to Palmeiras do Nordeste, who beat Cruzeiro Futebol Clube (BA) in the final on 12 May 2002. Vitória won the Supercampeonato, with the final over Fluminense de Feira on 20 June 2002. The RSSSF Brasil season page agrees with the row. The row is not consistent with Minas Gerais, Paraná and São Paulo 2002, where `brazil.tsv` uses the FA championship. The FA championship result is Palmeiras do Nordeste and Cruzeiro (BA), ended 2002-05. I did not find Palmeiras do Nordeste in `clubs/Brazil.tsv`.
2. Brazilian Santa Catarina State Championship 2002, `second` (row: Criciúma Esporte Clube). Bola na Área gives Avaí as second in the final placing. RSSSF Brasil names no overall runner-up. Figueirense won both stages: they beat Criciúma in the first-stage final and Avaí in the second-stage final. The research `.md` already lists this year as a point of doubt.
3. Brazilian Pernambuco State Championship 2004, `ended` (row: 2004-03). The final was Náutico against Santa Cruz on 11 and 18 April 2004, according to Bola na Área and the row's own RSSSF Brasil page. The real value is 2004-04.
4. Brazilian Pernambuco State Championship 2003, `ended` (row: 2003-08). Bola na Área dates the deciding Sport against Santa Cruz match 30 July 2003. RSSSF Brasil, the row's source, dates it 1 August 2003. I don't know which date is correct.

## No second source

- Ended month of the Minas Gerais Lower Division 2002 to 2009 and the Rio de Janeiro Lower Division 2002 to 2007. The row's source is RSSSF Brasil, and I found no page of another publisher with dates.
- Ended month of the Copa Centro-Oeste 2002, Copa do Nordeste 2002 and 2003, Copa Sul-Minas 2002 and Torneio Rio-São Paulo 2002. The row's source is RSSSF, and no Wikipedia season page exists. The RSSSF pages agree with the row.
- Ended month of the Amazonas (Northeast State Championship) 2002. The Bola na Área page for 2002 has no data.
- Runners-up of the Rio de Janeiro Lower Division 2008 (Tigres do Brasil) and 2009 (Olaria). The RSSSF Brasil pages for these years are linked but missing. The champions (Bangu, America) agree with the RSSSF Brasil list of champions.
- Torneio Rio-São Paulo 2009 `not held`. The RSSSF Brasil list ends at 2002 but was last updated in October 2008.

## The grouped state championships

The claim in `brazil.md` holds:

- The 3.9.68 rows of Brazilian Central State Championship name only clubs of Rio Branco, Acre. The 2001 row (Vasco da Gama, Rio Branco) is the real 2001 Acre result (RSSSF list of Acre champions).
- The 3.9.68 rows of Brazilian North State Championship name CSA, CRB, ASA and Miguelense, all clubs of Alagoas. The 2000 and 2001 rows (ASA) are the real Alagoas results.
- The 3.9.68 rows of Brazilian Northeast State Championship name Nacional, São Raimundo, Rio Negro and Clíper, all clubs of Manaus. The 2000 row (Nacional) and the 2001 row (Rio Negro) are the real Amazonas results.

The September 2009, February 2010 ODB, April 2010 SIM and June 2010 databases use the three names for other states. Each of their 2009 rows holds a 2008 state result (Bola na Área 2008 season pages):

- Central: Brasiliense and Dom Pedro II (Distrito Federal 2008), Ivinhema and Misto (Mato Grosso do Sul 2008), Mixto and União Rondonópolis (Mato Grosso 2008).
- North: Clube do Remo (Pará 2008 champion) and Holanda (Amazonas 2008 champion).
- Northeast: ABC (Rio Grande do Norte 2008 champion) and Fortaleza (Ceará 2008 champion).

## Other findings

- Three `ended` values that the row gives as `unknown` can be filled: Acre 2009 is 2009-05 (Juventus won the second-stage final on 3 May 2009, Bola na Área), Santa Catarina 2002 is 2002-07 (second-stage final 3 and 10 July 2002, RSSSF Brasil), and Santa Catarina 2003 is 2003-03 (final 16 and 23 March 2003, RSSSF Brasil).
- Two source URLs in `brazil.tsv` do not open. These are the archive copies of `tabless/sc2006.htm` and `tabless/sc2009.htm`. The live pages are at `https://www.rsssfbrasil.com/tablessz/sc2006.htm` and `.../tablessz/sc2009.htm`.
- The live RSSSF Brasil site answered on the day, so the archive copies are not needed.
