#include "OpenKNX.h"
#include "Logic.h"
#include "NetworkModule.h"
#include "FileTransferModule.h"
#include "FunctionBlocksModule.h"
#include "AirconditionModule.h"

void setup()
{

    Serial.begin(115200);
    pinMode(PROG_LED_PIN, OUTPUT);
    Serial.println("Starting");
    OPENKNX_AIR_CONDITION_SERIAL.begin(9600, SERIAL_8E1, OPENKNX_AIR_CONDITION_SERIAL_RX, OPENKNX_AIR_CONDITION_SERIAL_TX);
    Serial1.begin(9600, SERIAL_8E1, 32, 33);
   
    

    // const uint8_t firmwareRevision = 0;
    // openknx.init(firmwareRevision);
    // openknx.addModule(0, openknxNetwork);
    // openknx.addModule(1, openknxLogic);
    // openknx.addModule(2, openknxFunctionBlocksModule);
    // openknx.addModule(3, openknxAircondition);
    // openknx.addModule(6, openknxFileTransferModule);

    // openknx.setup();
}

bool lastReadFromSerialWlan = false;
bool lastReadFromSerialKlima = false;
void loop()
{
    int byte;
    byte = Serial1.read();
    if (byte >= 0)
    {
        if (!lastReadFromSerialWlan)
        {
            digitalWrite(PROG_LED_PIN, HIGH);
            Serial.println();
            Serial.println("WLAN: ");
            lastReadFromSerialWlan = true;
            
        }
        lastReadFromSerialKlima = false;
        Serial.printf("%02X", byte);
        OPENKNX_AIR_CONDITION_SERIAL.write(byte);
    }
    byte = OPENKNX_AIR_CONDITION_SERIAL.read();
    if (byte >= 0)
    {        
        if (!lastReadFromSerialKlima)
        {
            digitalWrite(PROG_LED_PIN, LOW);
            Serial.println();
            Serial.println("KLIMA: ");
            lastReadFromSerialKlima = true;
        }
        lastReadFromSerialWlan = false;
        Serial.printf("%02X", byte);
        Serial1.write(byte);
    }
   //openknx.loop();
}

void setup1()
{
   // openknx.setup1();
}

void loop1()
{
    // openknx.loop1();
}