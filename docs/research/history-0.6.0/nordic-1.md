# History research, part nordic-1: Sweden

Date 2026-10-17. Brief: `brief.md`. File: `nordic-1.tsv`, 151 rows for 21 Swedish competitions. 117 rows are `yes`, 34 are `not held` and 0 are `unknown`. All pages were read on 2026-10-17 with curl. A script read the league tables, and I checked the names by hand.

## Sources

- English Wikipedia season pages: `<year>_Allsvenskan`, `<year>_Superettan`, `<year>_Division_1_(Swedish_football)`, `<year>_Division_2_(Swedish_football)` and `<year>_Division_3_(Swedish_football)`, for 2001 to 2009.
- English Wikipedia `List_of_Svenska_Cupen_finals` for the cup.
- A Division 1 page does not exist for 2001 to 2005. This agrees with the tier structure below.

## Label conventions, with the 3.9.68 rows that prove them

| Competition | Convention | Proof |
|---|---|---|
| Swedish Premier Division | Calendar year | 3.9.68 2001 Hammarby IF / Djurgårdens IF = 2001 Allsvenskan, 1st and 2nd |
| Swedish First Division | Calendar year | 3.9.68 2001 Kalmar FF / Landskrona BOIS = 2001 Superettan, 1st and 2nd. 2000 Djurgårdens IF / Malmö FF = the first Superettan |
| Swedish Second Division (6 groups) | Calendar year | 3.9.68 2001 rows = 2001 Division 2 group winners and runners-up, all six (for example North IFK Luleå / Bodens BK = Division 2 Norrland) |
| Swedish Third Division (12 groups) | Calendar year | 3.9.68 2001 rows = 2001 Division 3 groups (for example Middle Gotaland Sandareds IF / Vara SK = Division 3 Mellersta Götaland) |
| Swedish Cup | Year of the final | 3.9.68 2000 Örgryte IS = the 1999–2000 cup, final in 2000. 1999 AIK / IFK Göteborg = 1998–99. 1998 Helsingborgs IF / Örgryte IS = 1997–98 |

The 3.9.68 Swedish Cup row of 2000 has no runner-up. The real runner-up was AIK (list of finals).

## Tier map

The 3.9.68 Second Division is the real Division 2 (6 groups) and the Third Division is the real Division 3 (12 groups). From 2000 to 2005, Division 2 was tier 3 and Division 3 was tier 4. In 2006 a new Division 1 in 2 groups became tier 3, Division 2 became tier 4 and Division 3 became tier 5. The rules table (`rules-0.6.0/den-swe.md`, decision 1) uses the same map for the Second Division.

| Data competition | 2002 to 2005 | 2006 to 2009 |
|---|---|---|
| Premier Division | Allsvenskan | Allsvenskan |
| First Division | Superettan | Superettan |
| Second Division North | Division 2 Norrland | Division 1 Norra |
| Second Division South Gotaland | Division 2 Södra Götaland | Division 1 Södra |
| Second Division East Svealand, West Gotaland | Division 2 group of the same name | `not held` |
| Second Division West Svealand | Division 2 Västra Svealand; 2005 Norra Svealand | `not held` |
| Second Division East Gotaland | Division 2 Östra Götaland; 2005 Mellersta Götaland | `not held` |
| Third Division (each of 12 groups) | Division 3 group of the same name | see below |
| Third Division North Svealand, East Svealand, South Gotaland | as above | Division 2 Norra Svealand, Östra Svealand, Södra Götaland |
| Third Division Middle Gotaland | as above | Division 2 Mellersta Götaland (2006, 2007), Östra Götaland (2008) |
| Third Division, the other 8 groups | as above | `not held` |

## Points of doubt

1. **Division 2 groups of 2005.** The 2005 page names the groups Norrland, Norra Svealand, Östra Svealand, Mellersta Götaland, Västra Götaland and Södra Götaland. Västra Svealand and Östra Götaland are gone. Norra Svealand holds most of the 2004 Västra Svealand clubs (BK Forward, Falu BS, Skiljebo SK, Västerås IK, Eskilstuna City). Mellersta Götaland holds most of the 2004 Östra Götaland clubs (Jönköpings Södra IF, Norrby IF, Husqvarna FF, Myresjö IF, Sandared, Grimsås, Oskarshamns AIK). I mapped them as successors. This is an inference.
2. **Third Division from 2006.** Tier 4 went from 12 groups to 6. I mapped a data group to a Division 2 group only when the names match (Norra Svealand, Östra Svealand, Mellersta Götaland, Södra Götaland). Division 2 Mellersta Götaland became Östra Götaland in 2008, and I kept Middle Gotaland on it. Division 2 Norrland and Västra Götaland have no data group of the same name, so their results are in no row. The other 8 data groups are `not held`. This is an inference and a choice for Max. There are two alternatives:
   - all 12 groups `not held` from 2006;
   - the brand, that is Division 3 (tier 5, the same 12 group names).
