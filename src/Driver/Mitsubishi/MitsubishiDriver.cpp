#include "MitsubishiDriver.h"
#include <HeatPump.h>

HeatPump hp;

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
    OPENKNX_AIR_CONDITION_SERIAL.begin(2400, SERIAL_8N1, OPENKNX_AIR_CONDITION_SERIAL_RX, OPENKNX_AIR_CONDITION_SERIAL_TX);
    hp.connect(&OPENKNX_AIR_CONDITION_SERIAL);

    requestInitialData();
}

void MitsubishiDriver::loop()
{

    if (millis() - _lastStatusRequest > 60000)
    {
        hp.sync();
        requestStatusData();
    }

    if (millis() - _lastSettingsRequest > 60000)
    {
        hp.sync();
        requestSettingsData();
    }


    

}

void MitsubishiDriver::requestSettingsData()
{
    heatpumpSettings settings = hp.getSettings();
    statusFeedback.targetTemperatureChanged(settings.temperature);
   
    if (settings.mode == "AUTO") {
        statusFeedback.modeChanged(AirConditionMode::AirConditionModeAuto);
    } else if (settings.mode == "COOL") {
        statusFeedback.modeChanged(AirConditionMode::AirConditionModeCool);
    } else if (settings.mode == "HEAT") {
        statusFeedback.modeChanged(AirConditionMode::AirConditionModeHeat);
    } else if (settings.mode == "DRY") {
        statusFeedback.modeChanged(AirConditionMode::AirConditionModeDry);
    } else if (settings.mode == "FAN") {
        statusFeedback.modeChanged(AirConditionMode::AirConditionModeFan);
    } else {
        logDebugP("Got unknown mode from HVAC: : %s", settings.mode);
    }

    if (settings.fan == "AUTO") {
        statusFeedback.fanSpeedChanged(0);
    } else if ("QUIET") {
        statusFeedback.fanSpeedChanged(1);
    } else if ("1") {
        statusFeedback.fanSpeedChanged(2);
    } else if ("2") {
        statusFeedback.fanSpeedChanged(3);
    } else if ("3") {
        statusFeedback.fanSpeedChanged(4);
    } else if ("4") {
        statusFeedback.fanSpeedChanged(5);
    }

    if (settings.power == "ON") {
        statusFeedback.powerChanged(true);
    } else if (settings.power == "OFF") {
        statusFeedback.powerChanged(false);
    }

    //settings.connected
    //settings.vane;
    //settings.wideVane;

    _lastSettingsRequest = millis();

}

void MitsubishiDriver::requestStatusData() {
    heatpumpStatus status = hp.getStatus();
    statusFeedback.roomTemperatureChanged(status.roomTemperature);

    _lastStatusRequest = millis();
}

void MitsubishiDriver::requestInitialData()
{
    logDebugP("Requesting initial data: statusData");
    requestStatusData();
    logDebugP("Requesting initial data: settingsData");
    requestSettingsData();
 
}

float MitsubishiDriver::getMinimumTargetTemperature()
{
    return  (float) ParamAIR_Mit_MinTemp;
}

float MitsubishiDriver::getMaximumTargetTemperature()
{
    return 31.0f; 
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

    if (power == true) {
        hp.setPowerSetting("ON");
    } else {
        hp.setPowerSetting("OFF");
    }
}

void MitsubishiDriver::setMode(AirConditionMode mode)
{
    
    switch (mode)
    {
        case AirConditionMode::AirConditionModeAuto:
            hp.setModeSetting("AUTO");
            break;
        case AirConditionMode::AirConditionModeCool:
            hp.setModeSetting("COOL");
            break;       
        case AirConditionMode::AirConditionModeHeat:
            hp.setModeSetting("HEAT");
            break;
        case AirConditionMode::AirConditionModeDry:
            hp.setModeSetting("DRY");
            break;
        case AirConditionMode::AirConditionModeFan:
            hp.setModeSetting("FAN");
            break;
        default:
            logErrorP("Unsupported mode: %d", (int)mode);
            return;
    }                         
}

void MitsubishiDriver::setTargetTemperature(float temperaturCelius)
{
    hp.setTemperature(temperaturCelius);
}

void MitsubishiDriver::setFanSpeed(unsigned int speed)
{
    switch (speed)
    {
        case 0: // auto
            hp.setFanSpeed("AUTO");
            break;
        case 1: // quiet
            hp.setFanSpeed("QUIET");
            break;
        case 2: // low
            hp.setFanSpeed("1");
            break;
        case 3: // medium
            hp.setFanSpeed("2");
            break;
        case 4: // 
            hp.setFanSpeed("3");
            break;
        case 5: // high
            hp.setFanSpeed("4");
            break;
        default:
            break;
    }    
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
