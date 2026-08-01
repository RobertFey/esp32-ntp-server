#include "NetworkCommands.h"
#include "../../network/NetworkManager.h"
#include "../../config/ConfigManager.h"
#include "../../ethernet/EthernetManager.h"
#include "../CommandRegistry.h"
#include "../ICommandInterface.h"

extern CommandRegistry commandRegistry;
extern NetworkManager networkManager;
extern EthernetManager ethernetManager;
extern ConfigManager configManager;

bool NetworkCommands::handle(const String& command, ICommandInterface& io)
{
    if (command == "net show")
    {
        auto& net = configManager.network();

        io.println("Network configuration");
        io.println("---------------------");
        io.println("Hostname : " + net.hostname);
        io.println("DHCP     : " + String(net.dhcp ? "ON" : "OFF"));
        io.println("IP       : " + net.ip.toString());
        io.println("Subnet   : " + net.subnet.toString());
        io.println("Gateway  : " + net.gateway.toString());
        io.println("DNS      : " + net.dns.toString());
        io.println("MAC      : " + configManager.getMacAddress());

        return true;
    }

    if (command == "net dhcp on")
    {
        configManager.network().dhcp = true;
        io.println("DHCP enabled");
        return true;
    }

    if (command == "net dhcp off")
    {
        configManager.network().dhcp = false;
        io.println("DHCP disabled");
        return true;
    }

    if (command.startsWith("net mac "))
    {
        String macString = command.substring(8);

        uint8_t mac[6];

        if (!configManager.parseMacAddress(macString, mac))
        {
            io.println("Invalid MAC format. Use XX:XX:XX:XX:XX:XX");

            return true;
        }

        if (!configManager.parseMacAddress(macString, mac))
        {
            io.println("Invalid MAC format or multicast MAC address");

            return true;
        }

        memcpy(configManager.network().mac, mac, 6);

        io.println("MAC address updated");

        return true;
    }

    if (command.startsWith("net ip "))
    {
        String value = command.substring(7);
        IPAddress ip;

        if (!ip.fromString(value))
        {
            io.println("Invalid IP address");
            return true;
        }

        configManager.network().ip = ip;

        io.println("IP address updated");

        return true;
    }

    if (command.startsWith("net subnet "))
    {
        String value = command.substring(11);
        IPAddress subnet;

        if (!subnet.fromString(value))
        {
            io.println("Invalid subnet mask");
            return true;
        }

        configManager.network().subnet = subnet;

        io.println("Subnet updated");

        return true;
    }

    if (command.startsWith("net gateway "))
    {
        String value = command.substring(12);

        IPAddress gateway;

        if (!gateway.fromString(value))
        {
            io.println("Invalid gateway address");
            return true;
        }

        configManager.network().gateway = gateway;

        io.println("Gateway updated");

        return true;
    }

    if (command.startsWith("net dns "))
    {
        String value = command.substring(8);
        IPAddress dns;

        if (!dns.fromString(value))
        {
            io.println("Invalid DNS address");
            return true;
        }
        configManager.network().dns = dns;
        io.println("DNS updated");

        return true;
    }

    if (command == "net status")
    {
        auto& net = configManager.network();

        io.println("");
        io.println("Ethernet Status");
        io.println("---------------");

        io.println("Ethernet : " +  String(networkManager.ethernetConnected() ? "UP" : "DOWN"));
        io.println("WiFi     : " + String(networkManager.wifiConnected() ? "UP" : "DOWN"));
        io.println("Active   : " + networkManager.activeInterface());
        io.println("");
        io.println("ETH Hardware     : " + ethernetManager.hardwareStatus());

        return true;
    }

    if (command == "net restart")
    {
        io.println("Restarting Ethernet...");

        if (ethernetManager.restart())
        {
            io.println("Ethernet restarted");
            io.println("IP: " + ethernetManager.localIP().toString());
        }
        else
        {
            io.println("Ethernet restart failed");
        }

        return true;
    }
    return false;
}

void NetworkCommands::printHelp(ICommandInterface& io)
{
    io.println("Network Commands");
    io.println("----------------");
    io.println("net status");
    io.println("net restart");
    io.println("net show");
    io.println("net dhcp on");
    io.println("net dhcp off");
    io.println("net mac <MAC_ADDRESS>");
    io.println("net ip <IP_ADDRESS>");
    io.println("net subnet <SUBNET_MASK>");
    io.println("net gateway <GATEWAY_ADDRESS>");
    io.println("net dns <DNS_ADDRESS>");
}

void NetworkCommands::registerCommands()
{
    CommandRegistry::registerHelp("net", printHelp);
}