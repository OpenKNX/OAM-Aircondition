#include "DaikinDriver.h"
#include <Arduino.h>
#include <esp_heap_caps.h>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <cctype>
#include "utils.h"
#include "macros.h"

// Implements the Faikin S21 protocol (8972aa9) (https://github.com/revk/ESP32-Faikin/wiki/S21-Protocol#protocol-versions)

using PayloadBuffer = std::array<uint8_t, daikin::DaikinSerial::STANDARD_PAYLOAD_SIZE>; // Utility payload buffer for S21 commands

// S21 decoding functions
namespace {
    // s21_decode_target_temp function
    float s21_decode_target_temp(uint8_t v) {
        return 18.0f + 0.5f * (static_cast<int8_t>(v) - '@');
    }
    // s21_decode_int_sensor function
    int s21_decode_int_sensor(const uint8_t* payload) {
        int v = (payload[0] - '0') + (payload[1] - '0') * 10 + (payload[2] - '0') * 100;
        if (payload[3] == '-') {
            v = -v;
        }
        return v;
    }
    // s21_decode_float_sensor function
    float s21_decode_float_sensor(const uint8_t* payload) {
        return static_cast<float>(s21_decode_int_sensor(payload)) * 0.1f;
    }
    // s21_decode_hex_sensor function
    uint16_t s21_decode_hex_sensor(const uint8_t* payload) {
        auto hex = [](uint8_t c) { return ((c & 0xF) + ((c > '9') ? 9 : 0)); };
        return (hex(payload[3]) << 12) | (hex(payload[2]) << 8) | (hex(payload[1]) << 4) | hex(payload[0]);
    }
}

DaikinDriver::DaikinDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : AirConditionDriver(statusFeedback)
{
    // Initialize S21 protocol state with safe defaults
    state_.power = false;
    state_.homeC = 20.0f;
    state_.targetC = 22.0f;
    state_.fan = daikin::DaikinFanMode::Auto;
    state_.mode = daikin::Mode::Auto;
    state_.swing = daikin::Swing::Off;
    state_.online = false;
    
    // Initialize pending settings
    pending_.climate.power = false;
    pending_.climate.mode = daikin::Mode::Auto;
    pending_.climate.targetC = 22.0f;
    pending_.climate.fan_mode = daikin::DaikinFanMode::Auto;
    
    logInfoP("DaikinDriver created with S21 protocol support");
}

// === AirConditionDriver Interface Implementation ===
const std::string DaikinDriver::name() const
{
    return "Daikin S21";
}

void DaikinDriver::showInformations()
{
    if (!serial_) {
        logInfoP("Daikin S21 Driver: Not initialized");
        return;
    }

    logInfoP("=== Daikin S21 Driver Status ===");
    logInfoP("Online: %s", state_.online ? "Yes" : "No");
    logInfoP("Power: %s", state_.power ? "On" : "Off");
    
    // Mode
    const char* modeStr = "Unknown";
    switch (state_.mode) {
        case daikin::Mode::Auto: modeStr = "Auto"; break;
        case daikin::Mode::Cool: modeStr = "Cool"; break;
        case daikin::Mode::Dry: modeStr = "Dry"; break;
        case daikin::Mode::Heat: modeStr = "Heat"; break;
        case daikin::Mode::FanOnly: modeStr = "Fan Only"; break;
        default: modeStr = "Off"; break;
    }
    logInfoP("Mode: %s", modeStr);
    
    logInfoP("Target Temp: %.1f°C", state_.targetC);
    logInfoP("Current Temp: %.1f°C", state_.homeC);
    logInfoP("Outside Temp: %.1f°C", state_.outsideC);
    
    // Fan
    const char* fanStr = "Unknown";
    switch (state_.fan) {
        case daikin::DaikinFanMode::Auto: fanStr = "Auto"; break;
        case daikin::DaikinFanMode::Silent: fanStr = "Silent"; break;
        case daikin::DaikinFanMode::Speed1: fanStr = "Speed 1"; break;
        case daikin::DaikinFanMode::Speed2: fanStr = "Speed 2"; break;
        case daikin::DaikinFanMode::Speed3: fanStr = "Speed 3"; break;
        case daikin::DaikinFanMode::Speed4: fanStr = "Speed 4"; break;
        case daikin::DaikinFanMode::Speed5: fanStr = "Speed 5"; break;
        default: fanStr = "Unknown"; break;
    }
    logInfoP("Fan: %s", fanStr);
    
    // Swing
    logInfoP("Swing: %s", state_.swing == daikin::Swing::Vertical ? "Vertical" : 
             state_.swing == daikin::Swing::Horizontal ? "Horizontal" : 
             state_.swing == daikin::Swing::Both ? "Both" : "Off");
    
    // Humidity Mode
    const char* humidityModeStr = "Unknown";
    switch (state_.humidityMode) {
        case daikin::HumidityMode::Off: humidityModeStr = "Off"; break;
        case daikin::HumidityMode::Low: humidityModeStr = "Low"; break;
        case daikin::HumidityMode::Standard: humidityModeStr = "Standard"; break;
        case daikin::HumidityMode::High: humidityModeStr = "High"; break;
        case daikin::HumidityMode::Continuous: humidityModeStr = "Continuous"; break;
        default: humidityModeStr = "Unknown"; break;
    }
    logInfoP("Humidity Mode: %s", humidityModeStr);
    
    // Advanced features
    logInfoP("--- Advanced Features ---");
    logInfoP("Powerful: %s", state_.powerful ? "On" : "Off");
    logInfoP("Econo: %s", state_.econo ? "On" : "Off");
    logInfoP("Quiet: %s", state_.quiet ? "On" : "Off");
    logInfoP("Comfort: %s", state_.comfort ? "On" : "Off");
    logInfoP("Sensor: %s", state_.sensor ? "On" : "Off");  // Intelligent Eye/Presence sensor
    logInfoP("LED: %s", state_.led ? "On" : "Off");
    logInfoP("Streamer: %s", state_.streamer ? "On" : "Off");
    
    // Sensor data
    logInfoP("--- Sensors ---");
    logInfoP("Humidity: %d%%", state_.humidity);
    logInfoP("Total Energy: %u Wh", state_.totalEnergyWh);
    logInfoP("Compressor Frequency: %d Hz", state_.compressorHz);
    
    // Protocol information
    logInfoP("--- Protocol ---");
    if (protocol_version_ == daikin::ProtocolUndetected) {
        logInfoP("Protocol Version: unknown");
    } else {
        logInfoP("Protocol Version: %d.%d", protocol_version_.major, protocol_version_.minor);
    }
    
    // Communication statistics
    logInfoP("--- Communication Stats ---");
    logInfoP("Frames OK: %d", stats_.frames_ok);
    logInfoP("Bad Checksum: %d", stats_.frames_bad_checksum);
    logInfoP("Bad Format: %d", stats_.frames_bad_format);
    logInfoP("NAKs: %d", stats_.frames_nak);
    logInfoP("ACKs: %d", stats_.acks);
    logInfoP("Resyncs: %d", stats_.resyncs);
}

void DaikinDriver::setup()
{
    DAIKIN_DEBUG_PRINT("Setting up Daikin S21 driver");
    // Get the UART reference from OpenKNX configuration
    #ifdef OPENKNX_AIR_CONDITION_SERIAL
        HardwareSerial& ser = OPENKNX_AIR_CONDITION_SERIAL;
    #else
        #error "OPENKNX_AIR_CONDITION_SERIAL must be defined in build flags"
    #endif
    // Validate pin configuration
    #ifndef OPENKNX_AIR_CONDITION_SERIAL_RX
        #error "OPENKNX_AIR_CONDITION_SERIAL_RX must be defined in build flags"
    #endif
    #ifndef OPENKNX_AIR_CONDITION_SERIAL_TX  
        #error "OPENKNX_AIR_CONDITION_SERIAL_TX must be defined in build flags"
    #endif
    
    // Parse pin configuration from build flags
    // Negative pin numbers indicate inversion: -48 = pin 48 inverted, 48 = pin 48 normal
    int raw_rx_pin = OPENKNX_AIR_CONDITION_SERIAL_RX;
    int raw_tx_pin = OPENKNX_AIR_CONDITION_SERIAL_TX;
    
    bool rx_inverted = (raw_rx_pin < 0);
    bool tx_inverted = (raw_tx_pin < 0);
    int actual_rx_pin = abs(raw_rx_pin);
    int actual_tx_pin = abs(raw_tx_pin);

    DAIKIN_DEBUG_PRINT("S21 Serial Config: Port=%s, RX=Pin%d(%s), TX=Pin%d(%s)", 
             STRINGIFY(OPENKNX_AIR_CONDITION_SERIAL), 
             actual_rx_pin, rx_inverted ? "inverted" : "normal",
             actual_tx_pin, tx_inverted ? "inverted" : "normal");
    
    size_t free_heap = esp_get_free_heap_size();    // Check available memory
    
    if (free_heap < 8192) {  // Need at least 8KB free for safe operation
        logErrorP("Insufficient memory for S21 driver (free: %u bytes)", free_heap);
        statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError, 
                                         "Insufficient memory");
        return;
    }
    try {
        // Create the serial communication object with callbacks to this instance
        serial_ = std::make_unique<daikin::DaikinSerial>(
            ser, 
            actual_rx_pin,
            actual_tx_pin,
            [this](daikin::DaikinSerial::Result result, uint8_t* data, size_t data_size) { 
                handle_serial_result(result, data, data_size); 
            },
            [this]() { 
                handle_serial_idle(); 
            },
            rx_inverted, tx_inverted  // Use parsed inversion settings from build flags
        );
        
        logDebugP("DaikinSerial object created, initializing serial port...");
        serial_->begin();

        DAIKIN_DEBUG_PRINT("Daikin S21 driver initialized successfully");
        statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateOk);
        
    } catch (const std::exception& e) {
        logErrorP("Failed to initialize S21 driver: %s", e.what());
        serial_.reset();
        statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError, 
                                         std::string("Initialization failed: ") + e.what());
    }
}

void DaikinDriver::startCommunication(bool restart)
{
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError, 
                                         "Driver not initialized");
        return;
    }
    DAIKIN_DEBUG_PRINT("Starting S21 communication (restart: %s)", restart ? "true" : "false");
    statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
    
    if (restart) {
        // Reset state for restart
        state_ = {};
        stats_ = {};
        query_index_ = 0;
        failed_queries_.clear();
        protocol_version_ = daikin::ProtocolUndetected;
        support_ = {};
        // Reinitialize default state
        state_.homeC = 20.0f;
        state_.targetC = 22.0f;
        state_.fan = daikin::DaikinFanMode::Auto;
        state_.mode = daikin::Mode::Auto;
        state_.swing = daikin::Swing::Off;
        
        DAIKIN_DEBUG_PRINT("S21 driver state reset for restart");
    }
    
    // Start communication with smart protocol detection
    last_query_cycle_ = 0; // Force immediate cycle start
    
    // SMART LOGIC: Only detect protocol if unknown, otherwise use known protocol
    if (protocol_version_ == daikin::ProtocolUndetected) {
        DAIKIN_DEBUG_PRINT("Starting S21 communication with protocol detection phase (unknown protocol)");
        initializeProtocolDetection();
    } else {
        DAIKIN_DEBUG_PRINT("Starting S21 communication with known protocol v%d.%d", 
                 protocol_version_.major, protocol_version_.minor);
        initializeQueries(); // Use already-detected protocol
    }
    
    triggerQueryCycle();
}

void DaikinDriver::requestAllData()
{
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    DAIKIN_DEBUG_PRINT("Requesting all S21 data");
    triggerQueryCycle();
}

void DaikinDriver::loop()
{
    if (!serial_) {
        return;
    }
    
    static uint32_t last_loop_time = 0;
    uint32_t now = millis();
    // Detect high load conditions (loop taking too long)
    if (last_loop_time != 0 && (now - last_loop_time) > 20) {
        if (!high_load_detected_) {
            DAIKIN_DEBUG_PRINT("High load detected (%lu ms loop), increasing query spacing", now - last_loop_time);
            high_load_detected_ = true;
        }
    } else if (high_load_detected_ && (now - last_loop_time) <= 10) {
        DAIKIN_DEBUG_PRINT("Load normalized, returning to normal query spacing");
        high_load_detected_ = false;
    }
    last_loop_time = now;
    
    
    serial_->loop(); // Process serial communication (non-blocking)
    
    updateQueryStateMachine(); // non-blocking state machine
    
    // Protocol detection retry mechanism - every 5 minutes if protocol unknown
    if (protocol_version_ == daikin::ProtocolUndetected) {
        if (last_protocol_detection_attempt_ == 0 || 
            (now - last_protocol_detection_attempt_) >= PROTOCOL_DETECTION_RETRY_MS) {
            
            last_protocol_detection_attempt_ = now;
            DAIKIN_DEBUG_PRINT("Protocol unknown - retrying protocol detection (5 minute interval)");
            
            // Reset state and restart protocol detection
            initializeProtocolDetection();
            resetQueryNakTracking();  // reset bad query tracking on comm restart
            query_index_ = 0;
            query_state_ = QueryState::WaitingToSend;
            state_start_time_ = now;
        }
    }
    updateOnlineStatus();    // Update online status based on recent communication
}

