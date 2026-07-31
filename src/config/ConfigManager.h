#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include <Preferences.h>

struct NetworkConfig
{
    bool dhcp;

    String hostname;

    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    IPAddress dns;

    uint8_t mac[6];
};

class ConfigManager
{
public:
    bool begin();

    void load();
    void save();

    bool parseMacAddress(
        const String& macString,
        uint8_t mac[6]);

    String getMacAddress();

    bool isValidIpAddress(
        const String& ipAddress);

    NetworkConfig& network();

private:
    Preferences prefs;
    NetworkConfig netConfig;
};