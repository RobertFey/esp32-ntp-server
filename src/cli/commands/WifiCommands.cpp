#include "WifiCommands.h"
#include "../../wifi/WifiManager.h"
#include "../../config/ConfigManager.h"
#include "../CommandRegistry.h"

extern CommandRegistry commandRegistry;
extern WifiManager wifiManager;
extern ConfigManager configManager;

bool WifiCommands::handle(
    const String& command,
    ICommandInterface& io)
    {
        if (command == "wifi status")
        {
            auto& wifi = configManager.wifi();

            io.println("WiFi Status");
            io.println("-----------");
            io.println("Enabled    : " + String(wifi.enabled ? "Yes" : "No"));
            io.println("Connected  : " + String(wifiManager.isConnected() ? "Yes" : "No"));
            io.println("DHCP       : " + String(wifi.dhcp ? "ON" : "OFF"));
            io.println("SSID       : " + wifi.ssid);
            io.println("IP         : " + wifiManager.localIP().toString());
            return true;
        }

        if (command == "wifi on")
        {
            configManager.wifi().enabled = true;
            io.println("WiFi enabled");
            return true;
        }

        if (command == "wifi off")
        {
            wifiManager.disconnect();
            configManager.wifi().enabled = false;
            io.println("WiFi disabled");
            return true;
        }

        if (command == "wifi connect")
        {
            if (wifiManager.connect())
            {
                io.println("WiFi connected");
            }
            else
            {
                io.println("WiFi connection failed");
            }

            return true;
        }

        if (command == "wifi disconnect")
        {
            wifiManager.disconnect();
            io.println("WiFi disconnected");
            return true;
        }

        if (command.startsWith("wifi ssid "))
        {
            configManager.wifi().ssid = command.substring(10);
            io.println("SSID updated");
            return true;
        }

        if (command.startsWith("wifi password "))
        {
            configManager.wifi().password = command.substring(14);
            io.println("Password updated");
            return true;
        }

        if (command == "wifi dhcp on")
        {
            configManager.wifi().dhcp = true;
            io.println("WiFi DHCP enabled");
            return true;
        }

        if (command == "wifi dhcp off")
        {
            configManager.wifi().dhcp = false;
            io.println("WiFi DHCP disabled");
            return true;
        }

        if (command.startsWith("wifi ip "))
        {
            String value = command.substring(7);
            IPAddress ip;

            if (!ip.fromString(value))
            {
                io.println("Invalid IP address");
                return true;
            }

            configManager.wifi().ip = ip;

            io.println("IP address updated");

            return true;
        }

        if (command.startsWith("wifi subnet "))
        {
            String value = command.substring(11);
            IPAddress subnet;

            if (!subnet.fromString(value))
            {
                io.println("Invalid subnet mask");
                return true;
            }

            configManager.wifi().subnet = subnet;

            io.println("Subnet updated");

            return true;
        }

        if (command.startsWith("wifi gateway "))
        {
            String value = command.substring(12);

            IPAddress gateway;

            if (!gateway.fromString(value))
            {
                io.println("Invalid gateway address");
                return true;
            }

            configManager.wifi().gateway = gateway;

            io.println("Gateway updated");

            return true;
        }

        if (command.startsWith("wifi dns "))
        {
            String value = command.substring(8);
            IPAddress dns;

            if (!dns.fromString(value))
            {
                io.println("Invalid DNS address");
                return true;
            }
            configManager.wifi().dns = dns;
            io.println("DNS updated");

            return true;
        }
        return false;
    }

void WifiCommands::printHelp(ICommandInterface& io)
{
    io.println("WiFi Commands");
    io.println("-------------");
    io.println("wifi on");
    io.println("wifi off");
    io.println("wifi connect");
    io.println("wifi disconnect");
    io.println("wifi ssid <SSID>");
    io.println("wifi password <PASSWORD>");
    io.println("wifi dhcp on");
    io.println("wifi dhcp off");
    io.println("wifi ip <IP_ADDRESS>");
    io.println("wifi subnet <SUBNET_MASK>");
    io.println("wifi gateway <GATEWAY_ADDRESS>");
    io.println("wifi dns <DNS_ADDRESS>");
}

void WifiCommands::registerCommands()
{
    CommandRegistry::registerHelp("wifi", printHelp);
}
