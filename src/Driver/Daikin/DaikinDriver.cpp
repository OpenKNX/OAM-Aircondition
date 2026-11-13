#include "DaikinDriver.h"
#include "macros.h"
#include <Arduino.h>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <numeric>
#include <cstdlib>
#include "s21_protocol.h"

// Implements the Faikout S21 protocol (ba4eb32) (https://github.com/revk/ESP32-Faikout/wiki/S21-Protocol)

// S21 decoding functions
namespace
{
    int s21_hex_nibble(uint8_t c)
    {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    }

    // Decodes a 1-byte value transported as 2 ASCII hex chars in S21 reverse order.
    // Example: payload "40" => value 0x04.
    bool s21_decode_hex_byte_reversed(const uint8_t* payload, uint8_t& out)
    {
        int lo = s21_hex_nibble(payload[0]);
        int hi = s21_hex_nibble(payload[1]);
        if (lo < 0 || hi < 0) return false;
        out = static_cast<uint8_t>((hi << 4) | lo);
        return true;
    }

    bool s21_valid_decimal(const uint8_t* data, size_t size, bool sign)
    {
        if (size != (sign ? 4U : 3U)) return false;
        for (size_t i=0;i<3;i++) if (data[i] < '0' || data[i] > '9') return false;
        return !sign || data[3] == '+' || data[3] == '-';
    }

    bool s21_valid_response(std::string_view cmd,const uint8_t* data,size_t size) { return daikin::s21::validResponse(cmd,data,size); }

    // s21_decode_target_temp function
    float s21_decode_target_temp(uint8_t v)
    {
        return 18.0f + 0.5f * (static_cast<int8_t>(v) - '@');
    }
    // s21_decode_int_sensor function
    int s21_decode_int_sensor(const uint8_t* payload)
    {
        int v = (payload[0] - '0') + (payload[1] - '0') * 10 + (payload[2] - '0') * 100;
        if (payload[3] == '-')
        {
            v = -v;
        }
        return v;
    }
    // s21_decode_float_sensor function
    float s21_decode_float_sensor(const uint8_t* payload)
    {
        return static_cast<float>(s21_decode_int_sensor(payload)) * 0.1f;
    }
    // s21_decode_hex_sensor function
    uint16_t s21_decode_hex_sensor(const uint8_t* payload)
    {
        auto hex = [](uint8_t c) { return ((c & 0xF) + ((c > '9') ? 9 : 0)); };
        return (hex(payload[3]) << 12) | (hex(payload[2]) << 8) | (hex(payload[1]) << 4) | hex(payload[0]);
    }
} // namespace

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
    pending_.climate.swing_v = daikin::Swing::Off; // Initialize swing directions
    pending_.climate.swing_h = daikin::Swing::Off;

    logInfoP("DaikinDriver created with S21 protocol support");
}

// === AirConditionDriver Interface Implementation ===
const std::string DaikinDriver::name() const
{
    return "Daikin S21";
}

void DaikinDriver::showInformations()
{
    if (!serial_)
    {
        logInfoP("Daikin S21 Driver: Not initialized");
        return;
    }

    logInfoP("=== Daikin S21 Driver Status ===");
    logInfoP("Online: %s", state_.online ? "Yes" : "No");
    logInfoP("Power: %s", state_.power ? "On" : "Off");

    // Mode
    const char* modeStr = "Unknown";
    switch (state_.mode)
    {
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
    switch (state_.fan)
    {
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
    logInfoP("Swing: %s", state_.swing == daikin::Swing::Vertical ? "Vertical" : state_.swing == daikin::Swing::Horizontal ? "Horizontal"
                                                                             : state_.swing == daikin::Swing::Both         ? "Both"
                                                                                                                           : "Off");

    // Humidity Mode
    const char* humidityModeStr = "Unknown";
    switch (state_.humidityMode)
    {
        case daikin::HumidityMode::Off: humidityModeStr = "Off"; break;
        case daikin::HumidityMode::Low: humidityModeStr = "Low"; break;
        case daikin::HumidityMode::Standard: humidityModeStr = "Standard"; break;
        case daikin::HumidityMode::High: humidityModeStr = "High"; break;
        case daikin::HumidityMode::Continuous: humidityModeStr = "Continuous"; break;
        default: humidityModeStr = "Unknown"; break;
    }
    if (!state_.humidityModeValid) humidityModeStr = "Unknown";
    else if (support_.f2_known && getMaximumHumidityModeLevels() == 0) humidityModeStr = "Unavailable";
    logInfoP("Humidity Mode: %s", humidityModeStr);

    showModelProfiles();

    // Advanced features
    logInfoP("--- Advanced Features ---");
    logInfoP("Powerful: %s", state_.powerful ? "On" : "Off");
    logInfoP("Econo: %s; FU00 advertised=%s, observed-On=%s, write-verified=%s",
        !state_.demandValid?"Unknown":state_.econo?"On":"Off",
        !support_.special_features_known?"Unknown":support_.econo?"Yes":"No",
        econo_observed_on_?"Yes":"No",fieldWriteVerified("F7",1,0xFF)?"Yes":"No");
    logInfoP("Quiet: %s (one S21 level; legacy Quiet 2 aliases Quiet 1)", state_.specialModesValid ? (state_.quiet ? "On" : "Off") : "Unknown");
    logInfoP("Comfort: %s", state_.comfort ? "On" : "Off");
    logInfoP("Sensor: %s", state_.sensor ? "On" : "Off"); // Intelligent Eye/Presence sensor
    const char* ledStr = state_.ledState == daikin::LedState::On ? "On" :
        state_.ledState == daikin::LedState::Dark ? "Dark" :
        state_.ledState == daikin::LedState::Off ? "Off" : "Unknown";
    logInfoP("LED: %s", ledStr);
    logInfoP("Streamer: %s", state_.streamer ? "On" : "Off");

    // Sensor data
    logInfoP("--- Sensors ---");
    logInfoP("Humidity: %d%%", state_.humidity);
    logInfoP("Total Energy: %u Wh", state_.totalEnergyWh);
    if (state_.serialErrorValid) logInfoP("S21 serial error (read-clears): %d, samples=%u", state_.serialError, state_.serialErrorSamples);
    logInfoP("Compressor Frequency: %d Hz", state_.compressorHz);
    if (state_.indoorHeatExchangerValid) logInfoP("RI indoor heat exchanger: %.1f C", state_.indoorHeatExchangerC);

    // Protocol information
    if (!model_name_.empty()) logInfoP("Model: %s",model_name_.c_str());
    if (!firmware_version_.empty()) logInfoP("Indoor firmware: %s",firmware_version_.c_str());
    logInfoP("--- Protocol ---");
    if (protocol_version_ == daikin::ProtocolUndetected)
    {
        logInfoP("Protocol Version: unknown");
    }
    else
    {
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
// Get the UART reference from OpenKNX configuration
#ifdef OPENKNX_AIR_CONDITION_SERIAL
    HardwareSerial& ser = OPENKNX_AIR_CONDITION_SERIAL;
#else
    // Fallback for platforms without explicit serial configuration
    HardwareSerial& ser = Serial1; // Default UART
#endif

// Get pin configuration from build flags or use defaults
#ifdef OPENKNX_AIR_CONDITION_SERIAL_RX
    int raw_rx_pin = OPENKNX_AIR_CONDITION_SERIAL_RX;
#else
    int raw_rx_pin = 16; // Default RX pin
#endif

#ifdef OPENKNX_AIR_CONDITION_SERIAL_TX
    int raw_tx_pin = OPENKNX_AIR_CONDITION_SERIAL_TX;
#else
    int raw_tx_pin = 17; // Default TX pin
#endif

    // Parse pin configuration from variables set above
    // Negative pin numbers indicate inversion: -48 = pin 48 inverted, 48 = pin 48 normal
    int actual_rx_pin = abs(raw_rx_pin);
    int actual_tx_pin = abs(raw_tx_pin);
    bool rx_inverted = (raw_rx_pin < 0);
    bool tx_inverted = (raw_tx_pin < 0);

    logInfoP("S21 Serial Config: RX=Pin%d(%s), TX=Pin%d(%s)",
             actual_rx_pin, rx_inverted ? "inverted" : "normal",
             actual_tx_pin, tx_inverted ? "inverted" : "normal");

    // Create the serial communication object with callbacks to this instance
    serial_ = std::make_unique<daikin::DaikinSerial>(
        ser,
        actual_rx_pin,
        actual_tx_pin,
        rx_inverted,
        tx_inverted,
        [this](daikin::DaikinSerial::Result result, uint8_t* data, size_t data_size) {
            handle_serial_result(result, data, data_size);
        },
        [this]() {
            handle_serial_idle();
        });

    logDebugP("DaikinSerial object created, initializing serial port...");
    serial_->begin();
    if (!serial_->initialized()) { statusFeedback.driverStateChanged(AirConditionDriverStateError,"UART initialization failed"); serial_.reset(); return; }
    statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateOk);
}

void DaikinDriver::startCommunication(bool restart)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError,
                                          "Driver not initialized");
        return;
    }
    statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);

    if (restart)
    {
        // Reset state for restart
        state_ = {};
        model_name_.clear(); firmware_version_.clear(); identification_={};
        diagnostic_active_=false;diagnostic_queue_.clear();variant_pending_=false;
        publication_initialized_=false;
        serial_->force_framed_mode();
        online_=false; last_rx_ok_ms_=0;
        query_state_=QueryState::Idle; last_write_time_=0;
        stats_ = {};
        query_index_ = 0;
        failed_queries_.clear();
        protocol_version_ = daikin::ProtocolUndetected;
        support_ = {};
        econo_observed_on_=econo_probe_pending_=false;
        d5_ = {}; d6_ = {}; d7_ = {}; dr_ = {}; d3_ = {}; fb_ = {};
        timer_disabled_bytes_ = {}; last_timer_minutes_={}; timer_dirty_=0; timer_lamp_pending_=false;
        requested_vertical_position_ = -1;
        vertical_pending_ = false;
        outstanding_write_ = {};
        humidity_modes_ = 0;
        humidity_pending_ = demand_pending_ = false;
        // Reinitialize default state
        state_.homeC = 20.0f;
        state_.targetC = 22.0f;
        state_.fan = daikin::DaikinFanMode::Auto;
        state_.mode = daikin::Mode::Auto;
        state_.swing = daikin::Swing::Off;
    }

    // Reset pending activation flags to prevent stale commands after restart
    pending_.activate_climate = false;
    pending_.activate_swing_mode = false;
    pending_.activate_powerful = false;
    pending_.activate_econo = false;
    pending_.activate_quiet = false;
    pending_.activate_led = false;
    pending_.activate_streamer = false;
    pending_.activate_sensor = false;
    pending_.activate_comfort = false;
    humidity_pending_ = demand_pending_ = timer_pending_ = vertical_pending_ = false;
    timer_dirty_=0;timer_lamp_pending_=false;timer_request_error_=lamp_request_error_=AirConditionWriteResult::None;
    pending_explicit_off_ = false;
    pending_explicit_fan_ = false;
    climate_dirty_ = 0;

    // Do not publish default/offline values on startup.
    // Wait until we have a real sample from the indoor unit, then seed all KOs once.
    sample_seen_mask_ = 0;
    online_since_ms_ = 0;
    gate_publish_until_full_sample_ = true;
    seed_kos_pending_ = true;
    startup_offline_published_ = false;
    comm_start_time_ = millis();
    if (comm_start_time_ == 0) comm_start_time_ = 1; // avoid 0 sentinel

    query_state_=QueryState::Idle;
    confirmation_resume_=SIZE_MAX;
    // Start communication with smart protocol detection
    last_query_cycle_ = 0; // Force immediate cycle start

    // Only detect protocol if unknown, otherwise use known protocol
    if (protocol_version_ == daikin::ProtocolUndetected)
    {
        initializeProtocolDetection();
    }
    else
    {
        initializeQueries(); // Use already-detected protocol
    }
    triggerQueryCycle();
}

void DaikinDriver::requestAllData()
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }
    triggerQueryCycle();
}

void DaikinDriver::loop()
{
    if (!serial_)
    {
        return;
    }


    uint32_t now = millis();
    // Detect high load conditions (loop taking too long)
    if (last_loop_time_ != 0 && (now - last_loop_time_) > 20)
    {
        if (!high_load_detected_)
        {
            high_load_detected_ = true;
        }
    }
    else if (high_load_detected_ && (now - last_loop_time_) <= 10)
    {
        high_load_detected_ = false;
    }
    last_loop_time_ = now;

    serial_->loop(); // Process serial communication (non-blocking)
    if (!serial_->busy() && !diagnostic_active_ && !isInPostWriteSettle()) {
        if (pending_.activate_climate) sendClimateCommand();
        if (pending_.activate_swing_mode || humidity_pending_) sendSwingCommand();
        if (pending_.activate_powerful || pending_.activate_quiet || pending_.activate_comfort ||
            pending_.activate_sensor || pending_.activate_led || pending_.activate_streamer)
            sendSpecialModesCommand();
        if (pending_.activate_econo || demand_pending_) sendEconoCommand();
        if (vertical_pending_) sendVerticalPositionCommand();
        if (timer_pending_) sendTimerCommand();
        if (timer_lamp_pending_) sendTimerLampCommand();
    }

    if (!serial_->busy() && (query_state_==QueryState::Idle || query_state_==QueryState::WaitingToSend)) serviceDiagnostic();
    if (!diagnostic_active_) updateQueryStateMachine(); // non-blocking state machine
    if (sent_climate_dirty_ && millis()-climate_confirmation_started_>30000) {
        climate_outcome_=climate_readback_seen_?WriteOutcome::Mismatch:WriteOutcome::Timeout;
        rejectClimateWrite();
    }
    expireWriteConfirmations();
    if(version_changed_ && !serial_->busy() && !diagnostic_active_) {version_changed_=false;startCommunication(true);}

    // Check online timeout (independent of query state machine)
    checkOnlineTimeout();

    // Protocol detection retry mechanism - configurable interval if protocol unknown
    if (protocol_version_ == daikin::ProtocolUndetected && !protocol_detection_phase_ && query_state_ == QueryState::Idle)
    {
        if (last_protocol_detection_attempt_ == 0 ||
            (now - last_protocol_detection_attempt_) >= PROTOCOL_DETECTION_RETRY_MS)
        {

            last_protocol_detection_attempt_ = now;
            logInfoP("Protocol unknown - retrying protocol detection (interval %lu ms)",
                     (unsigned long)PROTOCOL_DETECTION_RETRY_MS);

            // Reset state and restart protocol detection
            initializeProtocolDetection();
            resetQueryNakTracking(); // reset bad query tracking on comm restart
            query_index_ = 0;
            query_state_ = QueryState::WaitingToSend;
            state_start_time_ = now;
        }
    }
    updateOnlineStatus(); // Update online status based on recent communication
}

std::vector<uint8_t> DaikinDriver::diagnosticResponse(std::string_view command) const
{
    // Local diagnostics only. Production pages can contain serial numbers.
    auto q=std::find_if(queries_.begin(),queries_.end(),[&](const DaikinQueryState& q){return q.command==command;});
    return q == queries_.end() ? std::vector<uint8_t>{} : q->response_data;
}

DaikinDriver::CombinedCommand* DaikinDriver::combinedFor(std::string_view c)
{
    if(c=="D1" || c=="F1") return nullptr;
    if(c=="D3" || c=="F3") return &d3_;
    if(c=="D5" || c=="F5") return &d5_;
    if(c=="D6" || c=="F6") return &d6_;
    if(c=="D7" || c=="F7") return &d7_;
    if(c=="DR" || c=="FR") return &dr_;
    if(c=="FB") return &fb_;
    return nullptr;
}
const DaikinDriver::CombinedCommand* DaikinDriver::combinedFor(std::string_view c) const { return const_cast<DaikinDriver*>(this)->combinedFor(c); }
DaikinDriver::FeatureAccess DaikinDriver::commandAccess(std::string_view c) const
{
    const auto* combined=combinedFor(c);
    const auto query=std::find_if(queries_.begin(),queries_.end(),[&](const DaikinQueryState& q){return q.command==c;});
    if(query!=queries_.end() && query->bad) return FeatureAccess::Unsupported;
    if(combined && combined->write==CommandSupport::Supported) return FeatureAccess::Writable;
    if(combined && combined->write==CommandSupport::Unsupported) return FeatureAccess::ReadOnly;
    if((combined && combined->read_supported) || (query!=queries_.end() && query->acked)) return FeatureAccess::ReadSupported;
    return FeatureAccess::Unknown;
}
DaikinDriver::WriteOutcome DaikinDriver::writeOutcome(std::string_view c) const { if(c=="F1" || c=="D1") return climate_outcome_;const auto* p=combinedFor(c);return p?p->outcome:WriteOutcome::None; }
bool DaikinDriver::fieldWriteVerified(std::string_view c,size_t byte,uint8_t mask) const {
    const auto* p=combinedFor(c);return p && byte<4 && (p->verified_mask[byte]&mask)==mask;
}
void DaikinDriver::retryRejectedWrites() {
    for(auto* p:{&d3_,&d5_,&d6_,&d7_,&dr_,&fb_}) {p->rejected_valid=false;p->last_rejection_ms=0;}
}
void DaikinDriver::confirmCombined(std::string_view c,const uint8_t* data)
{
    auto* p=combinedFor(c);if(!p) return;p->read_supported=true;
    if(!p->awaiting_readback) return;
    bool matches=true;for(size_t i=0;i<4;i++) if((data[i]&p->mask[i])!=(p->requested[i]&p->mask[i])) matches=false;
    // Faikout deliberately excludes G7 demand byte '1' (99%): some units
    // return it spuriously. It must not verify a requested demand write.
    if (c=="D7" && p->mask[0] && data[0]=='1') matches=false;
    p->readback_seen=true;
    // All shared pages can lag a write. Keep observing, without resending,
    // until the requested fields match or the bounded deadline expires.
    if (!matches) {p->outcome=WriteOutcome::AwaitingReadback;return;}
    p->awaiting_readback=false;
    p->outcome=matches?WriteOutcome::Confirmed:WriteOutcome::Mismatch;
    if(matches) {
        p->write=CommandSupport::Supported;
        for(size_t i=0;i<4;i++) p->verified_mask[i]|=p->mask[i];
        if(c=="DR" && data[0]>='0' && data[0]<='5') verified_vertical_positions_|=1U<<(data[0]-'0');
    }
}

void DaikinDriver::expireWriteConfirmations()
{
    for(auto* page:{&d3_,&d5_,&d6_,&d7_,&dr_,&fb_}) {
        if(!page->awaiting_readback || millis()-page->sent_ms<=30000) continue;
        page->awaiting_readback=false;
        page->outcome=page->readback_seen?WriteOutcome::Mismatch:WriteOutcome::Timeout;
        if(page==&d3_ || page==&fb_) publishTimerStatus();
    }
}

bool DaikinDriver::configureTiming(const TimingProfile& p)
{
    if(p.cycle_ms<100 || p.inter_query_ms<5 || p.high_load_ms<5 || p.ack_ms<50 || p.rx_ms<50 || p.ack_ms>30000 || p.rx_ms>30000 || p.offline_ms<2000 || p.offline_ms>3600000 || p.reprobe_ms<1000 || p.reprobe_ms>86400000 || p.post_write_f1_ms<50 || p.post_write_f1_ms>30000 ||
       p.cycle_ms>3600000 || p.inter_query_ms>30000 || p.high_load_ms>30000 || p.d1_settle_ms>30000 || p.d5_settle_ms>30000 || p.other_settle_ms>30000 || p.v0_d1_settle_ms>30000 || p.v0_d5_settle_ms>30000 || p.v0_other_settle_ms>30000) return false;
    if(serial_ && serial_->busy()) return false;
    timing_=p;return !serial_ || serial_->configureTimeouts(p.ack_ms,p.rx_ms);
}

