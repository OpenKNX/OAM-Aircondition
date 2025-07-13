// This code is based on:
// https://github.com/pedobry/esphome_toshiba_suzumi

#include "ToshibaDriver.h"

static const int RECEIVE_TIMEOUT = 200;
static const int COMMAND_DELAY = 100;
constexpr const char* SPECIAL_MODE_EIGHT_DEG = "8 degrees";

const uint8_t ToshibaDriver::SpecialModeTempOffset = 16;

const std::vector<uint8_t> ToshibaDriver::PayloadHandshake[6] = {
    {2, 255, 255, 0, 0, 0, 0, 2},       {2, 255, 255, 1, 0, 0, 1, 2, 254}, {2, 0, 0, 0, 0, 0, 2, 2, 2, 250},
    {2, 0, 1, 129, 1, 0, 2, 0, 0, 123}, {2, 0, 1, 2, 0, 0, 2, 0, 0, 254},  {2, 0, 2, 0, 0, 0, 0, 254},
};

const std::vector<uint8_t> ToshibaDriver::PayloadPostHandshake[2] = {
    {2, 0, 2, 1, 0, 0, 2, 0, 0, 251},
    {2, 0, 2, 2, 0, 0, 2, 0, 0, 250},
};

ToshibaDriver::ToshibaDriver(AirConditionDriverStatusFeedback& statusFeedback)
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

void ToshibaDriver::startCommunication()
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
}

void ToshibaDriver::requestData(ToshibaCommandType cmd)
{
  std::vector<uint8_t> payload = {2, 0, 3, 16, 0, 0, 6, 1, 48, 1, 0, 1};
  payload.push_back(static_cast<uint8_t>(cmd));
  payload.push_back(checksum(payload, payload.size()));
  logInfoP("Requesting data from %d, checksum: %d", payload[12], payload[13]);
  enqueueCommand(ToshibaCommand{.cmd = cmd, .payload = std::vector<uint8_t>{payload}});
}

void ToshibaDriver::requestInitialData()
{
    logDebugP("Requesting initial data");
    requestData(ToshibaCommandType::ToshibaCommandTypePowerState);
    requestData(ToshibaCommandType::ToshibaCommandTypeMode);
    requestData(ToshibaCommandType::ToshibaCommandTypeTargetTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeFan);
    requestData(ToshibaCommandType::ToshibaCommandTypePowerSel);
    requestData(ToshibaCommandType::ToshibaCommandTypeSwing);
    requestData(ToshibaCommandType::ToshibaCommandTypeRoomTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeOutdoorTemperature);
    requestData(ToshibaCommandType::ToshibaCommandTypeSpecialMode);
    _lastTemperatureRequest = millis();
}

void ToshibaDriver::setup()
{
    OPENKNX_AIR_CONDITION_SERIAL.begin(9600, SERIAL_8N1, OPENKNX_AIR_CONDITION_SERIAL_RX, OPENKNX_AIR_CONDITION_SERIAL_TX);
    startCommunication();
    requestInitialData();

    // if (this->wifi_led_disabled_) {
    // // Disable Wifi LED
    //this->sendCmd(ToshibaCommandType::WIFI_LED, 128);
}

/**
 * Checksum is calculated from all bytes excluding start byte.
 * It's (256 - (sum % 256)).
 */
