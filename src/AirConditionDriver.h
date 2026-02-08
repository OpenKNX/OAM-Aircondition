#pragma once
#include "OpenKNX.h"

enum AirConditionMode
{
    AirConditionModeAuto = 0,
    AirConditionModeCool = 1,
    AirConditionModeHeat = 2,
    AirConditionModeDry = 3,
    AirConditionModeFan = 4,
};

enum AirConditionDriverState
{
    AirConditionDriverStateNotStarted = 0,
    AirConditionDriverStateStarting = 1,
    AirConditionDriverStateOk = 2,
    AirConditionDriverStateError = 3,
};

enum AirConditionDeviceMode
{
    AirConditionDeviceModeStandard = 1,
    AirConditionDeviceModeHiPower = 2,
    AirConditionDeviceModeEco = 3,
    AirConditionDeviceModeSilent1 = 4,
    AirConditionDeviceModeSilent2 = 5,

    // AirConditionDeviceModeFireplace1 = 5,
    // AirConditionDeviceModeFireplace2 = 6,
    // AirConditionDeviceModeEightDegree = 7,
    // AirConditionDeviceModeComfort,
    // AirConditionDeviceModeSleep,
    // AirConditionDeviceModeFloor,
    
    // Extended unified naming (aliases preserve existing numeric values where applicable)
    AirConditionDeviceModeNormal         = AirConditionDeviceModeStandard,
    AirConditionDeviceModeHighPerformance= AirConditionDeviceModeHiPower,
    AirConditionDeviceModePowerSaving    = AirConditionDeviceModeEco,
    AirConditionDeviceModeQuietMode      = AirConditionDeviceModeSilent1, // map primary quiet to Silent1

};

class AirConditionDriverStatusFeedback
{
public:
    virtual void driverStateChanged(AirConditionDriverState state, std::string error = "") = 0;
    virtual AirConditionDriverState getDriverState() const = 0;
    virtual void powerChanged(bool power) = 0;
    virtual void modeChanged(AirConditionMode mode) = 0;
    virtual void targetTemperatureChanged(float temperaturCelius, bool isFeedbackFromSettin) = 0;
    virtual void fanSpeedChanged(int speed) = 0;
    virtual void swingHorizontalChanged(bool swing) = 0;
    virtual void swingVerticalChanged(bool swing) = 0;
    virtual void swingHorizontalFixPositionChanged(int position) = 0;
    virtual void swingVerticalFixPositionChanged(int position) = 0;
    virtual void roomTemperatureChanged(float temperaturCelius) = 0;
    virtual void outsideTemperaturChanged(float temperaturCelius) = 0;
    virtual void deviceModeChanged(AirConditionDeviceMode mode) = 0;
    virtual void maxPowerLevelChanged(uint8_t percentage) = 0;
    virtual void airPurificationChanged(bool on) = 0;

    virtual void updatePower(bool power) = 0;
    virtual void updateMode(AirConditionMode mode) = 0;
    virtual void updateTargetTemperature(float temperaturCelius) = 0;
    virtual void updateFanSpeed(int speed) = 0;
    virtual void updateSwingHorizontal(bool swing) = 0;
    virtual void updateSwingVertical(bool swing) = 0;
    virtual void updateCurrentTemperature(float temperaturCelius) = 0;
    virtual void updateOutdoorTemperature(float temperaturCelius) = 0;
    virtual void updateDeviceMode(AirConditionDeviceMode mode) = 0;
    virtual void updateMaxPowerLevel(uint8_t percentage) = 0;
    virtual void updateAirPurification(bool on) = 0;
    virtual void updateOnlineStatus(bool online) = 0;
    virtual void updateWifiLed(bool on) = 0;
    virtual void updateHumidity(uint8_t humidity) = 0;
    virtual void updateHumidityMode(uint8_t step) = 0;     
    virtual void updateTotalEnergyConsumption(uint32_t totalEnergyWh) = 0;

    // Optional vendor-specific extension telemetry.
    // Default no-op so existing modules/drivers do not need to implement it.
    virtual void updateDaikinExtensionTelemetry(bool fu04Valid,
                                                uint32_t fu04CoolingWh10,
                                                uint32_t fu04HeatingWh10,
                                                bool fx60Valid,
                                                uint32_t fx60Value10)
    {
    }
};

class AirConditionDriver 
{
public:
    const int FAN_SPEED_AUTO = 0; 
protected:
    // Reference to the status feedback interface
    AirConditionDriverStatusFeedback& statusFeedback;
    AirConditionDriver(AirConditionDriverStatusFeedback& statusFeedback);
public:
    static std::string toHexString(const std::vector<uint8_t>& data);
    static std::string toHexString(const uint8_t* data, size_t length); 
    static const char* getDriverStateString(AirConditionDriverState state);
    // Lifecycle methods
    virtual void setup() = 0;
    virtual void startCommunication(bool restart) = 0;
    virtual void requestAllData() = 0;
   
    virtual void loop() = 0;
    virtual void processInputKo(GroupObject &ko) {}
    virtual bool processCommand(const std::string cmd, bool debugKo) { return false; }

    // Information about the air condition device
    virtual const std::string name() const = 0;
    const std::string logPrefix() const;
   
    virtual void showInformations() = 0;
    virtual float getMinimumTargetTemperature() = 0; 
    virtual float getMaximumTargetTemperature() = 0;
    virtual unsigned int getMaximumFanSpeed() = 0; 
    virtual unsigned int getMaximumHorizontalFixPosition() = 0; 
    virtual unsigned int getMaximumVerticalFixPosition() = 0; 
    virtual unsigned int getMaximumHumidityModeLevels() = 0;
    virtual bool supportExternalRoomTemperatureSensor() = 0;

    // Methods to control the air condition device
    virtual float accuracyInDegrees() = 0;
    virtual void setPower(bool power) = 0;
    virtual void setMode(AirConditionMode mode) = 0;
    virtual void setTargetTemperature(float temperaturCelsius) = 0;
    virtual void setFanSpeed(unsigned int speed) = 0;
    virtual void setSwingHorizontal(bool swing) = 0;
    virtual void setSwingVertical(bool swing) = 0;
    virtual void setSwingHorizontalFixPosition(unsigned int position) = 0;
    virtual void setSwingVerticalFixPosition(unsigned int position) = 0;
    virtual void setExternalSensorRoomTemperature(float temperaturCelius) = 0;
    virtual void setWifiLed(bool on) = 0;
    virtual void setDeviceMode(AirConditionDeviceMode mode) = 0;
    virtual void setMaxPowerLevel(uint8_t percentage) = 0;
    virtual void setAirPurification(bool on) = 0;
};