// === Capability Queries ===

float DaikinDriver::getMinimumTargetTemperature()
{
    return 18.0f;  // S21 protocol minimum temperature
}

float DaikinDriver::getMaximumTargetTemperature()
{
    return 32.0f;  // S21 protocol maximum temperature
}

unsigned int DaikinDriver::getMaximumFanSpeed()
{
    return 5;  // Daikin fan speeds: Auto (0), Level 1-5 (1-5)
}

unsigned int DaikinDriver::getMaximumHorizontalFixPosition()
{
    return 0;  // S21 protocol doesn't support fixed positions, only swing on/off
}

unsigned int DaikinDriver::getMaximumVertiacalFixPosition()
{
    return 0;  // S21 protocol doesn't support fixed positions, only swing on/off
}

bool DaikinDriver::supportExternalRoomTemperatureSensor()
{
    return support_.sensor;  // Detected during protocol initialization
}

float DaikinDriver::accuracyInDegrees()
{
    return 0.5f;  // S21 protocol supports 0.5°C accuracy
}

// === Control Methods ===

void DaikinDriver::setPower(bool power)
{
    DAIKIN_DEBUG_PRINT("Setting power: %s (current internal state: %s)", 
             power ? "On" : "Off", 
             state_.power ? "On" : "Off");
    
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    // Update pending state - preserve current settings except power
    pending_.climate.power = power;           // UPDATE power state
    pending_.climate.mode = state_.mode;      // PRESERVE current mode
    pending_.climate.fan_mode = state_.fan;   // PRESERVE current fan
    pending_.climate.targetC = state_.targetC;// PRESERVE current temperature
    pending_.activate_climate = true;
    
    DAIKIN_DEBUG_PRINT("Power change requested: %s -> %s", 
             state_.power ? "On" : "Off",
             power ? "On" : "Off");
    
    // Send S21 power command - D1 controls power/mode/temp/fan
    sendClimateCommand();
}

void DaikinDriver::setMode(AirConditionMode mode)
{
    DAIKIN_DEBUG_PRINT("Setting mode: %d", static_cast<int>(mode));
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    daikin::Mode daikinMode = openknx_to_daikin_mode(mode); // Convert OpenKNX mode to Daikin mode
    
    // Update pending state - preserve current power and other settings
    pending_.climate.power = state_.power;     // PRESERVE current power state
    pending_.climate.mode = daikinMode;        // UPDATE mode
    pending_.climate.fan_mode = state_.fan;    // PRESERVE current fan
    pending_.climate.targetC = state_.targetC; // PRESERVE current temperature
    pending_.activate_climate = true;
    
    sendClimateCommand();     // Send S21 mode command
}

void DaikinDriver::setTargetTemperature(float temperaturCelsius)
{
    DAIKIN_DEBUG_PRINT("Setting target temperature: %.1f°C", temperaturCelsius);
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    float temp = std::clamp(temperaturCelsius, getMinimumTargetTemperature(), getMaximumTargetTemperature());  // Clamp temperature to valid range
    if (temp != temperaturCelsius) {
        DAIKIN_DEBUG_PRINT("Temperature clamped from %.1f°C to %.1f°C", temperaturCelsius, temp);
    }
    
    // Update pending state - preserve current power and other settings
    pending_.climate.power = state_.power;  // PRESERVE current power state
    pending_.climate.mode = state_.mode;    // PRESERVE current mode  
    pending_.climate.fan_mode = state_.fan; // PRESERVE current fan
    pending_.climate.targetC = temp;        // UPDATE temperature
    pending_.activate_climate = true;
    
    sendClimateCommand(); // Send S21 temperature command - D1 controls power/mode/temp/fan
}

void DaikinDriver::setFanSpeed(unsigned int speed)
{
    DAIKIN_DEBUG_PRINT("Setting fan speed: %u", speed);
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    if (speed > getMaximumFanSpeed()) {
        logErrorP("Invalid fan speed: %u (max: %u)", speed, getMaximumFanSpeed());
        return;
    }
    
    daikin::DaikinFanMode fanMode = openknx_to_daikin_fan(speed); // Convert speed to Daikin fan mode
    
    // Update pending state - preserve current power and other settings
    pending_.climate.power = state_.power;     // PRESERVE current power state
    pending_.climate.mode = state_.mode;       // PRESERVE current mode
    pending_.climate.fan_mode = fanMode;       // UPDATE fan mode
    pending_.climate.targetC = state_.targetC; // PRESERVE current temperature
    pending_.activate_climate = true;
    
    sendClimateCommand();     // Send S21 fan command
}

void DaikinDriver::setSwingHorizontal(bool swing)
{
    DAIKIN_DEBUG_PRINT("Setting horizontal swing: %s", swing ? "On" : "Off");
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    // Update pending state
    pending_.climate.swing_h = swing ? daikin::Swing::Horizontal : daikin::Swing::Off;
    pending_.activate_swing_mode = true;
    
    sendSwingCommand(); // Send S21 swing command - D5 controls louvre/swing mode
}

void DaikinDriver::setSwingVertical(bool swing)
{
    DAIKIN_DEBUG_PRINT("Setting vertical swing: %s", swing ? "On" : "Off");
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    // Update pending state
    pending_.climate.swing_v = swing ? daikin::Swing::Vertical : daikin::Swing::Off;
    pending_.activate_swing_mode = true;
    
    sendSwingCommand();// Send S21 swing command - D5 controls louvre/swing mode
}

void DaikinDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // S21 protocol doesn't support fixed positions
    logInfoP("Fixed horizontal positions not supported by S21 protocol (requested: %u)", position);
}

void DaikinDriver::setSwingVerticalFixPosition(unsigned int position)
{
    // S21 protocol doesn't support fixed positions
    logInfoP("Fixed vertical positions not supported by S21 protocol (requested: %u)", position);
}

void DaikinDriver::setExternalSensorRoomTemperature(float temperaturCelius)
{
    logInfoP("Setting external sensor temperature: %.1f°C", temperaturCelius);
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    if (!support_.sensor) {
        logInfoP("External sensor not supported by this AC unit");
        return;
    }
    
    // Update pending state
    pending_.climate.sensor_temp = temperaturCelius;
    pending_.activate_sensor = true;
    
    sendSensorCommand(); // Send S21 sensor command - D6 controls sensor
}

void DaikinDriver::setWifiLed(bool on)
{
    DAIKIN_DEBUG_PRINT("Setting WiFi LED: %s", on ? "On" : "Off");
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    // Defer LED command until after first cycle completes to reduce early contention
    if (!first_cycle_completed_) {
        logInfoP("Deferring LED command until after first query cycle completes");
        pending_.climate.led = on;
        pending_.activate_led = true;
        return;
    }
    
    // Update pending state
    pending_.climate.led = on;
    pending_.activate_led = true;
    
    sendLedCommand(); // Send S21 LED command - D9 controls LED
}

void DaikinDriver::setDeviceMode(AirConditionDeviceMode mode)
{
    logInfoP("Setting device mode: %d", static_cast<int>(mode));
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    // Map OpenKNX device modes to S21 preset commands
    switch (mode) {
        case AirConditionDeviceMode::AirConditionDeviceModeHiPower:
            pending_.climate.powerful = true;
            pending_.climate.econo = false;
            pending_.climate.quiet = false;
            pending_.activate_powerful = true;
            sendPowerfulCommand();
            break;
            
        case AirConditionDeviceMode::AirConditionDeviceModeSilent1:
        case AirConditionDeviceMode::AirConditionDeviceModeSilent2:
            pending_.climate.powerful = false;
            pending_.climate.econo = false;
            pending_.climate.quiet = true;
            pending_.activate_quiet = true;
            sendQuietCommand();
            break;
            
        case AirConditionDeviceMode::AirConditionDeviceModeEco:
            pending_.climate.powerful = false;
            pending_.climate.econo = true;
            pending_.climate.quiet = false;
            pending_.activate_econo = true;
            sendEconoCommand();
            break;
            
        case AirConditionDeviceMode::AirConditionDeviceModeStandard:
        default:
            // Disable all special modes
            pending_.climate.powerful = false;
            pending_.climate.econo = false;
            pending_.climate.quiet = false;
            pending_.activate_powerful = true; // Use powerful command to reset
            sendPowerfulCommand();
            break;
    }
}

void DaikinDriver::setMaxPowerLevel(uint8_t percentage)
{
    // S21 protocol doesn't support power level limiting
    logInfoP("Power level limiting not supported by S21 protocol (requested: %u%%)", percentage);
}

void DaikinDriver::setAirPurification(bool on)
{
    logInfoP("Setting air purification (streamer): %s", on ? "On" : "Off");
    if (!serial_) {
        logErrorP("S21 driver not initialized");
        return;
    }
    
    // Update pending state
    pending_.climate.streamer = on;
    pending_.activate_streamer = true;
    
    sendStreamerCommand(); // Send S21 streamer command - DA controls streamer
}

// === S21 Protocol Implementation ===

void DaikinDriver::initializeProtocolDetection()
{
    logInfoP("Starting S21 protocol detection phase");
    
    // Clear existing queries
    queries_.clear();
    query_index_ = 0;
    protocol_detection_phase_ = true;
    protocol_detection_attempts_ = 0;
    
    last_protocol_detection_attempt_ = millis();  // Mark that we're attempting protocol detection now
    
    // Reset polarity detection to start from Faikin combination (TX=inverted, RX=inverted)
    if (serial_) {
        serial_->reset_polarity_detection();
        current_polarity_attempt_ = 3; // Start with Faikin combo 3
        auto [rx_inv, tx_inv] = serial_->get_current_polarity();
        logInfoP("Starting with Faikin polarity combo 3: TX=%s RX=%s", 
                 tx_inv ? "inverted" : "normal", rx_inv ? "inverted" : "normal");
    }
    
    // protocol detection according to S21 wiki:
    // 1. Send F8 first - this determines base protocol (v0, v2, v3+)
    // 2. Send FY00 only if F8 indicates v2+ (to distinguish v2 vs v3.x)
    // Try this sequence across all polarity combinations before giving up
    queries_.emplace_back(StateQuery::OldProtocol, [this](uint8_t* data, size_t data_size) { handle_f8_response(data, data_size); });
    queries_.emplace_back(StateQuery::NewProtocol, [this](uint8_t* data, size_t data_size) { handle_fy00_response(data, data_size); });
    
    logInfoP("Protocol detection phase initialized with %zu queries (F8 → FY00) across %u polarity combinations", 
             queries_.size(), MAX_POLARITY_COMBOS);
}

