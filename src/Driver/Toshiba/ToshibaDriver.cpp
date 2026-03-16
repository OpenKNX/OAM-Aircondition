// This code is based on:
// https://github.com/pedobry/esphome_toshiba_suzumi

#include "ToshibaDriver.h"
#include "NetworkModule.h"
#include <Arduino.h>
#if defined(ARDUINO_ARCH_RP2040)
    #include <hardware/gpio.h> // gpio_set_function, gpio_pull_up, gpio_set_input_hysteresis_enabled
#endif

static const int RECEIVE_TIMEOUT = 200;
static const int COMMAND_DELAY = 100;
static const int COMMAND_RETRY_AFTER = 1000;
static const int COMMAND_REQUEST_FEEDBACK_AFTER = 500;
static const int COMMAND_MAX_RETRY_COUNT = 3;

const uint8_t ToshibaDriver::EightDegreeSpecialModeTempOffset = 16;
const uint8_t ToshibaDriver::EightDegreeSpecialModeMinTemp = 5;
const uint8_t ToshibaDriver::EightDegreeSpecialModeMaxTemp = 16;
const uint8_t ToshibaDriver::StandardModeMinTemp = 17;
const uint8_t ToshibaDriver::StandardModeMaxTemp = 30;

const std::vector<uint8_t> ToshibaDriver::PayloadHandshake[6] = {
    {2, 255, 255, 0, 0, 0, 0, 2},
    {2, 255, 255, 1, 0, 0, 1, 2, 254},
    {2, 0, 0, 0, 0, 0, 2, 2, 2, 250},
    {2, 0, 1, 129, 1, 0, 2, 0, 0, 123},
    {2, 0, 1, 2, 0, 0, 2, 0, 0, 254},
    {2, 0, 2, 0, 0, 0, 0, 254},
};

const std::vector<uint8_t> ToshibaDriver::PayloadPostHandshake[2] = {
    {2, 0, 2, 1, 0, 0, 2, 0, 0, 251},
    {2, 0, 2, 2, 0, 0, 2, 0, 0, 250},
};

ToshibaDriver::ToshibaDriver(AirConditionDriverStatusFeedback &statusFeedback)
    : AirConditionDriver(statusFeedback)
{
}

const std::string ToshibaDriver::name() const
{
    return "Toshiba";
}

void ToshibaDriver::showInformations()
{
}

void ToshibaDriver::startCommunication(bool restart)
{
    logDebugP("Send communcation");
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadHandshake[0]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadHandshake[1]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadHandshake[2]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadHandshake[3]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadHandshake[4]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadHandshake[5]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeDelay, .delay = 2000});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadPostHandshake[0]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeHandshake, .payload = PayloadPostHandshake[1]});
    enqueueCommand(ToshibaCommand{.cmd = ToshibaCommandType::ToshibaCommandTypeDelay, .delay = 1000});

    requestData(ToshibaCommandType::ToshibaCommandTypePowerState);
}

ToshibaCommand ToshibaDriver::createRequestCommand(ToshibaCommandType cmd)
{
    ToshibaCommand command;
    command.cmd = cmd;
    command.payload = {2, 0, 3, 16, 0, 0, 6, 1, 48, 1, 0, 1};
    command.payload.push_back(static_cast<uint8_t>(cmd));
    command.payload.push_back(checksum(command.payload, command.payload.size()));
    command.awaitAnswer = true;
    logInfoP("Requesting for %02X, checksum: %02X", (int)command.payload[12], (int)command.payload[13]);
    return command;
}

void ToshibaDriver::requestData(ToshibaCommandType cmd)
{
    ToshibaCommand command = createRequestCommand(cmd);
    enqueueCommand(command);
}

void ToshibaDriver::requestAllData()
{
    requestData(ToshibaCommandType::ToshibaCommandTypePowerState);
    requestData(ToshibaCommandType::ToshibaCommandTypeMode);
    requestData(ToshibaCommandType::ToshibaCommandTypeTargetTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeFan);
    requestData(ToshibaCommandType::ToshibaCommandTypePowerSel);
    requestData(ToshibaCommandType::ToshibaCommandTypeSwing);
    requestData(ToshibaCommandType::ToshibaCommandTypeRoomTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeOutdoorTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeSpecialMode);
    requestData(ToshibaCommandType::ToshibaCommandTypePure);
    _lastTemperatureRequest = millis();
}

void ToshibaDriver::setup()
{
#if ARDUINO_ARCH_ESP32
    OPENKNX_AIR_CONDITION_SERIAL.begin(9600UL, SERIAL_8E1, OPENKNX_AIR_CONDITION_SERIAL_RX, OPENKNX_AIR_CONDITION_SERIAL_TX);
#else
    OPENKNX_AIR_CONDITION_SERIAL.setRX(OPENKNX_AIR_CONDITION_SERIAL_RX);
    OPENKNX_AIR_CONDITION_SERIAL.setTX(OPENKNX_AIR_CONDITION_SERIAL_TX);
    OPENKNX_AIR_CONDITION_SERIAL.begin(9600UL, SERIAL_8E1);
#endif
}

