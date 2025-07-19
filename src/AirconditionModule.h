#pragma once
#include "AirConditionDriver.h"

class SceneHandler;

class AirconditionModule : public OpenKNX::Module, AirConditionDriverStatusFeedback
{
    AirConditionMode _lastMode = AirConditionMode::AirConditionModeAuto;
    AirConditionDriver* _airConditionDriver = nullptr;
    SceneHandler* _sceneHandler = nullptr;
    AirConditionDriverState _driverState = AirConditionDriverState::AirConditionDriverStateNotStarted;
    unsigned long _errorSince = 0;
    std::string _errorMessage = "";
    bool _initialDataNeeded = false;
    
    void setLocked(bool locked);

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
    void targetTemperatureChanged(float temperaturCelius) override;
    void fanSpeedChanged(int speed) override;
    void swingHorizontalChanged(bool swing) override;
    void swingVerticalChanged(bool swing) override;
    void swingHorizontalFixPositionChanged(int position) override;
    void swingVerticalFixPositionChanged(int position) override;
    void roomTemperatureChanged(float temperaturCelius) override;
    void outsideTemperaturChanged(float temperaturCelius) override;
    void driverStateChanged(AirConditionDriverState state, std::string error = "") override;
    AirConditionDriverState getDriverState() const;
};

extern AirconditionModule openknxAircondition;
