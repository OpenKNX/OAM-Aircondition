#include "MitsubishiDriver.h"

MitsubishiDriver::MitsubishiDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : AirConditionDriver(statusFeedback)
{
}   

const std::string MitsubishiDriver::name() const
{
    return "Mitsubishi";
}

void MitsubishiDriver::showInformations()
{
   
}

void MitsubishiDriver::setup()
{
    // Initialization code for Mitsubishi air conditioner
}

void MitsubishiDriver::startCommunication(bool restart)
{
    // To Do: Implement the starting of the communication for Mitsubishi air conditioner
}

void MitsubishiDriver::requestAllData()
{
    // To Do: Implement the request for all data from Mitsubishi air conditioner
}

void MitsubishiDriver::loop()
{
    // Loop code for Mitsubishi air conditioner
}

float MitsubishiDriver::getMinimumTargetTemperature()
{
    return  (float) ParamAIR_Mit_MinTemp;
}
float MitsubishiDriver::getMaximumTargetTemperature()
{
    return 30.0f; // To Do (Mitsubishi): Check maximum temperature
}

unsigned int MitsubishiDriver::getMaximumFanSpeed()
{
    return 5; // To Do (Mitsubishi): Check maximum fan speed
}
unsigned int MitsubishiDriver::getMaximumHorizontalFixPosition()
{
    return 5; // To Do (Mitsubishi): Check maximum horizontal fix position
}
unsigned int MitsubishiDriver::getMaximumVertiacalFixPosition()
{
    return 5; // To Do (Mitsubishi): Check maximum vertical fix position
}

void MitsubishiDriver::setPower(bool power)
{
   // To Do (Mitsubishi): Implementation for power control
}

void MitsubishiDriver::setMode(AirConditionMode mode)
{
    // To Do (Mitsubishi): Implementation for mode control
}

void MitsubishiDriver::setTargetTemperature(float temperaturCelius)
{
    // To Do (Mitsubishi): Implementation for target temperature control
}

void MitsubishiDriver::setFanSpeed(unsigned int speed)
{
    // To Do (Mitsubishi): Implementation for fan speed control
}

void MitsubishiDriver::setSwingHorizontal(bool swing)
{
    // To Do (Mitsubishi): Implementation for horizontal swing control
}

void MitsubishiDriver::setSwingVertical(bool swing)
{
    // To Do (Mitsubishi): Implementation for vertical swing control
}

void MitsubishiDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // To Do (Mitsubishi): Implementation for horizontal fix position control
}

void MitsubishiDriver::setSwingVerticalFixPosition(unsigned int position)
{
    // To Do (Mitsubishi): Implementation for vertical fix position control
}

void MitsubishiDriver::setExternalSensorRoomTemperature(float temperaturCelius)
{
    // To Do (Mitsubishi): Implementation for external sensor room temperature control
}

void MitsubishiDriver::setWifiLed(bool on)
{
    // To Do (Mitsubishi): Implementation for WiFi LED control
}
