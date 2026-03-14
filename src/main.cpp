#include "FileTransferModule.h"
#include "FunctionBlocksModule.h"
#include "Logic.h"
#include "NetworkModule.h"
#include "OpenKNX.h"
#include "SensorDevices.h"
#ifdef SENSORMODULE
    #include "SensorModule.h"
#endif
#include "AirconditionModule.h"

void setup()
{
#ifdef VISO_SENSE_PIN
#ifdef DEVICE_UP1_GW_UART 
    pinMode(27, INPUT); // Only needed for development board
    pinMode(25, INPUT); // Only needed for development board
#endif
    pinMode(VISO_SENSE_PIN, INPUT_PULLUP);
#endif
    openknx.init();
#if defined(KNX_IP_WIFI) || defined(KNX_IP_LAN)
    openknx.addModule(0, openknxNetwork);
#endif
    openknx.addModule(1, openknxLogic);
    openknx.addModule(2, openknxFunctionBlocksModule);
    openknx.addModule(3, openknxAircondition);
#if defined(SENSORMODULE) || defined(PMMODULE)
    openknx.addModule(6, openknxSensorDevicesModule);
#endif
#ifdef SENSORMODULE
    openknx.addModule(4, openknxSensorModule);
#endif
    openknx.addModule(5, openknxFileTransferModule);

    openknx.setup();
}

void loop()
{
    openknx.loop();
}

#ifdef OPENKNX_DUALCORE
void setup1()
{
    openknx.setup1();
}

void loop1()
{
    openknx.loop1();
}
#endif