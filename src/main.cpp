#include <Arduino.h>
#include <Wire.h>

#include "cli/Cli.h"
#include "cli/SerialCommandInterface.h"
#include "rtc/RTCManager.h"
#include "config/ConfigManager.h"
#include "ethernet/EthernetManager.h"
#include "tcp/TcpCliServer.h"

TcpServer tcpServer;
EthernetManager ethernetManager;
ConfigManager configManager;
RTCManager rtcManager;
Cli cli;
SerialCommandInterface serialInterface;


void setup()
{
    Serial.begin(115200);

    Wire.begin(21, 22);

    if (rtcManager.begin())
    {
        Serial.println("RTC initialized");
    }
    else
    {
        Serial.println("RTC not found");
    }
    configManager.begin();
    configManager.load();

    if (ethernetManager.begin())
    {
        Serial.println("Ethernet started");
    }
    else
    {
        Serial.println("Ethernet failed");
    }

    if (ethernetManager.begin())
    {
        tcpServer.begin();
    }

    cli.begin(serialInterface);
}

void loop()
{
    cli.process();      // Serial
    tcpServer.process(); // TCP
}