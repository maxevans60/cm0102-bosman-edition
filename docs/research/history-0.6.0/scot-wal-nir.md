# History research: Scotland, Wales, Northern Ireland

Part `scot-wal-nir`, slice 11c. 17 competitions, label years 2002 to 2009, 136 rows in `scot-wal-nir.tsv`. All sources were read on 2026-10-17 as raw pages with curl.

- `yes`: 127 rows.
- `not held`: 9 rows (Northern Irish Gold Cup 2002 to 2009, Welsh Premier Cup 2009).
- `unknown`: 0 rows.

All competitions are club competitions, so `third` and `host` are empty.

## Sources

- Scottish leagues: RSSSF season pages `tabless/scotYY.html` (YY = the year the season ends).
- Scottish Cup: RSSSF `tabless/scotcuphist.html`. Scottish League Cup: RSSSF `tabless/scotleagcuphist.html`. Challenge Cup: English Wikipedia, Scottish Challenge Cup.
- Welsh league and FAW Premier Cup 2008/09: RSSSF `tablesw/walYY.html`. Welsh Cup: RSSSF `tablesw/walcuphist.html`. Welsh League Cup: RSSSF `tablesw/walleagcuphist.html`. FAW Premier Cup finals: English Wikipedia, FAW Premier Cup.
- Northern Irish leagues: RSSSF `tablesn/nilYY.html`. Irish Cup, League Cup, County Antrim Shield, Gold Cup: English Wikipedia lists of finals. The RSSSF season pages show the same cup finals and agree with Wikipedia.

## Label conventions

Season-end year (label Y = season Y-1/Y). This applies to 14 of the 17 competitions. Proof from the 3.9.68 rows:

- Scottish Premier Division 2001 Glasgow Celtic, Glasgow Rangers = 2000/01. 1995 Glasgow Rangers, Motherwell = 1994/95.
- Scottish First Division 2001 Livingston, Ayr United = 2000/01. Second Division 2001 Partick Thistle, Arbroath = 2000/01. Third Division 2000 Queen's Park, Berwick Rangers = 1999/2000.
- Scottish Cup 2001 Glasgow Celtic, Hibernian = 2000/01 final (RSSSF lists it as 2001).
- Scottish League Cup 1995 Raith Rovers, Glasgow Celtic = 1994/95. 1996 Aberdeen, Dundee = 1995-96 on RSSSF. 2001 Glasgow Celtic, Kilmarnock = 2000-01.
- Welsh Premier Division 2001 Barry Town, Cwmbran Town = 2000/01. Welsh Cup 2001 Barry Town, Total Network Solutions = 2000/01. Welsh League Cup 2001 Caersws, Barry Town = 2000/01 on RSSSF.
- Welsh Premier Cup 1998 Wrexham, Cardiff City = 1997/98, 1999 Barry Town, Wrexham = 1998/99, 2000 Wrexham, Cardiff City = 1999/2000 (Wikipedia).
- Northern Irish Premier Division 2002 Portadown, Glentoran = 2001/02. First Division 2002 Lisburn Distillery, Institute = 2001/02.
- Northern Irish Cup 2002 Linfield, Portadown = final of 11 May 2002. 1999 Portadown, Cliftonville = 1998/99, when Portadown were awarded the cup.
- Northern Irish League Cup 1995 Ards = 1994/95, 2002 Linfield = 2001/02 (final of 27 November 2001). This cup has its final early in some seasons, but the label still follows the season end.

Season-start year (label Y = season Y/Y+1):

- Scottish League Challenge Cup. 3.9.68 holds 1995 Stenhousemuir, Dundee United (1995-96), 1996 Stranraer (1996-97), 1997 Falkirk (1997-98), no 1998 (1998-99 not held, no sponsor), 1999 Alloa Athletic, Inverness Caledonian Thistle (1999-2000), 2000 Airdrieonians, Livingston (2000-01). The final takes place in October or November, so the label is the calendar year of the final. Wikipedia names each final by that year, for example "1995 Scottish Challenge Cup final". The 2001/02 final (14 October 2001, Airdrieonians 2-1 Alloa Athletic, RSSSF scot02) is label 2001. It is outside the range and is missing from the data.
- Northern Irish Gold Cup. 3.9.68 holds 1995 Crusaders, Linfield (1995-96), 1996 Linfield, Glenavon (1996-97), 1997 Glenavon, Coleraine (1997-98), 1998 Glentoran, Portadown (1998-99), 1999 Glentoran, Linfield (1999-2000), 2000 Glentoran, Coleraine (2000-01). The competition ended after 2000-01, so every label from 2002 is `not held`. Label 2001 (2001/02) is also not held, but it is outside the range.

