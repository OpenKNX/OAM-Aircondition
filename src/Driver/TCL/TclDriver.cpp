#include "TclDriver.h"


/*
TCL protocol implementation

Sources used for the TCL driver implementation:
I-am-nightingale/tclac: https://github.com/I-am-nightingale/tclac.git
adaasch/AC-hack: https://github.com/adaasch/AC-hack
*/


#include <algorithm>
#include <cmath>
#include <cstdio>

const uint8_t TclDriver::PollCommand[TclDriver::PollCommandLength] = {0xBB, 0x00, 0x01, 0x04, 0x02, 0x01, 0x00, 0xBD};

TclDriver::TclDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : AirConditionDriver(statusFeedback)
{
}

const std::string TclDriver::name() const
{
    return "TCL";
}

void TclDriver::setup()
{
#if ARDUINO_ARCH_ESP32
    OPENKNX_AIR_CONDITION_SERIAL.begin(9600UL, SERIAL_8E1, OPENKNX_AIR_CONDITION_SERIAL_RX, OPENKNX_AIR_CONDITION_SERIAL_TX);
#else
    OPENKNX_AIR_CONDITION_SERIAL.setRX(OPENKNX_AIR_CONDITION_SERIAL_RX);
    OPENKNX_AIR_CONDITION_SERIAL.setTX(OPENKNX_AIR_CONDITION_SERIAL_TX);
    OPENKNX_AIR_CONDITION_SERIAL.begin(9600UL, SERIAL_8E1);
#endif
}

void TclDriver::startCommunication(bool restart)
{
    if (restart)
    {
        _rxBuffer.clear();
    }
    sendPoll();
}

void TclDriver::requestAllData()
{
    sendPoll();
}

bool TclDriver::readLiveDataDump(std::string& rawDump, std::string& decodedDump, uint32_t timeoutMs)
{
    rawDump.clear();
    decodedDump.clear();

    if (_controlPending)
    {
        sendControl();
    }

    _onlineReadInProgress = true;
    _onlineReadDone = false;
    _rxBuffer.clear();

    while (OPENKNX_AIR_CONDITION_SERIAL.available() > 0)
    {
        OPENKNX_AIR_CONDITION_SERIAL.read();
    }

    sendPoll();

    const unsigned long start = millis();
    while (millis() - start < timeoutMs)
    {
        while (OPENKNX_AIR_CONDITION_SERIAL.available() > 0)
        {
            handleReceivedByte((uint8_t)OPENKNX_AIR_CONDITION_SERIAL.read());
        }

        if (!_rxBuffer.empty() && millis() - _lastReceivedByte > ReceiveTimeoutMs)
        {
            logDebugP("TCL online read RX timeout, discard %u bytes", (unsigned int)_rxBuffer.size());
            _rxBuffer.clear();
        }

        if (_onlineReadDone)
        {
            rawDump = _lastRawDump;
            decodedDump = _lastDecodedDump;
            _onlineReadInProgress = false;
            return true;
        }

        delay(5);
    }

    _onlineReadInProgress = false;
    return false;
}

void TclDriver::loop()
{
    if (!_rxBuffer.empty() && millis() - _lastReceivedByte > ReceiveTimeoutMs)
    {
        logDebugP("TCL RX timeout, discard %u bytes", (unsigned int)_rxBuffer.size());
        _rxBuffer.clear();
    }

    while (OPENKNX_AIR_CONDITION_SERIAL.available() > 0)
    {
        handleReceivedByte((uint8_t)OPENKNX_AIR_CONDITION_SERIAL.read());
    }

    processPendingControlUpdate();
    if (_controlPending)
        return;

    const unsigned long now = millis();
    if (now - _lastPoll > PollIntervalMs && now - _lastCommand > PollAfterCommandDelayMs)
    {
        sendPoll();
    }
}

