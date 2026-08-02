#pragma once

#include <Arduino.h>
#include <WiFi.h>

class WifiCliServer
{
public:
    explicit WifiCliServer(uint16_t port = 23);

    void begin();
    void process();

private:
    static const size_t MAX_COMMAND_LENGTH = 128;

    WiFiServer _server;
    WiFiClient _client;

    String _rxBuffer;

    void acceptClient();
    void handleClientInput();
    void disconnectClient(const String& reason);
};