"""Places every function of the original in its source file and writes function-files.tsv.

Methods, in order:
- assert: the function pushes one source path string (391 .cpp paths in the exe).
- vtable: a method or constructor of a class whose other methods assert in one file, inside that
  file's address window.
- order: the linker placed the objects in alphabetical order of file name, so a function between two
  functions of one file is in that file.
- callers: all callers are in one file and the function sits in the gap next to that file.
Functions between two different files keep both candidates. Exception unwind funclets belong to the
function whose FuncInfo lists them. Handler stubs, the library without asserts and the C runtime get
their own category.

Usage: CM0102_EXE=... FUNCMAP_WORK=... placement.py   (needs a configured build for build/recomp/gen)
"""
import bisect
import collections
import os
import re
import struct

import capstone
from capstone import x86_const as X

from common import MAP_DIR, all_entries, image, save
from analysis import Analyzer

LIB0, CRT0 = 0x933a00, 0x944c16     # library without asserts, then the C runtime (not 16-byte aligned)
FUNCINFO_MAGIC = 0x19930520


def decode_all(img, funcs):
    an = Analyzer(img, set(funcs))
    db = {}
    for e in funcs:
        f = an.function(e)
        refs = set()
        for ins in f.insns.values():
            for op in ins.operands:
                if op.type == X.X86_OP_IMM:
                    refs.add(op.imm & 0xFFFFFFFF)
                elif op.type == X.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                    refs.add(op.mem.disp & 0xFFFFFFFF)
        db[e] = dict(calls=set(f.calls) | set(f.tails), refs=refs, n=len(f.insns))
    return db


def source_paths(img):
    paths = {}
    for name, va, vsize, rptr, rsize, flags in img.sections:
        if img.is_code(va):
            continue
        for m in re.finditer(rb"[A-Za-z]:\\[ -~]{3,200}?\.(?:cpp|h|c)\x00", img.read(va, rsize)):
            paths[va + m.start()] = m.group()[:-1].decode()
    return paths


def funclet_owners(img, funcs):
    """Unwind and catch funclets -> the function whose FuncInfo lists them."""
    fset = set(funcs)

    def va_of(off):
        for _, va, vsize, rptr, rsize, _ in img.sections:
            if rptr <= off < rptr + rsize:
                return va + off - rptr

    tbase, tend = img.text
    text = img.read(tbase, tend - tbase)

    def find_code(pattern):
        return [tbase + m.start() for m in re.finditer(re.escape(pattern), text)]

    owner = {}
    for m in re.finditer(re.escape(struct.pack("<I", FUNCINFO_MAGIC)), img.data):
        fi = va_of(m.start())
        if not fi or img.is_code(fi):
            continue
        parents = set()
        for stub in find_code(b"\xb8" + struct.pack("<I", fi)):            # mov eax, FuncInfo
            for push in find_code(b"\x68" + struct.pack("<I", stub)):      # push handler
                parents.add(funcs[bisect.bisect_right(funcs, push) - 1])
        if len(parents) != 1:
            continue
        p = parents.pop()
        states, umap, ntry, tmap = img.u32(fi + 4), img.u32(fi + 8), img.u32(fi + 12), img.u32(fi + 16)
        for k in range(min(states, 4096)):
            a = img.u32(umap + 8 * k + 4)
            if a in fset and a != p:
                owner.setdefault(a, p)
        for t in range(min(ntry, 256)):
            nc, hm = img.u32(tmap + 20 * t + 12), img.u32(tmap + 20 * t + 16)
            for c in range(min(nc, 64)):
                a = img.u32(hm + 16 * c + 12)
                if a in fset and a != p:
                    owner.setdefault(a, p)
    return owner


def find_vtables(img, db, funcs):
    """Vtables: tables of entries that code references (constructors store them). Start address -> entries."""
    fset = set(funcs)
    coderefs = {r for e in funcs for r in db[e]["refs"] if not img.is_code(r) and img.u32(r) in fset}
    vtables = {}
    for st in sorted(coderefs):
        ents, a = [], st
        while img.u32(a) in fset and (a == st or a not in coderefs) and len(ents) < 400:
            ents.append(img.u32(a))
            a += 4
        if len(ents) >= 2:
            vtables[st] = ents
    return vtables


