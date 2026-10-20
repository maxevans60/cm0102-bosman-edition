#!/usr/bin/env python3
"""Writes a Markdown report of every rule change between consecutive seasons.

Reads the main rules files of docs/research/rules-0.6.0 (the files that are not
-clubs, check, check2, reconcile, databases or academy files). For each nation and
competition it lists the season where a field changes, with the old and new text.

A field value of unknown, n/a or empty is no information. It is skipped, and the
next known value is compared with the last known one, so a change is listed once
and an unknown season never shows as a change.

Usage: rule_changes_report.py [--rules DIR] [--out FILE] [--last-season 2025/26]
"""
import argparse
import collections
import csv
import glob
import os
import re
import sys

SKIP_PARTS = ('-clubs', '.check', '.reconcile', 'databases', 'academy')
FIELDS = [
    ('tier', 'tier'), ('name', 'name'), ('clubs', 'clubs'), ('up', 'promotion'),
    ('down', 'relegation'), ('playoffs', 'play-offs'), ('split', 'split'),
    ('format', 'format'), ('subs_used', 'substitutes used'), ('bench', 'bench'),
    ('foreign', 'foreign players'), ('work_permit', 'work permits'),
    ('registration', 'squad registration'), ('windows', 'transfer windows'),
]
GAME_BENCH_LIMIT = 9
MLS_CLUBS = 12

LIMITS = """\
## What the game will not do

A row of the rules files states the real rule. The game limits some of them, so the report lists the real change and marks the limit. Two limits apply in 0.6.0:

- The bench and the substitutes used stay at 9 or fewer (Q375). The match engine clamps at 9. A real rule of more than 9 shows in the report, and the game plays 9.
- The MLS stays at 12 clubs (Q404). The group code of the original holds 12. MLS expansion clubs get no place.

Source: docs/specs/2026-10-16-seasons-0.6.0-spec.md.
"""


def is_unknown(v):
    v = (v or '').strip().lower()
    return v == '' or v.startswith('unknown') or v.startswith('n/a')


def clean(v):
    return v.strip().replace('—', '-').replace('–', '-')


def main_files(folder):
    for f in sorted(glob.glob(os.path.join(folder, '*.tsv'))):
        b = os.path.basename(f)
        if not any(p in b for p in SKIP_PARTS):
            yield f


def load(folder):
    rows = []
    for f in main_files(folder):
        with open(f, encoding='utf-8', newline='') as fh:
            rows.extend(csv.DictReader(fh, delimiter='\t', quoting=csv.QUOTE_NONE))
    return rows


def season_key(s):
    m = re.match(r'(\d{4})', s)
    return int(m.group(1)) if m else 0


def tier_key(t):
    return int(t) if t.isdigit() else 99


def note(field, new, nation, comp, name):
    """Returns a short game-limit remark for a new value, or ''."""
    if field in ('bench', 'subs_used'):
        nums = [int(x) for x in re.findall(r'\d+', new)]
        if nums and max(nums) > GAME_BENCH_LIMIT:
            return ' The game keeps it at %d (Q375).' % GAME_BENCH_LIMIT
    if field == 'clubs' and 'major league soccer' in (name or '').lower():
        nums = [int(x) for x in re.findall(r'\d+', new)]
        if nums and max(nums) > MLS_CLUBS:
            return ' The game keeps %d clubs (Q404).' % MLS_CLUBS
    return ''


def changes(rows, last_season=None):
    """Returns {nation: {comp key: (title, [(season, real, text)])}}."""
    dup = set()
    seen = collections.Counter()
    for r in rows:
        seen[(r['nation'], r['comp_file'], r['comp'], r['tier'], r['season'])] += 1
    for k, n in seen.items():
        if n > 1:
            dup.add(k[:4])
    groups = collections.defaultdict(list)
    for r in rows:
        k = (r['nation'], r['comp_file'], r['comp'], r['tier'])
        if k in dup:
            k += (r['name'],)
        groups[k].append(r)
    out = collections.defaultdict(dict)
    for k, rs in groups.items():
        rs.sort(key=lambda r: season_key(r['season']))
        if last_season:
            rs = [r for r in rs if season_key(r['season']) <= season_key(last_season)]
        last = {}
        events = []
        for r in rs:
            for field, label in FIELDS:
                v = r[field]
                if is_unknown(v):
                    continue
                v = clean(v)
                if field in last and last[field] != v:
                    real = clean(r['real_season']) if r['real_season'] not in ('', 'n/a', r['season']) else ''
                    text = '%s changes from "%s" to "%s".' % (label, last[field], v)
                    events.append((r['season'], real, text + note(field, v, r['nation'], r['comp'], r['name'])))
                last[field] = v
        if events:
            title = r['name'] if r['comp'] != 'nation rules' else 'Nation rules'
            if r['name'] != 'n/a' and r['comp'] != 'nation rules':
                title = '%s (%s)' % (r['name'], r['comp'])
            out[k[0]][k] = (title, events)
    return out


def render(rows, last_season=None):
    ch = changes(rows, last_season)
    lines = ['# Rule changes by nation and season, 0.6.0 (draft)', '',
             'Every change between consecutive seasons in docs/research/rules-0.6.0, from 2001/02 to %s. '
             'Calendar-year leagues show the real year after the season. A value of unknown is skipped.' % (last_season or 'the last season'),
             '', LIMITS]
    total = 0
    for nation in sorted(ch):
        lines += ['## ' + nation, '']
        comps = sorted(ch[nation].items(), key=lambda kv: (kv[0][2] != 'nation rules', tier_key(kv[0][3]), kv[1][0]))
        for k, (title, events) in comps:
            lines += ['### ' + title, '']
            for season, real, text in events:
                lines.append('- %s%s: %s' % (season, ' (real %s)' % real if real else '', text))
                total += 1
            lines.append('')
    lines.insert(3, '%d changes in total.' % total)
    return '\n'.join(lines).rstrip() + '\n'


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.normpath(os.path.join(here, '..', '..'))
    ap = argparse.ArgumentParser()
    ap.add_argument('--rules', default=os.path.join(root, 'docs', 'research', 'rules-0.6.0'))
    ap.add_argument('--out', default=os.path.join(root, 'docs', 'release', '0.6.0-rule-changes.md'))
    ap.add_argument('--last-season', default='2025/26')
    a = ap.parse_args()
    text = render(load(a.rules), a.last_season)
    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with open(a.out, 'w', encoding='utf-8') as fh:
        fh.write(text)
    print('wrote', a.out)


if __name__ == '__main__':
    sys.exit(main())
