#!/usr/bin/env python3
"""Makes src/recomp_rt/win/season_starts.c: one RT_WRAP for each start_new_season of the competition classes
(0.6.0 slice 16, season_change.h).

Usage:
    python3 -I tools/seasons/make_start_wraps.py --exe <cm0102.exe> --classes <build>/seasons/comp_classes.c
        [--out src/recomp_rt/win/season_starts.c] [--check]

start_new_season is slot +0x8 of the vtable of a competition class: game_season_update_pass 0x5d5ae0 calls
it with "call dword ptr [edx + 0x8]" at 0x5d5d1c, and a top league calls the slot of its lower leagues (for
example eng_prm::start_new_season 0x574c10). The class table comes from tools/seasons/make_comp_classes.py
(the build file comp_classes.c). Many classes share one function (League::start_new_season and others), so
the file has one wrap per distinct function. A slot whose function the function map does not name
start_new_season is left out: 7 classes have the shared "return 0" 0x5a8f60 there, which other vtables use
for other slots too. Each wrap calls season_change_start(this, 0) before the
original and season_change_start(this, 1) after it.

The file is committed, because the recompiler reads the RT_WRAP lines of src/recomp_rt/win/*.c. --check
writes nothing and exits with code 1 when the committed file differs from what the tool makes (ctest
season_starts). RT_WRAP stops the build when an address is not a translated function.
"""
import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "recomp"))
from pe import Image  # noqa: E402

OUT = os.path.join(ROOT, "src", "recomp_rt", "win", "season_starts.c")
NAMES = os.path.join(ROOT, "docs", "research", "function-map", "names.tsv")
SLOT = 0x8


def make(exe, classes):
    img = Image(exe)
    vtables = [int(m.group(1), 16) for m in re.finditer(r"\{0x([0-9a-f]+)u, \"", open(classes).read())]
    if len(vtables) < 100:
        raise SystemExit(f"make_start_wraps.py: only {len(vtables)} classes in {classes}")
    names = {}
    for line in open(NAMES, encoding="utf-8"):
        c = line.split("\t")
        if c[0].startswith("0x"):
            names[int(c[0], 16)] = c[2]
    targets = {}
    for vt in vtables:
        fn = img.u32(vt + SLOT)
        if not img.is_code(fn):
            raise SystemExit(f"make_start_wraps.py: slot +8 of {vt:#x} is not code: {fn:#x}")
        targets.setdefault(fn, []).append(vt)
    lines = [
        "/* The start_new_season functions of the competition classes (slot +0x8 of each vtable), wrapped for the",
        " * season change (season_change.h). Made by tools/seasons/make_start_wraps.py: do not edit by hand. */",
        '#include "win.h"',
        '#include "season_change.h"',
        "",
        "/* Each wrap: season_change_start(this, 0), the original, season_change_start(this, 1). The recompiler",
        " * reads the RT_WRAP lines as they are (tools/recomp/recomp.py), so they are written out in full. */",
        "",
    ]
    for fn in sorted(targets):
        name = names.get(fn, "?")
        if "start_new_season" not in name:
            continue        # a shared function of other slots too (shared_noop_ret0_5a8f60): no season start
        lines += [f"/* {name}, {len(targets[fn])} classes */",
                  f"void o_{fn:08x}(void);",
                  f"RT_WRAP({fn:08x}, start_{fn:06x})",
                  "{",
                  "    uint32_t self = cpu.ecx;",
                  "    season_change_start(self, 0);",
                  "    if (rt_unwinding) return;",
                  f"    o_{fn:08x}();",
                  "    if (rt_unwinding) return;",
                  "    season_change_start(self, 1);",
                  "}",
                  ""]
    lines.append("")
    return "\n".join(lines)


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", required=True)
    ap.add_argument("--classes", required=True)
    ap.add_argument("--out", default=OUT)
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args(argv)
    text = make(a.exe, a.classes)
    if a.check:
        old = open(a.out, encoding="utf-8").read() if os.path.exists(a.out) else ""
        if old != text:
            print(f"make_start_wraps.py: {a.out} differs from the tool's output: run the tool")
            return 1
        print("make_start_wraps.py: the file is up to date")
        return 0
    with open(a.out, "w", encoding="utf-8") as f:
        f.write(text)
    print(f"make_start_wraps.py: {text.count('RT_WRAP(')} wraps")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
