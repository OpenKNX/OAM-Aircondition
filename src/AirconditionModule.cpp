#include "OpenKNX.h"
#include "AirconditionModule.h"

AirconditionModule openknxAircondition;

const std::string AirconditionModule::name()
{
    return "Klimaanlage";
}

const std::string AirconditionModule::version()
{
    return MAIN_ApplicationVersion;
}

void AirconditionModule::setup()
{
}


void AirconditionModule::loop()
{
}

void AirconditionModule::showInformations()
{
}

bool AirconditionModule::processCommand(const std::string cmd, bool debugKo)
{
    return false;
}


void AirconditionModule::showHelp()
{
   // openknx.console.printHelpLine("xxx", "Show information");
}