void TclDriver::showInformations()
{
    logInfoP("TCL state: power=%d mode=%d target=%.1f fan=%u swingV=%d swingH=%d posV=%u posH=%u sleep=%u eco=%d display=%d buzzer=%d health=%d turbo=%d lowNoise=%d ext=%u",
             _power,
             (int)_mode,
             _targetTemperature,
             _fanSpeed,
             _verticalSwing,
             _horizontalSwing,
             _verticalFixPosition,
             _horizontalFixPosition,
             _sleepMode,
             _eco,
             _display,
             _beep,
             _health,
             _turbo,
             _lowNoise,
             currentExtendedMode());
}

float TclDriver::getMinimumTargetTemperature()
{
    return 16.0f;
}

float TclDriver::getMaximumTargetTemperature()
{
    return 31.0f;
}

unsigned int TclDriver::getMaximumFanSpeed()
{
    return 5;
}

unsigned int TclDriver::getMaximumHorizontalFixPosition()
{
    return 5;
}

unsigned int TclDriver::getMaximumVerticalFixPosition()
{
    return 5;
}

unsigned int TclDriver::getMaximumHumidityModeLevels()
{
    return 0;
}

bool TclDriver::supportExternalRoomTemperatureSensor()
{
    return false;
}

float TclDriver::accuracyInDegrees()
{
    return 0.5f;
}

void TclDriver::setPower(bool power)
{
    _power = power;
    requestControlUpdate();
}

void TclDriver::setMode(AirConditionMode mode)
{
    _mode = mode;
    requestControlUpdate();
}

void TclDriver::setTargetTemperature(float temperaturCelsius)
{
    _targetTemperature = clampTargetTemperature(temperaturCelsius);
    requestControlUpdate();
}

void TclDriver::setFanSpeed(unsigned int speed)
{
    _fanSpeed = clampFanSpeed(speed);
    requestControlUpdate();
}

void TclDriver::setSwingHorizontal(bool swing)
{
    _horizontalSwing = swing;
    if (swing)
        _horizontalFixPosition = 0;
    requestControlUpdate();
}

void TclDriver::setSwingVertical(bool swing)
{
    _verticalSwing = swing;
    if (swing)
        _verticalFixPosition = 0;
    requestControlUpdate();
}

void TclDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    if (position > getMaximumHorizontalFixPosition())
    {
        logErrorP("Unsupported TCL horizontal fix position: %u", position);
        return;
    }
    _horizontalSwing = false;
    _horizontalFixPosition = (uint8_t)position;
    requestControlUpdate();
}

void TclDriver::setSwingVerticalFixPosition(unsigned int position)
{
    if (position > getMaximumVerticalFixPosition())
    {
        logErrorP("Unsupported TCL vertical fix position: %u", position);
        return;
    }
    _verticalSwing = false;
    _verticalFixPosition = (uint8_t)position;
    requestControlUpdate();
}

void TclDriver::setExternalSensorRoomTemperature(float temperaturCelsius)
{
    logDebugP("TCL does not support writing an external room temperature sensor value: %.1f", temperaturCelsius);
}

void TclDriver::setWifiLed(bool on)
{
    logDebugP("TCL has no WiFi LED control in this native driver: %d", on);
}

