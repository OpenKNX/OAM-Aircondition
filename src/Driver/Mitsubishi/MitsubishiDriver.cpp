#include "MitsubishiDriver.h"
#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>

constexpr uint8_t MitsubishiDriver::QUERY_CYCLE[];

MitsubishiDriver::MitsubishiDriver(
    AirConditionDriverStatusFeedback &statusFeedback)
    : AirConditionDriver(statusFeedback) {}

const std::string MitsubishiDriver::name() const { return "Mitsubishi CN105"; }

void MitsubishiDriver::showInformations() {
  logInfoP("=== Mitsubishi CN105 Driver ===");
  logInfoP("Online: %s", online_ ? "Yes" : "No");
  logInfoP("Connect ACKed: %s", connectAcked_ ? "Yes" : "No");
  logInfoP("Power: %s", state_.power ? "On" : "Off");

  const char *modeStr = "Unknown";
  switch (state_.modeRaw) {
  case mit::MODE_HEAT:
    modeStr = "Heat";
    break;
  case mit::MODE_DRY:
    modeStr = "Dry";
    break;
  case mit::MODE_COOL:
    modeStr = "Cool";
    break;
  case mit::MODE_FAN:
    modeStr = "Fan";
    break;
  case mit::MODE_AUTO:
    modeStr = "Auto";
    break;
  }
  logInfoP("Mode: %s", modeStr);
  logInfoP("Target: %.1f°C", state_.targetC);
  logInfoP("Room:   %.1f°C", state_.roomC);
  logInfoP("Fan raw: 0x%02X (knx step %u)", state_.fanRaw,
           mit::mit_to_openknx_fan(state_.fanRaw));
  logInfoP("VaneV raw: 0x%02X", state_.vaneVRaw);
  logInfoP("WideVane raw: 0x%02X", state_.wideVaneRaw);
  logInfoP("Operating: %s, compressor: %u Hz", state_.operating ? "Yes" : "No",
           state_.compressorHz);
  logInfoP("Stats: ok=%u badCRC=%u badFmt=%u to=%u connects=%u",
           stats_.framesOk, stats_.framesBadChecksum, stats_.framesBadFormat,
           stats_.timeouts, stats_.connectsSent);
}

void MitsubishiDriver::setup() {
#ifdef OPENKNX_AIR_CONDITION_SERIAL
  HardwareSerial &ser = OPENKNX_AIR_CONDITION_SERIAL;
#else
  HardwareSerial &ser = Serial1;
#endif

#ifdef OPENKNX_AIR_CONDITION_SERIAL_RX
  int rx_pin = OPENKNX_AIR_CONDITION_SERIAL_RX;
#else
  int rx_pin = 16;
#endif

#ifdef OPENKNX_AIR_CONDITION_SERIAL_TX
  int tx_pin = OPENKNX_AIR_CONDITION_SERIAL_TX;
#else
  int tx_pin = 17;
#endif

  // Mitsubishi CN105 needs non-inverted, plain 8E1 at 2400 baud.
  int actual_rx = std::abs(rx_pin);
  int actual_tx = std::abs(tx_pin);

  logInfoP("CN105 Serial Config: RX=Pin%d, TX=Pin%d, 2400 8E1", actual_rx,
           actual_tx);

  serial_ = std::unique_ptr<mit::CN105Serial>(new mit::CN105Serial(
      ser, actual_rx, actual_tx,
      [this](mit::CN105Serial::Result r, const uint8_t *d, size_t n,
             uint8_t t) { onSerialResult(r, d, n, t); },
      []() { /* idle: nothing yet */ }));

  serial_->begin();
  statusFeedback.driverStateChanged(
      AirConditionDriverState::AirConditionDriverStateOk);
}

void MitsubishiDriver::resetStateForRestart() {
  state_ = mit::State{};
  pending_ = mit::PendingSettings{};
  stats_ = mit::Stats{};
  connectAcked_ = false;
  online_ = false;
  startupOfflinePublished_ = false;
  lastRxOkMs_ = 0;
  lastConnectMs_ = 0;
  lastQueryMs_ = 0;
  queryIndex_ = 0;
}

