#pragma once
#include "../../AirConditionDriver.h"

class MideaDriver : public AirConditionDriver
{
    public:
        MideaDriver(AirConditionDriverStatusFeedback& statusFeedback);
  
        virtual void setup() override;
        virtual void startCommunication(bool restart) override;
        virtual void requestAllData() override;
        virtual void loop() override;

        virtual const std::string name() const override;
        virtual void showInformations() override;
        virtual float getMinimumTargetTemperature() override;;
        virtual float getMaximumTargetTemperature() override;;
        virtual unsigned int getMaximumFanSpeed() override;
        virtual unsigned int getMaximumHorizontalFixPosition() override;
        virtual unsigned int getMaximumVerticalFixPosition() override;
        virtual unsigned int getMaximumHumidityModeLevels() override;
        virtual bool supportExternalRoomTemperatureSensor() override;
        virtual float accuracyInDegrees() override;

        virtual void setPower(bool power) override;
        virtual void setMode(AirConditionMode mode) override;
        virtual void setTargetTemperature(float temperaturCelsius) override;
        virtual void setFanSpeed(unsigned int speed) override;
        virtual void setSwingHorizontal(bool swing) override;
        virtual void setSwingVertical(bool swing) override;
        virtual void setSwingHorizontalFixPosition(unsigned int position) override;
        virtual void setSwingVerticalFixPosition(unsigned int position) override;
        virtual void setExternalSensorRoomTemperature(float temperaturCelsius) override;
        virtual void setWifiLed(bool on) override;
        virtual void setDeviceMode(AirConditionDeviceMode mode) override;
        virtual void setMaxPowerLevel(uint8_t percentage) override;
        virtual void setAirPurification(bool on) override;

        void updatePower(bool power);
        void updateMode(AirConditionMode mode);
        void updateTargetTemperature(float temperaturCelsius);
        void updateFanSpeed(int speed);
        void updateSwingHorizontal(bool swing);
        void updateSwingVertical(bool swing);
        void updateCurrentTemperature(float temperaturCelsius);
        void updateOutdoorTemperature(float temperaturCelsius);
        void updateDeviceMode(AirConditionDeviceMode mode);
        void updateMaxPowerLevel(uint8_t percentage);
        void updateAirPurification(bool on);
        void updateOnlineStatus(bool online);
        void updateWifiLed(bool on);
        void updateHumidity(uint8_t humidity);
        void updateHumidityMode(uint8_t humidityMode);
        void updateTotalEnergyConsumption(uint32_t totalEnergyWh);
};