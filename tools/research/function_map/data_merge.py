"""Merges the part files of one data naming wave into the wave's names.jsonl, and picks a sample.

Each part must hold one row for each address of its batch list and no others, with every field.
A name that an earlier run or another row of the wave already uses gets the address added, and
the merge reports it. With --sample N, it writes N random rows to sample-input.jsonl for the check.

Usage: FUNCMAP_WORK=... data_merge.py <run> <earlier run> [<earlier run> ...] [--sample N]
Reads $FUNCMAP_WORK/runs-data/<run>/part-*.jsonl and sweep/<batch>. Writes runs-data/<run>/names.jsonl.
"""
import glob
import json
import os
import random
import re
import sys

from common import work_dir

FIELDS = ("address", "name", "type", "size", "description", "evidence", "confidence", "model")


def main(argv):
    sample = 0
    if "--sample" in argv:
        i = argv.index("--sample")
        sample = int(argv[i + 1])
        argv = argv[:i] + argv[i + 2:]
    run, earlier = argv[0], argv[1:]
    W = work_dir()
    taken = {}
    for r in earlier:
        for line in open(f"{W}/runs-data/{r}/names.jsonl"):
            x = json.loads(line)
            taken[x["name"]] = x["address"]
    rows, problems = [], []
    for part in sorted(glob.glob(f"{W}/runs-data/{run}/part-*.jsonl")):
        batch = os.path.basename(part)[5:-6]
        want = {l.strip() for l in open(f"{W}/sweep/{batch}") if l.strip()}
        got = set()
        for n, line in enumerate(open(part), 1):
            try:
                x = json.loads(line)
            except ValueError:
                problems.append(f"{batch}:{n}: does not parse")
                continue
            missing = [f for f in FIELDS if f not in x or x[f] in ("", None)]
            if missing:
                problems.append(f"{batch}: {x.get('address')}: missing {', '.join(missing)}")
                continue
            if not re.fullmatch(r"g_[a-z0-9_]+", x["name"]) or x["confidence"] not in ("high", "medium", "low"):
                problems.append(f"{batch}: {x['address']}: bad name or confidence")
                continue
            x["size"] = int(x["size"])
            got.add(x["address"])
            rows.append({f: x[f] for f in FIELDS})
        if got != want:
            problems.append(f"{batch}: {len(want - got)} addresses missing, {len(got - want)} extra")
    # Earlier script rows of these addresses do not block their own names.
    mine = {x["address"] for x in rows}
    taken = {n: a for n, a in taken.items() if a not in mine}
    for x in rows:
        if x["name"] in taken:
            problems.append(f"{x['address']}: name {x['name']} is taken by {taken[x['name']]}, address added")
            x["name"] = f"{x['name']}_{x['address'][2:]}"
        taken[x["name"]] = x["address"]
    with open(f"{W}/runs-data/{run}/names.jsonl", "w") as f:
        for x in sorted(rows, key=lambda r: r["address"]):
            f.write(json.dumps(x) + "\n")
    print("\n".join(problems))
    conf = {c: sum(x["confidence"] == c for x in rows) for c in ("high", "medium", "low")}
    print(len(rows), "rows", conf, len(problems), "problems")
    if sample:
        random.seed(run)
        with open(f"{W}/runs-data/{run}/sample-input.jsonl", "w") as f:
            for x in random.sample(rows, min(sample, len(rows))):
                f.write(json.dumps({"address": x["address"], "row": x}) + "\n")
        print("sample of", min(sample, len(rows)), "written")


if __name__ == "__main__":
    main(sys.argv[1:])
