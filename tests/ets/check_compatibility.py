#!/usr/bin/env python3
"""Check additions against the pre-extension release product (4c8ea0c).

Usage: python3 tests/ets/check_compatibility.py expanded.debug.xml generated.h
The baseline freezes old product identities, wire types, parameter values/storage,
references and all existing generated addresses. It does not certify an ETS import.
"""
import argparse
import gzip
import json
import re
from pathlib import Path
import xml.etree.ElementTree as ET

BASE = Path(__file__).with_name('compatibility_baseline.json.gz')
NS = '{http://knx.org/xml/project/20}'

def contract(xml, header):
    root = ET.parse(xml).getroot()
    app = next(root.iter(NS + 'ApplicationProgram'))
    app_id = app.attrib['Id']
    def norm(value):
        return re.sub(r'M-00FA_BG-AE\.2F3[56]\.2F[0-9A-F]{2}-', '%BAGGAGE%', value.replace(app_id, '%AID%'))
    def attrs(e):
        return {k: norm(v) for k, v in e.attrib.items() if not k.startswith('{')}
    parents = {child: parent for parent in root.iter() for child in parent}
    objects, parameters, refs, types = {}, {}, {}, {}
    for e in root.iter():
        tag = e.tag.removeprefix(NS)
        ident = norm(e.get('Id', ''))
        if tag == 'ComObject':
            objects[ident] = attrs(e)
        elif tag == 'Parameter':
            entry = {'attributes': attrs(e)}
            memory = e.find(NS + 'Memory')
            if memory is not None:
                entry['memory'] = attrs(memory)
            parent = parents[e]
            if parent.tag == NS + 'Union':
                entry['union'] = attrs(parent)
                entry['memory'] = attrs(parent.find(NS + 'Memory'))
            parameters[ident] = entry
        elif tag in ('ParameterRef', 'ComObjectRef'):
            refs[ident] = attrs(e)
        elif tag == 'ParameterType':
            # Existing enum values and encodings must stay stable; additions are allowed.
            children = []
            for child in e.iter():
                if child is not e:
                    children.append([child.tag.removeprefix(NS), attrs(child)])
            types[ident] = children
    defines = {}
    for line in Path(header).read_text().splitlines():
        m = re.match(r'#define\s+(\w+)\s+(.+?)(?:\s+//.*)?$', line)
        if m and m[1] not in ('MAIN_ApplicationVersion', 'MAIN_MaxKoNumber'):
            defines[m[1]] = m[2].strip()
    return {'applicationNumber': app.attrib['ApplicationNumber'], 'version': int(app.attrib['ApplicationVersion']),
            'replaces': [int(v) for v in app.attrib.get('ReplacesVersions', '').split()],
            'objects': objects, 'parameters': parameters, 'refs': refs, 'types': types, 'defines': defines}

def check(current, baseline):
    assert current['applicationNumber'] == baseline['applicationNumber'], 'Application identity changed'
    assert current['version'] > baseline['version'], 'Application version must increase'
    assert set(baseline['replaces'] + [baseline['version']]) <= set(current['replaces']), 'Previous versions missing from ReplacesVersions'
    for section in ('objects', 'parameters', 'refs', 'defines'):
        for key, value in baseline[section].items():
            assert current[section].get(key) == value, f'{section}: changed or missing {key}'
    for key, children in baseline['types'].items():
        assert key in current['types'], f'Missing type {key}'
        for child in children:
            if child[0] == 'TypeNumber' and key.endswith(('_PT-KONumber', '_PT-KOAbsNumber', '_PT-KORelNumber')):
                # Appended objects increase the valid KO selector range.
                candidates = [c for c in current['types'][key] if c[0] == child[0]]
                assert len(candidates) == 1 and int(candidates[0][1]['maxInclusive']) >= int(child[1]['maxInclusive'])
                revised = dict(candidates[0][1]); revised['maxInclusive'] = child[1]['maxInclusive']
                assert int(revised['minInclusive']) <= int(child[1]['minInclusive'])
                revised['minInclusive'] = child[1]['minInclusive']
                assert revised == child[1]
            else:
                assert child in current['types'][key], f'Type encoding changed: {key}: {child}'
    old_max = max(int(o['Number']) for o in baseline['objects'].values())
    for key, obj in current['objects'].items():
        if key not in baseline['objects']:
            assert int(obj['Number']) > old_max, f'New object reuses an existing/reserved slot: {key}'
    print(f"ETS compatibility contract passed: {len(baseline['objects'])} objects, {len(baseline['parameters'])} parameters and {len(baseline['defines'])} header definitions preserved")

if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('xml'); parser.add_argument('header')
    parser.add_argument('--variant', choices=('release','beta','dev'), default='release')
    args=parser.parse_args()
    baseline=json.loads(gzip.decompress(BASE.read_bytes()))
    # Pre-extension wrappers use identical object/parameter definitions. Only
    # their existing application identity, version history and name differ.
    if args.variant=='beta':
        baseline['defines']['MAIN_FirmwareName']='"Klimaanlagen KNX Adapter (Beta)"'
    elif args.variant=='dev':
        baseline['applicationNumber']='44597'
        baseline['version']=15; baseline['replaces']=list(range(14,-1,-1))
        baseline['defines']['MAIN_ApplicationNumber']='53'
        baseline['defines']['MAIN_FirmwareName']='"Klimaanlagen KNX Adapter (Dev)"'
    check(contract(args.xml,args.header), baseline)