void DaikinDriver::initializeQueries()
{
    logDebugP("Initializing S21 queries for protocol v%d.%d", protocol_version_.major, protocol_version_.minor);
    
    // Clear existing queries
    queries_.clear();
    query_index_ = 0;
    protocol_detection_phase_ = false;
    
    // Add ONLY supported queries based on detected protocol version
    // F1 - Basic status (supported by all protocols)
    queries_.emplace_back(StateQuery::Basic, [this](uint8_t* data, size_t data_size) { handle_f1_response(data, data_size); });
    
    // F2 - Optional features (supported by all protocols)
    queries_.emplace_back(StateQuery::OptionalFeatures, [this](uint8_t* data, size_t data_size) { handle_f2_response(data, data_size); });
    
    // F5 - Swing/Humidity (supported by all protocols)
    queries_.emplace_back(StateQuery::SwingOrHumidity, [this](uint8_t* data, size_t data_size) { handle_f5_response(data, data_size); });
    
    // Protocol version-specific queries
    uint8_t major = protocol_version_.major;
    uint8_t minor = protocol_version_.minor;
    
    if (major >= 1) {  // New protocol v1.0+
        // F6/F3 conditional logic:
        // Try F6 first, but if it's marked as bad, use F3 as fallback
        
        // First, try to find if F6 query exists and is marked bad
        bool f6_is_bad = false;
        for (const auto& query : queries_) {
            if (query.command == StateQuery::SpecialModes && query.bad) {
                f6_is_bad = true;
                break;
            }
        }
        
        if (!f6_is_bad && !support_.f6_special_modes) {
            // F6 not yet proven bad, add it first
            queries_.emplace_back(StateQuery::SpecialModes, [this](uint8_t* data, size_t data_size) { handle_f6_response(data, data_size); });
        } else {
            // F6 is bad or unsupported, use F3 fallback
            queries_.emplace_back(StateQuery::OnOffTimer, [this](uint8_t* data, size_t data_size) { handle_f3_response(data, data_size); });
            logInfoP("S21: Using F3 fallback for special modes (F6 unsupported)");
        }
        
        if (major >= 2) {  // Protocol v2.0+
            queries_.emplace_back(StateQuery::DemandAndEcono, [this](uint8_t* data, size_t data_size) { handle_f7_response(data, data_size); });
            queries_.emplace_back(StateQuery::InsideOutsideTemperatures, [this](uint8_t* data, size_t data_size) { handle_f9_response(data, data_size); });
            queries_.emplace_back(StateQuery::ModelCode, [this](uint8_t* data, size_t data_size) { handle_fc_response(data, data_size); });
            queries_.emplace_back(StateQuery::IRCounter, [this](uint8_t* data, size_t data_size) { handle_fg_response(data, data_size); });
            queries_.emplace_back(StateQuery::PowerConsumption, [this](uint8_t* data, size_t data_size) { handle_fm_response(data, data_size); });
            
            // additional capability discovery queries
            queries_.emplace_back(StateQuery::CapabilityN, [this](uint8_t* data, size_t data_size) { handle_fn_response(data, data_size); });
            queries_.emplace_back(StateQuery::CapabilityP, [this](uint8_t* data, size_t data_size) { handle_fp_response(data, data_size); });
            queries_.emplace_back(StateQuery::CapabilityQ, [this](uint8_t* data, size_t data_size) { handle_fq_response(data, data_size); });
            queries_.emplace_back(StateQuery::CapabilityS, [this](uint8_t* data, size_t data_size) { handle_fs_response(data, data_size); });
            queries_.emplace_back(StateQuery::CapabilityT, [this](uint8_t* data, size_t data_size) { handle_ft_response(data, data_size); });
            // FK is optional - only add for newer versions
            if (major >= 3) {
                queries_.emplace_back(StateQuery::CapabilityK, [this](uint8_t* data, size_t data_size) { handle_fk_response(data, data_size); });
            }
        }
    }
    
    // Environment queries (most are supported across protocols)
    queries_.emplace_back(EnvironmentQuery::InsideTemperature, [this](uint8_t* data, size_t data_size) { handle_rh_response(data, data_size); });
    queries_.emplace_back(EnvironmentQuery::TargetTemperature, [this](uint8_t* data, size_t data_size) { handle_rx_response(data, data_size); });
    queries_.emplace_back(EnvironmentQuery::FanMode, [this](uint8_t* data, size_t data_size) { handle_rg_response(data, data_size); });
    
    if (major >= 1) {  // New protocol environment queries
        queries_.emplace_back(EnvironmentQuery::OutsideTemperature, [this](uint8_t* data, size_t data_size) { handle_ra_response(data, data_size); });
        queries_.emplace_back(EnvironmentQuery::CompressorFrequency, [this](uint8_t* data, size_t data_size) { handle_rd_response(data, data_size); });
        queries_.emplace_back(EnvironmentQuery::IndoorHumidity, [this](uint8_t* data, size_t data_size) { handle_re_response(data, data_size); });
    }
    
    if (major >= 2) {  // Advanced environment queries for v2.0+
        queries_.emplace_back(EnvironmentQuery::CompressorOnOff, [this](uint8_t* data, size_t data_size) { handle_rg2_response(data, data_size); });
        
        if (major >= 3) {  // Very advanced queries for v3.0+
            queries_.emplace_back(EnvironmentQuery::UnitState, [this](uint8_t* data, size_t data_size) { handle_rzb2_response(data, data_size); });
            queries_.emplace_back(EnvironmentQuery::SystemState, [this](uint8_t* data, size_t data_size) { handle_rzc3_response(data, data_size); });
        }
    }
    
    logInfoP("Initialized %zu S21 queries for protocol v%d.%d (%s)", 
             queries_.size(), major, minor, 
             major == 0 ? "old protocol" : "new protocol");
}

void DaikinDriver::updateQueryStateMachine()
{
    uint32_t now = millis();
    
    switch (query_state_) {
        case QueryState::Idle:
            // Check if it's time to start a new query cycle
            if (now - last_query_cycle_ >= QUERY_CYCLE_INTERVAL_MS) {
                // Check if we're in command cooldown
                if (last_command_time_ != 0 && (now - last_command_time_) < COMMAND_COOLDOWN_MS) {
                    query_state_ = QueryState::Cooldown;
                    state_start_time_ = now;
                    logDebugP("Command cooldown active, waiting %lu ms", 
                             COMMAND_COOLDOWN_MS - (now - last_command_time_));
                } else {
                    triggerQueryCycle();
                }
            }
            break;
            
        case QueryState::WaitingToSend:
            // Check if enough time has passed since last send
            if (canSendQuery()) {
                // Find next supported query (skip bad queries)
                // Non-blocking: limit search iterations to prevent blocking
                constexpr int MAX_QUERY_SEARCH_ITERATIONS = 10;
                int search_iterations = 0;
                
                while (query_index_ < queries_.size() && search_iterations < MAX_QUERY_SEARCH_ITERATIONS) {
                    auto& query = queries_[query_index_];
                    search_iterations++;
                    
                    // Smart NAK tracking: skip bad queries
                    if (query.bad) {
                        logDebugP("Skipping bad query (marked as unsupported): %.*s", 
                                  static_cast<int>(query.command.size()), query.command.data());
                        query_index_++;
                        continue;
                    }
                    
                    if (isQuerySupported(query.command)) {
                        // Send this query
                        auto [rx_inv, tx_inv] = serial_->get_current_polarity();
                        DAIKIN_DEBUG_PRINT("S21: Protocol detection - sending %.*s (TX=%s, RX=%s)", 
                                  static_cast<int>(query.command.size()), query.command.data(),
                                  tx_inv ? "inverted" : "normal", rx_inv ? "inverted" : "normal");
                        
                        std::string cmd_str(query.command);
                        serial_->send_frame(cmd_str);
                        
                        last_send_time_ = now;
                        last_query_time_ = now;
                        query_state_ = QueryState::WaitingForAck; // Will transition to WaitingForGrace after ACK or directly on Frame
                        state_start_time_ = now;
                        break;
                    } else {
                        // Skip unsupported query
                        logDebugP("Skipping unsupported query: %.*s", 
                                  static_cast<int>(query.command.size()), query.command.data());
                        query_index_++;
                    }
                }
                
                if (query_index_ >= queries_.size()) {
                    // Cycle complete
                    if (protocol_detection_phase_) {
                        // Protocol detection phase complete for this polarity combination
                        protocol_detection_attempts_++;
                        
                        if (protocol_version_ != daikin::ProtocolUndetected) {
                            // Protocol detected
                            auto [rx_inv, tx_inv] = serial_->get_current_polarity();
                            logInfoP("Protocol detection: Successfully detected v%d.%d with polarity TX=%s RX=%s after %u attempts", 
                                     protocol_version_.major, protocol_version_.minor,
                                     tx_inv ? "inverted" : "normal", rx_inv ? "inverted" : "normal",
                                     protocol_detection_attempts_);
                            
                            initializeQueries(); // Initialize protocol-specific queries
                            query_index_ = 0;
                            query_state_ = QueryState::WaitingToSend;
                            state_start_time_ = now;
                            return; // Continue immediately with proper queries
                        }
                        
                        // No response with current polarity - check if we should try more attempts
                        if (protocol_detection_attempts_ < MAX_POLARITY_ATTEMPTS) {
                            // Try F8→FY00 again with same polarity
                            logDebugP("Protocol detection: No response, retry %u/%u with current polarity", 
                                      protocol_detection_attempts_, MAX_POLARITY_ATTEMPTS);
                            query_index_ = 0;
                            query_state_ = QueryState::WaitingToSend;
                            state_start_time_ = now;
                            return;
                        }
                        
                        // Max attempts reached with current polarity - try next polarity combo
                        if (current_polarity_attempt_ < MAX_POLARITY_COMBOS - 1) {
                            if (serial_->try_next_polarity_combo()) {
                                current_polarity_attempt_++;
                                protocol_detection_attempts_ = 0;
                                auto [rx_inv, tx_inv] = serial_->get_current_polarity();
                                logInfoP("Protocol detection: Trying polarity combo %u: TX=%s RX=%s", 
                                         current_polarity_attempt_, 
                                         tx_inv ? "inverted" : "normal", rx_inv ? "inverted" : "normal");
                                
                                query_index_ = 0;
                                query_state_ = QueryState::WaitingToSend;
                                state_start_time_ = now;
                                return;
                            }
                        }
                        
                        // All polarity combinations exhausted
                        logInfoP("Protocol detection: All polarity combinations exhausted, no AC unit detected");
                        logInfoP("Protocol detection phase completed, protocol remains unknown");
                        // Stay in current state, don't initialize queries
                        return;
                    }
                    
                    if (!first_cycle_completed_) {
                        first_cycle_completed_ = true;
                        logInfoP("First S21 query cycle completed with protocol v%d.%d", 
                                 protocol_version_.major, protocol_version_.minor);
                        
                        // Send any deferred LED commands now
                        if (pending_.activate_led) {
                            logInfoP("Sending deferred LED command after first cycle");
                            sendLedCommand();
                        }
                    }
                    
                    publishState();
                    query_index_ = 0;
                    last_query_cycle_ = now;
                    query_state_ = QueryState::Idle;
                }
            }
            break;
            
        case QueryState::WaitingForAck:
            // Non-blocking ACK wait with timeout
            if (now - state_start_time_ >= ACK_WAIT_TIMEOUT_MS) {
                logDebugP("Query timeout, entering grace period for late responses");
                query_state_ = QueryState::WaitingForGrace;
                state_start_time_ = now;
            }
            // Response handling is done in handle_serial_result callback
            break;
            
        case QueryState::WaitingForGrace:
            // Grace period for late responses
            if (now - state_start_time_ >= GRACE_PERIOD_MS) {
                if (query_index_ < queries_.size()) {
                    auto& query = queries_[query_index_];
                    logDebugP("Grace period expired for %.*s, advancing to next", 
                              static_cast<int>(query.command.size()), query.command.data());
                }
                query_index_++;
                query_state_ = QueryState::WaitingToSend;
                state_start_time_ = now;
            }
            // Still accept late responses during grace period
            break;
            
        case QueryState::Cooldown:
            // Wait for command cooldown to complete
            if (last_command_time_ == 0 || (now - last_command_time_) >= COMMAND_COOLDOWN_MS) {
                logDebugP("Command cooldown complete, resuming queries");
                query_state_ = QueryState::Idle;
                // Trigger immediate query cycle
                last_query_cycle_ = now - QUERY_CYCLE_INTERVAL_MS;
            }
            break;
    }
}

bool DaikinDriver::canSendQuery() const
{
    uint32_t now = millis();
    // Determine effective inter-query delay based on system load
    uint32_t effective_delay = high_load_detected_ ? INTER_QUERY_DELAY_HIGH_LOAD_MS : INTER_QUERY_DELAY_MS;
    // Check minimum time between sends (adaptive)
    if (last_send_time_ != 0 && (now - last_send_time_) < effective_delay) {
        return false;
    }
    // Check command cooldown
    if (last_command_time_ != 0 && (now - last_command_time_) < COMMAND_COOLDOWN_MS) {
        return false;
    }
    return true;
}

void DaikinDriver::triggerQueryCycle()
{
    uint32_t now = millis();
    // Don't start new cycle if one is already running
    if (query_state_ != QueryState::Idle) {
        return;
    }
    // Check command cooldown before starting
    if (last_command_time_ != 0 && (now - last_command_time_) < COMMAND_COOLDOWN_MS) {
        logDebugP("Query cycle delayed - in command cooldown (%lu ms remaining)",
                   COMMAND_COOLDOWN_MS - (now - last_command_time_));
        query_state_ = QueryState::Cooldown;
        state_start_time_ = now;
        return;
    }
    last_query_cycle_ = now;
    startQueryCycle();
}

void DaikinDriver::startQueryCycle()
{
    if (!serial_) {
        return;
    }
    logDebugP("Starting S21 query cycle (non-blocking)");
    query_index_ = 0;
    query_state_ = QueryState::WaitingToSend;
    state_start_time_ = millis();
}