/**
 * Checksum is calculated from all bytes excluding start byte.
 * It's (256 - (sum % 256)).
 */
uint8_t ToshibaDriver::checksum(std::vector<uint8_t> data, uint8_t length)
{
    uint8_t sum = 0;
    for (size_t i = 1; i < length; i++)
    {
        sum += data[i];
    }
    return 256 - sum;
}

// Handle data in RX buffer, validate message for content and checksum.
// Since we know the format only of some messages (expected length), unknown messages
// are ended via RECIEVE timeout.
bool ToshibaDriver::validateMessage()
{
    uint8_t at = _receivedMessage.size() - 1;
    auto *data = &_receivedMessage[0];
    uint8_t new_byte = data[at];

    // Byte 0: HEADER (always 0x02)
    if (at == 0)
        return new_byte == 0x02;

    // always get first three bytes
    if (at < 2)
    {
        return true;
    }

    // Byte 3
    if (data[2] != 0x03)
    {
        // Normal commands starts with 0x02 0x00 0x03 and have length between 15-17 bytes.
        // however there are some special unknown handshake commands which has non-standard replies.
        // Since we don't know their format, we can't validate them.
        return true;
    }

    if (at <= 5)
    {
        // no validation for these fields
        return true;
    }

    // Byte 7: LENGTH
    uint8_t length = 6 + data[6] + 1; // prefix + data + checksum

    // wait until all data is read
    if (at < length)
        return true;

    // last byte: CHECKSUM
    uint8_t rx_checksum = new_byte;
    uint8_t calc_checksum = checksum(_receivedMessage, at);

    if (rx_checksum != calc_checksum)
    {
        logDebugP("Received invalid message checksum %02X!=%02X DATA=[%s]", rx_checksum, calc_checksum,
                  toHexString(data, length).c_str());
        return false;
    }

    // valid message
#ifdef DEVELOPMENT
    logDebugP("Received:");
    auto hexString = toHexString(data, length);
    if (hexString.length() > OPENKNX_MAX_LOG_MESSAGE_LENGTH - 20)
    {
        hexString = hexString.substr(0, OPENKNX_MAX_LOG_MESSAGE_LENGTH - 20) + "...";
    }
    logDebugP(hexString.c_str());
#endif
    parseResponse(_receivedMessage);

    // return false to reset rx buffer
    return false;
}

