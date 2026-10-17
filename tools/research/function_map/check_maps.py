"""Checks the data map after each wave. Exit 1 on any failure, with one line per failure.

- Every global in scope (globals.pkl in the work folder) has exactly one row in globals.tsv.
- Each column holds a value from its set, and each reader and writer is a function of the function map.
- Each link in the data map pages resolves to a file.
- Each high row names a quote. Each address in its evidence is a function, a code address, a global or
  a string of the original, and each g_ name it quotes is a high global.
- No file of the two maps holds pseudocode or disassembly of the original.

Usage: CM0102_EXE=... FUNCMAP_WORK=... check_maps.py [--selftest]
--selftest plants one bad row of each kind in a copy of the data map and checks that each fails.
"""
import csv
import glob
import os
import re
import shutil
import sys
import tempfile

from common import MAP_DIR, REPO, image, load

DATA_DIR = os.path.join(REPO, "docs", "research", "data-map")
CONFIDENCE = {"high", "medium", "low"}
MODELS = {"script", "sonnet", "opus"}
TYPES = re.compile(r"^(u8|s8|u16|s16|u32|s32|float|double|long double|char \*|vtable|object|table|buffer|unknown|"
                   r"[A-Za-z_][A-Za-z0-9_ ]*\*?(\[[0-9a-fx]+\])?)$")
NAME = re.compile(r"^g_[a-z0-9_]+$")
QUOTE = re.compile(r"\"[^\"]*\"|\bassert\b|0x[0-9a-f]{6}\b|\bg_[a-z0-9_]+|\.rdata")
CODE = re.compile(r"```|\bDAT_00[0-9a-f]{6}\b|\bFUN_00[0-9a-f]{6}\(|\bundefined[1248] \*?[a-zA-Z_]\w*;|"
                  r"\b(mov|push|lea|call)\s+(e[abcd]x|e[sd]i|ebp|esp|dword ptr)\b|\(\*\(\w+ \*\)")


def string_at(img, a):
    try:
        b = img.read(a, 200).split(b"\0")[0]
    except Exception:
        return False
    return len(b) >= 1 and all(32 <= c < 127 or c in (9, 10, 13) for c in b)


def check(data_dir, scope, functions, img):
    fails = []
    tsv = os.path.join(data_dir, "globals.tsv")
    rows = list(csv.DictReader(open(tsv), delimiter="\t"))
    seen = {}
    for r in rows:
        a = r["address"]
        if a in seen:
            fails.append(f"{a}: more than one row")
        seen[a] = r
    for a in sorted(scope - {int(x, 16) for x in seen if re.fullmatch(r"0x[0-9a-f]{6}", x)}):
        fails.append(f"0x{a:06x}: global in scope without a row")
    for a in seen:
        if not re.fullmatch(r"0x[0-9a-f]{6}", a) or int(a, 16) not in scope:
            fails.append(f"{a}: row for an address outside the scope")
    names = {r["name"]: r for r in rows}
    for r in rows:
        a = r["address"]
        if r["confidence"] not in CONFIDENCE:
            fails.append(f"{a}: confidence {r['confidence']!r}")
        if r["model"] not in MODELS:
            fails.append(f"{a}: model {r['model']!r}")
        if not NAME.match(r["name"]):
            fails.append(f"{a}: name {r['name']!r}")
        if not r["size"].isdigit():
            fails.append(f"{a}: size {r['size']!r}")
        if not TYPES.match(r["type"]):
            fails.append(f"{a}: type {r['type']!r}")
        for col in ("readers", "writers"):
            for e in filter(None, r[col].split(",")):
                if not re.fullmatch(r"0x[0-9a-f]{6}", e) or int(e, 16) not in functions:
                    fails.append(f"{a}: {col} entry {e} is not a function of the function map")
    if len(names) != len(rows):
        fails.append(f"{len(rows) - len(names)} names belong to more than one global")
    # Evidence of the high rows, from the pages.
    for page in glob.glob(f"{data_dir}/*.md"):
        text = open(page).read()
        for m in re.finditer(r"\]\(([^)#]+)(#[^)]*)?\)", text):
            target = os.path.normpath(os.path.join(os.path.dirname(page), m.group(1)))
            if not m.group(1).startswith("http") and not os.path.exists(target):
                fails.append(f"{os.path.basename(page)}: link {m.group(1)} does not resolve")
        for line in text.splitlines():
            if not line.startswith("| 0x"):
                continue
            cells = [c.strip() for c in line.strip("|").split("|")]
            a, name, evidence, conf = cells[0], cells[3].strip("`"), cells[5], cells[6]
            if conf != "high":
                continue
            if not QUOTE.search(evidence):
                fails.append(f"{a}: high without a quote")
                continue
            for q in re.findall(r"0x([0-9a-f]{6})\b", evidence):
                v = int(q, 16)
                if v not in functions and v not in scope and not img.is_code(v) and not string_at(img, v):
                    fails.append(f"{a}: evidence quotes 0x{q}, which is no function, global or string")
            for q in re.findall(r"\bg_[a-z0-9_]+", evidence):
                if q == name:
                    continue
                if q not in names:
                    fails.append(f"{a}: evidence quotes {q}, which is no global")
                elif names[q]["confidence"] != "high":
                    fails.append(f"{a}: evidence quotes {q}, which is not high")
    for d in (data_dir, MAP_DIR):
        for p in glob.glob(f"{d}/*.md") + glob.glob(f"{d}/*.tsv"):
            for n, line in enumerate(open(p, errors="ignore"), 1):
                if CODE.search(line):
                    fails.append(f"{os.path.basename(p)}:{n}: looks like code of the original")
                    break
    return fails


