#pragma once
#include "../../AirConditionDriver.h"

class DaikinDriver : public AirConditionDriver
{
    public:
        DaikinDriver(AirConditionDriverStatusFeedback& statusFeedback);
  
        virtual void setup() const override;
        virtual void loop() const override;

        virtual const std::string name() const override;
        virtual void showInformations() const override;
        virtual float getMinimumTargetTemperature() const override;;
        virtual float getMaximumTargetTemperature() const override;;
        virtual unsigned int getMaximumFanSpeed() const override;
        virtual unsigned int getMaximumHorizontalFixPosition() const override;
        virtual unsigned int getMaximumVertiacalFixPosition() const override;

        virtual void setPower(bool power) override;
        virtual void setMode(AirConditionMode mode) override;
        virtual void setTargetTemperature(float temperaturCelius) override;
        virtual void setFanSpeed(unsigned int speed) override;
        virtual void setSwingHorizontal(bool swing) override;
        virtual void setSwingVertical(bool swing) override;
        virtual void setSwingHorizontalFixPosition(unsigned int position) override;
        virtual void setSwingVerticalFixPosition(unsigned int position) override;
        virtual void setExternalSensorRoomTemperature(float temperaturCelius) override;
};