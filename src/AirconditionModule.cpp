#include "OpenKNX.h"
#include "AirconditionModule.h"
#include "Driver/Toshiba/ToshibaDriver.h"

AirconditionModule openknxAircondition;

const std::string AirconditionModule::name()
{
    return "Klimaanlage";
}

const std::string AirconditionModule::version()
{
    return MAIN_ApplicationVersion;
}

void AirconditionModule::setup()
{
    // <Enumeration Text="Bitte wählen..." Value="0" Id="%ENID%" />
    // <Enumeration Text="Daikin" Value="1" Id="%ENID%" />
    // <Enumeration Text="Midea" Value="2" Id="%ENID%" />
    // <Enumeration Text="Mitsubishi" Value="3" Id="%ENID%" />
    // <Enumeration Text="Toshiba" Value="4" Id="%ENID%" />
    switch (ParamAIR_DeviceType)
    {
        case 0:
            logInfoP("No AirCondition Device Type selected");
            break;
        case 1: // Daikin
            logInfoP("Daikin driver not yet implemented");
            break;
        case 2: // Midea
            logInfoP("Initialize ToshibaDriver", ParamAIR_DeviceType);
            airConditionDriver = new ToshibaDriver(*this);
            break;
        case 3: // Mitsubishi
            logInfoP("Mitsubishi driver not yet implemented");
            break;
        case 4: // Toshiba
            logInfoP("Initialize ToshibaDriver", ParamAIR_DeviceType);
            airConditionDriver = new ToshibaDriver(*this);
            break;
        default:
            logErrorP("AirCondition Device Type %d not supported", ParamAIR_DeviceType);
            break;
    }
    if (airConditionDriver != nullptr)
    {
        setLocked(false);
        airConditionDriver->setup();
    }
}

void AirconditionModule::setLocked(bool locked)
{
    bool initialized =  KoAIR_LockReleaseState.initialized();
    int handleChangeParameterValue = 0;
    // <Enumeration Text="Keines" Value="0" Id="%ENID%" />
    // <Enumeration Text="Freigabe" Value="1" Id="%ENID%" />
    // <Enumeration Text="Sperre" Value="2" Id="%ENID%" />
    switch (ParamAIR_LockReleaseKo)
    {
        case 0:
            return;
        case 1:
            {
                bool release = !locked;
                if (KoAIR_LockReleaseState.valueCompare(release, DPT_Switch) && initialized)
                {
                    if (release)
                        handleChangeParameterValue = ParamAIR_ReleaseBehaviorOn;
                    else
                        handleChangeParameterValue = ParamAIR_ReleaseBehaviorOff;
                }
            }
            break;
        case 2:
            {
                if (KoAIR_LockReleaseState.valueCompare(locked, DPT_Switch) && initialized)
                {
                    if (locked)
                        handleChangeParameterValue = ParamAIR_LockBehaviorOn;
                    else
                        handleChangeParameterValue = ParamAIR_LockBehaviorOff;
                }
            }
    }
    // <Enumeration Text="Keine Änderung" Value="0" Id="%ENID%" />
    // <Enumeration Text="Einschalten" Value="1" Id="%ENID%" />
    // <Enumeration Text="Ausschalten" Value="2" Id="%ENID%" />
    switch (handleChangeParameterValue)
    {
        case 0: // Do nothing
            break;
        case 1: // Power on
            airConditionDriver->setPower(true);
            break;
        case 2: // Power off
            airConditionDriver->setPower(false);
            break;
    }
}


void AirconditionModule::loop()
{
}

void AirconditionModule::showInformations()
{
    if (airConditionDriver != nullptr)
    {
        logInfoP("AirCondition Driver '%s'", airConditionDriver->name().c_str());
        logInfoP("Minimum Target Temperature: %.1f °C", airConditionDriver->getMinimumTargetTemperature());
        logInfoP("Maximum Target Temperature: %.1f °C", airConditionDriver->getMaximumTargetTemperature());
        logInfoP("Maximum Fan Speed: %u", airConditionDriver->getMaximumFanSpeed());
        logInfoP("Maximum Horizontal Fix Position: %u", airConditionDriver->getMaximumHorizontalFixPosition());
        logInfoP("Maximum Vertical Fix Position: %u", airConditionDriver->getMaximumVertiacalFixPosition());
        airConditionDriver->showInformations();
    }
    else
    {
        logErrorP("No AirCondition Driver initialized");
    }
}

