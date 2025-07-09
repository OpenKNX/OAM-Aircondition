#include "DaikinDriver.h"

DaikinDriver::DaikinDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : AirConditionDriver(statusFeedback)
{
}   

const std::string DaikinDriver::name() const
{
    return "Daikin";
}

void DaikinDriver::showInformations() const
{
   
}

void DaikinDriver::setup() const
{
    // Initialization code for Daikin air conditioner
}

void DaikinDriver::loop() const
{
    // Loop code for Daikin air conditioner
}

float DaikinDriver::getMinimumTargetTemperature() const
{
    return 17.0f; // To Do (Daikin): Check minimum temperature
}
float DaikinDriver::getMaximumTargetTemperature() const
{
    return 30.0f; // To Do (Daikin): Check maximum temperature
}

unsigned int DaikinDriver::getMaximumFanSpeed() const
{
    return 5; // To Do (Daikin): Check maximum fan speed
}
unsigned int DaikinDriver::getMaximumHorizontalFixPosition() const
{
    return 5; // To Do (Daikin): Check maximum horizontal fix position
}
unsigned int DaikinDriver::getMaximumVertiacalFixPosition() const
{
    return 5; // To Do (Daikin): Check maximum vertical fix position
}

void DaikinDriver::setPower(bool power)
{
   // To Do (Daikin): Implementation for power control
}

void DaikinDriver::setMode(AirConditionMode mode)
{
    // To Do (Daikin): Implementation for mode control
}

void DaikinDriver::setTargetTemperature(float temperaturCelius)
{
    // To Do (Daikin): Implementation for target temperature control
}

void DaikinDriver::setFanSpeed(unsigned int speed)
{
    // To Do (Daikin): Implementation for fan speed control
}

void DaikinDriver::setSwingHorizontal(bool swing)
{
    // To Do (Daikin): Implementation for horizontal swing control
}

void DaikinDriver::setSwingVertical(bool swing)
{
    // To Do (Daikin): Implementation for vertical swing control
}

void DaikinDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // To Do (Daikin): Implementation for horizontal fix position control
}

void DaikinDriver::setSwingVerticalFixPosition(unsigned int position)
{
    // To Do (Daikin): Implementation for vertical fix position control
}

void DaikinDriver::setExternalSensorRoomTemperature(float temperaturCelius)
{
    // To Do (Daikin): Implementation for external sensor room temperature control
}