bool DaikinDriver::isQuerySupported(std::string_view command) const
{
    // Check if this query has been marked as failed
    auto it = std::find(failed_queries_.begin(), failed_queries_.end(), command);
    if (it != failed_queries_.end()) {
        return false;
    }
    
    // Protocol version-based filtering according to S21 wiki support matrix
    uint8_t major = protocol_version_.major;
    uint8_t minor = protocol_version_.minor;
    
    // FY00 (New Protocol) query filtering according to wiki:
    if (command == StateQuery::NewProtocol) {
        // Skip FY00 if old protocol (v0) already detected
        if (old_protocol_detected_ || major == 0) {
            logDebugP("Skipping FY00 - old protocol v%d.%d detected", major, minor);
            return false;
        }
        // Skip FY00 if we're in protocol detection and haven't done F8 yet
        if (protocol_detection_phase_ && protocol_version_ == daikin::ProtocolUndetected) {
            // FY00 should only run AFTER F8 in protocol detection
            return true; // Will be controlled by query order
        }
        return true;
    }
    
    // F8 (Old Protocol) query filtering:
    if (command == StateQuery::OldProtocol) {
        // F8 is always supported in protocol detection phase
        if (protocol_detection_phase_) {
            return true;
        }
        // Skip F8 in normal operation if new protocol detected
        if (major > 0) {
            return false;
        }
        return true;
    }
    
    // Skip v2+/v3+ queries if old protocol detected
    if (old_protocol_detected_ || (major == 0)) {
        // Protocol v0 - only basic F commands supported per wiki
        if (command == StateQuery::ModelCode ||          // FC - v2+ only  
            command == StateQuery::IRCounter ||          // FG - v2+ only
            command == StateQuery::PowerConsumption ||   // FM - v2+ only
            command == StateQuery::DemandAndEcono ||     // F7 - v2+ only
            command == StateQuery::InsideOutsideTemperatures || // F9 - v2+ only
            command == EnvironmentQuery::UnitState ||    // RzB2 - newer protocols
            command == EnvironmentQuery::SystemState) {  // RzC3 - newer protocols
            logDebugP("Skipping v2+/v3+ query %.*s - old protocol v%d.%d detected", 
                      static_cast<int>(command.size()), command.data(), major, minor);
            return false;
        }
    }
    
    // Protocol v2.0 specific filtering per wiki
    if (major == 2 && minor == 0) {
        // v2.0 supports most commands but has some limitations
        // (No specific exclusions mentioned in wiki for v2.0)
    }
    
    // Protocol v3+ specific commands per wiki
    if (major < 3) {
        if (command.starts_with("FU") ||              // FU** - v3.20+ only per wiki
            command.starts_with("FX")) {              // FX** - v3+ only per wiki
            logDebugP("Skipping v3+ query %.*s - protocol v%d.%d too old", 
                      static_cast<int>(command.size()), command.data(), major, minor);
            return false;
        }
    }
    
    // Extended FU queries require v3.20+ per wiki
    if (major < 3 || (major == 3 && minor < 20)) {
        if (command.starts_with("FU0") && command.size() > 3) {  // FU02, FU04, etc.
            logDebugP("Skipping v3.20+ query %.*s - protocol v%d.%d too old", 
                      static_cast<int>(command.size()), command.data(), major, minor);
            return false;
        }
    }
    
    return true;
}

// === Command Sending Implementation ===

void DaikinDriver::sendClimateCommand()
{
    if (!serial_ || !pending_.activate_climate) {
        return;
    }
    logInfoP("Sending D1 command - power: %s, mode: %d, temp: %.1f, fan: %d", 
             pending_.climate.power ? "ON" : "OFF", 
             static_cast<int>(pending_.climate.mode),
             pending_.climate.targetC,
             static_cast<int>(pending_.climate.fan_mode));
    
    PayloadBuffer payload;
    
    // Byte 0: Power ('0'/'1' ASCII, not 0x00/0x01)
    payload[0] = pending_.climate.power ? '1' : '0';
    
    // Byte 1: Mode
    daikin::Mode mode_to_send;
    if (pending_.climate.power) {
        mode_to_send = pending_.climate.mode; // Turning ON - use requested mode
    } else {
        mode_to_send = state_.mode; // Turning OFF - use current running mode
    }
    switch (mode_to_send) {
        case daikin::Mode::Heat: payload[1] = '4'; break;      
        case daikin::Mode::Cool: payload[1] = '3'; break;      
        case daikin::Mode::FanOnly: payload[1] = '6'; break;   
        case daikin::Mode::Dry: payload[1] = '2'; break;       
        case daikin::Mode::Auto:                               
        case daikin::Mode::Off:
        default: payload[1] = '1'; break;                      
    }
    
    // Byte 2: Target temperature (format: (setpoint/5) + 28)
    float temp_to_send = pending_.climate.power ? pending_.climate.targetC : state_.targetC;
    int16_t setpoint_c10 = static_cast<int16_t>(temp_to_send * 10);  // Convert to C*10
    payload[2] = (setpoint_c10 / 5) + 28; 
    
    // Byte 3: Fan speed (preserve current fan when turning off)
    daikin::DaikinFanMode fan_to_send = pending_.climate.power ? pending_.climate.fan_mode : state_.fan;
    switch (fan_to_send) {
        case daikin::DaikinFanMode::Auto: payload[3] = 'A'; break;
        case daikin::DaikinFanMode::Speed1: payload[3] = '3'; break;
        case daikin::DaikinFanMode::Speed2: payload[3] = '4'; break;
        case daikin::DaikinFanMode::Speed3: payload[3] = '5'; break;
        case daikin::DaikinFanMode::Speed4: payload[3] = '6'; break;
        case daikin::DaikinFanMode::Speed5: payload[3] = '7'; break;
        case daikin::DaikinFanMode::Silent: payload[3] = 'B'; break; 
        default: payload[3] = 'A'; break; 
    }
    logInfoP("D1 payload: ['%c', '%c', 0x%02X, '%c'] (power=%c, mode=%c, temp=%.1f°C, fan=%c, fan_enum=%d)", 
             payload[0], payload[1], payload[2], payload[3],
             payload[0], payload[1], temp_to_send, payload[3], static_cast<int>(fan_to_send));
    
    // Debug: Show exact bytes being sent
    DAIKIN_DEBUG_PRINT("D1 hex bytes: [0x%02X, 0x%02X, 0x%02X, 0x%02X]", 
             payload[0], payload[1], payload[2], payload[3]);
    
    std::string cmd("D1");
    serial_->send_frame(cmd, payload.data(), 4);
    
    // Record command time for non-blocking cooldown
    last_command_time_ = millis();
    last_send_time_ = millis();
    // Interrupt current query cycle by transitioning to cooldown state
    if (query_state_ != QueryState::Idle) {
        query_state_ = QueryState::Cooldown;
        state_start_time_ = millis();
        logInfoP("D1 command sent, entering cooldown state for %lu ms", COMMAND_COOLDOWN_MS);
    }
    // DON'T update internal state immediately - wait for AC response
    logInfoP("D1 command sent at %lu ms, next queries delayed %lu ms for AC processing", 
             last_command_time_, COMMAND_COOLDOWN_MS);
    
    pending_.activate_climate = false;
}

void DaikinDriver::sendSwingCommand()
{
    if (!serial_ || !pending_.activate_swing_mode) {
        return;
    }
    logDebugP("Sending S21 swing command D5");

    // Build D5 payload: swing modes
    PayloadBuffer payload;
    payload[0] = swing_mode_to_daikin(pending_.climate.swing_v);
    payload[1] = swing_mode_to_daikin(pending_.climate.swing_h);
    
    std::string cmd(StateCommand::LouvreSwingMode);
    serial_->send_frame(cmd, payload.data(), 2);
    
    pending_.activate_swing_mode = false;
}

void DaikinDriver::sendPowerfulCommand()
{
    if (!serial_ || !pending_.activate_powerful) {
        return;
    }
    logDebugP("Sending S21 powerful command D2");
    
    PayloadBuffer payload;
    payload[0] = pending_.climate.powerful ? 0x01 : 0x00;
    
    std::string cmd(StateCommand::Powerful);
    serial_->send_frame(cmd, payload.data(), 1);
    
    pending_.activate_powerful = false;
}

void DaikinDriver::sendEconoCommand()
{
    if (!serial_ || !pending_.activate_econo) {
        return;
    }
    logDebugP("Sending S21 econo command D3");

    PayloadBuffer payload;
    payload[0] = pending_.climate.econo ? 0x01 : 0x00;
    
    std::string cmd(StateCommand::Econo);
    serial_->send_frame(cmd, payload.data(), 1);
    
    pending_.activate_econo = false;
}

void DaikinDriver::sendQuietCommand()
{
    if (!serial_ || !pending_.activate_quiet) {
        return;
    }
    logDebugP("Sending S21 quiet command D4");
    
    PayloadBuffer payload;
    payload[0] = pending_.climate.quiet ? 0x01 : 0x00;
    
    std::string cmd(StateCommand::Quiet);
    serial_->send_frame(cmd, payload.data(), 1);
    
    pending_.activate_quiet = false;
}

void DaikinDriver::sendSensorCommand()
{
    if (!serial_ || !pending_.activate_sensor) {
        return;
    }
    logDebugP("Sending S21 sensor command D6");
    
    PayloadBuffer payload;
    payload[0] = static_cast<uint8_t>(pending_.climate.sensor_temp * 2); // 0.5°C steps
    
    std::string cmd(StateCommand::Sensor);
    serial_->send_frame(cmd, payload.data(), 1);
    
    pending_.activate_sensor = false;
}

void DaikinDriver::sendLedCommand()
{
    if (!serial_ || !pending_.activate_led) {
        return;
    }
    logDebugP("Sending S21 LED command D7");
    
    PayloadBuffer payload;
    payload[0] = pending_.climate.led ? 0x01 : 0x00;
    
    std::string cmd(StateCommand::Led);
    serial_->send_frame(cmd, payload.data(), 1);
    
    pending_.activate_led = false;
}

void DaikinDriver::sendStreamerCommand()
{
    if (!serial_ || !pending_.activate_streamer) {
        return;
    }
    logDebugP("Sending S21 streamer command DA");
    
    PayloadBuffer payload;
    payload[0] = pending_.climate.streamer ? 0x01 : 0x00;
    
    std::string cmd(StateCommand::Streamer);
    serial_->send_frame(cmd, payload.data(), 1);
    
    pending_.activate_streamer = false;
}

// === S21 Response Handlers ===

void DaikinDriver::handle_f1_response(uint8_t* data, size_t data_size)
{
    if (data_size < 4) {
        logErrorP("F1 response too short: %zu bytes", data_size);
        return;
    }
    stats_.frames_ok++; // Count successful response
    
    // F1/G1 contains: power(ASCII), mode(ASCII), target_temp(encoded), fan(encoded)
    bool changed = false;  // Track if any parameter changed from remote control
    bool old_power = state_.power;
    daikin::Mode old_mode = state_.mode;
    float old_targetC = state_.targetC;
    
    // Power: ASCII '1' = on, '0' = off
    state_.power = (data[0] == '1');
    
    // Mode: ASCII character mapped through Faikin's lookup table
    // Faikin: "30721003"[payload[1] & 0x7] - '0' (FHCA456D mapped from AXDCHXF)
    uint8_t mode_char = data[1] & 0x7;
    const char* mode_map = "30721003";
    
    logDebugP("F1: raw_byte=0x%02X ('%c'), masked=0x%02X, mapped='%c', faikin_mode=%d", 
             data[1], (data[1] >= 32 && data[1] < 127) ? data[1] : '?', 
             mode_char, mode_map[mode_char], mode_map[mode_char] - '0');
    
    if (mode_char < 8) {
        uint8_t faikin_mode = mode_map[mode_char] - '0';
        // Convert Faikin mode to our enum: 0=Fan, 1=Heat, 2=Cool, 3=Auto, 7=Dry
        switch (faikin_mode) {
            case 0: state_.mode = daikin::Mode::FanOnly; break;
            case 1: state_.mode = daikin::Mode::Heat; break;
            case 2: state_.mode = daikin::Mode::Cool; break;
            case 3: state_.mode = daikin::Mode::Auto; break;
            case 7: state_.mode = daikin::Mode::Dry; break;
            default: 
                logInfoP("F1: Unknown faikin_mode=%d, defaulting to Auto", faikin_mode);
                state_.mode = daikin::Mode::Auto; 
                break;
        }
        // Detect mode changes from remote control
        if (old_mode != state_.mode) {
            DAIKIN_DEBUG_PRINTLN("Mode changed via remote control");
            changed = true;
        }
    }
    
    // Temperature: Faikin's s21_decode_target_temp() - only for heat/cool/auto modes
    if (state_.mode == daikin::Mode::Heat || state_.mode == daikin::Mode::Cool || state_.mode == daikin::Mode::Auto) {
        // s21_decode_target_temp: 18.0 + 0.5 * ((signed) v - '@')
        state_.targetC = 18.0f + 0.5f * (static_cast<int8_t>(data[2]) - '@');
        
        // Detect temperature changes from remote control (with tolerance for float comparison)
        if (abs(old_targetC - state_.targetC) > 0.1f) {
            DAIKIN_DEBUG_PRINTLN("Temperature changed via remote control");
            changed = true;
        }
    }
    // else: keep existing temperature (Faikin: doesn't have temp in other modes)
    
    // Fan: Only if RG (fan query) doesn't work - Faikin logic
    if (!support_.fan_mode_query) {
        daikin::DaikinFanMode old_fan = state_.fan;
        
        // Handle direct ASCII fan values from S21 protocol
        switch (data[3]) {
            case 'A': state_.fan = daikin::DaikinFanMode::Auto; break;
            case 'B': state_.fan = daikin::DaikinFanMode::Silent; break;  // Silent/Quiet mode
            case '3': state_.fan = daikin::DaikinFanMode::Speed1; break;
            case '4': state_.fan = daikin::DaikinFanMode::Speed2; break;
            case '5': state_.fan = daikin::DaikinFanMode::Speed3; break;
            case '6': state_.fan = daikin::DaikinFanMode::Speed4; break;
            case '7': state_.fan = daikin::DaikinFanMode::Speed5; break;
            default: 
                logInfoP("F1: Unknown fan mode ASCII '%c' (0x%02X), defaulting to Auto", data[3], data[3]);
                state_.fan = daikin::DaikinFanMode::Auto; 
                break;
        }
        
        // Detect fan mode changes from remote control
        if (old_fan != state_.fan) {
            DAIKIN_DEBUG_PRINTLN("Fan mode changed via remote control");
            changed = true;
        }
    }
    
    logInfoP("F1: power=%s, mode=%d, target=%.1f°C, fan=%d (ASCII: %c%c%02X%c)", 
              state_.power ? "ON" : "OFF", static_cast<int>(state_.mode), state_.targetC, 
              static_cast<int>(state_.fan), data[0], data[1], data[2], data[3]);
    
    // Check for any state changes
    if (changed || old_power != state_.power) {
        if (old_power != state_.power) {
            logInfoP("F1: AC power state CHANGED: %s -> %s", 
                     old_power ? "ON" : "OFF", state_.power ? "ON" : "OFF");
        }
        publishState();
    }
}

