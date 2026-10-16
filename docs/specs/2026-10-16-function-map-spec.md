# Function map of the original
Status: Ready, except the route (see Open decisions)

Research: `docs/research/2026-10-16-function-map-estimate.md`, `-pilot-discipline.md` and `-pilot-club-screens.md`.

## Problem
The recompile runs every function of the original without a name. Only about 860 of the 6,609 game functions have a name or a description in a research doc or the runtime, all from feature work. Each new feature starts by reading unknown code to find the functions it needs. That reading is the slowest part of research, and agents repeat it.

## Solution
Each function of the original gets a name, a calling convention, a one-line purpose and a confidence, in one doc per source file. A table lists the source file of every function. The work runs on Max's Claude subscription with Claude Code agents. Scripts prepare the input, so the agents only read and name. It costs no extra money. It spreads over several usage windows and can stop and resume at any point.

## User stories
1. As an agent starting a feature, I want to look up which source file and which name a function has, so that I read only the functions that matter.
2. As an agent reading a function, I want the names of its callers and callees, so that I understand it without reading them all.
3. As Max, I want the map to cost no money beyond my subscription.
4. As Max, I want to see how much of my usage the map takes before I commit to the whole game.
5. As Max, I want the run to stop when my usage window runs out and to resume later without lost work.
6. As Max, I want to know how reliable the names are, so that agents treat a low-confidence name as a hint.
7. As an agent, I want names that earlier research proves to win over first-pass names.
8. As a maintainer, I want no code of the original in the repo: addresses, file names and descriptions only.
9. As a maintainer, I want the scripts in the repo, so that the map can be built again for another exe version.
10. As an agent, I want the per-nation files (leagues, cups, awards, rule books) named the same way as their template class.

## Decisions
- Placement (step 1 of the research): link order, assert path strings, vtables and the call graph place 82% of game functions in one file and the rest between two files. Exception funclets and handler stubs take the name of their parent function by script. The C runtime is named from a list.
- Input: one dossier per function, built by script from the Ghidra pseudocode (Ghidra 12.1.4 headless, Java from Homebrew openjdk@21), with strings, assert lines, globals, callers and callees with their files, and pointers from data. The pseudocode and dossiers stay outside the repo because they are derived code of the original.
- Output: one row per function: name, file, convention, purpose (at most 25 words), evidence, confidence (high, medium, low). The fields are those of pilot 2.
- Model by size (Max approved): up to 20 instructions Haiku, up to 150 Sonnet, larger Opus. On the subscription route these are the `model` values of the agents.
- Agents get batches. One agent names about 25 functions of one source file from their dossiers, so the agent's fixed start cost is paid once per batch. It writes its rows to a results file and reads nothing else.
- Waves: wave 1 names every function. Wave 2 names the low-confidence functions again, with the wave-1 names of the whole game in their dossiers.
- Names from earlier research win. A script marks every function that a research doc or the runtime names. The doc shows both names when they differ.
- Progress lives in one state file outside the repo: which batches are done. A session reads it, runs the next batches, and stops cleanly when the usage window runs out.
- Docs: one table per source file under `docs/research/function-map/`, with an index by address. Each doc states that the names are first-pass names with a confidence.
- Repo: the scripts go under `tools/research/function_map/`. The file table and the docs go under `docs/research/function-map/`. Never the pseudocode, the dossiers or the exe.

## Testing
No test suite covers research. Each slice has a manual acceptance check. The quality check is the same in every slice: compare the names of proven functions with earlier research. Pilot 1 (6 checks) and pilot 2 (8 checks) are the existing checks. The list grows to 20 proven functions from different files in slice 2.

## Out of scope
- Renaming the `f_xxxxxxxx` functions in the generated C. The build stays as it is.
- Types and structure layouts beyond what a dossier shows.
- Functions of the 3.9.60 and 3.9.65 data sets: the exe is the same 3.9.68 file.

## Open decisions
1. Route. Subscription (this spec), API with a cheap model (Haiku only about $5, Sonnet only about $50, mixed $120), or a mix. The Haiku-only and Sonnet-only tests of club_screens.cpp decide whether a cheap API route is good enough. Recommendation: wait for that result. If Haiku is good enough, spend $5 and skip slices 2 to 4. If not, take this spec.

## Slices
### 1. Scripts and file table in the repo
Blocked by: none
Delivers: `tools/research/function_map/` holds the placement script, the Ghidra export script, the dossier builder and the doc writer. `docs/research/function-map/function-files.tsv` lists all 11,747 functions with category, file or two candidate files, and method. A README says how to rebuild them.
Merge danger: two-way door. Blast radius: research files only, the build does not change.
- [ ] The placement script rebuilds the table from the exe and the result matches the table of the research.
- [ ] No file holds code or pseudocode of the original.
- [ ] Review by one fresh Opus agent against this spec and the project rules.

### 2. Subscription pilot and usage measure
Blocked by: 1
Delivers: the agent flow on two files, discipline.cpp and comp_screens.cpp: batches of about 25 functions, model by size, results files, state file, resume after a stop. A note of how much of one 5-hour usage window the two files took, read from the app's usage page before and after.
Merge danger: two-way door. Blast radius: none in the repo beyond the two docs.
- [ ] Both docs exist in the pilot 2 format.
- [ ] At least 5 of the 6 discipline checks agree, and the 20-function check list is written.
- [ ] A stop in the middle of a file and a resume lose no rows and do not name a function twice.
- [ ] Max reads the usage note and decides whether to go on.

### 3. Wave 1 over the whole game
Blocked by: 2, and Max's go after slice 2
Delivers: names for all 6,863 game and library functions. Several sessions, one per usage window, each running batches from the state file until the window runs out.
Merge danger: two-way door. Blast radius: none until the docs are written.
- [ ] The state file shows every batch done.
- [ ] Every function has a row, with no missing fields.

### 4. Wave 2 on the low-confidence names
Blocked by: 3
Delivers: new dossiers for the low-confidence functions, with the wave-1 names of the whole game next to every caller and callee, named again by Opus.
Merge danger: two-way door. Blast radius: none until the docs are written.
- [ ] The share of low-confidence names falls, and the result is in the state file.
- [ ] The 20 checks are run again and the result is recorded.

### 5. Docs, scripts for the rest, and the index
Blocked by: 4 (or 3, if Max skips wave 2)
Delivers: funclets, handler stubs and C runtime named by script. One doc per source file and an index by address under `docs/research/function-map/`. Names from earlier research shown next to the first-pass names. A pointer line in `docs/HANDOVER.md`.
Merge danger: two-way door. Blast radius: agents may trust a wrong name. The confidence column and the check result at the top of the index limit that.
- [ ] Every one of the 11,747 functions appears once in the index.
- [ ] The 20 checks and their result are at the top of the index.
- [ ] Review by one fresh Opus agent: a sample of 30 rows against the dossiers.
- [ ] Max approves the commit.
