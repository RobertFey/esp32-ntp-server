#pragma once

#include <Arduino.h>
#include <Ethernet.h>

class TcpCliServer
{
public:
    void begin();
    void process();

private:
    EthernetServer _server{23};
    EthernetClient _client;

    String _rxBuffer;
};
