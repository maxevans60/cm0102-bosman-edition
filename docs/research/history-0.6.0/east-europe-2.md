# History research, part east-europe-2 (Russia, Yugoslavia, Greece, Turkey)

Date 2026-10-17. Results are in `east-europe-2.tsv` (146 rows). All pages were read raw on the day. The main source is RSSSF. Three facts come from English Wikipedia: Greek B Division 2005/06, Greek Cup 2006/07 and Greek B Division 2009/10 (the 2009/10 row also agrees with RSSSF `grk2010.html`).

## Label conventions, with proof

The 3.9.68 rows of 1995 to 2002 prove each convention.

### Season-end year

- Russian Cup: 3.9.68 2000 Lokomotiv Moscow / CSKA Moscow is the 1999/2000 final. 2001 Lokomotiv / Anzhi is the 2000/01 final. 2002 CSKA / Zenith is the 2001/02 final of 12 May 2002.
- Yugoslav First Division, Yugoslav Cup and the Second Division groups: 3.9.68 2002 Partizan / Red Star is 2001/02. 2002 Cup Red Star / Sartid is the 2001/02 final. The 2002 group rows (Radnicki Obrenovac, Radnicki Nis, Javor, Mogren) are the 2001/02 group winners.

### Calendar year

Russian Premier Division, First Division and Second Division zones use the calendar year. 3.9.68 1995 Alania / Lokomotiv is the 1995 season. 2001 Spartak / Lokomotiv is the 2001 season. First Division 2001 Shinnik / Uralan is the 2001 season. The 2001 zone rows match the RSSSF 2001 zone tables:

- Dinamo St. Petersburg / Pskov-2000.
- Metallurg Lipetsk / Salyut-Energia.
- SKA Rostov / Dinamo Stavropol.
- Svetotekhnika / Volga.
- Uralmash / Sodovik.
- SKA-Energia / Metallurg-Zapsib.

### Start year of the season (label Y = season Y/Y+1)

These competitions use the start year:

- Greek National A Division: 3.9.68 1995 Panathinaikos / AEK is 1995/96. 1996 Olympiakos / AEK is 1996/97. 1997 Olympiakos / Panathinaikos is 1997/98. 1998 Olympiakos / AEK is 1998/99.
- Greek Cup: 3.9.68 1997 Panionios / Panathinaikos is 1997/98. 1998 Olympiakos / Panathinaikos is 1998/99. 1999 AEK / Ionikos is 1999/2000. 2000 PAOK / Olympiakos is 2000/01. 2001 AEK / Olympiakos is 2001/02. The 3.9.68 rows 1995 and 1996 are swapped: 1995 holds 1996/97 and 1996 holds 1995/96.
- Greek National B Division: 3.9.68 1995 Kavala is 1995/96. 1998 Trikala is 1998/99. 2000 Aigaleo is 2000/01. 2001 Pas Giannena is 2001/02.
- Turkish Premier Division: 3.9.68 1995 Fenerbahçe / Trabzonspor is 1995/96. 1996 Galatasaray / Besiktas is 1996/97. 2000 Fenerbahçe / Galatasaray is 2000/01.
- Turkish FA Cup: 3.9.68 1995 Galatasaray / Fenerbahçe is 1995/96. 1996 Kocaelispor / Trabzonspor is 1996/97. 2000 Gençlerbirligi / Fenerbahçe is 2000/01.

So the labels to research, 2002 to 2009, are the seasons 2002/03 to 2009/10.

- Turkish 2. Division has no 3.9.68 rows. I used the start-year convention of the other Turkish competitions. This is an inference.
- Greek Super Cup has no rows from 1995 on. RSSSF shows the last match before the gap in Aug 1996, which fits the data's last label 1996. I used the year of the match. This is an inference.

## Points of doubt

