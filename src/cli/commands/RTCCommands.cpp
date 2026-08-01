#include "RTCCommands.h"
#include "../../rtc/RTCManager.h"
#include "../CommandRegistry.h"
#include "../ICommandInterface.h"
#include "../CommandRegistry.h"

extern CommandRegistry commandRegistry;
extern RTCManager rtcManager;


bool RTCCommands::handle(
    const String& command,
    ICommandInterface& io)
{
    if (command == "rtc info")
    {
        io.println("RTC Status");
        io.println("----------");
        io.println("Time        : " + rtcManager.getDateTimeString());
        io.println(String("Lost Power  : ") + (rtcManager.hasLostPower() ? "Yes" : "No"));
        io.println(String("Temperature : ") + String(rtcManager.getTemperature(), 2) + " C");
        return true;
    }

    if (command == "time get")
    {
        io.println(rtcManager.getDateTimeString());
        return true;
    }

    if (command.startsWith("time set "))
    {
        int year;
        int month;
        int day;

        int hour;
        int minute;
        int second;

        int parsed = sscanf(command.c_str(),"time set %d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

        if (parsed == 6)
        {
            rtcManager.setTime(year, month, day, hour, minute, second);
            io.println("OK");
        }
        else
        {
            io.println("Invalid datetime");
        }

        return true;
    }

    return false;
}

void RTCCommands::printHelp(ICommandInterface& io)
{
    io.println("RTC Commands");
    io.println("------------");
    io.println("time get");
    io.println("time set YYYY-MM-DD HH:MM:SS");
    io.println("rtc info");
}

void RTCCommands::registerCommands()
{
    CommandRegistry::registerHelp("rtc", printHelp);
}