def selftest(scope, functions, img):
    base = check(DATA_DIR, scope, functions, img)
    if base:
        print("The data map fails before the self test:")
        print("\n".join(base[:20]))
        return 1
    # Each plant: the change, and a part of the failure line it must cause.
    plants = {
        "missing row": (lambda rows, pages: rows.pop(1), "without a row"),
        "double row": (lambda rows, pages: rows.append(rows[1]), "more than one row"),
        "bad confidence": (lambda rows, pages: rows.__setitem__(1, _set(rows[1], 6, "sure")), "confidence 'sure'"),
        "bad reader": (lambda rows, pages: rows.__setitem__(1, _set(rows[1], 4, "0x000001")), "not a function of the function map"),
        "bad link": (lambda rows, pages: pages.append("| x | [gone](no-such-page.md) |"), "does not resolve"),
        "high without quote": (lambda rows, pages: pages.append("| 0x000000 | 4 | u32 | `g_x` | d | because it is | high | opus |  |  |  |"), "high without a quote"),
        "quote of a missing function": (lambda rows, pages: pages.append("| 0x000000 | 4 | u32 | `g_x` | d | written by 0x000010 | high | opus |  |  |  |"), "no function, global or string"),
        "quote of a medium global": (lambda rows, pages: pages.append(f"| 0x000000 | 4 | u32 | `g_x` | d | set from {_medium(rows)} | high | opus |  |  |  |"), "which is not high"),
        "pseudocode": (lambda rows, pages: pages.append("  uVar1 = DAT_00b67a34;"), "looks like code"),
    }
    bad = 0
    for what, (plant, expect) in plants.items():
        tmp = tempfile.mkdtemp()
        try:
            d = os.path.join(tmp, "data-map")
            shutil.copytree(DATA_DIR, d)
            os.symlink(MAP_DIR, os.path.join(tmp, "function-map"))
            rows = open(f"{d}/globals.tsv").read().splitlines()
            pages = []
            plant(rows, pages)
            open(f"{d}/globals.tsv", "w").write("\n".join(rows) + "\n")
            if pages:
                open(f"{d}/planted.md", "w").write("\n".join(pages) + "\n")
            fails = check(d, scope, functions, img)
            hit = [f for f in fails if expect in f]
            bad += not hit
            print(f"{'fails as it should' if hit else 'WRONG'}: {what}: {hit[0] if hit else fails[:1]}")
        finally:
            shutil.rmtree(tmp)
    return 1 if bad else 0


def _set(row, col, value):
    f = row.split("\t")
    f[col] = value
    return "\t".join(f)


def _medium(rows):
    for r in rows[1:]:
        f = r.split("\t")
        if f[6] != "high":
            return f[3]
    return "g_none"


def main(argv):
    img = image()
    scope = set(load("globals.pkl")["G"])
    functions = {int(r["address"], 16) for r in csv.DictReader(open(os.path.join(MAP_DIR, "names.tsv")), delimiter="\t")}
    if "--selftest" in argv:
        return selftest(scope, functions, img)
    fails = check(DATA_DIR, scope, functions, img)
    print("\n".join(fails[:200]))
    print(f"{len(fails)} failures" if fails else "data map check passes")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