void ToshibaDriver::parseResponse(std::vector<uint8_t> rawData)
{
    uint8_t length = rawData.size();
    ToshibaCommandType commandType;
    uint8_t value;

    switch (length)
    {
        case 15: // response to requestData with the actual value of sensor/setting
            commandType = static_cast<ToshibaCommandType>(rawData[12]);
            value = rawData[13];
            break;
        case 16: // probably ACK for issued command
            logDebugP("Received message with length: %d and value %s", length, toHexString(rawData).c_str());
            _receivedMessage.clear();
            return;
        case 17: // response to requestData with the actual value of sensor/setting
            commandType = static_cast<ToshibaCommandType>(rawData[14]);
            value = rawData[15];
            break;
        default:
            logDebugP("Received unknown message with length: %d", length);
            return;
    }
    bool isFeedback = false;
    if (!_commandQueue.empty())
    {
        auto &currentCommand = _commandQueue.front();
        if (currentCommand.timestampSent != 0 && currentCommand.cmd == commandType)
        {
            // if we have a command in queue, and it matches the received command, erase it
            _commandQueue.erase(_commandQueue.begin());
            logDebugP("Response for command %d received", static_cast<int>(commandType));
            statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateOk);
            statusFeedback.updateOnlineStatus(true);
            if (currentCommand.requestFeedback)
            {
                logInfoP("Request feedback not yet sent, ignore value");
                _receivedMessage.clear();
                return;
            }
            isFeedback = true;
        }
        else
        {
            logDebugP("Received command %d does not match the await command %d", static_cast<int>(commandType), static_cast<int>(currentCommand.cmd));
        }
    }

    switch (commandType)
    {
        case ToshibaCommandType::ToshibaCommandTypeTargetTemperature:
            logInfoP("Received target temp: %d", value);
            if (_specialMode == ToshibaSpecialModeEightDegree)
            {
                // if special mode is EIGHT_DEG, shift the target temperature by SPECIAL_TEMP_OFFSET
                value -= EightDegreeSpecialModeTempOffset;
                logDebugP("Note: Special Mode \"%s\" is active, shifting target temp to %d", EightDegreeSpecialModeTempOffset, (int)value);
            }
            statusFeedback.targetTemperatureChanged(value, isFeedback);
            break;
        case ToshibaCommandType::ToshibaCommandTypeFan:
        {
            switch (value)
            {
                case ToshibaFan::ToshibaFanAuto:
                    logDebugP("Received fan speed: Auto");
                    statusFeedback.fanSpeedChanged(0);
                    break;
                case ToshibaFan::ToshibaFanQuit:
                    logDebugP("Received fan speed: Quit");
                    statusFeedback.fanSpeedChanged(1);
                    break;
                case ToshibaFan::ToshibaFanLow:
                    logDebugP("Received fan speed: Low");
                    statusFeedback.fanSpeedChanged(2);
                    break;
                case ToshibaFan::ToshibaFanMode2:
                    logDebugP("Received fan speed: Mode 2");
                    statusFeedback.fanSpeedChanged(3);
                    break;
                case ToshibaFan::ToshibaFanMedium:
                    logDebugP("Received fan speed: Medium");
                    statusFeedback.fanSpeedChanged(4);
                    break;
                case ToshibaFan::ToshibaFanMode4:
                    logDebugP("Received fan speed: Mode 4");
                    statusFeedback.fanSpeedChanged(5);
                    break;
                case ToshibaFan::ToshibaFanHigh:
                    logDebugP("Received fan speed: High");
                    statusFeedback.fanSpeedChanged(6);
                    break;
                default:
                    logDebugP("Unknown fan speed: %d", value);
            }
            break;
        }
        case ToshibaCommandType::ToshibaCommandTypeSwing:
        {
            _swingMode = static_cast<ToshibaSwingMode>(value);
            switch (_swingMode)
            {
                case ToshibaSwingMode::ToshibaSwingModeOff:
                    logDebugP("Received swing mode: Off");
                    _lastNoneMovingSwingMode = _swingMode;
                    statusFeedback.swingVerticalFixPositionChanged(0);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(false);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeBoth:
                    logDebugP("Received swing mode: Both");
                    statusFeedback.swingVerticalFixPositionChanged(0);
                    statusFeedback.swingHorizontalChanged(true);
                    statusFeedback.swingVerticalChanged(true);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeVertical:
                    logDebugP("Received swing mode: Vertical");
                    statusFeedback.swingVerticalFixPositionChanged(0);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(true);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeHorizontal:
                    logDebugP("Received swing mode: Horizontal");
                    statusFeedback.swingVerticalFixPositionChanged(0);
                    statusFeedback.swingHorizontalChanged(true);
                    statusFeedback.swingVerticalChanged(false);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeFixPosition1:
                    logDebugP("Received swing mode: Fix Position 1");
                    _lastNoneMovingSwingMode = _swingMode;
                    statusFeedback.swingVerticalFixPositionChanged(1);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(false);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeFixPosition2:
                    logDebugP("Received swing mode: Fix Position 2");
                    _lastNoneMovingSwingMode = _swingMode;
                    statusFeedback.swingVerticalFixPositionChanged(2);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(false);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeFixPosition3:
                    logDebugP("Received swing mode: Fix Position 3");
                    _lastNoneMovingSwingMode = _swingMode;
                    statusFeedback.swingVerticalFixPositionChanged(3);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(false);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeFixPosition4:
                    logDebugP("Received swing mode: Fix Position 4");
                    _lastNoneMovingSwingMode = _swingMode;
                    statusFeedback.swingVerticalFixPositionChanged(4);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(false);
                    break;
                case ToshibaSwingMode::ToshibaSwingModeFixPosition5:
                    logDebugP("Received swing mode: Fix Position 5");
                    _lastNoneMovingSwingMode = _swingMode;
                    statusFeedback.swingVerticalFixPositionChanged(5);
                    statusFeedback.swingHorizontalChanged(false);
                    statusFeedback.swingVerticalChanged(false);
                    break;

                default:
                    logErrorP("Unknown swing mode: %d", (int)_swingMode);
            }
            break;
        }
        case ToshibaCommandType::ToshibaCommandTypeMode:
        {
            auto mode = (ToshibaDriverMode)value;
            logDebugP("Received AC mode: %d", (int)mode);
            switch (mode)
            {
                case ToshibaDriverMode::ToshibaDriverModeAuto:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeAuto);
                    break;
                case ToshibaDriverMode::ToshibaDriverModeCool:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeCool);
                    break;
                case ToshibaDriverMode::ToshibaDriverModeHeat:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeHeat);
                    break;
                case ToshibaDriverMode::ToshibaDriverModeDry:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeDry);
                    break;
                case ToshibaDriverMode::ToshibaDriverModeFanOnly:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeFan);
                    break;
                default:
                    logDebugP("Unknown mode received: %d", (int)mode);
            }
            break;
        }
        case ToshibaCommandType::ToshibaCommandTypeRoomTemperature:
            logDebugP("Received room temp: %d °C", (int)value);
            statusFeedback.roomTemperatureChanged(value);
            break;
        case ToshibaCommandType::ToshibaCommandTypeOutdoorTemperature:
            logDebugP("Received outdoor temp: %d °C", (int)value);
            statusFeedback.outsideTemperaturChanged(value);
            break;
        case ToshibaCommandType::ToshibaCommandTypePowerSel:
        {
            switch ((ToshibaPowerLevel)value)
            {
                case ToshibaPowerLevel::ToshibaPowerLevel50:
                    logDebugP("Received power select: 50%");
                    statusFeedback.maxPowerLevelChanged(50);
                    break;
                case ToshibaPowerLevel::ToshibaPowerLevel75:
                    logDebugP("Received power select: 75%");
                    statusFeedback.maxPowerLevelChanged(75);
                    break;
                case ToshibaPowerLevel::ToshibaPowerLevel100:
                    logDebugP("Received power select: 100%");
                    statusFeedback.maxPowerLevelChanged(100);
                    break;
                default:
                    logDebugP("Unknown power select: %d", (int)value);
                    break;
            }
            break;
        }
        case ToshibaCommandType::ToshibaCommandTypePowerState:
        {
            auto climateState = static_cast<ToshibaState>(value);
            statusFeedback.powerChanged(climateState == ToshibaState::ToshibaStateOn);
        }
        break;
        case ToshibaCommandType::ToshibaCommandTypeSpecialMode:
        {
            logInfoP("Received special mode: %d", value);
            _specialMode = static_cast<ToshibaSpecialMode>(value);
            switch (_specialMode)
            {
                case ToshibaSpecialMode::ToshibaSpecialModeStandard:
                    logDebugP("Special mode: Standard");
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeHiPower:
                    logDebugP("Special mode: HiPower");
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeHiPower);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeEco:
                    logDebugP("Special mode: Eco");
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeEco);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeFireplace1:
                    logDebugP("Special mode: Fireplace1");
                    // statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeFireplace1);
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeFireplace2:
                    logDebugP("Special mode: Fireplace2");
                    // statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeFireplace2);
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeEightDegree:
                    logDebugP("Special mode: Eight Degree");
                    // statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeEightDegree);
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeSilent1:
                    logDebugP("Special mode: Silent1");
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeSilent1);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeSilent2:
                    logDebugP("Special mode: Silent2");
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeSilent2);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeSleep:
                    logDebugP("Special mode: Sleep");
                    // statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeSleep);
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeFloor:
                    logDebugP("Special mode: Floor");
                    // statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeFloor);
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                case ToshibaSpecialMode::ToshibaSpecialModeComfort:
                    logDebugP("Special mode: Comfort");
                    // statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeComfort);
                    statusFeedback.deviceModeChanged(AirConditionDeviceMode::AirConditionDeviceModeStandard);
                    break;
                default:
                    logErrorP("Unknown special mode: %d", (int)_specialMode);
            }
        }
        break;
        case ToshibaCommandType::ToshibaCommandTypePure:
            logInfoP("Received pure state: %d", value);
            statusFeedback.airPurificationChanged(value == (uint8_t)ToshibaPureState::ToshibaPureStateOn);
            break;
        default:
            logInfoP("Unknown command type %d with value %d", (int)commandType, (int)value);
            break;
    }
    _receivedMessage.clear(); // message processed, clear buffer
}

