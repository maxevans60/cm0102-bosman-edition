"""Season settings of the own season databases, and the template facts of the date shift research.

settings(year) returns the module season_<year>.py. template_shifts(name), kept_careers(name) and
last_row_comps(name) read docs/research/rules-0.6.0/databases-date-shifts*.tsv for an Update Team database."""
import importlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
RULES = os.path.join(REPO, "docs", "research", "rules-0.6.0")


def settings(year):
    sys.path.insert(0, HERE)
    return importlib.import_module(f"season_{year}")


def _rows(fname):
    with open(os.path.join(RULES, fname)) as f:
        head = f.readline().rstrip("\n").split("\t")
        for line in f:
            yield head, line.rstrip("\n").split("\t")


def template_shifts(name):
    """Years that the database's fields are early by: birth, joined, contract, officials, staff_history,
    comp_history, last_rows (databases-date-shifts.tsv)."""
    for head, p in _rows("databases-date-shifts.tsv"):
        if p[0] == name:
            row = dict(zip(head, p))
            return {k: int(row[k]) for k in ("birth", "joined", "contract", "officials", "staff_history",
                                              "comp_history", "last_rows")}
    sys.exit(f"own_db: {name} is not in databases-date-shifts.tsv")


def kept_careers(name):
    """(bit per staff row: the career keeps its real years, staff row count)."""
    with open(os.path.join(RULES, "databases-date-shifts-careers.tsv")) as f:
        for line in f:
            p = line.rstrip("\n").split("\t")
            if p[0] == name:
                return bytes.fromhex(p[2]), int(p[1])
    return b"", 0


def last_row_comps(name):
    """Competition ids whose 2001 history row holds the template's last champions."""
    with open(os.path.join(RULES, "databases-date-shifts-last-rows.tsv")) as f:
        for line in f:
            p = line.rstrip("\n").split("\t")
            if p[0] == name:
                return {int(v) for v in p[1].split()}
    return set()


def repair_clubs(T, S_):
    """Season settings CLUB_REPAIR: {template long name: {"nation": template nation name, "stadium": stadium id}}.
    Sets the nation (+0x53) and stadium (+0x69) of a template club record that lacks them, in memory, before the
    match, the leagues and the conversion read it. Stadium ids are the same in the Update Team databases."""
    fix = getattr(S_, "CLUB_REPAIR", {})
    if not fix:
        return
    nat = {bytes(r[4:55]).split(b"\0")[0].decode("latin1"): struct.unpack_from("<i", r, 0)[0] for r in T.rows(4)}
    n_stadium = T.idx[5]["count"]
    done = set()
    for r in T.rows(0):
        name = bytes(r[4:55]).split(b"\0")[0].decode("latin1")
        if name in fix:
            f = fix[name]
            if f["nation"] not in nat or not 0 <= f["stadium"] < n_stadium or name in done:
                sys.exit(f"own_db: CLUB_REPAIR {name}: no nation {f['nation']}, no stadium {f['stadium']} or two clubs")
            struct.pack_into("<i", r, 0x53, nat[f["nation"]])
            struct.pack_into("<i", r, 0x69, f["stadium"])
            done.add(name)
    if done != set(fix):
        sys.exit(f"own_db: CLUB_REPAIR names no template club: {sorted(set(fix) - done)}")
