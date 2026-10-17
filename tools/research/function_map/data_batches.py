"""Cuts the globals that are not high into naming batches, one or more address groups each.

A batch holds about TARGET globals to name. A group larger than MAX is cut into runs of adjacent
globals. Each batch gets a list file (one address a line) and one input file with the group context
(every member of its groups with its current name) and the dossier of each global to name. Waves of
WAVE batches take batches spread over the address range, so each wave samples the whole data.

Usage: FUNCMAP_WORK=... data_batches.py <prefix> <run> [<run> ...]
Writes $FUNCMAP_WORK/sweep/<prefix>-wNN-aX (lists), data-dossiers/batches/<batch>.txt and
sweep/<prefix>-batches.tsv (batch, wave, globals).
"""
import json
import os
import string
import sys

from common import load, work_dir

TARGET, MAX, WAVE = 70, 80, 8
CONTEXT = 4               # group members shown before and after each global of a batch


def main(prefix, runs):
    W = work_dir()
    D = load("globals.pkl")
    G, groups = D["G"], D["groups"]
    names = {}
    for run in runs:
        for line in open(f"{W}/runs-data/{run}/names.jsonl"):
            r = json.loads(line)
            names[int(r["address"], 16)] = r
    todo = lambda a: names.get(a, {}).get("confidence") != "high"
    chunks = []
    for grp in groups:
        open_ = [a for a in grp if todo(a)]
        while len(open_) > MAX:
            chunks.append((grp, open_[:TARGET]))
            open_ = open_[TARGET:]
        if open_:
            chunks.append((grp, open_))
    batches, cur = [], []
    for grp, items in chunks:
        if cur and sum(len(i) for _, i in cur) + len(items) > MAX:
            batches.append(cur)
            cur = []
        cur.append((grp, items))
    if cur:
        batches.append(cur)
    waves = (len(batches) + WAVE - 1) // WAVE
    out = os.path.join(W, "data-dossiers", "batches")
    os.makedirs(out, exist_ok=True)
    plan = ["batch\twave\tglobals\tfirst\tlast"]
    for i, b in enumerate(batches):
        wave, slot = i % waves + 1, i // waves
        name = f"{prefix}-w{wave:02d}-a{string.ascii_lowercase[slot]}"
        items = [a for _, its in b for a in its]
        open(f"{W}/sweep/{name}", "w").write("".join(f"0x{a:06x}\n" for a in items))
        text = [f"# Batch {name}: {len(items)} globals to name in {len(b)} address group parts", ""]
        for grp, its in b:
            text.append(f"## Address group 0x{grp[0]:06x} to 0x{grp[-1]:06x} ({len(grp)} globals)")
            keep = set()
            for a in its:
                i = grp.index(a)
                keep.update(range(max(0, i - CONTEXT), min(len(grp), i + CONTEXT + 1)))
            last = -1
            for i in sorted(keep):
                if i != last + 1:
                    text.append(f"  ... {i - last - 1} globals")
                a, last = grp[i], i
                r = names.get(a)
                mark = "NAME THIS" if a in its else ("named, high" if r and r["confidence"] == "high" else "other batch")
                cur_name = r["name"] if r else f"g_unk_{a:06x}"
                text.append(f"  0x{a:06x} +0x{a - grp[0]:x}  {cur_name}  [{mark}]")
            if last != len(grp) - 1:
                text.append(f"  ... {len(grp) - 1 - last} globals")
            text.append("")
        for a in items:
            r = names.get(a)
            if r:
                text.append(f"Script row for 0x{a:06x}: {r['name']} ({r['confidence']}): {r['evidence']}")
            text.append(open(f"{W}/data-dossiers/{a:06x}.txt").read())
        open(f"{out}/{name}.txt", "w").write("\n".join(text))
        plan.append(f"{name}\t{wave}\t{len(items)}\t0x{items[0]:06x}\t0x{items[-1]:06x}")
    open(f"{W}/sweep/{prefix}-batches.tsv", "w").write("\n".join(plan) + "\n")
    print(len(batches), "batches in", waves, "waves,", sum(1 for a in G if todo(a)), "globals to name")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2:])
