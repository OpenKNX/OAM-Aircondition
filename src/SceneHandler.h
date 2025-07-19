#pragma once

#include "AirConditionDriver.h"

struct SceneParameters
{
    uint8_t onOff = 255;
    uint8_t operationMode = 255;
    uint8_t temperature = 255;
    uint8_t fan = 255;
    uint8_t swing = 255;
    uint8_t position = 255;
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
