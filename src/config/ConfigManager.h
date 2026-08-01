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

struct WifiConfig
{
    bool enabled;
    
    bool dhcp;

    String ssid;
    String password;

    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    IPAddress dns;
};

struct SystemConfig
{
    NetworkConfig ethernet;
    WifiConfig wifi;
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
    WifiConfig& wifi();

private:
    Preferences prefs;
    NetworkConfig netConfig;
    WifiConfig wifiConfig;
};