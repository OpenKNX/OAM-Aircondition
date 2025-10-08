#pragma once
#include "../../AirConditionDriver.h"
#include "daikin_s21_openknx_types.h"
#include "daikin_s21_openknx_serial.h"
#include <vector>
#include <functional>
#include <memory>

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
    void setup() override;
    void startCommunication(bool restart) override;
    void requestAllData() override;
    void loop() override;

    // Information methods
    const std::string name() const override;
    void showInformations() override;
    
    // Capability queries
    float getMinimumTargetTemperature() override;
    float getMaximumTargetTemperature() override;
    unsigned int getMaximumFanSpeed() override;
    unsigned int getMaximumHorizontalFixPosition() override;
    unsigned int getMaximumVertiacalFixPosition() override;
    bool supportExternalRoomTemperatureSensor() override;
    float accuracyInDegrees() override;

    // Control methods
    void setPower(bool power) override;
    void setMode(AirConditionMode mode) override;
    void setTargetTemperature(float temperaturCelsius) override;
    void setFanSpeed(unsigned int speed) override;
    void setSwingHorizontal(bool swing) override;
    void setSwingVertical(bool swing) override;
    void setSwingHorizontalFixPosition(unsigned int position) override;
    void setSwingVerticalFixPosition(unsigned int position) override;
    void setExternalSensorRoomTemperature(float temperaturCelius) override;
    void setWifiLed(bool on) override;
    void setDeviceMode(AirConditionDeviceMode mode) override;
    void setMaxPowerLevel(uint8_t percentage) override;
    void setAirPurification(bool on) override;

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
    
    // timing constants (non-blocking)
    static constexpr uint32_t QUERY_CYCLE_INTERVAL_MS = 10000;      // 10 seconds for debugging
    static constexpr uint32_t PROTOCOL_DETECTION_RETRY_MS = 300000; // 5 minutes retry for unknown protocol
    static constexpr uint32_t INTER_QUERY_DELAY_MS = 35;            // 35ms between queries (non-blocking)
    static constexpr uint32_t INTER_QUERY_DELAY_HIGH_LOAD_MS = 80;  // Increased spacing under high load
    static constexpr uint32_t COMMAND_COOLDOWN_MS = 3000;           // 3 seconds cooldown after D1 commands
    static constexpr uint32_t ACK_WAIT_TIMEOUT_MS = 400;            // ACK wait timeout (non-blocking)
    static constexpr uint32_t GRACE_PERIOD_MS = 400;                // Grace period for late responses
    static constexpr uint32_t LATE_RX_WINDOW_MS = 800;              // Window to accept late responses in Idle
    
    // Configurable RX timing windows (mentioned in feedback)
    static constexpr uint32_t NORMAL_RX_WINDOW_MS = 25;        // Normal response window
    static constexpr uint32_t COALESCE_RX_WINDOW_MS = 15;      // Window for multi-byte collection after 0xC0
    
    // state machine
    enum class QueryState {
        Idle,           // Not running queries
        WaitingToSend,  // Waiting for timing interval
        WaitingForAck,  // Sent query, waiting for response
        WaitingForGrace,// Grace period for late responses
        Cooldown        // Waiting for command cooldown
    };
    
    QueryState query_state_{QueryState::Idle};
    uint32_t state_start_time_{0};   // When current state started
    bool high_load_detected_{false}; // Track if system is under high load
    
    // Protocol detection optimization
    bool old_protocol_detected_{false};    // Early detection flag
    uint32_t c0_response_count_{0};        // Count of 0xC0 responses
    bool first_cycle_completed_{false};    // Track first full query cycle
    bool protocol_detection_phase_{false}; // True during initial protocol detection
    
    // polarity detection
    uint8_t current_polarity_attempt_{0};    // Track current polarity combination (0-3)
    uint8_t protocol_detection_attempts_{0}; // Track F8→FY00 attempts per polarity
    static constexpr uint8_t MAX_POLARITY_ATTEMPTS = 3; // F8→FY00 attempts per polarity combo
    static constexpr uint8_t MAX_POLARITY_COMBOS = 4;   // 4 polarity combinations total

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
