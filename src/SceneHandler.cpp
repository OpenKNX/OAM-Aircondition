#include "OpenKNX.h"
#include "SceneHandler.h"


SceneHandler::SceneHandler(AirConditionDriver& driver, SceneHandlerCallback* callback)
    : _airConditionDriver(driver), _callback(callback)
{
}

std::string SceneHandler::logPrefix() const
{
    return "SceneHandler";
}

void SceneHandler::handleScene(uint8_t sceneNumber)
{
    logDebugP("Handling scene %u", sceneNumber);
   
    if (ParamAIR_SCAActive && ParamAIR_SCANumber == sceneNumber)
    {
       applyParameters(0);
    }
    if (ParamAIR_SCBActive && ParamAIR_SCBNumber == sceneNumber)
    {
        applyParameters(1);
    }
    if (ParamAIR_SCCActive && ParamAIR_SCCNumber == sceneNumber)
    {
        applyParameters(2);
    }   
    if (ParamAIR_SCDActive && ParamAIR_SCDNumber == sceneNumber)
    {
        applyParameters(3);
    }
    if (ParamAIR_SCEActive && ParamAIR_SCENumber == sceneNumber)
    {
        applyParameters(4);
    }
    if (ParamAIR_SCFActive && ParamAIR_SCFNumber == sceneNumber)
    {
        applyParameters(5);
    }           
    if (ParamAIR_SCGActive && ParamAIR_SCGNumber == sceneNumber)
    {
        applyParameters(6);
    }
    if (ParamAIR_SCHActive && ParamAIR_SCHNumber == sceneNumber)
    {
        applyParameters(7);
    }   
    if (ParamAIR_SCIActive && ParamAIR_SCINumber == sceneNumber)
    {
        applyParameters(8);
    }
    if (ParamAIR_SCJActive && ParamAIR_SCJNumber == sceneNumber)
    {
        applyParameters(9);
    }
#ifdef ParamAIR_SCKActive
#error "ParamAIR_SCKActive is not handled."
#endif
}

