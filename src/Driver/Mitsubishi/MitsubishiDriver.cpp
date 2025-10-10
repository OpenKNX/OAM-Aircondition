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
unsigned int MitsubishiDriver::getMaximumVerticalFixPosition()
{
    return 5; // To Do (Mitsubishi): Check maximum vertical fix position
}
unsigned int MitsubishiDriver::getMaximumHumidityModeLevels()
{
    return 0; 
}

void MitsubishiDriver::setPower(bool power)
{
   // To Do (Mitsubishi): Implementation for power control
}

void MitsubishiDriver::setMode(AirConditionMode mode)
{
    // To Do (Mitsubishi): Implementation for mode control
}

void MitsubishiDriver::setTargetTemperature(float temperaturCelsius)
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

void MitsubishiDriver::setExternalSensorRoomTemperature(float temperaturCelsius)
{
    // To Do (Mitsubishi): Implementation for external sensor room temperature control
}

void MitsubishiDriver::setWifiLed(bool on)
{
    // To Do (Mitsubishi): Implementation for WiFi LED control
}

bool MitsubishiDriver::supportExternalRoomTemperatureSensor()
{
    return true; 
}

float MitsubishiDriver::accuracyInDegrees()
{
    // To Do (Mitsubishi): Implement rounding of temperature to the air condition resolution
    return 1.f; // Assuming 1 degree Celsius accuracy
}

void MitsubishiDriver::setDeviceMode(AirConditionDeviceMode mode)
{
    // To Do (Mitsubishi): Implementation for device mode control
}

void MitsubishiDriver::setMaxPowerLevel(uint8_t percentage)
{
    // To Do (Mitsubishi): Implementation for max power level control
}

void MitsubishiDriver::setAirPurification(bool on)
{
    // To Do (Mitsubishi): Implementation for air purification control
}

void MitsubishiDriver::updatePower(bool power) { statusFeedback.updatePower(power); }
void MitsubishiDriver::updateMode(AirConditionMode mode) { statusFeedback.updateMode(mode); }
void MitsubishiDriver::updateTargetTemperature(float temperaturCelsius) { statusFeedback.updateTargetTemperature(temperaturCelsius); }
void MitsubishiDriver::updateFanSpeed(int speed) { statusFeedback.updateFanSpeed(speed); }
void MitsubishiDriver::updateSwingHorizontal(bool swing) { statusFeedback.updateSwingHorizontal(swing); }
void MitsubishiDriver::updateSwingVertical(bool swing) { statusFeedback.updateSwingVertical(swing); }
void MitsubishiDriver::updateCurrentTemperature(float temperaturCelsius) { statusFeedback.updateCurrentTemperature(temperaturCelsius); }
void MitsubishiDriver::updateOutdoorTemperature(float temperaturCelsius) { statusFeedback.updateOutdoorTemperature(temperaturCelsius); }
void MitsubishiDriver::updateDeviceMode(AirConditionDeviceMode mode) { statusFeedback.updateDeviceMode(mode); }
void MitsubishiDriver::updateMaxPowerLevel(uint8_t percentage) { statusFeedback.updateMaxPowerLevel(percentage); }
void MitsubishiDriver::updateAirPurification(bool on) { statusFeedback.updateAirPurification(on); }
void MitsubishiDriver::updateOnlineStatus(bool online) { statusFeedback.updateOnlineStatus(online); }
void MitsubishiDriver::updateWifiLed(bool on) { statusFeedback.updateWifiLed(on); }
void MitsubishiDriver::updateHumidity(uint8_t humidity) { statusFeedback.updateHumidity(humidity); }
void MitsubishiDriver::updateHumidityMode(uint8_t humidityMode) { statusFeedback.updateHumidityMode(humidityMode); }
void MitsubishiDriver::updateTotalEnergyConsumption(uint32_t totalEnergyWh) { statusFeedback.updateTotalEnergyConsumption(totalEnergyWh); }