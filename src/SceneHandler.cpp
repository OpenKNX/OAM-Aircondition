#include "OpenKNX.h"
#include "SceneHandler.h"


SceneHandler::SceneHandler(AirConditionDriver& driver)
    : _airConditionDriver(driver)
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
    
    SceneParameters params;
    switch (index)
    {
        case 0:
            params.onOff = ParamAIR_SCAOnOff; 
            params.operationMode = ParamAIR_SCAOperationMode; 
            params.temperature = ParamAIR_SCASceneTemperature;
            params.fan = ParamAIR_SCASceneFan;
            params.swing = ParamAIR_SCASceneSwing;
            params.position = ParamAIR_SCAScenePosition;
            break;
        case 1:
            params.onOff = ParamAIR_SCBOnOff;
            params.operationMode = ParamAIR_SCBOperationMode;
            params.temperature = ParamAIR_SCBSceneTemperature;
            params.fan = ParamAIR_SCBSceneFan;
            params.swing = ParamAIR_SCBSceneSwing;
            params.position = ParamAIR_SCBScenePosition;
            break;
        case 2:
            params.onOff = ParamAIR_SCCOnOff;
            params.operationMode = ParamAIR_SCCOperationMode;
            params.temperature = ParamAIR_SCCSceneTemperature;
            params.fan = ParamAIR_SCCSceneFan;
            params.swing = ParamAIR_SCCSceneSwing;
            params.position = ParamAIR_SCCScenePosition;
            break;      
        case 3:
            params.onOff = ParamAIR_SCDOnOff;
            params.operationMode = ParamAIR_SCDOperationMode;
            params.temperature = ParamAIR_SCDSceneTemperature;
            params.fan = ParamAIR_SCDSceneFan;
            params.swing = ParamAIR_SCDSceneSwing;
            params.position = ParamAIR_SCDScenePosition;
            break;
        case 4:
            params.onOff = ParamAIR_SCEOnOff;
            params.operationMode = ParamAIR_SCEOperationMode;
            params.temperature = ParamAIR_SCESceneTemperature;
            params.fan = ParamAIR_SCESceneFan;
            params.swing = ParamAIR_SCESceneSwing;
            params.position = ParamAIR_SCEScenePosition;
            break;
        case 5:
            params.onOff = ParamAIR_SCFOnOff;
            params.operationMode = ParamAIR_SCFOperationMode;
            params.temperature = ParamAIR_SCFSceneTemperature;
            params.fan = ParamAIR_SCFSceneFan;
            params.swing = ParamAIR_SCFSceneSwing;
            params.position = ParamAIR_SCFScenePosition;
            break;
        case 6:
            params.onOff = ParamAIR_SCGOnOff;
            params.operationMode = ParamAIR_SCGOperationMode;
            params.temperature = ParamAIR_SCGSceneTemperature;
            params.fan = ParamAIR_SCGSceneFan;
            params.swing = ParamAIR_SCGSceneSwing;
            params.position = ParamAIR_SCGScenePosition;
            break;
        case 7:
            params.onOff = ParamAIR_SCHOnOff;
            params.operationMode = ParamAIR_SCHOperationMode;
            params.temperature = ParamAIR_SCHSceneTemperature;
            params.fan = ParamAIR_SCHSceneFan;  
            params.swing = ParamAIR_SCHSceneSwing;
            params.position = ParamAIR_SCHScenePosition;
            break;
        case 8:
            params.onOff = ParamAIR_SCIOnOff;
            params.operationMode = ParamAIR_SCIOperationMode;
            params.temperature = ParamAIR_SCISceneTemperature;
            params.fan = ParamAIR_SCISceneFan;
            params.swing = ParamAIR_SCISceneSwing;
            params.position = ParamAIR_SCIScenePosition;
            break;
        case 9:
            params.onOff = ParamAIR_SCJOnOff;
            params.operationMode = ParamAIR_SCJOperationMode;
            params.temperature = ParamAIR_SCJSceneTemperature;
            params.fan = ParamAIR_SCJSceneFan;
            params.swing = ParamAIR_SCJSceneSwing;
            params.position = ParamAIR_SCJScenePosition;
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
    
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Ausschalten" Value="0" Id="%ENID%" />
    // <Enumeration Text="Einschalten" Value="1" Id="%ENID%" />
    switch (params.onOff)
    {
        case 255:
            break; // No change
        case 0:
            logDebugP("Turning off air conditioner");
            _airConditionDriver.setPower(false);
            break;
        case 1:
            logDebugP("Turning on air conditioner");
            _airConditionDriver.setPower(true);
            break;
        default:
            logErrorP("Invalid on/off value %d", params.onOff);
    }
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
            if (params.temperature < _airConditionDriver.getMinimumTargetTemperature() || params.temperature > _airConditionDriver.getMaximumTargetTemperature())
            {
                logErrorP("Invalid temperature %d, must be between %.1f and %.1f", (int) params.temperature, _airConditionDriver.getMinimumTargetTemperature(), _airConditionDriver.getMaximumTargetTemperature());
            }
            else
            {
                logDebugP("Setting target temperature to %d", params.temperature);
                _airConditionDriver.setTargetTemperature(params.temperature);
            }
            break;
    }   
    // <Enumeration Text="Keine Änderung" Value="255" Id="%ENID%" />
    // <Enumeration Text="Automatik" Value="0" Id="%ENID%" />
    // <Enumeration Text="Leise" Value="1" Id="%ENID%" />
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
                logErrorP("Invalid fan speed %d, must be between 0 and %d", (int) params.temperature, (int) _airConditionDriver.getMaximumFanSpeed());
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
            if (params.position > _airConditionDriver.getMaximumVertiacalFixPosition())
            {
                logErrorP("Invalid position %d, must be between 0 and %d", (int) params.position, (int) _airConditionDriver.getMaximumVertiacalFixPosition());
            }
            else
            {
                logDebugP("Setting position to %d", params.position);
                _airConditionDriver.setSwingVerticalFixPosition(params.position);
            }
            break;
    }
}