#pragma once

#include <Arduino.h>
#include <Ethernet.h>

class EthernetManager
{
public:
    bool begin();
    bool isConnected();
    IPAddress localIP();
    String linkStatus();
    String hardwareStatus();
    bool restart();

private:
    EthernetClass ethernet;
};