void DaikinDriver::handle_f2_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("F2 response empty");
        return;
    }
    if (data_size == 1) {
        logDebugP("F2: ACK only");
        return;
    }
    if (data_size < 2) {
        logErrorP("F2 response too short: %zu bytes", data_size);
        return;
    }
    stats_.frames_ok++;
    support_.swing = (data[0] & 0x01) != 0;
    support_.humidity = (data[0] & 0x02) != 0;
    support_.fan = (data[0] & 0x04) != 0;
    support_.sensor = (data[0] & 0x08) != 0;
    support_.streamer = (data[0] & 0x10) != 0;
    support_.led = (data[0] & 0x20) != 0;
    logDebugP("F2: swing=%d, humidity=%d, fan=%d, sensor=%d, streamer=%d, led=%d", 
              support_.swing, support_.humidity, support_.fan, support_.sensor, support_.streamer, support_.led);
}

void DaikinDriver::handle_f3_response(uint8_t* data, size_t data_size)
{
    if (data_size < 4) {
        logErrorP("F3 response too short: %zu bytes", data_size);
        return;
    }
    stats_.frames_ok++;
    
    // F3 response: Alternative special modes query (Faikin fallback for units that don't support F6)
    // Based on Faikin: F3 reports powerful state in byte 3 but lacks other modes
    // Faikin comment: "F3 does not report powerful state, so we give F6 a preference"
    // But for units that don't support F6 at all, F3 provides basic special mode info
    
    logDebugP("F3: Alternative special modes [%02X %02X %02X %02X]", 
              data[0], data[1], data[2], data[3]);
    
    // byte 3 contains powerful mode (bit 1 = 0x02)
    state_.powerful = (data[3] & 0x02) != 0;
    
    // F3 doesn't provide other special modes, so keep previous values for:
    // comfort, quiet, streamer, sensor, led (they remain unchanged)
    logDebugP("F3: powerful=%d (other special modes unchanged)", state_.powerful);
}

void DaikinDriver::handle_f5_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("F5 response empty");
        return;
    }
    if (data_size < 4) {
        logErrorP("F5 response too short: %zu bytes (expected 4-byte G5 response)", data_size);
        return;
    }
    stats_.frames_ok++;
    
    logInfoP("F5: raw G5 response [%02X %02X %02X %02X] (4 bytes: swing1, swing2, humidity, unknown)", 
            data_size >= 1 ? data[0] : 0,
            data_size >= 2 ? data[1] : 0, 
            data_size >= 3 ? data[2] : 0,
            data_size >= 4 ? data[3] : 0);
    
    // F5 response contains swing or humidity data
    if (support_.swing) {
        // Fixed swing decoding: F5 uses ASCII encoding in data[0]
        // ASCII '0' = off, '1' = vertical, '2' = horizontal, '7' = both
        char swing_char = data[0];
        switch (swing_char) {
            case '0': state_.swing = daikin::Swing::Off; break;
            case '1': state_.swing = daikin::Swing::Vertical; break;
            case '2': state_.swing = daikin::Swing::Horizontal; break;
            case '7': state_.swing = daikin::Swing::Both; break;
            default:
                logInfoP("F5: Unknown swing ASCII char '%c' (0x%02X), defaulting to Off", swing_char, swing_char);
                state_.swing = daikin::Swing::Off;
                break;
        }
        logInfoP("F5: swing=%d (ASCII='%c')", static_cast<int>(state_.swing), swing_char);
    }
    
    // Decode humidity mode from byte 2 (3rd byte in G5 response)
    if (data_size >= 3) {
        uint8_t humidity_byte = data[2];
        switch (humidity_byte) {
            case 0x00: // Off (field observation)
                state_.humidityMode = daikin::HumidityMode::Off;
                break;
            case 0x3A: // Low (field observation)
                state_.humidityMode = daikin::HumidityMode::Low;
                break;
            case 0x3B: // Standard (field observation)
                state_.humidityMode = daikin::HumidityMode::Standard;
                break;
            case 0x3C: // High (field observation)
                state_.humidityMode = daikin::HumidityMode::High;
                break;
            case 0xFF: // Continuous (field observation)
                state_.humidityMode = daikin::HumidityMode::Continuous;
                break;
            default:
                logInfoP("F5: Unknown humidity mode byte 0x%02X, defaulting to Off", humidity_byte);
                state_.humidityMode = daikin::HumidityMode::Off;
                break;
        }
        logInfoP("F5: humidityMode=%d (byte=0x%02X)", static_cast<int>(state_.humidityMode), humidity_byte);
    }
}

void DaikinDriver::handle_f6_response(uint8_t* data, size_t data_size)
{
    if (data_size < 4) { 
        logErrorP("F6 response too short: %zu bytes", data_size);
        return;
    }
    stats_.frames_ok++;
    
    support_.f6_special_modes = true; // Mark F6 as supported since we got a successful response
  
    state_.powerful = (data[0] & 0x02) != 0; // payload[0] & 0x02 = powerful
    state_.comfort = (data[0] & 0x40) != 0;  // payload[0] & 0x40 = comfort  
    state_.quiet = (data[0] & 0x80) != 0;    // payload[0] & 0x80 = quiet
    state_.streamer = (data[1] & 0x80) != 0; // payload[1] & 0x80 = streamer
    
    // Only set sensor state if hardware is available (from FK detection)
    if (support_.sensor_hardware) {
        state_.sensor = (data[3] & 0x08) != 0;   // F6 byte 3 bit 3: "Sensor" mode active
    } else {
        state_.sensor = false; // No sensor hardware, so sensor mode is always false
    }
    
    // F6 byte 3 bit definitions (per S21 protocol spec):
    // - Bit 3 (0x08): "Sensor" mode on 
    // - Bits 3,4 (mask 0x0C): if both set (0x0C), LED is turned off
    state_.led = (data[3] & 0x0C) != 0x0C;   // LED is ON when both bits 3&4 are NOT set (S21 spec compliant)
    
    logInfoP("F6: powerful=%d, comfort=%d, quiet=%d, streamer=%d, sensor=%d (hw_avail=%d), led=%d (raw: %02X %02X %02X %02X)", 
             state_.powerful, state_.comfort, state_.quiet, state_.streamer, state_.sensor, support_.sensor_hardware, state_.led,
             data[0], data[1], data[2], data[3]);
}

void DaikinDriver::handle_f7_response(uint8_t* data, size_t data_size)
{
    if (data_size < 2) {
        logErrorP("F7 response too short: %zu bytes", data_size);
        return;
    }
    stats_.frames_ok++;

    if (data[0] != '1') { 
        uint8_t demand_raw = data[0] - '0'; // Demand percentage calculation (as per S21 wiki)   
        if (demand_raw <= 9) { // Sanity check for ASCII digit
            state_.demand_percentage = 100 - demand_raw;
        }
    }
    state_.econo = (data[1] & 0x02) != 0; // econo: payload[1] & 0x02 (as per S21 wiki) 
    
    logInfoP("F7: demand=%d%%, econo=%d (raw: %c %02X)", 
             state_.demand_percentage, state_.econo, data[0], data[1]);
}

void DaikinDriver::handle_f8_response(uint8_t* data, size_t data_size)
{
    if (data_size >= 4) {
        stats_.frames_ok++;
        
        // Parse F8 response according to S21 wiki:
        // v0: "G8 '0' 0x00 0x00 0x00" 
        // v2: "G8 '0' '2' 0x00 0x00"
        // v3: "G8 '0' '2' 0x00 0x00" (same as v2, need FY00 to distinguish)
        // v3.1+: "G8 0200" (different format)
        
        logInfoP("F8: Received G8 response [%02X %02X %02X %02X]", data[0], data[1], data[2], data[3]);
        
        // Check if it's ASCII format ('0', '2') or binary format
        if (data[0] == '0' && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x00) {
            // Protocol v0.0 - old protocol
            protocol_version_ = {0, 0};
            old_protocol_detected_ = true;
            logInfoP("F8: Protocol v0.0 detected (old protocol) - skipping FY00");
        } else if (data[0] == '0' && data[1] == '2' && data[2] == 0x00 && data[3] == 0x00) {
            // Protocol v2+ detected - need FY00 to distinguish v2.0 vs v3.x
            logInfoP("F8: Protocol v2+ detected (G8 '0' '2') - sending FY00 to distinguish v2 vs v3");
            // Don't set protocol_version_ yet - wait for FY00 response
        } else if (data[0] == 0x02 && data[1] == 0x00) {
            // Protocol v3.1+ format "G8 0200"
            logInfoP("F8: Protocol v3.1+ detected (G8 0200) - sending FY00 for exact version");
            // Don't set protocol_version_ yet - wait for FY00 response
        } else {
            // Unknown F8 format - assume v2+ and let FY00 determine
            logInfoP("F8: Unknown G8 format [%02X %02X %02X %02X] - assuming v2+ and trying FY00", 
                     data[0], data[1], data[2], data[3]);
        }
        state_.online = true;  // Mark AC as online since it responded to F8
        
        return;
    } else if (data_size >= 1) {
        stats_.frames_ok++; // Count any F8 response
        logInfoP("F8: Short response (%zu bytes) - assuming basic protocol", data_size);
        // Fallback for units that give short responses
        protocol_version_ = {1, 0}; // Assume basic new protocol
        old_protocol_detected_ = false;
        state_.online = true;
        return;
    }
    
    DAIKIN_DEBUG_PRINT("F8: Protocol detection query sent");
}

void DaikinDriver::handle_f9_response(uint8_t* data, size_t data_size)
{
    if (data_size < 4) {
        logErrorP("F9 response too short: %zu bytes", data_size);
        return;
    }
    stats_.frames_ok++;
    
    // F9 response format: G9 with "Home temp (@ based) | Outside temp (@ based)"
    // However, the actual format uses a different encoding: (temp * 10) / 5 + 0x80
    // To decode: temp = ((byte - 0x80) * 5) / 10.0
    
    // Decode inside temperature (byte 0)
    if (data[0] >= 0x80) {
        float inside_temp = ((data[0] - 0x80) * 5.0f) / 10.0f;
        state_.homeC = inside_temp;
        logDebugP("F9: inside=%.1f°C (from 0x%02X)", inside_temp, data[0]);
    } else {
        logDebugP("F9: inside temp byte 0x%02X out of range (< 0x80)", data[0]);
        state_.homeC = 22.0f; // Default
    }
    
    // Decode outside temperature (byte 1) 
    if (data[1] >= 0x80) {
        float outside_temp = ((data[1] - 0x80) * 5.0f) / 10.0f;
        state_.outsideC = outside_temp;
        logDebugP("F9: outside=%.1f°C (from 0x%02X)", outside_temp, data[1]);
    } else {
        logDebugP("F9: outside temp byte 0x%02X out of range (< 0x80)", data[1]);
        state_.outsideC = 25.0f; // Default
    }
}