void TclDriver::setDeviceMode(AirConditionDeviceMode mode)
{
    switch (mode)
    {
        case AirConditionDeviceMode::AirConditionDeviceModeStandard:
            _eco = false;
            applyExtendedMode(0);
            break;
        case AirConditionDeviceMode::AirConditionDeviceModeHiPower:
            _turbo = true;
            break;
        case AirConditionDeviceMode::AirConditionDeviceModeEco:
            _eco = true;
            break;
        case AirConditionDeviceMode::AirConditionDeviceModeSilent1:
        case AirConditionDeviceMode::AirConditionDeviceModeSilent2:
            _lowNoise = true;
            break;
    }
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::processInputKo(GroupObject& ko)
{
    switch (ko.asap())
    {
        case AIR_KoTclSleepMode:
            setSleepMode((uint8_t)ko.value(DPT_Value_1_Ucount));
            break;
        case AIR_KoTclEcoMode:
            setEcoMode(ko.value(DPT_Switch));
            break;
        case AIR_KoTclDisplay:
            setDisplay(ko.value(DPT_Switch));
            break;
        case AIR_KoTclBuzzer:
            setBuzzer(ko.value(DPT_Switch));
            break;
        case AIR_KoTclExtendedMode:
            setExtendedMode((uint8_t)ko.value(DPT_Value_1_Ucount));
            break;
        case AIR_KoTclHealthMode:
            setHealthMode(ko.value(DPT_Switch));
            break;
        case AIR_KoTclTurboMode:
            setTurboMode(ko.value(DPT_Switch));
            break;
        case AIR_KoTclLowNoiseMode:
            setLowNoiseMode(ko.value(DPT_Switch));
            break;
        default:
            break;
    }
}

void TclDriver::setMaxPowerLevel(uint8_t percentage)
{
    logDebugP("TCL power limit is not implemented in the native protocol driver: %u%%", percentage);
}

void TclDriver::setAirPurification(bool on)
{
    logDebugP("TCL air purification is not implemented in the native protocol driver: %d", on);
}

void TclDriver::requestControlUpdate()
{
    _controlPending = true;
    _controlSendAt = millis() + ControlDebounceMs;
}

void TclDriver::processPendingControlUpdate()
{
    if (!_controlPending)
        return;

    const unsigned long now = millis();
    if ((long)(now - _controlSendAt) >= 0)
    {
        sendControl();
    }
}

void TclDriver::sendPoll()
{
#ifdef DEVELOPMENT
    logDebugP("TCL poll: %s", toHexString(PollCommand, PollCommandLength).c_str());
#endif
    OPENKNX_AIR_CONDITION_SERIAL.write(PollCommand, PollCommandLength);
    _lastPoll = millis();
}

void TclDriver::sendControl()
{
    _controlPending = false;
    uint8_t data[ControlCommandLength] = {0};

    data[0] = 0xBB;
    data[1] = 0x00;
    data[2] = 0x01;
    data[3] = 0x03;
    data[4] = 0x1D;
    data[5] = 0x00;
    data[6] = 0x00;

    if (_power)
        data[7] |= 0x04;
    if (_beep)
        data[7] |= 0x20;
    if (_display)
        data[7] |= 0x40;
    if (_eco)
        data[7] |= 0x80;

    data[8] |= encodeMode(_mode);
    if (_health)
        data[8] |= 0x10;
    if (_turbo)
        data[8] |= 0x40;
    if (_lowNoise)
        data[8] |= 0x80;

    const float targetTemperature = std::round(clampTargetTemperature(_targetTemperature) * 2.0f) / 2.0f;
    const int wholeDegrees = (int)std::floor(targetTemperature);
    data[9] = 0x50 | ((uint8_t)(31 - wholeDegrees) & 0x0F);
    if (targetTemperature - wholeDegrees >= 0.5f)
        data[11] |= 0x02;

    data[10] |= encodeFanSpeed(_fanSpeed);
    if (_verticalSwing)
        data[10] |= 0x38;
    if (_horizontalSwing)
        data[11] |= 0x08;

    data[19] |= ((_sleepMode & 0x03) << 6);

    if (_verticalSwing)
        data[32] = 0x08;
    else
        data[32] = _verticalFixPosition & 0x07;

    if (_horizontalSwing)
        data[33] = 0x88;
    else
        data[33] = 0x80 | (_horizontalFixPosition & 0x07);

    data[ControlCommandLength - 1] = checksum(data, ControlCommandLength);

#ifdef DEVELOPMENT
    logDebugP("TCL TX: %s", toHexString(data, ControlCommandLength).c_str());
#endif
    OPENKNX_AIR_CONDITION_SERIAL.write(data, ControlCommandLength);
    _lastCommand = millis();
    _lastPoll = _lastCommand;
}

void TclDriver::handleReceivedByte(uint8_t value)
{
    if (_rxBuffer.empty())
    {
        if (value != Header)
            return;
    }

    _rxBuffer.push_back(value);
    _lastReceivedByte = millis();

    if (_rxBuffer.size() == 5)
    {
        const size_t expectedLength = 5 + _rxBuffer[4] + 1;
        if (expectedLength > MaximumMessageLength)
        {
            logDebugP("TCL invalid length byte: %u", _rxBuffer[4]);
            _rxBuffer.clear();
        }
        return;
    }

    if (_rxBuffer.size() > 5)
    {
        const size_t expectedLength = 5 + _rxBuffer[4] + 1;
        if (_rxBuffer.size() == expectedLength)
        {
            const uint8_t expectedChecksum = checksum(_rxBuffer.data(), _rxBuffer.size());
            const uint8_t receivedChecksum = _rxBuffer.back();
            if (expectedChecksum == receivedChecksum)
            {
#ifdef DEVELOPMENT
                logDebugP("TCL RX: %s", toHexString(_rxBuffer).c_str());
#endif
                parseStatus(_rxBuffer);
            }
            else
            {
                logDebugP("TCL invalid checksum %02X != %02X", receivedChecksum, expectedChecksum);
            }
            _rxBuffer.clear();
        }
        else if (_rxBuffer.size() > expectedLength)
        {
            logDebugP("TCL RX overflow");
            _rxBuffer.clear();
        }
    }
}

void TclDriver::parseStatus(const std::vector<uint8_t>& rawData)
{
    if (rawData.size() < 61 || (rawData[3] != 0x03 && rawData[3] != 0x04))
    {
        logDebugP("TCL ignore unknown message length=%u type=%02X", (unsigned int)rawData.size(), rawData.size() > 3 ? rawData[3] : 0);
        return;
    }

    statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateOk);
    statusFeedback.updateOnlineStatus(true);

    const bool power = (rawData[7] & 0x10) != 0;
    const AirConditionMode mode = decodeMode(rawData[7] & 0x0F);
    const float targetTemperature = (float)((rawData[8] & 0x0F) + 16) + (((rawData[9] & 0x02) != 0) ? 0.5f : 0.0f);
    const uint8_t fanSpeed = decodeFanSpeed(rawData[8] & 0xF0);
    const uint8_t swingMode = rawData[10] & 0x60;
    const bool horizontalSwing = swingMode == 0x20 || swingMode == 0x60;
    const bool verticalSwing = swingMode == 0x40 || swingMode == 0x60;
    const bool display = (rawData[7] & 0x20) != 0;
    const bool eco = (rawData[7] & 0x40) != 0;
    const bool turbo = (rawData[7] & 0x80) != 0;
    const bool health = (rawData[9] & 0x04) != 0;
    const bool lowNoise = (rawData[33] & 0x80) != 0;
    const uint8_t sleepMode = (rawData[19] & 0xC0) != 0 ? ((rawData[19] >> 6) & 0x03) : _sleepMode;

    uint8_t verticalFixPosition = _verticalFixPosition;
    uint8_t horizontalFixPosition = _horizontalFixPosition;
    const bool hasVerticalFixPosition = decodeVerticalFixPosition(rawData[32], verticalFixPosition);
    const bool hasHorizontalFixPosition = decodeHorizontalFixPosition(rawData[33], horizontalFixPosition);

    const uint16_t rawRoomTemperature = ((uint16_t)rawData[17] << 8) | rawData[18];
    const float roomTemperature = ((rawRoomTemperature / 374.0f) - 32.0f) / 1.8f;

    _power = power;
    _mode = mode;
    _targetTemperature = targetTemperature;
    _fanSpeed = fanSpeed;
    _horizontalSwing = horizontalSwing;
    _verticalSwing = verticalSwing;
    _eco = eco;
    _display = display;
    _health = health;
    _turbo = turbo;
    _lowNoise = lowNoise;
    _sleepMode = sleepMode;
    if (hasVerticalFixPosition)
        _verticalFixPosition = verticalFixPosition;
    if (hasHorizontalFixPosition)
        _horizontalFixPosition = horizontalFixPosition;

    statusFeedback.powerChanged(power);
    statusFeedback.modeChanged(mode);
    statusFeedback.targetTemperatureChanged(targetTemperature, false);
    statusFeedback.fanSpeedChanged(fanSpeed);
    statusFeedback.swingHorizontalChanged(horizontalSwing);
    statusFeedback.swingVerticalChanged(verticalSwing);
    if (hasVerticalFixPosition)
        statusFeedback.swingVerticalFixPositionChanged(verticalFixPosition);
    if (hasHorizontalFixPosition)
        statusFeedback.swingHorizontalFixPositionChanged(horizontalFixPosition);
    statusFeedback.roomTemperatureChanged(roomTemperature);
    updateTclFeatureStatusObjects();

    _lastRawDump = toHexString(rawData);
    _lastDecodedDump = formatStatusDump(rawData,
                                        power,
                                        mode,
                                        targetTemperature,
                                        fanSpeed,
                                        horizontalSwing,
                                        verticalSwing,
                                        horizontalFixPosition,
                                        verticalFixPosition,
                                        roomTemperature);

    if (_onlineReadInProgress)
        _onlineReadDone = true;
}

