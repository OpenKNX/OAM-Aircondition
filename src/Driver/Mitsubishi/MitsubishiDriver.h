#pragma once
#include "../../AirConditionDriver.h"
#include "cn105_packet.h"
#include "cn105_serial.h"
#include "cn105_types.h"
#include <memory>

class MitsubishiDriver : public AirConditionDriver {
public:
  MitsubishiDriver(AirConditionDriverStatusFeedback &statusFeedback);

  virtual void setup() override;
  virtual void startCommunication(bool restart) override;
  virtual void requestAllData() override;
  virtual void loop() override;

  virtual const std::string name() const override;
  virtual void showInformations() override;
  virtual float getMinimumTargetTemperature() override;
  virtual float getMaximumTargetTemperature() override;
  virtual unsigned int getMaximumFanSpeed() override;
  virtual unsigned int getMaximumHorizontalFixPosition() override;
  virtual unsigned int getMaximumVerticalFixPosition() override;
  virtual unsigned int getMaximumHumidityModeLevels() override;
  virtual bool supportExternalRoomTemperatureSensor() override;
  virtual float accuracyInDegrees() override;

  virtual void setPower(bool power) override;
  virtual void setMode(AirConditionMode mode) override;
  virtual void setTargetTemperature(float temperaturCelsius) override;
  virtual void setFanSpeed(unsigned int speed) override;
  virtual void setSwingHorizontal(bool swing) override;
  virtual void setSwingVertical(bool swing) override;
  virtual void setSwingHorizontalFixPosition(unsigned int position) override;
  virtual void setSwingVerticalFixPosition(unsigned int position) override;
  virtual void
  setExternalSensorRoomTemperature(float temperaturCelsius) override;
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

private:
  // Serial + protocol state
  std::unique_ptr<mit::CN105Serial> serial_;
  mit::State state_{};
  mit::PendingSettings pending_{};
  mit::Stats stats_{};

  // Lifecycle
  bool connectAcked_{false};
  bool online_{false};
  bool startupOfflinePublished_{false};
  uint32_t commStartMs_{0};
  uint32_t lastRxOkMs_{0};
  uint32_t lastConnectMs_{0};
  uint32_t lastQueryMs_{0};
  uint32_t lastTxMs_{0};
  uint8_t queryIndex_{0};

  // Inter-query and timeout windows
  static constexpr uint32_t QUERY_INTERVAL_MS = 250;
  static constexpr uint32_t STARTUP_OFFLINE_TIMEOUT_MS = 15000;
  static constexpr uint32_t OFFLINE_AFTER_MS = 15000;
  static constexpr uint32_t CONNECT_RETRY_MS = 3000;
  // Reference enforces >300 ms between any TX (hp_writings.cpp
  // sendWantedSettings).
  static constexpr uint32_t MIN_TX_GAP_MS = 320;

  // Round-robin info queries
  static constexpr uint8_t QUERY_CYCLE[] = {
      mit::INFO_SETTINGS,
      mit::INFO_ROOM_TEMP,
      mit::INFO_STATUS,
      mit::INFO_STANDBY,
  };
  static constexpr uint8_t QUERY_CYCLE_LEN =
      sizeof(QUERY_CYCLE) / sizeof(QUERY_CYCLE[0]);

  // Internal helpers
  void onSerialResult(mit::CN105Serial::Result r, const uint8_t *data,
                      size_t len, uint8_t type);
  void sendConnect();
  void sendNextQuery();
  void flushPendingSet();
  bool hasPendingSet() const {
    return pending_.flags1 != 0 || pending_.flags2 != 0;
  }
  bool txGapElapsed(uint32_t now) const {
    return lastTxMs_ == 0 || (now - lastTxMs_) >= MIN_TX_GAP_MS;
  }
  void optimisticPublishPower();
  void optimisticPublishMode();
  void optimisticPublishTargetTemp();
  void optimisticPublishFan();
  void optimisticPublishVaneV();
  void optimisticPublishWideVane();
  void publishSettingsToFeedback();
  void publishRoomTempToFeedback();
  void checkOnlineTimeout();
  void markOnline();
  void resetStateForRestart();
};
