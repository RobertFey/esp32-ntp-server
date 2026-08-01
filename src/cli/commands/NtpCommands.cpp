#include "NtpCommands.h"
#include "../../ntp/NtpServer.h"
#include "../../rtc/RTCManager.h"
#include "../ICommandInterface.h"
#include "../CommandRegistry.h"
#include "../../ntp/NtpServer.h"

extern NtpServer ntpServer;
extern CommandRegistry commandRegistry;
extern NtpServer ntpServer;
extern RTCManager rtcManager;


bool NtpCommands::handle(const String& command, ICommandInterface& io)
    {
        if (command == "ntp status")
        {
            io.println("");
            io.println("NTP Status");
            io.println("----------");
            io.println("Requests    : " + String(ntpServer.requestCount()));
            io.println("Responses   : " + String(ntpServer.responseCount()));
            io.println("Last Client : " + ntpServer.lastClient());
            io.println("RTC Time    : " + rtcManager.getDateTimeString());
            io.println("");

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
}

void NtpCommands::registerCommands()
{
    CommandRegistry::registerHelp("ntp", printHelp);
}