/**
 * Periodically request room and outdoor temperature.
 * It servers two purposes - updates data and is like "watchdog" because
 * some people reported that without communication, the unit might stop responding.
 */
void ToshibaDriver::requestTemperatures()
{
    requestData(ToshibaCommandType::ToshibaCommandTypeRoomTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeOutdoorTemperature);
}

void ToshibaDriver::handleReceivedByte(uint8_t c)
{
    _receivedMessage.push_back(c);
    if (!validateMessage())
    {
        _receivedMessage.clear();
    }
    else
    {
        _lastReceivedByteTimestamp = millis();
    }
}

/*
  byte packet[12 + dataLen + 1];    // base + dataLen + checksum
        memset(packet, 0, sizeof(packet));  // set all index to 0
        memcpy(packet, header, headerLen);  // add header
        packet[3] = packetType; // add packet type
        packet[6] = sizeof(packet) - 8; // add packet size
        // add unknown byte
        packet[7] = 1;
        packet[8] = 48;
        packet[9] = 1;
        packet[11] = dataLen;  // add data type
        memcpy(packet + 12, data, dataLen);   // add data
        packet[sizeof(packet) - 1] = checksum(438, data, dataLen);  // add checksum

        #ifdef HVAC_DEBUG
        DEBUG_PORT.println(F("HVAC> Command/Query packet was created"));
*/

void ToshibaDriver::sendCommand(ToshibaCommandType cmd, uint8_t value)
{
    std::vector<uint8_t> payload = {2, 0, 3, 16, 0, 0, 7, 1, 48, 1, 0, 2};
    payload.push_back(static_cast<uint8_t>(cmd));
    payload.push_back(value);
    payload.push_back(checksum(payload, payload.size()));
    logDebugP("Sending command: %02X, value: %d, checksum: %02X", (int)cmd, value, (int)payload[14]);
    enqueueCommand({.cmd = cmd, .payload = std::vector<uint8_t>{payload}, .awaitAnswer = true, .requestFeedback = true});
}

