#include "dptconvert.h"
#include "AirConditionEnergy.h"
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>

int main()
{
    for (int64_t wh : {INT64_C(0),INT64_C(100),INT64_C(429496729500),INT64_MAX}) {
        std::array<uint8_t,8> bytes{};
        assert(KNX_Encode_Value(KNXValue(wh),bytes.data(),bytes.size(),DPT_ActiveEnergy_V64));
        KNXValue decoded(int64_t(0));
        assert(KNX_Decode_Value(bytes.data(),bytes.size(),DPT_ActiveEnergy_V64,decoded));
        assert(int64_t(decoded)==wh);
        uint64_t wire=0;for(uint8_t b:bytes) wire=(wire<<8)|b;
        assert(wire==uint64_t(wh));
    }
    for (float watts : {0.0f,500.0f,655350.0f}) {
        std::array<uint8_t,4> bytes{};
        assert(KNX_Encode_Value(KNXValue(watts),bytes.data(),bytes.size(),DPT_Value_Power));
        KNXValue decoded(0.0f);
        assert(KNX_Decode_Value(bytes.data(),bytes.size(),DPT_Value_Power,decoded));
        assert(float(decoded)==watts);
    }
    AirConditionEnergyTelemetry s;s.valid=15;s.indoorCoolingWh=UINT64_MAX;
    assert(s.knxValidity()==13);
    std::cout << "KNX power/energy DPT tests passed\n";
}
