"""Clubs per division of a database folder, for the 0.6.0 rules research.

Usage: db_divisions.py <database folder> <out folder>
Writes <out>/<folder name>.divisions.tsv (comp id, nation, comp name, clubs) and
<out>/<folder name>.clubs.tsv (club id, nation, division id, division name, club name).
club.dat: 581-byte records, nation id at +0x53, division (club_comp id) at +0x57.
The output holds database content: keep it outside the repo.
"""
import collections, os, struct, sys

def records(path, size):
    data = open(path, 'rb').read()
    return [data[i:i + size] for i in range(0, len(data) - size + 1, size)]

def name(rec, off=4, length=51):
    return rec[off:off + length].split(b'\0')[0].decode('latin1')

def main(folder, out):
    comps = {struct.unpack_from('<i', r, 0)[0]: name(r) for r in records(os.path.join(folder, 'club_comp.dat'), 107)}
    nations = {struct.unpack_from('<i', r, 0)[0]: name(r) for r in records(os.path.join(folder, 'nation.dat'), 290)}
    clubs = records(os.path.join(folder, 'club.dat'), 581)
    base = os.path.basename(folder.rstrip('/'))
    per_comp = collections.defaultdict(collections.Counter)
    with open(os.path.join(out, base + '.clubs.tsv'), 'w') as f:
        f.write('club_id\tnation\tdivision_id\tdivision\tclub\n')
        for c in clubs:
            cid, nat, div = struct.unpack_from('<i', c, 0)[0], struct.unpack_from('<i', c, 0x53)[0], struct.unpack_from('<i', c, 0x57)[0]
            if not name(c):
                continue
            per_comp[div][nat] += 1
            f.write(f"{cid}\t{nations.get(nat, nat)}\t{div}\t{comps.get(div, 'none')}\t{name(c)}\n")
    with open(os.path.join(out, base + '.divisions.tsv'), 'w') as f:
        f.write('comp_id\tnation\tcomp\tclubs\n')
        for div in sorted(per_comp):
            nat = per_comp[div].most_common(1)[0][0]
            f.write(f"{div}\t{nations.get(nat, nat)}\t{comps.get(div, 'none')}\t{sum(per_comp[div].values())}\n")

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
