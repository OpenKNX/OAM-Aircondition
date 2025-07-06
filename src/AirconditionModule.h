#pragma once

class AirconditionModule : public OpenKNX::Module
{
  public:
    const std::string name() override;
    const std::string version() override;
    void showInformations() override;
    void showHelp() override;
    bool processCommand(const std::string cmd, bool debugKo);
    void setup() override;
    void loop() override;
   

};

extern AirconditionModule openknxAircondition;
