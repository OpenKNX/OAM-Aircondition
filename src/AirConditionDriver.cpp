#include "AirConditionDriver.h"

AirConditionDriver::AirConditionDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : statusFeedback(statusFeedback)
{
}

const std::string AirConditionDriver::logPrefix() const 
{
    return name();
};
/**
 * Convert byte vector to hex string
 */
std::string AirConditionDriver::toHexString(const std::vector<uint8_t>& data) 
{
    std::string result;
    for (size_t i = 0; i < data.size(); i++) {
        if (i > 0) result += " ";
        char hex_str[4];
        sprintf(hex_str, "%02X", data[i]);
        result += hex_str;
    }
    return result;
}

/**
 * Convert byte array to hex string
 */
std::string AirConditionDriver::toHexString(const uint8_t* data, size_t length) 
{
    std::string result;
    for (size_t i = 0; i < length; i++) {
        if (i > 0) result += " ";
        char hex_str[4];
        sprintf(hex_str, "%02X", data[i]);
        result += hex_str;
    }
    return result;
}


const char* AirConditionDriver::getDriverStateString(AirConditionDriverState state)
{
    switch (state)
    {
        case AirConditionDriverState::AirConditionDriverStateNotStarted:
            return "Not Started";
        case AirConditionDriverState::AirConditionDriverStateStarting:
            return "Starting";
        case AirConditionDriverState::AirConditionDriverStateOk:
            return "Ok";
        case AirConditionDriverState::AirConditionDriverStateError:
            return "Error";
        default:
            return "Unknown State";
    }
}