Inference: Northern Irish County Antrim Shield. 3.9.68 has no rows for it, and `db-rows.tsv` has no row before 2008. I use the season-end year, as for the other Northern Irish cups. The finals are in January to March (27 February 2002, 2 March 2004, 23 January 2007 on RSSSF), so the season-end year is also the calendar year of the final.

## Tier mapping and names

- Scottish Premier Division = Scottish Premier League. The final table after the split gives first and second.
- Scottish First, Second and Third Division = Scottish Football League tiers 2, 3 and 4.
- Northern Irish League Premier Division = top tier: Irish League Premier Division (2001/02, 2002/03), Irish Premier League (2003/04 to 2007/08), IFA Premiership (2008/09).
- Northern Irish League First Division = tier 2: First Division (2001/02 to 2003/04), "Intermediate League First Division" on RSSSF (2004/05, 2005/06), First Division (2006/07, 2007/08), IFA Championship (2008/09). In 2007/08 the champion Loughgall was not promoted. The runner-up Dundela was relegated for the new structure.
- Welsh Premier Division = League of Wales (to 2001/02), then Welsh Premier League. Welsh League Cup = League of Wales Cup, later Loosemores League Cup. Welsh Premier Cup = FAW Premier Cup.
- Welsh Premier Cup 2009: not held. RSSSF wal09 says the FAW Premier Cup 2008/09 was "discontinued due to BBC Wales withdrawing sponsorship". The last final was 2007/08.

## Club names

- I use the 3.9.68 full name for a club that has a different full name in later databases: Heart Of Midlothian, Queen Of The South, Inverness Caledonian Thistle, Gretna (later Gretna FC, Gretna FC 2008), Total Network Solutions (later The New Saints).
- Total Network Solutions: RSSSF lists the club as TNS Llansantffraid to 2005/06. In 2006/07 RSSSF says it was renamed The New Saints. The `notes` column says so on each such row.
- Airdrieonians (Scottish First Division 2002, second place) is in the no-nation club list. The club went into liquidation after 2001/02.
- Newry Town: the club was Newry City from summer 2004 (RSSSF nil05). Northern Irish League Cup 2009 uses the data name Newry Town.
- Cardiff City, Swansea City, Wrexham and Newport County are in the England club list.
- All names matched a full name in the club files. No row needed `not in data`.

## Points of doubt

- Scottish League Challenge Cup 2009 is the 2009/10 final (Dundee 3-2 Inverness Caledonian Thistle, November 2009). A game in a database with start year 2009 begins before that final. The brief says to research such a year all the same. The databases hold the 2008/09 final under label 2009 (see below). Decide in the build whether label 2009 is written for this cup.
- Northern Irish Gold Cup: all eight rows are `not held`. The databases hold no Gold Cup row in the range.
- The rows in `February 2010 ODB` have the text `T` in third and host. This looks like filler, not a club.

## Database rows that disagree

90 database rows in the range agree with the research. 49 disagree:

| Database | Competition | Label | Database holds | Real |
|---|---|---|---|---|
| October 2003 | Northern Irish Cup | 2003 | Linfield, Portadown (2001/02) | Coleraine, Glentoran |
| October 2003 | Northern Irish League Cup | 2003 | Linfield (2001/02) | Glentoran, Linfield |
| October 2003 | Northern Irish League First Division | 2003 | Lisburn Distillery, Institute (2001/02) | Dungannon Swifts, Ballymena United |
| October 2003 | Northern Irish League Premier Division | 2003 | Portadown, Glentoran (2001/02) | Glentoran, Portadown |
| October 2003 | Scottish First Division | 2003 | Falkirk, Ayr United | Falkirk, Clyde |
| September 2005 | Northern Irish Cup | 2005 | Linfield, Portadown | Portadown, Larne |
| September 2005 | Northern Irish League Cup | 2005 | Linfield | Glentoran, Linfield |
| September 2005 | Northern Irish League First Division | 2005 | Loughgall, Armagh City (2003/04) | Armagh City, Glenavon |
| September 2005 | Northern Irish League Premier Division | 2005 | Linfield, Portadown (2003/04) | Glentoran, Linfield |
| September 2005 | Scottish First Division | 2005 | Inverness Caledonian Thistle, Clyde (2003/04) | Falkirk, St. Mirren |
| September 2005 | Scottish League Cup | 2005 | Glasgow Rangers, Glasgow Celtic (2002/03) | Glasgow Rangers, Motherwell |
| September 2005 | Scottish Second Division | 2005 | Airdrie United, Hamilton Academical (2003/04) | Brechin City, Stranraer |
| September 2005 | Scottish Third Division | 2005 | Stranraer, Stirling Albion (2003/04) | Gretna, Peterhead |
| September 2005 | Welsh Premier Division | 2005 | Rhyl, Total Network Solutions (2003/04) | Total Network Solutions, Rhyl |
| November 2007, February 2008 | Scottish Cup | 2007 | Heart Of Midlothian, Gretna FC (2005/06) | Glasgow Celtic, Dunfermline Athletic |
| December 2008 | Scottish Cup | 2008 | Hibernian, Gretna FC (no such final) | Glasgow Rangers, Queen Of The South |
| December 2008, March 2009, September 2008 | Scottish Second Division | 2008 | Greenock Morton, Stirling Albion (2006/07) | Ross County, Airdrie United |
| December 2008, March 2009, September 2008 | Scottish Third Division | 2008 | Berwick Rangers, Arbroath (2006/07) | East Fife, Stranraer |
| December 2008, March 2009, September 2008 | Welsh Premier Cup | 2008 | The New Saints, Newport County (2006/07) | Newport County, Llanelli AFC |
| March 2009, September 2008 | Northern Irish County Antrim Shield | 2008 | Glentoran, Ballymena United (2002/03) | Glentoran, Crusaders |
| September 2009, February 2010 ODB, June 2010 | Northern Irish County Antrim Shield | 2009 | Cliftonville, Lisburn Rangers | Cliftonville, Linfield |
| September 2009, February 2010 ODB, June 2010, April 2010 SIM | Scottish League Challenge Cup | 2009 | Airdrie United, Ross County (final of November 2008, label 2008 by the 3.9.68 convention) | Dundee, Inverness Caledonian Thistle |
| September 2009, February 2010 ODB, June 2010, April 2010 SIM | Welsh Premier Cup | 2009 | The New Saints, Newport County | not held |
| February 2010 ODB, June 2010 | Scottish Second Division | 2006 | Gretna FC 2008, Greenock | Gretna, Greenock Morton |
| February 2010 ODB, June 2010 | Scottish Second Division | 2007 | Greenock, Stirling Albion | Greenock Morton, Stirling Albion |
| April 2010 SIM | Northern Irish County Antrim Shield | 2009 | Glentoran, Ballymena United | Cliftonville, Linfield |
| April 2010 SIM | Northern Irish Cup | 2009 | Linfield, Coleraine (2007/08) | Crusaders, Cliftonville |
| April 2010 SIM | Northern Irish League Cup | 2009 | Linfield, Crusaders (2007/08) | Portadown, Newry Town |
| April 2010 SIM | Northern Irish League Premier Division | 2009 | Linfield, Glentoran (2007/08) | Glentoran, Linfield |
| April 2010 SIM | Scottish Cup | 2009 | Glasgow Rangers, Queen of the South (2007/08) | Glasgow Rangers, Falkirk |
| April 2010 SIM | Scottish League Cup | 2009 | Glasgow Rangers, Dundee United (2007/08) | Glasgow Celtic, Glasgow Rangers |

The season in brackets is my identification of the real result that the database row matches. Many rows hold the result of one or two seasons earlier.

Notes on the table:

- `Greenock` in February 2010 ODB and June 2010 is a separate club in the Scotland list. It is not Greenock Morton, so these rows name the wrong club.
- `Lisburn Rangers` in the 2009 County Antrim Shield rows is a separate club. The 2006/07 final was Cliftonville against Lisburn Distillery. Inference: the row means that final with the wrong second club.
- The "Hibernian, Gretna FC" Scottish Cup row in December 2008 matches no final. Between 2001 and 2010, Gretna played only one final (2006, against Heart Of Midlothian), and Hibernian played only one (2001, against Glasgow Celtic).

## Names not matched

None.