const char* TclDriver::modeToText(AirConditionMode mode) const
{
    switch (mode)
    {
        case AirConditionMode::AirConditionModeAuto:
            return "Auto";
        case AirConditionMode::AirConditionModeCool:
            return "Kuehlen";
        case AirConditionMode::AirConditionModeHeat:
            return "Heizen";
        case AirConditionMode::AirConditionModeDry:
            return "Entfeuchten";
        case AirConditionMode::AirConditionModeFan:
            return "Lueften";
        default:
            return "Unbekannt";
    }
}

std::string TclDriver::formatStatusDump(const std::vector<uint8_t>& rawData,
                                        bool power,
                                        AirConditionMode mode,
                                        float targetTemperature,
                                        uint8_t fanSpeed,
                                        bool horizontalSwing,
                                        bool verticalSwing,
                                        uint8_t horizontalFixPosition,
                                        uint8_t verticalFixPosition,
                                        float roomTemperature) const
{
    char line[96];
    std::string result;

    snprintf(line, sizeof(line), "Laenge: %u\n", (unsigned int)rawData.size());
    result += line;
    snprintf(line, sizeof(line), "Telegrammtyp: 0x%02X\n", rawData.size() > 3 ? rawData[3] : 0);
    result += line;
    snprintf(line, sizeof(line), "Checksumme: 0x%02X\n", rawData.empty() ? 0 : rawData.back());
    result += line;
    snprintf(line, sizeof(line), "Power: %s\n", power ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "Betriebsmodus: %s\n", modeToText(mode));
    result += line;
    snprintf(line, sizeof(line), "Solltemperatur: %.1f C\n", targetTemperature);
    result += line;
    snprintf(line, sizeof(line), "Raumtemperatur: %.1f C\n", roomTemperature);
    result += line;
    snprintf(line, sizeof(line), "Luefter: %u (%s)\n", fanSpeed, fanSpeed == 0 ? "Auto" : "Stufe");
    result += line;
    snprintf(line, sizeof(line), "Lamelle vertikal: %s, Position %u\n", verticalSwing ? "Swing" : "Fix", verticalFixPosition);
    result += line;
    snprintf(line, sizeof(line), "Lamelle horizontal: %s, Position %u\n", horizontalSwing ? "Swing" : "Fix", horizontalFixPosition);
    result += line;
    snprintf(line, sizeof(line), "Sleep Mode: %s (%u)\n", sleepModeToText(_sleepMode), _sleepMode);
    result += line;
    snprintf(line, sizeof(line), "Eco Mode: %s\n", _eco ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "7-Seg Display: %s\n", _display ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "Buzzer: %s\n", _beep ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "Health Mode: %s\n", _health ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "Turbo Mode: %s\n", _turbo ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "Low Noise Mode: %s\n", _lowNoise ? "Ein" : "Aus");
    result += line;
    snprintf(line, sizeof(line), "Extended Mode Bitmaske: %s (%u)\n", extendedModeToText(currentExtendedMode()), currentExtendedMode());
    result += line;

    if (rawData.size() > 33)
    {
        snprintf(line, sizeof(line), "Raw Status/Mode: 0x%02X (Status=0x%X, Mode=0x%X)\n", rawData[7], (rawData[7] & 0xF0) >> 4, rawData[7] & 0x0F);
        result += line;
        snprintf(line, sizeof(line), "Raw Temp/Fan: 0x%02X\n", rawData[8]);
        result += line;
        snprintf(line, sizeof(line), "Raw Swing: 0x%02X\n", rawData[10]);
        result += line;
        snprintf(line, sizeof(line), "Raw Pos V/H: 0x%02X / 0x%02X\n", rawData[32], rawData[33]);
        result += line;
        snprintf(line, sizeof(line), "Raw LowNoise: 0x%02X (%s)\n", rawData[33] & 0x80, (rawData[33] & 0x80) != 0 ? "Ein" : "Aus");
        result += line;
    }

    return result;
}

