# Function map scripts

These scripts build `docs/research/function-map/`: the source file and a first-pass name of every function of the original. The spec is `docs/specs/2026-10-16-function-map-spec.md`.

Everything derived from the original (decoded functions, Ghidra pseudocode, dossiers, model answers) goes to the work folder in `FUNCMAP_WORK`. It must be outside the repo, and the scripts refuse a folder inside it. The repo gets addresses, file names and the model's names and descriptions only.

## Steps

Set `CM0102_EXE` to the clean 3.9.68 exe and `FUNCMAP_WORK` to a folder outside the repo. Use the cm0102 venv. A configured build is needed: `placement.py` reads `build/recomp/gen` for the functions that `recomp.py` finds through data.

1. `placement.py` writes `function-files.tsv` (about 40 seconds).
2. `ghidra_export.sh` decompiles the 6,863 game and library functions once (about 40 minutes). It needs Ghidra and Java 21 (`brew install ghidra openjdk@21`).
3. `dossier.py --all` writes one dossier per function and one summary per source file (about 30 seconds).
4. `name_batch.py submit <run> --model haiku`, then `name_batch.py collect <run>`. This needs `ANTHROPIC_API_KEY` and the `anthropic` package. The whole game on Haiku costs about $2.50 at batch prices. `--model size` uses Haiku, Sonnet and Opus by function size (about $85). `--only addresses.txt` names only the listed functions.
5. `write_docs.py <run> [<run> ...]` writes the docs. A later run wins for the same address.

`slot_copy.py` names the overrides of the per-nation competition classes after the base method of their vtable slot in league.cpp or Cup.cpp, as `<file stem>::<method>` (run `slot-copy`, model "slot copy", confidence medium). Run it after the league, cup and hub sweeps. It leaves out every function that a sweep names.

The current order of runs: `write_docs.py haiku-2026-10-16 haiku-2 slot-copy sweep-league sweep-cup sweep-hubs`.

## Sweep of one file

A sweep replaces the first-pass names of one source file with better ones. It runs on the Claude subscription, inside a Claude Code session:

1. `dossier.py <file.cpp> --names <work>/runs/<first run>/names.jsonl [--names <later run>/names.jsonl ...]`. Callers and callees then show earlier names. A later `--names` wins for the same address.
2. An Opus agent reads the dossiers of that file in `$FUNCMAP_WORK/dossiers` and writes one JSON line a function to `$FUNCMAP_WORK/runs/sweep-<file>/names.jsonl`. It uses the same fields as `name_batch.py`: address, name, file, convention, purpose, evidence, confidence, model.
3. `write_docs.py <first run> sweep-<file> ...`, then commit the docs.
