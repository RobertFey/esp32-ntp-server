#include "ConfigManager.h"
#include <IPAddress.h>

bool ConfigManager::isValidIpAddress(
    const String& ipAddress)
{
    IPAddress ip;

    return ip.fromString(ipAddress);
}

bool ConfigManager::begin()
{
    return prefs.begin("ntpserver", false);
}

void ConfigManager::load()
{
    netConfig.dhcp = prefs.getBool("dhcp", true);

    netConfig.hostname =
        prefs.getString("hostname", "esp32-ntp");

    netConfig.ip.fromString(
        prefs.getString("ip", "192.168.2.100"));

    netConfig.subnet.fromString(
        prefs.getString("subnet", "255.255.255.0"));

    netConfig.gateway.fromString(
        prefs.getString("gateway", "192.168.2.254"));

    netConfig.dns.fromString(
        prefs.getString("dns", "8.8.8.8"));


    uint8_t defaultMac[6] =
    {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01 
    };

    prefs.getBytes(
        "mac",
        netConfig.mac,
        sizeof(netConfig.mac));
            
    if (prefs.getBytesLength("mac") != 6)
    {
        memcpy(
            netConfig.mac,
            defaultMac,
            sizeof(defaultMac)
        );
    }
}

void ConfigManager::save()
{
    prefs.putBool("dhcp", netConfig.dhcp);

    prefs.putString(
        "hostname",
        netConfig.hostname);

    prefs.putString(
        "ip",
        netConfig.ip.toString());

    prefs.putString(
        "subnet",
        netConfig.subnet.toString());

    prefs.putString(
        "gateway",
        netConfig.gateway.toString());

    prefs.putString(
        "dns",
        netConfig.dns.toString());

    prefs.putBytes(
        "mac",
        netConfig.mac,
        sizeof(netConfig.mac));
}


NetworkConfig& ConfigManager::network()
{
    return netConfig;
}

String ConfigManager::getMacAddress()
{
    char buffer[18];

    snprintf(
        buffer,
        sizeof(buffer),
        "%02X:%02X:%02X:%02X:%02X:%02X",
        netConfig.mac[0],
        netConfig.mac[1],
        netConfig.mac[2],
        netConfig.mac[3],
        netConfig.mac[4],
        netConfig.mac[5]);

    return String(buffer);
}

bool ConfigManager::parseMacAddress(
    const String& macString,
    uint8_t mac[6])
{
    unsigned int values[6];

    int result = sscanf(
        macString.c_str(),
        "%2x:%2x:%2x:%2x:%2x:%2x",
        &values[0],
        &values[1],
        &values[2],
        &values[3],
        &values[4],
        &values[5]);

    if (result != 6)
    {
        return false;
    }

    // Controle op 00:00:00:00:00:00
    bool allZero = true;
    
    for (int i = 0; i < 6; i++)
    {
        if (values[i] != 0)
        {
            allZero = false;
            break;
        }
    }
    
    if (allZero)
    {
        return false;
    }
    
    // Controle op FF:FF:FF:FF:FF:FF
    bool allFF = true;

    for (int i = 0; i < 6; i++)
    {
        if (values[i] != 0xFF)
        {
            allFF = false;
            break;
        }
    }

    if (allFF)
    {
        return false;
    }

    // Controle op multicast-adres
    if (values[0] & 0x01)
    {
        return false;
    }

    for (int i = 0; i < 6; i++)
    {
        mac[i] = static_cast<uint8_t>(values[i]);
    }

    return true;
}