bool AirconditionModule::processCommand(const std::string cmd, bool debugKo)
{
    if (airConditionDriver == nullptr)
    {
        if (airConditionDriver->processCommand(cmd, debugKo) == true)
        {
            return true;
        }
    }
    if (cmd == "power on")
    {
        KoAIR_Power.valueNoSend(true, DPT_Switch);
        processInputKo(KoAIR_Power);
        return true;
    }
    else if (cmd == "power off")
    {
        KoAIR_Power.valueNoSend(false, DPT_Switch);
        processInputKo(KoAIR_Power);
        return true;
    }
    else if (cmd.starts_with("temp "))
    {
        // Extract the temperature value from the command
        std::string tempStr = cmd.substr(5);
        try
        {
            float temperature = std::stof(tempStr);
            KoAIR_SetTemperature.valueNoSend(temperature, DPT_Value_Temp);
            processInputKo(KoAIR_SetTemperature);
        }
        catch (const std::invalid_argument& e)
        {
            logErrorP("Invalid temperature value: %s", tempStr.c_str());
        }
        return true;
    }
    else if (cmd.starts_with("fan "))
    {
        // Extract the fan speed value from the command
        std::string fanStr = cmd.substr(4);
        try
        {
            unsigned int fanSpeedPercent = std::stoi(fanStr);
            if (airConditionDriver != nullptr && fanSpeedPercent <= 100)
            {
                KoAIR_FanSpeed.valueNoSend((uint8_t) fanSpeedPercent, DPT_Scaling);
                processInputKo(KoAIR_FanSpeed);
            }
            else
            {
                logErrorP("Invalid fan speed value: %s. Value must between 0 and %u.", fanStr.c_str(), airConditionDriver->getMaximumFanSpeed());
            }
        }
        catch (const std::invalid_argument& e)
        {
            logErrorP("Invalid fan speed value: %s", fanStr.c_str());
        }
        return true;
    }
    return false;
}

void AirconditionModule::processInputKo(GroupObject &ko)
{
    if (airConditionDriver != nullptr)
    {
        // <Enumeration Text="Keines" Value="0" Id="%ENID%" />
        // <Enumeration Text="Freigabe" Value="1" Id="%ENID%" />
        // <Enumeration Text="Sperre" Value="2" Id="%ENID%" />
        switch (ParamAIR_LockReleaseKo)
        {
            case 1: // Release
                if (!KoAIR_LockReleaseState.value(DPT_Switch))
                    return; 
                break;
            case 2: // Lock
                if (KoAIR_LockReleaseState.value(DPT_Switch))
                    return;
                break;
        }
        switch (ko.asap())
        {
            case AIR_KoPower:
                airConditionDriver->setPower(ko.value(DPT_Switch));
                break;
            case AIR_KoOperationMode:
                {
                    uint8_t hvacMode = ko.value(DPT_HVACContrMode);
                    switch (hvacMode)
                    {
                        case 0: // Auto
                            airConditionDriver->setMode(AirConditionMode::AirConditionModeAuto);
                            break;
                        case 1: // Head
                            airConditionDriver->setMode(AirConditionMode::AirConditionModeHeat);
                            break;
                        case 3: // Cool
                            airConditionDriver->setMode(AirConditionMode::AirConditionModeCool);
                            break;
                        case 6: // Off
                            airConditionDriver->setPower(false);
                            break;
                        case 9: // Fan
                            airConditionDriver->setMode(AirConditionMode::AirConditionModeFan);
                            break;
                        case 14: // Dry
                            airConditionDriver->setMode(AirConditionMode::AirConditionModeDry);
                            break;
                        default:
                            logErrorP("Unknown HVAC mode %d", hvacMode);
                    }
                }
                break;
            case AIR_KoOperationModeAutomatic:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeAuto);
                else
                    airConditionDriver->setPower(false);
                break;
            case AIR_KoOperationModeCooling:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeCool);
                else
                    airConditionDriver->setPower(false);
                break;
            case AIR_KoOperationModeHeating:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeHeat);
                else
                    airConditionDriver->setPower(false);
                break;
            case AIR_KoOperationModeVentilation:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeFan);
                else
                    airConditionDriver->setPower(false);
             case AIR_KoOperationModeDehumidification:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeDry);
                else
                    airConditionDriver->setPower(false);
                break;
            case AIR_KoFanSpeed:  
                {
                    uint8_t fanSpeedPercent = ko.value(DPT_Scaling);
                    unsigned int fanSpeed =  airConditionDriver->getMaximumFanSpeed() * fanSpeedPercent / 100;
                    airConditionDriver->setFanSpeed(fanSpeed);
                }
                break;
            case AIR_KoFanSpeedUpDown:
                {
                    uint8_t currentFanSpeedPercentage = KoAIR_FanSpeedState.value(DPT_Scaling);
                    unsigned int currentFanSpeed = airConditionDriver->getMaximumFanSpeed() * currentFanSpeedPercentage / 100;
                    if (ko.value(DPT_Switch))
                    {
                        // Increase fan speed
                        if (currentFanSpeed < airConditionDriver->getMaximumFanSpeed())
                            airConditionDriver->setFanSpeed(currentFanSpeed + 1);
                    }
                    else
                    {
                        // Decrease fan speed
                        if (currentFanSpeed > 0)
                            airConditionDriver->setFanSpeed(currentFanSpeed - 1);
                    }  
                }
                break;
            case AIR_KoLouverVerticalMove:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setSwingVertical(true);
                else
                    airConditionDriver->setSwingVertical(false);
                break;
            case AIR_KoLouverHorizontalMove:
                if (ko.value(DPT_Switch))
                    airConditionDriver->setSwingHorizontal(true);
                else
                    airConditionDriver->setSwingHorizontal(false);
                break;
            case AIR_KoSetTemperature:
                {
                    float targetTemperature = ko.value(DPT_Value_Temp);
                    if (targetTemperature < airConditionDriver->getMinimumTargetTemperature() || targetTemperature > airConditionDriver->getMaximumTargetTemperature())
                    {
                        logErrorP("Target temperature %f is out of range (%f - %f)", targetTemperature, 
                                airConditionDriver->getMinimumTargetTemperature(),
                                airConditionDriver->getMaximumTargetTemperature());
                        return;
                    }
                    airConditionDriver->setTargetTemperature(targetTemperature);
                }
                break;       
            case AIR_KoSetTemperatureUpDown:
                {
                    float currentTemperature = KoAIR_SetTemperatureState.value(DPT_Value_Temp);
                    if (ko.value(DPT_Switch))
                    {             
                        if (currentTemperature < airConditionDriver->getMaximumTargetTemperature())
                            airConditionDriver->setTargetTemperature(currentTemperature + 1.f);
                    }
                    else
                    {             
                        if (currentTemperature > airConditionDriver->getMinimumTargetTemperature())
                            airConditionDriver->setTargetTemperature(currentTemperature - 1.f); 
                    }
                }
                break;  
            case AIR_KoRoomTemperatureInput:
                {
                    float roomTemperature = ko.value(DPT_Value_Temp);
                    if (roomTemperature < -50.f || roomTemperature > 50.f)
                    {
                        logErrorP("Room temperature %f is out of range (-50 - 50)", roomTemperature);
                        return;
                    }
                    airConditionDriver->setExternalSensorRoomTemperature(roomTemperature);
                }
                break;

        };
        airConditionDriver->processInputKo(ko);
    }
}

