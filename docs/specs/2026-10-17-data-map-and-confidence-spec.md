# Data map and confidence pass
Status: In build

Research: `docs/research/2026-10-16-function-map-estimate.md` (every sweep and review wave of the function map). Earlier spec: `docs/specs/2026-10-16-function-map-spec.md`.

## Problem
The function map gives every one of the 6,863 functions a name, but 1,406 names are not high: 1,378 medium and 28 low, in 176 files. A fresh Opus reviewer already checked each of these rows with its dossier and found no quote. A second pass with the same dossiers will mostly fail again, so these rows need new evidence.

The globals of the original have no names. 3,431 globals appear in the function dossiers, and 940 of the 1,406 rows read at least one of them. An agent that reads a function sees only an address where a name could pin the purpose.

## Solution
First a data map: every global in scope gets a name, a size, a type, its readers and writers and a confidence, in one page per owner file, like the function map. Scripts name the globals that a fixed rule pins. Sonnet agents name the rest, one address group at a time, and an Opus check of a sample decides whether Sonnet is good enough.

Then a confidence pass: the function dossiers are rebuilt with the global names and every reviewed function name. Opus agents check only the rows whose dossier has new evidence. A row reaches high only with a quote. A row that cannot reach high keeps its confidence with one line on what is missing, and Max decides on all such rows in one list at the end.

## User stories
1. As an agent reading a function, I want the names of the globals it reads and writes, so that I understand it without reading every other function that touches them.
2. As an agent starting a feature, I want to look up a global by address and see its readers and writers, so that I find the code that owns a value.
3. As an agent, I want to find a file's globals next to its functions, so that I read one place for one source file.
4. As Max, I want every name of the function map to be high, or to have a reason that I accepted.
5. As Max, I want the rule for high to stay strict, so that a high name is one that agents can build on.
6. As Max, I want the work to cost as little of my subscription as the quality allows.
7. As an agent, I want a high function name that rests on a global to rest on a high global, so that a weak name does not raise another name.
8. As an agent, I want the library rows matched with known open source code where they match, so that their names come from the source.
9. As a maintainer, I want no code of the original and no code of any open source project in the repo.
10. As a maintainer, I want the data map scripts in the repo, so that the map can be built again for another exe version.
11. As an agent, I want the four file questions of the review pass to have one answer, so that the docs do not contradict the asserts.
12. As Max, I want a global that a reader in one file and readers in many other files share to have one place in the docs.
13. As Max, I want to know after the first wave whether Sonnet names globals well enough, before the rest of the waves run.

## Decisions
Settled with Max on 2026-10-17 (grilling, Q1 to Q23).

Order and scope
- The data map comes first, then the confidence pass (Q1).
- The data map covers every global that a game or library function touches (Q7). The interview counted 3,431 from the "Globals referenced" lines of the function dossiers. Those lines stop at 80 globals per function, so the full count is 4,428 (slice 1). Scripts name the globals that only library code touches.
- The runtime trace is out of this spec. Max decides on it after the confidence pass, for the rows that are still medium (Q4).

Data map
- A global dossier is short: the names of its readers and writers, the access size and offset, and the code lines that touch the global. It has no full pseudocode (Q11).
- Scripts name the CRT and library globals, the key id table, string pointers and vtables.
- Sonnet agents name the other globals. One batch is one address group or several, with about 60 to 80 globals (Q12, Q13).
- After the first wave, one Opus agent checks 60 random Sonnet names. If more than 10% are wrong, the rest of the waves run on Opus (Q19).
- A global is high only when a quote pins its purpose: a string it points to or is compared with, an assert, or a high reader or writer whose name pins it (Q16).
- Names are snake_case with the prefix `g_`, for example `g_player_stats`. Fields of an address group are described as `g_<object>+0x<offset> <field>` (Q17).
- A global that no wave has named yet shows as `g_unk_<addr>` with low confidence and the model `script`.
- `globals.tsv` has one row for each address with these columns: address, size, type, name, readers, writers, confidence, model, cited in (Q8).
- The docs have one page for each owner file, `shared.md` for globals with readers in many files, and one index by address (Q18).
- Each data map row links to the function map pages of its readers and writers. The function map does not link back, so its doc writer does not change (Q21).

Confidence pass
- A script rebuilds the function dossiers with all runs and the global names. It sends a row to an agent only when its dossier has new evidence since its review: a high global, or a caller or callee with a new or raised name (Q14).
- Opus agents check about 25 rows each. The 93 library rows also go to Opus (Q6).
- The quotes for high are a string, an assert, a caller, a vtable slot, a high global that pins the purpose (Q2) and an open source match. An open source match cites the project and the function by name. No code of that project goes anywhere (Q3).
- The agent that quotes a global for high also checks the evidence of that global. Each global that raises a row thus gets an Opus check (Q13).
- Each new name goes to a third check, as in the review pass. A raise of confidence that keeps the name needs no third check, because the brief makes the agent check that its quote exists (Q20).
- A row that cannot reach high keeps its confidence with one line on what is missing. Max decides on all such rows in one list at the end (Q5).

File questions (Q9)
- The Finance code (assert finance.CPP), the Match_Stats block (assert match_stats.CPP), the six `FILE_LLIST` rows and the network.h rows stay under the file that the placement gives. Each row gets a note that names the file of the assert.