bool DaikinDriver::processCommand(const std::string command,bool debugKo)
{
    if(command.size()>160 || command.substr(0,4)!="s21 ") return false;
    std::vector<std::string_view> words;
    std::string_view rest(command.data()+4,command.size()-4);
    while(!rest.empty() && words.size()<40) {
        const auto start=rest.find_first_not_of(" \t");if(start==std::string_view::npos) break;rest.remove_prefix(start);
        const auto end=rest.find_first_of(" \t");words.push_back(rest.substr(0,end));if(end==std::string_view::npos) {rest={};break;}rest.remove_prefix(end);
    }
    if(words.empty()) return false;
    if(words[0]=="errors" && words.size()==1 && !debugKo) {showRejectedResponses();return true;}
    if(words[0]=="writes" && words.size()==1 && !debugKo) {showWriteDiagnostics();return true;}
    if(words[0]=="profiles" && words.size()==1) {showModelProfiles();return true;}
    auto integer=[&](size_t i,unsigned& out){if(i>=words.size() || words[i].empty()) return false;out=0;for(char c:words[i]){if(c<'0'||c>'9'||out>10000)return false;out=out*10+c-'0';}return true;};
    auto floating=[&](size_t i,float& out){if(i>=words.size())return false;std::string text(words[i]);char* end=nullptr;out=std::strtof(text.c_str(),&end);return end==text.c_str()+text.size() && std::isfinite(out);};
    auto onoff=[&](bool& value){if(words.size()!=2 || (words[1]!="on" && words[1]!="off"))return false;value=words[1]=="on";return true;};
    if(words[0]=="raw" && words.size()>=2) {
        if(debugKo) return false; // raw probing is a local console/API operation
        std::vector<uint8_t> data;
        for(size_t i=2;i<words.size();i++){if(words[i].size()%2) return false;for(size_t n=0;n<words[i].size();n+=2){int hi=daikin::s21::hex(words[i][n]),lo=daikin::s21::hex(words[i][n+1]);if(hi<0||lo<0)return false;data.push_back((hi<<4)|lo);}}
        return queueDiagnosticCommand(std::string(words[1]),data);
    }
    if(words[0]=="result" && words.size()==1) {
        if(debugKo) return false;
        logInfoP("S21 diagnostic %s status=%u bytes=%zu",diagnostic_result_.command.c_str(),unsigned(diagnostic_result_.status),diagnostic_result_.payload.size());
        logInfoP("S21 diagnostic payload: %s",toHexString(diagnostic_result_.payload).c_str());return true;
    }
    bool value=false;
    if(words[0]=="econo-probe" && onoff(value)) {if(debugKo)return false;return probeEcono(value);}
    if(words[0]=="comfort" && onoff(value)){setComfort(value);return true;}
    if(words[0]=="eye" && onoff(value)){setIntelligentEye(value);return true;}
    if(words[0]=="research" && onoff(value)){if(debugKo)return false;enableResearchPolling(value);return true;}
    if(words[0]=="production" && onoff(value)){if(debugKo)return false;enableProductionDiagnostics(value);return true;}
    if(words[0]=="rz-trust" && onoff(value)){if(debugKo)return false;trustRzStateReadings(value);return true;}
    if(words[0]=="extended" && onoff(value)){enableExtendedPolling(value);return true;}
    if(words[0]=="retry-writes" && words.size()==1){retryRejectedWrites();return true;}
    if(words[0]=="led" && words.size()==2) {
        if(words[1]!="on" && words[1]!="dark" && words[1]!="off")return false;
        setLedState(words[1]=="on"?daikin::LedState::On:words[1]=="dark"?daikin::LedState::Dark:daikin::LedState::Off);return true;
    }
    if(words[0]=="humidity" && words.size()==2) {
        using H=daikin::HumidityMode;
        const H mode=words[1]=="off"?H::Off:words[1]=="low"?H::Low:words[1]=="standard"?H::Standard:words[1]=="high"?H::High:words[1]=="continuous"?H::Continuous:H::Unknown;
        return mode!=H::Unknown && setHumidityMode(mode);
    }
    if(words[0]=="mode" && words.size()==2) {
        using V=daikin::ClimateVariant;
        const std::array<std::string_view,8> names={"auto","cool","dry-cooling","heat","humid-heating","sarara","ururu","fan"};
        for(size_t i=0;i<names.size();i++)if(words[1]==names[i])return setClimateVariant(static_cast<V>(i));return false;
    }
    if(words[0]=="vane" && words.size()==2 && words[1].size()==1)return setVerticalPositionCode(words[1][0]);
    unsigned a=0,b=0;
    if(words[0]=="vane-scale" && words.size()==2 && integer(1,a) && a<=255)return configureVaneAngleScale(a);
    if(words[0]=="demand" && words.size()==2 && integer(1,a) && a<=100){const bool accepted=demandRequestAllowed(a);setMaxPowerLevel(a);return accepted;}
    if(words[0]=="demand-range" && words.size()==3 && integer(1,a) && integer(2,b) && a<=100 && b<=100)return configureDemandLimits(a,b);
    if(words[0]=="timer-range" && words.size()==3 && integer(1,a) && integer(2,b) && a<=1440 && b<=1440)return configureTimerGranularity(a,b);
    if(words[0]=="timers" && words.size()==3 && integer(1,a) && integer(2,b) && a<=1440 && b<=1440)return setTimers(a,b);
    if(words[0]=="timer-lamp" && onoff(value))return setTimerLamp(value);
    float minimum=0,maximum=0;
    if(words[0]=="temp-range" && words.size()==4 && integer(1,a) && a<6 && floating(2,minimum) && floating(3,maximum))return configureTemperatureLimits(static_cast<daikin::Mode>(a),minimum,maximum);
    return false;
}

bool DaikinDriver::queueDiagnosticCommand(std::string command, std::vector<uint8_t> payload)
{
    if (command.empty() || command.size()>6 || payload.size()>32 || diagnostic_queue_.size()>=4) return false;
    for(char c:command) if(!std::isalnum(static_cast<unsigned char>(c))) return false;
    for(auto b:payload) if(daikin::s21::forbidden(b)) return false;
    if (command=="A" && payload.size()!=4) return false;
    if (command=="D2" && payload.size()!=1) return false;
    if ((command=="DH" || command=="DJ" || command=="DL" || command=="D7" || command=="D6" || command=="D5" || command=="D1" || command=="DR") && payload.size()!=4) return false;
    if(command=="DY10" && payload.size()!=8) return false;
    if(command=="DY20" && payload.size()!=4) return false;
    if(command=="D3" && payload.size()!=3 && payload.size()!=4) return false;
    if(command=="FB" && !payload.empty() && payload.size()!=4) return false;
    diagnostic_queue_.push_back({std::move(command),std::move(payload)});
    if(!diagnostic_active_) diagnostic_result_.status=DiagnosticStatus::Queued;
    return true;
}

void DaikinDriver::serviceDiagnostic()
{
    if(diagnostic_active_ || diagnostic_queue_.empty() || !serial_ || serial_->busy() || isInPostWriteSettle()) return;
    auto request=std::move(diagnostic_queue_.front()); diagnostic_queue_.erase(diagnostic_queue_.begin());
    diagnostic_result_={request.command,{},DiagnosticStatus::Running};
    diagnostic_ack_only_=request.command[0]=='D' || (request.command=="FB" && !request.payload.empty());
    diagnostic_active_=true;
    if(!serial_->send_frame(request.command,request.payload.data(),request.payload.size(),diagnostic_ack_only_)) {
        diagnostic_active_=false; diagnostic_result_.status=DiagnosticStatus::Error;
    }
    last_send_time_=millis();
}

bool DaikinDriver::setTimerLamp(bool on)
{
    if (!serial_ || !fb_.read_supported) return rejectTimerRequest(true,AirConditionWriteResult::Unavailable);
    lamp_request_error_=AirConditionWriteResult::None;
    requested_timer_lamp_=on;timer_lamp_pending_=true;
    sendTimerLampCommand();publishTimerStatus();return true;
}

void DaikinDriver::sendTimerLampCommand()
{
    if (!timer_lamp_pending_ || !fb_.valid) return;
    PayloadBuffer payload = fb_.confirmed;
    payload[0] = requested_timer_lamp_ ? '1' : '0';
    if (sendCombined("FB",payload)) timer_lamp_pending_=false;
}

bool DaikinDriver::configureTemperatureLimits(daikin::Mode mode,float minimum,float maximum)
{
    const auto index=static_cast<size_t>(mode);
    if(index>=configured_min_.size() || !std::isfinite(minimum) || !std::isfinite(maximum) || minimum<10 || maximum>49.5f || minimum>maximum || std::round(minimum*2)!=minimum*2 || std::round(maximum*2)!=maximum*2) return false;
    configured_min_[index]=minimum;configured_max_[index]=maximum;configured_temperature_mask_|=1U<<index;return true;
}

bool DaikinDriver::setClimateVariant(daikin::ClimateVariant variant,daikin::HumidityMode humidity)
{
    if(!serial_) return false;
    using V=daikin::ClimateVariant;using H=daikin::HumidityMode;
    const bool compound=variant==V::DryCooling || variant==V::HumidHeating || variant==V::SararaDry || variant==V::UruruHumidify;
    uint8_t mask=variant==V::DryCooling?2:variant==V::HumidHeating?4:variant==V::SararaDry?16:variant==V::UruruHumidify?32:0;
    if(compound && support_.simple_humidify && variant!=V::UruruHumidify) return false;
    if(compound && (!support_.f2_known || (!support_.simple_humidify && !(humidity_modes_&mask)) || (variant==V::UruruHumidify && !support_.humidify_operation))) return false;
    if(compound && (humidity==H::Off || humidity==H::Unknown || ((variant==V::SararaDry || variant==V::UruruHumidify) && humidity==H::Low))) return false;
    requested_variant_=variant;variant_pending_=true;
    const auto mode=variant==V::Auto?AirConditionModeAuto:variant==V::FanOnly?AirConditionModeFan:variant==V::SararaDry?AirConditionModeDry:
        (variant==V::Heat || variant==V::HumidHeating || variant==V::UruruHumidify)?AirConditionModeHeat:AirConditionModeCool;
    setMode(mode);
    if(!support_.simple_humidify || !compound) {
        requested_humidity_mode_=compound?humidity:H::Off;
        humidity_pending_=true;
    }
    return true;
}

bool DaikinDriver::setFixedVerticalPosition(uint8_t position)
{
    if (!serial_ || !dr_.read_supported || position > 6) return false;
    return setVerticalPositionCode(position == 6 ? '?' : char('0' + position));
}

bool DaikinDriver::setVerticalPositionCode(char position)
{
    if(position!='?' && (position<'0' || position>'5')) return false;
    requested_vertical_position_=position=='?' ? 6 : position-'0';vertical_pending_=true;sendVerticalPositionCommand();return true;
}

// === Capability Queries ===
float DaikinDriver::getMinimumTargetTemperature()
{
    const auto mode = climate_dirty_ & DirtyMode ? pending_.climate.mode : state_.mode;
    const auto index=static_cast<size_t>(mode);
    if(configured_temperature_mask_ & (1U<<index)) return configured_min_[index];
    if (mode == daikin::Mode::Heat && state_.heatMinimumValid)
        return state_.heatMinimumC;
    return 18.0f; // conservative fallback where no absolute limit is documented
}

float DaikinDriver::getMaximumTargetTemperature()
{
    const auto mode=climate_dirty_ & DirtyMode ? pending_.climate.mode : state_.mode;
    const auto index=static_cast<size_t>(mode);
    if(configured_temperature_mask_ & (1U<<index)) return configured_max_[index];
    return std::max(32.0f, state_.heatMinimumValid && mode==daikin::Mode::Heat ? state_.heatMinimumC : 32.0f); // fallback policy, not a universal protocol limit
}

unsigned int DaikinDriver::getMaximumFanSpeed()
{
    if (support_.fan_capability_known && !support_.fan) return 0;
    return 6; // Allow Scene enumerations up to value 6 (0=Auto, 1=Silent, 2..6 -> Stufe 1..5)
}

unsigned int DaikinDriver::getMaximumHorizontalFixPosition()
{
    return 0; // Horizontal fixed positions have no established S21 command.
}

unsigned int DaikinDriver::getMaximumVerticalFixPosition()
{
    return dr_.read_supported ? 5 : 0; // addressable range, not certification of every position

}

unsigned int DaikinDriver::getMaximumHumidityModeLevels()
{
    return humidity_modes_ && !support_.simple_humidify ? 5 : 0;
}

bool DaikinDriver::supportExternalRoomTemperatureSensor()
{
    return false; // Intelligent Eye is not an external temperature input.
}

float DaikinDriver::accuracyInDegrees()
{
    return 0.5f; // S21 protocol supports 0.5°C accuracy
}

// === Control Methods ===
void DaikinDriver::setPower(bool power)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }

    // Track explicit OFF commands from KO to prevent safety override
    if (!power)
    {
        pending_explicit_off_ = true;
    }

    // Update pending state - preserve current pending values, only change power
    // CRITICAL: Always preserve pending_ values, never overwrite with state_
    ++climate_generation_[0];
    climate_dirty_ |= DirtyPower;
    pending_.climate.power = power; // UPDATE power state only
    pending_.activate_climate = true;

    // Send S21 power command - D1 controls power/mode/temp/fan
    sendClimateCommand();
}

void DaikinDriver::setMode(AirConditionMode mode)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }

    daikin::Mode daikinMode = openknx_to_daikin_mode(mode); // Convert OpenKNX mode to Daikin mode

    // Update pending state - preserve current pending values, only change mode
    // CRITICAL: Always preserve pending_ values, never overwrite with state_
    ++climate_generation_[1];
    if (!variant_pending_) requested_variant_=daikin::ClimateVariant::Auto;
    climate_dirty_ |= DirtyMode;
    pending_.climate.mode = daikinMode; // UPDATE mode only
    pending_.activate_climate = true;

    sendClimateCommand(); // Send S21 mode command
}

void DaikinDriver::setTargetTemperature(float temperaturCelsius)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }

    if (!std::isfinite(temperaturCelsius)) return;
    float temp = std::round(std::clamp(temperaturCelsius, getMinimumTargetTemperature(), getMaximumTargetTemperature()) * 2.0f) / 2.0f; // Clamp temperature to valid range

    // Update pending state - preserve current pending values, only change temperature
    // CRITICAL: Always preserve pending_ values, never overwrite with state_
    ++climate_generation_[2];
    climate_dirty_ |= DirtyTemperature;
    pending_.climate.targetC = temp; // UPDATE temperature only
    pending_.activate_climate = true;

    sendClimateCommand(); // Send S21 temperature command - D1 controls power/mode/temp/fan
}

void DaikinDriver::setFanSpeed(unsigned int speed)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }
    if (support_.fan_capability_known && !support_.fan) return;
    if (speed > getMaximumFanSpeed())
    {
        logErrorP("Invalid fan speed: %u (max: %u)", speed, getMaximumFanSpeed());
        return;
    }
    daikin::DaikinFanMode fanMode = openknx_to_daikin_fan(speed); // Convert speed to Daikin fan mode

    // Track explicit fan commands to prevent safety guard from overriding them
    pending_explicit_fan_ = true;

    // Update pending state - preserve current pending values, only change fan
    // CRITICAL: Always preserve pending_ values, never overwrite with state_
    ++climate_generation_[3];
    climate_dirty_ |= DirtyFan;
    pending_.climate.fan_mode = fanMode; // UPDATE fan mode only
    pending_.activate_climate = true;

    sendClimateCommand(); // Send S21 fan command
}

void DaikinDriver::setSwingHorizontal(bool swing)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }

    if (swing && support_.f2_known && !support_.horizontal_swing) return;

    // What the unit currently reports (from last F5)
    const bool devV = (state_.swing == daikin::Swing::Vertical || state_.swing == daikin::Swing::Both);
    const bool devH = (state_.swing == daikin::Swing::Horizontal || state_.swing == daikin::Swing::Both);

    // Keep the other axis as it is (or last requested if available)
    const bool wantH = swing;
    const bool wantV = pending_.activate_swing_mode ? pending_.climate.swing_v == daikin::Swing::Vertical : devV;

    if (!pending_.activate_swing_mode && wantV == devV && wantH == devH)
    {
        logDebugP("Swing unchanged vs device; not sending D5");
        return;
    }

    swing_dirty_ |= 2;
    pending_.climate.swing_v = wantV ? daikin::Swing::Vertical : daikin::Swing::Off;
    pending_.climate.swing_h = wantH ? daikin::Swing::Horizontal : daikin::Swing::Off;
    pending_.activate_swing_mode = true;
    sendSwingCommand();
}

void DaikinDriver::setSwingVertical(bool swing)
{
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }

    // What the unit currently reports (from last F5)
    const bool devV = (state_.swing == daikin::Swing::Vertical || state_.swing == daikin::Swing::Both);
    const bool devH = (state_.swing == daikin::Swing::Horizontal || state_.swing == daikin::Swing::Both);

    // Keep the other axis as it is (or last requested if available)
    const bool wantV = swing;
    const bool wantH = pending_.activate_swing_mode ? pending_.climate.swing_h == daikin::Swing::Horizontal : devH;

    if (!pending_.activate_swing_mode && wantV == devV && wantH == devH)
    {
        logDebugP("Swing unchanged vs device; not sending D5");
        return;
    }

    swing_dirty_ |= 1;
    pending_.climate.swing_v = wantV ? daikin::Swing::Vertical : daikin::Swing::Off;
    pending_.climate.swing_h = wantH ? daikin::Swing::Horizontal : daikin::Swing::Off;
    pending_.activate_swing_mode = true;
    sendSwingCommand();
}

void DaikinDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // No verified indexed horizontal command is known. The documented FTXA
    // hold method is to stop horizontal swing at the desired position.
    logInfoP("Indexed horizontal position %u unavailable; use horizontal swing Off to hold the current position", position);
}

void DaikinDriver::setSwingVerticalFixPosition(unsigned int position)
{
    if (position > 5) return;
    requested_vertical_position_ = position;
    vertical_pending_ = true;
    sendVerticalPositionCommand();
}

void DaikinDriver::sendVerticalPositionCommand()
{
    if (!vertical_pending_ || !dr_.valid) return;
    PayloadBuffer payload = dr_.confirmed;
    payload[0] = requested_vertical_position_==6 ? '?' : '0' + requested_vertical_position_;
    payload[1]=payload[2]=payload[3]='0';
    if (sendCombined("DR", payload)) vertical_pending_ = false;
}

void DaikinDriver::setExternalSensorRoomTemperature(float)
{
    logInfoP("S21 external temperature injection has no verified command; request ignored");
}

void DaikinDriver::setWifiLed(bool on)
{
    setLedState(on ? daikin::LedState::On : daikin::LedState::Off);
}

bool DaikinDriver::encodeHumidity(daikin::HumidityMode mode,uint8_t& wire) const
{
    using H=daikin::HumidityMode;
    const auto operating=climate_dirty_ & DirtyMode ? pending_.climate.mode : state_.mode;
    const bool heat=operating==daikin::Mode::Heat || (operating==daikin::Mode::Auto && state_.autoBias==daikin::AutoBias::Heating);
    const bool cool=operating==daikin::Mode::Cool || operating==daikin::Mode::Dry || (operating==daikin::Mode::Auto && state_.autoBias==daikin::AutoBias::Cooling);
    const bool humidify=(variant_pending_ && requested_variant_==daikin::ClimateVariant::UruruHumidify) || state_.climateVariant==daikin::ClimateVariant::UruruHumidify;
    const uint8_t mask=humidify?32:operating==daikin::Mode::Auto?1:operating==daikin::Mode::Heat?4:operating==daikin::Mode::Cool?2:operating==daikin::Mode::Dry?16:0;
    if (mode==H::Off) {wire='0';return true;}
    if(support_.simple_humidify || !(humidity_modes_&mask) || (!heat && !cool) || ((operating==daikin::Mode::Dry || humidify) && mode==H::Low)) return false;
    switch(mode) {
        case H::Low:wire=heat?0x38:0x3A;break;
        case H::Standard:wire=heat?0x39:0x3B;break;
        case H::High:wire=heat?0x3A:0x3C;break;
        case H::Continuous:wire=0xFF;break;
        default:return false;
    }
    return true;
}

static daikin::HumidityMode toDaikinHumidity(AirConditionHumidityLevel level)
{
    using H=daikin::HumidityMode;
    switch (level) {
        case AirConditionHumidityLevel::Off:return H::Off;
        case AirConditionHumidityLevel::Low:return H::Low;
        case AirConditionHumidityLevel::Standard:return H::Standard;
        case AirConditionHumidityLevel::High:return H::High;
        case AirConditionHumidityLevel::Continuous:return H::Continuous;
    }
    return H::Unknown;
}

uint8_t DaikinDriver::availableHumidityLevels() const
{
    if (!support_.f2_known || support_.simple_humidify || !humidity_modes_ || (!state_.basicValid && !(climate_dirty_ & DirtyMode))) return 0;
    uint8_t available=0, wire=0;
    for (uint8_t level=0;level<=4;level++)
        if (encodeHumidity(toDaikinHumidity(static_cast<AirConditionHumidityLevel>(level)),wire)) available |= 1U<<level;
    return available;
}

bool DaikinDriver::setHumidityLevel(AirConditionHumidityLevel level)
{
    const uint8_t value=static_cast<uint8_t>(level);
    if (value>4 || !(availableHumidityLevels() & (1U<<value))) return false;
    return setHumidityMode(toDaikinHumidity(level));
}

bool DaikinDriver::setHumidityMode(daikin::HumidityMode mode)
{
    if(!serial_) return false;
    uint8_t wire=0;if(!encodeHumidity(mode,wire)) return false;
    requested_humidity_mode_=mode;requested_humidity_=wire;humidity_pending_=true;
    sendSwingCommand();return true;
}

AirConditionOptionalState DaikinDriver::optionalControlState() const
{
    AirConditionOptionalState result;
    // Econo belongs to F7; all other bits belong to F6. Eye is meaningful only
    // once FK has identified the hardware. Field validity is not write support.
    result.valid=(state_.specialModesValid?0x2D:0) | (state_.demandValid?2:0) |
        (state_.specialModesValid && support_.sensor_hardware_known && support_.sensor_hardware?16:0);
    result.values=(state_.powerful?1:0) | (state_.econo?2:0) | (state_.quiet?4:0) |
        (state_.comfort?8:0) | (state_.sensor?16:0) | (state_.streamer?32:0);
    result.available=(d6_.read_supported?0x2D:0) | (d7_.read_supported?2:0) |
        (d6_.read_supported && support_.sensor_hardware?16:0);
    if (support_.special_features_known) {
        if (!support_.powerful) result.available &= ~1;
        if (!econoControlAvailable()) result.available &= ~2;
        if (!support_.streamer) result.available &= ~32;
    }
    result.ledLevels=d6_.read_supported && support_.sensor_hardware_known ? (support_.sensor_hardware?5:7) : 0;
    result.ledValid=state_.ledState!=daikin::LedState::Unknown;
    result.led=state_.ledState==daikin::LedState::On?AirConditionUnitLed::On:
        state_.ledState==daikin::LedState::Dark?AirConditionUnitLed::Dark:AirConditionUnitLed::Off;
    return result;
}

bool DaikinDriver::econoControlAvailable() const
{
    return d7_.read_supported && (!support_.special_features_known || support_.econo ||
        econo_observed_on_ || fieldWriteVerified("F7",1,0xFF));
}

bool DaikinDriver::probeEcono(bool on)
{
    if (!serial_ || !d7_.read_supported || !state_.basicValid) return false;
    // A deliberate probe changes only documented D7 Econo. Do not silently
    // clear mutually exclusive presets or manufacture availability from ACK.
    if (on && state_.powerful) return false;
    pending_.climate.econo=on;pending_.activate_econo=true;econo_probe_pending_=true;
    d7_.rejected_valid=false;
    sendEconoCommand();
    return true;
}

