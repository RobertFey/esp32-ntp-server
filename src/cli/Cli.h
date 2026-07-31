#pragma once

#include <Arduino.h>
#include "ICommandInterface.h"

class Cli
{
public:
    void begin(ICommandInterface& io);
    void process();
    void executeCommand(
        const String& command,
        ICommandInterface& io);

private:
    ICommandInterface* _io;
    String _rxBuffer;
};