void MitsubishiDriver::startCommunication(bool restart) {
  if (!serial_) {
    logErrorP("CN105 driver not initialized");
    statusFeedback.driverStateChanged(
        AirConditionDriverState::AirConditionDriverStateError,
        "Driver not initialized");
    return;
  }

  statusFeedback.driverStateChanged(
      AirConditionDriverState::AirConditionDriverStateStarting);

  if (restart)
    resetStateForRestart();

  commStartMs_ = millis();
  if (commStartMs_ == 0)
    commStartMs_ = 1; // avoid 0 sentinel
  connectAcked_ = false;
  online_ = false;
  startupOfflinePublished_ = false;
  lastRxOkMs_ = 0;
  lastQueryMs_ = 0;
  queryIndex_ = 0;

  sendConnect();
}

void MitsubishiDriver::requestAllData() {
  queryIndex_ = 0;
  lastQueryMs_ = 0; // trigger immediate query in loop
}

void MitsubishiDriver::loop() {
  if (!serial_)
    return;

  serial_->loop();

  const uint32_t now = millis();

  // Connect handshake retry until we got CONNECT_RESP.
  if (!connectAcked_) {
    if (lastConnectMs_ != 0 && (now - lastConnectMs_) > CONNECT_RETRY_MS &&
        !serial_->isBusyTx() && txGapElapsed(now)) {
      sendConnect();
    }
  } else if (!serial_->isBusyTx() && txGapElapsed(now)) {
    // Priority 1: flush staged SET commands (user-initiated, time-sensitive).
    if (hasPendingSet()) {
      flushPendingSet();
    }
    // Priority 2: round-robin info polling.
    else if (lastQueryMs_ == 0 || (now - lastQueryMs_) > QUERY_INTERVAL_MS) {
      sendNextQuery();
    }
  }

  checkOnlineTimeout();
}

void MitsubishiDriver::sendConnect() {
  uint8_t buf[8];
  size_t n = mit::buildConnect(buf);
  serial_->setRxTimeoutMs(2000);
  serial_->sendFrame(buf, n);
  lastConnectMs_ = millis();
  lastTxMs_ = lastConnectMs_;
  stats_.connectsSent++;
  logDebugP("CN105 CONNECT sent");
}

void MitsubishiDriver::sendNextQuery() {
  uint8_t mode = QUERY_CYCLE[queryIndex_ % QUERY_CYCLE_LEN];
  queryIndex_ = (queryIndex_ + 1) % QUERY_CYCLE_LEN;

  uint8_t buf[mit::PACKET_LEN_STANDARD];
  mit::buildInfoRequest(buf, mode);
  serial_->setRxTimeoutMs(1500);
  serial_->sendFrame(buf, mit::PACKET_LEN_STANDARD);
  lastQueryMs_ = millis();
  lastTxMs_ = lastQueryMs_;
}

void MitsubishiDriver::flushPendingSet() {
  if (!hasPendingSet())
    return;

  uint8_t buf[mit::PACKET_LEN_STANDARD];
  mit::buildSetPacket(buf, pending_);
  serial_->setRxTimeoutMs(1500);
  serial_->sendFrame(buf, mit::PACKET_LEN_STANDARD);
  lastTxMs_ = millis();

  // Build a log line that only mentions the fields actually being transmitted.
  char fields[160] = {0};
  char *p = fields;
  char *end = fields + sizeof(fields);
  auto append = [&](const char *fmt, ...) {
    if (p >= end)
      return;
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(p, end - p, fmt, ap);
    va_end(ap);
    if (n > 0)
      p += n;
  };
  if (pending_.flags1 & mit::FLAG1_POWER)
    append(" power=%d", pending_.power);
  if (pending_.flags1 & mit::FLAG1_MODE)
    append(" mode=0x%02X", pending_.mode);
  if (pending_.flags1 & mit::FLAG1_TEMP)
    append(" temp=%.1f", pending_.targetC);
  if (pending_.flags1 & mit::FLAG1_FAN)
    append(" fan=0x%02X", pending_.fan);
  if (pending_.flags1 & mit::FLAG1_VANE)
    append(" vaneV=0x%02X", pending_.vaneV);
  if (pending_.flags2 & mit::FLAG2_WVANE)
    append(" wvane=0x%02X", pending_.wideVane);
  logInfoP("CN105 SET flags1=0x%02X flags2=0x%02X%s", pending_.flags1,
           pending_.flags2, fields);

  // Mirror pending into state so subsequent setters see current values, then
  // clear.
  if (pending_.flags1 & mit::FLAG1_POWER)
    state_.power = pending_.power;
  if (pending_.flags1 & mit::FLAG1_MODE)
    state_.modeRaw = pending_.mode;
  if (pending_.flags1 & mit::FLAG1_TEMP)
    state_.targetC = pending_.targetC;
  if (pending_.flags1 & mit::FLAG1_FAN)
    state_.fanRaw = pending_.fan;
  if (pending_.flags1 & mit::FLAG1_VANE)
    state_.vaneVRaw = pending_.vaneV;
  if (pending_.flags2 & mit::FLAG2_WVANE)
    state_.wideVaneRaw = pending_.wideVane;

  pending_.flags1 = 0;
  pending_.flags2 = 0;
}

