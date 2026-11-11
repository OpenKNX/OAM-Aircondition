# Daikin controls and telemetry in ETS

The additional objects are appended to the AIR object table. Existing object
numbers, IDs, sizes, DPTs and flags remain unchanged. The common driver APIs have
unsupported/no-op defaults, so other vendors are not assigned Daikin capabilities.
All status values are confirmed readings, not acknowledgements of a request.

## Fixed vane positions

The existing percent objects 421/422 are also visible for Daikin. A read FR page
permits explicit position probes; it does not certify physical support. The new
objects expose the distinction:

| Object | Meaning |
| --- | --- |
| 466 VerticalPositionCode | Request code 0…5, or 6 for S21 `?` (automatic marker). |
| 467 VerticalPositionCodeState | Confirmed code, including 6 for `?`. |
| 468 VerticalPositionMaximum | Addressable maximum, 5 after FR is discovered; 0 before discovery. |
| 469 VerticalPositionVerified | Bit 0…5 records positions with a write ACK and matching readback. |
| 470 VerticalPositionValid | Whether the last position-code sample is valid. |

The percent objects represent only numeric positions. They do not map `?` to a
physical position. Daikin scenes gain a position selector using a new parameter ID
at the previously reserved position byte; old scene settings and other vendors'
parameters retain their identities/defaults/offsets. No power-limit scene changes
are included. Unsupported positions can still NAK; each position needs a real
model/physical check before it is considered supported.

## Additional telemetry

Objects 471…486 carry the following values. Object 471 is a 16-bit validity mask;
bit numbering starts at zero. An invalid value retains its last numeric content;
consumers must check its validity bit and the existing OnlineState object.

| Bit | Object(s) | Value / units |
| --- | --- | --- |
| 0 | 472 IndoorCoilTemperature | RI indoor heat exchanger, °C (DPT 9.001). |
| 1 | 473 IndoorFanRpm | RL measured indoor fan speed, RPM. |
| 2 | 474 IndoorFanTargetRpm | RK target indoor fan speed, RPM. |
| 3 | 475 VaneAngleRaw | RN signed raw angle. |
| 4 | 476 VaneAngleTargetRaw | RM signed raw target angle. |
| 5 | 477 VaneAngleDegrees | Measured angle, degrees (DPT 14.007), only with configured model scaling. |
| 6 | 478 VaneAngleTargetDegrees | Target angle, degrees, only with configured model scaling. |
| 7 | 479 CompressorPull | Rb unit demand, 0…15. |
| 8 | 480 SerialErrorByte | Raw F4 byte 2; bit 5 is the read-cleared serial-interface error flag. This is not a decoded AC fault code. |
| 9 | 481 ProtocolVersion | Major × 256 + minor, e.g. v3.40 is 808 (`0x0328`). |
| 10 | 482 FirmwareVersion | VS firmware identifier, ASCII. |
| 11 | 483/484 ModelName/Continuation | Model text, first 14 and remaining characters; FC code is the fallback. |
| 12 | 485 CompressorFrequency | Rd frequency, Hz. |
| 13 | 486 CompressorRunning | Rg actual compressor on/off. |

The wiki establishes RK/RL for the indoor fan. No outdoor fan RPM or outdoor coil
temperature value is invented. Rm/Rt/RW and unknown extension-page fields remain
local raw diagnostics. Production/serial identifiers are not published to KNX.
Power/64-bit energy objects are a separate remaining task.

## Independent optional controls

New on/off controls and confirmed status pairs:

- 487/488 Comfort.
- 489/490 Intelligent Eye (hardware-dependent).
- 493/494 Powerful.
- 495/496 Econo.
- 497/498 Quiet.

Streamer keeps the existing AirPurification control/status pair. Object 499 is
the valid-status bitmask and 500 the available-for-probing bitmask. Their bit order
is Powerful, Econo, Quiet, Comfort, Intelligent Eye, Streamer (bits 0…5).
Availability means a read page exists and no known capability excludes the option;
it is not verified write support. An ACK alone does not change status.

