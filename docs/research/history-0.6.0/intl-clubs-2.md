# History research: intl-clubs-2

Part `intl-clubs-2`, slice 11c. 16 competitions, 87 rows in `intl-clubs-2.tsv`: 50 `yes`, 37 `not held`, 0 `unknown`. Results come from RSSSF. The `ended` months come from the RSSSF match pages or the English Wikipedia season pages.

## Conventions, with the 3.9.68 rows that prove them

- **African Champions League, CAF Cup, Cup Winners' Cup.** Calendar year. The 3.9.68 row 2001 Al-Ahly / Sundowns is the 2001 Champions League, with its final in December 2001. The row 2000 Jeunesse Sportive de Kabylie / Ismaily is the 2000 CAF Cup.
- **African Super Cup.** Label Y holds the match between the winners of the year-Y cups. That match takes place in January to March of Y+1. Proof: 3.9.68 row 1999 Raja / Africa Sports is the match of 5 March 2000. Row 1995 Orlando Pirates / Jeunesse Sportive de Kabylie is the match of 2 March 1996. RSSSF uses the same labels.
- **Afro-Asian Champions Cup.** The year of the final. The 3.9.68 row 1999 Raja / Pohang is the final of April 1999. The row 1996 Songnam / Orlando Pirates is the final of May 1996.
- **Arab Club Champions Cup, Arab Cup Winners' Cup, Arab Super Cup.** The edition's year. The 3.9.68 row 2001 Al Sadd / Mouloudia Club d'Oran is the August 2001 tournament. The Cup Winners' Cup row 2001 Stade Tunisien is the 2001 edition, which RSSSF says took place in 2002. So an edition named for a year goes in that year. For the Arab Champions League seasons (2003-04 on), I use the year in which the season ends: label 2004 = 2003-04, label 2005 = 2004-05. This gives one edition per label: 2002 (played January to February 2003), 2003 (July 2003), 2003-04 and 2004-05. The split is an inference.
- **Asian Club Championship and AFC Champions League, Asian Cup Winners' Cup.** The year in which the season ends. 3.9.68 row 2002 Suwon / Anyang is the 2001-02 final of 5 April 2002. Row 2001 Suwon / Jubilo is the 2000-01 final of May 2001. The Champions League 2002-03 ended in October 2003, so it goes in label 2003. From 2004 the Champions League is a calendar-year competition.
- **Asian Super Cup.** The year of the match, which is the end year of the season of the two cups. 3.9.68 row 2002 Suwon / Al Hilal is the July 2002 match between the 2001-02 winners.
- **CIS Champions Cup.** The calendar year of the January tournament. 3.9.68 row 1996 Dinamo Kiev / Alania Vladikavkaz is the January 1996 final. Row 2000 Spartak / Zimbru is January 2000.
- **Gulf Club Champions Cup.** The edition's year. 3.9.68 row 2002 Al Ahli (KSA) / Muharraq is the tournament of March 2002. RSSSF puts the 1996 edition, which the data holds as Al Nassr / Zofar, in November 1995. Thus the data follows the edition name.
- **Oceania Champions Cup.** Calendar year. 3.9.68 rows 1999 South Melbourne / Nadi and 2001 Wollongong / Tafea are the 1999 and 2001 tournaments.

## A Lower Division

The data shows that this is not a competition. Other research of this project (`docs/research/rules-0.6.0/fra.md`, `eng.md`, `aus-jap-kor.md`) finds that "A Lower Division" (division 357) is a catch-all division. It holds clubs of many nations that have no playing division. All seven rows are `not held`.

Its one history row, Adelaide United / Central Coast Mariners, agrees with two real results:

- the A-League Pre-Season Challenge Cup final of 19 August 2006: Adelaide United drew 1-1 with Central Coast Mariners and won 5-4 on penalties.
- the 2005-06 A-League: Adelaide United were premiers, and Central Coast Mariners lost the grand final.