void MitsubishiDriver::optimisticPublishPower() {
  statusFeedback.powerChanged(pending_.power);
}
void MitsubishiDriver::optimisticPublishMode() {
  statusFeedback.modeChanged(mit::mit_to_openknx_mode(pending_.mode));
}
void MitsubishiDriver::optimisticPublishTargetTemp() {
  statusFeedback.targetTemperatureChanged(pending_.targetC, true);
}
void MitsubishiDriver::optimisticPublishFan() {
  statusFeedback.fanSpeedChanged(
      static_cast<int>(mit::mit_to_openknx_fan(pending_.fan)));
}
void MitsubishiDriver::optimisticPublishVaneV() {
  statusFeedback.swingVerticalChanged(mit::is_vane_v_swing(pending_.vaneV));
  statusFeedback.swingVerticalFixPositionChanged(
      static_cast<int>(mit::mit_to_openknx_vane_v_pos(pending_.vaneV)));
}
void MitsubishiDriver::optimisticPublishWideVane() {
  statusFeedback.swingHorizontalChanged(
      mit::is_wide_vane_swing(pending_.wideVane));
  statusFeedback.swingHorizontalFixPositionChanged(
      static_cast<int>(mit::mit_to_openknx_wide_vane_pos(pending_.wideVane)));
}

void MitsubishiDriver::onSerialResult(mit::CN105Serial::Result r,
                                      const uint8_t *data, size_t len,
                                      uint8_t type) {
  switch (r) {
  case mit::CN105Serial::Result::Frame: {
    stats_.framesOk++;
    markOnline();

    switch (type) {
    case mit::TYPE_CONNECT_RESP:
      if (!connectAcked_) {
        connectAcked_ = true;
        logInfoP("CN105 CONNECT_RESP received");
        requestAllData();
      }
      break;

    case mit::TYPE_GET_RESP: {
      if (len < 1)
        break;
      uint8_t info = data[0];
      if (info == mit::INFO_SETTINGS) {
        if (mit::applySettingsResponse(data, len, state_))
          publishSettingsToFeedback();
      } else if (info == mit::INFO_ROOM_TEMP) {
        if (mit::applyRoomTempResponse(data, len, state_))
          publishRoomTempToFeedback();
      } else if (info == mit::INFO_STATUS) {
        mit::applyStatusResponse(data, len, state_);
        // Action/operating reporting has no direct KO in base interface (v1
        // scope).
      }
      // INFO_STANDBY/0x09 ignored for v1
      break;
    }

    case mit::TYPE_SET_RESP:
      // Confirmation only; settings will be re-read on next SETTINGS poll.
      break;

    default:
      logDebugP("CN105 frame with unhandled type 0x%02X", type);
      break;
    }
    break;
  }

  case mit::CN105Serial::Result::Timeout: {
    stats_.timeouts++;
    const size_t rxBytes = serial_->rxBytesSinceTx();
    if (rxBytes == 0) {
      logInfoP("CN105 RX timeout — no bytes received (check "
               "wiring/voltage/TX-RX swap)");
    } else {
      logInfoP("CN105 RX timeout — %u bytes received but no valid frame (first "
               "byte 0x%02X). Likely baud/parity/inversion mismatch.",
               (unsigned)rxBytes, (unsigned)serial_->firstRxByteSinceTx());
    }
    break;
  }

  case mit::CN105Serial::Result::BadChecksum:
    stats_.framesBadChecksum++;
    logDebugP("CN105 bad checksum");
    break;

  case mit::CN105Serial::Result::BadFormat:
    stats_.framesBadFormat++;
    break;
  }
}

void MitsubishiDriver::markOnline() {
  lastRxOkMs_ = millis();
  if (!online_) {
    online_ = true;
    startupOfflinePublished_ = false;
    statusFeedback.driverStateChanged(
        AirConditionDriverState::AirConditionDriverStateOk);
    statusFeedback.updateOnlineStatus(true);
    logInfoP("CN105 online");
  }
}

