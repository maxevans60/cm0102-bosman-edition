"""Makes src/recomp_rt/win/season_wc48_thirds.c from docs/research/2026-10-19-wc2026-thirds.tsv (0.6.0 slice 29).

The table gives, for each of the 495 sets of eight groups whose third-placed teams reach the round of 32 of the
2026 World Cup, the group of the third that meets the winner of A, B, D, E, G, I, K and L. Source: Annex C of the
FIFA World Cup 26 regulations, as listed by the Wikipedia template "2026 FIFA World Cup third-place table" (read
2026-10-19). Run: python3 tools/seasons/make_wc_thirds.py
"""
import itertools
import os

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SRC = os.path.join(ROOT, "docs/research/2026-10-19-wc2026-thirds.tsv")
OUT = os.path.join(ROOT, "src/recomp_rt/win/season_wc48_thirds.c")
WINNERS = "ABDEGIKL"
ALLOWED = {"A": "CEFHI", "B": "EFGIJ", "D": "BEFIJ", "E": "ABCDF", "G": "AEHIJ", "I": "CDFGH", "K": "DEIJL",
           "L": "EHIJK"}


def main():
    rows = []
    with open(SRC, encoding="utf-8") as f:
        next(f)
        for line in f:
            option, groups, vs = line.rstrip("\n").split("\t")
            assert len(groups) == 8 and sorted(groups) == sorted(vs), option
            for w, g in zip(WINNERS, vs):
                assert g in ALLOWED[w], (option, w, g)
            rows.append((int(option), groups, vs))
    assert [r[0] for r in rows] == list(range(1, 496))
    assert {r[1] for r in rows} == {"".join(c) for c in itertools.combinations("ABCDEFGHIJKL", 8)}
    out = ["/* The third-placed teams of the 2026 World Cup round of 32 (0.6.0 slice 29, season_wc48.c). Made by",
           " * tools/seasons/make_wc_thirds.py from docs/research/2026-10-19-wc2026-thirds.tsv: do not edit by hand.",
           " * Each row: the groups of the eight thirds (bit 0 = A), then the group (0 = A) of the third that meets the",
           " * winner of A, B, D, E, G, I, K and L. */",
           "#include \"season_wc48.h\"", "",
           "const SeasonWcThirds season_wc48_thirds[495] = {"]
    for option, groups, vs in rows:
        mask = sum(1 << (ord(g) - 65) for g in groups)
        out.append("    {0x%03x, {%s}},   /* %d: %s */" % (mask, ", ".join(str(ord(g) - 65) for g in vs), option,
                                                         groups))
    out.append("};")
    with open(OUT, "w", encoding="utf-8") as f:
        f.write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()