SceneParameters SceneHandler::getSceneParameters(int index)
{
    const bool supportsScenePosition = (ParamAIR_DeviceType == PT_AIRDeviceType::Toshiba || ParamAIR_DeviceType == PT_AIRDeviceType::TCL);
    const bool supportsScenePowerLimit = (ParamAIR_DeviceType == PT_AIRDeviceType::Toshiba);
    const bool supportsSceneDeviceMode = (ParamAIR_DeviceType != PT_AIRDeviceType::TCL);
    const bool supportsSceneAirPurification = (ParamAIR_DeviceType != PT_AIRDeviceType::TCL);
    SceneParameters params;
    switch (index)
    {
        case 0:
            params.onOff = ParamAIR_SCAOnOff; 
            params.operationMode = ParamAIR_SCAOperationMode; 
            params.temperature = ParamAIR_SCASceneTemperature;
            params.fan = ParamAIR_SCASceneFan;
            params.swing = ParamAIR_SCASceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCAScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCAScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCASceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCASceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCAAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCAAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCAAutoOffDelayTime;
            break;
        case 1:
            params.onOff = ParamAIR_SCBOnOff;
            params.operationMode = ParamAIR_SCBOperationMode;
            params.temperature = ParamAIR_SCBSceneTemperature;
            params.fan = ParamAIR_SCBSceneFan;
            params.swing = ParamAIR_SCBSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCBScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCBScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCBSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCBSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCBAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCBAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCBAutoOffDelayTime;
            break;
        case 2:
            params.onOff = ParamAIR_SCCOnOff;
            params.operationMode = ParamAIR_SCCOperationMode;
            params.temperature = ParamAIR_SCCSceneTemperature;
            params.fan = ParamAIR_SCCSceneFan;
            params.swing = ParamAIR_SCCSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCCScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCCScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCCSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCCSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCCAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCCAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCCAutoOffDelayTime;
            break;
        case 3:
            params.onOff = ParamAIR_SCDOnOff;
            params.operationMode = ParamAIR_SCDOperationMode;
            params.temperature = ParamAIR_SCDSceneTemperature;
            params.fan = ParamAIR_SCDSceneFan;
            params.swing = ParamAIR_SCDSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCDScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCDScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCDSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCDSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCDAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCDAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCDAutoOffDelayTime;
            break;
        case 4:
            params.onOff = ParamAIR_SCEOnOff;
            params.operationMode = ParamAIR_SCEOperationMode;
            params.temperature = ParamAIR_SCESceneTemperature;
            params.fan = ParamAIR_SCESceneFan;
            params.swing = ParamAIR_SCESceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCEScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCEScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCESceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCESceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCEAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCEAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCEAutoOffDelayTime;
            break;
        case 5:
            params.onOff = ParamAIR_SCFOnOff;
            params.operationMode = ParamAIR_SCFOperationMode;
            params.temperature = ParamAIR_SCFSceneTemperature;
            params.fan = ParamAIR_SCFSceneFan;
            params.swing = ParamAIR_SCFSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCFScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCFScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCFSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCFSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCFAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCFAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCFAutoOffDelayTime;
            break;
        case 6:
            params.onOff = ParamAIR_SCGOnOff;
            params.operationMode = ParamAIR_SCGOperationMode;
            params.temperature = ParamAIR_SCGSceneTemperature;
            params.fan = ParamAIR_SCGSceneFan;
            params.swing = ParamAIR_SCGSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCGScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCGScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCGSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCGSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCGAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCGAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCGAutoOffDelayTime;
            break;
        case 7:
            params.onOff = ParamAIR_SCHOnOff;
            params.operationMode = ParamAIR_SCHOperationMode;
            params.temperature = ParamAIR_SCHSceneTemperature;
            params.fan = ParamAIR_SCHSceneFan;  
            params.swing = ParamAIR_SCHSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCHScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCHScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCHSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCHSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCHAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCHAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCHAutoOffDelayTime;
            break;
        case 8:
            params.onOff = ParamAIR_SCIOnOff;
            params.operationMode = ParamAIR_SCIOperationMode;
            params.temperature = ParamAIR_SCISceneTemperature;
            params.fan = ParamAIR_SCISceneFan;
            params.swing = ParamAIR_SCISceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCIScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCIScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCISceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCISceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCIAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCIAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCIAutoOffDelayTime;
            break;
        case 9:
            params.onOff = ParamAIR_SCJOnOff;
            params.operationMode = ParamAIR_SCJOperationMode;
            params.temperature = ParamAIR_SCJSceneTemperature;
            params.fan = ParamAIR_SCJSceneFan;
            params.swing = ParamAIR_SCJSceneSwing;
            params.position = supportsScenePosition ? ParamAIR_SCJScenePosition : 255;
            params.powerLimit = supportsScenePowerLimit ? ParamAIR_SCJScenePowerLimit : 255;
            params.deviceMode = supportsSceneDeviceMode ? ParamAIR_SCJSceneDeviceMode : 255;
            params.airPurification = supportsSceneAirPurification ? ParamAIR_SCJSceneAirPurification : 255;
            params.autoOffActive = ParamAIR_SCJAutoOffActive;
            params.autoOffDelayBase = ParamAIR_SCJAutoOffDelayBase;
            params.autoOffDelayTime = ParamAIR_SCJAutoOffDelayTime;
            break;
#ifdef ParamAIR_SCKOnOff
#error "ParamAIR_SCKOnOff is not handled."
#endif
        default:
            logErrorP("Invalid scene index %d", index);
            break;
    }
    return params;
}

