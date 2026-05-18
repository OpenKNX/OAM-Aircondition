#pragma once

#include "../../AirConditionDriver.h"

#include <string>
#include <vector>

class TclDriver : public AirConditionDriver
{
  private:
    static constexpr uint8_t Header = 0xBB;
    static constexpr size_t PollCommandLength = 8;
    static constexpr size_t ControlCommandLength = 35;
    static constexpr size_t MaximumMessageLength = 80;
    static constexpr uint32_t PollIntervalMs = 5000;
    static constexpr uint32_t ReceiveTimeoutMs = 200;
    static constexpr uint32_t PollAfterCommandDelayMs = 1000;
    static constexpr uint32_t ControlDebounceMs = 100;

    static const uint8_t PollCommand[PollCommandLength];

    std::vector<uint8_t> _rxBuffer;
    unsigned long _lastPoll = 0;
    unsigned long _lastCommand = 0;
    unsigned long _lastReceivedByte = 0;
    bool _controlPending = false;
    unsigned long _controlSendAt = 0;

    bool _power = false;
    AirConditionMode _mode = AirConditionMode::AirConditionModeAuto;
    float _targetTemperature = 24.0f;
    uint8_t _fanSpeed = 0;
    bool _verticalSwing = false;
    bool _horizontalSwing = false;
    uint8_t _verticalFixPosition = 0;
    uint8_t _horizontalFixPosition = 0;
    bool _display = true;
    bool _beep = true;
    bool _eco = false;
    bool _health = false;
    bool _turbo = false;
    bool _lowNoise = false;
    uint8_t _sleepMode = 0;

    bool _onlineReadInProgress = false;
    bool _onlineReadDone = false;
    std::string _lastRawDump;
    std::string _lastDecodedDump;

    void sendPoll();
    void requestControlUpdate();
    void processPendingControlUpdate();
    void sendControl();
    void handleReceivedByte(uint8_t value);
    void parseStatus(const std::vector<uint8_t>& rawData);
    uint8_t checksum(const uint8_t* data, size_t length) const;
    uint8_t encodeMode(AirConditionMode mode) const;
    uint8_t encodeFanSpeed(uint8_t speed) const;
    AirConditionMode decodeMode(uint8_t rawMode) const;
    uint8_t decodeFanSpeed(uint8_t rawFan) const;
    bool decodeVerticalFixPosition(uint8_t rawPosition, uint8_t& position) const;
    bool decodeHorizontalFixPosition(uint8_t rawPosition, uint8_t& position) const;
    uint8_t clampFanSpeed(unsigned int speed) const;
    float clampTargetTemperature(float temperature) const;
    const char* modeToText(AirConditionMode mode) const;
    const char* sleepModeToText(uint8_t mode) const;
    const char* extendedModeToText(uint8_t mode) const;
    bool isValidExtendedModeMask(uint8_t mode) const;
    uint8_t currentExtendedMode() const;
    void applyExtendedMode(uint8_t mode);
    void updateTclFeatureStatusObjects();
    void setSleepMode(uint8_t mode);
    void setEcoMode(bool enabled);
    void setDisplay(bool enabled);
    void setBuzzer(bool enabled);
    void setExtendedMode(uint8_t mode);
    void setHealthMode(bool enabled);
    void setTurboMode(bool enabled);
    void setLowNoiseMode(bool enabled);
    std::string formatStatusDump(const std::vector<uint8_t>& rawData,
                                 bool power,
                                 AirConditionMode mode,
                                 float targetTemperature,
                                 uint8_t fanSpeed,
                                 bool horizontalSwing,
                                 bool verticalSwing,
                                 uint8_t horizontalFixPosition,
                                 uint8_t verticalFixPosition,
                                 float roomTemperature) const;

  public:
    TclDriver(AirConditionDriverStatusFeedback& statusFeedback);

    void setup() override;
    void startCommunication(bool restart) override;
    void requestAllData() override;
    void loop() override;
    void processInputKo(GroupObject& ko) override;
    bool readLiveDataDump(std::string& rawDump, std::string& decodedDump, uint32_t timeoutMs = 2000) override;

    const std::string name() const override;
    void showInformations() override;
    float getMinimumTargetTemperature() override;
    float getMaximumTargetTemperature() override;
    unsigned int getMaximumFanSpeed() override;
    unsigned int getMaximumHorizontalFixPosition() override;
    unsigned int getMaximumVerticalFixPosition() override;
    unsigned int getMaximumHumidityModeLevels() override;
    bool supportExternalRoomTemperatureSensor() override;
    float accuracyInDegrees() override;

    void setPower(bool power) override;
    void setMode(AirConditionMode mode) override;
    void setTargetTemperature(float temperaturCelsius) override;
    void setFanSpeed(unsigned int speed) override;
    void setSwingHorizontal(bool swing) override;
    void setSwingVertical(bool swing) override;
    void setSwingHorizontalFixPosition(unsigned int position) override;
    void setSwingVerticalFixPosition(unsigned int position) override;
    void setExternalSensorRoomTemperature(float temperaturCelsius) override;
    void setWifiLed(bool on) override;
    void setDeviceMode(AirConditionDeviceMode mode) override;
    void setMaxPowerLevel(uint8_t percentage) override;
    void setAirPurification(bool on) override;
};