uint8_t ToshibaDriver::checksum(std::vector<uint8_t> data, uint8_t length) 
{
  uint8_t sum = 0;
  for (size_t i = 1; i < length; i++) {
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
    if (at < 2) {
        return true;
    }

    // Byte 3
    if (data[2] != 0x03) {
        // Normal commands starts with 0x02 0x00 0x03 and have length between 15-17 bytes.
        // however there are some special unknown handshake commands which has non-standard replies.
        // Since we don't know their format, we can't validate them.
        return true;
    }

    if (at <= 5) {
        // no validation for these fields
        return true;
    }

    // Byte 7: LENGTH
    uint8_t length = 6 + data[6] + 1;  // prefix + data + checksum

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
    logDebugP("Received: DATA=[%s]", toHexString(data, length).c_str());
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
        case 15:  // response to requestData with the actual value of sensor/setting
            commandType = static_cast<ToshibaCommandType>(rawData[12]);
            value = rawData[13];
            break;
            case 16:  // probably ACK for issued command
            logDebugP("Received message with length: %d and value %s", length, toHexString(rawData).c_str());
            return;
        case 17:  // response to requestData with the actual value of sensor/setting
            commandType = static_cast<ToshibaCommandType>(rawData[14]);
            value = rawData[15];
            break;
        default:
            logDebugP("Received unknown message with length: %d and value %s", length,
            toHexString(rawData).c_str());
            return;
    }
    switch (commandType) 
    {
        case ToshibaCommandType::ToshibaCommandTypeTargetTemperature:
            logInfoP("Received target temp: %d", value);
            if (_specialMode == ToshibaSpecialModeEightDegree) 
            {
                // if special mode is EIGHT_DEG, shift the target temperature by SPECIAL_TEMP_OFFSET
                value -= SpecialModeTempOffset;
                logDebugP("Note: Special Mode \"%s\" is active, shifting target temp to %d", SpecialModeTempOffset, (int) value);
            }
            statusFeedback.targetTemperatureChanged(value);
        break;
        case ToshibaCommandType::ToshibaCommandTypeFan: 
        {
        // if (static_cast<FAN>(value) == FAN::FAN_AUTO) {
        //     ESP_LOGI(TAG, "Received fan mode: AUTO");
        //     this->set_fan_mode_(CLIMATE_FAN_AUTO);
        // } else if (static_cast<FAN>(value) == FAN::FAN_QUIET) {
        //     ESP_LOGI(TAG, "Received fan mode: QUIET");
        //     this->set_fan_mode_(CLIMATE_FAN_QUIET);
        // } else if (static_cast<FAN>(value) == FAN::FAN_LOW) {
        //     ESP_LOGI(TAG, "Received fan mode: LOW");
        //     this->set_fan_mode_(CLIMATE_FAN_LOW);
        // } else if (static_cast<FAN>(value) == FAN::FAN_MEDIUM) {
        //     ESP_LOGI(TAG, "Received fan mode: MEDIUM");
        //     this->set_fan_mode_(CLIMATE_FAN_MEDIUM);
        // } else if (static_cast<FAN>(value) == FAN::FAN_HIGH) {
        //     ESP_LOGI(TAG, "Received fan mode: HIGH");
        //     this->set_fan_mode_(CLIMATE_FAN_HIGH);
        // } else {
        //     auto fanMode = IntToCustomFanMode(static_cast<FAN>(value));
        //     ESP_LOGI(TAG, "Received fan mode: %s", fanMode.c_str());
        //     this->set_custom_fan_mode_(fanMode);
        // }
        break;
        }
        case ToshibaCommandType::ToshibaCommandTypeSwing: 
        {
            // auto swingMode = IntToClimateSwingMode(static_cast<SWING>(value));
            // ESP_LOGI(TAG, "Received swing mode: %s", climate_swing_mode_to_string(swingMode));
            // this->swing_mode = swingMode;
            // break;
        }
        case ToshibaCommandType::ToshibaCommandTypeMode:
        {
            auto mode = (ToshibaDriverMode) value;
            logDebugP("Received AC mode: %d", (int) mode);
            switch (mode)
            {
                case ToshibaDriverMode::HEAT_COOL:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeAuto);
                    break;
                case ToshibaDriverMode::COOL:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeCool);
                    break;
                case ToshibaDriverMode::HEAT:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeHeat);
                    break;
                case ToshibaDriverMode::DRY:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeDry);
                    break;
                case ToshibaDriverMode::FAN_ONLY:
                    statusFeedback.modeChanged(AirConditionMode::AirConditionModeFan);
                    break;
                default:
                    logDebugP("Unknown mode received: %d", (int) mode);

            }
            break;
        }
        case ToshibaCommandType::ToshibaCommandTypeRoomTemperature:
            logDebugP("Received room temp: %d °C", (int) value);
            statusFeedback.roomTemperatureChanged(value);
        break;
        case ToshibaCommandType::ToshibaCommandTypeOutdoorTemperature:
            logDebugP("Received outdoor temp: %d °C", (int) value);
            statusFeedback.outsideTemperaturChanged(value);
            break;
        case ToshibaCommandType::ToshibaCommandTypePowerSel: 
            {
            // auto pwr_level = IntToPowerLevel(static_cast<PWR_LEVEL>(value));
            // logDebugP("Received power select: %d", value);
            // if (pwr_select_ != nullptr) {
            //     pwr_select_->publish_state(pwr_level);
            // }
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
            }
            break;
        default:
        ESP_LOGW(TAG, "Unknown sensor: %d with value %d", sensor, value);
        break;
    }
    _receivedMessage.clear();  // message processed, clear buffer
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
  if (!validateMessage()) {
    _receivedMessage.clear();
  } else {
    _lastReceivedByteTimestamp = millis();
  }
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
    uint32_t now = millis();
    uint32_t cmdDelay = now - _lastCommandTimestamp;

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
        auto newCommand = _commandQueue.front();
        if (newCommand.cmd == ToshibaCommandType::ToshibaCommandTypeDelay && cmdDelay < newCommand.delay) 
        {
            // delay command did not finished yet
            return;
        }
        sendToUart(_commandQueue.front());
        _commandQueue.erase(_commandQueue.begin());
    }
}

