# History research: France, Belgium, Holland

Part `fra-bel-ned` of slice 11c. Research date 2026-10-17. The results are in `fra-bel-ned.tsv`: 179 rows for 23 competitions.

## Counts

- 179 rows. 163 rows are `held = yes`. 16 rows are `not held` (Belgian League Cup 2002 to 2009, Belgian Third Division 2002 to 2009).
- No row has `held = unknown`. No `first` or `second` is `unknown`.
- 38 rows have `ended = unknown`. These are the Belgian Fourth Divisions (all years), Belgian Third Division A and B 2002 to 2004, Dutch First Division 2002 to 2004 and French National 2002 and 2003. I found no page with the date of the last match day. All these leagues ended in April, May or June of the label year (inference from the other years).

## Label conventions, with proof from the 3.9.68 rows

Leagues and national cups: the year in which the season ends.

- French First Division 2002 = Olympique Lyonnais, Racing Club Lens (2001/02). French Cup 2001 = Racing Club Strasbourg, Amiens SC (final of May 2001).
- Belgian First Division 2002 = KRC Genk, Club Brugge KV (2001/02). Belgian Cup 2002 = Club Brugge KV, R Excelsior Mouscron (final of May 2002).
- Dutch Premier Division 2002 = Ajax, PSV (2001/02). Dutch Cup 2002 = Ajax, FC Utrecht (final of May 2002).
- The lower leagues follow the same rule: French Second Division 2002 = Ajaccio, Strasbourg (2001/02). Belgian Fourth Division A to D 2002 = the 2001/02 group tables. Dutch First Division 2001 = FC Den Bosch, Excelsior (2000/01).

Super cups: the calendar year in which the match is played, at the start of the next season.

- Belgian Super Cup 2001 = RSC Anderlecht, KVC Westerlo (July 2001, 2000/01 champion against cup winner).
- Dutch Super Cup 2001 = PSV, FC Twente (August 2001).
- French Champions Trophy 2000 = AS Monaco, FC Nantes Atlantique (July 2000). 3.9.68 has no row for 1996, the year the match was not held.

So label 2009 of a super cup is the match of July 2009. All of them were played before the newest database starts.

## Decisions to check

1. Belgian Third Division (only in the databases from September 2008 on). No single third division existed from 2001/02 to 2015/16. The tier was split into Third Division A and B, and the data holds those as separate competitions. I marked all 8 rows `not held`. The later databases fill this competition with the A champion as first and the B champion as second (December 2008, September 2008, March 2009, April 2010 SIM). Three databases use the A champion and A runner-up instead. Each row's `notes` give the A and B champions, so you can fill the rows if you choose the first pattern.
2. French CFA (4th tier, four groups). The federation gives one title, "champion de France amateur". This is the best promoted amateur club, decided by points against the top five of its group. There is no final, so `second` is empty. The reserve teams play a separate final for their own title. I did not use that final. For `ended`, I used the date of the reserve final, because it is the last match of the CFA season. The data rows agree with this convention: September 2008 and March 2009 hold Football Croix-de-Savoie 74 for 2008, and that club was the 2007/08 amateur champion.
3. Belgian League Cup: the last final was in 2000 (1999/2000). The competition was then cancelled, so labels 2002 to 2009 are `not held`.
4. Ties: Dutch Premier Division 2007 (PSV and Ajax level on points, PSV won on goal difference). Belgian First Division 2009 (Standard won the title play-off against Anderlecht). The Fourth Division rows with level points are marked in `notes`. In every tie, `second` is the club that finished second in the final table.
5. Belgian Third Division A 2003: K Berchem Sport won the table but did not go up (no licence). `first` is the table winner.

## Name matching

I used the 3.9.68 spelling when the club is in 3.9.68. Points of doubt:

- RUS Tournaisienne won Fourth Division A in 2001/02. That club is not in the data. It merged with RRC Tournaisien in 2002 to form RFC Tournai. 3.9.68 writes RRC Tournaisien for this row, and I followed it. RFC Tournai (Third Division A 2007, Fourth Division A 2005) is also written RRC Tournaisien, because the data has one Tournai club with the short name "Tournai".
- KSK Hasselt (Fourth Division C 2009, second) is matched to KSK Kermt-Hasselt, short name "Hasselt" (inference).
- RCS Verviétois (Fourth Division D 2005) is matched to RCS Verviers (inference).
- Seraing RUL = R Union Liégeoise - Seraing. URS du Centre = URS Centre. Sottegem = KSV Zottegem. Patro Eisden Maasmechelen = K Patro Maasmechelen.
- KVSK United and KVSK United Overpelt-Lommel are the same club. I used the second name, which is in more databases.
- AC Ajaccio: the 3.9.68 row for French Second Division 2002 reads "Ajaccio". The 3.9.68 club list gives the full name "Ajaccio AC", and I used that.

