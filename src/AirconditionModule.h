#pragma once
#include "AirConditionDriver.h"
#include "RoomTemperaturCorrection.h"

class SceneHandler;

class AirconditionModule : public OpenKNX::Module, AirConditionDriverStatusFeedback
{
    RoomTemperatureCorrection* _roomTemperatureCorrection = nullptr;
    AirConditionMode _lastMode = AirConditionMode::AirConditionModeAuto;
    bool _lastPower = false;
    bool _lastWifiLedState = true;
    bool _forceWifiLedState = false;
    unsigned long _lastWifiLedDebounceRunning = 0;
    bool _waitingForCooling = false;
    bool _waitingForHeating = false;
    bool _waitingForFan = false;
    bool _waitingForDehumidification = false;
    bool _waitingForAuto = false;
    unsigned long _waitingForModeChange = 0;

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


};

extern AirconditionModule openknxAircondition;
