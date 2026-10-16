"""Paths and shared loading for the function map scripts.

The work folder holds everything derived from the original (the decoded functions, the Ghidra
pseudocode, the dossiers and the model answers). It must stay outside the repo.
"""
import os
import pickle
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "recomp"))

from pe import Image  # noqa: E402

FUNCTIONS_TXT = os.path.join(REPO, "tools", "recomp", "functions.txt")
GEN_DIR = os.path.join(REPO, "build", "recomp", "gen")
MAP_DIR = os.path.join(REPO, "docs", "research", "function-map")


def work_dir():
    w = os.environ.get("FUNCMAP_WORK")
    if not w:
        raise SystemExit("Set FUNCMAP_WORK to a folder outside the repo")
    if os.path.abspath(w).startswith(REPO + os.sep):
        raise SystemExit("FUNCMAP_WORK must be outside the repo: it holds code derived from the original")
    os.makedirs(w, exist_ok=True)
    return w


def image():
    exe = os.environ.get("CM0102_EXE")
    if not exe:
        raise SystemExit("Set CM0102_EXE to the clean 3.9.68 cm0102.exe")
    return Image(exe)


def all_entries():
    """The functions the build translates: functions.txt plus the ones recomp.py finds through data."""
    entries = {int(x, 16) for x in open(FUNCTIONS_TXT).read().split()}
    for name in os.listdir(GEN_DIR):
        if name.endswith(".c"):
            for m in re.finditer(r"^[A-Za-z][^;\n]*\bf_00([0-9a-f]{6})\([^;\n]*$", open(os.path.join(GEN_DIR, name)).read(), re.M):
                entries.add(int(m.group(1), 16))
    return sorted(entries)


def load(name):
    return pickle.load(open(os.path.join(work_dir(), name), "rb"))


def save(obj, name):
    pickle.dump(obj, open(os.path.join(work_dir(), name), "wb"))