## Database rows that disagree with the research

Rows where only the spelling differs are left out, for example "AFC Ajax" for Ajax, "Olympique de Marseille", "Racing Club de Lens", "Stade Rennais FC", "Tours Football Club", "VVV-Venlo", "FC Istres Ouest Provence" and "Stade Lavallois Mayenne FC".

A pattern: October 2003 often holds the 2001/02 result under label 2003. September 2005 holds 2003/04 under 2005. February 2008 and November 2007 hold 2005/06 under 2007. April 2010 SIM holds 2007/08 under 2009. In these databases the label shift of slice 11b may be one year too small for some competitions.

| Database | Competition | Label | Database holds | Real |
|---|---|---|---|---|
| 3.9.68 | French National | 2002 | (empty) / (empty) | Clermont Foot Auvergne / Stade de Reims Champagne |
| October 2003 | Belgian Cup | 2003 | KVC Westerlo / KVSK United | RAA La Louviéroise / K Sint-Truidense VV |
| October 2003 | Belgian First Division | 2003 | KRC Genk / Club Brugge KV | Club Brugge KV / RSC Anderlecht |
| October 2003 | Belgian Second Division | 2003 | KVSK United / KFC Turnhout | KSV Cercle Brugge / KAS Eupen |
| October 2003 | Belgian Third Division A | 2003 | KSK Ronse / KSV Zulte Waregem | K Berchem Sport / KV Oostende |
| October 2003 | Belgian Third Division B | 2003 | R Excelsior SC Virton / R Francs Borains | AFC Tubize / FC ZD Oud-Heverlee Leuven |
| October 2003 | Dutch Cup | 2003 | Ajax / FC Utrecht | FC Utrecht / Feyenoord |
| October 2003 | Dutch Premier Division | 2003 | Ajax / PSV | PSV / Ajax |
| October 2003 | French First Division | 2003 | Olympique Lyonnais / Racing Club Lens | Olympique Lyonnais / AS Monaco |
| October 2003 | French League Cup | 2003 | Girondins de Bordeaux / FC Lorient | AS Monaco / FC Sochaux-Montbéliard |
| October 2003 | French Second Division | 2003 | Ajaccio / Racing Club Strasbourg | Toulouse FC / Le Mans Union Club 72 |
| September 2005 | Belgian Second Division | 2005 | KFC Brussels / KV Oostende | KSV Zulte Waregem / KSV Roeselare |
| September 2005 | Belgian Third Division A | 2005 | KSK Ronse / KSV Zulte Waregem | KV Mechelen / R Cappellen FC |
| September 2005 | Belgian Third Division B | 2005 | R Excelsior SC Virton / R Francs Borains | KVSK United Overpelt-Lommel / FC ZD Oud-Heverlee Leuven |
| September 2005 | French League Cup | 2005 | Girondins de Bordeaux / FC Lorient | Racing Club Strasbourg / Stade Malherbe de Caen |
| September 2005 | French National | 2005 | Stade de Reims Champagne / Stade Brestois 29 | Valenciennes FC / ASO Arménienne Valence |
| September 2005 | French Second Division | 2005 | AS Saint-Etienne / Stade Malherbe de Caen | AS Nancy-Lorraine / Le Mans Union Club 72 |
| February 2008, November 2007 | Belgian Fourth Division  C | 2007 | K Bocholter VV / Bahrain (February 2008), Papua New Guinea (November 2007) | K Racing Mol-Wezel / K Lyra TSV |
| February 2008, November 2007 | Belgian Fourth Division  D | 2007 | Racing Jet Wavre / (empty) | RRC Hamoir / FC Bleid |
| February 2008, November 2007 | Dutch Cup | 2007 | AFC Ajax / PSV | Ajax / AZ |
| February 2008, November 2007 | French Cup | 2007 | Paris-Saint-Germain / Olympique de Marseille | FC Sochaux-Montbéliard / Olympique Marseille |
| February 2008, November 2007 | French First Division | 2007 | Olympique Lyonnais / Girondins de Bordeaux | Olympique Lyonnais / Olympique Marseille |
| February 2008 to September 2009 (all 9 later databases) | Belgian Third Division A | 2002 | RRC Tournaisien / (empty) | KSV Zulte Waregem / KFC Vigor Wuitens Hamme |
| February 2008 to September 2009 (all 9 later databases) | Belgian Third Division B | 2002 | ROC Charleroi - Marchienne / (empty) | KAS Eupen / RW Walhain CG |
| December 2008, March 2009, September 2008 | Belgian Fourth Division  C | 2008 | K Bocholter VV / KF Vlazrimi Kicevo | Hoogstraten VV / K Patro Maasmechelen |
| December 2008, March 2009, September 2008 | Belgian Fourth Division  D | 2008 | Racing Jet Wavre / (empty) | URS Centre / RFC Turkania Faymonville |
| December 2008 | Dutch Premier Division | 2008 | PSV / FC Twente | PSV / Ajax |
| December 2008, March 2009, September 2008, April 2010 SIM, February 2010 ODB, June 2010, September 2009 | Belgian Third Division | 2008, 2009 | rows exist | not held (see decision 1) |
| April 2010 SIM, February 2010 ODB, June 2010, September 2009 | Belgian Fourth Division  C | 2009 | K Bocholter VV / KF Vlazrimi Kicevo | K Sportkring Heist / KSK Kermt-Hasselt |
| April 2010 SIM | Belgian Cup | 2009 | RSC Anderlecht / KAA Gent | KRC Genk / KV Mechelen |
| April 2010 SIM | Belgian Second Division | 2009 | KVK Kortrijk / AFC Tubize | K Sint-Truidense VV / K Lierse SK |
| April 2010 SIM | Dutch Cup | 2009 | Feyenoord / Roda JC | SC Heerenveen / FC Twente |
| April 2010 SIM | Dutch First Division | 2009 | FC Volendam / RKC Waalwijk | VVV / RKC Waalwijk |
| April 2010 SIM | Dutch Premier Division | 2009 | PSV / AFC Ajax | AZ / FC Twente |
| April 2010 SIM | French Cup | 2009 | Olympique Lyonnais / Paris-Saint-Germain | En Avant Guingamp / Stade Rennais |
| April 2010 SIM | French First Division | 2009 | Olympique Lyonnais / Girondins de Bordeaux | Girondins de Bordeaux / Olympique Marseille |
| April 2010 SIM | French League Cup | 2009 | Paris-Saint-Germain / Racing Club de Lens | Girondins de Bordeaux / Vannes Olympique Club |
| April 2010 SIM | French National | 2009 | Vannes Olympique Club / Tours Football Club | FC Istres / Stade Lavallois |
| April 2010 SIM | French Second Division | 2009 | Le Havre Athletic Club / FC Nantes Atlantique | Racing Club Lens / Montpellier Hérault SC |
| February 2010 ODB, June 2010, September 2009 | Belgian First Division | 2009 | RSC Anderlecht / R Standard de Liège | R Standard de Liège / RSC Anderlecht |
| February 2010 ODB, June 2010, September 2009 | French CFA | 2009 | Besançon Racing Club Football (group A winner) | US Luzenac (amateur champion) |

February 2010 ODB also has a stray "T" in the third and host places of several rows (Belgian Third Division, French CFA, French League Cup).

## Outside the range, for information

- 3.9.68 French Champions Trophy 1995 holds FC Nantes Atlantique as winner. Paris Saint-Germain won that match (January 1996) on penalties.
- French Champions Trophy 2001 (FC Nantes Atlantique beat Racing Club Strasbourg) has no row in 3.9.68 and is outside the range of this slice.

## Sources

- France: en.wikipedia lists of champions and Coupe de France finals, the Trophée des Champions article, RSSSF Coupe de la Ligue finals, fr.wikipedia palmarès of the second, third and fourth divisions and their season pages (for `ended`).
- Belgium: en.wikipedia lists of champions, cup finals, Super Cup and League Cup. RSSSF season pages `tablesb/belgYY.html` for the Second, Third and Fourth Divisions. nl.wikipedia for the Fourth Division of 2004/05, because RSSSF has no Fourth Division tables for that season.
- Holland: RSSSF season pages `tablesn/nedYY.html` for both leagues, en.wikipedia KNVB Cup and Johan Cruyff Shield articles. Each final's own article gives the date for `ended`.
