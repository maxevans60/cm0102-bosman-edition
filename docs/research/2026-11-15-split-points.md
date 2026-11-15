# Split formats and points at the split (0.7.2 league sizes, gap rows 21 and 22)

Research of 15 November 2026 for the gap fixes of slice 7 (lead-notes/sizes-gaps.md rows 21 and 22). Two research
agents read the season pages and the tables. The full tables with every URL are kept outside the repo in
run-logs/0.7.2-sz07/gaps/research-nir-wal.tsv and research-points.tsv.

## Row 21: the meetings of the Northern Irish Premiership

The spec text of slice 7 said "2 meetings in place of 4". The rules row says 3 meetings, then 1 + 1. The row is right.

- 2002/03 (Irish League, 12 clubs): 3 meetings, 33 matches, then sections of 6 that meet once more, 38 matches.
  Sources: https://en.wikipedia.org/wiki/2002%E2%80%9303_Irish_League and RSSSF.
- 2003/04 to 2007/08 (16 clubs): 2 meetings, 30 matches, no split.
- 2008/09 to 2025/26 (12 clubs): 3 meetings, 33 matches, then sections of 6 that meet once more, 38 matches.
  Checked on the season pages of 2008/09, 2009/10, 2017/18, 2019/20, 2020/21, 2022/23 and 2025/26
  (https://en.wikipedia.org/wiki/2008%E2%80%9309_IFA_Premiership and later). 2019/20 stopped after 31 rounds, before
  the split.

The spec now says 3 meetings. The 2002/03 split text gave no section meetings ("play 5 more (38)"), so that row had no
split. It now reads "(38 matches)", and the encoder gives 1 + 1, as for 2008/09.

## Row 22: the points at the split

Every league that the rules table splits keeps the points in full, where a source shows the rule.

| League | Seasons | Points | Evidence |
|---|---|---|---|
| Scottish Premier League, Premiership | 2001/02 to 2026/27 | kept | Stated on https://en.wikipedia.org/wiki/Scottish_Premiership. 2027/28 keeps the last known rule. |
| IFA and NIFL Premiership | 2002/03, 2008/09 to 2026/27 | kept | Cumulative totals in the season tables, for example Larne 73 then 83 in 2025/26. |
| NIFL Championship | 2016/17 to 2018/19, 2021/22 to 2025/26 | kept | Cumulative totals, for example Limavady 71 then 81 in 2025/26. |
| Welsh Premier League, Cymru Premier | 2010/11 to 2027/28 | kept | The Cymru Premier page says the phase 1 points are brought forward. The 2026/27 page says results carry over. |
| K League | 2014 to 2018, 2021 to 2025 | kept | Cumulative totals (https://rsssf.org/tabless/skor2012.html and the season pages). |
| Danish Superliga | 2019/20 | kept | Stated on https://en.wikipedia.org/wiki/2019%E2%80%9320_Danish_Superliga. |
| Super League Greece | 2019/20 to 2022/23 | kept | Cumulative totals (https://rsssf.org/tablesg/grk2022.html). |
| Super League Greece 2 | 2020/21 | kept | Cumulative totals on the season page. |
| Ekstraklasa | 2019/20 | kept | https://en.wikipedia.org/wiki/Ekstraklasa: 2016/17 was the last season with half points. |
| Prva HNL and Druga HNL | 2002/03 to 2005/06 | kept | Cumulative totals (https://rsssf.org/tablesk/kroa03.html and the season pages). |

The split text of each of these rows now ends "points kept in full", and the encoder writes "points kept".

## Rows left as they were

- Super League Greece 2026/27: places 1 to 4 and 9 to 14 keep their points, places 5 to 8 get half (the 2024/25 and
  2025/26 pages, https://en.wikipedia.org/wiki/2025%E2%80%9326_Super_League_Greece). No source gives the rounding. The
  rules table has one points value a split, so the row stays "unknown". Slice 11 owns it.
- NIFL Championship 2019/20 (22 + 10 or 33 + 5 not settled), 2020/21 (cancelled), 2026/27 and 2027/28 (no points rule
  found).
- Rows whose split itself is wrong, for their slices:
  - K League 2011: no split, a regular season and play-offs. The 8 + 8 split began in 2012.
  - K League 2013: 14 clubs, 26 matches, then 7 + 7, 38 matches.
  - K League 2019 split after 33, and 2020 after 22 (27 matches). The rows have them the other way round.
  - Veikkausliiga 2018: one table of 33 matches, no split.
  - Veikkausliiga 2019: the "+3" is a play-off for the last Europa League place, not a third section.
  - Russian First Division 2010: 20 clubs, 38 matches, no split.
