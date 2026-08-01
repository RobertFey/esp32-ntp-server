#pragma once

#include <Arduino.h>
#include <WiFi.h>

class WifiManager
{
public:
    bool begin();

    bool connect();
    void disconnect();

    bool isConnected();

    IPAddress localIP();

    String status();

private:
    bool configuredStaticIP();
};