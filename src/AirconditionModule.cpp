#include "AirconditionModule.h"
#include "Driver/Midea/MideaDriver.h"
#include "Driver/Mitsubishi/MitsubishiDriver.h"
#include "Driver/Toshiba/ToshibaDriver.h"
#include "OpenKNX.h"

AirconditionModule openknxAircondition;

const std::string AirconditionModule::name()
{
    return "Klimaanlage";
}

const std::string AirconditionModule::version()
{
    return std::to_string(MAIN_ApplicationVersion);
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
            logInfoP("Initialize MideaDriver");
            airConditionDriver = new MideaDriver(*this);
            break;
        case 2: // Midea
            logInfoP("Initialize ToshibaDriver");
            airConditionDriver = new ToshibaDriver(*this);
            break;
        case 3: // Mitsubishi
            logInfoP("Initialize MitsubishiDriver");
            airConditionDriver = new MitsubishiDriver(*this);
            break;
        case 4: // Toshiba
            logInfoP("Initialize ToshibaDriver");
            airConditionDriver = new ToshibaDriver(*this);
            break;
        default:
            logErrorP("AirCondition Device Type %d not supported", ParamAIR_DeviceType);
            break;
    }
    if (airConditionDriver != nullptr)
    {
        setLocked(false);
        logInfoP("Start driver");
        airConditionDriver->setup();
        logInfoP("Driver started");
        logInfoP("Start communication");
        driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
        airConditionDriver->startCommunication(false);
        logInfoP("Communication started");
    }
    else
    {
        driverStateChanged(AirConditionDriverState::AirConditionDriverStateNotStarted);
    }
}

void AirconditionModule::setLocked(bool locked)
{
    bool initialized = KoAIR_LockReleaseState.initialized();
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
                {
                    logInfoP("AirCondition released");
                    handleChangeParameterValue = ParamAIR_ReleaseBehaviorOn;
                }
                else
                {
                    logInfoP("AirCondition not released");
                    handleChangeParameterValue = ParamAIR_ReleaseBehaviorOff;
                }
            }
        }
        break;
        case 2:
        {
            if (KoAIR_LockReleaseState.valueCompare(locked, DPT_Switch) && initialized)
            {
                if (locked)
                {
                    logInfoP("AirCondition locked");
                    handleChangeParameterValue = ParamAIR_LockBehaviorOn;
                }
                else
                {
                    logInfoP("AirCondition unlocked");
                    handleChangeParameterValue = ParamAIR_LockBehaviorOff;
                }
            }
        }
    }
    if (!initialized)
    {
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
}

void AirconditionModule::loop()
{
    if (airConditionDriver != nullptr)
    {
        airConditionDriver->loop();
        if (_errorSince > 0 && millis() - _errorSince > 60000) // If error is set for more than 60 seconds, try to restart
        {
            logInfoP("AirCondition Error still active since %lu seconds. Restart communication", (millis() - _errorSince) / 1000);
            driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
            airConditionDriver->startCommunication(true);
            logInfoP("Communication restarted");
        }
        else if (_driverState == AirConditionDriverState::AirConditionDriverStateOk && _initialDataNeeded)
        {
            // If driver is ok and initial data is needed, request all data again
            logInfoP("Request all data after driver started");
             _initialDataNeeded = false; // Reset initial data needed flag
            airConditionDriver->requestAllData();
        }
    }
}

