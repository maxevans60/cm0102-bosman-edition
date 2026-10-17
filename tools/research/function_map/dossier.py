"""Builds the naming dossiers: one text block per function and one summary per source file.

A dossier holds the function's placement, size, ret, strings, assert line numbers, globals, callers
and callees with their files, pointers to it from data, and the Ghidra pseudocode (or capstone
disassembly when Ghidra gave none). With --names, callers and callees also show earlier names and
their confidence. Globals show their data map name and confidence from docs/research/data-map/globals.tsv.

Usage: CM0102_EXE=... FUNCMAP_WORK=... dossier.py --all [--names names.jsonl ...]
       CM0102_EXE=... FUNCMAP_WORK=... dossier.py <file.cpp> [--names names.jsonl]
Writes $FUNCMAP_WORK/dossiers/<address>.txt, summaries/<group>.txt and functions.tsv.
"""
import collections
import json
import os
import re
import sys

from common import REPO, image, load, work_dir
from analysis import Analyzer

MAX_PSEUDO_LINES = 2500

img = image()
P = load("placement.pkl")
db, paths, eh, cat, fo, how, bnd = P["db"], P["paths"], P["owner"], P["cat"], P["file_of"], P["how"], P["gaps"]
fset = set(db)
an = Analyzer(img, fset)
short = {a: os.path.basename(p.replace("\\", "/")) for a, p in paths.items()}
WORK = work_dir()
NAMES = {}
CONF = {}
GLOBALS_TSV = os.path.join(REPO, "docs", "research", "data-map", "globals.tsv")
GLOBALS = {}
if os.path.exists(GLOBALS_TSV):
    for line in list(open(GLOBALS_TSV))[1:]:
        c = line.rstrip("\n").split("\t")
        GLOBALS[int(c[0], 16)] = (c[3], c[6])


def string_at(a):
    if img.is_code(a):
        return None
    b = img.read(a, 200).split(b"\0")[0]
    if len(b) >= 3 and all(32 <= c < 127 or c in (9, 10, 13) for c in b):
        return b.decode("latin1")
    return None


def where(e):
    c = cat.get(e)
    w = (fo.get(e) or "/".join(bnd.get(e, ["?"]))) if c == "game" else (c or "?")
    if e not in NAMES:
        return w
    return f"{w}: {NAMES[e]}" + (f" ({CONF[e]})" if e in CONF else "")


def global_label(v):
    if v not in GLOBALS:
        return f"0x{v:06x}"
    name, conf = GLOBALS[v]
    return f"0x{v:06x} {name} ({conf})"


callers = collections.defaultdict(set)
for e in db:
    for c in db[e]["calls"]:
        if c in fset:
            callers[c].add(e)
dataref = collections.defaultdict(list)
for name, va, vsize, rptr, rsize, flags in img.sections:
    if img.is_code(va) or not rsize:
        continue
    data = img.read(va, rsize)
    for off in range(0, len(data) - 3, 4):
        v = int.from_bytes(data[off:off + 4], "little")
        if v in fset:
            dataref[v].append(va + off)


def facts(e):
    f = an.function(e)
    order = sorted(f.insns)
    rets = sorted({f.insns[a].op_str or "0" for a in order if f.insns[a].mnemonic == "ret"})
    strings, globals_, asserts = [], [], []
    prev = None
    for a in order:
        ins = f.insns[a]
        for tok in re.findall(r"0x[0-9a-f]{6,}", ins.op_str):
            v = int(tok, 16)
            if v in short:
                if prev is not None and prev.mnemonic == "push" and re.fullmatch(r"(0x[0-9a-f]+|[0-9]+)", prev.op_str or ""):
                    asserts.append(int(prev.op_str, 0))
                continue
            s = string_at(v)
            if s:
                strings.append((a, v, s))
            elif not img.is_code(v) and 0x960000 <= v < 0x2000000:
                globals_.append(v)
        prev = ins
    return f, rets, strings, sorted(set(globals_)), asserts