bool DaikinDriver::setUnitLed(AirConditionUnitLed state)
{
    const auto value=static_cast<uint8_t>(state);
    if (!serial_ || value>2 || !(optionalControlState().ledLevels & (1U<<value))) return false;
    setLedState(state==AirConditionUnitLed::On?daikin::LedState::On:state==AirConditionUnitLed::Dark?daikin::LedState::Dark:daikin::LedState::Off);
    return true;
}

bool DaikinDriver::setOptionalControl(AirConditionOptionalControl control, bool on)
{
    const auto value=static_cast<uint8_t>(control);
    if (!serial_ || value>5) return false;
    if (!(optionalControlState().available & (1U<<value))) {
        logInfoP("Optional control %u unavailable or not discovered; request ignored", value);
        return false;
    }
    using C=AirConditionOptionalControl;
    // Powerful excludes Econo and Outdoor Quiet. Econo and Quiet may coexist
    // (FTXA user guide, section 5.3.6); independent controls preserve that pair.
    if (on && control==C::Powerful) {
        pending_.climate.econo=false;pending_.activate_econo=true;
        pending_.climate.quiet=false;pending_.activate_quiet=true;
    }
    if (on && (control==C::Econo || control==C::Quiet)) {
        pending_.climate.powerful=false;pending_.activate_powerful=true;
    }
    switch (control) {
        case C::Powerful:pending_.climate.powerful=on;pending_.activate_powerful=true;break;
        case C::Econo:pending_.climate.econo=on;pending_.activate_econo=true;break;
        case C::Quiet:pending_.climate.quiet=on;pending_.activate_quiet=true;break;
        case C::Comfort:setComfort(on);return true;
        case C::IntelligentEye:setIntelligentEye(on);return true;
        case C::Streamer:setAirPurification(on);return true;
    }
    if (state_.econo && on && control==C::Powerful) sendEconoCommand();
    else sendSpecialModesCommand();
    sendEconoCommand();return true;
}

void DaikinDriver::setLedState(daikin::LedState led)
{
    if (led == daikin::LedState::Unknown || (support_.sensor_hardware && led == daikin::LedState::Dark)) return;
    pending_.led = led;
    pending_.climate.led = led == daikin::LedState::On;
    pending_.activate_led = true;
    sendLedCommand();
}

void DaikinDriver::setDeviceMode(AirConditionDeviceMode mode)
{
    if (!serial_) return;
    // Reject before queuing clears: unsupported Econo must not silently disable
    // an active Powerful/Quiet setting through the legacy preset selector.
    if (mode==AirConditionDeviceModeEco && !(optionalControlState().available & 2)) {
        logInfoP("Econo preset unavailable or not discovered; request ignored");
        return;
    }
    if (mode==AirConditionDeviceModeSilent2)
        logInfoP("Quiet 2 aliases Quiet 1 on S21; feedback uses Quiet 1");
    pending_.climate.powerful = mode == AirConditionDeviceModeHiPower;
    pending_.climate.quiet = mode == AirConditionDeviceModeSilent1 || mode == AirConditionDeviceModeSilent2;
    pending_.climate.econo = mode == AirConditionDeviceModeEco;
    pending_.activate_powerful = pending_.activate_quiet = pending_.activate_econo = true;
    // D7 Econo and D6 Powerful/Quiet share mutually exclusive presets.
    // Disable confirmed Econo before requesting another special mode.
    if (!pending_.climate.econo && state_.econo) sendEconoCommand();
    else sendSpecialModesCommand();
}

void DaikinDriver::setMaxPowerLevel(uint8_t percentage)
{
    if (!demandRequestAllowed(percentage)) {
        logInfoP("Demand percentage not verified for this model: %u", percentage);
        return;
    }
    requested_demand_ = percentage;
    demand_pending_ = true;
    sendEconoCommand();
}

bool DaikinDriver::demandRequestAllowed(uint8_t percentage) const
{
    return (support_.demand || demand_configured_) && percentage<=100 &&
        percentage>=demand_minimum_ && (100-percentage)%demand_step_==0;
}

void DaikinDriver::showModelProfiles() const
{
    logInfoP("--- S21 model profiles ---");
    logInfoP("Model: %s; fallback code: %s; indoor firmware: %s",
        model_name_.empty()?"Unknown":model_name_.c_str(), identification_.modelCode.c_str(), firmware_version_.c_str());
    logInfoP("Demand: FK advertised=%s, configured=%s, minimum=%u%%, step=%u%%",
        support_.demand?"Yes":"No/not discovered", demand_configured_?"Yes":"No", demand_minimum_, demand_step_);
    logInfoP("Demand readback: %s; demand field write verified=%s",
        state_.demandPercentageValid?"Valid":"Unavailable", fieldWriteVerified("F7",0,0xFF)?"Yes":"No");
    logInfoP("Timers: configured=%s, step=%u min, maximum=%u min; field write verified=%s",
        timer_step_?"Yes":"No", timer_step_, timer_max_,
        fieldWriteVerified("F3",0,0xFF)?"Yes":"No");
    const std::array<const char*,8> outcomes={"None","Awaiting ACK","Awaiting readback","Confirmed","Rejected","Timeout","Error","Mismatch"};
    logInfoP("Climate confirmation: %s", outcomes[static_cast<size_t>(climate_outcome_)]);
    logInfoP("Timer lamp: %s; write=%s; field write verified=%s",
        !state_.timerLampValid?"Unknown":state_.timerLamp?"On":"Off",
        outcomes[static_cast<size_t>(fb_.outcome)],fieldWriteVerified("FB",0,0xFF)?"Yes":"No");
    logInfoP("Configured ranges are installer assertions, not automatic model verification");
    logInfoP("Timer delays are fixed at programming time, not remaining countdowns");
}

bool DaikinDriver::configureDemandLimits(uint8_t minimum, uint8_t step)
{
    if (minimum > 100 || step == 0 || step > 100 || (100 - minimum) % step) return false;
    demand_configured_=true;
    demand_minimum_ = minimum;
    demand_step_ = step;
    return true;
}

void DaikinDriver::setAirPurification(bool on)
{
    logInfoP("Setting air purification (streamer): %s", on ? "On" : "Off");
    if (!serial_)
    {
        logErrorP("S21 driver not initialized");
        return;
    }

    // Update pending state
    pending_.climate.streamer = on;
    pending_.activate_streamer = true;

    sendStreamerCommand(); // Send S21 streamer command - D6 controls streamer bitfield
}

// === S21 Protocol Implementation ===

void DaikinDriver::initializeProtocolDetection()
{
    // Clear existing queries
    queries_.clear();
    query_index_ = 0;
    protocol_detection_phase_ = true;
    protocol_detection_failed_reported_ = false;
    protocol_detection_attempts_ = 0;

    last_protocol_detection_attempt_ = millis(); // Mark that we're attempting protocol detection now

    logInfoP("Starting protocol detection phase");

    // protocol detection according to S21 wiki:
    // 1. Send F8 first - this determines base protocol (v0, v2, v3+)
    // 2. Send FY00 only if F8 indicates v2+ (to distinguish v2 vs v3.x)
    // Try this sequence across multiple attempts before giving up
    queries_.emplace_back(StateQuery::OldProtocol, [this](uint8_t* data, size_t data_size) { handle_f8_response(data, data_size); });
    queries_.emplace_back(StateQuery::NewProtocol, [this](uint8_t* data, size_t data_size) { handle_fy00_response(data, data_size); });

    logInfoP("Protocol detection phase initialized with %zu queries (F8 → FY00)",
             queries_.size());
}

void DaikinDriver::initializeQueries()
{
    queries_.clear();query_index_=0;confirmation_resume_=SIZE_MAX;protocol_detection_phase_=false;
    auto add=[&](std::string_view cmd,DaikinQueryState::handler_fn handler=nullptr,bool once=false,uint32_t interval=0){
        queries_.emplace_back(cmd,handler,once);queries_.back().interval_ms=interval;
    };
    add("F1",[this](uint8_t* d,size_t n){handle_f1_response(d,n);});
    add("F2",[this](uint8_t* d,size_t n){handle_f2_response(d,n);});
    add("F3",[this](uint8_t* d,size_t n){handle_f3_response(d,n);});
    add("F4",[this](uint8_t* d,size_t n){handle_f4_response(d,n);});
    add("F5",[this](uint8_t* d,size_t n){handle_f5_response(d,n);});
    add("F6",[this](uint8_t* d,size_t n){handle_f6_response(d,n);});
    add("F8",[this](uint8_t* d,size_t n){handle_f8_response(d,n);});
    for(auto cmd:{std::string_view{"M"},std::string_view{"V"}}) add(cmd,[this,cmd](uint8_t* d,size_t n){handle_metadata(cmd,d,n);},true);
    const auto major=protocol_version_.major,minor=protocol_version_.minor;
    if(major>=1) {
        add("F9",[this](uint8_t* d,size_t n){handle_f9_response(d,n);});
        add("FB",[this](uint8_t* d,size_t n){handle_fb_response(d,n);});
        add("FC",[this](uint8_t* d,size_t n){handle_fc_response(d,n);},true);
        add("FA");
    }
    if(major>=2) {
        add("F7",[this](uint8_t* d,size_t n){handle_f7_response(d,n);});
        add("FR",[this](uint8_t* d,size_t n){handle_fr_response(d,n);});
        add("FG",[this](uint8_t* d,size_t n){handle_fg_response(d,n);});
        add("FK",[this](uint8_t* d,size_t n){handle_fk_response(d,n);});
        add("FM",[this](uint8_t* d,size_t n){handle_fm_response(d,n);});
        for(auto cmd:{std::string_view{"FN"},std::string_view{"FP"},std::string_view{"FQ"},std::string_view{"FS"},std::string_view{"FT"}})
            add(cmd,[this,cmd](uint8_t* d,size_t n){handle_metadata(cmd,d,n);});
        add("FL");add("FV");
        add("VS000M",[this](uint8_t* d,size_t n){handle_firmware_response(d,n);},true);
    }
    if(major>=3) {
        add("FY00",[this](uint8_t* d,size_t n){handle_fy00_response(d,n);},true);
        add("FY10",nullptr,true);add("FY20",nullptr,true);
    }
    if(major>3 || (major==3 && minor>=1)) add("FU02",[this](uint8_t* d,size_t n){handle_fu_fx_response("FU02",d,n);});
    if(major>3 || (major==3 && minor>=20)) {
        add("FU00",[this](uint8_t* d,size_t n){handle_fu_fx_response("FU00",d,n);},true);
        add("FU04",[this](uint8_t* d,size_t n){handle_fu_fx_response("FU04",d,n);});
        for(std::string_view cmd:std::initializer_list<std::string_view>{"FX00","FX10","FX20","FX30","FX40","FX50","FX60","FX70","FX80","FX90","FXA0","FXB0","FXC0"}) {
#ifdef DAIKIN_LIVE_POWER_INTERVAL_MS
            const uint32_t live_interval=DAIKIN_LIVE_POWER_INTERVAL_MS;
#else
            const uint32_t live_interval=10000;
#endif
            add(cmd,[this,cmd](uint8_t* d,size_t n){handle_fu_fx_response(cmd,d,n);},false,cmd=="FX60"?live_interval:60000);
        }
        add("Rz52",nullptr,false,60000);add("Rz72",nullptr,false,60000);
    }
    if(major>3 || (major==3 && minor>=40)) {
        for(std::string_view cmd:std::initializer_list<std::string_view>{"FU05","FU15","FU25","FU35","FU45"}) add(cmd,[this,cmd](uint8_t* d,size_t n){handle_fu_fx_response(cmd,d,n);},true);
        for(std::string_view cmd:std::initializer_list<std::string_view>{"FXD0","FXE0","FXF0","FX01","FX11","FX21","FX31","FX41","FX51","FX61","FX71","FX81"}) add(cmd,[this,cmd](uint8_t* d,size_t n){handle_fu_fx_response(cmd,d,n);},false,60000);
    }
    for(std::string_view cmd:std::initializer_list<std::string_view>{"RA","RB","RC"}) add(cmd,[this,cmd](uint8_t* d,size_t n){handle_status_sensor(cmd,d,n);});
    for(std::string_view cmd:std::initializer_list<std::string_view>{"RD","RE","RF"}) add(cmd,[this,cmd](uint8_t* d,size_t n){handle_supplemental_sensor(cmd,d,n);});
    for(std::string_view cmd:std::initializer_list<std::string_view>{"Rb","Rm","Rt","RW"}) add(cmd,[this,cmd](uint8_t* d,size_t n){handle_aux_sensor(cmd,d,n);});
    add("RM",[this](uint8_t* d,size_t n){handle_vane_angle(true,d,n);});add("RN",[this](uint8_t* d,size_t n){handle_vane_angle(false,d,n);});
    add("RK",[this](uint8_t* d,size_t n){handle_fan_rpm(true,d,n);});add("RL",[this](uint8_t* d,size_t n){handle_fan_rpm(false,d,n);});
    add("RI",[this](uint8_t* d,size_t n){handle_ri_response(d,n);});add("RH",[this](uint8_t* d,size_t n){handle_rh_response(d,n);});
    add("RX",[this](uint8_t* d,size_t n){handle_rx_response(d,n);});add("RG",[this](uint8_t* d,size_t n){handle_rg_response(d,n);});
    add("Ra",[this](uint8_t* d,size_t n){handle_ra_response(d,n);});add("Rd",[this](uint8_t* d,size_t n){handle_rd_response(d,n);});
    add("Re",[this](uint8_t* d,size_t n){handle_re_response(d,n);});add("Rg",[this](uint8_t* d,size_t n){handle_rg2_response(d,n);});
    add("RzB2",[this](uint8_t* d,size_t n){handle_rzb2_response(d,n);});add("RzC3",[this](uint8_t* d,size_t n){handle_rzc3_response(d,n);});
}

