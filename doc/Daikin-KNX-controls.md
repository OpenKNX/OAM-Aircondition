# Daikin controls and telemetry in ETS

The additional objects are appended to the AIR object table. Existing object
numbers, IDs, sizes, DPTs and flags remain unchanged. The common driver APIs have
unsupported/no-op defaults, so other vendors are not assigned Daikin capabilities.
All status values are confirmed readings, not acknowledgements of a request.

## Fixed vane positions

The existing percent objects 421/422 expose numeric vertical codes for Daikin.
After FR discovery, codes 0–5 are addressable; discovery is not physical feature
certification. Zero holds the current angle, and an index is not a calibrated
angle. The explicit objects distinguish read support from confirmed writes:

| Object | Meaning |
| --- | --- |
| 466 VerticalPositionCode | Request 0–5, or 6 for the S21 `?` automatic marker. |
| 467 VerticalPositionCodeState | Confirmed code, including 6 for `?`. |
| 468 VerticalPositionMaximum | Addressable maximum 5 after FR discovery; 0 before discovery. |
| 469 VerticalPositionVerified | Bits 0–5 record matching post-write readbacks for changed vertical fields. |
| 470 VerticalPositionValid | Position-code sample validity; also check OnlineState. |
| 529 HorizontalPositionCode | Request 0–configured maximum, DPT 5.010. Zero holds. |
| 530 HorizontalPositionCodeState | Confirmed horizontal code, DPT 5.010. |
| 531 HorizontalPositionValid | Horizontal sample validity, DPT 1.002. |
| 532 HorizontalPositionMaximum | Configured model maximum once FR is readable, otherwise 0. |
| 533 HorizontalPositionVerified | DPT 7.001 bitmask, bits 0–8: changed horizontal codes confirmed by readback. |
| 534 LouvreWriteResult | Shared DR transaction result, using the write-result enumeration below. |

The horizontal installer profile defaults to 0 (disabled). A model-verified
maximum 1–8 enables codes 0 through that maximum; reading FR alone never enables
horizontal writes. Vertical uses DR/FR byte 0, horizontal byte 1. Each setter
preserves the other axis and unknown bytes 2–3, merging queued changes only after
prior readback. Swing should be stopped on the affected axis before requesting a
fixed position. The raw/code feedback does not independently establish movement.

Vertical percent objects represent only numeric positions, not the `?` marker.
Scenes expose vertical 0–5 and horizontal 0–8 (subject to the model maximum), plus
255 for no change. The automatic marker is only available through the explicit
vertical code API/object. Demand power limiting is not a Daikin scene field.
New profile/scene fields use reserved storage; existing offsets remain fixed.

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
Power and full-range energy feedback are described below.

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

Turning an individual control off only clears that control. Powerful excludes
both Econo and Outdoor Quiet; independent Econo and Quiet controls preserve one
another and can be requested together. An unverified first independent Econo-On
trial is rejected while Powerful is active. The explicit single-preset selector
(including scenes) instead replaces the other power presets: selecting Econo
clears Powerful first and waits for its readback. Comfort, Eye, Streamer and LED
fields are preserved. A negative FU00 Econo flag does not mask valid F7 feedback.
Readability does not guarantee execution in every operating mode.

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

## Power and energy

The new optional shared callback carries explicit watts and 64-bit Wh counters.
Other drivers default to no additional samples. New KNX objects are:

| Object | Meaning |
| --- | --- |
| 508 IndoorPower | FX60 watts, DPT 14.056. Raw S21 × 10 W; no further scaling. |
| 509 IndoorCoolingEnergy | FU04 indoor cooling lifetime energy, Wh, DPT 29.010 (signed 64 bit). |
| 510 IndoorHeatingEnergy | FU04 indoor heating lifetime energy, Wh, DPT 29.010. |
| 511 SystemEnergy | Independent FM shared outdoor/system counter, Wh, DPT 29.010. |
| 512 EnergyTelemetryValid | Validity bits: 0 power, 1 indoor cooling, 2 indoor heating, 3 system energy. |

FU04's maximum wire value is 429,496,729,500 Wh and fits these exact integer
objects without narrowing. Generic unsigned samples above INT64_MAX are marked
unavailable for their respective KNX object rather than wrapped. Invalid samples
retain their last numeric content; use the validity mask and OnlineState. FM is
not an indoor total or a sum of FU04 counters; resets/wraps are not guessed.
The existing TotalEnergy object and legacy 32-bit callbacks remain compatible.
`tests/knx/run.sh` exercises the real KNX library's DPT encoders/decoders for zero,
full S21 ranges and the signed-64 maximum under ASan/UBSan.

## Relative timers and timer lamp

Enable **Timer-Steuerung aktivieren (Modellwerte prüfen)** in ETS after checking
the unit's timer range. This parameter defaults to No, so updating an existing
project introduces no automatic timer writes. Step and maximum are stored in
reserved bytes and applied at startup. Defaults are 60 and 720 minutes, respectively;
these are installer choices, not a claim about a particular model. Step must be a
positive multiple of 10; maximum must be a multiple of step and at most 1,440.
New objects are visible when the timer parameter is enabled:

