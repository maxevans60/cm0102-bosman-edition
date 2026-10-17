"""Builds the data map input: every global that a game or library function touches, its accesses,
its address group and owner file, one short dossier per global and the script names.

A global is an address in .rdata or .data that an instruction of a game or library function uses,
with the same rule as dossier.py: not code, not an assert path, not a string. Each access records
the function, the kind (read, write or address) and the size. The dossier lists the readers and
writers with their function map names, the accesses and the Ghidra lines that touch the global.

Usage: CM0102_EXE=... FUNCMAP_WORK=... globals.py
Writes $FUNCMAP_WORK/globals.pkl, data-dossiers/<address>.txt and runs-data/data-script/names.jsonl.
"""
import collections
import csv
import json
import os
import re
import sys
import unicodedata

from capstone import x86_const as X

from common import MAP_DIR, REPO, image, load, save, work_dir
from analysis import Analyzer

GROUP_GAP = 0x40          # adjacent globals closer than this are one address group
MAX_FUNCS = 12            # functions shown in one dossier
MAX_LINES = 3             # Ghidra lines shown for one function
KEY_IDS_C = os.path.join(REPO, "src", "cm3", "key_ids_table.c")
KEY_ID_FUNCTION = 0x601a40

# Instructions whose memory operand is only read, even when it is the first operand.
READ_ONLY = {"cmp", "test", "bt", "push", "call", "jmp", "fld", "fild", "fcom", "fcomp", "ficom", "ficomp",
             "fadd", "fsub", "fsubr", "fmul", "fdiv", "fdivr", "fiadd", "fisub", "fisubr", "fimul", "fidiv",
             "fidivr", "fldcw", "fldenv", "frstor", "lea"}
# Instructions whose immediate is a number or a bit mask, never an address.
MASK_OPS = {"and", "or", "xor", "test", "imul", "shl", "shr", "sar", "rol", "ror", "sbb", "adc"}
X87_STORE = {"fst", "fstp", "fist", "fistp", "fnstcw", "fnstsw", "fstcw", "fstsw", "fnsave", "fbstp"}

img = image()
P = load("placement.pkl")
db, paths, cat = P["db"], P["paths"], P["cat"]
fset = set(db)
an = Analyzer(img, fset)
WORK = work_dir()


def section_of(a):
    for name, va, vsize, rptr, rsize, flags in img.sections:
        if va <= a < va + max(vsize, rsize):
            return name, a < va + rsize
    return "?", False


def string_at(a):
    if img.is_code(a):
        return None
    name, init = section_of(a)
    if not init:
        return None
    b = img.read(a, 200).split(b"\0")[0]
    if len(b) >= 3 and all(32 <= c < 127 or c in (9, 10, 13) for c in b):
        return b.decode("latin1")
    return None


def in_data(v):
    return any(name in (".rdata", ".data") and va <= v < va + max(vsize, rsize)
               for name, va, vsize, rptr, rsize, flags in img.sections)


def text_at(a):
    """The text at a in initialised data, with Latin-1 letters and of any length, or None."""
    name, init = section_of(a)
    if not init:
        return None
    b = img.read(a, 200)
    if b"\0" not in b:
        return None
    b = b.split(b"\0")[0]
    if b and all(32 <= c < 127 or c in (9, 10, 13) or c >= 0xa0 for c in b):
        return b.decode("latin1")
    return None


def is_global(v):
    return in_data(v) and v not in paths and v not in img.imports and not string_at(v)


def function_names():
    rows = {}
    for r in csv.DictReader(open(os.path.join(MAP_DIR, "names.tsv")), delimiter="\t"):
        rows[int(r["address"], 16)] = r
    return rows


def scan(members):
    """address -> list of (function, insn address, kind, size, mnemonic, op_str, indexed)."""
    acc = collections.defaultdict(list)
    for e in members:
        f = an.function(e)
        for a in sorted(f.insns):
            ins = f.insns[a]
            seen = set()
            ops = list(ins.operands)
            for i, op in enumerate(ops):
                if op.type == X.X86_OP_MEM:
                    v = op.mem.disp & 0xffffffff
                    if not is_global(v):
                        continue
                    m = ins.mnemonic
                    if m in X87_STORE:
                        kind = "write"
                    elif m in READ_ONLY or i > 0:
                        kind = "read"
                    elif m.startswith("set") or len(ops) >= 2 or m in ("inc", "dec", "not", "neg", "pop"):
                        kind = "write"
                    else:
                        kind = "read"
                    indexed = op.mem.base != 0 or op.mem.index != 0
                    acc[v].append((e, a, kind, op.size, m, ins.op_str, indexed))
                    seen.add(v)
                elif op.type == X.X86_OP_IMM:
                    v = op.imm & 0xffffffff
                    if v in seen or ins.mnemonic in MASK_OPS or not is_global(v):
                        continue
                    acc[v].append((e, a, "address", 0, ins.mnemonic, ins.op_str, False))
                    seen.add(v)
    return acc


