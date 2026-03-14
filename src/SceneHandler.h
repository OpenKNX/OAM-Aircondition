#pragma once

#include "AirConditionDriver.h"

struct SceneParameters
{
    PT_AIRSceneOnOff onOff = PT_AIRSceneOnOff::NoChange;
    uint8_t operationMode = 255;
    uint8_t temperature = 255;
    uint8_t fan = 255;
    uint8_t swing = 255;
    uint8_t position = 255;
    uint8_t powerLimit = 255;
    uint8_t deviceMode = 255;
    uint8_t airPurification = 255;
};

class SceneHandler
{
public:
    SceneHandler(AirConditionDriver& driver);
    void handleScene(uint8_t sceneNumber);
    std::string logPrefix() const;
  

private:
    AirConditionDriver& _airConditionDriver;
    void applyParameters(int index);
    SceneParameters getSceneParameters(int index);
   
};
