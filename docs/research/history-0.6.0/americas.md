# History research: americas (Argentina, Mexico, United States)

Part `americas`, slice 11c. The rows are in `americas.tsv`: 63 rows, all `held = yes`, no `unknown` rows. Each row has an `ended` month. Some `ended` values are inferred, and the notes of those rows say so.

## Conventions, with proof from the 3.9.68 rows

### Argentine Premier Division: two rows per label, label Y = Apertura Y and Clausura Y+1

- 3.9.68 label 2001 holds Racing Club / River Plate and River Plate / Gimnasia y Esgrima de La Plata. RSSSF (arghist-pro2000s) shows that Racing won the Apertura 2001 with River second, and River won the Clausura 2002 with Gimnasia (LP) second.
- 3.9.68 label 2000 holds Boca Juniors / River Plate (Apertura 2000) and San Lorenzo / River Plate (Clausura 2001). RSSSF agrees.
- `first` is the champion. `second` is the club second in that tournament's table, or the loser of a title play-off (Apertura 2006, Apertura 2008).

### Argentine Second Division: one row per label, label Y = season Y/Y+1 (Primera B Nacional)

- 3.9.68 label 1995 = Huracán de Corrientes, 1999 = Huracán, 2000 = Banfield, 2001 = Olimpo. RSSSF arg2champ gives these clubs as champions of 1995/96, 1999/2000, 2000/01 and 2001/02.
- `second` in 3.9.68 is the loser of the championship play-off when there was one: Quilmes lost the play-offs of 1999/2000 (to Huracán) and 2000/01 (to Banfield) (RSSSF arg2hist). In 2001/02 Olimpo won the Apertura and went up directly, and 3.9.68 gives Quilmes, the Apertura runner-up.
- I used the same rule: the play-off loser, or else the runner-up of the deciding table. In 2002/03 and 2006/07 one club won both tournaments. For those seasons I took the second club of the aggregate table, which is an inference.

### Mexican First Division: two rows per label, label Y = Apertura (Invierno) Y and Clausura (Verano) Y+1

- 3.9.68 label 2001 holds Pachuca / UANL (Invierno 2001 final) and América / Necaxa (Verano 2002 final). Label 2000 holds Morelia / Toluca (Invierno 2000) and Santos Laguna / Pachuca (Verano 2001). Source: List of Mexican football champions (en.wikipedia).
- `first` and `second` are the winner and loser of the liguilla final.

### Mexican First Division A: one row per label, label Y = the promotion final (Campeón de Ascenso) of season Y/Y+1

- 3.9.68 label 1997 = Pachuca / Tigrillos de Saltillo. Pachuca beat Tigrillos UANL in the 1997-98 promotion final (es.wikipedia, Campeón de Ascenso).
- 3.9.68 label 2001 = Atlético San Luis / Irapuato. San Luis beat Veracruz in the 2001-02 final. RSSSF mex03 also names "San Luis (champion of second Division), Veracruz (runners-up)". The data's second club does not match (see below).
- This competition has one row per label, so `part` is empty.

### Mexican Second Division: one row per label, label Y = Campeón de Campeones of season Y/Y+1 (Segunda División, third level)

- 3.9.68 label 2001 = Cihuatlán / Astros de Ciudad Juárez. That is the 2001-02 Campeón de Campeones result, Cihuatlán 2-0 Astros (es.wikipedia). Label 1998 = Alacranes de Durango, who won both 1998-99 tournaments.
- The 3.9.68 rows for 1999 and 2000 match the real results only in part, so this convention is the weakest of the part. See "Points of doubt".

### PrepreLibertadores Cup (Mexico): label Y = the Mexican qualifier played in Y for Copa Libertadores Y+1

- 3.9.68 label 2001 = América / Morelia. In the Pre-Libertadores group of September to November 2001, America finished first and Morelia second (RSSSF copa02).
- 3.9.68 label 2000 = Cruz Azul / Atlante. In the 2000 group, Cruz Azul finished first and Atlante fourth, but Atlante was the second Mexican club (RSSSF copa01). So `first` and `second` are the Mexican clubs in group order.
- Label 2002 is the 2002 group (Pumas first, Cruz Azul second). The Pre-Libertadores ended in 2002. I mapped label 2003 to its successor, the InterLiga of January 2004 (Santos Laguna Mexico 1, América Mexico 2). This mapping is an inference.

### American Major League, US Open Cup, American A-League: calendar year