1. Turkey and Greece: the later databases appear to use the season-end year. They do this for every Turkish and Greek row in `db-rows.tsv`. Examples: June 2010 Turkish Premier 2009 Besiktas / Sivasspor is 2008/09, and September 2009 Greek Cup 2009 Olympiakos / AEK is 2008/09. The 3.9.68 data uses the start year. I followed 3.9.68, as the brief says. If the game shows 3.9.68 labels as the start year, all my Turkish and Greek rows fit. If the game reads the later databases as season-end, the Turkish and Greek rows are one year late. The lead must decide this.
2. Turkey: under the start-year convention, 3.9.68 has no label 2001 (2001/02) for the Turkish Premier Division or the Turkish FA Cup, because its last label is 2000. Label 2001 is not in the targets, so I did not write it. The 2001/02 league champion is Galatasaray (RSSSF `turkchamp.html`). I did not research the runner-up. The 2001/02 Cup final is Kocaelispor / Besiktas (RSSSF `turkcuphist.html`).
3. Russian Ural and Volga zones: from 2003 they were one zone, Ural-Povolzhye. I wrote the merged zone's result in both the Ural row and the Volga row for 2003 to 2008. The other option is to mark one of them `not held`.
4. Yugoslav Second Division (no group): from 1999/2000 the second level was played only in regional groups. I marked 2002 to 2005 `not held`. In 2004/05 the North, East and West groups became one Serbia group. I wrote its result in the North and West rows (East is not a target for 2005). The Montenegro group of 2004/05 goes in the South row, because group South was the Montenegrin group in earlier seasons.
5. Yugoslavia became Serbia and Montenegro in February 2003. The data's Yugoslav competitions map to the Serbia and Montenegro First League, Cup and Second League groups to 2004/05. After the split in 2006 nothing maps, but the targets end at 2005.
6. Greek A Division 2007/08 to 2009/10: a play-off for European places followed the league. I wrote the league table's runner-up as second, and the last regular match day as `ended`.
7. `ended` unknown: the Turkish 2. Division 2002/03 and 2003/04. RSSSF gives no match dates for those tables. Yugoslav South 2001/02 has an undated last round after 26 May 2002, so its month (2002-06) is an inference. Yugoslav West 2001/02 ended with a replayed match on 3 Aug 2002.
8. Greek Cup 2002/03: RSSSF prints the final date as 18.5.1993. This is a typo for 2003.
9. Name matches that are inferences:
   - Spartak-MZK Ryazan (Russian Centre 2006) = data club "Spartak-MGK".
   - Lukoil Chelyabinsk (Ural-Povolzhye 2003 and 2004) = data club "Gelios-Lukoil Tchelyabinsk", short name Lukoil.
   - FK Moskva (Cup 2006/07) has four spellings in the data. I used "FK Moscow".
10. Ties settled by tiebreak:
    - Russian Premier 2002 was settled by a golden match.
    - Russian Centre 2003 was settled by a golden match.
    - Russian Premier 2006 was settled on wins.
    - Greek A 2002/03 was settled on head-to-head.
    - Russian West 2008 and Ural 2002: the clubs were level on points, and I followed the RSSSF order.
    - Greek B 2005/06: the clubs were level on points, and I followed the Wikipedia order.

## Database rows that disagree with the result

