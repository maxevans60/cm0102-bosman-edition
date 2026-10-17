# Italy: check pass

Part `italy`, slice 11c. The check covers every filled field in `italy.tsv`: 82 rows, first and second only, 164 fields. No row is `not held`. The research source for every row is the Italian Wikipedia. The second source for every field is RSSSF, read raw on 2026-10-17.

## Counts

| Result | Fields |
|---|---|
| agree | 163 |
| disagree | 1 |
| no second source | 0 |

## Second sources

- Serie A: the RSSSF season pages `ital02.html` to `ital09.html`.
- Serie B and the Serie C1 and C2 groups: the same season pages for 2001/02 to 2003/04, then `ital2-05.html` to `ital2-09.html` (Serie B), `ital3-05.html` to `ital3-09.html` (C1) and `ital4-05.html` to `ital4-09.html` (C2).
- Italian Cup: `italcuphist.html`. Italian Super Cup: `italsupcuphist.html`. Italian Serie C Cup: `italcup2hist.html`.
- Italian C1 Super Cup: `ital3champ.html`. It lists both group winners of each season and marks the Supercoppa winner. It does not name the loser. The page says that the two group winners play off, so the check takes the other group winner as the loser. This is an inference.

The official Serie C honours list (seriec.com) also gives the same Serie C Cup winners for 2001/02 to 2006/07.

## Disagreement

Italian Serie C2/B, label 2009 (Lega Pro Seconda Divisione 2008/09, girone B), field `second`. The research row holds Giulianova. RSSSF (`ital4-09.html`) lists the play-off final as Prato against Giulianova, 0-1 and 1-0, and names Prato as the winner.

The Italian Wikipedia gives the same two scores, 1-1 on aggregate after extra time. It says that Giulianova went up as the better-placed club. The RSSSF table on the same page has Giulianova 3rd and Prato 5th. Older RSSSF pages say that a tie on goals goes to the better regular-season place. My inference is that RSSSF names the wrong club and the research value is right. I found no third source that names the winner.

## Notes on agreed fields

- Serie A 2004/05 (label 2005): RSSSF has Juventus 1st and marks it "Stripped of Title July 2006". The row follows the result on the pitch, as the research `.md` says.
- Serie A 2005/06 (label 2006): RSSSF has the table after the July 2006 rulings, with Internazionale 1st and AS Roma 2nd.
- Serie B 2004/05 (label 2005): RSSSF has Empoli 1st and Torino 2nd, with Genoa placed last.
- RSSSF uses other names for some clubs: Florentia Viola (C2/B 2002/03), Napoli Soccer (C1/B 2005/06), Gela Juve Terranova (C2/C 2004/05), A.S. Calcio Potenza (C2/C 2006/07). They are the same clubs as in the rows.
