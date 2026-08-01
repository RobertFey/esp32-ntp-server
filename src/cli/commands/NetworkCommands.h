#pragma once

#include "../ICommandInterface.h"

class NetworkCommands
{
public:
    static bool handle(const String& command, ICommandInterface& io);
    static void printHelp(ICommandInterface& io);
    static void registerCommands(); 
};