void MitsubishiDriver::checkOnlineTimeout() {
  const uint32_t now = millis();

  if (online_ && lastRxOkMs_ != 0 && (now - lastRxOkMs_) > OFFLINE_AFTER_MS) {
    online_ = false;
    connectAcked_ = false;
    statusFeedback.driverStateChanged(
        AirConditionDriverState::AirConditionDriverStateError, "No response");
    statusFeedback.updateOnlineStatus(false);
    logInfoP("CN105 went offline after %lu ms silence",
             (unsigned long)(now - lastRxOkMs_));
    return;
  }

  if (!online_ && !startupOfflinePublished_ && commStartMs_ != 0 &&
      (now - commStartMs_) > STARTUP_OFFLINE_TIMEOUT_MS) {
    startupOfflinePublished_ = true;
    statusFeedback.driverStateChanged(
        AirConditionDriverState::AirConditionDriverStateError,
        "No response after startup");
    statusFeedback.updateOnlineStatus(false);
    logInfoP("CN105 startup timeout — publishing offline default");
  }
}

void MitsubishiDriver::publishSettingsToFeedback() {
  statusFeedback.powerChanged(state_.power);
  statusFeedback.modeChanged(mit::mit_to_openknx_mode(state_.modeRaw));
  statusFeedback.targetTemperatureChanged(state_.targetC, false);
  statusFeedback.fanSpeedChanged(
      static_cast<int>(mit::mit_to_openknx_fan(state_.fanRaw)));
  statusFeedback.swingVerticalChanged(mit::is_vane_v_swing(state_.vaneVRaw));
  statusFeedback.swingVerticalFixPositionChanged(
      static_cast<int>(mit::mit_to_openknx_vane_v_pos(state_.vaneVRaw)));
  statusFeedback.swingHorizontalChanged(
      mit::is_wide_vane_swing(state_.wideVaneRaw));
  statusFeedback.swingHorizontalFixPositionChanged(
      static_cast<int>(mit::mit_to_openknx_wide_vane_pos(state_.wideVaneRaw)));
}

void MitsubishiDriver::publishRoomTempToFeedback() {
  statusFeedback.roomTemperatureChanged(state_.roomC);
}

// === Capability queries ====================================================
float MitsubishiDriver::getMinimumTargetTemperature() {
  uint8_t v = ParamAIR_Mit_MinTemp;
  if (v < 10 || v > 30)
    v = 16;
  return static_cast<float>(v);
}

float MitsubishiDriver::getMaximumTargetTemperature() { return 30.0f; }

unsigned int MitsubishiDriver::getMaximumFanSpeed() {
  return 5; // 0=auto, 1=quiet, 2..5 = Mitsubishi 1..4
}

unsigned int MitsubishiDriver::getMaximumHorizontalFixPosition() { return 5; }

unsigned int MitsubishiDriver::getMaximumVerticalFixPosition() { return 5; }

unsigned int MitsubishiDriver::getMaximumHumidityModeLevels() { return 0; }

bool MitsubishiDriver::supportExternalRoomTemperatureSensor() { return true; }

float MitsubishiDriver::accuracyInDegrees() { return 0.5f; }

// === Control methods ========================================================
// Setters only stage the change and publish optimistically. The actual SET
// frame is built and transmitted from loop() under the 300 ms TX-gap and the
// "no in-flight RX" constraint, which lets multiple setter calls in the same
// loop iteration batch into a single SET packet.

void MitsubishiDriver::setPower(bool power) {
  pending_.power = power;
  pending_.flags1 |= mit::FLAG1_POWER;
  optimisticPublishPower();
}

void MitsubishiDriver::setMode(AirConditionMode mode) {
  pending_.mode = mit::openknx_to_mit_mode(mode);
  pending_.flags1 |= mit::FLAG1_MODE;
  optimisticPublishMode();
}

void MitsubishiDriver::setTargetTemperature(float temperaturCelsius) {
  float t = std::clamp(temperaturCelsius, getMinimumTargetTemperature(),
                       getMaximumTargetTemperature());
  pending_.targetC = t;
  pending_.flags1 |= mit::FLAG1_TEMP;
  optimisticPublishTargetTemp();
}

void MitsubishiDriver::setFanSpeed(unsigned int speed) {
  if (speed > getMaximumFanSpeed())
    speed = 0;
  pending_.fan = mit::openknx_to_mit_fan(speed);
  pending_.flags1 |= mit::FLAG1_FAN;
  optimisticPublishFan();
}

void MitsubishiDriver::setSwingVertical(bool swing) {
  if (swing) {
    pending_.vaneV = mit::VANE_V_SWING;
  } else {
    pending_.vaneV = (state_.vaneVRaw == mit::VANE_V_SWING) ? mit::VANE_V_AUTO
                                                            : state_.vaneVRaw;
  }
  pending_.flags1 |= mit::FLAG1_VANE;
  optimisticPublishVaneV();
}