void AirconditionModule::showInformations()
{
    if (airConditionDriver != nullptr)
    {
        logInfoP("AirCondition Driver '%s'", airConditionDriver->name().c_str());
        logInfoP("Driver State: %s %s",AirConditionDriver::getDriverStateString(_driverState), _errorMessage.c_str());
        logInfoP("Minimum Target Temperature: %.1f °C", airConditionDriver->getMinimumTargetTemperature());
        logInfoP("Maximum Target Temperature: %.1f °C", airConditionDriver->getMaximumTargetTemperature());
        logInfoP("Maximum Fan Speed: %u", airConditionDriver->getMaximumFanSpeed());
        logInfoP("Maximum Horizontal Fix Position: %u", airConditionDriver->getMaximumHorizontalFixPosition());
        logInfoP("Maximum Vertical Fix Position: %u", airConditionDriver->getMaximumVertiacalFixPosition());
        logInfoP("Current Driver State: %d", _driverState);
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
    if (cmd == "rc")
    {
        if (airConditionDriver != nullptr)
        {
            logInfoP("Restart communication");
            driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
            airConditionDriver->startCommunication(true);
            logInfoP("Communication started");
        }
        else
        {
            logErrorP("No AirCondition Driver initialized");
        }
        return true;
    }
    if (cmd == "all")
    {
        if (airConditionDriver != nullptr)
        {
            logInfoP("Request all data");
            airConditionDriver->requestAllData();
            logInfoP("Request all data");
        }
        else
        {
            logErrorP("No AirCondition Driver initialized");
        }
        return true;
    }
    else if (cmd == "power on")
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
                KoAIR_FanSpeed.valueNoSend((uint8_t)fanSpeedPercent, DPT_Scaling);
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

void AirconditionModule::processInputKo(GroupObject& ko)
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
            {
                bool power = ko.value(DPT_Switch);
                airConditionDriver->setPower(power);
                if (power)
                    airConditionDriver->setMode(_lastMode); // Reset mode to last known state
                break;
            }
            case AIR_KoOperationMode:
            {
                uint8_t hvacMode = ko.value(DPT_Value_1_Ucount /*DPT_HVACContrMode currently not supported*/);
                switch (hvacMode)
                {
                    case 0: // Auto
                        logInfoP("Set mode to Auto");
                        _lastMode = AirConditionMode::AirConditionModeAuto;
                        airConditionDriver->setMode(AirConditionMode::AirConditionModeAuto);
                        break;
                    case 1: // Head
                        logInfoP("Set mode to Heat");
                        _lastMode = AirConditionMode::AirConditionModeHeat;
                        airConditionDriver->setMode(AirConditionMode::AirConditionModeHeat);
                        break;
                    case 3: // Cool
                        logInfoP("Set mode to Cool");
                        _lastMode = AirConditionMode::AirConditionModeCool;
                        airConditionDriver->setMode(AirConditionMode::AirConditionModeCool);
                        break;
                    case 6: // Off
                        logInfoP("Set mode to Off");
                        airConditionDriver->setPower(false);
                        break;
                    case 9: // Fan
                        logInfoP("Set mode to Fan");
                        _lastMode = AirConditionMode::AirConditionModeFan;
                        airConditionDriver->setMode(AirConditionMode::AirConditionModeFan);
                        break;
                    case 14: // Dry
                        logInfoP("Set mode to Dry");
                        _lastMode = AirConditionMode::AirConditionModeDry;
                        airConditionDriver->setMode(AirConditionMode::AirConditionModeDry);
                        break;
                    default:
                        logErrorP("Unknown HVAC mode %d", hvacMode);
                }
                if (hvacMode != 6) // If not Off, set power to true
                {
                    airConditionDriver->setPower(true);
                }
            }
            break;
            case AIR_KoOperationModeAutomatic:
                if (ko.value(DPT_Switch))
                {
                    logInfoP("Set mode to Auto");
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeAuto);
                }
                else
                {
                    logInfoP("Set power to Off");
                    airConditionDriver->setPower(false);
                }
                break;
            case AIR_KoOperationModeCooling:
                if (ko.value(DPT_Switch))
                {
                    logInfoP("Set mode to Cool");
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeCool);
                }
                else
                {
                    logInfoP("Set power to Off");
                    airConditionDriver->setPower(false);
                }
                break;
            case AIR_KoOperationModeHeating:
                if (ko.value(DPT_Switch))
                {
                    logInfoP("Set mode to Heat");
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeHeat);
                }
                else
                {
                    logInfoP("Set power to Off");
                    airConditionDriver->setPower(false);
                }
                break;
            case AIR_KoOperationModeVentilation:
                if (ko.value(DPT_Switch))
                {
                    logInfoP("Set mode to Fan");
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeFan);
                }
                else
                {
                    logInfoP("Set power to Off");
                    airConditionDriver->setPower(false);
                }
                break;
            case AIR_KoOperationModeDehumidification:
                if (ko.value(DPT_Switch))
                {
                    logInfoP("Set mode to Dry");
                    airConditionDriver->setMode(AirConditionMode::AirConditionModeDry);
                }
                else
                {
                    logInfoP("Set power to Off");
                    airConditionDriver->setPower(false);
                }
                break;
            case AIR_KoFanSpeed:
            {
                uint8_t fanSpeedPercent = ko.value(DPT_Scaling);
                unsigned int fanSpeed = airConditionDriver->getMaximumFanSpeed() * fanSpeedPercent / 100;
                logInfoP("Set fan speed to %u (%u%%)", fanSpeed, fanSpeedPercent);
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
                    {
                        logInfoP("Increase fan speed from %u to %u", currentFanSpeed, currentFanSpeed + 1);
                        airConditionDriver->setFanSpeed(currentFanSpeed + 1);
                    }
                    else
                    {
                        logInfoP("Fan speed is already at maximum %u", currentFanSpeed);
                    }
                }
                else
                {
                    // Decrease fan speed
                    if (currentFanSpeed > 0)
                    {
                        logInfoP("Decrease fan speed from %u to %u", currentFanSpeed, currentFanSpeed - 1);
                        airConditionDriver->setFanSpeed(currentFanSpeed - 1);
                    }
                    else
                    {
                        logInfoP("Fan speed is already at minimum 0");
                    }
                }
            }
            break;
            case AIR_KoLouverVerticalMove:
                logInfoP("Set vertical swing to %s", ko.value(DPT_Switch) ? "on" : "off");
                airConditionDriver->setSwingVertical(ko.value(DPT_Switch));
                break;
            case AIR_KoLouverHorizontalMove:
                logInfoP("Set horizontal swing to %s", ko.value(DPT_Switch) ? "on" : "off");
                airConditionDriver->setSwingHorizontal(ko.value(DPT_Switch));
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
                logInfoP("Set target temperature to %.1f °C", targetTemperature);
                airConditionDriver->setTargetTemperature(targetTemperature);
            }
            break;
            case AIR_KoSetTemperatureUpDown:
            {
                float currentTemperature = KoAIR_SetTemperatureState.value(DPT_Value_Temp);
                if (ko.value(DPT_Switch))
                {
                    if (currentTemperature < airConditionDriver->getMaximumTargetTemperature())
                    {
                        logInfoP("Increase target temperature from %.1f °C to %.1f °C", currentTemperature, currentTemperature + 1.f);
                        airConditionDriver->setTargetTemperature(currentTemperature + 1.f);
                    }
                    else
                    {
                        logInfoP("Target temperature is already at maximum %.1f °C", airConditionDriver->getMaximumTargetTemperature());
                    }
                }
                else
                {
                    if (currentTemperature > airConditionDriver->getMinimumTargetTemperature())
                    {
                        logInfoP("Decrease target temperature from %.1f °C to %.1f °C", currentTemperature, currentTemperature - 1.f);
                        airConditionDriver->setTargetTemperature(currentTemperature - 1.f);
                    }
                    else
                    {
                        logInfoP("Target temperature is already at minimum %.1f °C", airConditionDriver->getMinimumTargetTemperature());
                    }
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
                logInfoP("Set external sensor room temperature to %.1f °C", roomTemperature);
                airConditionDriver->setExternalSensorRoomTemperature(roomTemperature);
            }
            break;
        };
        airConditionDriver->processInputKo(ko);
    }
}

