# Check pass: intl-clubs-2

Part `intl-clubs-2`, slice 11c. The first source of `intl-clubs-2.tsv` is RSSSF. This check uses the English Wikipedia as the second publisher. I read every page raw (`action=raw`) with curl on 2026-10-17 and took the values from the infobox fields `champion_other` and `second_other`, the final match boxes and the date fields.

`intl-clubs-2.check.tsv` has 179 lines. It covers each filled field of the 46 `yes` rows (`first`, `second` and `ended`) and the `held` field of the 41 `not held` rows.

## Counts

| Result | Lines |
|---|---|
| agree | 172 |
| disagree | 0 |
| no second source | 7 |

The 7 `no second source` lines are the seven `A Lower Division` rows (2002 to 2008). This is a catch-all division in the game data and not a real competition, so no outside page can confirm or deny the `not held` value.

## Disagreements

None. Each winner, runner-up and end month agrees with the Wikipedia season page.

## Notes

1. The `.md` of the part says 50 `yes` rows and 37 `not held` rows. The `.tsv` holds 46 `yes` rows and 41 `not held` rows. The count in the `.md` is wrong, and the rows themselves are correct.
2. Arab Cup Winners' Cup 2002 `not held`: Wikipedia says that the cup merged into the Arab Unified Club Championship in 2002. It also says that the last edition took place in Tunis in 2002. That edition is the 2001 edition, which the 3.9.68 data holds under label 2001, so the `not held` value for label 2002 agrees.
3. Arab Club Champions Cup 2002: Wikipedia also labels it 2002 (17 January to 2 February 2003). This agrees with the label that the first agent chose.
4. Far East Club Championship: the only second source that I found is the Wikipedia honours list of FC Rotor Volgograd. It says that the competition took place only once, in China in 1998.
5. Results decided on penalties, which Wikipedia confirms: African Super Cup label 2005 (Al Ahly 0-0 FAR Rabat, 4-2 on penalties), Asian Super Cup 2002 (1-1 on aggregate, Suwon won 4-2 on penalties), CIS Champions Cup 2007 (Pakhtakor beat Ventspils 9-8 on penalties).
6. Gulf Club Champions Cup 2003: Wikipedia says that Al-Muharraq came second on the toss of a coin.
7. The mapping of the CAF Confederation Cup to the `African CAF Cup` rows from 2004 is the first agent's inference. The check confirms only the results of the Confederation Cup, not the mapping.
