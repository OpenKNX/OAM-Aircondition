#include "AirconditionModule.h"
#include "Driver/Daikin/DaikinDriver.h"
#include "Driver/Midea/MideaDriver.h"
#include "Driver/Mitsubishi/MitsubishiDriver.h"
#include "Driver/Toshiba/ToshibaDriver.h"
#include "NetworkModule.h"
#include "OpenKNX.h"
#include "SceneHandler.h"

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
            logInfoP("Initialize DaikinDriver");
            _airConditionDriver = new DaikinDriver(*this);
            break;
        case 2: // Midea
            logInfoP("Initialize MideaDriver");
            _airConditionDriver = new MideaDriver(*this);
            break;
        case 3: // Mitsubishi
            logInfoP("Initialize MitsubishiDriver");
            _airConditionDriver = new MitsubishiDriver(*this);
            break;
        case 4: // Toshiba
            logInfoP("Initialize ToshibaDriver");
            _airConditionDriver = new ToshibaDriver(*this);
            break;
        default:
            logErrorP("AirCondition Device Type %d not supported", ParamAIR_DeviceType);
            break;
    }
    if (_airConditionDriver != nullptr)
    {
        _sceneHandler = new SceneHandler(*_airConditionDriver);
        setLocked(false);
        logInfoP("Start driver");
        _airConditionDriver->setup();
        logInfoP("Driver started");
        logInfoP("Start communication");
        driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
        _airConditionDriver->startCommunication(false);
        logInfoP("Communication started");

        // <Enumeration Text="WLAN Status" Value="0" Id="%ENID%" />
        // <Enumeration Text="Aus" Value="1" Id="%ENID%" />
        // <Enumeration Text="Ein" Value="2" Id="%ENID%" />
        // <Enumeration Text="Schaltbar über Gruppenobjekt" Value="3" Id="%ENID%" />
        if (ParamAIR_WifiLED == 3 && !KoAIR_WifiLED.initialized())
            KoAIR_WifiLED.requestObjectRead();
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
    if (initialized)
    {
        // <Enumeration Text="Keine Änderung" Value="0" Id="%ENID%" />
        // <Enumeration Text="Einschalten" Value="1" Id="%ENID%" />
        // <Enumeration Text="Ausschalten" Value="2" Id="%ENID%" />
        switch (handleChangeParameterValue)
        {
            case 0: // Do nothing
                break;
            case 1: // Power on
                _airConditionDriver->setPower(true);
                break;
            case 2: // Power off
                _airConditionDriver->setPower(false);
                break;
        }
    }
}

void AirconditionModule::loop()
{
    if (_airConditionDriver != nullptr)
    {
        _airConditionDriver->loop();
        if (_lastWifiLedDebounceRunning != 0 && millis() - _lastWifiLedDebounceRunning > 1000)
        {
            _lastWifiLedDebounceRunning = 0;
        }
        if (_errorSince > 0 && millis() - _errorSince > 60000) // If error is set for more than 60 seconds, try to restart
        {
            logInfoP("AirCondition Error still active since %lu seconds. Restart communication", (millis() - _errorSince) / 1000);
            driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
            _airConditionDriver->startCommunication(true);
            logInfoP("Communication restarted");
        }
        else if (_driverState == AirConditionDriverState::AirConditionDriverStateOk)
        {
            bool on = false;
            bool needDebounce = false;

            // <Enumeration Text="WLAN Status" Value="0" Id="%ENID%" />
            // <Enumeration Text="Aus" Value="1" Id="%ENID%" />
            // <Enumeration Text="Ein" Value="2" Id="%ENID%" />
            // <Enumeration Text="Schaltbar über Gruppenobjekt" Value="3" Id="%ENID%" />
            switch (ParamAIR_WifiLED)
            {
                case 0: // WLAN Status
#if defined(KNX_IP_WIFI) || defined(KNX_IP_LAN)
                    on = openknxNetwork.connected();
#endif
                    needDebounce = true;
                    break;
                case 1: // Always off
                    on = false;
                    break;
                case 2: // Always on
                    on = true;
                    break;
                case 3: // Switchable via Group Object
                    on = KoAIR_WifiLED.value(DPT_Switch);

                    break;
            }
            if (_forceWifiLedState || (on != _lastWifiLedState && _lastWifiLedDebounceRunning == 0))
            {
                _forceWifiLedState = false;
                _lastWifiLedState = on;
                if (needDebounce)
                    _lastWifiLedDebounceRunning = max(1UL, millis());
                logInfoP("Wifi LED state changed to %s", on ? "ON" : "OFF");
                _airConditionDriver->setWifiLed(on);
            }

            if (_initialDataNeeded)
            {
                // If driver is ok and initial data is needed, request all data again
                logInfoP("Request all data after driver started");
                _initialDataNeeded = false; // Reset initial data needed flag
                _airConditionDriver->requestAllData();
            }
        }
        handleDebouncedModeChange();
    }
}