Turning an individual control off only clears that control. Enabling Powerful,
Econo or Quiet queues the necessary clears for the other mutually exclusive power
presets. Comfort, Eye, Streamer and LED settings are preserved. Legacy preset
objects retain their existing behavior and associations.

For independent unit/sensor LED control, enable the new ETS parameter
**Geräte-/Sensor-LED separat steuern (ersetzt WLAN-LED-Regelung)**. Its default is
**No**, preserving the previous WLAN LED policy on updated projects. When enabled,
the automatic WLAN LED policy no longer writes Daikin D6. New objects are:

- 491 UnitLed: 0 On, 1 Dark, 2 Off.
- 492 UnitLedState: confirmed setting.
- 501 UnitLedLevels: allowed-setting bitmask (bits 0 On, 1 Dark, 2 Off).
- 502 UnitLedValid: status validity.

Intelligent Eye hardware uses sensor-LED On/Off only; Dark is rejected. Without
that hardware, model-dependent unit-LED On/Dark/Off is supported. FK hardware
identification and F6 readback are required before accepting these controls.
The new parameter occupies reserved module memory; existing addresses do not move.

## Qualitative humidity control

Object 503 requests a level; 504 reports the confirmed level:

| Value | Setting |
| --- | --- |
| 0 | Off |
| 1 | Low |
| 2 | Standard |
| 3 | High |
| 4 | Continuous |

Object 505 is the available-level bitmask (bits 0…4), 506 marks status validity,
and 507 reports the F2-derived operating-mode capability mask: bit 0 Auto, 1 Cool,
2 Heat, 4 Sarara Dry, 5 Ururu Humidify and 7 Laundry availability. A Laundry flag is
not a setter: the wiki has no verified Laundry command encoding. Unassigned bits
remain reserved. Simple Humidify units do not advertise level control.

Levels are checked against the current/pending operating mode, then translated to
the mode-specific D5 byte. Low is unavailable in Sarara/Ururu modes. Shared
register writes preserve swing settings. Unsupported or invalid requests are
ignored without publishing success. The existing measured RH and percentage
setting-status objects keep their original IDs, DPTs and flags; the setting enum
is distinct from measured percent humidity. Compound-mode selection remains
available through local `s21 mode …` commands; a dedicated common/KNX compound-mode
selector is a separate remaining task.

## ETS update compatibility and validation

Release/beta retain application number 0x36 and advance 0.10 → 0.11, explicitly
replacing 0.10 and its predecessors. Development retains application number 0x35
and advances 0.15 → 1.0; a 4-bit minor version cannot encode 0.16. Development
also retains 0.15 and all predecessors in its replacement list. Product/order and
manufacturer identities are unchanged.

The regression in `tests/ets/check_compatibility.py` checks the generated product
against the previous release. It preserves all 395 existing objects, 28,842
parameters, their references and encodings, and 3,848 header definitions. It also
requires an increased application version, the replacement history and strictly
appended object numbers. Development and beta are checked against their baselines
as well. Existing parameter memory size remains 9,298 bytes.

Use the ETS application-version update/replacement workflow appropriate to the
installed ETS version. The automatic Update button described by KNX requires an
online-catalog update; a local import alone does not establish that workflow.
See [KNX's application update documentation](https://support.knx.org/hc/en-us/articles/360022260019-Change-Update-Application-Program).
The firmware and product must be deployed as a matching pair.

OpenKNXproducer structural checks and XML/header generation pass for release,
beta and development. Final signed `.knxprod` creation, XSD validation and an actual
ETS project migration cannot be run here because ETS/converter/XSD support is not
installed. Native ASan/UBSan regressions and ESP32 TP/IP plus RP2040 firmware builds
validate the implementation. No device was flashed or commanded; hardware and ETS
acceptance remain pending.