| Database | Competition | Label | Database holds | Real |
|---|---|---|---|---|
| October 2003 | Russian Cup | 2003 | CSKA Moscow / Zenith (2001/02) | Spartak Moscow / FK Rostov |
| October 2003 | Turkish Premier Division | 2003 | Besiktas / Galatasaray (2002/03) | Fenerbahçe / Trabzonspor (2003/04) |
| October 2003 | Yugoslav Cup | 2003 | Red Star / Sartid (2001/02) | Sartid / Red Star |
| October 2003 | Yugoslav Second Division East | 2003 | Radnicki Nis / Napredak (2001/02) | Napredak / Mladi Radnik Pozarevac |
| October 2003 | Yugoslav Second Division North | 2003 | Radnicki Obrenovac / Buducnost B. Dvor (2001/02) | Buducnost B. Dvor / Mladost Apatin |
| October 2003 | Yugoslav Second Division South | 2003 | Mogren / Buducnost Podgorica (2001/02) | Kom / Bokelj |
| October 2003 | Yugoslav Second Division West | 2003 | Javor / Borac Cacak (2001/02) | Borac Cacak / Bane Raska |
| September 2005 | Greek National B Division | 2005 | Kerkira / Apollon Kalamarias (2003/04) | Ergotelis / Kerkira (2005/06) |
| September 2005 | Russian First Division | 2005 | Amkar / Kuban (2003) | Luch-Energia / Spartak Nalchik |
| September 2005 | Russian Premier Division | 2005 | Lokomotiv / CSKA (2004) | CSKA / Spartak Moscow |
| September 2005 | Russian Second Division - Ural | 2005 | Ural Yekaterinburg / Sodovik | Sodovik / Lada Togliatti |
| September 2005 | Turkish Premier Division | 2005 | Fenerbahçe / Trabzonspor (2004/05) | Galatasaray / Fenerbahçe (2005/06) |
| September 2005 | Yugoslav Cup | 2005 | Crvena Zvezda / Sartid | Zeleznik / Red Star |
| September 2005 | Yugoslav First Division | 2005 | Crvena Zvezda / Partizan (2003/04) | Partizan / Red Star |
| September 2005 | Yugoslav Second Division South | 2005 | Mogren / Buducnost Podgorica | Jedinstvo Bijelo Polje / Kom |
| September 2005 | Yugoslav Second Division West | 2005 | Javor / Borac Cacak | Buducnost B. Dvor / Javor (Serbia group) |
| February 2008, November 2007 | Russian Second Division - Ural | 2007 | Ural Yekaterinburg / Sodovik | Volga Uljanovsk / Gazovik Orenburg |
| February 2008, November 2007 | Russian Second Division - West | 2007 | Petrotrest / Pskov-2000 | Sportakademclub / Sheksna |
| December 2008, March 2009, September 2008 | Russian Second Division - Ural | 2008 | Ural Yekaterinburg / Sodovik | Volga Nizhniy Novgorod / Gazovik Orenburg |
| December 2008, March 2009, September 2008 | Russian Second Division - West | 2008 | Petrotrest / Pskov-2000 | FK MVD Russia / Torpedo Vladimir |
| February 2010 ODB, June 2010, September 2009 | Russian Second Division - West | 2009 | Petrotrest / Pskov-2000 | Dinamo St. Petersburg / Torpedo Vladimir |
| March 2009, September 2008 | Greek Cup | 2008 | Olympiakos / Aris (2007/08) | Olympiakos / AEK (2008/09) |
| April 2010 SIM | Greek Cup | 2009 | Olympiakos / Aris (2007/08) | Panathinaikos / Aris (2009/10) |
| February 2010 ODB, June 2010, September 2009 | Greek Cup | 2009 | Olympiakos / AEK (2008/09) | Panathinaikos / Aris |
| September 2008 | Greek National B Division | 2008 | Panserraikos / Thrasivoulos (2007/08) | Atromitos / Pas Giannena (2008/09) |
| April 2010 SIM | Greek National B Division | 2009 | Panserraikos / empty (2007/08) | Olympiakos Volou / Kerkira (2009/10) |
| February 2010 ODB, June 2010, September 2009 | Greek National B Division | 2009 | Atromitos / PAS Giannina (2008/09) | Olympiakos Volou / Kerkira |
| March 2009, September 2008 | Turkish FA Cup | 2008 | Kayserispor / Gençlerbirligi (2007/08) | Besiktas / Fenerbahçe (2008/09) |
| April 2010 SIM | Turkish FA Cup | 2009 | Kayserispor / Gençlerbirligi (2007/08) | Trabzonspor / Fenerbahçe (2009/10) |
| February 2010 ODB, June 2010, September 2009 | Turkish FA Cup | 2009 | Besiktas / Fenerbahçe (2008/09) | Trabzonspor / Fenerbahçe |
| March 2009, September 2008 | Turkish Premier Division | 2008 | Galatasaray / Fenerbahçe (2007/08) | Besiktas / Sivasspor (2008/09) |
| April 2010 SIM | Turkish Premier Division | 2009 | Galatasaray / Fenerbahçe | Bursaspor / Fenerbahçe (2009/10) |
| February 2010 ODB, June 2010, September 2009 | Turkish Premier Division | 2009 | Besiktas / Sivasspor (2008/09) | Bursaspor / Fenerbahçe |
| February 2010 ODB, June 2010 | Turkish 2. Division | 2009 | Manisaspor / Diyarbakirspor (2008/09) | Kardemir Demir-Çelik Karabükspor / Bucaspor (2009/10) |

A season in brackets is the real season that the database's pair matches, where one matches. Most Turkish and Greek disagreements are the one-year shift of point of doubt 1.

Rows that agree:

- 3.9.68 label 2002 rows.
- October 2003 Yugoslav First Division 2003. The pair Partizan / Red Star happens to be right for 2002/03 too.
- September 2005 Russian Cup 2005.
- The Russian Cup 2002 rows of the later databases. February 2008 and December 2008 spell the club "Zenit St. Petersburg", which is a spelling difference only.
- March 2009 and September 2008 Russian Cup 2008.
- February 2008 and November 2007 Turkish Premier Division 2007. The pair matches 2007/08 and also 2005/06, so the agreement may be chance.

## Names not matched

Every name in `first` and `second` is a full name in `clubs/Russia.tsv`, `clubs/Yugoslavia.tsv`, `clubs/Greece.tsv` or `clubs/Turkey.tsv`. The inferred matches are in point of doubt 9. Where a club changed its name, I used the 3.9.68 spelling of the same club and gave the real name in `notes`:

- Zenit is "Zenith St. Petersburg".
- Kayseri Erciyesspor is "Erciyesspor".
- Manisaspor is "Vestel Manisaspor".
- Spartak Vladikavkaz 2006 is "Alania Vladikavkaz".
