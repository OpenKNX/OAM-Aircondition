#include "OpenKNX.h"
#include "Logic.h"
#include "NetworkModule.h"
#include "FileTransferModule.h"
#include "FunctionBlocksModule.h"
#include "AirconditionModule.h"

void setup()
{

    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);
    openknx.addModule(0, openknxNetwork);
    openknx.addModule(1, openknxLogic);
    openknx.addModule(2, openknxFunctionBlocksModule);
    openknx.addModule(3, openknxAircondition);
    openknx.addModule(6, openknxFileTransferModule);

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