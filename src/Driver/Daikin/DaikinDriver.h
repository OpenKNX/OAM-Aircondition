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
  // v3.20+ extension field queries (U-family)
  inline constexpr std::string_view ExtensionFU00{"FU00"};
  inline constexpr std::string_view ExtensionFU02{"FU02"};
  inline constexpr std::string_view ExtensionFU04{"FU04"};
  // v3.40+ extension field queries (U-family)
  inline constexpr std::string_view ExtensionFU05{"FU05"};
  inline constexpr std::string_view ExtensionFU15{"FU15"};
  inline constexpr std::string_view ExtensionFU25{"FU25"};
  inline constexpr std::string_view ExtensionFU35{"FU35"};
  inline constexpr std::string_view ExtensionFU45{"FU45"};
  // v3.20+ extension field queries (X-family)
  inline constexpr std::string_view ExtensionFX00{"FX00"};
  inline constexpr std::string_view ExtensionFX10{"FX10"};
  inline constexpr std::string_view ExtensionFX20{"FX20"};
  inline constexpr std::string_view ExtensionFX30{"FX30"};
  inline constexpr std::string_view ExtensionFX40{"FX40"};
  inline constexpr std::string_view ExtensionFX50{"FX50"};
  inline constexpr std::string_view ExtensionFX60{"FX60"};
  inline constexpr std::string_view ExtensionFX70{"FX70"};
  inline constexpr std::string_view ExtensionFX80{"FX80"};
  inline constexpr std::string_view ExtensionFX90{"FX90"};
  inline constexpr std::string_view ExtensionFXA0{"FXA0"};
  inline constexpr std::string_view ExtensionFXB0{"FXB0"};
  inline constexpr std::string_view ExtensionFXC0{"FXC0"};
  // v3.40+ extension field queries (X-family)
  inline constexpr std::string_view ExtensionFXD0{"FXD0"};
  inline constexpr std::string_view ExtensionFXE0{"FXE0"};
  inline constexpr std::string_view ExtensionFXF0{"FXF0"};
  inline constexpr std::string_view ExtensionFX01{"FX01"};
  inline constexpr std::string_view ExtensionFX11{"FX11"};
  inline constexpr std::string_view ExtensionFX21{"FX21"};
  inline constexpr std::string_view ExtensionFX31{"FX31"};
  inline constexpr std::string_view ExtensionFX41{"FX41"};
  inline constexpr std::string_view ExtensionFX51{"FX51"};
  inline constexpr std::string_view ExtensionFX61{"FX61"};
  inline constexpr std::string_view ExtensionFX71{"FX71"};
  inline constexpr std::string_view ExtensionFX81{"FX81"};
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
  inline constexpr std::string_view Comfort{"D6"};
  inline constexpr std::string_view Powerful{"D6"};
  inline constexpr std::string_view Econo{"D7"};
  inline constexpr std::string_view Quiet{"D6"};
  inline constexpr std::string_view Sensor{"D6"};
  inline constexpr std::string_view Led{"D6"};
  inline constexpr std::string_view Streamer{"D6"};
}

namespace MiscQuery {
  inline constexpr std::string_view Model{"M"};
  inline constexpr std::string_view Version{"V"};
}

// Query state management for S21 protocol
class DaikinQueryState {
public:
    using handler_fn = std::function<void(uint8_t*, size_t)>;

    DaikinQueryState() = default;
    DaikinQueryState(std::string_view cmd, handler_fn h = nullptr, bool static_query = false)
        : command(cmd.data(),cmd.size()), handler(h), is_static(static_query) {}

    std::string command{};
    handler_fn handler{nullptr};
    uint32_t interval_ms{0}, last_sample_ms{0};
    bool is_static{false};
    bool acked{false};
    uint8_t naks{0};
    uint32_t last_nak_ms{0};
    bool bad{false};  // temporarily suppress repeated NAKs until timed reprobe
    std::vector<uint8_t> response_data{};

