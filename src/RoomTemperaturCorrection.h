#pragma once
#include <string>

class AirConditionDriver;

class RoomTemperatureCorrection {

    AirConditionDriver& _airConditionDriver;
private:
    bool _hasFirstTargetTemperaturFeedback = false;
    unsigned long _correctedTargetTemperatureNeededSince = 0;
    float _tartetTemperaturNeedToSent = 0;
    float _aircondtionRoomTemperature = 0.0f; 
    float _externalRoomTemperature = 0.0f;
    float _currentOffset = 0.0f; // Current offset applied to the room temperature
    float _usedOffset = 0.0f; // Offset used for the last target temperature
    float _targetTemperature = 0.0f; // Last target temperature set to the air condition
    float _correctedTargetTemperature = 0.0f; // Current offset applied to the target temperature
    unsigned long _lastExternalRoomTemperaturUpdate = 0; // Timestamp of the last update
    unsigned long _lastExternalRoomTemperatureRead = 0; // Timestamp of the last read
    void recalculateOffset();

    std::string logPrefix();
 
public:
    RoomTemperatureCorrection(AirConditionDriver& airConditionDriver);
    void setup();
    void setAirconditionRoomTemperatur(float temperature);
    void setNewExternalRoomTemperature(float temperature);
    void setTargetTemperaturToAircondition(float temperature);
    void airconditionReportTargetTemperatureChanged(float temperature);
    float correctTemperatureFeedbackFromAircondition(float temperature);
    void loop();
  
   


};