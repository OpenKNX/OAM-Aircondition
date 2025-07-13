#pragma once
#include "../../AirConditionDriver.h"

class MitsubishiDriver : public AirConditionDriver
{

    private:
        void requestInitialData();
        void requestSettingsData();
        void requestStatusData();
        uint32_t _lastStatusRequest = 0;
        uint32_t _lastSettingsRequest = 0;

    public:
        MitsubishiDriver(AirConditionDriverStatusFeedback& statusFeedback);
  
        virtual void setup() override;
        virtual void loop() override;

        virtual const std::string name() const override;
        virtual void showInformations() override;
        virtual float getMinimumTargetTemperature() override;;
        virtual float getMaximumTargetTemperature() override;;
        virtual unsigned int getMaximumFanSpeed() override;
        virtual unsigned int getMaximumHorizontalFixPosition() override;
        virtual unsigned int getMaximumVertiacalFixPosition() override;

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