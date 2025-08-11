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

void RoomTemperatureCorrection::setup()
{
    logInfoP("RoomTemperatureCorrection setup");
    if (!KoAIR_RoomTemperatureInput.initialized())
    {
        KoAIR_RoomTemperatureInput.requestObjectRead();
        _lastExternalRoomTemperatureRead = max(1UL, millis());
    }
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
    _externalRoomTemperature = temperature;
    _lastExternalRoomTemperatureRead = 0;
    // <Enumeration Text="Nein" Value="0" Id="%ENID%" />
    // <Enumeration Text="Ja, nach Zeitablauf Lesen, dann ignorieren" Value="1" Id="%ENID%" />
    // <Enumeration Text="Ja, nach Zeitablauf ignorieren" Value="2" Id="%ENID%" />
    if (ParamAIR_ExternTempWatchdog > 0)
        _lastExternalRoomTemperaturUpdate = max(1UL, millis());
    recalculateOffset();
}

void RoomTemperatureCorrection::recalculateOffset()
{
    float offset = _currentOffset;
    logDebugP("Recalculate room temperature offset: external=%.1f °C, aircondition=%.1f °C", _externalRoomTemperature, _aircondtionRoomTemperature);
    if (_externalRoomTemperature != 0.0f && _aircondtionRoomTemperature != 0.0f)
    {
        _currentOffset = _externalRoomTemperature - _aircondtionRoomTemperature;
        logInfoP("Calculated room temperature offset: %.1f °C", _currentOffset);
    }
    else
    {
        _currentOffset = 0.0f;
        logInfoP("Reset room temperature offset to 0.0 °C");
    }
    if (_targetTemperature != 0.0f && abs(offset - _currentOffset) > 0.01f)
    {
        logInfoP("Room temperature offset changed from %.1f °C to %.1f °C", offset, _currentOffset);
        setTargetTemperaturToAircondition(_targetTemperature);
    }
    else
    {
        _usedOffset = 0.0f; // Reset used offset if no target temperature is set
    }
}

void RoomTemperatureCorrection::loop()
{
    unsigned long currentMillis = millis();
    if (_lastExternalRoomTemperaturUpdate && (currentMillis - _lastExternalRoomTemperaturUpdate > ParamAIR_CHMonitoringWDTTimeoutDelayTimeMS)) // 10 minutes timeout
    {
         // <Enumeration Text="Nein" Value="0" Id="%ENID%" />
        // <Enumeration Text="Ja, nach Zeitablauf Lesen, dann ignorieren" Value="1" Id="%ENID%" />
        // <Enumeration Text="Ja, nach Zeitablauf ignorieren" Value="2" Id="%ENID%" />
        if (ParamAIR_ExternTempWatchdog == 1)
        {
            logInfoP("External room temperature not updated for more than %d minutes, reading", (int) (ParamAIR_CHMonitoringWDTTimeoutDelayTimeMS / 1000 / 60));

            KoAIR_RoomTemperatureInput.requestObjectRead();
            _lastExternalRoomTemperaturUpdate = 0;
            _lastExternalRoomTemperatureRead = max(1UL, currentMillis);
        }
        else
        {
            logErrorP("External room temperature not updated for more than %d minutes, ignoring", (int) (ParamAIR_CHMonitoringWDTTimeoutDelayTimeMS / 1000 / 60));
            _lastExternalRoomTemperatureRead = 0;
            _externalRoomTemperature = 0.0f;
            recalculateOffset();
        }
        
    }
    else if (_lastExternalRoomTemperatureRead && (currentMillis - _lastExternalRoomTemperatureRead > 1000 * 5)) // 5 seconds timeout
    {
        logErrorP("External room temperature not read for more than 5 seconds, ignoring");
        _lastExternalRoomTemperatureRead = 0;
        _externalRoomTemperature = 0.0f;
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
    _usedOffset = temperature - _correctedTargetTemperature;
    logInfoP("Set target temperatur %.1f °C as %.1f °C to aircondition (Used offset: %.1f °C)", temperature, _correctedTargetTemperature, _usedOffset);
    _airConditionDriver.setTargetTemperature(_correctedTargetTemperature);
}

void RoomTemperatureCorrection::airconditionReportTargetTemperatureChanged(float temperature)
{
    logInfoP("Aircondition reported target temperature changed to %.1f °C, try to correct it", temperature);
    if (ParamAIR_ClimateSetTemperature)
        KoAIR_ClimateTargetTemperatur.value(temperature, DPT_Value_Temp);
    setTargetTemperaturToAircondition(temperature);
}

float RoomTemperatureCorrection::correctTemperatureFeedbackFromAircondition(float temperature)
{
    float result = temperature + _usedOffset;
    logInfoP("Correcting temperature feedback from aircondition %.1f °C with offset %.1f °C to %.1f °C", temperature, _currentOffset, result);
     if (ParamAIR_ClimateSetTemperature)
        KoAIR_ClimateTargetTemperatur.value(temperature, DPT_Value_Temp);
    return result;
}