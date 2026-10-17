# Competition history research: nordic-2 (Norway, Denmark, Finland)

Part `nordic-2` of slice 11c. It covers the Norway, Denmark and Finland competitions of the `nordic` input. Sweden is in another part. The table is `nordic-2.tsv`: 132 rows for 21 competitions.

All sources were read on 2026-10-17 with `curl` and grep. English Wikipedia season pages were read as raw wikitext. RSSSF pages were read as raw HTML.

## Conventions, with proof from the 3.9.68 rows

### Norway: calendar year

The label year is the calendar year of the season.

- Norwegian Premier Division 2001 = Rosenborg BK, Lillestrøm SK. These are the champion and runner-up of the 2001 Tippeligaen.
- Norwegian First Division 2001 = Vålerenga Fotball, IK Start. These are the top two of the 2001 1. divisjon.
- Norwegian Cup 2001 = Viking FK, Bryne FK. This is the final of November 2001.
- Norwegian Second Division Group 1 to 8 end at label 2000 in the 3.9.68 data. They follow the same calendar rule.

### Denmark: the year where the season ends

The Danish leagues and the cup play autumn to spring. The label year is the year in which the season ends.

- Danish Premier Division 2002 = Brøndby, FC København = Superliga 2001/02. Label 2001 = FC København, Brøndby = 2000/01.
- Danish Cup 2002 = Odense Boldklub, FC København = the final of May 2002. Label 2001 = Silkeborg, AB = the 2000/01 final.
- Danish First Division 2002 = Køge, Farum = 1st Division 2001/02.
- Danish Second Division 2002 = BK Skjold, Ølstykke = 2nd Division 2001/02.

### Finland: calendar year

- Finnish Premier Division 2001 = Tampere United, HJK = Veikkausliiga 2001.
- Finnish Cup 2001 = Atlantis FC, Tampere United = the final of 12 November 2001 (RSSSF fincuphist).
- Finnish First Division 2001 = FC Hämeenlinna, FF Jaro. In 2001 the Ykkönen had two zones and a promotion playoff. Hämeenlinna beat Jaro in the playoff final (RSSSF fin2hist). So the 3.9.68 data uses the playoff final for this year.
- Finnish Second Division East, North, South and West 2001 = FC KooTeePee, FC Korsholm, FC Viikingit and VG-62. These are the winners of the Itälohko, Pohjoislohko, Etelälohko and Länsilohko groups of the 2001 Kakkonen. So East, North, South and West map to those four groups by name.

## The `ended` column

- Norway: league end months come from the infobox dates of the Wikipedia season pages. Cup final dates come from the cup pages (all November). The 2. divisjon end months come from the last match day on the RSSSF Norway group pages (always October).
- Denmark: Superliga end months come from the Wikipedia season text. For 2004/05 the source is the Wikipedia text (19 June 2005). Cup final dates come from the cup pages. 1st Division 2006/07 and 2007/08 come from the Wikipedia text.
- Finland: the end months come from the last dated round on the RSSSF season pages (`fin02.html` to `fin09.html`). The Veikkausliiga 2006 date comes from the Wikipedia text. Cup final dates come from RSSSF fincuphist.
- `unknown` rows: I found no dated page for these. They are the Danish 1st Division 2001/02 to 2005/06 and 2008/09, every Danish 2nd Division row, and the Finnish Second Division groups of 2002. Inference: the Danish seasons play autumn to spring, and the 2002 Kakkonen groups end before the promotion playoffs of October 2002. Thus each of these editions ended before every database start date that uses it, but I have no page that proves the month.
- League end months are the last round of the league. Promotion or relegation playoffs after the last round are not counted (for example the Veikkausliiga playoffs of November 2003, 2007 and 2008).

## Points of doubt