def groups_of(addrs):
    out, cur = [], []
    for a in addrs:
        if cur and (a - cur[-1] > GROUP_GAP or section_of(a)[0] != section_of(cur[-1])[0]):
            out.append(cur)
            cur = []
        cur.append(a)
    if cur:
        out.append(cur)
    return out


def owner_file(funcs, fname):
    files = collections.Counter(fname(e) for e in funcs)
    top, n = sorted(files.items(), key=lambda kv: (-kv[1], kv[0].lower()))[0]
    if len(files) >= 4 and n * 2 < len(funcs):
        return "shared"
    return top


def slug(s, limit=40):
    for sign, word in (("\u00a3", " pound "), ("\u00a5", " yen "), ("\u00a4", " currency "), ("\u00b0", " degree "), ("$", " dollar ")):
        s = s.replace(sign, word)
    s = unicodedata.normalize("NFKD", s).encode("ascii", "ignore").decode()
    s = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", s)
    s = re.sub(r"[^a-z0-9]+", "_", s.lower()).strip("_")
    return s[:limit].rstrip("_") or "empty"


def key_id_names():
    """Key id globals named after the first string of their name chain in FUN_00601a40's callees."""
    text = open(KEY_IDS_C).read()
    globals_ = [int(x, 16) for x in re.findall(r"^\s+\{(0x[0-9a-f]+), -?\d+, [01]\},$", text, re.M)]
    out = {}
    for m in re.finditer(r"/\* (\d+) ([a-z ]+?) names, (inline at 0x[0-9a-f]+|FUN_00[0-9a-f]+), .*?\*/\n"
                         r"const key_id_entry_t \w+\[\] = \{\n(.*?)\n\};", text, re.S):
        what, where = m.group(2), m.group(3)
        for name_addr, s1, s2, chain in re.findall(r"\{(0x[0-9a-f]+), (-?\d+), (-?\d+), (\d+)\}", m.group(4)):
            name = text_at(int(name_addr, 16)) or ""
            for s in (int(s1), int(s2)):
                if not name:
                    continue
                if s >= 0 and globals_[s] not in out:
                    out[globals_[s]] = (what, name, int(name_addr, 16), where)
    return set(globals_), out


def vtable_class(a, fn, starts=()):
    """Slots of a vtable at a: the function entries in a row from a, up to the next global."""
    slots = []
    while True:
        v = img.u32(a + 4 * len(slots))
        if v not in fset or (slots and a + 4 * len(slots) in starts):
            break
        slots.append(v)
    return slots


