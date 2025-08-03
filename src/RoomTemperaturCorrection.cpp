#include "RoomTemperaturCorrection.h"
#include "AirConditionDriver.h"

RoomTemperatureCorrection::RoomTemperatureCorrection(AirConditionDriver& airConditionDriver)
    : _airConditionDriver(airConditionDriver)
{
}

void RoomTemperatureCorrection::setAirconditionRoomTemperatur(float temperature)
{
    _aircondtionRoomTemperature = temperature;
    recalculateOffset();
}

void RoomTemperatureCorrection::setNewExternalRoomTemperature(float temperature)
{
    _externalRooomTemperature = temperature;
    _lastExternalRoomTemperaturUpdate = max(1UL, millis());
    recalculateOffset();
}

void RoomTemperatureCorrection::setTargetTemperaturToAircondition(float temperature)
{
    _correctedTargetTemperature = _airConditionDriver.roundTemperatureToAirconditionResolution(temperature + _currentOffset);
    _airConditionDriver.setTargetTemperature(_correctedTargetTemperature);
}

bool RoomTemperatureCorrection::airconditionReportTargetTemperatureChanged(float temperature)
{
    if (abs(temperature - _correctedTargetTemperature) < 0.01f)
    {
        // No significant change, return false
        return false;
    }
    _correctedTargetTemperature
    float correctedTemperature = temperature + _currentOffset;
    return _airConditionDriver.airconditionReportTargetTemperatureChanged(correctedTemperature);
}