- 3.9.68 MLS 2001 = San Jose / Los Angeles Galaxy (MLS Cup 2001). Open Cup 2001 = Los Angeles Galaxy / New England (2001 final). A-League 2001 = Rochester / Hershey (2001 final). These are play-off or cup final winners and losers.
- The A-League became the USL First Division in 2005. It stayed the second tier, so rows 2005 to 2008 are the USL First Division.

## Points of doubt

- Mexican Second Division: the 3.9.68 rows for 1999 (Potros Zitácuaro / Truenos de Cuautitlán) and 2000 (Monarcas de Zitácuaro / Cihuatlán) differ from es.wikipedia (1999-2000 Marte beat Cuautitlán, 2000-01 Águilas de Tamaulipas beat Potros Zitácuaro). The label convention (season Y/Y+1) still holds. Only the clubs differ.
- Mexican Second Division 2004/05: Académicos de Atlas won both tournaments, so no deciding match took place. `second` = Cachorros UdeG, the Clausura 2005 losing finalist, which also played the promotion play-off (RSSSF mex05). This is an inference.
- Mexican Second Division 2002/03: Coatzacoalcos is matched to Atlético Delfines de Coatzacoalcos. This is an inference, because the data also has a club named Delfines Fútbol Club.
- Argentine Apertura 2008: San Lorenzo, Boca and Tigre all finished on 39 points. In the play-off group Boca came first and Tigre second on goal difference. I put Tigre as `second`. The league table puts San Lorenzo first and Boca second.
- Argentine Second Division 2006/07: San Martín (SJ) and Huracán both had 69 aggregate points. RSSSF lists San Martín (SJ) second.
- PrepreLibertadores 2003 maps to InterLiga 2004 (see above).
- `ended` is inferred for these rows: Argentine Second Division 2002, 2006, 2007, 2008 and 2009, Mexican Second Division 2002 and 2003, and A-League 2007. All of these editions ended between May and October, as the season pages show for the adjacent years.

## Database rows that disagree with the result

| Database | Competition | Label | Database holds | Real result |
|---|---|---|---|---|
| April 2010 SIM | Argentine Premier Division | 2009 | Vélez Sarsfield / Huracán (third Lanús) | That is Clausura 2009 (label 2008). Label 2009 Clausura = Clausura 2010, Argentinos Juniors / Estudiantes de La Plata. The Banfield / Newell's row agrees. |
| February 2008, November 2007 | Argentine Premier Division | 2007 | Boca Juniors / Lanús | Apertura 2007 Lanús / Tigre, Clausura 2008 River Plate / Boca Juniors |
| March 2009, September 2008 | Argentine Second Division | 2008 | San Martín de Tucumán / Godoy Cruz Antonio Tomba | 2008/09: Atlético Tucumán / Chacarita Juniors (the database pair is 2007/08, label 2007) |
| April 2010 SIM, February 2010 ODB, June 2010, September 2009 | Argentine Second Division | 2009 | San Martín de Tucumán / Godoy Cruz Antonio Tomba | 2009/10: Olimpo de Bahía Blanca / Quilmes. February 2010 ODB also holds "T" in third and host. |

Older 3.9.68 rows that differ from the real results (outside the range researched here, for information only):

- Mexican First Division A 2001: the second club is Irapuato. The real loser of the final was Veracruz.
- Mexican First Division A 2000: the first club is Tampico Madero. The real winner was La Piedad, who beat Gallos de Aguascalientes.
- Mexican First Division A 1999: the first club is Gallos Blancos. The real winner was Irapuato, who won both tournaments.

## Names

Clubs not in any database, with their common names in the TSV:

- Lobos BUAP
- Académicos de Atlas
- Cachorros UdeG
- Pegaso Anáhuac
- Delfines de Coatzacoalcos B

Mappings that need care:

- Houston Dynamo (MLS Cup 2006, 2007). Its full name in most databases is Tampa Bay Mutiny. In November 2007 it is San Jose Earthquakes. Both have the short name Houston Dynamo. The 2003 San Jose Earthquakes row points at the Houston side in November 2007.
- New York Red Bulls (2008) is NY/NJ Metrostars. MetroStars (2003) is NYNJ Metrostars, the 3.9.68 spelling.
- FC Dallas is Dallas Burn.
- Pumas UNAM is Club de Fútbol Universidad, and Tecos is Club Tecos de la UAG. These are the 3.9.68 names, and these Mexican rows run only to 2005.
- Dorados uses the data spelling Club Dorados de Culiacn.
- The 2009 Open Cup winner, Seattle Sounders FC, is the data club Seattle Sounders.