void ToshibaDriver::enqueueCommand(const ToshibaCommand &command)
{
    _commandQueue.push_back(command);
    processCommandQueue();
}

/**
 * Detect RX timeout and send next command in the queue to the unit.
 */
void ToshibaDriver::processCommandQueue()
{
    unsigned long now = max(1UL, millis()); // 0 is not a valid timestamp, it's used as a marker for uninitialized timestamp
    unsigned long cmdDelay = now - _lastCommandTimestamp;

    // when we have not processed message and timeout since last received byte has expired,
    // we likely won't receive any more data and there is nothing we can do with the message as it's
    // format is was not recognized by validate_message_ function.
    // Nothing to do - drop the message to free up communication and allow to send next command.
    if (now - _lastReceivedByteTimestamp > RECEIVE_TIMEOUT)
    {
        _receivedMessage.clear();
    }

    // when there is no RX message and there is a command to send
    if (cmdDelay > COMMAND_DELAY && !_commandQueue.empty() && this->_receivedMessage.empty())
    {
        auto &newCommand = _commandQueue.front();
        if (newCommand.cmd == ToshibaCommandType::ToshibaCommandTypeDelay)
        {
            if (cmdDelay < newCommand.delay)
            {
                // delay command did not finished yet
                return;
            }
            logDebugP("Delay command finished waiting {%d}ms, removing from queue", (int)newCommand.delay);
            _commandQueue.erase(_commandQueue.begin());
            return;
        }
        if (newCommand.timestampSent != 0)
        {
            if (newCommand.requestFeedback)
            {
                if (now - newCommand.timestampSent >= COMMAND_REQUEST_FEEDBACK_AFTER)
                {
                    newCommand.requestFeedback = false;
                    auto requestCommand = createRequestCommand(newCommand.cmd);
                    sendToUart(requestCommand);
                }
                return;
            }
            if (now - newCommand.timestampSent > COMMAND_RETRY_AFTER)
            {
                if (newCommand.retryCount >= COMMAND_MAX_RETRY_COUNT)
                {
                    // Command timeout
                    statusFeedback.driverStateChanged(AirConditionDriverState::AirConditionDriverStateError, "Command timeout");
                    statusFeedback.updateOnlineStatus(false);
                    _commandQueue.erase(_commandQueue.begin());
                    return;
                }
                newCommand.retryCount++;
                logErrorP("Command %02X timed out, retrying (%d/%d)", (int)newCommand.cmd, newCommand.retryCount, COMMAND_MAX_RETRY_COUNT);
            }
            else
            {
                // still waiting for answer
                return;
            }
        }
        _lastCommandTimestamp = now;
        newCommand.timestampSent = now;
        sendToUart(newCommand);
        if (!newCommand.awaitAnswer)
            _commandQueue.erase(_commandQueue.begin());
    }
}

void ToshibaDriver::loop()
{
    while (OPENKNX_AIR_CONDITION_SERIAL.available())
    {
        uint8_t c = (uint8_t)OPENKNX_AIR_CONDITION_SERIAL.read(); // Read one byte from the serial port
        handleReceivedByte(c);
    }
    processCommandQueue();

    if (statusFeedback.getDriverState() == AirConditionDriverState::AirConditionDriverStateOk)
    {
        if (millis() - _lastTemperatureRequest > 60000)
        {
            _lastTemperatureRequest = millis();
            requestTemperatures();
        }
    }
}

/**
 * Send the command to UART interface.
 */
void ToshibaDriver::sendToUart(const ToshibaCommand &command)
{
    logDebugP("Sending: [%s]", toHexString(command.payload).c_str());
    OPENKNX_AIR_CONDITION_SERIAL.write(command.payload.data(), command.payload.size());
}

float ToshibaDriver::getMinimumTargetTemperature()
{
    if (_specialMode == ToshibaSpecialMode::ToshibaSpecialModeEightDegree && _power && _mode == ToshibaDriverMode::ToshibaDriverModeHeat)
    {
        return EightDegreeSpecialModeMinTemp;
    }
    return StandardModeMinTemp;
}
float ToshibaDriver::getMaximumTargetTemperature()
{
    if (_specialMode == ToshibaSpecialMode::ToshibaSpecialModeEightDegree && _power && _mode == ToshibaDriverMode::ToshibaDriverModeHeat)
    {
        return EightDegreeSpecialModeMaxTemp;
    }
    return StandardModeMaxTemp;
}

unsigned int ToshibaDriver::getMaximumFanSpeed()
{
    return 6;
}
unsigned int ToshibaDriver::getMaximumHorizontalFixPosition()
{
    return 5;
}
unsigned int ToshibaDriver::getMaximumVerticalFixPosition()
{
    return 5;
}
unsigned int ToshibaDriver::getMaximumHumidityModeLevels()
{
    return 0;
}

