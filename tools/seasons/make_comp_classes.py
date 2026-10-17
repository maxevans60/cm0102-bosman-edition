#!/usr/bin/env python3
"""Makes the table of competition classes for the rules report (0.6.0 slice 12).

Usage:
    python3 -I tools/seasons/make_comp_classes.py --exe <cm0102.exe> --out <comp_classes.c>

The rules report (src/recomp_rt/win/season_report.c) names the source file of each competition object, so
that tests/seasons/rules_report_check.py can compare it with the comp_file column of the rules table. An
object holds its class as the vtable pointer at +0. This tool finds the vtable of each class in the code:

- A constructor stores its vtable with "mov dword ptr [reg], imm32" after the call of the constructor of its
  base class (for example EngPrm::EngPrm 0x573380 calls League::League 0x682200 at 0x5733a3 and stores
  0x969d1c at 0x5733d0). The constructors are the functions "X::X" of the function map
  (docs/research/function-map/names.tsv). The last such store of .rdata in the constructor is its vtable.
- The kind of a class follows the first call of its constructor down to Comp::Comp 0x48c580 (the vtable
  0x9689e8 at 0x48c587), League::League 0x682200 (0x96d020 at 0x68227e) or Cup::Cup 0x518640 (0x9693c8 at
  0x51864a). A league holds its club count at +0x3e and its matches per club at +0xa7 (League::check_complete
  0x689ad0, 0x689b1d..0x689b5f). A cup holds its round count at +0x3c (Cup::map_fixture_tree 0x518790 at
  0x51879c).
- The file of a class is the file of its constructor in the function map. The function map takes the file
  from the assert strings of the original. Its names are hints: the tool only takes the file and the
  constructor address from it, and reads the code itself.

The output is C for the build folder. It is not committed: it holds addresses of the original only.
The stop rule: no constructor found, two files for one vtable, or a base kind that the tool cannot follow,
stop the tool with exit code 1.
"""
import argparse
import bisect
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "recomp"))
from pe import Image  # noqa: E402

NAMES = os.path.join(ROOT, "docs", "research", "function-map", "names.tsv")
RDATA = (0x967000, 0x987000)
BASES = {0x48c580: "comp", 0x682200: "league", 0x518640: "cup"}
VTABLE_OF_BASE = {0x48c580: 0x9689e8, 0x682200: 0x96d020, 0x518640: 0x9693c8}
STORE = re.compile(r"^dword ptr \[e[a-z]{2}\], (0x[0-9a-f]+)$")


def fail(text):
    raise SystemExit("make_comp_classes.py: " + text)


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", required=True)
    ap.add_argument("--out", required=True)
    a = ap.parse_args(argv)
    import capstone
    img = Image(a.exe)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    with open(NAMES, newline="", encoding="utf-8") as f:
        names = list(csv.DictReader(f, delimiter="\t"))
    starts = sorted(int(r["address"], 16) for r in names)

    ctors = {}     # address -> (file, class, vtable, first call)
    for r in names:
        m = re.match(r"^(\w+)::(\w+)$", r["name"])
        if not m or m.group(1) != m.group(2):
            continue
        at = int(r["address"], 16)
        i = bisect.bisect_right(starts, at)
        end = starts[i] if i < len(starts) else at + 0x1000
        vtable, first = None, None
        for ins in md.disasm(img.read(at, end - at), at):
            if ins.mnemonic == "call" and first is None and ins.op_str.startswith("0x"):
                first = int(ins.op_str, 16)
            s = STORE.match(ins.op_str) if ins.mnemonic == "mov" else None
            if s and RDATA[0] <= int(s.group(1), 16) < RDATA[1]:
                vtable = int(s.group(1), 16)
        if vtable is not None:
            ctors[at] = (r["file"], m.group(1), vtable, first)
    for base, vt in VTABLE_OF_BASE.items():
        if base not in ctors or ctors[base][2] != vt:
            fail(f"the constructor {base:#x} does not store the vtable {vt:#x}")

    def kind(at, depth=0):
        if at in BASES:
            return BASES[at]
        if at not in ctors or depth > 8:
            return None
        return kind(ctors[at][3], depth + 1)

    table = {}
    for at, (file, cls, vt, _) in ctors.items():
        k = kind(at)
        if k is None:
            continue                      # not a competition class (rule books, awards and others)
        file = file[:-4] if file.endswith(".cpp") else file
        if vt in table and table[vt][0] != file:
            fail(f"the vtable {vt:#x} has two files: {table[vt][0]} and {file}")
        table[vt] = (file, cls, k, at)
    # Classes whose constructor is inline in another function (ReserveComp::create_sub_comps 0x7e61c0 makes
    # BIntComp and Under21IntComp: 0x7e620e, 0x7e6274): any vtable that a "mov dword ptr [reg], imm32" of
    # .text stores, whose slot +0xc is Comp::all_stages_complete 0x48ce10 (the slot of every competition
    # class, game_season_update_pass 0x5d5ccb) and whose slot 0 is the scalar deleting destructor of a class
    # of the function map. The kind comes from slot +0x10: League::check_complete 0x689ad0, Cup 0x521e00.
    dtor = {int(r["address"], 16): r for r in names
            if re.match(r"^\w+::(`scalar deleting destructor'|scalar_deleting_destructor)$", r["name"])}
    def stores():
        for i, at in enumerate(starts):            # function by function: capstone stops at data bytes
            end = starts[i + 1] if i + 1 < len(starts) else at + 0x1000
            for ins in md.disasm(img.read(at, end - at), at):
                if ins.mnemonic == "mov":
                    yield ins

    for ins in stores():
        s = STORE.match(ins.op_str)
        if not s:
            continue
        vt = int(s.group(1), 16)
        if not RDATA[0] <= vt < RDATA[1] or vt in table or img.u32(vt + 0xc) != 0x48ce10:
            continue
        r = dtor.get(img.u32(vt))
        if not r:
            continue
        k = {0x689ad0: "league", 0x521e00: "cup"}.get(img.u32(vt + 0x10), "comp")
        file = r["file"][:-4] if r["file"].endswith(".cpp") else r["file"]
        table[vt] = (file, r["name"].split("::")[0], k, ins.address)
    if len(table) < 100:
        fail(f"only {len(table)} competition classes found")
    lines = [
        "/* Competition classes of the original for the rules report (season_report.c). Made by",
        " * tools/seasons/make_comp_classes.py from cm0102.exe and the function map. A build file: not committed. */",
        '#include "season_rules.h"',
        "",
        "const SeasonCompClass season_comp_classes[] = {",
    ]
    for vt in sorted(table):
        file, cls, k, at = table[vt]
        lines.append(f'    {{0x{vt:x}u, "{file}", "{cls}", SEASON_KIND_{k.upper()}}},   /* stored at 0x{at:x} or after */')
    lines += ["};", f"const int season_comp_class_count = {len(table)};", ""]
    os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
    tmp = a.out + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    os.replace(tmp, a.out)
    print(f"make_comp_classes.py: {len(table)} classes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