def main():
    img = image()
    funcs = all_entries()
    fset = set(funcs)
    db = decode_all(img, funcs)
    paths = source_paths(img)
    short = {a: os.path.basename(p.replace("\\", "/")) for a, p in paths.items()}
    owner = funclet_owners(img, funcs)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    def is_stub(e):
        i = list(md.disasm(img.read(e, 12), e, 2))
        return (len(i) == 2 and i[0].mnemonic == "mov" and i[0].op_str.startswith("eax, 0x") and i[1].mnemonic == "jmp"
                and img.u32(int(i[0].op_str.split(", ")[1], 16)) == FUNCINFO_MAGIC)

    cat = {}
    for e in funcs:
        cat[e] = ("funclet" if e in owner else "ehstub" if is_stub(e) else "crt" if e >= CRT0
                  else "lib" if e >= LIB0 else "game")
    game = [e for e in funcs if cat[e] == "game"]
    idx = {e: i for i, e in enumerate(game)}

    file_of, how = {}, {}
    for e in game:
        fs = {short[r] for r in db[e]["refs"] if r in short}
        if len(fs) == 1:
            file_of[e], how[e] = fs.pop(), "assert"
    asserts = sorted(file_of)

    def in_window(e, f):
        i = bisect.bisect_left(asserts, e)
        lo = file_of[asserts[i - 1]].lower() if i > 0 else ""
        hi = file_of[asserts[i]].lower() if i < len(asserts) else "~"
        return lo <= f.lower() <= hi

    vtables = find_vtables(img, db, funcs)
    in_vt = collections.defaultdict(set)
    for s, ents in vtables.items():
        for x in ents:
            in_vt[x].add(s)
    refs_vt = collections.defaultdict(set)
    for e in game:
        for r in db[e]["refs"]:
            if r in vtables:
                refs_vt[r].add(e)
    members = {s: {x for x in set(vtables[s]) | refs_vt[s] if cat.get(x) == "game"} for s in vtables}

    def vt_file(s):
        c = collections.Counter(file_of[x] for x in members[s] if how.get(x) == "assert")
        return c.most_common(1)[0][0] if c else None

    vtf = {s: vt_file(s) for s in vtables}
    for x in sorted({x for m in members.values() for x in m}):
        if x in file_of:
            continue
        ss = [s for s in vtables if x in members[s]]
        fs = {vtf[s] for s in ss if vtf[s]}
        owned = [s for s in ss if x in refs_vt[s] or len(in_vt[x]) == 1]
        if len(fs) == 1 and owned:
            f = fs.pop()
            if in_window(x, f):
                file_of[x], how[x] = f, "vtable"

    def close_spans():
        changed = False
        lab = [e for e in game if e in file_of]
        for a, b in zip(lab, lab[1:]):
            if file_of[a] == file_of[b]:
                for e in game[idx[a] + 1:idx[b]]:
                    if e not in file_of:
                        file_of[e], how[e] = file_of[a], "order"
                        changed = True
        return changed

    allfiles = sorted(set(short.values()), key=str.lower)

    def gap_candidates():
        lab = [e for e in game if e in file_of]
        placed = set(file_of.values())
        out = {}
        for a, b in zip(lab, lab[1:]):
            A, B = file_of[a], file_of[b]
            if A == B:
                continue
            mid = [f for f in allfiles if A.lower() < f.lower() < B.lower() and f not in placed]
            for e in game[idx[a] + 1:idx[b]]:
                if e not in file_of:
                    out[e] = [A] + mid + [B]
        return out

    callers = collections.defaultdict(set)
    for e in funcs:
        for c in db[e]["calls"]:
            if c in fset:
                callers[c].add(e)
    close_spans()
    changed = True
    while changed:
        changed = False
        cand = gap_candidates()
        for e in game:
            if e in file_of or e not in cand:
                continue
            cs = [c for c in callers[e] if cat[c] == "game"]
            if cs and all(c in file_of for c in cs) and len({file_of[c] for c in cs}) == 1 \
                    and file_of[cs[0]] in cand[e] and in_window(e, file_of[cs[0]]):
                file_of[e], how[e] = file_of[cs[0]], "callers"
                changed = True
        changed |= close_spans()
    gaps = gap_candidates()

    save(dict(db=db, paths=paths, owner=owner, cat=cat, file_of=file_of, how=how, gaps=gaps), "placement.pkl")
    os.makedirs(MAP_DIR, exist_ok=True)
    with open(os.path.join(MAP_DIR, "function-files.tsv"), "w") as out:
        out.write("address\tcategory\tfile\tmethod\tinstructions\n")
        for e in funcs:
            c = cat[e]
            if c == "game":
                f, m = (file_of[e], how[e]) if e in file_of else ("|".join(gaps[e]), "gap")
            elif c == "funclet":
                f, m = "parent 0x%06x" % owner[e], "eh"
            else:
                f, m = "", ""
            out.write("0x%06x\t%s\t%s\t%s\t%d\n" % (e, c, f, m, db[e]["n"]))
    print(len(funcs), "functions:", dict(collections.Counter(cat.values())))
    print("game:", dict(collections.Counter(how.values())), "gap:", len(gaps))


if __name__ == "__main__":
    main()
