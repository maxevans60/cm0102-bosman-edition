# History research: Australia and South Africa (aus-rsa)

Part `aus-rsa`, slice 11c. The rows are in `aus-rsa.tsv` (66 rows, 15 competitions). All pages were read on 2026-10-17 with `curl`.

## Label conventions with proof

### Australian National Soccer League: start year of the season

The 3.9.68 rows give the grand final winner and loser, and the label is the year in which the season starts.

| 3.9.68 label | 3.9.68 row | Real grand final |
|---|---|---|
| 1997 | South Melbourne, Carlton SC | 1997/98: South Melbourne 2-1 Carlton |
| 1999 | Wollongong Wolves, Perth Glory | 1999/2000: Wollongong 3-3 (7-6 pens) Perth Glory |
| 2000 | Wollongong Wolves, South Melbourne | 2000/01: Wollongong 2-1 South Melbourne |
| 2001 | Sydney Olympic Sharks, Perth Glory | 2001/02: Sydney Olympic 1-0 Perth Glory |

So label 2002 = 2002/03 and label 2009 = 2009/10.

What the competition maps to:

- Labels 2002 and 2003 are the last two NSL seasons (2002/03 and 2003/04).
- Label 2004 is `not held`. The NSL folded after the grand final of 4 April 2004, and the A-League began on 26 August 2005. No national league ran in 2004/05.
- Labels 2005 to 2009 map to the A-League (2005/06 to 2009/10). It is the same tier, and it replaced the NSL. The rows hold the grand final winner and loser. They do not hold the premiers.

### New South Wales Super League: top state tier, start year for summer seasons

The game's "New South Wales Super League" is the top NSW tier. It was called the NSW Super League up to 2000 and the NSW Premier League from 2000/01 to 2012. The 3.9.68 rows hold first place only:

- 1996 Parramatta Eagles, the grand final winner.
- 1998 Bonnyrigg White Eagles, the premiers (Blacktown won the grand final).
- 1999 Blacktown City Demons, the grand final winner (Bonnyrigg were premiers).
- 2000 has two rows, Blacktown City Demons and Bonnyrigg White Eagles. Blacktown won the 2000 winter season, and Bonnyrigg won the 2000/01 grand final.
- 2001 Parramatta Eagles, who won the 2001/02 season (premiers and grand final).

Label 2001 cannot be 2000/01, because Bonnyrigg won that grand final and Blacktown were premiers. So the label for a summer season is its start year, the same as the NSL in the same data. This is an inference. I use the grand final winner and loser, because two of the three rows that differ use the grand final winner, and because the NSW State League rows prove that convention (see below).

Labels:

- 2002, 2003 and 2004 = the summer seasons 2002/03, 2003/04 and 2004/05. These seasons ended in May and June.
- 2005 is `not held`. The league moved to a winter season from 2006, so no season started in 2005.
- 2006 to 2009 are calendar-year winter seasons.

### New South Wales State League: second state tier, calendar year

The 3.9.68 row for 2001 is Manly-Warringah Dolphins and Bankstown City. The 2001 NSW Winter Super League (second tier) grand final was Manly Warringah Dolphins 1-0 Bankstown City, and Bankstown were the premiers. So the rows hold the grand final winner and loser of the calendar year. The second tier was called the Winter Super League up to 2005 and the NSW Super League from 2006.

### Other Australian state leagues: calendar year

The 3.9.68 rows hold first place only. The winner type differs by state:

- Victorian Premier League: the grand final winner (champions). 1998 Bulleen Zebras won the grand final and St Albans were premiers. 2001 Heidelberg United won the grand final and Port Melbourne were premiers. Both 3.9.68 rows hold the grand final winner.
- South Australian Premier League: the grand final winner. 1996 Adelaide Blue Eagles won the grand final and White City Woodville were premiers. 1999 Cumberland United won the grand final and Port Adelaide Lion were premiers.
- Western Australian Premier League: the premiers. 1996 Inglewood United SC, 1999 ECU Joondalup Saints, 2000 Fremantle City SC and 2001 Sorrento SC were premiers. The cup or finals winners were Stirling Macedonia, Sorrento, Perth SC and Perth SC.
- Queensland Premier League: mostly the premiers. The data's competition maps to the Brisbane Premier League, the top Queensland league in these years. The 1998, 1999 and 2001 rows hold the premiers (Brisbane City, Rochedale Rovers, Wynnum District). The 1995 row holds the grand final winner (Rochedale Rovers). I follow the majority. This only matters for 2005, where Palm Beach were premiers and Pine Rivers United won the grand final.

For Queensland and Western Australia I give first place only, as the 3.9.68 rows do. A premiers row needs the league runner-up, and I found no page for it.

### South Africa

- South African Premier Division: the year in which the season ends. The 3.9.68 rows are 1999 Sundowns, Kaizer Chiefs (1998/99), 2000 Sundowns, Orlando Pirates (1999/2000), 2001 Orlando Pirates, Kaizer Chiefs (2000/01) and 2002 Santos, Supersport United (2001/02).
- First Division Coastal and Inland: the end year. The 3.9.68 rows are 2001 AmaZulu (Coastal) and 2001 Black Leopards (Inland), the 2000/01 stream winners. The 2002 rows are African Wanderers and Dynamos, the 2001/02 stream winners.
- Cups: the calendar year of the final.
  - League Cup 2001 in 3.9.68 is Kaizer Chiefs, Jomo Cosmos. That is the Coca-Cola Cup final of 24 November 2001. The 2000 final (December 2000) was Ajax Cape Town against Orlando Pirates.
  - Top 8 Cup 2001 is Kaizer Chiefs, Sundowns. That is the BP Top Eight final of 1 September 2001.
  - Superbowl 2001 is Santos, Sundowns. That is the Bob Save Superbowl final of 9 June 2001.
- Charity Cup: inferred to be the calendar year, like the other cups. The 3.9.68 row for 2001 (Kaizer Chiefs, Orlando Pirates) does not match the 2001 final on RSSSF (Orlando Pirates 1-0 Moroka Swallows, 28 July 2001). So this convention is not proved.

What the cups map to:

- Superbowl: the national cup. It was the Bob Save Superbowl up to 2001, was not played in 2002, and was the ABSA Cup from 2003.
- Top 8 Cup: the BP Top Eight up to 2002, and the SAA Supa 8 from 2003.
- League Cup: the Coca-Cola Cup.
- Charity Cup: the Telkom Charity Cup.

## The `ended` column

- For leagues decided by a grand final, `ended` is the month of the grand final. This also applies to the Western Australian and Queensland rows that hold the premiers, because the finals series is part of the same edition.
- For the South African Premier Division, it is the last match day on RSSSF.
- Eight rows have `unknown`. The pages give no date for these:
  - Charity Cup 2002.
  - South Australian Premier League 2002.
  - Coastal and Inland 2002 to 2004.

## Points of doubt

- **South African First Division (2002, 2003): `not held`.** No single first division existed before 2004/05. Each season had a Coastal and an Inland stream, and both winners went up, with no combined final. The 3.9.68 row for 2002 (Dynamos, African Wanderers) puts the two stream winners side by side. If the game wants the same pairing for 2003, it is Tycoon Silver Stars (Inland) and AmaZulu (Coastal). That would be a construction, not a real result.
- **Coastal and Inland 2005: `not held`.** The streams merged into one National First Division (Mvela Golden League) for 2004/05. Free State Stars won it.
- **NSW Super League 2005: `not held`.** This is an inference from the start-year convention. If the label were the end year instead, every NSW Super League row would move by one year.
- **Queensland 2005.** I give the premiers (Palm Beach). The grand final winner was Pine Rivers United.
- **Charity Cup convention.** It is not proved (see above).
- **NSW State League 2007.** Bonnyrigg White Eagles appear as second-tier grand final winners in 2007 on the source page. I kept the page's result.

## Database rows that disagree with the result