void ToshibaDriver::setPower(bool power)
{
    _power = power;
    sendCommand(ToshibaCommandType::ToshibaCommandTypePowerState, static_cast<uint8_t>(power ? ToshibaState::ToshibaStateOn : ToshibaState::ToshibaStateOff));
}

void ToshibaDriver::setMode(AirConditionMode mode)
{
    switch (mode)
    {
        case AirConditionMode::AirConditionModeAuto:
            _mode = ToshibaDriverMode::ToshibaDriverModeAuto;
            sendCommand(ToshibaCommandType::ToshibaCommandTypeMode, static_cast<uint8_t>(ToshibaDriverMode::ToshibaDriverModeAuto));
            break;
        case AirConditionMode::AirConditionModeCool:
            _mode = ToshibaDriverMode::ToshibaDriverModeCool;
            sendCommand(ToshibaCommandType::ToshibaCommandTypeMode, static_cast<uint8_t>(ToshibaDriverMode::ToshibaDriverModeCool));
            break;
        case AirConditionMode::AirConditionModeHeat:
            _mode = ToshibaDriverMode::ToshibaDriverModeHeat;
            sendCommand(ToshibaCommandType::ToshibaCommandTypeMode, static_cast<uint8_t>(ToshibaDriverMode::ToshibaDriverModeHeat));
            break;
        case AirConditionMode::AirConditionModeDry:
            _mode = ToshibaDriverMode::ToshibaDriverModeDry;
            sendCommand(ToshibaCommandType::ToshibaCommandTypeMode, static_cast<uint8_t>(ToshibaDriverMode::ToshibaDriverModeDry));
            break;
        case AirConditionMode::AirConditionModeFan:
            _mode = ToshibaDriverMode::ToshibaDriverModeFanOnly;
            sendCommand(ToshibaCommandType::ToshibaCommandTypeMode, static_cast<uint8_t>(ToshibaDriverMode::ToshibaDriverModeFanOnly));
            break;
        default:
            logErrorP("Unsupported mode: %d", (int)mode);
            return;
    }
}

void ToshibaDriver::setTargetTemperature(float temperaturCelsius)
{
    uint8_t newTargetTemp = (uint8_t)round(temperaturCelsius);
    bool special_mode_changed = false;
    if (newTargetTemp >= StandardModeMinTemp && _specialMode == ToshibaSpecialMode::ToshibaSpecialModeEightDegree)
    {
        _specialMode = ToshibaSpecialMode::ToshibaSpecialModeStandard;
        special_mode_changed = true;
        logInfoP("Changing to Standard Mode");
    }
    else if (newTargetTemp < StandardModeMinTemp && _specialMode != ToshibaSpecialMode::ToshibaSpecialModeEightDegree)
    {
        _specialMode = ToshibaSpecialMode::ToshibaSpecialModeEightDegree;
        special_mode_changed = true;
        logInfoP("Changing to FrostGuard Mode");
    }
    if (special_mode_changed)
    {
        sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, static_cast<uint8_t>(_specialMode));
    }

    if (_specialMode == ToshibaSpecialMode::ToshibaSpecialModeEightDegree)
    {
        newTargetTemp += EightDegreeSpecialModeTempOffset;
        logDebugP("Note: Special Mode '8 degree' active, shifting setpoint temp to %d",
                  newTargetTemp);
    }
    // send command to set the target temperature to the unit
    // (which will be shifted by SPECIAL_TEMP_OFFSET if special mode is active)
    sendCommand(ToshibaCommandType::ToshibaCommandTypeTargetTemperature, newTargetTemp);
}

void ToshibaDriver::setFanSpeed(unsigned int speed)
{
    switch (speed)
    {
        case 0: // auto
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanAuto));
            break;
        case 1: // quit
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanQuit));
            break;
        case 2: // low
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanLow));
            break;
        case 3: // mode 2
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanMode2));
            break;
        case 4: // medium
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanMedium));
            break;
        case 5: // mode 4
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanMode4));
            break;
        case 6: // high
            sendCommand(ToshibaCommandType::ToshibaCommandTypeFan, static_cast<uint8_t>(ToshibaFan::ToshibaFanHigh));
            break;
        default:
            break;
    }
}

