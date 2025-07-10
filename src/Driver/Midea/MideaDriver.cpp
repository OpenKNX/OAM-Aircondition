#include "MideaDriver.h"

MideaDriver::MideaDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : AirConditionDriver(statusFeedback)
{
}   

const std::string MideaDriver::name() const
{
    return "Midea";
}

void MideaDriver::showInformations() const
{
   
}

void MideaDriver::setup() const
{
    // Initialization code for Midea air conditioner
}

void MideaDriver::loop() const
{
    // Loop code for Midea air conditioner
}

float MideaDriver::getMinimumTargetTemperature() const
{
    return 17.0f;
}
float MideaDriver::getMaximumTargetTemperature() const
{
    return 30.0f;
}

unsigned int MideaDriver::getMaximumFanSpeed() const
{
    return 5; 
}
unsigned int MideaDriver::getMaximumHorizontalFixPosition() const
{
    return 5; 
}
unsigned int MideaDriver::getMaximumVertiacalFixPosition() const
{
    return 5; 
}

void MideaDriver::setPower(bool power)
{
   // To Do: Implementation for power control
   statusFeedback.powerChanged(power);
}

void MideaDriver::setMode(AirConditionMode mode)
{
    // To Do: Implementation for mode control
    statusFeedback.modeChanged(mode);
}

void MideaDriver::setTargetTemperature(float temperaturCelius)
{
    // To Do: Implementation for target temperature control
    statusFeedback.targetTemperatureChanged(temperaturCelius);
}

void MideaDriver::setFanSpeed(unsigned int speed)
{
    // To Do: Implementation for fan speed control
    statusFeedback.fanSpeedChanged(speed);
}

void MideaDriver::setSwingHorizontal(bool swing)
{
    // To Do: Implementation for horizontal swing control
    statusFeedback.swingHorizontalChanged(swing);
}

void MideaDriver::setSwingVertical(bool swing)
{
    // To Do: Implementation for vertical swing control
    statusFeedback.swingVerticalChanged(swing);
}

void MideaDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // To Do: Implementation for horizontal fix position control
    statusFeedback.swingHorizontalFixPositionChanged(position);
}

void MideaDriver::setSwingVerticalFixPosition(unsigned int position)
{
    // To Do: Implementation for vertical fix position control
    statusFeedback.swingVerticalFixPositionChanged(position);
}

void MideaDriver::setExternalSensorRoomTemperature(float temperaturCelius)
{
    // To Do: Implementation for external sensor room temperature control
    statusFeedback.roomTemperatureChanged(temperaturCelius);
}
