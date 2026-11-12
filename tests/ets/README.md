# ETS update compatibility regression

`compatibility_baseline.json.gz` is a deterministic gzip JSON snapshot of the
expanded release 0.11 product at `63f5dc2`, generated with OpenKNXproducer 4.3.12.
It freezes all 437 objects, 28,853 parameters, parameter/object references,
parameter type encodings and 3,956 existing generated header definitions.
The beta/dev CLI variants preserve their original wrapper identity and version
history while using the same frozen object/parameter definitions. The dev variant
was also checked against an independently generated pre-extension dev product.
Application-specific baggage paths are normalized; existing storage, identities,
object sizes/DPTs/flags, defaults and generated offsets must remain unchanged.
Only appended object numbers and expanded KO-selector bounds are permitted.

Generate the release XML/header, then run:

```
OpenKNXproducer create src/Aircondition-Release.xml -h include/knxprod.h -o /tmp/Aircondition.knxprod -debug --ExchangeKoTexts
python3 tests/ets/check_compatibility.py src/Aircondition-Release.debug.xml include/knxprod.h
# For a separately generated beta/dev product, add --variant beta or --variant dev.
```

The new version must retain the application number, increase its version and
include all old replacements plus version 0.11 in `ReplacesVersions`. Development
products retain application number 0x35, advance 1.0 to 1.1 and replace 1.0;
the preceding 0.15-to-1.0 rollover respected the 4-bit minor version.
Release/beta retain application number 0x36 and advance 0.11 to 0.12.

The subsequent OFM-Network 0.8 / OGM-Common 1.9 update advances release/beta
to 0.13 and development to 1.2. The strict 0.11 baseline check intentionally
reports upstream changes: latitude/longitude suffixes, the HTTP parameter/ref
identity and default, and removal of the unused comment module. Existing
communication objects and parameter storage offsets remain unchanged; this
does not establish lossless ETS application replacement across that update.

Run `PYTHONDONTWRITEBYTECODE=1 python3 tests/ets/test_compatibility.py` for the
eight positive/negative regression checks of the guard itself.

These structural checks preserve the prerequisites for updating existing ETS
project settings and associations. A signed `.knxprod` export/import and actual
ETS application replacement still require ETS and its converter/XSD installation.
Do not claim that a changed application has been commissioned without that test.