1. Norwegian Second Division groups. From 2002 the 2. divisjon has 4 groups, not 8. Groups 1 to 4 are mapped by group number. This mapping is an inference. The regions changed: 3.9.68 Group 4 held southern clubs (Vidar, Mandalskameratene), while the real avdeling 4 from 2002 holds the northern clubs. Groups 5 to 8 are `not held` for 2002 to 2005.
2. Norwegian Second Division Group 2, 2005. RSSSF names the winner "Oslo Øst". Wikipedia names it Manglerud Star Toppfotball, and says that club went up to the 1. divisjon. I wrote `FK Oslo Øst`, the name that RSSSF uses and that is in the 3.9.68, October 2003 and September 2005 data. `Manglerud/Star IL` is the alternative.
3. Finnish First Division 2002. That year there was no single Ykkönen table. Each zone sent two sides to a promotion/relegation group with Veikkausliiga sides. KuPS (a Premier side) won that group. The Ykkönen sides placed TPS 2nd, Jokerit 4th, KooTeePee 6th and TP-47 8th. I wrote TPS and Jokerit as first and second. This is an inference. The zone winners were KooTeePee (south) and TPS (north). The English Wikipedia list of Ykkönen champions gives KuPS for 2002. That is wrong, because KuPS played in the Veikkausliiga in 2002.
4. Danish Second Division from 2005/06 (labels 2006 to 2009). The division was split into two pools, East and West. I wrote one row per pool with `part` = `East` or `West`. If the game can hold only one row per label, there is no single champion. The February 2010 ODB and June 2010 databases put the two pool winners in first and second, in an order that changes from year to year.
5. Reserve sides in the Danish 2nd Division: Esbjerg fB II (West winner 2005/06), Brøndby IF II (East runner-up 2006/07), Ikast FS, the FC Midtjylland reserve side (West runner-up 2006/07), and Odense BK II (West runner-up 2008/09). They are not in data. The databases write the first-team name (for example `Esbjerg Forenede Boldklubber` for 2006).
6. Kolding FC (2nd Division 2004/05) is matched to the databases' `Kolding Idrætsforening FC`. This is an inference.
7. Randers FC (1st Division runner-up 2003/04 and 2005/06) is the 2003 successor of Randers Freja FC. I wrote `Randers FC`. The September 2005 database writes `Randers KFUM` for 2003/04, which is a different club.
8. Lyngby BK (1st Division 2006/07) is `Lyngby Boldklub af 1921`. The `Lyngby Football Club` of the 3.9.68 data is the earlier club.
9. PK-35 in the Kakkonen 2003 and 2004 was the Helsinki club. RSSSF writes "PK-35 (Helsinki)". I wrote `Pallokerho-35` (3.9.68). `PK-35 Vantaa` in the newer databases is the later club.
10. Spelling: I used the 3.9.68 spelling where the club is there, for example `Vålerenga Fotball`, `Aalesund FK`, `Alta IL`, `Stabæk IF`, `FC Inter`, `Jalkapallo TPS` and `Myllykosken Pallo-47`. Some clubs are only in the newer databases, so I used the newer spelling: `Hønefoss BK`, `Sparta Sarpsborg`, `AC Oulu`, `Kokkolan Palloveikot`, `JIPPO Joensuu` and `Klubi-04 Helsinki`.

## Database rows that disagree with the result

The labels are after the year shift of slice 11b. "Real" is this research.

Many of these rows hold the season of the year before the label. This applies to most calendar-year rows (Norway, Finland) and to some Danish rows. Inference: the source data stored the previous complete season under the start year.

