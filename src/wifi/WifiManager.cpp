#include "WifiManager.h"

#include "../config/ConfigManager.h"

extern ConfigManager configManager;

bool WifiManager::begin()
{
    auto& wifi = configManager.wifi();

    if (!wifi.enabled)
    {
        Serial.println("WiFi disabled");
        return false;
    }

    return connect();
}

bool WifiManager::connect()
{
    auto& wifi = configManager.wifi();

    WiFi.mode(WIFI_STA);

    if (!wifi.dhcp)
    {
        WiFi.config(
            wifi.ip,
            wifi.gateway,
            wifi.subnet,
            wifi.dns);
    }

    WiFi.begin(
        wifi.ssid.c_str(),
        wifi.password.c_str());

    Serial.print("Connecting to WiFi");

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");

        delay(500);

        if ((millis() - startTime) > 15000)
        {
            Serial.println();
            Serial.println("WiFi timeout");

            return false;
        }
    }

    Serial.println();
    Serial.println("WiFi connected");

    return true;
}

void WifiManager::disconnect()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("WiFi disconnected");
}

bool WifiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

IPAddress WifiManager::localIP()
{
    return WiFi.localIP();
}

String WifiManager::status()
{
    switch (WiFi.status())
    {
        case WL_CONNECTED:
            return "CONNECTED";

        case WL_NO_SSID_AVAIL:
            return "SSID NOT FOUND";

        case WL_CONNECT_FAILED:
            return "CONNECT FAILED";

        case WL_DISCONNECTED:
            return "DISCONNECTED";

        default:
            return "UNKNOWN";
    }
}