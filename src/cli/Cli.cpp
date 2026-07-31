#include <Arduino.h>
#include <Ethernet.h>
#include "Cli.h"
#include "SerialCommandInterface.h"
#include "../rtc/RTCManager.h"
#include "../config/ConfigManager.h"
#include "../ethernet/EthernetManager.h"

extern ConfigManager configManager;

extern RTCManager rtcManager;

extern EthernetManager ethernetManager;


void Cli::begin(ICommandInterface& io)
{
    _io = &io;
    _rxBuffer.reserve(128);
}


void Cli::process()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            _rxBuffer.trim();

            if (!_rxBuffer.isEmpty())
            {
                extern class SerialCommandInterface serialInterface;

                executeCommand(_rxBuffer, *_io);
            }

            _rxBuffer.clear();
        }
        else
        {
            _rxBuffer += c;
        }
    }
}

void Cli::executeCommand(
    const String& command,
    ICommandInterface& io)
{
    if (command == "help")
    {
        io.println("help");
        io.println("time get");
        io.println("time set YYYY-MM-DD HH:MM:SS");
        io.println("rtc info");
        io.println("---config---");
        io.println("net show");
        io.println("net status");
        io.println("net dhcp on");
        io.println("net dhcp off");
        io.println("hostname <name>");
        io.println("net mac <XX:XX:XX:XX:XX:XX>");
        io.println("net ip <IP_ADDRESS>");
        io.println("net subnet <SUBNET_MASK>");
        io.println("net gateway <GATEWAY_ADDRESS>");
        io.println("net dns <DNS_ADDRESS>");
        io.println("net restart");
        io.println("save");
        io.println("reboot");
        return;
    }

    if (command == "time get")
    {
        io.println(
            rtcManager.getDateTimeString());

        return;
    }


    if (command.startsWith("time set "))
    {
        int year;
        int month;
        int day;

        int hour;
        int minute;
        int second;

        int parsed = sscanf(
            command.c_str(),
            "time set %d-%d-%d %d:%d:%d",
            &year,
            &month,
            &day,
            &hour,
            &minute,
            &second);

        if (parsed == 6)
        {
            rtcManager.setTime(
                year,
                month,
                day,
                hour,
                minute,
                second);

            io.println("OK");
        }
        else
        {
            io.println("Invalid datetime");
        }

        return;
    }

    if (command == "rtc info")
    {
        io.println("RTC Status");
        io.println("");

        io.println("Time : " + rtcManager.getDateTimeString());

        io.println(
            String("Lost Power : ") +
            (rtcManager.hasLostPower() ? "Yes" : "No"));

        io.println(
            String("Temperature : ") +
            String(rtcManager.getTemperature(), 2) +
            " C");

        return;
    }

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

        return;
    }

    if (command == "net dhcp on")
    {
        configManager.network().dhcp = true;
        io.println("DHCP enabled");
        return;
    }

    if (command == "net dhcp off")
    {
        configManager.network().dhcp = false;
        io.println("DHCP disabled");
        return;
    }

    if (command.startsWith("hostname "))
    {
        configManager.network().hostname =
            command.substring(9);

        io.println("Hostname updated");
        return;
    }

    if (command == "save")
    {
        configManager.save();
        io.println("Configuration saved");
        return;
    }

    if (command.startsWith("net mac "))
    {
        String macString = command.substring(8);

        uint8_t mac[6];

        if (!configManager.parseMacAddress(
                macString,
                mac))
        {
            io.println("Invalid MAC format. Use XX:XX:XX:XX:XX:XX");

            return;
        }

        if (!configManager.parseMacAddress(macString, mac))
        {
            io.println("Invalid MAC format or multicast MAC address");

            return;
        }

        memcpy(
            configManager.network().mac,
            mac,
            6);

        io.println("MAC address updated");

        return;
    }

    if (command.startsWith("net ip "))
    {
        String value = command.substring(7);
        IPAddress ip;

        if (!ip.fromString(value))
        {
            io.println("Invalid IP address");
            return;
        }

        configManager.network().ip = ip;

        io.println("IP address updated");

        return;
    }

    if (command.startsWith("net subnet "))
    {
        String value = command.substring(11);
        IPAddress subnet;

        if (!subnet.fromString(value))
        {
            io.println("Invalid subnet mask");
            return;
        }

        configManager.network().subnet = subnet;

        io.println("Subnet updated");

        return;
    }

    if (command.startsWith("net gateway "))
    {
        String value = command.substring(12);

        IPAddress gateway;

        if (!gateway.fromString(value))
        {
            io.println("Invalid gateway address");
            return;
        }

        configManager.network().gateway = gateway;

        io.println("Gateway updated");

        return;
    }

    if (command.startsWith("net dns "))
    {
        String value = command.substring(8);
        IPAddress dns;

        if (!dns.fromString(value))
        {
            io.println("Invalid DNS address");
            return;
        }
        configManager.network().dns = dns;
        io.println("DNS updated");

        return;
    }

    if (command == "reboot")
    {
        io.println("Rebooting...");
        delay(1000);
        ESP.restart();
        return;
    }
    

    if (command == "net status")
    {
        auto& net = configManager.network();

        io.println("");
        io.println("Ethernet Status");
        io.println("---------------");

        io.println("Hardware   : " + ethernetManager.hardwareStatus());
        io.println("Link       : " + ethernetManager.linkStatus());
        io.println("DHCP       : " + String(net.dhcp ? "ON" : "OFF"));
        io.println("IP         : " + ethernetManager.localIP().toString());
        io.println("Hostname   : " + net.hostname);
        io.println("MAC        : " + configManager.getMacAddress());
        io.println("");

        return;
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

        return;
    }

    io.println("Unknown command");
}