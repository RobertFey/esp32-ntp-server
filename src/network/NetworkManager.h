#pragma once

#include <Arduino.h>
#include <IPAddress.h>

class NetworkManager
{
public:
    bool begin();

    bool ethernetConnected();
    bool wifiConnected();

    bool isConnected();

    IPAddress localIP();

    String activeInterface();

    void startEthernet();
    void startWifi();
};