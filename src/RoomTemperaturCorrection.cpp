#include "RoomTemperaturCorrection.h"
#include "AirConditionDriver.h"

RoomTemperatureCorrection::RoomTemperatureCorrection(AirConditionDriver& airConditionDriver)
    : _airConditionDriver(airConditionDriver)
{
}

std::string RoomTemperatureCorrection::logPrefix()
{
    return "RoomTemperatureCorrection";
}

void RoomTemperatureCorrection::setAirconditionRoomTemperatur(float temperature)
{
    logInfoP("Set aircondition room temperature to %.1f °C", temperature);
    _aircondtionRoomTemperature = temperature;
    recalculateOffset();
}

void RoomTemperatureCorrection::setNewExternalRoomTemperature(float temperature)
{
    logInfoP("Set new external room temperature to %.1f °C", temperature);
    _externalRooomTemperature = temperature;
    _lastExternalRoomTemperatureRead = 0;
    _lastExternalRoomTemperaturUpdate = max(1UL, millis());
    recalculateOffset();
}

void RoomTemperatureCorrection::recalculateOffset()
{
    float offset = _currentOffset;
    if (_externalRooomTemperature != 0.0f && _aircondtionRoomTemperature != 0.0f)
    {
        _currentOffset = _externalRooomTemperature - _aircondtionRoomTemperature;
        logInfoP("Calculated room temperature offset: %.1f °C", _currentOffset);
    }
    else
    {
        _currentOffset = 0.0f;
        logInfoP("Reset room temperature offset to 0.0 °C");
    }
    if (abs(offset - _currentOffset) > 0.01f)
    {
        logInfoP("Room temperature offset changed from %.1f °C to %.1f °C", offset, _currentOffset);
        setTargetTemperaturToAircondition(_targetTemperature);
    }
}

void RoomTemperatureCorrection::loop()
{
    unsigned long currentMillis = millis();
    if (_lastExternalRoomTemperaturUpdate && (currentMillis - _lastExternalRoomTemperaturUpdate > 1000 * 60 * 10)) // 10 minutes timeout
    {
        logInfoP("External room temperature not updated for more than 10 minutes, reading");
        KoAIR_RoomTemperatureInput.requestObjectRead();
        _lastExternalRoomTemperaturUpdate = 0;
        _lastExternalRoomTemperatureRead = max(1UL, currentMillis);
        
    }
    else if (_lastExternalRoomTemperatureRead && (currentMillis - _lastExternalRoomTemperatureRead > 1000 * 5)) // 5 seconds timeout
    {
        logInfoP("External room temperature not read for more than 5 seconds, resetting");
        _lastExternalRoomTemperatureRead = 0;
        _externalRooomTemperature = 0.0f;
        recalculateOffset();
    }
}

void RoomTemperatureCorrection::setTargetTemperaturToAircondition(float temperature)
{
    logInfoP("Set target temperature to %.1f °C", temperature);
    _targetTemperature = temperature;
    auto calculatedTemperature = temperature - _currentOffset;
    float precision = 1 / _airConditionDriver.accuracyInDegrees();
    _correctedTargetTemperature = round(calculatedTemperature * precision) / precision; // Round to the air condition resolution
    logInfoP("Calculated target temperature %.1f °C with offset %.1f °C, rounded to %.1f °C", calculatedTemperature, _currentOffset, _correctedTargetTemperature);
    auto roundedTemperature = round(temperature * precision) / precision;
    if (_correctedTargetTemperature == roundedTemperature && abs(_currentOffset) > 0.2f)
    {
        // Target temperature is same as the real target temeperature, but we have still an offset, try to correct it
        if (_currentOffset > 0)
        {
            _correctedTargetTemperature -= _airConditionDriver.accuracyInDegrees();
            logInfoP("Target temperature %.1f °C would be the correct value, but room is still to hot, set to %.1f °C", _targetTemperature, _correctedTargetTemperature);
        }
        else
        {
            _correctedTargetTemperature += _airConditionDriver.accuracyInDegrees();
            logInfoP("Target temperature %.1f °C would be the correct value, but room is still to cold, set to %.1f °C", _targetTemperature, _correctedTargetTemperature);
        }
    }
    if (_correctedTargetTemperature < _airConditionDriver.getMinimumTargetTemperature())
    {
        logInfoP("Corrected target temperature %.1f °C is below minimum, setting to %.1f °C", _correctedTargetTemperature, _airConditionDriver.getMinimumTargetTemperature());
        _correctedTargetTemperature = _airConditionDriver.getMinimumTargetTemperature();
    }
    else if (_correctedTargetTemperature > _airConditionDriver.getMaximumTargetTemperature())
    {
        logInfoP("Corrected target temperature %.1f °C is above maximum, setting to %.1f °C", _correctedTargetTemperature, _airConditionDriver.getMaximumTargetTemperature());
        _correctedTargetTemperature = _airConditionDriver.getMaximumTargetTemperature();
    }
    logInfoP("Set target temperatur %.1f °C as %.1f °C to aircondition", temperature, _correctedTargetTemperature);
    _airConditionDriver.setTargetTemperature(_correctedTargetTemperature);
}

void RoomTemperatureCorrection::airconditionReportTargetTemperatureChanged(float temperature)
{
    logInfoP("Aircondition reported target temperature changed to %.1f °C, try to correct it", temperature);
    setTargetTemperaturToAircondition(temperature);
}