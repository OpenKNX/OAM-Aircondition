#pragma once
#include "AirConditionDriver.h"

class AirconditionModule : public OpenKNX::Module, AirConditionDriverStatusFeedback
{
    AirConditionDriver* airConditionDriver = nullptr;
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
    void errorStateChanged(const char* error) override;
};

extern AirconditionModule openknxAircondition;