void MitsubishiDriver::setSwingHorizontal(bool swing) {
  if (swing) {
    pending_.wideVane = mit::WVANE_SWING;
  } else {
    pending_.wideVane = mit::is_wide_vane_swing(state_.wideVaneRaw)
                            ? mit::WVANE_MID
                            : state_.wideVaneRaw;
  }
  pending_.flags2 |= mit::FLAG2_WVANE;
  optimisticPublishWideVane();
}

void MitsubishiDriver::setSwingVerticalFixPosition(unsigned int position) {
  pending_.vaneV =
      (position == 0) ? mit::VANE_V_AUTO : mit::openknx_to_mit_vane_v(position);
  pending_.flags1 |= mit::FLAG1_VANE;
  optimisticPublishVaneV();
}

void MitsubishiDriver::setSwingHorizontalFixPosition(unsigned int position) {
  pending_.wideVane = (position == 0) ? mit::WVANE_MID
                                      : mit::openknx_to_mit_wide_vane(position);
  pending_.flags2 |= mit::FLAG2_WVANE;
  optimisticPublishWideVane();
}

void MitsubishiDriver::setExternalSensorRoomTemperature(
    float /*temperaturCelsius*/) {
  // CN105 supports a remote-temperature packet (different info_mode), not
  // wired in v1. Tracked as TODO for v2 — see plan offene Punkte.
  logInfoP("setExternalSensorRoomTemperature: not implemented in v1");
}

void MitsubishiDriver::setWifiLed(bool /*on*/) {
  logInfoP("setWifiLed: not supported by CN105");
}

void MitsubishiDriver::setDeviceMode(AirConditionDeviceMode /*mode*/) {
  logInfoP("setDeviceMode: not supported by CN105");
}

void MitsubishiDriver::setMaxPowerLevel(uint8_t /*percentage*/) {
  logInfoP("setMaxPowerLevel: not supported by CN105");
}

void MitsubishiDriver::setAirPurification(bool /*on*/) {
  logInfoP("setAirPurification: not supported by CN105");
}

// === Inbound update passthroughs ===========================================
void MitsubishiDriver::updatePower(bool power) {
  statusFeedback.updatePower(power);
}
void MitsubishiDriver::updateMode(AirConditionMode mode) {
  statusFeedback.updateMode(mode);
}
void MitsubishiDriver::updateTargetTemperature(float temperaturCelsius) {
  statusFeedback.updateTargetTemperature(temperaturCelsius);
}
void MitsubishiDriver::updateFanSpeed(int speed) {
  statusFeedback.updateFanSpeed(speed);
}
void MitsubishiDriver::updateSwingHorizontal(bool swing) {
  statusFeedback.updateSwingHorizontal(swing);
}
void MitsubishiDriver::updateSwingVertical(bool swing) {
  statusFeedback.updateSwingVertical(swing);
}
void MitsubishiDriver::updateCurrentTemperature(float temperaturCelsius) {
  statusFeedback.updateCurrentTemperature(temperaturCelsius);
}
void MitsubishiDriver::updateOutdoorTemperature(float temperaturCelsius) {
  statusFeedback.updateOutdoorTemperature(temperaturCelsius);
}
void MitsubishiDriver::updateDeviceMode(AirConditionDeviceMode mode) {
  statusFeedback.updateDeviceMode(mode);
}
void MitsubishiDriver::updateMaxPowerLevel(uint8_t percentage) {
  statusFeedback.updateMaxPowerLevel(percentage);
}
void MitsubishiDriver::updateAirPurification(bool on) {
  statusFeedback.updateAirPurification(on);
}
void MitsubishiDriver::updateOnlineStatus(bool online) {
  statusFeedback.updateOnlineStatus(online);
}
void MitsubishiDriver::updateWifiLed(bool on) {
  statusFeedback.updateWifiLed(on);
}
void MitsubishiDriver::updateHumidity(uint8_t humidity) {
  statusFeedback.updateHumidity(humidity);
}
void MitsubishiDriver::updateHumidityMode(uint8_t humidityMode) {
  statusFeedback.updateHumidityMode(humidityMode);
}
void MitsubishiDriver::updateTotalEnergyConsumption(uint32_t totalEnergyWh) {
  statusFeedback.updateTotalEnergyConsumption(totalEnergyWh);
}
