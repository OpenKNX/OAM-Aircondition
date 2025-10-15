#pragma once
#include "../../AirConditionDriver.h"
#include "daikin_s21_openknx_types.h"
#include "daikin_s21_openknx_serial.h"
#include <vector>
#include <functional>
#include <memory>
#include <array>

// Type definitions
using PayloadBuffer = std::array<uint8_t, daikin::DaikinSerial::STANDARD_PAYLOAD_SIZE>; // Utility payload buffer for S21 commands

// S21 Protocol Query Definitions
namespace StateQuery {
  inline constexpr std::string_view Basic{"F1"};
  inline constexpr std::string_view OptionalFeatures{"F2"};
  inline constexpr std::string_view OnOffTimer{"F3"};  
  inline constexpr std::string_view SwingOrHumidity{"F5"};
  inline constexpr std::string_view SpecialModes{"F6"};
  inline constexpr std::string_view DemandAndEcono{"F7"};
  inline constexpr std::string_view OldProtocol{"F8"};
  inline constexpr std::string_view InsideOutsideTemperatures{"F9"};
  inline constexpr std::string_view ModelCode{"FC"};
  inline constexpr std::string_view IRCounter{"FG"};
  inline constexpr std::string_view PowerConsumption{"FM"};
  inline constexpr std::string_view CapabilityN{"FN"};
  inline constexpr std::string_view CapabilityP{"FP"};
  inline constexpr std::string_view CapabilityQ{"FQ"};
  inline constexpr std::string_view CapabilityS{"FS"};
  inline constexpr std::string_view CapabilityT{"FT"};
  inline constexpr std::string_view CapabilityK{"FK"}; 
  inline constexpr std::string_view NewProtocol{"FY00"};
}

namespace EnvironmentQuery {
  inline constexpr std::string_view FanMode{"RG"};
  inline constexpr std::string_view InsideTemperature{"RH"};
  inline constexpr std::string_view TargetTemperature{"RX"};
  inline constexpr std::string_view OutsideTemperature{"Ra"};
  inline constexpr std::string_view CompressorFrequency{"Rd"};
  inline constexpr std::string_view IndoorHumidity{"Re"};
  inline constexpr std::string_view CompressorOnOff{"Rg"};
  inline constexpr std::string_view UnitState{"RzB2"};
  inline constexpr std::string_view SystemState{"RzC3"};
}

namespace StateCommand {
  inline constexpr std::string_view PowerModeTempFan{"D1"};
  inline constexpr std::string_view LouvreSwingMode{"D5"};
  inline constexpr std::string_view Powerful{"D6"};
  inline constexpr std::string_view Econo{"D7"};
  inline constexpr std::string_view Quiet{"D4"};
  inline constexpr std::string_view Sensor{"D8"};
  inline constexpr std::string_view Led{"D9"};
  inline constexpr std::string_view Streamer{"DA"};
}

namespace MiscQuery {
  inline constexpr std::string_view Model{"S2"};
  inline constexpr std::string_view Version{"RV"};
}

// Query state management for S21 protocol
class DaikinQueryState {
public:
    using handler_fn = std::function<void(uint8_t*, size_t)>;
    
    DaikinQueryState() = default;
    DaikinQueryState(std::string_view cmd, handler_fn h = nullptr, bool static_query = false)
        : command(cmd), handler(h), is_static(static_query) {}
    
    std::string_view command{};
    handler_fn handler{nullptr};
    bool is_static{false};
    bool acked{false};
    uint8_t naks{0};
    bool bad{false};  // permanently mark unsupported queries
    std::vector<uint8_t> response_data{};
    
    void set_response(std::span<const uint8_t> data) {
        response_data.assign(data.begin(), data.end());
    }
    
    std::span<const uint8_t> get_response() const {
        return response_data;
    }
};

/**
 * DaikinDriver - Complete S21 protocol implementation
 * 
 * This driver provides full Daikin S21 protocol support including:
 * - Temperature control (18-32°C with 0.5°C accuracy)
 * - Mode control (Auto, Cool, Heat, Dry, Fan)
 * - Fan speed control (Auto + 5 levels)
 * - Swing control (Vertical and Horizontal)
 * - Advanced features (Powerful, Econo, Quiet)
 * - LED control
 * - Air purification (Streamer)
 * - External sensor support
 * - Telemetry (humidity, power consumption, temperatures)
 */
