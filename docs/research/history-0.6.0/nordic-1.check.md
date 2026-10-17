# Check pass, part nordic-1: Sweden

Date 2026-10-17. Brief: `brief.md`. Checked file: `nordic-1.tsv`. Result file: `nordic-1.check.tsv`.

The second source is RSSSF, one season page per year (`https://www.rsssf.org/tablesz/zwedYY.html`, 2002 to 2009). I read every page raw with curl. The first source of every row is English Wikipedia, so RSSSF is another publisher. worldfootball.net refused the requests (HTTP 403). I found no other archive with the regional groups of these years.

## Counts

312 fields checked.

| Result | Count |
|---|---|
| agree | 225 |
| disagree | 1 |
| no second source | 86 |

I did not check the 73 `ended` fields that hold `unknown`, because they hold no fact.

## Disagreement

Swedish Second Division East Svealand, label 2004, `second`. The row holds Vasalunds IF (Vasalund/Essinge IF). The RSSSF final table of Division 2 Östra Svealand 2004 has Valsta Syrianska IK 2nd with 43 points (13 wins, 4 draws, 5 losses, 45-19) and Vasalund/Essinge IF 3rd with 41 points. The Wikipedia table shows Valsta Syrianska IK with the same record but 40 points, and has no source for it. A record of 13 wins and 4 draws gives 43 points. A points deduction could explain the 40, but I found no page that says so. The lead must decide. I recommend Valsta/Syrianska IK, the name the row uses for this club in 2003.

## How the checks were made

- Allsvenskan, Superettan and Division 1 (2006 to 2009): the RSSSF final tables.
- Division 2 of 2004 and 2005, Division 3 of 2005 and 2006, and Division 2 of 2006 to 2008: the RSSSF final tables.
- 2003, Division 2 and Division 3: RSSSF gives the match results of every group. A script added up the tables from the results (3 points a win, then goal difference, then goals scored). Every top team had 22 matches. The Norra Norrland group also has its final table on the page.
- Swedish Cup: the RSSSF cup final of each year.
- `ended`: the RSSSF date of the last round or of the final.
- `not held` rows of 2006 to 2008: the RSSSF pages show only Division 1 Norra and Södra at the third level, and only 6 Division 2 groups at the fourth level. For these rows the check confirms only that no group of that name exists. The map of the 12 old fourth-level groups onto the 6 new groups is still a choice for the lead.

## Points to note

1. In 2008, RSSSF names a fourth-level group "södra Svealand". The research maps Third Division East Svealand 2008 to a group it calls Östra Svealand. Both sources give the same top two (Karlslunds IF, Eskilstuna City FK), so the values agree.
2. The 2005 Division 2 groups Norra Svealand and Mellersta Götaland exist on RSSSF with the same top two as the research rows. The map of West Svealand and East Gotaland onto these groups is an inference in the research, and the check confirms only the values.
3. Ties on points that RSSSF orders the same way as the research: Division 2 Västra Svealand 2004 (46 each), Östra Svealand 2005 (41 each), Västra Götaland 2003 and Södra Götaland 2003 (on goal difference), Division 1 Södra 2009 (46 each).

## No second source (86 fields)

- Division 2 and Division 3 of 2002 (36 fields). The RSSSF 2002 page has only the first and second levels.
- Division 3 of 2004 (24 fields). The RSSSF 2004 page has no fourth level.
- Third Division North Svealand 2009, Dalkurd FF and Skiljebo SK (2 fields). The RSSSF 2009 page has no fourth level.
- Third Division Southwest Gotaland 2009, `not held` (1 field). Same reason.
- `ended` of Superettan 2002 to 2004 (3 fields). RSSSF gives no dates for these rounds.
- `ended` of Division 1 2006 to 2009 and of Division 2 2006 to 2008 (20 fields). RSSSF gives no dates for these groups.
