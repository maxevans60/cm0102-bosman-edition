"""Names the overrides in the per-nation competition classes after the base method of their vtable slot.

Every national league and cup derives from League (league.cpp) or Cup (Cup.cpp). A derived vtable keeps
the base layout: a slot that holds a function other than the base one is an override of the base method.
The sweeps name the base methods with Opus. This script gives each override `<file stem>::<method>`,
for example `eng_prm::create_schedule`, with model "slot copy" and confidence medium.

A vtable derives from the base whose league.cpp or Cup.cpp entries it shares most (at least MIN_SHARED).
A function that two vtables put at slots of different base methods is left out. A function that a sweep
names is left out: the sweep row wins.

Usage: CM0102_EXE=... FUNCMAP_WORK=... slot_copy.py   (after the league, cup and hub sweeps)
Writes $FUNCMAP_WORK/runs/slot-copy/names.jsonl.
"""
import collections
import glob
import json
import os

from common import image, load, work_dir
from placement import find_vtables

BASES = {"League": (0x96d020, 45, "league.cpp"), "Cup": (0x9693c8, 40, "Cup.cpp")}
MIN_SHARED = 5


def main():
    img = image()
    P = load("placement.pkl")
    db, cat, fo, gaps = P["db"], P["cat"], P["file_of"], P["gaps"]
    work = work_dir()
    swept = {}
    for p in sorted(glob.glob(f"{work}/runs/sweep-*/names.jsonl")):
        for line in open(p):
            r = json.loads(line)
            swept[int(r["address"], 16)] = r
    vtables = find_vtables(img, db, sorted(db))
    base = {cls: vtables[st][:n] for cls, (st, n, _) in BASES.items()}
    base_vt = {st for st, _, _ in BASES.values()}
    own = {cls: {i for i, x in enumerate(ents) if fo.get(x) == BASES[cls][2]} for cls, ents in base.items()}

    slots = collections.defaultdict(set)      # function -> {(class, slot, base function, vtable)}
    for st, ents in vtables.items():
        if st in base_vt:
            continue
        shared = {cls: sum(1 for i in own[cls] if i < len(ents) and ents[i] == base[cls][i]) for cls in base}
        cls = max(shared, key=shared.get)
        if shared[cls] < MIN_SHARED:
            continue
        for i, x in enumerate(ents[:len(base[cls])]):
            b = base[cls][i]
            if x != b and i in own[cls] and b in swept and cat.get(x) == "game" \
                    and fo.get(x) not in ("league.cpp", "Cup.cpp"):
                slots[x].add((cls, i, b, st))

    rows, skipped = [], collections.Counter()
    for x in sorted(slots):
        methods = {(cls, swept[b]["name"].split("::")[-1]) for cls, i, b, st in slots[x]}
        if x in swept:
            skipped["swept"] += 1
            continue
        if len(methods) != 1:
            skipped["conflict"] += 1
            continue
        (cls, method), = methods
        cls_, i, b, st = min(slots[x])
        f = fo.get(x)
        if f is None:
            vt_files = collections.Counter(fo.get(y) for y in vtables[st] if fo.get(y) and fo.get(y) in gaps.get(x, ()))
            f = vt_files.most_common(1)[0][0] if vt_files else gaps[x][0]
        stem = os.path.splitext(f)[0]
        r = swept[b]
        rows.append({"address": f"0x{x:06x}", "name": f"{stem}::{method}", "file": f,
                     "convention": r.get("convention", ""),
                     "purpose": f"Override of {cls}::{method} for the competition of {f}.",
                     "evidence": f"Slot {i} of vtable 0x{st:06x} ({len(slots[x])} vtable(s)), where {cls} 0x{BASES[cls][0]:06x} "
                                 f"holds 0x{b:06x} {r['name']}.",
                     "confidence": "medium", "model": "slot copy"})
    os.makedirs(f"{work}/runs/slot-copy", exist_ok=True)
    with open(f"{work}/runs/slot-copy/names.jsonl", "w") as out:
        for r in rows:
            out.write(json.dumps(r) + "\n")
    print(len(rows), "slot copies,", dict(skipped), "skipped,",
          dict(collections.Counter(r["name"].split("::")[1] for r in rows).most_common(8)))


if __name__ == "__main__":
    main()
