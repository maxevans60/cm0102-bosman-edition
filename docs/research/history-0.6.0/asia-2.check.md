# Asia 2: check pass

Part `asia-2` (Hong Kong, India, Indonesia, Malaysia, Singapore, Thailand). I checked every filled field in `asia-2.tsv` on 2026-10-17. The fields are `first`, `second` and `ended` of the `yes` rows, and `held` of the `not held` rows. Fields with `unknown` and the `unknown` rows have no line. All pages were read raw with curl.

## Counts

| Result | Lines |
|---|---|
| agree | 169 |
| disagree | 4 |
| no second source | 76 |
| total | 249 |

## Method

- The check page is from a publisher that is not the row's first source for that field. When a row gives two sources for a field, I used the second one if it is another publisher.
- Second sources: RSSSF season pages and history lists, the English Wikipedia lists and season pages, the zh Wikipedia list of Hong Kong third division champions (for rows whose source is RSSSF), and one archived HKFA page.
- worldfootball.net returned a bot check. The web archive was offline for most of the day, so the indianfootball.de pages could not be read.

## Disagreements

1. Hong Kong League Cup, label 2005, `ended` 2005-02. Wikipedia gives the final (Sun Hei 1-0 Happy Valley) as 27 December 2004 and cites an HKFA report. The archived HKFA page is dated 2004-12-27. RSSSF, the row's source, gives the final as 27 February. The real value is probably 2004-12. The label (2004/05) does not change.
2. Malaysian League Premier One, label 2003, `ended` 2003-08. Wikipedia gives the season as 9 February to 6 July 2003. RSSSF, the row's source, has Round 26 on 2 August 2003. The two publishers differ. The 13 and 16 August matches on RSSSF are a Super League qualifying round, not league matches.
3. Thai Premier League, label 2003, `ended` 2004-08. RSSSF has the last league round (Round 18) on 3 July 2004. 11 August 2004, which the row uses from Wikipedia, is the second leg of the promotion/relegation playoff. The row does not count the playoff for 2002/03 (31 May and 3 June 2003), so the two rows use different rules. By the brief (last match day of a league) the value is 2004-07.
4. Indian Chennai League, label 2003, `first` Integral Coach Factory. Wikipedia gives Indian Bank for 2002-03, Madras Sporting Union for 2003-04 and ICF for 2004-05. RSSSF gives Madras Sporting Union as the Chennai champion of 2004, so the Wikipedia year "2003-04" is the 2004 season. By that mapping the 2003 champion is Indian Bank. This is an inference from the two lists.

## Points of note

- Indian Bangalore Super Division, label 2005. RSSSF calls this edition 2005/06. It gives Madras Engineering Group Centre as runner-up and the last match on 25 February 2006. The row has the runner-up and `ended` as `unknown`. If the label stays 2005, `ended` is 2006-02.
- Indonesian Premier Division 2002. Wikipedia has the final as Persita 1-2 Petrokimia Putra after extra time, in line with the row. Its lead text says 3-2. Only the note is affected.
- The `no second source` lines are mostly lower divisions and cups. RSSSF has only part-season tables for Hong Kong Second Division 2001/02, Third Division A and B 2001/02, Third Division B 2004/05 and Singapore Prime League 2004. RSSSF has no Junior Shield before 2005/06 and no Singapore FA Cup before 2005. Most of the `ended` lines with no second source are cup finals and league ends where only RSSSF gives a date.
- Hong Kong Second Division 2003/04 champion Citizens agrees only by inference. RSSSF gives "Promoted: Citizen" and no table.
