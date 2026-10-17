# Asia part 1: check pass

Part `asia-1` (Japan, South Korea, China PR and China). I checked every filled field in `asia-1.tsv` on 2026-10-17. I read all pages as raw HTML with curl and grep.

## Counts

| Result | Lines |
|---|---|
| agree | 287 |
| disagree | 2 |
| no second source | 65 |
| total | 354 |

The lines are `first`, `second` and `ended` of the 118 `yes` rows, and `held` of the 10 `not held` rows. No row uses `third` or `host`. The 10 rows with `ended` = `unknown` have no line for `ended`, because the field holds no value. The `competition` column adds the nation in brackets for the Chinese competitions, because each one has a China PR row and a China row.

## Rule for the second source

A source counts only if its publisher is not in the row's `sources` for that field. Wikipedia in any language is one publisher. When a row cites RSSSF only for `ended`, RSSSF counts as a second source for `first` and `second`, and Wikipedia counts for `ended`.

## Second sources

- Transfermarkt: final tables, full fixture lists with dates, and winners lists. These cover the J.League Cup, the Emperor's Cup, the Japanese Super Cup, J1 (2004 to 2009), J2, the JFL, the K League (2002 and 2003 tables, all winners), the Korean FA Cup, the Chinese top tier (all years) and the Chinese FA Cup (2005 and 2006). The winners lists also give the winners of the Chinese FA Cup and Super Cup, the Korean League Cup and the Korean Super Cup.
- RSSSF season pages: J1 2002 and 2003, Korea 2002 to 2009, China 2001 and China 2005 to 2008. I used them only where the row does not cite RSSSF for that field.
- Wikipedia: season pages for `ended` dates where the row cites RSSSF for `ended`. Chinese Wikipedia gives the Jia-B 2002 and 2003 results and some lower-tier and cup dates.

## Disagreements

1. Chinese FA Super Cup, label 2002 (China PR row and China row), `ended`. The row holds `2002-02`. The 2001 edition had two legs, on 26 February 2002 and 2 March 2002 (RSSSF China 2001). The row's own Wikipedia source gives the same two dates. The real value is `2002-03`. The winner and runner-up agree.

## No second source (65 lines)

- Japanese U-18 Championship and Japanese University League, 2002 to 2005 (16 lines). The JFA pages that I found give no history before 2011. Transfermarkt has no editions before 2011.
- Korean President Cup 2002 and 2003 (4 lines). The RSSSF pages for these years do not give the President Cup. I found no other page.
- Chinese FA Cup runner-up 2002 to 2004, and Chinese FA Super Cup runner-up for labels 2003 and 2004 (10 lines, both nation sets). Only Wikipedia and RSSSF give these finals, and the rows cite both.
- Chinese First Division B 2004 and 2005, all three fields (12 lines). Chinese Second Division 2002 to 2004, `first` and `second` (12 lines). The rows cite Wikipedia and RSSSF. Transfermarkt has no lower-tier tables before 2008.
- Other `ended` lines with no date on another publisher (11 lines):
  - Chinese First Division A 2002, 2004 and 2005.
  - Chinese First Division B 2006.
  - Chinese Second Division 2005 to 2007.

## Points of note

1. Chinese First Division A 2003. The Transfermarkt table has Shanghai Shenhua at the top, but its winners list leaves out 2003. This agrees with point of doubt 1 in `asia-1.md` (the title was revoked in 2013).
2. Chinese First Division A 2003 runner-up. Transfermarkt names the runner-up "Shanghai COSCO Sanlin". It has the same club id as "International Shanghai" (2004) and the later Beijing Renhe line, which went through Xi'an. This supports the inference that maps the club to Shaanxi Baorong Chanba FC in the China row.
3. The Transfermarkt China League One table for 2008 is incomplete. It has only a few matches and puts Nanchang Bayi Hengyuan second. I did not use it. RSSSF China 2008 gives Jiangsu Shuntian and Chongqing Lifan, which agree with the row.
4. The Transfermarkt fixture list for the 2008 Chinese Super League has only 29 matches. Its last match is on 30 November 2008. This agrees with `2008-11`, but the source is weak.
5. Chinese Second Division 2008 runner-up. RSSSF calls the club "Guangdong Rizhiquan". Rizhiquan is the Chinese name of the sponsor that Wikipedia calls Sunray Cave. So the club agrees with "Guangdong Sunray Cave".
6. Chinese Second Division 2003. The Chinese Wikipedia infobox gives the end as 17 November with the year 2004. The start date of the same page is in 2003, so I read the year as a misprint. The month agrees with `2003-11`.
7. J-League 1 2002 and 2003. The RSSSF aggregate tables put Yokohama F. Marinos (2002) and Jubilo Iwata (2003) second. This agrees with the rows.
8. Japanese Football League 2002 runner-up. Transfermarkt gives Sagawa Express Tokyo SC. The mapping to the data's Sagawa Soccer Club is an inference in `asia-1.md`, and a source cannot check it.
