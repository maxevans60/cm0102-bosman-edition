# Brief for the history research of slice 11c

Date 2026-10-17. Spec `docs/specs/2026-10-16-seasons-0.6.0-spec.md`, Q347 and slice 11c. Process: the `league-research` skill. Every research agent and every check agent gets this brief and its part name only.

## The task

The season databases before 2010/11 (October 2003, September 2005, November 2007, September 2008, September 2009, their extra versions, and the 3.9.68 data on the 2002/03 tile) have no competition history between 2001 and the season before the start. The game will add the real rows in memory at a new game. Your part lists the competitions and the label years to research. For each competition and each label year, find the real result.

## Input (outside the repo, database content: never copy it into the repo)

Folder `/private/tmp/claude-501/-Users-max-Projects-cm0102/beafac03-d2b9-463c-91fd-0fbf792817c8/scratchpad/hist-input/`:

- `<part>/targets.tsv`: one line per competition: kind (club or nation), the competition name as the game data has it, its nation (empty for a continental or world competition), the last label year before 2002 in the data, the label years to research, and the databases that hold it.
- `<part>/db-rows.tsv`: the history rows of these competitions. The rows of `3.9.68` from 1995 on show how the original data labels a season. The rows of the other databases are their rows inside the label years to research, after the year shift of slice 11b (`label_after_shift`).
- `clubs/<nation>.tsv`: the full and short club names of each nation in the 3.9.68 data and in every database before 2010/11. `clubs/national sides.tsv`: the national sides.

## The record

A history row holds a competition, a label year and four places (`club_comp_history.dat` and `nation_comp_history.dat`, 26 bytes; layout in `tools/seasons/history_gaps.py`): first, second, third and host. Club competitions use first and second only: the champion and the runner-up of a league, the winner and the losing finalist of a cup or a super cup. Nation competitions (and the Olympic Games) use all four: winner, runner-up (the losing finalist), third (the winner of the third place match; if there was none, write `none` and say how the data shows such a year), host. For a tournament with two hosts, check the 3.9.68 rows (Euro 2000) and follow them.

## The label year

The label year is the year that the row holds. Find the convention of each competition from the 3.9.68 rows of 1997 to 2002 and real results:

- Most competitions: the year where the season ends (3.9.68: English Premier Division 2002 Arsenal = 2001/02).
- Calendar-year competitions: the calendar year (3.9.68: Swedish Premier Division 2001 Hammarby = the 2001 season).
- Other conventions exist. Example: Argentine Premier Division has two rows per label year, and the label year Y holds the Apertura of Y and the Clausura of Y + 1. Write the convention in the `.md` with the 3.9.68 rows that prove it.

Research every label year of the range in `targets.tsv`, also when a database already holds a row for it. The last label year is the start year of the newest database that holds the competition. For a calendar-year competition, the last label year is a season that ends after the game starts. Research it all the same (the 3.9.68 data does the same for 2001).

## Output (in the repo, `docs/research/history-0.6.0/`)

1. `<part>.tsv`, one row per competition, label year and tournament, columns:
   `kind  competition  nation  label  real_season  part  held  first  second  third  host  notes  sources  ended`
   - `ended` (added 2026-10-17, last column): the year and month of the last match of that edition (the final, or the last match day of a league), as `YYYY-MM`. The game uses it to add only the editions that ended before its start date. Empty for `not held` rows.
   - `real_season`: the real label, for example `2001/02`, `2002`, `Apertura 2002`.
   - `part`: empty, or the tournament when a label year has two rows (`Apertura`, `Clausura`).
   - `held`: `yes`, `not held` (the competition did not take place, was not finished, or no longer exists; say which in `notes`), or `unknown`.
   - `first` to `host`: the names exactly as the full name in `clubs/<nation>.tsv` (or `national sides.tsv`), when the club is in one of the databases. Prefer the spelling of the 3.9.68 data. If the club is in none of them, write its common English name and `not in data` in `notes`. Empty when the record does not use the place.
   - The tier, not the brand: when a competition changed its name or sponsor (for example English First Division, from 2004/05 the Championship), research the competition at the same level and give the real name in `notes`. When a league was split, merged or replaced, say what the data's competition maps to in `notes`, and mark it `not held` if nothing maps.
   - `sources`: `field=URL ; field=URL` or `all=URL`. Every filled fact needs a page that you read on the day. Read raw pages with `curl -s -A "Mozilla/5.0" <url>` and grep them: the WebFetch summary can get names and numbers wrong.
2. `<part>.md`: the conventions with proof, points of doubt, every database row in `db-rows.tsv` that disagrees with your result (database, competition, label, what it holds, what is real), and names that you could not match.

Check pass (a second agent): `<part>.check.tsv` with columns `competition  label  part  field  value  check_url  result  note`, one line per filled field (`first`, `second`, `third`, `host`, and `held` for `not held` rows), result `agree`, `disagree` or `no second source`, against a page of another publisher than the first source. Wikipedia in another language is the same publisher. RSSSF, worldfootball.net, the governing body's own site, soccerway and national newspapers are good second sources. Then `<part>.check.md` with the count of each result and every disagreement.

## Rules

1. Never name another modder, their patch or tool, or a forum poster, in any file. Name a database by month and year only. Do not open or name the source zip or folder names of the databases.
2. Never put an email address or personal data in a request header or URL.
3. Use your own temporary folder from `mktemp -d` in the system temp folder. Never use `/tmp` itself or the home folder. Never run `rm` with a wildcard or a path in a variable.
4. Write only your own part's files. Do not edit any other file in the repo. Do not commit.
5. Write "unknown" rather than guess. Mark inferences as inferences.
6. Plain English, British spelling, in the `.md` files.
7. Report: at most 200 words: rows written, `unknown` rows, conventions found, disagreements with the databases, and your files.
