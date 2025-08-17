// This code is based on:
// https://github.com/pedobry/esphome_toshiba_suzumi

#pragma once
#include "../../AirConditionDriver.h"

enum class ToshibaCommandType : uint8_t
{
    ToshibaCommandTypeHandshake = 0, // dummy command to handle all handshake requests
    ToshibaCommandTypeDelay = 1,     // dummy command to issue a delay in communication
    ToshibaCommandTypePowerState = 0x80, //128,
    ToshibaCommandTypePowerSel = 0x87, // 135,
    ToshibaCommandTypeComfortSleep = 0x94, // 148 { ON = 0x41 (65), OFF = 0x42 (66) }
    ToshibaCommandTypeFan = 0xA0, // 160,
    ToshibaCommandTypeSwing = 0xA3, // 163,
    ToshibaCommandTypeMode = 0xB0, // 176,
    ToshibaCommandTypeTargetTemperature = 0xB3, // 179,
    ToshibaCommandTypeRoomTemperature = 0xBB, // 187,
    ToshibaCommandTypeOutdoorTemperature = 0xBE, // 190,
    ToshibaCommandTypeLED = 0xDE, // 222
    ToshibaCommandTypeWifiLED = 0xDF, // 223,
    ToshibaCommandTypeSpecialMode = 0xF7, // 247,
    ToshibaCommandTypePure = 0xC7, // 199,
};

enum class ToshibaState : uint8_t
{
    ToshibaStateOn = 0x30, // 48
    ToshibaStateOff = 0x31, // 49
};

enum class ToshibaPureState : uint8_t
{
    ToshibaPureStateOn = 0x18, // 24
    ToshibaPureStateOff = 0x10, // 16
};

enum ToshibaSpecialMode : uint8_t
{
    ToshibaSpecialModeStandard = 0,
    ToshibaSpecialModeHiPower = 1,
    ToshibaSpecialModeEco = 3,
    ToshibaSpecialModeFireplace1 = 32,
    ToshibaSpecialModeFireplace2 = 48,
    ToshibaSpecialModeEightDegree = 4,
    ToshibaSpecialModeSilent1 = 2,
    ToshibaSpecialModeSilent2 = 10,
    ToshibaSpecialModeSleep = 5,
    ToshibaSpecialModeFloor = 6,
    ToshibaSpecialModeComfort = 7
};

enum ToshibaDriverMode : uint8_t
{
    ToshibaDriverModeAuto = 65,
    ToshibaDriverModeCool = 66,
    ToshibaDriverModeHeat = 67,
    ToshibaDriverModeDry = 68,
    ToshibaDriverModeFanOnly = 69
};

enum ToshibaFan : uint8_t
{
    ToshibaFanQuit = 49,
    ToshibaFanLow = 50,
    ToshibaFanMode2 = 51,
    ToshibaFanMedium = 52,
    ToshibaFanMode4 = 53,
    ToshibaFanHigh = 54,
    ToshibaFanAuto = 65
};

enum class ToshibaSwingMode : uint8_t
{
    ToshibaSwingModeOff = 49,
    ToshibaSwingModeBoth = 67,
    ToshibaSwingModeVertical = 65,
    ToshibaSwingModeHorizontal = 66,
    ToshibaSwingModeFixPosition1 = 80,
    ToshibaSwingModeFixPosition2 = 81,
    ToshibaSwingModeFixPosition3 = 82,
    ToshibaSwingModeFixPosition4 = 83,
    ToshibaSwingModeFixPosition5 = 84
};

enum class ToshibaPowerLevel : uint8_t 
{ 
    ToshibaPowerLevel50 = 50, 
    ToshibaPowerLevel75 = 75, 
    ToshibaPowerLevel100 = 100 
};



enum class ToshibaLedState : uint8_t
{
    Off = 128,
    On = 0
};

struct ToshibaCommand
{
    ToshibaCommandType cmd;
    std::vector<uint8_t> payload;
    int delay = 0;
    int retryCount = 0;
    bool awaitAnswer = false; 
    unsigned long timestampSent = 0; 
    bool requestFeedback = false;
};

class ToshibaDriver : public AirConditionDriver
{
  private:
    static const std::vector<uint8_t> PayloadHandshake[6];
    static const std::vector<uint8_t> PayloadPostHandshake[2];
    static const uint8_t EightDegreeSpecialModeTempOffset;
    static const uint8_t EightDegreeSpecialModeMinTemp;
    static const uint8_t EightDegreeSpecialModeMaxTemp;
    static const uint8_t StandardModeMinTemp;
    static const uint8_t StandardModeMaxTemp;

    void sendCommand(ToshibaCommandType cmd, uint8_t payload = 0);
    void enqueueCommand(const ToshibaCommand &command);
    void startCommunication(bool restart);
    void processCommandQueue();
    void requestData(ToshibaCommandType cmd);
    ToshibaCommand createRequestCommand(ToshibaCommandType cmd);
    void requestTemperatures();
    void requestAllData();
    void handleReceivedByte(uint8_t c);
    bool validateMessage();
    void parseResponse(std::vector<uint8_t> rawData);
    uint8_t checksum(std::vector<uint8_t> data, uint8_t length);
    void sendToUart(const ToshibaCommand& command);

    std::vector<uint8_t> _receivedMessage = {};
    std::vector<ToshibaCommand> _commandQueue = {};
    unsigned long _lastTemperatureRequest = 0;
    unsigned long _lastCommandTimestamp = 0;
    unsigned long _lastReceivedByteTimestamp = 0;
    ToshibaSwingMode _lastNoneMovingSwingMode = ToshibaSwingMode::ToshibaSwingModeOff;
    ToshibaSpecialMode _specialMode = ToshibaSpecialMode::ToshibaSpecialModeStandard;
    ToshibaSwingMode _swingMode = ToshibaSwingMode::ToshibaSwingModeOff;
    ToshibaDriverMode _mode = ToshibaDriverMode::ToshibaDriverModeAuto;
    bool _power = false;


  public:
    ToshibaDriver(AirConditionDriverStatusFeedback &statusFeedback);

    virtual void setup() override;
    virtual void loop() override;

    virtual const std::string name() const override;
    virtual void showInformations() override;
    virtual float getMinimumTargetTemperature() override;
    virtual float getMaximumTargetTemperature() override;
    virtual unsigned int getMaximumFanSpeed() override;
    virtual unsigned int getMaximumHorizontalFixPosition() override;
    virtual unsigned int getMaximumVertiacalFixPosition() override;
    virtual bool supportExternalRoomTemperatureSensor() override;
    virtual float accuracyInDegrees() override;

    virtual void setPower(bool power) override;
    virtual void setMode(AirConditionMode mode) override;
    virtual void setTargetTemperature(float temperaturCelius) override;
    virtual void setFanSpeed(unsigned int speed) override;
    virtual void setSwingHorizontal(bool swing) override;
    virtual void setSwingVertical(bool swing) override;
    virtual void setSwingHorizontalFixPosition(unsigned int position) override;
    virtual void setSwingVerticalFixPosition(unsigned int position) override;
    virtual void setExternalSensorRoomTemperature(float temperaturCelius) override;
    virtual void setWifiLed(bool on) override;
    virtual void setDeviceMode(AirConditionDeviceMode mode) override;
    virtual void setMaxPowerLevel(uint8_t percentage) override;
    virtual void setAirPurification(bool on) override;

};