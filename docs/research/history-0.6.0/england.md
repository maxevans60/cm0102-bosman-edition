# England: competition history 2002 to 2009

Part `england`, 14 club competitions, label years 2002 to 2009. The file `england.tsv` has 108 rows: 102 `yes`, 6 `not held`, 0 `unknown`. All pages were read on 2026-10-17 as raw HTML with curl. Every name in `first` and `second` is a full name in `clubs/England.tsv`.

## Label conventions, with the 3.9.68 rows that prove them

Season-end year (label Y = season Y-1/Y). This applies to the Premier Division, First, Second and Third Divisions, Conference, Northern Premier, Southern and Isthmian Premier Divisions, FA Cup, League Cup, FA Trophy and Vans Trophy.

- English Premier Division 2002 Arsenal, Liverpool = 2001/02.
- English First Division 2002 Manchester City, West Bromwich Albion = 2001/02.
- English Conference 2002 Boston United, Dagenham and Redbridge = 2001/02.
- English FA Cup 2002 Arsenal, Chelsea = 2001/02 final.
- English League Cup 2002 Blackburn Rovers, Tottenham Hotspur = 2001/02 final.
- English Vans Trophy 2002 Blackpool, Cambridge United = 2001/02 final.
- English FA Trophy 2002 Yeovil Town, Stevenage Borough = 2001/02 final.
- English Isthmian 2002 Gravesend and Northfleet, Canvey Island. Northern Premier 2002 Burton Albion, Vauxhall Motors. Southern 2002 Kettering Town, Tamworth. All three are 2001/02.

Calendar year of the match (August). This applies to the English Charity Shield. The 3.9.68 rows 2000 Chelsea, Manchester United and 2001 Liverpool, Manchester United are the August 2000 and August 2001 matches. Label 2009 is the August 2009 match, which is the start of the 2009/10 season.

Season-start year (label Y = season Y/Y+1). This applies to the English Conference Cup only. Proof from the 3.9.68 rows:

- 1998 and 1999 Doncaster Rovers. Doncaster dropped into the Conference in 1998, so they could win only in 1998/99 and 1999/2000.
- 1995 Bromsgrove Rovers, Macclesfield Town = 1995/96. 1997 Morecambe, Woking = 1997/98.
- The last label before 2002 is 2000, which matches the last edition before the break (2000/01).

With this convention the cup was held at labels 2004 (2004/05), 2007 (2007/08) and 2008 (2008/09). Labels 2002, 2003, 2005, 2006 and 2009 are `not held`.

## Points of doubt

1. Conference Cup convention. The February 2010 ODB and June 2010 databases label this cup by the end year: 2005 Woking, 2008 Aldershot Town, 2009 AFC Telford United. I follow the 3.9.68 convention, as the brief says. I think the 3.9.68 rows are an off-by-one mistake in the original data, but I cannot prove it. If the game must show this cup like the other cups, move each row up one year: 2005 Woking, 2008 Aldershot Town, 2009 AFC Telford United. Labels 2002, 2003, 2004, 2006 and 2007 are then `not held`.
2. Tier 6 split in 2004/05. Conference North and South became tier 6, and the Northern Premier, Southern and Isthmian Premier Divisions dropped to tier 7. From label 2005 I map Northern Premier to Conference North and Southern to Conference South. The February 2010 ODB and June 2010 rows do the same. Nothing is left for the Isthmian Premier at label 2005, so I mark it `not held`. This is an inference. If the September 2005 database uses its Isthmian Premier Division as the tier 7 league, use the real 2004/05 Isthmian result instead: Yeading first, Billericay Town second (source: `2004–05_Isthmian_League` on Wikipedia). I don't know which tier the September 2005 database means.
3. The Isthmian clubs moved mostly to Conference South, and some Southern clubs moved to Conference North. The mapping in point 2 follows the later databases, not geography.
4. I did not use the `third` field because club competitions use first and second only. Some later databases put a third club in league rows (for example the play-off winner). I ignore those.

## Database rows that disagree with the real result

