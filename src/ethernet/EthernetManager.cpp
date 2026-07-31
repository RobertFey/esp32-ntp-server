#include <Ethernet.h>
#include <SPI.h>
#include "EthernetManager.h"
#include "../config/ConfigManager.h"

extern ConfigManager configManager;


bool EthernetManager::begin()
{
    SPI.begin(
    18, // SCK
    19, // MISO
    23, // MOSI
    5   // SS
);

Ethernet.init(5);
    auto& net = configManager.network();

    if (net.dhcp)
    {
        if (!Ethernet.begin(net.mac))
        {
            return false;
        }
    }
    else
    {
        Ethernet.begin(
            net.mac,
            net.ip,
            net.dns,
            net.gateway,
            net.subnet);
    }

    delay(1000);

    return true;
}

bool EthernetManager::isConnected()
{
    return Ethernet.linkStatus() == LinkON;
}

IPAddress EthernetManager::localIP()
{
    return Ethernet.localIP();
}

String EthernetManager::linkStatus()
{
    switch (Ethernet.linkStatus())
    {
        case LinkON:
            return "UP";

        case LinkOFF:
            return "DOWN";

        default:
            return "UNKNOWN";
    }
}

String EthernetManager::hardwareStatus()
{
    switch (Ethernet.hardwareStatus())
    {
        case EthernetNoHardware:
            return "NO HARDWARE";

        case EthernetW5100:
            return "W5100";

        case EthernetW5200:
            return "W5200";

        case EthernetW5500:
            return "W5500";

        default:
            return "UNKNOWN";
    }
}

bool EthernetManager::restart()
{
    delay(500);
    return begin();
}