void ToshibaDriver::loop()
{
    while (OPENKNX_AIR_CONDITION_SERIAL.available()) 
    {
        uint8_t c = (uint8_t) OPENKNX_AIR_CONDITION_SERIAL.read();  // Read one byte from the serial port
        handleReceivedByte(c);
    }
    processCommandQueue();
    if (millis() - _lastTemperatureRequest > 60000) 
    {
        _lastTemperatureRequest = millis();
        requestTemperatures();
    }
}


/**
 * Send the command to UART interface.
 */
void ToshibaDriver::sendToUart(ToshibaCommand command) 
{
    _lastCommandTimestamp = millis();
    logDebugP("Sending: [%s]", toHexString(command.payload).c_str());
    OPENKNX_AIR_CONDITION_SERIAL.write(command.payload.data(), command.payload.size());
}

float ToshibaDriver::getMinimumTargetTemperature()
{
    return 17.0f;
}
float ToshibaDriver::getMaximumTargetTemperature()
{
    return 30.0f;
}

unsigned int ToshibaDriver::getMaximumFanSpeed()
{
    return 5; 
}
unsigned int ToshibaDriver::getMaximumHorizontalFixPosition()
{
    return 5; 
}
unsigned int ToshibaDriver::getMaximumVertiacalFixPosition()
{
    return 5; 
}

void ToshibaDriver::setPower(bool power)
{
   // To Do: Implementation for power control
   statusFeedback.powerChanged(power);
}

void ToshibaDriver::setMode(AirConditionMode mode)
{
    // To Do: Implementation for mode control
    statusFeedback.modeChanged(mode);
}

void ToshibaDriver::setTargetTemperature(float temperaturCelius)
{
    // To Do: Implementation for target temperature control
    statusFeedback.targetTemperatureChanged(temperaturCelius);
}

void ToshibaDriver::setFanSpeed(unsigned int speed)
{
    // To Do: Implementation for fan speed control
    statusFeedback.fanSpeedChanged(speed);
}

void ToshibaDriver::setSwingHorizontal(bool swing)
{
    // To Do: Implementation for horizontal swing control
    statusFeedback.swingHorizontalChanged(swing);
}

void ToshibaDriver::setSwingVertical(bool swing)
{
    // To Do: Implementation for vertical swing control
    statusFeedback.swingVerticalChanged(swing);
}

void ToshibaDriver::setSwingHorizontalFixPosition(unsigned int position)
{
    // To Do: Implementation for horizontal fix position control
    statusFeedback.swingHorizontalFixPositionChanged(position);
}

void ToshibaDriver::setSwingVerticalFixPosition(unsigned int position)
{
    // To Do: Implementation for vertical fix position control
    statusFeedback.swingVerticalFixPositionChanged(position);
}

void ToshibaDriver::setExternalSensorRoomTemperature(float temperaturCelius)
{
    // To Do: Implementation for external sensor room temperature control
    statusFeedback.roomTemperatureChanged(temperaturCelius);
}