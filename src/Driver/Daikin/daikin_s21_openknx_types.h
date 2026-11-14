#pragma once

#include "daikin_s21_openknx_fan_modes.h"
#include <cstdint>
#include <cmath>
#include <array>
#include <string>
#include <functional>
#include <limits>
#include <type_traits>

namespace daikin
{

    // forward declaration
    class S21Driver;

    class ProtocolVersion
    {
      public:
        uint8_t major{};
        uint8_t minor{};

        // C++17 compatible comparison operators
        bool operator==(const ProtocolVersion &other) const
        {
            return major == other.major && minor == other.minor;
        }
        bool operator!=(const ProtocolVersion &other) const
        {
            return !(*this == other);
        }
        bool operator<(const ProtocolVersion &other) const
        {
            return major < other.major || (major == other.major && minor < other.minor);
        }
        bool operator<=(const ProtocolVersion &other) const
        {
            return *this < other || *this == other;
        }
        bool operator>(const ProtocolVersion &other) const
        {
            return !(*this <= other);
        }
        bool operator>=(const ProtocolVersion &other) const
        {
            return !(*this < other);
        }
    };

    inline constexpr ProtocolVersion ProtocolUndetected{0xFF, 0xFF};
    inline constexpr ProtocolVersion ProtocolUnknown{0, 0xFF}; // treat as a protocol 0

    /**
     * OpenKNX-compatible climate mode enumeration
     */
    enum class Mode : uint8_t
    {
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
    enum class Action : uint8_t
    {
        Off = 0,
        Idle = 1,
        Cooling = 2,
        Heating = 3,
        Drying = 4,
        Fan = 5,
        Auto = 6, // Auto mode without specific bias information
        Unknown = 7,
        Defrost = 8,
        Standby = 9
    };

    /**
     * Auto mode bias (from S21 mode byte '0'/'7')
     */
    enum class AutoBias : uint8_t
    {
        Unknown = 0,
        Cooling = 1, // S21 mode '0' - Auto with cooling bias
        Heating = 2  // S21 mode '7' - Auto with heating bias
    };

    /**
     * OpenKNX-compatible swing mode enumeration
     */
    enum class Swing : uint8_t
    {
        Off = 0,
        Vertical = 1,
        Horizontal = 2,
        Both = 3
    };

    /**
     * OpenKNX-compatible preset enumeration
     */
    enum class Preset : uint8_t
    {
        None = 0,
        Eco = 1,
        Comfort = 2,
        Boost = 3,
        Sleep = 4
    };

    /**
     * Humidity mode enumeration (F5 command, byte 2)
     */
    enum class HumidityMode : uint8_t
    {
        Unknown = 0xFE,
        Off = 0x00,
        Low = 0x3A,
        Standard = 0x3B,
        High = 0x3C,
        Continuous = 0xFF
    };

    /**
     * Class representing a temperature in degrees C scaled by 10, the most granular internal temperature measurement format
     */
    class DaikinC10
    {
        template<typename T> static constexpr int16_t fromIntegral(T v) {
            if constexpr(std::is_signed_v<T>) {
                if(v<std::numeric_limits<int16_t>::min() || v>std::numeric_limits<int16_t>::max()) return nan_sentinel;
            } else if(v>static_cast<unsigned>(std::numeric_limits<int16_t>::max())) return nan_sentinel;
            return static_cast<int16_t>(v);
        }
      public:
        static constexpr int16_t nan_sentinel = std::numeric_limits<int16_t>::min();

        constexpr DaikinC10() = default;

        template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
        constexpr DaikinC10(const T valf) : value(
            valf!=valf || valf>3276.7L || valf<-3276.7L ? nan_sentinel :
            static_cast<int16_t>(valf*10 + (valf>=0 ? 0.5L : -0.5L))) {} // round to nearest 0.1C

        template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
        constexpr DaikinC10(const T vali) : value(fromIntegral(vali)) {}

        explicit constexpr operator float() const { return (value == nan_sentinel) ? NAN : (value / 10.0F); }
        explicit constexpr operator int16_t() const { return value; }
        constexpr float f_degc() const { return static_cast<float>(*this); }

        // C++17 compatible comparison operators
        constexpr bool operator==(const DaikinC10 &other) const { return value == other.value; }
        constexpr bool operator!=(const DaikinC10 &other) const { return value != other.value; }
        constexpr bool operator<(const DaikinC10 &other) const { return value < other.value; }
        constexpr bool operator<=(const DaikinC10 &other) const { return value <= other.value; }
        constexpr bool operator>(const DaikinC10 &other) const { return value > other.value; }
        constexpr bool operator>=(const DaikinC10 &other) const { return value >= other.value; }

