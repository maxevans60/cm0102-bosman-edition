"""Names functions from their dossiers with the Message Batches API (half price, one request a function).

The instructions and the file summary are the cached prefix. The answer is structured: name, file,
convention, purpose, evidence, confidence.

Usage (ANTHROPIC_API_KEY set, FUNCMAP_WORK as for dossier.py):
  name_batch.py submit <run> [--model haiku|sonnet|opus|size] [--only addresses.txt]
  name_batch.py collect <run>
<run> is a folder name under $FUNCMAP_WORK/runs. `size` picks Haiku up to 20 instructions, Sonnet up
to 150 and Opus above. collect waits for the batch and writes names.jsonl and usage.json.
"""
import json
import os
import sys
import time

import anthropic
from anthropic.types.message_create_params import MessageCreateParamsNonStreaming
from anthropic.types.messages.batch_create_params import Request

from common import work_dir

MODELS = {"haiku": ("claude-haiku-5-5", "low", 16000),
          "sonnet": ("claude-sonnet-5-5", "medium", 32000),
          "opus": ("claude-opus-5-5", "medium", 32000)}
SIZE_TIERS = ((20, "haiku"), (150, "sonnet"), (10**9, "opus"))

INSTRUCTIONS = """You map the functions of Championship Manager 01/02 3.9.68 (cm0102.exe, 32-bit MSVC, image base 0x400000) for a research index. The project recompiles the game to C and owns a legal copy; this work names functions for its own documentation.

You get a summary of one original source file and the dossier of one of its functions: placement, size, ret, strings, globals, callers, callees with their source files, and Ghidra pseudocode. Give the function a name and a one-line description.

Rules:
- Base every claim on the dossier. Callee names are not given: use their files and how they are called.
- Name in snake_case. Use Class::method when the function is a thiscall member of an object of this file (ECX is `this`, a global object pointer, or a vtable slot).
- Convention: cdecl, stdcall, thiscall or fastcall, with the argument list as types and roles, and `ret n`.
- Purpose: one sentence of at most 25 words, active voice, British English, no semicolons, no dashes.
- Evidence: the two or three facts that decide the name (a string, a constant, a caller, a field).
- Confidence: high when a string or a caller pins the purpose, medium when the code shape and callers agree, low when it is a guess.
- If the function is in a gap between two files, say which file it belongs to in `file`.
"""

SCHEMA = {
    "type": "object",
    "properties": {
        "name": {"type": "string"},
        "file": {"type": "string"},
        "convention": {"type": "string"},
        "purpose": {"type": "string"},
        "evidence": {"type": "string"},
        "confidence": {"type": "string", "enum": ["high", "medium", "low"]},
    },
    "required": ["name", "file", "convention", "purpose", "evidence", "confidence"],
    "additionalProperties": False,
}

# Batch prices per million tokens: input, cache write, cache read, output (half the list price).
PRICES = {"haiku": (0.05, 0.0625, 0.005, 0.25), "sonnet": (1, 1.25, 0.1, 5), "opus": (2, 2.5, 0.1, 10)}


def submit(run, model, only):
    work = work_dir()
    ddir = f"{work}/dossiers"
    rdir = f"{work}/runs/{run}"
    os.makedirs(rdir, exist_ok=True)
    wanted = set(open(only).read().split()) if only else None
    reqs = []
    for line in open(f"{ddir}/functions.tsv"):
        addr, n, group = line.rstrip("\n").split("\t")
        if wanted is not None and addr not in wanted:
            continue
        tier = next(t for lim, t in SIZE_TIERS if int(n) <= lim) if model == "size" else model
        name, effort, max_tokens = MODELS[tier]
        system = [{"type": "text", "text": INSTRUCTIONS},
                  {"type": "text", "text": open(f"{ddir}/summaries/{group}.txt").read(), "cache_control": {"type": "ephemeral"}}]
        block = open(f"{ddir}/{addr[2:]}.txt").read()
        reqs.append(Request(custom_id=f"{addr}-{tier}", params=MessageCreateParamsNonStreaming(
            model=name, max_tokens=max_tokens, system=system,
            output_config={"effort": effort, "format": {"type": "json_schema", "schema": SCHEMA}},
            messages=[{"role": "user", "content": f"File: {group}\n\n{block}"}])))
    batch = anthropic.Anthropic().messages.batches.create(requests=reqs)
    json.dump({"id": batch.id, "created": time.time(), "requests": len(reqs)}, open(f"{rdir}/batch.json", "w"))
    print(batch.id, len(reqs))


def collect(run):
    rdir = f"{work_dir()}/runs/{run}"
    client = anthropic.Anthropic()
    info = json.load(open(f"{rdir}/batch.json"))
    while client.messages.batches.retrieve(info["id"]).processing_status != "ended":
        time.sleep(30)
    usage = {}
    with open(f"{rdir}/names.jsonl", "w") as out:
        for r in client.messages.batches.results(info["id"]):
            addr, tier = r.custom_id.split("-")
            row = {"address": addr, "model": tier, "status": r.result.type}
            if r.result.type == "succeeded":
                m = r.result.message
                row["stop_reason"] = m.stop_reason
                if m.stop_reason == "refusal":
                    row["refusal"] = str(m.stop_details)
                else:
                    text = next((x.text for x in m.content if x.type == "text"), "")
                    try:
                        row.update(json.loads(text))
                    except json.JSONDecodeError:
                        row["raw"] = text
                u = usage.setdefault(tier, {"requests": 0, "input": 0, "cache_write": 0, "cache_read": 0, "output": 0})
                u["requests"] += 1
                u["input"] += m.usage.input_tokens
                u["cache_write"] += m.usage.cache_creation_input_tokens or 0
                u["cache_read"] += m.usage.cache_read_input_tokens or 0
                u["output"] += m.usage.output_tokens
            else:
                row["error"] = str(getattr(r.result, "error", ""))
            out.write(json.dumps(row) + "\n")
    cost = sum((u["input"] * p[0] + u["cache_write"] * p[1] + u["cache_read"] * p[2] + u["output"] * p[3]) / 1e6
               for t, u in usage.items() for p in [PRICES[t]])
    usage["cost_usd"] = round(cost, 2)
    usage["wall_seconds"] = round(time.time() - info["created"])
    json.dump(usage, open(f"{rdir}/usage.json", "w"), indent=1)
    print(json.dumps(usage, indent=1))


if __name__ == "__main__":
    args = sys.argv[1:]
    opt = lambda k, d: args[args.index(k) + 1] if k in args else d
    if args[0] == "submit":
        submit(args[1], opt("--model", "size"), opt("--only", None))
    else:
        collect(args[1])