void AirconditionModule::showInformations()
{
    if (_airConditionDriver != nullptr)
    {
        logInfoP("AirCondition Driver '%s'", _airConditionDriver->name().c_str());
        logInfoP("Driver State: %s %s", AirConditionDriver::getDriverStateString(_driverState), _errorMessage.c_str());
        logInfoP("Minimum Target Temperature: %.1f °C", _airConditionDriver->getMinimumTargetTemperature());
        logInfoP("Maximum Target Temperature: %.1f °C", _airConditionDriver->getMaximumTargetTemperature());
        logInfoP("Maximum Fan Speed: %u", _airConditionDriver->getMaximumFanSpeed());
        logInfoP("Maximum Horizontal Fix Position: %u", _airConditionDriver->getMaximumHorizontalFixPosition());
        logInfoP("Maximum Vertical Fix Position: %u", _airConditionDriver->getMaximumVerticalFixPosition());
        logInfoP("Current Driver State: %d", _driverState);
        _airConditionDriver->showInformations();
    }
    else
    {
        logErrorP("No AirCondition Driver initialized");
    }
}

bool AirconditionModule::processCommand(const std::string cmd, bool debugKo)
{
    if (_airConditionDriver == nullptr)
    {
        if (_airConditionDriver->processCommand(cmd, debugKo) == true)
        {
            return true;
        }
    }
    if (cmd == "rc")
    {
        if (_airConditionDriver != nullptr)
        {
            logInfoP("Restart communication");
            driverStateChanged(AirConditionDriverState::AirConditionDriverStateStarting);
            _airConditionDriver->startCommunication(true);
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
        if (_airConditionDriver != nullptr)
        {
            logInfoP("Request all data");
            _airConditionDriver->requestAllData();
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
    else if (cmd.length() >= 5 && cmd.substr(0, 5) == "temp ")
    {
         std::string tempStr = cmd.substr(5);
        float temperature = std::stof(tempStr);
        KoAIR_SetTemperature.valueNoSend(temperature, DPT_Value_Temp);
        processInputKo(KoAIR_SetTemperature);
        return true;
    }
    else if (cmd.length() >= 5 && cmd.substr(0, 5) == "room ")
    {
        std::string tempStr = cmd.substr(5);
        float temperature = std::stof(tempStr);
        KoAIR_RoomTemperatureInput.valueNoSend(temperature, DPT_Value_Temp);
        processInputKo(KoAIR_RoomTemperatureInput);
        return true;
    }
    else if (cmd.length() >= 4 && cmd.substr(0, 4) == "fan ")
    {
        // Extract the fan speed value from the command
        std::string fanStr = cmd.substr(4);

        unsigned int fanSpeedPercent = std::stoi(fanStr);
        if (_airConditionDriver != nullptr && fanSpeedPercent <= 100)
        {
            KoAIR_FanSpeed.valueNoSend((uint8_t)fanSpeedPercent, DPT_Scaling);
            processInputKo(KoAIR_FanSpeed);
        }
        else
        {
            logErrorP("Invalid fan speed value: %s. Value must between 0 and %u.", fanStr.c_str(), _airConditionDriver->getMaximumFanSpeed());
        }
        return true;
    }
    else if (cmd == "fan+")
    {
        KoAIR_FanSpeedUpDown.valueNoSend(true, DPT_Switch);
        processInputKo(KoAIR_FanSpeedUpDown);
        return true;
    }
    else if (cmd == "fan-")
    {
        KoAIR_FanSpeedUpDown.valueNoSend(false, DPT_Switch);
        processInputKo(KoAIR_FanSpeedUpDown);
        return true;
    }

    return false;
}

void AirconditionModule::handleDebouncedModeChange()
{
    if (_airConditionDriver == nullptr || _waitingForModeChange == 0 || millis() - _waitingForModeChange < 100)
        return;
    _waitingForModeChange = 0; // Reset waiting for mode change
    if (_waitingForCooling && _waitingForHeating)
    {
        // Heading and Colling are both requested, set to Auto
        _waitingForAuto = true;
        _waitingForCooling = false;
        _waitingForHeating = false;
    }
    if (_waitingForAuto)
    {
        logInfoP("Set mode to Auto");
        _airConditionDriver->setMode(AirConditionMode::AirConditionModeAuto);
        _airConditionDriver->setPower(true);
    }
    else if (_waitingForCooling)
    {
        logInfoP("Set mode to Cool");
        _airConditionDriver->setMode(AirConditionMode::AirConditionModeCool);
        _airConditionDriver->setPower(true);
    }
    else if (_waitingForHeating)
    {
        logInfoP("Set mode to Heat");
        _airConditionDriver->setMode(AirConditionMode::AirConditionModeHeat);
        _airConditionDriver->setPower(true);
    }
    else if (_waitingForFan)
    {
        logInfoP("Set mode to Fan");
        _airConditionDriver->setMode(AirConditionMode::AirConditionModeFan);
        _airConditionDriver->setPower(true);
    }
    else if (_waitingForDehumidification)
    {
        logInfoP("Set mode to Dry");
        _airConditionDriver->setMode(AirConditionMode::AirConditionModeDry);
        _airConditionDriver->setPower(true);
    }
    else
    {
        logInfoP("Set power to Off");
        _airConditionDriver->setPower(false);
    }
    _waitingForAuto = false;
    _waitingForCooling = false;
    _waitingForHeating = false;
    _waitingForFan = false;
    _waitingForDehumidification = false;
}

void AirconditionModule::processInputKo(GroupObject& ko)
{
    if (_airConditionDriver != nullptr)
    {
        // <Enumeration Text="Keines" Value="0" Id="%ENID%" />
        // <Enumeration Text="Freigabe" Value="1" Id="%ENID%" />
        // <Enumeration Text="Sperre" Value="2" Id="%ENID%" />
        switch (ParamAIR_LockReleaseKo)
        {
            case 1: // Release
                if (ko.asap() == AIR_KoLockRelease)
                {
                    setLocked(!ko.value(DPT_Switch));
                    return;
                }
                if (!KoAIR_LockReleaseState.value(DPT_Switch))
                    return;
                break;
            case 2: // Lock
                if (ko.asap() == AIR_KoLockRelease)
                {
                    setLocked(ko.value(DPT_Switch));
                    return;
                }
                if (KoAIR_LockReleaseState.value(DPT_Switch))
                    return;
                break;
        }
        switch (ko.asap())
        {
            case AIR_KoPower:
            {
                bool power = ko.value(DPT_Switch);
                _airConditionDriver->setPower(power);
            }
            break;
            case AIR_KoOperationMode:
            {
                uint8_t hvacMode = ko.value(DPT_Value_1_Ucount /*DPT_HVACContrMode currently not supported*/);
                switch (hvacMode)
                {
                    case 0: // Auto
                        logInfoP("Set mode to Auto");
                        _lastMode = AirConditionMode::AirConditionModeAuto;
                        _airConditionDriver->setMode(AirConditionMode::AirConditionModeAuto);
                        break;
                    case 1: // Head
                        logInfoP("Set mode to Heat");
                        _lastMode = AirConditionMode::AirConditionModeHeat;
                        _airConditionDriver->setMode(AirConditionMode::AirConditionModeHeat);
                        break;
                    case 3: // Cool
                        logInfoP("Set mode to Cool");
                        _lastMode = AirConditionMode::AirConditionModeCool;
                        _airConditionDriver->setMode(AirConditionMode::AirConditionModeCool);
                        break;
                    case 6: // Off
                        logInfoP("Set mode to Off");
                        _airConditionDriver->setPower(false);
                        break;
                    case 9: // Fan
                        logInfoP("Set mode to Fan");
                        _lastMode = AirConditionMode::AirConditionModeFan;
                        _airConditionDriver->setMode(AirConditionMode::AirConditionModeFan);
                        break;
                    case 14: // Dry
                        logInfoP("Set mode to Dry");
                        _lastMode = AirConditionMode::AirConditionModeDry;
                        _airConditionDriver->setMode(AirConditionMode::AirConditionModeDry);
                        break;
                    default:
                        logErrorP("Unknown HVAC mode %d", hvacMode);
                }
                if (hvacMode != 6) // If not Off, set power to true
                {
                    _airConditionDriver->setPower(true);
                }
            }
            break;
            case AIR_KoOperationModeAutomatic:
                _waitingForAuto = ko.value(DPT_Switch);
                _waitingForModeChange = max(1UL, millis());
                break;
            case AIR_KoOperationModeCooling:
                _waitingForCooling = ko.value(DPT_Switch);
                _waitingForModeChange = max(1UL, millis());
                break;
            case AIR_KoOperationModeHeating:
                _waitingForHeating = ko.value(DPT_Switch);
                _waitingForModeChange = max(1UL, millis());
                break;
            case AIR_KoOperationModeVentilation:
                _waitingForFan = ko.value(DPT_Switch);
                _waitingForModeChange = max(1UL, millis());
                break;
            case AIR_KoOperationModeDehumidification:
                _waitingForDehumidification = ko.value(DPT_Switch);
                _waitingForModeChange = max(1UL, millis());
                break;
            case AIR_KoFanSpeed:
            {
                float fanSpeedPercent = ko.value(DPT_Scaling);
                unsigned int fanSpeed = round((float)_airConditionDriver->getMaximumFanSpeed() * fanSpeedPercent / 100.f);
                logInfoP("Set fan speed to %u (%u%%)", fanSpeed, fanSpeedPercent);
                _airConditionDriver->setFanSpeed(fanSpeed);
            }
            break;
            case AIR_KoFanSpeedUpDown:
            {
                float currentFanSpeedPercentage = KoAIR_FanSpeedState.value(DPT_Scaling);
                unsigned int currentFanSpeed = round((float)_airConditionDriver->getMaximumFanSpeed() * currentFanSpeedPercentage / 100.f);
                if (ko.value(DPT_Switch))
                {
                    // Increase fan speed
                    if (currentFanSpeed < _airConditionDriver->getMaximumFanSpeed())
                    {
                        logInfoP("Increase fan speed from %u to %u", currentFanSpeed, currentFanSpeed + 1);
                        _airConditionDriver->setFanSpeed(currentFanSpeed + 1);
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
                        _airConditionDriver->setFanSpeed(currentFanSpeed - 1);
                    }
                    else
                    {
                        logInfoP("Fan speed is already at minimum 0");
                    }
                }
            }
            break;
            case AIR_KoLouverVerticalSwing:
                logInfoP("Set vertical swing to %s", ko.value(DPT_Switch) ? "on" : "off");
                _airConditionDriver->setSwingVertical(ko.value(DPT_Switch));
                break;
            case AIR_KoLouverVerticalPosition:
            {
                float verticalPositionPercent = ko.value(DPT_Scaling);
                unsigned int verticalPosition = round((float)_airConditionDriver->getMaximumVerticalFixPosition() * verticalPositionPercent / 100.f);
                if (verticalPosition > _airConditionDriver->getMaximumVerticalFixPosition())
                {
                    logErrorP("Vertical position %u is out of range (0 - %u)", verticalPosition, _airConditionDriver->getMaximumVerticalFixPosition());
                    return;
                }
                logInfoP("Set vertical swing position to %u", verticalPosition);
                _airConditionDriver->setSwingVerticalFixPosition(verticalPosition);
            }
            break;
            case AIR_KoLouverHorizontalSwing:
                logInfoP("Set horizontal swing to %s", ko.value(DPT_Switch) ? "on" : "off");
                _airConditionDriver->setSwingHorizontal(ko.value(DPT_Switch));
                break;

            case AIR_KoSetTemperature:
            {
                float targetTemperature = ko.value(DPT_Value_Temp);
                if (targetTemperature < _airConditionDriver->getMinimumTargetTemperature() || targetTemperature > _airConditionDriver->getMaximumTargetTemperature())
                {
                    logErrorP("Target temperature %f is out of range (%f - %f)", targetTemperature,
                              _airConditionDriver->getMinimumTargetTemperature(),
                              _airConditionDriver->getMaximumTargetTemperature());
                    return;
                }
                setTargetTemperaturToAircondition(targetTemperature);
            }
            break;
            case AIR_KoSetTemperatureUpDown:
            {
                float currentTemperature = KoAIR_SetTemperatureState.value(DPT_Value_Temp);
                if (ko.value(DPT_Switch))
                {
                    if (currentTemperature < _airConditionDriver->getMaximumTargetTemperature())
                    {
                        logInfoP("Increase target temperature from %.1f °C to %.1f °C", currentTemperature, currentTemperature + 1.f);
                        setTargetTemperaturToAircondition(currentTemperature + 1.f);
                    }
                    else
                    {
                        logInfoP("Target temperature is already at maximum %.1f °C", _airConditionDriver->getMaximumTargetTemperature());
                    }
                }
                else
                {
                    if (currentTemperature > _airConditionDriver->getMinimumTargetTemperature())
                    {
                        logInfoP("Decrease target temperature from %.1f °C to %.1f °C", currentTemperature, currentTemperature - 1.f);
                        setTargetTemperaturToAircondition(currentTemperature - 1.f);
                    }
                    else
                    {
                        logInfoP("Target temperature is already at minimum %.1f °C", _airConditionDriver->getMinimumTargetTemperature());
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
                _airConditionDriver->setExternalSensorRoomTemperature(roomTemperature);
            }
            break;
            case AIR_KoScene:
            {
                uint8_t scene = 1 + (uint8_t)ko.value(DPT_SceneNumber);
                if (_sceneHandler != nullptr)
                {
                    logInfoP("Trigger scene to %u", scene);
                    _sceneHandler->handleScene(scene);
                }
                else
                {
                    logErrorP("No SceneHandler initialized");
                }
            }
            case AIR_KoPowerLimit:
            {
                uint8_t powerLimit = (uint8_t)ko.value(DPT_Scaling);
                logInfoP("Set power limit to %u", powerLimit);
                _airConditionDriver->setMaxPowerLevel(powerLimit);
            }
            break;
            case AIR_KoDeviceMode:
            {
                uint8_t deviceMode = (uint8_t)ko.value(DPT_SceneNumber) + 1;
                logInfoP("Set device mode to %u", deviceMode);
                _airConditionDriver->setDeviceMode((AirConditionDeviceMode) deviceMode);
            }
            break;
            case AIR_KoDeviceModeStandard:
            {
                logInfoP("Set device mode to Standard");
                _airConditionDriver->setDeviceMode(AirConditionDeviceMode::AirConditionDeviceModeStandard);
            }
            break;
            case AIR_KoDeviceModeHiPower:
            {
                logInfoP("Set device mode to HiPower");
                _airConditionDriver->setDeviceMode(AirConditionDeviceMode::AirConditionDeviceModeHiPower);
            }
            break;
            case AIR_KoDeviceModeEco:
            {
                logInfoP("Set device mode to Eco");
                _airConditionDriver->setDeviceMode(AirConditionDeviceMode::AirConditionDeviceModeEco);
            }
            break;
            case AIR_KoDeviceModeSilent1:
            {
                logInfoP("Set device mode to Silent1");
                _airConditionDriver->setDeviceMode(AirConditionDeviceMode::AirConditionDeviceModeSilent1);
            }
            break;
            case AIR_KoDeviceModeSilent2:
            {
                logInfoP("Set device mode to Silent2");
                _airConditionDriver->setDeviceMode(AirConditionDeviceMode::AirConditionDeviceModeSilent2);
            }
            break;
            case AIR_KoAirPurification:
            {
                bool airPurification = ko.value(DPT_Switch);
                logInfoP("Set air purification to %d", airPurification);
                _airConditionDriver->setAirPurification(airPurification);
            }
        };
        _airConditionDriver->processInputKo(ko);
    }
}

void AirconditionModule::setTargetTemperaturToAircondition(float temperature)
{
    if (_airConditionDriver != nullptr)
    {
       
        logInfoP("Set target temperature to %.1f °C", temperature);
        _airConditionDriver->setTargetTemperature(temperature);
    }
}

void AirconditionModule::powerChanged(bool power)
{
    logInfoP("AirCondition report power changed to %s", power ? "on" : "off");
    _lastPower = power;
    KoAIR_PowerState.valueCompare(power, DPT_Switch);
    if (power)
    {
        modeChanged(_lastMode); // Reset mode to last known state
    }
    else
    {
        KoAIR_OperationModeState.valueCompare((uint8_t)6 /* OFF */, DPT_Value_1_Ucount /*DPT_HVACContrMode currently not supported*/);
        KoAIR_OperationModeAutomaticState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeCoolingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeHeatingState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeVentilationState.valueCompare(false, DPT_Switch);
        KoAIR_OperationModeDehumidificationState.valueCompare(false, DPT_Switch);
    }
}
void AirconditionModule::targetTemperatureChanged(float temperature, bool isFeedbackFromSetting)
{
    logInfoP("AirCondition report target temperature changed to %.1f °C", temperature);
    KoAIR_SetTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}
void AirconditionModule::fanSpeedChanged(int fanSpeed)
{
    unsigned int fanSpeedPercent = fanSpeed * 100 / _airConditionDriver->getMaximumFanSpeed();
    logInfoP("AirCondition report fan speed changed to %u (%u%%)", fanSpeed, fanSpeedPercent);
    KoAIR_FanSpeedState.valueCompare((uint8_t)fanSpeedPercent, DPT_Scaling);
}
void AirconditionModule::modeChanged(AirConditionMode mode)
{
    _lastMode = mode;
    if (_lastPower == false)
    {
        logInfoP("AirCondition is off, not changing mode for KO's");
        return;
    }
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
    KoAIR_LouverHorizontalSwingState.valueCompare(swing, DPT_Switch);
}

void AirconditionModule::swingVerticalChanged(bool swing)
{
    logInfoP("AirCondition report vertical swing changed to %s", swing ? "on" : "off");
    KoAIR_LouverVerticalSwingState.valueCompare(swing, DPT_Switch);
}

void AirconditionModule::swingHorizontalFixPositionChanged(int position)
{
    unsigned int currentHorizontalPositionPercent = position * 100 / _airConditionDriver->getMaximumHorizontalFixPosition();
    logInfoP("AirCondition report horizontal fix position changed to %u (%u%%)", position, currentHorizontalPositionPercent);
    // To Do: Horizontal position KO
    // KoAIR_LouverHorizontalPositionState.valueCompare((uint8_t)currentHorizontalPositionPercent, DPT_Scaling);
}

void AirconditionModule::swingVerticalFixPositionChanged(int position)
{
    unsigned int currentVertialPositionPercent = position * 100 / _airConditionDriver->getMaximumVerticalFixPosition();
    logInfoP("AirCondition report vertical fix position changed to %u (%u%%)", position, currentVertialPositionPercent);
    KoAIR_LouverVerticalPositionState.valueCompare((uint8_t)currentVertialPositionPercent, DPT_Scaling);
}

void AirconditionModule::roomTemperatureChanged(float temperature)
{
    logInfoP("AirCondition report room temperature changed to %.1f °C", temperature);
    KoAIR_RoomTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}

void AirconditionModule::outsideTemperaturChanged(float temperature)
{
    if (temperature >= 100.f || temperature <= -100.f)
    {
        logInfoP("Outside temperature %f is out of range (-100 - 100)", temperature);
        return;
    }
    logInfoP("AirCondition report outside temperature changed to %.1f °C", temperature);
    KoAIR_OutsideTemperatureState.valueCompare(temperature, DPT_Value_Temp);
}

AirConditionDriverState AirconditionModule::getDriverState() const
{
    return _driverState;
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
                _lastWifiLedDebounceRunning = 0;
                _initialDataNeeded = true;
                break;
            case AirConditionDriverState::AirConditionDriverStateOk:
                logInfoP("AirCondition Driver state: %s", AirConditionDriver::getDriverStateString(state));
                _errorSince = 0; // Reset error timestamp
                _errorMessage = "";
                _forceWifiLedState = true;
                break;
            case AirConditionDriverState::AirConditionDriverStateError:
                if (error == "")
                    error = "Unknown error";
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

void AirconditionModule::maxPowerLevelChanged(uint8_t maxPower)
{
    logInfoP("AirCondition report max power level changed to %d", maxPower);
    KoAIR_PowerLimitState.valueCompare(maxPower, DPT_Scaling);
}

void AirconditionModule::deviceModeChanged(AirConditionDeviceMode mode)
{
    logInfoP("AirCondition report device mode changed to %d", (int)mode);
    KoAIR_DeviceModeState.valueCompare((uint8_t) (((uint8_t) mode) - 1), DPT_SceneNumber);
    KoAIR_DeviceModeStandardState.valueCompare(mode == AirConditionDeviceMode::AirConditionDeviceModeStandard, DPT_Switch);
    KoAIR_DeviceModeHiPowerState.valueCompare(mode == AirConditionDeviceMode::AirConditionDeviceModeHiPower, DPT_Switch);
    KoAIR_DeviceModeEcoState.valueCompare(mode == AirConditionDeviceMode::AirConditionDeviceModeEco, DPT_Switch);
    KoAIR_DeviceModeSilent1State.valueCompare(mode == AirConditionDeviceMode::AirConditionDeviceModeSilent1, DPT_Switch);
    KoAIR_DeviceModeSilent2State.valueCompare(mode == AirConditionDeviceMode::AirConditionDeviceModeSilent2, DPT_Switch);
}

void AirconditionModule::airPurificationChanged(bool on)
{
    logInfoP("AirCondition report air purification changed to %d", on);
    KoAIR_AirPurificationState.valueCompare(on, DPT_Switch);
}

void AirconditionModule::showHelp()
{
    openknx.console.printHelpLine("power on", "Switch the air condition on");
    openknx.console.printHelpLine("power off", "Switch the air condition off");
    openknx.console.printHelpLine("temp <value>", "Set the target temperature in Celsius (e.g., temp 22)");
    openknx.console.printHelpLine("fan <value>", "Set the fan speed in percent (0-100, e.g., fan 50)");
    openknx.console.printHelpLine("fan+", "Increase the fan speed");
    openknx.console.printHelpLine("fan-", "Decrease the fan speed");
    openknx.console.printHelpLine("rc", "Restart the air condition communication");
    openknx.console.printHelpLine("all", "Request all data from the air condition");
    openknx.console.printHelpLine("acroom <value>", "Simulate room temperature feedback from aircondition (e.g., acroom 22)");
    openknx.console.printHelpLine("room <value>", "Set the room temperature (e.g., room 22)");

}

void AirconditionModule::updatePower(bool power) {
    powerChanged(power);
}

void AirconditionModule::updateMode(AirConditionMode mode) {
    modeChanged(mode);
}

void AirconditionModule::updateTargetTemperature(float c) {
    targetTemperatureChanged(c, /*fromKnx=*/false);
}

void AirconditionModule::updateFanSpeed(int speed) {
    fanSpeedChanged(speed);
}

void AirconditionModule::updateSwingHorizontal(bool swing) {
    swingHorizontalChanged(swing);
}

void AirconditionModule::updateSwingVertical(bool swing) {
    swingVerticalChanged(swing);
}

void AirconditionModule::updateCurrentTemperature(float c) {
    roomTemperatureChanged(c);
}

void AirconditionModule::updateOutdoorTemperature(float c) {
    outsideTemperaturChanged(c);
}

void AirconditionModule::updateDeviceMode(AirConditionDeviceMode mode) {
    deviceModeChanged(mode);
}

void AirconditionModule::updateMaxPowerLevel(uint8_t percentage) {
    maxPowerLevelChanged(percentage);
}

void AirconditionModule::updateAirPurification(bool on) {
    airPurificationChanged(on);
}

void AirconditionModule::updateOnlineStatus(bool online) {
    logInfoP("AirCondition report online state: %s", online ? "online" : "offline");
    KoAIR_OnlineState.valueCompare(online, DPT_State);
}

void AirconditionModule::updateWifiLed(bool on) {
    // optional: publish to KO
}

void AirconditionModule::updateHumidity(uint8_t humidity) {
    logInfoP("AirCondition report humidity changed to %u%%", humidity);
    KoAIR_HumidityState.valueCompare((float)humidity, DPT_Value_Humidity);
}

void AirconditionModule::updateHumidityMode(uint8_t step) {
    const uint8_t levels = _airConditionDriver->getMaximumHumidityModeLevels();
    uint8_t percent = (levels > 1) ? uint8_t(std::round(100.0f * step / float(levels - 1))) : 0;

    logInfoP("AirCondition report humidity mode step=%u (levels=%u -> %u%%)", step, levels, percent);
    KoAIR_HumidityModePercent.valueCompare(percent, DPT_Scaling);
}

void AirconditionModule::updateTotalEnergyConsumption(uint32_t totalEnergyWh) {
    KoAIR_TotalEnergy.valueCompare(totalEnergyWh / 1000.0f, DPT_ActiveEnergy_kWh);
}

