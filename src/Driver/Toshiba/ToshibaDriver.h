#pragma once
#include "../Midea/MideaDriver.h"

class ToshibaDriver : public MideaDriver
{
    public:
        ToshibaDriver(AirConditionDriverStatusFeedback& statusFeedback);
        virtual const std::string name() const override;
};