        constexpr DaikinC10 operator+(const DaikinC10 &arg) const { return valid() && arg.valid() ? DaikinC10(this->value + arg.value) : DaikinC10(nan_sentinel); }
        constexpr DaikinC10 operator-(const DaikinC10 &arg) const { return valid() && arg.valid() ? DaikinC10(this->value - arg.value) : DaikinC10(nan_sentinel); }
        constexpr DaikinC10 operator*(const DaikinC10 &arg) const { return valid() && arg.valid() ? DaikinC10(this->value * arg.value) : DaikinC10(nan_sentinel); }
        constexpr DaikinC10 operator/(const DaikinC10 &arg) const { return valid() && arg.valid() && arg.value!=0 ? DaikinC10(this->value / arg.value) : DaikinC10(nan_sentinel); }

        constexpr bool valid() const { return value!=nan_sentinel; }
        static DaikinC10 diff(const DaikinC10 &a, const DaikinC10 &b) { return a.valid() && b.valid() ? DaikinC10(std::abs(a.value-b.value)) : DaikinC10(nan_sentinel); }

      private:
        int16_t value{};
    };

    inline constexpr DaikinC10 SETPOINT_STEP{0.5F};                          // Daikin setpoint granularity
    inline constexpr DaikinC10 TEMPERATURE_STEP{0.1F};                       // Daikin temperature sensor granularity
    inline constexpr DaikinC10 TEMPERATURE_INVALID{DaikinC10::nan_sentinel}; // NaN

    /**
     * Possible sources of active flag.
     */
    enum class ActiveSource : uint8_t
    {
        Unknown,
        CompressorOnOff, // directly read from query
        UnitState,       // interpreted from unit state bitfield
        Unsupported,     // hardcoded to active
    };

    /**
     * Unit state (RzB2) bitfield decoder
     */
    class DaikinUnitState
    {
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
    class DaikinSystemState
    {
      public:
        constexpr DaikinSystemState(const uint8_t value = 0U) : raw(value) {}
        constexpr bool locked() const { return (this->raw & 0x01) != 0; }
        constexpr bool active() const { return (this->raw & 0x04) != 0; }
        constexpr bool defrost() const { return (this->raw & 0x08) != 0; }
        constexpr bool other_units_online() const { return (this->raw & 0x20) != 0; }
        uint8_t raw{};
    };

    enum class LedState : uint8_t { Unknown, On, Dark, Off };

    enum class ClimateVariant : uint8_t { Auto, Cool, DryCooling, Heat, HumidHeating, SararaDry, UruruHumidify, FanOnly };
    struct ProductionInfo {
        bool valid{false};
        std::string siteCode, lineCode, orderCode;
        struct Unit { bool valid{false}; std::string siteCode, dateCode, serialCode; uint8_t baseCode{0}, deviceCode{0}; } indoor, outdoor;
    };
    struct Identification {
        std::string modelCode, legacyModelWire, legacyVersionWire, itelc;
        bool wifiValid{false}, ftValid{false};
        char wifiMode{'0'}; uint8_t wifiCounter{0}; uint16_t ftRawDecimal{0};
        bool fkValid{false}, laundryAvailable{false}, simpleHumidify{false};
        bool japaneseTemperatureRange{false};
        uint8_t acledRaw{0}, electricityRaw{0}, destinationRaw{0}, displayDryRaw{0};
        std::array<uint8_t,4> fkRaw{};
        ProductionInfo production;
    };

    struct ClimateSettings
    {
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

        // C++17 compatible equality operator
        bool operator==(const ClimateSettings &other) const
        {
            return power == other.power &&
                   mode == other.mode &&
                   targetC == other.targetC &&
                   fan_mode == other.fan_mode &&
                   swing_v == other.swing_v &&
                   swing_h == other.swing_h &&
                   powerful == other.powerful &&
                   econo == other.econo &&
                   quiet == other.quiet &&
                   led == other.led &&
                   streamer == other.streamer &&
                   sensor == other.sensor &&
                   sensor_temp == other.sensor_temp;
        }
        bool operator!=(const ClimateSettings &other) const
        {
            return !(*this == other);
        }
    };

    /**
     * Complete state structure that represents the current AC unit status
     */
    struct State
    {
        bool heatMinimumValid{false}, autoLimitOffsetValid{false};
        float heatMinimumC{18}, autoLimitOffsetC{0}; // offset is not an absolute setpoint
        bool basicValid{false}, targetTemperatureValid{false}, fanValid{false}, swingValid{false};
        bool specialModesValid{false}, demandValid{false}, demandPercentageValid{false}, compressorOnValid{false}, compressorHzValid{false};
        bool unitStateValid{false}, systemStateValid{false}, actionValid{false};
        bool targetOffsetValid{false};
        float targetOffsetC{NAN};
        uint8_t setpointRaw{0};
        bool setpointRawValid{false};
        ProtocolVersion protocol{ProtocolUndetected};
        // Basic state
        bool online{false};
        bool power{false};
        Mode mode{Mode::Off};
        Action action{Action::Off};
        AutoBias autoBias{AutoBias::Unknown}; // S21 mode '0'/'7' distinction
        float targetC{23.0f};
        float realTargetC{23.0f}; // RX: Actual target temp with sensor adjustments
        bool insideTemperatureValid{false}, outsideTemperatureValid{false}, humidityValid{false};
        float homeC{25.0f};
        float outsideC{25.0f};
        float liquidC{25.0f};
        bool indoorHeatExchangerValid{false};
        float indoorHeatExchangerC{NAN};