const char* TclDriver::sleepModeToText(uint8_t mode) const
{
    switch (mode)
    {
        case 0:
            return "Aus";
        case 1:
            return "Standard";
        case 2:
            return "Alt";
        case 3:
            return "Jung";
        default:
            return "Unbekannt";
    }
}

const char* TclDriver::extendedModeToText(uint8_t mode) const
{
    switch (mode & 0x0D)
    {
        case 0:
            return "Normal";
        case 1:
            return "Health";
        case 4:
            return "Turbo";
        case 5:
            return "Health + Turbo";
        case 8:
            return "Low Noise";
        case 9:
            return "Health + Low Noise";
        case 12:
            return "Turbo + Low Noise";
        case 13:
            return "Health + Turbo + Low Noise";
        default:
            return "Unbekannt";
    }
}

bool TclDriver::isValidExtendedModeMask(uint8_t mode) const
{
    return (mode & ~0x0D) == 0;
}

uint8_t TclDriver::currentExtendedMode() const
{
    uint8_t mode = 0;
    if (_health)
        mode |= 0x01;
    if (_turbo)
        mode |= 0x04;
    if (_lowNoise)
        mode |= 0x08;
    return mode;
}

void TclDriver::applyExtendedMode(uint8_t mode)
{
    _health = (mode & 0x01) != 0;
    _turbo = (mode & 0x04) != 0;
    _lowNoise = (mode & 0x08) != 0;
}

