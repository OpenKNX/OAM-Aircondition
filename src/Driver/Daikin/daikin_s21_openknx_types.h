#pragma once

#include <compare>
#include <functional>
#include <limits>
#include <type_traits>
#include "daikin_s21_openknx_fan_modes.h"

namespace daikin {

// forward declaration
class S21Driver;

class ProtocolVersion {
 public:
  uint8_t major{};
  uint8_t minor{};

  auto operator<=>(const ProtocolVersion&) const = default;
};

inline constexpr ProtocolVersion ProtocolUndetected{0xFF, 0xFF};
inline constexpr ProtocolVersion ProtocolUnknown{0,0xFF};  // treat as a protocol 0

/**
 * OpenKNX-compatible climate mode enumeration
 */
enum class Mode : uint8_t {
  Off = 0,
  Auto = 1,
  Cool = 2,
  Heat = 3,
  Dry = 4,
  FanOnly = 5
};

/**
 * OpenKNX-compatible climate action enumeration
 */
enum class Action : uint8_t {
  Off = 0,
  Idle = 1,
  Cooling = 2,
  Heating = 3,
  Drying = 4,
  Fan = 5,
  Auto = 6  // Auto mode without specific bias information
};

/**
 * Auto mode bias (from S21 mode byte '0'/'7')
 */
enum class AutoBias : uint8_t {
  Unknown = 0,
  Cooling = 1,  // S21 mode '0' - Auto with cooling bias
  Heating = 2   // S21 mode '7' - Auto with heating bias
};

/**
 * OpenKNX-compatible swing mode enumeration
 */
enum class Swing : uint8_t {
  Off = 0,
  Vertical = 1,
  Horizontal = 2,
  Both = 3
};

/**
 * OpenKNX-compatible preset enumeration
 */
enum class Preset : uint8_t {
  None = 0,
  Eco = 1,
  Comfort = 2,
  Boost = 3,
  Sleep = 4
};

/**
 * Humidity mode enumeration (F5 command, byte 2)
 */
enum class HumidityMode : uint8_t {
  Off = 0x00,
  Low = 0x3A,
  Standard = 0x3B,
  High = 0x3C,
  Continuous = 0xFF
};

/**
 * Class representing a temperature in degrees C scaled by 10, the most granular internal temperature measurement format
 */
class DaikinC10 {
 public:
  static constexpr auto nan_sentinel = 500; // internal Daikin 0x80

  constexpr DaikinC10() = default;

  template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
  constexpr DaikinC10(const T valf) : value((static_cast<int16_t>(valf * 10 * 2) + 1) / 2) {} // round to nearest 0.1C

  template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
  constexpr DaikinC10(const T vali) : value(vali) {}

  explicit constexpr operator float() const { return (value == nan_sentinel) ? NAN : (value / 10.0F); }
  explicit constexpr operator int16_t() const { return value; }
  constexpr float f_degc() const { return static_cast<float>(*this); }

  constexpr auto operator<=>(const DaikinC10 &other) const = default;
  constexpr DaikinC10 operator+(const DaikinC10 &arg) const { return this->value + arg.value; }
  constexpr DaikinC10 operator-(const DaikinC10 &arg) const { return this->value - arg.value; }
  constexpr DaikinC10 operator*(const DaikinC10 &arg) const { return this->value * arg.value; }
  constexpr DaikinC10 operator/(const DaikinC10 &arg) const { return this->value / arg.value; }

  static DaikinC10 diff(const DaikinC10 &a, const DaikinC10 &b) { return std::abs(a.value - b.value); }