3. **Second Division from 2006.** North and South Gotaland follow Division 1 Norra and Södra. The databases of September 2009 and later keep only these two Second Division groups, which agrees with this map. The other four groups are `not held` for 2006 to 2008.
4. **Club competitions use first and second only.** Several database rows also hold a third club for the Premier and First Divisions. I left the third column empty, as the brief says.
5. **Ties on points.** Some groups have the top two level on points (for example Division 3 Sydöstra Götaland 2003). I took the order of the page table.

## Database rows that disagree

Most rows hold the result of the year before the label. This suggests that the year shift of slice 11b moved them one year too far. That is an inference.

| Database | Competition | Label | Database holds | Real |
|---|---|---|---|---|
| September 2005 | Premier Division | 2005 | Malmö FF / Halmstad BK (real 2004) | Djurgårdens IF / IFK Göteborg |
| September 2008 | Premier Division | 2008 | IFK Göteborg / Kalmar FF / Djurgårdens IF (real 2007) | Kalmar FF / IF Elfsborg |
| March 2009 | Premier Division | 2008 | IFK Göteborg / Kalmar FF / Djurgårdens IF (real 2007) | Kalmar FF / IF Elfsborg |
| April 2010 SIM | Premier Division | 2009 | IFK Göteborg / Kalmar FF / Djurgårdens IF (real 2007) | AIK / IFK Göteborg |
| September 2009, February 2010 ODB, June 2010 | Premier Division | 2009 | Kalmar FF / IF Elfsborg / IFK Göteborg (real 2008) | AIK / IFK Göteborg |
| September 2008, March 2009 | First Division | 2008 | IFK Norrköping / Ljungskile SK / GIF Sundsvall (real 2007) | Örgryte IS / BK Häcken |
| April 2010 SIM | First Division | 2009 | IFK Norrköping / Ljungskile SK / GIF Sundsvall (real 2007) | Mjällby AIF / Åtvidabergs FF |
| September 2008, March 2009 | Swedish Cup | 2008 | Kalmar FF / IFK Göteborg (real 2007 final) | IFK Göteborg / Kalmar FF |
| April 2010 SIM | Swedish Cup | 2009 | Kalmar FF / IFK Göteborg (real 2007 final) | AIK / IFK Göteborg |
| September 2009, February 2010 ODB, June 2010 | Swedish Cup | 2009 | IFK Göteborg / Kalmar FF (real 2008 final) | AIK / IFK Göteborg |
| November 2007, February 2008 | Second Division West Gotaland | 2007 | Ljungskile SK / Ytterby IS (the 2001 result) | `not held` |
| September 2008, December 2008, March 2009 | Second Division West Gotaland | 2008 | Ljungskile SK / Ytterby IS (the 2001 result) | `not held` |

The First Division rows of label 2009 in September 2009, February 2010 ODB and June 2010 agree (Mjällby AIF / Åtvidabergs FF). The February 2010 ODB rows hold the letter `T` in the third and host columns of the Cup, First Division and Premier Division rows. I do not know what it means.

## Gaps outside the range

These labels are missing in the data but are not in `targets.tsv`. They are for the lead to decide.

- Swedish Cup 2001 (the 2000–01 cup): IF Elfsborg beat AIK 1–1, 9–8 on penalties (`List_of_Svenska_Cupen_finals`).
- Swedish Third Division East Svealand 2001: IFK Lidingö FK 1st and Gustavsbergs IF 2nd (`2001_Division_3_(Swedish_football)`). The data names are IFK Lidingö and Gustavsbergs IF.

## Names

The rows use the full name of `clubs/Sweden.tsv`, with the 3.9.68 spelling where the club has two names:

- Ljungskile SK is written as Panos Ljungskile SK (the 3.9.68 name). The later databases call it Ljungskile SK.
- Vasalund/Essinge IF (2004, 2005) and Vasalunds IF are written as Vasalunds IF. The later databases call it Vasalund Essinge.
- Malmö Anadolu BI is written as Malmö Anadolu Boll IF. The later databases call it Malmö Anadolu BI.
- Syrianska FC is written as Syrianska Föreningen. The 2001 Division 2 page links Syrianska Föreningen to the Syrianska FC article.
- Bunkeflo (2005, 2006) is written as Bunkeflo IF. The page links it to IF Limhamn Bunkeflo, which is the merged club formed in 2008.
- Some names are in the later databases only: Hammarby TFF (page name Hammarby Talang FF), Gröndals IK, Väsby United (page name FC Väsby United) and Malmö Anadolu BI.
- Other name changes: Lindome GIF = Lindome IF, Qviding FIF = Qviding Fräntorps IF, Kalmar AIK FK = Kalmar AIK, Motala AIF FK = Motala AIF, Skellefteå AIK FK = Skellefteå AIK, Varbergs GIF FF = Varbergs GIF, Örebro SK Ungdom = Örebro Ungdom, IK Frej = IK Frej Täby, Värmbols FC = Värmbol FC.

These clubs are in no database. The rows use the page name and say `not in data`:

- Harmångers IF (Division 3 2002 and 2003)
- Kubikenborgs IF (Division 3 2004 and 2005)
- LSW IF (Division 3 2002)
- Ersboda SK (Division 3 2005)
- Dalkurd FF (Division 2 2009)