void DaikinDriver::handle_fc_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("FC response empty");
        return;
    }
    // FC response contains model code
    if (data_size >= 4) {
        stats_.frames_ok++;
        // Store in a local variable since model_code might not exist in State
        uint32_t model_code = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
        logDebugP("FC: model_code=0x%08X", model_code);
    }
}

void DaikinDriver::handle_fg_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("FG response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("FG: Single-byte ACK received (0x%02X) - no IR counter data", response);
        return;
    }
    // FG response contains IR counter
    if (data_size >= 2) {
        stats_.frames_ok++;
        state_.irCounter = (data[1] << 8) | data[0];
        logDebugP("FG: ir_counter=%d", state_.irCounter);
    }
}

void DaikinDriver::handle_fm_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("FM response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("FM: Single-byte ACK received (0x%02X) - no power consumption data", response);
        return; 
    }
    // FM response contains power consumption
    if (data_size >= 4) {
        stats_.frames_ok++; // Count actual power consumption data response
        
        // FM response: 4 digit ASCII encoded hex number in reverse order
        // Indicates total energy consumption in 100Wh units
        // Convert from ASCII hex chars to actual hex value, accounting for reverse order
        auto hex = [](uint8_t c) { 
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            return 0;
        };
        
        // Reverse order: data[3] data[2] data[1] data[0] -> actual hex value
        uint16_t hex_value = (hex(data[3]) << 12) | (hex(data[2]) << 8) | (hex(data[1]) << 4) | hex(data[0]);
        
        // Convert to total energy consumption in Wh (multiply by 100)
        state_.totalEnergyWh = hex_value * 100;
        
        logInfoP("FM: total_energy=%u Wh (hex=0x%04X, raw=%c%c%c%c)", 
                 state_.totalEnergyWh, hex_value, data[0], data[1], data[2], data[3]);
    }
}

void DaikinDriver::handle_fy00_response(uint8_t* data, size_t data_size)
{
    if (data_size >= 4) {
        // Multi-byte GY00 response - parse exact protocol version
        // According to S21 wiki: "GY00 0030" = v3.00, "GY00 0230" = v3.20, etc.
        // Format: GY00 followed by version as ASCII: major=position2, minor=position1*10+position3
        
        // Parse protocol version from GY00 payload using wiki format
        const uint8_t d0 = data[0], d1 = data[1], d2 = data[2], d3 = data[3];
        const auto toN = [](uint8_t c){ return (c>='0' && c<='9') ? (c-'0') : -1; };
        int n0=toN(d0), n1=toN(d1), n2=toN(d2), n3=toN(d3);
        
        if (n0>=0 && n1>=0 && n2>=0 && n3>=0) {
            // Wiki format: major=n2, minor=n1*10+n3
            uint8_t major = uint8_t(n2);
            uint8_t minor = uint8_t(n1*10 + n3);
            
            protocol_version_ = {major, minor};
            old_protocol_detected_ = false;
            logInfoP("FY00: Protocol version %d.%02d parsed from GY00 response [%c%c%c%c] (raw: %02X %02X %02X %02X)", 
                     major, minor, d0, d1, d2, d3, d0, d1, d2, d3);
        } else {
            // Fall back to v3.0 if parsing fails
            protocol_version_ = {3, 0}; 
            old_protocol_detected_ = false;
            logErrorP("FY00: Failed to parse version from [%02X %02X %02X %02X], defaulting to v3.0", 
                      data[0], data[1], data[2], data[3]);
        }
        
        stats_.frames_ok++;
    } else if (data_size > 1) {
        // Short multi-byte response - indicates v3+ but can't parse exact version
        protocol_version_ = {3, 0}; // Default v3.0
        old_protocol_detected_ = false;
        logInfoP("FY00: New protocol detected (short response), defaulting to v3.0");
        stats_.frames_ok++;
    }
    
    // Handle NAK case for FY00 according to wiki:
    // If we get here without setting protocol_version_, and F8 indicated v2+, then it's v2.0
    // (because v2.0 units NAK FY00, but v3+ units respond with GY00)
}

void DaikinDriver::handle_fn_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("FN response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("FN: Single-byte ACK received (0x%02X) - no capability data", response);
        return;
    }
    // FN capability query - reported as first 4 bytes of itelc= in /aircon/get_monitordata
    if (data_size >= 4) {
        stats_.frames_ok++;
        
        logInfoP("FN: Capability data (itelc) [%02X %02X %02X %02X]", 
                 data[0], data[1], data[2], data[3]);
        // Store the capability data if needed for future reference
        // This data appears in Daikin's get_monitordata response as itelc= parameter
    }
}

void DaikinDriver::handle_fp_response(uint8_t* data, size_t data_size)
{
    // FP capability/feature query - purpose not fully documented but used by Faikin for feature detection
    if (data_size > 0) {
        if (data_size == 1) { // Single byte ACK
            DAIKIN_DEBUG_PRINT("FP response: ACK (0x%02X)", data[0]);
            stats_.frames_ok++;
            return;
        }
        // Multi-byte capability data
        if (data_size >= 4) {
            DAIKIN_DEBUG_PRINT("FP capability data: %zu bytes", data_size);
            for (size_t i = 0; i < data_size; i++) {
                DAIKIN_DEBUG_PRINT("  FP[%zu] = 0x%02X", i, data[i]);
            }
        } else {
            DAIKIN_DEBUG_PRINT("FP response: %zu bytes (unexpected size)", data_size);
        }
        stats_.frames_ok++;
        // Store capability data if needed for future feature detection
    }
}

void DaikinDriver::handle_fq_response(uint8_t* data, size_t data_size)
{
    // FQ capability/feature query - purpose not fully documented but used by Faikin for feature detection
    if (data_size > 0) {
        if (data_size == 1) { // Single byte ACK
            DAIKIN_DEBUG_PRINT("FQ response: ACK (0x%02X)", data[0]);
            stats_.frames_ok++;
            return;
        }
        // Multi-byte capability data
        if (data_size >= 4) {
            DAIKIN_DEBUG_PRINT("FQ capability data: %zu bytes", data_size);
            for (size_t i = 0; i < data_size; i++) {
                DAIKIN_DEBUG_PRINT("  FQ[%zu] = 0x%02X", i, data[i]);
            }
        } else {
            DAIKIN_DEBUG_PRINT("FQ response: %zu bytes (unexpected size)", data_size);
        }
        stats_.frames_ok++;
        // Store capability data if needed for future feature detection
    }
}

void DaikinDriver::handle_fs_response(uint8_t* data, size_t data_size)
{
    // FS capability/feature query - purpose not fully documented but used by Faikin for feature detection
    if (data_size > 0) {
        if (data_size == 1) {         // Single byte ACK
            DAIKIN_DEBUG_PRINT("FS response: ACK (0x%02X)", data[0]);
            stats_.frames_ok++;
            return;
        }
        // Multi-byte capability data
        if (data_size >= 4) {
            DAIKIN_DEBUG_PRINT("FS capability data: %zu bytes", data_size);
            for (size_t i = 0; i < data_size; i++) {
                DAIKIN_DEBUG_PRINT("  FS[%zu] = 0x%02X", i, data[i]);
            }
        } else {
            DAIKIN_DEBUG_PRINT("FS response: %zu bytes (unexpected size)", data_size);
        }
        stats_.frames_ok++;
        // Store capability data if needed for future feature detection
    }
}

void DaikinDriver::handle_ft_response(uint8_t* data, size_t data_size)
{
    // FT capability/feature query - purpose not fully documented but used by Faikin for feature detection
    if (data_size > 0) {
        if (data_size == 1) { // Single byte ACK
            DAIKIN_DEBUG_PRINT("FT response: ACK (0x%02X)", data[0]);
            stats_.frames_ok++;
            return;
        }
        // Multi-byte capability data
        if (data_size >= 4) {
            DAIKIN_DEBUG_PRINT("FT capability data: %zu bytes", data_size);
            for (size_t i = 0; i < data_size; i++) {
                DAIKIN_DEBUG_PRINT("  FT[%zu] = 0x%02X", i, data[i]);
            }
        } else {
            DAIKIN_DEBUG_PRINT("FT response: %zu bytes (unexpected size)", data_size);
        }
        stats_.frames_ok++;
        // Store capability data if needed for future feature detection
    }
}

void DaikinDriver::handle_fk_response(uint8_t* data, size_t data_size)
{
    // FK capability/feature query - optional features, used by Faikin for feature detection
    if (data_size > 0) { // Single byte ACK
        if (data_size == 1) {
            DAIKIN_DEBUG_PRINT("FK response: ACK (0x%02X)", data[0]);
            stats_.frames_ok++;
            return;
        }
        // Multi-byte capability data
        if (data_size >= 4) {
            stats_.frames_ok++;
            
            // Parse FK capabilities according to S21 protocol spec
            // byte1 bit 3 - "motion detector" AKA "Intelligent eye" availability
            bool sensor_hardware_available = (data[1] & 0x08) != 0;
            support_.sensor_hardware = sensor_hardware_available;
            
            logInfoP("FK capabilities: sensor_hardware=%d (byte1=0x%02X, bit3=%d)", 
                     sensor_hardware_available, data[1], (data[1] & 0x08) ? 1 : 0);
            
            DAIKIN_DEBUG_PRINT("FK capability data: %zu bytes", data_size);
            for (size_t i = 0; i < data_size; i++) {
                DAIKIN_DEBUG_PRINT("  FK[%zu] = 0x%02X", i, data[i]);
            }
        } else {
            DAIKIN_DEBUG_PRINT("FK response: %zu bytes (unexpected size)", data_size);
            stats_.frames_ok++;
        }
        // Store capability data if needed for future feature detection
    }
}

void DaikinDriver::handle_rg_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("RG response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RG: Single-byte ACK received (0x%02X) - no fan mode data", response);
        return;
    }
    // Multi-byte response contains fan mode
    if (data_size >= 1) {
        stats_.frames_ok++; 
        
        support_.fan_mode_query = true; // Mark that fan mode query is supported since we got multi-byte data
        
        daikin::DaikinFanMode old_fan = state_.fan;
        state_.fan = daikin_to_fan_mode(data[0]);
        logDebugP("RG: fan_mode=%d", static_cast<int>(state_.fan));
        
        // Report fan speed change if it changed
        if (old_fan != state_.fan) {
            logInfoP("RG: Fan speed changed from %d to %d", static_cast<int>(old_fan), static_cast<int>(state_.fan));
            publishState(); // Trigger immediate state update
        }
    }
}

void DaikinDriver::handle_rh_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("RH response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RH: Single-byte ACK received (0x%02X) - no inside temperature data", response);
        return; // Single-byte response processed
    }
    // RH/SH response: expect 4-byte sensor response (SH case)
    if (data_size >= 4) {
        stats_.frames_ok++;
        float temp = s21_decode_float_sensor(data);
        
        if (temp < 100.0f) { //sanity check
            state_.homeC = temp;
            logInfoP("RH/SH: inside_temp=%.1f°C (raw: %c%c%c%c)", 
                     state_.homeC, data[0], data[1], data[2], data[3]);
        } else {
            logErrorP("RH/SH: Invalid temperature %.1f°C", temp);
        }
    } else if (data_size >= 2) { // Fallback
        stats_.frames_ok++;
        state_.homeC = static_cast<float>(static_cast<int16_t>((data[1] << 8) | data[0])) / 10.0f;
        logDebugP("RH legacy: inside_temp=%.1f°C", state_.homeC);
    }
}

