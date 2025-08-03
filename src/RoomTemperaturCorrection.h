class RoomTemperatureCorrection {

    AirConditionDriver& _airConditionDriver;
private:
    float _aircondtionRoomTemperature = 0.0f; 
    float _externalRooomTemperature = 0.0f;
    float _currentOffset = 0.0f; // Current offset applied to the room temperature
    float _correctedTargetTemperature = 0.0f; // Current offset applied to the target temperature
    unsigned long _lastExternalRoomTemperaturUpdate = 0; // Timestamp of the last update
    bool _waitForExternalRoomTemperature = false;
    void recalculateOffset();
 
public:
    RoomTemperatureCorrection(AirConditionDriver& airConditionDriver);
    void setAirconditionRoomTemperatur(float temperature);
    void setNewExternalRoomTemperature(float temperature);
    void setTargetTemperaturToAircondition(float temperature);
    bool airconditionReportTargetTemperatureChanged(float temperature);
     void loop();
  
   


};