        bool timersValid{false}, onTimerEnabled{false}, offTimerEnabled{false};
        uint16_t onTimerConfiguredMinutes{0}, offTimerConfiguredMinutes{0};
        bool supplementalSwingValid{false}, onTimerPeriodsValid{false}, offTimerPeriodsValid{false};
        Swing supplementalSwing{Swing::Off};
        uint16_t onTimerPeriods{0}, offTimerPeriods{0}; // 10-minute units from time set, not remaining time
        bool independentPowerValid{false}, independentPower{false};
        bool independentModeValid{false}, coarseTargetValid{false};
        uint8_t independentMode{0};
        int16_t coarseTargetC{0};
        bool verticalPositionValid{false}, verticalPositionVerified{false}, verticalPositionCodeValid{false};
        uint8_t verticalPosition{0};
        bool horizontalPositionValid{false};
        uint8_t horizontalPosition{0};
        bool timerLampValid{false}, timerLamp{false};
        bool serialErrorValid{false}, serialError{false};
        uint8_t serialErrorByte{0};
        uint32_t serialErrorSamples{0};

        // System status
        bool compressorOn{false}; // RG2: compressor state for action determination

        // Fan and swing
        DaikinFanMode fan{DaikinFanMode::Auto};
        Swing swing{Swing::Off};
        uint8_t humidityRaw{0};
        bool humidityModeValid{false};
        HumidityMode humidityMode{HumidityMode::Unknown}; // F5 byte 2: humidity setting
        bool fanRpmValid{false}, fanRpmSetpointValid{false};
        uint16_t fanRpm{0};
        uint16_t fanRpmSetpoint{0};
        bool vaneAngleValid{false}, vaneAngleSetpointValid{false};
        bool vaneAngleDegreesValid{false}, vaneAngleSetpointDegreesValid{false};
        float vaneAngleDegrees{NAN}, vaneAngleSetpointDegrees{NAN};
        int16_t swingVerticalAngle{0}; // signed raw decimal; model scaling can differ
        int16_t swingVerticalAngleSetpoint{0};

        // Advanced features and modifiers
        bool quiet{false};    // outdoor unit fan/compressor limit
        bool econo{false};    // limits demand for power consumption
        bool powerful{false}; // maximum output (20 minute timeout), mutually exclusive with quiet and econo
        bool comfort{false};  // fan angle depends on heating/cooling action
        bool streamer{false}; // electron emitter decontamination?
        bool sensor{false};   // "intelligent eye" PIR occupancy setpoint offset
        LedState ledState{LedState::Unknown};
        bool sensorLed{false};
        bool led{true};       // unit LED status

        // Sensors and telemetry
        uint8_t humidity{50};
        uint8_t demand{0};
        uint8_t demand_percentage{100}; // demand percentage (0-100)

        bool totalEnergyValid{false};
        uint32_t totalEnergyWh{0}; // Total energy consumption in Watt-hours (from FM command)
        uint16_t compressorHz{0};
        std::array<uint8_t,4> irCounterRaw{}; // field encoding is undocumented; never invent a number
        bool irCounterValid{false}; uint32_t irObservedChanges{0};
        bool pullValid{false}, rmRawValid{false}, rtRawValid{false};
        uint8_t pull{0}; uint16_t rmRaw{0}, rtRaw{0};
        ClimateVariant climateVariant{ClimateVariant::Auto};
        bool climateVariantValid{false}, simpleHumidifyAvailable{false};

        // v3.20+/v3.40+ extension pages (FU/FX):
        bool fu04_valid{false};
        uint64_t fu04_cooling_wh{0}; // value is Wh (FU04 value * 100 Wh)
        uint64_t fu04_heating_wh{0}; // value is Wh (FU04 value * 100 Wh)

        bool fx60_valid{false};
        uint16_t fx60_raw{0}; // reverse-ASCII hex, ten-watt units
        uint32_t fx60_power_w{0};

        // System status
        DaikinUnitState unitState{};
        DaikinSystemState systemState{};
        uint8_t protocolVersion{0xFF};
    };

    /**
     * Communication statistics
     */
    struct Stats
    {
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
    inline constexpr DaikinModel ModelFTXC24AXVJU{0x0B66}; // outdoor RXC24AXVJU

} // namespace daikin