void AirconditionModule::powerChanged(bool power)
{
    logInfoP("AirCondition report power changed to %s", power ? "on" : "off");
    KoAIR_PowerState.valueCompare(power, DPT_Switch);
    if (!power)
    {
        KoAIR_OperationModeState.valueCompare((uint8_t)6, DPT_Value_1_Ucount /*DPT_HVACContrMode currently not supported*/);
        KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
    }
    else
    {
        KoAIR_OperationModeAutomaticState.valueCompare(true, DPT_Switch);
        KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
    }
}
void AirconditionModule::targetTemperatureChanged(float temperature)
{
    logInfoP("AirCondition report target temperature changed to %.1f °C", temperature);
    KoAIR_SetTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}
void AirconditionModule::fanSpeedChanged(int fanSpeed)
{
    unsigned int fanSpeedPercent = fanSpeed * 100 / airConditionDriver->getMaximumFanSpeed();
    logInfoP("AirCondition report fan speed changed to %u (%u%%)", fanSpeed, fanSpeedPercent);
    KoAIR_FanSpeedState.valueCompare((uint8_t)fanSpeedPercent, DPT_Scaling);
}
void AirconditionModule::modeChanged(AirConditionMode mode)
{
    _lastMode = mode;
    uint8_t hvacMode = 0;
    switch (mode)
    {
        case AirConditionMode::AirConditionModeAuto:
            logInfoP("AirCondition report mode changed to Auto");
            hvacMode = 0;
            KoAIR_OperationModeAutomaticState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeCool:
            logInfoP("AirCondition report mode changed to Cool");
            hvacMode = 3;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeHeat:
            logInfoP("AirCondition report mode changed to Heat");
            hvacMode = 1;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeDry:
            logInfoP("AirCondition report mode changed to Dry");
            hvacMode = 14;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(true, DPT_Switch);
            break;
        case AirConditionMode::AirConditionModeFan:
            logInfoP("AirCondition report mode changed to Fan");
            hvacMode = 9;
            KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
            KoAIR_OperationModeVentilationState.valueCompare(true, DPT_Switch);
            KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
            break;
        default:
            logErrorP("AirCondition report mode changed to unknown mode %d", mode);
            return;
    }
    KoAIR_OperationModeState.valueCompare(hvacMode, DPT_Value_1_Ucount /*DPT_HVACContrMode currently not supported*/);
}

