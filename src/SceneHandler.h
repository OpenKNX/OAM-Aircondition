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
    bool autoOffActive = false;
    PT_AIRSceneAutoOffDelayBase autoOffDelayBase = PT_AIRSceneAutoOffDelayBase::Minutes;
    uint8_t autoOffDelayTime = 0;
};

class SceneHandlerCallback
{
  public:
    virtual void sceneApplied(const SceneParameters& params) = 0;
};

class SceneHandler
{
public:
    SceneHandler(AirConditionDriver& driver, SceneHandlerCallback* callback = nullptr);
    void handleScene(uint8_t sceneNumber);
    std::string logPrefix() const;
  

private:
    AirConditionDriver& _airConditionDriver;
    SceneHandlerCallback* _callback = nullptr;
    void applyParameters(int index);
    SceneParameters getSceneParameters(int index);
   
};
