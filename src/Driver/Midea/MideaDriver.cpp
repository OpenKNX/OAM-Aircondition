#include "MideaDriver.h"

MideaDriver::MideaDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : AirConditionDriver(statusFeedback)
{
}   

const std::string MideaDriver::name() const
{
    return "Midea";
}

void MideaDriver::showInformations()
{
   
}

void MideaDriver::setup()
{
    // Initialization code for Midea air conditioner
}

void MideaDriver::startCommunication(bool restart)
{
    // To Do (Midea): Implement the starting of the communication for Midea air conditioner
    statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateOk);
}

void MideaDriver::requestAllData()
{
    // To Do (Midea): Implement the request for all data from Midea air conditioner
}

void MideaDriver::loop()
{
    // Loop code for Midea air conditioner
}

float MideaDriver::getMinimumTargetTemperature()
{
    return 17.0f;
}
float MideaDriver::getMaximumTargetTemperature()
{
    return 30.0f;
}

unsigned int MideaDriver::getMaximumFanSpeed()
{
    return 5; 
}
unsigned int MideaDriver::getMaximumHorizontalFixPosition()
{
    return 5; 
}
unsigned int MideaDriver::getMaximumVerticalFixPosition()
{
    return 5; 
}
unsigned int MideaDriver::getMaximumHumidityModeLevels()
{
    return 0; 
}

void MideaDriver::setPower(bool power)
{
   // To Do (Midea): Implementation for power control
   statusFeedback.powerChanged(power);
}

void MideaDriver::setMode(AirConditionMode mode)
{
    // To Do (Midea): Implementation for mode control
    statusFeedback.modeChanged(mode);
}

void MideaDriver::setTargetTemperature(float temperaturCelsius)
{
    // To Do (Midea): Implementation for target temperature control
    statusFeedback.targetTemperatureChanged(temperaturCelsius, true);
}

void MideaDriver::setFanSpeed(unsigned int speed)
{
    // To Do (Midea): Implementation for fan speed control
    statusFeedback.fanSpeedChanged(speed);
}

void MideaDriver::setSwingHorizontal(bool swing)
{
    // To Do (Midea): Implementation for horizontal swing control
    statusFeedback.swingHorizontalChanged(swing);
}

void MideaDriver::setSwingVertical(bool swing)
{
    // To Do (Midea): Implementation for vertical swing control
    statusFeedback.swingVerticalChanged(swing);
}

void MideaDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // To Do (Midea): Implementation for horizontal fix position control
    statusFeedback.swingHorizontalFixPositionChanged(position);
}

void MideaDriver::setSwingVerticalFixPosition(unsigned int position)
{
    // To Do (Midea): Implementation for vertical fix position control
    statusFeedback.swingVerticalFixPositionChanged(position);
}

void MideaDriver::setExternalSensorRoomTemperature(float temperaturCelsius)
{
    // To Do (Midea): Implementation for external sensor room temperature control
    statusFeedback.roomTemperatureChanged(temperaturCelsius);
}

void MideaDriver::setWifiLed(bool on)
{
    // To Do (Midea): Implementation for WiFi LED control
}

bool MideaDriver::supportExternalRoomTemperatureSensor()
{
    // To Do (Midea): Check if external room temperature sensor is supported
    return false; 
}

float MideaDriver::accuracyInDegrees()
{
    // To Do (Midea): Implement rounding of temperature to the air condition resolution
    return 1.f; // Assuming 1 degree Celsius accuracy
}

void MideaDriver::setDeviceMode(AirConditionDeviceMode mode)
{
    // To Do (Midea): Implementation for device mode control
    statusFeedback.deviceModeChanged(mode);
}

void MideaDriver::setMaxPowerLevel(uint8_t percentage)
{
    // To Do (Midea): Implementation for max power level control
    statusFeedback.maxPowerLevelChanged(percentage);
}

void MideaDriver::setAirPurification(bool on)
{
    // To Do (Midea): Implementation for air purification control
    statusFeedback.airPurificationChanged(on);
}

void MideaDriver::updatePower(bool power) { statusFeedback.updatePower(power); }
void MideaDriver::updateMode(AirConditionMode mode) { statusFeedback.updateMode(mode); }
void MideaDriver::updateTargetTemperature(float temperaturCelsius) { statusFeedback.updateTargetTemperature(temperaturCelsius); }
void MideaDriver::updateFanSpeed(int speed) { statusFeedback.updateFanSpeed(speed); }
void MideaDriver::updateSwingHorizontal(bool swing) { statusFeedback.updateSwingHorizontal(swing); }
void MideaDriver::updateSwingVertical(bool swing) { statusFeedback.updateSwingVertical(swing); }
void MideaDriver::updateCurrentTemperature(float temperaturCelsius) { statusFeedback.updateCurrentTemperature(temperaturCelsius); }
void MideaDriver::updateOutdoorTemperature(float temperaturCelsius) { statusFeedback.updateOutdoorTemperature(temperaturCelsius); }
void MideaDriver::updateDeviceMode(AirConditionDeviceMode mode) { statusFeedback.updateDeviceMode(mode); }
void MideaDriver::updateMaxPowerLevel(uint8_t percentage) { statusFeedback.updateMaxPowerLevel(percentage); }
void MideaDriver::updateAirPurification(bool on) { statusFeedback.updateAirPurification(on); }
void MideaDriver::updateOnlineStatus(bool online) { statusFeedback.updateOnlineStatus(online); }
void MideaDriver::updateWifiLed(bool on) { statusFeedback.updateWifiLed(on); }
void MideaDriver::updateHumidity(uint8_t humidity) { statusFeedback.updateHumidity(humidity); }
void MideaDriver::updateHumidityMode(uint8_t humidityMode) { statusFeedback.updateHumidityMode(humidityMode); }
void MideaDriver::updateTotalEnergyConsumption(uint32_t totalEnergyWh) { statusFeedback.updateTotalEnergyConsumption(totalEnergyWh); }