| Database | Competition | Label | Holds | Real |
|---|---|---|---|---|
| September 2005 | English Charity Shield | 2005 | Manchester United, Arsenal | Chelsea, Arsenal |
| October 2003 | English Conference | 2003 | Yeovil Town, Dagenham and Redbridge | Yeovil Town, Morecambe |
| September 2005 | English Conference | 2005 | Chester City, Hereford United | Barnet, Hereford United |
| April 2010 SIM | English Conference | 2009 | Aldershot Town, Cambridge United | Burton Albion, Cambridge United |
| February 2010 ODB, June 2010 | English Conference Cup | 2009 | Chester City (Chester), Kingstonian | not held (Chester City, Kingstonian is 2000/01) |
| February 2010 ODB, June 2010 | English Conference Cup | 2005 | Woking, Stalybridge Celtic | not held at label 2005 (Woking is label 2004, see doubt 1) |
| February 2010 ODB, June 2010 | English Conference Cup | 2008 | Aldershot Town, Rushden & Diamonds | AFC Telford United, Forest Green Rovers (see doubt 1) |
| February 2010 ODB, June 2010 | English Conference Cup | 2009 | AFC Telford United, Forest Green Rovers | not held (see doubt 1) |
| April 2010 SIM | English FA Cup | 2009 | Portsmouth, Cardiff City | Chelsea, Everton |
| April 2010 SIM | English FA Trophy | 2009 | Ebbsfleet United, Torquay United | Stevenage Borough, York City |
| December 2008 | English FA Trophy | 2008 | Ebbsfleet United, Oxford United | Ebbsfleet United, Torquay United |
| February 2008, November 2007 | English FA Trophy | 2007 | Canvey Island, Forest Green Rovers | Stevenage Borough, Kidderminster Harriers |
| October 2003 | English FA Trophy | 2003 | Yeovil Town, Stevenage Borough | Burscough, Tamworth |
| September 2005 | English FA Trophy | 2005 | Yeovil Town, Stevenage Borough | Grays Athletic, Hucknall Town |
| April 2010 SIM | English First Division | 2009 | West Bromwich Albion, Stoke City | Wolverhampton Wanderers, Birmingham City |
| September 2005 | English First Division | 2005 | Norwich City, West Bromwich Albion | Sunderland, Wigan Athletic |
| April 2010 SIM | English League Cup | 2009 | Tottenham Hotspur, Chelsea | Manchester United, Tottenham Hotspur |
| September 2005 | English League Cup | 2005 | Middlesbrough, Bolton Wanderers | Chelsea, Liverpool |
| February 2010 ODB, June 2010 | English Northern Premier League Premier Division | 2005 | Southport, Nuneaton Town | Southport, Nuneaton Borough (name of the club at that time) |
| February 2010 ODB, June 2010 | English Northern Premier League Premier Division | 2009 | Tamworth, Droylsden | Tamworth, Gateshead |
| April 2010 SIM | English Premier Division | 2009 | Manchester United, Chelsea | Manchester United, Liverpool |
| April 2010 SIM | English Second Division | 2009 | Swansea City, Nottingham Forest | Leicester City, Peterborough United |
| September 2005 | English Second Division | 2005 | Plymouth Argyle, Queens Park Rangers | Luton Town, Hull City |
| February 2010 ODB, June 2010 | English Southern League Premier Division | 2005 | Grays Athletic, Eastbourne Borough | Grays Athletic, Cambridge City |
| February 2010 ODB, June 2010 | English Southern League Premier Division | 2009 | AFC Wimbledon, Maidenhead United | AFC Wimbledon, Hampton & Richmond Borough |
| April 2010 SIM | English Third Division | 2009 | Milton Keynes Dons, Peterborough United | Brentford, Exeter City |
| September 2005 | English Third Division | 2005 | Doncaster Rovers, Hull City | Yeovil Town, Scunthorpe United |
| April 2010 SIM | English Vans Trophy | 2009 | Milton Keynes Dons, Grimsby Town | Luton Town, Scunthorpe United |
| February 2008, November 2007 | English Vans Trophy | 2007 | Port Vale, Brentford | Doncaster Rovers, Bristol Rovers |

A pattern shows in these rows. The April 2010 SIM rows at label 2009 hold the 2007/08 results. The September 2005 rows at label 2005 mostly hold the 2003/04 results. The newest row of each such database is one or two seasons old.

The rows of February 2010 ODB and June 2010 at label 2009 also put Chester City, Kingstonian (the 2000/01 Conference Cup) next to AFC Telford United. That is two rows at one label.

Short names in some databases are not disagreements. Examples: Stevenage and Chester in June 2010.

## Names

All names matched `clubs/England.tsv`. Notes:

- Barrow (Northern Premier 2003, second) is `Barrow AFC` in 3.9.68, October 2003 and September 2005, and `Barrow` in the 2007 to 2010 databases. The row uses the 3.9.68 spelling `Barrow AFC`.
- Nuneaton Borough is in 3.9.68 to February 2008. The later databases have Nuneaton Town only.
- Swansea City, Cardiff City and Wrexham are Welsh clubs in English competitions. They are in `clubs/England.tsv`.

## The `ended` column

I added `ended` (YYYY-MM of the last match) to every row after the first pass. The `sources` column was not changed, so the sources for `ended` are listed here.

- Finals (FA Cup, League Cup, Vans Trophy, FA Trophy, Charity Shield): the infobox date of each Wikipedia final article, for example `https://en.wikipedia.org/wiki/2002_FA_Cup_final`, `2002_Football_League_Cup_final`, `2002_Football_League_Trophy_final`, `2002_FA_Trophy_final` and `2002_FA_Community_Shield`. The 2005 FA Trophy final article redirects to `2004–05_FA_Trophy`, whose Final section gives 22 May 2005.
- Conference Cup: `2007–08_Conference_League_Cup` (final 3 April 2008) and `2008–09_Conference_League_Cup` (final 9 April 2009). The 2004/05 page (`2004–05_Conference_Cup`) gives no date, so that row is `unknown`.
- Premier League, Football League tiers 2 to 4 (all seasons), Conference 2005/06 to 2008/09: the latest match date in the full-season CSV files of football-data.co.uk (`https://football-data.co.uk/mmz4281/<season>/E0.csv`, `E1`, `E2`, `E3`, `EC`). Each file has the full number of matches (380 or 552, Conference 462 or 552).
- Conference 2002/03 and 2003/04: the season text on Wikipedia ("ended on 26 April 2003", "ended on 24 April 2004").
- Northern Premier League 2001/02 to 2003/04: the "updated to" date of the Premier Division results grid on the Wikipedia season page (3 May 2002, 26 April 2003, 24 April 2004).
- `unknown` (19 rows): Conference 2001/02 and 2004/05, Conference Cup 2004/05, Conference North 2004/05 to 2008/09 (Northern Premier rows 2005 to 2009), Southern Premier and Conference South (all 8 Southern rows), and Isthmian 2001/02 to 2003/04. I found no page with the date of the last match for these. Every league among them probably ended in late April or early May, but that is a guess and the rows do not use it.