void AirconditionModule::swingHorizontalChanged(bool swing)
{
    logInfoP("AirCondition report horizontal swing changed to %s", swing ? "on" : "off");
    KoAIR_LouverHorizontalMoveState.valueNoSend(swing, DPT_Switch);
}

void AirconditionModule::swingVerticalChanged(bool swing)
{
    logInfoP("AirCondition report vertical swing changed to %s", swing ? "on" : "off");
    KoAIR_LouverVerticalMoveState.valueNoSend(swing, DPT_Switch);
}

void AirconditionModule::swingHorizontalFixPositionChanged(int position)
{
    logInfoP("AirCondition report horizontal fix position changed to %d", position);
    // To Do: Implementation for horizontal fix position KO
}

void AirconditionModule::swingVerticalFixPositionChanged(int position)
{
    logInfoP("AirCondition report vertical fix position changed to %d", position);
    // To Do: Implementation for vertical fix position KO
}

void AirconditionModule::roomTemperatureChanged(float temperature)
{
    logInfoP("AirCondition report room temperature changed to %.1f °C", temperature);
    KoAIR_RoomTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}

void AirconditionModule::outsideTemperaturChanged(float temperature)
{
    logInfoP("AirCondition report outside temperature changed to %.1f °C", temperature);
    KoAIR_OutsideTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}



void AirconditionModule::driverStateChanged(AirConditionDriverState state, std::string error)
{
    if (_driverState != state || 
        state == AirConditionDriverState::AirConditionDriverStateNotStarted || 
        state == AirConditionDriverState::AirConditionDriverStateStarting || 
        state == AirConditionDriverState::AirConditionDriverStateError)
    {
        _driverState = state;
        switch (state)
        {
            case AirConditionDriverState::AirConditionDriverStateNotStarted:
                logInfoP("AirCondition Driver state: %s", AirConditionDriver::getDriverStateString(state));
                _errorSince = 0; // Reset error timestamp
                _errorMessage = "";
                _initialDataNeeded = true; 
                break;
            case AirConditionDriverState::AirConditionDriverStateStarting:
                 logInfoP("AirCondition Driver state: %s", AirConditionDriver::getDriverStateString(state));
                _errorSince = max(1UL, millis()); // Set error timestamp for handling restart
                _errorMessage = "";
                _initialDataNeeded = true; 
                break;
            case AirConditionDriverState::AirConditionDriverStateOk:
                 logInfoP("AirCondition Driver state: %s",  AirConditionDriver::getDriverStateString(state));
                _errorSince = 0; // Reset error timestamp
                _errorMessage = "";
                break;
            case AirConditionDriverState::AirConditionDriverStateError:
                if (error == "")
                    error ="Unknown error";
                if (_errorMessage != error)
                {
                    _errorSince = max(1UL, millis());
                    _errorMessage = error;
                    logErrorP("AirCondition Driver state: %s error: %s", AirConditionDriver::getDriverStateString(state), _errorMessage.c_str());
                }
                break;
        }
    }
}

void AirconditionModule::showHelp()
{
    openknx.console.printHelpLine("power on", "Switch the air condition on");
    openknx.console.printHelpLine("power off", "Switch the air condition off");
    openknx.console.printHelpLine("temp <value>", "Set the target temperature in Celsius (e.g., temp 22.5)");
    openknx.console.printHelpLine("rc", "Restart the air condition communication");
    openknx.console.printHelpLine("all", "Request all data from the air condition");
}