Thus the editors possibly copied the record from an A-League competition. That is an inference. If the slice decides to map the record to the Pre-Season Challenge Cup, the finals were as follows (source https://en.wikipedia.org/wiki/A-League_Pre-Season_Challenge_Cup, one source only):

- 2005: Central Coast Mariners 1-0 Perth Glory FC, August 2005.
- 2006: Adelaide United 1-1 Central Coast Mariners (5-4 on penalties), August 2006.
- 2007: Adelaide United 2-1 Perth Glory FC, August 2007.
- 2008: Melbourne Victory 0-0 Wellington Phoenix (8-7 on penalties), August 2008.

## Points of doubt

1. **Confederation Cup in the CAF Cup rows.** From 2004 the CAF Confederation Cup replaced the CAF Cup and the African Cup Winners' Cup. I put it in the African CAF Cup rows, because the databases of 2007 and 2008 keep the CAF Cup record and drop the Cup Winners' Cup record. The Cup Winners' Cup rows of 2004 and 2005 are `not held`. This is an inference. The slice can move the rows to the Cup Winners' Cup.
2. **The AFC Cup is not mapped.** The Asian Cup Winners' Cup merged into the AFC Champions League. The AFC Cup (from 2004) is a new second-level cup for the developing nations, so I did not map it. If the slice maps it to the Asian Cup Winners' Cup, these are the winners and runners-up (RSSSF https://www.rsssf.org/tablesa/as3.html):
   - 2004: Al-Jaish beat Al-Wahda (both Damascus), November 2004.
   - 2005: Al-Faysali beat Al-Nijmeh, October 2005.
   - 2006: Al-Faysali beat Muharraq, November 2006.
   - 2007: Shabab Al-Ordon beat Al-Faysali, November 2007.
   - 2008: Muharraq beat Safa, November 2008.
   - 2009: Al-Kuwait SC beat Al-Karama (Homs), November 2009.
   I did not match these names to the club lists.
3. **Far East Club Championship.** Only the 1998 edition took place. RSSSF links it to the A3 Champions Cup (2003 to 2007), but the A3 Cup has other entrants: the champions of Japan, Korea and China and one host club. I did not map it.
4. **Arab label split.** See the conventions above. The 2002 edition took place in 2003. RSSSF labels it 2003, and Wikipedia labels it 2002.
5. **Spellings from 3.9.68 that differ from the real names.** Al Merreikh Omduran (Omdurman), Coton Sport (Cotonsport Garoua), Chonbuk Hyundai Motors (Jeonbuk), Songnam Ilhwa Chunma (Seongnam), Al Kuwait (Kuwait SC), SC Neftchi Baku, FK Skonto Riga.
6. **Clubs only in later databases.** Dolphins FC and Heartland (from September 2008), Auckland City and Waitakere United (from September 2009). Waitakere City is a different club, so I did not use it.

All names matched a club list. No row has `not in data`.

## Database rows that disagree

Within the label years to research:

| Database | Competition | Label | Holds | Real |
|---|---|---|---|---|
| November 2007, February 2008 | Asian Club Championship | 2007 | Jeonbuk Hyundai Motors / Al Karama | Urawa Red Diamonds / Sepahan Isfahan (Jeonbuk / Al Karama is 2006) |
| December 2008 | Asian Club Championship | 2008 | Jeonbuk Hyundai Motors / Al Karama | Gamba Osaka / Adelaide United |
| September 2008, March 2009 | Asian Club Championship | 2008 | Suwon Samsung Blue Wings / Jubilo Iwata | Gamba Osaka / Adelaide United (the data holds the 2001 result) |
| November 2007, February 2008 | Asian Cup Winners' Cup | 2007 | Al Hilal (KSA) / Jeonbuk Hyundai Motors | not held (the data holds the 2002 result) |
| December 2008 | Asian Cup Winners' Cup | 2008 | Al Hilal (KSA) / Jeonbuk Hyundai Motors | not held |
| September 2008, March 2009 | Asian Cup Winners' Cup | 2008 | Al Shabab (KSA) / Dalian Shide | not held (the data holds the 2001 result) |
| November 2007, February 2008 | Asian Super Cup | 2007 | Suwon Samsung Blue Wings / Al Shabab (KSA) | not held (the data holds the 2001 result) |
| December 2008 | Asian Super Cup | 2008 | Suwon Samsung Blue Wings / Al Shabab (KSA) | not held |
| November 2007, February 2008, September 2008, December 2008, March 2009 | A Lower Division | 2007 or 2008 | Adelaide United / Central Coast Mariners | not a competition (see above) |

The rows of 2002 agree: Asian Club Championship (October 2003, September 2005), Asian Cup Winners' Cup (October 2003, September 2005) and Gulf Club Champions Cup (October 2003). The Asian Super Cup 2002 row is in October 2003, September 2005 and 3.9.68 twice. The result is right, but the row appears twice in each of these databases.

Errors in the 3.9.68 rows before the range (for information only):

- African Champions League 1996 has two rows. Zamalek / Shooting Stars is right. Orlando Pirates / Africa Sports is the 1996 Super Cup pairing and does not belong there.
- African Super Cup 2001: the data holds Hearts of Oak / Zamalek again. Real: Al-Ahly 4-1 Kaizer Chiefs, played 15 March 2002.
- Arab Cup Winners' Cup 2001: the runner-up is Al-Hilal of Sudan, but the data holds Al Hilal (KSA).
- Arab Super Cup 1996: RSSSF gives Riyadh SC as the runner-up. The data holds Al Riyadh, which is probably the same club.