void ToshibaDriver::setSwingHorizontal(bool swing)
{
    ToshibaSwingMode newSwingMode = ToshibaSwingMode::ToshibaSwingModeOff;
    logDebugP("Setting horizontal swing %s", swing ? "on" : "off");
    logDebugP("Current swing mode: %d", (int)_swingMode);
    if (swing)
    {
        switch (_swingMode)
        {
            case ToshibaSwingMode::ToshibaSwingModeOff:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeHorizontal;
                break;
                ;
            case ToshibaSwingMode::ToshibaSwingModeBoth:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeBoth;
                break;
            case ToshibaSwingMode::ToshibaSwingModeVertical:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeBoth;
                return;
            case ToshibaSwingMode::ToshibaSwingModeHorizontal:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeHorizontal;
                break;
            default: // Fix vertical position modes
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeHorizontal;
                break;
        }
    }
    else
    {
        switch (_swingMode)
        {
            case ToshibaSwingMode::ToshibaSwingModeOff:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeOff;
                break;
            case ToshibaSwingMode::ToshibaSwingModeBoth:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeVertical;
                break;
            case ToshibaSwingMode::ToshibaSwingModeVertical:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeVertical;
                break;
            case ToshibaSwingMode::ToshibaSwingModeHorizontal:
                newSwingMode = _lastNoneMovingSwingMode;
                break;
            default: // Fix vertical position modes
                newSwingMode = _lastNoneMovingSwingMode;
                break;
        }
    }
    logDebugP("Setting horizontal swing to %d", (int)newSwingMode);
    _swingMode = newSwingMode;
    sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)newSwingMode);
}

void ToshibaDriver::setSwingVertical(bool swing)
{
    ToshibaSwingMode newSwingMode = ToshibaSwingMode::ToshibaSwingModeOff;
    logDebugP("Setting vertical swing %s", swing ? "on" : "off");
    logDebugP("Current swing mode: %d", (int)_swingMode);
    if (swing)
    {
        switch (_swingMode)
        {
            case ToshibaSwingMode::ToshibaSwingModeOff:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeVertical;
                break;
            case ToshibaSwingMode::ToshibaSwingModeBoth:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeBoth;
                break;
            case ToshibaSwingMode::ToshibaSwingModeVertical:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeVertical;
                break;
            case ToshibaSwingMode::ToshibaSwingModeHorizontal:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeBoth;
                break;
            default: //  Fix vertical position modes
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeVertical;
                break;
        }
    }
    else
    {
        switch (_swingMode)
        {
            case ToshibaSwingMode::ToshibaSwingModeOff:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeOff;
                break;
            case ToshibaSwingMode::ToshibaSwingModeBoth:
                newSwingMode = ToshibaSwingMode::ToshibaSwingModeHorizontal;
                break;
            case ToshibaSwingMode::ToshibaSwingModeVertical:
                newSwingMode = _lastNoneMovingSwingMode;
                break;
            case ToshibaSwingMode::ToshibaSwingModeHorizontal:
                newSwingMode = _lastNoneMovingSwingMode;
                break;
            default: // Fix vertical position modes
                return;
        }
    }
    logDebugP("Setting vertical swing to %d", (int)newSwingMode);
    _swingMode = newSwingMode;
    sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)newSwingMode);
}

void ToshibaDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // To Do: Implementation for horizontal fix position control
    statusFeedback.swingHorizontalFixPositionChanged(position);
}

void ToshibaDriver::setSwingVerticalFixPosition(unsigned int position)
{
    switch (position)
    {
        case 0:
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)ToshibaSwingMode::ToshibaSwingModeOff);
            break;
        case 1:
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)ToshibaSwingMode::ToshibaSwingModeFixPosition1);
            break;
        case 2:
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)ToshibaSwingMode::ToshibaSwingModeFixPosition2);
            break;
        case 3:
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)ToshibaSwingMode::ToshibaSwingModeFixPosition3);
            break;
        case 4:
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)ToshibaSwingMode::ToshibaSwingModeFixPosition4);
            break;
        case 5:
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSwing, (uint8_t)ToshibaSwingMode::ToshibaSwingModeFixPosition5);
            break;
        default:
            logErrorP("Unsupported vertical fix position: %d", position);
            return;
    }
    statusFeedback.swingVerticalFixPositionChanged(position);
}

void ToshibaDriver::setWifiLed(bool on)
{
    sendCommand(ToshibaCommandType::ToshibaCommandTypeWifiLED, on ? (uint8_t)ToshibaLedState::On : (uint8_t)ToshibaLedState::Off);
}

void ToshibaDriver::setExternalSensorRoomTemperature(float temperaturCelsius)
{
}

bool ToshibaDriver::supportExternalRoomTemperatureSensor()
{
    return false;
}

float ToshibaDriver::accuracyInDegrees()
{
    return 1.f;
}