    void set_response(const uint8_t* data, size_t size) {
        response_data.assign(data, data + size);
    }

    const std::vector<uint8_t>& get_response() const {
        return response_data;
    }
};

/**
 * DaikinDriver - Complete S21 protocol implementation
 *
 * S21 protocol support varies by model; optional controls use runtime discovery.
 * - Temperature control (18-32°C with 0.5°C accuracy)
 * - Mode control (Auto, Cool, Heat, Dry, Fan)
 * - Fan speed control (Auto + 5 levels)
 * - Swing control (Vertical and Horizontal)
 * - Advanced features (Powerful, Econo, Quiet)
 * - LED control
 * - Air purification (Streamer)
 * - Intelligent Eye control (not external temperature injection)
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
    bool processCommand(const std::string command,bool debugKo) override;

    enum class DiagnosticStatus : uint8_t { Idle, Queued, Running, Ack, Frame, Nak, Timeout, Error };
    struct DiagnosticResult { std::string command; std::vector<uint8_t> payload; DiagnosticStatus status{DiagnosticStatus::Idle}; };
    bool queueDiagnosticCommand(std::string command, std::vector<uint8_t> payload = {});
    const DiagnosticResult& diagnosticResult() const { return diagnostic_result_; }
    const daikin::Identification& identification() const { return identification_; }
    bool setClimateVariant(daikin::ClimateVariant mode, daikin::HumidityMode humidity = daikin::HumidityMode::Standard);
    bool setVerticalPositionCode(char position);
    bool setFixedVerticalPosition(uint8_t position) override;
    bool setOptionalControl(AirConditionOptionalControl control, bool on) override;
    bool setUnitLed(AirConditionUnitLed state) override;
    AirConditionOptionalState optionalControlState() const;
    bool setHumidityLevel(AirConditionHumidityLevel level) override;
    uint8_t availableHumidityLevels() const;
 // 0..5 or '?' (swing)
    bool setTimerLamp(bool on) override;
    void enableExtendedPolling(bool enabled) { extended_polling_=enabled; }
    void trustRzStateReadings(bool trusted) { rz_states_trusted_=trusted; }
    uint8_t verifiedVerticalPositions() const { return verified_vertical_positions_; }
    void enableResearchPolling(bool enabled) { research_polling_=enabled; }
    void enableProductionDiagnostics(bool enabled) { production_polling_=enabled; }
    bool configureTemperatureLimits(daikin::Mode mode, float minimum, float maximum);
    const std::string& modelName() const { return model_name_; }
    const std::string& firmwareVersion() const { return firmware_version_; }
    std::vector<uint8_t> diagnosticResponse(std::string_view command) const;
    const daikin::State& telemetry() const { return state_; }
    enum class FeatureAccess : uint8_t { Unknown, Unsupported, ReadOnly, Writable, ReadSupported };
    FeatureAccess commandAccess(std::string_view read_command) const;
    enum class WriteOutcome : uint8_t { None, AwaitingAck, AwaitingReadback, Confirmed, Rejected, Timeout, Error, Mismatch };
    WriteOutcome writeOutcome(std::string_view read_command) const;
    bool fieldWriteVerified(std::string_view read_command,size_t byte,uint8_t mask) const;
    void retryRejectedWrites();

    struct TimingProfile {
        uint32_t cycle_ms{10000}, inter_query_ms{35}, high_load_ms{500};
        uint32_t ack_ms{900}, rx_ms{900}, post_write_f1_ms{2500};
        uint32_t d1_settle_ms{3000}, d5_settle_ms{2000}, other_settle_ms{1000};
        uint32_t v0_d1_settle_ms{4500}, v0_d5_settle_ms{3100}, v0_other_settle_ms{1500};
        uint32_t offline_ms{15000}, reprobe_ms{300000};
    };
    bool configureTiming(const TimingProfile& profile);

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

    // Configure only after validating the model's reduced-demand range on hardware.
    bool configureDemandLimits(uint8_t minimum, uint8_t step) override;
    bool configureTimerGranularity(uint16_t minutes, uint16_t maximum_minutes) override;
    bool setTimerDuration(AirConditionTimer timer, uint16_t minutes) override;
    bool setTimerEnabled(AirConditionTimer timer, bool enabled) override;
    AirConditionTimerStatus timerStatus() const;
    bool setTimers(uint16_t on_minutes, uint16_t off_minutes);
    bool configureVaneAngleScale(uint8_t divisor);
    bool setHumidityMode(daikin::HumidityMode mode);
    void setLedState(daikin::LedState state);
    void setComfort(bool on);
    void setIntelligentEye(bool on);
    // === S21 Serial Communication Callbacks ===
    void handle_serial_result(daikin::DaikinSerial::Result result, uint8_t* data, size_t data_size);
    void handle_serial_idle();

private:
    // === S21 Protocol State Management ===
    daikin::State state_{};  // Current device state from S21 protocol
    daikin::Stats stats_{};  // Communication statistics

    // Sample completeness tracking (prevent publishing wrong values after Online)
    uint8_t sample_seen_mask_{0};
    static constexpr uint8_t SEEN_F1 = 1<<0;  // Basic status
    static constexpr uint8_t SEEN_RH = 1<<1;  // Inside temperature
    static constexpr uint8_t SEEN_RX = 1<<2;  // Target temperature
    static constexpr uint8_t SEEN_RA = 1<<3;  // Outside temperature
    bool gate_publish_until_full_sample_{true};
    bool seed_kos_pending_{true};  // Initial KO seeding after first complete sample
    uint32_t online_since_ms_{0};  // when we last went online (for gate timeout)

    // Gate timeout (publish even without full sample)
    static constexpr uint32_t FULL_SAMPLE_GATE_TIMEOUT_MS = 5000; // 5s

    // Query management
    std::vector<DaikinQueryState> queries_;
    std::size_t query_index_{0};
    std::vector<std::string> failed_queries_;

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
        bool activate_comfort{false};
        daikin::LedState led{daikin::LedState::Unknown};
        bool comfort{false};
    } pending_;

    enum ClimateDirty : uint8_t { DirtyPower=1, DirtyMode=2, DirtyTemperature=4, DirtyFan=8 };
    uint8_t climate_dirty_{0};

    // Track explicit OFF commands to prevent safety guard from overriding them
    bool pending_explicit_off_{false};

    // Track explicit fan commands to prevent safety guard from overriding them
    bool pending_explicit_fan_{false};

    // Protocol support detection
    daikin::ProtocolVersion protocol_version_{daikin::ProtocolUndetected};
    struct {
        bool f2_known{false}, fan_capability_known{false}, sensor_hardware_known{false};
        bool auto_limit_offset{false}, simple_humidify{false}, humidify_operation{false};
        bool special_features_known{false};
        bool powerful{false};
        bool econo{false};
        bool demand{false};
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

    enum class CommandSupport : uint8_t { Unknown, Supported, Unsupported };
    struct CombinedCommand {
        PayloadBuffer confirmed{};
        bool valid{false};
        CommandSupport write{CommandSupport::Unknown};
        bool read_supported{false}, awaiting_readback{false}, rejected_valid{false}, readback_seen{false};
        PayloadBuffer requested{}, mask{}, verified_mask{}, rejected{};
        uint32_t last_rejection_ms{0}, sent_ms{0};
        WriteOutcome outcome{WriteOutcome::None};
    } d5_, d6_, d7_, dr_, d3_, fb_;
    CombinedCommand* combinedFor(std::string_view command);
    const CombinedCommand* combinedFor(std::string_view command) const;
    void confirmCombined(std::string_view command,const uint8_t* data);
    void expireWriteConfirmations();
    uint8_t swing_dirty_{0};
    bool demand_configured_{false};
    bool demandRequestAllowed(uint8_t percentage) const;
    void showModelProfiles() const;
    uint8_t verified_vertical_positions_{0};
    uint8_t humidity_modes_{0}, requested_humidity_{0};
    bool humidity_pending_{false};
    uint8_t demand_minimum_{100}, demand_step_{1}, requested_demand_{100};
    bool demand_pending_{false};
    int requested_vertical_position_{-1};
    bool vertical_pending_{false};
    void sendVerticalPositionCommand();
    void sendTimerCommand();
    void sendTimerLampCommand();
    void publishTimerStatus();
    bool rejectTimerRequest(bool lamp, AirConditionWriteResult reason);
    uint16_t timer_step_{0}, timer_max_{0};
    uint16_t requested_on_minutes_{0}, requested_off_minutes_{0};
    std::array<uint8_t,2> timer_disabled_bytes_{};
    uint8_t timer_dirty_{0};
    std::array<uint16_t,2> last_timer_minutes_{};
    bool timer_lamp_pending_{false}, requested_timer_lamp_{false};
    AirConditionWriteResult timer_request_error_{AirConditionWriteResult::None}, lamp_request_error_{AirConditionWriteResult::None};
    bool timer_pending_{false};
    std::string_view outstanding_write_{};
    bool sendCombined(std::string_view command, const PayloadBuffer& payload);
    void sendSpecialModesCommand();

    daikin::Identification identification_{};
    struct DiagnosticRequest { std::string command; std::vector<uint8_t> payload; };
    std::vector<DiagnosticRequest> diagnostic_queue_;
    DiagnosticResult diagnostic_result_{};
    bool diagnostic_active_{false}, diagnostic_ack_only_{false};
    struct RejectedResponse {
        uint32_t at{0};
        std::string command;
        std::vector<uint8_t> bytes;
        bool headerMismatch{false};
    };
    std::array<RejectedResponse,8> rejected_responses_{};
    size_t rejected_response_next_{0}, rejected_response_count_{0};
    void recordRejectedResponse(std::string_view command,const uint8_t* bytes,size_t size,bool headerMismatch);
    void showRejectedResponses() const;
    void showWriteDiagnostics() const;
    bool extended_polling_{true}, research_polling_{false};
#ifdef DAIKIN_PRODUCTION_DIAGNOSTICS
    bool production_polling_{true};
#else
    bool production_polling_{false};
#endif
    bool version_changed_{false};
    bool rz_states_trusted_{false};
    std::array<float,6> configured_min_{}, configured_max_{};
    uint8_t configured_temperature_mask_{0};
    daikin::ClimateVariant requested_variant_{daikin::ClimateVariant::Auto};
    bool variant_pending_{false};
    daikin::HumidityMode requested_humidity_mode_{daikin::HumidityMode::Unknown};
    bool encodeHumidity(daikin::HumidityMode mode, uint8_t& wire) const;
    void handle_metadata(std::string_view command, uint8_t* data, size_t size);
    void handle_aux_sensor(std::string_view command, uint8_t* data, size_t size);
    void serviceDiagnostic();
    std::string firmware_version_, model_name_;
    void handle_firmware_response(uint8_t* data, size_t size);

    TimingProfile timing_;
    // Serial communication
    std::unique_ptr<daikin::DaikinSerial> serial_;
    uint32_t last_query_cycle_{0};
    uint32_t last_protocol_detection_attempt_{0}; // Track when we last tried protocol detection
    uint32_t last_command_time_{0};  // Track when D1 commands are sent
    uint32_t last_send_time_{0};     // Track last serial send for pacing
    uint32_t last_query_time_{0};    // Track individual query timing

    // timing constants (optimized for v0 protocol stability)
    static constexpr uint32_t QUERY_CYCLE_INTERVAL_MS = 10000;      // 10 seconds for debugging
    // Retry-Intervall für Protokolldetektion, standard 10 s; via -DDAIKIN_PROTOCOL_DETECTION_RETRY_MS=1000 z. B. auf 1 s setzbar
    #ifdef DAIKIN_PROTOCOL_DETECTION_RETRY_MS
    static constexpr uint32_t PROTOCOL_DETECTION_RETRY_MS = DAIKIN_PROTOCOL_DETECTION_RETRY_MS;
    #else
    static constexpr uint32_t PROTOCOL_DETECTION_RETRY_MS = 10000;  // 10 seconds retry for unknown protocol
    #endif
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
    PayloadBuffer last_sent_d1_payload_{};
    uint32_t last_loop_time_{0}, last_framed_retry_ms_{0};
    daikin::State published_{};
    bool publication_initialized_{false};
    std::array<uint32_t,4> climate_generation_{} , sent_climate_generation_{};
    uint8_t sent_climate_dirty_{0};
    bool d1_awaiting_ack_{false};
    uint32_t climate_confirmation_started_{0};
    void rejectClimateWrite();
    void confirmClimateFields(uint8_t fields);
    void selectConfirmationQuery();
    void advanceQuery();
    size_t confirmation_resume_{SIZE_MAX};
    uint32_t last_confirmation_poll_{0};
    uint8_t confirmation_cursor_{0};
    bool climate_readback_seen_{false};
    void invalidateTelemetry();
    void invalidatePage(std::string_view command);
    WriteOutcome climate_outcome_{WriteOutcome::None};   // Dedupe identical D1 commands

    // Online/offline detection (robust S21 connectivity tracking)
    uint32_t last_rx_ok_ms_{0};            // Time of last ACK or valid frame
    bool online_{false};                   // Current online status
    uint32_t comm_start_time_{0};          // When startCommunication() was called
    bool startup_offline_published_{false}; // True after startup offline state was published
    static constexpr uint32_t STARTUP_OFFLINE_TIMEOUT_MS = 15000; // 15s before publishing offline defaults at startup

    // Protocol detection optimization
    bool old_protocol_detected_{false};    // Early detection flag
    uint32_t c0_response_count_{0};        // Count of 0xC0 responses
    bool first_cycle_completed_{false};    // Track first full query cycle
    bool protocol_detection_phase_{false}; // True during initial protocol detection
    bool protocol_detection_failed_reported_{false}; // avoid repeated error logs after giving up

    // protocol detection
    uint8_t protocol_detection_attempts_{0}; // Track F8→FY00 attempts (capped at 12)


    // === S21 Protocol Implementation ===
    void initializeProtocolDetection();  // Initialize protocol detection queries first
    void initializeQueries();
    void triggerQueryCycle();
    void updateQueryStateMachine();  // Non-blocking state machine
    void startQueryCycle();
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
    uint8_t vane_angle_divisor_{0}; // unknown until verified per model (1 or 10)
    void handle_vane_angle(bool requested, uint8_t* data, size_t size);
    void handle_fan_rpm(bool requested, uint8_t* data, size_t size);
    void handle_ri_response(uint8_t* data, size_t size);
    void handle_supplemental_sensor(std::string_view command, uint8_t* data, size_t size);
    void handle_status_sensor(std::string_view command, uint8_t* data, size_t size);
    void handle_fr_response(uint8_t* data, size_t data_size);
    void handle_fb_response(uint8_t* data, size_t data_size);
    void handle_f4_response(uint8_t* data, size_t data_size);
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
    void handle_fu_fx_response(std::string_view command, uint8_t* data, size_t data_size);
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

    // Conversion utilities
    daikin::Action calculateCurrentAction() const;
    daikin::DaikinFanMode daikin_to_fan_mode(uint8_t fan) const;

    // OpenKNX to Daikin conversions
    daikin::Mode openknx_to_daikin_mode(AirConditionMode mode) const;
    AirConditionMode daikin_to_openknx_mode(daikin::Mode mode) const;
    daikin::DaikinFanMode openknx_to_daikin_fan(unsigned int speed) const;
    unsigned int daikin_to_openknx_fan(daikin::DaikinFanMode fan) const;
};
