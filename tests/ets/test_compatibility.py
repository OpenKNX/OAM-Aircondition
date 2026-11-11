#!/usr/bin/env python3
"""Ensure the ETS upgrade guard rejects changes that would invalidate projects."""
import contextlib
import copy
import gzip
import io
import json
import unittest
from check_compatibility import BASE, check

class CompatibilityGuardTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.baseline=json.loads(gzip.decompress(BASE.read_bytes()))

    def setUp(self):
        self.current=copy.deepcopy(self.baseline)
        self.current['version']+=1
        self.current['replaces'].append(self.baseline['version'])

    def verify(self):
        with contextlib.redirect_stdout(io.StringIO()):
            check(self.current,self.baseline)

    def test_additive_upgrade(self):
        old_max=max(int(o['Number']) for o in self.baseline['objects'].values())
        self.current['objects']['new']={'Number':str(old_max+1)}
        self.verify()

    def test_changed_object_encoding(self):
        next(iter(self.current['objects'].values()))['ObjectSize']='14 Bytes'
        with self.assertRaises(AssertionError): self.verify()

    def test_changed_parameter_default(self):
        next(iter(self.current['parameters'].values()))['attributes']['Value']='changed'
        with self.assertRaises(AssertionError): self.verify()

    def test_changed_memory_address(self):
        parameter=next(p for p in self.current['parameters'].values() if 'memory' in p)
        parameter['memory']['Offset']='99999'
        with self.assertRaises(AssertionError): self.verify()

    def test_removed_reference(self):
        self.current['refs'].pop(next(iter(self.current['refs'])))
        with self.assertRaises(AssertionError): self.verify()

    def test_lost_replacement_history(self):
        self.current['replaces'].remove(self.baseline['version'])
        with self.assertRaises(AssertionError): self.verify()

    def test_new_object_in_old_number_space(self):
        self.current['objects']['new']={'Number':'16'}
        with self.assertRaises(AssertionError): self.verify()

    def test_changed_generated_offset(self):
        self.current['defines']['AIR_DeviceType']='99999'
        with self.assertRaises(AssertionError): self.verify()

if __name__=='__main__': unittest.main()
