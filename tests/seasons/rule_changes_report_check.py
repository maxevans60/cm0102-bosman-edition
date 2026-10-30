"""Test for tools/release/rule_changes_report.py with a small fixture."""
import os, sys
here = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(here, '..', '..', 'tools', 'release'))
import rule_changes_report as r

H = 'id season real_season nation comp_file comp tier name clubs up down playoffs split format subs_used bench foreign work_permit registration windows notes sources'.split()
def row(i, season, real, name, clubs, bench='n/a', comp='league', tier='1', nation='Testland', foreign='n/a',
        comp_file='tl'):
    d = dict.fromkeys(H, 'n/a'); d.update(id=i, season=season, real_season=real, nation=nation,
        comp_file=comp_file, comp=comp, tier=tier, name=name, clubs=clubs, bench=bench, foreign=foreign, sources='x')
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
    # ArgPrm holds 20 clubs (create_table 0x405552), so the real 30 of 2015 needs a mark.
    row('9', '2013/14', '2013-14', 'Primera', '20', nation='Argentina', comp_file='arg_prm'),
    row('10', '2014/15', '2015', 'Primera', '30', nation='Argentina', comp_file='arg_prm'),
    row('11', '2017/18', '2017-18', 'Primera', '28', nation='Argentina', comp_file='arg_prm'),
    row('12', '2019/20', '2020', 'Primera', '18', nation='Argentina', comp_file='arg_prm'),
    # A count under the floor that a cup of the nation needs (FLOOR_COUNT of tools/recomp/league_sizes.py).
    row('13', '2001/02', '2002', 'Scottish Premier', '12', nation='Scotland', comp_file='sco_prm'),
    row('14', '2002/03', '2003', 'Scottish Premier', '10', nation='Scotland', comp_file='sco_prm'),
    # The Serie C groups hold up to 60 in the Serie C era (season_ita_cap): no mark. Before it the C1 floor is 18.
    row('15', '2012/13', '2013', 'Serie C1/A', '18', nation='Italy', comp='Serie C1 group A', comp_file='ita_ser_c1a'),
    row('16', '2013/14', '2014', 'Serie C1/A', '16', nation='Italy', comp='Serie C1 group A', comp_file='ita_ser_c1a'),
    row('19', '2014/15', '2015', 'Serie C/A', '20', nation='Italy', comp='Serie C group A', comp_file='ita_ser_c1a'),
    row('22', '2015/16', '2016', 'Serie C/A', '16', nation='Italy', comp='Serie C group A', comp_file='ita_ser_c1a'),
    # League One plays 23 in 2019/20 (Q282): no mark there.
    row('20', '2018/19', '2019', 'League One', '24', nation='England', comp_file='eng_second'),
    row('21', '2019/20', '2020', 'League One', '23', nation='England', comp_file='eng_second'),
    # The Danish First Division keeps the clubs of the database up to 16 (season_sizes_keeps_database).
    row('17', '2010/11', '2011', 'Danish First', '16', nation='Denmark', comp_file='den_first'),
    row('18', '2011/12', '2012', 'Danish First', '12', nation='Denmark', comp_file='den_first'),
]
text = r.render(rows, '2025/26')
assert '- 2003/04 (real 2004): clubs changes from "18" to "20".' in text, text
assert '- 2002/03 (real 2003): bench changes from "5" to "7".' in text
assert 'The game keeps it at 9 (Q375).' in text
assert 'The game keeps 12 clubs (Q404).' in text
assert '- 2014/15 (real 2015): clubs changes from "20" to "30". The game keeps 20 clubs (league sizes).' in text, text
assert 'clubs changes from "30" to "28". The game keeps 20 clubs (league sizes).' in text
assert 'clubs changes from "28" to "18".\n' in text            # under the class count: the game plays it
assert 'clubs changes from "12" to "10". The game keeps 12 clubs (league sizes).' in text
assert 'clubs changes from "18" to "16". The game keeps 18 clubs (league sizes).' in text
assert 'clubs changes from "20" to "16".\n' in text            # Serie C era group: no mark
assert 'clubs changes from "24" to "23".\n' in text            # League One 2019/20: the game plays 23
assert '20 clubs at most in the Argentine Primera' in text      # the limits section names the league
assert 'clubs changes from "16" to "12". The game keeps the clubs of the database, up to 16 (league sizes).' in text
assert text.count('clubs changes') == 10          # unknown skipped, 2026/27 cut off
assert 'foreign players changes' not in text      # unknown and equal values are no change
assert '—' not in text and '–' not in text
print('ok')
