"""Test for tools/release/rule_changes_report.py with a small fixture."""
import os, sys
here = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(here, '..', '..', 'tools', 'release'))
import rule_changes_report as r

H = 'id season real_season nation comp_file comp tier name clubs up down playoffs split format subs_used bench foreign work_permit registration windows notes sources'.split()
def row(i, season, real, name, clubs, bench='n/a', comp='league', tier='1', nation='Testland', foreign='n/a'):
    d = dict.fromkeys(H, 'n/a'); d.update(id=i, season=season, real_season=real, nation=nation,
        comp_file='tl', comp=comp, tier=tier, name=name, clubs=clubs, bench=bench, foreign=foreign, sources='x')
    return d

rows = [
    row('1', '2001/02', '2002', 'Top', '18', bench='5'),
    row('2', '2002/03', '2003', 'Top', 'unknown', bench='7', foreign='3 foreigners'),
    row('3', '2003/04', '2004', 'Top', '20', bench='7', foreign='3 foreigners'),
    row('4', '2004/05', '2005', 'Top', '20', bench='12', foreign='3 foreigners'),
    row('5', '2005/06', '2006', 'Top', '20', bench='12', foreign='unknown'),
    row('6', '2026/27', '2027', 'Top', '10'),
    row('7', '2001/02', '2002', 'Major League Soccer', '10', nation='USA', tier='1'),
    row('8', '2002/03', '2003', 'Major League Soccer', '14', nation='USA', tier='1'),
]
text = r.render(rows, '2025/26')
assert '- 2003/04 (real 2004): clubs changes from "18" to "20".' in text, text
assert '- 2002/03 (real 2003): bench changes from "5" to "7".' in text
assert 'The game keeps it at 9 (Q375).' in text
assert 'The game keeps 12 clubs (Q404).' in text
assert text.count('clubs changes') == 2          # unknown skipped, 2026/27 cut off
assert 'foreign players changes' not in text      # unknown and equal values are no change
assert '—' not in text and '–' not in text
print('ok')