| Database | Competition | Label | Database holds | Real result |
|---|---|---|---|---|
| September 2005 | Australian National Soccer League | 2005 | Perth Glory FC, Parramatta Power | Sydney FC, Central Coast Mariners (2005/06 A-League). The row is the 2003/04 NSL. |
| November 2007, February 2008 | Australian National Soccer League | 2007 | Adelaide United, Central Coast Mariners | Newcastle United Jets, Central Coast Mariners (2007/08) |
| December 2008 | Australian National Soccer League | 2008 | Adelaide United, Central Coast Mariners | Melbourne Victory, Adelaide United (2008/09) |
| September 2009, February 2010 ODB, April 2010 SIM, June 2010 | New South Wales State League | 2009 | Manly United FC, Bankstown City | GHFA Spirit FC, PCYC Parramatta (2009). The row is the 2001 result moved by the shift. |
| 3.9.68, October 2003, September 2005 | South African First Division Coastal | 2002 | African Wanderers, Park United | African Wanderers, Avendale Athletico. Park United were 4th. |
| September 2005 | South African First Division Coastal | 2005 | Zulu Royals, Park United | Not held (streams merged in 2004/05) |
| 3.9.68, October 2003, September 2005 | South African First Division Inland | 2002 | Dynamos, no second | Dynamos, Tycoon Silver Stars (second missing) |
| 3.9.68, October 2003 | South African First Division | 2002 | Dynamos, African Wanderers | Not held as a single division (see above) |
| 3.9.68, October 2003 | South African Premier Division | 2002 | Santos Futebol Clube, Supersport United | Santos (RSA), Supersport United. "Santos Futebol Clube" is not in the South African club list. It may point at the wrong club. |
| September 2005 | South African Premier Division | 2005 | Kaizer Chiefs, Ajax Cape Town | Kaizer Chiefs, Orlando Pirates (2004/05). The row is 2003/04. |

These 3.9.68 rows fall outside the research range, but they also disagree with RSSSF:

- First Division Inland 2001 has second Dobsonville All Nations. RSSSF has Bloemfontein Young Tigers as runner-up.
- Charity Cup 2001 (see above).

## Names that I could not match, or matched by inference

- Not in the data:
  - Sydney Crescent Star (NSW State League 2003).
  - PCYC Parramatta (NSW State League 2008, 2009).
  - Palm Beach Sharks (Queensland 2005). The data's Gold Coast City may be the same club, but this is not confirmed.
- Matched to a club with a different real name:
  - APIA Leichhardt Tigers = Leichhardt Tigers.
  - Bankstown City Lions = Bankstown City.
  - Belconnen Blue Devils = Belconnen United Blue Devils.
  - Queensland Lions = Brisbane Lions (the club changed its name in 1997).
  - MetroStars = North-Eastern MetroStars.
  - Adelaide Olympic = Adelaide Olympic FC Flames.
  - Adelaide City (2005) = Adelaide City Zebras, the only Adelaide City in the September 2005 data.
  - Stanmore Hawks = Stanmore City Hawks.
  - Fraser Park Dragons = Fraser Park.
  - Hajduk Wanderers United = Hajduk Wanderers.
  - Spirit FC = GHFA Spirit FC.
  - Silver Stars = Tycoon Silver Stars.
  - Mamelodi Sundowns = Sundowns (3.9.68 spelling).
  - Cape Town Santos = Santos (RSA).
  - Newcastle Jets = Newcastle United Jets.
- Inferences:
  - Wollongong FC (2008) = Wollongong Wolves. The 3.9.68 spelling is used. The databases from September 2009 hold Wollongong Community FC.
  - FC Bossy Liverpool = Liverpool Bossy. The 3.9.68 spelling is used.
  - Canterbury Marrickville Olympic (2005) = Canterbury-Marrickville Berries.
  - Fawkner (2002 Victorian runner-up, called Manningham United on the Victorian Wikipedia list) = Fawkner Blues. The 2002 fixtures page names Fawkner Blues.