def script_names(G, fn):
    names = {}
    key_set, key_named = key_id_names()
    for a, g in G.items():
        readers = sorted({x[0] for x in g["acc"]})
        sec, init = section_of(a)
        if a in key_named:
            what, s, sa, where = key_named[a]
            kind = slug(what.split()[-1] if what.split()[-1] not in ("comp", "comps") else what, 20)
            names[a] = dict(name=f"g_key_{slug(what, 20)}_{slug(s)}", type="s32", size=4,
                            description=f"Key id of the {what[:-1] if what.endswith('s') else what} \"{s}\": the row index that the database load stores for this name.",
                            evidence=f"string 0x{sa:06x} \"{s}\" in the name chain {where} of the key id load 0x{KEY_ID_FUNCTION:06x}, which stores the row here",
                            confidence="high")
            continue
        if a in key_set:
            names[a] = dict(name=f"g_key_id_unnamed_{a:06x}", type="s32", size=4,
                            description="Key id that the key id load 0x601a40 sets to -1 and no name chain sets.",
                            evidence=f"0x{KEY_ID_FUNCTION:06x} stores -1 here before the name chains run",
                            confidence="medium")
            continue
        if sec == ".rdata":
            slots = vtable_class(a, fn, G)
            if len(slots) >= 2:
                # The class is the one whose destructor sits in a slot, or whose constructor or
                # destructor stores this address as the object's vtable pointer.
                votes = collections.defaultdict(list)
                storers = collections.defaultdict(list)
                for s in slots:
                    nm = fn.get(s, {}).get("name", "")
                    if "::" in nm and ("destructor" in nm or "::~" in nm):
                        votes[nm.split("::")[0]].append(s)
                for x in g["acc"]:
                    nm = fn.get(x[0], {}).get("name", "")
                    if x[2] == "address" and x[4] == "mov" and "ptr [" in x[5] and "::" in nm:
                        cls, meth = nm.split("::")[0], nm.split("::")[-1]
                        if meth in (cls, "~" + cls) or "destructor" in meth:
                            votes[cls].append(x[0])
                            storers[cls].append(x[0])
                if votes:
                    cls = max(votes, key=lambda c: (len(storers[c]), len(votes[c]), c))
                    proof = sorted(set(storers[cls]))
                    high = [e for e in proof if fn[e]["confidence"] == "high"]
                    q = ", ".join(f"0x{e:06x} {fn[e]['name']}" for e in (high or proof)[:3])
                    names[a] = dict(name=f"g_vtable_{slug(cls, 50)}", type="vtable", size=4 * len(slots),
                                    description=f"Vtable of {cls}, {len(slots)} slots.",
                                    evidence=f"the destructor or constructor of {cls} holds it: {q}",
                                    confidence="high" if high and sum(1 for v in storers.values() if v) == 1 else "medium")
                    continue
        if sec == ".rdata" and g["type"] in ("float", "double") and all(x[2] == "read" for x in g["acc"]):
            import struct
            val = struct.unpack("<d" if g["type"] == "double" else "<f", img.read(a, 8 if g["type"] == "double" else 4))[0]
            txt = repr(float(val)) if g["type"] == "double" else ("%.7g" % val)
            label = slug(txt.replace("-", "minus_").replace(".", "_").replace("e+", "e"), 30)
            names[a] = dict(name=f"g_{'f64' if g['type'] == 'double' else 'f32'}_{label}", type=g["type"], size=g["size"],
                            description=f"Read-only {g['type']} constant {txt}.",
                            evidence=f"the {g['type']} at this address in .rdata is {txt}, and every access reads it",
                            confidence="high")
            continue
        if init:
            v = img.u32(a)
            s = string_at(v) if v else None
            if s and all(x[2] != "write" for x in g["acc"]):
                names[a] = dict(name=f"g_str_{slug(s)}", type="char *", size=4,
                                description=f"Pointer to the string \"{s[:60]}\".",
                                evidence=f"string 0x{v:06x} \"{s[:60]}\" is the value at this address, and no function writes it",
                                confidence="high" if sec == ".rdata" else "medium")
                continue
        t = text_at(a)
        if t and g["acc"] and all(x[2] == "address" for x in g["acc"]):
            names[a] = dict(name=f"g_str_{slug(t, 40) if slug(t, 40) != 'empty' else 'chars_' + t.encode('latin1').hex()}", type=f"char[{len(t.encode('latin1')) + 1}]",
                            size=len(t.encode("latin1")) + 1, description=f"The text \"{t}\".",
                            evidence=f"the bytes at this address are the text \"{t}\", and every access passes its address",
                            confidence="high")
            continue
        if all(cat.get(r) in ("lib", "crt") for r in readers):
            first = min(readers)
            rn = fn.get(first, {}).get("name", f"sub_{first:06x}")
            names[a] = dict(name=f"g_lib_{slug(rn.split('::')[-1], 40)}_{a:06x}", type=g["type"], size=g["size"],
                            description=f"Data of the library code, used by {', '.join(fn.get(r, {}).get('name', hex(r)) for r in readers[:3])}.",
                            evidence=f"only library functions touch it: {', '.join(f'0x{r:06x}' for r in readers[:3])}",
                            confidence="medium")
    # Names must be unique: add the address to every name that two globals share.
    count = collections.Counter(n["name"] for n in names.values())
    for a, n in names.items():
        if count[n["name"]] > 1:
            n["name"] = f"{n['name']}_{a:06x}"
    return names


def guess_type(a, acc, nxt):
    sizes = collections.Counter(x[3] for x in acc if x[3])
    floats = any(x[4].startswith("f") and x[3] in (4, 8, 10) for x in acc)
    indexed = any(x[6] for x in acc)
    addr_taken = any(x[2] == "address" for x in acc)
    ecx = any(x[2] == "address" and re.match(r"ecx, 0x", x[5]) for x in acc)
    size = max(sizes) if sizes else 0
    if ecx:
        t = "object"
    elif indexed or (addr_taken and not sizes):
        t = "table" if indexed else "buffer"
    elif floats:
        t = {4: "float", 8: "double", 10: "long double"}.get(size, "float")
    else:
        t = {1: "u8", 2: "u16", 4: "u32"}.get(size, "unknown")
    if t in ("object", "table", "buffer") and nxt:
        size = max(size, min(nxt - a, 0x100000))
    return t, size


def ghidra_lines(e, a):
    p = f"{WORK}/ghidra/out/{e:x}.c"
    if not os.path.exists(p):
        return []
    pat = re.compile(rf"(_00{a:06x}\b|0x{a:x}\b)")
    return [l.strip() for l in open(p) if pat.search(l)][:MAX_LINES]


