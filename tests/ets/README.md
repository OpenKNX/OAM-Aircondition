# ETS update compatibility regression

`compatibility_baseline.json.gz` is a deterministic gzip JSON snapshot of the
expanded release 0.10 product at `4c8ea0c`, generated with OpenKNXproducer 4.3.12.
It freezes all 395 objects, 28,842 parameters, parameter/object references,
parameter type encodings and 3,848 existing generated header definitions.
Application-specific baggage paths are normalized; existing storage, identities,
object sizes/DPTs/flags, defaults and generated offsets must remain unchanged.
Only appended object numbers and expanded KO-selector bounds are permitted.

Generate the release XML/header, then run:

```
OpenKNXproducer create src/Aircondition-Release.xml -h include/knxprod.h -o /tmp/Aircondition.knxprod -debug --ExchangeKoTexts
python3 tests/ets/check_compatibility.py src/Aircondition-Release.debug.xml include/knxprod.h
```

The new version must retain the application number, increase its version and
include all old replacements plus version 0.10 in `ReplacesVersions`. Development
products retain application number 0x35, advance 0.15 to 0.16 and replace 0.15;
release/beta retain application number 0x36 and advance 0.10 to 0.11.

These structural checks preserve the prerequisites for updating existing ETS
project settings and associations. A signed `.knxprod` export/import and actual
ETS application replacement still require ETS and its converter/XSD installation.
Do not claim that a changed application has been commissioned without that test.