| Database | Competition | Label | Database holds | Real |
|---|---|---|---|---|
| October 2003 | Danish Premier Division | 2003 | Brøndby, FC København (2001/02) | FC København, Brøndby |
| October 2003 | Danish Cup | 2003 | Odense, FC København (2001/02) | Brøndby, FC Midtjylland |
| October 2003 | Danish First Division | 2003 | Køge, Farum (2001/02) | Herfølge, BK Frem |
| October 2003 | Danish Second Division | 2003 | BK Skjold, Ølstykke (2001/02) | FC Nordjylland, Nykøbing Falster Alliancen |
| September 2005 | Danish First Division | 2005 | Silkeborg, Randers KFUM (2003/04) | SønderjyskE, AC Horsens |
| September 2005 | Danish Second Division | 2005 | BK Skjold, Ølstykke (2001/02) | Kolding FC, Brabrand IF |
| November 2007, February 2008 | Danish Premier Division | 2007 | FC København, Brøndby (2005/06) | FC København, FC Midtjylland |
| December 2008 | Danish Premier Division | 2008 | FC København, Brøndby | Aalborg, FC Midtjylland |
| September 2008, March 2009 | Danish First Division | 2008 | Lyngby BK (2006/07) | Vejle, SønderjyskE |
| April 2010 SIM | Danish Premier Division | 2009 | Aalborg, FC Midtjylland (2007/08) | FC København, Odense |
| April 2010 SIM | Danish Cup | 2009 | Brøndby, Esbjerg (2007/08) | FC København, Aalborg |
| April 2010 SIM | Danish First Division | 2009 | Lyngby BK (2006/07) | Herfølge, Silkeborg |
| February 2010 ODB, June 2010 | Danish Second Division | 2006 to 2008 | the two pool winners in first and second | one row per pool, see doubt 4 |
| September 2005 | Finnish Premier Division | 2005 | Haka, Allianssi (2004) | MyPa, HJK |
| September 2009, February 2010 ODB, June 2010 | Finnish Premier Division | 2009 | Inter, Honka, Lahti (2008) | HJK, Honka |
| September 2005 | Finnish Cup | 2005 | MyPa, Hämeenlinna (2004) | Haka, TPS |
| September 2008, March 2009 | Finnish Cup | 2008 | Tampere United, Honka (2007) | HJK, Honka |
| September 2009, February 2010 ODB, June 2010 | Finnish Cup | 2009 | HJK, Honka (2008) | Inter, Tampere United |
| April 2010 SIM | Finnish Cup | 2009 | Tampere United, Honka (2007) | Inter, Tampere United |
| September 2005 | Finnish First Division | 2005 | TP-47, RoPS (2003) | Honka, VPS |
| September 2008, March 2009 | Finnish First Division | 2008 | KuPS, RoPS, JJK (2007) | JJK, Viikingit |
| September 2009, February 2010 ODB, June 2010 | Finnish First Division | 2009 | JJK, Viikingit, Hämeenlinna (2008) | AC Oulu, KPV |
| April 2010 SIM | Finnish First Division | 2009 | KuPS, RoPS, JJK (2007) | AC Oulu, KPV |
| September 2005 | Norwegian Cup | 2005 | Brann, Lyn (2004) | Molde, Lillestrøm |
| September 2008, March 2009 | Norwegian Cup | 2008 | Lillestrøm, Haugesund (2007) | Vålerenga, Stabæk |
| September 2009, February 2010 ODB, June 2010 | Norwegian Cup | 2009 | Vålerenga, Stabæk (2008) | Aalesund, Molde |
| April 2010 SIM | Norwegian Cup | 2009 | Lillestrøm, Haugesund (2007) | Aalesund, Molde |
| September 2005 | Norwegian First Division | 2005 | HamKam, Fredrikstad (2003) | Stabæk, Sandefjord |
| September 2008, March 2009 | Norwegian First Division | 2008 | Molde, (empty), Bodø/Glimt (2007) | Odd Grenland, Sandefjord |
| September 2009, February 2010 ODB, June 2010 | Norwegian First Division | 2009 | Sogndal, Hønefoss, Kongsvinger | Haugesund, Hønefoss |
| April 2010 SIM | Norwegian First Division | 2009 | Molde, HamKam, Bodø/Glimt (2007) | Haugesund, Hønefoss |
| September 2008, March 2009 | Norwegian Premier Division | 2008 | Brann, Stabæk, Viking (2007) | Stabæk, Fredrikstad |
| April 2010 SIM | Norwegian Premier Division | 2009 | Brann, Stabæk, Viking (2007) | Rosenborg, Molde |

These rows agree: 3.9.68 label 2002 (Danish Cup, First, Premier and Second Division), September 2005 Danish Premier Division and Danish Cup, September 2008 and March 2009 Danish Premier Division and Danish Cup (the Brøndby spelling differs, but the club is the same), the September 2009, February 2010 ODB and June 2010 Danish rows of label 2009, and the February 2010 ODB and June 2010 Danish Second Division rows of 2004 and 2005.

The third place of the February 2010 ODB and June 2010 Danish Second Division rows of 2004 and 2005 (Næstved, Lyngby) is the club of the division's top scorer. Club competitions do not use third place.

## Names that I could not match

These are all reserve sides that are not in data: Esbjerg fB II, Brøndby IF II, Ikast FS (FC Midtjylland reserves) and Odense BK II. Every other club matched a full name in `clubs/Norway.tsv`, `clubs/Denmark.tsv` or `clubs/Finland.tsv`.
