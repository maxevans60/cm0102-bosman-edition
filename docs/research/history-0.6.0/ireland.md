# Republic of Ireland: competition history 2002 to 2009

Part `ireland`, slice 11c. Rows are in `ireland.tsv`: 134 rows, 42 held, 92 not held, 0 unknown. All pages were read on 17 October 2026.

## Label conventions

### Premier Division, First Division, FAI Cup (Senior Challenge Cup), Munster Senior Cup

The label year is the year where the season ends. Proof from the 3.9.68 rows:

- Premier Division 1995 Dundalk, 1996 Saint Patrick's Athletic, 2000 Shelbourne, 2001 Bohemians, 2002 Shelbourne. These are the champions of 1994/95, 1995/96, 1999/00, 2000/01 and 2001/02 (RSSSF `ierchamp.html`).
- First Division 2002 Drogheda United and Finn Harps is the 2001/02 table (RSSSF `ier02.html`).
- Senior Challenge Cup 1998 Cork City and Shelbourne is the final of May 1998. 2002 Dundalk and Bohemians is the final of 7 April 2002 (2001/02).
- Munster Senior Cup 2001 Cork City and College Corinthians is the 2000/01 final. 2002 Cork City and Rockmount is the 2001/02 final.

The League of Ireland changed to calendar seasons in 2003. A short transition season ran from July 2002 to January 2003 (RSSSF `ier03.html`). From 2003, label Y is the calendar season Y.

### Point of doubt: two seasons end in 2003

The 2002/03 transition season and the 2003 season both end in 2003. I wrote two rows for label 2003, with `part` set to `2002/03` and `2003`, for these competitions:

- Premier Division: 2002/03 Bohemians and Shelbourne, 2003 Shelbourne and Bohemians.
- First Division: 2002/03 Waterford United and Finn Harps, 2003 Dublin City and Bray Wanderers.
- FAI Cup: the 2002/03 cup (final 27 October 2002, Derry City 1-0 Shamrock Rovers) and the 2003 cup (Longford Town 2-0 Saint Patrick's Athletic).

If the game can hold only one row per label year, the caller must choose one of each pair. The Munster Senior Cup did not change: it stays on winter seasons (2002/03 to 2007/08), so it has one row per label year.

### League Cup

The label year is the year where the season starts. Proof from the 3.9.68 rows against RSSSF `ier98.html` to `ier02.html`:

- 1997 Sligo Rovers and Shelbourne is the 1997/98 final (February 1998).
- 1998 Cork City and Shamrock Rovers is the 1998/99 final (December 1998).
- 1999 Derry City and Athlone Town is the 1999/00 final (December 1999).
- 2000 Saint Patrick's Athletic and UCD is the 2000/01 final (January and February 2001).
- 2001 Limerick and Derry City is the 2001/02 final (April 2002).

So label 2002 is the 2002/03 League Cup, which was not held because of the change of seasons (Wikipedia, RSSSF list skips it). Labels 2003 to 2009 are the calendar-year cups.

### LFA President's Cup (`Irish Presidents Cup`)

The competition in the data is the Leinster FA President's Cup (the 3.9.68 rows hold Dublin clubs). The 2014 President of Ireland's Cup is a different FAI competition. The label year is the year where the season ends. The 3.9.68 rows 1996 Shelbourne, 1997 Saint Patrick's Athletic, 1999 Shelbourne and Saint Patrick's Athletic, and 2001 Bohemians and Shelbourne match the 1995/96, 1996/97, 1998/99 and 2000/01 editions.

Doubts:

- The 3.9.68 row 2000 holds Bohemians and Shelbourne, but the sources say there was no 1999/00 competition. The 3.9.68 row 1998 holds Saint Patrick's Athletic as runner-up, but the sources give Shelbourne.
- The cup was a pre-season match, so the 2001/02 edition was played in 2001. If the 3.9.68 row 2001 is in fact the 2001/02 edition (start-year label), my label 2002 row repeats it. I don't know which reading the original data used. I followed the season-end reading, which matches more of the 3.9.68 rows.
- The last edition was 2002/03, Shelbourne 2-0 Dundalk, 6 September 2002. Labels 2004 to 2008 are not held.

### FAI Super Cup

Calendar year. The 3.9.68 row 1998 Shamrock Rovers and Saint Patrick's Athletic is the July 1998 tournament. 1999 Saint Patrick's Athletic and Shelbourne is the final of 4 July 1999 (RSSSF `ier99.html`). The competition ran from 1998 to 2001 only, so labels 2002 to 2008 are not held.

## Defunct competitions (all labels not held)

| Competition | Last held | Source |
|---|---|---|
| Blaxnit Cup | 1973/74 | RSSSF `alliercuphist.html`, Wikipedia |
| Dublin City Cup | 1983/84 | RSSSF `ierdublincityhist.html`, Wikipedia |
| Dublin and Belfast Inter-City Cup | 1948/49 | RSSSF `alliercuphist.html`, Wikipedia |
| First Division Shield | 1997/98 | RSSSF `ier1divshieldhist.html`, Wikipedia |
| North-South Cup | 1962/63 | RSSSF `alliercuphist.html`, Wikipedia |
| P.J. Casey Cup | 1962/63, played once | RSSSF `ierleagcuphist.html` |
| League of Ireland Shield | 1983/84 (one revival after 1972/73) | RSSSF `iershieldhist.html`, Wikipedia |
| Irish Texaco Cup | 1974/75 | RSSSF `alliercuphist.html`, Wikipedia |
| Top Four Cup | 1973/74 | RSSSF `iertop4hist.html`, Wikipedia |
| Tyler Cup | 1980/81 | RSSSF `alliercuphist.html`, Wikipedia |
| Leinster Senior Cup | 1999/00 final; 2000/01 abandoned; revived 2010 | Wikipedia |

Possible successors that I did not map (the caller can decide):

- The Setanta Cup (2005 on) is a cross-border cup with League of Ireland and Irish League clubs, like the Blaxnit, North-South, Texaco, Tyler and Inter-City cups. It is a different competition, so I did not map any of them to it. RSSSF `alliercuphist.html` has its finals: 2005 Linfield, 2006 Drogheda United, 2007 Drogheda United, 2008 Cork City.
- A one-off First Division Cup was played as a pre-season competition in July to September 2002 (2002/03 season). Finn Harps beat Kildare County 2-0 and 2-0 (RSSSF `ier03.html`). It is at the same level as the First Division Shield, but it is a new competition four years later. I marked the Shield `not held`. If the caller maps it, it would be label 2003, Finn Harps and Kildare County F.C.

## Database rows that disagree with the result

Rows inside the label range only. Rows that agree: 3.9.68, November 2007, February 2008 and December 2008 at label 2002 for the Premier Division, First Division, Senior Challenge Cup and Munster Senior Cup (where they hold these).

| Database | Competition | Label | Holds | Real |
|---|---|---|---|---|
| October 2003 | Premier Division | 2003 | Shelbourne, Shamrock Rovers (2001/02) | 2002/03 Bohemians, Shelbourne; 2003 Shelbourne, Bohemians |
| September 2005 | Premier Division | 2005 | Shelbourne, "Cork" (2004) | Cork City, Derry City |
| September 2008, March 2009 | Premier Division | 2008 | Drogheda United, Saint Patrick's Athletic, third Bohemians (2007) | Bohemians, Saint Patrick's Athletic |
| April 2010 SIM | Premier Division | 2009 | Drogheda United, Saint Patrick's Athletic, Bohemians (2007) | Bohemians, Shamrock Rovers |
| September 2009, February 2010 ODB, June 2010 | Premier Division | 2009 | Bohemians, Derry City, third Shamrock Rovers | Bohemians, Shamrock Rovers (2008 was Bohemians, Saint Patrick's Athletic, Derry City; the row matches neither) |
| October 2003 | First Division | 2003 | Drogheda United, Finn Harps (2001/02) | 2002/03 Waterford United, Finn Harps; 2003 Dublin City, Bray Wanderers |
| September 2005 | First Division | 2005 | Drogheda United, Finn Harps | Sligo Rovers, Dublin City |
| September 2008, March 2009 | First Division | 2008 | Cobh Ramblers, Finn Harps, third Dundalk (2007) | Dundalk, Shelbourne |
| April 2010 SIM | First Division | 2009 | Cobh Ramblers, Finn Harps, Dundalk (2007) | University College Dublin, Shelbourne |
| September 2009, February 2010 ODB, June 2010 | First Division | 2009 | Shelbourne, University College Dublin, third Waterford United | University College Dublin, Shelbourne (first and second swapped; Waterford United was third in 2008, Sporting Fingal in 2009) |
| September 2008, March 2009 | League Cup | 2008 | Saint Patrick's Athletic, University College Dublin (2000/01 final) | Derry City, Wexford Youths F.C. |
| October 2003 | Senior Challenge Cup | 2003 | Dundalk, Bohemians (2001/02) | 2002/03 Derry City, Shamrock Rovers; 2003 Longford Town, Saint Patrick's Athletic |
| September 2005 | Senior Challenge Cup | 2005 | Dundalk, Bohemians (2001/02) | Drogheda United, Cork City |
| October 2003 | Munster Senior Cup | 2003 | Cork City, Rockmount (2001/02) | Cork City, Waterford United |
| September 2005 | Munster Senior Cup | 2005 | Cork City, Rockmount (2001/02) | Cork City, Avondale United |

Several newer databases also fill the third place (and February 2010 ODB a host value `T`) in club league rows. The club record uses first and second only.

## Names not matched

- Carrick United (Munster Senior Cup 2003/04 runner-up): not in the data.
- Passage (Munster Senior Cup 2006/07 runner-up, Passage West, Cork): not in the data.
- The September 2005 Premier Division row holds the name "Cork". I read it as Cork City.
- Bray Wanderers: I used the 3.9.68 spelling `Bray Wanderers A.F.C.`. The databases from September 2008 spell it `Bray Wanderers`.
- Wexford Youths F.C. and Sporting Fingal exist only in databases from September 2008.
