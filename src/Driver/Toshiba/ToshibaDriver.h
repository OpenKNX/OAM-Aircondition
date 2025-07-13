// This code is based on:
// https://github.com/pedobry/esphome_toshiba_suzumi

#pragma once
#include "../../AirConditionDriver.h"


enum class ToshibaCommandType : uint8_t {
  ToshibaCommandTypeHandshake = 0,  // dummy command to handle all handshake requests
  ToshibaCommandTypeDelay = 1, // dummy command to issue a delay in communication
  ToshibaCommandTypePowerState = 128,
  ToshibaCommandTypePowerSel = 135,
  ToshibaCommandTypeComfortSleep = 148, // { ON = 65, OFF = 66 }
  ToshibaCommandTypeFan = 160,
  ToshibaCommandTypeSwing = 163,
  ToshibaCommandTypeMode = 176,
  ToshibaCommandTypeTargetTemperature = 179,
  ToshibaCommandTypeRoomTemperature = 187,
  ToshibaCommandTypeOutdoorTemperature = 190,
  ToshibaCommandTypeWifiLED = 223,
  ToshibaCommandTypeSpecialMode = 247,
};

enum class ToshibaState { ToshibaStateOn = 48, ToshibaStateOff = 49 };

struct ToshibaCommand {
  ToshibaCommandType cmd;
  std::vector<uint8_t> payload;
  int delay;
};

enum ToshibaSpecialMode {
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

enum ToshibaDriverMode { HEAT_COOL = 65, COOL = 66, HEAT = 67, DRY = 68, FAN_ONLY = 69 };

class ToshibaDriver : public AirConditionDriver
{
    private:
        static const std::vector<uint8_t> PayloadHandshake[6];
        static const std::vector<uint8_t> PayloadPostHandshake[2];
        static const uint8_t SpecialModeTempOffset;

        void enqueueCommand(const ToshibaCommand &command);
        void startCommunication();
        void processCommandQueue();
        void requestData(ToshibaCommandType cmd);
        void requestTemperatures();
        void requestInitialData();
        void handleReceivedByte(uint8_t c);
        bool validateMessage();
        void parseResponse(std::vector<uint8_t> rawData);
        uint8_t checksum(std::vector<uint8_t> data, uint8_t length);
        void sendToUart(const ToshibaCommand command);

        std::vector<uint8_t> _receivedMessage = {};
        std::vector<ToshibaCommand> _commandQueue = {};
        uint32_t _lastTemperatureRequest = 0;
        uint32_t _lastCommandTimestamp = 0;
        uint32_t _lastReceivedByteTimestamp = 0;
        ToshibaSpecialMode _specialMode = ToshibaSpecialMode::ToshibaSpecialModeStandard;
  
    public:
        ToshibaDriver(AirConditionDriverStatusFeedback& statusFeedback);
    
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