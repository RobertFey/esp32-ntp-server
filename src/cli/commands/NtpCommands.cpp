#include "NtpCommands.h"
#include "../../ntp/NtpServer.h"
#include "../../rtc/RTCManager.h"
#include "../ICommandInterface.h"
#include "../CommandRegistry.h"
#include "../../ntp/NtpServer.h"
#include "../../config/ConfigManager.h"
#include "../../ntp/NtpClient.h"

extern NtpServer ntpServer;
extern CommandRegistry commandRegistry;
extern NtpServer ntpServer;
extern NtpClient ntpClient;
extern RTCManager rtcManager;
extern ConfigManager configManager;

bool NtpCommands::handle(const String& command, ICommandInterface& io)
{
    if (command == "ntp status")
    {
        auto& ntp = configManager.ntp();

        io.println("");
        io.println("NTP Status");
        io.println("----------");
        io.println("");
        io.println("Server      : " + ntp.server); 
        io.println("Auto Sync   : " + String(ntp.autoSync ? "ON" : "OFF"));
        io.println("Interval    : " + String(ntp.syncIntervalHours) + " hours");
        io.println("");
        io.println("Requests    : " + String(ntpServer.requestCount()));
        io.println("Responses   : " + String(ntpServer.responseCount()));
        io.println("Send Errors : " + String(ntpServer.sendErrorCount()));
        io.println("Last Client : " + ntpServer.lastClient());
        io.println("");
        io.println("RTC Time    : " + rtcManager.getDateTimeString());
        io.println("Last Sync   : " + String(ntpClient.lastSyncTime().isEmpty() ? "Never" : ntpClient.lastSyncTime()));
        io.println("Sync Status : " + String(ntpClient.lastSyncSuccess() ? "OK" : "FAILED"));
        io.println("Last Result : " + ntpClient.resultToString(ntpClient.lastResult()));
        io.println("");

        return true;
    }

    if (command.startsWith("ntp server "))
    {
        String server = command.substring(11);
        server.trim();
        if (server.isEmpty())
        {
            io.println("Invalid server name");
            return true;
        }
        configManager.ntp().server = server;
        io.println("NTP server set to: " + server);

        return true;
    }

    if (command == "ntp sync")
    {
        io.println("Starting NTP synchronization...");

        NtpSyncResult result = ntpClient.syncRtcEx();

        if (result == NtpSyncResult::Success)
        {
            io.println("Synchronization successful");
            io.println("RTC Time: " + rtcManager.getDateTimeString());
        }
        else
        {
            io.println("Synchronization failed");
            io.println("Reason: " + ntpClient.resultToString(result));
        }
        return true;
    }

    if (command == "ntp show")
    {
        auto& ntp = configManager.ntp();

        io.println("");
        io.println("NTP Configuration");
        io.println("-----------------");
        io.println("Server       : " + ntp.server);
        io.println("Auto Sync    : " + String(ntp.autoSync ? "ON" : "OFF"));
        io.println("Interval     : " + String(ntp.syncIntervalHours) + " hours");
        io.println("");
        return true;
    }

    if (command == "ntp sync on")
    {
        configManager.ntp().autoSync = true;
        io.println("NTP auto sync enabled");
        return true;
    }

    if (command == "ntp sync off")
    {
        configManager.ntp().autoSync = false;
        io.println("NTP auto sync disabled");
        return true;
    }

    if (command.startsWith("ntp interval "))
    {
        String value = command.substring(13);
        uint32_t hours = value.toInt();
        if (hours == 0)
        {
            io.println("Invalid interval");
            return true;
        }
        configManager.ntp().syncIntervalHours = hours;
        io.println("Sync interval set to " + String(hours) + " hour(s)");
        return true;
    }

    if (command == "ntp debug on")
    {
        ntpServer.setDebug(true);
        io.println("NTP debug enabled");
        return true;
    }

    if (command == "ntp debug off")
    {
        ntpServer.setDebug(false);
        io.println("NTP debug disabled");
        return true;
    }

    return false;
}

void NtpCommands::printHelp(ICommandInterface& io)
{
    io.println("Ntp Commands");
    io.println("------------");
    io.println("ntp status");
    io.println("ntp requests");
    io.println("ntp lastclient");
    io.println("ntp set <YYYY-MM-DD HH:MM:SS>");
    io.println("ntp sync");
    io.println("ntp interval <hours>");
    io.println("ntp sync on");
    io.println("ntp sync off");
    io.println("ntp show");
    io.println("ntp server <hostname>");
    io.println("ntp debug on");
    io.println("ntp debug off");
}

void NtpCommands::registerCommands()
{
    CommandRegistry::registerHelp("ntp", printHelp);
}