 private:
  int16_t value{};
};

inline constexpr DaikinC10 SETPOINT_STEP{1.0F}; // Daikin setpoint granularity
inline constexpr DaikinC10 TEMPERATURE_STEP{0.5F}; // Daikin temperature sensor granularity
inline constexpr DaikinC10 TEMPERATURE_INVALID{DaikinC10::nan_sentinel}; // NaN

/**
 * Possible sources of active flag.
 */
enum class ActiveSource : uint8_t {
  Unknown,
  CompressorOnOff,  // directly read from query
  UnitState,        // interpreted from unit state bitfield
  Unsupported,      // hardcoded to active
};

/**
 * Unit state (RzB2) bitfield decoder
 */
class DaikinUnitState {
 public:
  constexpr DaikinUnitState(const uint8_t value = 0U) : raw(value) {}
  constexpr bool powerful() const { return (this->raw & 0x1) != 0; }
  constexpr bool defrost() const { return (this->raw & 0x2) != 0; }
  constexpr bool active() const { return (this->raw & 0x4) != 0; }
  constexpr bool online() const { return (this->raw & 0x8) != 0; }
  uint8_t raw{};
};

/**
 * System state (RzC3) bitfield decoder
 */
class DaikinSystemState {
 public:
  constexpr DaikinSystemState(const uint8_t value = 0U) : raw(value) {}
  constexpr bool locked() const { return (this->raw & 0x01) != 0; }
  constexpr bool active() const { return (this->raw & 0x04) != 0; }
  constexpr bool defrost() const { return (this->raw & 0x08) != 0; }
  constexpr bool multizone_conflict() const { return (this->raw & 0x20) != 0; }
  uint8_t raw{};
};

struct ClimateSettings {
  bool power{false};
  Mode mode{Mode::Off};
  float targetC{23.0f};
  DaikinFanMode fan_mode{DaikinFanMode::Auto};
  Swing swing_v{Swing::Off};
  Swing swing_h{Swing::Off};
  bool powerful{false};
  bool econo{false};
  bool quiet{false};
  bool led{false};
  bool streamer{false};
  bool sensor{false};
  float sensor_temp{23.0f};

  constexpr bool operator==(const ClimateSettings &other) const = default;
};

/**
 * Complete state structure that represents the current AC unit status
 */
struct State {
  // Basic state
  bool online{false};
  bool power{false};
  Mode mode{Mode::Off};
  Action action{Action::Off};
  AutoBias autoBias{AutoBias::Unknown};  // S21 mode '0'/'7' distinction
  float targetC{23.0f};
  float realTargetC{23.0f};  // RX: Actual target temp with sensor adjustments
  float homeC{25.0f};
  float outsideC{25.0f};
  float liquidC{25.0f};
  
  // System status
  bool compressorOn{false};  // RG2: compressor state for action determination
  
  // Fan and swing
  DaikinFanMode fan{DaikinFanMode::Auto};
  Swing swing{Swing::Off};
  HumidityMode humidityMode{HumidityMode::Off};  // F5 byte 2: humidity setting
  uint16_t fanRpm{0};
  uint16_t fanRpmSetpoint{0};
  int16_t swingVerticalAngle{0};
  int16_t swingVerticalAngleSetpoint{0};
  
  // Advanced features and modifiers
  bool quiet{false};       // outdoor unit fan/compressor limit
  bool econo{false};       // limits demand for power consumption
  bool powerful{false};    // maximum output (20 minute timeout), mutually exclusive with quiet and econo
  bool comfort{false};     // fan angle depends on heating/cooling action
  bool streamer{false};    // electron emitter decontamination?
  bool sensor{false};      // "intelligent eye" PIR occupancy setpoint offset
  bool led{true};          // unit LED status
  
  // Sensors and telemetry
  uint8_t humidity{50};
  uint8_t demand{0};
  uint8_t demand_percentage{0}; // demand percentage (0-100)

  uint32_t totalEnergyWh{0};    // Total energy consumption in Watt-hours (from FM command)
  uint8_t compressorHz{0};
  uint16_t irCounter{0};
  
  // System status
  DaikinUnitState unitState{};
  DaikinSystemState systemState{};
  uint8_t protocolVersion{0};
};

/**
 * Communication statistics
 */
struct Stats {
  uint32_t frames_ok{0};
  uint32_t frames_bad_checksum{0};
  uint32_t frames_bad_format{0};
  uint32_t frames_nak{0};
  uint32_t acks{0};
  uint32_t resyncs{0};
};

// MiscQuery::Model or StateQuery::ModelCode responses, reversed ascii hex (these are byte swapped from controller response)
using DaikinModel = uint16_t;
inline constexpr DaikinModel ModelUnknown{0xFFFF};

// V0 outdoor units?
inline constexpr DaikinModel ModelRXB35C2V1B{0x2806}; // indoor FTXB25C2V1B
inline constexpr DaikinModel Model4MXL36TVJU{0x35E3}; // indoor CTXS07LVJU, FTXS12LVJU, FTXS15LVJU
inline constexpr DaikinModel ModelRXC24AXVJU{0x4431}; // indoor FTXC24AXVJU

// V2+ indoor units
inline constexpr DaikinModel ModelFTXC24AXVJU{0x0B66};  // outdoor RXC24AXVJU

} // namespace daikin