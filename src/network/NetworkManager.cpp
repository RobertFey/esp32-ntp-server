#include "NetworkManager.h"

#include "../ethernet/EthernetManager.h"
#include "../wifi/WifiManager.h"

extern EthernetManager ethernetManager;
extern WifiManager wifiManager;

bool NetworkManager::begin()
{
    startEthernet();
    startWifi();

    return isConnected();
}

void NetworkManager::startEthernet()
{
    ethernetManager.begin();
}

void NetworkManager::startWifi()
{
    wifiManager.begin();
}

bool NetworkManager::ethernetConnected()
{
    return ethernetManager.isConnected();
}

bool NetworkManager::wifiConnected()
{
    return wifiManager.isConnected();
}

bool NetworkManager::isConnected()
{
    return ethernetConnected() ||
           wifiConnected();
}

IPAddress NetworkManager::localIP()
{
    if (ethernetConnected())
    {
        return ethernetManager.localIP();
    }

    if (wifiConnected())
    {
        return wifiManager.localIP();
    }

    return IPAddress(0, 0, 0, 0);
}

String NetworkManager::activeInterface()
{
    if (ethernetConnected())
    {
        return "Ethernet";
    }

    if (wifiConnected())
    {
        return "WiFi";
    }

    return "None";
}