void SceneHandler::applyParameters(int index)
{
    logDebugP("Applying parameters for scene %c", index + 65);
    SceneParameters params = getSceneParameters(index);
    
    // Apply mode, temperature, and fan settings FIRST before turning on power
    // This ensures the pending state is set correctly before the D1 command is sent
    
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Automatik" Value="0" Id="%ENID%" />
    // <Enumeration Text="Kühlen" Value="1" Id="%ENID%" />
    // <Enumeration Text="Heizen" Value="2" Id="%ENID%" />
    // <Enumeration Text="Trocknen" Value="3" Id="%ENID%" />
    // <Enumeration Text="Lüfter" Value="4" Id="%ENID%" />
    switch (params.operationMode)
    {
        case 255:
            break; // No change
        case 0:
            logDebugP("Setting operation mode to Auto");
            _airConditionDriver.setMode(AirConditionMode::AirConditionModeAuto);
            break;
        case 1:
            logDebugP("Setting operation mode to Cool");
            _airConditionDriver.setMode(AirConditionMode::AirConditionModeCool);
            break;
        case 2:
            logDebugP("Setting operation mode to Heat");
            _airConditionDriver.setMode(AirConditionMode::AirConditionModeHeat);
            break;
        case 3:
            logDebugP("Setting operation mode to Dry");
            _airConditionDriver.setMode(AirConditionMode::AirConditionModeDry);
            break;
        case 4:
            logDebugP("Setting operation mode to Fan");
            _airConditionDriver.setMode(AirConditionMode::AirConditionModeFan);
            break;
        default:
            logErrorP("Invalid operation mode %d", params.operationMode);
    }
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="17°" Value="17" Id="%ENID%" />
    // <Enumeration Text="18°" Value="18" Id="%ENID%" />
    // <Enumeration Text="19°" Value="19" Id="%ENID%" />
    // <Enumeration Text="20°" Value="20" Id="%ENID%" />
    // <Enumeration Text="21°" Value="21" Id="%ENID%" />
    // <Enumeration Text="22°" Value="22" Id="%ENID%" />
    // <Enumeration Text="23°" Value="23" Id="%ENID%" />
    // <Enumeration Text="24°" Value="24" Id="%ENID%" />
    // <Enumeration Text="25°" Value="25" Id="%ENID%" />
    // <Enumeration Text="26°" Value="26" Id="%ENID%" />
    // <Enumeration Text="27°" Value="27" Id="%ENID%" />
    // <Enumeration Text="28°" Value="28" Id="%ENID%" />
    switch (params.temperature)
    {
        case 255:
            break; // No change
        default:

            float temp;
            if (params.temperature > 100)
            {
                temp = params.temperature - 100 + 0.5f;
            }
            else
            {
                temp = params.temperature;
            }


            if (temp < _airConditionDriver.getMinimumTargetTemperature() || temp > _airConditionDriver.getMaximumTargetTemperature())
            {
                logErrorP("Invalid temperature %.1f, must be between %.1f and %.1f", (float) temp, _airConditionDriver.getMinimumTargetTemperature(), _airConditionDriver.getMaximumTargetTemperature());
            }
            else
            {
                logDebugP("Setting target temperature to %.1f", temp);
                _airConditionDriver.setTargetTemperature(temp);
            }
            break;
    }   
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Automatik" Value="0" Id="%ENID%" />
    // <Enumeration Text="Geräuscharm" Value="1" Id="%ENID%" />
    // <Enumeration Text="Stufe 1" Value="2" Id="%ENID%" />
    // <Enumeration Text="Stufe 2" Value="3" Id="%ENID%" />
    // <Enumeration Text="Stufe 3" Value="4" Id="%ENID%" />
    // <Enumeration Text="Stufe 4" Value="5" Id="%ENID%" />
    // <Enumeration Text="Stufe 5" Value="6" Id="%ENID%" />
    switch (params.fan)
    {
        case 255:
            break; // No change
        default:
            if (params.fan > _airConditionDriver.getMaximumFanSpeed())
            {
                logErrorP("Invalid fan speed %d, must be between 0 and %d", (int) params.fan, (int) _airConditionDriver.getMaximumFanSpeed());
            }
            else
            {
                logDebugP("Setting fan speed to %d", params.fan);
                _airConditionDriver.setFanSpeed(params.fan);
            }
            break;
    }

    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Aus" Value="0" Id="%ENID%" />
    // <Enumeration Text="Vertikal" Value="1" Id="%ENID%" />
    // <Enumeration Text="Horizontal" Value="2" Id="%ENID%" />
    // <Enumeration Text="Vertikal und Horizontal" Value="3" Id="%ENID%" />
    switch (params.swing)
    {
        case 255:
            break; // No change
        case 0:
            logDebugP("Setting swing to Off");
            _airConditionDriver.setSwingHorizontal(false);
            _airConditionDriver.setSwingVertical(false);
            break;
        case 1:
            logDebugP("Setting swing to Vertical");
            _airConditionDriver.setSwingVertical(true);
            _airConditionDriver.setSwingHorizontal(false);
            break;
        case 2:
            logDebugP("Setting swing to Horizontal");
            _airConditionDriver.setSwingHorizontal(true);
            _airConditionDriver.setSwingVertical(false);
            break;
        case 3:
            logDebugP("Setting swing to Both");
            _airConditionDriver.setSwingHorizontal(true);
            _airConditionDriver.setSwingVertical(true);
            break;
        default:
            logErrorP("Invalid swing mode %d", params.swing);
    }
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Ganz Oben" Value="0" Id="%ENID%" />
    // <Enumeration Text="Oben" Value="1" Id="%ENID%" />
    // <Enumeration Text="Mitte" Value="2" Id="%ENID%" />
    // <Enumeration Text="Unten" Value="3" Id="%ENID%" />
    // <Enumeration Text="Ganz Unten" Value="4" Id="%ENID%" />
    switch (params.position)
    {
        case 255:
            break; // No change
        default:
            if (params.position > _airConditionDriver.getMaximumVerticalFixPosition())
            {
                logErrorP("Invalid position %d, must be between 0 and %d", (int) params.position, (int) _airConditionDriver.getMaximumVerticalFixPosition());
            }
            else
            {
                logDebugP("Setting position to %d", params.position);
                _airConditionDriver.setSwingVerticalFixPosition(params.position);
            }
            break;
    }
    switch (params.powerLimit)
    {
        case 255:
            break; // No change
        default:
            logDebugP("Setting power limit to %d", params.powerLimit);
            _airConditionDriver.setMaxPowerLevel(params.powerLimit);
            break;
    }
    switch (params.deviceMode)
    {
        case 255:
            break; // No change
        default:
            logDebugP("Setting device mode to %d", params.deviceMode);
            _airConditionDriver.setDeviceMode((AirConditionDeviceMode) params.deviceMode);
            break;
    }
    switch (params.airPurification)
    {
        case 255:
            break; // No change
        case 0:
            logDebugP("Setting air purification to Off");
            _airConditionDriver.setAirPurification(false);
            break;
        case 1:
            logDebugP("Setting air purification to On");
            _airConditionDriver.setAirPurification(true);
            break;
        default:
            logErrorP("Invalid air purification value %d", params.airPurification);
    }
    
    // Apply power state LAST after all parameters are set
    // This ensures the D1 command uses the correct pending state
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Ausschalten" Value="0" Id="%ENID%" />
    // <Enumeration Text="Einschalten" Value="1" Id="%ENID%" />
    switch (params.onOff)
    {
        case PT_AIRSceneOnOff::NoChange:
            break; // No change
        case PT_AIRSceneOnOff::Off:
            logDebugP("Turning off air conditioner");
            _airConditionDriver.setPower(false);
            break;
        case PT_AIRSceneOnOff::On:
            logDebugP("Turning on air conditioner");
            _airConditionDriver.setPower(true);
            break;
        default:
            logErrorP("Invalid on/off value %d", params.onOff);
    }

    if (_callback != nullptr)
    {
        _callback->sceneApplied(params);
    }
}