class DaikinDriver : public AirConditionDriver
{
public:
    explicit DaikinDriver(AirConditionDriverStatusFeedback& statusFeedback);
    virtual ~DaikinDriver() = default;

    // === AirConditionDriver Interface Implementation ===
    
    // Lifecycle methods
    virtual void setup() override;
    virtual void startCommunication(bool restart) override;
    virtual void requestAllData() override;
    virtual void loop() override;

    // Information methods
    virtual const std::string name() const override;
    virtual void showInformations() override;
    
    // Capability queries
    virtual float getMinimumTargetTemperature() override;
    virtual float getMaximumTargetTemperature() override;
    virtual unsigned int getMaximumFanSpeed() override;
    virtual unsigned int getMaximumHorizontalFixPosition() override;
    virtual unsigned int getMaximumVerticalFixPosition() override;
    virtual unsigned int getMaximumHumidityModeLevels() override;
    virtual bool supportExternalRoomTemperatureSensor() override;
    virtual float accuracyInDegrees() override;

    // Control methods
    virtual void setPower(bool power) override;
    virtual void setMode(AirConditionMode mode) override;
    virtual void setTargetTemperature(float temperaturCelsius) override;
    virtual void setFanSpeed(unsigned int speed) override;
    virtual void setSwingHorizontal(bool swing) override;
    virtual void setSwingVertical(bool swing) override;
    virtual void setSwingHorizontalFixPosition(unsigned int position) override;
    virtual void setSwingVerticalFixPosition(unsigned int position) override;
    virtual void setExternalSensorRoomTemperature(float temperaturCelsius) override;
    virtual void setWifiLed(bool on) override;
    virtual void setDeviceMode(AirConditionDeviceMode mode) override;
    virtual void setMaxPowerLevel(uint8_t percentage) override;
    virtual void setAirPurification(bool on) override;

    void updatePower(bool power);
    void updateMode(AirConditionMode mode);
    void updateTargetTemperature(float temperaturCelsius);
    void updateFanSpeed(int speed);
    void updateSwingHorizontal(bool swing);
    void updateSwingVertical(bool swing);
    void updateCurrentTemperature(float temperaturCelsius);
    void updateOutdoorTemperature(float temperaturCelsius);
    void updateDeviceMode(AirConditionDeviceMode mode);
    void updateMaxPowerLevel(uint8_t percentage);
    void updateAirPurification(bool on);
    void updateOnlineStatus(bool online);
    void updateWifiLed(bool on);
    void updateHumidity(uint8_t humidity);
    void updateHumidityMode(uint8_t humidityMode);
    void updateTotalEnergyConsumption(uint32_t totalEnergyWh);

    // === S21 Serial Communication Callbacks ===
    void handle_serial_result(daikin::DaikinSerial::Result result, uint8_t* data, size_t data_size);
    void handle_serial_idle();

private:
    // === S21 Protocol State Management ===
    daikin::State state_{};  // Current device state from S21 protocol
    daikin::Stats stats_{};  // Communication statistics
    
    // Query management
    std::vector<DaikinQueryState> queries_;
    std::size_t query_index_{0};
    std::vector<std::string_view> failed_queries_;
    
    // Smart NAK tracking
    void resetQueryNakTracking();
    void checkF6ToF3Fallback();  // Dynamically switch from F6 to F3 when needed
    
    // Pending settings for transmission to AC unit
    struct {
        daikin::ClimateSettings climate{};
        bool activate_climate{false};
        bool activate_swing_mode{false};
        bool activate_powerful{false};
        bool activate_econo{false};
        bool activate_quiet{false};
        bool activate_led{false};
        bool activate_streamer{false};
        bool activate_sensor{false};
    } pending_;
    
    // Protocol support detection
    daikin::ProtocolVersion protocol_version_{daikin::ProtocolUndetected};
    struct {
        bool fan_mode_query{false};
        bool inside_temperature_query{false};
        bool outside_temperature_query{false};
        bool humidity_query{false};
        bool unit_system_state_queries{false};
        daikin::ActiveSource active_source{daikin::ActiveSource::Unknown};
        bool swing{false};
        bool horizontal_swing{false};
        bool humidity{false};
        bool fan{false};
        bool sensor{false};
        bool sensor_hardware{false};  // FK byte1 bit3: Intelligent Eye hardware availability
        bool streamer{false};
        bool led{false};
        bool f6_special_modes{false};  // true if F6 works, false to use F3 fallback
    } support_;