void DaikinDriver::handle_rx_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("RX response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RX: Single-byte ACK received (0x%02X) - no target temperature data", response);
        return; // Single-byte response processed
    }
    // RX/SX response: expect 4-byte sensor response (SX case)
    if (data_size >= 4) {
        stats_.frames_ok++;
        float temp = s21_decode_float_sensor(data);
        
        if (temp < 100.0f) { //sanity check
            state_.realTargetC = temp;  // Store sensor-adjusted target
            
            // If real target differs from set target, sensor is actively adjusting
            bool sensor_active = std::abs(state_.realTargetC - state_.targetC) > 0.5f;
            if (sensor_active != state_.sensor) {
                state_.sensor = sensor_active;
                logInfoP("Sensor activity via RX: %s (real: %.1f°C, set: %.1f°C)", 
                         sensor_active ? "active" : "inactive", 
                         state_.realTargetC, state_.targetC);
            }
            
            logInfoP("RX/SX: sensor_adjusted_target=%.1f°C (raw: %c%c%c%c)", 
                     state_.realTargetC, data[0], data[1], data[2], data[3]);
        } else {
            logErrorP("RX/SX: Invalid temperature %.1f°C", temp);
        }
    } else if (data_size >= 2) { // Fallback
        stats_.frames_ok++;
        state_.realTargetC = static_cast<float>(static_cast<int16_t>((data[1] << 8) | data[0])) / 10.0f;
        
        // Check for sensor activity via temperature difference
        bool sensor_active = std::abs(state_.realTargetC - state_.targetC) > 0.5f;
        if (sensor_active != state_.sensor) {
            state_.sensor = sensor_active;
            logInfoP("Sensor activity via RX legacy: %s (real: %.1f°C, set: %.1f°C)", 
                     sensor_active ? "active" : "inactive", 
                     state_.realTargetC, state_.targetC);
        }
        
        logDebugP("RX legacy: sensor_adjusted_target=%.1f°C", state_.realTargetC);
    }
}

void DaikinDriver::handle_ra_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("Ra response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        DAIKIN_DEBUG_PRINT("Ra: Single-byte ACK received (0x%02X) - no outside temperature data", response);
        return; 
    }
    // Ra/Sa response: expect 4-byte sensor response (Sa case)
    if (data_size >= 4) {
        stats_.frames_ok++;
        float temp = s21_decode_float_sensor(data);
        
        if (temp < 100.0f) { //sanity check
            state_.outsideC = temp;
            DAIKIN_DEBUG_PRINT("Ra/Sa: outside_temp=%.1f°C (raw: %c%c%c%c)", 
                     state_.outsideC, data[0], data[1], data[2], data[3]);
        } else {
            logErrorP("Ra/Sa: Invalid temperature %.1f°C", temp);
        }
    } else if (data_size >= 2) { // Fallback
        stats_.frames_ok++;
        state_.outsideC = static_cast<float>(static_cast<int16_t>((data[1] << 8) | data[0])) / 10.0f;
        DAIKIN_DEBUG_PRINT("Ra legacy: outside_temp=%.1f°C", state_.outsideC);
    }
}

void DaikinDriver::handle_rd_response(uint8_t* data, size_t data_size)
{
    // Rd response contains compressor frequency
    if (data_size >= 2) {
        stats_.frames_ok++;
        state_.compressorHz = (data[1] << 8) | data[0];
        DAIKIN_DEBUG_PRINT("Rd: compressor_freq=%dHz", state_.compressorHz);
    }
}

void DaikinDriver::handle_re_response(uint8_t* data, size_t data_size)
{
    // Re response contains indoor humidity
    if (data_size >= 1) {
        stats_.frames_ok++;
        state_.humidity = data[0];
        DAIKIN_DEBUG_PRINT("Re: humidity=%d%%", state_.humidity);
    }
}

void DaikinDriver::handle_rg2_response(uint8_t* data, size_t data_size)
{
    // Rg response contains compressor on/off
    if (data_size >= 1) {
        stats_.frames_ok++;
        bool compressor_on = (data[0] & 0x01) != 0; // Process compressor state (not directly stored in State)
        DAIKIN_DEBUG_PRINT("Rg: compressor_on=%d", compressor_on);
    }
}

void DaikinDriver::handle_rzb2_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("RzB2 response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RzB2: Single-byte ACK received (0x%02X) - no unit state data", response);
        return;
    }
    // RzB2 response contains unit state bitfield
    if (data_size >= 2) {
        stats_.frames_ok++;
        
        state_.unitState = daikin::DaikinUnitState(data[0]); // Parse unit state bitfield using DaikinUnitState class
        
        logInfoP("RzB2: Unit state bitfield 0x%02X - powerful=%d, defrost=%d, active=%d, online=%d", 
                 data[0],
                 state_.unitState.powerful(), 
                 state_.unitState.defrost(), 
                 state_.unitState.active(), 
                 state_.unitState.online());
        
        // Update online status from unit state if available
        if (state_.unitState.online()) {
            state_.online = true;
        }
    }
}

void DaikinDriver::handle_rzc3_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1) {
        logErrorP("RzC3 response empty");
        return;
    }
    if (data_size == 1) { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RzC3: Single-byte ACK received (0x%02X) - no system state data", response);
        return; 
    }
    // RzC3 response contains system state bitfield
    if (data_size >= 2) {
        stats_.frames_ok++;
        
        state_.systemState = daikin::DaikinSystemState(data[0]); // Parse system state bitfield using DaikinSystemState class
        
        logInfoP("RzC3: System state bitfield 0x%02X - locked=%d, active=%d, defrost=%d, multizone_conflict=%d", 
                 data[0],
                 state_.systemState.locked(), 
                 state_.systemState.active(), 
                 state_.systemState.defrost(), 
                 state_.systemState.multizone_conflict());
        
        // Additional bytes may contain more system information
        if (data_size >= 3) {
            logDebugP("RzC3: Additional system data bytes available (%zu total)", data_size);
        }
    }
}

// === Serial Communication Callbacks ===

void DaikinDriver::handle_serial_result(daikin::DaikinSerial::Result result, uint8_t* data, size_t data_size)
{
    uint32_t now = millis();
    
    bool accept_response = false; // Accept responses if we're actively waiting OR within the late response window
    
    if (query_state_ == QueryState::WaitingForAck || query_state_ == QueryState::WaitingForGrace) {
        accept_response = true;
    } else if (query_state_ == QueryState::Idle && last_send_time_ != 0 && 
               (now - last_send_time_) < LATE_RX_WINDOW_MS) {
        accept_response = true; // Accept late responses in Idle state if within window
        logDebugP("Accepting late response %lu ms after send", now - last_send_time_);
    } else if (query_state_ == QueryState::WaitingToSend && last_send_time_ != 0 && 
               (now - last_send_time_) < 3000) { // 3 second window for very late responses
        accept_response = true;  // Accept very late responses even in WaitingToSend state
        logDebugP("Accepting very late response %lu ms after send", now - last_send_time_);
    } else if (data_size == 1 && data && data[0] == 0x06) { // 0x06 is proper ACK
        accept_response = true; // Always accept standalone ACKs regardless of timing (they prove communication works)
        logDebugP("Accepting standalone ACK 0x%02X regardless of timing", data[0]);
    }
    if (!accept_response) {
        logDebugP("Received result but outside acceptance window (state: %d, time since send: %lu ms)", 
                  static_cast<int>(query_state_), 
                  last_send_time_ != 0 ? now - last_send_time_ : 0);
        return;
    }
    if (query_index_ >= queries_.size()) {
        logDebugP("Received result but no active query");
        return;
    }

    auto& query = queries_[query_index_];
    
    switch (result) {
            case daikin::DaikinSerial::Result::Ack:
                DAIKIN_DEBUG_PRINT("S21: ACK for %.*s", static_cast<int>(query.command.size()), query.command.data());
                stats_.acks++;
                query.acked = true;
                query.naks = 0;
                // Wait for framed reply (if any) -> stay in WaitingForGrace
                query_state_ = QueryState::WaitingForGrace;
                state_start_time_ = millis();
                break;
            case daikin::DaikinSerial::Result::Frame:
                if (data_size >= 1) {
                    // S21 protocol command/response mapping:
                    // F* → G* (same tail): F8 → G8, FY00 → GY00, etc.
                    // R* → R* or r* (read queries can be echoed in upper or lowercase)
                    // Special case: F6 may reply as G6 (new) or G3 (old)
                    
                    bool is_valid_response = false;
                    size_t expected_cmd_len = query.command.size();
                    uint8_t* payload = nullptr;
                    size_t payload_len = 0;
                    
                    if (data_size >= expected_cmd_len) {
                        std::string response_header(data, data + expected_cmd_len);
                        std::string expected_cmd(query.command);
                        
                        if (expected_cmd[0] == 'F') {
                            // F* commands expect G* responses (same tail)
                            std::string expected_response = "G" + expected_cmd.substr(1);
                            if (response_header == expected_response) {
                                is_valid_response = true;
                            }
                            // Special case: F6 may reply as G6 (new) or G3 (old)
                            else if (expected_cmd == "F6" && response_header == "G3") {
                                is_valid_response = true;
                                DAIKIN_DEBUG_PRINT("S21: F6 got G3 response (old AC unit behavior)");
                            }
                        } else if (expected_cmd[0] == 'R') {
                            // R* queries expect S* responses (R->S mapping per S21 protocol)
                            std::string expected_response = "S" + expected_cmd.substr(1);
                            if (response_header == expected_response) {
                                is_valid_response = true;
                            }
                        } else {
                            // Other commands expect exact echo
                            if (response_header == expected_cmd) {
                                is_valid_response = true;
                            }
                        }
                        
                        if (is_valid_response) {
                            payload = data + expected_cmd_len;
                            payload_len = data_size - expected_cmd_len;
                        }
                    }
                    
                    if (is_valid_response) {
                        if (payload_len > 0) {
                            query.response_data.assign(payload, payload + payload_len);
                            if (query.handler) query.handler(payload, payload_len);
                        }
                        stats_.frames_ok++;
                        DAIKIN_DEBUG_PRINT("S21: Valid response for %.*s (%zu bytes payload)", 
                                  static_cast<int>(query.command.size()), query.command.data(), payload_len);
                    } else {
                        logErrorP("S21: Frame command mismatch for %.*s (rx header: %.*s)", 
                                  static_cast<int>(query.command.size()), query.command.data(),
                                  static_cast<int>(std::min(data_size, expected_cmd_len)), data);
                        stats_.frames_bad_format++;
                    }
                }
                // Advance after frame
                query_index_++;
                query_state_ = QueryState::WaitingToSend;
                state_start_time_ = millis();
                break;
            case daikin::DaikinSerial::Result::Nak:
                DAIKIN_DEBUG_PRINT("S21: NAK for %.*s", static_cast<int>(query.command.size()), query.command.data());
                query.naks++;
                stats_.frames_nak++;
                
                // Special handling for FY00 NAK during protocol detection (per S21 wiki)
                if (query.command == StateQuery::NewProtocol && protocol_detection_phase_) {
                    if (protocol_version_ == daikin::ProtocolUndetected) {
                        // FY00 NAK with unknown protocol = v2.0 (per wiki: "v2 protocol units respond with NAK")
                        protocol_version_ = {2, 0};
                        old_protocol_detected_ = false;
                        DAIKIN_DEBUG_PRINT("S21: FY00 NAK during detection - protocol v2.0 detected");
                    } else {
                        DAIKIN_DEBUG_PRINT("S21: FY00 NAK with known protocol v%d.%d - expected behavior", 
                                  protocol_version_.major, protocol_version_.minor);
                    }
                }
                
                // Smart NAK tracking with overflow protection
                query.naks++;
                if (!query.naks) {  // Overflow protection (255 -> 0 wraparound)
                    query.bad = true;
                    DAIKIN_DEBUG_PRINT("S21: Query %.*s marked as permanently bad (NAK overflow)", 
                             static_cast<int>(query.command.size()), query.command.data());
                } else if (query.naks >= 3) {
                    query.bad = true;
                    DAIKIN_DEBUG_PRINT("S21: Query %.*s marked as bad after %d NAKs", 
                             static_cast<int>(query.command.size()), query.command.data(), query.naks);
                    failed_queries_.push_back(query.command);
                    
                    // Check if F6 failed and switch to F3 fallback
                    if (query.command == StateQuery::SpecialModes) {
                        checkF6ToF3Fallback();
                    }
                }
                
                query_index_++;
                query_state_ = QueryState::WaitingToSend;
                state_start_time_ = millis();
                break;
            case daikin::DaikinSerial::Result::Timeout:
                if (protocol_detection_phase_) {
                    auto [rx_inv, tx_inv] = serial_->get_current_polarity();
                    DAIKIN_DEBUG_PRINT("S21: Protocol detection timeout for %.*s (TX=%s, RX=%s) - trying next query", 
                             static_cast<int>(query.command.size()), query.command.data(),
                             tx_inv ? "inverted" : "normal", rx_inv ? "inverted" : "normal");
                } else {
                    DAIKIN_DEBUG_PRINT("S21: Timeout waiting for %.*s", static_cast<int>(query.command.size()), query.command.data());
                }
                
                // Timeouts also count as NAKs for bad query detection
                query.naks++;
                if (!query.naks) {  // Overflow protection (255 -> 0 wraparound)
                    query.bad = true;
                    DAIKIN_DEBUG_PRINT("S21: Query %.*s marked as permanently bad (timeout overflow)", 
                             static_cast<int>(query.command.size()), query.command.data());
                } else if (query.naks >= 3) {
                    query.bad = true;
                    DAIKIN_DEBUG_PRINT("S21: Query %.*s marked as bad after %d timeouts", 
                             static_cast<int>(query.command.size()), query.command.data(), query.naks);
                    failed_queries_.push_back(query.command);
                    
                    // Check if F6 failed and switch to F3 fallback
                    if (query.command == StateQuery::SpecialModes) {
                        checkF6ToF3Fallback();
                    }
                }
                
                query_index_++;
                query_state_ = QueryState::WaitingToSend;
                state_start_time_ = millis();
                break;
            case daikin::DaikinSerial::Result::Error:
                logErrorP("S21: Error on %.*s (checksum mismatch or comm error)", 
                          static_cast<int>(query.command.size()), query.command.data());
                stats_.frames_bad_checksum++;
                query_index_++;
                query_state_ = QueryState::WaitingToSend;
                state_start_time_ = millis();
                break;
        }
}