void DaikinDriver::updateQueryStateMachine()
{
    uint32_t now = millis();

    // Readbacks of pending writes must not wait behind the full telemetry cycle.
    if (query_state_ == QueryState::Idle && !protocol_detection_phase_ &&
        (sent_climate_dirty_ || d3_.awaiting_readback || d5_.awaiting_readback ||
         d6_.awaiting_readback || d7_.awaiting_readback || dr_.awaiting_readback || fb_.awaiting_readback) &&
        now-last_confirmation_poll_ >= 1000)
        query_state_ = QueryState::WaitingToSend;

    // Check if we need extended timeout for first F1 after write command
    auto isFirstF1AfterWrite = post_write_first_f1_pending_ && query_index_ < queries_.size() && queries_[query_index_].command == StateQuery::Basic; // F1

    switch (query_state_)
    {
        case QueryState::Idle:
            if(now-last_query_cycle_>=timing_.cycle_ms) triggerQueryCycle();
            break;

        case QueryState::WaitingToSend:
            // Check if enough time has passed since last send
            if (canSendQuery())
            {
                selectConfirmationQuery();
                isFirstF1AfterWrite = post_write_first_f1_pending_ && query_index_ < queries_.size() && queries_[query_index_].command == StateQuery::Basic;
                // Find next supported query (skip bad queries)
                // Non-blocking: limit search iterations to prevent blocking
                constexpr int MAX_QUERY_SEARCH_ITERATIONS = 10;
                int search_iterations = 0;

                while (query_index_ < queries_.size() && search_iterations < MAX_QUERY_SEARCH_ITERATIONS)
                {
                    auto& query = queries_[query_index_];
                    search_iterations++;

                    // Smart NAK tracking: skip bad queries
                    if(query.bad && now-query.last_nak_ms>=timing_.reprobe_ms) {query.bad=false;query.naks=0;}
                    if (query.bad)
                    {
                        // logDebugP("Skipping bad query (marked as unsupported): %.*s", static_cast<int>(query.command.size()), query.command.data());
                        query_index_++;
                        continue;
                    }

                    // Skip static queries that have already been successfully read
                    if (query.acked && (query.is_static || (query.interval_ms && (now - query.last_sample_ms) < query.interval_ms)))
                    {
                        // logDebugP("Skipping static query (already read): %.*s", static_cast<int>(query.command.size()), query.command.data());
                        query_index_++;
                        continue;
                    }

                    if (isQuerySupported(query.command))
                    {
                        // Send this query
                        // logDebugP("S21: Protocol detection - sending %.*s", static_cast<int>(query.command.size()), query.command.data());

                        invalidatePage(query.command);
                        serial_->configureTimeouts(isFirstF1AfterWrite?timing_.post_write_f1_ms:timing_.ack_ms,isFirstF1AfterWrite?timing_.post_write_f1_ms:timing_.rx_ms);
                        std::string cmd_str(query.command);
                        query_state_=QueryState::WaitingForAck;
                        if (!serial_->send_frame(cmd_str)) break;

                        last_send_time_ = now;
                        last_query_time_ = now;
                        query_state_ = QueryState::WaitingForAck; // Will transition to WaitingForGrace after ACK or directly on Frame
                        state_start_time_ = now;
                        break;
                    }
                    else
                    {
                        // Skip unsupported query
                        logDebugP("Skipping unsupported query: %.*s",
                                  static_cast<int>(query.command.size()), query.command.data());
                        query_index_++;
                    }
                }

                if (query_index_ >= queries_.size())
                {
                    // Cycle complete
                    if (protocol_detection_phase_)
                    {
                        // Protocol detection phase complete for this attempt
                        protocol_detection_attempts_++;
                        if (protocol_detection_attempts_ > 12) protocol_detection_attempts_ = 12;

                        if (protocol_version_ != daikin::ProtocolUndetected)
                        {
                            // Protocol detected
                            logInfoP("Protocol detection: Successfully detected v%d.%d after %u attempts",
                                     protocol_version_.major, protocol_version_.minor,
                                     protocol_detection_attempts_);

                            initializeQueries(); // Initialize protocol-specific queries
                            query_index_ = 0;
                            query_state_ = QueryState::WaitingToSend;
                            state_start_time_ = now;
                            return; // Continue immediately with proper queries
                        }

                        // Max attempts reached for protocol detection
                        if (protocol_detection_attempts_ >= 12)
                        { // 12 attempts should be enough for protocol detection
                            // Protocol detection failed completely (report ONCE)
                            if (!protocol_detection_failed_reported_)
                            {
                                logErrorP("Protocol detection failed after %d attempts", protocol_detection_attempts_);
                                statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError, "Protocol detection failed");
                                if (seed_kos_pending_ && !online_ && !state_.online)
                                {
                                    logInfoP("Protocol detection failed at startup -> publishing offline default state");
                                    state_.online = false;
                                    gate_publish_until_full_sample_ = false;
                                    publishState();
                                }
                                protocol_detection_failed_reported_ = true;
                            }

                            protocol_detection_phase_ = false;      // Leave detection phase and wait for the periodic retry
                            last_protocol_detection_attempt_ = now; // schedule next retry using existing timer logic
                            query_index_ = 0;                       // reset query loop to idle
                            query_state_ = QueryState::Idle;
                            return;
                        }

                        // Try F8→FY00 again
                        logDebugP("Protocol detection: No response, retry %u", protocol_detection_attempts_);
                        query_index_ = 0;
                        query_state_ = QueryState::WaitingToSend;
                        state_start_time_ = now;
                        return;
                    }

                    if (!first_cycle_completed_)
                    {
                        first_cycle_completed_ = true;
                        logInfoP("First S21 query cycle completed with protocol v%d.%d",
                                 protocol_version_.major, protocol_version_.minor);

                        // Send any deferred LED commands now
                        if (pending_.activate_led)
                        {
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
        case QueryState::WaitingForGrace:
            // ACK/frame/NAK/timeout/error callbacks exclusively advance the query.
            break;

        case QueryState::Cooldown:
            query_state_=QueryState::Idle;
            break;

        case QueryState::PostWriteSettle:
            // v0 stability: Extended settling period after write commands
            if ((now - last_write_time_) >= getSettlePeriodForCommand(last_write_command_))
            {
                logInfoP("Post-write settle complete, scheduling first F1 query with extended timeout");

                // Clear completed settle tracking BEFORE scheduling follow-up work.
                // scheduleFirstF1AfterWrite() may flush a coalesced D1/D5 write, which
                // re-enters PostWriteSettle and sets a new last_write_time_.
                // Clearing afterwards can overwrite that new timestamp and stall the state machine.
                last_write_time_ = 0;
                last_write_command_ = LastWriteCommand::None;

                scheduleFirstF1AfterWrite(); // First query after write gets special handling for v0 stability
            }
            break;
    }
}

bool DaikinDriver::canSendQuery() const
{
    if (!serial_ || serial_->busy()) return false;
    uint32_t now = millis();
    // Determine effective inter-query delay based on system load
    uint32_t effective_delay = high_load_detected_ ? timing_.high_load_ms : timing_.inter_query_ms;
    // Check minimum time between sends
    if (last_send_time_ != 0 && (now - last_send_time_) < effective_delay)
    {
        return false;
    }
    return true;
}

void DaikinDriver::triggerQueryCycle()
{
    uint32_t now = millis();
    // Don't start new cycle if one is already running
    if (query_state_ != QueryState::Idle)
    {
        return;
    }
    last_query_cycle_ = now;
    startQueryCycle();
}

void DaikinDriver::startQueryCycle()
{
    if (!serial_)
    {
        return;
    }
    logDebugP("Starting S21 query cycle (non-blocking)");
    query_index_ = 0;
    query_state_ = QueryState::WaitingToSend;
    state_start_time_ = millis();
}

bool DaikinDriver::isQuerySupported(std::string_view command) const
{
    if(state_.serialErrorValid && state_.serialError && command!="F1" && command!="F2" && command!="F3" && command!="F4") return false;
    if((command=="RM" || command=="RN") && support_.f2_known && !support_.swing) return false;
    if((command=="RK" || command=="RL") && support_.fan_capability_known && !support_.fan) return false;
    if(!production_polling_ && (command=="FU15" || command=="FU25" || command=="FU35" || command=="FU45")) return false;
    if(!research_polling_ && (command=="FA" || command=="FL" || command=="FV" || command=="Rm" || command=="Rt" || command=="RW")) return false;
    if(!extended_polling_ && (command=="RA" || command=="RB" || command=="RC" || command=="RD" || command=="RE" || command=="RF")) return false;
    return true;
}

// === Post-write command management for v0 stability ===
void DaikinDriver::handleWriteCommandSent(LastWriteCommand cmd_type)
{
    last_write_command_ = cmd_type;
    last_write_time_ = millis();
    last_command_time_ = last_write_time_; // Sync legacy cooldown tracking
    query_state_ = QueryState::PostWriteSettle;

    // Clear RX buffer to avoid processing stale data
    clearRxBuffer();

    logInfoP("Entering post-write settle for %d ms", getSettlePeriodForCommand(cmd_type));
}

uint32_t DaikinDriver::getSettlePeriodForCommand(LastWriteCommand cmd_type) const
{
    // v0 units need longer settling periods
    bool is_v0 = isProtocolV0();

    switch (cmd_type)
    {
        case LastWriteCommand::D1_ModePower:
            return is_v0 ? timing_.v0_d1_settle_ms : timing_.d1_settle_ms; // v0: 4.5s, others: 3s
        case LastWriteCommand::D5_Swing:
            return is_v0 ? timing_.v0_d5_settle_ms : timing_.d5_settle_ms; // v0: 2.8s, others: 2s
        case LastWriteCommand::Other:
            return is_v0 ? timing_.v0_other_settle_ms : timing_.other_settle_ms; // v0: 1.5s, others: 1s
        default:
            return is_v0 ? timing_.v0_other_settle_ms : timing_.other_settle_ms; // Default fallback
    }
}

bool DaikinDriver::isInPostWriteSettle() const
{
    return query_state_ == QueryState::PostWriteSettle;
}

bool DaikinDriver::isProtocolV0() const
{
    return protocol_version_.major == 0;
}

void DaikinDriver::clearRxBuffer()
{
    // Clear any stale response data from previous query
    if (serial_)
    {
        // The DaikinSerial interface manages internal buffers automatically.
        // For now we clear our response tracking; if stale G-frames still appear
        // after D-commands, we may need to add a flush_input() method to DaikinSerial
        logDebugP("Clearing response tracking for fresh start after write command");

        // Reset response tracking state
        if (query_index_ < queries_.size())
        {
            auto& query = queries_[query_index_];
            query.response_data.clear();
        }

        // TODO: Consider adding serial_->flush_input() if stale frames persist
    }
}

void DaikinDriver::scheduleFirstF1AfterWrite()
{
    // First F1 after write gets extended timeout for v0 stability
    // Mark that we need extended handling for next query
    post_write_first_f1_pending_ = true;
    query_state_ = QueryState::WaitingToSend;
    query_index_ = 0; // Start with F1 query
    state_start_time_ = millis();

    // Flush any pending writes that were coalesced during settle
    if (write_pending_)
    {
        write_pending_ = false;
        logInfoP("Flushing coalesced D1 after settle period");
        // Send the accumulated pending_ state as one unified command
        sendClimateCommand();
        return; // sendClimateCommand will handle its own scheduling
    }

    // Flush any pending swing commands that were delayed during settle
    if (pending_.activate_swing_mode)
    {
        logInfoP("Flushing delayed D5 swing command after settle period");
        sendSwingCommand();
        return; // Let swing command handle its own scheduling
    }
    logInfoP("Scheduled first F1 after write with extended timeout");
}

// === Command Sending Implementation ===
void DaikinDriver::sendClimateCommand()
{
    if (!serial_ || !pending_.activate_climate)
    {
        return;
    }

    // Write coordination: Block new D1 commands during post-write settle
    if (isInPostWriteSettle() || serial_->busy())
    {
        write_pending_ = true; // Mark that we have a pending write
        logDebugP("Coalescing D1 during settle; will send after settle completes");
        return;
    }

    PayloadBuffer payload;

    // Only explicit requests override the most recent confirmed device sample.
    if (!state_.basicValid || !(sample_seen_mask_ & SEEN_F1)) return;
    if (!(climate_dirty_ & DirtyPower)) pending_.climate.power = state_.power;
    if (!(climate_dirty_ & DirtyMode)) pending_.climate.mode = state_.mode;
    if (!(climate_dirty_ & DirtyTemperature)) pending_.climate.targetC = state_.targetC;
    if (!(climate_dirty_ & DirtyFan)) pending_.climate.fan_mode = state_.fan;

    // ALWAYS use pending_ values as single source of truth
    const auto power_to_send = pending_.climate.power;
    const auto mode_to_send = pending_.climate.mode;
    const auto temp_to_send = std::round(std::clamp(pending_.climate.targetC, getMinimumTargetTemperature(), getMaximumTargetTemperature()) * 2.0f) / 2.0f;
    if (climate_dirty_ & DirtyTemperature) pending_.climate.targetC = temp_to_send;
    const auto fan_to_send = pending_.climate.fan_mode;

    // Byte 0: Power ('0'/'1' ASCII, not 0x00/0x01)
    payload[0] = power_to_send ? '1' : '0';

    // Byte 1: Mode - ALWAYS from pending_, no conditional logic
    // Map OpenKNX modes to S21 protocol ASCII characters (official S21 Protocol wiki)
    // S21 Protocol: "1"=Auto, "2"=Dry, "3"=Cool, "4"=Heat, "6"=Fan
    switch (mode_to_send)
    {
        case daikin::Mode::Heat: payload[1] = '4'; break;
        case daikin::Mode::Cool: payload[1] = '3'; break;
        case daikin::Mode::FanOnly: payload[1] = '6'; break;
        case daikin::Mode::Dry: payload[1] = '2'; break;
        case daikin::Mode::Auto:
        case daikin::Mode::Off:
        default: payload[1] = '1'; break;
    }

    // Byte 2: Target temperature (format: (setpoint/5) + 28)
    int16_t setpoint_c10 = static_cast<int16_t>(temp_to_send * 10); // Convert to C*10
    payload[2] = (setpoint_c10 / 5) + 28;
    if(mode_to_send==daikin::Mode::Dry) payload[2]=0x80;
    if(variant_pending_ && requested_variant_==daikin::ClimateVariant::UruruHumidify) payload[2]=0xC0;
    if (!(climate_dirty_ & DirtyTemperature) && !(climate_dirty_ & DirtyMode) && state_.targetTemperatureValid)
        payload[2]=state_.setpointRaw;
    if (!(climate_dirty_ & (DirtyMode | DirtyTemperature)) && state_.setpointRawValid &&
        (state_.setpointRaw == 0x80 || state_.setpointRaw == 0xC0)) payload[2] = state_.setpointRaw;

    // Byte 3: Fan speed
    switch (fan_to_send)
    {
        case daikin::DaikinFanMode::Auto: payload[3] = 'A'; break;
        case daikin::DaikinFanMode::Speed1: payload[3] = '3'; break;
        case daikin::DaikinFanMode::Speed2: payload[3] = '4'; break;
        case daikin::DaikinFanMode::Speed3: payload[3] = '5'; break;
        case daikin::DaikinFanMode::Speed4: payload[3] = '6'; break;
        case daikin::DaikinFanMode::Speed5: payload[3] = '7'; break;
        case daikin::DaikinFanMode::Silent: payload[3] = 'B'; break;
        default: payload[3] = 'A'; break;
    }

    // Deduplication: Skip only if identical to last D1 payload AND device already matches it.
    // This prevents skipping valid re-send requests after external changes (e.g. unit turned off via app/remote).
    const bool same_as_last_sent = std::equal(payload.begin(), payload.begin() + 4, last_sent_d1_payload_.begin());
    const auto current_power_payload = state_.power ? static_cast<uint8_t>('1') : static_cast<uint8_t>('0');

    uint8_t current_mode_payload = '1';
    switch (state_.mode)
    {
        case daikin::Mode::Heat: current_mode_payload = '4'; break;
        case daikin::Mode::Cool: current_mode_payload = '3'; break;
        case daikin::Mode::FanOnly: current_mode_payload = '6'; break;
        case daikin::Mode::Dry: current_mode_payload = '2'; break;
        case daikin::Mode::Auto:
        case daikin::Mode::Off:
        default: current_mode_payload = '1'; break;
    }

    const int16_t current_setpoint_c10 = static_cast<int16_t>(state_.targetC * 10);
    const auto current_temp_payload = static_cast<uint8_t>((current_setpoint_c10 / 5) + 28);

    uint8_t current_fan_payload = 'A';
    switch (state_.fan)
    {
        case daikin::DaikinFanMode::Auto: current_fan_payload = 'A'; break;
        case daikin::DaikinFanMode::Speed1: current_fan_payload = '3'; break;
        case daikin::DaikinFanMode::Speed2: current_fan_payload = '4'; break;
        case daikin::DaikinFanMode::Speed3: current_fan_payload = '5'; break;
        case daikin::DaikinFanMode::Speed4: current_fan_payload = '6'; break;
        case daikin::DaikinFanMode::Speed5: current_fan_payload = '7'; break;
        case daikin::DaikinFanMode::Silent: current_fan_payload = 'B'; break;
        default: current_fan_payload = 'A'; break;
    }

    const bool device_matches_payload = state_.online &&
                                        payload[0] == current_power_payload &&
                                        payload[1] == current_mode_payload &&
                                        payload[2] == current_temp_payload &&
                                        payload[3] == current_fan_payload;

    if (same_as_last_sent && device_matches_payload)
    {
        logDebugP("Skipping D1: payload unchanged ['%c','%c',0x%02X,'%c']",
                  payload[0], payload[1], payload[2], payload[3]);

        // Clear explicit OFF flag even when command is skipped
        if (pending_explicit_off_)
        {
            pending_explicit_off_ = false;
        }
        // Clear explicit fan flag even when command is skipped
        if (pending_explicit_fan_)
        {
            pending_explicit_fan_ = false;
        }
        pending_.activate_climate = false;
        return;
    }
    else if (same_as_last_sent && !device_matches_payload)
    {
        logInfoP("D1 dedupe bypass: payload unchanged but device state differs (or offline), forcing resend");
    }

    // Get human-readable names for logging
    const char* modeStr = (mode_to_send == daikin::Mode::Heat) ? "Heat" : (mode_to_send == daikin::Mode::Cool)  ? "Cool"
                                                                      : (mode_to_send == daikin::Mode::FanOnly) ? "Fan"
                                                                      : (mode_to_send == daikin::Mode::Dry)     ? "Dry"
                                                                                                                : "Auto";

    const char* fanStr = (fan_to_send == daikin::DaikinFanMode::Auto) ? "Auto" : (fan_to_send == daikin::DaikinFanMode::Silent) ? "Silent"
                                                                             : (fan_to_send == daikin::DaikinFanMode::Speed1)   ? "Speed1"
                                                                             : (fan_to_send == daikin::DaikinFanMode::Speed2)   ? "Speed2"
                                                                             : (fan_to_send == daikin::DaikinFanMode::Speed3)   ? "Speed3"
                                                                             : (fan_to_send == daikin::DaikinFanMode::Speed4)   ? "Speed4"
                                                                             : (fan_to_send == daikin::DaikinFanMode::Speed5)   ? "Speed5"
                                                                                                                                : "Unknown";

    logInfoP("Sending D1: power=%s, mode=%s('%c'), temp=%.1f°C, fan=%s('%c')",
             power_to_send ? "ON" : "OFF", modeStr, payload[1], temp_to_send, fanStr, payload[3]);

    std::string cmd("D1");
    sent_climate_dirty_ = climate_dirty_;
    sent_climate_generation_ = climate_generation_;
    d1_awaiting_ack_ = true;
    climate_confirmation_started_ = millis();
    climate_readback_seen_ = false;
    climate_outcome_=WriteOutcome::AwaitingAck;
    if (!serial_->send_frame(cmd, payload.data(), 4)) {pending_.activate_climate=climate_dirty_!=0;return;}

    // Store payload for deduplication
    std::copy(payload.begin(), payload.begin() + 4, last_sent_d1_payload_.begin());

    // Record command time for non-blocking cooldown
    last_command_time_ = millis();
    last_send_time_ = millis();

    // v0 stability: Enter post-write settle instead of simple cooldown
    handleWriteCommandSent(LastWriteCommand::D1_ModePower);

    // Device state changes only through F1/RG readback, never on transmission.

    logInfoP("D1 sent, entering post-write settle for %dms", getSettlePeriodForCommand(LastWriteCommand::D1_ModePower));

    // Clear explicit OFF flag after command processing
    if (pending_explicit_off_)
    {
        pending_explicit_off_ = false;
    }
    // Clear explicit fan flag after command processing
    if (pending_explicit_fan_)
    {
        pending_explicit_fan_ = false;
    }
    pending_.activate_climate = false;
    variant_pending_=false;
}



void DaikinDriver::sendSwingCommand()
{
    if ((!pending_.activate_swing_mode && !humidity_pending_) || !d5_.valid) return;
    if (pending_.activate_swing_mode && support_.f2_known && !support_.swing) {
        pending_.activate_swing_mode = false;
        if (!humidity_pending_) return;
    }
    const bool v = swing_dirty_&1 ? pending_.climate.swing_v==daikin::Swing::Vertical : d5_.confirmed[0]=='1' || d5_.confirmed[0]=='7';
    const bool h = swing_dirty_&2 ? pending_.climate.swing_h==daikin::Swing::Horizontal : d5_.confirmed[0]=='2' || d5_.confirmed[0]=='7';
    PayloadBuffer payload = d5_.confirmed;
    if (pending_.activate_swing_mode) {
        payload[0] = v && h ? '7' : v ? '1' : h ? '2' : '0';
        payload[1] = v || h ? '?' : '0';
    }
    if (humidity_pending_) {
        if (!encodeHumidity(requested_humidity_mode_,requested_humidity_)) {humidity_pending_=false;return;}
        payload[2] = requested_humidity_;
    }
    payload[3] = '0'; // request format differs from G5 response byte 3
    // Preserve the humidity field; response-only reserved bytes are not echoed.
    if (sendCombined("D5", payload)) { pending_.activate_swing_mode = false; humidity_pending_ = false; swing_dirty_=0; }
}

void DaikinDriver::sendPowerfulCommand()
{
    if (pending_.activate_powerful) sendSpecialModesCommand();
}

void DaikinDriver::sendEconoCommand()
{
    if ((!pending_.activate_econo && !demand_pending_) || !d7_.valid) return;
    if (pending_.activate_econo && pending_.climate.econo && state_.powerful) return;
    if (!econoControlAvailable() && !econo_probe_pending_) pending_.activate_econo = false;
    if (!pending_.activate_econo && !demand_pending_) return;
    PayloadBuffer payload = d7_.confirmed;
    payload[2] = payload[3] = '0';
    if (pending_.activate_econo) payload[1] = pending_.climate.econo ? '2' : '0';
    if (demand_pending_) payload[0] = 0x30 + (100 - requested_demand_);
    if (sendCombined("D7", payload)) {
        pending_.activate_econo = false;
        demand_pending_ = false;
        econo_probe_pending_ = false;
    }
}

void DaikinDriver::sendQuietCommand()
{
    if (pending_.activate_quiet) sendSpecialModesCommand();
}

void DaikinDriver::sendSensorCommand()
{
    if (pending_.activate_sensor) sendSpecialModesCommand();
}

void DaikinDriver::sendLedCommand()
{
    if (pending_.activate_led) sendSpecialModesCommand();
}

void DaikinDriver::sendStreamerCommand()
{
    if (pending_.activate_streamer) sendSpecialModesCommand();
}

bool DaikinDriver::sendCombined(std::string_view command,const PayloadBuffer& payload)
{
    auto* p=combinedFor(command);
    if(!p || !serial_ || serial_->busy() || diagnostic_active_ || isInPostWriteSettle() || !p->valid || p->awaiting_readback) return false;
    const size_t size=command=="D3"?3:4;
    for(size_t i=0;i<size;i++) if(daikin::s21::forbidden(payload[i])) return false;
    if(p->rejected_valid && payload==p->rejected && millis()-p->last_rejection_ms<timing_.reprobe_ms) return true; // report rejection, never replay it later
    p->mask.fill(0);
    for(size_t i=0;i<size;i++) {
        if((command=="D5" && i==3) || (command=="D7" && i>=2) || (command=="DR" && i>=1) || (command=="FB" && i>=1)) continue;
        if(payload[i]!=p->confirmed[i]) p->mask[i]=command=="D6" ? payload[i]^p->confirmed[i] : 0xFF;
    }
    bool changed=false;for(auto m:p->mask) changed|=m!=0;
    if(!changed) return true; // already confirmed; do not manufacture write capability
    p->requested=payload;p->outcome=WriteOutcome::AwaitingAck;p->sent_ms=millis();p->awaiting_readback=false;p->readback_seen=false;
    outstanding_write_=command;
    logInfoP("S21 write %.*s payload: %s",int(command.size()),command.data(),toHexString(std::vector<uint8_t>(payload.begin(),payload.begin()+size)).c_str());
    if(!serial_->send_frame(command,payload.data(),size,command=="FB")) {p->outcome=WriteOutcome::Error;outstanding_write_={};return false;}
    p->valid=false;last_send_time_=millis();
    handleWriteCommandSent(command=="D5"?LastWriteCommand::D5_Swing:LastWriteCommand::Other);
    return true;
}

void DaikinDriver::sendSpecialModesCommand()
{
    if (!d6_.valid) return; // read before modifying shared fields
    if (state_.econo && pending_.activate_powerful && pending_.climate.powerful) return;
    if (support_.special_features_known) {
        if (!support_.powerful) pending_.activate_powerful = false;
        if (!support_.streamer) pending_.activate_streamer = false;
    }
    const bool write_led = pending_.activate_led && support_.sensor_hardware_known;
    if (!(pending_.activate_powerful || pending_.activate_quiet || pending_.activate_comfort ||
          pending_.activate_streamer || pending_.activate_sensor || write_led)) return;
    PayloadBuffer payload = d6_.confirmed;
    auto update_bit = [&](size_t byte, uint8_t mask, bool on) {
        payload[byte] = (payload[byte] & ~mask) | (on ? mask : 0);
    };
    if (pending_.activate_powerful) update_bit(0, 0x02, pending_.climate.powerful);
    if (pending_.activate_quiet) update_bit(0, 0x80, pending_.climate.quiet);
    if (pending_.activate_comfort) update_bit(0, 0x40, pending_.comfort);
    if (pending_.activate_streamer) update_bit(1, 0x80, pending_.climate.streamer);
    if (pending_.activate_sensor && support_.sensor_hardware) update_bit(3, 0x08, pending_.climate.sensor);
    if (write_led) {
        if (support_.sensor_hardware) update_bit(3, 0x04, pending_.climate.led);
        else payload[3] = (payload[3] & ~0x0C) |
            (pending_.led == daikin::LedState::On ? 0x04 : pending_.led == daikin::LedState::Dark ? 0x08 : 0x0C);
    }
    if (!sendCombined("D6", payload)) return;
    pending_.activate_powerful = pending_.activate_quiet = pending_.activate_comfort = false;
    pending_.activate_streamer = pending_.activate_sensor = false;
    if (write_led) pending_.activate_led = false;
}

void DaikinDriver::setComfort(bool on)
{
    pending_.comfort = on;
    pending_.activate_comfort = true;
    sendSpecialModesCommand();
}

void DaikinDriver::setIntelligentEye(bool on)
{
    if (!support_.sensor_hardware) return;
    pending_.climate.sensor = on;
    pending_.activate_sensor = true;
    sendSpecialModesCommand();
}

// === S21 Response Handlers ===
void DaikinDriver::handle_f1_response(uint8_t* data, size_t data_size)
{
    if (data_size < 4)
    {
        logErrorP("F1 response too short: %zu bytes", data_size);
        return;
    }
    if (!s21_valid_response("F1",data,data_size)) return;
    stats_.frames_ok++; // Count successful response

    // F1/G1 contains: power(ASCII), mode(ASCII), target_temp(encoded), fan(encoded)
    bool changed = false; // Track if any parameter changed from remote control
    bool old_power = state_.power;
    daikin::Mode old_mode = state_.mode;
    float old_targetC = state_.targetC;

    state_.basicValid = true;
    state_.setpointRaw = data[2]; state_.setpointRawValid = true;
    state_.targetTemperatureValid = data[2] >= 0x30 && data[2] <= 0x7F;
    using Variant=daikin::ClimateVariant;
    state_.climateVariantValid=true;
    state_.climateVariant=data[1]=='2'?Variant::SararaDry:data[1]=='4'?(data[2]==0xC0?Variant::UruruHumidify:Variant::Heat):data[1]=='3'?Variant::Cool:data[1]=='6'?Variant::FanOnly:Variant::Auto;
    // Power: ASCII '1' = on, '0' = off
    state_.power = (data[0] == '1');

    // Mode: Direct S21 protocol values (from official S21 Protocol wiki)
    // S21 Protocol: "1"=Auto, "2"=Dry, "3"=Cool, "4"=Heat, "6"=Fan
    // Additional: "0"=Auto,Cooling, "7"=Auto,Heating (also treated as Auto)
    char mode_char = data[1];

    logDebugP("F1: raw_byte=0x%02X ('%c'), s21_mode='%c'",
              data[1], (data[1] >= 32 && data[1] < 127) ? data[1] : '?', mode_char);

    // Direct S21 protocol mode mapping
    switch (mode_char)
    {
        case '1': state_.mode = daikin::Mode::Auto; break;
        case '2': state_.mode = daikin::Mode::Dry; break;
        case '3': state_.mode = daikin::Mode::Cool; break;
        case '4': state_.mode = daikin::Mode::Heat; break;
        case '6': state_.mode = daikin::Mode::FanOnly; break;
        case '0': // Auto, Cooling (according to wiki, treated same as '1')
        case '7': // Auto, Heating (according to wiki, treated same as '1')
            state_.mode = daikin::Mode::Auto;
            break;
        default:
            logInfoP("F1: Unknown S21 mode='%c' (0x%02X), defaulting to Auto", mode_char, data[1]);
            state_.mode = daikin::Mode::Auto;
            break;
    }

    // Extract Auto bias information from S21 mode byte '0'/'7'
    state_.autoBias = (mode_char == '0') ? daikin::AutoBias::Cooling : (mode_char == '7') ? daikin::AutoBias::Heating
                                                                                          : daikin::AutoBias::Unknown;

    // Detect mode changes from remote control
    if (old_mode != state_.mode)
    {
        changed = true;
    }

    // Temperature: Faikin's s21_decode_target_temp() - only for heat/cool/auto modes
    if (state_.mode == daikin::Mode::Heat || state_.mode == daikin::Mode::Cool || state_.mode == daikin::Mode::Auto)
    {
        // s21_decode_target_temp: 18.0 + 0.5 * ((signed) v - '@')
        if (data[2] >= 0x30 && data[2] <= 0x7F)
            state_.targetC = 18.0f + 0.5f * (int(data[2]) - '@');

        // Detect temperature changes from remote control (with tolerance for float comparison)
        if (abs(old_targetC - state_.targetC) > 0.1f)
        {
            changed = true;
        }
    }
    else if (state_.mode == daikin::Mode::Dry && data[2] == 0x80)
    {
        // Dry mode: temperature is N/A (0x80), keep existing temperature
        logDebugP("F1: Dry mode, temperature N/A (0x80), keeping existing target %.1f°C", state_.targetC);
    }
    // Fan mode: no temperature setpoint, keep existing

    // Decode fan nibble from F1 for diagnostics (and as fallback when RG is unavailable).
    daikin::DaikinFanMode f1_fan = daikin::DaikinFanMode::Auto;
    bool f1_fan_known = true;
    switch (data[3])
    {
        case 'A':
        case '0': f1_fan = daikin::DaikinFanMode::Auto; break;
        case 'B': f1_fan = daikin::DaikinFanMode::Silent; break;
        case '3': f1_fan = daikin::DaikinFanMode::Speed1; break;
        case '4': f1_fan = daikin::DaikinFanMode::Speed2; break;
        case '5': f1_fan = daikin::DaikinFanMode::Speed3; break;
        case '6': f1_fan = daikin::DaikinFanMode::Speed4; break;
        case '7': f1_fan = daikin::DaikinFanMode::Speed5; break;
        default:
            f1_fan_known = false;
            break;
    }

    state_.fanValid = support_.fan_mode_query || data[3] != 'A';
    // Fan: Only if RG (fan query) doesn't work - Faikin logic
    if (!support_.fan_mode_query)
    {
        daikin::DaikinFanMode old_fan = state_.fan;

        if (f1_fan_known)
        {
            state_.fan = f1_fan;
        }
        else
        {
            logInfoP("F1: Unknown fan mode ASCII '%c' (0x%02X), defaulting to Auto", data[3], data[3]);
            state_.fan = daikin::DaikinFanMode::Auto;
        }

        // Detect fan mode changes from remote control
        if (old_fan != state_.fan)
        {
            changed = true;
        }
    }

    // Get mode name for better UX (avoid confusion with S21 protocol numbers)
    const char* modeStr = "Unknown";
    switch (state_.mode)
    {
        case daikin::Mode::Auto: modeStr = "Auto"; break;
        case daikin::Mode::Cool: modeStr = "Cool"; break;
        case daikin::Mode::Dry: modeStr = "Dry"; break;
        case daikin::Mode::Heat: modeStr = "Heat"; break;
        case daikin::Mode::FanOnly: modeStr = "Fan"; break;
        default: modeStr = "Off"; break;
    }

    auto fanToStr = [](daikin::DaikinFanMode f) {
        switch (f)
        {
            case daikin::DaikinFanMode::Auto: return "Auto";
            case daikin::DaikinFanMode::Silent: return "Silent";
            case daikin::DaikinFanMode::Speed1: return "1";
            case daikin::DaikinFanMode::Speed2: return "2";
            case daikin::DaikinFanMode::Speed3: return "3";
            case daikin::DaikinFanMode::Speed4: return "4";
            case daikin::DaikinFanMode::Speed5: return "5";
            default: return "?";
        }
    };

    // Add AutoBias info for Auto modes (S21 mode '0'/'7')
    const char* biasInfo = "";
    if (state_.mode == daikin::Mode::Auto)
    {
        biasInfo = (state_.autoBias == daikin::AutoBias::Cooling) ? " [Auto-Cool]" : (state_.autoBias == daikin::AutoBias::Heating) ? " [Auto-Heat]"
                                                                                                                                    : " [Auto]";
    }

    // With RG support, state_.fan may still reflect the previous sample when F1 arrives.
    // Log both values to avoid misleading output and make nibble decoding explicit.
    const char* f1FanStr = f1_fan_known ? fanToStr(f1_fan) : "?";
    logInfoP("F1: power=%s, mode=%s%s, target=%.1f degC, fan=%s (state=%s, ASCII fanNibble: %c->%s)",
             state_.power ? "ON" : "OFF", modeStr, biasInfo, state_.targetC,
             support_.fan_mode_query ? f1FanStr : fanToStr(state_.fan),
             support_.fan_mode_query ? "RG" : "F1",
             (char)data[3], f1FanStr);

    if (state_.power == pending_.climate.power) climate_dirty_ &= ~DirtyPower;
    if (state_.mode == pending_.climate.mode) climate_dirty_ &= ~DirtyMode;
    if (std::abs(state_.targetC - pending_.climate.targetC) < 0.1f) climate_dirty_ &= ~DirtyTemperature;
    if (!support_.fan_mode_query && state_.fan == pending_.climate.fan_mode) climate_dirty_ &= ~DirtyFan;

    // G1 can alias Quiet to Auto. Only RG may confirm that fan request.
    const bool quiet_alias = pending_.climate.fan_mode==daikin::DaikinFanMode::Silent && data[3]=='A';
    confirmClimateFields(DirtyPower | DirtyMode | DirtyTemperature |
        (!support_.fan_mode_query && !quiet_alias ? DirtyFan : 0));

    // Mark F1 sample as seen for publish gating
    sample_seen_mask_ |= SEEN_F1;
    if (gate_publish_until_full_sample_ &&
        (sample_seen_mask_ & (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA)) == (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA))
    {
        logDebugP("F1: full sample reached -> immediate publish");
        publishState();
    }

    // Check for any state changes
    if (changed || old_power != state_.power)
    {
        if (old_power != state_.power)
        {
            logInfoP("F1: AC power state CHANGED: %s -> %s",
                     old_power ? "ON" : "OFF", state_.power ? "ON" : "OFF");
        }
        publishState();
    }
}

void DaikinDriver::handle_f2_response(uint8_t* data, size_t data_size)
{
    if(!daikin::s21::validResponse("F2",data,data_size)) return;
    if (data_size != 4) return;
    stats_.frames_ok++;

    support_.f2_known = true;
    // De-shield F2 capability bytes (S21 protocol uses 0x30 shield bits on byte 0 only)
    uint8_t b0 = data[0] & ~0x30; // Remove shield bits from byte 0 (bits 4,5 are shields)
    uint8_t b3 = data[3];         // Byte 3: keep all bits - bit1=Humidify, bit4=extended humidity

    // Extract humidity capabilities correctly from byte 3
    bool hasHumidifyMode = (b3 & 0x02) != 0;
    support_.humidify_operation=hasHumidifyMode;   // bit 1 - Humidify operation mode available
    bool hasExtraHumdModes = (b3 & 0x10) != 0; // bit 4 - humidity settings in additional modes
    humidity_modes_ = hasHumidifyMode ? (hasExtraHumdModes ? 0xB7 : 0xA5) : (hasExtraHumdModes ? 0x92 : 0);
    uint8_t b3_view = b3 & 0x7F;               // cosmetic: mask always-1 bit7 for cleaner logs

    // Humidity capability matrix (from S21 wiki):
    // bit4=0, bit1=0 → no humidity settings (s_humd=0)
    // bit4=1, bit1=0 → settings in Cool+Dry (s_humd=0x92)
    // bit4=0, bit1=1 → settings in Auto+Heat+Humidify (s_humd=0xA5)
    // bit4=1, bit1=1 → settings in Auto+Cool+Heat+Dry+Humidify (s_humd=0xB7)

    // Use de-shielded bytes for all feature flags to prevent false positives/negatives
    support_.swing = (b0 & 0x04) != 0;    // bit 2 - swing (any kind) available
    support_.humidity = humidity_modes_ != 0;  // bit 1 of byte3 - Humidify operation mode available
    support_.horizontal_swing = (b0 & 0x08) != 0;
    if(support_.fan_capability_known && !support_.fan) support_.swing=support_.horizontal_swing=false;
    if(support_.simple_humidify) humidity_modes_=0x10;
    // F2 bits 4/5 are shields. Fan and Intelligent Eye belong to FK;
    // Streamer/Powerful/Econo availability belongs to FU00.

    logDebugP("F2 capability: swing=%d, humidity=%d (humidify_mode=%d, extra_modes=%d), fan=%d, sensor=%d, streamer=%d, led=%d",
              support_.swing, support_.humidity, hasHumidifyMode, hasExtraHumdModes,
              support_.fan, support_.sensor, support_.streamer, support_.led);
    logDebugP("F2: bytes [0x%02X, -, -, 0x%02X] de-shielded [0x%02X, -, -, 0x%02X]",
              data[0], data[3], b0, b3_view);
}

bool DaikinDriver::configureVaneAngleScale(uint8_t divisor)
{
    if (divisor != 1 && divisor != 10) return false;
    vane_angle_divisor_ = divisor;
    return true;
}

void DaikinDriver::handle_vane_angle(bool requested, uint8_t* data, size_t size)
{
    bool& valid = requested ? state_.vaneAngleSetpointValid : state_.vaneAngleValid;
    bool& degrees_valid = requested ? state_.vaneAngleSetpointDegreesValid : state_.vaneAngleDegreesValid;
    valid = s21_valid_decimal(data,size,true);
    degrees_valid = valid && vane_angle_divisor_ != 0;
    if (!valid) return;
    const int16_t value = s21_decode_int_sensor(data);
    if (requested) state_.swingVerticalAngleSetpoint = value;
    else state_.swingVerticalAngle = value;
    if (degrees_valid) {
        if (requested) state_.vaneAngleSetpointDegrees = float(value) / vane_angle_divisor_;
        else state_.vaneAngleDegrees = float(value) / vane_angle_divisor_;
    }
}

void DaikinDriver::handle_firmware_response(uint8_t* data, size_t size)
{
    if(!daikin::s21::validResponse("VS000M",data,size)) return;
    if (size != 14) return;
    for (size_t i=0;i<8;i++) if (data[i]<0x20 || data[i]>0x7E) return;
    firmware_version_.assign(data,data+8);
    std::reverse(firmware_version_.begin(),firmware_version_.end());
}

void DaikinDriver::handle_fan_rpm(bool requested, uint8_t* data, size_t size)
{
    bool& valid = requested ? state_.fanRpmSetpointValid : state_.fanRpmValid;
    valid = s21_valid_decimal(data,size,false);
    if (!valid) return;
    const uint16_t rpm = ((data[0]-'0') + (data[1]-'0')*10 + (data[2]-'0')*100) * 10;
    if (requested) state_.fanRpmSetpoint = rpm;
    else state_.fanRpm = rpm;
}

void DaikinDriver::handle_ri_response(uint8_t* data, size_t size)
{
    if (!s21_valid_decimal(data,size,true)) { state_.indoorHeatExchangerValid = false; return; }
    state_.indoorHeatExchangerC = s21_decode_float_sensor(data);
    state_.indoorHeatExchangerValid = true;
}

void DaikinDriver::handle_supplemental_sensor(std::string_view command, uint8_t* data, size_t size)
{
    if (command == "RF") {
        if (size != 2 || (data[0]!='0' && data[0]!='1' && data[0]!='2' && data[0]!='7') ||
            (data[1]!='0' && data[1]!='F')) return;
        state_.supplementalSwing = data[0]=='1' ? daikin::Swing::Vertical : data[0]=='2' ? daikin::Swing::Horizontal :
                                   data[0]=='7' ? daikin::Swing::Both : daikin::Swing::Off;
        state_.supplementalSwingValid = true;
    } else if (s21_valid_decimal(data,size,false)) {
        const uint16_t value=(data[0]-'0') + (data[1]-'0')*10 + (data[2]-'0')*100;
        if (value > 144) return;
        if (command=="RD") { state_.onTimerPeriods=value; state_.onTimerPeriodsValid=true; }
        if (command=="RE") { state_.offTimerPeriods=value; state_.offTimerPeriodsValid=true; }
    }
}

void DaikinDriver::handle_status_sensor(std::string_view command, uint8_t* data, size_t size)
{
    if (command == "RA" && size == 1 && (data[0] == '0' || data[0] == '1')) {
        state_.independentPower = data[0] == '1'; state_.independentPowerValid = true;
    } else if (command == "RB" && size == 1 && (data[0] >= '0' && data[0] <= '7') && data[0] != '5') {
        state_.independentMode = data[0] - '0'; state_.independentModeValid = true;
    } else if (command == "RC" && size == 4) {
        for (size_t i=0;i<3;i++) if (data[i] < '0' || data[i] > '9') return;
        if (data[3] != '+' && data[3] != '-') return;
        state_.coarseTargetC = s21_decode_int_sensor(data); // whole degrees, not C/10
        state_.coarseTargetValid = true;
    }
}

void DaikinDriver::handle_fr_response(uint8_t* data, size_t size)
{
    if(!daikin::s21::validResponse("FR",data,size)) return;
    if (size != 4 || ((data[0] < '0' || data[0] > '5') && data[0] != '?')) return;
    confirmCombined("DR",data);
    std::copy_n(data, 4, dr_.confirmed.begin());
    dr_.valid = true;
    state_.verticalPositionCodeValid = true;
    state_.verticalPositionValid = data[0] != '?';
    state_.verticalPosition = data[0] == '?' ? 6 : data[0] - '0';
    if (state_.verticalPositionValid) {
        state_.verticalPosition = data[0] - '0';
        if (requested_vertical_position_ > 0 && state_.verticalPosition == requested_vertical_position_ &&
            dr_.outcome == WriteOutcome::Confirmed) state_.verticalPositionVerified = true;
    }
}

void DaikinDriver::handle_fb_response(uint8_t* data, size_t size)
{
    if(!daikin::s21::validResponse("FB",data,size)) return;
    if (size != 4 || (data[0] != '0' && data[0] != '1')) return;
    confirmCombined("FB",data);
    std::copy_n(data,4,fb_.confirmed.begin());fb_.valid=true;
    state_.timerLampValid = true;
    state_.timerLamp = data[0] == '1'; // orange timer indicator, independent of D6 LEDs
    publishTimerStatus();
}

void DaikinDriver::handle_f4_response(uint8_t* data, size_t size)
{
    if (size != 4) return;
    // Read-clears at the unit: this is the latest sample, never a latched error.
    state_.serialErrorValid = true;
    state_.serialErrorByte = data[2];
    state_.serialError = (data[2] & 0x20) != 0;
    if (state_.serialError) ++state_.serialErrorSamples;
}

void DaikinDriver::handle_f3_response(uint8_t* data, size_t size)
{
    if(!daikin::s21::validResponse("F3",data,size)) return;
    if (size != 4 || data[0] < '0' || data[0] > '3') return;
    const uint8_t enabled = data[0] - '0';
    for (size_t i=1;i<=2;i++) {
        if (enabled & (1 << (i-1))) {
            if (data[i] < 0x31 || data[i] > 0xC0) return;
        } else if (data[i] != 0xFE && (data[i] < 0x30 || data[i] > 0xC0)) return;
    }
    for (size_t i=1;i<=2;i++) if (!(enabled & (1<<(i-1)))) timer_disabled_bytes_[i-1] = data[i];
    confirmCombined("D3",data);
    std::copy_n(data,4,d3_.confirmed.begin()); d3_.valid=true;
    state_.timersValid=true; state_.onTimerEnabled=enabled&1; state_.offTimerEnabled=enabled&2;
    state_.onTimerConfiguredMinutes=state_.onTimerEnabled ? (data[1]-0x30)*10 : 0;
    state_.offTimerConfiguredMinutes=state_.offTimerEnabled ? (data[2]-0x30)*10 : 0;
    if (state_.onTimerConfiguredMinutes) last_timer_minutes_[0]=state_.onTimerConfiguredMinutes;
    if (state_.offTimerConfiguredMinutes) last_timer_minutes_[1]=state_.offTimerConfiguredMinutes;
    publishTimerStatus();
    // Byte 3 remains unknown: never infer Powerful or wall-clock expiry from it.
}

bool DaikinDriver::configureTimerGranularity(uint16_t minutes, uint16_t maximum)
{
    if (!minutes || minutes%10 || maximum>1440 || maximum<minutes || maximum%minutes) return false;
    timer_step_=minutes; timer_max_=maximum; publishTimerStatus();return true;
}

bool DaikinDriver::setTimers(uint16_t on, uint16_t off)
{
    if (!serial_ || !timer_step_) return rejectTimerRequest(false,AirConditionWriteResult::Unavailable);
    if (on>timer_max_ || off>timer_max_ || on%timer_step_ || off%timer_step_) return rejectTimerRequest(false,AirConditionWriteResult::InvalidRequest);
    if ((!on && !timer_disabled_bytes_[0]) || (!off && !timer_disabled_bytes_[1])) return rejectTimerRequest(false,AirConditionWriteResult::Unavailable);
    timer_request_error_=AirConditionWriteResult::None;
    requested_on_minutes_=on;requested_off_minutes_=off;timer_dirty_=3;timer_pending_=true;
    if (on) last_timer_minutes_[0]=on;
    if (off) last_timer_minutes_[1]=off;
    sendTimerCommand();publishTimerStatus();return true;
}

bool DaikinDriver::setTimerDuration(AirConditionTimer timer,uint16_t minutes)
{
    const uint8_t index=uint8_t(timer);
    if (index>1) return rejectTimerRequest(false,AirConditionWriteResult::InvalidRequest);
    if (!serial_ || !d3_.read_supported || !timer_step_ || (!minutes && !timer_disabled_bytes_[index])) return rejectTimerRequest(false,AirConditionWriteResult::Unavailable);
    if (minutes>timer_max_ || minutes%timer_step_) return rejectTimerRequest(false,AirConditionWriteResult::InvalidRequest);
    timer_request_error_=AirConditionWriteResult::None;
    if (index==0) requested_on_minutes_=minutes;else requested_off_minutes_=minutes;
    if (minutes) last_timer_minutes_[index]=minutes;
    timer_dirty_|=1U<<index;timer_pending_=true;
    sendTimerCommand();publishTimerStatus();return true;
}

bool DaikinDriver::setTimerEnabled(AirConditionTimer timer,bool enabled)
{
    const uint8_t index=uint8_t(timer);
    if (index>1) return rejectTimerRequest(false,AirConditionWriteResult::InvalidRequest);
    if (!enabled) return setTimerDuration(timer,0);
    if (!last_timer_minutes_[index]) return rejectTimerRequest(false,AirConditionWriteResult::InvalidRequest); // never invent a delay on enable
    return setTimerDuration(timer,last_timer_minutes_[index]);
}

void DaikinDriver::sendTimerCommand()
{
    if (!timer_pending_ || !d3_.valid) return;
    const uint16_t on=timer_dirty_&1?requested_on_minutes_:state_.onTimerConfiguredMinutes;
    const uint16_t off=timer_dirty_&2?requested_off_minutes_:state_.offTimerConfiguredMinutes;
    // Only modified axes need the profile check. Preserve a remote's independently
    // configured other timer even when it is outside this installer's profile.
    if ((!on && !timer_disabled_bytes_[0]) || (!off && !timer_disabled_bytes_[1])) return;
    PayloadBuffer payload=d3_.confirmed;
    payload[0]='0'+(on?1:0)+(off?2:0);
    payload[1]=on?0x30+on/10:timer_disabled_bytes_[0];
    payload[2]=off?0x30+off/10:timer_disabled_bytes_[1];
    if (sendCombined("D3",payload)) {timer_pending_=false;timer_dirty_=0;}
}

AirConditionTimerStatus DaikinDriver::timerStatus() const
{
    AirConditionTimerStatus result;
    result.timersValid=state_.timersValid;result.lampValid=state_.timerLampValid;
    result.onEnabled=state_.onTimerEnabled;result.offEnabled=state_.offTimerEnabled;result.lamp=state_.timerLamp;
    result.onMinutes=state_.onTimerConfiguredMinutes;result.offMinutes=state_.offTimerConfiguredMinutes;
    result.stepMinutes=timer_step_;result.maximumMinutes=timer_max_;
    result.available=(d3_.read_supported?1:0) | (fb_.read_supported?2:0) | (timer_step_?4:0) |
        (timer_disabled_bytes_[0]?8:0) | (timer_disabled_bytes_[1]?16:0);
    // Shared result enum inserts Queued before the transport/readback outcomes.
    const auto convert=[](WriteOutcome value) {return value==WriteOutcome::None?AirConditionWriteResult::None:static_cast<AirConditionWriteResult>(uint8_t(value)+1);};
    result.timerResult=timer_request_error_!=AirConditionWriteResult::None?timer_request_error_:timer_pending_?AirConditionWriteResult::Queued:convert(d3_.outcome);
    result.lampResult=lamp_request_error_!=AirConditionWriteResult::None?lamp_request_error_:timer_lamp_pending_?AirConditionWriteResult::Queued:convert(fb_.outcome);
    return result;
}

void DaikinDriver::publishTimerStatus() {statusFeedback.updateTimerStatus(timerStatus());}
bool DaikinDriver::rejectTimerRequest(bool lamp,AirConditionWriteResult reason)
{
    (lamp?lamp_request_error_:timer_request_error_)=reason;publishTimerStatus();return false;
}

void DaikinDriver::handle_f5_response(uint8_t* data, size_t data_size)
{
    if(!daikin::s21::validResponse("F5",data,data_size)) return;
    if (data_size < 1)
    {
        logErrorP("F5 response empty");
        return;
    }
    if (data_size < 4)
    {
        logErrorP("F5 response too short: %zu bytes (expected 4-byte G5 response)", data_size);
        return;
    }
    if (data[0] != '0' && data[0] != '1' && data[0] != '2' && data[0] != '7') return;
    confirmCombined("D5",data);
    std::copy_n(data, 4, d5_.confirmed.begin());
    d5_.valid = true;
    state_.swingValid = true;
    stats_.frames_ok++;

    logInfoP("F5: raw G5 response [%02X %02X %02X %02X] (4 bytes: swing1, swing2, humidity, unknown)",
             data_size >= 1 ? data[0] : 0,
             data_size >= 2 ? data[1] : 0,
             data_size >= 3 ? data[2] : 0,
             data_size >= 4 ? data[3] : 0);

    // F5 response contains swing or humidity data
    if (support_.swing)
    {
        daikin::Swing swing = data[0] == '1' ? daikin::Swing::Vertical :
                              data[0] == '2' ? daikin::Swing::Horizontal :
                              data[0] == '7' ? daikin::Swing::Both : daikin::Swing::Off;
        state_.swing = swing;
    }

    state_.humidityRaw = data[2];
    state_.humidityMode = daikin::HumidityMode::Unknown;
    const bool heating = state_.mode == daikin::Mode::Heat || state_.autoBias == daikin::AutoBias::Heating;
    const bool cooling = state_.mode == daikin::Mode::Cool || state_.mode == daikin::Mode::Dry ||
                         state_.autoBias == daikin::AutoBias::Cooling;
    if (data[2] == '0') state_.humidityMode = daikin::HumidityMode::Off;
    else if (data[2] == 0xFF && (heating || cooling)) state_.humidityMode = daikin::HumidityMode::Continuous;
    else if (heating && data[2] >= 0x38 && data[2] <= 0x3A)
        state_.humidityMode = data[2] == 0x38 ? daikin::HumidityMode::Low : data[2] == 0x39 ? daikin::HumidityMode::Standard : daikin::HumidityMode::High;
    else if (cooling && data[2] >= 0x3A && data[2] <= 0x3C)
        state_.humidityMode = data[2] == 0x3A ? daikin::HumidityMode::Low : data[2] == 0x3B ? daikin::HumidityMode::Standard : daikin::HumidityMode::High;
    state_.humidityModeValid = state_.humidityMode != daikin::HumidityMode::Unknown;
    using V=daikin::ClimateVariant;
    state_.climateVariantValid=state_.basicValid;
    state_.climateVariant=state_.mode==daikin::Mode::Dry?V::SararaDry:state_.mode==daikin::Mode::FanOnly?V::FanOnly:
        state_.mode==daikin::Mode::Heat?(state_.setpointRaw==0xC0?V::UruruHumidify:data[2]!='0'?V::HumidHeating:V::Heat):
        state_.mode==daikin::Mode::Cool?(data[2]!='0'?V::DryCooling:V::Cool):V::Auto;

}

void DaikinDriver::handle_f6_response(uint8_t* data, size_t data_size)
{
    if(!daikin::s21::validResponse("F6",data,data_size)) return;
    if (data_size != 4)
    {
        logErrorP("F6 response too short: %zu bytes", data_size);
        return;
    }
    for (size_t i=0; i<4; ++i) if ((data[i] & 0x30) != 0x30) return;
    stats_.frames_ok++;

    confirmCombined("D6",data);
    std::copy_n(data, 4, d6_.confirmed.begin());
    d6_.valid = true;
    state_.specialModesValid = true;
    support_.f6_special_modes = true; // Mark F6 as supported since we got a successful response

    state_.powerful = (!support_.special_features_known || support_.powerful) && (data[0] & 0x02) != 0; // payload[0] & 0x02 = powerful
    state_.comfort = (data[0] & 0x40) != 0;  // payload[0] & 0x40 = comfort
    state_.quiet = (data[0] & 0x80) != 0;    // payload[0] & 0x80 = quiet
    state_.streamer = (!support_.special_features_known || support_.streamer) && (data[1] & 0x80) != 0; // payload[1] & 0x80 = streamer

    // Only set sensor state if hardware is available (from FK detection)
    if (support_.sensor_hardware)
    {
        state_.sensor = (data[3] & 0x08) != 0; // F6 byte 3 bit 3: "Sensor" mode active
    }
    else
    {
        state_.sensor = false; // No sensor hardware, so sensor mode is always false
    }

    // Bits 2 and 3 overlap with sensor controls; distinguish the hardware variants.
    state_.sensorLed = support_.sensor_hardware && (data[3] & 0x04);
    if (support_.sensor_hardware) {
        state_.ledState = state_.sensorLed ? daikin::LedState::On : daikin::LedState::Off;
    } else if (support_.sensor_hardware_known) {
        switch (data[3] & 0x0C) {
            case 0x0C: state_.ledState = daikin::LedState::Off; break;
            case 0x08: state_.ledState = daikin::LedState::Dark; break;
            case 0x04: state_.ledState = daikin::LedState::On; break;
            default: state_.ledState = daikin::LedState::Unknown; break;
        }
    }
    if (!support_.sensor_hardware_known) state_.ledState = daikin::LedState::Unknown;
    if (state_.ledState != daikin::LedState::Unknown)
        state_.led = state_.ledState == daikin::LedState::On;

    logInfoP("F6: powerful=%d, comfort=%d, quiet=%d, streamer=%d, sensor=%d (hw_avail=%d), led=%d (raw: %02X %02X %02X %02X)",
             state_.powerful, state_.comfort, state_.quiet, state_.streamer, state_.sensor, support_.sensor_hardware, state_.led,
             data[0], data[1], data[2], data[3]);
}

void DaikinDriver::handle_f7_response(uint8_t* data, size_t data_size)
{
    if(!daikin::s21::validResponse("F7",data,data_size)) return;
    if (data_size != 4)
    {
        logErrorP("F7 response too short: %zu bytes", data_size);
        return;
    }
    if (data[0] < 0x30 || data[0] > 0x94 || (data[1] != '0' && data[1] != '2')) return;
    confirmCombined("D7",data);
    std::copy_n(data, 4, d7_.confirmed.begin());
    d7_.valid = true;
    state_.demandValid = true;
    stats_.frames_ok++;

    state_.demandPercentageValid=data[0]!='1';
    if (state_.demandPercentageValid)
        state_.demand_percentage = 100 - (data[0] - 0x30);
    state_.econo = data[1] == '2';
    if (state_.econo) econo_observed_on_=true;

    if (state_.demandPercentageValid)
        logInfoP("F7: demand=%d%%, econo=%d (raw: %c %02X)", state_.demand_percentage, state_.econo, data[0], data[1]);
    else
        logInfoP("F7: demand unavailable (ambiguous 99%% sentinel), econo=%d", state_.econo);
}

void DaikinDriver::handle_f8_response(uint8_t* data, size_t data_size)
{
    if(!daikin::s21::validResponse("F8",data,data_size)) return;
    if (data_size != 4) return;
    unsigned value = 0, multiplier = 1;
    bool padding = false;
    for (size_t i=0; i<4; ++i) {
        if (data[i] == 0) { padding = true; continue; }
        if (padding || data[i] < '0' || data[i] > '9') return;
        value += (data[i] - '0') * multiplier;
        multiplier *= 10;
    }
    if (data[0] != '0' || (value != 0 && value != 10 && value != 20)) return;
    if(!protocol_detection_phase_ && protocol_version_!=daikin::ProtocolUndetected && protocol_version_.major>=3 && value==20) return;
    if(!protocol_detection_phase_ && protocol_version_!=daikin::ProtocolUndetected && protocol_version_.major!=value/10) version_changed_=true;
    protocol_version_ = {static_cast<uint8_t>(value / 10), 0};
    old_protocol_detected_ = value == 0;
    state_.protocol=protocol_version_; state_.protocolVersion=protocol_version_.major;
    stats_.frames_ok++;
}

void DaikinDriver::handle_f9_response(uint8_t* data, size_t size)
{
    if (size != 4) return;
    // Prefer precise RH/Ra/Re samples; FF is unavailable, never a measurement.
    if (!support_.inside_temperature_query) {
        state_.insideTemperatureValid = data[0] != 0xFF;
        if (state_.insideTemperatureValid) state_.homeC = (int(data[0])-0x80)/2.0f;
    }
    if (!support_.outside_temperature_query) {
        state_.outsideTemperatureValid = data[1] != 0xFF;
        if (state_.outsideTemperatureValid) state_.outsideC = (int(data[1])-0x80)/2.0f;
    }
    if (!support_.humidity_query) {
        state_.humidityValid = data[2] >= 0x30 && data[2] <= 0x94;
        if (state_.humidityValid) state_.humidity = data[2]-0x30;
    }
}

void DaikinDriver::handle_metadata(std::string_view command,uint8_t* data,size_t size)
{
    if(size!=4) return;
    auto reversed=[&](){std::string text(data,data+4);std::reverse(text.begin(),text.end());return text;};
    if(command=="FC") identification_.modelCode=reversed();
    else if(command=="M") identification_.legacyModelWire.assign(data,data+4);
    else if(command=="V") identification_.legacyVersionWire.assign(data,data+4);
    else if(command=="FN") identification_.itelc=reversed();
    else if(command=="FS") {
        if(data[0]<'1' || data[0]>'4' || daikin::s21::hex(data[1])<0) return;
        identification_.wifiMode=data[0];identification_.wifiCounter=daikin::s21::hex(data[1]);identification_.wifiValid=true;
    } else if(command=="FT") {
        unsigned value=0,multiplier=1;for(size_t i=0;i<4;i++){if(data[i]<'0'||data[i]>'9') return;value+=(data[i]-'0')*multiplier;multiplier*=10;}
        identification_.ftRawDecimal=value;identification_.ftValid=true;
    }
}

void DaikinDriver::handle_aux_sensor(std::string_view command,uint8_t* data,size_t size)
{
    if(command=="RW") return; // unknown two-byte status: raw query storage only
    if(!daikin::s21::decimal(data,size)) return;
    const auto value=(data[0]-'0')+(data[1]-'0')*10+(data[2]-'0')*100;
    if(command=="Rb" && value<=15) {state_.pull=value;state_.pullValid=true;}
    if(command=="Rm") {state_.rmRaw=value;state_.rmRawValid=true;}
    if(command=="Rt") {state_.rtRaw=value;state_.rtRawValid=true;}
}

void DaikinDriver::handle_fc_response(uint8_t* data,size_t size) { handle_metadata("FC",data,size); }

void DaikinDriver::handle_fg_response(uint8_t* data,size_t size)
{
    if(size!=4) return;
    if(state_.irCounterValid && !std::equal(data,data+4,state_.irCounterRaw.begin())) ++state_.irObservedChanges;
    std::copy_n(data,4,state_.irCounterRaw.begin());state_.irCounterValid=true;
}

void DaikinDriver::handle_fm_response(uint8_t* data, size_t size)
{
    state_.totalEnergyValid = false;
    if (size != 4) return;
    uint32_t raw=0;
    for (size_t i=0;i<4;i++) {
        int nibble=s21_hex_nibble(data[i]); if (nibble<0) return;
        raw |= uint32_t(nibble) << (4*i);
    }
    state_.totalEnergyWh = raw * 100U; // system-wide outdoor energy, not FU04 indoor lifetime
    state_.totalEnergyValid = true;
}

void DaikinDriver::handle_fy00_response(uint8_t* data, size_t data_size)
{
    if(!daikin::s21::validResponse("FY00",data,data_size)) return;
    if (data_size != 4) return;
    for (size_t i=0; i<4; ++i) if (data[i] < '0' || data[i] > '9') return;
    const uint8_t major = (data[3]-'0')*10 + data[2]-'0';
    const uint8_t minor = (data[1]-'0')*10 + data[0]-'0';
    if (major < 3) return;
    if(!protocol_detection_phase_ && protocol_version_!=daikin::ProtocolUndetected && protocol_version_!=daikin::ProtocolVersion{major,minor}) version_changed_=true;
    protocol_version_ = {major, minor};
    state_.protocol=protocol_version_; state_.protocolVersion=major;
    old_protocol_detected_ = false;
    stats_.frames_ok++;
}

void DaikinDriver::handle_fn_response(uint8_t* data,size_t size) { handle_metadata("FN",data,size); }

void DaikinDriver::handle_fp_response(uint8_t* data,size_t size) { handle_metadata("FP",data,size); }

void DaikinDriver::handle_fq_response(uint8_t* data,size_t size) { handle_metadata("FQ",data,size); }

void DaikinDriver::handle_fs_response(uint8_t* data,size_t size) { handle_metadata("FS",data,size); }

void DaikinDriver::handle_ft_response(uint8_t* data,size_t size) { handle_metadata("FT",data,size); }

void DaikinDriver::handle_fk_response(uint8_t* data, size_t data_size)
{
    // FK capability/feature query - optional features, used by Faikin for feature detection
    if (data_size > 0)
    { // Single byte ACK
        if (data_size == 1)
        {
            logInfoP("FK response: ACK (0x%02X)", data[0]);
            stats_.frames_ok++;
            return;
        }
        // Multi-byte capability data
        if (data_size >= 4)
        {
            stats_.frames_ok++;

            // Parse FK capabilities according to S21 protocol spec
            // byte1 bit 3 - "motion detector" AKA "Intelligent eye" availability
            bool sensor_hardware_available = (data[1] & 0x08) != 0;
            support_.sensor_hardware_known = support_.fan_capability_known = true;
            support_.auto_limit_offset = (data[0] & 0x40) != 0;
            support_.sensor_hardware = sensor_hardware_available;
            support_.sensor = sensor_hardware_available;
            support_.fan = (data[2] & 0x04) != 0;
            if (!support_.fan) support_.swing = support_.horizontal_swing = false;
            support_.simple_humidify=(data[2]&2)!=0;
            state_.simpleHumidifyAvailable=support_.simple_humidify;
            identification_.fkValid=true;identification_.fkRaw={data[0],data[1],data[2],data[3]};
            identification_.laundryAvailable=(data[0]&8)!=0;identification_.simpleHumidify=support_.simple_humidify;
            identification_.japaneseTemperatureRange=(data[1]&4)!=0;identification_.acledRaw=data[0]&1;identification_.electricityRaw=data[1]&1;
            identification_.destinationRaw=data[2]&1;identification_.displayDryRaw=(data[2]>>3)&1;
            if (support_.simple_humidify) humidity_modes_ = 0x10; // simple Humidify has no level controls
            support_.demand = (data[3] & 0x01) != 0 && protocol_version_.major > 2;

            logInfoP("FK capabilities: sensor_hardware=%d (byte1=0x%02X, bit3=%d)",
                     sensor_hardware_available, data[1], (data[1] & 0x08) ? 1 : 0);

            for (size_t i = 0; i < data_size; i++)
            {
                // logInfoP("  FK[%zu] = 0x%02X", i, data[i]);
            }
        }
        else
        {
            logErrorP("FK response: %zu bytes (unexpected size)", data_size);
            stats_.frames_ok++;
        }
        // Store capability data if needed for future feature detection
    }
}

void DaikinDriver::handle_fu_fx_response(std::string_view command, uint8_t* data, size_t data_size)
{
    if (data_size < 1)
    {
        logErrorP("%.*s response empty", static_cast<int>(command.size()), command.data());
        return;
    }
    if (data_size == 1)
    {
        logDebugP("%.*s: ACK only (0x%02X)", static_cast<int>(command.size()), command.data(), data[0]);
        return;
    }
    stats_.frames_ok++;

    auto hex_nibble = [](uint8_t c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
        return -1;
    };

    auto parse_reversed_ascii_hex = [&](const uint8_t* src, size_t len, uint32_t& out) -> bool {
        if (len == 0 || len > 8) return false;
        uint32_t value = 0;
        for (size_t i = 0; i < len; i++)
        {
            int n = hex_nibble(src[len - 1 - i]);
            if (n < 0) return false;
            value = (value << 4) | static_cast<uint32_t>(n);
        }
        out = value;
        return true;
    };

    if (command == StateQuery::ExtensionFU00 && data_size == 32)
    {
        if ((data[0] != '0' && data[0] != '3') || (data[1] != '0' && data[1] != '3') ||
            (data[5] != '0' && data[5] != '3')) return;
        support_.special_features_known = true;
        support_.powerful = data[0] == 0x33;
        support_.econo = data[1] == 0x33;
        support_.streamer = data[5] == 0x33;
        return;
    }

    if (command == StateQuery::ExtensionFU05) {
        if (data_size != 32) return;
        std::string model;
        for (size_t i=0;i<22;i++) {
            if (data[i]==0xFF || data[i]==0) break;
            if (data[i]<0x20 || data[i]>0x7E) return;
            model.push_back(static_cast<char>(data[i]));
        }
        while (!model.empty() && model.back()==' ') model.pop_back();
        model_name_=model;
        return;
    }
    if (command == "FU15" || command == "FU25" || command == "FU35" || command == "FU45") {
        if(data_size!=32) return;
        auto text=[&](size_t offset,size_t length){std::string value;for(size_t i=offset;i<offset+length && data[i]!=0xFF && data[i]!=0;i++){if(data[i]<0x20 || data[i]>0x7E) return std::string{};value.push_back(char(data[i]));}while(!value.empty() && value.back()==' ') value.pop_back();return value;};
        auto& p=identification_.production;
        if(command=="FU15") {p.siteCode=text(0,4);p.lineCode=text(4,6);p.valid=true;}
        if(command=="FU25") {p.orderCode=text(0,15);p.valid=true;}
        if(command=="FU35" || command=="FU45") {auto& unit=command=="FU35"?p.indoor:p.outdoor;unit.siteCode=text(0,2);unit.baseCode=data[2];unit.deviceCode=data[3];unit.dateCode=text(4,8);unit.serialCode=text(12,12);unit.valid=true;}
        return; // sensitive local diagnostics only; never log production/serial content
    }

    if (command == StateQuery::ExtensionFU02)
    {
        state_.heatMinimumValid = data_size == 32 && data[2] >= 0x30 && data[2] <= 0x6F;
        state_.autoLimitOffsetValid = data_size == 32 && support_.auto_limit_offset && data[1] >= 0xA0 && data[1] <= 0xB0;
        if (state_.heatMinimumValid) state_.heatMinimumC = 10.0f + (data[2] - 0x30) / 2.0f;
        if (state_.autoLimitOffsetValid) state_.autoLimitOffsetC = (data[1] - 0xA0) / 2.0f;
        return; // no other offsets have established absolute temperature semantics
    }

    if (command == StateQuery::ExtensionFU04)
    {
        uint32_t cool = 0, heat = 0;
        state_.fu04_valid = data_size == 32 && parse_reversed_ascii_hex(data,8,cool) &&
                            parse_reversed_ascii_hex(data+8,8,heat);
        if (state_.fu04_valid) {
            state_.fu04_cooling_wh = uint64_t(cool) * 100ULL;
            state_.fu04_heating_wh = uint64_t(heat) * 100ULL;
            logInfoP("FU04 indoor lifetime cooling=%llu Wh heating=%llu Wh", state_.fu04_cooling_wh, state_.fu04_heating_wh);
        }
        return;
    }

    if (command == StateQuery::ExtensionFX60)
    {
        uint32_t value = 0;
        state_.fx60_valid = data_size == 4 && parse_reversed_ascii_hex(data,4,value);
        if (state_.fx60_valid) {
            state_.fx60_raw = value;
            // RevK Faikout.c uses consumption = raw * 10 (ten-watt units).
            state_.fx60_power_w = value * 10U;
            logInfoP("FX60 indoor power=%u W (raw=0x%04X)",state_.fx60_power_w,value);
        }
        return;
    }

    // Unknown FU/FX pages are retained in the query's response_data verbatim.
    // Avoid speculative conversions and repeated raw dumps on ordinary status logs.
    logDebugP("%.*s: raw diagnostic payload retained (%zu bytes)",static_cast<int>(command.size()),command.data(),data_size);

}

void DaikinDriver::handle_rg_response(uint8_t* data, size_t data_size)
{
    if (data_size < 1)
    {
        logErrorP("RG response empty");
        return;
    }

    stats_.frames_ok++;

    const uint8_t raw = data[0];
    const bool known_fan_value = std::string_view{"AB34567"}.find(char(raw))!=std::string_view::npos;

    if (!known_fan_value)
    {
        logDebugP("RG: unexpected fan payload byte 0x%02X (len=%zu) - ignoring", raw, data_size);
        return;
    }

    support_.fan_mode_query = true; // Mark fan mode query as supported once valid payload is seen

    daikin::DaikinFanMode old_fan = state_.fan;
    state_.fanValid = true;
    state_.fan = daikin_to_fan_mode(raw);
    if (state_.fan == pending_.climate.fan_mode) climate_dirty_ &= ~DirtyFan;
    confirmClimateFields(DirtyFan);
    logDebugP("RG: fan_mode decoded from raw 0x%02X ('%c') -> %d",
              raw, (raw >= 32 && raw < 127) ? static_cast<char>(raw) : '?', static_cast<int>(state_.fan));

    // Report fan speed change if it changed
    if (old_fan != state_.fan)
    {
        logInfoP("RG: Fan speed changed from %d to %d", static_cast<int>(old_fan), static_cast<int>(state_.fan));
        publishState(); // Trigger immediate state update
    }
}

void DaikinDriver::handle_rh_response(uint8_t* data, size_t data_size)
{
    if (!s21_valid_decimal(data,data_size,true)) return;
    support_.inside_temperature_query=true;
    state_.insideTemperatureValid=true;
    if (data_size < 1)
    {
        logErrorP("RH response empty");
        return;
    }
    if (data_size == 1)
    { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RH: Single-byte ACK received (0x%02X) - no inside temperature data", response);
        return; // Single-byte response processed
    }
    // RH/SH response: expect 4-byte sensor response (SH case)
    if (data_size >= 4)
    {
        stats_.frames_ok++;
        float temp = s21_decode_float_sensor(data);

        if (temp < 100.0f)
        { // sanity check
            state_.homeC = temp;
            logInfoP("RH/SH: inside_temp=%.1f°C (raw: %c%c%c%c)",
                     state_.homeC, data[0], data[1], data[2], data[3]);

            // Mark RH sample as seen for publish gating
            sample_seen_mask_ |= SEEN_RH;
            // If gating active and mask now complete, publish immediately
            if (gate_publish_until_full_sample_ &&
                (sample_seen_mask_ & (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA)) == (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA))
            {
                logDebugP("RH: full sample reached -> immediate publish");
                publishState();
            }
        }
        else
        {
            logErrorP("RH/SH: Invalid temperature %.1f°C", temp);
        }
    }
}

void DaikinDriver::handle_rx_response(uint8_t* data, size_t data_size)
{
    if (!s21_valid_decimal(data,data_size,true)) return;
    if (data_size < 1)
    {
        logErrorP("RX response empty");
        return;
    }
    if (data_size == 1)
    { // Single-byte response is always ACK
        uint8_t response = data[0];
        logDebugP("RX: Single-byte ACK received (0x%02X) - no target temperature data", response);
        return; // Single-byte response processed
    }
    // RX/SX response: expect 4-byte sensor response (SX case)
    if (data_size >= 4)
    {
        stats_.frames_ok++;
        float temp = s21_decode_float_sensor(data);

        if (temp < 100.0f)
        {                              // sanity check
            state_.realTargetC = temp; // Store sensor-adjusted target

            state_.targetOffsetValid = state_.targetTemperatureValid;
            state_.targetOffsetC = state_.realTargetC - state_.targetC;

            logInfoP("RX/SX: sensor_adjusted_target=%.1f°C (raw: %c%c%c%c)",
                     state_.realTargetC, data[0], data[1], data[2], data[3]);

            // Mark RX sample as seen for publish gating
            sample_seen_mask_ |= SEEN_RX;
            if (gate_publish_until_full_sample_ &&
                (sample_seen_mask_ & (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA)) == (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA))
            {
                logDebugP("RX: full sample reached -> immediate publish");
                publishState();
            }
        }
        else
        {
            logErrorP("RX/SX: Invalid temperature %.1f°C", temp);
        }
    }
}

void DaikinDriver::handle_ra_response(uint8_t* data, size_t data_size)
{
    if (!s21_valid_decimal(data,data_size,true)) return;
    support_.outside_temperature_query=true;
    state_.outsideTemperatureValid=true;
    if (data_size < 1)
    {
        logErrorP("Ra response empty");
        return;
    }
    if (data_size == 1)
    { // Single-byte response is always ACK
        return;
    }
    // Ra/Sa response: expect 4-byte sensor response (Sa case)
    if (data_size >= 4)
    {
        stats_.frames_ok++;
        float temp = s21_decode_float_sensor(data);

        if (temp < 100.0f)
        { // sanity check
            state_.outsideC = temp;
            logInfoP("Ra/Sa: outside_temp=%.1f°C (raw: %c%c%c%c)",
                     state_.outsideC, data[0], data[1], data[2], data[3]);

            // Mark RA sample as seen for publish gating (after successful decode)
            sample_seen_mask_ |= SEEN_RA;
            if (gate_publish_until_full_sample_ &&
                (sample_seen_mask_ & (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA)) == (SEEN_F1 | SEEN_RH | SEEN_RX | SEEN_RA))
            {
                logDebugP("RA: full sample reached -> immediate publish");
                publishState();
            }
        }
        else
        {
            logErrorP("Ra/Sa: Invalid temperature %.1f°C", temp);
        }
    }
}

void DaikinDriver::handle_rd_response(uint8_t* data, size_t size)
{
    if (!s21_valid_decimal(data,size,false)) return;
    const unsigned hz=(data[0]-'0')+(data[1]-'0')*10+(data[2]-'0')*100;
    if (hz>=999) return; // observed unsupported sentinel
    state_.compressorHz=hz; state_.compressorHzValid=true;
}

void DaikinDriver::handle_re_response(uint8_t* data, size_t size)
{
    if (!s21_valid_decimal(data,size,false)) return;
    const unsigned humidity=(data[0]-'0')+(data[1]-'0')*10+(data[2]-'0')*100;
    if (humidity>100) return;
    support_.humidity_query=true;
    state_.humidityValid=true;
    state_.humidity=humidity;
}

void DaikinDriver::handle_rg2_response(uint8_t* data, size_t size)
{
    if (size != 1 || (data[0] != '0' && data[0] != '1')) return;
    state_.compressorOn = data[0] == '1'; state_.compressorOnValid = true;
}

void DaikinDriver::handle_rzb2_response(uint8_t* data, size_t data_size)
{
    uint8_t value = 0;
    if (data_size != 2 || !s21_decode_hex_byte_reversed(data, value)) return;
    state_.unitState.raw = value; state_.unitStateValid=true;
    stats_.frames_ok++;
}

void DaikinDriver::handle_rzc3_response(uint8_t* data, size_t data_size)
{
    uint8_t value = 0;
    if (data_size != 2 || !s21_decode_hex_byte_reversed(data, value)) return;
    state_.systemState.raw = value; state_.systemStateValid=true;
    stats_.frames_ok++;
}

// === Serial Communication Callbacks ===

void DaikinDriver::handle_serial_result(daikin::DaikinSerial::Result result, uint8_t* data, size_t data_size)
{
    uint32_t now = millis();
    if(diagnostic_active_) {
        if(result==daikin::DaikinSerial::Result::Ack || result==daikin::DaikinSerial::Result::Frame) {markOnline(now);if(result==daikin::DaikinSerial::Result::Ack)++stats_.acks;}
        if(result==daikin::DaikinSerial::Result::Ack && !diagnostic_ack_only_) return;
        diagnostic_result_.status=result==daikin::DaikinSerial::Result::Ack?DiagnosticStatus::Ack:
            result==daikin::DaikinSerial::Result::Frame?DiagnosticStatus::Frame:result==daikin::DaikinSerial::Result::Nak?DiagnosticStatus::Nak:
            result==daikin::DaikinSerial::Result::Timeout?DiagnosticStatus::Timeout:DiagnosticStatus::Error;
        if(result==daikin::DaikinSerial::Result::Frame) {
            const auto header=daikin::s21::responseHeader(diagnostic_result_.command);
            if (data_size < header.size() || !std::equal(header.begin(), header.end(), data))
            {
                diagnostic_result_.status = DiagnosticStatus::Error;
            }
            else
            {
                diagnostic_result_.payload.assign(data + header.size(), data + data_size);
                if (!daikin::s21::validResponse(diagnostic_result_.command, diagnostic_result_.payload.data(), diagnostic_result_.payload.size()))
                    diagnostic_result_.status = DiagnosticStatus::Error;
            }
        }
        diagnostic_active_=false;
        if(diagnostic_ack_only_) handleWriteCommandSent(LastWriteCommand::Other);
        return;
    }
    if (d1_awaiting_ack_) {
        if (result == daikin::DaikinSerial::Result::Nak || result == daikin::DaikinSerial::Result::Error || result == daikin::DaikinSerial::Result::Timeout)
        {
            climate_outcome_=result==daikin::DaikinSerial::Result::Nak?WriteOutcome::Rejected:result==daikin::DaikinSerial::Result::Timeout?WriteOutcome::Timeout:WriteOutcome::Error;
            rejectClimateWrite();
        }
        else if (result == daikin::DaikinSerial::Result::Ack) {d1_awaiting_ack_ = false;climate_outcome_=WriteOutcome::AwaitingReadback;}
    }

    const bool accept_response = query_state_ == QueryState::WaitingForAck ||
                                 query_state_ == QueryState::WaitingForGrace ||
                                 query_state_ == QueryState::PostWriteSettle;
    if (!accept_response) return;

    if(!outstanding_write_.empty()) {
        auto* c=combinedFor(outstanding_write_);
        if(c) {
            logInfoP("S21 write %.*s transport result=%u",int(outstanding_write_.size()),outstanding_write_.data(),unsigned(result));
            if(result==daikin::DaikinSerial::Result::Ack) {c->awaiting_readback=true;c->outcome=WriteOutcome::AwaitingReadback;}
            if(result==daikin::DaikinSerial::Result::Frame && outstanding_write_=="FB") {c->awaiting_readback=true;c->outcome=WriteOutcome::AwaitingReadback;}
            if(result==daikin::DaikinSerial::Result::Nak) {c->rejected=c->requested;c->rejected_valid=true;c->last_rejection_ms=now;c->outcome=WriteOutcome::Rejected;c->awaiting_readback=false;}
            if(result==daikin::DaikinSerial::Result::Timeout) c->outcome=WriteOutcome::Timeout;
            if(result==daikin::DaikinSerial::Result::Error) c->outcome=WriteOutcome::Error;
        }
        const bool timerWrite=outstanding_write_=="D3" || outstanding_write_=="FB";
        if(result!=daikin::DaikinSerial::Result::Frame || outstanding_write_=="FB") outstanding_write_={};
        if(timerWrite) publishTimerStatus();
    }
    // During post-write settle we may receive ACK/frames that belong to D-commands.
    // Keep the settle state active so the coalesced D1 flush can run after timeout.
    if (query_state_ == QueryState::PostWriteSettle)
    {
        switch (result)
        {
            case daikin::DaikinSerial::Result::Ack:
                markOnline(now);
                stats_.acks++;
                break;
            case daikin::DaikinSerial::Result::Frame:
                markOnline(now);
                stats_.frames_ok++;
                // FB writes may return GB immediately after ACK (or without
                // ACK). Consume it here instead of discarding it during settle.
                if (fb_.awaiting_readback && data_size==6 && data[0]=='G' && data[1]=='B' &&
                    daikin::s21::validResponse("FB",data+2,data_size-2))
                    handle_fb_response(data+2,data_size-2);
                break;
            case daikin::DaikinSerial::Result::Nak:
                stats_.frames_nak++;
                break;
            case daikin::DaikinSerial::Result::Error:
                stats_.frames_bad_checksum++;
                break;
            case daikin::DaikinSerial::Result::Timeout:
                break;
        }
        logDebugP("Handled write response during settle without advancing query state");
        return;
    }

    if (query_index_ >= queries_.size())
    {
        logDebugP("Received result but no active query");
        return;
    }

    auto& query = queries_[query_index_];
    if(result==daikin::DaikinSerial::Result::Frame || result==daikin::DaikinSerial::Result::Nak || result==daikin::DaikinSerial::Result::Timeout || result==daikin::DaikinSerial::Result::Error) invalidatePage(query.command);

    switch (result)
    {
        case daikin::DaikinSerial::Result::Ack:
            markOnline(millis()); // Mark device online on ACK
            stats_.acks++;
            // An ACK alone does not satisfy a static read.
            // Wait for framed reply (if any) -> stay in WaitingForGrace
            query_state_ = QueryState::WaitingForGrace;
            state_start_time_ = millis();
            break;
        case daikin::DaikinSerial::Result::Frame:
            if (data_size >= 1)
            {
                // S21 protocol command/response mapping:
                // F* → G* (same tail): F8 → G8, FY00 → GY00, etc.
                // R* → R* or r* (read queries can be echoed in upper or lowercase)
                // Special case: F6 may reply as G6 (new) or G3 (old)

                bool is_valid_response = false;
                size_t expected_cmd_len = query.command == "VS000M" ? 2 : query.command.size();
                uint8_t* payload = nullptr;
                size_t payload_len = 0;

                if (data_size >= expected_cmd_len)
                {
                    std::string response_header(data, data + expected_cmd_len);
                    std::string expected_cmd(query.command == "VS000M" ? "VS" : query.command);

                    if (expected_cmd[0] == 'F')
                    {
                        // F* commands expect G* responses (same tail)
                        std::string expected_response = "G" + expected_cmd.substr(1);
                        if (response_header == expected_response)
                        {
                            is_valid_response = true;
                        }
                    }
                    else if (expected_cmd[0] == 'R')
                    {
                        // R* queries expect S* responses (R->S mapping per S21 protocol)
                        std::string expected_response = "S" + expected_cmd.substr(1);
                        if (response_header == expected_response)
                        {
                            is_valid_response = true;
                        }
                    }
                    else
                    {
                        // Other commands expect exact echo
                        if (response_header == expected_cmd)
                        {
                            is_valid_response = true;
                        }
                    }

                    if (is_valid_response)
                    {
                        payload = data + expected_cmd_len;
                        payload_len = data_size - expected_cmd_len;
                    }
                }

                if (is_valid_response)
                {
                    if (!s21_valid_response(query.command,payload,payload_len)) {
                        stats_.frames_bad_format++;
                        recordRejectedResponse(query.command,payload,payload_len,false);
                        advanceQuery(); query_state_=QueryState::WaitingToSend; state_start_time_=now;
                        return;
                    }
                    markOnline(now);
                    if (payload_len > 0)
                    {
                        query.naks = 0;
                        query.acked = true;
                        query.last_sample_ms = now;
                        query.response_data.assign(payload, payload + payload_len);
                        if (query.handler) query.handler(payload, payload_len);
                    }
                    stats_.frames_ok++;

                    // Clear post-write F1 flag if this was the first F1 after write (v0 stability)
                    if (query.command == StateQuery::Basic && post_write_first_f1_pending_)
                    {
                        post_write_first_f1_pending_ = false;
                        logDebugP("Cleared post-write F1 flag after successful response");
                    }
                }
                else
                {
                    logErrorP("S21: Frame command mismatch for %.*s (rx header: %.*s)",
                              static_cast<int>(query.command.size()), query.command.data(),
                              static_cast<int>(std::min(data_size, expected_cmd_len)), data);
                    stats_.frames_bad_format++;
                    recordRejectedResponse(query.command,data,data_size,true);
                    return; // unrelated frames must not advance the outstanding query
                }
            }
            // Advance after frame
            advanceQuery();
            query_state_ = QueryState::WaitingToSend;
            state_start_time_ = millis();
            break;
        case daikin::DaikinSerial::Result::Nak:
            // logInfoP("S21: NAK for %.*s", static_cast<int>(query.command.size()), query.command.data());
            query.last_nak_ms=now;
            query.naks++;
            stats_.frames_nak++;

            // Special handling for FY00 NAK during protocol detection (per S21 wiki)
            if (query.command == StateQuery::NewProtocol && protocol_detection_phase_)
            {
                if (protocol_version_ == daikin::ProtocolUndetected)
                {
                    // Both version probes NAK on documented unversioned units.
                    const auto f8 = std::find_if(queries_.begin(), queries_.end(), [](const DaikinQueryState& q) { return q.command == "F8"; });
                    if (f8 != queries_.end() && f8->naks) {
                        protocol_version_ = daikin::ProtocolUnknown;
                        state_.protocol=protocol_version_; state_.protocolVersion=0;
                        old_protocol_detected_ = true;
                    }
                }
                else if (protocol_version_.major == 0 && protocol_version_.minor == 0)
                {
                    // FY00 NAK with preliminary v0.0 from F8 = confirms v0.0 (old protocol)
                    old_protocol_detected_ = true;
                    logInfoP("FY00: NAK confirms F8's v0.0 detection - old protocol confirmed");
                }
                else
                {
                    logInfoP("S21: FY00 NAK with known protocol v%d.%d - expected behavior",
                             protocol_version_.major, protocol_version_.minor);
                }
            }
            // Smart NAK tracking with overflow protection
            if (!query.naks)
            { // Overflow protection (255 -> 0 wraparound)
                query.bad = true;
            }
            else if (query.naks >= 3)
            {
                query.bad = true;
                failed_queries_.push_back(query.command);
                // Check if F6 failed and switch to F3 fallback
                if (query.command == StateQuery::SpecialModes)
                {
                    checkF6ToF3Fallback();
                }
            }

            advanceQuery();
            query_state_ = QueryState::WaitingToSend;
            state_start_time_ = millis();
            break;
        case daikin::DaikinSerial::Result::Timeout:
            if (protocol_detection_phase_)
            {
                // Special handling for FY00 timeout during protocol detection
                if (query.command == StateQuery::NewProtocol && protocol_version_.major == 0 && protocol_version_.minor == 0)
                {
                    // FY00 timeout with preliminary v0.0 from F8 = confirms v0.0 (old protocol)
                    old_protocol_detected_ = true;
                    logInfoP("FY00: Timeout confirms F8's v0.0 detection - old protocol confirmed");
                }
            }
            else
            {
                // logInfoP("S21: Timeout waiting for %.*s", static_cast<int>(query.command.size()), query.command.data());
            }
            // Silence is transport failure, not evidence of an unsupported command.
            advanceQuery();
            query_state_ = QueryState::WaitingToSend;
            state_start_time_ = millis();
            break;
        case daikin::DaikinSerial::Result::Error:
            logErrorP("S21: Error on %.*s (checksum mismatch or comm error)",
                      static_cast<int>(query.command.size()), query.command.data());
            stats_.frames_bad_checksum++;
            advanceQuery();
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
    // Publish only confirmed, valid properties. Cache belongs to this instance.
    const auto& p = published_;
    const bool seed = !publication_initialized_ || seed_kos_pending_;
    if (state_.basicValid) {
        if (seed || !p.basicValid || state_.power != p.power) statusFeedback.updatePower(state_.power);
        if (seed || !p.basicValid || state_.mode != p.mode) statusFeedback.updateMode(daikin_to_openknx_mode(state_.mode));
    }
    if (state_.targetTemperatureValid && (seed || !p.targetTemperatureValid || state_.targetC != p.targetC)) statusFeedback.updateTargetTemperature(state_.targetC);
    if (state_.insideTemperatureValid && (seed || !p.insideTemperatureValid || state_.homeC != p.homeC)) statusFeedback.updateCurrentTemperature(state_.homeC);
    if (state_.outsideTemperatureValid && (seed || !p.outsideTemperatureValid || state_.outsideC != p.outsideC)) statusFeedback.updateOutdoorTemperature(state_.outsideC);
    if (state_.fanValid && (seed || !p.fanValid || state_.fan != p.fan)) statusFeedback.updateFanSpeed(daikin_to_openknx_fan(state_.fan));
    if (state_.swingValid && (seed || !p.swingValid || state_.swing != p.swing)) {
        statusFeedback.updateSwingHorizontal(state_.swing == daikin::Swing::Horizontal || state_.swing == daikin::Swing::Both);
        statusFeedback.updateSwingVertical(state_.swing == daikin::Swing::Vertical || state_.swing == daikin::Swing::Both);
    }
    if (state_.verticalPositionValid && getMaximumVerticalFixPosition() && (seed || !p.verticalPositionValid || p.verticalPosition != state_.verticalPosition))
        statusFeedback.swingVerticalFixPositionChanged(state_.verticalPosition);
    if (state_.specialModesValid || state_.demandValid) {
        if (seed || !p.specialModesValid || state_.powerful!=p.powerful || state_.quiet!=p.quiet || state_.econo!=p.econo)
            statusFeedback.updateDeviceMode(state_.specialModesValid && state_.powerful ? AirConditionDeviceModeHiPower : state_.specialModesValid && state_.quiet ? AirConditionDeviceModeSilent1 : state_.demandValid && state_.econo ? AirConditionDeviceModeEco : AirConditionDeviceModeStandard);
        if (state_.specialModesValid && (seed || !p.specialModesValid || state_.streamer != p.streamer)) statusFeedback.updateAirPurification(state_.streamer);
    }
    if (state_.demandPercentageValid && (seed || !p.demandPercentageValid || state_.demand_percentage != p.demand_percentage)) statusFeedback.updateMaxPowerLevel(state_.demand_percentage);
    if (state_.ledState != daikin::LedState::Unknown && (seed || p.ledState != state_.ledState)) statusFeedback.updateWifiLed(state_.led);
    if (state_.humidityValid && (seed || !p.humidityValid || state_.humidity != p.humidity)) statusFeedback.updateHumidity(state_.humidity);
    if (state_.humidityModeValid && getMaximumHumidityModeLevels()>1 && (seed || !p.humidityModeValid || state_.humidityMode != p.humidityMode)) {
        uint8_t step = 0;
        switch(state_.humidityMode) {
            case daikin::HumidityMode::Low: step=1; break;
            case daikin::HumidityMode::Standard: step=2; break;
            case daikin::HumidityMode::High: step=3; break;
            case daikin::HumidityMode::Continuous: step=4; break;
            default: break;
        }
        statusFeedback.updateHumidityMode(step);
    }
    if (state_.totalEnergyValid && (seed || !p.totalEnergyValid || state_.totalEnergyWh != p.totalEnergyWh)) statusFeedback.updateTotalEnergyConsumption(state_.totalEnergyWh);
    if (seed || state_.online != p.online) statusFeedback.updateOnlineStatus(state_.online);
    if (seed || state_.fu04_valid!=p.fu04_valid || state_.fu04_cooling_wh!=p.fu04_cooling_wh || state_.fu04_heating_wh!=p.fu04_heating_wh || state_.fx60_valid!=p.fx60_valid || state_.fx60_raw!=p.fx60_raw)
        statusFeedback.updateDaikinExtensionTelemetry(state_.fu04_valid && state_.fu04_cooling_wh<=UINT32_MAX && state_.fu04_heating_wh<=UINT32_MAX,
            uint32_t(std::min<uint64_t>(state_.fu04_cooling_wh,UINT32_MAX)),uint32_t(std::min<uint64_t>(state_.fu04_heating_wh,UINT32_MAX)),state_.fx60_valid,state_.fx60_power_w*10U);
    state_.action = calculateCurrentAction(); state_.actionValid=state_.action!=daikin::Action::Unknown;
    statusFeedback.updateVerticalPositionStatus(state_.verticalPositionCodeValid, state_.verticalPosition, getMaximumVerticalFixPosition(), verified_vertical_positions_);
    AirConditionTelemetry sample;
    using T=AirConditionTelemetry;
    sample.valid=(state_.indoorHeatExchangerValid?T::Coil:0) | (state_.fanRpmValid?T::FanRpm:0) |
        (state_.fanRpmSetpointValid?T::FanTargetRpm:0) | (state_.vaneAngleValid?T::AngleRaw:0) |
        (state_.vaneAngleSetpointValid?T::AngleTargetRaw:0) | (state_.vaneAngleDegreesValid?T::AngleDegrees:0) |
        (state_.vaneAngleSetpointDegreesValid?T::AngleTargetDegrees:0) | (state_.pullValid?T::Pull:0) |
        (state_.serialErrorValid?T::ErrorByte:0) | (state_.protocol.major!=0xFF && state_.protocol.minor!=0xFF?T::Protocol:0) |
        (!firmware_version_.empty()?T::Firmware:0) | ((!model_name_.empty() || !identification_.modelCode.empty())?T::Model:0) |
        (state_.compressorHzValid?T::CompressorFrequency:0) | (state_.compressorOnValid?T::CompressorRunning:0);
    sample.indoorCoilC=state_.indoorHeatExchangerC;
    sample.fanRpm=state_.fanRpm; sample.fanTargetRpm=state_.fanRpmSetpoint;
    sample.angleRaw=state_.swingVerticalAngle; sample.angleTargetRaw=state_.swingVerticalAngleSetpoint;
    sample.angleDegrees=state_.vaneAngleDegrees; sample.angleTargetDegrees=state_.vaneAngleSetpointDegrees;
    sample.pull=state_.pull; sample.errorByte=state_.serialErrorByte;
    sample.protocol=(uint16_t(state_.protocol.major)<<8)|state_.protocol.minor;
    sample.firmware=firmware_version_; sample.model=model_name_.empty()?identification_.modelCode:model_name_;
    sample.compressorHz=state_.compressorHz; sample.compressorRunning=state_.compressorOn;
    statusFeedback.updateAdditionalTelemetry(sample);
    statusFeedback.updateOptionalControls(optionalControlState());
    uint8_t humidityLevel=0;
    switch (state_.humidityMode) {
        case daikin::HumidityMode::Low:humidityLevel=1;break;
        case daikin::HumidityMode::Standard:humidityLevel=2;break;
        case daikin::HumidityMode::High:humidityLevel=3;break;
        case daikin::HumidityMode::Continuous:humidityLevel=4;break;
        default:break;
    }
    statusFeedback.updateHumidityControl(state_.basicValid && state_.humidityModeValid && getMaximumHumidityModeLevels()>1,
        static_cast<AirConditionHumidityLevel>(humidityLevel), availableHumidityLevels(),
        support_.f2_known && !support_.simple_humidify ? humidity_modes_ : 0);
    AirConditionEnergyTelemetry energy;
    energy.valid=(state_.fx60_valid?AirConditionEnergyTelemetry::Power:0) |
        (state_.fu04_valid?AirConditionEnergyTelemetry::Cooling|AirConditionEnergyTelemetry::Heating:0) |
        (state_.totalEnergyValid?AirConditionEnergyTelemetry::System:0);
    energy.indoorPowerW=state_.fx60_power_w;
    energy.indoorCoolingWh=state_.fu04_cooling_wh; energy.indoorHeatingWh=state_.fu04_heating_wh;
    energy.systemWh=state_.totalEnergyWh;
    statusFeedback.updateEnergyTelemetry(energy);
    publishTimerStatus();
    published_ = state_; publication_initialized_=true;
    seed_kos_pending_=false; gate_publish_until_full_sample_=false;
}

void DaikinDriver::confirmClimateFields(uint8_t fields)
{
    if (!sent_climate_dirty_ || d1_awaiting_ack_) return;
    climate_readback_seen_ = true;
    for (size_t i=0;i<4;i++) {
        const uint8_t bit=1U<<i;
        if ((fields & bit) && (!(climate_dirty_ & bit) || climate_generation_[i]!=sent_climate_generation_[i]))
            sent_climate_dirty_ &= ~bit;
    }
    if (!sent_climate_dirty_) {
        climate_outcome_=WriteOutcome::Confirmed;
        climate_confirmation_started_=0;
    }
    // A stale first readback is not a rejection. Keep observing for up to 30 s,
    // without resending the write, and publish only actual AC samples.
}

void DaikinDriver::selectConfirmationQuery()
{
    if (protocol_detection_phase_ || confirmation_resume_!=SIZE_MAX || millis()-last_confirmation_poll_<1000) return;
    const std::array<std::string_view,8> commands={"F1","RG","F3","F5","F6","F7","FR","FB"};
    for (size_t count=0;count<commands.size();count++) {
        const auto slot=confirmation_cursor_++ % commands.size();
        const auto cmd=commands[slot];
        const auto* combined=combinedFor(cmd);
        const bool needed=cmd=="F1" ? (sent_climate_dirty_ & (DirtyPower|DirtyMode|DirtyTemperature|(!support_.fan_mode_query?DirtyFan:0)))!=0 :
            cmd=="RG" ? (sent_climate_dirty_ & DirtyFan)!=0 : combined && combined->awaiting_readback;
        if (!needed) continue;
        for (size_t i=0;i<queries_.size();i++) if (queries_[i].command==cmd && !queries_[i].bad && isQuerySupported(cmd)) {
            confirmation_resume_=query_index_==i?i+1:query_index_;
            query_index_=i;
            last_confirmation_poll_=millis();
            return;
        }
    }
}

void DaikinDriver::advanceQuery()
{
    if (confirmation_resume_!=SIZE_MAX) {
        query_index_=confirmation_resume_;
        confirmation_resume_=SIZE_MAX;
    } else ++query_index_;
}

void DaikinDriver::recordRejectedResponse(std::string_view command,const uint8_t* bytes,size_t size,bool headerMismatch)
{
    auto& entry=rejected_responses_[rejected_response_next_];
    entry.at=millis();entry.command=std::string(command.substr(0,6));entry.headerMismatch=headerMismatch;
    entry.bytes.clear();
    if(bytes && size) entry.bytes.assign(bytes,bytes+std::min<size_t>(size,38));
    rejected_response_next_=(rejected_response_next_+1)%rejected_responses_.size();
    rejected_response_count_=std::min(rejected_response_count_+1,rejected_responses_.size());
    logInfoP("Rejected S21 %s %s: %s",entry.command.c_str(),headerMismatch?"header":"payload",toHexString(entry.bytes).c_str());
}

void DaikinDriver::showRejectedResponses() const
{
    logInfoP("Last %u rejected S21 responses (bounded to 38 bytes each)",unsigned(rejected_response_count_));
    const size_t first=(rejected_response_next_+rejected_responses_.size()-rejected_response_count_)%rejected_responses_.size();
    for(size_t i=0;i<rejected_response_count_;i++) {
        const auto& entry=rejected_responses_[(first+i)%rejected_responses_.size()];
        logInfoP("%lu ms %s %s: %s",static_cast<unsigned long>(entry.at),entry.command.c_str(),entry.headerMismatch?"header":"payload",toHexString(entry.bytes).c_str());
    }
}

void DaikinDriver::showWriteDiagnostics() const
{
    for(auto cmd:{"D3","D5","D6","D7","DR","FB"}) {
        const auto* page=combinedFor(cmd);
        logInfoP("%s outcome=%u awaiting=%u valid=%u age=%lu ms",cmd,unsigned(page->outcome),unsigned(page->awaiting_readback),unsigned(page->valid),static_cast<unsigned long>(millis()-page->sent_ms));
        logInfoP("requested=%s mask=%s read=%s",toHexString(std::vector<uint8_t>(page->requested.begin(),page->requested.end())).c_str(),
            toHexString(std::vector<uint8_t>(page->mask.begin(),page->mask.end())).c_str(),
            toHexString(std::vector<uint8_t>(page->confirmed.begin(),page->confirmed.end())).c_str());
    }
    logInfoP("D7 queued: econo=%u/%u demand=%u/%u",unsigned(pending_.activate_econo),unsigned(pending_.climate.econo),unsigned(demand_pending_),unsigned(requested_demand_));
}

void DaikinDriver::rejectClimateWrite()
{
    for (size_t i=0;i<4;i++) if ((sent_climate_dirty_ & (1U<<i)) && climate_generation_[i] == sent_climate_generation_[i]) climate_dirty_ &= ~(1U<<i);
    sent_climate_dirty_=0; d1_awaiting_ack_=false; climate_confirmation_started_=0;
}

void DaikinDriver::invalidatePage(std::string_view c)
{
    if(auto* p=combinedFor(c)) p->valid=false;
    if(c=="F1") {state_.basicValid=state_.targetTemperatureValid=state_.setpointRawValid=state_.climateVariantValid=false;if(!support_.fan_mode_query) state_.fanValid=false;}
    if(c=="F3") state_.timersValid=false;
    if(c=="F4") state_.serialErrorValid=false;
    if(c=="F5") state_.swingValid=state_.humidityModeValid=false;
    if(c=="F6") {state_.specialModesValid=false;state_.ledState=daikin::LedState::Unknown;}
    if(c=="F7") state_.demandValid=state_.demandPercentageValid=false;
    if(c=="F9") {if(!support_.inside_temperature_query) state_.insideTemperatureValid=false;if(!support_.outside_temperature_query) state_.outsideTemperatureValid=false;if(!support_.humidity_query) state_.humidityValid=false;}
    if(c=="RH") {state_.insideTemperatureValid=false;support_.inside_temperature_query=false;}
    if(c=="Ra") {state_.outsideTemperatureValid=false;support_.outside_temperature_query=false;}
    if(c=="Re") {state_.humidityValid=false;support_.humidity_query=false;}
    if(c=="RX") state_.targetOffsetValid=false;
    if(c=="RI") state_.indoorHeatExchangerValid=false;
    if(c=="RG") {state_.fanValid=false;support_.fan_mode_query=false;}
    if(c=="Rg") state_.compressorOnValid=false;
    if(c=="Rd") state_.compressorHzValid=false;
    if(c=="RK") state_.fanRpmSetpointValid=false;
    if(c=="RL") state_.fanRpmValid=false;
    if(c=="RM") state_.vaneAngleSetpointValid=state_.vaneAngleSetpointDegreesValid=false;
    if(c=="RN") state_.vaneAngleValid=state_.vaneAngleDegreesValid=false;
    if(c=="FR") state_.verticalPositionValid=state_.verticalPositionCodeValid=false;
    if(c=="FB") state_.timerLampValid=false;
    if(c=="FM") state_.totalEnergyValid=false;
    if(c=="FG") state_.irCounterValid=false;
    if(c=="FU04") state_.fu04_valid=false;
    if(c=="FX60") state_.fx60_valid=false;
    if(c=="RzB2") state_.unitStateValid=false;
    if(c=="RzC3") state_.systemStateValid=false;
    if(c=="RA") state_.independentPowerValid=false;
    if(c=="RB") state_.independentModeValid=false;
    if(c=="RC") state_.coarseTargetValid=false;
    if(c=="RD") state_.onTimerPeriodsValid=false;
    if(c=="RE") state_.offTimerPeriodsValid=false;
    if(c=="RF") state_.supplementalSwingValid=false;
    if(c=="Rb") state_.pullValid=false;
    if(c=="Rm") state_.rmRawValid=false;
    if(c=="Rt") state_.rtRawValid=false;
    if(c=="FS") identification_.wifiValid=false;
    if(c=="FT") identification_.ftValid=false;
}

void DaikinDriver::invalidateTelemetry()
{
    state_.online=false;state_.protocol=daikin::ProtocolUndetected;state_.protocolVersion=0xFF;
    state_.basicValid=state_.targetTemperatureValid=state_.fanValid=state_.swingValid=false;
    state_.insideTemperatureValid=state_.outsideTemperatureValid=state_.humidityValid=false;
    state_.specialModesValid=state_.demandValid=state_.demandPercentageValid=state_.compressorOnValid=state_.compressorHzValid=false;
    state_.indoorHeatExchangerValid=state_.humidityModeValid=state_.unitStateValid=state_.systemStateValid=false;
    state_.vaneAngleValid=state_.vaneAngleSetpointValid=state_.vaneAngleDegreesValid=state_.vaneAngleSetpointDegreesValid=false;
    state_.fanRpmValid=state_.fanRpmSetpointValid=state_.timerLampValid=state_.serialErrorValid=false;
    state_.timersValid=state_.verticalPositionValid=state_.verticalPositionCodeValid=state_.targetOffsetValid=state_.climateVariantValid=false;
    state_.fu04_valid=state_.fx60_valid=state_.totalEnergyValid=false;
    state_.irCounterValid=state_.pullValid=state_.rmRawValid=state_.rtRawValid=false;
    state_.independentPowerValid=state_.independentModeValid=state_.coarseTargetValid=false;
    state_.supplementalSwingValid=state_.onTimerPeriodsValid=state_.offTimerPeriodsValid=false;
    state_.setpointRawValid=false; state_.ledState=daikin::LedState::Unknown;
    support_.inside_temperature_query=support_.outside_temperature_query=support_.humidity_query=support_.fan_mode_query=false;
    d5_.valid=d6_.valid=d7_.valid=d3_.valid=dr_.valid=fb_.valid=false;
    rejectClimateWrite(); sample_seen_mask_=0;
    publishState();
}

void DaikinDriver::updateOnlineStatus()
{
    // markOnline()/invalidateTelemetry() already update state_.online. Compare
    // against the module's state, otherwise its Starting/Error state never clears.
    state_.online = online_;
    const auto reported = statusFeedback.getDriverState();
    if (online_ && reported != AirConditionDriverStateOk)
    {
        statusFeedback.driverStateChanged(AirConditionDriverStateOk, "");
    }
    else if (!online_ && reported == AirConditionDriverStateOk)
    {
        statusFeedback.driverStateChanged(AirConditionDriverStateError, "Communication lost");
    }
}

// === Conversion Utilities ===






daikin::Action DaikinDriver::calculateCurrentAction() const
{
    if (!state_.online || !state_.basicValid) return daikin::Action::Unknown;
    if (!state_.power) return daikin::Action::Off;
    if (state_.mode == daikin::Mode::FanOnly) return daikin::Action::Fan;
    if(rz_states_trusted_ && state_.systemStateValid && state_.systemState.defrost()) return daikin::Action::Defrost;
    if(rz_states_trusted_ && state_.systemStateValid && state_.systemState.locked()) return daikin::Action::Standby;
    if(rz_states_trusted_ && state_.unitStateValid && !state_.unitState.active()) return daikin::Action::Idle;
    if (!state_.compressorOnValid && !(rz_states_trusted_ && state_.unitStateValid)) return daikin::Action::Unknown;
    if (state_.compressorOnValid && !state_.compressorOn) return daikin::Action::Idle;
    if (state_.mode == daikin::Mode::Cool) return daikin::Action::Cooling;
    if (state_.mode == daikin::Mode::Heat) return daikin::Action::Heating;
    if (state_.mode == daikin::Mode::Dry) return daikin::Action::Drying;
    if (state_.autoBias == daikin::AutoBias::Cooling) return daikin::Action::Cooling;
    if (state_.autoBias == daikin::AutoBias::Heating) return daikin::Action::Heating;
    return daikin::Action::Unknown;
}







daikin::DaikinFanMode DaikinDriver::daikin_to_fan_mode(uint8_t fan) const
{
    switch (fan)
    {
        case 0x00:
        case '0':
        case 'A': return daikin::DaikinFanMode::Auto;
        case 0x0B:
        case 'B': return daikin::DaikinFanMode::Silent;
        case 0x03:
        case '3': return daikin::DaikinFanMode::Speed1;
        case 0x04:
        case '4': return daikin::DaikinFanMode::Speed2;
        case 0x05:
        case '5': return daikin::DaikinFanMode::Speed3;
        case 0x06:
        case '6': return daikin::DaikinFanMode::Speed4;
        case 0x07:
        case '7': return daikin::DaikinFanMode::Speed5;
        default: return daikin::DaikinFanMode::Auto;
    }
}

// === OpenKNX to Daikin Conversions ===
daikin::Mode DaikinDriver::openknx_to_daikin_mode(AirConditionMode mode) const
{
    switch (mode)
    {
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
    switch (mode)
    {
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
    switch (speed)
    {
        case 0: return daikin::DaikinFanMode::Auto;
        case 1: return daikin::DaikinFanMode::Silent;
        case 2: return daikin::DaikinFanMode::Speed1;
        case 3: return daikin::DaikinFanMode::Speed2;
        case 4: return daikin::DaikinFanMode::Speed3;
        case 5: return daikin::DaikinFanMode::Speed4;
        case 6: return daikin::DaikinFanMode::Speed5;
        default: return daikin::DaikinFanMode::Auto;
    }
}

unsigned int DaikinDriver::daikin_to_openknx_fan(daikin::DaikinFanMode fan) const
{
    switch (fan)
    {
        case daikin::DaikinFanMode::Auto: return 0;
        case daikin::DaikinFanMode::Silent: return 1;
        case daikin::DaikinFanMode::Speed1: return 2;
        case daikin::DaikinFanMode::Speed2: return 3;
        case daikin::DaikinFanMode::Speed3: return 4;
        case daikin::DaikinFanMode::Speed4: return 5;
        case daikin::DaikinFanMode::Speed5: return 6;
        default: return 0;
    }
}

// === Smart NAK Tracking ===
void DaikinDriver::resetQueryNakTracking()
{
    // Reset NAK counters and bad flags for all queries
    for (auto& query : queries_)
    {
        query.acked = false;
        query.naks = 0;
        query.bad = false;
    }
    failed_queries_.clear();
    logDebugP("S21: Reset NAK tracking for all queries (communication restart)");
}

// === F6 to F3 Fallback Logic ===
void DaikinDriver::checkF6ToF3Fallback()
{
    // No verified model currently establishes F3 byte 3 as Powerful status.
    // Keep the unsupported F6 entry in place so callback references stay valid.
    support_.f6_special_modes = false;
    d6_.valid = false;
}

// === Online/Offline Detection ===
void DaikinDriver::markOnline(uint32_t now)
{
    last_rx_ok_ms_ = now;
    if (!online_)
    {
        online_ = true;
        state_.online = true;

        // Reset sample tracking to ensure full snapshot before publishing
        sample_seen_mask_ = 0;
        gate_publish_until_full_sample_ = true;
        seed_kos_pending_ = true; // Seed all KOs after first complete sample
        online_since_ms_ = now;   // start gate timeout window

        // Nach Reconnect: statische Capability-Frames (F2/FN/FP/FQ/FS/FT/FK) einmalig neu erlauben
        for (auto& q : queries_)
        {
            if (q.is_static) q.acked = false;
        }
        logInfoP("S21 online status changed: Online");
        statusFeedback.updateOnlineStatus(true); // → KO463 = 1
    }
}

void DaikinDriver::checkOnlineTimeout()
{
    const uint32_t OFFLINE_MS = timing_.offline_ms; // 15s timeout for offline detection


    uint32_t now = millis();
    if (online_ && last_rx_ok_ms_ != 0 && (now - last_rx_ok_ms_) > OFFLINE_MS)
    {
        online_ = false;
        invalidateTelemetry();
        model_name_.clear();firmware_version_.clear();identification_={};
        protocol_version_=daikin::ProtocolUndetected;protocol_detection_phase_=false;
        support_={};econo_observed_on_=econo_probe_pending_=false;queries_.clear();query_index_=0;query_state_=QueryState::Idle;
        d3_={};d5_={};d6_={};d7_={};dr_={};fb_={};outstanding_write_={};
        timer_disabled_bytes_={};last_timer_minutes_={};timer_dirty_=0;timer_lamp_pending_=false;timer_request_error_=lamp_request_error_=AirConditionWriteResult::None;
        pending_={};humidity_pending_=demand_pending_=vertical_pending_=timer_pending_=false;
        verified_vertical_positions_=0;last_write_time_=last_command_time_=0;
        publishTimerStatus();
        logInfoP("S21 online status changed: Offline (no ACK/frame for %lu ms)", (unsigned long)(now - last_rx_ok_ms_));
        statusFeedback.updateOnlineStatus(false); // → KO463 = 0

        // Force framed mode retry after offline timeout (recovery mechanism)
        if (serial_ && (last_framed_retry_ms_ == 0 || (now - last_framed_retry_ms_) > 30000))
        {
            serial_->force_framed_mode("offline recovery");
            last_framed_retry_ms_ = now;
        }
    }

    // Startup offline detection: if device never came online after startup,
    // publish offline default state so all KOs are initialized on the bus.
    // This handles the case where the AC is powered off when the GW starts.
    if (!startup_offline_published_ && !online_ && seed_kos_pending_ &&
        comm_start_time_ != 0 && (now - comm_start_time_) > STARTUP_OFFLINE_TIMEOUT_MS)
    {
        startup_offline_published_ = true;
        logInfoP("Device offline at startup after %lu ms -> publishing offline default state (all off)",
                 (unsigned long)(now - comm_start_time_));
        state_.online = false;
        gate_publish_until_full_sample_ = false;
        publishState(); // seed_kos_pending_ will force all KOs to be written
    }
}