def block(e):
    f, rets, strings, globals_, asserts = facts(e)
    lines = [f"### Function 0x{e:06x}"]
    lines.append(f"Placement: {where(e)} ({how.get(e, 'gap between two files')}). Size: {len(f.insns)} instructions. "
                 f"Returns: {', '.join('ret ' + r for r in rets) or 'no ret seen'}. Unwind funclets: {sum(1 for v in eh.values() if v == e)}.")
    if asserts:
        lines.append("Assert line numbers: " + ", ".join(str(x) for x in asserts))
    if strings:
        lines.append("Strings referenced:")
        seen = set()
        for a, v, s in strings:
            if v in seen:
                continue
            seen.add(v)
            lines.append(f"  0x{a:06x}: 0x{v:06x} \"{s[:160]}\"")
    if globals_:
        lines.append("Globals referenced: " + ", ".join(global_label(v) for v in globals_))
    cs = sorted(c for c in f.calls | f.tails if c in fset)
    if cs:
        lines.append("Calls: " + ", ".join(f"0x{c:06x} [{where(c)}]" for c in cs))
    cb = sorted(callers.get(e, ()))
    if cb:
        lines.append("Called by: " + ", ".join(f"0x{c:06x} [{where(c)}]" for c in cb[:60]) + (f" and {len(cb) - 60} more" if len(cb) > 60 else ""))
    if dataref.get(e):
        lines.append("Pointer to it in data at: " + ", ".join(f"0x{v:06x}" for v in dataref[e][:20]))
    p = f"{WORK}/ghidra/out/{e:x}.c"
    code = open(p).read().splitlines() if os.path.exists(p) else []
    if not code or code[0].startswith("// no function") or code[0].startswith("// decompile failed"):
        f2 = an.function(e)
        asm = [f"{a:06x}  {f2.insns[a].mnemonic} {f2.insns[a].op_str}" for a in sorted(f2.insns)]
        if len(asm) > MAX_PSEUDO_LINES:
            asm = asm[:MAX_PSEUDO_LINES] + [f"; ... {len(asm) - MAX_PSEUDO_LINES} more lines cut"]
        lines.append("Disassembly (Ghidra gave no pseudocode):")
        lines.append("```asm")
        lines.extend(asm)
        lines.append("```")
    elif code:
        if len(code) > MAX_PSEUDO_LINES:
            code = code[:MAX_PSEUDO_LINES] + [f"// ... {len(code) - MAX_PSEUDO_LINES} more lines cut"]
        lines.append("Ghidra pseudocode:")
        lines.append("```c")
        lines.extend(code)
        lines.append("```")
    return "\n".join(lines), len(f.insns)


def group_of(e):
    if cat.get(e) == "lib":
        return "library_without_asserts"
    return fo.get(e) or bnd[e][0]


def build(members, out):
    os.makedirs(f"{out}/summaries", exist_ok=True)
    groups = collections.defaultdict(list)
    rows = []
    for e in members:
        text, n = block(e)
        open(f"{out}/{e:06x}.txt", "w").write(text)
        groups[group_of(e)].append((e, n))
        rows.append(f"0x{e:06x}\t{n}\t{group_of(e)}")
    for gname, items in groups.items():
        summary = [f"# Source file {gname}", f"{len(items)} functions.", "",
                   "| Address | Instructions | Placement | First strings |", "|---|---|---|---|"]
        for e, n in items:
            _, _, strings, _, _ = facts(e)
            first = "; ".join(s[:40] for _, _, s in strings[:3]).replace("|", "/")
            place = how.get(e) if e in fo else ("gap " + "/".join(bnd[e]) if e in bnd else "library")
            summary.append(f"| 0x{e:06x} | {n} | {place} | {first} |")
        open(f"{out}/summaries/{gname}.txt", "w").write("\n".join(summary) + "\n")
    open(f"{out}/functions.tsv", "w").write("\n".join(rows) + "\n")
    print(len(members), "functions in", len(groups), "groups")


def main(argv):
    while "--names" in argv:
        i = argv.index("--names")
        for line in open(argv[i + 1]):
            r = json.loads(line)
            if r.get("name"):
                NAMES[int(r["address"], 16)] = r["name"]
                if r.get("confidence"):
                    CONF[int(r["address"], 16)] = r["confidence"]
        argv = argv[:i] + argv[i + 2:]
    if argv[0] == "--all":
        members = sorted(e for e in db if cat.get(e) in ("game", "lib"))
    else:
        members = sorted(e for e in db if fo.get(e) == argv[0] or (e in bnd and argv[0] in bnd[e]))
    build(members, f"{WORK}/dossiers")


if __name__ == "__main__":
    main(sys.argv[1:])
