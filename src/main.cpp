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
#include "ntp/NtpClient.h"
#include "tcp/WifiCliServer.h"
#include "indicators/IndicatorManager.h"
#include "tasks/TaskManager.h"

Cli cli;
SerialCommandInterface serialInterface;
WifiCliServer wifiCliServer;
IndicatorManager indicatorManager;
TaskManager taskManager;

NtpServer ntpServer;
TcpCliServer tcpCliServer;
NtpClient ntpClient;

RTCManager rtcManager;
ConfigManager configManager;
NetworkManager networkManager;
WifiManager wifiManager;
EthernetManager ethernetManager;

bool lastNetworkState = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin(21, 22);
    // taskManager.begin();

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

    // Initialize indicator manager
    indicatorManager.begin();

    // Start network
    if (networkManager.begin())
    {
        Serial.println("Network started");

        Serial.print("Active interface: ");
        Serial.println(networkManager.activeInterface());

        Serial.print("IP: ");
        Serial.println(networkManager.localIP());

        if (ethernetManager.isConnected())
        {
            tcpCliServer.begin();
            Serial.print("Ethernet CLI available at: ");
            Serial.println(ethernetManager.localIP());
        }

        if (wifiManager.isConnected())
        {
            wifiCliServer.begin();
            Serial.print("WiFi CLI available at: ");
            Serial.println(wifiManager.localIP());
        }

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
    // vTaskDelay(portMAX_DELAY);
    ntpServer.process();    // NTP
    ntpClient.process();    // NTP client auto sync
    cli.process();          // Serial
    tcpCliServer.process(); // TCP CLI
    wifiCliServer.process(); // WiFi CLI
    indicatorManager.process(); // Process indicator LEDs

    // Check network state and update indicator manager RUN LED accordingly
    bool networkConnected = networkManager.isConnected();
    if (networkConnected != lastNetworkState)
    {
        lastNetworkState = networkConnected;
        if (networkConnected)
        {
            // Network is connected, set RUN LED to normal mode
            indicatorManager.setRunMode(RunMode::Normal);
        }
        else
        {
            // Network is disconnected, set RUN LED to no network mode
            indicatorManager.setRunMode(RunMode::NoNetwork);
        }
    }
}