#!/usr/bin/env python3
import sys
import json
import re

def normalize_text(s):
    if s is None:
        return ""
    # normalize line endings, collapse whitespace, strip
    s = s.replace('\r', '\n')
    s = re.sub(r"\s+", " ", s)
    return s.strip()

def flatten_cells(tables):
    flat = []
    for ti, table in enumerate(tables):
        data = table.get('data', [])
        # data is an array of areas -> rows -> cells with 'text'
        # We'll flatten in row-major order across areas and rows
        for area in data:
            for row in area:
                for cell in row:
                    txt = cell.get('text') if isinstance(cell, dict) else None
                    flat.append(normalize_text(txt))
    return flat

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: compare_jsons.py expected.json actual.json')
        sys.exit(2)
    a_path, b_path = sys.argv[1], sys.argv[2]
    with open(a_path, 'r', encoding='utf-8') as f:
        a = json.load(f)
    with open(b_path, 'r', encoding='utf-8') as f:
        b = json.load(f)

    a_flat = flatten_cells(a)
    b_flat = flatten_cells(b)

    n = max(len(a_flat), len(b_flat))
    diffs = []
    for i in range(n):
        av = a_flat[i] if i < len(a_flat) else ''
        bv = b_flat[i] if i < len(b_flat) else ''
        if av != bv:
            diffs.append((i, av, bv))

    if not diffs:
        print('OK: JSON cell texts match (normalized) —', len(a_flat), 'cells compared')
        sys.exit(0)

    print('DIFFS: {} mismatched cells'.format(len(diffs)))
    for idx, av, bv in diffs[:50]:
        print('--- Cell idx', idx)
        print('JAVA:', repr(av))
        print('C++ :', repr(bv))
        print()
    if len(diffs) > 50:
        print('... plus', len(diffs)-50, 'more differences')
    sys.exit(1)
