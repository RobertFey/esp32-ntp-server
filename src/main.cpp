#include <Arduino.h>
#include <Wire.h>

#include "cli/Cli.h"
#include "cli/SerialCommandInterface.h"
#include "rtc/RTCManager.h"
#include "config/ConfigManager.h"
#include "ethernet/EthernetManager.h"
#include "tcp/TcpCliServer.h"
#include "ntp/NtpServer.h"
#include "network/NetworkManager.h"
#include "wifi/WifiManager.h"
#include "cli/commands/RTCCommands.h"
#include "cli/commands/SystemCommands.h"
#include "cli/commands/NetworkCommands.h"
#include "cli/commands/WifiCommands.h"
#include "cli/commands/NtpCommands.h"
#include "cli/CommandRegistry.h"


Cli cli;
SerialCommandInterface serialInterface;

NtpServer ntpServer;
TcpCliServer tcpCliServer;

RTCManager rtcManager;
ConfigManager configManager;
NetworkManager networkManager;
WifiManager wifiManager;
EthernetManager ethernetManager;


void setup()
{
    Serial.begin(115200);
    Wire.begin(21, 22);

    // Initialize RTC
    if (rtcManager.begin())
    {
        Serial.println("RTC initialized");
    }
    else
    {
        Serial.println("RTC not found");
    }

    // Load configuration
    configManager.begin();
    configManager.load();

    // Start network
    if (networkManager.begin())
    {
        Serial.println("Network started");

        Serial.print("Active interface: ");
        Serial.println(networkManager.activeInterface());

        Serial.print("IP: ");
        Serial.println(networkManager.localIP());

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
        Serial.println("No active network");
    }

    // Initialize CLI
    cli.begin(serialInterface);

    // Register command handlers
    RTCCommands::registerCommands();
    NetworkCommands::registerCommands();
    WifiCommands::registerCommands();
    NtpCommands::registerCommands();
    SystemCommands::registerCommands();
}

void loop()
{
    cli.process();      // Serial
    tcpCliServer.process(); // TCP CLI
    ntpServer.process(); // NTP
}