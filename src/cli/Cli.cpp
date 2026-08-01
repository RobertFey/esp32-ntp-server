#include <Arduino.h>
#include <Ethernet.h>
#include "Cli.h"
#include "SerialCommandInterface.h"
#include "commands/RTCCommands.h"
#include "commands/NetworkCommands.h"
#include "commands/WifiCommands.h"
#include "commands/NtpCommands.h"
#include "commands/SystemCommands.h"


void Cli::begin(ICommandInterface& io)
{
    _io = &io;
    _rxBuffer.reserve(128);
}


void Cli::process()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            _rxBuffer.trim();

            if (!_rxBuffer.isEmpty())
            {
                extern class SerialCommandInterface serialInterface;
                Serial.print("Command=[");
                Serial.print(_rxBuffer);
                Serial.println("]");

                executeCommand(_rxBuffer, *_io);
            }

            _rxBuffer.clear();
        }
        else
        {
            _rxBuffer += c;
        }
    }
}

void Cli::executeCommand(const String& command, ICommandInterface& io)
{
    if (RTCCommands::handle(command, io))
        return;

    if (NetworkCommands::handle(command, io))
        return;

    if (WifiCommands::handle(command, io))
        return;

    if (NtpCommands::handle(command, io))
        return;

    if (SystemCommands::handle(command, io))
        return;

    io.println("Unknown command");
}

