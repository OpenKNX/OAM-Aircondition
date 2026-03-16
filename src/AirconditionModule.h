#pragma once
#include "AirConditionDriver.h"

class SceneHandler;

class AirconditionModule : public OpenKNX::Module, AirConditionDriverStatusFeedback
{
    const int retryConnectDelay = 60000; // 60 seconds delay before retrying connection after an error
    AirConditionMode _lastMode = AirConditionMode::AirConditionModeAuto;
    bool _hasKnownMode = false;
    bool _lastPower = false;
    bool _lastWifiLedState = true;
    bool _forceWifiLedState = false;
    unsigned long _lastWifiLedDebounceRunning = 0;
    bool _waitingForCooling = false;
    bool _waitingForHeating = false;
    bool _waitingForFan = false;
    bool _waitingForDehumidification = false;
    bool _waitingForAuto = false;
    bool _lastAirConditionPower = false;
    unsigned long _waitingForModeChange = 0;
    bool _daikinFu04Valid = false;
    uint32_t _daikinFu04CoolingWh = 0;
    uint32_t _daikinFu04HeatingWh = 0;
    bool _daikinFx60Valid = false;
    uint32_t _daikinFx60Value10 = 0;

    AirConditionDriver* _airConditionDriver = nullptr;
    SceneHandler* _sceneHandler = nullptr;
    AirConditionDriverState _driverState = AirConditionDriverState::AirConditionDriverStateNotStarted;
    unsigned long _errorSince = 0;
    std::string _errorMessage = "";
    bool _initialDataNeeded = false;
   
    void setLocked(bool locked);
    void handleDebouncedModeChange();

    void setTargetTemperaturToAircondition(float temperature);

  public:
    const std::string name() override;
    const std::string version() override;
    void showInformations() override;
    void showHelp() override;
    bool processCommand(const std::string cmd, bool debugKo) override;
    void processInputKo(GroupObject &ko) override;
    void setup() override;
    void loop() override;
   

    // AirConditionDriverStatusFeedback interface
    void powerChanged(bool power) override;
    void modeChanged(AirConditionMode mode) override;
    void targetTemperatureChanged(float temperaturCelius, bool isFeedbackFromSettin) override;
    void fanSpeedChanged(int speed) override;
    void swingHorizontalChanged(bool swing) override;
    void swingVerticalChanged(bool swing) override;
    void swingHorizontalFixPositionChanged(int position) override;
    void swingVerticalFixPositionChanged(int position) override;
    void roomTemperatureChanged(float temperaturCelius) override;
    void outsideTemperaturChanged(float temperaturCelius) override;
    void driverStateChanged(AirConditionDriverState state, std::string error = "") override;
    void maxPowerLevelChanged(uint8_t maxPower) override;
    void deviceModeChanged(AirConditionDeviceMode mode) override;
    void airPurificationChanged(bool on) override;
    AirConditionDriverState getDriverState() const;

    void updatePower(bool power) override;
    void updateMode(AirConditionMode mode) override;
    void updateTargetTemperature(float temperaturCelsius) override;
    void updateFanSpeed(int speed) override;
    void updateSwingHorizontal(bool swing) override;
    void updateSwingVertical(bool swing) override;
    void updateCurrentTemperature(float temperaturCelsius) override;
    void updateOutdoorTemperature(float temperaturCelsius) override;
    void updateDeviceMode(AirConditionDeviceMode mode) override;
    void updateMaxPowerLevel(uint8_t percentage) override;
    void updateAirPurification(bool on) override;
    void updateOnlineStatus(bool online) override;
    void updateWifiLed(bool on) override;
    void updateHumidity(uint8_t humidity) override;
    void updateHumidityMode(uint8_t step) override;
    void updateTotalEnergyConsumption(uint32_t totalEnergyWh) override;
    void updateDaikinExtensionTelemetry(bool fu04Valid,
                                        uint32_t fu04CoolingWh,
                                        uint32_t fu04HeatingWh,
                                        bool fx60Valid,
                                        uint32_t fx60Value10) override;
};

extern AirconditionModule openknxAircondition;