void DaikinDriver::handle_serial_idle()
{
    // Serial communication is idle - could be used for additional processing
    // Currently handled by periodic timer in loop()
}

// === State Management ===

void DaikinDriver::publishState()
{
    // Update the OpenKNX status feedback with current S21 state
    logDebugP("Publishing S21 state to OpenKNX");
    
    // Cache previous state to avoid redundant callbacks
    static bool last_power = false;
    static AirConditionMode last_mode = AirConditionMode::AirConditionModeAuto;
    static float last_target_temp = 0.0f;
    static float last_current_temp = 0.0f;
    static float last_outdoor_temp = 0.0f;
    static unsigned int last_fan_speed = 0;
    static bool last_swing_h = false;
    static bool last_swing_v = false;
    static AirConditionDeviceMode last_device_mode = AirConditionDeviceMode::AirConditionDeviceModeStandard;
    static bool last_air_purification = false;
    static bool last_wifi_led = false;
    static int last_humidity = 0;
    static uint8_t last_humidity_mode = 0;
    static uint32_t last_total_energy = 0;
    static bool last_online = false;
    
    // Only update changed values
    if (state_.power != last_power) {
        statusFeedback.updatePower(state_.power);
        last_power = state_.power;
    }
    
    AirConditionMode current_mode = daikin_to_openknx_mode(state_.mode);
    if (current_mode != last_mode) {
        statusFeedback.updateMode(current_mode);
        last_mode = current_mode;
    }
    
    if (abs(state_.targetC - last_target_temp) > 0.1f) {
        statusFeedback.updateTargetTemperature(state_.targetC);
        last_target_temp = state_.targetC;
    }
    
    if (abs(state_.homeC - last_current_temp) > 0.1f) {
        statusFeedback.updateCurrentTemperature(state_.homeC);
        last_current_temp = state_.homeC;
    }
    
    if (abs(state_.outsideC - last_outdoor_temp) > 0.1f) {
        statusFeedback.updateOutdoorTemperature(state_.outsideC);
        last_outdoor_temp = state_.outsideC;
    }
    
    unsigned int current_fan_speed = daikin_to_openknx_fan(state_.fan);
    if (current_fan_speed != last_fan_speed) {
        statusFeedback.updateFanSpeed(current_fan_speed);
        last_fan_speed = current_fan_speed;
    }
    
    bool current_swing_h = (state_.swing == daikin::Swing::Horizontal || state_.swing == daikin::Swing::Both);
    if (current_swing_h != last_swing_h) {
        statusFeedback.updateSwingHorizontal(current_swing_h);
        last_swing_h = current_swing_h;
    }
    
    bool current_swing_v = (state_.swing == daikin::Swing::Vertical || state_.swing == daikin::Swing::Both);
    if (current_swing_v != last_swing_v) {
        statusFeedback.updateSwingVertical(current_swing_v);
        last_swing_v = current_swing_v;
    }
    
    // Device modes
    AirConditionDeviceMode current_device_mode;
    if (state_.powerful) {
        current_device_mode = AirConditionDeviceMode::AirConditionDeviceModeHiPower;
    } else if (state_.quiet) {
        current_device_mode = AirConditionDeviceMode::AirConditionDeviceModeSilent1;
    } else if (state_.econo) {
        current_device_mode = AirConditionDeviceMode::AirConditionDeviceModeEco;
    } else {
        current_device_mode = AirConditionDeviceMode::AirConditionDeviceModeStandard;
    }
    
    if (current_device_mode != last_device_mode) {
        statusFeedback.updateDeviceMode(current_device_mode);
        last_device_mode = current_device_mode;
    }
    
    // Extended features - only update if changed
    if (state_.streamer != last_air_purification) {
        statusFeedback.updateAirPurification(state_.streamer);
        last_air_purification = state_.streamer;
    }
    
    if (state_.led != last_wifi_led) {
        statusFeedback.updateWifiLed(state_.led);
        last_wifi_led = state_.led;
    }
    
    if (state_.humidity != last_humidity) {
        statusFeedback.updateHumidity(state_.humidity);
        last_humidity = state_.humidity;
    }

    uint8_t current_humidity_mode = static_cast<uint8_t>(state_.humidityMode);
    if (current_humidity_mode != last_humidity_mode) {
        statusFeedback.updateHumidityMode(current_humidity_mode);
        last_humidity_mode = current_humidity_mode;
    }

    if (state_.totalEnergyWh != last_total_energy) {
        statusFeedback.updateTotalEnergyConsumption(state_.totalEnergyWh);
        last_total_energy = state_.totalEnergyWh;
    }

    if (state_.online != last_online) {
        statusFeedback.updateOnlineStatus(state_.online);
        last_online = state_.online;
    }
}

void DaikinDriver::updateOnlineStatus()
{
    // Consider online if we have recent successful communication
    bool was_online = state_.online;
    state_.online = (stats_.acks > 0 || stats_.frames_ok > 0);
    
    if (was_online != state_.online) {
        logInfoP("S21 online status changed: %s", state_.online ? "Online" : "Offline");
        
        if (state_.online) {
            statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateOk);
        } else {
            statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError, 
                                             "Communication lost");
        }
    }
}

// === Conversion Utilities ===

uint8_t DaikinDriver::mode_to_daikin(daikin::Mode mode) const
{
    switch (mode) {
        case daikin::Mode::Auto:     return 0x00;
        case daikin::Mode::Dry:      return 0x02;
        case daikin::Mode::Cool:     return 0x03;
        case daikin::Mode::Heat:     return 0x04;
        case daikin::Mode::FanOnly:  return 0x06;
        default:                     return 0x00;
    }
}

daikin::Mode DaikinDriver::daikin_to_mode(uint8_t mode) const
{
    switch (mode) {
        case 0x00: return daikin::Mode::Auto;
        case 0x02: return daikin::Mode::Dry;
        case 0x03: return daikin::Mode::Cool;
        case 0x04: return daikin::Mode::Heat;
        case 0x06: return daikin::Mode::FanOnly;
        default:   return daikin::Mode::Auto;
    }
}

daikin::Action DaikinDriver::daikin_to_action(uint8_t action) const
{
    switch (action) {
        case 0x00: return daikin::Action::Off;
        case 0x01: return daikin::Action::Idle;
        case 0x02: return daikin::Action::Cooling;
        case 0x03: return daikin::Action::Heating;
        case 0x04: return daikin::Action::Drying;
        case 0x05: return daikin::Action::Fan;
        default:   return daikin::Action::Off;
    }
}

daikin::Swing DaikinDriver::daikin_to_swing_mode(uint8_t mode) const
{
    switch (mode) {
        case 0x00: return daikin::Swing::Off;
        case 0x01: return daikin::Swing::Vertical;
        default:   return daikin::Swing::Off;
    }
}

uint8_t DaikinDriver::swing_mode_to_daikin(daikin::Swing mode) const
{
    switch (mode) {
        case daikin::Swing::Off:        return 0x00;
        case daikin::Swing::Vertical:   return 0x01;
        default:                        return 0x00;
    }
}

uint8_t DaikinDriver::fan_mode_to_daikin(daikin::DaikinFanMode fan) const
{
    switch (fan) {
        case daikin::DaikinFanMode::Auto:   return 0x00;
        case daikin::DaikinFanMode::Speed1: return 0x03;
        case daikin::DaikinFanMode::Speed2: return 0x04;
        case daikin::DaikinFanMode::Speed3: return 0x05;
        case daikin::DaikinFanMode::Speed4: return 0x06;
        case daikin::DaikinFanMode::Speed5: return 0x07;
        default:                            return 0x00;
    }
}

daikin::DaikinFanMode DaikinDriver::daikin_to_fan_mode(uint8_t fan) const
{
    switch (fan) {
        case 0x00: return daikin::DaikinFanMode::Auto;
        case 0x03: return daikin::DaikinFanMode::Speed1;
        case 0x04: return daikin::DaikinFanMode::Speed2;
        case 0x05: return daikin::DaikinFanMode::Speed3;
        case 0x06: return daikin::DaikinFanMode::Speed4;
        case 0x07: return daikin::DaikinFanMode::Speed5;
        default:   return daikin::DaikinFanMode::Auto;
    }
}

// === OpenKNX to Daikin Conversions ===

daikin::Mode DaikinDriver::openknx_to_daikin_mode(AirConditionMode mode) const
{
    switch (mode) {
        case AirConditionMode::AirConditionModeAuto:
            return daikin::Mode::Auto;
        case AirConditionMode::AirConditionModeCool:
            return daikin::Mode::Cool;
        case AirConditionMode::AirConditionModeHeat:
            return daikin::Mode::Heat;
        case AirConditionMode::AirConditionModeDry:
            return daikin::Mode::Dry;
        case AirConditionMode::AirConditionModeFan:
            return daikin::Mode::FanOnly;
        default:
            return daikin::Mode::Auto;
    }
}

AirConditionMode DaikinDriver::daikin_to_openknx_mode(daikin::Mode mode) const
{
    switch (mode) {
        case daikin::Mode::Auto:
            return AirConditionMode::AirConditionModeAuto;
        case daikin::Mode::Cool:
            return AirConditionMode::AirConditionModeCool;
        case daikin::Mode::Heat:
            return AirConditionMode::AirConditionModeHeat;
        case daikin::Mode::Dry:
            return AirConditionMode::AirConditionModeDry;
        case daikin::Mode::FanOnly:
            return AirConditionMode::AirConditionModeFan;
        default:
            return AirConditionMode::AirConditionModeAuto;
    }
}

daikin::DaikinFanMode DaikinDriver::openknx_to_daikin_fan(unsigned int speed) const
{
    switch (speed) {
        case 0:  return daikin::DaikinFanMode::Auto;
        case 1:  return daikin::DaikinFanMode::Speed1;
        case 2:  return daikin::DaikinFanMode::Speed2;
        case 3:  return daikin::DaikinFanMode::Speed3;
        case 4:  return daikin::DaikinFanMode::Speed4;
        case 5:  return daikin::DaikinFanMode::Speed5;
        default: return daikin::DaikinFanMode::Auto;
    }
}

unsigned int DaikinDriver::daikin_to_openknx_fan(daikin::DaikinFanMode fan) const
{
    switch (fan) {
        case daikin::DaikinFanMode::Auto:   return 0;
        case daikin::DaikinFanMode::Speed1: return 1;
        case daikin::DaikinFanMode::Speed2: return 2;
        case daikin::DaikinFanMode::Speed3: return 3;
        case daikin::DaikinFanMode::Speed4: return 4;
        case daikin::DaikinFanMode::Speed5: return 5;
        default:                            return 0;
    }
}

// === Smart NAK Tracking ===
void DaikinDriver::resetQueryNakTracking() {
    // Reset NAK counters and bad flags for all queries (like Faikin does when daikin.talking is false)
    for (auto& query : queries_) {
        query.acked = false;
        query.naks = 0;
        query.bad = false;
    }
    failed_queries_.clear();
    logDebugP("S21: Reset NAK tracking for all queries (communication restart)");
}

// === F6 to F3 Fallback Logic ===
void DaikinDriver::checkF6ToF3Fallback() {
    // Look for F6 query that's marked as bad and replace it with F3
    for (auto it = queries_.begin(); it != queries_.end(); ++it) {
        if (it->command == StateQuery::SpecialModes && it->bad) {
            // F6 is bad, replace with F3 fallback
            logInfoP("S21: F6 marked as bad, switching to F3 fallback for special modes");
            
            // Erase the bad F6 query
            it = queries_.erase(it);
            
            // Insert F3 at the same position
            queries_.insert(it, DaikinQueryState(StateQuery::OnOffTimer, 
                [this](uint8_t* data, size_t data_size) { handle_f3_response(data, data_size); }));
            
            support_.f6_special_modes = false;  // Mark F6 as unsupported
            break;
        }
    }
}