Work rules
- Waves of up to 8 agents. At the usage limit, finish the current wave, update the handover and stop (Q10, Q15).
- One brief for all agents of a step, written before the first agent starts.
- Agents run on Max's subscription in a Claude Code session, never through the API.
- The data map scripts go in the function map scripts folder and are committed (Q22). The briefs, the batch lists, the dossiers and the model output stay in the work folder.
- Commit on main after each wave. Do not push.
- Commit only the two map folders, the data map scripts, the estimate doc, this spec and the function map paragraph of the handover. Other sessions leave other research files uncommitted.

## Testing
Research has no test suite. The one seam is a check script that runs after each wave (Q23). It fails when:
- a global in scope has no row, or more than one row
- a column holds a value outside its set
- a link does not resolve
- a high row names no quote, or quotes a function or global that does not exist, or quotes a global that is not high
- a committed file holds pseudocode or code of the original

Each slice also has a manual acceptance check below. The checks of the function map are the model: the cited-function check of each sweep wave and the gate of the review pass (`sweep/review.py` in the work folder).

## Out of scope
- The runtime trace. The place to start later is `tools/research/trace_game.py`, which runs the database load and game creation of the original in Unicorn and records the functions that run.
- Renaming functions or globals in the generated C. The build does not change.
- Struct layouts beyond what the address groups and the access offsets show.
- Globals that no function dossier lists, for example data that code reaches only through a pointer.
- The `news_*_text` rows of manager_manager.cpp that mix free and member names, unless the confidence pass reaches them through new evidence.

## Open decisions

## Slices
### 1. Global dossiers, scripted names and docs
Blocked by: none
Delivers: a short dossier for each global in scope (4,428), the address groups, the owner files, script names for the CRT and library globals, the key id table, string pointers and vtables, `globals.tsv`, one page per owner file, `shared.md`, the index and the check script. The Sonnet brief and the batch lists are ready in the work folder.
Merge danger: two-way door. Blast radius: research docs only. A wrong script name can mislead an agent.
- [x] `globals.tsv` has exactly one row for each of the 4,428 globals.
- [x] Each script name is high only with its quote, and a sample of 20 script names agrees with the disassembly.
- [x] The check script passes, and it fails on a planted bad row of each kind.
- [x] No committed file holds a dossier line, pseudocode or code of the original.

Done (2026-10-17): 2,289 script names (2,148 high: key ids, read-only float constants, vtables), 2,280 globals left in 31 batches over 4 waves. The estimate doc records the counts under "Data map".

### 2. Data map wave 1 and the Sonnet gate
Blocked by: 1
Delivers: the first wave of up to 8 Sonnet agents, and one Opus check of 60 random names from that wave. The estimate doc records the wave, its tokens and the gate result.
Merge danger: two-way door. Blast radius: the names of one wave.
- [x] Every global of the wave's batches has a name.
- [x] The gate result is recorded. Above 10% wrong, the remaining waves are set to Opus in the brief.
- [x] The check script passes.

Done (2026-10-17): 600 names (435 high), 1 of 60 sample rows wrong (1.7%), so the rest stays on Sonnet. The string rule now names short and accented text, and 813 globals are left in 12 batches.

### 3. Remaining data map waves (repeat per wave)
Blocked by: 2
Delivers: names for every global in scope, one wave per run, on Sonnet or Opus as the gate decided.
Merge danger: two-way door. Blast radius: the names of one wave.
- [x] No row is `g_unk_<addr>` after the last wave.
- [x] Each wave is in the estimate doc with its tokens.
- [x] The check script passes after each wave.

Done (2026-10-17): waves data-w02 and data-w03 named the last 813 globals. The data map has 3,808 high, 574 medium and 46 low names.

### 4. Confidence pass setup
Blocked by: 3
Delivers: the function dossiers rebuilt with all runs and the global names, the new evidence filter with the count of rows it sends to agents, the Opus brief with the open source rule and the global check, the batch lists and the notes for the four file questions.
Merge danger: two-way door. Blast radius: the scope of the pass. A filter that is too strict leaves rows without a check.
- [x] Each of the 1,406 rows is either in a batch or on the list for Max with "no new evidence".
- [x] A sample of 10 rows on the list for Max has no new evidence in its rebuilt dossier.
- [x] The rows of the four file questions carry their notes in the function map.

Done (2026-10-17): 956 rows in 38 batches over 5 waves, 450 rows on the list for Max. Float and double constants do not count as new evidence. A global that a check finds wrong drops to medium with every high row that quotes it. 99 rows carry a file note from `function-map/file-notes.tsv`. The plan and the brief are `sweep/CONF-PLAN.md` and `sweep/BRIEF-confidence.md` in the work folder.

### 5. Confidence pass waves (repeat per wave)
Blocked by: 4
Delivers: Opus checks of the rows in scope, one wave per run, with a third check for each new name.
Merge danger: two-way door. Blast radius: the names of one wave.
- [ ] Each row of the wave is high with a quote, or keeps its confidence with one line on what is missing.
- [ ] Each new name has a third check.
- [ ] Each wave is in the estimate doc. The check script passes.

### 6. Close
Blocked by: 5
Delivers: the list for Max of every row that is not high, each with its reason, the end result in the estimate doc, the function map paragraph of the handover, the updated `HANDOVER-next.md` and this spec marked shipped.
Merge danger: two-way door. Blast radius: docs only.
- [ ] Max has decided on each row of the list.
- [ ] The handover describes both maps.
- [ ] The check script passes on the final state.
