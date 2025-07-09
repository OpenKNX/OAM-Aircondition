#include "ToshibaDriver.h"

ToshibaDriver::ToshibaDriver(AirConditionDriverStatusFeedback& statusFeedback)
    : MideaDriver(statusFeedback)
{
}   

const std::string ToshibaDriver::name() const
{
    return "Toshiba";
}
