#pragma once
#include "OpenKNX.h"

enum AirConditionMode
{
    AirConditionModeAuto = 0,
    AirConditionModeCool = 1,
    AirConditionModeHeat = 2,
    AirConditionModeDry = 3,
    AirConditionModeFan = 4,
};

class AirConditionDriverStatusFeedback
{
public:
    const char* NO_ERROR = nullptr;
    virtual void errorStateChanged(const char* error) = 0;
    virtual void powerChanged(bool power) = 0;
    virtual void modeChanged(AirConditionMode mode) = 0;
    virtual void targetTemperatureChanged(float temperaturCelius) = 0;
    virtual void fanSpeedChanged(int speed) = 0;
    virtual void swingHorizontalChanged(bool swing) = 0;
    virtual void swingVerticalChanged(bool swing) = 0;
    virtual void swingHorizontalFixPositionChanged(int position) = 0;
    virtual void swingVerticalFixPositionChanged(int position) = 0;
    virtual void roomTemperatureChanged(float temperaturCelius) = 0;
    virtual void outsideTemperaturChanged(float temperaturCelius) = 0;
};

class AirConditionDriver 
{
public:
    const int FAN_SPEED_AUTO = 0; 
protected:
    // Reference to the status feedback interface
    AirConditionDriverStatusFeedback& statusFeedback;

    AirConditionDriver(AirConditionDriverStatusFeedback& statusFeedback)
        : statusFeedback(statusFeedback)
    {
    }
public:
    // Lifecycle methods
    virtual void setup() const = 0;
    virtual void loop() const = 0;
    virtual void processInputKo(GroupObject &ko) {}
    virtual bool processCommand(const std::string cmd, bool debugKo) { return false; }

    // Information about the air condition device
    virtual const std::string name() const = 0;
    const std::string logPrefix() const 
    {
        return name();
    };
   
    virtual void showInformations() const = 0;
    virtual float getMinimumTargetTemperature() const = 0; 
    virtual float getMaximumTargetTemperature() const = 0;
    virtual unsigned int getMaximumFanSpeed() const = 0; 
    virtual unsigned int getMaximumHorizontalFixPosition() const = 0; 
    virtual unsigned int getMaximumVertiacalFixPosition() const = 0; 

    // Methods to control the air condition device
    virtual void setPower(bool power) = 0;
    virtual void setMode(AirConditionMode mode) = 0;
    virtual void setTargetTemperature(float temperaturCelius) = 0;
    virtual void setFanSpeed(unsigned int speed) = 0;
    virtual void setSwingHorizontal(bool swing) = 0;
    virtual void setSwingVertical(bool swing) = 0;
    virtual void setSwingHorizontalFixPosition(unsigned int position) = 0;
    virtual void setSwingVerticalFixPosition(unsigned int position) = 0;
    virtual void setExternalSensorRoomTemperature(float temperaturCelius) = 0;
};