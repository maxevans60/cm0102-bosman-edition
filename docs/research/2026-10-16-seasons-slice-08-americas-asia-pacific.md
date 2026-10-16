# Rules table, slice 8: Argentina, Brazil, the United States, Australia, Japan and South Korea

Date 2026-10-16. Slice 8 of `docs/specs/2026-10-16-seasons-0.6.0-spec.md`. Brief: `research-brief-0.6.0.md`. The rows cover the seasons 2001/02 to 2027/28.

## Files

Each part has its rules table (`<part>.tsv`), its notes (`<part>.md`, with the sections Reconcile, Loose ends and Source fill), the second-source check (`<part>.check.tsv`, `<part>.check.md`) and the reconcile decisions (`<part>.reconcile.tsv`). Parts with club moves also have `<part>-clubs.tsv`. All files are in `rules-0.6.0/`. Changes of the consistency pass are in `rules-0.6.0/consistency.md`.

## Counts

Fields are filled fact fields of the rules table. "No second source" means that the check agent found no page of another publisher for that field. "Unknown" means that no page gave the fact. "Without a source" counts filled fields whose row cites no page for them (`tools/research/rules_lint.py`).

| Part | Rows | Agree | Disagree | Found | No second source | Reconcile first / second / conflict | Unknown fields | Without a source | Club rows | Clubs not in the database |
|---|---|---|---|---|---|---|---|---|---|---|
| [bra](rules-0.6.0/bra.md) | 756 | 2145 | 39 | 111 | 1541 | 6 / 33 / 1 | 848 | 162 | 2689 | 258 |
| [arg-usa](rules-0.6.0/arg-usa.md) | 378 | 1065 | 7 | 43 | 607 | 0 / 7 / 0 | 504 | 0 | 1316 | 45 |
| [aus-jap-kor](rules-0.6.0/aus-jap-kor.md) | 459 | 1181 | 16 | 0 | 880 | 6 / 17 / 1 | 566 | 0 | 1656 | 39 |

## Method

1. An Opus agent wrote the rows of each part from pages read on the day.
2. A second Opus agent checked every filled field against a page of another publisher.
3. A third Opus agent read a third source for each disagreement, wrote the settled values into the table and listed the conflicts.
4. Loose-ends, consistency and source-fill agents closed the points the earlier agents found, made `up`/`down` follow the rule places, and added a source for each filled field.

The conflicts and decisions for Max are in `2026-10-16-seasons-research-for-max.md`.