    // Serial communication
    std::unique_ptr<daikin::DaikinSerial> serial_;
    uint32_t last_query_cycle_{0};
    uint32_t last_protocol_detection_attempt_{0}; // Track when we last tried protocol detection
    uint32_t last_command_time_{0};  // Track when D1 commands are sent
    uint32_t last_send_time_{0};     // Track last serial send for pacing
    uint32_t last_query_time_{0};    // Track individual query timing
    
    // timing constants (optimized for v0 protocol stability)
    static constexpr uint32_t QUERY_CYCLE_INTERVAL_MS = 10000;      // 10 seconds for debugging
    static constexpr uint32_t PROTOCOL_DETECTION_RETRY_MS = 300000; // 5 minutes retry for unknown protocol
    static constexpr uint32_t INTER_QUERY_DELAY_MS = 35;            // 35ms between queries (non-blocking)
    static constexpr uint32_t INTER_QUERY_DELAY_HIGH_LOAD_MS = 500; // High load: 500-800ms spacing
    
    // Write command settle times (v0 units need longer settling)
    static constexpr uint32_t SETTLE_AFTER_D1_MS = 4500;           // 4.5s after D1 (mode/power) for v0
    static constexpr uint32_t SETTLE_AFTER_D5_MS = 3100;           // 3.1s after D5 (swing) for v0 (>cooldown)
    static constexpr uint32_t SETTLE_AFTER_OTHER_MS = 1500;        // 1.5s for other D commands
    
    // ACK and response timeouts
    static constexpr uint32_t ACK_DEADLINE_MS = 200;               // ACK comes quickly
    static constexpr uint32_t LATE_RX_WINDOW_MS = 5000;            // 5s window for late frame acceptance
    static constexpr uint32_t COMMAND_COOLDOWN_MS = 3000;          // Legacy cooldown period (replaced by post-write settle)
    static constexpr uint32_t ACK_WAIT_TIMEOUT_MS = 400;           // ACK wait timeout (non-blocking)
    static constexpr uint32_t POST_WRITE_F1_TIMEOUT_MS = 2500;     // First F1 after write command (v0 needs up to 5s)
    static constexpr uint32_t GRACE_PERIOD_MS = 400;               // Grace period for normal responses
    static constexpr uint32_t LATE_ACCEPT_WINDOW_MS = 5000;        // Extended window for late frames
    
    // Configurable RX timing windows
    static constexpr uint32_t NORMAL_RX_WINDOW_MS = 25;            // Normal response window
    static constexpr uint32_t COALESCE_RX_WINDOW_MS = 15;          // Window for multi-byte collection after 0xC0
    
    // state machine
    enum class QueryState {
        Idle,             // Not running queries
        WaitingToSend,    // Waiting for timing interval
        WaitingForAck,    // Sent query, waiting for response
        WaitingForGrace,  // Grace period for late responses
        Cooldown,         // Waiting for command cooldown
        PostWriteSettle   // Extended settle after D1/D5 commands (v0 specific)
    };
    
    QueryState query_state_{QueryState::Idle};
    uint32_t state_start_time_{0};   // When current state started
    bool high_load_detected_{false}; // Track if system is under high load
    
    // Post-write command tracking (v0 stability)
    enum class LastWriteCommand {
        None,
        D1_ModePower,     // Need 4.5s settle + first F1 with extended timeout
        D5_Swing,         // Need 2.8s settle
        Other             // Need 1.5s settle
    };
    
    LastWriteCommand last_write_command_{LastWriteCommand::None};
    uint32_t last_write_time_{0};           // When last write command was sent
    bool post_write_first_f1_pending_{false}; // Need to do first F1 with extended timeout
    bool scheduler_paused_{false};          // True when scheduler is paused for settling
    
    // Write coordination to prevent race conditions
    bool write_pending_{false};            // New write request during settle period
    PayloadBuffer last_sent_d1_payload_;   // Dedupe identical D1 commands
    
    // Online/offline detection (robust S21 connectivity tracking)
    uint32_t last_rx_ok_ms_{0};            // Time of last ACK or valid frame
    bool online_{false};                   // Current online status
    