def spread(funcs, fn):
    """High names first, one function per file before a second one from the same file."""
    rank = {"high": 0, "medium": 1, "low": 2}
    funcs = sorted(funcs, key=lambda e: (rank.get(fn.get(e, {}).get("confidence"), 3), e))
    seen, first, rest = set(), [], []
    for e in funcs:
        f = fn.get(e, {}).get("file")
        (rest if f in seen else first).append(e)
        seen.add(f)
    return first + rest


def dossier(a, g, G, fn, group):
    def fname(e):
        r = fn.get(e)
        return f"0x{e:06x} {r['file']}: {r['name']} ({r['confidence']})" if r else f"0x{e:06x} ({cat.get(e)})"
    sec, init = section_of(a)
    lines = [f"### Global 0x{a:06x}",
             f"Section: {sec}{'' if init else ' (zero at start)'}. Guess: {g['type']}, {g['size']} bytes. Owner file: {g['owner']}."]
    if init:
        lines.append("First 16 bytes in the file: " + img.read(a, 16).hex(" "))
    i = group.index(a)
    near = [f"0x{x:06x} ({x - group[0]:+#x})" for x in group[max(0, i - 4):i + 5] if x != a]
    lines.append(f"Address group 0x{group[0]:06x} to 0x{group[-1]:06x}, {len(group)} globals. Neighbours: {', '.join(near) or 'none'}.")
    byf = collections.defaultdict(list)
    for x in g["acc"]:
        byf[x[0]].append(x)
    w = spread(sorted(e for e in byf if any(x[2] == "write" for x in byf[e])), fn)
    r = spread(sorted(e for e in byf if e not in w), fn)
    lines.append(f"Writers ({len(w)}): " + ("; ".join(fname(e) for e in w[:MAX_FUNCS]) or "none"))
    lines.append(f"Readers only ({len(r)}): " + ("; ".join(fname(e) for e in r[:MAX_FUNCS]) or "none"))
    lines.append("Accesses:")
    for e in (w + r)[:MAX_FUNCS]:
        kinds = ", ".join(sorted({f"{x[2]} {x[3] or ''}".strip() for x in byf[e]}))
        lines.append(f"  in 0x{e:06x} ({kinds}):")
        for x in byf[e][:2]:
            lines.append(f"    0x{x[1]:06x}  {x[4]} {x[5]}")
        for l in ghidra_lines(e, a):
            lines.append(f"    | {l[:200]}")
    if len(byf) > MAX_FUNCS:
        lines.append(f"  ... {len(byf) - MAX_FUNCS} more functions")
    return "\n".join(lines) + "\n"


def main():
    fn = function_names()
    members = sorted(e for e in db if cat.get(e) in ("game", "lib"))
    acc = scan(members)
    addrs = sorted(acc)
    groups = groups_of(addrs)
    G = {}
    for grp in groups:
        for i, a in enumerate(grp):
            nxt = grp[i + 1] if i + 1 < len(grp) else None
            t, size = guess_type(a, acc[a], nxt)
            funcs = sorted({x[0] for x in acc[a]})
            fl = lambda e: "library_without_asserts" if cat.get(e) == "lib" else fn.get(e, {}).get("file", "?")
            G[a] = dict(acc=acc[a], type=t, size=size, group=grp[0], owner=owner_file(funcs, fl),
                        readers=sorted({x[0] for x in acc[a] if x[2] != "write"}),
                        writers=sorted({x[0] for x in acc[a] if x[2] == "write"}))
    names = script_names(G, fn)
    out = f"{WORK}/data-dossiers"
    os.makedirs(out, exist_ok=True)
    gmap = {g[0]: g for g in groups}
    for a, g in G.items():
        open(f"{out}/{a:06x}.txt", "w").write(dossier(a, g, G, fn, gmap[g["group"]]))
    save(dict(G=G, groups=groups), "globals.pkl")
    os.makedirs(f"{WORK}/runs-data/data-script", exist_ok=True)
    with open(f"{WORK}/runs-data/data-script/names.jsonl", "w") as f:
        for a in sorted(names):
            f.write(json.dumps(dict(address=f"0x{a:06x}", model="script", **names[a])) + "\n")
    conf = collections.Counter(n["confidence"] for n in names.values())
    print(len(G), "globals in", len(groups), "groups;", len(names), "script names", dict(conf))
    print("owners", len({g['owner'] for g in G.values()}), "shared", sum(g["owner"] == "shared" for g in G.values()))
    print("types", collections.Counter(g["type"] for g in G.values()))


if __name__ == "__main__":
    main()
