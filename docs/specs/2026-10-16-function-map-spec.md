# Function map of the original
Status: In build

Research: `docs/research/2026-10-16-function-map-estimate.md`, `-pilot-discipline.md` and `-pilot-club-screens.md`.

## Problem
The recompile runs every function of the original without a name. Only about 860 of the 6,609 game functions have a name or a description in a research doc or the runtime, all from feature work. Each new feature starts by reading unknown code to find the functions it needs. That reading is the slowest part of research, and agents repeat it.

## Solution
Each function of the original gets a name, a calling convention, a one-line purpose and a confidence, in one doc per source file. A table lists the source file of every function. A first pass names the whole game with Haiku through the Message Batches API for about $2.50 (Max, 2026-10-16). A sweep later names one source file again with an Opus agent on Max's Claude subscription, when a feature or Max asks for that file. Scripts prepare the input, so the models only read and name.

## User stories
1. As an agent starting a feature, I want to look up which source file and which name a function has, so that I read only the functions that matter.
2. As an agent reading a function, I want the names of its callers and callees, so that I understand it without reading them all.
3. As Max, I want the map to cost a few dollars at most. The sweeps run on my subscription.
4. As an agent starting a feature, I want to sweep the files I need, so that their names are reliable before I build on them.
5. As Max, I want to know how reliable the names are, so that agents treat a low-confidence name as a hint.
6. As an agent, I want names that earlier research proves to win over first-pass names.
7. As a maintainer, I want no code of the original in the repo: addresses, file names and descriptions only.
8. As a maintainer, I want the scripts in the repo, so that the map can be built again for another exe version.
9. As an agent, I want the per-nation files (leagues, cups, awards, rule books) named the same way as their template class.

## Decisions
- Placement (step 1 of the research): link order, assert path strings, vtables and the call graph place 82% of game functions in one file and the rest between two files. Exception funclets and handler stubs take the name of their parent function by script. The C runtime is named from a list.
- Input: one dossier per function, built by script from the Ghidra pseudocode (Ghidra 12.1.4 headless, Java from Homebrew openjdk@21), with strings, assert lines, globals, callers and callees with their files, and pointers from data. The pseudocode and dossiers stay outside the repo because they are derived code of the original.
- Output: one row per function: name, file, convention, purpose (at most 25 words), evidence, confidence (high, medium, low). The fields are those of pilot 2.
- Route (Max, 2026-10-16): first pass with Haiku for every function, one batch request each, about $2.50 for the game. Pilot 2 measured Haiku at 5 of 8 checks right, with its wrong names at medium confidence, so every first-pass name is marked with its model.
- Sweep: one source file at a time, by an Opus agent on the subscription, with the first-pass names of callers and callees in the dossiers. A sweep row wins over the first-pass row.
- No second wave over the whole game and no Sonnet or Opus batch: Max does not want to spend $45 to $120.
- Names from earlier research win. A script marks every function that a research doc or the runtime names. The doc shows both names when they differ.
- Docs: one table per source file under `docs/research/function-map/`, with an index by address. Each doc states that the names are first-pass names with a confidence.
- Repo: the scripts go under `tools/research/function_map/`. The file table and the docs go under `docs/research/function-map/`. Never the pseudocode, the dossiers or the exe.

## Testing
No test suite covers research. Each slice has a manual acceptance check. The quality check is the same in every slice: compare the names of proven functions with earlier research. Pilot 1 (6 checks) and pilot 2 (8 checks) are the existing checks. The first pass is checked against the 14 proven functions of the pilots. Each sweep checks the proven functions of its file.

## Out of scope
- Renaming the `f_xxxxxxxx` functions in the generated C. The build stays as it is.
- Types and structure layouts beyond what a dossier shows.
- Functions of the 3.9.60 and 3.9.65 data sets: the exe is the same 3.9.68 file.

## Open decisions

## Slices
### 1. Scripts and file table in the repo
Blocked by: none
Delivers: `tools/research/function_map/` with the placement script, the Ghidra export, the dossier builder, the batch runner, the doc writer and a README. `docs/research/function-map/function-files.tsv` lists all 11,747 functions.
Merge danger: two-way door. Blast radius: research files only, the build does not change.
- [x] The placement script rebuilds the table of the research exactly.
- [x] The dossier builder rebuilds the pilot dossiers exactly.
- [x] No file holds code or pseudocode of the original. The scripts refuse a work folder inside the repo.

### 2. Haiku first pass and the docs
Blocked by: 1
Delivers: first-pass names for all 6,863 game and library functions, one doc per source file, `names.tsv` and an index.
Merge danger: two-way door. Blast radius: agents may trust a wrong name. The model and confidence columns and the "Cited in" column limit that.
- [ ] Every game and library function has a row.
- [ ] The 14 proven functions of the pilots are checked and the result is in the index.
- [ ] Cost and time recorded in the estimate doc.

### 3. Sweep of one source file (repeat per file, on demand)
Blocked by: 2
Delivers: Opus names for one source file on the subscription, following the README.
Merge danger: two-way door. Blast radius: one file's names.
- [ ] Every function of the file has a sweep row.
- [ ] The proven functions of the file agree, or the doc says why not.