void ToshibaDriver::setDeviceMode(AirConditionDeviceMode mode)
{
    switch (mode)
    {
        case AirConditionDeviceMode::AirConditionDeviceModeStandard:
            logDebugP("Setting device mode to Standard");
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeStandard);
            break;
        case AirConditionDeviceMode::AirConditionDeviceModeHiPower:
            logDebugP("Setting device mode to HiPower");
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeHiPower);
            break;
        case AirConditionDeviceMode::AirConditionDeviceModeEco:
            logDebugP("Setting device mode to Eco");
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeEco);
            break;
        // case AirConditionDeviceMode::AirConditionDeviceModeFireplace1:
        //     logDebugP("Setting device mode to Fireplace1");
        //     sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeFireplace1);
        //     break;
        // case AirConditionDeviceMode::AirConditionDeviceModeFireplace2:
        //     logDebugP("Setting device mode to Fireplace2");
        //     sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeFireplace2);
        //     break;
        // case AirConditionDeviceMode::AirConditionDeviceModeEightDegree:
        //     logDebugP("Setting device mode to EightDegree");
        //     sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeEightDegree);
        //     break;
        case AirConditionDeviceMode::AirConditionDeviceModeSilent1:
            logDebugP("Setting device mode to Silent1");
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeSilent1);
            break;
        case AirConditionDeviceMode::AirConditionDeviceModeSilent2:
            logDebugP("Setting device mode to Silent2");
            sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeSilent2);
            break;
            // case AirConditionDeviceMode::AirConditionDeviceModeSleep:
            //     logDebugP("Setting device mode to Sleep");
            //     sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeSleep);
            //     break;
            // case AirConditionDeviceMode::AirConditionDeviceModeFloor:
            //     logDebugP("Setting device mode to Floor");
            //     sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeFloor);
            //     break;
            // case AirConditionDeviceMode::AirConditionDeviceModeComfort:
            //     logDebugP("Setting device mode to Comfort");
            //     sendCommand(ToshibaCommandType::ToshibaCommandTypeSpecialMode, ToshibaSpecialMode::ToshibaSpecialModeComfort);
            //     break;
    }
}

void ToshibaDriver::setMaxPowerLevel(uint8_t percentage)
{
    auto value = (uint8_t)(round(percentage / 25.f));
    switch (value)
    {
        case 0:
        case 1:
        case 2:
            logDebugP("Setting max power level to 50%");
            sendCommand(ToshibaCommandType::ToshibaCommandTypePowerSel, (uint8_t)ToshibaPowerLevel::ToshibaPowerLevel50);
            break;
        case 3:
            logDebugP("Setting max power level to 75%");
            sendCommand(ToshibaCommandType::ToshibaCommandTypePowerSel, (uint8_t)ToshibaPowerLevel::ToshibaPowerLevel75);
            break;
        case 4:
            logDebugP("Setting max power level to 100%");
            sendCommand(ToshibaCommandType::ToshibaCommandTypePowerSel, (uint8_t)ToshibaPowerLevel::ToshibaPowerLevel100);
            break;
    }
}

void ToshibaDriver::setAirPurification(bool on)
{
    logDebugP("Setting air purification to %s", on ? "on" : "off");
    sendCommand(ToshibaCommandType::ToshibaCommandTypePure, on ? (uint8_t)ToshibaPureState::ToshibaPureStateOn : (uint8_t)ToshibaPureState::ToshibaPureStateOff);
}

void ToshibaDriver::updatePower(bool power) { statusFeedback.updatePower(power); }
void ToshibaDriver::updateMode(AirConditionMode mode) { statusFeedback.updateMode(mode); }
void ToshibaDriver::updateTargetTemperature(float temperaturCelsius) { statusFeedback.updateTargetTemperature(temperaturCelsius); }
void ToshibaDriver::updateFanSpeed(int speed) { statusFeedback.updateFanSpeed(speed); }
void ToshibaDriver::updateSwingHorizontal(bool swing) { statusFeedback.updateSwingHorizontal(swing); }
void ToshibaDriver::updateSwingVertical(bool swing) { statusFeedback.updateSwingVertical(swing); }
void ToshibaDriver::updateCurrentTemperature(float temperaturCelsius) { statusFeedback.updateCurrentTemperature(temperaturCelsius); }
void ToshibaDriver::updateOutdoorTemperature(float temperaturCelsius) { statusFeedback.updateOutdoorTemperature(temperaturCelsius); }
void ToshibaDriver::updateDeviceMode(AirConditionDeviceMode mode) { statusFeedback.updateDeviceMode(mode); }
void ToshibaDriver::updateMaxPowerLevel(uint8_t percentage) { statusFeedback.updateMaxPowerLevel(percentage); }
void ToshibaDriver::updateAirPurification(bool on) { statusFeedback.updateAirPurification(on); }
void ToshibaDriver::updateOnlineStatus(bool online) { statusFeedback.updateOnlineStatus(online); }
void ToshibaDriver::updateWifiLed(bool on) { statusFeedback.updateWifiLed(on); }
void ToshibaDriver::updateHumidity(uint8_t humidity) { statusFeedback.updateHumidity(humidity); }
void ToshibaDriver::updateHumidityMode(uint8_t humidityMode) { statusFeedback.updateHumidityMode(humidityMode); }
void ToshibaDriver::updateTotalEnergyConsumption(uint32_t totalEnergyWh) { statusFeedback.updateTotalEnergyConsumption(totalEnergyWh); }