    // Protocol detection optimization
    bool old_protocol_detected_{false};    // Early detection flag
    uint32_t c0_response_count_{0};        // Count of 0xC0 responses
    bool first_cycle_completed_{false};    // Track first full query cycle
    bool protocol_detection_phase_{false}; // True during initial protocol detection
    
    // protocol detection
    uint8_t protocol_detection_attempts_{0}; // Track F8→FY00 attempts 


    // === S21 Protocol Implementation ===
    void initializeProtocolDetection();  // Initialize protocol detection queries first
    void initializeQueries();
    void triggerQueryCycle();
    void updateQueryStateMachine();  // Non-blocking state machine
    void startQueryCycle();
    void advanceQuery();
    bool canSendQuery() const;        // Check if timing allows sending
    bool isQuerySupported(std::string_view command) const;
    
    // Command sending methods
    void sendClimateCommand();
    void sendSwingCommand();
    void sendPowerfulCommand();
    void sendEconoCommand();
    void sendQuietCommand();
    void sendSensorCommand();
    void sendLedCommand();
    void sendStreamerCommand();
    
    // Post-write command management (v0 stability)
    void handleWriteCommandSent(LastWriteCommand cmd_type);
    uint32_t getSettlePeriodForCommand(LastWriteCommand cmd_type) const;
    bool isInPostWriteSettle() const;
    bool isProtocolV0() const;
    void clearRxBuffer();
    void scheduleFirstF1AfterWrite();
    
    // Online/offline detection methods
    void markOnline(uint32_t now);
    void checkOnlineTimeout();
    
    // Response handlers
    void handle_f1_response(uint8_t* data, size_t data_size);
    void handle_f2_response(uint8_t* data, size_t data_size);
    void handle_f3_response(uint8_t* data, size_t data_size); 
    void handle_f5_response(uint8_t* data, size_t data_size);
    void handle_f6_response(uint8_t* data, size_t data_size);
    void handle_f7_response(uint8_t* data, size_t data_size);
    void handle_f8_response(uint8_t* data, size_t data_size);
    void handle_f9_response(uint8_t* data, size_t data_size);
    void handle_fc_response(uint8_t* data, size_t data_size);
    void handle_fg_response(uint8_t* data, size_t data_size);
    void handle_fm_response(uint8_t* data, size_t data_size);
    void handle_fy00_response(uint8_t* data, size_t data_size);
    void handle_fn_response(uint8_t* data, size_t data_size);
    void handle_fp_response(uint8_t* data, size_t data_size);
    void handle_fq_response(uint8_t* data, size_t data_size);
    void handle_fs_response(uint8_t* data, size_t data_size);
    void handle_ft_response(uint8_t* data, size_t data_size);
    void handle_fk_response(uint8_t* data, size_t data_size);
    void handle_ra_response(uint8_t* data, size_t data_size);
    void handle_rd_response(uint8_t* data, size_t data_size);
    void handle_re_response(uint8_t* data, size_t data_size);
    void handle_rg_response(uint8_t* data, size_t data_size);
    void handle_rh_response(uint8_t* data, size_t data_size);
    void handle_rx_response(uint8_t* data, size_t data_size);
    void handle_rg2_response(uint8_t* data, size_t data_size);
    void handle_rzb2_response(uint8_t* data, size_t data_size);
    void handle_rzc3_response(uint8_t* data, size_t data_size);
    
    // State management and publishing
    void publishState();
    void updateOnlineStatus();
    void handleCommandResponse(const std::string& command, bool success);
    
    // Conversion utilities
    uint8_t mode_to_daikin(daikin::Mode mode) const;
    daikin::Mode daikin_to_mode(uint8_t mode) const;
    daikin::Action daikin_to_action(uint8_t action) const;
    daikin::Swing daikin_to_swing_mode(uint8_t mode) const;
    uint8_t swing_mode_to_daikin(daikin::Swing mode) const;
    uint8_t fan_mode_to_daikin(daikin::DaikinFanMode fan) const;
    daikin::DaikinFanMode daikin_to_fan_mode(uint8_t fan) const;
    
    // OpenKNX to Daikin conversions
    daikin::Mode openknx_to_daikin_mode(AirConditionMode mode) const;
    AirConditionMode daikin_to_openknx_mode(daikin::Mode mode) const;
    daikin::DaikinFanMode openknx_to_daikin_fan(unsigned int speed) const;
    unsigned int daikin_to_openknx_fan(daikin::DaikinFanMode fan) const;
};
