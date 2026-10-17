# Spain and Portugal: check pass

Part `esp-por`, slice 11c. The check covers every filled field in `esp-por.tsv`: 101 rows, 88 of them `yes` and 13 `not held`. For `yes` rows it checks `first`, `second` and `ended`. For `not held` rows it checks `held`. That gives 277 fields. All pages were read raw on 2026-10-17.

## Counts

| Result | Fields |
|---|---|
| agree | 247 |
| disagree | 0 |
| no second source | 30 |

## Second sources

The research rows cite RSSSF for nearly every field, so the check uses other publishers.

- Spanish First Division, Portuguese Premier League: the English Wikipedia season pages (final table and infobox dates).
- Spanish Second Division and the four Segunda División B groups: BDFutbol (`bdfutbol.com/es/t/t<season>2a.html` and `...2aB<group>.html`), final tables and match dates. The B group rows cite the Spanish Wikipedia and RSSSF, so BDFutbol is a third publisher.
- Spanish Cup, Spanish Super Cup, Portuguese Cup, Portuguese Super Cup: the English Wikipedia pages for each final or each Supercopa and Supertaça edition.
- Portuguese Second League: the English Wikipedia season pages (Segunda Liga 2001/02 to 2004/05, Liga de Honra 2005/06 to 2007/08).
- Portuguese Second Division B zones 2001/02 to 2004/05: the English Wikipedia season pages, one table per zone.
- `not held` zones, label 2006: the English Wikipedia page for the 2005/06 Segunda Divisão shows four series A to D and a championship play-off. Label 2009 (South): the RSSSF list of third-tier champions gives one national champion from 2005/06 on. The research row for 2009 cites only Wikipedia.
- Portuguese Third Division, label 2002, `first`: the RSSSF list of fourth-level champions gives CD Mafra for 2001/02. The research row cites only Wikipedia.

## Disagreements

None.

## Fields with no second source

- Portuguese Second League, `ended`, labels 2002 to 2008 (7 fields). The Wikipedia pages give no date for the last round. Worldfootball, zerozero, playmakerstats, foradejogo, fbref and footballdatabase refused the request or showed a bot check.
- Portuguese Second Division B zones, `ended`, labels 2002 to 2005 (12 fields). The research marks these values as inferences. No dated second source was found.
- Portuguese Second Division B zones, `held`, labels 2007 and 2008 (6 fields). The research cites both Wikipedia and RSSSF, and no third publisher was reachable. Both of those publishers show the four-series format, so nothing contradicts the rows.
- Portuguese Third Division, label 2002, `second` and `ended` (2 fields). RSSSF names only the champion. The research marks `ended` as an inference.
- Portuguese Third Division, `held`, labels 2003 to 2005 (3 fields). The research cites both Wikipedia and RSSSF. The RSSSF list of fourth-level champions also says no final play-off took place from 2002/03 to 2012/13, but it is the same publisher.

## Notes on agreed fields

- Spanish Super Cup: the winner follows the aggregate score on Wikipedia, for example 2003 Mallorca 2, Real Madrid 4 and 2008 Valencia 5, Real Madrid 6.
- Spanish Second Division B, `ended`: BDFutbol gives the last regular-season match day in May for all 16 group rows. This agrees with the research note that `ended` is the end of the regular phase.
- Portuguese Second Division B Centre 2002/03, `second`: Wikipedia names Estrela Portalegre. The research row and note give the same club.
- Spanish Second Division B3 2003/04, `second`: BDFutbol names Lorca Deportiva, the same club as the research row.
