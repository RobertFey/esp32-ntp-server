#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "../cli/ICommandInterface.h"

class WifiCommandInterface : public ICommandInterface
{
public:
    explicit WifiCommandInterface(WiFiClient& client);

    void print(const String& text) override;
    void println(const String& text) override;

private:
    WiFiClient& _client;
};