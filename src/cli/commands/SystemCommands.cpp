#include "SystemCommands.h"
#include "../../config/ConfigManager.h"
#include "../../network/NetworkManager.h"
#include "../../ethernet/EthernetManager.h"
#include "../../wifi/WifiManager.h"
#include "../CommandRegistry.h"

extern CommandRegistry commandRegistry;
extern ConfigManager configManager;
extern NetworkManager networkManager;
extern EthernetManager ethernetManager;
extern WifiManager wifiManager;

bool SystemCommands::handle(const String& command, ICommandInterface& io)
{
    if (command == "help")
    {
        CommandRegistry::printTopics(io);
        return true;
    }

    if (command.startsWith("help "))
    {
        String topic = command.substring(5);

        topic.trim();
        topic.toLowerCase();

        if (!CommandRegistry::printHelp(topic, io))
        {
            io.println("Unknown help topic");
        }

        return true;
    }

    if (command.startsWith("hostname "))
    {
        configManager.network().hostname = command.substring(9);

        io.println("Hostname updated");
        return true;
    }

    if (command == "save")
    {
        configManager.save();
        io.println("Configuration saved");
        return true;
    }

    if (command == "reboot")
    {
        io.println("Rebooting...");
        delay(1000);
        ESP.restart();
        return true;
    }

    if (command == "show config")
    {
        auto& wifi = configManager.wifi();
        auto& net = configManager.network();
        io.println("Active Interface : " +  networkManager.activeInterface());
        io.println("ETH link         : " + String(networkManager.ethernetConnected() ? "UP" : "DOWN"));
        io.println("WiFi link        : " + String(networkManager.wifiConnected() ? "UP" : "DOWN"));
        io.println("");

        io.println("Network");
        io.println("-------");
        io.println("DHCP             : " + String(net.dhcp ? "ON" : "OFF"));
        io.println("IP               : " + networkManager.localIP().toString());
        io.println("Gateway          : " + net.gateway.toString());
        io.println("Subnet           : " + net.subnet.toString());
        io.println("Hostname         : " + net.hostname);
        io.println("DNS              : " + net.dns.toString());
        io.println("MAC              : " + configManager.getMacAddress());
        io.println("");

        io.println("WiFi");
        io.println("----");
        io.println("Enabled      : " + String(wifi.enabled ? "Yes" : "No"));
        io.println("DHCP         : " + String(wifi.dhcp ? "ON" : "OFF"));
        io.println("SSID         : " + wifi.ssid);
        io.println("IP           : " + wifi.ip.toString());
        io.println("Subnet Mask  : " + wifi.subnet.toString());
        io.println("Gateway      : " + wifi.gateway.toString());
        io.println("DNS Server   : " + wifi.dns.toString());
        io.println("");

        return true;
    }
    
    return false;
}

void SystemCommands::printHelp(ICommandInterface& io)
{
    io.println("System Commands");
    io.println("---------------");
    io.println("help");
    io.println("hostname <name>");
    io.println("save");
    io.println("reboot");
}

void SystemCommands::registerCommands()
{
    CommandRegistry::registerHelp("system", printHelp);
}
