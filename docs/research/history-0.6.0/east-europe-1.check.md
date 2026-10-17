# Check pass: east-europe-1 (Poland, Croatia, Czech Republic)

Check of `east-europe-1.tsv` against publishers other than RSSSF. All pages were read on 17 October 2026 with `curl` (Wikipedia as raw wikitext, 90minut.pl as raw HTML) and grep. The lines are in `east-europe-1.check.tsv`.

## Counts

The file has 275 check lines for 112 rows. One line covers each filled `first`, `second` and `ended` field, and the `held` field of each `not held` row.

| Result | Lines |
|---|---|
| agree | 261 |
| disagree | 0 |
| no second source | 14 |

The 23 `ended` fields that hold `unknown` have no line, because they hold no fact. These are the 22 Croatian third-level rows to 2007/08 and Czech Second Division 2002.

## Disagreements

None. No first, second, held or ended value conflicts with the second source.

## No second source (14 lines, all `ended`)

I found no end date on a second publisher for these rows:

- Croatian Second Division 2006 (2006/07). Croatian Wikipedia lists the autumn rounds only.
- Croatian Second Division North 2002 to 2005 and South 2002 to 2005.
- Croatian Third Division South 2008 and 2009. Croatian Wikipedia has the tables with no dates.
- Czech Second Division 2003, 2004 and 2005.

worldfootball.net returned a bot challenge, so I did not use it.

## Second sources used

- Poland: English Wikipedia season pages and the Polish Cup finals table, Polish Wikipedia (II liga seasons, Superpuchar Polski, Puchar Ekstraklasy), and 90minut.pl league pages for the last match day.
- Croatia: English Wikipedia (Prva HNL and Druga HNL seasons, the Cup finals table and final pages, the Super Cup table and match pages) and Croatian Wikipedia (3. HNL seasons).
- Czech Republic: English Wikipedia (First League and Cup seasons) and Czech Wikipedia (Gambrinus liga, 2. liga).

The Ekstraklasa Cup page and some Croatian Wikipedia 3. HNL group pages give RSSSF as their source. Their agreement is weaker evidence than the other lines.

## Points for the research agent

- Czech Second Division 2003 (2002/03). The English Wikipedia table puts Vysocina Jihlava second and Opava fifth, but still marks Opava as promoted. Czech Wikipedia has Plzen 62 points and Opava 60 points, the same as RSSSF. I marked the row `agree` on the Czech Wikipedia page. The English table looks wrong.
- Polish Second Division 2002: 90minut.pl gives 5 May 2002 for round 38, which agrees with `2002-05`.
- Polish Super Cup 2009: the note says Lech won 5-4 on penalties. Polish Wikipedia gives 4:3 and English Wikipedia 3-4 from the Wisla side. The winner is the same. The date is 25 July 2009 on Polish Wikipedia and 27 July 2009 on English Wikipedia. Both give July, so `ended` agrees.
- Croatian Third Division Central 2005: Wikipedia puts Vinogradar at Mladina near Jastrebarsko and RSSSF puts it at Lokosin Dol. It is the same club.
- Croatian Super Cup: English Wikipedia lists 1999 to 2001 as "not held" with two different qualifiers each year. This is outside the range, but it may matter if anyone uses those years as proof of the winner-only convention. The `.md` uses the double years 1995 to 1998.
- Polish FA Cup 2005: English Wikipedia confirms that PZPN took the title away on 2 September 2020.

## The six inferred name matches

The club files have no town column, so a town match cannot be proven from the input. Wikipedia confirms the real club in each case:

| Row | Real club (Wikipedia) | Data name | Finding |
|---|---|---|---|
| East 2005 first | Croatia Đakovo | NK Croatia | The only "NK Croatia" apart from NK Croatia Sesvete. Plausible. |
| West 2004 second | Rudar Labin | NK Rudar | The only Rudar in the Croatian file. Plausible. |
| West 2005 first | Jadran Poreč | NK Jadran | NK Jadran (KS) and NK Jadran Luka Ploce are the other Jadrans. Plausible. |
| South 2005 first | GOŠK Dubrovnik | NK GOSK | GOSK Adriachem is the other GOSK. Plausible. |
| East 2003 second, East 2004 first | Graničar Županja | NK Granicar | Two entries have the full name NK Granicar, short names "Granicar" and "Granicar (K)". The full name cannot tell them apart. |
| North 2003 first | Međimurje Čakovec | NK Medimurje | The full name exists in the September 2005 data only. Plausible. |

Croatian Wikipedia gives "Višnjevac" for East 2003, which confirms the inferred spelling. The "not in data" names were checked against `clubs/Croatia.tsv`: Polet Pribislavec, Naftas Ivanic Grad, Rudes Zagreb, Dugopolje, Hrvace and Zagora Unesic are absent. All other filled names exist as full names in the clubs files.
