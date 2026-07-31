#include <Arduino.h>
#include <Wire.h>

#include "cli/Cli.h"
#include "cli/SerialCommandInterface.h"
#include "rtc/RTCManager.h"
#include "config/ConfigManager.h"
#include "ethernet/EthernetManager.h"
#include "tcp/TcpCliServer.h"
#include "ntp/NtpServer.h"


NtpServer ntpServer;
TcpCliServer tcpCliServer;
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
        Serial.print("IP: ");
        Serial.println(ethernetManager.localIP());

        tcpCliServer.begin();

        if (ntpServer.begin())
        {
            Serial.println("NTP server started");
        }
        else
        {
            Serial.println("NTP server failed");
        }
    }
    else
    {
        Serial.println("Ethernet failed");
    }

    ntpServer.begin();
 
    cli.begin(serialInterface);
}

void loop()
{
    cli.process();      // Serial
    // tcpCliServer.process(); // TCP CLI
    ntpServer.process(); // NTP
}