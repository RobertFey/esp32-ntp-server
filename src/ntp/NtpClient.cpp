#include <WiFi.h>
#include "NtpClient.h"

#include "../config/ConfigManager.h"
#include "../rtc/RTCManager.h"
#include "../network/NetworkManager.h"
#include "../cli/ICommandInterface.h"   

extern ConfigManager configManager;
extern RTCManager rtcManager;
extern NetworkManager networkManager;

static const uint32_t NTP_EPOCH_OFFSET = 2208988800UL;

String NtpClient::resultToString(NtpSyncResult result)
{
    switch (result)
    {
        case NtpSyncResult::Success:
            return "Success";

        case NtpSyncResult::NetworkUnavailable:
            return "Network unavailable";

        case NtpSyncResult::DnsLookupFailed:
            return "DNS lookup failed";

        case NtpSyncResult::SendFailed:
            return "Send failed";

        case NtpSyncResult::Timeout:
            return "Timeout";

        case NtpSyncResult::InvalidResponse:
            return "Invalid response";

        case NtpSyncResult::RtcUpdateFailed:
            return "RTC update failed";

        default:
            return "Unknown";
    }
}

bool NtpClient::syncRtc()
{
    return syncRtcEx() == NtpSyncResult::Success;
}

bool NtpClient::queryServer( const String& hostname, uint32_t& unixTime)
{
    IPAddress ntpServerIp;

    if (!WiFi.hostByName(hostname.c_str(), ntpServerIp))
    {
        return false;
    }

    uint8_t packet[NTP_PACKET_SIZE];

    memset(packet, 0, sizeof(packet));

    packet[0] = 0x1B;

    udp.begin(2390);

    udp.beginPacket(
        ntpServerIp,
        NTP_PORT);

    udp.write(packet, sizeof(packet));
    udp.endPacket();

    unsigned long startTime = millis();

    while ((millis() - startTime) < 3000)
    {
        int packetSize =
            udp.parsePacket();

        if (packetSize >= 48)
        {
            udp.read(packet, sizeof(packet));

            uint32_t ntpSeconds =
                ((uint32_t)packet[40] << 24) |
                ((uint32_t)packet[41] << 16) |
                ((uint32_t)packet[42] << 8)  |
                ((uint32_t)packet[43]);

            unixTime =
                ntpSeconds - NTP_EPOCH_OFFSET;

            udp.stop();

            return true;
        }

        delay(10);
    }

    udp.stop();

    return false;
}


NtpSyncResult NtpClient::syncRtcEx()
{
    uint32_t unixTime;

    if (!networkManager.isConnected())
    {
        return NtpSyncResult::NetworkUnavailable;
    }

    if (!queryServer( configManager.ntp().server, unixTime))
    {
        return NtpSyncResult::Timeout;
    }

    DateTime dt(unixTime);

    rtcManager.setTime(
        dt.year(),
        dt.month(),
        dt.day(),
        dt.hour(),
        dt.minute(),
        dt.second());

    return NtpSyncResult::Success;
}

String NtpClient::lastSyncTime()
{
    return _lastSyncTime;
}
bool NtpClient::lastSyncSuccess()
{
    return _lastSyncSuccess;
}

void NtpClient::process()
{
    auto& ntp = configManager.ntp();

    if (!ntp.autoSync)
    {
        return;
    }
    if (!networkManager.isConnected())
    {
        return;
    }
    if (_firstRun)
    {
        _firstRun = false;
        Serial.println("Initial NTP synchronization");
        syncRtcEx();
        _lastSyncMillis = millis();
        return;
    }

    unsigned long intervalMs = ntp.syncIntervalHours * 3600UL * 1000UL;

    if ((millis() - _lastSyncMillis) >= intervalMs)
    {
        Serial.println("Periodic NTP synchronization");
        syncRtcEx();
        _lastSyncMillis = millis();
    }
}
