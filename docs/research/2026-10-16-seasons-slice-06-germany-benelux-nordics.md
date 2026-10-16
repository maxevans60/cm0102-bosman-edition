# Rules table, slice 6: Germany, Holland, Belgium, Denmark, Sweden, Norway and Finland

Date 2026-10-16. Slice 6 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Brief: `research-brief-0.6.0.md`. The rows cover the seasons 2001/02 to 2027/28.

## Files

Each part has its rules table (`<part>.tsv`), its notes (`<part>.md`, with the sections Reconcile, Loose ends and Source fill), the second-source check (`<part>.check.tsv`, `<part>.check.md`) and the reconcile decisions (`<part>.reconcile.tsv`). Parts with club moves also have `<part>-clubs.tsv`. All files are in `rules-0.6.0/`. Changes of the consistency pass are in `rules-0.6.0/consistency.md`.

## Counts

Fields are filled fact fields of the rules table. "No second source" means that the check agent found no page of another publisher for that field. "Unknown" means that no page gave the fact. "Without a source" counts filled fields whose row cites no page for them (`tools/research/rules_lint.py`).

| Part | Rows | Agree | Disagree | Found | No second source | Reconcile first / second / conflict | Unknown fields | Without a source | Club rows | Clubs not in the database |
|---|---|---|---|---|---|---|---|---|---|---|
| [ger](rules-0.6.0/ger.md) | 351 | 1251 | 14 | 2 | 426 | 5 / 6 / 3 | 314 | 0 | 1925 | 51 |
| [hol-bel](rules-0.6.0/hol-bel.md) | 351 | 949 | 14 | 12 | 971 | 2 / 10 / 2 | 505 | 0 | 2158 | 31 |
| [den-swe](rules-0.6.0/den-swe.md) | 324 | 898 | 15 | 27 | 840 | 15 / 25 / 2 | 383 | 0 | 1602 | 72 |
| [nor-fin](rules-0.6.0/nor-fin.md) | 270 | 887 | 19 | 35 | 558 | 0 / 16 / 3 | 289 | 0 | 644 | 0 |

## Method

1. An Opus agent wrote the rows of each part from pages read on the day.
2. A second Opus agent checked every filled field against a page of another publisher.
3. A third Opus agent read a third source for each disagreement, wrote the settled values into the table and listed the conflicts.
4. Loose-ends, consistency and source-fill agents closed the points the earlier agents found, made `up`/`down` follow the rule places, and added a source for each filled field.

The conflicts and decisions for Max are in `2026-10-16-seasons-research-for-max.md`.