| Object | Meaning |
| --- | --- |
| 513/514 TimerOnMinutes / TimerOffMinutes | Set relative on/off delay in minutes (DPT 7.006). A nonzero value enables that timer; zero disables it. |
| 515/516 TimerOnMinutesState / TimerOffMinutesState | Confirmed configured delays. These do not count down. |
| 517/518 TimerOnEnable / TimerOffEnable | Disable, or re-enable with the last known positive delay for that axis. An unknown delay is rejected. |
| 519/520 TimerOnEnabledState / TimerOffEnabledState | Confirmed enabled flags. |
| 521/522 TimerLamp / TimerLampState | Set the independent orange timer lamp with DB and read it with FB, not the D6 unit/sensor LED. |
| 523 TimerStatusValid | Bit 0: timer sample valid; bit 1: lamp sample valid. |
| 524 TimerControlAvailable | Bit 0: F3 read discovered; 1: FB read discovered; 2: step/range configured; 3/4: on/off disabled encoding observed. This does not certify write support. |
| 525 TimerWriteResult | Last timer request/transaction result. |
| 526 TimerLampWriteResult | Last lamp request/transaction result. |
| 527/528 TimerConfiguredStep / TimerConfiguredMaximum | Actual configured step and maximum in minutes. |

Write results: 0 None/no recorded transaction, 1 Queued, 2 Awaiting ACK, 3 Awaiting
readback, 4 Confirmed, 5 Rejected (NAK), 6 Timeout, 7 Transport error, 8 Readback
mismatch, 9 Invalid request, 10 Unavailable prerequisites. A matching post-write
readback is required for Confirmed; an ACK alone never changes numeric or enabled
status. An unchanged request need not create a new transaction and can retain
the prior result. Fields without valid samples retain their last value and must be checked
against TimerStatusValid and OnlineState.

The unit must provide a valid F3/FB sample before the corresponding KNX control
is accepted. Disabled timer bytes are learned separately for each axis from F3
(`0x30` or `0xFE`); they are not guessed. If a unit boots with both timers enabled,
disable via its remote once to establish the relevant encoding before using the
KNX disable control. Re-enabling requires an observed or explicitly requested
positive delay during the current communication session; restart/link loss clears
the remembered delays. Setting a nonzero delay directly remains the way to
program a timer without a previously remembered value.

Changes to one timer preserve the other timer's latest confirmed configured
setting. Queued writes merge against fresh readback so an earlier change cannot
be overwritten by stale data. S21 writes both fields together and evaluates them
relative to programming time; changing a timer may also restart the other timer's
reference. The driver does not invent remaining time or a wall-clock schedule.
The production lamp setter writes DB and confirms with independent FB/GB
readback. All four bytes are preserved except the requested lamp byte. Legacy
raw FB-with-payload diagnostic transactions may drain an optional GB reply; they
are not the production setter.

The minute objects retain DPT 7.006 metadata in ETS. Their wire integer is encoded
directly with the equivalent unsigned-16 codec to avoid the local-time conversion
in this version of the KNX library's time-period encoder. The real codec tests
verify the declared minute decoding in UTC and Europe/Vienna.

## ETS update compatibility and validation

Release/beta retain application number 0x36 and use application 0.15; development
retains 0x35 and uses 1.3. Both replacement lists include their preceding versions.
The 0.15 update accommodates LogicModule 4.4; its available-channel UI is now
read-only and defaults to the compiled channel count. Product/order/manufacturer
identities and the 9,298-byte parameter area remain unchanged.

Deploy firmware and product as a matching pair. Preserve project parameters and
object associations when replacing an application; a local product import alone
does not migrate a configured device. Use the appropriate ETS application-version
update/replacement workflow. The automatic Update button documented by KNX
requires an online-catalog update; see [KNX's application update documentation](https://support.knx.org/hc/en-us/articles/360022260019-Change-Update-Application-Program).

`tests/ets/check_compatibility.py` compares against the frozen 0.11 contract. Later
upstream module metadata/default changes intentionally exceed that strict
baseline, so it is not a blanket certification of lossless upgrades. Its guard
self-tests and product XML/header checks are available locally. Full signing,
XSD validation and actual ETS replacement require the corresponding toolchain.
Native and firmware checks do not certify physical device behavior or ETS
project replacement.

## Driver connection lifecycle

Fan command diagnostics print the floating-point KNX percentage rounded to a
whole percent; the fan-step conversion and wire encoding are unchanged.

The module leaves Starting/Error when valid S21 traffic establishes communication.
Connection state is compared with the module's reported state, independently of
the telemetry online flag. A healthy connection therefore clears the module's
60-second recovery timer. Actual loss of traffic still reports Error and permits
recovery; startup without a response remains Starting until detection fails.

Timer-indicator writes modify only DB byte 0 and preserve the other three bytes
from the confirmed FB/GB register response. Those fields are model-dependent and are not
reset to invented defaults. Success still requires a subsequent matching readback.

Local raw diagnostics retain the payload of a correctly addressed frame even if
its field encoding is not recognised. Such replies keep Error status and never
update typed telemetry; `s21 result` can show the bytes for model investigation
instead of reporting a misleading empty response.

F3 timer enable bits are authoritative. A disabled axis may retain a valid
encoded delay (observed reply `30 32 30 30`) rather than returning only `30` or
`FE`. Such replies are accepted without reporting an active timer. Writes to the
other axis preserve the observed disabled-axis encoding. Out-of-range delays
and enabled axes with zero/disabled encodings are still rejected.

The local status console displays Unknown for uninitialised LED/humidity state,
Unavailable for humidity control excluded by F2, and distinguishes LED Dark
from Off. Default booleans or uninterpreted humidity fields are not presented
as confirmed optional settings.