void AirconditionModule::powerChanged(bool power)
{
    KoAIR_PowerState.valueCompare(power, DPT_Switch);
    if (!power)
    {
        KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
    }
}
void AirconditionModule::targetTemperatureChanged(float temperature)
{
    KoAIR_SetTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}
void AirconditionModule::fanSpeedChanged(int fanSpeed)
{
    unsigned int fanSpeedPercent = fanSpeed * 100 / airConditionDriver->getMaximumFanSpeed();
    KoAIR_FanSpeedState.valueCompare((uint8_t) fanSpeedPercent, DPT_Scaling);
}
void AirconditionModule::modeChanged(AirConditionMode mode)
{
    uint8_t hvacMode = 0;
    switch (mode)
    {
        case AirConditionMode::AirConditionModeAuto:
            hvacMode= 0;
            KoAIR_OperationModeAutomaticState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeCool:
            hvacMode = 3;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeHeat:
            hvacMode = 1;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeDry:
            hvacMode = 14;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(true, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeFan:
            hvacMode = 9;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        default:
            logErrorP("Unknown AirConditionMode %d", mode);
            return;
    }
    KoAIR_OperationModeState.valueCompare(hvacMode, DPT_HVACContrMode);
}

void AirconditionModule::swingHorizontalChanged(bool swing)
{
    KoAIR_LouverHorizontalMoveState.valueNoSend(swing, DPT_Switch);
}

void AirconditionModule::swingVerticalChanged(bool swing)
{
    KoAIR_LouverVerticalMoveState.valueNoSend(swing, DPT_Switch);
}

void AirconditionModule::swingHorizontalFixPositionChanged(int position)
{
    // To Do: Implementation for horizontal fix position KO
}

void AirconditionModule::swingVerticalFixPositionChanged(int position)
{
    // To Do: Implementation for vertical fix position KO
}

void AirconditionModule::roomTemperatureChanged(float temperature)
{
    KoAIR_RoomTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}

void AirconditionModule::outsideTemperaturChanged(float temperature)
{
    KoAIR_OutsideTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}

void AirconditionModule::errorStateChanged(const char* error)
{
    if (error != NO_ERROR)
       logErrorP("AirCondition Error: %s", error);
    else
       logInfoP("AirCondition Error cleared");
}

void AirconditionModule::showHelp()
{
    openknx.console.printHelpLine("power on", "Switch the air condition on");
    openknx.console.printHelpLine("power off", "Switch the air condition off");
    openknx.console.printHelpLine("temp <value>", "Set the target temperature in Celsius (e.g., temp 22.5)");
}