void TclDriver::updateTclFeatureStatusObjects()
{
    KoAIR_TclSleepModeState.valueCompare(_sleepMode, DPT_Value_1_Ucount);
    KoAIR_TclEcoModeState.valueCompare(_eco, DPT_Switch);
    KoAIR_TclDisplayState.valueCompare(_display, DPT_Switch);
    KoAIR_TclBuzzerState.valueCompare(_beep, DPT_Switch);
    KoAIR_TclExtendedModeState.valueCompare(currentExtendedMode(), DPT_Value_1_Ucount);
    KoAIR_TclHealthModeState.valueCompare(_health, DPT_Switch);
    KoAIR_TclTurboModeState.valueCompare(_turbo, DPT_Switch);
    KoAIR_TclLowNoiseModeState.valueCompare(_lowNoise, DPT_Switch);

    // Mirror TCL Eco/Turbo/Low-Noise into the existing generic device-mode status objects when these KOs exist.
    KoAIR_DeviceModeEcoState.valueCompare(_eco, DPT_Switch);
    KoAIR_DeviceModeHiPowerState.valueCompare(_turbo, DPT_Switch);
    KoAIR_DeviceModeSilent1State.valueCompare(_lowNoise, DPT_Switch);
}

void TclDriver::setSleepMode(uint8_t mode)
{
    if (mode > 3)
    {
        logErrorP("Unsupported TCL sleep mode: %u", mode);
        return;
    }
    _sleepMode = mode;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setEcoMode(bool enabled)
{
    _eco = enabled;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setDisplay(bool enabled)
{
    _display = enabled;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setBuzzer(bool enabled)
{
    _beep = enabled;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setExtendedMode(uint8_t mode)
{
    if (!isValidExtendedModeMask(mode))
    {
        logErrorP("Unsupported TCL extended mode bitmask: %u", mode);
        return;
    }
    applyExtendedMode(mode);
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setHealthMode(bool enabled)
{
    _health = enabled;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setTurboMode(bool enabled)
{
    _turbo = enabled;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

void TclDriver::setLowNoiseMode(bool enabled)
{
    _lowNoise = enabled;
    updateTclFeatureStatusObjects();
    requestControlUpdate();
}

uint8_t TclDriver::checksum(const uint8_t* data, size_t length) const
{
    uint8_t result = 0;
    if (length == 0)
        return result;
    for (size_t i = 0; i < length - 1; i++)
    {
        result ^= data[i];
    }
    return result;
}

uint8_t TclDriver::encodeMode(AirConditionMode mode) const
{
    switch (mode)
    {
        case AirConditionMode::AirConditionModeHeat:
            return 0x01;
        case AirConditionMode::AirConditionModeDry:
            return 0x02;
        case AirConditionMode::AirConditionModeCool:
            return 0x03;
        case AirConditionMode::AirConditionModeFan:
            return 0x07;
        case AirConditionMode::AirConditionModeAuto:
        default:
            return 0x08;
    }
}

uint8_t TclDriver::encodeFanSpeed(uint8_t speed) const
{
    switch (speed)
    {
        case 0:
            return 0x00;
        case 1:
            return 0x02;
        case 2:
            return 0x06;
        case 3:
            return 0x03;
        case 4:
            return 0x07;
        case 5:
            return 0x05;
        default:
            return 0x00;
    }
}

AirConditionMode TclDriver::decodeMode(uint8_t rawMode) const
{
    switch (rawMode & 0x0F)
    {
        case 0x05:
            return AirConditionMode::AirConditionModeAuto;
        case 0x01:
            return AirConditionMode::AirConditionModeCool;
        case 0x03:
            return AirConditionMode::AirConditionModeDry;
        case 0x02:
            return AirConditionMode::AirConditionModeFan;
        case 0x04:
            return AirConditionMode::AirConditionModeHeat;
        default:
            return _mode;
    }
}


bool TclDriver::decodeVerticalFixPosition(uint8_t rawPosition, uint8_t& position) const
{
    if (rawPosition >= 0x01 && rawPosition <= 0x05)
    {
        position = rawPosition;
        return true;
    }
    return false;
}

bool TclDriver::decodeHorizontalFixPosition(uint8_t rawPosition, uint8_t& position) const
{
    if (rawPosition >= 0x81 && rawPosition <= 0x85)
    {
        position = rawPosition & 0x07;
        return true;
    }
    return false;
}

uint8_t TclDriver::decodeFanSpeed(uint8_t rawFan) const
{
    switch (rawFan)
    {
        case 0x80:
            return 0;
        case 0x90:
            return 1;
        case 0xC0:
            return 2;
        case 0xA0:
            return 3;
        case 0xD0:
            return 4;
        case 0xB0:
            return 5;
        default:
            return _fanSpeed;
    }
}

uint8_t TclDriver::clampFanSpeed(unsigned int speed) const
{
    if (speed > 5)
        return 5;
    return (uint8_t)speed;
}

float TclDriver::clampTargetTemperature(float temperature) const
{
    return std::max(16.0f, std::min(31.0f, temperature));
}
