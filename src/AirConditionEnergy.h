#pragma once
#include <cstdint>
#include <limits>

// Confirmed energy samples. Indoor lifetime counters and the independent system
// meter must never be summed or treated as a derived delta/wrap counter.
struct AirConditionEnergyTelemetry
{
    enum Valid : uint8_t { Power=1, Cooling=2, Heating=4, System=8 };
    uint8_t valid{0};
    uint32_t indoorPowerW{0};
    uint64_t indoorCoolingWh{0}, indoorHeatingWh{0}, systemWh{0};

    // DPT 29.010 is signed 64-bit Wh. Reject an unrepresentable counter rather
    // than narrowing/wrapping it. Raw uint64 values remain available to callers.
    uint8_t knxValidity() const
    {
        uint8_t result=valid & 0x0F;
        const auto maximum=uint64_t(std::numeric_limits<int64_t>::max());
        if (indoorCoolingWh>maximum) result &= ~Cooling;
        if (indoorHeatingWh>maximum) result &= ~Heating;
        if (systemWh>maximum) result &= ~System;
        return result;
    }
};
